/***********************************************************************
* ftermoutput.cpp - Implements the terminal output                     *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2021-2026 Markus Gans                                      *
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

template <typename T>
constexpr auto clampValue (T value, T minVal, T maxVal) -> T
{
  if ( value < minVal )
    return minVal;

  return ( value > maxVal ) ? maxVal : value;
}

template <typename T>
constexpr auto adjustValue (T current, T reference, T delta, T minLimit) -> T
{
  if ( current > reference )
    return reference + delta;

  if ( reference >= delta )
    return reference - delta;
  else
    return minLimit;  // Avoid underflow
}

constexpr auto getNoChangesMask() -> uInt32
{
  FCharAttribute mask{};
  mask.no_changes = true;
  return FCharAttribute_to_uInt32(mask);
}

constexpr auto getPrintedMask() -> uInt32
{
  FCharAttribute mask{};
  mask.printed = true;
  return FCharAttribute_to_uInt32(mask);
}

struct var
{
  static Encoding terminal_encoding;
  static constexpr auto no_changes_mask = getNoChangesMask();
  static constexpr auto printed_mask = getPrintedMask();
};

Encoding var::terminal_encoding{Encoding::Unknown};
constexpr uInt32 var::no_changes_mask;
constexpr uInt32 var::printed_mask;

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
auto FTermOutput::isFlushTimeout() const noexcept -> bool
{
  const auto now_us = uInt64(duration_cast<microseconds>( clock::now()
                                                         .time_since_epoch()).count() );
  if ( now_us < time_last_flush_us )
    return false;

  const auto diff_us = now_us - time_last_flush_us;
  return diff_us > flush_wait;
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
  output_buffer = std::make_shared<FOutputBuffer>();
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
  time_last_flush_us = 0;
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

  int changedlines{0};
  const auto first_row = vterm->changes_in_row.ymin;
  const auto last_row  = vterm->changes_in_row.ymax;

  for (uInt y{first_row}; y <= last_row; y++)
  {
    FVTerm::reduceTerminalLineUpdates(y);

    if ( updateTerminalLine(y) )
      changedlines++;
  }

  vterm->changes_in_row = {uInt(vterm->size.height), 0};  // Reset row changes
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
  term_attribute.ch = {L'\0', L'\0', L'\0', L'\0', L'\0'};
  term_attribute.color.pair = {FColor::Undefined, FColor::Undefined};
  term_attribute.attr.data = 0;
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

  const auto& data_ref = output_buffer->data;
  std::size_t offset = 0;  // The read position in the string

  while ( ! output_buffer->isEmpty() )
  {
    const auto& first = output_buffer->slices.front();
    const auto& type = first.type;
    const auto& length = first.length;

    if ( type == FOutputBuffer::OutputType::String )
      FTerm::stringPrint (&data_ref[offset], length);
    else if ( type == FOutputBuffer::OutputType::Control )
      FTerm::paddingPrint (&data_ref[offset], length);

    offset += length;
    output_buffer->slices.pop();
  }

  output_buffer->data.clear();
  std::fflush(stdout);
  static auto& mouse = FMouseControl::getInstance();
  mouse.drawPointer();
  time_last_flush_us = uInt64(duration_cast<microseconds>( clock::now()
                                                          .time_since_epoch()).count() );
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
inline auto FTermOutput::isInputCursorInsideTerminal() const noexcept -> bool
{
  if ( ! vterm || ! vterm->input_cursor_visible )
    return false;

  const int x = vterm->input_cursor.x;
  const int y = vterm->input_cursor.y;
  return ( x >= 0 && x < int(getColumnNumber())
        && y >= 0 && y < int(getLineNumber()) );
}

//----------------------------------------------------------------------
inline auto FTermOutput::isDefaultPaletteTheme() const -> bool
{
  const FStringList default_themes
  {
    "default8ColorPalette",
    "default16ColorPalette",
    "default16DarkColorPalette"
  };

  const auto iter = std::find ( default_themes.cbegin()
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

  if ( ! ce )
    return false;

  const auto width = uInt(vterm->size.width);
  const auto* row_begin = &vterm->data[y * width];
  const auto* row_end = std::next(row_begin, width);
  const auto* min_char = std::next(row_begin, xmin);

  if ( min_char->ch.unicode_data[0] != L' ' )
    return false;

  auto all_same = std::all_of ( min_char
                              , row_end
                              , [min_char] (const FChar& ch)
                                {
                                  return *min_char != ch;
                                } );
  if ( ! all_same )
    return false;

  const auto remaining_width = width - xmin;
  const auto normal = FOptiAttr::isNormal(*min_char);
  const auto ut = FTermcap::background_color_erase;

  return ( (ut || normal) && clr_eol_length < remaining_width );
}

//----------------------------------------------------------------------
auto FTermOutput::canClearLeadingWS (uInt& xmin, uInt y) const -> bool
{
  // Line has leading whitespace
  // => clear from xmin to beginning of line

  const auto& cb = TCAP(t_clr_bol);

  if ( ! cb )
    return false;

  const auto width = uInt(vterm->size.width);
  const auto* row_begin = &vterm->data[y * width];
  const auto* row_end = std::next(row_begin, width);
  const auto& first_char = *row_begin;

  if ( first_char.ch.unicode_data[0] != L' ' )
    return false;

  auto first_diff = std::find_if ( std::next(row_begin)
                                 , row_end
                                 , [&first_char] (const FChar& ch)
                                   {
                                     return first_char != ch;
                                   } );

  auto leading_whitespace = uInt(std::distance(row_begin, first_diff));
  const auto normal = FOptiAttr::isNormal(first_char);
  const auto ut = FTermcap::background_color_erase;

  if ( leading_whitespace > xmin && (ut || normal)
    && clr_bol_length < leading_whitespace )
  {
    xmin = leading_whitespace - 1;  // Set new xmin value
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

  if ( ! ce )
    return false;

  const int width = vterm->size.width;
  const auto* row_begin = &vterm->data[y * unsigned(width)];
  const auto* row_end = std::next(row_begin, width);
  const auto* last_char = std::prev(row_end);

  if ( last_char->ch.unicode_data[0] != L' ' )
    return false;

  auto r_begin = std::make_reverse_iterator(row_end);
  auto r_end = std::make_reverse_iterator(row_begin);

  auto first_diff = std::find_if ( std::next(r_begin)
                                 , r_end
                                 , [last_char] (const FChar& ch)
                                   {
                                     return *last_char != ch;
                                   } );

  const auto trailing_whitespace = uInt(std::distance(r_begin, first_diff));
  const auto normal = FOptiAttr::isNormal(*last_char);
  const auto ut = FTermcap::background_color_erase;

  if ( trailing_whitespace > uInt(width) - xmax && (ut || normal)
    && clr_eol_length < trailing_whitespace )
  {
    xmax = uInt(width) - trailing_whitespace;  // Set new xmax value
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
auto FTermOutput::skipUnchangedCharacters ( uInt& x, uInt xmax, uInt y
                                          , FChar_iterator iter ) -> bool
{
  // Skip characters without changes if it is faster than redrawing

  constexpr auto mask = internal::var::no_changes_mask;

  if ( ! (iter->attr.data & mask) )
    return false;

  auto ch = iter + 1;  // Skip first unchanged character
  const auto end = ch + xmax - x;

  // Unroll the loop for better performance
  while ( ch + 4 <= end
       && ch[0].attr.data & mask && ch[1].attr.data & mask
       && ch[2].attr.data & mask && ch[3].attr.data & mask )
  {
    ch += 4;
  }

  // Handle the remaining elements
  while ( ch < end && ch->attr.data & mask )
  {
    ++ch;
  }

  auto count = uInt(ch - iter);  // Number of unchanged characters

  if ( count > cursor_address_length )
  {
    x += count;  // Add unchanged number of characters to the pointer
    setCursor (FPoint{int(x), int(y)});
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
void FTermOutput::printRange (uInt xmin, uInt xmax, uInt y)
{
  const auto& ec = TCAP(t_erase_chars);
  const auto& rp = TCAP(t_repeat_char);
  const auto& lr = TCAP(t_repeat_last_char);
  const auto vterm_width = uInt(vterm->size.width - 1);
  auto iter = vterm->getFCharIterator(int(xmin), int(y));
  uInt x = xmin;
  uInt x_last = xmin;

  while ( x <= xmax )
  {
    // Update pointer and mark character as printed
    iter += (x - x_last);
    iter->attr.bit.printed = true;
    x_last = x;

    // Handle non-printable full-width characters on terminal margins
    replaceNonPrintableFullwidth (x, vterm_width, *iter);

    // Skip unchanged characters
    if ( skipUnchangedCharacters(x, xmax, y, iter) )
      continue;

    // Erase character
    if ( ec && iter->ch.unicode_data[0] == L' ' )
    {
      if ( eraseCharacters(x, xmax, y, iter) \
           == PrintState::LineCompletelyPrinted )
        break;
    }
    else if ( rp || lr )  // Repeat one character n-fold
    {
      repeatCharacter(x, xmax, y, iter);
    }
    else  // General character output
    {
      bool min_and_not_max{ x == xmin && xmin != xmax };
      printCharacter (x, y, min_and_not_max, iter);
    }

    ++x;
  }
}

//----------------------------------------------------------------------
inline void FTermOutput::replaceNonPrintableFullwidth ( uInt x, uInt vterm_width
                                                      , FChar& print_char ) const noexcept
{
  // Replace non-printable full-width characters that are truncated
  // from the right or left terminal side

  if ( x == 0 && isFullWidthPaddingChar(print_char) )
  {
    print_char.ch[0] = wchar_t(UniChar::SingleLeftAngleQuotationMark);  // ‹
    print_char.ch[1] = L'\0';
    print_char.attr.bit.fullwidth_padding = false;
  }
  else if ( x == vterm_width && isFullWidthChar(print_char) )
  {
    print_char.ch[0] = wchar_t(UniChar::SingleRightAngleQuotationMark);  // ›
    print_char.ch[1] = L'\0';
    print_char.attr.bit.char_width = 1;
  }
}

//----------------------------------------------------------------------
void FTermOutput::printCharacter ( uInt& x, uInt y, bool min_and_not_max
                                 , FChar_iterator& iter)
{
  // General character output on terminal

  if ( x < uInt(vterm->size.width - 1) && isFullWidthChar(*iter) )
  {
    printFullWidthCharacter (x, y, iter);
  }
  else if ( x > 0 && x < uInt(vterm->size.width - 1)
         && isFullWidthPaddingChar(*iter)  )
  {
    printFullWidthPaddingCharacter (x, y, iter);
  }
  else if ( x > 0 && min_and_not_max )
  {
    printHalfCovertFullWidthCharacter (x, y, iter);
  }
  else
  {
    // Print a half-width character
    appendCharacter (iter);
    markAsPrinted (x, y);
  }
}

//----------------------------------------------------------------------
void FTermOutput::printFullWidthCharacter ( uInt& x, uInt y
                                          , FChar_iterator& iter )
{
  auto& next_char = vterm->getFChar(int(x + 1), int(y));

  if ( iter->attr.byte[0] == next_char.attr.byte[0]
    && iter->attr.byte[1] == next_char.attr.byte[1]
    && iter->color.data == next_char.color.data
    && isFullWidthChar(*iter)
    && isFullWidthPaddingChar(next_char) )
  {
    // Print a full-width character
    appendCharacter (iter);
    markAsPrinted (x, y);
    skipPaddingCharacter (x, y, *iter);
  }
  else
  {
    // Print ellipses for the 1st full-width character column
    printEllipsis (x, y, *iter);

    if ( ! isFullWidthPaddingChar(next_char) )
      return;

    // Print ellipses for the 2nd full-width character column
    x++;
    printEllipsis (x, y, next_char);
  }
}

//----------------------------------------------------------------------
void FTermOutput::printFullWidthPaddingCharacter ( uInt& x, uInt y
                                                 , const FChar_iterator& iter)
{
  auto prev_char_iter = vterm->getFCharIterator(int(x - 1), int(y));

  if ( iter->attr.byte[0] == prev_char_iter->attr.byte[0]
    && iter->attr.byte[1] == prev_char_iter->attr.byte[1]
    && iter->color.data == prev_char_iter->color.data
    && isFullWidthChar(*prev_char_iter)
    && isFullWidthPaddingChar(*iter) )
  {
    if ( moveCursorLeft() == CursorMoved::No )
    {
      skipPaddingCharacter (x, y, *prev_char_iter);
      return;
    }

    // Print a full-width character
    x--;
    --(term_pos->x_ref());
    appendCharacter (prev_char_iter);
    markAsPrinted (x, y);
    skipPaddingCharacter (x, y, *prev_char_iter);
  }
  else
  {
    // Print ellipses for the 1st full-width character column
    printEllipsis (x, y, *iter);
  }
}

//----------------------------------------------------------------------
void FTermOutput::printHalfCovertFullWidthCharacter ( uInt x, uInt y
                                                    , FChar_iterator& iter )
{
  auto& prev_char = vterm->getFChar(int(x - 1), int(y));

  if ( isFullWidthChar(prev_char)
    && ! isFullWidthPaddingChar(*iter)
    && moveCursorLeft() == CursorMoved::Yes )
  {
    // Print ellipses for the 1st full-width character column
    --(term_pos->x_ref());
    printEllipsis (x - 1, y, prev_char);
  }

  // Print a half-width character
  appendCharacter (iter);
  markAsPrinted (x, y);
}

//----------------------------------------------------------------------
inline void FTermOutput::printEllipsis (uInt x, uInt y, FChar& fchar)
{
  // Printing ellipses with color and attributes from fchar
  appendAttributes (fchar);
  appendOutputBuffer (UniChar::HorizontalEllipsis);  // …
  ++(term_pos->x_ref());
  markAsPrinted (x, y);
}

//----------------------------------------------------------------------
inline void FTermOutput::skipPaddingCharacter ( uInt& x, uInt y
                                              , const FChar& print_char ) const noexcept
{
  if ( isFullWidthChar(print_char) )  // full-width character
  {
    x++;  // Skip the following padding character
    ++(term_pos->x_ref());
    markAsPrinted (x, y);
  }
}

//----------------------------------------------------------------------
auto FTermOutput::eraseCharacters (uInt& x, uInt xmax, uInt y, FChar_iterator& iter) -> PrintState
{
  // Erase a number of characters to draw simple whitespaces

  const auto& ec = TCAP(t_erase_chars);

  if ( ! (ec && iter->ch.unicode_data[0] == L' ') )
    return PrintState::NothingPrinted;

  const auto whitespace = countRepetitions(iter, x, xmax);

  if ( whitespace == 1 )
  {
    appendCharacter (iter);
    markAsPrinted (x, y);
    return PrintState::WhitespacesPrinted;
  }

  const uInt end_pos = x + whitespace - 1;

  if ( canUseEraseCharacters(*iter, whitespace) )
  {
    appendAttributes (*iter);
    appendOutputBuffer (FTermControl{FTermcap::encodeParameter(ec, whitespace)});

    if ( end_pos <= xmax )
      setCursor (FPoint{static_cast<int>(x + whitespace), static_cast<int>(y)});
    else
      return PrintState::LineCompletelyPrinted;
  }
  else
    appendCharacter_n (iter, whitespace);

  markAsPrinted (x, end_pos, y);
  x = end_pos;
  return PrintState::WhitespacesPrinted;
}

//----------------------------------------------------------------------
auto FTermOutput::repeatCharacter (uInt& x, uInt xmax, uInt y, FChar_iterator& iter) -> PrintState
{
  // Repeat one character n-fold

  const auto& rp = TCAP(t_repeat_char);
  const auto& lr = TCAP(t_repeat_last_char);

  if ( ! (rp || lr) )
    return PrintState::NothingPrinted;

  const auto repetitions = countRepetitions(iter, x, xmax);

  if ( repetitions == 1 )
  {
    printCharacter (x, y, x != xmax, iter);
    return PrintState::RepeatCharacterPrinted;
  }

  // Every full-width character is always followed by a padding
  // character, so that two or more consecutive full-width characters
  // cannot repeat in their byte sequence
  const auto repetition_type = getRepetitionType(*iter, repetitions);

  if ( rp && repetition_type == Repetition::ASCII )
  {
    newFontChanges (*iter);
    charsetChanges (*iter);
    appendAttributes (*iter);
    appendOutputBuffer (FTermControl{FTermcap::encodeParameter(rp, iter->ch.unicode_data[0], repetitions)});
    term_pos->x_ref() += static_cast<int>(repetitions);
  }
  else if ( lr && repetition_type == Repetition::UTF8 )
  {
    appendChar (*iter);
    appendOutputBuffer (FTermControl{FTermcap::encodeParameter(lr, repetitions)});
    term_pos->x_ref() += static_cast<int>(repetitions);
  }
  else
    appendCharacter_n (iter, repetitions);

  const uInt end_pos = x + repetitions - 1;
  markAsPrinted (x, end_pos, y);
  x = end_pos;
  return PrintState::RepeatCharacterPrinted;
}

//----------------------------------------------------------------------
inline auto FTermOutput::countRepetitions ( FChar_const_iterator iter
                                          , uInt from, uInt to ) const noexcept -> uInt
{
  if ( to < from )
    return 0;

  const FChar first = iter[0];
  const uInt end = (to - from) + 1;
  uInt index = 1;

  for (; index < end; ++index)
  {
    if ( iter[index] != first )
      break;
  }

  return index;
}

//----------------------------------------------------------------------
inline auto FTermOutput::canUseEraseCharacters ( const FChar& print_char
                                               , uInt whitespace ) const noexcept -> bool
{
  const auto normal = FOptiAttr::isNormal(print_char);
  const auto ut = FTermcap::background_color_erase;
  const uInt total_length_required = erase_char_length
                                   + cursor_address_length;

  return whitespace > total_length_required && (ut || normal);
}

//----------------------------------------------------------------------
inline auto FTermOutput::canUseCharacterRepetitions ( const FChar& print_char
                                                    , uInt repetitions ) const noexcept -> bool
{
  return repetitions > repeat_char_length
      && print_char.ch[0] != L'\0'
      && print_char.ch[1] == L'\0';
}

//----------------------------------------------------------------------
inline auto FTermOutput::getRepetitionType ( const FChar& print_char
                                           , uInt repetitions ) const noexcept -> Repetition
{
  if ( canUseCharacterRepetitions(print_char, repetitions) )
  {
    if ( is7bit(print_char.ch.unicode_data[0]) )
      return Repetition::ASCII;

    if ( isPrintable(print_char.ch.unicode_data[0]) )
      return Repetition::UTF8;
  }

  return Repetition::NotOptimized;
}

//----------------------------------------------------------------------
inline auto FTermOutput::isFullWidthChar (const FChar& ch) const noexcept -> bool
{
  return ch.attr.bit.char_width == 2;
}

//----------------------------------------------------------------------
inline auto FTermOutput::isFullWidthPaddingChar (const FChar& ch) const noexcept -> bool
{
  return ch.attr.bit.fullwidth_padding;
}

//----------------------------------------------------------------------
void FTermOutput::cursorWrap() const noexcept
{
  // Wrap the cursor

  if ( term_pos->getX() < vterm->size.width )
    return;

  if ( term_pos->getY() == vterm->size.height - 1 )
  {
    --(term_pos->x_ref());  // Prevent wrapping at the bottom-right
    return;
  }

  if ( FTermcap::eat_nl_glitch )
  {
    term_pos->setPoint(-1, -1);  // Reset position
    return;
  }

  if ( FTermcap::automatic_right_margin )
  {
    term_pos->setX(0);  // Wrap to the next line
    ++(term_pos->y_ref());
    return;
  }

  --(term_pos->x_ref());  // Default case: Move cursor left
}

//----------------------------------------------------------------------
inline void FTermOutput::adjustCursorPosition (FPoint& p) const noexcept
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

  auto& vterm_changes = vterm->changes_in_line[y];
  uInt& xmin = vterm_changes.xmin;
  uInt& xmax = vterm_changes.xmax;

  if ( xmin > xmax )  // This line has no changes
    return false;

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
inline void FTermOutput::flushTimeAdjustment() noexcept
{
  const auto now_us = uInt64(duration_cast<microseconds>( clock::now()
                                                         .time_since_epoch()).count() );
  const auto diff_us = now_us - time_last_flush_us;

  if ( diff_us > RESET_THRESHOLD )
  {
    flush_wait    = MIN_FLUSH_WAIT;  // Reset to minimum values after 400 ms
    flush_average = MIN_FLUSH_WAIT;
    flush_median  = MIN_FLUSH_WAIT;
    return;
  }

  const uInt64 us = internal::clampValue(diff_us, MIN_FLUSH_WAIT, MAX_FLUSH_WAIT);
  const uInt64 avg = flush_average;
  const uInt64 delta_avg = ( us > avg ) ? (us - avg) / 10
                                        : (avg - us) / 10;
  flush_average = internal::adjustValue (us, avg, delta_avg, MIN_FLUSH_WAIT);
  const uInt64 med  = flush_median;
  const uInt64 step = flush_average / 5;
  flush_median  = internal::adjustValue (us, med, step, MIN_FLUSH_WAIT);
  flush_wait = flush_median;
}

//----------------------------------------------------------------------
inline void FTermOutput::markAsPrinted (uInt x, uInt y) const noexcept
{
  // Marks a character as printed

  vterm->getFChar(int(x), int(y)).attr.data |= internal::var::printed_mask;
}

//----------------------------------------------------------------------
inline void FTermOutput::markAsPrinted (uInt from, uInt to, uInt y) const noexcept
{
  // Marks characters in the specified range [from .. to] as printed

  auto iter = vterm->getFCharIterator(int(from), int(y));
  const auto end = iter + to - from + 1;

  // Unroll the loop for better performance
  while ( iter + 4 <= end )
  {
    iter[0].attr.data |= internal::var::printed_mask;
    iter[1].attr.data |= internal::var::printed_mask;
    iter[2].attr.data |= internal::var::printed_mask;
    iter[3].attr.data |= internal::var::printed_mask;
    iter += 4;
  }

  // Handle the remaining elements
  while ( iter < end )
  {
    iter->attr.data |= internal::var::printed_mask;
    ++iter;
  }
}

//----------------------------------------------------------------------
inline void FTermOutput::newFontChanges (FChar& next_char) const
{
  // NewFont special cases
  if ( ! FTerm::isNewFont() )
    return;

  if ( next_char.ch.unicode_data[0] == UniChar::LowerHalfBlock )
  {
    next_char.ch.unicode_data[0] = wchar_t(UniChar::UpperHalfBlock);
    next_char.attr.bit.reverse = true;
  }
  else if ( isReverseNewFontchar(next_char.ch.unicode_data[0]) )
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

  const auto& ch = next_char.ch.unicode_data[0];
  const auto& ch_enc = FTerm::charEncode(ch);

  if ( ch_enc == ch )
    return;

  auto& first_enc_char = next_char.encoded_char.unicode_data[0];

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
inline void FTermOutput::appendCharacter (FChar_iterator& next_char_iter)
{
  const int term_width = vterm->size.width - 1;
  const int term_height = vterm->size.height - 1;

  if ( term_pos->getX() == term_width
    && term_pos->getY() == term_height )
    appendLowerRight (next_char_iter);
  else
    appendChar (*next_char_iter);

  ++(term_pos->x_ref());
}

//----------------------------------------------------------------------
inline void FTermOutput::appendCharacter_n (FChar_iterator& next_char_iter, uInt number)
{
  for (uInt n{0}; n < number; n++)
    appendCharacter (next_char_iter);
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
        appendOutputBuffer (UTF8_Char({{char(uChar(ch)), '\0', '\0','\0'}, 1}));
    }

    if ( ! combined_char_support )
      return;
  }
}

//----------------------------------------------------------------------
inline void FTermOutput::appendAttributes (FChar& next_attr)
{
  // generate attribute string for the next character

  if ( term_attribute.color.data == next_attr.color.data
    && (term_attribute.attr.data & 0xffff) == (next_attr.attr.data & 0xffff) )
    return;  // No changes

  static auto& opti_attr = FOptiAttr::getInstance();
  const auto attr_str = opti_attr.changeAttribute (term_attribute, next_attr);

  if ( attr_str.empty() )
    return;

  appendOutputBuffer (FTermControl{attr_str});
}

//----------------------------------------------------------------------
void FTermOutput::appendLowerRight (const FChar_iterator& last_char_iter)
{
  const auto& SA = TCAP(t_enter_am_mode);
  const auto& RA = TCAP(t_exit_am_mode);

  if ( ! FTermcap::automatic_right_margin )
  {
    appendChar (*last_char_iter);
  }
  else if ( SA && RA )
  {
    appendOutputBuffer (FTermControl{RA});
    appendChar (*last_char_iter);
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
    appendChar (*last_char_iter);
    ++(term_pos->x_ref());

    setCursor (FPoint{x, y});
    auto second_last = last_char_iter - 1;

    if ( IC )
    {
      appendOutputBuffer (FTermControl{FTermcap::encodeParameter(IC, 1)});
      appendChar (*second_last);
    }
    else if ( im && ei )
    {
      appendOutputBuffer (FTermControl{im});
      appendChar (*second_last);

      if ( ip )
        appendOutputBuffer (FTermControl{ip});

      appendOutputBuffer (FTermControl{ei});
    }
    else if ( ic )
    {
      appendOutputBuffer (FTermControl{ic});
      appendChar (*second_last);

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

  auto& first_enc_char = next_char.encoded_char.unicode_data[0];
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
  if ( output_buffer->slices.isFull() )
    flush();
}

//----------------------------------------------------------------------
inline void FTermOutput::appendOutputBuffer (const FTermControl& ctrl)
{
  appendOutputBuffer ( FOutputBuffer::OutputType::Control
                     , ctrl.string.c_str()
                     , uInt32(ctrl.string.length()) );
}

//----------------------------------------------------------------------
inline void FTermOutput::appendOutputBuffer (const UniChar& ch)
{
  appendOutputBuffer(unicode_to_utf8(wchar_t(ch)));
}

//----------------------------------------------------------------------
inline void FTermOutput::appendOutputBuffer (const UTF8_Char& utf8_char)
{
  appendOutputBuffer ( FOutputBuffer::OutputType::String
                     , &utf8_char.u8.byte1
                     , utf8_char.length );
}

//----------------------------------------------------------------------
inline void FTermOutput::appendOutputBuffer ( FOutputBuffer::OutputType type
                                            , const char* data
                                            , uInt32 length )
{
  auto& slices = output_buffer->slices;
  auto& last = slices.back();

  if ( ! slices.isEmpty() && last.type == type )
  {
    output_buffer->data.append(data, length);
    last.length += length;
  }
  else
  {
    slices.emplace(type, uInt32(length));
    output_buffer->data.append(data, length);
    checkFreeBufferSize();
  }
}

}  // namespace finalcut
