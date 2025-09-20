/***********************************************************************
* monitor.cpp - Monitoring object base class                           *
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

#include "final/eventloop/eventloop.h"
#include "final/eventloop/monitor.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class monitor_error
//----------------------------------------------------------------------

monitor_error::~monitor_error() = default;


//----------------------------------------------------------------------
// class Monitor
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
Monitor::Monitor (EventLoop* eloop)
  : eventloop{eloop}
{
  validateEventLoop();

  try
  {
    eventloop->addMonitor(this);
  }
  catch (const std::exception& e)
  {
    eventloop = nullptr;  // Clean up if adding to the event loop fails
    auto err_msg{ std::string{"Failed to add monitor to event loop: "}
                + e.what() };
    throw monitor_error(err_msg);
  }
}

//----------------------------------------------------------------------
Monitor::~Monitor()  // destructor
{
  // Set state to inactive to prevent any new events
  setState(State::Inactive);

  // Clear handler to prevent any callbacks during destruction
  handler = nullptr;

  if ( eventloop )
  {
    try
    {
      eventloop->removeMonitor(this);
    }
    catch (const monitor_error& ex)
    {
      std::clog << "Exception on removing monitor: " << ex.what();
    }
  }

  user_context.reset();  // Clear user context
}

//----------------------------------------------------------------------
void Monitor::validateEventLoop() const
{
  if ( ! eventloop )
    throw monitor_error{"EventLoop cannot be null"};
}

}  // namespace finalcut
