/***********************************************************************
* fterm.cpp - Base class for terminal control                          *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2012-2021 Markus Gans                                      *
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
#include <array>
#include <string>
#include <unordered_map>
#include <vector>

#include "final/fapplication.h"
#include "final/fc.h"
#include "final/fcharmap.h"
#include "final/fkey_map.h"
#include "final/fkeyboard.h"
#include "final/flog.h"
#include "final/fmouse.h"
#include "final/foptiattr.h"
#include "final/foptimove.h"
#include "final/fstartoptions.h"
#include "final/fstring.h"
#include "final/fsystemimpl.h"
#include "final/fterm.h"
#include "final/ftermbuffer.h"
#include "final/ftermcap.h"
#include "final/ftermcapquirks.h"
#include "final/ftermdata.h"
#include "final/ftermdebugdata.h"
#include "final/ftermdetection.h"
#include "final/ftermios.h"
#include "final/ftermxterminal.h"

#if defined(UNIT_TEST)
  #include "final/ftermlinux.h"
  #include "final/ftermfreebsd.h"
  #include "final/ftermopenbsd.h"
#elif defined(__linux__)
  #include "final/ftermlinux.h"
#elif defined(__FreeBSD__) || defined(__DragonFly__)
  #include "final/ftermfreebsd.h"
#elif defined(__NetBSD__) || defined(__OpenBSD__)
  #include "final/ftermopenbsd.h"
#endif

namespace finalcut
{

namespace internal
{

struct var
{
  static FTerm* init_term_object;  // Global FTerm object
  static bool   term_initialized;  // Global init state
  static uInt   object_counter;    // Counts the number of object instances
};

FTerm* var::init_term_object{nullptr};
bool   var::term_initialized{false};
uInt   var::object_counter{0};

}  // namespace internal


//----------------------------------------------------------------------
// class FTerm
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FTerm::FTerm()
{
  internal::var::object_counter++;
}

//----------------------------------------------------------------------
FTerm::~FTerm()  // destructor
{
  if ( internal::var::init_term_object == this )
    finish();  // Resetting console settings

  internal::var::object_counter--;

  if ( internal::var::object_counter == 0 )
    printExitMessage();
}


// public methods of FTerm
//----------------------------------------------------------------------
std::size_t FTerm::getLineNumber()
{
  const auto& data = FTerm::getFTermData();
  const auto& term_geometry = data->getTermGeometry();

  if ( term_geometry.getHeight() == 0 )
    detectTermSize();

  return term_geometry.getHeight();
}

//----------------------------------------------------------------------
std::size_t FTerm::getColumnNumber()
{
  const auto& data = FTerm::getFTermData();
  const auto& term_geometry = data->getTermGeometry();

  if ( term_geometry.getWidth() == 0 )
    detectTermSize();

  return term_geometry.getWidth();
}

//----------------------------------------------------------------------
FString FTerm::getKeyName (FKey keynum)
{
  const auto& keyboard = FTerm::getFKeyboard();
  return keyboard->getKeyName (keynum);
}

//----------------------------------------------------------------------
charSubstitution& FTerm::getCharSubstitutionMap()
{
  const auto& data = FTerm::getFTermData();
  return data->getCharSubstitutionMap();
}

//----------------------------------------------------------------------
int FTerm::getTTYFileDescriptor()
{
  const auto& data = FTerm::getFTermData();
  return data->getTTYFileDescriptor();
}

//----------------------------------------------------------------------
std::string FTerm::getTermType()
{
  const auto& data = FTerm::getFTermData();
  return data->getTermType();
}

//----------------------------------------------------------------------
std::string FTerm::getTermFileName()
{
  const auto& data = FTerm::getFTermData();
  return data->getTermFileName();
}

//----------------------------------------------------------------------
int FTerm::getTabstop()
{
  return FTermcap::tabstop;
}

//----------------------------------------------------------------------
int FTerm::getMaxColor()
{
  return FTermcap::max_color;
}

//----------------------------------------------------------------------
auto FTerm::getColorPaletteTheme() -> std::shared_ptr<FColorPalette>&
{
  static const auto& color_theme = make_unique<std::shared_ptr<FColorPalette>>();
  return *color_theme.get();
}

//----------------------------------------------------------------------
auto FTerm::getFTermData() -> const std::unique_ptr<FTermData>&
{
  static const auto& data = make_unique<FTermData>();
  return data;
}

//----------------------------------------------------------------------
auto FTerm::getFSystem() -> std::unique_ptr<FSystem>&
{
  static const auto& fsys = make_unique<std::unique_ptr<FSystem>>(make_unique<FSystemImpl>());
  return *fsys.get();
}

//----------------------------------------------------------------------
auto FTerm::getFOptiMove() -> const std::unique_ptr<FOptiMove>&
{
  static const auto& opti_move = make_unique<FOptiMove>();
  return opti_move;
}

//----------------------------------------------------------------------
auto FTerm::getFOptiAttr() -> const std::unique_ptr<FOptiAttr>&
{
  static const auto& opti_attr = make_unique<FOptiAttr>();
  return opti_attr;
}

//----------------------------------------------------------------------
auto FTerm::getFTermDetection() -> const std::unique_ptr<FTermDetection>&
{
  static const auto& term_detection = make_unique<FTermDetection>();
  return term_detection;
}

//----------------------------------------------------------------------
auto FTerm::getFTermXTerminal() -> const std::unique_ptr<FTermXTerminal>&
{
  static const auto& xterm = make_unique<FTermXTerminal>();
  return xterm;
}

//----------------------------------------------------------------------
auto FTerm::getFKeyboard() -> const std::unique_ptr<FKeyboard>&
{
  static const auto& keyboard = make_unique<FKeyboard>();
  return keyboard;
}

//----------------------------------------------------------------------
auto FTerm::getFMouseControl() -> const std::unique_ptr<FMouseControl>&
{
  static const auto& mouse = make_unique<FMouseControl>();
  return mouse;
}

#if defined(__linux__) || defined(UNIT_TEST)
//----------------------------------------------------------------------
auto FTerm::getFTermLinux() -> const std::unique_ptr<FTermLinux>&
{
  static const auto& linux_console = make_unique<FTermLinux>();
  return linux_console;
}
#endif

#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
//----------------------------------------------------------------------
auto FTerm::getFTermFreeBSD() -> const std::unique_ptr<FTermFreeBSD>&
{
  static const auto& freebsd_console = make_unique<FTermFreeBSD>();
  return freebsd_console;
}
#endif

#if defined(__NetBSD__) || defined(__OpenBSD__) || defined(UNIT_TEST)
//----------------------------------------------------------------------
auto FTerm::getFTermOpenBSD() -> const std::unique_ptr<FTermOpenBSD>&
{
  static const auto& openbsd_console = make_unique<FTermOpenBSD>();
  return openbsd_console;
}
#endif

#if DEBUG
//----------------------------------------------------------------------
auto FTerm::getFTermDebugData() -> const std::unique_ptr<FTermDebugData>&
{
  static const auto& debug_data = make_unique<FTermDebugData>();
  return debug_data;
}
#endif  // DEBUG

//----------------------------------------------------------------------
bool FTerm::isNormal (const FChar& ch)
{
  return FOptiAttr::isNormal(ch);
}

//----------------------------------------------------------------------
bool FTerm::hasUTF8()
{
  const auto& data = FTerm::getFTermData();
  return data->hasUTF8Console();
}

//----------------------------------------------------------------------
bool FTerm::isMonochron()
{
  const auto& data = FTerm::getFTermData();
  return data->isMonochron();
}

//----------------------------------------------------------------------
bool FTerm::isAnsiTerminal()
{
  const auto& term_detection = FTerm::getFTermDetection();
  return term_detection->isAnsiTerminal();
}

//----------------------------------------------------------------------
bool FTerm::isXTerminal()
{
  const auto& term_detection = FTerm::getFTermDetection();
  return term_detection->isXTerminal();
}

//----------------------------------------------------------------------
bool FTerm::isRxvtTerminal()
{
  const auto& term_detection = FTerm::getFTermDetection();
  return term_detection->isRxvtTerminal();
}

//----------------------------------------------------------------------
bool FTerm::isUrxvtTerminal()
{
  const auto& term_detection = FTerm::getFTermDetection();
  return term_detection->isUrxvtTerminal();
}

//----------------------------------------------------------------------
bool FTerm::isKdeTerminal()
{
  const auto& term_detection = FTerm::getFTermDetection();
  return term_detection->isKdeTerminal();
}

//----------------------------------------------------------------------
bool FTerm::isGnomeTerminal()
{
  const auto& term_detection = FTerm::getFTermDetection();
  return term_detection->isGnomeTerminal();
}

//----------------------------------------------------------------------
bool FTerm::isPuttyTerminal()
{
  const auto& term_detection = FTerm::getFTermDetection();
  return term_detection->isPuttyTerminal();
}

//----------------------------------------------------------------------
bool FTerm::isWindowsTerminal()
{
  const auto& term_detection = FTerm::getFTermDetection();
  return term_detection->isWindowsTerminal();
}

//----------------------------------------------------------------------
bool FTerm::isTeraTerm()
{
  const auto& term_detection = FTerm::getFTermDetection();
  return term_detection->isTeraTerm();
}

//----------------------------------------------------------------------
bool FTerm::isCygwinTerminal()
{
  const auto& term_detection = FTerm::getFTermDetection();
  return term_detection->isCygwinTerminal();
}

//----------------------------------------------------------------------
bool FTerm::isMinttyTerm()
{
  const auto& term_detection = FTerm::getFTermDetection();
  return term_detection->isMinttyTerm();
}

//----------------------------------------------------------------------
bool FTerm::isLinuxTerm()
{
  const auto& term_detection = FTerm::getFTermDetection();
  return term_detection->isLinuxTerm();
}

//----------------------------------------------------------------------
bool FTerm::isFreeBSDTerm()
{
  const auto& term_detection = FTerm::getFTermDetection();
  return term_detection->isFreeBSDTerm();
}

//----------------------------------------------------------------------
bool FTerm::isNetBSDTerm()
{
  const auto& term_detection = FTerm::getFTermDetection();
  return term_detection->isNetBSDTerm();
}

//----------------------------------------------------------------------
bool FTerm::isOpenBSDTerm()
{
  const auto& term_detection = FTerm::getFTermDetection();
  return term_detection->isOpenBSDTerm();
}

//----------------------------------------------------------------------
bool FTerm::isSunTerminal()
{
  const auto& term_detection = FTerm::getFTermDetection();
  return term_detection->isSunTerminal();
}

//----------------------------------------------------------------------
bool FTerm::isScreenTerm()
{
  const auto& term_detection = FTerm::getFTermDetection();
  return term_detection->isScreenTerm();
}

//----------------------------------------------------------------------
bool FTerm::isTmuxTerm()
{
  const auto& term_detection = FTerm::getFTermDetection();
  return term_detection->isTmuxTerm();
}

//----------------------------------------------------------------------
bool FTerm::isKtermTerminal()
{
  const auto& term_detection = FTerm::getFTermDetection();
  return term_detection->isKtermTerminal();
}

//----------------------------------------------------------------------
bool FTerm::isMltermTerminal()
{
  const auto& term_detection = FTerm::getFTermDetection();
  return term_detection->isMltermTerminal();
}

//----------------------------------------------------------------------
bool FTerm::isKittyTerminal()
{
  const auto& term_detection = FTerm::getFTermDetection();
  return term_detection->isKittyTerminal();
}

//----------------------------------------------------------------------
bool FTerm::isNewFont()
{
  const auto& data = FTerm::getFTermData();
  return data->isNewFont();
}

//----------------------------------------------------------------------
bool FTerm::isInitialized()
{
  return internal::var::term_initialized;
}

//----------------------------------------------------------------------
bool FTerm::isCursorHideable()
{
  const char* cursor_off_str = disableCursorString();

  if ( cursor_off_str && std::strlen(cursor_off_str) > 0 )
    return true;

  return false;
}

//----------------------------------------------------------------------
bool FTerm::hasChangedTermSize()
{
  const auto& data = FTerm::getFTermData();
  return data->hasTermResized();
}

//----------------------------------------------------------------------
bool FTerm::hasShadowCharacter()
{
  const auto& data = FTerm::getFTermData();
  return data->hasShadowCharacter();
}

//----------------------------------------------------------------------
bool FTerm::hasHalfBlockCharacter()
{
  const auto& data = FTerm::getFTermData();
  return data->hasHalfBlockCharacter();
}

//----------------------------------------------------------------------
bool FTerm::hasAlternateScreen()
{
  const auto& data = FTerm::getFTermData();
  return data->hasAlternateScreen();
}

//----------------------------------------------------------------------
bool FTerm::canChangeColorPalette()
{
  if ( isCygwinTerminal()
    || isKdeTerminal()
    || isTeraTerm()
    || isMltermTerminal()
    || isNetBSDTerm()
    || isOpenBSDTerm()
    || isSunTerminal()
    || isAnsiTerminal() )
    return false;

  return FTermcap::can_change_color_palette;
}

//----------------------------------------------------------------------
void FTerm::setTermType (const std::string& term_name)
{
  const auto& data = FTerm::getFTermData();
  data->setTermType(term_name);
}

//----------------------------------------------------------------------
void FTerm::setInsertCursor (bool enable)
{
  if ( enable )
    setInsertCursorStyle();
  else
    setOverwriteCursorStyle();
}

//----------------------------------------------------------------------
void FTerm::redefineDefaultColors (bool enable)
{
  if ( isNewFont() )  // NewFont need the reverse-video attribute
    return;

  getFTermXTerminal()->redefineDefaultColors (enable);
}

//----------------------------------------------------------------------
void FTerm::setDblclickInterval (const uInt64 timeout)
{
  const auto& mouse = FTerm::getFMouseControl();
  mouse->setDblclickInterval(timeout);
}

//----------------------------------------------------------------------
void FTerm::useAlternateScreen (bool enable)
{
  // Sets alternate screen usage

  getFTermData()->useAlternateScreen(enable);
}

//----------------------------------------------------------------------
bool FTerm::setUTF8 (bool enable)  // UTF-8 (Unicode)
{
  const auto& data = FTerm::getFTermData();

  if ( data->isUTF8() == enable )
    return enable;

  if ( enable )
    data->setUTF8(true);
  else
    data->setUTF8(false);

#if defined(__linux__)
  FTerm::getFTermLinux()->setUTF8 (enable);
#endif

  return data->isUTF8();
}

//----------------------------------------------------------------------
bool FTerm::setVGAFont()
{
  const auto& data = FTerm::getFTermData();

  if ( data->isVGAFont() )
    return data->isVGAFont();

  if ( hasNoFontSettingOption() )
    return false;

  if ( isXTerminal() || isScreenTerm()
    || isUrxvtTerminal() || FTermcap::osc_support )
  {
    data->setVGAFont(true);
    // Set font in xterm to vga
    getFTermXTerminal()->setFont("vga");
    data->setTermEncoding (Encoding::PC);
    data->setNewFont(false);
  }
#if defined(__linux__)
  else if ( isLinuxTerm() )
  {
    const auto& linux_console = FTerm::getFTermLinux();
    data->setVGAFont(linux_console->loadVGAFont());
  }
#endif  // defined(__linux__)
  else
    data->setVGAFont(false);

  if ( data->isVGAFont() )
  {
    data->supportShadowCharacter (true);
    data->supportHalfBlockCharacter (true);
  }

  return data->isVGAFont();
}

//----------------------------------------------------------------------
bool FTerm::setNewFont()
{
  const auto& data = FTerm::getFTermData();

  if ( isNewFont() )
    return true;

  if ( hasNoFontSettingOption() )
    return false;

  if ( isXTerminal() || isScreenTerm()
    || isUrxvtTerminal() || FTermcap::osc_support )
  {
    data->setNewFont(true);
    // Set font in xterm to 8x16graph
    getFTermXTerminal()->setFont("8x16graph");
  }
#if defined(__linux__)
  else if ( isLinuxTerm() )
  {
    const auto& linux_console = FTerm::getFTermLinux();
    data->setNewFont(linux_console->loadNewFont());
  }
#endif  // defined(__linux__)
  else
    data->setNewFont(false);

  if ( isNewFont() )
  {
    data->supportShadowCharacter (true);
    data->supportHalfBlockCharacter (true);
  }

  return isNewFont();
}

//----------------------------------------------------------------------
bool FTerm::resetFont()
{
  bool retval{false};
  const auto& data = FTerm::getFTermData();

  if ( ! (data->isNewFont() || data->isVGAFont()) )
    return false;

  data->setNewFont(false);
  data->setVGAFont(false);

  if ( isXTerminal() || isScreenTerm()
    || isUrxvtTerminal() || FTermcap::osc_support )
  {
    const auto& font = data->getXtermFont();

    if ( font.getLength() > 2 )
    {
      // restore saved xterm font
      getFTermXTerminal()->setFont(font);
    }
    else
    {
      // Set font in xterm to vga
      getFTermXTerminal()->setFont("vga");
    }

    retval = true;
  }
#if defined(__linux__)
  else if ( isLinuxTerm() )
  {
    const auto& linux_console = FTerm::getFTermLinux();
    retval = linux_console->loadOldFont();
  }
#endif  // defined(__linux__)

  if ( retval )
  {
    data->setVGAFont(false);
    data->setNewFont(false);
  }

  return retval;
}

//----------------------------------------------------------------------
int FTerm::openConsole()
{
  const auto& data = FTerm::getFTermData();
  int fd = data->getTTYFileDescriptor();
  const auto& termfilename = data->getTermFileName();

  if ( ! termfilename.empty() )
    return 0;

  if ( fd >= 0 )  // console is already opened
    return 0;

  constexpr std::array<const char*, 6> terminal_devices =
  {{
    "/proc/self/fd/0",
    "/dev/tty",
    "/dev/tty0",
    "/dev/vc/0",
    "/dev/systty",
    "/dev/console"
  }};

  for (auto&& entry : terminal_devices)
  {
    const auto& fsys = FTerm::getFSystem();
    fd = fsys->open(entry, O_RDWR, 0);
    data->setTTYFileDescriptor(fd);

    if ( fd >= 0 )
      return 0;
  }

  return -1;  // No file descriptor referring to the console
}

//----------------------------------------------------------------------
int FTerm::closeConsole()
{
  const auto& data = FTerm::getFTermData();
  const int fd = data->getTTYFileDescriptor();
  int ret{-1};

  if ( fd < 0 )  // console is already closed
    return 0;

  const auto& fsys = FTerm::getFSystem();
  ret = fsys->close(fd);  // close console
  data->setTTYFileDescriptor(-1);

  if ( ret == 0 )
    return 0;
  else
    return -1;
}

//----------------------------------------------------------------------
std::string FTerm::moveCursorString (int xold, int yold, int xnew, int ynew)
{
  // Returns the cursor move string

  const auto& data = FTerm::getFTermData();

  if ( data->hasCursorOptimisation() )
  {
    const auto& opti_move = FTerm::getFOptiMove();
    return opti_move->moveCursor (xold, yold, xnew, ynew);
  }
  else
  {
    const auto& cursor_addr = FTermcap::encodeMotionParameter(TCAP(t_cursor_address), xnew, ynew);
    return cursor_addr;
  }
}

//----------------------------------------------------------------------
const char* FTerm::cursorsVisibilityString (bool enable)
{
  // Hides or shows the input cursor on the terminal

  const char* visibility_str{nullptr};
  const auto& data = FTerm::getFTermData();

  if ( data->isCursorHidden() == enable )
    return nullptr;

  if ( enable )
  {
    visibility_str = disableCursorString();

    if ( visibility_str )
      data->setCursorHidden (true);  // Global state
  }
  else
  {
    visibility_str = enableCursorString();

    if ( visibility_str )
      data->setCursorHidden (false);  // Global state
  }

  return visibility_str;
}

//----------------------------------------------------------------------
void FTerm::detectTermSize()
{
  // Detect the terminal width and height

  const auto& data = FTerm::getFTermData();
  struct winsize win_size{};
  auto& term_geometry = data->getTermGeometry();
  int ret{};
  errno = 0;

  do
  {
    const auto& fsys = FTerm::getFSystem();
    ret = fsys->ioctl (FTermios::getStdOut(), TIOCGWINSZ, &win_size);
  }
  while (errno == EINTR);

  if ( ret != 0 || win_size.ws_col == 0 || win_size.ws_row == 0 )
  {
    term_geometry.setPos (1, 1);
    // Use COLUMNS or fallback to the xterm default width of 80 characters
    uInt Columns = env2uint ("COLUMNS");
    term_geometry.setWidth( ( Columns == 0) ? 80 : Columns);
    // Use LINES or fallback to the xterm default height of 24 characters
    uInt Lines = env2uint ("LINES");
    term_geometry.setHeight( ( Lines == 0 ) ? 24 : Lines);
  }
  else
  {
    term_geometry.setRect(1, 1, win_size.ws_col, win_size.ws_row);
  }

  const auto& opti_move = FTerm::getFOptiMove();
  opti_move->setTermSize ( term_geometry.getWidth()
                         , term_geometry.getHeight() );
}

//----------------------------------------------------------------------
void FTerm::setTermSize (const FSize& size)
{
  // Set xterm size

  getFTermXTerminal()->setTermSize (size);
}

//----------------------------------------------------------------------
void FTerm::setTermTitle (const FString& title)
{
  // Set the xterm window title

  getFTermXTerminal()->setTitle (title);
}

//----------------------------------------------------------------------
void FTerm::setKDECursor (KdeKonsoleCursorShape style)
{
  // Set cursor style in KDE konsole

  if ( isKdeTerminal() )
  {
    oscPrefix();
    putstringf (OSC "50;CursorShape=%d" BEL, style);
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTerm::saveColorMap()
{
#if defined(__linux__)
  FTerm::getFTermLinux()->saveColorMap();
#endif
}

//----------------------------------------------------------------------
void FTerm::resetColorMap()
{
  const auto& oc = TCAP(t_orig_colors);
  const auto& op = TCAP(t_orig_pair);

  if ( oc )
    putstring (oc);

#if defined(__linux__)
  else
    FTerm::getFTermLinux()->resetColorMap();
#endif

  if ( op )
    putstring (op);

  std::fflush(stdout);
}

//----------------------------------------------------------------------
void FTerm::setPalette (FColor index, int r, int g, int b)
{
  // Redefine RGB color value for a palette entry

  const auto& Ic = TCAP(t_initialize_color);
  const auto& Ip = TCAP(t_initialize_pair);
  bool state{false};

  index = FOptiAttr::vga2ansi(index);

  if ( Ic || Ip )
  {
    std::string color_str{};

    const int rr = (r * 1001) / 256;
    const int gg = (g * 1001) / 256;
    const int bb = (b * 1001) / 256;

    if ( Ic )
      color_str = FTermcap::encodeParameter(Ic, uInt16(index), rr, gg, bb);
    else if ( Ip )
      color_str = FTermcap::encodeParameter(Ip, uInt16(index), 0, 0, 0, rr, gg, bb);

    if ( ! color_str.empty() )
    {
      putstring (color_str);
      state = true;
    }
  }
#if defined(__linux__)
  else
  {
    state = FTerm::getFTermLinux()->setPalette(index, r, g, b);
  }
#endif

  if ( state )
    std::fflush(stdout);
}

//----------------------------------------------------------------------
#if defined(__linux__) || defined(UNIT_TEST)
void FTerm::setBeep (int Hz, int ms)
{
  const auto& linux_console = FTerm::getFTermLinux();
  linux_console->setBeep (Hz, ms);
}
#elif defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
void FTerm::setBeep (int Hz, int ms)
{
  const auto& freebsd_console = FTerm::getFTermFreeBSD();
  freebsd_console->setBeep (Hz, ms);
}
#elif defined(__NetBSD__) || defined(__OpenBSD__) || defined(UNIT_TEST)
void FTerm::setBeep (int Hz, int ms)
{
  const auto& openbsd_console = FTerm::getFTermOpenBSD();
  openbsd_console->setBeep (Hz, ms);
}
#else
void FTerm::setBeep (int, int)
{ }
#endif  // defined(__linux__)

//----------------------------------------------------------------------
void FTerm::resetBeep()
{
#if defined(__linux__) || defined(UNIT_TEST)
  const auto& linux_console = FTerm::getFTermLinux();
  linux_console->resetBeep();
#endif

#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  const auto& freebsd_console = FTerm::getFTermFreeBSD();
  freebsd_console->resetBeep();
#endif

#if defined(__NetBSD__) || defined(__OpenBSD__) || defined(UNIT_TEST)
  const auto& openbsd_console = FTerm::getFTermOpenBSD();
  openbsd_console->resetBeep();
#endif
}

//----------------------------------------------------------------------
void FTerm::beep()
{
  if ( TCAP(t_bell) )
    putstring (TCAP(t_bell));
}

//----------------------------------------------------------------------
void FTerm::setEncoding (Encoding enc)
{
  const auto& data = FTerm::getFTermData();
  data->setTermEncoding (enc);

  assert ( enc == Encoding::UTF8
        || enc == Encoding::VT100  // VT100 line drawing
        || enc == Encoding::PC     // CP-437
        || enc == Encoding::ASCII
        || enc == Encoding::Unknown
        || enc == Encoding::NUM_OF_ENCODINGS );

  // Set the new putchar() function pointer
  switch ( enc )
  {
    case Encoding::UTF8:
      putchar() = &FTerm::putchar_UTF8;
      break;

    case Encoding::VT100:
    case Encoding::PC:
      if ( isXTerminal() && data->hasUTF8Console() )
        putchar() = &FTerm::putchar_UTF8;
      else
        putchar() = &FTerm::putchar_ASCII;
      break;

    case Encoding::ASCII:
    case Encoding::Unknown:
    case Encoding::NUM_OF_ENCODINGS:
      putchar() = &FTerm::putchar_ASCII;
  }

  if ( isLinuxTerm() )
  {
    const auto& opti_move = FTerm::getFOptiMove();

    if ( enc == Encoding::VT100 || enc == Encoding::PC )
    {
      const char* empty{nullptr};
      opti_move->set_tabular (empty);
    }
    else
      opti_move->set_tabular (TCAP(t_tab));
  }
}

//----------------------------------------------------------------------
Encoding FTerm::getEncoding()
{
  const auto& data = FTerm::getFTermData();
  return data->getTermEncoding();
}

//----------------------------------------------------------------------
std::string FTerm::getEncodingString()
{
  const auto& data = FTerm::getFTermData();
  const auto& term_encoding = data->getTermEncoding();
  const auto& encoding_list = data->getEncodingList();
  const auto& end = encoding_list.end();

  for (auto it = encoding_list.begin(); it != end; ++it )
    if ( it->second == term_encoding )
      return it->first;

  return "";
}

//----------------------------------------------------------------------
bool FTerm::charEncodable (wchar_t c)
{
  const wchar_t ch = charEncode(c);
  return ch > 0 && ch != c;
}

//----------------------------------------------------------------------
wchar_t FTerm::charEncode (wchar_t c)
{
  const auto& data = FTerm::getFTermData();
  return charEncode (c, data->getTermEncoding());
}

//----------------------------------------------------------------------
wchar_t FTerm::charEncode (wchar_t c, Encoding enc)
{
  wchar_t ch_enc = c;
  auto found = std::find_if ( fc::character.begin()
                            , fc::character.end()
                            , [&c] (const fc::CharEncodeMap& entry)
                              {
                                return entry.unicode == c;
                              } );

  if ( found != fc::character.end() )
    ch_enc = getCharacter(*found, enc);

  if ( enc == Encoding::PC && ch_enc == c )
    ch_enc = finalcut::unicode_to_cp437(c);

  return ch_enc;
}

//----------------------------------------------------------------------
bool FTerm::scrollTermForward()
{
  if ( TCAP(t_scroll_forward) )
  {
    putstring (TCAP(t_scroll_forward));
    std::fflush(stdout);
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
bool FTerm::scrollTermReverse()
{
  if ( TCAP(t_scroll_reverse) )
  {
    putstring (TCAP(t_scroll_reverse));
    std::fflush(stdout);
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
FTerm::defaultPutChar& FTerm::putchar()
{
  static const auto& fputchar = make_unique<defaultPutChar>(&FTerm::putchar_ASCII);
  return *fputchar.get();
}

//----------------------------------------------------------------------
void FTerm::putstring (const std::string& str, int affcnt)
{
  auto status = FTermcap::paddingPrint (str, affcnt, FTerm::putchar_ASCII);

  if ( status == FTermcap::Status::Error )
  {
    // Possible error handling
  }
}

//----------------------------------------------------------------------
int FTerm::putchar_ASCII (int c)
{
  const auto& fsys = FTerm::getFSystem();

  if ( fsys->putchar(char(c)) == EOF )
    return 0;
  else
    return 1;
}

//----------------------------------------------------------------------
int FTerm::putchar_UTF8 (int c)
{
  const auto& fsys = FTerm::getFSystem();

  if ( c < 0x80 )
  {
    // 1 Byte (7-bit): 0xxxxxxx
    fsys->putchar (c);
    return 1;
  }
  else if ( c < 0x800 )
  {
    // 2 byte (11-bit): 110xxxxx 10xxxxxx
    fsys->putchar (0xc0 | (c >> 6) );
    fsys->putchar (0x80 | (c & 0x3f) );
    return 2;
  }
  else if ( c < 0x10000 )
  {
    // 3 byte (16-bit): 1110xxxx 10xxxxxx 10xxxxxx
    fsys->putchar (0xe0 | (c >> 12) );
    fsys->putchar (0x80 | ((c >> 6) & 0x3f) );
    fsys->putchar (0x80 | (c & 0x3f) );
    return 3;
  }
  else if ( c < 0x200000 )
  {
    // 4 byte (21-bit): 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    fsys->putchar (0xf0 | (c >> 18) );
    fsys->putchar (0x80 | ((c >> 12) & 0x3f) );
    fsys->putchar (0x80 | ((c >> 6) & 0x3f) );
    fsys->putchar (0x80 | (c & 0x3f));
    return 4;
  }
  else
    return EOF;
}


// protected methods of FTerm
//----------------------------------------------------------------------
void FTerm::initScreenSettings()
{
#if defined(__linux__)
  // Important: Do not use setNewFont() or setVGAFont() after
  //            the console character mapping has been initialized
  const auto& linux_console = FTerm::getFTermLinux();
  linux_console->initCharMap();
#elif defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  const auto& freebsd_console = FTerm::getFTermFreeBSD();
  freebsd_console->initCharMap();
#endif

  // set xterm underline cursor
  getFTermXTerminal()->setCursorStyle (XTermCursorStyle::BlinkingUnderline);

  // set xterm color settings to defaults
  getFTermXTerminal()->setDefaults();
}

//----------------------------------------------------------------------
const char* FTerm::changeAttribute (FChar& term_attr, FChar& next_attr)
{
  const auto& opti_attr = FTerm::getFOptiAttr();
  return opti_attr->changeAttribute (term_attr, next_attr);
}

//----------------------------------------------------------------------
void FTerm::changeTermSizeFinished()
{
  const auto& data = FTerm::getFTermData();
  data->setTermResized(false);
}


// private methods of FTerm
//----------------------------------------------------------------------
inline FStartOptions& FTerm::getStartOptions()
{
  return FStartOptions::getFStartOptions();
}

//----------------------------------------------------------------------
void FTerm::init_global_values()
{
  // Initialize global values

  const auto& data = FTerm::getFTermData();

  // Preset to false
  data->setNewFont(false);

  if ( ! getStartOptions().terminal_detection )
  {
    const auto& term_detection = FTerm::getFTermDetection();
    term_detection->setTerminalDetection (false);
  }
}

//----------------------------------------------------------------------
void FTerm::init_terminal_device_path()
{
  std::array<char, 256> termfilename{};
  const int stdout_no = FTermios::getStdOut();

  if ( ttyname_r(stdout_no, termfilename.data(), termfilename.size()) )
    termfilename[0] = '\0';

  const auto& data = FTerm::getFTermData();
  data->setTermFileName(termfilename.data());
}

//----------------------------------------------------------------------
void FTerm::oscPrefix()
{
  if ( isTmuxTerm() )
  {
    // tmux device control string
    putstring (ESC "Ptmux;" ESC);
  }
  else if ( isScreenTerm() )
  {
    // GNU Screen device control string
    putstring (ESC "P");
  }
}

//----------------------------------------------------------------------
void FTerm::oscPostfix()
{
  if ( isScreenTerm() || isTmuxTerm() )
  {
    // GNU Screen/tmux string terminator
    putstring (ESC "\\");
  }
}

//----------------------------------------------------------------------
void FTerm::init_alt_charset()
{
  // Read the used VT100 pairs

  std::unordered_map<uChar, uChar> vt100_alt_char;

  if ( TCAP(t_acs_chars) )
  {
    for (std::size_t n{0}; TCAP(t_acs_chars)[n]; n += 2)
    {
      // insert the VT100 key/value pairs into a map
      const auto p1 = uChar(TCAP(t_acs_chars)[n]);
      const auto p2 = uChar(TCAP(t_acs_chars)[n + 1]);
      vt100_alt_char[p1] = p2;
    }
  }

  // Update array 'character' with discovered VT100 pairs
  for (auto&& pair : fc::dec_special_graphics)
  {
    const auto keyChar = uChar(pair.key);
    const auto altChar = wchar_t(vt100_alt_char[keyChar]);
    const auto utf8char = wchar_t(pair.unicode);
    const auto p = std::find_if ( fc::character.begin()
                                , fc::character.end()
                                , [&utf8char] (fc::CharEncodeMap entry)
                                  { return entry.unicode == utf8char; } );
    if ( p != fc::character.end() )  // found in character
    {
      const auto item = std::size_t(std::distance(fc::character.begin(), p));

      if ( altChar )                 // update alternate character set
        getCharacter(fc::character[item], Encoding::VT100) = altChar;
      else                           // delete VT100 char in character
        getCharacter(fc::character[item], Encoding::VT100) = L'\0';
    }
  }
}

//----------------------------------------------------------------------
void FTerm::init_pc_charset()
{
  bool reinit{false};
  const auto& opti_attr = FTerm::getFOptiAttr();

  // rxvt does not support pc charset
  if ( isRxvtTerminal() || isUrxvtTerminal() )
    return;

  if ( isGnomeTerminal() || isLinuxTerm() )
  {
    const auto& data = FTerm::getFTermData();

    // Fallback if tcap "S2" is not found
    if ( ! TCAP(t_enter_pc_charset_mode) )
    {
      if ( data->hasUTF8Console() )
      {
        // Select iso8859-1 + null mapping
        TCAP(t_enter_pc_charset_mode) = ESC "%@" ESC "(U";
      }
      else
      {
        // Select null mapping
        TCAP(t_enter_pc_charset_mode) = ESC "(U";
      }

      opti_attr->set_enter_pc_charset_mode \
        (TCAP(t_enter_pc_charset_mode));
      reinit = true;
    }

    // Fallback if tcap "S3" is not found
    if ( ! TCAP(t_exit_pc_charset_mode) )
    {
      if ( data->hasUTF8Console() )
      {
        // Select ascii mapping + utf8
        TCAP(t_exit_pc_charset_mode) = ESC "(B" ESC "%G";
      }
      else
      {
        // Select ascii mapping
        TCAP(t_enter_pc_charset_mode) = ESC "(B";
      }

      opti_attr->set_exit_pc_charset_mode \
          (TCAP(t_exit_pc_charset_mode));
      reinit = true;
    }
  }

  if ( reinit )
    opti_attr->initialize();
}

//----------------------------------------------------------------------
void FTerm::init_cygwin_charmap()
{
  // Replace don't printable PC charset characters in a Cygwin terminal

  if ( ! isCygwinTerminal() )
    return;

  // PC encoding changes
  for (auto&& entry : fc::character)
  {
    if ( entry.unicode == UniChar::BlackUpPointingTriangle )  // ▲
      entry.pc = 0x18;

    if ( entry.unicode == UniChar::BlackDownPointingTriangle )  // ▼
      entry.pc = 0x19;

    if ( entry.unicode == UniChar::InverseBullet  // ◘
      || entry.unicode == UniChar::InverseWhiteCircle  // ◙
      || entry.unicode == UniChar::UpDownArrow  // ↕
      || entry.unicode == UniChar::LeftRightArrow  // ↔
      || entry.unicode == UniChar::DoubleExclamationMark  // ‼
      || entry.unicode == UniChar::BlackRectangle  // ▬
      || entry.unicode == UniChar::RightwardsArrow  // →
      || entry.unicode == UniChar::Section  // §
      || entry.unicode == UniChar::SquareRoot )  // SquareRoot √
      entry.pc = entry.ascii;
  }

  // General encoding changes
  const auto& data = FTerm::getFTermData();
  charSubstitution& sub_map = data->getCharSubstitutionMap();
  sub_map[L'•'] = L'*';
  sub_map[L'●'] = L'*';
  sub_map[L'◘'] = L'*';
  sub_map[L'○'] = L'*';
  sub_map[L'◙'] = L'*';
  sub_map[L'♪'] = L'♫';
  sub_map[L'√'] = L'x';
  sub_map[L'ˣ'] = L'`';
}

//----------------------------------------------------------------------
void FTerm::init_fixed_max_color()
{
  // Initialize maximum number of colors for known terminals

  if ( isCygwinTerminal()
    || isPuttyTerminal()
    || isTeraTerm()
    || isRxvtTerminal() )
  {
    FTermcap::max_color = 16;
  }
}

//----------------------------------------------------------------------
void FTerm::init_teraterm_charmap()
{
  // Tera Term can't print ascii characters < 0x20

  if ( ! isTeraTerm() )
    return;

  for (auto&& entry : fc::character)
    if ( entry.pc < 0x20 )
      entry.pc = entry.ascii;
}

//----------------------------------------------------------------------
void FTerm::init_termcap()
{
  // Initialize the terminal capabilities

  FTermcap::init();
}

//----------------------------------------------------------------------
void FTerm::init_quirks()
{
  // Initialize terminal quirks

  FTermcapQuirks::terminalFixup();  // Fix terminal quirks
}

//----------------------------------------------------------------------
void FTerm::init_optiMove()
{
  // Duration precalculation of the cursor movement strings

  FOptiMove::TermEnv optimove_env =
  {
    TCAP(t_cursor_home),
    TCAP(t_carriage_return),
    TCAP(t_cursor_to_ll),
    TCAP(t_tab),
    TCAP(t_back_tab),
    TCAP(t_cursor_up),
    TCAP(t_cursor_down),
    TCAP(t_cursor_left),
    TCAP(t_cursor_right),
    TCAP(t_cursor_address),
    TCAP(t_column_address),
    TCAP(t_row_address),
    TCAP(t_parm_up_cursor),
    TCAP(t_parm_down_cursor),
    TCAP(t_parm_left_cursor),
    TCAP(t_parm_right_cursor),
    TCAP(t_erase_chars),
    TCAP(t_repeat_char),
    TCAP(t_clr_bol),
    TCAP(t_clr_eol),
    FTermcap::tabstop,
    FTermcap::automatic_left_margin,
    FTermcap::eat_nl_glitch
  };

  const auto& opti_move = FTerm::getFOptiMove();
  opti_move->setTermEnvironment(optimove_env);
}

//----------------------------------------------------------------------
void FTerm::init_optiAttr()
{
  // Setting video attribute optimization

  FOptiAttr::TermEnv optiattr_env =
  {
    TCAP(t_enter_bold_mode),
    TCAP(t_exit_bold_mode),
    TCAP(t_enter_dim_mode),
    TCAP(t_exit_dim_mode),
    TCAP(t_enter_italics_mode),
    TCAP(t_exit_italics_mode),
    TCAP(t_enter_underline_mode),
    TCAP(t_exit_underline_mode),
    TCAP(t_enter_blink_mode),
    TCAP(t_exit_blink_mode),
    TCAP(t_enter_reverse_mode),
    TCAP(t_exit_reverse_mode),
    TCAP(t_enter_standout_mode),
    TCAP(t_exit_standout_mode),
    TCAP(t_enter_secure_mode),
    TCAP(t_exit_secure_mode),
    TCAP(t_enter_protected_mode),
    TCAP(t_exit_protected_mode),
    TCAP(t_enter_crossed_out_mode),
    TCAP(t_exit_crossed_out_mode),
    TCAP(t_enter_dbl_underline_mode),
    TCAP(t_exit_dbl_underline_mode),
    TCAP(t_set_attributes),
    TCAP(t_exit_attribute_mode),
    TCAP(t_enter_alt_charset_mode),
    TCAP(t_exit_alt_charset_mode),
    TCAP(t_enter_pc_charset_mode),
    TCAP(t_exit_pc_charset_mode),
    TCAP(t_set_a_foreground),
    TCAP(t_set_a_background),
    TCAP(t_set_foreground),
    TCAP(t_set_background),
    TCAP(t_orig_pair),
    TCAP(t_orig_pair),
    TCAP(t_orig_colors),
    FTermcap::max_color,
    FTermcap::attr_without_color,
    FTermcap::ansi_default_color
  };

  const auto& opti_attr = FTerm::getFOptiAttr();
  opti_attr->setTermEnvironment(optiattr_env);
}

//----------------------------------------------------------------------
bool FTerm::init_font()
{
  const auto& data = FTerm::getFTermData();

  if ( getStartOptions().vgafont && ! setVGAFont() )
  {
    data->setExitMessage("VGAfont is not supported by this terminal");
    FApplication::exit(EXIT_FAILURE);
  }

  if ( getStartOptions().newfont && ! setNewFont() )
  {
    data->setExitMessage("Newfont is not supported by this terminal");
    FApplication::exit(EXIT_FAILURE);
  }

  return ! FApplication::isQuit();
}

//----------------------------------------------------------------------
void FTerm::init_locale()
{
  // Init current locale

  const auto& data = FTerm::getFTermData();
  const auto& termtype = data->getTermType();
  const char* locale_name = std::setlocale (LC_ALL, "");
  std::setlocale (LC_NUMERIC, "");

  // Get XTERM_LOCALE
  const char* locale_xterm = std::getenv("XTERM_LOCALE");

  // set LC_ALL to XTERM_LOCALE
  if ( locale_xterm )
    locale_name = std::setlocale (LC_ALL, locale_xterm);

  // TeraTerm can not show UTF-8 character
  if ( isTeraTerm() && ! std::strcmp(nl_langinfo(CODESET), "UTF-8") )
    locale_name = std::setlocale (LC_ALL, "C");

  // Kterm
  if ( isKtermTerminal() && ! std::strcmp(nl_langinfo(CODESET), "UTF-8") )
    locale_name = std::setlocale (LC_ALL, "C");

  // Sun (color) workstation console can't show UTF-8 character
  if ( termtype.substr(0,3) == "sun"
    && ! std::strcmp(nl_langinfo(CODESET), "UTF-8") )
    locale_name = std::setlocale (LC_ALL, "C");

  // Try to found a meaningful content for locale_name
  if ( locale_name )
    locale_name = std::setlocale (LC_CTYPE, nullptr);
  else
  {
    locale_name = std::getenv("LC_ALL");

    if ( ! locale_name )
    {
      locale_name = std::getenv("LC_CTYPE");

      if ( ! locale_name )
        locale_name = std::getenv("LANG");
    }
  }

  // Fallback to C
  if ( ! locale_name )
    std::setlocale (LC_ALL, "C");
}

//----------------------------------------------------------------------
void FTerm::init_encoding()
{
  // detect encoding and set the putchar() function pointer

  bool force_vt100{false};  // VT100 line drawing (G1 character set)
  init_encoding_set();

  if ( isRxvtTerminal() && ! isUrxvtTerminal() )
    force_vt100 = true;  // This rxvt terminal does not support utf-8

  init_term_encoding();
  init_pc_charset();
  init_individual_term_encoding();

  if ( force_vt100 )
    init_force_vt100_encoding();
  else
    init_utf8_without_alt_charset();

  init_tab_quirks();

  if ( getStartOptions().encoding != Encoding::Unknown )
  {
    setEncoding(getStartOptions().encoding);
  }
}

//----------------------------------------------------------------------
inline void FTerm::init_encoding_set()
{
  // Define the encoding set

  const auto& data = FTerm::getFTermData();
  auto& encoding_list = data->getEncodingList();
  encoding_list["UTF8"]  = Encoding::UTF8;
  encoding_list["UTF-8"] = Encoding::UTF8;
  encoding_list["VT100"] = Encoding::VT100;  // VT100 line drawing
  encoding_list["PC"]    = Encoding::PC;     // CP-437
  encoding_list["ASCII"] = Encoding::ASCII;
}

//----------------------------------------------------------------------
void FTerm::init_term_encoding()
{
  const int stdout_no = FTermios::getStdOut();
  const auto& data = FTerm::getFTermData();
  const auto& termtype = data->getTermType();
  const auto& fsys = FTerm::getFSystem();

  if ( fsys->isTTY(stdout_no)
    && ! std::strcmp(nl_langinfo(CODESET), "UTF-8") )
  {
    data->setUTF8Console(true);
    data->setTermEncoding (Encoding::UTF8);
    putchar() = &FTerm::putchar_UTF8;  // function pointer
    data->setUTF8(true);
    setUTF8(true);
    const auto& keyboard = FTerm::getFKeyboard();
    keyboard->enableUTF8();
  }
  else if ( fsys->isTTY(stdout_no)
         && (termtype.length() > 0)
         && (TCAP(t_exit_alt_charset_mode) != nullptr) )
  {
    data->setVT100Console (true);
    data->setTermEncoding (Encoding::VT100);
    putchar() = &FTerm::putchar_ASCII;  // function pointer
  }
  else
  {
    data->setASCIIConsole (true);
    data->setTermEncoding (Encoding::ASCII);
    putchar() = &FTerm::putchar_ASCII;  // function pointer
  }
}

//----------------------------------------------------------------------
void FTerm::init_individual_term_encoding()
{
  const auto& data = FTerm::getFTermData();

  if ( isNewFont()
    || (isPuttyTerminal() && ! data->isUTF8())
    || (isTeraTerm() && ! data->isUTF8()) )
  {
    data->setTermEncoding (Encoding::PC);
    putchar() = &FTerm::putchar_ASCII;  // function pointer

    if ( hasUTF8()
      && getStartOptions().encoding == Encoding::Unknown
      && isXTerminal() )
      putchar() = &FTerm::putchar_UTF8;  // function pointer
  }
}

//----------------------------------------------------------------------
void FTerm::init_force_vt100_encoding()
{
  const auto& data = FTerm::getFTermData();
  data->setVT100Console(true);
  data->setTermEncoding (Encoding::VT100);
  putchar() = &FTerm::putchar_ASCII;  // function pointer
}

//----------------------------------------------------------------------
void FTerm::init_utf8_without_alt_charset()
{
  // Fall back to ascii for utf-8 terminals that
  // do not support VT100 line drawings

  const auto& data = FTerm::getFTermData();

  if ( FTermcap::no_utf8_acs_chars && data->isUTF8()
    && data->getTermEncoding() == Encoding::VT100 )
  {
    data->setASCIIConsole(true);
    data->setTermEncoding (Encoding::ASCII);
    putchar() = &FTerm::putchar_ASCII;  // function pointer
  }
}

//----------------------------------------------------------------------
void FTerm::init_tab_quirks()
{
  // In some alternative character sets, a tab character prints a '○'
  // on the terminal and does not move the cursor to the next tab stop
  // position

  const auto& data = FTerm::getFTermData();
  const auto& enc = data->getTermEncoding();

  if ( enc == Encoding::VT100 || enc == Encoding::PC )
  {
    const char* empty{nullptr};
    const auto& opti_move = FTerm::getFOptiMove();
    opti_move->set_tabular (empty);
  }
}

//----------------------------------------------------------------------
void FTerm::init_captureFontAndTitle()
{
  // Save the used xterm font and window title

  if ( ! FStartOptions::getFStartOptions().terminal_data_request )
    return;

  getFTermXTerminal()->captureFontAndTitle();
  const auto& font = getFTermXTerminal()->getFont();
  const auto& title = getFTermXTerminal()->getTitle();
  const auto& data = FTerm::getFTermData();

  if ( ! font.isEmpty() )
    data->setXtermFont(font);

  if ( ! title.isEmpty() )
    data->setXtermTitle(title);
}

//----------------------------------------------------------------------
inline bool FTerm::hasNoFontSettingOption()
{
  if ( isGnomeTerminal()
    || isKdeTerminal()
    || isPuttyTerminal()
    || isTeraTerm()
    || isCygwinTerminal()
    || isMinttyTerm() )
    return true;

  return false;
}

//----------------------------------------------------------------------
inline bool FTerm::isDefaultPaletteTheme()
{
  FStringList default_themes
  {
    "default8ColorPalette",
    "default16ColorPalette",
    "default16DarkColorPalette"
  };

  auto iter = std::find ( default_themes.begin()
                        , default_themes.end()
                        , getColorPaletteTheme()->getClassName() );

  if ( iter == default_themes.end() )  // No default theme
    return false;

  return true;
}

//----------------------------------------------------------------------
void FTerm::redefineColorPalette()
{
  // Redefine the color palette

  if ( ! (canChangeColorPalette() && getStartOptions().color_change) )
    return;

  resetColorMap();
  saveColorMap();

  if ( getColorPaletteTheme().use_count() > 0 && ! isDefaultPaletteTheme() )
  {
    // A user color palette theme is in use
    getColorPaletteTheme()->setColorPalette();
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
void FTerm::restoreColorPalette()
{
  if ( ! (canChangeColorPalette() && getStartOptions().color_change) )
    return;

  // Reset screen settings
  getColorPaletteTheme()->resetColorPalette();
  getFTermXTerminal()->resetColorMap();
  resetColorMap();
}

//----------------------------------------------------------------------
void FTerm::setInsertCursorStyle()
{
  getFTermXTerminal()->setCursorStyle (XTermCursorStyle::BlinkingUnderline);
  setKDECursor(KdeKonsoleCursorShape::Underline);

#if defined(__linux__)
  const auto& linux_console = FTerm::getFTermLinux();
  linux_console->setCursorStyle (LinuxConsoleCursorStyle::Underscore);
#elif defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  const auto& freebsd_console = FTerm::getFTermFreeBSD();
  freebsd_console->setCursorStyle (FreeBSDConsoleCursorStyle::Destructive);
#endif

  if ( isUrxvtTerminal() )
    getFTermXTerminal()->setCursorColor ("rgb:ffff/ffff/ffff");
}

//----------------------------------------------------------------------
void FTerm::setOverwriteCursorStyle()
{
  getFTermXTerminal()->setCursorStyle (XTermCursorStyle::SteadyBlock);
  setKDECursor(KdeKonsoleCursorShape::Block);

#if defined(__linux__)
  const auto& linux_console = FTerm::getFTermLinux();
  linux_console->setCursorStyle (LinuxConsoleCursorStyle::FullBlock);
#elif defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  const auto& freebsd_console = FTerm::getFTermFreeBSD();
  freebsd_console->setCursorStyle (FreeBSDConsoleCursorStyle::Normal);
#endif

  if ( isUrxvtTerminal() )
    getFTermXTerminal()->setCursorColor ("rgb:eeee/0000/0000");
}

//----------------------------------------------------------------------
const char* FTerm::enableCursorString()
{
  // Returns the cursor enable string

  static constexpr std::size_t SIZE = 32;
  static std::array<char, SIZE> enable_str{};
  const auto& vs = TCAP(t_cursor_visible);
  const auto& ve = TCAP(t_cursor_normal);

  if ( ve )
    std::strncpy (enable_str.data(), ve, SIZE - 1);
  else if ( vs )
    std::strncpy (enable_str.data(), vs, SIZE - 1);

#if defined(__linux__)
  if ( isLinuxTerm() )
  {
    // Restore the last used Linux console cursor style
    const auto& linux_console = FTerm::getFTermLinux();
    const char* cstyle = linux_console->getCursorStyleString();
    std::size_t length = std::strlen(enable_str.data());
    std::strncat (enable_str.data(), cstyle, SIZE - length - 1);
  }
#endif  // defined(__linux__)

  enable_str[SIZE - 1] = '\0';

#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  if ( isFreeBSDTerm() )
  {
    // Restore the last used FreeBSD console cursor style
    const auto& freebsd_console = FTerm::getFTermFreeBSD();
    freebsd_console->setCursorStyle (freebsd_console->getCursorStyle());
  }
#endif  // defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)

  return enable_str.data();
}

//----------------------------------------------------------------------
const char* FTerm::disableCursorString()
{
  // Returns the cursor disable string

  const auto& vi = TCAP(t_cursor_invisible);

  if ( vi )
    return vi;

  return nullptr;
}

//----------------------------------------------------------------------
void FTerm::enableMouse()
{
  // Enable the terminal mouse support

  if ( ! getStartOptions().mouse_support )
    return;

  bool gpm_mouse{false};
  bool xterm_mouse{false};

#if defined(__linux__)
  if ( isLinuxTerm() && openConsole() == 0 )
  {
    if ( FTermLinux::isLinuxConsole() )
      gpm_mouse = true;

    closeConsole();
  }
#endif  // defined(__linux__)

  if ( TCAP(t_key_mouse) && ! isLinuxTerm() )
    xterm_mouse = true;

  const auto& keyboard = FTerm::getFKeyboard();
  keyboard->enableMouseSequences();
  const auto& mouse = FTerm::getFMouseControl();
  mouse->setMaxWidth (uInt16(getColumnNumber()));
  mouse->setMaxHeight (uInt16(getLineNumber()));
  // Enable the linux general purpose mouse (gpm) server
  mouse->useGpmMouse (gpm_mouse);
  // Enable xterm mouse support
  mouse->useXtermMouse (xterm_mouse);
  mouse->enable();
}

//----------------------------------------------------------------------
inline void FTerm::disableMouse()
{
  // Disable the terminal mouse support

  FTerm::getFKeyboard()->disableMouseSequences();
  FTerm::getFMouseControl()->disable();
}

//----------------------------------------------------------------------
inline void FTerm::enableKeypad()
{
  // Enter 'keyboard_transmit' mode

  if ( TCAP(t_keypad_xmit) )
  {
    putstring (TCAP(t_keypad_xmit));
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
inline void FTerm::disableKeypad()
{
  // Leave 'keyboard_transmit' mode

  if ( TCAP(t_keypad_local) )
  {
    putstring (TCAP(t_keypad_local));
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
inline void FTerm::enableAlternateCharset()
{
  // Enable alternate charset

  if ( TCAP(t_enable_acs) )
  {
    putstring (TCAP(t_enable_acs));
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
inline void FTerm::enableApplicationEscKey()
{
  // switch to application escape key mode

  if ( isMinttyTerm() )
    FTerm::putstring (CSI "?7727h");
}

//----------------------------------------------------------------------
inline void FTerm::disableApplicationEscKey()
{
  // Switch to normal escape key mode

  if ( isMinttyTerm() )
    putstring (CSI "?7727l");
}

//----------------------------------------------------------------------
void FTerm::useAlternateScreenBuffer()
{
  // Switch to the alternate screen

  if ( ! hasAlternateScreen() )
    return;

  // Save current cursor position
  if ( TCAP(t_save_cursor) )
  {
    putstring (TCAP(t_save_cursor));
    std::fflush(stdout);
  }

  // Saves the screen and the cursor position
  if ( TCAP(t_enter_ca_mode) )
  {
    putstring (TCAP(t_enter_ca_mode));
    std::fflush(stdout);
    getFTermData()->setAlternateScreenInUse(true);
  }
}

//----------------------------------------------------------------------
void FTerm::useNormalScreenBuffer()
{
  // Switch to the normal screen

  if ( ! hasAlternateScreen() )
    return;

  // restores the screen and the cursor position
  if ( TCAP(t_exit_ca_mode) )
  {
    putstring (TCAP(t_exit_ca_mode));
    std::fflush(stdout);
    getFTermData()->setAlternateScreenInUse(false);
  }

  // restore cursor to position of last save_cursor
  if ( TCAP(t_restore_cursor) )
  {
    putstring (TCAP(t_restore_cursor));
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTerm::init()
{
  internal::var::init_term_object = this;

  // Initialize global values for all objects
  init_global_values();

  // Initialize the terminal
  if ( ! init_terminal() )
    return;

  // Set maximum number of colors for detected terminals
  init_fixed_max_color();

  // Initializes variables for the current terminal
  init_termcap();

  // Initialize terminal quirks
  init_quirks();

  // Initialize cursor movement optimization
  init_optiMove();

  // Initialize video attributes optimization
  init_optiAttr();

  // Initialize vt100 alternate character set
  init_alt_charset();

  // Pass the terminal capabilities to the keyboard object
  FTerm::getFKeyboard()->setTermcapMap();

  // Initializes locale information
  init_locale();

  // Detect environment and set encoding
  init_encoding();

  // Enable the terminal mouse support
  enableMouse();

  // Activate meta key sends escape
  if ( isXTerminal() )
    getFTermXTerminal()->metaSendsESC(true);

  // switch to application escape key mode
  enableApplicationEscKey();

  // Enter 'keyboard_transmit' mode
  enableKeypad();

  // Switch to the alternate screen
  useAlternateScreenBuffer();

  // Enable alternate charset
  enableAlternateCharset();

  // Save the used xterm font and window title
  init_captureFontAndTitle();

  // KDE terminal cursor and cygwin + teraterm charmap correction
  initTermspecifics();

  // Redefine the color palette
  redefineColorPalette();

  // Set 220 Hz beep (100 ms)
  setBeep(220, 100);

  // Set FTerm signal handler
  setSignalHandler();

  if ( ! getStartOptions().cursor_optimisation )
  {
    FTerm::getFTermData()->supportCursorOptimisation(false);
  }

  // Activate the VGA or the new graphic font
  // (depending on the initialization values)
  if ( ! init_font() )
    return;

  // Turn off hardware echo
  FTermios::unsetHardwareEcho();

  // Switch to the raw mode
  FTermios::setRawMode();

  // The terminal is now initialized
  internal::var::term_initialized = true;
}

//----------------------------------------------------------------------
bool FTerm::init_terminal() const
{
  // Initialize termios
  FTermios::init();
  const auto& data = FTerm::getFTermData();
  const auto& fsys = FTerm::getFSystem();

  // Check if stdin is a tty
  if ( ! fsys->isTTY(FTermios::getStdIn()) )
  {
    data->setExitMessage("FTerm: Standard input is not a TTY.");
    FApplication::exit(EXIT_FAILURE);
    return false;
  }

  // Get pathname of the terminal device
  init_terminal_device_path();

  // Initialize Linux or *BSD console
  initOSspecifics();

  // Save termios settings
  try
  {
    FTermios::storeTTYsettings();
  }
  catch (const std::system_error& ex)
  {
    FString msg = "FTerm: " + FString{ex.what()};
    data->setExitMessage(msg);
    FApplication::exit(EXIT_FAILURE);
    return false;
  }

  // Get output baud rate
  initBaudRate();

  // Terminal detection
  FTermDetection::detect();
  const auto& term_detection = FTerm::getFTermDetection();
  setTermType (term_detection->getTermType());
  return true;
}

//----------------------------------------------------------------------
void FTerm::initOSspecifics() const
{
#if defined(__linux__)
  const auto& linux_console = FTerm::getFTermLinux();
  linux_console->init();    // Initialize Linux console

#if DEBUG
  const auto& data = FTerm::getFTermData();
  data->setFramebufferBpp (linux_console->getFramebufferBpp());
#endif

#endif  // defined(__linux__)

#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  const auto& freebsd_console = FTerm::getFTermFreeBSD();

  if ( getStartOptions().meta_sends_escape )
    freebsd_console->enableMetaSendsEscape();
  else
    freebsd_console->disableMetaSendsEscape();

  if ( getStartOptions().change_cursorstyle )
    freebsd_console->enableChangeCursorStyle();
  else
    freebsd_console->disableChangeCursorStyle();

  freebsd_console->init();  // Initialize BSD console
#elif defined(__NetBSD__) || defined(__OpenBSD__) || defined(UNIT_TEST)
  const auto& openbsd_console = FTerm::getFTermOpenBSD();

  if ( getStartOptions().meta_sends_escape )
    openbsd_console->enableMetaSendsEscape();
  else
    openbsd_console->disableMetaSendsEscape();

  openbsd_console->init();  // Initialize wscons console
#endif
}

//----------------------------------------------------------------------
void FTerm::initTermspecifics() const
{
  if ( isKdeTerminal() )
    setKDECursor(KdeKonsoleCursorShape::Underline);

  if ( isCygwinTerminal() )
    init_cygwin_charmap();

  if ( isTeraTerm() )
    init_teraterm_charmap();
}

//----------------------------------------------------------------------
void FTerm::initBaudRate() const
{
  const int stdout_no = FTermios::getStdOut();
  const uInt baud = FTermios::getBaudRate();
  FTerm::getFTermData()->setBaudrate(baud);
  const auto& fsys = FTerm::getFSystem();

  if ( fsys->isTTY(stdout_no) )
  {
    const auto& opti_move = FTerm::getFOptiMove();
    opti_move->setBaudRate(int(baud));
  }
}

//----------------------------------------------------------------------
void FTerm::finish() const
{
  // Set default signal handler
  resetSignalHandler();

  if ( isXTerminal() && ! isRxvtTerminal() )
    getFTermXTerminal()->resetTitle();

  // Restore the saved termios settings
  FTermios::restoreTTYsettings();

  // Turn off all attributes
  if ( TCAP(t_exit_attribute_mode) )
  {
    putstring (TCAP(t_exit_attribute_mode));
    std::fflush(stdout);
  }

  // Turn off pc charset mode
  if ( TCAP(t_exit_pc_charset_mode) )
  {
    putstring (TCAP(t_exit_pc_charset_mode));
    std::fflush(stdout);
  }

  // Reset xterm color settings to default values
  getFTermXTerminal()->resetDefaults();

  // Set xterm full block cursor
  getFTermXTerminal()->setCursorStyle (XTermCursorStyle::SteadyBlock);

  // Restore the color palette
  restoreColorPalette();

  // Switch to normal escape key mode
  disableApplicationEscKey();

  finishOSspecifics();

  if ( isKdeTerminal() )
    setKDECursor(KdeKonsoleCursorShape::Block);

  resetBeep();

  // Disable the terminal mouse support
  if ( getStartOptions().mouse_support )
    disableMouse();

  // Deactivate meta key sends escape
  if ( isXTerminal() )
    getFTermXTerminal()->metaSendsESC(false);

  // Switch to the normal screen
  useNormalScreenBuffer();

  // leave 'keyboard_transmit' mode
  disableKeypad();

  finish_encoding();
  const auto& data = FTerm::getFTermData();

  if ( data->isNewFont() || data->isVGAFont() )
    resetFont();
}

//----------------------------------------------------------------------
void FTerm::finishOSspecifics() const
{
#if defined(__linux__)
  const auto& linux_console = FTerm::getFTermLinux();
  linux_console->finish();
#elif defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  const auto& freebsd_console = FTerm::getFTermFreeBSD();
  freebsd_console->finish();
#elif defined(__NetBSD__) || defined(__OpenBSD__) || defined(UNIT_TEST)
  const auto& openbsd_console = FTerm::getFTermOpenBSD();
  openbsd_console->finish();
#endif
}

//----------------------------------------------------------------------
void FTerm::finish_encoding() const
{
#if defined(__linux__)
  if ( isLinuxTerm() && FTerm::getFTermData()->hasUTF8Console() )
    setUTF8(true);
#endif
}

//----------------------------------------------------------------------
void FTerm::printExitMessage()
{
  // Print exit message
  const auto& data = FTerm::getFTermData();
  const auto& exit_message = data->getExitMessage();

  if ( ! exit_message.isEmpty() )
    std::cerr << "Exit: " << exit_message << std::endl;
}

//----------------------------------------------------------------------
void FTerm::terminalSizeChange()
{
  const auto& data = FTerm::getFTermData();

  if ( data->hasTermResized() )
    return;

  // Initialize a resize event to the root element
  data->setTermResized(true);
}

//----------------------------------------------------------------------
void FTerm::processTermination (int signum)
{
  if ( internal::var::init_term_object )
    internal::var::init_term_object->finish();

  std::fflush (stderr);
  std::fflush (stdout);
  const auto& data = FTerm::getFTermData();
  FStringStream msg{};
  msg << "Program stopped: signal " << signum
      << " (" << strsignal(signum) << ")";
  data->setExitMessage(msg.str());
  printExitMessage();
  std::terminate();
}

//----------------------------------------------------------------------
void FTerm::setSignalHandler()
{
  signal(SIGTERM,  FTerm::signal_handler);  // Termination signal
  signal(SIGQUIT,  FTerm::signal_handler);  // Quit from keyboard (Ctrl-\)
  signal(SIGINT,   FTerm::signal_handler);  // Keyboard interrupt (Ctrl-C)
  signal(SIGABRT,  FTerm::signal_handler);  // Abort signal from abort(3)
  signal(SIGILL,   FTerm::signal_handler);  // Illegal Instruction
  signal(SIGSEGV,  FTerm::signal_handler);  // Invalid memory reference
  signal(SIGWINCH, FTerm::signal_handler);  // Window resize signal
}

//----------------------------------------------------------------------
void FTerm::resetSignalHandler()
{
  signal(SIGWINCH, SIG_DFL);  // Window resize signal
  signal(SIGSEGV,  SIG_DFL);  // Invalid memory reference
  signal(SIGILL,   SIG_DFL);  // Illegal Instruction
  signal(SIGABRT,  SIG_DFL);  // Abort signal from abort(3)
  signal(SIGINT,   SIG_DFL);  // Keyboard interrupt (Ctrl-C)
  signal(SIGQUIT,  SIG_DFL);  // Quit from keyboard (Ctrl-\)
  signal(SIGTERM,  SIG_DFL);  // Termination signal
}

//----------------------------------------------------------------------
void FTerm::signal_handler (int signum)
{
  switch (signum)
  {
    case SIGWINCH:
      terminalSizeChange();
      break;

    case SIGTERM:
    case SIGQUIT:
    case SIGINT:
    case SIGABRT:
    case SIGILL:
    case SIGSEGV:
      processTermination(signum);

    default:
      break;
  }
}

}  // namespace finalcut
