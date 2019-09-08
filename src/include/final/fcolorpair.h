/***********************************************************************
* fcolorpair.h - Foreground and background color of a character        *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2019 Markus Gans                                           *
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
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FClassName ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FCOLORPAIR_H
#define FCOLORPAIR_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fc.h"
#include "final/ftypes.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FColorPair
//----------------------------------------------------------------------

class FColorPair
{
  public:
    // Constructors
    explicit FColorPair (FColor fg = fc::Default, FColor bg = fc::Default)
      : fg_color{fg}
      , bg_color{bg}
    { }

    // Copy constructor
    FColorPair (const FColorPair& pair)
      : fg_color{pair.fg_color}
      , bg_color{pair.bg_color}
    { }

    // Destructor
    ~FColorPair() = default;

    // Assignment operator (=)
    FColorPair& operator = (const FColorPair& pair)
    {
      fg_color = pair.fg_color;
      bg_color = pair.bg_color;
      return *this;
    }

    // Accessor
    const char* getClassName() const
    { return "FColorPair"; }

    FColor getForegroundColor() const
    { return fg_color; }

    FColor getBackgroundColor() const
    { return bg_color; }

    // Mutators
    void setForegroundColor (FColor color)
    { fg_color = color; }

    void setBackgroundColor (FColor color)
    { bg_color = color; }

    void setColorPair (const FColorPair& pair)
    {
      fg_color = pair.fg_color;
      bg_color = pair.bg_color;
    }

    void setColorPair (FColor fg, FColor bg)
    {
      fg_color = fg;
      bg_color = bg;
    }

    // Methods
    void swap()
    {
      std::swap (fg_color, bg_color);
    }

  private:
    // Data members
    FColor fg_color;  // Foreground color
    FColor bg_color;  // Background color
};

}  // namespace finalcut

#endif  // FCOLORPAIR_H


