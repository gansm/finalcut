/***********************************************************************
* ftermxterminal.cpp - Contains all xterm-specific terminal functions  *
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

#include "final/fterm.h"
#include "final/ftermfreebsd.h"
#include "final/ftermxterminal.h"

namespace finalcut
{

// static class attributes
bool                 FTermXTerminal::mouse_support;
bool                 FTermXTerminal::meta_sends_esc;
bool                 FTermXTerminal::xterm_default_colors;
std::size_t          FTermXTerminal::term_width = 80;
std::size_t          FTermXTerminal::term_height = 24;
const FString*       FTermXTerminal::xterm_font = 0;
const FString*       FTermXTerminal::xterm_title = 0;
const FString*       FTermXTerminal::foreground_color = 0;
const FString*       FTermXTerminal::background_color = 0;
const FString*       FTermXTerminal::cursor_color = 0;
const FString*       FTermXTerminal::mouse_foreground_color = 0;
const FString*       FTermXTerminal::mouse_background_color = 0;
const FString*       FTermXTerminal::highlight_background_color = 0;
FTermcap::tcap_map*  FTermXTerminal::tcap = 0;
FTermDetection*      FTermXTerminal::term_detection = 0;
fc::xtermCursorStyle FTermXTerminal::cursor_style = fc::unknown_cursor_style;


//----------------------------------------------------------------------
// class FTermXTerminal
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FTermXTerminal::FTermXTerminal()
{
  // Preset to false
  mouse_support        = \
  meta_sends_esc       = \
  xterm_default_colors = false;

  tcap = FTermcap::getTermcapMap();
}

//----------------------------------------------------------------------
FTermXTerminal::~FTermXTerminal()  // destructor
{
  if ( highlight_background_color )
    delete highlight_background_color;

  if ( mouse_background_color )
    delete mouse_background_color;

  if ( mouse_foreground_color )
    delete mouse_foreground_color;

  if ( cursor_color )
    delete cursor_color;

  if ( background_color )
    delete background_color;

  if ( foreground_color )
    delete foreground_color;

  if ( xterm_title )
    delete xterm_title;

  if ( xterm_font )
    delete xterm_font;
}


// public methods of FTermXTerminal
//----------------------------------------------------------------------
void FTermXTerminal::setCursorStyle (fc::xtermCursorStyle style)
{
  // Set the xterm cursor style

  cursor_style = style;
  setXTermCursorStyle();
}

//----------------------------------------------------------------------
void FTermXTerminal::setFont (const FString& fontname)
{
  // Change the XTerm font (needs the allowFontOps resource)

  if ( xterm_font )
    delete xterm_font;

  xterm_font = new FString(fontname);
  setXTermFont();
}

//----------------------------------------------------------------------
void FTermXTerminal::setTitle (const FString& title)
{
  // Set the xterm title

  if ( xterm_title )
    delete xterm_title;

  xterm_title = new FString(title);
  setXTermTitle();
}

//----------------------------------------------------------------------
void FTermXTerminal::setTermSize (std::size_t width, std::size_t height)
{
  // Set xterm size to {term_width} x {term_height}

  term_width = width;
  term_height = height;
  setXTermSize();
}

//----------------------------------------------------------------------
void FTermXTerminal::setForeground (const FString& fg)
{
  // Set the XTerm text foreground color

  if ( foreground_color )
    delete foreground_color;

  foreground_color = new FString(fg);
  setXTermForeground();
}

//----------------------------------------------------------------------
void FTermXTerminal::setBackground (const FString& bg)
{
  // Set the XTerm text background color

  if ( background_color )
    delete background_color;

  background_color = new FString(bg);
  setXTermBackground();
}

//----------------------------------------------------------------------
void FTermXTerminal::setCursorColor (const FString& cc)
{
  // Set the text cursor color

  if ( cursor_color )
    delete cursor_color;

  cursor_color = new FString(cc);
  setXTermCursorColor();
}

//----------------------------------------------------------------------
void FTermXTerminal::setMouseForeground (const FString& mfg)
{
  // Set the mouse foreground color

  if ( mouse_foreground_color )
    delete mouse_foreground_color;

  mouse_foreground_color = new FString(mfg);
  setXTermMouseForeground();
}

//----------------------------------------------------------------------
void FTermXTerminal::setMouseBackground (const FString& mbg)
{
  // Set the mouse background color

  if ( mouse_background_color )
    delete mouse_background_color;

  mouse_background_color = new FString(mbg);
  setXTermMouseBackground();
}

//----------------------------------------------------------------------
void FTermXTerminal::setHighlightBackground (const FString& hbg)
{
  // Set the highlight background color

  if ( highlight_background_color )
    delete highlight_background_color;

  highlight_background_color = new FString(hbg);
  setXTermHighlightBackground();
}

//----------------------------------------------------------------------
void FTermXTerminal::setMouseSupport (bool on)
{
  // activate/deactivate the xterm mouse support

  if ( on )
    enableXTermMouse();
  else
    disableXTermMouse();
}

//----------------------------------------------------------------------
void FTermXTerminal::metaSendsESC (bool on)
{
  // activate/deactivate the xterm meta key sends escape prefix

  if ( on )
    enableXTermMetaSendsESC();
  else
    disableXTermMetaSendsESC();
}

//----------------------------------------------------------------------
void FTermXTerminal::setDefaults()
{
  // Redefinition of the XTerm default colors
  // for the final cut color theme

  if ( FTerm::getMaxColor() >= 16 )
    setXTerm16ColorDefaults();
  else
    setXTerm8ColorDefaults();
}

//----------------------------------------------------------------------
void FTermXTerminal::resetColorMap()
{
  // Reset the entire color table

  resetXTermColorMap();
}

//----------------------------------------------------------------------
void FTermXTerminal::resetForeground()
{
  // Reset the XTerm text foreground color

  if ( foreground_color )
    delete foreground_color;

  foreground_color = 0;
  resetXTermForeground();
}

//----------------------------------------------------------------------
void FTermXTerminal::resetBackground()
{
  // Reset the XTerm text background color

  if ( background_color )
    delete background_color;

  background_color = 0;
  resetXTermBackground();
}

//----------------------------------------------------------------------
void FTermXTerminal::resetCursorColor()
{
  // Reset the text cursor color

  if ( cursor_color )
    delete cursor_color;

  cursor_color = 0;
  resetXTermCursorColor();
}

//----------------------------------------------------------------------
void FTermXTerminal::resetMouseForeground()
{
  // Reset the mouse foreground color

  if ( mouse_foreground_color )
    delete mouse_foreground_color;

  mouse_foreground_color = 0;
  resetXTermMouseForeground();
}

//----------------------------------------------------------------------
void FTermXTerminal::resetMouseBackground()
{
  // Reset the mouse background color

  if ( mouse_background_color )
    delete mouse_background_color;

  mouse_background_color = 0;
  resetXTermMouseBackground();
}

//----------------------------------------------------------------------
void FTermXTerminal::resetHighlightBackground()
{
  // Reset the highlight background color

  if ( highlight_background_color )
    delete highlight_background_color;

  highlight_background_color = 0;
  resetXTermHighlightBackground();
}

//----------------------------------------------------------------------
void FTermXTerminal::resetDefaults()
{
  if ( term_detection->isPuttyTerminal() )
    return;

  // Redefines the cursor color if resetCursorColor() doesn't work
  setCursorColor("rgb:b1b1/b1b1/b1b1");
  // Reset mouse color to default
  resetMouseForeground();
  resetMouseBackground();
  // Reset text cursor color to default
  resetCursorColor();

  if ( xterm_default_colors )
  {
    // Reset text foreground, text background, and mouse text highlight
    // colors to default values
    resetForeground();
    resetBackground();
    resetHighlightBackground();
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::captureFontAndTitle()
{
  if ( ( term_detection->isXTerminal()
    || term_detection->isUrxvtTerminal() )
    && ! term_detection->isRxvtTerminal() )
  {
    if ( xterm_font )
      delete xterm_font;

    if ( xterm_title )
      delete xterm_title;

    FTermios::setCaptureSendCharacters();
    xterm_font  = captureXTermFont();
    xterm_title = captureXTermTitle();
    FTermios::unsetCaptureSendCharacters();
  }
}


// private methods of FTermXTerminal
//----------------------------------------------------------------------
void FTermXTerminal::setXTermCursorStyle()
{
  // Set the xterm cursor style

#if defined(__FreeBSD__) || defined(__DragonFly__)
  if ( FTermFreeBSD::isFreeBSDConsole() )
    return;
#endif

  if ( term_detection->isGnomeTerminal()
    && ! term_detection->hasSetCursorStyleSupport() )
    return;

  if ( term_detection->isKdeTerminal() )
    return;

  if ( TCAP(fc::t_cursor_style)
    || term_detection->isXTerminal()
    || term_detection->isMinttyTerm()
    || term_detection->hasSetCursorStyleSupport() )
  {
    FTerm::putstringf (CSI "%d q", cursor_style);
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTermTitle()
{
  // Set the xterm title

  if ( term_detection->isXTerminal()
    || term_detection->isScreenTerm()
    || term_detection->isMinttyTerm()
    || term_detection->isPuttyTerminal()
    || FTermcap::osc_support )
  {
    oscPrefix();
    FTerm::putstringf (OSC "0;%s" BEL, xterm_title->c_str());
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTermSize()
{
  if ( term_detection->isXTerminal() )
  {
    FTerm::putstringf ( CSI "8;%lu;%lut"
                      , uLong(term_height)
                      , uLong(term_width) );
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTermFont()
{
  // Change the XTerm font (needs the allowFontOps resource)

  if ( term_detection->isXTerminal()
    || term_detection->isScreenTerm()
    || term_detection->isUrxvtTerminal()
    || FTermcap::osc_support )
  {
    oscPrefix();
    FTerm::putstringf (OSC "50;%s" BEL, xterm_font->c_str() );
    oscPostfix();
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTermForeground()
{
  // Set the XTerm text foreground color

  if ( term_detection->isXTerminal()
    || term_detection->isScreenTerm()
    || term_detection->isMinttyTerm()
    || term_detection->isMltermTerminal()
    || FTermcap::osc_support )
  {
    oscPrefix();
    FTerm::putstringf (OSC "10;%s" BEL, foreground_color->c_str());
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTermBackground()
{
  // Set the XTerm text background color

  if ( term_detection->isXTerminal()
    || term_detection->isScreenTerm()
    || term_detection->isMinttyTerm()
    || term_detection->isMltermTerminal()
    || FTermcap::osc_support )
  {
    oscPrefix();
    FTerm::putstringf (OSC "11;%s" BEL, background_color->c_str());
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTermCursorColor()
{
  // Set the text cursor color

  if ( term_detection->isXTerminal()
    || term_detection->isScreenTerm()
    || term_detection->isMinttyTerm()
    || term_detection->isUrxvtTerminal()
    || FTermcap::osc_support )
  {
    oscPrefix();
    FTerm::putstringf (OSC "12;%s" BEL, cursor_color->c_str());
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTermMouseForeground()
{
  // Set the mouse foreground color

  if ( term_detection->isXTerminal()
    || term_detection->isScreenTerm()
    || term_detection->isUrxvtTerminal()
    || FTermcap::osc_support )
  {
    oscPrefix();
    FTerm::putstringf (OSC "13;%s" BEL, mouse_foreground_color->c_str());
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTermMouseBackground()
{
  // Set the mouse background color

  if ( term_detection->isXTerminal()
    || term_detection->isScreenTerm()
    || FTermcap::osc_support )
  {
    oscPrefix();
    FTerm::putstringf (OSC "14;%s" BEL, mouse_background_color->c_str());
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTermHighlightBackground()
{
  // Set the highlight background color

  if ( term_detection->isXTerminal()
    || term_detection->isScreenTerm()
    || term_detection->isUrxvtTerminal()
    || FTermcap::osc_support )
  {
    oscPrefix();
    FTerm::putstringf (OSC "17;%s" BEL, highlight_background_color->c_str());
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTerm8ColorDefaults()
{
  // Redefinition of the XTerm default colors
  // for the final cut 8 color theme

  if ( term_detection->isPuttyTerminal() )
    return;

  setMouseBackground("rgb:ffff/ffff/ffff");        // white
  setMouseForeground ("rgb:0000/0000/0000");       // black

  if ( ! term_detection->isGnomeTerminal() )
    setCursorColor("rgb:ffff/ffff/ffff");          // white

  if ( xterm_default_colors
    && ! (term_detection->isMinttyTerm()
       || term_detection->isMltermTerminal()
       || term_detection->isRxvtTerminal()
       || term_detection->isScreenTerm()) )
  {
    // mintty and rxvt can't reset these settings
    setBackground("rgb:2222/2222/b2b2");           // blue
    setForeground("rgb:0000/0000/0000");           // black
    setHighlightBackground("rgb:8787/8787/8787");  // gray53
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTerm16ColorDefaults()
{
  // Redefinition of the XTerm default colors
  // for the final cut 16 color theme

  if ( term_detection->isPuttyTerminal() )
    return;

  setMouseBackground("rgb:ffff/ffff/ffff");        // white
  setMouseForeground ("rgb:0000/0000/0000");       // black

  if ( ! term_detection->isGnomeTerminal() )
    setCursorColor("rgb:ffff/ffff/ffff");          // white

  if ( xterm_default_colors
    && ! (term_detection->isMinttyTerm()
       || term_detection->isMltermTerminal()
       || term_detection->isRxvtTerminal()
       || term_detection->isScreenTerm()) )
  {
    // mintty and rxvt can't reset these settings
    setBackground("rgb:8080/a4a4/ecec");           // very light blue
    setForeground("rgb:0000/0000/0000");           // black
    setHighlightBackground("rgb:8787/8787/8787");  // gray53
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::resetXTermColorMap()
{
  // Reset the entire color table

  if ( term_detection->isGnomeTerminal()
    && term_detection->getGnomeTerminalID() < 3502 )
    return;

  if ( term_detection->isPuttyTerminal()
    || term_detection->isMltermTerminal() )
    return;

  if ( term_detection->isXTerminal()
    || term_detection->isScreenTerm()
    || FTermcap::osc_support )
  {
    oscPrefix();
    FTerm::putstringf (OSC "104" BEL);
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::resetXTermForeground()
{
  // Reset the XTerm text foreground color

  if ( term_detection->isGnomeTerminal()
    && term_detection->getGnomeTerminalID() < 3502 )
    return;

  if ( term_detection->isPuttyTerminal()
    || term_detection->isMltermTerminal() )
    return;

  if ( term_detection->isXTerminal()
    || term_detection->isScreenTerm()
    || FTermcap::osc_support )
  {
    oscPrefix();
    FTerm::putstring (OSC "110" BEL);
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::resetXTermBackground()
{
  // Reset the XTerm text background color

  if ( term_detection->isGnomeTerminal()
    && term_detection->getGnomeTerminalID() < 3502 )
    return;

  if ( term_detection->isPuttyTerminal()
    || term_detection->isMltermTerminal() )
    return;

  if ( term_detection->isXTerminal()
    || term_detection->isScreenTerm()
    || FTermcap::osc_support )
  {
    oscPrefix();
    FTerm::putstring (OSC "111" BEL);
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::resetXTermCursorColor()
{
  // Reset the text cursor color

  if ( term_detection->isGnomeTerminal()
    && term_detection->getGnomeTerminalID() < 3502 )
    return;

  if ( term_detection->isPuttyTerminal() )
    return;

  if ( term_detection->isXTerminal()
    || term_detection->isScreenTerm()
    || FTermcap::osc_support )
  {
    oscPrefix();
    FTerm::putstring (OSC "112" BEL);
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::resetXTermMouseForeground()
{
  // Reset the mouse foreground color

  if ( term_detection->isGnomeTerminal()
    && term_detection->getGnomeTerminalID() < 3502 )
    return;

  if ( term_detection->isPuttyTerminal() )
    return;

  if ( term_detection->isXTerminal()
    || term_detection->isScreenTerm()
    || FTermcap::osc_support )
  {
    oscPrefix();
    FTerm::putstring (OSC "113" BEL);
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::resetXTermMouseBackground()
{
  // Reset the mouse background color

  if ( term_detection->isGnomeTerminal()
    && term_detection->getGnomeTerminalID() < 3502 )
    return;

  if ( term_detection->isPuttyTerminal() )
    return;

  if ( term_detection->isXTerminal()
    || term_detection->isScreenTerm()
    || FTermcap::osc_support )
  {
    oscPrefix();
    FTerm::putstring (OSC "114" BEL);
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::resetXTermHighlightBackground()
{
  // Reset the highlight background color

  if ( term_detection->isGnomeTerminal()
    && term_detection->getGnomeTerminalID() < 3502 )
    return;

  if ( term_detection->isPuttyTerminal() )
    return;

  if ( term_detection->isXTerminal()
    || term_detection->isScreenTerm()
    || term_detection->isUrxvtTerminal()
    || FTermcap::osc_support )
  {
    oscPrefix();
    FTerm::putstringf (OSC "117" BEL);
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::oscPrefix()
{
  if ( term_detection->isTmuxTerm() )
  {
    // tmux device control string
    FTerm::putstring (ESC "Ptmux;" ESC);
  }
  else if ( term_detection->isScreenTerm() )
  {
    // GNU Screen device control string
    FTerm::putstring (ESC "P");
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::oscPostfix()
{
  if ( term_detection->isScreenTerm()
    || term_detection->isTmuxTerm() )
  {
    // GNU Screen/tmux string terminator
    FTerm::putstring (ESC "\\");
  }
}

//----------------------------------------------------------------------
const FString* FTermXTerminal::captureXTermFont()
{
  if ( term_detection->isXTerminal()
    || term_detection->isScreenTerm()
    || FTermcap::osc_support )
  {
    fd_set ifds;
    struct timeval tv;
    int stdin_no = FTermios::getStdIn();

    oscPrefix();
    FTerm::putstring (OSC "50;?" BEL);  // get font
    oscPostfix();
    std::fflush(stdout);

    FD_ZERO(&ifds);
    FD_SET(stdin_no, &ifds);
    tv.tv_sec  = 0;
    tv.tv_usec = 150000;  // 150 ms

    // Read the terminal answer
    if ( select(stdin_no + 1, &ifds, 0, 0, &tv) > 0 )
    {
      char temp[150] = { };

      if ( std::scanf("\033]50;%148[^\n]s", temp) == 1 )
      {
        FString* xtermfont;
        std::size_t n = std::strlen(temp);

        // BEL + '\0' = string terminator
        if ( n >= 5 && temp[n - 1] == BEL[0] && temp[n] == '\0' )
          temp[n - 1] = '\0';

        try
        {
          xtermfont = new FString(temp);
        }
        catch (const std::bad_alloc& ex)
        {
          std::cerr << bad_alloc_str << ex.what() << std::endl;
          return 0;
        }

        return xtermfont;
      }
    }
  }

  return 0;
}

//----------------------------------------------------------------------
const FString* FTermXTerminal::captureXTermTitle()
{
  if ( term_detection->isKdeTerminal() )
    return 0;

  fd_set ifds;
  struct timeval tv;
  int stdin_no = FTermios::getStdIn();

  FTerm::putstring (CSI "21t");  // get title
  std::fflush(stdout);

  FD_ZERO(&ifds);
  FD_SET(stdin_no, &ifds);
  tv.tv_sec  = 0;
  tv.tv_usec = 150000;  // 150 ms

  // read the terminal answer
  if ( select (stdin_no + 1, &ifds, 0, 0, &tv) > 0 )
  {
    char temp[512] = { };

    if ( std::scanf("\033]l%509[^\n]s", temp) == 1 )
    {
      std::size_t n = std::strlen(temp);

      // Esc + \ = OSC string terminator
      if ( n >= 2 && temp[n - 2] == ESC[0] && temp[n - 1] == '\\' )
      {
        FString* xtermtitle;

        if ( n < 4 )
          return 0;

        temp[n - 2] = '\0';

        try
        {
          xtermtitle = new FString(temp);
        }
        catch (const std::bad_alloc& ex)
        {
          std::cerr << bad_alloc_str << ex.what() << std::endl;
          return 0;
        }

        return xtermtitle;
      }
    }
  }

  return 0;
}

//----------------------------------------------------------------------
void FTermXTerminal::enableXTermMouse()
{
  // Activate the xterm mouse support

  if ( mouse_support )
    return;

  FTerm::putstring (CSI "?1001s"    // save old highlight mouse tracking
                    CSI "?1000h"    // enable x11 mouse tracking
                    CSI "?1002h"    // enable cell motion mouse tracking
                    CSI "?1015h"    // enable urxvt mouse mode
                    CSI "?1006h");  // enable SGR mouse mode
  std::fflush(stdout);
  mouse_support = true;
}

//----------------------------------------------------------------------
void FTermXTerminal::disableXTermMouse()
{
  // Deactivate the xterm mouse support

  if ( ! mouse_support )
    return;

  FTerm::putstring (CSI "?1006l"    // disable SGR mouse mode
                    CSI "?1015l"    // disable urxvt mouse mode
                    CSI "?1002l"    // disable cell motion mouse tracking
                    CSI "?1000l"    // disable x11 mouse tracking
                    CSI "?1001r");  // restore old highlight mouse tracking
  std::fflush(stdout);
  mouse_support = false;
}

//----------------------------------------------------------------------
void FTermXTerminal::enableXTermMetaSendsESC()
{
  // Activate the xterm meta key sends escape prefix

  if ( meta_sends_esc )
    return;

  FTerm::putstring (CSI "?1036s"    // save meta key sends escape
                    CSI "?1036h");  // enable meta key sends escape
  std::fflush(stdout);
  meta_sends_esc = true;
}

//----------------------------------------------------------------------
void FTermXTerminal::disableXTermMetaSendsESC()
{
  // Deactivate the xterm meta key sends escape prefix

  if ( ! meta_sends_esc )
    return;

  FTerm::putstring (CSI "?1036r");  // restore meta key sends escape
  std::fflush(stdout);
  meta_sends_esc = false;
}

}  // namespace finalcut
