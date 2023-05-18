/***********************************************************************
* fstyle.h - Style attribute of a character                            *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2020-2023 Markus Gans                                      *
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
    auto getClassName() const -> FString
    { return "FStyle"; }

    inline auto getStyle() const noexcept -> Style
    { return attribute; }

    // Mutators
    inline void setStyle (const FStyle& style) noexcept
    { attribute = style.attribute & Style(ATTRIBUTE_MASK); }

    inline void setStyle (Style attr) noexcept
    { attribute = attr & Style(ATTRIBUTE_MASK); }

    auto toFAttribute() const noexcept -> FAttribute
    {
      FAttribute fattr{};
      fattr.bit.bold               = (attribute & Style::Bold) != Style::None;
      fattr.bit.dim                = (attribute & Style::Dim) != Style::None;
      fattr.bit.italic             = (attribute & Style::Italic) != Style::None;
      fattr.bit.underline          = (attribute & Style::Underline) != Style::None;
      fattr.bit.blink              = (attribute & Style::Blink) != Style::None;
      fattr.bit.reverse            = (attribute & Style::Reverse) != Style::None;
      fattr.bit.standout           = (attribute & Style::Standout) != Style::None;
      fattr.bit.invisible          = (attribute & Style::Invisible) != Style::None;
      fattr.bit.protect            = (attribute & Style::Protected) != Style::None;
      fattr.bit.crossed_out        = (attribute & Style::CrossedOut) != Style::None;
      fattr.bit.dbl_underline      = (attribute & Style::DoubleUnderline) != Style::None;
      fattr.bit.transparent        = (attribute & Style::Transparent) != Style::None;
      fattr.bit.color_overlay      = (attribute & Style::ColorOverlay) != Style::None;
      fattr.bit.inherit_background = (attribute & Style::InheritBackground) != Style::None;
      return fattr;
    }

  private:
    // Constant
    static constexpr auto ATTRIBUTE_MASK = \
        Style((uInt16(Style::InheritBackground) << 1) - 1);

    // Data members
    Style attribute;  // Save character attributes
};

}  // namespace finalcut

#endif  // FSTYLE_H
