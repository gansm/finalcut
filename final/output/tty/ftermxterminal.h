/***********************************************************************
* ftermxterminal.h - Contains all xterm-specific terminal functions    *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2021 Markus Gans                                      *
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
    void                  redefineDefaultColors (bool = true) noexcept;
    void                  setCursorStyle (XTermCursorStyle);
    void                  setFont (const FString&);
    void                  setTitle (const FString&);
    void                  setTermSize (const FSize&);
    void                  setForeground (const FString&);
    void                  setBackground (const FString&);
    void                  setCursorColor (const FString&);
    void                  setMouseForeground (const FString&);
    void                  setMouseBackground (const FString&);
    void                  setHighlightBackground (const FString&);
    static void           setMouseSupport (bool = true);
    static void           unsetMouseSupport();
    void                  metaSendsESC (bool = true);

    // Accessors
    FString               getClassName() const;
    static auto           getInstance() -> FTermXTerminal&;
    XTermCursorStyle      getCursorStyle() const noexcept;
    FString               getFont() const;
    FString               getTitle() const;
    FString               getForeground() const;
    FString               getBackground() const;
    FString               getCursorColor() const;
    FString               getMouseForeground() const;
    FString               getMouseBackground() const;
    FString               getHighlightBackground() const;

    // Inquiries
    bool                  hasFont() const;
    bool                  hasTitle() const;

    // Methods
    void                  setDefaults();
    void                  resetColorMap() const;
    void                  resetForeground();
    void                  resetBackground();
    void                  resetCursorColor();
    void                  resetMouseForeground();
    void                  resetMouseBackground();
    void                  resetHighlightBackground();
    void                  resetDefaults();
    void                  resetTitle();
    void                  captureFontAndTitle();

  private:
    // Methods
    void                  warnNotInitialized() const;
    void                  setXTermCursorStyle();
    void                  setXTermFont();
    void                  setXTermTitle();
    void                  setXTermSize() const;
    void                  setXTermForeground();
    void                  setXTermBackground();
    void                  setXTermCursorColor();
    void                  setXTermMouseForeground();
    void                  setXTermMouseBackground();
    void                  setXTermHighlightBackground();
    void                  setXTerm8ColorDefaults();
    void                  setXTerm16ColorDefaults();
    void                  setXTermDefaultsMouseCursor();
    bool                  canSetXTermBackground() const;
    void                  resetXTermColorMap() const;
    void                  resetXTermForeground() const;
    void                  resetXTermBackground() const;
    void                  resetXTermCursorColor() const;
    void                  resetXTermMouseForeground() const;
    void                  resetXTermMouseBackground() const;
    void                  resetXTermHighlightBackground() const;
    bool                  canResetColor() const;
    void                  oscPrefix() const;
    void                  oscPostfix() const;
    FString               captureXTermFont() const;
    FString               captureXTermTitle() const;
    static void           enableXTermMouse();
    static void           disableXTermMouse();
    void                  enableXTermMetaSendsESC();
    void                  disableXTermMetaSendsESC();

    // Data members
    static bool           mouse_support;
    bool                  meta_sends_esc{false};
    bool                  xterm_default_colors{false};
    bool                  title_was_changed{false};
    std::size_t           term_width{80};
    std::size_t           term_height{24};
    FString               xterm_font{};
    FString               xterm_title{};
    FString               foreground_color{};
    FString               background_color{};
    FString               cursor_color{};
    FString               mouse_foreground_color{};
    FString               mouse_background_color{};
    FString               highlight_background_color{};
    XTermCursorStyle      cursor_style{XTermCursorStyle::UnknownCursorStyle};
};


// FTermXTerminal inline functions
//----------------------------------------------------------------------
inline FString FTermXTerminal::getClassName() const
{ return "FTermXTerminal"; }

//----------------------------------------------------------------------
inline void FTermXTerminal::redefineDefaultColors (bool enable) noexcept
{ xterm_default_colors = enable; }

//----------------------------------------------------------------------
inline XTermCursorStyle FTermXTerminal::getCursorStyle() const noexcept
{ return cursor_style; }

//----------------------------------------------------------------------
inline FString FTermXTerminal::getFont() const
{ return xterm_font; }

//----------------------------------------------------------------------
inline FString FTermXTerminal::getTitle() const
{ return xterm_title; }

//----------------------------------------------------------------------
inline FString FTermXTerminal::getForeground() const
{ return foreground_color; }

//----------------------------------------------------------------------
inline FString FTermXTerminal::getBackground() const
{ return background_color; }

//----------------------------------------------------------------------
inline FString FTermXTerminal::getCursorColor() const
{ return cursor_color; }

//----------------------------------------------------------------------
inline FString FTermXTerminal::getMouseForeground() const
{ return mouse_foreground_color; }

//----------------------------------------------------------------------
inline FString FTermXTerminal::getMouseBackground() const
{ return mouse_background_color; }

//----------------------------------------------------------------------
inline FString FTermXTerminal::getHighlightBackground() const
{ return highlight_background_color; }

//----------------------------------------------------------------------
inline bool FTermXTerminal::hasFont() const
{ return xterm_font.getLength() > 2; }

//----------------------------------------------------------------------
inline bool FTermXTerminal::hasTitle() const
{ return xterm_title.getLength() > 0; }

//----------------------------------------------------------------------
inline void FTermXTerminal::unsetMouseSupport()
{ setMouseSupport (false); }

}  // namespace finalcut

#endif  // FTERMXTERMINAL_H
