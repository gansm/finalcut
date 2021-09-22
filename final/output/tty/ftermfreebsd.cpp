/***********************************************************************
* ftermfreebsd.cpp - Contains the FreeBSD terminal functions           *
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

#include "final/fapplication.h"
#include "final/ftypes.h"
#include "final/output/tty/fcharmap.h"
#include "final/output/tty/ftermdata.h"
#include "final/output/tty/ftermfreebsd.h"
#include "final/output/tty/fterm.h"
#include "final/util/flog.h"
#include "final/util/fsystem.h"

#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)

namespace finalcut
{

// static class attributes
uInt                      FTermFreeBSD::bsd_alt_keymap{0};
FTermFreeBSD::CursorStyle FTermFreeBSD::cursor_style{FreeBSDConsoleCursorStyle::Normal};
bool                      FTermFreeBSD::change_cursorstyle{true};
bool                      FTermFreeBSD::meta_sends_escape{true};


//----------------------------------------------------------------------
// class FTermFreeBSD
//----------------------------------------------------------------------

// public methods of FTermFreeBSD
//----------------------------------------------------------------------
auto FTermFreeBSD::getInstance() -> FTermFreeBSD&
{
  static const auto& freebsd_console = make_unique<FTermFreeBSD>();
  return *freebsd_console;
}

//----------------------------------------------------------------------
FTermFreeBSD::CursorStyle FTermFreeBSD::getCursorStyle()
{
  return cursor_style;
}

//----------------------------------------------------------------------
bool FTermFreeBSD::setCursorStyle (CursorStyle style)
{
  // Set cursor style in a BSD console

  if ( ! isFreeBSDConsole() || ! change_cursorstyle )
    return false;

  cursor_style = style;
  static auto& fterm_data = FTermData::getInstance();

  if ( fterm_data.isCursorHidden() )
    return false;

  return setFreeBSDCursorStyle(style);
}

//----------------------------------------------------------------------
bool FTermFreeBSD::isFreeBSDConsole()
{
  // Check if it's a FreeBSD console

  keymap_t keymap{};
  static const auto& fsystem = FSystem::getInstance();

  if ( fsystem->ioctl(0, GIO_KEYMAP, &keymap) == 0 )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
void FTermFreeBSD::setBeep (int Hz, int ms)
{
  static auto& fterm_data = FTermData::getInstance();

  if ( ! fterm_data.isTermType(FTermType::freebsd_con) )
    return;

  // Range for frequency: 21-32766
  if ( Hz < 21 || Hz > 32766 )
    return;

  // Range for duration:  0-1999
  if ( ms < 0 || ms > 1999 )
    return;

  constexpr int timer_frequency = 1193182;
  int period = timer_frequency / Hz;
  ms /= 10;
  FTerm::paddingPrintf (CSI "=%d;%dB", period, ms);
  std::fflush(stdout);
}

//----------------------------------------------------------------------
void FTermFreeBSD::resetBeep()
{
  static auto& fterm_data = FTermData::getInstance();

  if ( ! fterm_data.isTermType(FTermType::freebsd_con) )
    return;

  // Default frequency: 1491 Hz
  // Default duration:  50 ms
  FTerm::paddingPrint (CSI "=800;5B");
  std::fflush(stdout);
}

//----------------------------------------------------------------------
void FTermFreeBSD::init()
{
  // Initialize BSD console

  if ( ! isFreeBSDConsole() )
    return;

  if ( meta_sends_escape )
  {
    // Save current left alt key mapping
    saveFreeBSDAltKey();

    // Map meta key to left alt key
    setFreeBSDAlt2Meta();
  }

  if ( change_cursorstyle )
  {
    // Initialize FreeBSD console cursor
    setCursorStyle (FreeBSDConsoleCursorStyle::Destructive);
  }
}

//----------------------------------------------------------------------
void FTermFreeBSD::initCharMap()
{
  // A FreeBSD console can't show ASCII codes from 0x00 to 0x1b

  if ( ! isFreeBSDConsole() )
    return;

  for (auto&& entry : FCharMap::getCharEncodeMap())
    if ( entry.pc < 0x1c )
      entry.pc = entry.ascii;
}

//----------------------------------------------------------------------
void FTermFreeBSD::finish()
{
  // Resetting the FreeBSD console settings

  if ( ! isFreeBSDConsole() )
    return;

  if ( meta_sends_escape )
    resetFreeBSDAlt2Meta();

  setFreeBSDCursorStyle (FreeBSDConsoleCursorStyle::Normal);
}


// private methods of FTermFreeBSD
//----------------------------------------------------------------------
void FTermFreeBSD::warnNotInitialized()
{
  std::clog << FLog::LogLevel::Warn
            << "The FTermFreeBSD object has "
            << "not yet been initialized! "
            << "Please call the init() method first."
            << std::endl;
}

//----------------------------------------------------------------------
bool FTermFreeBSD::saveFreeBSDAltKey()
{
  // Saving the current mapping for the alt key

  static constexpr int left_alt = 0x38;
  int ret{-1};
  keymap_t keymap{};
  static const auto& fsystem = FSystem::getInstance();
  ret = fsystem->ioctl (0, GIO_KEYMAP, &keymap);

  if ( ret < 0 )
    return false;

  // Save current mapping
  bsd_alt_keymap = uInt(keymap.key[left_alt].map[0]);
  return true;
}

//----------------------------------------------------------------------
bool FTermFreeBSD::setFreeBSDAltKey (uInt key)
{
  // Remapping the alt key

  static constexpr int left_alt = 0x38;
  int ret{-1};
  keymap_t keymap{};
  static const auto& fsystem = FSystem::getInstance();
  ret = fsystem->ioctl (0, GIO_KEYMAP, &keymap);

  if ( ret < 0 )
    return false;

  // Mapping "key" on the left alt key
  keymap.key[left_alt].map[0] = int(key);

  if ( (keymap.n_keys > 0)
    && (fsystem->ioctl(0, PIO_KEYMAP, &keymap) < 0) )
    return false;
  else
    return true;
}

//----------------------------------------------------------------------
bool FTermFreeBSD::setFreeBSDAlt2Meta()
{
  // Use the meta key when pressing the Alt key

  return setFreeBSDAltKey (META);
}

//----------------------------------------------------------------------
bool FTermFreeBSD::resetFreeBSDAlt2Meta()
{
  // Restore the alt key mapping

  return setFreeBSDAltKey (bsd_alt_keymap);
}

//----------------------------------------------------------------------
bool FTermFreeBSD::setFreeBSDCursorStyle (CursorStyle style)
{
  static const auto& fsystem = FSystem::getInstance();

  if ( fsystem->ioctl(0, CONS_CURSORTYPE, &style) == 0 )
    return true;
  else
    return false;
}

}  // namespace finalcut

#endif  // defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
