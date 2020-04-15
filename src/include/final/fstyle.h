/***********************************************************************
* fstyle.h - Style attribute of a character                            *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2020 Markus Gans                                           *
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
#include "final/fstring.h"
#include "final/ftypes.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FStyle
//----------------------------------------------------------------------

class FStyle
{
  public:
    // Constructors
    explicit FStyle (FAttribute attr = fc::Reset)
      : attribute{attr}
    { }

    // Copy constructor
    FStyle (const FStyle& style)
      : attribute{style.attribute}
    { }

    // Destructor
    ~FStyle() = default;

    // copy assignment operator (=)
    FStyle& operator = (const FStyle& style)
    {
      attribute = style.attribute;
      return *this;
    }

    // Accessor
    const FString getClassName() const
    { return "FStyle"; }

    FColor getStyle() const
    { return attribute; }

    // Mutators
    void setStyle (const FStyle& style)
    { attribute = style.attribute; }

    void setStyle (FAttribute attr)
    { attribute = attr; }

  private:
    // Data members
    FAttribute attribute;  // Save character attributes
};

}  // namespace finalcut

#endif  // FSTYLE_H
