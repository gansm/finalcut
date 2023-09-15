/***********************************************************************
* io_monitor.h - I/O monitoring object                                 *
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

/*  Inheritance diagram
 *  ═══════════════════
 *
 *      ▕▔▔▔▔▔▔▔▔▔▏
 *      ▕ Monitor ▏
 *      ▕▁▁▁▁▁▁▁▁▁▏
 *           ▲
 *           │
 *     ▕▔▔▔▔▔▔▔▔▔▔▔▏
 *     ▕ IoMonitor ▏
 *     ▕▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef IO_MONITOR_H
#define IO_MONITOR_H

#include "final/eventloop/monitor.h"
#include "final/util/fstring.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class IoMonitor
//----------------------------------------------------------------------

class IoMonitor final : public Monitor
{
  public:
    // Disable default constructor
    IoMonitor() = delete;

    explicit IoMonitor (EventLoop*);

    // Disable copy constructor
    IoMonitor(const IoMonitor&) = delete;

    // Disable move constructor
    IoMonitor(const IoMonitor&&) = delete;

    // Destructor
    ~IoMonitor() noexcept override;

    // Accessor
    auto getClassName() const -> FString override;

    // Method
    template <typename T>
    void init (int, short, handler_t, T&&);
    auto operator = (const IoMonitor&) -> IoMonitor& = delete;
    auto operator = (const IoMonitor&&) -> IoMonitor& = delete;
};

// IoMonitor inline functions
//----------------------------------------------------------------------
inline auto IoMonitor::getClassName() const -> FString
{ return "IoMonitor"; }

//----------------------------------------------------------------------
template <typename T>
inline void IoMonitor::init ( int file_descriptor, short ev
                            , handler_t hdl, T&& uc )
{
  setFileDescriptor (file_descriptor);
  setEvents (ev);
  setHandler (std::move(hdl));
  setUserContext (std::forward<T>(uc));
}

}  // namespace finalcut

#endif // IO_MONITOR_H
