/***********************************************************************
* kqueue_timer.cpp - Time monitoring object with a kqueue timer        *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2023-2025 Markus Gans                                      *
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

#if defined(USE_KQUEUE_TIMER) || defined(UNIT_TEST)

#include <unistd.h>
#include <fcntl.h>

#if defined(UNIT_TEST)

  #include <cstdint>

  using u_short = unsigned short;
  using u_int = unsigned int;

  struct kevent
  {
    uintptr_t ident;   // Identifier for this event
    short     filter;  // Filter for event
    u_short   flags;   // Action flags for kqueue
    u_int     fflags;  // Filter flag value
    int64_t   data;    // Filter data value
    void*     udata;   // Opaque user data identifier
  };

  #define EV_SET(kevp, a, b, c, d, e, f) do  \
          {                                  \
            struct kevent* __kevp = (kevp);  \
            (__kevp)->ident  = (a);          \
            (__kevp)->filter = (b);          \
            (__kevp)->flags  = (c);          \
            (__kevp)->fflags = (d);          \
            (__kevp)->data   = (e);          \
            (__kevp)->udata  = (f);          \
          } while(0)

  // Event filter
  #define EVFILT_READ    (-1)  // File descriptor read
  #define EVFILT_WRITE   (-2)  // File descriptor write
  #define EVFILT_AIO     (-3)  // Attached to aio requests
  #define EVFILT_VNODE   (-4)  // Attached to vnodes
  #define EVFILT_PROC    (-5)  // Attached to struct process
  #define EVFILT_SIGNAL  (-6)  // Attached to struct process
  #define EVFILT_TIMER   (-7)  // Timers
  #define EVFILT_DEVICE  (-8)  // Devices
  #define EVFILT_EXCEPT  (-9)  // Exceptional conditions

  // Actions
  #define EV_ADD       0x0001  // Add event to kq (implies enable)
  #define EV_DELETE    0x0002  // Delete event from kq
  #define EV_ENABLE    0x0004  // Enable event
  #define EV_DISABLE   0x0008  // Disable event (not reported)

  // Flags
  #define EV_ONESHOT   0x0010  // Only report one occurrence
  #define EV_CLEAR     0x0020  // Clear event state after reporting
  #define EV_RECEIPT   0x0040  // Force EV_ERROR on success, data=0
  #define EV_DISPATCH  0x0080  // Disable event after reporting

  #define EV_SYSFLAGS  0xf800  // Reserved by system
  #define EV_FLAG1     0x2000  // Filter-specific flag

  // Returned values
  #define EV_EOF       0x8000  // EOF detected
  #define EV_ERROR     0x4000  // error, data contains errno

#else

  #include <sys/event.h>
  #include <sys/types.h>

#endif

#include <algorithm>
#include <limits>
#include <memory>
#include <mutex>
#include <system_error>
#include <unordered_map>

#include "final/eventloop/eventloop.h"
#include "final/eventloop/timer_monitor.h"
#include "final/util/fsystem.h"

namespace finalcut
{

// Using-declaration
using TimerNodesList = std::unordered_map<int, KqueueTimer*>;
using KEventList = std::vector<struct kevent>;

// Helper functions
//----------------------------------------------------------------------
#if defined(UNIT_TEST)
static auto getKqueue() -> int
{
  // Creates a new kernel event queue
  static const auto& fsystem = FSystem::getInstance();
  return fsystem->kqueue();
}
#else
static auto getKqueue() -> const int&
{
  // Creates a new kernel event queue
  static const auto& fsystem = FSystem::getInstance();
  static const auto kq = fsystem->kqueue();
  return kq;
}
#endif

//----------------------------------------------------------------------
static auto getTimerID() -> int&
{
  // Timer id generator (sequential number)
  static int timer_id = 0;
  static constexpr auto max = std::numeric_limits<int>::max();

  if ( timer_id < max )
    timer_id++;
  else
    timer_id = 1;

  return timer_id;
}

//----------------------------------------------------------------------
constexpr auto durationToMilliseconds (std::chrono::nanoseconds duration) -> int
{
  return int(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
}

//----------------------------------------------------------------------
static auto getTimerNodes() -> TimerNodesList&
{
  // Encapsulate global list object
  static const auto& timer_nodes = std::make_unique<TimerNodesList>();
  return *timer_nodes;
}

//----------------------------------------------------------------------
static auto getKEvents() -> KEventList&
{
  // Encapsulate global list object
  static const auto& time_events = std::make_unique<KEventList>();
  return *time_events;
}


//----------------------------------------------------------------------
// TimerNode
//----------------------------------------------------------------------

using TimerNode = std::pair<int, KqueueTimer*>;

inline void validateTimer (const TimerNode& timer)
{
  if ( ! timer.second )
    throw std::invalid_argument("Timer monitor pointer cannot be null");

  if ( timer.first < 0 )
    throw std::invalid_argument("Timer ID must be >= 0");
}

//----------------------------------------------------------------------
// class KqueueHandler
//----------------------------------------------------------------------

class KqueueHandler final
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
    void operator () (Monitor* monitor, short revents) const
    {
      // Check active events in the event list
      static const auto& fsystem = FSystem::getInstance();
      struct timespec timeout{0, 0};  // Do not wait
      auto& time_events = getKEvents();

      if ( time_events.empty() )
        return;

      auto data = time_events.data();
      const auto size = int(time_events.size());
      const auto n = fsystem->kevent(getKqueue(), nullptr, 0, data, size, &timeout);

      if ( n <= 0 )
        return;

      static auto& timer_nodes = getTimerNodes();

      for (const auto& time_event : time_events)
      {
        if ( time_event.filter != EVFILT_TIMER )
          continue;

        std::lock_guard<std::mutex> lock_guard(timer_nodes_mutex);

        const auto event_ident = int(time_event.ident);
        const auto iter = timer_nodes.find(event_ident);

        if ( iter == timer_nodes.end() )
          continue;

        auto kqueue_timer_ptr = iter->second;

        if ( ! kqueue_timer_ptr )
          continue;

        if ( kqueue_timer_ptr->first_interval )  // First timer event
        {
          kqueue_timer_ptr->first_interval = false;
          struct kevent ev_set{};
          const auto kq_flags = EV_ADD | EV_ENABLE;
          const auto ms = kqueue_timer_ptr->timer_spec.period_ms;

          // Filling the struct events with values
          EV_SET(&ev_set, uintptr_t(event_ident), EVFILT_TIMER, kq_flags, 0, ms, nullptr);

          // Register event with kqueue
          if ( fsystem->kevent(getKqueue(), &ev_set, 1, nullptr, 0, nullptr) != 0 )
            throw monitor_error{"Cannot register event in kqueue."};
        }

        if ( kqueue_timer_ptr->timer_handler )
          kqueue_timer_ptr->timer_handler(monitor, revents);
      }
    }

  private:
    // Data members
    mutable std::mutex timer_nodes_mutex{};
};


//----------------------------------------------------------------------
// struct KqueueHandlerInstaller
//----------------------------------------------------------------------

struct KqueueHandlerInstaller final
{
  // constructor
  KqueueHandlerInstaller()
  {
    // Creates a new kernel event queue
    if ( getKqueue() != -1 )
      return;

    // Could not get kqueue
    const int error = errno;
    const std::error_code err_code{error, std::generic_category()};
    const std::system_error sys_err{err_code, strerror(error)};
    throw sys_err;
  }

  // destructor
  ~KqueueHandlerInstaller()
  {
    static const auto& fsystem = FSystem::getInstance();
    fsystem->close(getKqueue());
  }
};

//----------------------------------------------------------------------
static auto startKqueueHandlerInstaller() -> KqueueHandlerInstaller*
{
  static const auto& kqueue_handler = std::make_unique<KqueueHandlerInstaller>();
  return kqueue_handler.get();
}

// static class attributes
KqueueHandlerInstaller* KqueueTimer::kqueue_handler_installer{};


//----------------------------------------------------------------------
// class KqueueTimer
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
KqueueTimer::KqueueTimer (EventLoop* eloop)
  : TimerMonitorImpl(eloop)
{
  kqueue_handler_installer = startKqueueHandlerInstaller();
}

//----------------------------------------------------------------------
KqueueTimer::~KqueueTimer() noexcept  // destructor
{
  cleanupResources();
}


// public methods of KqueueTimer
//----------------------------------------------------------------------
void KqueueTimer::setInterval ( std::chrono::nanoseconds first,
                                std::chrono::nanoseconds periodic )
{
  // Input validation
  validateIntervals (first, periodic);

  struct kevent ev_set{};
  static const auto& fsystem = FSystem::getInstance();
  timer_spec.first_ms  = durationToMilliseconds(first);
  timer_spec.period_ms = durationToMilliseconds(periodic);
  const auto kq_flags = EV_ADD | EV_ENABLE | EV_ONESHOT;
  const auto ms = timer_spec.first_ms;

  // Filling the struct events with values
  EV_SET(&ev_set, uintptr_t(timer_id), EVFILT_TIMER, kq_flags, 0, ms, nullptr);

  // Register event with kqueue
  if ( fsystem->kevent(getFileDescriptor(), &ev_set, 1, nullptr, 0, nullptr) != 0 )
    throw monitor_error{"Cannot register event in kqueue."};
}

//----------------------------------------------------------------------
void KqueueTimer::trigger (short return_events)
{
  try
  {
    // Call base class trigger
    Monitor::trigger(return_events);
  }
  catch (...)
  {
    throw;  // Re-throw other errors
  }
}

// private methods of KqueueTimer
//----------------------------------------------------------------------
void KqueueTimer::init()
{
  try
  {
    setFileDescriptor (getKqueue());
    setEvents (POLLIN);
    setHandler (KqueueHandler());
    timer_id = getTimerID();

    // Add event
    getKEvents().emplace_back();

    // Add timer node
    auto timer = std::make_pair(timer_id, this);
    validateTimer(timer);
    getTimerNodes().emplace(std::move(timer));

    setInitialized();
  }
  catch (...)
  {
    cleanupResources();  // Clean up on failure
    throw;  // Re-throw the original exception
  }
}
//----------------------------------------------------------------------
void KqueueTimer::cleanupResources() noexcept
{
  auto& timer_nodes{getTimerNodes()};
  auto& time_events{getKEvents()};

  // Remove timer node

  const auto iter = timer_nodes.find(timer_id);

  if ( iter != timer_nodes.end() )
  {
    // Remove corresponding events
    auto is_timer_id = [this] (const auto& event)
                       {
                         return int(event.ident) == timer_id;
                       };
    time_events.erase ( std::remove_if ( time_events.begin()
                                       , time_events.end()
                                       , is_timer_id )
                      , time_events.end() );
    // Remove timer node
    timer_nodes.erase(iter);
  }

  // Reset timer state
  timer_id = NO_TIMER_ID;
  first_interval = true;
  timer_spec = TimerSpec{};
  timer_handler = handler_t{};
}

}  // namespace finalcut

#endif  // defined(USE_KQUEUE_TIMER) || defined(UNIT_TEST)
