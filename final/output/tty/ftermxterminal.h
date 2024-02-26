/***********************************************************************
* ftermxterminal.h - Contains all xterm-specific terminal functions    *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2024 Markus Gans                                      *
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
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FTermXTerminal ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FTERMXTERMINAL_H
#define FTERMXTERMINAL_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/util/fstring.h"

namespace finalcut
{

// class forward declaration
class FSize;

//----------------------------------------------------------------------
// class FTermXTerminal
//----------------------------------------------------------------------

class FTermXTerminal final
{
  public:
    // Constructor
    FTermXTerminal();

    // Mutators
    void  redefineDefaultColors (bool = true) noexcept;
    void  setCursorStyle (XTermCursorStyle);
    void  setFont (const FString&);
    void  setTitle (const FString&);
    void  setTermSize (const FSize&);
    void  setForeground (const FString&);
    void  setBackground (const FString&);
    void  setCursorColor (const FString&);
    void  setMouseForeground (const FString&);
    void  setMouseBackground (const FString&);
    void  setHighlightBackground (const FString&);
    void  setMouseSupport (bool = true);
    void  unsetMouseSupport();
    void  setFocusSupport (bool enable = true);
    void  unsetFocusSupport();
    void  metaSendsESC (bool = true);

    // Accessors
    auto  getClassName() const -> FString;
    static auto  getInstance() -> FTermXTerminal&;
    auto  getCursorStyle() const noexcept -> XTermCursorStyle;
    auto  getFont() const -> FString;
    auto  getTitle() const -> FString;
    auto  getForeground() const -> FString;
    auto  getBackground() const -> FString;
    auto  getCursorColor() const -> FString;
    auto  getMouseForeground() const -> FString;
    auto  getMouseBackground() const -> FString;
    auto  getHighlightBackground() const -> FString;

    // Inquiries
    auto  hasFont() const -> bool;
    auto  hasTitle() const -> bool;

    // Methods
    void  setDefaults();
    void  resetColorMap() const;
    void  resetForeground();
    void  resetBackground();
    void  resetCursorColor();
    void  resetMouseForeground();
    void  resetMouseBackground();
    void  resetHighlightBackground();
    void  resetDefaults();
    void  resetTitle();
    void  captureFontAndTitle();

  private:
    // Methods
    void  warnNotInitialized() const;
    auto  canResetDefaults() const -> bool;
    auto  canCaptureFontAndTitle() const -> bool;
    auto  canSetXTermCursorStyle() const -> bool;
    void  setXTermCursorStyle();
    auto  canSetXTermTitle() const -> bool;
    void  setXTermTitle();
    auto  canSetXTermSize() const -> bool;
    void  setXTermSize() const;
    auto  canSetXTermFont() const -> bool;
    void  setXTermFont();
    auto  canSetXTermColor() const -> bool;
    auto  canSetXTermForeground() const -> bool;
    void  setXTermForeground();
    auto  canSetXTermBackground() const -> bool;
    void  setXTermBackground();
    auto  canSetXTermCursorColor() const -> bool;
    void  setXTermCursorColor();
    auto  canSetXTermMouseForeground() const -> bool;
    void  setXTermMouseForeground();
    auto  canSetXTermMouseBackground() const -> bool;
    void  setXTermMouseBackground();
    auto  canSetXTermHighlightBackground() const -> bool;
    void  setXTermHighlightBackground();
    auto  canSetXTermColors() const -> bool;
    void  setXTerm8ColorDefaults();
    void  setXTerm16ColorDefaults();
    auto  canSetXTermDefaultsMouseCursor() const -> bool;
    void  setXTermDefaultsMouseCursor();
    auto  needsFullReset() const -> bool;
    auto  canOnlyResetColorSeparately() const -> bool;
    auto  canResetColor() const -> bool;
    void  resetXTermColorMap() const;
    void  resetXTermForeground() const;
    void  resetXTermBackground() const;
    void  resetXTermCursorColor() const;
    void  resetXTermMouseForeground() const;
    void  resetXTermMouseBackground() const;
    void  resetXTermHighlightBackground() const;
    void  oscPrefix() const;
    void  oscPostfix() const;
    auto  canCaptureXTermFont() const -> bool;
    auto  captureXTermFont() const -> FString;
    auto  canCaptureXTermTitle() const -> bool;
    auto  captureXTermTitle() const -> FString;
    void  enableXTermMouse();
    void  disableXTermMouse();
    void  enableXTermFocus();
    void  disableXTermFocus();
    auto  canUseXTermMetaSendsESC() const -> bool;
    void  enableXTermMetaSendsESC();
    void  disableXTermMetaSendsESC();

    // Data members
    bool              mouse_support{false};
    bool              focus_support{false};
    bool              meta_sends_esc{false};
    bool              xterm_default_colors{false};
    bool              title_was_changed{false};
    std::size_t       term_width{80};
    std::size_t       term_height{24};
    FString           xterm_font{};
    FString           xterm_title{};
    FString           foreground_color{};
    FString           background_color{};
    FString           cursor_color{};
    FString           mouse_foreground_color{};
    FString           mouse_background_color{};
    FString           highlight_background_color{};
    XTermCursorStyle  cursor_style{XTermCursorStyle::UnknownCursorStyle};
};


// FTermXTerminal inline functions
//----------------------------------------------------------------------
inline auto FTermXTerminal::getClassName() const -> FString
{ return "FTermXTerminal"; }

//----------------------------------------------------------------------
inline void FTermXTerminal::redefineDefaultColors (bool enable) noexcept
{ xterm_default_colors = enable; }

//----------------------------------------------------------------------
inline auto FTermXTerminal::getCursorStyle() const noexcept -> XTermCursorStyle
{ return cursor_style; }

//----------------------------------------------------------------------
inline auto FTermXTerminal::getFont() const -> FString
{ return xterm_font; }

//----------------------------------------------------------------------
inline auto FTermXTerminal::getTitle() const -> FString
{ return xterm_title; }

//----------------------------------------------------------------------
inline auto FTermXTerminal::getForeground() const -> FString
{ return foreground_color; }

//----------------------------------------------------------------------
inline auto FTermXTerminal::getBackground() const -> FString
{ return background_color; }

//----------------------------------------------------------------------
inline auto FTermXTerminal::getCursorColor() const -> FString
{ return cursor_color; }

//----------------------------------------------------------------------
inline auto FTermXTerminal::getMouseForeground() const -> FString
{ return mouse_foreground_color; }

//----------------------------------------------------------------------
inline auto FTermXTerminal::getMouseBackground() const -> FString
{ return mouse_background_color; }

//----------------------------------------------------------------------
inline auto FTermXTerminal::getHighlightBackground() const -> FString
{ return highlight_background_color; }

//----------------------------------------------------------------------
inline auto FTermXTerminal::hasFont() const -> bool
{ return xterm_font.getLength() > 2; }

//----------------------------------------------------------------------
inline auto FTermXTerminal::hasTitle() const -> bool
{ return xterm_title.getLength() > 0; }

//----------------------------------------------------------------------
inline void FTermXTerminal::unsetMouseSupport()
{ setMouseSupport (false); }

//----------------------------------------------------------------------
inline void FTermXTerminal::unsetFocusSupport()
{ setFocusSupport (false); }

}  // namespace finalcut

#endif  // FTERMXTERMINAL_H
