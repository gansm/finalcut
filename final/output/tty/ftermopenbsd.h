/***********************************************************************
* ftermopenbsd.h - Contains the NetBSD/OpenBSD terminal functions      *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2022 Markus Gans                                      *
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

/*  Standalone class
 *  ════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FTermOpenBSD ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FTERMOPENBSD_H
#define FTERMOPENBSD_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <sys/ioctl.h>

#if defined(UNIT_TEST)
  #include <cstdint>

  #define WSKBDIO_GETENCODING uInt32(0x4004570F)
  #define WSKBDIO_SETENCODING uInt32(0x80045710)
  #define WSKBDIO_GETDEFAULTBELL uInt32(0x40105706)
  #define WSKBDIO_SETBELL uInt32(0x80105703)
  #define WSKBD_BELL_DOPITCH  0x1  // get/set pitch
  #define WSKBD_BELL_DOPERIOD 0x2  // get/set period
  #define WSKBD_BELL_DOVOLUME 0x4  // get/set volume
  #define WSKBD_BELL_DOALL    0x7  // all of the above

  using kbd_t = std::uint32_t;

  struct wskbd_bell_data
  {
    unsigned int which;   // values to get/set
    unsigned int pitch;   // pitch, in Hz
    unsigned int period;  // period, in milliseconds
    unsigned int volume;  // percentage of max volume
  };
#elif defined(__NetBSD__) || defined(__OpenBSD__)
  #include <sys/time.h>
  #include <dev/wscons/wsconsio.h>
#endif

#include "final/util/fstring.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FTermOpenBSD
//----------------------------------------------------------------------

class FTermOpenBSD final
{
  public:
    // Accessor
    auto        getClassName() const -> FString;
    static auto getInstance() -> FTermOpenBSD&;

    // Inquiries
    static auto isBSDConsole() -> bool;

    // Mutators
    static void disableMetaSendsEscape() noexcept;
    static void enableMetaSendsEscape() noexcept;

    // Methods
    static void init();
    static void finish();
    static auto setBeep (int, int) -> bool;
    static auto resetBeep() -> bool;

#if defined(__NetBSD__) || defined(__OpenBSD__) || defined(UNIT_TEST)
  private:
    // Methods
    static void warnNotInitialized();
    static auto saveBSDConsoleEncoding() -> bool;
    static auto setBSDConsoleEncoding (kbd_t) -> bool;
    static auto setBSDConsoleMetaEsc() -> bool;
    static auto resetBSDConsoleEncoding() -> bool;

    // Data members
    static kbd_t  bsd_keyboard_encoding;
    static bool   meta_sends_escape;
#endif  // defined(__NetBSD__) || defined(__OpenBSD__) || defined(UNIT_TEST)
};


// FTermOpenBSD inline functions
//----------------------------------------------------------------------
inline auto FTermOpenBSD::getClassName() const -> FString
{ return "FTermOpenBSD"; }

//----------------------------------------------------------------------
#if defined(__NetBSD__) || defined(__OpenBSD__) || defined(UNIT_TEST)
inline void FTermOpenBSD::enableMetaSendsEscape() noexcept
{ meta_sends_escape = true; }

//----------------------------------------------------------------------
inline void FTermOpenBSD::disableMetaSendsEscape() noexcept
{ meta_sends_escape = false; }

#endif  // defined(__NetBSD__) || defined(__OpenBSD__) || defined(UNIT_TEST)

}  // namespace finalcut

#endif  // FTERMOPENBSD_H
