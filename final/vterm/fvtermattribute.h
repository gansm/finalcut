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
  mask.no_changes = true;
  return 0xffffffff & ~(FCharAttribute_to_uInt32(mask));
}

struct attr_var
{
  static constexpr auto reset_mask = getAttrResetMask();
};

template<typename T>
constexpr auto createFCellColor (T setter) noexcept -> FCellColor
{
  FColors fcolors{};
  setter(fcolors);
  return FCellColor(fcolors);
}

constexpr void setDefaultColorPair (FColors& fcolors) noexcept
{
  fcolors = {FColor::Default, FColor::Default};
}

struct color_var
{
  static constexpr auto default_color_pair = createFCellColor(setDefaultColorPair);
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
    static void  setBold (bool) noexcept;
    static void  setBold() noexcept;
    static void  unsetBold() noexcept;
    static void  setDim (bool) noexcept;
    static void  setDim() noexcept;
    static void  unsetDim() noexcept;
    static void  setItalic (bool) noexcept;
    static void  setItalic() noexcept;
    static void  unsetItalic() noexcept;
    static void  setUnderline (bool) noexcept;
    static void  setUnderline() noexcept;
    static void  unsetUnderline() noexcept;
    static void  setBlink (bool) noexcept;
    static void  setBlink() noexcept;
    static void  unsetBlink() noexcept;
    static void  setReverse (bool) noexcept;
    static void  setReverse() noexcept;
    static void  unsetReverse() noexcept;
    static void  setStandout (bool) noexcept;
    static void  setStandout() noexcept;
    static void  unsetStandout() noexcept;
    static void  setInvisible (bool) noexcept;
    static void  setInvisible() noexcept;
    static void  unsetInvisible() noexcept;
    static void  setProtected (bool) noexcept;
    static void  setProtected() noexcept;
    static void  unsetProtected() noexcept;
    static void  setCrossedOut (bool) noexcept;
    static void  setCrossedOut() noexcept;
    static void  unsetCrossedOut() noexcept;
    static void  setDoubleUnderline (bool) noexcept;
    static void  setDoubleUnderline() noexcept;
    static void  unsetDoubleUnderline() noexcept;
    static void  setAltCharset (bool) noexcept;
    static void  setAltCharset() noexcept;
    static void  unsetAltCharset() noexcept;
    static void  setPCcharset (bool) noexcept;
    static void  setPCcharset() noexcept;
    static void  unsetPCcharset() noexcept;
    static void  setTransparent (bool) noexcept;
    static void  setTransparent() noexcept;
    static void  unsetTransparent() noexcept;
    static void  setColorOverlay (bool) noexcept;
    static void  setColorOverlay() noexcept;
    static void  unsetColorOverlay() noexcept;
    static void  setInheritBackground (bool) noexcept;
    static void  setInheritBackground() noexcept;
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
{ return next_attribute.color.getPair().fg; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::getTermBackgroundColor() noexcept -> FColor
{ return next_attribute.color.getPair().bg; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::getAttribute() noexcept -> FChar&
{ return next_attribute; }

//----------------------------------------------------------------------
inline void FVTermAttribute::setColor (FColor fg, FColor bg) noexcept
{
  // Changes colors
  next_attribute.color.setPair({fg, bg});
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
  next_attribute.color = internal::color_var::default_color_pair;
  next_attribute.unsetBit(internal::attr_var::reset_mask);
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setBold (bool enable) noexcept
{
  next_attribute.setBit(internal::attr::bold(), enable);
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setBold() noexcept
{
  next_attribute.setBit(internal::attr::bold());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetBold() noexcept
{
  next_attribute.unsetBit(internal::attr::bold_reset());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setDim (bool enable) noexcept
{
  next_attribute.setBit(internal::attr::dim(), enable);
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setDim() noexcept
{
  next_attribute.setBit(internal::attr::dim());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetDim() noexcept
{
  next_attribute.unsetBit(internal::attr::dim_reset());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setItalic (bool enable) noexcept
{
  next_attribute.setBit(internal::attr::italic(), enable);
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setItalic() noexcept
{
  next_attribute.setBit(internal::attr::italic());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetItalic() noexcept
{
  next_attribute.unsetBit(internal::attr::italic_reset());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setUnderline (bool enable) noexcept
{
  next_attribute.setBit(internal::attr::underline(), enable);
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setUnderline() noexcept
{
  next_attribute.setBit(internal::attr::underline());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetUnderline() noexcept
{
  next_attribute.unsetBit(internal::attr::underline_reset());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setBlink (bool enable) noexcept
{
  next_attribute.setBit(internal::attr::blink(), enable);
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setBlink() noexcept
{
  next_attribute.setBit(internal::attr::blink());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetBlink() noexcept
{
  next_attribute.unsetBit(internal::attr::blink_reset());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setReverse (bool enable) noexcept
{
  next_attribute.setBit(internal::attr::reverse(), enable);
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setReverse() noexcept
{
  next_attribute.setBit(internal::attr::reverse());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetReverse() noexcept
{
  next_attribute.unsetBit(internal::attr::reverse_reset());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setStandout (bool enable) noexcept
{
  next_attribute.setBit(internal::attr::standout(), enable);
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setStandout() noexcept
{
  next_attribute.setBit(internal::attr::standout());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetStandout() noexcept
{
  next_attribute.unsetBit(internal::attr::standout_reset());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setInvisible (bool enable) noexcept
{
  next_attribute.setBit(internal::attr::invisible(), enable);
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setInvisible() noexcept
{
  next_attribute.setBit(internal::attr::invisible());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetInvisible() noexcept
{
  next_attribute.unsetBit(internal::attr::invisible_reset());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setProtected (bool enable) noexcept
{
  next_attribute.setBit(internal::attr::protect(), enable);
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setProtected() noexcept
{
  next_attribute.setBit(internal::attr::protect());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetProtected() noexcept
{
  next_attribute.unsetBit(internal::attr::protect_reset());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setCrossedOut (bool enable) noexcept
{
  next_attribute.setBit(internal::attr::crossed_out(), enable);
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setCrossedOut() noexcept
{
  next_attribute.setBit(internal::attr::crossed_out());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetCrossedOut() noexcept
{
  next_attribute.unsetBit(internal::attr::crossed_out_reset());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setDoubleUnderline (bool enable) noexcept
{
  next_attribute.setBit(internal::attr::dbl_underline(), enable);
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setDoubleUnderline() noexcept
{
  next_attribute.setBit(internal::attr::dbl_underline());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetDoubleUnderline() noexcept
{
  next_attribute.unsetBit(internal::attr::dbl_underline_reset());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setAltCharset (bool enable) noexcept
{
  next_attribute.setBit(internal::attr::alt_charset(), enable);
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setAltCharset() noexcept
{
  next_attribute.setBit(internal::attr::alt_charset());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetAltCharset() noexcept
{
  next_attribute.unsetBit(internal::attr::alt_charset_reset());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setPCcharset (bool enable) noexcept
{
  next_attribute.setBit(internal::attr::pc_charset(), enable);
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setPCcharset() noexcept
{
  next_attribute.setBit(internal::attr::pc_charset());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetPCcharset() noexcept
{
  next_attribute.unsetBit(internal::attr::pc_charset_reset());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setTransparent (bool enable) noexcept
{
  next_attribute.setBit(internal::attr::transparent(), enable);
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setTransparent() noexcept
{
  next_attribute.setBit(internal::attr::transparent());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetTransparent() noexcept
{
  next_attribute.unsetBit(internal::attr::transparent_reset());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setColorOverlay (bool enable) noexcept
{
  next_attribute.setBit(internal::attr::color_overlay(), enable);
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setColorOverlay() noexcept
{
  next_attribute.setBit(internal::attr::color_overlay());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetColorOverlay() noexcept
{
  next_attribute.unsetBit(internal::attr::color_overlay_reset());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setInheritBackground (bool enable) noexcept
{
  next_attribute.setBit(internal::attr::inherit_background(), enable);
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setInheritBackground() noexcept
{
  next_attribute.setBit(internal::attr::inherit_background());
}

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetInheritBackground() noexcept
{
  next_attribute.unsetBit(internal::attr::inherit_background_reset());
}

//----------------------------------------------------------------------
inline auto FVTermAttribute::isBold() noexcept -> bool
{
  return next_attribute.isBitSet(internal::attr::bold());
}

//----------------------------------------------------------------------
inline auto FVTermAttribute::isDim() noexcept -> bool
{
  return next_attribute.isBitSet(internal::attr::dim());
}

//----------------------------------------------------------------------
inline auto FVTermAttribute::isItalic() noexcept -> bool
{
  return next_attribute.isBitSet(internal::attr::italic());
}

//----------------------------------------------------------------------
inline auto FVTermAttribute::isUnderline() noexcept -> bool
{
  return next_attribute.isBitSet(internal::attr::underline());
}

//----------------------------------------------------------------------
inline auto FVTermAttribute::isBlink() noexcept -> bool
{
  return next_attribute.isBitSet(internal::attr::blink());
}

//----------------------------------------------------------------------
inline auto FVTermAttribute::isReverse() noexcept -> bool
{
  return next_attribute.isBitSet(internal::attr::reverse());
}

//----------------------------------------------------------------------
inline auto FVTermAttribute::isStandout() noexcept -> bool
{
  return next_attribute.isBitSet(internal::attr::standout());
}

//----------------------------------------------------------------------
inline auto FVTermAttribute::isInvisible() noexcept -> bool
{
  return next_attribute.isBitSet(internal::attr::invisible());
}

//----------------------------------------------------------------------
inline auto FVTermAttribute::isProtected() noexcept -> bool
{
  return next_attribute.isBitSet(internal::attr::protect());
}

//----------------------------------------------------------------------
inline auto FVTermAttribute::isCrossedOut() noexcept -> bool
{
  return next_attribute.isBitSet(internal::attr::crossed_out());
}

//----------------------------------------------------------------------
inline auto FVTermAttribute::isDoubleUnderline() noexcept -> bool
{
  return next_attribute.isBitSet(internal::attr::dbl_underline());
}

//----------------------------------------------------------------------
inline auto FVTermAttribute::isAltCharset() noexcept -> bool
{
  return next_attribute.isBitSet(internal::attr::alt_charset());
}

//----------------------------------------------------------------------
inline auto FVTermAttribute::isPCcharset() noexcept -> bool
{
  return next_attribute.isBitSet(internal::attr::pc_charset());
}

//----------------------------------------------------------------------
inline auto FVTermAttribute::isTransparent() noexcept -> bool
{
  return next_attribute.isBitSet(internal::attr::transparent());
}

//----------------------------------------------------------------------
inline auto FVTermAttribute::isColorOverlay() noexcept -> bool
{
  return next_attribute.isBitSet(internal::attr::color_overlay());
}

//----------------------------------------------------------------------
inline auto FVTermAttribute::isInheritBackground() noexcept -> bool
{
  return next_attribute.isBitSet(internal::attr::inherit_background());
}

}  // namespace finalcut

#endif  // FVTERMATTRIBUTE_H
