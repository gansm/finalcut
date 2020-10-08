/***********************************************************************
* ftermxterminal.cpp - Contains all xterm-specific terminal functions  *
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

#if defined(__CYGWIN__)
  #include "final/fconfig.h"  // includes _GNU_SOURCE for fd_set
#endif

#include "final/fapplication.h"
#include "final/fc.h"
#include "final/flog.h"
#include "final/fkeyboard.h"
#include "final/fstring.h"
#include "final/fterm.h"
#include "final/ftermcap.h"
#include "final/ftermdetection.h"
#include "final/ftermfreebsd.h"
#include "final/ftermios.h"
#include "final/ftermxterminal.h"
#include "final/fsize.h"

#define initCheck(ret_value)           \
    if ( ! isInitialized() )           \
    {                                  \
      if ( ! FApplication::isQuit() )  \
        warnNotInitialized();          \
                                       \
      return ret_value;                \
    }

namespace finalcut
{

// static class attributes
bool       FTermXTerminal::mouse_support{false};
FSystem*   FTermXTerminal::fsystem{nullptr};
FKeyboard* FTermXTerminal::keyboard{nullptr};


//----------------------------------------------------------------------
// class FTermXTerminal
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FTermXTerminal::FTermXTerminal()
{
  // Get FSystem object
  fsystem = FTerm::getFSystem();
  keyboard = FTerm::getFKeyboard();
}

//----------------------------------------------------------------------
FTermXTerminal::~FTermXTerminal()  // destructor
{ }


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

  xterm_font = fontname;
  setXTermFont();
}

//----------------------------------------------------------------------
void FTermXTerminal::setTitle (const FString& title)
{
  // Set the xterm title

  xterm_title = title;
  setXTermTitle();
}

//----------------------------------------------------------------------
void FTermXTerminal::setTermSize (const FSize& size)
{
  // Set xterm size to {term_width} x {term_height}

  term_width = size.getWidth();
  term_height = size.getHeight();
  setXTermSize();
}

//----------------------------------------------------------------------
void FTermXTerminal::setForeground (const FString& fg)
{
  // Set the XTerm text foreground color

  foreground_color = fg;
  setXTermForeground();
}

//----------------------------------------------------------------------
void FTermXTerminal::setBackground (const FString& bg)
{
  // Set the XTerm text background color

  background_color = bg;
  setXTermBackground();
}

//----------------------------------------------------------------------
void FTermXTerminal::setCursorColor (const FString& cc)
{
  // Set the text cursor color

  cursor_color = cc;
  setXTermCursorColor();
}

//----------------------------------------------------------------------
void FTermXTerminal::setMouseForeground (const FString& mfg)
{
  // Set the mouse foreground color

  mouse_foreground_color = mfg;
  setXTermMouseForeground();
}

//----------------------------------------------------------------------
void FTermXTerminal::setMouseBackground (const FString& mbg)
{
  // Set the mouse background color

  mouse_background_color = mbg;
  setXTermMouseBackground();
}

//----------------------------------------------------------------------
void FTermXTerminal::setHighlightBackground (const FString& hbg)
{
  // Set the highlight background color

  highlight_background_color = hbg;
  setXTermHighlightBackground();
}

//----------------------------------------------------------------------
void FTermXTerminal::setMouseSupport (bool enable)
{
  // activate/deactivate the xterm mouse support

  if ( enable )
    enableXTermMouse();
  else
    disableXTermMouse();
}

//----------------------------------------------------------------------
void FTermXTerminal::metaSendsESC (bool enable)
{
  // activate/deactivate the xterm meta key sends escape prefix

  if ( enable )
    enableXTermMetaSendsESC();
  else
    disableXTermMetaSendsESC();
}

//----------------------------------------------------------------------
void FTermXTerminal::init()
{
  term_detection = FTerm::getFTermDetection();
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
void FTermXTerminal::resetColorMap() const
{
  // Reset the entire color table

  resetXTermColorMap();
}

//----------------------------------------------------------------------
void FTermXTerminal::resetForeground()
{
  // Reset the XTerm text foreground color

  foreground_color.clear();
  resetXTermForeground();
}

//----------------------------------------------------------------------
void FTermXTerminal::resetBackground()
{
  // Reset the XTerm text background color

  background_color.clear();
  resetXTermBackground();
}

//----------------------------------------------------------------------
void FTermXTerminal::resetCursorColor()
{
  // Reset the text cursor color

  cursor_color.clear();
  resetXTermCursorColor();
}

//----------------------------------------------------------------------
void FTermXTerminal::resetMouseForeground()
{
  // Reset the mouse foreground color

  mouse_foreground_color.clear();
  resetXTermMouseForeground();
}

//----------------------------------------------------------------------
void FTermXTerminal::resetMouseBackground()
{
  // Reset the mouse background color

  mouse_background_color.clear();
  resetXTermMouseBackground();
}

//----------------------------------------------------------------------
void FTermXTerminal::resetHighlightBackground()
{
  // Reset the highlight background color

  highlight_background_color.clear();
  resetXTermHighlightBackground();
}

//----------------------------------------------------------------------
void FTermXTerminal::resetDefaults()
{
  initCheck();

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
void FTermXTerminal::resetTitle()
{
  if ( title_was_changed )
    setTitle(xterm_title);
}

//----------------------------------------------------------------------
void FTermXTerminal::captureFontAndTitle()
{
  initCheck();

  if ( ( term_detection->isXTerminal()
    || term_detection->isUrxvtTerminal() )
    && ! term_detection->isRxvtTerminal() )
  {
    FTermios::setCaptureSendCharacters();
    keyboard->setNonBlockingInput();
    xterm_font  = captureXTermFont();
    xterm_title = captureXTermTitle();
    keyboard->unsetNonBlockingInput();
    FTermios::unsetCaptureSendCharacters();
  }
}


// private methods of FTermXTerminal
//----------------------------------------------------------------------
void FTermXTerminal::warnNotInitialized() const
{
  std::clog << FLog::Warn
            << "The FTermXTerminal object has "
            << "not yet been initialized! "
            << "Please call the init() method first."
            << std::endl;
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTermCursorStyle()
{
  // Set the xterm cursor style

#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  if ( FTermFreeBSD::isFreeBSDConsole() )
    return;
#endif

  initCheck();

  if ( term_detection->isGnomeTerminal()
    && ! term_detection->hasSetCursorStyleSupport() )
    return;

  if ( term_detection->isKdeTerminal() )
    return;

  if ( TCAP(fc::t_cursor_style)
    || term_detection->isXTerminal()
    || term_detection->isCygwinTerminal()
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
  initCheck();

  if ( term_detection->isXTerminal()
    || term_detection->isScreenTerm()
    || term_detection->isUrxvtTerminal()
    || term_detection->isCygwinTerminal()
    || term_detection->isMinttyTerm()
    || term_detection->isPuttyTerminal()
    || FTermcap::osc_support )
  {
    oscPrefix();

    if ( xterm_title.isNull() )
      xterm_title = "";

    FTerm::putstringf (OSC "0;%s" BEL, xterm_title.c_str());
    oscPostfix();
    std::fflush(stdout);
    title_was_changed = true;
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTermSize() const
{
  initCheck();

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

  initCheck();

  if ( term_detection->isXTerminal()
    || term_detection->isScreenTerm()
    || term_detection->isUrxvtTerminal()
    || FTermcap::osc_support )
  {
    oscPrefix();
    FTerm::putstringf (OSC "50;%s" BEL, xterm_font.c_str() );
    oscPostfix();
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTermForeground()
{
  // Set the XTerm text foreground color

  initCheck();

  if ( term_detection->isXTerminal()
    || term_detection->isScreenTerm()
    || term_detection->isMinttyTerm()
    || term_detection->isMltermTerminal()
    || FTermcap::osc_support )
  {
    oscPrefix();
    FTerm::putstringf (OSC "10;%s" BEL, foreground_color.c_str());
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTermBackground()
{
  // Set the XTerm text background color

  initCheck();

  if ( term_detection->isXTerminal()
    || term_detection->isScreenTerm()
    || term_detection->isMinttyTerm()
    || term_detection->isMltermTerminal()
    || FTermcap::osc_support )
  {
    oscPrefix();
    FTerm::putstringf (OSC "11;%s" BEL, background_color.c_str());
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTermCursorColor()
{
  // Set the text cursor color

  initCheck();

  if ( term_detection->isXTerminal()
    || term_detection->isScreenTerm()
    || term_detection->isMinttyTerm()
    || term_detection->isUrxvtTerminal()
    || FTermcap::osc_support )
  {
    oscPrefix();
    FTerm::putstringf (OSC "12;%s" BEL, cursor_color.c_str());
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTermMouseForeground()
{
  // Set the mouse foreground color

  initCheck();

  if ( term_detection->isXTerminal()
    || term_detection->isScreenTerm()
    || term_detection->isUrxvtTerminal()
    || FTermcap::osc_support )
  {
    oscPrefix();
    FTerm::putstringf (OSC "13;%s" BEL, mouse_foreground_color.c_str());
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTermMouseBackground()
{
  // Set the mouse background color

  initCheck();

  if ( term_detection->isXTerminal()
    || term_detection->isScreenTerm()
    || FTermcap::osc_support )
  {
    oscPrefix();
    FTerm::putstringf (OSC "14;%s" BEL, mouse_background_color.c_str());
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTermHighlightBackground()
{
  // Set the highlight background color

  initCheck();

  if ( term_detection->isXTerminal()
    || term_detection->isScreenTerm()
    || term_detection->isUrxvtTerminal()
    || FTermcap::osc_support )
  {
    oscPrefix();
    FTerm::putstringf (OSC "17;%s" BEL, highlight_background_color.c_str());
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTerm8ColorDefaults()
{
  // Redefinition of the XTerm default colors
  // for the final cut 8 color theme

  initCheck();

  if ( term_detection->isPuttyTerminal() )
    return;

  setXTermDefaultsMouseCursor();

  if ( canSetXTermBackground() )
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

  initCheck();

  if ( term_detection->isPuttyTerminal() )
    return;

  setXTermDefaultsMouseCursor();

  if ( canSetXTermBackground() )
  {
    // mintty and rxvt can't reset these settings
    setBackground("rgb:8080/a4a4/ecec");           // very light blue
    setForeground("rgb:0000/0000/0000");           // black
    setHighlightBackground("rgb:8787/8787/8787");  // gray53
  }
}

//----------------------------------------------------------------------
inline void FTermXTerminal::setXTermDefaultsMouseCursor()
{
  setMouseBackground("rgb:ffff/ffff/ffff");        // white
  setMouseForeground ("rgb:0000/0000/0000");       // black

  initCheck();

  if ( ! term_detection->isGnomeTerminal() )
    setCursorColor("rgb:ffff/ffff/ffff");          // white
}

//----------------------------------------------------------------------
inline bool FTermXTerminal::canSetXTermBackground() const
{
  initCheck(false);

  if ( xterm_default_colors
    && ! (term_detection->isMinttyTerm()
       || term_detection->isMltermTerminal()
       || term_detection->isRxvtTerminal()
       || term_detection->isScreenTerm()) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
void FTermXTerminal::resetXTermColorMap() const
{
  // Reset the entire color table

  initCheck();

  if ( term_detection->isMinttyTerm() )
  {
    FTerm::putstring (ESC "c");  // Full Reset (RIS)
  }
  else if ( canResetColor() )
  {
    oscPrefix();
    FTerm::putstring (OSC "104" BEL);
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::resetXTermForeground() const
{
  // Reset the XTerm text foreground color

  if ( canResetColor() )
  {
    oscPrefix();
    FTerm::putstring (OSC "110" BEL);
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::resetXTermBackground() const
{
  // Reset the XTerm text background color

  if ( canResetColor() )
  {
    oscPrefix();
    FTerm::putstring (OSC "111" BEL);
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::resetXTermCursorColor() const
{
  // Reset the text cursor color

  if ( canResetColor() )
  {
    oscPrefix();
    FTerm::putstring (OSC "112" BEL);
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::resetXTermMouseForeground() const
{
  // Reset the mouse foreground color

  if ( canResetColor() )
  {
    oscPrefix();
    FTerm::putstring (OSC "113" BEL);
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::resetXTermMouseBackground() const
{
  // Reset the mouse background color

  if ( canResetColor() )
  {
    oscPrefix();
    FTerm::putstring (OSC "114" BEL);
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::resetXTermHighlightBackground() const
{
  // Reset the highlight background color

  if ( canResetColor() )
  {
    oscPrefix();
    FTerm::putstring (OSC "117" BEL);
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
bool FTermXTerminal::canResetColor() const
{
  initCheck(false);

  if ( term_detection->isGnomeTerminal()
    && term_detection->getGnomeTerminalID() < 3502 )
    return false;

  if ( term_detection->isPuttyTerminal()
    || term_detection->isMltermTerminal() )
    return false;

  if ( term_detection->isXTerminal()
    || term_detection->isScreenTerm()
    || FTermcap::osc_support )
    return true;

  return false;
}

//----------------------------------------------------------------------
void FTermXTerminal::oscPrefix() const
{
  initCheck();

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
void FTermXTerminal::oscPostfix() const
{
  initCheck();

  if ( term_detection->isScreenTerm()
    || term_detection->isTmuxTerm() )
  {
    // GNU Screen/tmux string terminator
    FTerm::putstring (ESC "\\");
  }
}

//----------------------------------------------------------------------
FString FTermXTerminal::captureXTermFont() const
{
  initCheck(FString{});

  if ( term_detection->isXTerminal()
    || term_detection->isScreenTerm()
    || FTermcap::osc_support )
  {
    fd_set ifds{};
    struct timeval tv{};
    const int stdin_no = FTermios::getStdIn();

    // Querying the terminal font
    oscPrefix();
    FTerm::putstring (OSC "50;?" BEL);
    oscPostfix();
    std::fflush(stdout);
    FD_ZERO(&ifds);
    FD_SET(stdin_no, &ifds);
    tv.tv_sec  = 0;
    tv.tv_usec = 150000;  // 150 ms

    // Read the terminal answer
    if ( select(stdin_no + 1, &ifds, nullptr, nullptr, &tv) > 0 )
    {
      std::array<char, 150> temp{};
      std::size_t pos{0};

      do
      {
        std::size_t bytes_free = temp.size() - pos - 1;
        const ssize_t bytes = read(stdin_no, &temp[pos], bytes_free);

        if ( bytes <= 0 )
          break;

        pos += std::size_t(bytes);
      }
      while ( pos < temp.size() && std::strchr(temp.data(), '\a') == nullptr );

      if ( pos > 5 && temp[0] == ESC[0] && temp[1] == ']'
        && temp[2] == '5' && temp[3] == '0' && temp[4] == ';' )
      {
        // Skip leading Esc ] 5 0 ;
        char* str = &temp[5];
        const std::size_t n = std::strlen(str);

        // BEL + '\0' = string terminator
        if ( n >= 5 && str[n - 1] == BEL[0] && str[n] == '\0' )
          str[n - 1] = '\0';

        return FString{str};
      }
    }
  }

  return FString{};
}

//----------------------------------------------------------------------
FString FTermXTerminal::captureXTermTitle() const
{
  initCheck(FString{});

  if ( term_detection->isKdeTerminal() )
    return FString{};

  fd_set ifds{};
  struct timeval tv{};
  const int stdin_no{FTermios::getStdIn()};

  // Report window title
  FTerm::putstring (CSI "21t");
  std::fflush(stdout);
  FD_ZERO(&ifds);
  FD_SET(stdin_no, &ifds);
  tv.tv_sec  = 0;
  tv.tv_usec = 150000;  // 150 ms

  // read the terminal answer
  if ( select (stdin_no + 1, &ifds, nullptr, nullptr, &tv) > 0 )
  {
    std::array<char, 512> temp{};
    std::size_t pos{0};

    do
    {
      std::size_t bytes_free = temp.size() - pos - 1;
      const ssize_t bytes = read(stdin_no, &temp[pos], bytes_free);

      if ( bytes <= 0 )
        break;

      pos += std::size_t(bytes);
    }
    while ( pos < temp.size() && std::strstr(temp.data(), ESC "\\") == nullptr );

    if ( pos > 6 && temp[0] == ESC[0] && temp[1] == ']' && temp[2] == 'l' )
    {
      // Skip leading Esc + ] + l = OSC l
      char* str = &temp[3];
      const std::size_t n = std::strlen(str);

      // Esc + \ = OSC string terminator
      if ( n >= 2 && str[n - 2] == ESC[0] && str[n - 1] == '\\' )
      {
        if ( n < 4 )
          return FString{};

        str[n - 2] = '\0';
        return FString{str};
      }
    }
  }

  return FString{};
}

//----------------------------------------------------------------------
void FTermXTerminal::enableXTermMouse()
{
  // Activate the xterm mouse support

  if ( mouse_support )
    return;  // The mouse is already activated

  if ( ! fsystem )
    fsystem = FTerm::getFSystem();

  FTerm::putstring (CSI "?1001s"    // save old highlight mouse tracking
                    CSI "?1000;"    // enable x11 mouse tracking
                         "1002;"    // enable cell motion mouse tracking
                         "1015;"    // enable urxvt mouse mode
                         "1006h");  // enable SGR mouse mode
  std::fflush(stdout);
  mouse_support = true;
}

//----------------------------------------------------------------------
void FTermXTerminal::disableXTermMouse()
{
  // Deactivate the xterm mouse support

  if ( ! mouse_support )
    return;  // The mouse was already deactivated

  FTerm::putstring (CSI "?1006;"    // disable SGR mouse mode
                         "1015;"    // disable urxvt mouse mode
                         "1002;"    // disable cell motion mouse tracking
                         "1000l"    // disable x11 mouse tracking
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
