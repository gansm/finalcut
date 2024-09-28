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
 *                          ▕▔▔▔▔▔▔▔▔▔▏
 *                          ▕ Monitor ▏
 *                          ▕▁▁▁▁▁▁▁▁▁▏
 *                               ▲
 *                               │
 *                      ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *                      ▕ TimerMonitorImpl ▏
 *                      ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *                        ▲              ▲
 *                        │              │
 * ▕▔▔▔▔▔▔▔▔▔▔▏1   1▕▔▔▔▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ PipeData ▏- - -▕ PosixTimer ▏ ▕ KqueueTimer ▏ (platform-specific)
 * ▕▁▁▁▁▁▁▁▁▁▁▏     ▕▁▁▁▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *                           ▲         ▲
 *                           │         │
 *                         ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *                         ▕ TimerMonitor ▏
 *                         ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
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

#include <ctime>

#include <chrono>
#include <vector>

#include "final/eventloop/monitor.h"
#include "final/eventloop/pipedata.h"
#include "final/ftypes.h"

namespace finalcut
{

// class and struct forward declaration
class SigAlrmHandlerInstaller;
struct KqueueHandlerInstaller;

//----------------------------------------------------------------------
// class TimerMonitorImpl
//----------------------------------------------------------------------
class TimerMonitorImpl : public Monitor
{
  public:
    // Using-declaration
    using Monitor::Monitor;

    // Destructor
    ~TimerMonitorImpl() override;

    // Methods
    virtual void setInterval ( std::chrono::nanoseconds
                             , std::chrono::nanoseconds ) = 0;
};


//----------------------------------------------------------------------
// class PosixTimer
//----------------------------------------------------------------------
class PosixTimer : public TimerMonitorImpl
{
  public:
    // Using-declaration
    using TimerMonitorImpl::TimerMonitorImpl;

    // Constructor
    explicit PosixTimer (EventLoop*);

    // Disable copy constructor
    PosixTimer (const PosixTimer&) = delete;

    // Disable move constructor
    PosixTimer (PosixTimer&&) noexcept = delete;

    // Destructor
    ~PosixTimer() noexcept override;

    // Disable copy assignment operator (=)
    auto operator = (const PosixTimer&) -> PosixTimer& = delete;

    // Disable move assignment operator (=)
    auto operator = (PosixTimer&&) noexcept -> PosixTimer& = delete;

    // Methods
    template <typename T>
    void init (handler_t, T&&);
    void setInterval ( std::chrono::nanoseconds
                     , std::chrono::nanoseconds ) override;
    void trigger(short) override;

  private:
    void init();
    void createAlarmPipe();
    void installTime();

#if defined(USE_POSIX_TIMER)
    // Data members
    timer_t timer_id{};
    PipeData alarm_pipe{NO_FILE_DESCRIPTOR, NO_FILE_DESCRIPTOR};
    static SigAlrmHandlerInstaller* sig_alrm_handler_installer;
#endif  // defined(USE_POSIX_TIMER)
};

#if defined(USE_POSIX_TIMER)
//----------------------------------------------------------------------
template <typename T>
inline void PosixTimer::init (handler_t hdl, T&& uc)
{
  if ( isInitialized() )
    throw monitor_error{"This instance has already been initialised."};

  setHandler (std::move(hdl));
  setUserContext (std::forward<T>(uc));
  init();
}
#endif  // defined(USE_POSIX_TIMER)


//----------------------------------------------------------------------
// class KqueueTimer
//----------------------------------------------------------------------

class KqueueTimer : public TimerMonitorImpl
{
  public:
    // Using-declaration
    using TimerMonitorImpl::TimerMonitorImpl;

    // Constructor
    explicit KqueueTimer (EventLoop*);

    // Disable copy constructor
    KqueueTimer (const KqueueTimer&) = delete;

    // Disable move constructor
    KqueueTimer (KqueueTimer&&) noexcept = delete;

    // Destructor
    ~KqueueTimer() noexcept override;

    // Disable copy assignment operator (=)
    auto operator = (const KqueueTimer&) -> KqueueTimer& = delete;

    // Disable move assignment operator (=)
    auto operator = (KqueueTimer&&) noexcept -> KqueueTimer& = delete;

    // Methods
    template <typename T>
    void init (handler_t, T&&);
    void setInterval ( std::chrono::nanoseconds
                     , std::chrono::nanoseconds ) override;
    void trigger(short) override;

  private:
    void init();

#if defined(USE_KQUEUE_TIMER) || defined(UNIT_TEST)
    struct TimerSpec
    {
      int period_ms{};
      int first_ms{};
    };

    // Constants
    static constexpr int NO_TIMER_ID{-1};

    // Data members
    int       timer_id{NO_TIMER_ID};
    bool      first_interval{true};
    TimerSpec timer_spec{};
    handler_t timer_handler{};
    static KqueueHandlerInstaller* kqueue_handler_installer;

    // Friend classes
    friend class KqueueHandler;
#endif  // defined(USE_KQUEUE_TIMER)
};

#if defined(USE_KQUEUE_TIMER) || defined(UNIT_TEST)
//----------------------------------------------------------------------
template <typename T>
inline void KqueueTimer::init (handler_t hdl, T&& uc)
{
  if ( isInitialized() )
    throw monitor_error{"This instance has already been initialised."};

  timer_handler = std::move(hdl);
  setUserContext (std::forward<T>(uc));
  init();
}
#endif  // defined(USE_KQUEUE_TIMER)


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
    // Using-declarations
    using base_class = TimerClass::type;
    using base_class::base_class;

    TimerMonitor() = delete;

    // Disable copy constructor
    TimerMonitor (const TimerMonitor&) = delete;

    // Disable move constructor
    TimerMonitor (TimerMonitor&&) noexcept = delete;

    // Destructor
    ~TimerMonitor() override;

    // Disable copy assignment operator (=)
    auto operator = (const TimerMonitor&) -> TimerMonitor& = delete;

    // Disable move assignment operator (=)
    auto operator = (TimerMonitor&&) noexcept -> TimerMonitor& = delete;

    // Accessor
    auto getClassName() const -> FString override;
};

// TimerMonitor inline functions
//----------------------------------------------------------------------
inline auto TimerMonitor::getClassName() const -> FString
{ return "TimerMonitor"; }

}  // namespace finalcut

#endif  // TIMER_MONITOR_H
