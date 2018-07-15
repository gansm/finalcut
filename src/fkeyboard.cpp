/***********************************************************************
* fkeyboard.cpp - Read keyboard events                                 *
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

#include <fcntl.h>

#include "final/fkeyboard.h"
#include "final/fkey_map.h"
#include "final/ftermios.h"

// static class attributes
long FKeyboard::key_timeout = 100000;  // 100 ms (default timeout for keypress)

#if defined(__linux__)
  FTermLinux* FKeyboard::linux = 0;
#endif

//----------------------------------------------------------------------
// class FKeyboardCommand
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FKeyboardCommand::FKeyboardCommand ( FApplication* object
                                   , void(FApplication::*method)() )
  : instance(0)
  , handler(0)
{
  instance = object;
  handler = method;
}

// public methods of FKeyboardCommand
//----------------------------------------------------------------------
void FKeyboardCommand::execute()
{
  (instance->*handler)();
}


//----------------------------------------------------------------------
// class FKeyboard
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FKeyboard::FKeyboard()
  : key(0)
  , fifo_offset(0)
  , fifo_in_use(false)
  , fifo_buf_size(sizeof(fifo_buf))
  , stdin_status_flags(0)
  , input_data_pending(false)
  , utf8_input(false)
  , mouse_support(true)
  , non_blocking_stdin(false)
  , keypressed_cmd()
  , keyreleased_cmd()
  , escape_key_cmd()
  , time_keypressed()
  , termcap_map(0)
{
  // Initialize keyboard values
  time_keypressed.tv_sec = 0;
  time_keypressed.tv_usec = 0;

  // Get the stdin file status flags
  stdin_status_flags = fcntl(FTermios::getStdIn(), F_GETFL);

  if ( stdin_status_flags == -1 )
    std::abort();

  // Initialize arrays with '\0'
  std::fill_n (k_buf, sizeof(k_buf), '\0');
  std::fill_n (fifo_buf, fifo_buf_size, '\0');
}

//----------------------------------------------------------------------
FKeyboard::~FKeyboard()  // destructor
{ }

// public methods of FKeyboard
//----------------------------------------------------------------------
void FKeyboard::fetchKeyCode()
{
  parseKeyBuffer();
}

//----------------------------------------------------------------------
const FString FKeyboard::getKeyName (int keynum)
{
  for (int i = 0; fc::FkeyName[i].string[0] != 0; i++)
    if ( fc::FkeyName[i].num && fc::FkeyName[i].num == keynum )
      return FString(fc::FkeyName[i].string);

  if ( keynum > 32 && keynum < 127 )
    return FString(char(keynum));

  return FString("");
}

//----------------------------------------------------------------------
void FKeyboard::setTermcapMap (fc::fkeymap* keymap)
{
  termcap_map = keymap;
}

//----------------------------------------------------------------------
bool& FKeyboard::unprocessedInput()
{
  return input_data_pending;
}

//----------------------------------------------------------------------
bool FKeyboard::isKeyPressed()
{
  register int result;
  fd_set ifds;
  struct timeval tv;
  int stdin_no = FTermios::getStdIn();

  FD_ZERO(&ifds);
  FD_SET(stdin_no, &ifds);
  tv.tv_sec  = 0;
  tv.tv_usec = 100000;  // 100 ms
  result = select (stdin_no + 1, &ifds, 0, 0, &tv);

  if ( result > 0 && FD_ISSET(stdin_no, &ifds) )
    FD_CLR (stdin_no, &ifds);

  return ( result > 0 );
}

//----------------------------------------------------------------------
void FKeyboard::emptyKeyBufferOnTimeout()
{
  // Empty the buffer on timeout
  if ( fifo_in_use && isKeypressTimeout(&time_keypressed) )
  {
    fifo_offset = 0;
    key = 0;
    std::fill_n (fifo_buf, fifo_buf_size, '\0');
    fifo_in_use = false;
  }
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
    && isKeypressTimeout(&time_keypressed) )
  {
    fifo_offset = 0;
    fifo_buf[0] = 0x00;
    fifo_in_use = false;
    input_data_pending = false;
    escapeKeyPressed();
  }
}


// private methods of FKeyboard
//----------------------------------------------------------------------
inline int FKeyboard::getMouseProtocolKey()
{
  // Looking for mouse string in the key buffer

  if ( ! mouse_support )
    return -1;

  register std::size_t buf_len = std::strlen(fifo_buf);

  // x11 mouse tracking
  if ( buf_len >= 6 && fifo_buf[1] == '[' && fifo_buf[2] == 'M' )
    return fc::Fkey_mouse;

  // SGR mouse tracking
  if ( fifo_buf[1] == '[' && fifo_buf[2] == '<' && buf_len >= 9
    && (fifo_buf[buf_len - 1] == 'M' || fifo_buf[buf_len - 1] == 'm') )
    return fc::Fkey_extended_mouse;

  // urxvt mouse tracking
  if ( fifo_buf[1] == '[' && fifo_buf[2] >= '1' && fifo_buf[2] <= '9'
    && fifo_buf[3] >= '0' && fifo_buf[3] <= '9' && buf_len >= 9
    && fifo_buf[buf_len - 1] == 'M' )
    return fc::Fkey_urxvt_mouse;

  return -1;
}

//----------------------------------------------------------------------
inline int FKeyboard::getTermcapKey()
{
  // Looking for termcap key strings in the buffer
  assert ( fifo_buf_size > 0 );

  //for (int i = 0; fc::Fkey[i].tname[0] != 0; i++)
  fc::fkeymap* keymap = reinterpret_cast<fc::fkeymap*>(termcap_map);
  for (int i = 0; keymap[i].tname[0] != 0; i++)
  {
    //char* k = fc::Fkey[i].string;
    char* k = keymap[i].string;
    register int len = ( k ) ? int(std::strlen(k)) : 0;

    if ( k && std::strncmp(k, fifo_buf, uInt(len)) == 0 )  // found
    {
      register int n;

      for (n = len; n < fifo_buf_size; n++)  // Remove founded entry
        fifo_buf[n - len] = fifo_buf[n];

      for (; n - len < len; n++)  // Fill rest with '\0'
        fifo_buf[n - len] = '\0';

      input_data_pending = bool(fifo_buf[0] != '\0');
      return fc::Fkey[i].num;
    }
  }

  return -1;
}

//----------------------------------------------------------------------
inline int FKeyboard::getMetaKey()
{
  // Looking for meta key strings in the buffer
  assert ( fifo_buf_size > 0 );

  for (int i = 0; fc::Fmetakey[i].string[0] != 0; i++)
  {
    char* kmeta = fc::Fmetakey[i].string;  // The string is never null
    register int len = int(std::strlen(kmeta));

    if ( std::strncmp(kmeta, fifo_buf, uInt(len)) == 0 )  // found
    {
      register int n;

      if ( len == 2 && ( fifo_buf[1] == 'O'
                      || fifo_buf[1] == '['
                      || fifo_buf[1] == ']' ) )
      {
        if ( ! isKeypressTimeout(&time_keypressed) )
          return NEED_MORE_DATA;
      }

      for (n = len; n < fifo_buf_size; n++)  // Remove founded entry
        fifo_buf[n - len] = fifo_buf[n];

      for (; n - len < len; n++)  // Fill rest with '\0'
        fifo_buf[n - len] = '\0';

      input_data_pending = bool(fifo_buf[0] != '\0');
      return fc::Fmetakey[i].num;
    }
  }

  return -1;
}

//----------------------------------------------------------------------
inline int FKeyboard::getSingleKey()
{
  register uChar firstchar = uChar(fifo_buf[0]);
  int keycode, n, len;
  len = 1;

  // Look for a utf-8 character
  if ( utf8_input && (firstchar & 0xc0) == 0xc0 )
  {
    char utf8char[4] = { };  // Init array with '\0'

    if ( (firstchar & 0xe0) == 0xc0 )
      len = 2;
    else if ( (firstchar & 0xf0) == 0xe0 )
      len = 3;
    else if ( (firstchar & 0xf8) == 0xf0 )
      len = 4;

    for (int i = 0; i < len ; i++)
      utf8char[i] = char(fifo_buf[i] & 0xff);

    keycode = UTF8decode(utf8char);
  }
  else
    keycode = uChar(fifo_buf[0] & 0xff);

  for (n = len; n < fifo_buf_size; n++)  // Remove the key from the buffer front
    fifo_buf[n - len] = fifo_buf[n];

  for (n = n - len; n < fifo_buf_size; n++)   // Fill the rest with '\0' bytes
    fifo_buf[n] = '\0';

  input_data_pending = bool(fifo_buf[0] != '\0');

  if ( keycode == 0 )  // Ctrl+Space or Ctrl+@
    keycode = fc::Fckey_space;

  return int(keycode == 127 ? fc::Fkey_backspace : keycode);
}

//----------------------------------------------------------------------
bool FKeyboard::setNonBlockingInput (bool on)
{
  if ( on == non_blocking_stdin )
    return non_blocking_stdin;

  if ( on )  // make stdin non-blocking
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
int FKeyboard::UTF8decode (const char utf8[])
{
  register int ucs = 0;

  for (register int i = 0; i < int(std::strlen(utf8)); ++i)
  {
    register uChar ch = uChar(utf8[i]);

    if ( (ch & 0xc0) == 0x80 )
    {
      // byte 2..4 = 10xxxxxx
      ucs = (ucs << 6) | (ch & 0x3f);
    }
    else if ( ch < 128 )
    {
      // byte 1 = 0xxxxxxx (1 byte mapping)
      ucs = ch & 0xff;
    }
    else if ( (ch & 0xe0) == 0xc0 )
    {
      // byte 1 = 110xxxxx (2 byte mapping)
      ucs = ch & 0x1f;
    }
    else if ( (ch & 0xf0) == 0xe0 )
    {
      // byte 1 = 1110xxxx (3 byte mapping)
      ucs = ch & 0x0f;
    }
    else if ( (ch & 0xf8) == 0xf0 )
    {
      // byte 1 = 11110xxx (4 byte mapping)
      ucs = ch & 0x07;
    }
    else
    {
      // error
      ucs = EOF;
    }
  }

  return ucs;
}

//----------------------------------------------------------------------
inline ssize_t FKeyboard::readKey()
{
  register ssize_t bytes;
  setNonBlockingInput();
  bytes = read(FTermios::getStdIn(), &k_buf, sizeof(k_buf) - 1);
  unsetNonBlockingInput();
  return bytes;
}

//----------------------------------------------------------------------
inline void FKeyboard::parseKeyBuffer()
{
  register ssize_t bytesread;
  FObject::getCurrentTime (&time_keypressed);

  while ( (bytesread = readKey()) > 0 )
  {
    if ( bytesread + fifo_offset <= fifo_buf_size )
    {
      for (int i = 0; i < bytesread; i++)
      {
        fifo_buf[fifo_offset] = k_buf[i];
        fifo_offset++;
      }

      fifo_in_use = true;
    }

    // Read the rest from the fifo buffer
    while ( ! isKeypressTimeout(&time_keypressed)
         && fifo_offset > 0
         && key != NEED_MORE_DATA )
    {
      key = parseKeyString();
      key = keyCorrection(key);

      if ( key != NEED_MORE_DATA )
        keyPressed();

      fifo_offset = int(std::strlen(fifo_buf));
    }

    // Send key up event
    if ( key > 0 )
      keyReleased();

    key = 0;
  }

  std::fill_n (k_buf, sizeof(k_buf), '\0');
}

//----------------------------------------------------------------------
int FKeyboard::parseKeyString()
{
  uChar firstchar = uChar(fifo_buf[0]);

  if ( firstchar == ESC[0] )
  {
    int keycode = getMouseProtocolKey();

    if ( keycode > 0 )
      return keycode;

    keycode = getTermcapKey();

    if ( keycode > 0 )
      return keycode;

    keycode = getMetaKey();

    if ( keycode > 0 )
      return keycode;

    if ( ! isKeypressTimeout(&time_keypressed) )
      return NEED_MORE_DATA;
  }

  return getSingleKey();
}

//----------------------------------------------------------------------
int FKeyboard::keyCorrection (const int& keycode)
{
  int key_correction;

#if defined(__linux__)
  key_correction = linux->modifierKeyCorrection(keycode);
#else
  key_correction = key;
#endif

  return key_correction;
}

//----------------------------------------------------------------------
void FKeyboard::keyPressed()
{
  keypressed_cmd.execute();
}

//----------------------------------------------------------------------
void FKeyboard::keyReleased()
{
  keyreleased_cmd.execute();
}

//----------------------------------------------------------------------
void FKeyboard::escapeKeyPressed()
{
  escape_key_cmd.execute();
}
