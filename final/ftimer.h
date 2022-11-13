/***********************************************************************
* ftimer.h - Timer for executing recurring tasks                       *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2022 Markus Gans                                           *
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
 * ▕▔▔▔▔▔▔▔▔▏
 * ▕ FTimer ▏
 * ▕▁▁▁▁▁▁▁▁▏
 */

#ifndef FTIMER_H
#define FTIMER_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <chrono>
#include <mutex>

#include "final/fevent.h"
#include "final/util/fstring.h"

namespace finalcut
{

namespace internal
{

struct timer_var
{
  static std::mutex mutex;
};

}  // namespace internal

using std::chrono::duration_cast;
using std::chrono::seconds;
using std::chrono::milliseconds;
using std::chrono::microseconds;
using std::chrono::system_clock;
using std::chrono::time_point;

// class forward declaration
class FEvent;

//----------------------------------------------------------------------
// class FTimer
//----------------------------------------------------------------------

template <typename ObjectT>
class FTimer
{
  public:
    // Destructor
    virtual ~FTimer() = default;

    // Accessors
    inline virtual auto getClassName() const -> FString
    {
      return "FTimer";
    }

    static inline auto getCurrentTime() -> TimeValue
    {
      return system_clock::now();  // Get the current time
    }

    // Inquiries
    static auto  isTimeout (const TimeValue&, uInt64) -> bool;

    // Methods
    auto  addTimer (int) & -> int;
    auto  delTimer (int) const & -> bool;
    auto  delOwnTimers() const & -> bool;
    auto  delAllTimers() const & -> bool;

  protected:
    struct FTimerData
    {
      int          id;
      milliseconds interval;
      TimeValue    timeout;
      ObjectT*     object;
    };

    // Using-declaration
    using FTimerList = std::vector<FTimerData>;
    using FTimerListUniquePtr = std::unique_ptr<FTimerList>;

    // Accessor
    auto getTimerList() const -> FTimerList*
    {
      return globalTimerList().get();
    }

    // Method
    auto processTimerEvent() -> uInt;

  private:
    // Method
    virtual void performTimerAction (ObjectT*, FEvent*);
    static auto globalTimerList() -> const FTimerListUniquePtr&;
};

// public methods of FTimer
//----------------------------------------------------------------------
template <typename ObjectT>
inline auto FTimer<ObjectT>::isTimeout (const TimeValue& time, uInt64 timeout) -> bool
{
  // Checks whether the specified time span (timeout in µs) has elapsed

  const auto& now = getCurrentTime();

  if ( now < time )
    return false;

  const auto diff = now - time;
  const auto& diff_usec = uInt64(duration_cast<microseconds>(diff).count());
  return diff_usec > timeout;
}

//----------------------------------------------------------------------
template <typename ObjectT>
auto FTimer<ObjectT>::addTimer (int interval) & -> int
{
  // Create a timer and returns the timer identifier number
  // (interval in ms)

  static int id{0};
  std::lock_guard<std::mutex> lock_guard(internal::timer_var::mutex);
  auto& timer_list = globalTimerList();

  if ( id != std::numeric_limits<int>::max() )
    id++;
  else
    id = 1;

  const auto time_interval = milliseconds(interval);
  const auto timeout = getCurrentTime() + time_interval;
  FTimerData t{ id, time_interval, timeout, static_cast<ObjectT*>(this) };

  // insert in list sorted by timeout
  auto iter = timer_list->cbegin();
  const auto& last = timer_list->cend();

  while ( iter != last && iter->timeout < t.timeout )
    ++iter;

  timer_list->insert (iter, t);
  return id;
}

//----------------------------------------------------------------------
template <typename ObjectT>
auto FTimer<ObjectT>::delTimer (int id) const & -> bool
{
  // Deletes a timer by using the timer identifier number

  if ( id <= 0 )
    return false;

  std::lock_guard<std::mutex> lock_guard(internal::timer_var::mutex);
  auto& timer_list = globalTimerList();
  auto iter = timer_list->cbegin();
  const auto& last = timer_list->cend();

  while ( iter != last && iter->id != id )
    ++iter;

  if ( iter != last )
  {
    timer_list->erase(iter);
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
template <typename ObjectT>
auto FTimer<ObjectT>::delOwnTimers() const & -> bool
{
  // Deletes all timers of this object

  std::lock_guard<std::mutex> lock_guard(internal::timer_var::mutex);
  auto& timer_list = globalTimerList();

  if ( ! timer_list )
    return false;

  if ( timer_list->empty() )
    return false;

  auto iter = timer_list->cbegin();

  while ( iter != timer_list->cend() )
  {
    if ( iter->object == this )
      iter = timer_list->erase(iter);
    else
      ++iter;
  }

  return true;
}

//----------------------------------------------------------------------
template <typename ObjectT>
auto FTimer<ObjectT>::delAllTimers() const & -> bool
{
  // Deletes all timers of all objects

  std::lock_guard<std::mutex> lock_guard(internal::timer_var::mutex);
  auto& timer_list = globalTimerList();

  if ( ! timer_list )
    return false;

  if ( timer_list->empty() )
    return false;

  timer_list->clear();
  timer_list->shrink_to_fit();
  return true;
}

// protected methods of FTimer
//----------------------------------------------------------------------
template <typename ObjectT>
auto FTimer<ObjectT>::processTimerEvent() -> uInt
{
  uInt activated{0};
  std::unique_lock<std::mutex> unique_lock ( internal::timer_var::mutex
                                           , std::defer_lock );

  if ( ! unique_lock.try_lock() )
    return 0;

  auto currentTime = getCurrentTime();
  auto& timer_list = globalTimerList();

  if ( ! timer_list || timer_list->empty() )
    return 0;

  for (auto&& timer : *timer_list)
  {
    if ( ! timer.id
      || ! timer.object
      || currentTime < timer.timeout )  // Timer not expired
      continue;

    timer.timeout += timer.interval;

    if ( timer.timeout < currentTime )
      timer.timeout = currentTime + timer.interval;

    if ( timer.interval > microseconds(0) )
      ++activated;

    auto id = timer.id;
    auto object = timer.object;

    unique_lock.unlock();
    FTimerEvent t_ev(Event::Timer, id);
    performTimerAction (object, &t_ev);
    unique_lock.lock();
  }

  return activated;
}

// private methods of FTimer
//----------------------------------------------------------------------
template <typename ObjectT>
inline void FTimer<ObjectT>::performTimerAction (ObjectT*, FEvent*)
{
  // This method must be reimplemented in a subclass
  // to process the passed object and timer event
}

//----------------------------------------------------------------------
template <typename ObjectT>
inline auto FTimer<ObjectT>::globalTimerList() -> const FTimerListUniquePtr&
{
  static const auto& timer_list = std::make_unique<FTimerList>();
  return timer_list;
}

}  // namespace finalcut

#endif  // FTIMER_H

