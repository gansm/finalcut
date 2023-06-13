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
#include <utility>

#include "eventloop.h"
#include "signal_monitor.h"

// static class attributes
std::map<int, SignalMonitor*> SignalMonitor::signal_monitors{};


class SigactionImpl
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

// SigactionImpl functions
//----------------------------------------------------------------------
auto SignalMonitor::getSigactionImpl() const -> const SigactionImpl*
{ return impl.get(); }

//----------------------------------------------------------------------
auto SignalMonitor::getSigactionImpl() -> SigactionImpl*
{ return impl.get(); }


// SigactionImpl inline functions
//----------------------------------------------------------------------
inline auto SigactionImpl::getSigaction() const -> const struct sigaction*
{ return &old_sig_action; }

//----------------------------------------------------------------------
inline auto SigactionImpl::getSigaction() -> struct sigaction*
{ return &old_sig_action; }


// SignalMonitor constructors and destructor
//----------------------------------------------------------------------
SignalMonitor::SignalMonitor (EventLoop* eloop)
  : Monitor(eloop)
  , impl(std::make_unique<SigactionImpl>())
{ }

//----------------------------------------------------------------------
SignalMonitor::~SignalMonitor() noexcept  // destructor
{
  // Restore original signal handling.
  sigaction (signal_number, getSigactionImpl()->getSigaction(), nullptr);

  // Remove monitor instance from the assignment table.
  signal_monitors.erase(signal_number);
}


// public methods of SignalMonitor
//----------------------------------------------------------------------
void SignalMonitor::init (int sn, handler_t hdl, void* uc)
{
  if ( already_initialized )
    throw std::runtime_error{"This instance has already been initialised."};

  handler       = std::move(hdl);
  user_context  = uc;
  events        = POLLIN;
  signal_number = sn;

  // SIGALRM is handled by the timer monitor
  if ( SIGALRM == sn )
    throw std::invalid_argument{"signal_number must not be SIGALRM."};

  // Each signal may be managed by only one monitor instance at a time
  if ( signal_monitors.find(sn) != signal_monitors.end() )
  {
    throw std::invalid_argument
    {
      "The specified signal is already being handled by another "
      "monitor instance."
    };
  }

  // Set up pipe for notification
  if ( ::pipe(signal_pipe_fd) != 0 )
  {
    throw std::runtime_error{"No pipe could be set up for the signal monitor."};
  }

  fd = signal_pipe_fd[0];  // Read end of pipe

  // Install signal handler
  struct sigaction SigAction{};
  SigAction.sa_handler = onSignal;
  sigemptyset(&SigAction.sa_mask);
  SigAction.sa_flags = 0;

  if ( sigaction(sn, &SigAction, getSigactionImpl()->getSigaction()) != 0 )
  {
    int Error = errno;
    (void)::close(signal_pipe_fd[0]);
    (void)::close(signal_pipe_fd[1]);
    std::error_code ErrCode{Error, std::generic_category()};
    std::system_error SysErr{ErrCode, strerror(Error)};
    throw (SysErr);
  }

  // Enter the monitor instance in the assignment table
  signal_monitors[sn] = this;
  already_initialized = true;
}

//----------------------------------------------------------------------
void SignalMonitor::onSignal (int signal_number)
{
  // Determine the signal monitor instance
  const auto iter = signal_monitors.find(signal_number);

  if ( iter == signal_monitors.end() )
    return;

  SignalMonitor* monitor{iter->second};

  if ( monitor->isActive() )
  {
    // The event loop is notified by write access to the pipe
    uint64_t buffer{1U};
    auto successful = ::write (monitor->signal_pipe_fd[1], &buffer, sizeof(buffer)) > 0;

    if ( ! successful )
    {
      // Possible error handling
    }
  }
}

//----------------------------------------------------------------------
void SignalMonitor::trigger (short return_events)
{
  // Pipe to reset the signaling for poll()
  uint64_t buffer{0};
  std::size_t bytes_read{0};

  // Ensure that the correct number of bytes are read from the pipe
  while ( bytes_read < sizeof(buffer) )
  {
    ssize_t current_bytes_read = ::read (fd, &buffer, sizeof(buffer) - bytes_read);

    if ( current_bytes_read == -1 )
    {
      int error{errno};
      std::error_code err_code{error, std::generic_category()};
      std::system_error sys_err{err_code, strerror(error)};
      throw (sys_err);
    }
    else
    {
      bytes_read += static_cast<size_t>(current_bytes_read);
    }
  }

  Monitor::trigger(return_events);
}
