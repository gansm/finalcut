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
#include <stdexcept>
#include <string>

namespace finalcut
{

// class forward declaration
class EventLoop;
class Monitor;

//----------------------------------------------------------------------
// class monitor_error
//----------------------------------------------------------------------

class monitor_error : public std::runtime_error
{
  public:
    using std::runtime_error::runtime_error;

    // Copy constructor
    monitor_error (const monitor_error&) = default;

    // Move constructor
    monitor_error (monitor_error&&) noexcept = default;

    // Copy assignment operator (=)
    auto operator = (const monitor_error&) -> monitor_error& = default;

    // Move assignment operator (=)
    auto operator = (monitor_error&&) noexcept -> monitor_error& = default;

    // Destructor
    ~monitor_error() override;
};



// Using-declaration
using handler_t = std::function<void(Monitor*, short)>;


//----------------------------------------------------------------------
// class Monitor
//----------------------------------------------------------------------

class Monitor
{
  public:
    // Constructor
    explicit Monitor (EventLoop*);
    Monitor() = delete;

    // Disable copy constructor
    Monitor (const Monitor&) = delete;

    // Disable move constructor
    Monitor (Monitor&&) noexcept = delete;

    // Destructor
    virtual ~Monitor();

    // Accessors
    auto getEvents() const -> short;
    auto getFileDescriptor() const -> int;
    auto getUserContext() const -> void*;

    // Inquiry
    auto isActive() const -> bool;

    // Disable copy assignment operator (=)
    auto operator = (const Monitor&) -> Monitor& = delete;

    // Disable move assignment operator (=)
    auto operator = (Monitor&&) noexcept -> Monitor& = delete;

    // Methods
    virtual void resume();
    virtual void suspend();

  protected:
    // Constants
    static constexpr int NO_FILE_DESCRIPTOR{-1};

    // Mutators
    void         setFileDescriptor (int);
    void         setEvents (short);
    void         setHandler (handler_t&&);
    void         setUserContext (void*);
    void         setInitialized();

    // Inquiry
    auto         isInitialized() -> bool;

    // Methods
    virtual void trigger (short);

  private:
    // Data member
    bool        active{false};
    EventLoop*  eventloop{};
    int         fd{NO_FILE_DESCRIPTOR};
    short       events{0};
    handler_t   handler{};
    void*       user_context{nullptr};
    bool        monitor_initialized{false};

    // Friend classes
    friend class EventLoop;
};

// Monitor inline functions
//----------------------------------------------------------------------
inline auto Monitor::getEvents() const -> short
{ return events; }

//----------------------------------------------------------------------
inline auto Monitor::getFileDescriptor() const -> int
{ return fd; }


//----------------------------------------------------------------------
inline auto Monitor::getUserContext() const -> void*
{ return user_context; }

//----------------------------------------------------------------------
inline auto Monitor::isActive() const -> bool
{ return active; }

//----------------------------------------------------------------------
inline void Monitor::resume()
{ active = true; }

//----------------------------------------------------------------------
inline void Monitor::suspend()
{ active = false; }

//----------------------------------------------------------------------
inline void Monitor::trigger (short return_events)
{
  if ( handler )
    handler (this, return_events);
}

//----------------------------------------------------------------------
inline void Monitor::setFileDescriptor (int file_descriptor)
{ fd = file_descriptor; }

//----------------------------------------------------------------------
inline void Monitor::setEvents (short ev)
{ events = ev; }

//----------------------------------------------------------------------
inline void Monitor::setHandler (handler_t&& hdl)
{ handler = std::move(hdl); }

//----------------------------------------------------------------------
inline void Monitor::setUserContext (void* uc)
{ user_context = uc; }

//----------------------------------------------------------------------
inline void Monitor::setInitialized()
{ monitor_initialized = true; }

//----------------------------------------------------------------------
inline auto Monitor::isInitialized() -> bool
{ return monitor_initialized; }

}  // namespace finalcut

#endif  // MONITOR_H
