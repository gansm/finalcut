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
    SignalMonitor(const SignalMonitor&) = delete;

    // Disable move constructor
    SignalMonitor(const SignalMonitor&&) = delete;

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

    // Methods
    static void onSignal (int);
    void init();
    void handledAlarmSignal() const;
    void ensureSignalIsUnmonitored() const;
    void createPipe();
    void installSignalHandler();
    void enterMonitorInstanceInTable();
    auto getSigactionImpl() const -> const SigactionImpl*;
    auto getSigactionImpl() -> SigactionImpl*;

    // Data members
    int signal_number{-1};
    PipeData signal_pipe{NO_FILE_DESCRIPTOR, NO_FILE_DESCRIPTOR};
    std::unique_ptr<SigactionImpl> impl;
};

// SignalMonitor inline functions
//----------------------------------------------------------------------
inline auto SignalMonitor::getClassName() const -> FString
{ return "SignalMonitor"; }

//----------------------------------------------------------------------
template <typename T>
inline void SignalMonitor::init (int sn, handler_t hdl, T&& uc)
{
  if ( isInitialized() )
    throw monitor_error{"This instance has already been initialised."};

  signal_number = sn;
  setHandler (std::move(hdl));
  setUserContext (std::forward<T>(uc));
  init();
}

}  // namespace finalcut

#endif  // SIGNAL_MONITOR_H
