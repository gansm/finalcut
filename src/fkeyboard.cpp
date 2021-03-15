/***********************************************************************
* fkeyboard.cpp - Read keyboard events                                 *
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

#include <fcntl.h>
#include <sys/ioctl.h>

#if defined(__CYGWIN__)
  #include <sys/select.h>  // need for FD_ZERO, FD_SET, FD_CLR, ...
#endif

#include <algorithm>
#include <array>
#include <string>

#include "final/fapplication.h"
#include "final/fkeyboard.h"
#include "final/fkey_map.h"
#include "final/fobject.h"
#include "final/fterm.h"
#include "final/ftermdetection.h"
#include "final/ftermios.h"

#if defined(__linux__)
  #include "final/ftermlinux.h"
#endif

namespace finalcut
{

// static class attributes
uInt64 FKeyboard::key_timeout{100000};             // 100 ms  (10 Hz)
uInt64 FKeyboard::read_blocking_time{100000};      // 100 ms  (10 Hz)
uInt64 FKeyboard::read_blocking_time_short{5000};  //   5 ms (200 Hz)
bool   FKeyboard::non_blocking_input_support{true};
struct timeval FKeyboard::time_keypressed{};


//----------------------------------------------------------------------
// class FKeyboard
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FKeyboard::FKeyboard()
{
  // Initialize keyboard values
  time_keypressed.tv_sec = 0;
  time_keypressed.tv_usec = 0;

  // Get the stdin file status flags
  stdin_status_flags = fcntl(FTermios::getStdIn(), F_GETFL);

  if ( stdin_status_flags == -1 )
    std::abort();
}


// public methods of FKeyboard
//----------------------------------------------------------------------
void FKeyboard::fetchKeyCode()
{
  if ( fkey_queue.size() < MAX_QUEUE_SIZE )
    parseKeyBuffer();
}

//----------------------------------------------------------------------
FString FKeyboard::getKeyName (const FKey keynum) const
{
  const auto& found_key = std::find_if
  (
    fc::fkeyname.cbegin(),
    fc::fkeyname.cend(),
    [&keynum] (const fc::FKeyName& kn)
    {
      return ( kn.num != FKey::None && kn.num == keynum );
    }
  );

  if ( found_key != fc::fkeyname.end() )
    return {found_key->string};

  if ( keynum > 32 && keynum < 127 )
    return {char(keynum)};

  return {""};
}

//----------------------------------------------------------------------
bool FKeyboard::setNonBlockingInput (bool enable)
{
  if ( enable == non_blocking_stdin )
    return non_blocking_stdin;

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

  return non_blocking_stdin;
}

//----------------------------------------------------------------------
bool& FKeyboard::hasUnprocessedInput()
{
  return unprocessed_buffer_data;
}

//----------------------------------------------------------------------
bool FKeyboard::isKeyPressed (uInt64 blocking_time)
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
void FKeyboard::clearKeyBuffer()
{
  // Empty the buffer

  fifo_offset = 0;
  fkey = FKey::None;
  key = FKey::None;
  std::fill_n (fifo_buf, FIFO_BUF_SIZE, '\0');
  fifo_in_use = false;
}

//----------------------------------------------------------------------
void FKeyboard::clearKeyBufferOnTimeout()
{
  // Empty the buffer on timeout

  if ( fifo_in_use && isKeypressTimeout() )
    clearKeyBuffer();
}

//----------------------------------------------------------------------
void FKeyboard::escapeKeyHandling()
{
  // Send an escape key press event if there is only one 0x1b
  // in the buffer and the timeout is reached

  if ( fifo_in_use
    && fifo_offset == 1
    && fifo_buf[0] == 0x1b
    && fifo_buf[1] == 0x00
    && isKeypressTimeout() )
  {
    fifo_offset = 0;
    fifo_buf[0] = 0x00;
    fifo_in_use = false;
    unprocessed_buffer_data = false;
    escapeKeyPressed();
  }

  // Handling of keys that are substrings of other keys
  substringKeyHandling();
}

//----------------------------------------------------------------------
void FKeyboard::processQueuedInput()
{
  while ( ! fkey_queue.empty() )
  {
    key = fkey_queue.front();
    fkey_queue.pop();

    if ( key > FKey::None )
    {
      keyPressed();

      if ( FApplication::isQuit() )
        return;

      keyReleased();

      if ( FApplication::isQuit() )
        return;

      key = FKey::None;
    }
  }
}

// private methods of FKeyboard
//----------------------------------------------------------------------
inline FKey FKeyboard::getMouseProtocolKey() const
{
  // Looking for mouse string in the key buffer

  if ( ! mouse_support )
    return NOT_SET;

  const std::size_t buf_len = std::strlen(fifo_buf);

  // x11 mouse tracking
  if ( buf_len >= 6 && fifo_buf[1] == '[' && fifo_buf[2] == 'M' )
    return FKey::X11mouse;

  // SGR mouse tracking
  if ( fifo_buf[1] == '[' && fifo_buf[2] == '<' && buf_len >= 9
    && (fifo_buf[buf_len - 1] == 'M' || fifo_buf[buf_len - 1] == 'm') )
    return FKey::Extended_mouse;

  // urxvt mouse tracking
  if ( fifo_buf[1] == '[' && fifo_buf[2] >= '1' && fifo_buf[2] <= '9'
    && fifo_buf[3] >= '0' && fifo_buf[3] <= '9' && buf_len >= 9
    && fifo_buf[buf_len - 1] == 'M' )
    return FKey::Urxvt_mouse;

  return NOT_SET;
}

//----------------------------------------------------------------------
inline FKey FKeyboard::getTermcapKey()
{
  // Looking for termcap key strings in the buffer

  assert ( FIFO_BUF_SIZE > 0 );

  if ( key_map.use_count() == 0 )
    return NOT_SET;

  for (auto&& entry : *key_map)
  {
    const char* kstr = entry.string;
    const std::size_t len = ( kstr ) ? std::strlen(kstr) : 0;

    if ( kstr && std::strncmp(kstr, fifo_buf, len) == 0 )  // found
    {
      std::size_t n{};

      for (n = len; n < FIFO_BUF_SIZE; n++)  // Remove founded entry
        fifo_buf[n - len] = fifo_buf[n];

      for (n = n - len; n < FIFO_BUF_SIZE; n++)  // Fill rest with '\0'
        fifo_buf[n] = '\0';

      unprocessed_buffer_data = bool(fifo_buf[0] != '\0');
      return entry.num;
    }
  }

  return NOT_SET;
}

//----------------------------------------------------------------------
inline FKey FKeyboard::getKnownKey()
{
  // Looking for a known key strings in the buffer

  assert ( FIFO_BUF_SIZE > 0 );

  for (auto&& entry : fc::fkey_table)
  {
    const char* kstr = entry.string;  // The string is never null
    const std::size_t len = std::strlen(kstr);

    if ( std::strncmp(kstr, fifo_buf, len) == 0 )  // found
    {
      std::size_t n{};

      if ( len == 2
        && ( fifo_buf[1] == 'O'
          || fifo_buf[1] == '['
          || fifo_buf[1] == ']' )
        && ! isKeypressTimeout() )
      {
        return FKey::Incomplete;
      }

      for (n = len; n < FIFO_BUF_SIZE; n++)  // Remove founded entry
        fifo_buf[n - len] = fifo_buf[n];

      for (n = n - len; n < FIFO_BUF_SIZE; n++)  // Fill rest with '\0'
        fifo_buf[n] = '\0';

      unprocessed_buffer_data = bool(fifo_buf[0] != '\0');
      return entry.num;
    }
  }

  return NOT_SET;
}

//----------------------------------------------------------------------
inline FKey FKeyboard::getSingleKey()
{
  // Looking for single key code in the buffer

  std::size_t n{};
  std::size_t len{1};
  const auto firstchar = uChar(fifo_buf[0]);
  FKey keycode{};

  // Look for a utf-8 character
  if ( utf8_input && (firstchar & 0xc0) == 0xc0 )
  {
    std::array<char, 5> utf8char{};  // Init array with '\0'
    const std::size_t buf_len = std::strlen(fifo_buf);

    if ( (firstchar & 0xe0) == 0xc0 )
      len = 2;
    else if ( (firstchar & 0xf0) == 0xe0 )
      len = 3;
    else if ( (firstchar & 0xf8) == 0xf0 )
      len = 4;

    if ( buf_len <  len && ! isKeypressTimeout() )
      return FKey::Incomplete;

    for (std::size_t i{0}; i < len ; i++)
      utf8char[i] = char(fifo_buf[i] & 0xff);

    keycode = UTF8decode(utf8char.data());
  }
  else
    keycode = FKey(fifo_buf[0] & 0xff);

  for (n = len; n < FIFO_BUF_SIZE; n++)  // Remove the key from the buffer front
    fifo_buf[n - len] = fifo_buf[n];

  for (n = n - len; n < FIFO_BUF_SIZE; n++)  // Fill the rest with '\0' bytes
    fifo_buf[n] = '\0';

  unprocessed_buffer_data = bool(fifo_buf[0] != '\0');

  if ( keycode == FKey(0) )  // Ctrl+Space or Ctrl+@
    keycode = FKey::Ctrl_space;

  return FKey(keycode == FKey(127) ? FKey::Backspace : keycode);
}

//----------------------------------------------------------------------
inline bool FKeyboard::isKeypressTimeout()
{
  return FObject::isTimeout (&time_keypressed, key_timeout);
}

//----------------------------------------------------------------------
FKey FKeyboard::UTF8decode (const char utf8[]) const
{
  FKey ucs{FKey::None};  // Universal coded character
  constexpr std::size_t max = 4;
  std::size_t len = std::strlen(utf8);

  if ( len > max )
    len = max;

  for (std::size_t i{0}; i < len; ++i)
  {
    const auto ch = uChar(utf8[i]);

    if ( (ch & 0xc0) == 0x80 )
    {
      // byte 2..4 = 10xxxxxx
      ucs = (ucs << 6) | FKey(ch & 0x3f);
    }
    else if ( ch < 128 )
    {
      // byte 1 = 0xxxxxxx (1 byte mapping)
      ucs = FKey(ch & 0xff);
    }
    else if ( (ch & 0xe0) == 0xc0 )
    {
      // byte 1 = 110xxxxx (2 byte mapping)
      ucs = FKey(ch & 0x1f);
    }
    else if ( (ch & 0xf0) == 0xe0 )
    {
      // byte 1 = 1110xxxx (3 byte mapping)
      ucs = FKey(ch & 0x0f);
    }
    else if ( (ch & 0xf8) == 0xf0 )
    {
      // byte 1 = 11110xxx (4 byte mapping)
      ucs = FKey(ch & 0x07);
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
inline ssize_t FKeyboard::readKey()
{
  setNonBlockingInput();
  const ssize_t bytes = read(FTermios::getStdIn(), &read_character, 1);
  unsetNonBlockingInput();
  return bytes;
}

//----------------------------------------------------------------------
void FKeyboard::parseKeyBuffer()
{
  ssize_t bytesread{};
  FObject::getCurrentTime (&time_keypressed);

  while ( (bytesread = readKey()) > 0 )
  {
    has_pending_input = false;

    if ( bytesread + fifo_offset <= int(FIFO_BUF_SIZE) )
    {
      fifo_buf[fifo_offset] = read_character;
      fifo_offset++;
      fifo_in_use = true;
    }

    // Read the rest from the fifo buffer
    while ( ! isKeypressTimeout()
         && fifo_offset > 0
         && fkey != FKey::Incomplete )
    {
      fkey = parseKeyString();
      fkey = keyCorrection(fkey);

      if ( fkey == FKey::X11mouse
        || fkey == FKey::Extended_mouse
        || fkey == FKey::Urxvt_mouse )
      {
        key = fkey;
        mouseTracking();
        fifo_offset = int(std::strlen(fifo_buf));
        break;
      }

      if ( fkey != FKey::Incomplete )
      {
        fkey_queue.push(fkey);
        fifo_offset = int(std::strlen(fifo_buf));
      }
    }

    fkey = FKey::None;

    if ( fkey_queue.size() >= MAX_QUEUE_SIZE )
      break;
  }
}

//----------------------------------------------------------------------
FKey FKeyboard::parseKeyString()
{
  const auto firstchar = uChar(fifo_buf[0]);

  if ( firstchar == ESC[0] )
  {
    FKey keycode = getMouseProtocolKey();

    if ( keycode != NOT_SET )
      return keycode;

    keycode = getTermcapKey();

    if ( keycode != NOT_SET )
      return keycode;

    keycode = getKnownKey();

    if ( keycode != NOT_SET )
      return keycode;

    if ( ! isKeypressTimeout() )
      return FKey::Incomplete;
  }

  return getSingleKey();
}

//----------------------------------------------------------------------
FKey FKeyboard::keyCorrection (const FKey& keycode) const
{
  FKey key_correction;

#if defined(__linux__)
  if ( FTerm::isLinuxTerm() )
  {
    const auto& linux_console = FTerm::getFTermLinux();
    key_correction = linux_console->modifierKeyCorrection(keycode);
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

  if ( fifo_in_use
    && fifo_offset == 2
    && fifo_buf[0] == 0x1b
    && (fifo_buf[1] == 'O' || fifo_buf[1] == '[' || fifo_buf[1] == ']')
    && fifo_buf[2] == '\0'
    && isKeypressTimeout() )
  {
    fifo_offset = 0;
    fifo_buf[0] = 0x00;
    fifo_in_use = false;
    unprocessed_buffer_data = false;

    if ( fifo_buf[1] == 'O' )
      fkey = FKey::Meta_O;
    else if ( fifo_buf[1] == '[' )
      fkey = FKey::Meta_left_square_bracket;
    else
      fkey = FKey::Meta_right_square_bracket;

    fkey_queue.push(fkey);
  }
}

//----------------------------------------------------------------------
void FKeyboard::keyPressed() const
{
  keypressed_cmd.execute();
}

//----------------------------------------------------------------------
void FKeyboard::keyReleased() const
{
  keyreleased_cmd.execute();
}

//----------------------------------------------------------------------
void FKeyboard::escapeKeyPressed() const
{
  escape_key_cmd.execute();
}

//----------------------------------------------------------------------
void FKeyboard::mouseTracking() const
{
  mouse_tracking_cmd.execute();
}

}  // namespace finalcut
