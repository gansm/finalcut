/***********************************************************************
* foutput.h - Abstract Virtual terminal output class                   *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2021-2024 Markus Gans                                      *
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
 * ▕▔▔▔▔▔▔▔▔▔▏
 * ▕ FOutput ▏
 * ▕▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FOUTPUT_H
#define FOUTPUT_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <memory>

#include "final/fc.h"
#include "final/ftypes.h"
#include "final/output/fcolorpalette.h"
#include "final/vterm/fvterm.h"

// Fixes problem with -Weffc++, because the base class
// std::enable_shared_from_this has a non-virtual destructor
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"

namespace finalcut
{

//----------------------------------------------------------------------
// class FOutput
//----------------------------------------------------------------------

class FOutput : public std::enable_shared_from_this<FOutput>
{
  public:
    // Using-declarations
    using FSetPalette = FColorPalette::FSetPalette;

    // Constructor
    FOutput() = default;

    explicit FOutput (const FVTerm& t)
      : fvterm{t}
    { }

    // Destructor
    virtual ~FOutput() noexcept;

    // Accessors
    virtual auto getClassName() const -> FString;
    auto         getInstance() -> std::shared_ptr<FOutput>;
    auto         getFVTerm() const & -> const FVTerm&;
    virtual auto getColumnNumber() const -> std::size_t = 0;
    virtual auto getLineNumber() const -> std::size_t = 0;
    virtual auto getTabstop() const -> int = 0;
    virtual auto getMaxColor() const -> int = 0;
    virtual auto getEncoding() const -> Encoding = 0;
    virtual auto getKeyName (FKey) const -> FString = 0;

    // Mutators
    virtual void setCursor (FPoint) = 0;
    virtual void setCursor (CursorMode) = 0;
    virtual void hideCursor (bool = true) = 0;
    virtual void showCursor() = 0;
    virtual void setTerminalSize (FSize) = 0;
    virtual auto setVGAFont() -> bool = 0;
    virtual auto setNewFont() -> bool = 0;
    virtual void setNonBlockingRead (bool = true) = 0;
    template <typename ClassT>
    void         setColorPaletteTheme() const;
    template <typename ClassT>
    void         setColorPaletteTheme (const FSetPalette&) const;

    // Inquiries
    virtual auto isCursorHideable() const -> bool = 0;
    virtual auto isMonochron() const -> bool = 0;
    virtual auto isNewFont() const -> bool = 0;
    virtual auto isEncodable (const wchar_t&) const -> bool = 0;
    virtual auto isFlushTimeout() const -> bool = 0;
    virtual auto hasTerminalResized() const -> bool = 0;
    virtual auto allowsTerminalSizeManipulation() const -> bool = 0;
    virtual auto canChangeColorPalette() const -> bool = 0;
    virtual auto hasHalfBlockCharacter() const -> bool = 0;
    virtual auto hasShadowCharacter() const -> bool = 0;
    virtual auto areMetaAndArrowKeysSupported() const -> bool = 0;

    // Methods
    virtual void initTerminal (FVTerm::FTermArea*) = 0;
    virtual void finishTerminal() = 0;
    virtual auto updateTerminal() -> bool = 0;
    virtual void detectTerminalSize() = 0;
    virtual void commitTerminalResize() = 0;
    virtual void initScreenSettings() = 0;
    virtual auto scrollTerminalForward() -> bool = 0;
    virtual auto scrollTerminalReverse() -> bool = 0;
    virtual void clearTerminalAttributes() = 0;
    virtual void clearTerminalState() = 0;
    virtual auto clearTerminal (wchar_t = L' ') -> bool = 0;
    virtual void flush() = 0;
    virtual void beep() const = 0;

  private:
    // Accessors
    virtual auto getFSetPaletteRef() const & -> const FSetPalette& = 0;

    // Methods
    virtual auto isDefaultPaletteTheme() const -> bool = 0;
    virtual void redefineColorPalette() = 0;
    virtual void restoreColorPalette() = 0;

    // Data members
    const FVTerm& fvterm{};
};

// FOutput inline functions
//----------------------------------------------------------------------
inline auto FOutput::getClassName() const -> FString
{ return "FOutput"; }

//----------------------------------------------------------------------
inline auto FOutput::getFVTerm() const & -> const FVTerm&
{ return fvterm; }

//----------------------------------------------------------------------
template <typename ClassT>
inline void FOutput::setColorPaletteTheme() const
{
  const auto& set_Palette = getFSetPaletteRef();
  setColorPaletteTheme<ClassT>(set_Palette);
}

//----------------------------------------------------------------------
template <typename ClassT>
inline void FOutput::setColorPaletteTheme (const FSetPalette& f) const
{
  // Set instance
  FColorPalette::getInstance() = std::make_shared<ClassT>(f);
  // Set palette
  FColorPalette::getInstance()->setColorPalette();
}

}  // namespace finalcut

#endif  // FOUTPUT_H

