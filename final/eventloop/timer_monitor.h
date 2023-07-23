/***********************************************************************
* timer_monitor.h - Time monitoring object                             *
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
 *          ▕▔▔▔▔▔▔▔▔▔▏
 *          ▕ Monitor ▏
 *          ▕▁▁▁▁▁▁▁▁▁▏
 *               ▲
 *               │
 *      ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *      ▕ TimerMonitorImpl ▏
 *      ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *        ▲              ▲
 *        │              │
 *  ▕▔▔▔▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *  ▕ PosixTimer ▏ ▕ KqueueTimer ▏ (platform-specific)
 *  ▕▁▁▁▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *           ▲         ▲
 *           │         │
 *         ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *         ▕ TimerMonitor ▏
 *         ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef TIMER_MONITOR_H
#define TIMER_MONITOR_H

#if (defined(__APPLE__) && defined(__MACH__)) || defined(__OpenBSD__)
  #define USE_KQUEUE_TIMER
  #include <unistd.h>
  #include <sys/event.h>
  #include <sys/types.h>
#else
  #define USE_POSIX_TIMER
#endif

#include <time.h>

#include <array>
#include <chrono>
#include <vector>

#include "final/eventloop/monitor.h"
#include "final/ftypes.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class TimerMonitorImpl
//----------------------------------------------------------------------

class TimerMonitorImpl : public Monitor
{
  public:
    // Constructor
    explicit TimerMonitorImpl (EventLoop* eloop)
      : Monitor(eloop)
    { }

    // Destructor
    ~TimerMonitorImpl() override;

    // Methods
    virtual void init (handler_t, void*) = 0;
    virtual void setInterval ( std::chrono::nanoseconds
                             , std::chrono::nanoseconds ) = 0;
};


//----------------------------------------------------------------------
// class PosixTimer
//----------------------------------------------------------------------

class PosixTimer : public TimerMonitorImpl
{
  public:
    // Constructor
    explicit PosixTimer (EventLoop*);

    // Destructor
    ~PosixTimer() noexcept override;

    // Methods
    void init (handler_t, void*) override;
    void setInterval ( std::chrono::nanoseconds
                     , std::chrono::nanoseconds ) override;
    void trigger(short) override;

  private:
#if defined(USE_POSIX_TIMER)
    // Data members
    timer_t timer_id{};
    std::array<int, 2> alarm_pipe_fd{NO_FILE_DESCRIPTOR, NO_FILE_DESCRIPTOR};
#endif  // defined(USE_POSIX_TIMER)
};


//----------------------------------------------------------------------
// class KqueueTimer
//----------------------------------------------------------------------

class KqueueTimer : public TimerMonitorImpl
{
  public:
    // Constructor
    explicit KqueueTimer (EventLoop*);

    // Destructor
    ~KqueueTimer() noexcept override;

    // Methods
    void init (handler_t, void*) override;
    void setInterval ( std::chrono::nanoseconds
                     , std::chrono::nanoseconds ) override;
    void trigger(short) override;

  private:
#if defined(USE_KQUEUE_TIMER)
    // Constants
    static constexpr int NO_TIMER_ID{-1};

    // Data members
    struct TimerSpec
    {
      int period_ms{};
      int first_ms{};
    };

    int       timer_id{NO_TIMER_ID};
    bool      first_interval{true};
    TimerSpec timer_spec{};
    handler_t timer_handler{};

    // Friend classes
    friend class KqueueHandler;
#endif  // defined(USE_KQUEUE_TIMER)
};


//----------------------------------------------------------------------
// struct TimerClass
//----------------------------------------------------------------------

struct TimerClass
{
  #if defined(__APPLE__) && defined(__MACH__)
    using type = KqueueTimer;
  #elif defined(__OpenBSD__)
    using type = KqueueTimer;
  #else
    using type = PosixTimer;
  #endif
};


//----------------------------------------------------------------------
// class TimerMonitor
//----------------------------------------------------------------------

class TimerMonitor final : public TimerClass::type
{
  public:
    TimerMonitor() = delete;

    // Disable copy constructor
    TimerMonitor (const TimerMonitor&) = delete;

    // Disable move constructor
    TimerMonitor (TimerMonitor&&) noexcept = delete;

    // Constructor
    explicit TimerMonitor (EventLoop* eloop)
      : TimerClass::type(eloop)
    { }

    // Destructor
    ~TimerMonitor() override;

    // Disable copy assignment operator (=)
    auto operator = (const TimerMonitor&) -> TimerMonitor& = delete;

    // Disable move assignment operator (=)
    auto operator = (TimerMonitor&&) noexcept -> TimerMonitor& = delete;
};

}  // namespace finalcut

#endif  // TIMER_MONITOR_H
