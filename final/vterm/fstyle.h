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

    auto toFCharAttribute() const noexcept -> FCharAttribute
    {
      FCharAttribute fchar_attr{};
      fchar_attr.bold               = (attribute & Style::Bold) != Style::None;
      fchar_attr.dim                = (attribute & Style::Dim) != Style::None;
      fchar_attr.italic             = (attribute & Style::Italic) != Style::None;
      fchar_attr.underline          = (attribute & Style::Underline) != Style::None;
      fchar_attr.blink              = (attribute & Style::Blink) != Style::None;
      fchar_attr.reverse            = (attribute & Style::Reverse) != Style::None;
      fchar_attr.standout           = (attribute & Style::Standout) != Style::None;
      fchar_attr.invisible          = (attribute & Style::Invisible) != Style::None;
      fchar_attr.protect            = (attribute & Style::Protected) != Style::None;
      fchar_attr.crossed_out        = (attribute & Style::CrossedOut) != Style::None;
      fchar_attr.dbl_underline      = (attribute & Style::DoubleUnderline) != Style::None;
      fchar_attr.transparent        = (attribute & Style::Transparent) != Style::None;
      fchar_attr.color_overlay      = (attribute & Style::ColorOverlay) != Style::None;
      fchar_attr.inherit_background = (attribute & Style::InheritBackground) != Style::None;
      return fchar_attr;
    }

    auto toFAttribute() const noexcept -> FAttribute
    {
      FCharAttribute fchar_attr{toFCharAttribute()};
      FAttribute fattr{};
      std::memcpy(&fattr, &fchar_attr, sizeof(fattr));
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
