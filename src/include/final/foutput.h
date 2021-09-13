/***********************************************************************
* foutput.h - Abstract Virtual terminal output class                   *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2021 Markus Gans                                           *
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
#include "final/fcolorpalette.h"
#include "final/ftypes.h"
#include "final/fvterm.h"

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
    virtual FString     getClassName() const;
    auto                getInstance() -> std::shared_ptr<FOutput>;
    const FVTerm&       getFVTerm() const;
    virtual std::size_t getColumnNumber() const = 0;
    virtual std::size_t getLineNumber() const = 0;
    virtual int         getTabstop() const = 0;
    virtual int         getMaxColor() const = 0;
    virtual Encoding    getEncoding() const = 0;
    virtual FString     getKeyName (FKey) const = 0;

    // Mutators
    virtual void        setCursor (FPoint) = 0;
    virtual void        setCursor (CursorMode) = 0;
    virtual void        hideCursor (bool = true) = 0;
    virtual void        showCursor() = 0;
    virtual void        setTerminalSize (FSize) = 0;
    virtual bool        setVGAFont() = 0;
    virtual bool        setNewFont() = 0;
    virtual void        setNonBlockingRead (bool = true) = 0;
    template <typename ClassT>
    void                setColorPaletteTheme() const;
    template <typename ClassT>
    void                setColorPaletteTheme (const FSetPalette&) const;

    // Inquiries
    virtual bool        isCursorHideable() const = 0;
    virtual bool        isMonochron() const = 0;
    virtual bool        isNewFont() const = 0;
    virtual bool        isEncodable (wchar_t) const = 0;
    virtual bool        hasTerminalResized() const = 0;
    virtual bool        allowsTerminalSizeManipulation() const = 0;
    virtual bool        canChangeColorPalette() const = 0;
    virtual bool        hasHalfBlockCharacter() const = 0;
    virtual bool        hasShadowCharacter() const = 0;
    virtual bool        areMetaAndArrowKeysSupported() const = 0;

    // Methods
    virtual void        initTerminal (FVTerm::FTermArea*) = 0;
    virtual void        finishTerminal() = 0;
    virtual bool        updateTerminal() = 0;
    virtual void        detectTerminalSize() = 0;
    virtual void        commitTerminalResize() = 0;
    virtual void        initScreenSettings() = 0;
    virtual bool        scrollTerminalForward() = 0;
    virtual bool        scrollTerminalReverse() = 0;
    virtual bool        clearTerm (wchar_t = L' ') = 0;
    virtual void        flush() = 0;
    virtual void        beep() const = 0;

  private:
    // Accessors
    virtual const FSetPalette&
                        getFSetPaletteRef() const = 0;

    // Methods
    virtual bool        isDefaultPaletteTheme() = 0;
    virtual void        redefineColorPalette() = 0;
    virtual void        restoreColorPalette() = 0;

    // Data members
    const FVTerm& fvterm{};
};

// FOutput inline functions
//----------------------------------------------------------------------
//----------------------------------------------------------------------
inline FString FOutput::getClassName() const
{ return "FOutput"; }

//----------------------------------------------------------------------
inline const FVTerm& FOutput::getFVTerm() const
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

