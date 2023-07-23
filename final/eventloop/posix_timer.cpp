/***********************************************************************
* posix_timer.cpp - Time monitoring object with a posix timer          *
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

#if !(defined(__APPLE__) && defined(__MACH__)) && !(defined(__OpenBSD__))
  #define USE_POSIX_TIMER
#endif

#if defined(USE_POSIX_TIMER)

#if defined(__CYGWIN__)
  #define _XOPEN_SOURCE 700
#endif

#include <time.h>
#include <unistd.h>

#include <csignal>
#include <cstring>
#include <ctime>
#include <list>
#include <mutex>
#include <stdexcept>
#include <system_error>
#include <utility>

#include "final/eventloop/eventloop.h"
#include "final/eventloop/timer_monitor.h"

namespace finalcut
{

//----------------------------------------------------------------------
// struct TimerNode
//----------------------------------------------------------------------

struct TimerNode
{
  public:
    // Constructor
    TimerNode (timer_t tid, PosixTimer* tmon_ptr, int file_descriptor)
      : timer_id{tid}
      , timer_monitor{tmon_ptr}
      , fd{file_descriptor}
    { }

    // Data members
    timer_t     timer_id{};
    PosixTimer* timer_monitor{};
    int         fd{};
};

static std::list<TimerNode> timer_nodes{};
static std::mutex timer_nodes_mutex{};


//----------------------------------------------------------------------
#if __cplusplus > 1 && __cplusplus >= 201703L
  constexpr auto durationToTimespec (std::chrono::nanoseconds duration) -> timespec
#else
  static auto durationToTimespec (std::chrono::nanoseconds duration) -> timespec
#endif
{
  const auto seconds{std::chrono::duration_cast<std::chrono::seconds>(duration)};
  duration -= seconds;

  return timespec{ static_cast<time_t>(seconds.count())
                 , static_cast<long>(duration.count()) };
}

//----------------------------------------------------------------------
static void onSigAlrm (int, siginfo_t* signal_info, void*)
{
  const auto& timer_id = *static_cast<const timer_t*>(signal_info->si_value.sival_ptr);
  std::lock_guard<std::mutex> lock_guard(timer_nodes_mutex);

  for (const auto& timer_node : timer_nodes)
  {
    if ( timer_id != timer_node.timer_id )
      continue;

    if ( timer_node.timer_monitor->isActive() )
    {
      // The event loop is notified by write access to the pipe
      uint64_t buffer{1U};
      auto successful = ::write (timer_node.fd, &buffer, sizeof(buffer)) > 0;

      if ( ! successful )
      {
        // Possible error handling
      }
    }

    break;
  }
}


//----------------------------------------------------------------------
// class PosixTimer
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
PosixTimer::PosixTimer (EventLoop* eloop)
  : TimerMonitorImpl(eloop)
{ }

//----------------------------------------------------------------------
PosixTimer::~PosixTimer() noexcept  // destructor
{
  ::close (alarm_pipe_fd[0]);
  ::close (alarm_pipe_fd[1]);

  if ( timer_id == timer_t{} )
    return;

  auto iter{timer_nodes.begin()};

  while ( iter != timer_nodes.end() )
  {
    if ( iter->timer_id == timer_id )
    {
      timer_delete(timer_id);
      timer_nodes.erase(iter);
      break;
    }

    ++iter;
  }
}


// public methods of PosixTimer
//----------------------------------------------------------------------
void PosixTimer::init (handler_t hdl, void* uc)
{
  if ( already_initialized )
    throw monitor_error{"This instance has already been initialised."};

  handler      = std::move(hdl);
  user_context = uc;
  events       = POLLIN;

  if ( ::pipe(alarm_pipe_fd.data()) != 0 )
    throw monitor_error{"No pipe could be set up for the timer."};

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
    throw monitor_error{"No POSIX timer could be reserved."};
  }

  already_initialized = true;
}

//----------------------------------------------------------------------
void PosixTimer::setInterval ( std::chrono::nanoseconds first,
                               std::chrono::nanoseconds periodic )
{
  struct itimerspec timer_spec { durationToTimespec(periodic)
                               , durationToTimespec(first) };

  if ( timer_settime(timer_id, 0, &timer_spec, nullptr) != -1 )
    return;

  const int error = errno;
  std::error_code err_code{error, std::generic_category()};
  std::system_error sys_err{err_code, strerror(error)};
  throw sys_err;
}

//----------------------------------------------------------------------
void PosixTimer::trigger (short return_events)
{
  // Pipe to reset the signaling for poll()
  uint64_t buffer{0};
  std::size_t bytes_read{0};

  // Ensure that the correct number of bytes are read from the pipe
  while ( bytes_read < sizeof(buffer) )
  {
    auto current_bytes_read = ::read(fd, &buffer, sizeof(buffer) - bytes_read);

    if ( current_bytes_read == -1 )
    {
      int error{errno};
      std::error_code err_code{error, std::generic_category()};
      std::system_error sys_err{err_code, strerror(error)};
      throw sys_err;
    }
    else
    {
      bytes_read += static_cast<size_t>(current_bytes_read);
    }
  }

  Monitor::trigger(return_events);
}


//----------------------------------------------------------------------
// class SigAlrmHandlerInstaller
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

      const int error = errno;
      std::error_code err_code{error, std::generic_category()};
      std::system_error sys_err{err_code, strerror(error)};
      throw sys_err;
    }

    ~SigAlrmHandlerInstaller()  // destructor
    {
      sigaction (SIGALRM, &original_signal_handle, nullptr);
    }

  private:
    // Data member
    struct sigaction original_signal_handle{};
};

static SigAlrmHandlerInstaller Installer{};

}  // namespace finalcut

#endif  // defined(USE_POSIX_TIMER)
