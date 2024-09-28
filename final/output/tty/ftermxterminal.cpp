/***********************************************************************
* ftermxterminal.cpp - Contains all xterm-specific terminal functions  *
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

#include <array>
#include <memory>
#include <string>

#include "final/fapplication.h"
#include "final/fc.h"
#include "final/input/fkeyboard.h"
#include "final/output/tty/ftermcap.h"
#include "final/output/tty/ftermdata.h"
#include "final/output/tty/ftermdetection.h"
#include "final/output/tty/ftermfreebsd.h"
#include "final/output/tty/fterm.h"
#include "final/output/tty/ftermios.h"
#include "final/output/tty/ftermxterminal.h"
#include "final/util/flog.h"
#include "final/util/fsize.h"
#include "final/util/fstring.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FTermXTerminal
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FTermXTerminal::FTermXTerminal() = default;

// public methods of FTermXTerminal
//----------------------------------------------------------------------
auto FTermXTerminal::getInstance() -> FTermXTerminal&
{
  static const auto& xterm = std::make_unique<FTermXTerminal>();
  return *xterm;
}

//----------------------------------------------------------------------
void FTermXTerminal::setCursorStyle (XTermCursorStyle style)
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

  term_width = size.getWidth();    // width  = columns
  term_height = size.getHeight();  // height = lines
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
void FTermXTerminal::setFocusSupport (bool enable)
{
  // activate/deactivate the terminal focus events

  if ( enable )
    enableXTermFocus();
  else
    disableXTermFocus();
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
void FTermXTerminal::setDefaults()
{
  // Redefinition of the XTerm default colors
  // for the final cut color theme

  setXTermDefaultsMouseCursor();

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
  if ( ! canResetDefaults() )
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
  if ( canCaptureFontAndTitle() )
  {
    FTermios::setCaptureSendCharacters();
    static auto& keyboard = FKeyboard::getInstance();
    keyboard.setNonBlockingInput();
    xterm_font  = captureXTermFont();
    xterm_title = captureXTermTitle();
    keyboard.unsetNonBlockingInput();
    FTermios::unsetCaptureSendCharacters();
  }
}


// private methods of FTermXTerminal
//----------------------------------------------------------------------
void FTermXTerminal::warnNotInitialized() const
{
  std::clog << FLog::LogLevel::Warn
            << "The FTermXTerminal object has "
            << "not yet been initialized! "
            << "Please call the init() method first."
            << std::endl;
}

//----------------------------------------------------------------------
inline auto FTermXTerminal::canResetDefaults() const -> bool
{
  return ! FTermData::getInstance().isTermType(FTermType::putty);
}

//----------------------------------------------------------------------
inline auto FTermXTerminal::canCaptureFontAndTitle() const -> bool
{
  static const auto& fterm_data = FTermData::getInstance();
  return fterm_data.isTermType(FTermType::xterm | FTermType::urxvt)
    && ! fterm_data.isTermType(FTermType::rxvt);
}

//----------------------------------------------------------------------
inline auto FTermXTerminal::canSetXTermCursorStyle() const -> bool
{
#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  if ( FTermFreeBSD::isFreeBSDConsole() )
    return false;
#endif

  static const auto& fterm_data = FTermData::getInstance();
  static const auto& term_detection = FTermDetection::getInstance();

  if ( fterm_data.isTermType(FTermType::gnome_terminal)
    && ! term_detection.hasSetCursorStyleSupport() )
    return false;

  if ( fterm_data.isTermType(FTermType::kde_konsole) )
    return false;

  return TCAP(t_cursor_style)
      || term_detection.hasSetCursorStyleSupport()
      || fterm_data.isTermType ( FTermType::xterm
                               | FTermType::cygwin
                               | FTermType::mintty );
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTermCursorStyle()
{
  // Set the xterm cursor style

  if ( canSetXTermCursorStyle() )
  {
    FTerm::paddingPrintf (CSI "%d q", cursor_style);
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
inline auto FTermXTerminal::canSetXTermTitle() const -> bool
{
  static const auto& fterm_data = FTermData::getInstance();
  return FTermcap::osc_support
      || fterm_data.isTermType ( FTermType::xterm
                               | FTermType::screen
                               | FTermType::urxvt
                               | FTermType::cygwin
                               | FTermType::mintty
                               | FTermType::putty );
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTermTitle()
{
  // Set the xterm title

  if ( canSetXTermSize() )
  {
    oscPrefix();
    FTerm::paddingPrintf (OSC "0;%s" BEL, xterm_title.c_str());
    oscPostfix();
    std::fflush(stdout);
    title_was_changed = true;
  }
}

//----------------------------------------------------------------------
inline auto FTermXTerminal::canSetXTermSize() const -> bool
{
  static const auto& fterm_data = FTermData::getInstance();
  return fterm_data.isTermType(FTermType::xterm);
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTermSize() const
{
  if ( canSetXTermSize() )
  {
    FTerm::paddingPrintf ( CSI "8;%lu;%lut"
                         , uLong(term_height)
                         , uLong(term_width) );
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
inline auto FTermXTerminal::canSetXTermFont() const -> bool
{
  static const auto& fterm_data = FTermData::getInstance();
  return FTermcap::osc_support
      || fterm_data.isTermType ( FTermType::xterm
                               | FTermType::screen
                               | FTermType::urxvt );
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTermFont()
{
  // Change the XTerm font (needs the allowFontOps resource)

  if ( canSetXTermFont() )
  {
    oscPrefix();
    FTerm::paddingPrintf (OSC "50;%s" BEL, xterm_font.c_str() );
    oscPostfix();
  }
}

//----------------------------------------------------------------------
inline auto FTermXTerminal::canSetXTermColor() const -> bool
{
  static const auto& fterm_data = FTermData::getInstance();
  return FTermcap::osc_support
      || fterm_data.isTermType ( FTermType::xterm
                               | FTermType::screen
                               | FTermType::mintty
                               | FTermType::mlterm );
}

//----------------------------------------------------------------------
inline auto FTermXTerminal::canSetXTermForeground() const -> bool
{
  return canSetXTermColor();
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTermForeground()
{
  // Set the XTerm text foreground color

  if ( canSetXTermForeground() )
  {
    oscPrefix();
    FTerm::paddingPrintf (OSC "10;%s" BEL, foreground_color.c_str());
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
inline auto FTermXTerminal::canSetXTermBackground() const -> bool
{
  return canSetXTermColor();
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTermBackground()
{
  // Set the XTerm text background color

  if ( canSetXTermBackground() )
  {
    oscPrefix();
    FTerm::paddingPrintf (OSC "11;%s" BEL, background_color.c_str());
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
inline auto FTermXTerminal::canSetXTermCursorColor() const -> bool
{
  static const auto& fterm_data = FTermData::getInstance();
  return FTermcap::osc_support
      || fterm_data.isTermType ( FTermType::xterm
                               | FTermType::screen
                               | FTermType::mintty
                               | FTermType::urxvt );
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTermCursorColor()
{
  // Set the text cursor color

  if ( canSetXTermCursorColor() )
  {
    oscPrefix();
    FTerm::paddingPrintf (OSC "12;%s" BEL, cursor_color.c_str());
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
inline auto FTermXTerminal::canSetXTermMouseForeground() const -> bool
{
  static const auto& fterm_data = FTermData::getInstance();
  return FTermcap::osc_support
      || ( fterm_data.isTermType ( FTermType::xterm
                                 | FTermType::screen
                                 | FTermType::urxvt )
        && ! fterm_data.isTermType (FTermType::kitty) );
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTermMouseForeground()
{
  // Set the mouse foreground color

  if ( canSetXTermMouseForeground() )
  {
    oscPrefix();
    FTerm::paddingPrintf (OSC "13;%s" BEL, mouse_foreground_color.c_str());
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
inline auto FTermXTerminal::canSetXTermMouseBackground() const -> bool
{
  static const auto& fterm_data = FTermData::getInstance();
  return FTermcap::osc_support
      || ( fterm_data.isTermType(FTermType::xterm | FTermType::screen)
         && ! fterm_data.isTermType(FTermType::kitty) );
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTermMouseBackground()
{
  // Set the mouse background color

  if ( canSetXTermMouseBackground() )
  {
    oscPrefix();
    FTerm::paddingPrintf (OSC "14;%s" BEL, mouse_background_color.c_str());
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
inline auto FTermXTerminal::canSetXTermHighlightBackground() const -> bool
{
  static const auto& fterm_data = FTermData::getInstance();
  return FTermcap::osc_support
      || fterm_data.isTermType ( FTermType::xterm
                               | FTermType::screen
                               | FTermType::urxvt );
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTermHighlightBackground()
{
  // Set the highlight background color

  if ( canSetXTermHighlightBackground() )
  {
    oscPrefix();
    FTerm::paddingPrintf (OSC "17;%s" BEL, highlight_background_color.c_str());
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
inline auto FTermXTerminal::canSetXTermColors() const -> bool
{
  static const auto& fterm_data = FTermData::getInstance();
  return xterm_default_colors
      && ! fterm_data.isTermType ( FTermType::mintty
                                 | FTermType::mlterm
                                 | FTermType::rxvt
                                 | FTermType::screen );
}

//----------------------------------------------------------------------
void FTermXTerminal::setXTerm8ColorDefaults()
{
  // Redefinition of the XTerm default colors
  // for the final cut 8 color theme

  if ( canSetXTermColors() )
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

  if ( canSetXTermColors() )
  {
    // mintty and rxvt can't reset these settings
    setBackground("rgb:8080/a4a4/ecec");           // very light blue
    setForeground("rgb:0000/0000/0000");           // black
    setHighlightBackground("rgb:8787/8787/8787");  // gray53
  }
}

//----------------------------------------------------------------------
inline auto FTermXTerminal::canSetXTermDefaultsMouseCursor() const -> bool
{
  return ! FTermData::getInstance().isTermType(FTermType::putty);
}

//----------------------------------------------------------------------
inline void FTermXTerminal::setXTermDefaultsMouseCursor()
{
  if ( canSetXTermDefaultsMouseCursor() )
  {
    setMouseBackground("rgb:ffff/ffff/ffff");   // white
    setMouseForeground ("rgb:0000/0000/0000");  // black
    setCursorColor("rgb:ffff/ffff/ffff");       // white
  }
}

//----------------------------------------------------------------------
inline auto FTermXTerminal::needsFullReset() const -> bool
{
  return FTermData::getInstance().isTermType(FTermType::mintty);
}

//----------------------------------------------------------------------
inline auto FTermXTerminal::canOnlyResetColorSeparately() const -> bool
{
  return FTermData::getInstance().isTermType(FTermType::stterm);
}

//----------------------------------------------------------------------
auto FTermXTerminal::canResetColor() const -> bool
{
  static const auto& fterm_data = FTermData::getInstance();

  if ( fterm_data.isTermType(FTermType::gnome_terminal)
    && fterm_data.getGnomeTerminalID() < 3502 )
    return false;

  auto unsupported = FTermType::putty
                   | FTermType::mlterm
                   | FTermType::kitty;

  if ( fterm_data.isTermType(unsupported) )
    return false;

  return ( fterm_data.isTermType(FTermType::xterm | FTermType::screen)
        || FTermcap::osc_support );
}

//----------------------------------------------------------------------
void FTermXTerminal::resetXTermColorMap() const
{
  // Reset the entire color table

  if ( needsFullReset() )
  {
    FTerm::paddingPrint (ESC "c");  // Full Reset (RIS)
  }
  else if ( canResetColor() )
  {
    if ( canOnlyResetColorSeparately() )
    {
      for (int c{0}; c < 16; c++)
      {
        oscPrefix();
        FTerm::paddingPrintf (OSC "104;%d" BEL, c);
        oscPostfix();
      }
    }
    else if ( canResetColor() )
    {
      oscPrefix();
      FTerm::paddingPrint (OSC "104" BEL);
      oscPostfix();
    }
  }

  std::fflush(stdout);
}

//----------------------------------------------------------------------
void FTermXTerminal::resetXTermForeground() const
{
  // Reset the XTerm text foreground color

  if ( canResetColor() )
  {
    oscPrefix();
    FTerm::paddingPrint (OSC "110" BEL);
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
    FTerm::paddingPrint (OSC "111" BEL);
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
    FTerm::paddingPrint (OSC "112" BEL);
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
    FTerm::paddingPrint (OSC "113" BEL);
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
    FTerm::paddingPrint (OSC "114" BEL);
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
    FTerm::paddingPrint (OSC "117" BEL);
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::oscPrefix() const
{
  static const auto& fterm_data = FTermData::getInstance();

  if ( fterm_data.isTermType(FTermType::tmux) )
  {
    // tmux device control string
    FTerm::paddingPrint (ESC "Ptmux;" ESC);
  }
  else if ( fterm_data.isTermType(FTermType::screen) )
  {
    // GNU Screen device control string
    FTerm::paddingPrint (ESC "P");
  }
}

//----------------------------------------------------------------------
void FTermXTerminal::oscPostfix() const
{
  static const auto& fterm_data = FTermData::getInstance();

  if ( fterm_data.isTermType(FTermType::screen | FTermType::tmux) )
  {
    // GNU Screen/tmux string terminator
    FTerm::paddingPrint (ESC "\\");
  }
}

//----------------------------------------------------------------------
inline auto FTermXTerminal::canCaptureXTermFont() const -> bool
{
  static const auto& fterm_data = FTermData::getInstance();
  return FTermcap::osc_support
      || ( fterm_data.isTermType(FTermType::xterm | FTermType::screen)
        && ! fterm_data.isTermType(FTermType::kitty) );
}

//----------------------------------------------------------------------
auto FTermXTerminal::captureXTermFont() const -> FString
{
  if ( ! canCaptureXTermFont() )
    return {};

  // Querying the terminal font
  oscPrefix();
  FTerm::paddingPrint (OSC "50;?" BEL);
  oscPostfix();
  std::fflush(stdout);
  std::array<char, 150> temp{};
  // BEL = \a = bell character
  auto isWithout_BEL = [] (const auto& t) { return ! std::strchr(t.data(), '\a'); };
  auto pos = captureTerminalInput(temp, 150'000, isWithout_BEL);

  if ( pos > 5 && temp[0] == ESC[0] && temp[1] == ']' && temp[2] == '5'
               && temp[3] == '0' && temp[4] == ';' )
  {
    // Skip leading Esc ] 5 0 ;
    std::string str = &temp[5];
    const std::size_t n = str.length();

    // BEL + '\0' = string terminator
    if ( n >= 5 && str[n - 1] == BEL[0] && str[n] == '\0' )
      str.erase(n - 1);

    return {str};
  }

  return {};
}

//----------------------------------------------------------------------
inline auto FTermXTerminal::canCaptureXTermTitle() const -> bool
{
  const auto unsupported = FTermType::kde_konsole | FTermType::kitty;
  static const auto& fterm_data = FTermData::getInstance();
  return ! fterm_data.isTermType(unsupported);
}

//----------------------------------------------------------------------
auto FTermXTerminal::captureXTermTitle() const -> FString
{
  if ( ! canCaptureXTermTitle() )
    return {};

  // Report window title
  FTerm::paddingPrint (CSI "21t");
  std::fflush(stdout);
  std::array<char, 512> temp{};
  // ST = ESC + \ = string terminator
  auto isWithout_ST  = [] (const auto& t) { return ! std::strstr(t.data(), ESC "\\"); };
  auto pos = captureTerminalInput(temp, 150'000, isWithout_ST);

  if ( pos > 6 && temp[0] == ESC[0] && temp[1] == ']' && temp[2] == 'l' )
  {
    // Skip leading Esc + ] + l = OSC l
    std::string str = &temp[3];
    const std::size_t n = str.length();

    // Esc + \ = OSC string terminator
    if ( n >= 2 && str[n - 2] == ESC[0] && str[n - 1] == '\\' )
    {
      if ( n < 4 )
        return {};

      str.erase(n - 2);
      return {str};
    }
  }

  return {};
}

//----------------------------------------------------------------------
void FTermXTerminal::enableXTermMouse()
{
  // Activate the xterm mouse support

  if ( mouse_support )
    return;  // The mouse is already activated

  FTerm::paddingPrint (CSI "?1001s"    // save old highlight mouse tracking
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

  FTerm::paddingPrint (CSI "?1006;"    // disable SGR mouse mode
                            "1015;"    // disable urxvt mouse mode
                            "1002;"    // disable cell motion mouse tracking
                            "1000l"    // disable x11 mouse tracking
                       CSI "?1001r");  // restore old highlight mouse tracking
  std::fflush(stdout);
  mouse_support = false;
}

//----------------------------------------------------------------------
void FTermXTerminal::enableXTermFocus()
{
  // Activate terminal focus event

  if ( focus_support )
    return;  // The terminal focus event is already activated

  FTerm::paddingPrint (CSI "?1004h");  // enable send FocusIn/FocusOut
  std::fflush(stdout);
  focus_support = true;
}

//----------------------------------------------------------------------
void FTermXTerminal::disableXTermFocus()
{
  // Deactivate terminal focus event

  if ( ! focus_support )
    return;  // The terminal focus event was already deactivated

  FTerm::paddingPrint (CSI "?1004l");  // disable Send FocusIn/FocusOut
  std::fflush(stdout);
  focus_support = false;
}

//----------------------------------------------------------------------
inline auto FTermXTerminal::canUseXTermMetaSendsESC() const -> bool
{
  return ! FTermData::getInstance().isTermType(FTermType::kitty);
}

//----------------------------------------------------------------------
void FTermXTerminal::enableXTermMetaSendsESC()
{
  // Activate the xterm meta key sends escape prefix

  if ( meta_sends_esc || canUseXTermMetaSendsESC() )
    return;

  FTerm::paddingPrint (CSI "?1036s"    // save meta key sends escape
                       CSI "?1036h");  // enable meta key sends escape
  std::fflush(stdout);
  meta_sends_esc = true;
}

//----------------------------------------------------------------------
void FTermXTerminal::disableXTermMetaSendsESC()
{
  // Deactivate the xterm meta key sends escape prefix

  if ( ! meta_sends_esc || canUseXTermMetaSendsESC() )
    return;

  FTerm::paddingPrint (CSI "?1036r");  // restore meta key sends escape
  std::fflush(stdout);
  meta_sends_esc = false;
}

}  // namespace finalcut
