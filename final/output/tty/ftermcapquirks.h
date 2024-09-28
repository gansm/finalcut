/***********************************************************************
* ftermcapquirks.h - Termcap quirks for some well-known terminals      *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2024 Markus Gans                                      *
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
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FTermcapQuirks ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FTERMCAPQUIRKS_H
#define FTERMCAPQUIRKS_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/util/fstring.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FTermcapsQuirks
//----------------------------------------------------------------------

class FTermcapQuirks final
{
  public:
    // Constructors
    FTermcapQuirks() = default;

    // Accessor
    auto getClassName() const -> FString;

    // Methods
    static void terminalFixup();

  private:
    // Methods
#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
    static void freebsd();
#endif
    static void cygwin();
#ifdef linux
  #undef linux
#endif
    static void linux();
    static void xterm();
    static void rxvt();
    static void vte();
    static void kitty();
    static void putty();
    static void teraterm();
    static void sunConsole();
    static void screen();
    static void general();
    static void caModeExtension();
    static void repeatLastChar();
    static void ecma48();
};

// FTermcapQuirks inline functions
//----------------------------------------------------------------------
inline auto FTermcapQuirks::getClassName() const -> FString
{ return "FTermcapQuirks"; }

}  // namespace finalcut

#endif  // FTERMCAPQUIRKS_H
