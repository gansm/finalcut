/***********************************************************************
* fvtermattribute.h - Manipulation of FChar colors and attributes      *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2021-2026 Markus Gans                                      *
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
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FVTermAttribute ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FVTERMATTRIBUTE_H
#define FVTERMATTRIBUTE_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/ftypes.h"
#include "final/util/fstring.h"
#include "final/vterm/fcolorpair.h"

namespace finalcut
{

namespace internal
{

constexpr auto getAttrResetMask() noexcept -> uInt32
{
  FCharAttribute mask{};
  mask.bold = true;
  mask.dim = true;
  mask.italic = true;
  mask.underline = true;
  mask.blink = true;
  mask.reverse = true;
  mask.standout = true;
  mask.invisible = true;
  mask.protect = true;
  mask.crossed_out = true;
  mask.dbl_underline = true;
  mask.alt_charset = true;
  mask.pc_charset = true;
  mask.transparent = true;
  mask.color_overlay = true;
  mask.inherit_background = true;
  return 0xffffffff & ~(FCharAttribute_to_uInt32(mask));
}

struct attr_var
{
  static constexpr auto reset_mask = getAttrResetMask();
};

}  // namespace internal

// class forward declaration
class FColorPair;
class FStyle;

//----------------------------------------------------------------------
// class FVTermAttribute
//----------------------------------------------------------------------

class FVTermAttribute
{
  public:
    // Constructors
    FVTermAttribute();

    // Disable copy constructor
    FVTermAttribute (const FVTermAttribute&) = delete;

    // Disable move constructor
    FVTermAttribute (FVTermAttribute&&) noexcept = delete;

    // Destructor
    virtual ~FVTermAttribute() noexcept;

    // Disable copy assignment operator (=)
    auto operator = (const FVTermAttribute&) -> FVTermAttribute& = delete;

    // Disable move assignment operator (=)
    auto operator = (FVTermAttribute&&) noexcept -> FVTermAttribute& = delete;

    // Accessors
    virtual auto getClassName() const -> FString;
    static auto  getTermForegroundColor() noexcept -> FColor;
    static auto  getTermBackgroundColor() noexcept -> FColor;
    static auto  getAttribute() noexcept -> FChar&;

    // Mutators
    static void  setColor (FColor, FColor) noexcept;
    static void  setColor (const FColorPair&) noexcept;
    static void  setNormal() noexcept;
    static void  setBold (bool = true) noexcept;
    static void  unsetBold() noexcept;
    static void  setDim (bool = true) noexcept;
    static void  unsetDim() noexcept;
    static void  setItalic (bool = true) noexcept;
    static void  unsetItalic() noexcept;
    static void  setUnderline (bool = true) noexcept;
    static void  unsetUnderline() noexcept;
    static void  setBlink (bool = true) noexcept;
    static void  unsetBlink() noexcept;
    static void  setReverse (bool = true) noexcept;
    static void  unsetReverse() noexcept;
    static void  setStandout (bool = true) noexcept;
    static void  unsetStandout() noexcept;
    static void  setInvisible (bool = true) noexcept;
    static void  unsetInvisible() noexcept;
    static void  setProtected (bool = true) noexcept;
    static void  unsetProtected() noexcept;
    static void  setCrossedOut (bool = true) noexcept;
    static void  unsetCrossedOut() noexcept;
    static void  setDoubleUnderline (bool = true) noexcept;
    static void  unsetDoubleUnderline() noexcept;
    static void  setAltCharset (bool = true) noexcept;
    static void  unsetAltCharset() noexcept;
    static void  setPCcharset (bool = true) noexcept;
    static void  unsetPCcharset() noexcept;
    static void  setTransparent (bool = true) noexcept;
    static void  unsetTransparent() noexcept;
    static void  setColorOverlay (bool = true) noexcept;
    static void  unsetColorOverlay() noexcept;
    static void  setInheritBackground (bool = true) noexcept;
    static void  unsetInheritBackground() noexcept;

    // Inquiries
    static auto  isBold() noexcept -> bool;
    static auto  isDim() noexcept -> bool;
    static auto  isItalic() noexcept -> bool;
    static auto  isUnderline() noexcept -> bool;
    static auto  isBlink() noexcept -> bool;
    static auto  isReverse() noexcept -> bool;
    static auto  isStandout() noexcept -> bool;
    static auto  isInvisible() noexcept -> bool;
    static auto  isProtected() noexcept -> bool;
    static auto  isCrossedOut() noexcept -> bool;
    static auto  isDoubleUnderline() noexcept -> bool;
    static auto  isAltCharset() noexcept -> bool;
    static auto  isPCcharset() noexcept -> bool;
    static auto  isTransparent() noexcept -> bool;
    static auto  isColorOverlay() noexcept -> bool;
    static auto  isInheritBackground() noexcept -> bool;

    // Methods
    static void  initAttribute() noexcept;
    static void  print (const FStyle&);
    static void  print (const FColorPair&) noexcept;

  private:
    // Data members
    static FChar next_attribute;
};


// FVTermAttribute inline functions
//----------------------------------------------------------------------
inline auto FVTermAttribute::getClassName() const -> FString
{ return "FVTermAttribute"; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::getTermForegroundColor() noexcept -> FColor
{ return next_attribute.color.pair.fg; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::getTermBackgroundColor() noexcept -> FColor
{ return next_attribute.color.pair.bg; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::getAttribute() noexcept -> FChar&
{ return next_attribute; }

//----------------------------------------------------------------------
inline void FVTermAttribute::setColor (FColor fg, FColor bg) noexcept
{
  // Changes colors
  next_attribute.color.pair = {fg, bg};
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setColor (const FColorPair& pair) noexcept
{
  setColor (pair.getForegroundColor(), pair.getBackgroundColor());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setNormal() noexcept
{
  // Reset all character attributes
  next_attribute.attr.data = next_attribute.attr.data
                           & internal::attr_var::reset_mask;
  next_attribute.attr.bit.no_changes = false;
  next_attribute.color.pair = {FColor::Default, FColor::Default};
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setBold (bool enable) noexcept
{ next_attribute.attr.bit.bold = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetBold() noexcept
{ setBold(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setDim (bool enable) noexcept
{ next_attribute.attr.bit.dim = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetDim() noexcept
{ setDim(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setItalic (bool enable) noexcept
{ next_attribute.attr.bit.italic = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetItalic() noexcept
{ setItalic(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setUnderline (bool enable) noexcept
{ next_attribute.attr.bit.underline = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetUnderline() noexcept
{ setUnderline(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setBlink (bool enable) noexcept
{ next_attribute.attr.bit.blink = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetBlink() noexcept
{ setBlink(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setReverse (bool enable) noexcept
{ next_attribute.attr.bit.reverse = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetReverse() noexcept
{ setReverse(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setStandout (bool enable) noexcept
{ next_attribute.attr.bit.standout = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetStandout() noexcept
{ setStandout(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setInvisible (bool enable) noexcept
{ next_attribute.attr.bit.invisible = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetInvisible() noexcept
{ setInvisible(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setProtected (bool enable) noexcept
{ next_attribute.attr.bit.protect = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetProtected() noexcept
{ setProtected(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setCrossedOut (bool enable) noexcept
{ next_attribute.attr.bit.crossed_out = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetCrossedOut() noexcept
{ setCrossedOut(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setDoubleUnderline (bool enable) noexcept
{ next_attribute.attr.bit.dbl_underline = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetDoubleUnderline() noexcept
{ setDoubleUnderline(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setAltCharset (bool enable) noexcept
{ next_attribute.attr.bit.alt_charset = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetAltCharset() noexcept
{ setAltCharset(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setPCcharset (bool enable) noexcept
{ next_attribute.attr.bit.pc_charset = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetPCcharset() noexcept
{ setPCcharset(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setTransparent (bool enable) noexcept
{ next_attribute.attr.bit.transparent = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetTransparent() noexcept
{ setTransparent(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setColorOverlay (bool enable) noexcept
{ next_attribute.attr.bit.color_overlay = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetColorOverlay() noexcept
{ setColorOverlay(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setInheritBackground (bool enable) noexcept
{ next_attribute.attr.bit.inherit_background = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetInheritBackground() noexcept
{ setInheritBackground(false); }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isBold() noexcept -> bool
{ return next_attribute.attr.bit.bold; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isDim() noexcept -> bool
{ return next_attribute.attr.bit.dim; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isItalic() noexcept -> bool
{ return next_attribute.attr.bit.italic; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isUnderline() noexcept -> bool
{ return next_attribute.attr.bit.underline; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isBlink() noexcept -> bool
{ return next_attribute.attr.bit.blink; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isReverse() noexcept -> bool
{ return next_attribute.attr.bit.reverse; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isStandout() noexcept -> bool
{ return next_attribute.attr.bit.standout; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isInvisible() noexcept -> bool
{ return next_attribute.attr.bit.invisible; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isProtected() noexcept -> bool
{ return next_attribute.attr.bit.protect; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isCrossedOut() noexcept -> bool
{ return next_attribute.attr.bit.crossed_out; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isDoubleUnderline() noexcept -> bool
{ return next_attribute.attr.bit.dbl_underline; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isAltCharset() noexcept -> bool
{ return next_attribute.attr.bit.alt_charset; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isPCcharset() noexcept -> bool
{ return next_attribute.attr.bit.pc_charset; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isTransparent() noexcept -> bool
{ return next_attribute.attr.bit.transparent; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isColorOverlay() noexcept -> bool
{ return next_attribute.attr.bit.color_overlay; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isInheritBackground() noexcept -> bool
{ return next_attribute.attr.bit.inherit_background; }

}  // namespace finalcut

#endif  // FVTERMATTRIBUTE_H
