/***********************************************************************
* fstyle.h - Style attribute of a character                            *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2020-2021 Markus Gans                                      *
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
 * ▕▔▔▔▔▔▔▔▔▏
 * ▕ FStyle ▏
 * ▕▁▁▁▁▁▁▁▁▏
 */

#ifndef FSTYLE_H
#define FSTYLE_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fc.h"
#include "final/ftypes.h"
#include "final/util/fstring.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FStyle
//----------------------------------------------------------------------

class FStyle
{
  public:
    // Constructors
    explicit FStyle (Style attr = Style::None)
      : attribute{attr}
    { }

    // Accessor
    FString getClassName() const
    { return "FStyle"; }

    Style getStyle() const noexcept
    { return attribute; }

    // Mutators
    void setStyle (const FStyle& style)
    { attribute = style.attribute; }

    void setStyle (Style attr) noexcept
    { attribute = attr; }

  private:
    // Data members
    Style attribute;  // Save character attributes
};

}  // namespace finalcut

#endif  // FSTYLE_H
