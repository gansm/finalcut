/***********************************************************************
* backend_monitor.h - Monitoring general objects                       *
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

/*  Inheritance diagram
 *  ═══════════════════
 *
 *      ▕▔▔▔▔▔▔▔▔▔▏
 *      ▕ Monitor ▏
 *      ▕▁▁▁▁▁▁▁▁▁▏
 *           ▲
 *           │
 *   ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏1       1▕▔▔▔▔▔▔▔▔▔▔▏
 *   ▕ BackendMonitor ▏- - - - -▕ PipeData ▏
 *   ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏         ▕▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef BACKEND_MONITOR_H
#define BACKEND_MONITOR_H

#include "final/eventloop/monitor.h"
#include "final/eventloop/pipedata.h"
#include "final/util/fstring.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class BackendMonitor
//----------------------------------------------------------------------

class BackendMonitor final : public Monitor
{
  public:
    explicit BackendMonitor (EventLoop*);

    // Disable default constructor
    BackendMonitor() = delete;

    // Disable copy constructor
    BackendMonitor (const BackendMonitor&) = delete;

    // Disable move constructor
    BackendMonitor (BackendMonitor&&) = delete;

    // Destructor
    ~BackendMonitor() noexcept override;

    // Disable copy assignment operator (=)
    auto operator = (const BackendMonitor&) -> BackendMonitor& = delete;

    // Disable move assignment operator (=)
    auto operator = (BackendMonitor&&) noexcept -> BackendMonitor& = delete;

    // Accessor
    auto getClassName() const -> FString override;

    // Mutator
    void setEvent() const noexcept;

    // Methods
    template <typename T>
    void init (handler_t, T&&);
    void trigger (short) override;

  private:
    // Constants
    static constexpr std::uint64_t SIGNAL_NOTIFICATION{1U};

    // Mutator
    void clearEvent() const;

    // Methods
    void init();
    void validate (const handler_t&) const;

    // Data members
    PipeData self_pipe{NO_FILE_DESCRIPTOR, NO_FILE_DESCRIPTOR};
};

// BackendMonitor inline functions
//----------------------------------------------------------------------
inline auto BackendMonitor::getClassName() const -> FString
{ return "BackendMonitor"; }

//----------------------------------------------------------------------
template <typename T>
inline void BackendMonitor::init (handler_t hdl, T&& uc)
{
  validate (hdl);

  try
  {
    setHandler (std::move(hdl));
    setUserContext (std::forward<T>(uc));
    init();
  }
  catch (...)
  {
    setHandler(handler_t{});  // Clear handler
    clearUserContext();       // Clear user context
    throw;  // Re-throw the original exception
  }
}

//----------------------------------------------------------------------
inline void BackendMonitor::validate (const handler_t& hdl) const
{
  if ( isInitialized() )
    throw monitor_error{"This instance has already been initialised."};

  if ( ! hdl )
    throw monitor_error{"Handler cannot be null."};
}

}  // namespace finalcut

#endif  // BACKEND_MONITOR_H
