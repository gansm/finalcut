/***********************************************************************
* monitor.h - Monitoring object base class                             *
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

/*  Base class
 *  ══════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▏
 * ▕ Monitor ▏
 * ▕▁▁▁▁▁▁▁▁▁▏
 */

#ifndef MONITOR_H
#define MONITOR_H

#include <functional>


// class forward declaration
class EventLoop;
class Monitor;

typedef std::function<void(Monitor*, short)> handler_t;

class Monitor
{
  public:
    // Constructor
    explicit Monitor (EventLoop*);
    Monitor() = delete;
    virtual ~Monitor();

    // Accessors
    auto getEvents() -> short;
    auto getFd() -> int;
    auto getUserContext() -> void*;

    // Inquiry
    auto isActive() -> bool;

    // Methods
    virtual void resume();
    virtual void suspend();

  protected:
    // Methods
    virtual void trigger (short);

    // Data member
    EventLoop*  eventloop{};
    int         fd{-1};
    short       events{0};
    handler_t   handler{};
    void*       user_context{nullptr};
    bool        already_initialized{false};

  private:
    // Data member
    bool active{false};

    // Friend classes
    friend class EventLoop;
};

#endif  // MONITOR_H
