/***********************************************************************
* ftermxterminal.h - Contains all xterm-specific terminal functions    *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2020 Markus Gans                                      *
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

    // Disable copy constructor
    FTermXTerminal (const FTermXTerminal&) = delete;

    // Destructor
    ~FTermXTerminal();

    // Disable copy assignment operator (=)
    FTermXTerminal& operator = (const FTermXTerminal&) = delete;

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
    fc::xtermCursorStyle  getCursorStyle() const;
    const FString         getFont() const;
    const FString         getTitle() const;
    const FString         getForeground() const;
    const FString         getBackground() const;
    const FString         getCursorColor() const;
    const FString         getMouseForeground() const;
    const FString         getMouseBackground() const;
    const FString         getHighlightBackground() const;

    // Inquiries
    bool                  hasFont() const;
    bool                  hasTitle() const;

    // Methods
    void                  init();
    void                  setDefaults();
    void                  resetColorMap() const;
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
    bool                  isInitialized() const;
    bool                  canResetColor() const;
    void                  oscPrefix() const;
    void                  oscPostfix() const;
    const FString         captureXTermFont() const;
    const FString         captureXTermTitle() const;
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
inline fc::xtermCursorStyle FTermXTerminal::getCursorStyle() const
{ return cursor_style; }

//----------------------------------------------------------------------
inline const FString FTermXTerminal::getFont() const
{ return xterm_font; }

//----------------------------------------------------------------------
inline const FString FTermXTerminal::getTitle() const
{ return xterm_title; }

//----------------------------------------------------------------------
inline const FString FTermXTerminal::getForeground() const
{ return foreground_color; }

//----------------------------------------------------------------------
inline const FString FTermXTerminal::getBackground() const
{ return background_color; }

//----------------------------------------------------------------------
inline const FString FTermXTerminal::getCursorColor() const
{ return cursor_color; }

//----------------------------------------------------------------------
inline const FString FTermXTerminal::getMouseForeground() const
{ return mouse_foreground_color; }

//----------------------------------------------------------------------
inline const FString FTermXTerminal::getMouseBackground() const
{ return mouse_background_color; }

//----------------------------------------------------------------------
inline const FString FTermXTerminal::getHighlightBackground() const
{ return highlight_background_color; }

//----------------------------------------------------------------------
inline bool FTermXTerminal::hasFont() const
{ return bool(xterm_font.getLength() > 2); }

//----------------------------------------------------------------------
inline bool FTermXTerminal::hasTitle() const
{ return bool(xterm_title.getLength() > 0); }

//----------------------------------------------------------------------
inline void FTermXTerminal::setMouseSupport()
{ setMouseSupport (true); }

//----------------------------------------------------------------------
inline void FTermXTerminal::unsetMouseSupport()
{ setMouseSupport (false); }

//----------------------------------------------------------------------
inline bool FTermXTerminal::isInitialized() const
{ return bool(fsystem && term_detection); }

}  // namespace finalcut

#endif  // FTERMXTERMINAL_H
