/***********************************************************************
* timer_monitor.cpp - Time monitoring object                           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2023 Andreas Noe                                           *
*                                                                      *
* FINAL CUT is free software; you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as       *
* published by the Free Software Foundation; either version 3 of       *
* the License, or (at your option) any later version.                  *
*                                                                      *
* FINAL CUT is distributed in the hope that it will be useful, but     *
* WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU Lesser General Public License for more details.                  *
*                                                                      *
* You should have received a copy of the GNU Lesser General Public     *
* License along with this program.  If not, see                        *
* <http://www.gnu.org/licenses/>.                                      *
***********************************************************************/

#include <unistd.h>

#include <csignal>
#include <cstring>
#include <ctime>
#include <list>
#include <mutex>
#include <stdexcept>
#include <system_error>
#include <utility>

#include "eventloop.h"
#include "timer_monitor.h"

namespace
{

struct TimerNode
{
  public:
    TimerNode (timer_t tid, TimerMonitor* tmon, int file_descriptor)
      : timer_id{tid}
      , timer_monitor{tmon}
      , fd{file_descriptor}
    { }

    // Data members
    timer_t       timer_id{};
    TimerMonitor* timer_monitor{};
    int           fd{};
};

std::list<TimerNode> timer_nodes{};
std::mutex timer_nodes_mutex{};


//----------------------------------------------------------------------
#if __cplusplus > 1 && __cplusplus > 201703L
  constexpr timespec durationToTimespec (std::chrono::nanoseconds duration)
#else
  auto durationToTimespec (std::chrono::nanoseconds duration) -> timespec
#endif
{
  const auto seconds{std::chrono::duration_cast<std::chrono::seconds>(duration)};
  duration -= seconds;

  return timespec{ seconds.count(), duration.count() };
}

//----------------------------------------------------------------------
void onSigAlrm (int, siginfo_t* signal_info, void*)
{
  const timer_t timer_id{*static_cast<timer_t*>(signal_info->si_value.sival_ptr)};
  timer_nodes_mutex.lock();

  for (auto& timer_node : timer_nodes)
  {
    if ( timer_id != timer_node.timer_id )
      continue;

    if ( timer_node.timer_monitor->isActive() )
    {
      // The event loop is notified by write access to the pipe
      uint64_t buffer{1U};
      ::write (timer_node.fd, &buffer, sizeof(buffer));
    }

    break;
  }

  timer_nodes_mutex.unlock();
}

}  // anonymous namespace


// constructors and destructor
//----------------------------------------------------------------------
TimerMonitor::TimerMonitor(EventLoop* eloop)
  : Monitor(eloop)
{ }

//----------------------------------------------------------------------
TimerMonitor::~TimerMonitor() noexcept  // destructor
{
  ::close (alarm_pipe_fd[0]);
  ::close (alarm_pipe_fd[1]);

  if ( nullptr == timer_id )
    return;

  auto iter{timer_nodes.begin()};

  while ( iter != timer_nodes.end() )
  {
    if ( timer_id == iter->timer_id )
    {
      timer_nodes.erase(iter);
      break;
    }

    ++iter;
  }
}

// public methods of TimerMonitor
//----------------------------------------------------------------------
void TimerMonitor::init (handler_t hdl, void* uc)
{
  if ( already_initialized )
  {
    throw std::runtime_error{"This instance has already been initialised."};
  }

  handler      = std::move(hdl);
  user_context = uc;
  events       = POLLIN;

  if ( ::pipe(alarm_pipe_fd) != 0 )
  {
    throw std::runtime_error{"No pipe could be set up for the timer."};
  }

  fd = alarm_pipe_fd[0];  // Read end of pipe

  struct sigevent sig_event{};
  sig_event.sigev_notify          = SIGEV_SIGNAL;
  sig_event.sigev_signo           = SIGALRM;
  sig_event.sigev_value.sival_ptr = &timer_id;

  if ( timer_create(CLOCK_MONOTONIC, &sig_event, &timer_id) == 0 )
  {
    timer_nodes.emplace_back(timer_id, this, alarm_pipe_fd[1]);
  }
  else
  {
    ::close (alarm_pipe_fd[0]);
    ::close (alarm_pipe_fd[1]);
    throw std::runtime_error{"No POSIX timer could be reserved."};
  }

  already_initialized = true;
}

//----------------------------------------------------------------------
void TimerMonitor::setInterval ( std::chrono::nanoseconds first,
                                 std::chrono::nanoseconds periodic )
{
  struct itimerspec timer_spec { durationToTimespec(periodic)
                               , durationToTimespec(first) };

  if ( timer_settime(timer_id, 0, &timer_spec, nullptr) != -1 )
    return;

  int error = errno;
  std::error_code ErrCode{error, std::generic_category()};
  std::system_error SysErr{ErrCode, strerror(error)};
  throw (SysErr);
}

//----------------------------------------------------------------------
void TimerMonitor::trigger (short return_events)
{
  // Pipe to reset the signaling for poll()
  uint64_t buffer{0};
  ::read (fd, &buffer, sizeof(buffer));
  Monitor::trigger(return_events);
}

//----------------------------------------------------------------------
class SigAlrmHandlerInstaller
{
  public:
    SigAlrmHandlerInstaller()  // constructor
    {
      struct sigaction signal_handle{};
      sigemptyset(&signal_handle.sa_mask);
      signal_handle.sa_sigaction = onSigAlrm;
      signal_handle.sa_flags = SA_SIGINFO;

      if ( sigaction(SIGALRM, &signal_handle, &original_signal_handle) != -1 )
        return;

      int error = errno;
      std::error_code ErrCode{error, std::generic_category()};
      std::system_error SysErr{ErrCode, strerror(error)};
      throw (SysErr);
    }

    ~SigAlrmHandlerInstaller()  // destructor
    {
      sigaction (SIGALRM, &original_signal_handle, nullptr);
    }

  private:
    // Data member
    struct sigaction original_signal_handle{};
};

SigAlrmHandlerInstaller Installer{};
