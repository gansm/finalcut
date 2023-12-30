/***********************************************************************
* fvtermattribute.h - Manipulation of FChar colors and attributes      *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2021-2023 Markus Gans                                      *
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
    static auto  getTermForegroundColor() -> FColor;
    static auto  getTermBackgroundColor() -> FColor;
    static auto  getAttribute() -> FChar&;

    // Mutators
    static void  setColor (FColor, FColor);
    static void  setColor (const FColorPair&);
    static void  setNormal();
    static void  setBold (bool = true);
    static void  unsetBold();
    static void  setDim (bool = true);
    static void  unsetDim();
    static void  setItalic (bool = true);
    static void  unsetItalic();
    static void  setUnderline (bool = true);
    static void  unsetUnderline();
    static void  setBlink (bool = true);
    static void  unsetBlink();
    static void  setReverse (bool = true);
    static void  unsetReverse();
    static void  setStandout (bool = true);
    static void  unsetStandout();
    static void  setInvisible (bool = true);
    static void  unsetInvisible();
    static void  setProtected (bool = true);
    static void  unsetProtected();
    static void  setCrossedOut (bool = true);
    static void  unsetCrossedOut();
    static void  setDoubleUnderline (bool = true);
    static void  unsetDoubleUnderline();
    static void  setAltCharset (bool = true);
    static void  unsetAltCharset();
    static void  setPCcharset (bool = true);
    static void  unsetPCcharset();
    static void  setTransparent (bool = true);
    static void  unsetTransparent();
    static void  setColorOverlay (bool = true);
    static void  unsetColorOverlay();
    static void  setInheritBackground (bool = true);
    static void  unsetInheritBackground();

    // Inquiries
    static auto  isBold() -> bool;
    static auto  isDim() -> bool;
    static auto  isItalic() -> bool;
    static auto  isUnderline() -> bool;
    static auto  isBlink() -> bool;
    static auto  isReverse() -> bool;
    static auto  isStandout() -> bool;
    static auto  isInvisible() -> bool;
    static auto  isProtected() -> bool;
    static auto  isCrossedOut() -> bool;
    static auto  isDoubleUnderline() -> bool;
    static auto  isAltCharset() -> bool;
    static auto  isPCcharset() -> bool;
    static auto  isTransparent() -> bool;
    static auto  isColorOverlay() -> bool;
    static auto  isInheritBackground() -> bool;

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
inline void FVTermAttribute::setColor (const FColorPair& pair)
{
  setColor (pair.getForegroundColor(), pair.getBackgroundColor());
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
