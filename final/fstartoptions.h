/***********************************************************************
* fstartoptions.h - Contains the start options for initialization      *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2019-2023 Markus Gans                                      *
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
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FStartOptions ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FSTARTOPTIONS_H
#define FSTARTOPTIONS_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <fstream>
#include <iostream>

#include "final/fc.h"
#include "final/ftypes.h"
#include "final/util/fstring.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FStartOptions
//----------------------------------------------------------------------

class FStartOptions final
{
  public:
    // Constructors
    FStartOptions();

    // Accessors
    static auto getClassName() -> FString;
    static auto getInstance() -> FStartOptions&;

    // Mutator
    void setDefault();

    // Data members
    uInt8 cursor_optimisation   : 1;
    uInt8 mouse_support         : 1;
    uInt8 terminal_detection    : 1;
    uInt8 terminal_data_request : 1;
    uInt8 terminal_focus_events : 1;
    uInt8 sgr_optimizer         : 1;
    uInt8 vgafont               : 1;
    uInt8 newfont               : 1;

#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
    uInt8 meta_sends_escape     : 1;
    uInt8 change_cursorstyle    : 1;
    uInt8                       : 6;  // padding bits
#elif defined(__NetBSD__) || defined(__OpenBSD__)
    uInt8 meta_sends_escape     : 1;
    uInt8                       : 7;  // padding bits
#endif

    uInt16 dark_theme           : 1;
    uInt16 color_change         : 1;
    uInt16                      : 14;  // padding bits

    Encoding      encoding{Encoding::Unknown};
    std::ofstream logfile_stream{};
};

//----------------------------------------------------------------------
inline auto FStartOptions::getClassName() -> FString
{ return "FStartOptions"; }

}  // namespace finalcut

#endif  // FSTARTOPTIONS_H
