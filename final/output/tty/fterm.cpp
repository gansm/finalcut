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
#include "final/fstartoptions.h"
#include "final/input/fkeyboard.h"
#include "final/input/fkey_map.h"
#include "final/input/fmouse.h"
#include "final/output/tty/fcharmap.h"
#include "final/output/tty/foptiattr.h"
#include "final/output/tty/foptimove.h"
#include "final/output/tty/ftermcap.h"
#include "final/output/tty/ftermcapquirks.h"
#include "final/output/tty/ftermdata.h"
#include "final/output/tty/ftermdebugdata.h"
#include "final/output/tty/ftermdetection.h"
#include "final/output/tty/fterm.h"
#include "final/output/tty/ftermios.h"
#include "final/output/tty/ftermxterminal.h"
#include "final/util/flog.h"
#include "final/util/fstring.h"
#include "final/util/fsystemimpl.h"
#include "final/vterm/fvtermbuffer.h"

#if defined(UNIT_TEST)
  #include "final/output/tty/ftermlinux.h"
  #include "final/output/tty/ftermfreebsd.h"
  #include "final/output/tty/ftermopenbsd.h"
#elif defined(__linux__)
  #include "final/output/tty/ftermlinux.h"
#elif defined(__FreeBSD__) || defined(__DragonFly__)
  #include "final/output/tty/ftermfreebsd.h"
#elif defined(__NetBSD__) || defined(__OpenBSD__)
  #include "final/output/tty/ftermopenbsd.h"
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
  static auto& fterm_data = FTermData::getInstance();
  const auto& term_geometry = fterm_data.getTermGeometry();

  if ( term_geometry.getHeight() == 0 )
    detectTermSize();

  return term_geometry.getHeight();
}

//----------------------------------------------------------------------
std::size_t FTerm::getColumnNumber()
{
  static auto& fterm_data = FTermData::getInstance();
  const auto& term_geometry = fterm_data.getTermGeometry();

  if ( term_geometry.getWidth() == 0 )
    detectTermSize();

  return term_geometry.getWidth();
}

//----------------------------------------------------------------------
FString FTerm::getKeyName (FKey keynum)
{
  static const auto& keyboard = FKeyboard::getInstance();
  return keyboard.getKeyName (keynum);
}

//----------------------------------------------------------------------
charSubstitution& FTerm::getCharSubstitutionMap()
{
  static auto& fterm_data = FTermData::getInstance();
  return fterm_data.getCharSubstitutionMap();
}

//----------------------------------------------------------------------
int FTerm::getTTYFileDescriptor()
{
  static const auto& fterm_data = FTermData::getInstance();
  return fterm_data.getTTYFileDescriptor();
}

//----------------------------------------------------------------------
std::string FTerm::getTermType()
{
  static const auto& fterm_data = FTermData::getInstance();
  return fterm_data.getTermType();
}

