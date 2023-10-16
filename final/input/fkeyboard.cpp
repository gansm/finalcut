/***********************************************************************
* fkeyboard.cpp - Read keyboard events                                 *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2023 Markus Gans                                      *
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

#include <fcntl.h>
#include <sys/ioctl.h>

#if defined(__CYGWIN__)
  #include <sys/select.h>  // need for FD_ZERO, FD_SET, FD_CLR, ...
#endif

#include <algorithm>
#include <array>
#include <string>

#include "final/fapplication.h"
#include "final/fobject.h"
#include "final/input/fkeyboard.h"
#include "final/input/fkey_map.h"
#include "final/output/tty/fterm.h"
#include "final/output/tty/ftermios.h"

#if defined(__linux__)
  #include "final/output/tty/ftermlinux.h"
#endif

namespace finalcut
{

// static class attributes
uInt64    FKeyboard::key_timeout{100'000};             // 100 ms  (10 Hz)
uInt64    FKeyboard::read_blocking_time{100'000};      // 100 ms  (10 Hz)
uInt64    FKeyboard::read_blocking_time_short{5'000};  //   5 ms (200 Hz)
bool      FKeyboard::non_blocking_input_support{true};
TimeValue FKeyboard::time_keypressed{};


//----------------------------------------------------------------------
// class FKeyboard
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FKeyboard::FKeyboard()
{
  // Initialize keyboard values
  time_keypressed = TimeValue{};  // Set to epoch time

  // Get the stdin file status flags
  stdin_status_flags = fcntl(FTermios::getStdIn(), F_GETFL);

  if ( stdin_status_flags == -1 )
    std::abort();

  // Sort the known key map by string length
  auto& key_map = FKeyMap::getKeyMap();
  std::sort ( key_map.begin(), key_map.end()
            , [] (const auto& lhs, const auto& rhs)
              {
                return lhs.length < rhs.length;
              }
            );
}


// public methods of FKeyboard
//----------------------------------------------------------------------
auto FKeyboard::getInstance() -> FKeyboard&
{
  static const auto& keyboard = std::make_unique<FKeyboard>();
  return *keyboard;
}

//----------------------------------------------------------------------
void FKeyboard::fetchKeyCode()
{
  if ( ! fkey_queue.isFull() )
    parseKeyBuffer();
}

//----------------------------------------------------------------------
auto FKeyboard::getKeyName (const FKey keynum) const -> FString
{
  const auto& fkeyname = FKeyMap::getKeyName();
  const auto& found_key = std::find_if
  (
    fkeyname.cbegin(),
    fkeyname.cend(),
    [&keynum] (const auto& kn)
    {
      return ( kn.num != FKey::None && kn.num == keynum );
    }
  );

  if ( found_key != fkeyname.end() )
    return {found_key->string.data()};

  if ( keynum > 32 && keynum < 127 )
    return {char(keynum)};

  return {""};
}

//----------------------------------------------------------------------
void FKeyboard::setNonBlockingInput (bool enable)
{
  if ( enable == non_blocking_stdin )
    return;

  if ( enable )  // make stdin non-blocking
  {
    stdin_status_flags |= O_NONBLOCK;

    if ( fcntl (FTermios::getStdIn(), F_SETFL, stdin_status_flags) != -1 )
      non_blocking_stdin = true;
  }
  else
  {
    stdin_status_flags &= ~O_NONBLOCK;

    if ( fcntl (FTermios::getStdIn(), F_SETFL, stdin_status_flags) != -1 )
      non_blocking_stdin = false;
  }
}

//----------------------------------------------------------------------
auto FKeyboard::hasUnprocessedInput() const noexcept -> bool
{
  return fifo_buf.hasData();
}

//----------------------------------------------------------------------
auto FKeyboard::isKeyPressed (uInt64 blocking_time) -> bool
{
  if ( has_pending_input )
    return false;

  fd_set ifds{};
  struct timeval tv{};
  const int stdin_no = FTermios::getStdIn();

  FD_ZERO(&ifds);
  FD_SET(stdin_no, &ifds);
  tv.tv_sec = tv.tv_usec = 0;  // Non-blocking input

  if ( blocking_time > 0
    && non_blocking_input_support
    && select(stdin_no + 1, &ifds, nullptr, nullptr, &tv) > 0
    && FD_ISSET(stdin_no, &ifds) )
  {
    return (has_pending_input = true);
  }

  if ( isKeypressTimeout() || ! non_blocking_input_support )
    tv.tv_usec = suseconds_t(blocking_time);
  else
    tv.tv_usec = suseconds_t(read_blocking_time_short);

  if ( ! has_pending_input
    && select(stdin_no + 1, &ifds, nullptr, nullptr, &tv) > 0
    && FD_ISSET(stdin_no, &ifds) )
  {
    has_pending_input = true;
  }

  return has_pending_input;
}

//----------------------------------------------------------------------
void FKeyboard::clearKeyBuffer() noexcept
{
  // Empty the buffer

  fkey = FKey::None;
  key = FKey::None;
  fifo_buf.clear();
}

//----------------------------------------------------------------------
void FKeyboard::clearKeyBufferOnTimeout()
{
  // Empty the buffer on timeout

  if ( fifo_buf.hasData() && isKeypressTimeout() )
    clearKeyBuffer();
}

//----------------------------------------------------------------------
void FKeyboard::escapeKeyHandling()
{
  // Send an escape key press event if there is only one 0x1b
  // in the buffer and the timeout is reached

  if ( fifo_buf.getSize() == 1
    && fifo_buf[0] == 0x1b
    && isKeypressTimeout() )
  {
    fifo_buf.clear();
    escapeKeyPressedCommand();
  }

  // Handling of keys that are substrings of other keys
  substringKeyHandling();
}

//----------------------------------------------------------------------
void FKeyboard::processQueuedInput()
{
  while ( ! fkey_queue.isEmpty() )
  {
    key = fkey_queue.front();
    fkey_queue.pop();

    if ( key > FKey::None )
    {
      keyPressedCommand();

      if ( FApplication::isQuit() )
        return;

      keyReleasedCommand();

      if ( FApplication::isQuit() )
        return;

      key = FKey::None;
    }
  }
}

// private methods of FKeyboard
//----------------------------------------------------------------------
inline auto FKeyboard::getMouseProtocolKey() const -> FKey
{
  // Looking for mouse string in the key buffer

  if ( ! mouse_support )
    return NOT_SET;

  const auto buf_len = fifo_buf.getSize();

  if ( buf_len < 3 || fifo_buf[1] != '[' )
    return NOT_SET;

  // x11 mouse tracking
  if ( fifo_buf[2] == 'M' )
    return ( buf_len < 6 ) ? FKey::Incomplete : FKey::X11mouse;

  // SGR mouse tracking
  if ( fifo_buf[2] == '<' )
  {
    if ( buf_len < 9 || (fifo_buf[buf_len - 1] != 'M' && fifo_buf[buf_len - 1] != 'm') )
      return FKey::Incomplete;  // Incomplete mouse sequence

    return FKey::Extended_mouse;
  }

  // urxvt mouse tracking
  if ( fifo_buf[2] >= '1' && fifo_buf[2] <= '9'
    && std::isdigit(fifo_buf[3]) && buf_len >= 9
    && fifo_buf[buf_len - 1] == 'M' )
    return FKey::Urxvt_mouse;

  return NOT_SET;
}

//----------------------------------------------------------------------
inline auto FKeyboard::getTermcapKey() -> FKey
{
  // Looking for termcap key strings in the buffer

  static_assert ( FIFO_BUF_SIZE > 0, "FIFO buffer too small" );

  if ( key_cap_ptr.use_count() == 0 )
    return NOT_SET;

  const auto buf_len = fifo_buf.getSize();
  auto found_key = fkeyhashmap::getTermcapKey(fifo_buf);

  if ( found_key != FKey::None )  // found
  {
    fifo_buf.pop(buf_len);  // Remove founded entry
    return found_key;
  }

  return NOT_SET;
}

//----------------------------------------------------------------------
inline auto FKeyboard::getKnownKey() -> FKey
{
  // Looking for a known key strings in the buffer

  static_assert ( FIFO_BUF_SIZE > 0, "FIFO buffer too small" );
  const auto buf_len = fifo_buf.getSize();
  auto found_key = fkeyhashmap::getKnownKey(fifo_buf);

  if ( found_key != FKey::None )  // found
  {
    if ( buf_len == 2
      && ( fifo_buf[1] == 'O'
        || fifo_buf[1] == '['
        || fifo_buf[1] == ']' )
      && ! isKeypressTimeout() )
    {
      return FKey::Incomplete;
    }

    fifo_buf.pop(buf_len);  // Remove founded entry
    return found_key;
  }

  return NOT_SET;
}

//----------------------------------------------------------------------
inline auto FKeyboard::getSingleKey() -> FKey
{
  // Looking for single key code in the buffer

  const auto& firstchar = fifo_buf.front();
  FKey keycode{};

  // Use a lookup table to map firstchar to the corresponding length
  static constexpr std::array<uInt8, 256> len_lookup
  {{
    1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U,  // 0x00 - 0x0F
    1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U,  // 0x10 - 0x1F
    1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U,  // 0x20 - 0x2F
    1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U,  // 0x30 - 0x3F
    1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U,  // 0x40 - 0x4F
    1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U,  // 0x50 - 0x5F
    1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U,  // 0x60 - 0x6F
    1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U,  // 0x70 - 0x7F
    1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U,  // 0x80 - 0x8F
    1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U,  // 0x90 - 0x9F
    1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U,  // 0xA0 - 0xAF
    1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U, 1U,  // 0xB0 - 0xBF
    2U, 2U, 2U, 2U, 2U, 2U, 2U, 2U, 2U, 2U, 2U, 2U, 2U, 2U, 2U, 2U,  // 0xC0 - 0xCF
    2U, 2U, 2U, 2U, 2U, 2U, 2U, 2U, 2U, 2U, 2U, 2U, 2U, 2U, 2U, 2U,  // 0xD0 - 0xDF
    3U, 3U, 3U, 3U, 3U, 3U, 3U, 3U, 3U, 3U, 3U, 3U, 3U, 3U, 3U, 3U,  // 0xE0 - 0xEF
    4U, 4U, 4U, 4U, 4U, 4U, 4U, 4U, 5U, 5U, 5U, 5U, 6U, 6U, 1U, 1U   // 0xF0 - 0xFF
  }};

  auto len = len_lookup[firstchar & 0xff];

  // Look for a utf-8 character
  if ( utf8_input && (firstchar & uChar(0xc0)) == 0xc0 )
  {
    const auto buf_len = fifo_buf.getSize();

    if ( buf_len < len && ! isKeypressTimeout() )
      return FKey::Incomplete;

    keycode = UTF8decode(len);
  }
  else
    keycode = FKey(uChar(firstchar));

  fifo_buf.pop(len);  // Remove founded entry

  if ( keycode == FKey(0) )  // Ctrl+Space or Ctrl+@
    keycode = FKey::Ctrl_space;

  return FKey(keycode == FKey(127) ? FKey::Backspace : keycode);
}

//----------------------------------------------------------------------
inline auto FKeyboard::isKeypressTimeout() -> bool
{
  return FObjectTimer::isTimeout (time_keypressed, key_timeout);
}

//----------------------------------------------------------------------
auto FKeyboard::UTF8decode (const std::size_t len) const noexcept -> FKey
{
  using distance_type = CharRingBuffer<FIFO_BUF_SIZE>::difference_type;
  constexpr std::size_t max = 4U;
  FKey ucs{FKey::None};  // Universal coded character
  const auto begin = std::begin(fifo_buf);
  const auto end = begin + static_cast<distance_type>(std::min(len, max));

  for (auto iter{begin}; iter != end; ++iter)
  {
    const auto ch = uChar(*iter);

    if ( (ch & uChar(0xc0)) == 0x80 )
    {
      // byte 2..4 = 10xxxxxx
      ucs = (ucs << 6) | FKey(ch & uChar(0x3f));
    }
    else if ( ch < 128 )
    {
      // byte 1 = 0xxxxxxx (1 byte mapping)
      ucs = FKey(uChar(ch));
    }
    else if ( len == 2U )
    {
      // byte 1 = 110xxxxx (2 byte mapping)
      ucs = FKey(ch & uChar(0x1f));
    }
    else if ( len == 3U )
    {
      // byte 1 = 1110xxxx (3 byte mapping)
      ucs = FKey(ch & uChar(0x0f));
    }
    else if ( len == 4U )
    {
      // byte 1 = 11110xxx (4 byte mapping)
      ucs = FKey(ch & uChar(0x07));
    }
    else
    {
      // error
      ucs = NOT_SET;
    }
  }

  return ucs;
}

//----------------------------------------------------------------------
inline auto FKeyboard::readKey() -> ssize_t
{
  setNonBlockingInput();
  const ssize_t bytes = read(FTermios::getStdIn(), &read_character, 1);
  unsetNonBlockingInput();
  return bytes;
}

//----------------------------------------------------------------------
void FKeyboard::parseKeyBuffer()
{
  while ( readKey() > 0 )
  {
    time_keypressed = FObjectTimer::getCurrentTime();
    has_pending_input = false;

    if ( ! fifo_buf.isFull() )
      fifo_buf.push(read_character);

    // Read the rest from the fifo buffer
    while ( fifo_buf.hasData() && fkey != FKey::Incomplete )
    {
      fkey = parseKeyString();
      fkey = keyCorrection(fkey);

      if ( fkey == FKey::X11mouse
        || fkey == FKey::Extended_mouse
        || fkey == FKey::Urxvt_mouse )
      {
        key = fkey;
        mouseTrackingCommand();
        break;
      }

      if ( fkey != FKey::Incomplete )
        fkey_queue.emplace(fkey);
    }

    fkey = FKey::None;

    if ( fkey_queue.isFull() )
      break;
  }
}

//----------------------------------------------------------------------
auto FKeyboard::parseKeyString() -> FKey
{
  const auto& firstchar = fifo_buf.front();

  if ( firstchar != ESC[0] )
    return getSingleKey();

  if ( fifo_buf.getSize() == 1 )
    return isKeypressTimeout() ? getSingleKey() : FKey::Incomplete;

  FKey keycode = getMouseProtocolKey();

  if ( keycode != NOT_SET )
    return keycode;

  keycode = getTermcapKey();

  if ( keycode != NOT_SET )
    return keycode;

  keycode = getKnownKey();

  if ( keycode != NOT_SET )
    return keycode;

  return isKeypressTimeout() ? getSingleKey() : FKey::Incomplete;
}

//----------------------------------------------------------------------
auto FKeyboard::keyCorrection (const FKey& keycode) const -> FKey
{
  FKey key_correction;

#if defined(__linux__)
  static const auto& fterm_data = FTermData::getInstance();

  if ( fterm_data.isTermType(FTermType::linux_con) )
  {
    static auto& linux_console = FTermLinux::getInstance();
    key_correction = linux_console.modifierKeyCorrection(keycode);
  }
  else
    key_correction = keycode;
#else
  key_correction = keycode;
#endif

  return key_correction;
}

//----------------------------------------------------------------------
void FKeyboard::substringKeyHandling()
{
  // Some keys (Meta-O, Meta-[, Meta-]) used substrings
  // of other keys and are only processed after a timeout

  if ( fifo_buf.getSize() == 2
    && fifo_buf[0] == 0x1b
    && (fifo_buf[1] == 'O' || fifo_buf[1] == '[' || fifo_buf[1] == ']')
    && isKeypressTimeout() )
  {
    if ( fifo_buf[1] == 'O' )
      fkey = FKey::Meta_O;
    else if ( fifo_buf[1] == '[' )
      fkey = FKey::Meta_left_square_bracket;
    else
      fkey = FKey::Meta_right_square_bracket;

    fkey_queue.emplace(fkey);
    fifo_buf.clear();
  }
}

//----------------------------------------------------------------------
void FKeyboard::keyPressedCommand() const
{
  keypressed_cmd.execute();
}

//----------------------------------------------------------------------
void FKeyboard::keyReleasedCommand() const
{
  keyreleased_cmd.execute();
}

//----------------------------------------------------------------------
void FKeyboard::escapeKeyPressedCommand() const
{
  escape_key_cmd.execute();
}

//----------------------------------------------------------------------
void FKeyboard::mouseTrackingCommand() const
{
  mouse_tracking_cmd.execute();
}

}  // namespace finalcut
