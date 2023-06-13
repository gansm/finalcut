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

#include <csignal>
#include <map>

#include "monitor.h"

class SignalMonitor final : public Monitor
{
  public:
    // Constructor
    SignalMonitor() = delete;
    SignalMonitor(const SignalMonitor&) = delete;
    SignalMonitor(const SignalMonitor&&) = delete;
    explicit SignalMonitor(EventLoop*);
    ~SignalMonitor() noexcept override;

    // Methods
    void init (int, handler_t, void*);
    void trigger (short) override;

  private:
    // Method
    auto operator=(const SignalMonitor&) -> SignalMonitor& = delete;
    auto operator=(const SignalMonitor&&) -> SignalMonitor& = delete;
    static void onSignal (int);

    // Data members
    int signal_number{-1};
    int signal_pipe_fd[2]{-1, -1};
    struct sigaction old_sig_action{};
    static std::map<int, SignalMonitor*> signal_monitors;
};

#endif  // SIGNAL_MONITOR_H
