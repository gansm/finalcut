/***********************************************************************
* ftermxterminal.h - Contains all xterm-specific terminal functions    *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2018-2019 Markus Gans                                      *
*                                                                      *
* The Final Cut is free software; you can redistribute it and/or       *
* modify it under the terms of the GNU Lesser General Public License   *
* as published by the Free Software Foundation; either version 3 of    *
* the License, or (at your option) any later version.                  *
*                                                                      *
* The Final Cut is distributed in the hope that it will be useful,     *
* but WITHOUT ANY WARRANTY; without even the implied warranty of       *
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

namespace finalcut
{

// class forward declaration
class FString;
class FSystem;
class FTermDetection;

//----------------------------------------------------------------------
// class FTermXTerminal
//----------------------------------------------------------------------

class FTermXTerminal final
{
  public:
    // Constructors
    FTermXTerminal();

    // Destructor
    virtual ~FTermXTerminal();

    // Mutators
    void                  redefineDefaultColors (bool);
    void                  setCursorStyle (fc::xtermCursorStyle);
    void                  setFont (const FString&);
    void                  setTitle (const FString&);
    void                  setTermSize (const FSize&);
    void                  setForeground (const FString&);
    void                  setBackground (const FString&);
    void                  setCursorColor (const FString&);
    void                  setMouseForeground (const FString&);
    void                  setMouseBackground (const FString&);
    void                  setHighlightBackground (const FString&);
    static void           setMouseSupport (bool);
    static void           setMouseSupport();
    static void           unsetMouseSupport();
    void                  metaSendsESC (bool);

    // Accessors
    const FString         getClassName() const;
    fc::xtermCursorStyle  getCursorStyle();
    const FString         getFont();
    const FString         getTitle();
    const FString         getForeground();
    const FString         getBackground();
    const FString         getCursorColor();
    const FString         getMouseForeground();
    const FString         getMouseBackground();
    const FString         getHighlightBackground();

    // Inquiries
    bool                  hasFont();
    bool                  hasTitle();

    // Methods
    void                  init();
    void                  setDefaults();
    void                  resetColorMap();
    void                  resetForeground();
    void                  resetBackground();
    void                  resetCursorColor();
    void                  resetMouseForeground();
    void                  resetMouseBackground();
    void                  resetHighlightBackground();
    void                  resetDefaults();
    void                  captureFontAndTitle();

  private:
    // Methods
    void                  setXTermCursorStyle();
    void                  setXTermFont();
    void                  setXTermTitle();
    void                  setXTermSize();
    void                  setXTermForeground();
    void                  setXTermBackground();
    void                  setXTermCursorColor();
    void                  setXTermMouseForeground();
    void                  setXTermMouseBackground();
    void                  setXTermHighlightBackground();
    void                  setXTerm8ColorDefaults();
    void                  setXTerm16ColorDefaults();
    void                  setXTermDefaultsMouseCursor();
    bool                  canSetXTermBackground();
    void                  resetXTermColorMap();
    void                  resetXTermForeground();
    void                  resetXTermBackground();
    void                  resetXTermCursorColor();
    void                  resetXTermMouseForeground();
    void                  resetXTermMouseBackground();
    void                  resetXTermHighlightBackground();
    bool                  canResetColor();
    void                  oscPrefix();
    void                  oscPostfix();
    const FString         captureXTermFont();
    const FString         captureXTermTitle();
    static void           enableXTermMouse();
    static void           disableXTermMouse();
    void                  enableXTermMetaSendsESC();
    void                  disableXTermMetaSendsESC();

    // Data members
    static bool           mouse_support;
    bool                  meta_sends_esc{false};
    bool                  xterm_default_colors{false};
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
    static FSystem*       fsystem;
    FTermDetection*       term_detection{nullptr};
    fc::xtermCursorStyle  cursor_style{fc::unknown_cursor_style};
};


// FTermXTerminal inline functions
//----------------------------------------------------------------------
inline const FString FTermXTerminal::getClassName() const
{ return "FTermXTerminal"; }

//----------------------------------------------------------------------
inline void FTermXTerminal::redefineDefaultColors (bool enable)
{ xterm_default_colors = enable; }

//----------------------------------------------------------------------
inline fc::xtermCursorStyle FTermXTerminal::getCursorStyle()
{ return cursor_style; }

//----------------------------------------------------------------------
inline const FString FTermXTerminal::getFont()
{ return xterm_font; }

//----------------------------------------------------------------------
inline const FString FTermXTerminal::getTitle()
{ return xterm_title; }

//----------------------------------------------------------------------
inline const FString FTermXTerminal::getForeground()
{ return foreground_color; }

//----------------------------------------------------------------------
inline const FString FTermXTerminal::getBackground()
{ return background_color; }

//----------------------------------------------------------------------
inline const FString FTermXTerminal::getCursorColor()
{ return cursor_color; }

//----------------------------------------------------------------------
inline const FString FTermXTerminal::getMouseForeground()
{ return mouse_foreground_color; }

//----------------------------------------------------------------------
inline const FString FTermXTerminal::getMouseBackground()
{ return mouse_background_color; }

//----------------------------------------------------------------------
inline const FString FTermXTerminal::getHighlightBackground()
{ return highlight_background_color; }

//----------------------------------------------------------------------
inline bool FTermXTerminal::hasFont()
{ return bool(xterm_font.getLength() > 2); }

//----------------------------------------------------------------------
inline bool FTermXTerminal::hasTitle()
{ return bool(xterm_title.getLength() > 0); }

//----------------------------------------------------------------------
inline void FTermXTerminal::setMouseSupport()
{ setMouseSupport (true); }

//----------------------------------------------------------------------
inline void FTermXTerminal::unsetMouseSupport()
{ setMouseSupport (false); }

}  // namespace finalcut

#endif  // FTERMXTERMINAL_H
