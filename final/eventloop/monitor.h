/***********************************************************************
* monitor.h - Monitoring object base class                             *
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

/*  Base class
 *  ══════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▏
 * ▕ Monitor ▏
 * ▕▁▁▁▁▁▁▁▁▁▏
 */

#ifndef MONITOR_H
#define MONITOR_H

#include <atomic>
#include <functional>
#include <stdexcept>
#include <string>

#include "final/util/fdata.h"
#include "final/util/fstring.h"

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
    // Enumeration
    enum class State : uInt8 { Inactive, Active, Suspended };

    // Constructor
    explicit Monitor (EventLoop*);
    Monitor() = delete;

    // Disable copy constructor
    Monitor (const Monitor&) = delete;

    // Disable move constructor
    Monitor (Monitor&&) noexcept = delete;

    // Destructor
    virtual ~Monitor();

    // Disable copy assignment operator (=)
    auto operator = (const Monitor&) -> Monitor& = delete;

    // Disable move assignment operator (=)
    auto operator = (Monitor&&) noexcept -> Monitor& = delete;

    // Accessors
    virtual auto getClassName() const -> FString;
    auto getEvents() const noexcept -> short;
    auto getFileDescriptor() const noexcept -> int;
    template <typename T>
    auto getUserContext() const -> clean_fdata_t<T>&;

    // Inquiry
    auto isActive() const noexcept -> bool;
    auto isSuspended() const noexcept -> bool;
    auto hasValidFileDescriptor() const noexcept -> bool;
    auto hasHandler() const noexcept -> bool;

    // Methods
    virtual void resume() noexcept;
    virtual void suspend() noexcept;
    virtual void deactivate() noexcept;

  protected:
    // Constant
    static constexpr int NO_FILE_DESCRIPTOR{-1};

    // Mutators
    void         setFileDescriptor (int) noexcept;
    void         setEvents (short) noexcept;
    void         setHandler (handler_t&&) noexcept;
    void         setHandler (const handler_t& handler);
    template <typename T>
    void         setUserContext (T&&) noexcept;
    void         clearUserContext() noexcept;
    void         setInitialized() noexcept;

    // Inquiry
    auto         isInitialized() const -> bool;

    // Methods
    virtual void trigger (short);

  private:
    // Using-declaration
    using FDataAccessPtr = std::shared_ptr<FDataAccess>;

    // Accessors
    auto getState() const noexcept -> State;

    // Methods
    void validateEventLoop() const;
    void setState (State) noexcept;

    // Data member
    EventLoop*          eventloop{};
    std::atomic<State>  state{State::Inactive};
    std::atomic<int>    fd{NO_FILE_DESCRIPTOR};
    std::atomic<short>  events{0};
    std::atomic<bool>   monitor_initialized{false};
    handler_t           handler{};
    FDataAccessPtr      user_context{nullptr};

    // Friend classes
    friend class EventLoop;
};

// Monitor inline functions
//----------------------------------------------------------------------
inline auto Monitor::getClassName() const -> FString
{ return "Monitor"; }

//----------------------------------------------------------------------
inline auto Monitor::getEvents() const noexcept -> short
{ return events.load(std::memory_order_relaxed); }

//----------------------------------------------------------------------
inline auto Monitor::getFileDescriptor() const noexcept -> int
{ return fd.load(std::memory_order_relaxed); }

//----------------------------------------------------------------------
template <typename T>
auto Monitor::getUserContext() const -> clean_fdata_t<T>&
{
  static T empty_lvalue = T{};
  return user_context
       ? static_cast<FData<clean_fdata_t<T>>&>(*user_context).get()
       : empty_lvalue;
}

//----------------------------------------------------------------------
inline auto Monitor::isActive() const noexcept -> bool
{ return getState() == State::Active; }

//----------------------------------------------------------------------
inline auto Monitor::isSuspended() const noexcept -> bool
{ return getState() == State::Suspended; }

//----------------------------------------------------------------------
inline auto Monitor::hasValidFileDescriptor() const noexcept -> bool
{ return fd.load(std::memory_order_relaxed) != NO_FILE_DESCRIPTOR; }

//----------------------------------------------------------------------
inline auto Monitor::hasHandler() const noexcept -> bool
{ return static_cast<bool>(handler); }

//----------------------------------------------------------------------
inline void Monitor::resume() noexcept
{ setState(State::Active); }

//----------------------------------------------------------------------
inline void Monitor::suspend() noexcept
{ setState(State::Suspended); }

//----------------------------------------------------------------------
inline void Monitor::deactivate() noexcept
{ setState(State::Inactive); }

//----------------------------------------------------------------------
inline void Monitor::trigger (short return_events)
{
  if ( ! handler || ! isActive() )
    return;

  try
  {
    handler (this, return_events);
  }
  catch (...)
  {
    suspend();
    throw;  // Re-throw other errors
  }
}

//----------------------------------------------------------------------
inline void Monitor::setFileDescriptor (int file_descriptor) noexcept
{ fd.store(file_descriptor, std::memory_order_relaxed); }

//----------------------------------------------------------------------
inline void Monitor::setEvents (short ev) noexcept
{ events.store(ev, std::memory_order_relaxed); }

//----------------------------------------------------------------------
inline void Monitor::setHandler (handler_t&& hdl) noexcept
{ handler = std::move(hdl); }

//----------------------------------------------------------------------
inline void Monitor::setHandler(const handler_t& hdl)
{ handler = hdl; }

//----------------------------------------------------------------------
template <typename T>
inline void Monitor::setUserContext (T&& uc) noexcept
{ user_context.reset(makeFData(std::forward<T>(uc))); }

//----------------------------------------------------------------------
inline void Monitor::clearUserContext() noexcept
{ user_context.reset(); }

//----------------------------------------------------------------------
inline void Monitor::setInitialized() noexcept
{ monitor_initialized.store(true, std::memory_order_relaxed); }

//----------------------------------------------------------------------
inline auto Monitor::isInitialized() const -> bool
{ return monitor_initialized.load(std::memory_order_relaxed); }

//----------------------------------------------------------------------
inline auto Monitor::getState() const noexcept -> State
{ return state.load(std::memory_order_relaxed); }

//----------------------------------------------------------------------
inline void Monitor::setState (State new_state) noexcept
{ state.store(new_state, std::memory_order_relaxed); }

}  // namespace finalcut

#endif  // MONITOR_H
