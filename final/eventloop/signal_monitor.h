/***********************************************************************
* signal_monitor.h - Signal monitoring object                          *
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
 *      ▕▔▔▔▔▔▔▔▔▔▏
 *      ▕ Monitor ▏
 *      ▕▁▁▁▁▁▁▁▁▁▏
 *           ▲
 *           │
 *   ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏1       1▕▔▔▔▔▔▔▔▔▔▔▏
 *   ▕ SignalMonitor ▏- - - - -▕ PipeData ▏
 *   ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏         ▕▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef SIGNAL_MONITOR_H
#define SIGNAL_MONITOR_H

#include <map>
#include <memory>

#include "final/eventloop/monitor.h"
#include "final/eventloop/pipedata.h"
#include "final/util/fstring.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class SignalMonitor
//----------------------------------------------------------------------

class SignalMonitor final : public Monitor
{
  public:
    explicit SignalMonitor(EventLoop*);

    // Disable default constructor
    SignalMonitor() = delete;

    // Disable copy constructor
    SignalMonitor (const SignalMonitor&) = delete;

    // Disable move constructor
    SignalMonitor (SignalMonitor&&) = delete;

    // Destructor
    ~SignalMonitor() noexcept override;

    // Disable copy assignment operator (=)
    auto operator = (const SignalMonitor&) -> SignalMonitor& = delete;

    // Disable move assignment operator (=)
    auto operator = (SignalMonitor&&) noexcept -> SignalMonitor& = delete;

    // Accessor
    auto getClassName() const -> FString override;

    // Methods
    template <typename T>
    void init (int, handler_t, T&&);
    void trigger (short) override;

  private:
    // class forward declaration
    class SigactionImpl;

    // Constants
    static constexpr int INVALID_SIGNAL{-1};
    static constexpr std::uint64_t SIGNAL_NOTIFICATION{1U};

    // Accessor
    auto getSigactionImpl() const -> const SigactionImpl*;
    auto getSigactionImpl() -> SigactionImpl*;

    // Methods
    static void onSignal (int);
    void init();
    void validate (int, const handler_t&) const;
    void handledAlarmSignal() const;
    void ensureSignalIsUnmonitored() const;
    void createPipe();
    void installSignalHandler();
    void cleanupResources() noexcept;
    void enterMonitorInstanceInTable();

    // Data members
    int signal_number{INVALID_SIGNAL};
    PipeData signal_pipe{NO_FILE_DESCRIPTOR, NO_FILE_DESCRIPTOR};
    std::unique_ptr<SigactionImpl> impl;
};

// SignalMonitor inline functions
//----------------------------------------------------------------------
inline auto SignalMonitor::getClassName() const -> FString
{ return "SignalMonitor"; }

//----------------------------------------------------------------------
inline auto SignalMonitor::getSigactionImpl() const -> const SigactionImpl*
{ return impl.get(); }

//----------------------------------------------------------------------
inline auto SignalMonitor::getSigactionImpl() -> SigactionImpl*
{ return impl.get(); }

//----------------------------------------------------------------------
template <typename T>
inline void SignalMonitor::init (int sn, handler_t hdl, T&& uc)
{
  try
  {
    validate(sn, hdl);
    signal_number = sn;
    setHandler (std::move(hdl));
    setUserContext (std::forward<T>(uc));
    init();
  }
  catch (...)
  {
    signal_number = INVALID_SIGNAL;  // Clear signal number
    setHandler(handler_t{});         // Clear handler
    clearUserContext();              // Clear user context
    cleanupResources();
    throw;  // Re-throw the original exception
  }
}

//----------------------------------------------------------------------
inline void SignalMonitor::validate (int sn, const handler_t& hdl) const
{
  if ( isInitialized() )
    throw monitor_error{"This instance has already been initialised."};

  if ( sn <= 0 || sn >= NSIG )
    throw monitor_error{ "Invalid signal number: must be > 0 and < "
                       + std::to_string(NSIG) };

  if ( ! hdl )
    throw monitor_error{"Handler cannot be null"};
}

}  // namespace finalcut

#endif  // SIGNAL_MONITOR_H
