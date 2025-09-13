/***********************************************************************
* eventloop.h - Implements the event loop                              *
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

/*  Standalone class
 *  ════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ EventLoop ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <atomic>
#include <list>
#include <poll.h>
#include <thread>

#include "final/eventloop/monitor.h"
#include "final/util/fstring.h"

using namespace std::chrono_literals;

namespace finalcut
{

//----------------------------------------------------------------------
// class EventLoop
//----------------------------------------------------------------------

class EventLoop
{
  public:
    // Enumeration
    enum class PollResult : uInt8 { Success, Timeout, Error, Interrupted };

    // Constructor
    EventLoop() = default;

    // Disable copy constructor
    EventLoop (const EventLoop&) = delete;

    // Disable move constructor
    EventLoop (EventLoop&&) = delete;

    // Destructor
    ~EventLoop() = default;

    // Disable copy assignment operator (=)
    auto operator = (const EventLoop&) -> EventLoop& = delete;

    // Disable move assignment operator (=)
    auto operator = (EventLoop&&) noexcept -> EventLoop& = delete;

    // Accessor
    auto getClassName() const -> FString;

    // Methods
    auto run() -> int;
    void leave() noexcept;

  private:
    // Constants
    static constexpr std::size_t DEFAULT_MONITOR_CAPACITY{64};
    static constexpr auto WAIT_INDEFINITELY{static_cast<int>(-1)};
    static constexpr int POLL_WAIT_MS{1};

    // Inquiry
    auto isRunning() const noexcept -> bool;
    auto isChanged() const noexcept -> bool;

    // Methods
    void reserveInitialCapacity();
    void nonPollWaiting() const noexcept;
    void rebuildPollStructures();
    auto processNextEvents() -> bool;
    auto processPoll (int&) -> PollResult;
    void dispatcher (int, nfds_t);
    void addMonitor (Monitor*);
    void removeMonitor (Monitor*);

    // Data members
    std::atomic<bool> running{false};
    std::atomic<bool> monitors_changed{false};
    std::vector<Monitor*> monitors{};

    // Cached poll structures - rebuilt when monitors change
    std::vector<struct pollfd> cached_fds{};
    std::vector<Monitor*>      cached_lookup{};

    // Friend classes
    friend class Monitor;
};

// EventLoop inline functions
//----------------------------------------------------------------------
inline auto EventLoop::getClassName() const -> FString
{ return "EventLoop"; }

//----------------------------------------------------------------------
inline void EventLoop::leave() noexcept
{ running.store(false, std::memory_order_relaxed); }

//----------------------------------------------------------------------
inline auto EventLoop::isRunning() const noexcept -> bool
{ return running.load(std::memory_order_relaxed); }

//----------------------------------------------------------------------
inline auto EventLoop::isChanged() const noexcept -> bool
{ return monitors_changed.load(std::memory_order_acquire); }

//----------------------------------------------------------------------
inline void EventLoop::reserveInitialCapacity()
{
  // Reserve initial capacity to avoid frequent reallocations
  monitors.reserve(DEFAULT_MONITOR_CAPACITY);
  cached_fds.reserve(DEFAULT_MONITOR_CAPACITY);
  cached_lookup.reserve(DEFAULT_MONITOR_CAPACITY);
}

}  // namespace finalcut

#endif  // EVENTLOOP_H
