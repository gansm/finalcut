/***********************************************************************
* posix_timer.cpp - Time monitoring object with a posix timer          *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2023-2025 Andreas Noe                                      *
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

#include <unistd.h>

#include <csignal>
#include <cstring>
#include <ctime>
#include <ctime>

#include <algorithm>
#include <mutex>
#include <stdexcept>
#include <system_error>
#include <unordered_map>
#include <utility>

#include "final/eventloop/eventloop_functions.h"
#include "final/eventloop/eventloop.h"
#include "final/eventloop/timer_monitor.h"
#include "final/util/fsystem.h"

// Fix warning with recursive macros 'sa_handler' and 'sa_sigaction'
#if defined(__clang__)
  #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
#endif


namespace finalcut
{

//----------------------------------------------------------------------
// TimerNode
//----------------------------------------------------------------------

struct Timer
{
  PosixTimer* timer_monitor{};
  int         fd{-1};
};

// Using-declaration
using TimerNode = std::pair<timer_t, Timer>;

inline void validateTimer (const TimerNode& timer)
{
  if ( ! timer.second.timer_monitor )
    throw std::invalid_argument("Timer monitor pointer cannot be null");

  if ( timer.second.fd < 0 )
    throw std::invalid_argument("File descriptor must be >= 0");
}

// Using-declaration
using TimerNodesList = std::unordered_map<timer_t, Timer>;

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
static auto getTimerNodes() -> TimerNodesList&
{
  // Encapsulate global list object
  static const auto& timer_nodes = std::make_unique<TimerNodesList>();
  return *timer_nodes;
}

//----------------------------------------------------------------------
// class SigAlrmHandler
//----------------------------------------------------------------------

class SigAlrmHandler final
{
  public:
    // Using-declaration
    using HandlerReturnType = std::decay_t<void(int, siginfo_t*, void*)>;

    // Constant
    static constexpr std::uint64_t NOTIFICATION_MESSAGE{1U};

    // Overloaded operators
    void operator () (const siginfo_t* signal_info) const noexcept
    {
      if ( ! signal_info || ! signal_info->si_value.sival_ptr )
        return;

      std::lock_guard<std::mutex> lock_guard(timer_nodes_mutex);

      auto& timer_nodes = getTimerNodes();
      const auto& timer_id = *static_cast<const timer_t*>(signal_info->si_value.sival_ptr);
      const auto iter = timer_nodes.find(timer_id);

      if ( iter == timer_nodes.end() )
        return;

      const auto& timer_node = *iter;

      if ( timer_node.second.timer_monitor
        && timer_node.second.timer_monitor->isActive() )
      {
        // The event loop is notified by write access to the pipe
        const std::uint64_t buffer{NOTIFICATION_MESSAGE};
        const auto successful = ::write ( timer_node.second.fd
                                        , &buffer
                                        , sizeof(buffer) ) > 0;

        if ( ! successful )
        {
          // Possible error handling
        }
      }
    }

    operator HandlerReturnType () const noexcept
    {
      // Converts a member function pointer to a function pointer
      return &invoke;
    }

  private:
    // Method
    static void invoke (int, siginfo_t* signal_info, void*)
    {
      SigAlrmHandler{}.operator()(signal_info);
    }

    // Data members
    static std::mutex timer_nodes_mutex;
};

// static class attribute
std::mutex SigAlrmHandler::timer_nodes_mutex{};


//----------------------------------------------------------------------
// class SigAlrmHandlerInstaller
//----------------------------------------------------------------------

class SigAlrmHandlerInstaller final
{
  public:
    SigAlrmHandlerInstaller()  // constructor
    {
      const auto& fsystem = FSystem::getInstance();
      struct sigaction signal_handle{};
      sigemptyset(&signal_handle.sa_mask);
      signal_handle.sa_sigaction = SigAlrmHandler();
      signal_handle.sa_flags = SA_SIGINFO;

      if ( fsystem->sigaction(SIGALRM, &signal_handle, &original_signal_handle) != -1 )
        return;

      const int error_code = errno;
      const std::error_code err_code{error_code, std::generic_category()};
      const std::system_error sys_err{err_code, strerror(error_code)};
      throw sys_err;
    }

    // Disable copy constructor
    SigAlrmHandlerInstaller (const SigAlrmHandlerInstaller&) = delete;

    // Disable move constructor
    SigAlrmHandlerInstaller (SigAlrmHandlerInstaller&&) noexcept = delete;

    ~SigAlrmHandlerInstaller()  // destructor
    {
      static const auto& fsystem = FSystem::getInstance();
      fsystem->sigaction (SIGALRM, &original_signal_handle, nullptr);
    }

    // Disable copy assignment operator (=)
    auto operator = (const SigAlrmHandlerInstaller&) -> SigAlrmHandlerInstaller& = delete;

    // Disable move assignment operator (=)
    auto operator = (SigAlrmHandlerInstaller&&) noexcept -> SigAlrmHandlerInstaller& = delete;

  private:
    // Data member
    struct sigaction original_signal_handle{};
};

//----------------------------------------------------------------------
static auto startSigAlrmHandlerInstaller() -> SigAlrmHandlerInstaller*
{
  static const auto& sig_alrm_handler = std::make_unique<SigAlrmHandlerInstaller>();
  return sig_alrm_handler.get();
}

// static class attribute
SigAlrmHandlerInstaller* PosixTimer::sig_alrm_handler_installer{};


//----------------------------------------------------------------------
// class PosixTimer
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
PosixTimer::PosixTimer (EventLoop* eloop)
  : TimerMonitorImpl(eloop)
{
  sig_alrm_handler_installer = startSigAlrmHandlerInstaller();
}

//----------------------------------------------------------------------
PosixTimer::~PosixTimer() noexcept  // destructor
{
  cleanupResources();  // Clean up on failure
}


// public methods of PosixTimer
//----------------------------------------------------------------------
void PosixTimer::setInterval ( std::chrono::nanoseconds first,
                               std::chrono::nanoseconds periodic )
{
  // Input validation
  validateIntervals (first, periodic);

  static const auto& fsystem = FSystem::getInstance();
  const struct itimerspec timer_spec { durationToTimespec(periodic)
                                     , durationToTimespec(first) };

  if ( fsystem->timer_settime(timer_id, 0, &timer_spec, nullptr) != -1 )
    return;

  const int error = errno;
  const std::error_code err_code{error, std::generic_category()};
  const std::system_error sys_err{err_code, strerror(error)};
  throw sys_err;
}

//----------------------------------------------------------------------
void PosixTimer::trigger (short return_events)
{
  try
  {
    // Clear the pipe to reset timer signaling
    drainPipe(getFileDescriptor());

    // Call base class trigger
    Monitor::trigger(return_events);
  }
  catch (...)
  {
    // Attempt to clear pipe even if trigger failed
    drainPipe(getFileDescriptor());
    throw;  // Re-throw other errors
  }
}

#if defined(UNIT_TEST)
//----------------------------------------------------------------------
void PosixTimer::ReinstallSigAlrmHandler()
{
  static std::unique_ptr<SigAlrmHandlerInstaller> sig_alrm_handler{};
  cleanupResources();
  auto new_handler = std::make_unique<SigAlrmHandlerInstaller>();
  sig_alrm_handler.swap(new_handler);
  sig_alrm_handler_installer = sig_alrm_handler.get();
}
#endif  // defined(UNIT_TEST)


// private methods of PosixTimer
//----------------------------------------------------------------------
void PosixTimer::init()
{
  try
  {
    setEvents (POLLIN);
    createAlarmPipe();
    installTime();
    setInitialized();
  }
  catch (...)
  {
    cleanupResources();  // Clean up on failure
    throw;  // Re-throw the original exception
  }
}

//----------------------------------------------------------------------
void PosixTimer::createAlarmPipe()
{
  static const auto& fsystem = FSystem::getInstance();

  if ( fsystem->pipe(alarm_pipe) != 0 )
    throw monitor_error{"No pipe could be set up for the timer."};

  if ( alarm_pipe.getReadFd() == PipeData::NO_FILE_DESCRIPTOR
    || alarm_pipe.getWriteFd() == PipeData::NO_FILE_DESCRIPTOR )
  {
    throw monitor_error{"Pipe creation succeeded but "
                        "file descriptors are invalid"};
  }

  // Set the read end of the pipe as our file descriptor
  setFileDescriptor(alarm_pipe.getReadFd());

}

//----------------------------------------------------------------------
void PosixTimer::installTime()
{
  static const auto& fsystem = FSystem::getInstance();
  struct sigevent sig_event{};
  sig_event.sigev_notify = SIGEV_SIGNAL;
  sig_event.sigev_signo = SIGALRM;
  sig_event.sigev_value.sival_ptr = &timer_id;

  if ( fsystem->timer_create(CLOCK_MONOTONIC, &sig_event, &timer_id) == 0 )
  {
    try
    {
      auto timer = std::make_pair(timer_id, Timer{this, alarm_pipe.getWriteFd()});
      validateTimer(timer);
      getTimerNodes().emplace(std::move(timer));
    }
    catch (...)
    {
      // Clean up timer if node creation fails
      fsystem->timer_delete(timer_id);
      throw monitor_error{"Failed to register timer node"};
    }
  }
  else
  {
    cleanupResources();
    throw monitor_error{"No POSIX timer could be reserved."};
  }
}

//----------------------------------------------------------------------
void PosixTimer::cleanupResources() noexcept
{
  static const auto& fsystem = FSystem::getInstance();

  // Close pipe file descriptors
  if ( alarm_pipe.getReadFd() != PipeData::NO_FILE_DESCRIPTOR )
  {
    fsystem->close (alarm_pipe.getReadFd());
    alarm_pipe.setReadFd(PipeData::NO_FILE_DESCRIPTOR);
  }

  if ( alarm_pipe.getWriteFd() != PipeData::NO_FILE_DESCRIPTOR )
  {
    fsystem->close (alarm_pipe.getWriteFd());
    alarm_pipe.setWriteFd(PipeData::NO_FILE_DESCRIPTOR);
  }

  if ( timer_id == timer_t{} )
    return;

  // Delete timer and remove from timer nodes list
  static auto& timer_nodes = getTimerNodes();
  const auto iter = \
      std::find_if ( timer_nodes.begin()
                   , timer_nodes.end()
                   , [this] (const TimerNode& node)
                     {
                       return node.first == timer_id;
                     } );

  if ( iter == timer_nodes.end() )  // Timer not found
    return;

  static_cast<void>(fsystem->timer_delete(timer_id));
  timer_nodes.erase(iter);
  timer_id = timer_t{};
}

}  // namespace finalcut

#endif  // defined(USE_POSIX_TIMER)
