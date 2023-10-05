/***********************************************************************
* backend_monitor.cpp - Monitoring general objects                     *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2023 Markus Gans                                           *
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

#include "final/eventloop/eventloop_functions.h"
#include "final/eventloop/eventloop.h"
#include "final/eventloop/backend_monitor.h"
#include "final/util/fsystem.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class BackendMonitor
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
BackendMonitor::BackendMonitor (EventLoop* eloop)
  : Monitor(eloop)
{ }

//----------------------------------------------------------------------
BackendMonitor::~BackendMonitor() noexcept // destructor
{
  // Close pipe file descriptors
  static const auto& fsystem = FSystem::getInstance();
  (void)fsystem->close(self_pipe.getReadFd());
  (void)fsystem->close(self_pipe.getWriteFd());
}

// public methods of BackendMonitor
//----------------------------------------------------------------------
void BackendMonitor::setEvent() noexcept
{
  // The event loop is notified by write access to the pipe
  uint64_t buffer{1U};
  auto successful = ::write ( self_pipe.getWriteFd()
                            , &buffer, sizeof(buffer) ) > 0;

  if ( ! successful )
  {
    // Possible error handling
  }
}

//----------------------------------------------------------------------
void BackendMonitor::clearEvent() noexcept
{
  drainPipe(getFileDescriptor());
}

//----------------------------------------------------------------------
void BackendMonitor::trigger (short return_events)
{
  clearEvent();
  Monitor::trigger(return_events);
}


// private methods of BackendMonitor
//----------------------------------------------------------------------
void BackendMonitor::init()
{
  static const auto& fsystem = FSystem::getInstance();
  setEvents (POLLIN);

  // Set up pipe for notification
  if ( fsystem->pipe(self_pipe) != 0 )
  {
    throw monitor_error{"No pipe could be set up for the signal monitor."};
  }

  setFileDescriptor(self_pipe.getReadFd());  // Read end of pipe
  setInitialized();
}

}  // namespace finalcut
