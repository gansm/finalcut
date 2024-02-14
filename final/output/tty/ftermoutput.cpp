/***********************************************************************
* ftermoutput.cpp - Implements the terminal output                     *
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

#include <algorithm>
#include <unistd.h>
#include <unordered_map>

#include "final/fobject.h"
#include "final/fstartoptions.h"
#include "final/input/fkeyboard.h"
#include "final/input/fmouse.h"
#include "final/output/tty/foptiattr.h"
#include "final/output/tty/foptimove.h"
#include "final/output/tty/ftermcap.h"
#include "final/output/tty/ftermdata.h"
#include "final/output/tty/ftermfreebsd.h"
#include "final/output/tty/ftermios.h"
#include "final/output/tty/ftermoutput.h"
#include "final/output/tty/ftermxterminal.h"
#include "final/util/char_ringbuffer.h"
#include "final/util/fpoint.h"
#include "final/util/fsize.h"

namespace finalcut
{

namespace internal
{

struct var
{
  static Encoding terminal_encoding;
};

Encoding var::terminal_encoding{Encoding::Unknown};

}  // namespace internal

// static class attributes
FVTerm::FTermArea* FTermOutput::vterm{nullptr};
FTermData*         FTermOutput::fterm_data{nullptr};
constexpr uInt64   FTermOutput::MIN_FLUSH_WAIT;
constexpr uInt64   FTermOutput::MAX_FLUSH_WAIT;

//----------------------------------------------------------------------
// class FTermOutput
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FTermOutput::FTermOutput (const FVTerm& t)  // constructor
  : FOutput{t}
{
  fterm_data = &FTermData::getInstance();
  FTermcap::setDefaultPutCharFunction();
  FTermcap::setDefaultPutStringFunction();
}

//----------------------------------------------------------------------
FTermOutput::~FTermOutput() noexcept = default;  // destructor


// public methods of FTermOutput
//----------------------------------------------------------------------
auto FTermOutput::getColumnNumber() const -> std::size_t
{
  return FTerm::getColumnNumber();
}

//----------------------------------------------------------------------
auto FTermOutput::getLineNumber() const -> std::size_t
{
  return FTerm::getLineNumber();
}

//----------------------------------------------------------------------
auto FTermOutput::getTabstop() const -> int
{
  return FTerm::getTabstop();
}

//----------------------------------------------------------------------
auto FTermOutput::getMaxColor() const -> int
{
  return FTerm::getMaxColor();
}

//----------------------------------------------------------------------
auto FTermOutput::getEncoding() const -> Encoding
{
  return internal::var::terminal_encoding;
}

//----------------------------------------------------------------------
auto FTermOutput::getKeyName (FKey keynum) const -> FString
{
  return FTerm::getKeyName(keynum);
}

//----------------------------------------------------------------------
auto FTermOutput::isMonochron() const -> bool
{
  return FTerm::isMonochron();
}

//----------------------------------------------------------------------
auto FTermOutput::isNewFont() const -> bool
{
  return FTerm::isNewFont();
}

//----------------------------------------------------------------------
auto FTermOutput::isEncodable (const wchar_t& wide_char) const -> bool
{
  return FTerm::isEncodable(wide_char);
}

//----------------------------------------------------------------------
auto FTermOutput::isFlushTimeout() const -> bool
{
  return FObjectTimer::isTimeout (time_last_flush, flush_wait);
}

//----------------------------------------------------------------------
auto FTermOutput::hasTerminalResized() const -> bool
{
  return FTerm::hasChangedTermSize();
}

//----------------------------------------------------------------------
auto FTermOutput::allowsTerminalSizeManipulation() const -> bool
{
  return fterm_data->isTermType(FTermType::xterm);
}

//----------------------------------------------------------------------
auto FTermOutput::canChangeColorPalette() const -> bool
{
  return FTerm::canChangeColorPalette();
}

//----------------------------------------------------------------------
auto FTermOutput::hasHalfBlockCharacter() const -> bool
{
  return FTerm::hasHalfBlockCharacter();
}

//----------------------------------------------------------------------
auto FTermOutput::hasShadowCharacter() const -> bool
{
  return FTerm::hasShadowCharacter();
}

//----------------------------------------------------------------------
auto FTermOutput::areMetaAndArrowKeysSupported() const -> bool
{
  return ! fterm_data->isTermType(FTermType::linux_con);
}

//----------------------------------------------------------------------
void FTermOutput::setCursor (FPoint p)
{
  // Sets the hardware cursor to the given (x,y) position

  if ( p == *term_pos )  // Check if cursor position is unchanged
    return;

  adjustCursorPosition(p);
  const auto term_x = term_pos->getX();
  const auto term_y = term_pos->getY();
  const auto x = p.getX();
  const auto y = p.getY();
  const auto& move_str = FTerm::moveCursorString (term_x, term_y, x, y);

  if ( ! move_str.empty() )
    appendOutputBuffer (FTermControl{move_str});

  term_pos->setPoint(x, y);
}

//----------------------------------------------------------------------
void FTermOutput::setCursor (CursorMode mode)
{
  if ( mode == CursorMode::Insert )
    FTerm::setInsertCursor();
  else if ( mode == CursorMode::Overwrite )
    FTerm::unsetInsertCursor();
  else
    throw std::invalid_argument("Unimplemented cursor mode");
}

//----------------------------------------------------------------------
void FTermOutput::hideCursor (bool enable)
{
  // Hides or shows the input cursor on the terminal

  if ( ! isCursorHideable() )
    return;

  auto visibility_str = FTerm::cursorsVisibilityString (enable);

  if ( visibility_str.empty() )  // Exit the function if the string is empty
    return;

  appendOutputBuffer (FTermControl{std::move(visibility_str)});
  flush();
}

//----------------------------------------------------------------------
void FTermOutput::setTerminalSize (FSize size)
{
  FTerm::setTermSize(size);
}

//----------------------------------------------------------------------
auto FTermOutput::setVGAFont() -> bool
{
  return FTerm::setVGAFont();
}

//----------------------------------------------------------------------
auto FTermOutput::setNewFont() -> bool
{
  return FTerm::setNewFont();
}

//----------------------------------------------------------------------
void FTermOutput::setNonBlockingRead (bool enable)
{
#if defined(__CYGWIN__)
  // Fixes problem with mouse input
  char termfilename[256]{};

  if ( ttyname_r(1, termfilename, sizeof(termfilename)) )
    termfilename[0] = '\0';

  if ( std::memcmp(termfilename, "/dev/cons", 9) == 0 )
  {
    FKeyboard::setNonBlockingInputSupport(false);
    return;
  }
#endif

  uInt64 blocking_time = enable ? 5000 : 100'000;  // 5 or 100 ms
  FKeyboard::setReadBlockingTime (blocking_time);
}

//----------------------------------------------------------------------
void FTermOutput::initTerminal (FVTerm::FTermArea* virtual_terminal)
{
  getFTerm().initTerminal();
  internal::var::terminal_encoding = fterm_data->getTerminalEncoding();

  // Redefine the color palette
  redefineColorPalette();

  vterm         = virtual_terminal;
  output_buffer = std::make_shared<OutputBuffer>();
  term_pos      = std::make_shared<FPoint>(-1, -1);

  // Hide the input cursor
  cursor_hideable = FTerm::isCursorHideable();
  hideCursor();

  // Defining the character length of termcap strings
  init_characterLengths();

  // Check for support for combined characters
  init_combined_character();

  // Resetting the status of terminal attributes
  clearTerminalState();

  // Initialize the last flush time
  time_last_flush = TimeValue{};
}

//----------------------------------------------------------------------
void FTermOutput::finishTerminal()
{
  // Restore the color palette
  restoreColorPalette();

  // Show the input cursor
  showCursor();

  // Clear the terminal
  if ( fterm_data->isInAlternateScreen() )
    clearTerminal();
}

//----------------------------------------------------------------------
auto FTermOutput::updateTerminal() -> bool
{
  // Updates pending changes to the terminal

  std::size_t changedlines = 0;

  for (uInt y{0}; y < uInt(vterm->size.height); y++)
  {
    FVTerm::reduceTerminalLineUpdates(y);

    if ( updateTerminalLine(y) )
      changedlines++;
  }

  vterm->has_changes = false;

  // sets the new input cursor position
  const auto& cursor_update = updateTerminalCursor();
  return cursor_update || changedlines > 0;
}

//----------------------------------------------------------------------
void FTermOutput::detectTerminalSize()
{
  FTerm::detectTermSize();
}

//----------------------------------------------------------------------
void FTermOutput::commitTerminalResize()
{
  FTerm::changeTermSizeFinished();
}

//----------------------------------------------------------------------
void FTermOutput::initScreenSettings()
{
  FTerm::initScreenSettings();
}

//----------------------------------------------------------------------
auto FTermOutput::scrollTerminalForward() -> bool
{
  if ( ! TCAP(t_scroll_forward) )
    return false;

  FTerm::scrollTermForward();
  return true;
}

//----------------------------------------------------------------------
auto FTermOutput::scrollTerminalReverse() -> bool
{
  if ( ! TCAP(t_scroll_reverse) )
    return false;

  FTerm::scrollTermReverse();
  return true;
}

//----------------------------------------------------------------------
void FTermOutput::clearTerminalAttributes()
{
  FTerm::clearTerminalAttributes();
}

//----------------------------------------------------------------------
void FTermOutput::clearTerminalState()
{
  // term_attribute stores the current state of the terminal
  term_attribute.ch           = {{ L'\0' }};
  term_attribute.fg_color     = FColor::Undefined;
  term_attribute.bg_color     = FColor::Undefined;
  term_attribute.attr.byte[0] = 0;
  term_attribute.attr.byte[1] = 0;
  term_attribute.attr.byte[2] = 0;
  term_attribute.attr.byte[3] = 0;
}

//----------------------------------------------------------------------
auto FTermOutput::clearTerminal (wchar_t fillchar) -> bool
{
  // Clear the real terminal and put cursor at home

  const auto& cl = TCAP(t_clear_screen);
  const auto& cd = TCAP(t_clr_eos);
  const auto& cb = TCAP(t_clr_eol);
  const bool ut = FTermcap::background_color_erase;
  auto& next_attribute = FVTerm::getAttribute();
  const auto& normal = FOptiAttr::isNormal(next_attribute);
  appendAttributes (next_attribute);

  if ( ! ( (cl || cd || cb) && (normal || ut) )
    || fillchar != L' '
    || ! FVTerm::isDrawingFinished() )
  {
    return false;
  }

  if ( cl )  // Clear screen
  {
    appendOutputBuffer (FTermControl{cl});
    term_pos->setPoint(0, 0);
  }
  else if ( cd )  // Clear to end of screen
  {
    setCursor (FPoint{0, 0});
    appendOutputBuffer (FTermControl{cd});
    term_pos->setPoint(-1, -1);
  }
  else if ( cb )  // Clear to end of line
  {
    term_pos->setPoint(-1, -1);

    for (auto i{0}; i < int(getLineNumber()); i++)
    {
      setCursor (FPoint{0, i});
      appendOutputBuffer (FTermControl{cb});
    }

    setCursor (FPoint{0, 0});
  }

  flush();
  return true;
}

//----------------------------------------------------------------------
void FTermOutput::flush()
{
  // Flush the output buffer

  flushTimeAdjustment();

  if ( ! output_buffer || output_buffer->isEmpty()
    || ! (isFlushTimeout() || getFVTerm().isTerminalUpdateForced()) )
    return;

  while ( ! output_buffer->isEmpty() )
  {
    const auto& first = output_buffer->front();
    const auto& type = first.type;
    const auto& data = first.data;

    if ( type == OutputType::String )
      FTerm::stringPrint (data);
    else if ( type == OutputType::Control )
      FTerm::paddingPrint (data);

    output_buffer->pop();
  }

  std::fflush(stdout);
  static auto& mouse = FMouseControl::getInstance();
  mouse.drawPointer();
  time_last_flush = FObjectTimer::getCurrentTime();
}

//----------------------------------------------------------------------
void FTermOutput::beep() const
{
  return FTerm::beep();
}


// private methods of FTermOutput
//----------------------------------------------------------------------
inline auto FTermOutput::getStartOptions() & -> FStartOptions&
{
  return FStartOptions::getInstance();
}

//----------------------------------------------------------------------
inline auto FTermOutput::isInputCursorInsideTerminal() const -> bool
{
  if ( ! vterm || ! vterm->input_cursor_visible )
    return false;

  const int x = vterm->input_cursor.x;
  const int y = vterm->input_cursor.y;
  return ( x >= 0 && x < int(getColumnNumber())
        && y >= 0 && y < int(getLineNumber()) );
}

//----------------------------------------------------------------------
inline auto FTermOutput::isDefaultPaletteTheme() -> bool
{
  FStringList default_themes
  {
    "default8ColorPalette",
    "default16ColorPalette",
    "default16DarkColorPalette"
  };

  const auto& iter = std::find ( default_themes.cbegin()
                               , default_themes.cend()
                               , FColorPalette::getInstance()->getClassName() );
  return iter != default_themes.cend();  // Default theme found
}

//----------------------------------------------------------------------
void FTermOutput::redefineColorPalette()
{
  // Redefine the color palette

  if ( ! (canChangeColorPalette() && getStartOptions().color_change) )
    return;

  FTerm::resetColorMap();
  FTerm::saveColorMap();

  if ( FColorPalette::getInstance().use_count() > 0
    && ! isDefaultPaletteTheme() )
  {
    // A user color palette theme is in use
    FColorPalette::getInstance()->setColorPalette();
    return;
  }

  if ( getStartOptions().dark_theme )
  {
    setColorPaletteTheme<default16DarkColorPalette>();
  }
  else
  {
    if ( getMaxColor() >= 16 )
      setColorPaletteTheme<default16ColorPalette>();
    else  // 8 colors
      setColorPaletteTheme<default8ColorPalette>();
  }
}

//----------------------------------------------------------------------
void FTermOutput::restoreColorPalette()
{
  if ( ! (canChangeColorPalette() && getStartOptions().color_change) )
    return;

  // Reset screen settings
  FColorPalette::getInstance()->resetColorPalette();
  FTermXTerminal::getInstance().resetColorMap();
  FTerm::resetColorMap();
}

//----------------------------------------------------------------------
void FTermOutput::init_characterLengths()
{
  const auto& opti_move = FOptiMove::getInstance();
  cursor_address_length = opti_move.getCursorAddressLength();
  erase_char_length     = opti_move.getEraseCharsLength();
  repeat_char_length    = opti_move.getRepeatCharLength();
  clr_bol_length        = opti_move.getClrBolLength();
  clr_eol_length        = opti_move.getClrEolLength();

  if ( cursor_address_length == 0 )
    cursor_address_length = INT_MAX;

  if ( erase_char_length == 0 )
    erase_char_length = INT_MAX;

  if ( repeat_char_length == 0 )
    repeat_char_length = INT_MAX;

  if ( clr_bol_length == 0 )
    clr_bol_length = INT_MAX;

  if ( clr_eol_length == 0 )
    clr_eol_length = INT_MAX;
}

//----------------------------------------------------------------------
void FTermOutput::init_combined_character()
{
#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  if ( FTermFreeBSD::isFreeBSDConsole() )
    return;
#endif

  if ( internal::var::terminal_encoding != Encoding::UTF8
    || fterm_data->isTermType(FTermType::cygwin) )
    return;

  if ( fterm_data->isTermType ( FTermType::xterm
                              | FTermType::urxvt
                              | FTermType::mintty
                              | FTermType::putty ) )
  {
    combined_char_support = true;
  }
}

//----------------------------------------------------------------------
auto FTermOutput::canClearToEOL (uInt xmin, uInt y) const -> bool
{
  // Is the line from xmin to the end of the line blank?
  // => clear to end of line

  const auto& ce = TCAP(t_clr_eol);
  const auto* min_char = &vterm->getFChar(int(xmin), int(y));

  if ( ! ce || min_char->ch[0] != L' ' )
    return false;

  uInt beginning_whitespace = 1;
  const auto& normal = FOptiAttr::isNormal(*min_char);
  const auto& ut = FTermcap::background_color_erase;
  const auto* ch = min_char + 1;

  for (int x{int(xmin) + 1}; x < vterm->size.width; x++)
  {
    if ( *min_char == *ch )
      beginning_whitespace++;
    else
      break;

    ++ch;
  }

  return ( beginning_whitespace == uInt(vterm->size.width) - xmin
        && (ut || normal)
        && clr_eol_length < beginning_whitespace );
}

//----------------------------------------------------------------------
auto FTermOutput::canClearLeadingWS (uInt& xmin, uInt y) const -> bool
{
  // Line has leading whitespace
  // => clear from xmin to beginning of line

  const auto& cb = TCAP(t_clr_bol);
  const auto* first_char = &vterm->getFChar(0, int(y));

  if ( ! cb || first_char->ch[0] != L' ' )
    return false;

  uInt leading_whitespace = 1;
  const auto& normal = FOptiAttr::isNormal(*first_char);
  const auto& ut = FTermcap::background_color_erase;
  const auto* ch = first_char + 1;

  for (int x{1}; x < vterm->size.width; x++)
  {
    if ( *first_char == *ch )
      leading_whitespace++;
    else
      break;

    ++ch;
  }

  if ( leading_whitespace > xmin && (ut || normal)
    && clr_bol_length < leading_whitespace )
  {
    xmin = leading_whitespace - 1;
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
auto FTermOutput::canClearTrailingWS (uInt& xmax, uInt y) const -> bool
{
  // Line has trailing whitespace
  // => clear from xmax to end of line

  const auto& ce = TCAP(t_clr_eol);
  const auto* last_char = &vterm->getFChar(vterm->size.width - 1, int(y));

  if ( ! ce || last_char->ch[0] != L' ' )
    return false;

  uInt trailing_whitespace = 1;
  const auto& normal = FOptiAttr::isNormal(*last_char);
  const auto& ut = FTermcap::background_color_erase;
  const auto* ch = last_char;

  for (int x{vterm->size.width - 1}; x > 0 ; x--)
  {
    if ( *last_char == *ch )
      trailing_whitespace++;
    else
      break;

    --ch;
  }

  if ( trailing_whitespace > uInt(vterm->size.width) - xmax && (ut || normal)
    && clr_bol_length < trailing_whitespace )
  {
    xmax = uInt(vterm->size.width) - trailing_whitespace;
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
auto FTermOutput::skipUnchangedCharacters (uInt& x, uInt xmax, uInt y) -> bool
{
  // Skip characters without changes if it is faster than redrawing

  auto* print_char = &vterm->getFChar(int(x), int(y));
  print_char->attr.bit.printed = true;

  if ( ! print_char->attr.bit.no_changes )
    return false;

  uInt count{1};
  const auto* ch = print_char + 1;
  const auto* end = print_char + xmax - x + 1;

  // Unroll the loop for better performance
  while ( ch + 4 <= end )
  {
    if ( ch[0].attr.bit.no_changes && ch[1].attr.bit.no_changes
      && ch[2].attr.bit.no_changes && ch[3].attr.bit.no_changes )
    {
      count += 4;
    }
    else
      break;

    ch += 4;
  }

  // Handle the remaining elements
  while ( ch < end )
  {
    if ( ch->attr.bit.no_changes )
      count++;
    else
      break;

    ++ch;
  }

  if ( count > cursor_address_length )
  {
    setCursor (FPoint{int(x + count), int(y)});
    x = x + count - 1;
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
void FTermOutput::printRange (uInt xmin, uInt xmax, uInt y)
{
  const auto& ec = TCAP(t_erase_chars);
  const auto& rp = TCAP(t_repeat_char);
  uInt x = xmin;
  uInt x_last = x;
  auto* min_char = &vterm->getFChar(int(x), int(y));
  auto* print_char = min_char;

  while ( x <= xmax )
  {
    print_char += x - x_last;
    print_char->attr.bit.printed = true;
    x_last = x;
    replaceNonPrintableFullwidth (x, *print_char);

    // skip character with no changes
    if ( skipUnchangedCharacters(x, xmax, y) )
    {
      x++;
      continue;
    }

    // Erase character
    if ( ec && print_char->ch[0] == L' ' )
    {
      if ( eraseCharacters(x, xmax, y) \
           == PrintState::LineCompletelyPrinted )
        break;
    }
    else if ( rp )  // Repeat one character n-fold
    {
      repeatCharacter(x, xmax, y);
    }
    else  // General character output
    {
      bool min_and_not_max( x == xmin && xmin != xmax );
      printCharacter (x, y, min_and_not_max, *print_char);
    }

    x++;
  }
}

//----------------------------------------------------------------------
inline void FTermOutput::replaceNonPrintableFullwidth ( uInt x
                                                      , FChar& print_char ) const
{
  // Replace non-printable full-width characters that are truncated
  // from the right or left terminal side

  if ( x == 0 && isFullWidthPaddingChar(print_char) )
  {
    print_char.ch[0] = wchar_t(UniChar::SingleLeftAngleQuotationMark);  // ‹
    print_char.ch[1] = L'\0';
    print_char.attr.bit.fullwidth_padding = false;
  }
  else if ( x == uInt(vterm->size.width - 1) && isFullWidthChar(print_char) )
  {
    print_char.ch[0] = wchar_t(UniChar::SingleRightAngleQuotationMark);  // ›
    print_char.ch[1] = L'\0';
    print_char.attr.bit.char_width = 1;
  }
}

//----------------------------------------------------------------------
void FTermOutput::printCharacter ( uInt& x, uInt y, bool min_and_not_max
                                 , FChar& print_char)
{
  // General character output on terminal

  if ( x < uInt(vterm->size.width - 1) && isFullWidthChar(print_char) )
  {
    printFullWidthCharacter (x, y, print_char);
  }
  else if ( x > 0 && x < uInt(vterm->size.width - 1)
         && isFullWidthPaddingChar(print_char)  )
  {
    printFullWidthPaddingCharacter (x, y, print_char);
  }
  else if ( x > 0 && min_and_not_max )
  {
    printHalfCovertFullWidthCharacter (x, y, print_char);
  }
  else
  {
    // Print a half-width character
    appendCharacter (print_char);
    markAsPrinted (x, y);
  }
}

//----------------------------------------------------------------------
void FTermOutput::printFullWidthCharacter ( uInt& x, uInt y
                                          , FChar& print_char )
{
  auto& next_char = vterm->getFChar(int(x + 1), int(y));

  if ( print_char.attr.byte[0] == next_char.attr.byte[0]
    && print_char.attr.byte[1] == next_char.attr.byte[1]
    && print_char.fg_color == next_char.fg_color
    && print_char.bg_color == next_char.bg_color
    && isFullWidthChar(print_char)
    && isFullWidthPaddingChar(next_char) )
  {
    // Print a full-width character
    appendCharacter (print_char);
    markAsPrinted (x, y);
    skipPaddingCharacter (x, y, print_char);
  }
  else
  {
    // Print ellipses for the 1st full-width character column
    printEllipsis (x, y, print_char);

    if ( isFullWidthPaddingChar(next_char) )
    {
      // Print ellipses for the 2nd full-width character column
      x++;
      printEllipsis (x, y, next_char);
    }
  }
}

//----------------------------------------------------------------------
void FTermOutput::printFullWidthPaddingCharacter ( uInt& x, uInt y
                                                 , FChar& print_char)
{
  auto& prev_char = vterm->getFChar(int(x - 1), int(y));

  if ( print_char.attr.byte[0] == prev_char.attr.byte[0]
    && print_char.attr.byte[1] == prev_char.attr.byte[1]
    && print_char.fg_color == prev_char.fg_color
    && print_char.bg_color == prev_char.bg_color
    && isFullWidthChar(prev_char)
    && isFullWidthPaddingChar(print_char) )
  {
    if ( moveCursorLeft() == CursorMoved::No )
    {
      skipPaddingCharacter (x, y, prev_char);
      return;
    }

    // Print a full-width character
    x--;
    term_pos->x_ref()--;
    appendCharacter (prev_char);
    markAsPrinted (x, y);
    skipPaddingCharacter (x, y, prev_char);
  }
  else
  {
    // Print ellipses for the 1st full-width character column
    printEllipsis (x, y, print_char);
  }
}

//----------------------------------------------------------------------
void FTermOutput::printHalfCovertFullWidthCharacter ( uInt x, uInt y
                                                    , FChar& print_char )
{
  auto& prev_char = vterm->getFChar(int(x - 1), int(y));

  if ( isFullWidthChar(prev_char)
    && ! isFullWidthPaddingChar(print_char)
    && moveCursorLeft() == CursorMoved::Yes )
  {
    // Print ellipses for the 1st full-width character column
    term_pos->x_ref()--;
    printEllipsis (x - 1, y, prev_char);
  }

  // Print a half-width character
  appendCharacter (print_char);
  markAsPrinted (x, y);
}

//----------------------------------------------------------------------
inline void FTermOutput::printEllipsis (uInt x, uInt y, FChar& fchar)
{
  // Printing ellipses with color and attributes from fchar
  appendAttributes (fchar);
  appendOutputBuffer (UniChar::HorizontalEllipsis);  // …
  term_pos->x_ref()++;
  markAsPrinted (x, y);
}

//----------------------------------------------------------------------
inline void FTermOutput::skipPaddingCharacter ( uInt& x, uInt y
                                              , const FChar& print_char ) const
{
  if ( isFullWidthChar(print_char) )  // full-width character
  {
    x++;  // Skip the following padding character
    term_pos->x_ref()++;
    markAsPrinted (x, y);
  }
}

//----------------------------------------------------------------------
auto FTermOutput::eraseCharacters (uInt& x, uInt xmax, uInt y) -> PrintState
{
  // Erase a number of characters to draw simple whitespaces

  const auto& ec = TCAP(t_erase_chars);
  auto* print_char = &vterm->getFChar(int(x), int(y));

  if ( ! ec || print_char->ch[0] != L' ' )
    return PrintState::NothingPrinted;

  auto whitespace = countRepetitions(print_char, x, xmax);

  if ( whitespace == 1 )
  {
    appendCharacter (*print_char);
    markAsPrinted (x, y);
    return PrintState::WhitespacesPrinted;
  }

  const uInt start_pos = x;
  const uInt end_pos = x + whitespace - 1;

  if ( canUseEraseCharacters(print_char, whitespace) )
  {
    appendAttributes (*print_char);
    appendOutputBuffer (FTermControl{FTermcap::encodeParameter(ec, whitespace)});

    if ( end_pos <= xmax )
      setCursor (FPoint{int(x + whitespace), int(y)});
    else
      return PrintState::LineCompletelyPrinted;
  }
  else
    appendCharacter_n (*print_char, whitespace);

  markAsPrinted (start_pos, end_pos, y);
  x = end_pos;
  return PrintState::WhitespacesPrinted;
}

//----------------------------------------------------------------------
auto FTermOutput::repeatCharacter (uInt& x, uInt xmax, uInt y) -> PrintState
{
  // Repeat one character n-fold

  const auto& rp = TCAP(t_repeat_char);
  const auto& lr = TCAP(t_repeat_last_char);
  auto* print_char = &vterm->getFChar(int(x), int(y));

  if ( ! rp && ! lr )
    return PrintState::NothingPrinted;

  auto repetitions = countRepetitions(print_char, x, xmax);

  if ( repetitions == 1 )
  {
    bool min_and_not_max( x != xmax );
    printCharacter (x, y, min_and_not_max, *print_char);
    return PrintState::RepeatCharacterPrinted;
  }

  // Every full-width character is always followed by a padding
  // character, so that two or more consecutive full-width characters
  // cannot repeat in their byte sequence
  const uInt start_pos = x;
  const uInt end_pos = x + repetitions - 1;
  const auto repetition_type = getRepetitionType(print_char, repetitions);

  if ( rp && repetition_type == Repetition::ASCII )
  {
    newFontChanges (*print_char);
    charsetChanges (*print_char);
    appendAttributes (*print_char);
    appendOutputBuffer (FTermControl{FTermcap::encodeParameter(rp, print_char->ch[0], repetitions)});
    term_pos->x_ref() += static_cast<int>(repetitions);
  }
  else if ( lr && repetition_type == Repetition::UTF8 )
  {
    appendChar (*print_char);
    appendOutputBuffer (FTermControl{FTermcap::encodeParameter(lr, repetitions)});
    term_pos->x_ref() += static_cast<int>(repetitions);
  }
  else
    appendCharacter_n (*print_char, repetitions);

  markAsPrinted (start_pos, end_pos, y);
  x = end_pos;
  return PrintState::RepeatCharacterPrinted;
}

//----------------------------------------------------------------------
inline auto FTermOutput::countRepetitions ( const FChar* print_char
                                          , uInt from, uInt to ) const -> uInt
{
  uInt repetitions{1};
  const auto* ch = print_char + 1;

  for (uInt i{from + 1}; i <= to; i++)
  {
    if ( *print_char == *ch )
      repetitions++;
    else
      break;

    ++ch;
  }

  return repetitions;
}

//----------------------------------------------------------------------
inline auto FTermOutput::canUseEraseCharacters ( const FChar* print_char
                                               , uInt whitespace ) const -> bool
{
  const auto normal = FOptiAttr::isNormal(*print_char);
  const auto ut = FTermcap::background_color_erase;
  const uInt total_length_required = erase_char_length
                                   + cursor_address_length;

  return whitespace > total_length_required && (ut || normal);
}

//----------------------------------------------------------------------
inline auto FTermOutput::canUseCharacterRepetitions ( const FChar* print_char
                                                    , uInt repetitions ) const -> bool
{
  return repetitions > repeat_char_length
      && print_char->ch[0] != L'\0' && print_char->ch[1] == L'\0';
}

//----------------------------------------------------------------------
inline auto FTermOutput::getRepetitionType ( const FChar* print_char
                                           , uInt repetitions ) const -> Repetition
{
  if ( canUseCharacterRepetitions(print_char, repetitions) )
  {
    if ( is7bit(print_char->ch[0]) )
      return Repetition::ASCII;

    if ( isPrintable(print_char->ch[0]) )
      return Repetition::UTF8;
  }

  return Repetition::NotOptimized;
}

//----------------------------------------------------------------------
inline auto FTermOutput::isFullWidthChar (const FChar& ch) const -> bool
{
  return ch.attr.bit.char_width == 2;
}

//----------------------------------------------------------------------
inline auto FTermOutput::isFullWidthPaddingChar (const FChar& ch) const -> bool
{
  return ch.attr.bit.fullwidth_padding;
}

//----------------------------------------------------------------------
void FTermOutput::cursorWrap() const
{
  // Wrap the cursor

  if ( term_pos->getX() < vterm->size.width )
    return;

  if ( term_pos->getY() == vterm->size.height - 1 )
  {
    term_pos->x_ref()--;
  }
  else if ( FTermcap::eat_nl_glitch )
  {
    term_pos->setPoint(-1, -1);
  }
  else if ( FTermcap::automatic_right_margin )
  {
    term_pos->setX(0);
    term_pos->y_ref()++;
  }
  else
    term_pos->x_ref()--;
}

//----------------------------------------------------------------------
inline void FTermOutput::adjustCursorPosition (FPoint& p) const
{
  const auto term_width = int(getColumnNumber());
  const auto term_height = int(getLineNumber());
  auto& x = p.x_ref();
  auto& y = p.y_ref();

  if ( x >= term_width && term_width > 0 )
  {
    y += x / term_width;
    x %= term_width;
  }

  if ( term_pos->getY() >= term_height )
    term_pos->setY(term_height - 1);

  if ( y >= term_height )
    y = term_height - 1;
}

//----------------------------------------------------------------------
inline auto FTermOutput::updateTerminalLine (uInt y) -> bool
{
  // Updates pending changes from line y to the terminal

  auto& vterm_changes = vterm->changes[y];
  uInt& xmin = vterm_changes.xmin;
  uInt& xmax = vterm_changes.xmax;

  if ( xmin > xmax )  // This line has no changes
  {
    cursorWrap();
    return false;
  }

  // Clear rest of line
  if ( canClearToEOL (xmin, y) )
  {
    setCursor (FPoint{int(xmin), int(y)});
    auto& min_char = vterm->getFChar(int(xmin), int(y));
    appendAttributes (min_char);
    appendOutputBuffer (FTermControl{TCAP(t_clr_eol)});
    markAsPrinted (xmin, uInt(vterm->size.width - 1), y);
  }
  else
  {
    bool draw_leading_ws = canClearLeadingWS (xmin, y);  // leading whitespace
    bool draw_trailing_ws = canClearTrailingWS (xmax, y);  // trailing whitespace
    setCursor (FPoint{int(xmin), int(y)});

    if ( draw_leading_ws )
    {
      auto& first_char = vterm->getFChar(int(0), int(y));
      appendAttributes (first_char);
      appendOutputBuffer (FTermControl{TCAP(t_clr_bol)});
      markAsPrinted (0, xmin, y);
    }

    printRange (xmin, xmax, y);

    if ( draw_trailing_ws )
    {
      auto& last_char = vterm->getFChar(vterm->size.width - 1, int(y));
      appendAttributes (last_char);
      appendOutputBuffer (FTermControl{TCAP(t_clr_eol)});
      markAsPrinted (xmax + 1, uInt(vterm->size.width - 1), y);
    }
  }

  // Reset line changes and wrap the cursor
  xmin = uInt(vterm->size.width);
  xmax = 0;
  cursorWrap();
  return true;
}

//----------------------------------------------------------------------
auto FTermOutput::updateTerminalCursor() -> bool
{
  // Updates the input cursor visibility and the position

  if ( isInputCursorInsideTerminal() )
  {
    setCursor (FPoint{vterm->input_cursor.x, vterm->input_cursor.y});
    showCursor();
    return true;
  }

  hideCursor();
  return false;
}

//----------------------------------------------------------------------
inline void FTermOutput::flushTimeAdjustment()
{
  const auto now = FObjectTimer::getCurrentTime();
  const auto diff = now - time_last_flush;

  if ( diff > milliseconds(400) )
  {
    flush_wait = MIN_FLUSH_WAIT;  // Reset to minimum values after 400 ms
    flush_average = MIN_FLUSH_WAIT;
    flush_median = MIN_FLUSH_WAIT;
  }
  else
  {
    auto usec = uInt64(duration_cast<microseconds>(diff).count());
    usec = std::min(std::max(usec, MIN_FLUSH_WAIT), MAX_FLUSH_WAIT);

    if ( usec >= flush_average )
      flush_average += (usec - flush_average) / 10;
    else
    {
      uInt64 delta = (flush_average - usec) / 10;

      if ( flush_average >= delta )  // Avoid uInt64 underflow
        flush_average -= delta;
    }

    if ( usec >= flush_median )
      flush_median += flush_average / 5;
    else
    {
      uInt64 delta = flush_average / 5;

      if ( flush_median >= delta )  // Avoid uInt64 underflow
        flush_median -= delta;
    }

    flush_wait = flush_median;
  }
}

//----------------------------------------------------------------------
inline void FTermOutput::markAsPrinted (uInt x, uInt y) const
{
  // Marks a character as printed

  vterm->getFChar(int(x), int(y)).attr.bit.printed = true;
}

//----------------------------------------------------------------------
inline void FTermOutput::markAsPrinted (uInt from, uInt to, uInt y) const
{
  // Marks characters in the specified range [from .. to] as printed

  auto* ch = &vterm->getFChar(int(from), int(y));
  const auto* end = ch + to - from + 1;

  // Unroll the loop for better performance
  while (ch + 4 <= end)
  {
    ch[0].attr.bit.printed = true;
    ch[1].attr.bit.printed = true;
    ch[2].attr.bit.printed = true;
    ch[3].attr.bit.printed = true;
    ch += 4;
  }

  // Handle the remaining elements
  while ( ch < end )
  {
    ch->attr.bit.printed = true;
    ++ch;
  }
}

//----------------------------------------------------------------------
inline void FTermOutput::newFontChanges (FChar& next_char) const
{
  // NewFont special cases
  if ( ! FTerm::isNewFont() )
    return;

  if ( next_char.ch[0] == UniChar::LowerHalfBlock )
  {
    next_char.ch[0] = wchar_t(UniChar::UpperHalfBlock);
    next_char.attr.bit.reverse = true;
  }
  else if ( isReverseNewFontchar(next_char.ch[0]) )
    next_char.attr.bit.reverse = true;  // Show in reverse video
}

//----------------------------------------------------------------------
inline void FTermOutput::charsetChanges (FChar& next_char) const
{
  auto iter_enc_ch = next_char.encoded_char.begin();
  auto iter_ch = next_char.ch.cbegin();
  auto end_ch = next_char.ch.cend();

  while ( iter_ch < end_ch && *iter_ch != L'\0' )
  {
    *iter_enc_ch = *iter_ch;
    ++iter_ch;
    ++iter_enc_ch;
  }

  const auto terminal_encoding = internal::var::terminal_encoding;

  if ( terminal_encoding == Encoding::UTF8 )
    return;

  const auto& ch = next_char.ch[0];
  const auto& ch_enc = FTerm::charEncode(ch);

  if ( ch_enc == ch )
    return;

  auto& first_enc_char = next_char.encoded_char[0];

  if ( ch_enc == 0 )
  {
    first_enc_char = wchar_t(FTerm::charEncode(ch, Encoding::ASCII));
    return;
  }

  first_enc_char = ch_enc;

  if ( terminal_encoding == Encoding::VT100 )
    next_char.attr.bit.alt_charset = true;
  else if ( terminal_encoding == Encoding::PC )
  {
    next_char.attr.bit.pc_charset = true;
    const auto is_putty = fterm_data->isTermType(FTermType::putty);
    const auto is_xterm = fterm_data->isTermType(FTermType::xterm);

    if ( ! is_putty && is_xterm && ch_enc < 0x20 )  // Character 0x00..0x1f
    {
      if ( FTerm::hasUTF8() )
        first_enc_char = int(FTerm::charEncode(ch, Encoding::ASCII));
      else
      {
        first_enc_char += 0x5f;
        next_char.attr.bit.alt_charset = true;
      }
    }
  }
}

//----------------------------------------------------------------------
inline void FTermOutput::appendCharacter (FChar& next_char)
{
  const int term_width = vterm->size.width - 1;
  const int term_height = vterm->size.height - 1;

  if ( term_pos->getX() == term_width
    && term_pos->getY() == term_height )
    appendLowerRight (next_char);
  else
    appendChar (next_char);

  term_pos->x_ref()++;
}

//----------------------------------------------------------------------
inline void FTermOutput::appendCharacter_n (FChar& next_char, uInt number)
{
  for (uInt n{0}; n < number; n++)
    appendCharacter (next_char);
}

//----------------------------------------------------------------------
inline void FTermOutput::appendChar (FChar& next_char)
{
  newFontChanges (next_char);
  charsetChanges (next_char);
  appendAttributes (next_char);
  characterFilter (next_char);

  for (const auto& ch : next_char.encoded_char)
  {
    if ( ch != L'\0')
    {
      if ( internal::var::terminal_encoding == Encoding::UTF8 )
        appendOutputBuffer (unicode_to_utf8(ch));
      else
        appendOutputBuffer (std::string(1, char(uChar(ch))));
    }

    if ( ! combined_char_support )
      return;
  }
}

//----------------------------------------------------------------------
inline void FTermOutput::appendAttributes (FChar& next_attr)
{
  // generate attribute string for the next character
  static auto& opti_attr = FOptiAttr::getInstance();
  const auto& attr_str = opti_attr.changeAttribute (term_attribute, next_attr);

  if ( ! attr_str.empty() )
    appendOutputBuffer (FTermControl{attr_str});
}

//----------------------------------------------------------------------
void FTermOutput::appendLowerRight (FChar& last_char)
{
  const auto& SA = TCAP(t_enter_am_mode);
  const auto& RA = TCAP(t_exit_am_mode);

  if ( ! FTermcap::automatic_right_margin )
  {
    appendChar (last_char);
  }
  else if ( SA && RA )
  {
    appendOutputBuffer (FTermControl{RA});
    appendChar (last_char);
    appendOutputBuffer (FTermControl{SA});
  }
  else
  {
    const auto& IC = TCAP(t_parm_ich);
    const auto& im = TCAP(t_enter_insert_mode);
    const auto& ei = TCAP(t_exit_insert_mode);
    const auto& ip = TCAP(t_insert_padding);
    const auto& ic = TCAP(t_insert_character);

    const int x = int(getColumnNumber()) - 2;
    const int y = int(getLineNumber()) - 1;
    setCursor (FPoint{x, y});
    appendChar (last_char);
    term_pos->x_ref()++;

    setCursor (FPoint{x, y});
    FChar& second_last = *(&last_char - 1);

    if ( IC )
    {
      appendOutputBuffer (FTermControl{FTermcap::encodeParameter(IC, 1)});
      appendChar (second_last);
    }
    else if ( im && ei )
    {
      appendOutputBuffer (FTermControl{im});
      appendChar (second_last);

      if ( ip )
        appendOutputBuffer (FTermControl{ip});

      appendOutputBuffer (FTermControl{ei});
    }
    else if ( ic )
    {
      appendOutputBuffer (FTermControl{ic});
      appendChar (second_last);

      if ( ip )
        appendOutputBuffer (FTermControl{ip});
    }
  }
}

//----------------------------------------------------------------------
inline void FTermOutput::characterFilter (FChar& next_char)
{
  static const auto& sub_map = getFTerm().getCharSubstitutionMap();

  if ( sub_map.isEmpty() )
    return;

  auto& first_enc_char = next_char.encoded_char[0];
  const auto& entry = sub_map.getMappedChar(first_enc_char);

  if ( entry )
    first_enc_char = entry;
}

//----------------------------------------------------------------------
inline auto FTermOutput::moveCursorLeft() -> CursorMoved
{
  // Move cursor one character to the left
  const auto& le = TCAP(t_cursor_left);
  const auto& LE = TCAP(t_parm_left_cursor);

  if ( le )
    appendOutputBuffer (FTermControl{le});
  else if ( LE )
    appendOutputBuffer (FTermControl{FTermcap::encodeParameter(LE, 1)});
  else
    return CursorMoved::No;  // Cursor could not be moved

  return CursorMoved::Yes;  // Cursor has moved
}

//----------------------------------------------------------------------
inline void FTermOutput::checkFreeBufferSize()
{
  if ( output_buffer->isFull() )
    flush();
}

//----------------------------------------------------------------------
inline void FTermOutput::appendOutputBuffer (const FTermControl& ctrl)
{
  output_buffer->emplace(OutputType::Control, ctrl.string);
  checkFreeBufferSize();
}

//----------------------------------------------------------------------
inline void FTermOutput::appendOutputBuffer (const UniChar& ch)
{
  appendOutputBuffer(unicode_to_utf8(wchar_t(ch)));
}

//----------------------------------------------------------------------
void FTermOutput::appendOutputBuffer (std::string&& string)
{
  auto& last = output_buffer->back();

  if ( ! output_buffer->isEmpty() && last.type == OutputType::String )
  {
    // Append string data to the back element
    auto& string_buf = last.data;
    string_buf.append(string);
  }
  else
  {
    output_buffer->emplace(OutputType::String, std::move(string));
    checkFreeBufferSize();
  }
}

}  // namespace finalcut
