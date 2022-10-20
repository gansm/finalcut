/***********************************************************************
* fvtermattribute.h - Manipulation of FChar colors and attributes      *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2021-2022 Markus Gans                                      *
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

namespace finalcut
{

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
    inline static auto  getTermForegroundColor() -> FColor;
    inline static auto  getTermBackgroundColor() -> FColor;
    inline static auto  getAttribute() -> FChar&;

    // Mutators
    inline static void  setColor (FColor, FColor);
    inline static void  setNormal();
    inline static void  setBold (bool = true);
    inline static void  unsetBold();
    inline static void  setDim (bool = true);
    inline static void  unsetDim();
    inline static void  setItalic (bool = true);
    inline static void  unsetItalic();
    inline static void  setUnderline (bool = true);
    inline static void  unsetUnderline();
    inline static void  setBlink (bool = true);
    inline static void  unsetBlink();
    inline static void  setReverse (bool = true);
    inline static void  unsetReverse();
    inline static void  setStandout (bool = true);
    inline static void  unsetStandout();
    inline static void  setInvisible (bool = true);
    inline static void  unsetInvisible();
    inline static void  setProtected (bool = true);
    inline static void  unsetProtected();
    inline static void  setCrossedOut (bool = true);
    inline static void  unsetCrossedOut();
    inline static void  setDoubleUnderline (bool = true);
    inline static void  unsetDoubleUnderline();
    inline static void  setAltCharset (bool = true);
    inline static void  unsetAltCharset();
    inline static void  setPCcharset (bool = true);
    inline static void  unsetPCcharset();
    inline static void  setTransparent (bool = true);
    inline static void  unsetTransparent();
    inline static void  setColorOverlay (bool = true);
    inline static void  unsetColorOverlay();
    inline static void  setInheritBackground (bool = true);
    inline static void  unsetInheritBackground();

    // Inquiries
    inline static auto  isBold() -> bool;
    inline static auto  isDim() -> bool;
    inline static auto  isItalic() -> bool;
    inline static auto  isUnderline() -> bool;
    inline static auto  isBlink() -> bool;
    inline static auto  isReverse() -> bool;
    inline static auto  isStandout() -> bool;
    inline static auto  isInvisible() -> bool;
    inline static auto  isProtected() -> bool;
    inline static auto  isCrossedOut() -> bool;
    inline static auto  isDoubleUnderline() -> bool;
    inline static auto  isAltCharset() -> bool;
    inline static auto  isPCcharset() -> bool;
    inline static auto  isTransparent() -> bool;
    inline static auto  isColorOverlay() -> bool;
    inline static auto  isInheritBackground() -> bool;

    // Methods
    static void  initAttribute();
    static void  print (const FStyle&);
    static void  print (const FColorPair&);

  private:
    // Data members
    static FChar next_attribute;
};


// FVTermAttribute inline functions
//----------------------------------------------------------------------
inline auto FVTermAttribute::getClassName() const -> FString
{ return "FVTermAttribute"; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::getTermForegroundColor() -> FColor
{ return next_attribute.fg_color; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::getTermBackgroundColor() -> FColor
{ return next_attribute.bg_color; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::getAttribute() -> FChar&
{ return next_attribute; }

//----------------------------------------------------------------------
inline void FVTermAttribute::setColor (FColor fg, FColor bg)
{
  // Changes colors
  next_attribute.fg_color = fg;
  next_attribute.bg_color = bg;
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setNormal()
{
  // Reset all character attributes
  next_attribute.attr.byte[0] = 0;
  next_attribute.attr.byte[1] = 0;
  next_attribute.attr.bit.no_changes = false;
  next_attribute.fg_color = FColor::Default;
  next_attribute.bg_color = FColor::Default;
}

//----------------------------------------------------------------------
inline void FVTermAttribute::setBold (bool enable)
{ next_attribute.attr.bit.bold = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetBold()
{ setBold(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setDim (bool enable)
{ next_attribute.attr.bit.dim = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetDim()
{ setDim(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setItalic (bool enable)
{ next_attribute.attr.bit.italic = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetItalic()
{ setItalic(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setUnderline (bool enable)
{ next_attribute.attr.bit.underline = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetUnderline()
{ setUnderline(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setBlink (bool enable)
{ next_attribute.attr.bit.blink = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetBlink()
{ setBlink(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setReverse (bool enable)
{ next_attribute.attr.bit.reverse = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetReverse()
{ setReverse(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setStandout (bool enable)
{ next_attribute.attr.bit.standout = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetStandout()
{ setStandout(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setInvisible (bool enable)
{ next_attribute.attr.bit.invisible = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetInvisible()
{ setInvisible(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setProtected (bool enable)
{ next_attribute.attr.bit.protect = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetProtected()
{ setProtected(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setCrossedOut (bool enable)
{ next_attribute.attr.bit.crossed_out = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetCrossedOut()
{ setCrossedOut(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setDoubleUnderline (bool enable)
{ next_attribute.attr.bit.dbl_underline = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetDoubleUnderline()
{ setDoubleUnderline(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setAltCharset (bool enable)
{ next_attribute.attr.bit.alt_charset = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetAltCharset()
{ setAltCharset(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setPCcharset (bool enable)
{ next_attribute.attr.bit.pc_charset = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetPCcharset()
{ setPCcharset(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setTransparent (bool enable)
{ next_attribute.attr.bit.transparent = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetTransparent()
{ setTransparent(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setColorOverlay (bool enable)
{ next_attribute.attr.bit.color_overlay = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetColorOverlay()
{ setColorOverlay(false); }

//----------------------------------------------------------------------
inline void FVTermAttribute::setInheritBackground (bool enable)
{ next_attribute.attr.bit.inherit_background = enable; }

//----------------------------------------------------------------------
inline void FVTermAttribute::unsetInheritBackground()
{ setInheritBackground(false); }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isBold() -> bool
{ return next_attribute.attr.bit.bold; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isDim() -> bool
{ return next_attribute.attr.bit.dim; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isItalic() -> bool
{ return next_attribute.attr.bit.italic; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isUnderline() -> bool
{ return next_attribute.attr.bit.underline; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isBlink() -> bool
{ return next_attribute.attr.bit.blink; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isReverse() -> bool
{ return next_attribute.attr.bit.reverse; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isStandout() -> bool
{ return next_attribute.attr.bit.standout; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isInvisible() -> bool
{ return next_attribute.attr.bit.invisible; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isProtected() -> bool
{ return next_attribute.attr.bit.protect; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isCrossedOut() -> bool
{ return next_attribute.attr.bit.crossed_out; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isDoubleUnderline() -> bool
{ return next_attribute.attr.bit.dbl_underline; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isAltCharset() -> bool
{ return next_attribute.attr.bit.alt_charset; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isPCcharset() -> bool
{ return next_attribute.attr.bit.pc_charset; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isTransparent() -> bool
{ return next_attribute.attr.bit.transparent; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isColorOverlay() -> bool
{ return next_attribute.attr.bit.color_overlay; }

//----------------------------------------------------------------------
inline auto FVTermAttribute::isInheritBackground() -> bool
{ return next_attribute.attr.bit.inherit_background; }

}  // namespace finalcut

#endif  // FVTERMATTRIBUTE_H
