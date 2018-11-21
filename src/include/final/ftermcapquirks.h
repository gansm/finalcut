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
#include "final/ftermdata.h"
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
    virtual ~FTermcapQuirks();

    // Accessor
    const char* getClassName() const;

    // Mutator
    static void setFTermData (FTermData*);
    static void setFTermDetection (FTermDetection*);

    // Methods
    static void terminalFixup();

  private:
    // Methods
#if defined(__FreeBSD__) || defined(__DragonFly__)
    static void freebsd();
#endif
    static void cygwin();
    static void linux();
    static void xterm();
    static void rxvt();
    static void vte();
    static void putty();
    static void teraterm();
    static void sunConsole();
    static void screen();
    static void general();

    // Data Members
    static FTermcap::tcap_map* tcap;
    static FTermData*          fterm_data;
    static FTermDetection*     term_detection;
};
#pragma pack(pop)

// FTermcapQuirks inline functions
//----------------------------------------------------------------------
inline const char* FTermcapQuirks::getClassName() const
{ return "FTermcapQuirks"; }

}  // namespace finalcut

#endif  // FTERMCAPQUIRKS_H
