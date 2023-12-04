/***********************************************************************
* signal_monitor.cpp - Signal monitoring object                        *
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

#if defined(__CYGWIN__)
  #define _XOPEN_SOURCE 700
#endif

#include <unistd.h>

#include <csignal>
#include <cstring>
#include <stdexcept>
#include <system_error>
#include <unordered_map>
#include <utility>

#include "final/eventloop/eventloop_functions.h"
#include "final/eventloop/eventloop.h"
#include "final/eventloop/signal_monitor.h"
#include "final/util/fsystem.h"

// Fix warning with recursive macros 'sa_handler' and 'sa_sigaction'
#if defined(__clang__)
  #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
#endif


namespace finalcut
{

// Using-declaration
using SignalMonitorMap = std::unordered_map<int, SignalMonitor*>;

//----------------------------------------------------------------------
static auto getSignalMonitorMap() -> SignalMonitorMap&
{
  // Encapsulate global map object
  static const auto& signal_monitors = std::make_unique<SignalMonitorMap>();
  return *signal_monitors;
}


//----------------------------------------------------------------------
// class SignalMonitor::SigactionImpl
//----------------------------------------------------------------------

class SignalMonitor::SigactionImpl
{
  public:
    // Destructor
    ~SigactionImpl() = default;

    // Accessors
    auto getSigaction() const -> const struct sigaction*;
    auto getSigaction() -> struct sigaction*;

  private:
    // Data members
    struct sigaction old_sig_action{};
};

// SignalMonitor::SigactionImpl inline functions
//----------------------------------------------------------------------
inline auto SignalMonitor::SigactionImpl::getSigaction() const -> const struct sigaction*
{ return &old_sig_action; }

//----------------------------------------------------------------------
inline auto SignalMonitor::SigactionImpl::getSigaction() -> struct sigaction*
{ return &old_sig_action; }


//----------------------------------------------------------------------
// class SignalMonitor
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
SignalMonitor::SignalMonitor (EventLoop* eloop)
  : Monitor(eloop)
  , impl(std::make_unique<SigactionImpl>())
{ }

//----------------------------------------------------------------------
SignalMonitor::~SignalMonitor() noexcept  // destructor
{
  // Restore original signal handling.
  static const auto& fsystem = FSystem::getInstance();
  fsystem->sigaction (signal_number, getSigactionImpl()->getSigaction(), nullptr);

  // Close pipe file descriptors
  (void)fsystem->close(signal_pipe.getReadFd());
  (void)fsystem->close(signal_pipe.getWriteFd());

  // Remove monitor instance from the assignment table.
  getSignalMonitorMap().erase(signal_number);
}


// public methods of SignalMonitor
//----------------------------------------------------------------------
void SignalMonitor::trigger (short return_events)
{
  drainPipe(getFileDescriptor());
  Monitor::trigger(return_events);
}


// private methods of SignalMonitor
//----------------------------------------------------------------------
void SignalMonitor::onSignal (int signal_number)
{
  // Determine the signal monitor instance
  static auto& signal_monitors = getSignalMonitorMap();
  const auto iter = signal_monitors.find(signal_number);

  if ( iter == signal_monitors.end() )
    return;

  const SignalMonitor* monitor{iter->second};

  if ( ! monitor->isActive() )
    return;

  // The event loop is notified by write access to the pipe
  uint64_t buffer{1U};
  auto successful = ::write ( monitor->signal_pipe.getWriteFd()
                            , &buffer, sizeof(buffer) ) > 0;

  if ( ! successful )
  {
    // Possible error handling
  }
}

//----------------------------------------------------------------------
void SignalMonitor::init()
{
  setEvents (POLLIN);
  handledAlarmSignal();
  ensureSignalIsUnmonitored();
  createPipe();
  installSignalHandler();
  enterMonitorInstanceInTable();
  setInitialized();
}

//----------------------------------------------------------------------
inline void SignalMonitor::handledAlarmSignal() const
{
  // SIGALRM is handled by the posix timer monitor

  if ( SIGALRM == signal_number )
    throw std::invalid_argument{"signal_number must not be SIGALRM."};
}

//----------------------------------------------------------------------
inline void SignalMonitor::ensureSignalIsUnmonitored() const
{
  // Each signal can only be managed by one monitor instance

  static auto& signal_monitors = getSignalMonitorMap();

  if ( signal_monitors.find(signal_number) == signal_monitors.end() )
    return;  // Not found

  throw std::invalid_argument
  {
    "The specified signal is already being handled by another "
    "monitor instance."
  };
}

//----------------------------------------------------------------------
inline void SignalMonitor::createPipe()
{
  // Set up pipe for notification

  static const auto& fsystem = FSystem::getInstance();
  
  if ( fsystem->pipe(signal_pipe) != 0 )
  {
    throw monitor_error{"No pipe could be set up for the signal monitor."};
  }

  setFileDescriptor(signal_pipe.getReadFd());  // Read end of pipe
}

//----------------------------------------------------------------------
inline void SignalMonitor::installSignalHandler()
{
  // Install signal handler
  struct sigaction sig_action{};
  sig_action.sa_handler = onSignal;
  sigemptyset(&sig_action.sa_mask);
  sig_action.sa_flags = 0;
  static const auto& fsystem = FSystem::getInstance();

  if ( fsystem->sigaction( signal_number, &sig_action
                         , getSigactionImpl()->getSigaction() ) != 0 )
  {
    int Error = errno;
    (void)fsystem->close(signal_pipe.getReadFd());
    (void)fsystem->close(signal_pipe.getWriteFd());
    std::error_code err_code{Error, std::generic_category()};
    std::system_error sys_err{err_code, strerror(Error)};
    throw sys_err;
  }
}

//----------------------------------------------------------------------
inline void SignalMonitor::enterMonitorInstanceInTable()
{
  // Enter the monitor instance in the assignment table

  static auto& signal_monitors = getSignalMonitorMap();
  signal_monitors[signal_number] = this;
}

//----------------------------------------------------------------------
auto SignalMonitor::getSigactionImpl() const -> const SigactionImpl*
{
  return impl.get();
}

//----------------------------------------------------------------------
auto SignalMonitor::getSigactionImpl() -> SigactionImpl*
{
  return impl.get();
}

}  // namespace finalcut
