/***********************************************************************
* ftermopenbsd.cpp - Contains the NetBSD/OpenBSD terminal functions    *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2018-2020 Markus Gans                                      *
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

#include "final/fapplication.h"
#include "final/flog.h"
#include "final/fsystem.h"
#include "final/fterm.h"
#include "final/ftermopenbsd.h"

#if defined(__NetBSD__) || defined(__OpenBSD__) || defined(UNIT_TEST)
#define initCheck(ret_value)   \
    if ( ! isInitialized() )   \
    {                          \
      warnNotInitialized();    \
      return ret_value;        \
    }
#endif

namespace finalcut
{

// static class attributes
#if defined(__NetBSD__) || defined(__OpenBSD__) || defined(UNIT_TEST)
  kbd_t    FTermOpenBSD::bsd_keyboard_encoding{0};
  bool     FTermOpenBSD::meta_sends_escape{true};
  FSystem* FTermOpenBSD::fsystem{nullptr};
#endif


//----------------------------------------------------------------------
// class FTermOpenBSD
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FTermOpenBSD::~FTermOpenBSD()  // destructor
{ }

// public methods of FTermOpenBSD
//----------------------------------------------------------------------
#if defined(__NetBSD__) || defined(__OpenBSD__) || defined(UNIT_TEST)
bool FTermOpenBSD::isBSDConsole()
{
  // Check if it's a NetBSD/OpenBSD workstation console

  static kbd_t kbdencoding{};

  if ( ! fsystem )
    fsystem = FTerm::getFSystem();

  if ( fsystem
    && fsystem->ioctl(0, WSKBDIO_GETENCODING, &kbdencoding) == 0 )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
void FTermOpenBSD::init()
{
  // Initialize BSD workstation console

  fsystem = FTerm::getFSystem();

  if ( ! isBSDConsole() )
    return;

  if ( meta_sends_escape )
  {
    // Save current left alt key mapping
    saveBSDConsoleEncoding();

    // Alt key generate ESC prefix
    setBSDConsoleMetaEsc();
  }
}

//----------------------------------------------------------------------
void FTermOpenBSD::finish()
{
  initCheck();

  if ( ! isBSDConsole() )
    return;

  if ( meta_sends_escape )
    resetBSDConsoleEncoding();
}

//----------------------------------------------------------------------
bool FTermOpenBSD::setBeep (int Hz, int ms)
{
  initCheck(false);

  if ( ! isBSDConsole() )
    return false;

  // Range for frequency: 21-32766
  if ( Hz < 21 || Hz > 32766 )
    return false;

  // Range for duration:  0-1999
  if ( ms < 0 || ms > 1999 )
    return false;

  wskbd_bell_data bell;
  bell.which  = WSKBD_BELL_DOALL;
  bell.pitch  = uInt(Hz);
  bell.period = uInt(ms);
  bell.volume = 50;  // 50% volume

  if ( fsystem && fsystem->ioctl(0, WSKBDIO_SETBELL, &bell) < 0 )
    return false;
  else
    return true;
}

//----------------------------------------------------------------------
bool FTermOpenBSD::resetBeep()
{
  initCheck(false);
  wskbd_bell_data default_bell;

  // Gets the default setting for the bell
  if ( fsystem
    && fsystem->ioctl(0, WSKBDIO_GETDEFAULTBELL, &default_bell) < 0 )
    return false;

  default_bell.which = WSKBD_BELL_DOALL;

  // Sets the bell settings
  if ( fsystem
    && fsystem->ioctl(0, WSKBDIO_SETBELL, &default_bell) < 0 )
    return false;
  else
    return true;
}


// private methods of FTermOpenBSD
//----------------------------------------------------------------------
void FTermOpenBSD::warnNotInitialized()
{
  *FApplication::getLog() << FLog::Warn
                          << "The FTermOpenBSD object has "
                          << "not yet been initialized! "
                          << "Please call the init() method first."
                          << std::endl;
}

//----------------------------------------------------------------------
bool FTermOpenBSD::saveBSDConsoleEncoding()
{
  static kbd_t k_encoding{};
  int ret{-1};

  if ( fsystem )
    ret = fsystem->ioctl (0, WSKBDIO_GETENCODING, &k_encoding);

  if ( ret < 0 )
    return false;

  // Save current encoding
  bsd_keyboard_encoding = k_encoding;
  return true;
}

//----------------------------------------------------------------------
bool FTermOpenBSD::setBSDConsoleEncoding (kbd_t k_encoding)
{
  if ( fsystem
    && fsystem->ioctl(0, WSKBDIO_SETENCODING, &k_encoding) < 0 )
    return false;
  else
    return true;
}

//----------------------------------------------------------------------
bool FTermOpenBSD::setBSDConsoleMetaEsc()
{
  static constexpr kbd_t meta_esc = 0x20;  // Generate ESC prefix on ALT-key

  return setBSDConsoleEncoding (bsd_keyboard_encoding | meta_esc);
}

//----------------------------------------------------------------------
bool FTermOpenBSD::resetBSDConsoleEncoding()
{
  return setBSDConsoleEncoding (bsd_keyboard_encoding);
}
#endif  // defined(__NetBSD__) || defined(__OpenBSD__) || defined(UNIT_TEST)

}  // namespace finalcut
