/***********************************************************************
* kqueue_timer.cpp - Time monitoring object with a kqueue timer        *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2023 Markus Gans                                           *
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

#if (defined(__APPLE__) && defined(__MACH__)) || defined(__OpenBSD__)
  #define USE_KQUEUE_TIMER
#endif

#if defined(USE_KQUEUE_TIMER)

#include <unistd.h>
#include <fcntl.h>

#include <sys/event.h>
#include <sys/types.h>

#include <algorithm>
#include <mutex>

#include "final/eventloop/eventloop.h"
#include "final/eventloop/timer_monitor.h"


namespace finalcut
{

//----------------------------------------------------------------------
static auto getKqueue() -> int
{
  // Creates a new kernel event queue
  static const auto kq = ::kqueue();
  return kq;
}

//----------------------------------------------------------------------
static auto getTimerID() -> int
{
  // Timer id generator (sequential number)
  static int timer_id = 0;
  constexpr auto max = std::numeric_limits<int>::max();
  timer_id = ( timer_id > 0 && timer_id < max ) ? timer_id + 1 : 1;
  return timer_id;
}

//----------------------------------------------------------------------
constexpr auto durationToMilliseconds (std::chrono::nanoseconds duration) -> int
{
  return int(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
}


//----------------------------------------------------------------------
// struct TimerNode
//----------------------------------------------------------------------
struct TimerNode
{
  public:
    // Constructor
    TimerNode (int tid, KqueueTimer* tmon_ptr)
      : timer_id{tid}
      , timer_monitor{tmon_ptr}
    { }

    // Constants
    static constexpr int NO_TIMER_ID{-1};

    // Data members
    int             timer_id{NO_TIMER_ID};
    KqueueTimer*    timer_monitor{};
};

static std::vector<TimerNode> timer_nodes{};
static std::vector<struct kevent> time_events{};


//----------------------------------------------------------------------
// class KqueueHandler
//----------------------------------------------------------------------

class KqueueHandler
{
  public:
    // Constructor
    KqueueHandler() = default;

    // Copy constructor - no mutex copies
    KqueueHandler (const KqueueHandler&)
    { }

    // Move constructor - no mutex copies
    KqueueHandler (KqueueHandler&&) noexcept
    { }

    // Overloaded operator
    void operator () (Monitor* monitor, short revents)
    {
      std::lock_guard<std::mutex> lock_guard(timer_nodes_mutex);

      // Check active events in the event list
      struct timespec timeout{0, 0};  // Do not wait
      auto data = time_events.data();
      const auto size = time_events.size();
      const auto n = ::kevent(getKqueue(), nullptr, 0, data, size, &timeout);

      if ( n <= 0 )
        return;

      for (int i{0}; i < n; i++)
      {
        if ( time_events[i].filter != EVFILT_TIMER )
          continue;

        const auto ident = int(time_events[i].ident);
        auto is_timer_id = [&ident] (const auto& item)
                           {
                             return item.timer_id == ident;
                           };
        const auto iter = std::find_if( timer_nodes.begin()
                                      , timer_nodes.end()
                                      , is_timer_id );

        if ( iter == timer_nodes.end() )
          continue;

        auto kqueue_timer_ptr = iter->timer_monitor;

        if ( kqueue_timer_ptr->first_interval )  // First timer event
        {
          kqueue_timer_ptr->first_interval = false;
          struct kevent ev_set{};
          const auto kq_flags = EV_ADD | EV_ENABLE;
          const auto ms = kqueue_timer_ptr->timer_spec.period_ms;

          // Filling the struct events with values
          EV_SET(&ev_set, ident, EVFILT_TIMER, kq_flags, 0, ms, nullptr);

          // Register event with kqueue
          if ( ::kevent(getKqueue(), &ev_set, 1, nullptr, 0, nullptr) != 0 )
            throw monitor_error{"Cannot register event in kqueue."};
        }

        kqueue_timer_ptr->timer_handler(monitor, revents);
      }
    }

  private:
    // Data members
    mutable std::mutex timer_nodes_mutex{};
};


//----------------------------------------------------------------------
// class KqueueTimer
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
KqueueTimer::KqueueTimer (EventLoop* eloop)
  : TimerMonitorImpl(eloop)
{ }

//----------------------------------------------------------------------
KqueueTimer::~KqueueTimer() noexcept  // destructor
{
  auto iter{timer_nodes.begin()};

  while ( iter != timer_nodes.end() )
  {
    if ( iter->timer_id == timer_id )
    {
      auto is_timer_id = [this] (const auto& timer_item)
                         {
                           return int(timer_item.ident) == timer_id;
                         };
      time_events.erase ( std::remove_if( time_events.begin()
                                        , time_events.end()
                                        , is_timer_id )
                        , time_events.end() );
      timer_nodes.erase(iter);
      break;
    }

    ++iter;
  }
}


// public methods of KqueueTimer
//----------------------------------------------------------------------
void KqueueTimer::init (handler_t hdl, void* uc)
{
  if ( already_initialized )
    throw monitor_error{"This instance has already been initialised."};

  timer_id      = getTimerID();
  fd            = getKqueue();
  events        = POLLIN;
  timer_handler = std::move(hdl);
  user_context  = uc;
  handler       = KqueueHandler();
  time_events.emplace_back();
  timer_nodes.emplace_back(timer_id, this);

  already_initialized = true;
}

//----------------------------------------------------------------------
void KqueueTimer::setInterval ( std::chrono::nanoseconds first,
                                std::chrono::nanoseconds periodic )
{
  struct kevent ev_set{};
  timer_spec.first_ms  = durationToMilliseconds(first);
  timer_spec.period_ms = durationToMilliseconds(periodic);
  const auto kq_flags = EV_ADD | EV_ENABLE | EV_ONESHOT;
  const auto ms = timer_spec.first_ms;

  // Filling the struct events with values
  EV_SET(&ev_set, timer_id, EVFILT_TIMER, kq_flags, 0, ms, nullptr);

  // Register event with kqueue
  if ( ::kevent(fd, &ev_set, 1, nullptr, 0, nullptr) != 0 )
    throw monitor_error{"Cannot register event in kqueue."};
}

//----------------------------------------------------------------------
void KqueueTimer::trigger (short return_events)
{
  Monitor::trigger(return_events);
}


//----------------------------------------------------------------------
// struct KqueueHandlerInstaller
//----------------------------------------------------------------------

struct KqueueHandlerInstaller
{
  // constructor
  KqueueHandlerInstaller()
  {
    // Creates a new kernel event queue
    if ( getKqueue() != -1 )
      return;

    // Could not get kqueue
    const int error = errno;
    std::error_code err_code{error, std::generic_category()};
    std::system_error sys_err{err_code, strerror(error)};
    throw sys_err;
  }

  // destructor
  ~KqueueHandlerInstaller()
  {
    ::close(getKqueue());
  }
};

KqueueHandlerInstaller Installer{};

}  // namespace finalcut

#endif  // defined(USE_KQUEUE_TIMER)
