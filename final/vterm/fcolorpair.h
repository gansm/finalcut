/***********************************************************************
* fcolorpair.h - Foreground and background color of a character        *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2019-2025 Markus Gans                                      *
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
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FColorPair ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FCOLORPAIR_H
#define FCOLORPAIR_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fc.h"
#include "final/ftypes.h"
#include "final/util/fstring.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FColorPair
//----------------------------------------------------------------------

class FColorPair
{
  public:
    // Constructors
    constexpr explicit FColorPair (FColor fg = FColor::Default, FColor bg = FColor::Default)
      : fg_color{fg}
      , bg_color{bg}
    { }

    // Accessor
    auto getClassName() const -> FString
    { return "FColorPair"; }

    constexpr auto getForegroundColor() const noexcept -> FColor
    { return fg_color; }

    constexpr auto getBackgroundColor() const noexcept -> FColor
    { return bg_color; }

    // Mutators
    constexpr void setForegroundColor (FColor color) noexcept
    { fg_color = color; }

    constexpr void setBackgroundColor (FColor color) noexcept
    { bg_color = color; }

    constexpr void setColorPair (const FColorPair& pair) noexcept
    {
      fg_color = pair.fg_color;
      bg_color = pair.bg_color;
    }

    constexpr void setColorPair (FColor fg, FColor bg) noexcept
    {
      fg_color = fg;
      bg_color = bg;
    }

    // Methods
    inline void swap() noexcept
    {
      std::swap (fg_color, bg_color);
    }

    friend constexpr auto operator == (const FColorPair& c1, const FColorPair& c2) noexcept -> bool
    {
      return c1.fg_color == c2.fg_color
          && c1.bg_color == c2.bg_color;
    }

    friend constexpr auto operator != (const FColorPair& c1, const FColorPair& c2) noexcept -> bool
    {
      return ! (c1 == c2);
    }

  private:
    // Data members
    FColor fg_color;  // Foreground color
    FColor bg_color;  // Background color
};

}  // namespace finalcut

#endif  // FCOLORPAIR_H