//----------------------------------------------------------------------
std::string FTerm::getTermFileName()
{
  static const auto& fterm_data = FTermData::getInstance();
  return fterm_data.getTermFileName();
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
bool FTerm::isNormal (const FChar& ch)
{
  return FOptiAttr::isNormal(ch);
}

//----------------------------------------------------------------------
bool FTerm::hasUTF8()
{
  static const auto& fterm_data = FTermData::getInstance();
  return fterm_data.hasUTF8Console();
}

//----------------------------------------------------------------------
bool FTerm::isMonochron()
{
  static const auto& fterm_data = FTermData::getInstance();
  return fterm_data.isMonochron();
}

//----------------------------------------------------------------------
bool FTerm::isNewFont()
{
  static const auto& fterm_data = FTermData::getInstance();
  return fterm_data.isNewFont();
}

//----------------------------------------------------------------------
bool FTerm::isInitialized()
{
  return internal::var::term_initialized;
}

//----------------------------------------------------------------------
bool FTerm::isCursorHideable()
{
  const auto& cursor_off_str = disableCursorString();

  if ( ! cursor_off_str.empty() )
    return true;

  return false;
}

//----------------------------------------------------------------------
bool FTerm::isEncodable (wchar_t c)
{
  const wchar_t ch = charEncode(c);
  return ch > 0 && ch != c;
}

//----------------------------------------------------------------------
bool FTerm::hasChangedTermSize()
{
  static auto& fterm_data = FTermData::getInstance();
  return fterm_data.hasTermResized();
}

//----------------------------------------------------------------------
bool FTerm::hasShadowCharacter()
{
  static const auto& fterm_data = FTermData::getInstance();
  return fterm_data.hasShadowCharacter();
}

//----------------------------------------------------------------------
bool FTerm::hasHalfBlockCharacter()
{
  static const auto& fterm_data = FTermData::getInstance();
  return fterm_data.hasHalfBlockCharacter();
}

//----------------------------------------------------------------------
bool FTerm::hasAlternateScreen()
{
  static const auto& fterm_data = FTermData::getInstance();
  return fterm_data.hasAlternateScreen();
}

//----------------------------------------------------------------------
bool FTerm::canChangeColorPalette()
{
  static const auto& fterm_data = FTermData::getInstance();

  if ( fterm_data.isTermType ( FTermType::cygwin
                             | FTermType::kde_konsole
                             | FTermType::tera_term
                             | FTermType::mlterm
                             | FTermType::netbsd_con
                             | FTermType::openbsd_con
                             | FTermType::sun_con
                             | FTermType::ansi ) )
    return false;

  return FTermcap::can_change_color_palette;
}

//----------------------------------------------------------------------
void FTerm::setTermType (const std::string& term_name)
{
  static auto& fterm_data = FTermData::getInstance();
  fterm_data.setTermType(term_name);
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

  FTermXTerminal::getInstance().redefineDefaultColors (enable);
}

//----------------------------------------------------------------------
void FTerm::setDblclickInterval (const uInt64 timeout)
{
  static const auto& mouse = FMouseControl::getInstance();
  mouse.setDblclickInterval(timeout);
}

//----------------------------------------------------------------------
void FTerm::useAlternateScreen (bool enable)
{
  // Sets alternate screen usage

  FTermData::getInstance().useAlternateScreen(enable);
}

//----------------------------------------------------------------------
bool FTerm::setUTF8 (bool enable)  // UTF-8 (Unicode)
{
  static auto& data = FTermData::getInstance();

  if ( data.isUTF8() == enable )
    return enable;

  if ( enable )
    data.setUTF8(true);
  else
    data.setUTF8(false);

#if defined(__linux__)
  FTermLinux::getInstance().setUTF8 (enable);
#endif

  return data.isUTF8();
}

//----------------------------------------------------------------------
bool FTerm::setVGAFont()
{
  static auto& data = FTermData::getInstance();

  if ( data.isVGAFont() )
    return data.isVGAFont();

  if ( hasNoFontSettingOption() )
    return false;

  if ( data.isTermType ( FTermType::xterm
                       | FTermType::screen
                       | FTermType::urxvt )
    || FTermcap::osc_support )
  {
    data.setVGAFont(true);
    // Set font in xterm to vga
    FTermXTerminal::getInstance().setFont("vga");
    data.setTermEncoding (Encoding::PC);
    data.setNewFont(false);
  }
#if defined(__linux__)
  else if ( data.isTermType(FTermType::linux_con) )
  {
    static auto& linux_console = FTermLinux::getInstance();
    data.setVGAFont(linux_console.loadVGAFont());
  }
#endif  // defined(__linux__)
  else
    data.setVGAFont(false);

  if ( data.isVGAFont() )
  {
    data.supportShadowCharacter (true);
    data.supportHalfBlockCharacter (true);
  }

  return data.isVGAFont();
}

//----------------------------------------------------------------------
bool FTerm::setNewFont()
{
  static auto& data = FTermData::getInstance();

  if ( isNewFont() )
    return true;

  if ( hasNoFontSettingOption() )
    return false;

  if ( data.isTermType ( FTermType::xterm
                       | FTermType::screen
                       | FTermType::urxvt )
    || FTermcap::osc_support )
  {
    data.setNewFont(true);
    // Set font in xterm to 8x16graph
    FTermXTerminal::getInstance().setFont("8x16graph");
  }
#if defined(__linux__)
  else if ( data.isTermType(FTermType::linux_con) )
  {
    static auto& linux_console = FTermLinux::getInstance();
    data.setNewFont(linux_console.loadNewFont());
  }
#endif  // defined(__linux__)
  else
    data.setNewFont(false);

  if ( isNewFont() )
  {
    data.supportShadowCharacter (true);
    data.supportHalfBlockCharacter (true);
  }

  return isNewFont();
}

//----------------------------------------------------------------------
bool FTerm::resetFont()
{
  bool retval{false};
  auto& data = FTermData::getInstance();

  if ( ! (data.isNewFont() || data.isVGAFont()) )
    return false;

  data.setNewFont(false);
  data.setVGAFont(false);

  if ( data.isTermType ( FTermType::xterm
                       | FTermType::screen
                       | FTermType::urxvt )
    || FTermcap::osc_support )
  {
    const auto& font = data.getXtermFont();

    if ( font.getLength() > 2 )
    {
      // restore saved xterm font
      FTermXTerminal::getInstance().setFont(font);
    }
    else
    {
      // Set font in xterm to vga
      FTermXTerminal::getInstance().setFont("vga");
    }

    retval = true;
  }
#if defined(__linux__)
  else if ( data.isTermType(FTermType::linux_con) )
  {
    static auto& linux_console = FTermLinux::getInstance();
    retval = linux_console.loadOldFont();
  }
#endif  // defined(__linux__)

  if ( retval )
  {
    data.setVGAFont(false);
    data.setNewFont(false);
  }

  return retval;
}

//----------------------------------------------------------------------
int FTerm::openConsole()
{
  static auto& data = FTermData::getInstance();
  int fd = data.getTTYFileDescriptor();
  const auto& termfilename = data.getTermFileName();

  if ( termfilename.empty() )
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
    static const auto& fsys = FSystem::getInstance();
    fd = fsys->open(entry, O_RDWR, 0);
    data.setTTYFileDescriptor(fd);

    if ( fd >= 0 )
      return 0;
  }

  return -1;  // No file descriptor referring to the console
}

//----------------------------------------------------------------------
int FTerm::closeConsole()
{
  static auto& data = FTermData::getInstance();
  const int fd = data.getTTYFileDescriptor();
  int ret{-1};

  if ( fd < 0 )  // console is already closed
    return 0;

  static const auto& fsys = FSystem::getInstance();
  ret = fsys->close(fd);  // close console
  data.setTTYFileDescriptor(-1);

  if ( ret == 0 )
    return 0;
  else
    return -1;
}

