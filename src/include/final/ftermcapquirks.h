/***********************************************************************
* ftermcapquirks.h - Termcap quirks for some well-known terminals      *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2018 Markus Gans                                           *
*                                                                      *
* The Final Cut is free software; you can redistribute it and/or       *
* modify it under the terms of the GNU Lesser General Public License   *
* as published by the Free Software Foundation; either version 3 of    *
* the License, or (at your option) any later version.                  *
*                                                                      *
* The Final Cut is distributed in the hope that it will be useful,     *
* but WITHOUT ANY WARRANTY; without even the implied warranty of       *
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

#include "final/fc.h"
#include "final/fterm.h"
#include "final/ftermcap.h"
#include "final/ftermdetection.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FTermcapsQuirks
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FTermcapQuirks
{
  public:
    // Constructors
    FTermcapQuirks();

    // Destructor
    ~FTermcapQuirks();

    // Accessor
    const char* getClassName() const;

    // Mutator
    static void setTerminalType (const char[]);
    static void setTermcapMap (FTermcap::tcap_map*);
    static void setFTermDetection (FTermDetection*);

    // Methods
    static void terminalFixup();

  private:
    // Methods
#if defined(__FreeBSD__) || defined(__DragonFly__)
    static void init_termcap_freebsd_quirks();
#endif
    static void init_termcap_cygwin_quirks();
    static void init_termcap_linux_quirks();
    static void init_termcap_xterm_quirks();
    static void init_termcap_rxvt_quirks();
    static void init_termcap_vte_quirks();
    static void init_termcap_putty_quirks();
    static void init_termcap_teraterm_quirks();
    static void init_termcap_sun_quirks();
    static void init_termcap_screen_quirks();
    static void init_termcap_general_quirks();

    // Data Members
    static char                termtype[256];
    static FTermcap::tcap_map* tcap;
    static FTermDetection*     term_detection;
};
#pragma pack(pop)

// FTermcapQuirks inline functions
//----------------------------------------------------------------------
inline const char* FTermcapQuirks::getClassName() const
{ return "FTermcapQuirks"; }

}  // namespace finalcut

#endif  // FTERMCAPQUIRKS_H
