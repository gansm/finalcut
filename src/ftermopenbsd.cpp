/***********************************************************************
* ftermopenbsd.cpp - Contains the NetBSD/OpenBSD terminal functions    *
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

#include "final/ftermopenbsd.h"

// static class attributes
#if defined(__NetBSD__) || defined(__OpenBSD__)
  kbd_t FTermOpenBSD::wscons_keyboard_encoding = 0;
#endif

//----------------------------------------------------------------------
// class FTermOpenBSD
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FTermOpenBSD::FTermOpenBSD()
{ }

//----------------------------------------------------------------------
FTermOpenBSD::~FTermOpenBSD()  // destructor
{ }

// public methods of FTermOpenBSD
//----------------------------------------------------------------------
#if defined(__NetBSD__) || defined(__OpenBSD__)
bool FTermOpenBSD::isWSConsConsole()
{
  // Check if it's a NetBSD/OpenBSD console

  static kbd_t kbdencoding;

  if ( ioctl(0, WSKBDIO_GETENCODING, &kbdencoding) == 0 )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
void FTermOpenBSD::init()
{
  // initialize wscons console

  if ( ! isWSConsConsole() )
    return;

  // save current left alt key mapping
  saveWSConsEncoding();

  // alt key generate ESC prefix
  setWSConsMetaEsc();
}

//----------------------------------------------------------------------
void FTermOpenBSD::finish()
{
  if ( ! isWSConsConsole() )
    return;

  resetWSConsEncoding();
}


// private methods of FTermOpenBSD
//----------------------------------------------------------------------
bool FTermOpenBSD::saveWSConsEncoding()
{
  static kbd_t k_encoding;
  int ret = ioctl(0, WSKBDIO_GETENCODING, &k_encoding);

  if ( ret < 0 )
    return false;

  // save current encoding
  wscons_keyboard_encoding = k_encoding;
  return true;
}

//----------------------------------------------------------------------
bool FTermOpenBSD::setWSConsEncoding (kbd_t k_encoding)
{
  if ( ioctl(0, WSKBDIO_SETENCODING, &k_encoding) < 0 )
    return false;
  else
    return true;
}

//----------------------------------------------------------------------
bool FTermOpenBSD::setWSConsMetaEsc()
{
  static const kbd_t meta_esc = 0x20;  // generate ESC prefix on ALT-key

  return setWSConsEncoding (wscons_keyboard_encoding | meta_esc);
}

//----------------------------------------------------------------------
bool FTermOpenBSD::resetWSConsEncoding()
{
  return setWSConsEncoding (wscons_keyboard_encoding);
}

#endif