//----------------------------------------------------------------------
std::string FTerm::moveCursorString (int xold, int yold, int xnew, int ynew)
{
  // Returns the cursor move string

  static const auto& data = FTermData::getInstance();

  if ( data.hasCursorOptimisation() )
  {
    static auto& opti_move = FOptiMove::getInstance();
    return opti_move.moveCursor (xold, yold, xnew, ynew);
  }
  else
  {
    const auto& cursor_addr = FTermcap::encodeMotionParameter(TCAP(t_cursor_address), xnew, ynew);
    return cursor_addr;
  }
}

//----------------------------------------------------------------------
std::string FTerm::cursorsVisibilityString (bool enable)
{
  // Hides or shows the input cursor on the terminal

  std::string visibility_str{};
  static auto& data = FTermData::getInstance();

  if ( data.isCursorHidden() == enable )
    return {};

  if ( enable )
  {
    visibility_str = disableCursorString();

    if ( ! visibility_str.empty() )
      data.setCursorHidden (true);  // Global state
  }
  else
  {
    visibility_str = enableCursorString();

    if ( ! visibility_str.empty() )
      data.setCursorHidden (false);  // Global state
  }

  return visibility_str;
}

//----------------------------------------------------------------------
void FTerm::detectTermSize()
{
  // Detect the terminal width and height

  struct winsize win_size{};
  static auto& data = FTermData::getInstance();
  auto& term_geometry = data.getTermGeometry();
  int ret{};
  errno = 0;

  do
  {
    static const auto& fsys = FSystem::getInstance();
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

  static auto& opti_move = FOptiMove::getInstance();
  opti_move.setTermSize ( term_geometry.getWidth()
                        , term_geometry.getHeight() );
}

//----------------------------------------------------------------------
void FTerm::setTermSize (const FSize& size)
{
  // Set xterm size

  static auto& xterm = FTermXTerminal::getInstance();
  xterm.setTermSize (size);
}

//----------------------------------------------------------------------
void FTerm::setTermTitle (const FString& title)
{
  // Set the xterm window title

  static auto& xterm = FTermXTerminal::getInstance();
  xterm.setTitle (title);
}

//----------------------------------------------------------------------
void FTerm::setKDECursor (KdeKonsoleCursorShape style)
{
  // Set cursor style in KDE konsole

  if ( ! FTermData::getInstance().isTermType(FTermType::kde_konsole) )
    return;

  oscPrefix();
  paddingPrintf (OSC "50;CursorShape=%d" BEL, style);
  oscPostfix();
  std::fflush(stdout);
}

//----------------------------------------------------------------------
void FTerm::saveColorMap()
{
#if defined(__linux__)
  FTermLinux::getInstance().saveColorMap();
#endif
}

//----------------------------------------------------------------------
void FTerm::resetColorMap()
{
  const auto& oc = TCAP(t_orig_colors);
  const auto& op = TCAP(t_orig_pair);

  if ( oc )
    paddingPrint (oc);

#if defined(__linux__)
  else
    FTermLinux::getInstance().resetColorMap();
#endif

  if ( op )
    paddingPrint (op);

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
      paddingPrint (color_str);
      state = true;
    }
  }
#if defined(__linux__)
  else
  {
    state = FTermLinux::getInstance().setPalette(index, r, g, b);
  }
#endif

  if ( state )
    std::fflush(stdout);
}

//----------------------------------------------------------------------
#if defined(__linux__) || defined(UNIT_TEST)
void FTerm::setBeep (int Hz, int ms)
{
  FTermLinux::getInstance().setBeep (Hz, ms);
}
#elif defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
void FTerm::setBeep (int Hz, int ms)
{
  FTermFreeBSD::getInstance().setBeep (Hz, ms);
}
#elif defined(__NetBSD__) || defined(__OpenBSD__) || defined(UNIT_TEST)
void FTerm::setBeep (int Hz, int ms)
{
  FTermOpenBSD::getInstance().setBeep (Hz, ms);
}
#else
void FTerm::setBeep (int, int)
{ }
#endif  // defined(__linux__)

//----------------------------------------------------------------------
void FTerm::resetBeep()
{
#if defined(__linux__) || defined(UNIT_TEST)
  FTermLinux::getInstance().resetBeep();
#endif

#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  FTermFreeBSD::getInstance().resetBeep();
#endif

#if defined(__NetBSD__) || defined(__OpenBSD__) || defined(UNIT_TEST)
  FTermOpenBSD::getInstance().resetBeep();
#endif
}

