/***********************************************************************
* ftermxterminal.h - Contains all xterm-specific terminal functions    *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2018 Markus Gans                                           *
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

#include "final/fc.h"
#include "final/fstring.h"
#include "final/ftermcap.h"
#include "final/ftermdetection.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FTermXTerminal
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FTermXTerminal
{
  public:
    // Constructors
    FTermXTerminal();

    // Destructor
    virtual ~FTermXTerminal();

    // Mutators
    static void           setFTermDetection (FTermDetection*);
    static void           redefineDefaultColors (bool);
    static void           setCursorStyle (fc::xtermCursorStyle);
    static void           setFont (const FString&);
    static void           setTitle (const FString&);
    static void           setTermSize (std::size_t, std::size_t);
    static void           setForeground (const FString&);
    static void           setBackground (const FString&);
    static void           setCursorColor (const FString&);
    static void           setMouseForeground (const FString&);
    static void           setMouseBackground (const FString&);
    static void           setHighlightBackground (const FString&);
    static void           setMouseSupport (bool);
    static void           setMouseSupport();
    static void           unsetMouseSupport();
    static void           metaSendsESC (bool);

    // Accessors
    const char*           getClassName() const;
    static fc::xtermCursorStyle getCursorStyle();
    static const FString* getFont();
    static const FString* getTitle();
    static const FString* getForeground();
    static const FString* getBackground();
    static const FString* getCursorColor();
    static const FString* getMouseForeground();
    static const FString* getMouseBackground();
    static const FString* getHighlightBackground();

    // Inquiries
    static bool           hasFont();
    static bool           hasTitle();

    // Methods
    static void           setDefaults();
    static void           resetColorMap();
    static void           resetForeground();
    static void           resetBackground();
    static void           resetCursorColor();
    static void           resetMouseForeground();
    static void           resetMouseBackground();
    static void           resetHighlightBackground();
    static void           resetDefaults();
    static void           captureFontAndTitle();

  private:
    // Methods
    static void           setXTermCursorStyle();
    static void           setXTermFont();
    static void           setXTermTitle();
    static void           setXTermSize();
    static void           setXTermForeground();
    static void           setXTermBackground();
    static void           setXTermCursorColor();
    static void           setXTermMouseForeground();
    static void           setXTermMouseBackground();
    static void           setXTermHighlightBackground();
    static void           setXTerm8ColorDefaults();
    static void           setXTerm16ColorDefaults();
    static void           resetXTermColorMap();
    static void           resetXTermForeground();
    static void           resetXTermBackground();
    static void           resetXTermCursorColor();
    static void           resetXTermMouseForeground();
    static void           resetXTermMouseBackground();
    static void           resetXTermHighlightBackground();
    static void           oscPrefix();
    static void           oscPostfix();
    static const FString* captureXTermFont();
    static const FString* captureXTermTitle();
    static void           enableXTermMouse();
    static void           disableXTermMouse();
    static void           enableXTermMetaSendsESC();
    static void           disableXTermMetaSendsESC();

    // Data Members
    static bool                 mouse_support;
    static bool                 meta_sends_esc;
    static bool                 xterm_default_colors;
    static std::size_t          term_width;
    static std::size_t          term_height;
    static const FString*       xterm_font;
    static const FString*       xterm_title;
    static const FString*       foreground_color;
    static const FString*       background_color;
    static const FString*       cursor_color;
    static const FString*       mouse_foreground_color;
    static const FString*       mouse_background_color;
    static const FString*       highlight_background_color;
    static FTermcap::tcap_map*  tcap;
    static FTermDetection*      term_detection;
    static fc::xtermCursorStyle cursor_style;
};
#pragma pack(pop)

// FTermXTerminal inline functions
//----------------------------------------------------------------------
inline const char* FTermXTerminal::getClassName() const
{ return "FTermXTerminal"; }

//----------------------------------------------------------------------
inline void FTermXTerminal::setFTermDetection (FTermDetection* td)
{ term_detection = td; }

//----------------------------------------------------------------------
inline void FTermXTerminal::redefineDefaultColors (bool on)
{ xterm_default_colors = on; }

//----------------------------------------------------------------------
inline fc::xtermCursorStyle FTermXTerminal::getCursorStyle()
{ return cursor_style; }

//----------------------------------------------------------------------
inline const FString* FTermXTerminal::getFont()
{ return xterm_font; }

//----------------------------------------------------------------------
inline const FString* FTermXTerminal::getTitle()
{ return xterm_title; }

//----------------------------------------------------------------------
inline const FString* FTermXTerminal::getForeground()
{ return foreground_color; }

//----------------------------------------------------------------------
inline const FString* FTermXTerminal::getBackground()
{ return background_color; }

//----------------------------------------------------------------------
inline const FString* FTermXTerminal::getCursorColor()
{ return cursor_color; }

//----------------------------------------------------------------------
inline const FString* FTermXTerminal::getMouseForeground()
{ return mouse_foreground_color; }

//----------------------------------------------------------------------
inline const FString* FTermXTerminal::getMouseBackground()
{ return mouse_background_color; }

//----------------------------------------------------------------------
inline const FString* FTermXTerminal::getHighlightBackground()
{ return highlight_background_color; }

//----------------------------------------------------------------------
inline bool FTermXTerminal::hasFont()
{ return bool(xterm_font && xterm_font->getLength() > 2); }

//----------------------------------------------------------------------
inline bool FTermXTerminal::hasTitle()
{ return bool(xterm_title && xterm_title->getLength() > 0); }

//----------------------------------------------------------------------
inline void FTermXTerminal::setMouseSupport()
{ setMouseSupport (true); }

//----------------------------------------------------------------------
inline void FTermXTerminal::unsetMouseSupport()
{ setMouseSupport (false); }

}  // namespace finalcut

#endif  // FTERMXTERMINAL_H
