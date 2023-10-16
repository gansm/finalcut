/***********************************************************************
* ftimer.h - Timer for executing recurring tasks                       *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2022-2023 Markus Gans                                      *
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

/*  Standalone class    Base class
 *  ════════════════    ══════════
 *
 *     ▕▔▔▔▔▔▔▔▔▏    ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *     ▕ FTimer ▏    ▕ FObjectTimer ▏
 *     ▕▁▁▁▁▁▁▁▁▏    ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *
 */

#ifndef FTIMER_H
#define FTIMER_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <algorithm>
#include <chrono>
#include <memory>
#include <shared_mutex>
#include <vector>

#include "final/fevent.h"
#include "final/util/fstring.h"

namespace finalcut
{

namespace internal
{

struct timer_var
{
  static std::shared_timed_mutex mutex;
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

    inline auto getCurrentTime() const -> TimeValue
    {
      return system_clock::now();  // Get the current time
    }

    // Inquiries
    auto  isTimeout (const TimeValue&, uInt64) -> bool;

    // Methods
    auto  addTimer (ObjectT*, int) & -> int;
    auto  delTimer (int) const & -> bool;
    auto  delOwnTimers (const ObjectT*) const & -> bool;
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
    template <typename CallbackT>
    auto processTimerEvent (CallbackT) -> uInt;

  private:
    // Method
    static auto globalTimerList() -> const FTimerListUniquePtr&;

    // Friend classes
    friend class FObjectTimer;
};

// non-member function forward declarations
//----------------------------------------------------------------------
auto getNextId() -> int;

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
auto FTimer<ObjectT>::addTimer (ObjectT* object, int interval) & -> int
{
  // Create a timer and returns the timer identifier number
  // (interval in ms)

  std::lock_guard<std::shared_timed_mutex> lock_guard(internal::timer_var::mutex);
  auto& timer_list = globalTimerList();
  int id = getNextId();
  const auto time_interval = milliseconds(interval);
  const auto timeout = getCurrentTime() + time_interval;
  FTimerData timedata{ id, time_interval, timeout, object };

  // Insert in list sorted by timeout
  timer_list->insert( std::lower_bound( timer_list->cbegin()
                                      , timer_list->cend()
                                      , timedata
                                      , [] (const auto& a, const auto& b)
                                        {
                                          return a.timeout < b.timeout;
                                        }
                                      )
                    , timedata);
  return id;
}

//----------------------------------------------------------------------
template <typename ObjectT>
auto FTimer<ObjectT>::delTimer (int id) const & -> bool
{
  // Deletes a timer by using the timer identifier number

  if ( id <= 0 )
    return false;

  std::lock_guard<std::shared_timed_mutex> lock_guard(internal::timer_var::mutex);
  auto& timer_list = globalTimerList();

  if ( ! timer_list || timer_list->empty() )
    return false;

  auto iter = std::find_if ( timer_list->cbegin()
                           , timer_list->cend()
                           , [id] (const auto& timer)
                             {
                               return timer.id == id;
                             }
                           );

  if ( iter == timer_list->cend() )
    return false;

  timer_list->erase(iter);
  return true;
}

//----------------------------------------------------------------------
template <typename ObjectT>
auto FTimer<ObjectT>::delOwnTimers(const ObjectT* object) const & -> bool
{
  // Deletes all timers of this object

  std::lock_guard<std::shared_timed_mutex> lock_guard(internal::timer_var::mutex);
  auto& timer_list = globalTimerList();

  if ( ! timer_list || timer_list->empty() )
    return false;

  timer_list->erase ( std::remove_if( timer_list->begin()
                                    , timer_list->end()
                                    , [&object] (const auto& timer)
                                      {
                                        return timer.object == object;
                                      }
                                    )
                    , timer_list->end() );
  return true;
}

//----------------------------------------------------------------------
template <typename ObjectT>
auto FTimer<ObjectT>::delAllTimers() const & -> bool
{
  // Deletes all timers of all objects

  std::lock_guard<std::shared_timed_mutex> lock_guard(internal::timer_var::mutex);
  auto& timer_list = globalTimerList();

  if ( ! timer_list || timer_list->empty() )
    return false;

  timer_list->clear();
  timer_list->shrink_to_fit();
  return true;
}

// protected methods of FTimer
//----------------------------------------------------------------------
template <typename ObjectT>
template <typename CallbackT>
auto FTimer<ObjectT>::processTimerEvent (CallbackT callback) -> uInt
{
  uInt activated{0};
  std::shared_lock<std::shared_timed_mutex> lock ( internal::timer_var::mutex
                                                 , std::defer_lock );

  if ( ! lock.try_lock() )
    return 0;

  auto& timer_list = globalTimerList();

  if ( ! timer_list || timer_list->empty() )
    return 0;

  const auto& currentTime = getCurrentTime();

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

    lock.unlock();
    FTimerEvent t_ev(Event::Timer, timer.id);
    callback (timer.object, &t_ev);
    lock.lock();
  }

  return activated;
}

// private methods of FTimer
//----------------------------------------------------------------------
template <typename ObjectT>
auto FTimer<ObjectT>::globalTimerList() -> const FTimerListUniquePtr&
{
  static const auto& timer_list = std::make_unique<FTimerList>();
  return timer_list;
}

// class forward declaration
class FObject;

// Specialization for FObject
template <>
auto FTimer<FObject>::globalTimerList() -> const FTimerListUniquePtr&;


//----------------------------------------------------------------------
// class FObjectTimer
//----------------------------------------------------------------------
class FObjectTimer
{
  public:
    FObjectTimer();

    // Destructor
    virtual ~FObjectTimer() = default;

    // Accessors
    inline virtual auto getClassName() const -> FString
    {
      return "FObjectTimer";
    }

    static inline auto getCurrentTime() -> TimeValue
    {
      return timer->getCurrentTime();
    }

    // Inquiries
    static auto isTimeout (const TimeValue& time, uInt64 timeout) -> bool
    {
      return timer->isTimeout(time, timeout);
    }

    // Methods
    auto addTimer (int interval) -> int
    {
      return timer->addTimer(selfPointer<FObject*>(), interval);
    }

    auto delTimer (int id) const -> bool
    {
      return timer->delTimer(id);
    }

    auto delOwnTimers() const -> bool
    {
      return timer->delOwnTimers(selfPointer<const FObject*>());
    }

    auto delAllTimers() const -> bool
    {
      return timer->delAllTimers();
    }

  protected:
    auto getTimerList() const -> FTimer<FObject>::FTimerList*
    {
      return timer->globalTimerList().get();
    }

    // Method
    auto processTimerEvent() -> uInt
    {
      return timer->processTimerEvent ( [this] (FObject* receiver, FEvent* event)
                                        {
                                          performTimerAction(receiver, event);
                                        }
                                      );
    }

  private:
    // Method
    virtual void performTimerAction (FObject*, FEvent*);

    template <typename T>
    inline auto selfPointer() -> T
    {
      return T(this);
    }

    template <typename T>
    inline auto selfPointer() const -> T
    {
      return T(this);
    }

    // Data members
    static FTimer<FObject>* timer;
};


}  // namespace finalcut

#endif  // FTIMER_H