//----------------------------------------------------------------------
void FTerm::beep()
{
  if ( TCAP(t_bell) )
  {
    paddingPrint (TCAP(t_bell));
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTerm::setEncoding (Encoding enc)
{
  assert ( enc == Encoding::UTF8
        || enc == Encoding::VT100  // VT100 line drawing
        || enc == Encoding::PC     // CP-437
        || enc == Encoding::ASCII
        || enc == Encoding::Unknown
        || enc == Encoding::NUM_OF_ENCODINGS );

  static auto& data = FTermData::getInstance();
  data.setTermEncoding (enc);

  if ( data.isTermType(FTermType::linux_con) )
  {
    static auto& opti_move = FOptiMove::getInstance();

    if ( enc == Encoding::VT100 || enc == Encoding::PC )
    {
      const char* empty{nullptr};
      opti_move.set_tabular (empty);
    }
    else
      opti_move.set_tabular (TCAP(t_tab));
  }
}

//----------------------------------------------------------------------
std::string FTerm::getEncodingString()
{
  static auto& data = FTermData::getInstance();
  const auto& term_encoding = data.getTermEncoding();
  const auto& encoding_list = data.getEncodingList();
  const auto& end = encoding_list.end();

  for (auto it = encoding_list.begin(); it != end; ++it )
    if ( it->second == term_encoding )
      return it->first;

  return "";
}

//----------------------------------------------------------------------
wchar_t FTerm::charEncode (wchar_t c)
{
  static const auto& data = FTermData::getInstance();
  return charEncode (c, data.getTermEncoding());
}

//----------------------------------------------------------------------
wchar_t FTerm::charEncode (wchar_t c, Encoding enc)
{
  wchar_t ch_enc = c;
  auto& character = FCharMap::getCharEncodeMap();
  auto found = std::find_if ( character.begin()
                            , character.end()
                            , [&c] (const FCharMap::CharEncodeMap& entry)
                              {
                                return entry.unicode == c;
                              } );

  if ( found != character.end() )
    ch_enc = FCharMap::getCharacter(*found, enc);

  if ( enc == Encoding::PC && ch_enc == c )
    ch_enc = finalcut::unicode_to_cp437(c);

  return ch_enc;
}

//----------------------------------------------------------------------
bool FTerm::scrollTermForward()
{
  if ( TCAP(t_scroll_forward) )
  {
    paddingPrint (TCAP(t_scroll_forward));
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
    paddingPrint (TCAP(t_scroll_reverse));
    std::fflush(stdout);
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
void FTerm::paddingPrint (const std::string& str, int affcnt)
{
  auto status = FTermcap::paddingPrint (str, affcnt, FTerm::putchar);

  if ( status == FTermcap::Status::Error )
  {
    // Possible error handling
  }
}

//----------------------------------------------------------------------
int FTerm::putstring (const std::string& str)
{
  static const auto& fsys = FSystem::getInstance();
  auto put = [] (const std::string& string)
             {
               return fsys->fputs(string.c_str(), stdout);
             };
  return put(str);
}

//----------------------------------------------------------------------
int FTerm::putchar (int c)
{
  static const auto& fsys = FSystem::getInstance();
  auto put = [] (int ch) { return fsys->putchar(ch); };
  return put(char(c));
}


// protected methods of FTerm
//----------------------------------------------------------------------
void FTerm::initScreenSettings()
{
#if defined(__linux__)
  // Important: Do not use setNewFont() or setVGAFont() after
  //            the console character mapping has been initialized
  FTermLinux::getInstance().initCharMap();
#elif defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  FTermFreeBSD::getInstance().initCharMap();
#endif

  // set xterm underline cursor
  FTermXTerminal::getInstance().setCursorStyle (XTermCursorStyle::BlinkingUnderline);

  // set xterm color settings to defaults
  FTermXTerminal::getInstance().setDefaults();
}

//----------------------------------------------------------------------
std::string FTerm::changeAttribute (FChar& term_attr, FChar& next_attr)
{
  static auto& opti_attr = FOptiAttr::getInstance();
  return opti_attr.changeAttribute (term_attr, next_attr);
}

//----------------------------------------------------------------------
void FTerm::changeTermSizeFinished()
{
  static auto& data = FTermData::getInstance();
  data.setTermResized(false);
}


// private methods of FTerm
//----------------------------------------------------------------------
inline FStartOptions& FTerm::getStartOptions()
{
  static auto& start_options = FStartOptions::getInstance();
  return start_options;
}

//----------------------------------------------------------------------
void FTerm::init_global_values()
{
  // Initialize global values

  FTermData::getInstance().setNewFont(false);  // Preset to false

  if ( ! getStartOptions().terminal_detection )
  {
    FTermDetection::getInstance().setTerminalDetection (false);
  }
}

//----------------------------------------------------------------------
void FTerm::init_terminal_device_path()
{
  std::array<char, 256> termfilename{};
  const int stdout_no = FTermios::getStdOut();

  if ( ttyname_r(stdout_no, termfilename.data(), termfilename.size()) )
    termfilename[0] = '\0';

  FTermData::getInstance().setTermFileName(termfilename.data());
}

//----------------------------------------------------------------------
void FTerm::oscPrefix()
{
  static const auto& data = FTermData::getInstance();

  if ( data.isTermType(FTermType::tmux) )
  {
    // tmux device control string
    paddingPrint (ESC "Ptmux;" ESC);
  }
  else if ( data.isTermType(FTermType::screen) )
  {
    // GNU Screen device control string
    paddingPrint (ESC "P");
  }
}

//----------------------------------------------------------------------
void FTerm::oscPostfix()
{
  static const auto& data = FTermData::getInstance();

  if ( data.isTermType(FTermType::screen)
    || data.isTermType(FTermType::tmux) )
  {
    // GNU Screen/tmux string terminator
    paddingPrint (ESC "\\");
  }
}

//----------------------------------------------------------------------
void FTerm::init_alt_charset()
{
  // Read the used VT100 pairs

  std::unordered_map<uChar, uChar> vt100_alt_char;
  auto& character = FCharMap::getCharEncodeMap();

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
  for (auto&& pair : FCharMap::getDECSpecialGraphics())
  {
    const auto keyChar = uChar(pair.key);
    const auto altChar = wchar_t(vt100_alt_char[keyChar]);
    const auto utf8char = wchar_t(pair.unicode);
    const auto p = std::find_if ( character.begin()
                                , character.end()
                                , [&utf8char] (FCharMap::CharEncodeMap entry)
                                  { return entry.unicode == utf8char; } );
    if ( p != character.end() )  // found in character
    {
      const auto item = std::size_t(std::distance(character.begin(), p));

      if ( altChar )                 // update alternate character set
        FCharMap::getCharacter(character[item], Encoding::VT100) = altChar;
      else                           // delete VT100 char in character
        FCharMap::getCharacter(character[item], Encoding::VT100) = L'\0';
    }
  }
}

//----------------------------------------------------------------------
void FTerm::init_pc_charset()
{
  bool reinit{false};
  static auto& opti_attr = FOptiAttr::getInstance();
  static const auto& data = FTermData::getInstance();

  // rxvt does not support pc charset
  if ( data.isTermType(FTermType::rxvt | FTermType::urxvt) )
    return;

  if ( data.isTermType ( FTermType::gnome_terminal
                             | FTermType::linux_con ) )
  {
    // Fallback if tcap "S2" is not found
    if ( ! TCAP(t_enter_pc_charset_mode) )
    {
      if ( data.hasUTF8Console() )
      {
        // Select iso8859-1 + null mapping
        TCAP(t_enter_pc_charset_mode) = ESC "%@" ESC "(U";
      }
      else
      {
        // Select null mapping
        TCAP(t_enter_pc_charset_mode) = ESC "(U";
      }

      opti_attr.set_enter_pc_charset_mode \
        (TCAP(t_enter_pc_charset_mode));
      reinit = true;
    }

    // Fallback if tcap "S3" is not found
    if ( ! TCAP(t_exit_pc_charset_mode) )
    {
      if ( data.hasUTF8Console() )
      {
        // Select ascii mapping + utf8
        TCAP(t_exit_pc_charset_mode) = ESC "(B" ESC "%G";
      }
      else
      {
        // Select ascii mapping
        TCAP(t_enter_pc_charset_mode) = ESC "(B";
      }

      opti_attr.set_exit_pc_charset_mode \
          (TCAP(t_exit_pc_charset_mode));
      reinit = true;
    }
  }

  if ( reinit )
    opti_attr.initialize();
}

//----------------------------------------------------------------------
void FTerm::init_cygwin_charmap()
{
  // Replace don't printable PC charset characters in a Cygwin terminal

  static auto& data = FTermData::getInstance();

  if ( ! data.isTermType(FTermType::cygwin) )
    return;

  // PC encoding changes
  for (auto&& entry : FCharMap::getCharEncodeMap())
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
  auto& sub_map = data.getCharSubstitutionMap();
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

  static const auto& data = FTermData::getInstance();

  if ( data.isTermType ( FTermType::cygwin
                       | FTermType::putty
                       | FTermType::tera_term
                       | FTermType::rxvt ) )
  {
    FTermcap::max_color = 16;
  }
}

//----------------------------------------------------------------------
void FTerm::init_teraterm_charmap()
{
  // Tera Term can't print ascii characters < 0x20

  if ( ! FTermData::getInstance().isTermType(FTermType::tera_term) )
    return;

  for (auto&& entry : FCharMap::getCharEncodeMap())
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

  static auto& opti_move = FOptiMove::getInstance();
  opti_move.setTermEnvironment(optimove_env);
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

  static auto& opti_attr = FOptiAttr::getInstance();
  opti_attr.setTermEnvironment(optiattr_env);
}

//----------------------------------------------------------------------
bool FTerm::init_font()
{
  if ( getStartOptions().vgafont && ! setVGAFont() )
  {
    setExitMessage("VGAfont is not supported by this terminal");
    FApplication::exit(EXIT_FAILURE);
  }

  if ( getStartOptions().newfont && ! setNewFont() )
  {
    setExitMessage("Newfont is not supported by this terminal");
    FApplication::exit(EXIT_FAILURE);
  }

  return ! FApplication::isQuit();
}

//----------------------------------------------------------------------
void FTerm::init_locale()
{
  // Init current locale

  static const auto& data = FTermData::getInstance();
  const auto& termtype = data.getTermType();
  const char* locale_name = std::setlocale (LC_ALL, "");
  std::setlocale (LC_NUMERIC, "");

  // Get XTERM_LOCALE
  const char* locale_xterm = std::getenv("XTERM_LOCALE");

  // set LC_ALL to XTERM_LOCALE
  if ( locale_xterm )
    locale_name = std::setlocale (LC_ALL, locale_xterm);

  // TeraTerm can not show UTF-8 character
  if ( data.isTermType(FTermType::tera_term)
    && ! std::strcmp(nl_langinfo(CODESET), "UTF-8") )
    locale_name = std::setlocale (LC_ALL, "C");

  // Kterm
  if ( data.isTermType(FTermType::kterm)
    && ! std::strcmp(nl_langinfo(CODESET), "UTF-8") )
    locale_name = std::setlocale (LC_ALL, "C");

  // Sun (color) workstation console can't show UTF-8 character
  if ( termtype.substr(0, 3) == "sun"
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
  // detect encoding

  bool force_vt100{false};  // VT100 line drawing (G1 character set)
  init_encoding_set();
  static const auto& data = FTermData::getInstance();

  if ( data.isTermType(FTermType::rxvt)
    && ! data.isTermType(FTermType::urxvt) )
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

  static auto& data = FTermData::getInstance();
  auto& encoding_list = data.getEncodingList();
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
  static auto& data = FTermData::getInstance();
  const auto& termtype = data.getTermType();
  static const auto& fsys = FSystem::getInstance();

  if ( fsys->isTTY(stdout_no)
    && ! std::strcmp(nl_langinfo(CODESET), "UTF-8") )
  {
    data.setUTF8Console(true);
    data.setTermEncoding (Encoding::UTF8);
    data.setUTF8(true);
    setUTF8(true);
    static auto& keyboard = FKeyboard::getInstance();
    keyboard.enableUTF8();
  }
  else if ( fsys->isTTY(stdout_no)
         && (termtype.length() > 0)
         && (TCAP(t_exit_alt_charset_mode) != nullptr) )
  {
    data.setVT100Console (true);
    data.setTermEncoding (Encoding::VT100);
  }
  else
  {
    data.setASCIIConsole (true);
    data.setTermEncoding (Encoding::ASCII);
  }
}

//----------------------------------------------------------------------
void FTerm::init_individual_term_encoding()
{
  static auto& data = FTermData::getInstance();
  const auto is_utf8 = data.isUTF8();
  const auto is_putty = data.isTermType(FTermType::putty);
  const auto is_teraterm = data.isTermType(FTermType::tera_term);
  const auto is_non_utf8_putty = is_putty && ! is_utf8;
  const auto is_non_utf8_teraterm = is_teraterm && ! is_utf8;

  if ( isNewFont() || is_non_utf8_putty || is_non_utf8_teraterm )
  {
    data.setTermEncoding (Encoding::PC);
  }
}

//----------------------------------------------------------------------
void FTerm::init_force_vt100_encoding()
{
  static auto& data = FTermData::getInstance();
  data.setVT100Console(true);
  data.setTermEncoding (Encoding::VT100);
}

//----------------------------------------------------------------------
void FTerm::init_utf8_without_alt_charset()
{
  // Fall back to ascii for utf-8 terminals that
  // do not support VT100 line drawings

  static auto& data = FTermData::getInstance();

  if ( FTermcap::no_utf8_acs_chars && data.isUTF8()
    && data.getTermEncoding() == Encoding::VT100 )
  {
    data.setASCIIConsole(true);
    data.setTermEncoding (Encoding::ASCII);
  }
}

//----------------------------------------------------------------------
void FTerm::init_tab_quirks()
{
  // In some alternative character sets, a tab character prints a '○'
  // on the terminal and does not move the cursor to the next tab stop
  // position

  const auto& enc = FTermData::getInstance().getTermEncoding();

  if ( enc == Encoding::VT100 || enc == Encoding::PC )
  {
    const char* empty{nullptr};
    static auto& opti_move = FOptiMove::getInstance();
    opti_move.set_tabular (empty);
  }
}

//----------------------------------------------------------------------
void FTerm::init_captureFontAndTitle()
{
  // Save the used xterm font and window title

  if ( ! getStartOptions().terminal_data_request )
    return;

  static auto& data = FTermData::getInstance();
  static auto& xterm = FTermXTerminal::getInstance();
  xterm.captureFontAndTitle();
  const auto& font = xterm.getFont();
  const auto& title = xterm.getTitle();

  if ( ! font.isEmpty() )
    data.setXtermFont(font);

  if ( ! title.isEmpty() )
    data.setXtermTitle(title);
}

//----------------------------------------------------------------------
inline bool FTerm::hasNoFontSettingOption()
{
  static const auto& data = FTermData::getInstance();

  if ( data.isTermType ( FTermType::gnome_terminal
                       | FTermType::kde_konsole
                       | FTermType::putty
                       | FTermType::tera_term
                       | FTermType::cygwin
                       | FTermType::mintty ) )
    return true;

  return false;
}

//----------------------------------------------------------------------
void FTerm::setInsertCursorStyle()
{
  FTermXTerminal::getInstance().setCursorStyle (XTermCursorStyle::BlinkingUnderline);
  setKDECursor(KdeKonsoleCursorShape::Underline);

#if defined(__linux__)
  static auto& linux_console = FTermLinux::getInstance();
  linux_console.setCursorStyle (LinuxConsoleCursorStyle::Underscore);
#elif defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  static auto& freebsd_console = FTermFreeBSD::getInstance();
  freebsd_console.setCursorStyle (FreeBSDConsoleCursorStyle::Destructive);
#endif

  if ( FTermData::getInstance().isTermType(FTermType::urxvt) )
    FTermXTerminal::getInstance().setCursorColor ("rgb:ffff/ffff/ffff");
}

//----------------------------------------------------------------------
void FTerm::setOverwriteCursorStyle()
{
  FTermXTerminal::getInstance().setCursorStyle (XTermCursorStyle::SteadyBlock);
  setKDECursor(KdeKonsoleCursorShape::Block);

#if defined(__linux__)
  static auto& linux_console = FTermLinux::getInstance();
  linux_console.setCursorStyle (LinuxConsoleCursorStyle::FullBlock);
#elif defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  static auto& freebsd_console = FTermFreeBSD::getInstance();
  freebsd_console.setCursorStyle (FreeBSDConsoleCursorStyle::Normal);
#endif

  if ( FTermData::getInstance().isTermType(FTermType::urxvt) )
    FTermXTerminal::getInstance().setCursorColor ("rgb:eeee/0000/0000");
}

//----------------------------------------------------------------------
std::string FTerm::enableCursorString()
{
  // Returns the cursor enable string

  static constexpr std::string::size_type SIZE{32u};
  std::string enable_str{};
  enable_str.reserve(SIZE);
  static const auto& vs = TCAP(t_cursor_visible);
  static const auto& ve = TCAP(t_cursor_normal);

  if ( ve )
    enable_str = ve;
  else if ( vs )
    enable_str = vs;

#if defined(__linux__)
  if ( FTermData::getInstance().isTermType(FTermType::linux_con) )
  {
    // Restore the last used Linux console cursor style
    static auto& linux_console = FTermLinux::getInstance();
    const char* cstyle = linux_console.getCursorStyleString();
    enable_str.append(cstyle);
  }
#endif  // defined(__linux__)

#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  if ( FTermData::getInstance().isTermType(FTermType::freebsd_con) )
  {
    // Restore the last used FreeBSD console cursor style
    static auto& freebsd_console = FTermFreeBSD::getInstance();
    freebsd_console.setCursorStyle (freebsd_console.getCursorStyle());
  }
#endif  // defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)

  return enable_str;
}

//----------------------------------------------------------------------
std::string FTerm::disableCursorString()
{
  // Returns the cursor disable string

  static const auto& vi = TCAP(t_cursor_invisible);

  if ( vi )
    return vi;

  return {};
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
  if ( FTermData::getInstance().isTermType(FTermType::linux_con)
    && openConsole() == 0 )
  {
    if ( FTermLinux::isLinuxConsole() )
      gpm_mouse = true;

    closeConsole();
  }
#endif  // defined(__linux__)

  if ( TCAP(t_key_mouse)
    && ! FTermData::getInstance().isTermType(FTermType::linux_con) )
    xterm_mouse = true;

  static auto& keyboard = FKeyboard::getInstance();
  keyboard.enableMouseSequences();
  static auto& mouse = FMouseControl::getInstance();
  mouse.setMaxWidth (uInt16(getColumnNumber()));
  mouse.setMaxHeight (uInt16(getLineNumber()));
  // Enable the linux general purpose mouse (gpm) server
  mouse.useGpmMouse (gpm_mouse);
  // Enable xterm mouse support
  mouse.useXtermMouse (xterm_mouse);
  mouse.enable();
}

//----------------------------------------------------------------------
inline void FTerm::disableMouse()
{
  // Disable the terminal mouse support

  static auto& keyboard = FKeyboard::getInstance();
  keyboard.disableMouseSequences();
  FMouseControl::getInstance().disable();
}

//----------------------------------------------------------------------
inline void FTerm::enableKeypad()
{
  // Enter 'keyboard_transmit' mode

  if ( TCAP(t_keypad_xmit) )
  {
    paddingPrint (TCAP(t_keypad_xmit));
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
inline void FTerm::disableKeypad()
{
  // Leave 'keyboard_transmit' mode

  if ( TCAP(t_keypad_local) )
  {
    paddingPrint (TCAP(t_keypad_local));
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
inline void FTerm::enableAlternateCharset()
{
  // Enable alternate charset

  if ( TCAP(t_enable_acs) )
  {
    paddingPrint (TCAP(t_enable_acs));
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
inline void FTerm::enableApplicationEscKey()
{
  // switch to application escape key mode

  if ( FTermData::getInstance().isTermType(FTermType::mintty) )
    FTerm::paddingPrint (CSI "?7727h");
}

//----------------------------------------------------------------------
inline void FTerm::disableApplicationEscKey()
{
  // Switch to normal escape key mode

  if ( FTermData::getInstance().isTermType(FTermType::mintty) )
    paddingPrint (CSI "?7727l");
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
    paddingPrint (TCAP(t_save_cursor));
    std::fflush(stdout);
  }

  // Saves the screen and the cursor position
  if ( TCAP(t_enter_ca_mode) )
  {
    paddingPrint (TCAP(t_enter_ca_mode));
    std::fflush(stdout);
    FTermData::getInstance().setAlternateScreenInUse(true);
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
    paddingPrint (TCAP(t_exit_ca_mode));
    std::fflush(stdout);
    FTermData::getInstance().setAlternateScreenInUse(false);
  }

  // restore cursor to position of last save_cursor
  if ( TCAP(t_restore_cursor) )
  {
    paddingPrint (TCAP(t_restore_cursor));
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
  FKeyboard::getInstance().setTermcapMap();

  // Initializes locale information
  init_locale();

  // Detect environment and set encoding
  init_encoding();

  // Enable the terminal mouse support
  enableMouse();

  // Activate meta key sends escape
  if ( FTermData::getInstance().isTermType(FTermType::xterm) )
    FTermXTerminal::getInstance().metaSendsESC(true);

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

  // Set 220 Hz beep (100 ms)
  setBeep(220, 100);

  // Set FTerm signal handler
  setSignalHandler();

  if ( ! getStartOptions().cursor_optimisation )
  {
    FTermData::getInstance().supportCursorOptimisation(false);
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
  static const auto& fsys = FSystem::getInstance();

  // Check if stdin is a tty
  if ( ! fsys->isTTY(FTermios::getStdIn()) )
  {
    setExitMessage("FTerm: Standard input is not a TTY.");
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
    setExitMessage(msg);
    FApplication::exit(EXIT_FAILURE);
    return false;
  }

  // Get output baud rate
  initBaudRate();

  // Terminal detection
  static auto& term_detection = FTermDetection::getInstance();
  term_detection.detect();
  setTermType (term_detection.getTermType().c_str());
  return true;
}

//----------------------------------------------------------------------
void FTerm::initOSspecifics() const
{
#if defined(__linux__)
  static auto& linux_console = FTermLinux::getInstance();
  linux_console.init();  // Initialize Linux console

#if DEBUG
  static auto& data = FTermData::getInstance();
  data.setFramebufferBpp (linux_console.getFramebufferBpp());
#endif

#endif  // defined(__linux__)

#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  static auto& freebsd_console = FTermFreeBSD::getInstance();

  if ( getStartOptions().meta_sends_escape )
    freebsd_console.enableMetaSendsEscape();
  else
    freebsd_console.disableMetaSendsEscape();

  if ( getStartOptions().change_cursorstyle )
    freebsd_console.enableChangeCursorStyle();
  else
    freebsd_console.disableChangeCursorStyle();

  freebsd_console.init();  // Initialize BSD console
#elif defined(__NetBSD__) || defined(__OpenBSD__) || defined(UNIT_TEST)
  static auto& openbsd_console = FTermOpenBSD::getInstance();

  if ( getStartOptions().meta_sends_escape )
    openbsd_console.enableMetaSendsEscape();
  else
    openbsd_console.disableMetaSendsEscape();

  openbsd_console.init();  // Initialize wscons console
#endif
}

//----------------------------------------------------------------------
void FTerm::initTermspecifics() const
{
  static const auto& data = FTermData::getInstance();

  if ( data.isTermType(FTermType::kde_konsole) )
    setKDECursor(KdeKonsoleCursorShape::Underline);

  if ( data.isTermType(FTermType::cygwin) )
    init_cygwin_charmap();

  if ( data.isTermType(FTermType::tera_term) )
    init_teraterm_charmap();
}

//----------------------------------------------------------------------
void FTerm::initBaudRate() const
{
  const int stdout_no = FTermios::getStdOut();
  const uInt baud = FTermios::getBaudRate();
  FTermData::getInstance().setBaudrate(baud);
  static const auto& fsys = FSystem::getInstance();

  if ( fsys->isTTY(stdout_no) )
  {
    static auto& opti_move = FOptiMove::getInstance();
    opti_move.setBaudRate(int(baud));
  }
}

//----------------------------------------------------------------------
void FTerm::finish() const
{
  // Set default signal handler
  resetSignalHandler();

  static const auto& data = FTermData::getInstance();
  static auto& xterm = FTermXTerminal::getInstance();

  if ( data.isTermType(FTermType::xterm)
    && ! data.isTermType(FTermType::rxvt) )
    xterm.resetTitle();

  // Restore the saved termios settings
  FTermios::restoreTTYsettings();

  // Turn off all attributes
  if ( TCAP(t_exit_attribute_mode) )
  {
    paddingPrint (TCAP(t_exit_attribute_mode));
    std::fflush(stdout);
  }

  // Turn off pc charset mode
  if ( TCAP(t_exit_pc_charset_mode) )
  {
    paddingPrint (TCAP(t_exit_pc_charset_mode));
    std::fflush(stdout);
  }

  // Reset xterm color settings to default values
  xterm.resetDefaults();

  // Set xterm full block cursor
  xterm.setCursorStyle (XTermCursorStyle::SteadyBlock);

  // Switch to normal escape key mode
  disableApplicationEscKey();

  finishOSspecifics();

  if ( data.isTermType(FTermType::kde_konsole) )
    setKDECursor(KdeKonsoleCursorShape::Block);

  resetBeep();

  // Disable the terminal mouse support
  if ( getStartOptions().mouse_support )
    disableMouse();

  // Deactivate meta key sends escape
  if ( data.isTermType(FTermType::xterm) )
    xterm.metaSendsESC(false);

  // Switch to the normal screen
  useNormalScreenBuffer();

  // leave 'keyboard_transmit' mode
  disableKeypad();

  finish_encoding();

  if ( data.isNewFont() || data.isVGAFont() )
    resetFont();
}

//----------------------------------------------------------------------
void FTerm::finishOSspecifics() const
{
#if defined(__linux__)
  FTermLinux::getInstance().finish();
#elif defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  FTermFreeBSD::getInstance().finish();
#elif defined(__NetBSD__) || defined(__OpenBSD__) || defined(UNIT_TEST)
  FTermOpenBSD::getInstance().finish();
#endif
}

//----------------------------------------------------------------------
void FTerm::finish_encoding() const
{
#if defined(__linux__)
  static const auto& data = FTermData::getInstance();

  if ( data.isTermType(FTermType::linux_con)
    && data.hasUTF8Console() )
    setUTF8(true);
#endif
}

//----------------------------------------------------------------------
void FTerm::printExitMessage()
{
  // Print exit message
  const auto& exit_message = getExitMessage();

  if ( ! exit_message.empty() )
    std::cerr << "Exit: " << exit_message << std::endl;
}

//----------------------------------------------------------------------
void FTerm::terminalSizeChange()
{
  // Initialize a resize event to the root element
  static auto& fterm_data = FTermData::getInstance();
  fterm_data.setTermResized(true);
}

//----------------------------------------------------------------------
void FTerm::processTermination (int signum)
{
  if ( internal::var::init_term_object )
    internal::var::init_term_object->finish();

  std::fflush (stderr);
  std::fflush (stdout);
  FStringStream msg{};
  msg << "Program stopped: signal " << signum
      << " (" << strsignal(signum) << ")";
  setExitMessage(msg.str());
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
void FTerm::signal_handler (int sig_num)
{
  switch ( sig_num )
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
      processTermination(sig_num);

    default:
      break;
  }
}

}  // namespace finalcut
