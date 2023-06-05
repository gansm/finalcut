/***********************************************************************
* io_monitor.cpp - I/O monitoring object                               *
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

#include <utility>

#include "io_monitor.h"

// constructors and destructor
//----------------------------------------------------------------------
IoMonitor::IoMonitor (EventLoop* eloop)
  : Monitor(eloop)
{ }

//----------------------------------------------------------------------
IoMonitor::~IoMonitor() noexcept = default;  // destructor


// public methods of IoMonitor
//----------------------------------------------------------------------
void IoMonitor::init ( int file_descriptor, short ev
                     , handler_t hdl, void* uc )
{
  fd           = file_descriptor;
  events       = ev;
  handler      = std::move(hdl);
  user_context = uc;
}
