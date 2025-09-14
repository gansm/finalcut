/***********************************************************************
* eventloop.cpp - Implements the event loop                            *
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

#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <thread>

#include "final/eventloop/eventloop.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class EventLoop
//----------------------------------------------------------------------

// public methods of EventLoop
//----------------------------------------------------------------------
auto EventLoop::run() -> int
{
  reserveInitialCapacity();
  running.store(true, std::memory_order_relaxed);

  try
  {
    while ( isRunning() )
    {
      if ( ! processNextEvents() )
        nonPollWaiting();
    }
  }
  catch (...)
  {
    running.store(false, std::memory_order_relaxed);
    return -1;
  }

  return 0;
}


// private methods of EventLoop
//----------------------------------------------------------------------
inline void EventLoop::nonPollWaiting() const noexcept
{
  // Saves cpu time when polling fails
  static constexpr auto POLL_WAIT_TIME{std::chrono::milliseconds(POLL_WAIT_MS)};
  std::this_thread::sleep_for(POLL_WAIT_TIME);
}

//----------------------------------------------------------------------
void EventLoop::rebuildPollStructures()
{
  // Clear existing structures
  cached_fds.clear();
  cached_lookup.clear();

  // Reserve space to avoid reallocations
  cached_fds.reserve(monitors.size());
  cached_lookup.reserve(monitors.size());

  // Build poll structures from active monitors
  for (Monitor* monitor : monitors)
  {
    if (monitor != nullptr && monitor->isActive())
    {
      cached_fds.push_back({ monitor->getFileDescriptor()  // file descriptor
                           , monitor->getEvents()  // Requested events
                           , 0  // The returned event is filled by poll()
                           });
      cached_lookup.push_back(monitor);
    }
  }

  if ( cached_fds.capacity() > cached_fds.size() * 2 )
  {
    // Shrink containers to fit if they're much larger than needed
    cached_fds.shrink_to_fit();
    cached_lookup.shrink_to_fit();
  }
}

//----------------------------------------------------------------------
inline auto EventLoop::processNextEvents() -> bool
{
  if ( isChanged() )
  {
    rebuildPollStructures();
    monitors_changed.store(false, std::memory_order_release);
  }

  if ( cached_fds.empty() )
    return false;

  int num_of_events{0};
  const auto poll_result = processPoll(num_of_events);

  if ( poll_result == PollResult::Success )
  {
    // Dispatch events waiting in cached_fds
    dispatcher (num_of_events, cached_fds.size());
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
auto EventLoop::processPoll (int& num_of_events) -> PollResult
{
  int poll_result{};

  while (true)
  {
    poll_result = poll( cached_fds.data()
                      , cached_fds.size()
                      , WAIT_INDEFINITELY );

    if ( poll_result > 0 )
    {
      num_of_events = poll_result;
      return PollResult::Success;
    }

    if ( poll_result == 0 )
      return PollResult::Timeout;

    // poll_result < 0, check errno
    if ( errno == EINTR )
    {
      // Interrupted by signal, retry unless we should stop running
      if ( ! isRunning() )
        return PollResult::Interrupted;

      continue; // Retry
    }

    // Other error occurred
    return PollResult::Error;
  }
}

//----------------------------------------------------------------------
inline void EventLoop::dispatcher (int event_num, nfds_t fd_count)
{
  // Dispatching events that are waiting in cached_fds
  nfds_t processed_fds{0};
  const auto max_fds = std::min(fd_count, nfds_t(cached_fds.size()));

  for (nfds_t index{0}; index < max_fds; index++)
  {
    const pollfd& current_fd = cached_fds[index];

    // Check if this fd has events and they match what we're monitoring
    if ( current_fd.revents == 0
      || ! (current_fd.revents & current_fd.events) )
      continue;

    if ( index < cached_lookup.size() && cached_lookup[index] != nullptr )
    {
      // Call the event handler for current_fd
      cached_lookup[index]->trigger(current_fd.revents);
      ++processed_fds;
    }

    // Early exit conditions
    if ( isChanged() || ! isRunning() || int(processed_fds) >= event_num )
      break;
  }
}

//----------------------------------------------------------------------
void EventLoop::addMonitor (Monitor* monitor)
{
  if ( ! monitor
    || std::find(monitors.begin(), monitors.end(), monitor) != monitors.end() )
    return;

  monitors.push_back(monitor);
  monitors_changed.store(true, std::memory_order_release);
}

//----------------------------------------------------------------------
void EventLoop::removeMonitor (Monitor* monitor)
{
  if ( ! monitor )
    return;

  auto iter = std::remove(monitors.begin(), monitors.end(), monitor);

  if ( iter != monitors.end() )
  {
    monitors.erase(iter, monitors.end());
    monitors_changed.store(true, std::memory_order_release);
  }
}

}  // namespace finalcut
