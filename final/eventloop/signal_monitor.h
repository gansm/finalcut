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
 *   ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *   ▕ SignalMonitor ▏
 *   ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef SIGNAL_MONITOR_H
#define SIGNAL_MONITOR_H

#include <map>
#include <memory>

#include "monitor.h"

class SigactionImpl;

class SignalMonitor final : public Monitor
{
  public:
    // Constructor
    SignalMonitor() = delete;
    explicit SignalMonitor(EventLoop*);

    // Destructor
    ~SignalMonitor() noexcept override;

    // Copy assignment operator (=)
    SignalMonitor& operator = (const SignalMonitor&);

    // Methods
    void init (int, handler_t, void*);
    void trigger (short) override;

  private:
    // Methods
    static void onSignal (int);
    auto getSigactionImpl() const -> const SigactionImpl*;
    auto getSigactionImpl() -> SigactionImpl*;

    // Data members
    int signal_number{-1};
    int signal_pipe_fd[2]{-1, -1};
    static std::map<int, SignalMonitor*> signal_monitors;
    std::unique_ptr<SigactionImpl> impl;
};

// SigactionImpl inline functions
//----------------------------------------------------------------------
auto SignalMonitor::getSigactionImpl() const -> const SigactionImpl*
{ return impl.get(); }

//----------------------------------------------------------------------
auto SignalMonitor::getSigactionImpl() -> SigactionImpl*
{ return impl.get(); }

#endif  // SIGNAL_MONITOR_H
