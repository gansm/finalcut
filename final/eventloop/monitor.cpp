/***********************************************************************
* monitor.cpp - Monitoring object base class                           *
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
  eventloop->addMonitor(this);
}

//----------------------------------------------------------------------
Monitor::~Monitor()  // destructor
{
  if ( eventloop )
    eventloop->removeMonitor(this);
}

}  // namespace finalcut
