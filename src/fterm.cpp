/***********************************************************************
* fterm.cpp - Base class for terminal detection and control            *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2012-2018 Markus Gans                                      *
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

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include "final/fterm.h"
#include "final/fcharmap.h"

namespace finalcut
{

// global FTerm object
static FTerm* init_term_object = 0;

// global init state
static bool term_initialized = false;

// function pointer
int (*FTerm::Fputchar)(int);

// static class attributes
FTerm::initializationValues FTerm::init_values;
FTermData*          FTerm::data             = 0;
FTermcap::tcap_map* FTerm::tcap             = 0;
FOptiMove*          FTerm::opti_move        = 0;
FOptiAttr*          FTerm::opti_attr        = 0;
FTermDetection*     FTerm::term_detection   = 0;
FTermXTerminal*     FTerm::xterm            = 0;
FKeyboard*          FTerm::keyboard         = 0;
FMouseControl*      FTerm::mouse            = 0;

#if defined(__linux__)
  FTermLinux* FTerm::linux = 0;
#elif defined(__FreeBSD__) || defined(__DragonFly__)
  FTermFreeBSD* FTerm::freebsd = 0;
#elif defined(__NetBSD__) || defined(__OpenBSD__)
  FTermOpenBSD* FTerm::openbsd = 0;
#endif

#if DEBUG
  FTermDebugData* FTerm::debug_data = 0;
#endif

// function prototypes
uInt env2uint (const char*);


//----------------------------------------------------------------------
// class FTerm
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FTerm::FTerm (bool disable_alt_screen)
{
  if ( ! term_initialized )
    init (disable_alt_screen);
}

//----------------------------------------------------------------------
FTerm::~FTerm()  // destructor
{
  if ( init_term_object == this )
    finish();  // Resetting console settings
}


// public methods of FTerm
//----------------------------------------------------------------------
std::size_t FTerm::getLineNumber()
{
  FRect& term_geometry = data->getTermGeometry();

  if ( term_geometry.getHeight() == 0 )
    detectTermSize();

  return term_geometry.getHeight();
}

//----------------------------------------------------------------------
std::size_t FTerm::getColumnNumber()
{
  FRect& term_geometry = data->getTermGeometry();

  if ( term_geometry.getWidth() == 0 )
    detectTermSize();

  return term_geometry.getWidth();
}

//----------------------------------------------------------------------
const FString FTerm::getKeyName (FKey keynum)
{
  return keyboard->getKeyName (keynum);
}

//----------------------------------------------------------------------
bool FTerm::isNormal (charData*& ch)
{
  return opti_attr->isNormal(ch);
}

//----------------------------------------------------------------------
bool FTerm::isCursorHideable()
{
  char* cursor_off_str = disableCursor();

  if ( cursor_off_str && std::strlen(cursor_off_str) > 0 )
    return true;

  return false;
}

//----------------------------------------------------------------------
void FTerm::setTermType (const char term_name[])
{
  data->setTermType(term_name);
}

//----------------------------------------------------------------------
void FTerm::setInsertCursor (bool on)
{
  if ( on )
    setInsertCursorStyle();
  else
    setOverwriteCursorStyle();
}

//----------------------------------------------------------------------
void FTerm::redefineDefaultColors (bool on)
{
  xterm->redefineDefaultColors (on);
}

//----------------------------------------------------------------------
void FTerm::setDblclickInterval (const long timeout)
{
  mouse->setDblclickInterval(timeout);
}

//----------------------------------------------------------------------
bool FTerm::setUTF8 (bool on)  // UTF-8 (Unicode)
{
  if ( on == data->isUTF8() )
    return on;

  if ( on )
    data->setUTF8(true);
  else
    data->setUTF8(false);

#if defined(__linux__)
  linux->setUTF8 (on);
#endif

  return data->isUTF8();
}

//----------------------------------------------------------------------
bool FTerm::setVGAFont()
{
  if ( data->isVGAFont() )
    return data->isVGAFont();

  if ( hasNoFontSettingOption() )
    return false;

  if ( isXTerminal() || isScreenTerm()
    || isUrxvtTerminal() || FTermcap::osc_support )
  {
    data->setVGAFont(true);
    // Set font in xterm to vga
    xterm->setFont("vga");
    data->setNewFont(false);
    data->setTermEncoding (fc::PC);

    if ( isXTerminal() && data->hasUTF8Console() )
      Fputchar = &FTerm::putchar_UTF8;
    else
      Fputchar = &FTerm::putchar_ASCII;
  }
#if defined(__linux__)
  else if ( isLinuxTerm() )
  {
    data->setVGAFont(linux->loadVGAFont());
    data->setTermEncoding (fc::PC);
    Fputchar = &FTerm::putchar_ASCII;
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
  if ( isNewFont() )
    return true;

  if ( hasNoFontSettingOption() )
    return false;

  if ( isXTerminal() || isScreenTerm()
    || isUrxvtTerminal() || FTermcap::osc_support )
  {
    data->setNewFont(true);
    // Set font in xterm to 8x16graph
    xterm->setFont("8x16graph");
    data->setTermEncoding (fc::PC);

    if ( isXTerminal() && data->hasUTF8Console() )
      Fputchar = &FTerm::putchar_UTF8;
    else
      Fputchar = &FTerm::putchar_ASCII;
  }
#if defined(__linux__)
  else if ( isLinuxTerm() )
  {
    data->setNewFont(linux->loadNewFont());
    data->setTermEncoding (fc::PC);
    Fputchar = &FTerm::putchar_ASCII;  // function pointer
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
bool FTerm::setOldFont()
{
  bool retval = false;

  if ( ! (data->isNewFont() || data->isVGAFont()) )
    return false;

  retval  = false;
  data->setNewFont(false);
  data->setVGAFont(false);

  if ( isXTerminal() || isScreenTerm()
    || isUrxvtTerminal() || FTermcap::osc_support )
  {
    const FString& font = data->getXtermFont();

    if ( font.getLength() > 2 )
    {
      // restore saved xterm font
      xterm->setFont (font);
    }
    else
    {
      // Set font in xterm to vga
      xterm->setFont("vga");
    }

    retval = true;
  }
#if defined(__linux__)
  else if ( isLinuxTerm() )
  {
    retval = linux->loadOldFont(fc::character);

    if ( retval )
    {
      data->supportShadowCharacter (linux->hasShadowCharacter());
      data->supportHalfBlockCharacter (linux->hasHalfBlockCharacter());
    }
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
  int fd = data->getTTYFileDescriptor();
  char* termfilename = data->getTermFileName();

  static const char* terminal_devices[] =
  {
    "/proc/self/fd/0",
    "/dev/tty",
    "/dev/tty0",
    "/dev/vc/0",
    "/dev/systty",
    "/dev/console",
    0
  };

  if ( fd >= 0 )  // console is already opened
    return 0;

  if ( ! *termfilename )
    return 0;

  for (int i = 0; terminal_devices[i] != 0; i++)
  {
    fd = open(terminal_devices[i], O_RDWR, 0);
    data->setTTYFileDescriptor(fd);

    if ( fd >= 0 )
      return 0;
  }

  return -1;  // No file descriptor referring to the console
}

//----------------------------------------------------------------------
int FTerm::closeConsole()
{
  int fd = data->getTTYFileDescriptor();

  if ( fd < 0 )  // console is already closed
    return 0;

  // use 'close' from the global namespace
  int ret = ::close (fd);
  data->setTTYFileDescriptor(-1);

  if ( ret == 0 )
    return 0;
  else
    return -1;
}

//----------------------------------------------------------------------
char* FTerm::moveCursor (int xold, int yold, int xnew, int ynew)
{
  // Returns the cursor move string

  if ( data->hasCursorOptimisation() )
    return opti_move->moveCursor (xold, yold, xnew, ynew);
  else
    return tgoto(TCAP(fc::t_cursor_address), xnew, ynew);
}

//----------------------------------------------------------------------
char* FTerm::cursorsVisibility (bool on)
{
  // Hides or shows the input cursor on the terminal

  char* visibility_str = 0;

  if ( on == data->isCursorHidden() )
    return 0;

  if ( on )
  {
    visibility_str = disableCursor();

    if ( visibility_str )
      data->setCursorHidden (true);  // Global state
  }
  else
  {
    visibility_str = enableCursor();

    if ( visibility_str )
      data->setCursorHidden (false);  // Global state
  }

  return visibility_str;
}

//----------------------------------------------------------------------
void FTerm::printMoveDurations()
{
  finalcut::printDurations(*opti_move);
}

//----------------------------------------------------------------------
char* FTerm::enableCursor()
{
  // Returns the cursor enable string

  static const std::size_t SIZE = 32;
  static char enable_str[SIZE] = { };
  char*& vs = TCAP(fc::t_cursor_visible);
  char*& ve = TCAP(fc::t_cursor_normal);

  if ( ve )
    std::strncpy (enable_str, ve, SIZE - 1);
  else if ( vs )
    std::strncpy (enable_str, vs, SIZE - 1);

#if defined(__linux__)
  if ( isLinuxTerm() )
  {
    // Restore the last used Linux console cursor style
    char* cstyle;
    cstyle = linux->restoreCursorStyle();
    std::strncat (enable_str, cstyle, SIZE - std::strlen(enable_str) - 1);
  }
#endif  // defined(__linux__)

  enable_str[SIZE - 1] = '\0';

#if defined(__FreeBSD__) || defined(__DragonFly__)
  if ( isFreeBSDTerm() )
  {
    // Restore the last used FreeBSD console cursor style
    freebsd->restoreCursorStyle();
  }
#endif  // defined(__FreeBSD__) || defined(__DragonFly__)

  return enable_str;
}

//----------------------------------------------------------------------
char* FTerm::disableCursor()
{
  // Returns the cursor disable string

  char*& vi = TCAP(fc::t_cursor_invisible);

  if ( vi )
    return vi;

  return 0;
}

//----------------------------------------------------------------------
void FTerm::detectTermSize()
{
  // Detect the terminal width and height

  struct winsize win_size;
  bool close_after_detect = false;
  int fd = data->getTTYFileDescriptor();
  int ret;

  if ( fd < 0 )  // console is closed
  {
    if ( openConsole() != 0 )
      return;

    fd = data->getTTYFileDescriptor();
    close_after_detect = true;
  }

  FRect& term_geometry = data->getTermGeometry();
  ret = ioctl (fd, TIOCGWINSZ, &win_size);

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

  opti_move->setTermSize ( term_geometry.getWidth()
                         , term_geometry.getHeight() );

  if ( close_after_detect )
    closeConsole();
}

//----------------------------------------------------------------------
void FTerm::setTermSize (std::size_t width, std::size_t height)
{
  // Set xterm size to {width} x {height}

  xterm->setTermSize (width, height);
}

//----------------------------------------------------------------------
void FTerm::setTermTitle (const FString& title)
{
  // Set the xterm window title

  xterm->setTitle (title);
}

//----------------------------------------------------------------------
void FTerm::setKDECursor (fc::kdeKonsoleCursorShape style)
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
  linux->saveColorMap();
#endif
}

//----------------------------------------------------------------------
void FTerm::resetColorMap()
{
  char*& oc = TCAP(fc::t_orig_colors);
  char*& op = TCAP(fc::t_orig_pair);

  if ( oc )
    putstring (oc);

#if defined(__linux__)
  else
    linux->resetColorMap();
#endif

  if ( op )
    putstring (op);

  std::fflush(stdout);
}

//----------------------------------------------------------------------
void FTerm::setPalette (FColor index, int r, int g, int b)
{
  // Redefine RGB color value for a palette entry

  char*& Ic = TCAP(fc::t_initialize_color);
  char*& Ip = TCAP(fc::t_initialize_pair);

  index = FOptiAttr::vga2ansi(index);

  if ( Ic || Ip )
  {
    const char* color_str = "";

    int rr = (r * 1001) / 256
      , gg = (g * 1001) / 256
      , bb = (b * 1001) / 256;

    if ( Ic )
      color_str = tparm(Ic, index, rr, gg, bb, 0, 0, 0, 0, 0);
    else if ( Ip )
      color_str = tparm(Ip, index, 0, 0, 0, rr, gg, bb, 0, 0);

    putstring (color_str);
  }
#if defined(__linux__)
  else
  {
    linux->setPalette(index, r, g, b);
  }
#endif

  std::fflush(stdout);
}

//----------------------------------------------------------------------
#if defined(__linux__)
void FTerm::setBeep (int Hz, int ms)
{
  linux->setBeep (Hz, ms);
}
#else
void FTerm::setBeep (int, int)
{ }
#endif  // defined(__linux__)

//----------------------------------------------------------------------
void FTerm::resetBeep()
{
#if defined(__linux__)
  linux->resetBeep();
#endif
}

//----------------------------------------------------------------------
void FTerm::beep()
{
  if ( TCAP(fc::t_bell) )
  {
    putstring (TCAP(fc::t_bell));
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTerm::setEncoding (fc::encoding enc)
{
  data->setTermEncoding (enc);

  assert ( enc == fc::UTF8
        || enc == fc::VT100  // VT100 line drawing
        || enc == fc::PC     // CP-437
        || enc == fc::ASCII );

  // Set the new Fputchar function pointer
  switch ( enc )
  {
    case fc::UTF8:
      Fputchar = &FTerm::putchar_UTF8;
      break;

    case fc::VT100:
    case fc::PC:
      if ( isXTerminal() && data->hasUTF8Console() )
        Fputchar = &FTerm::putchar_UTF8;
      else
        Fputchar = &FTerm::putchar_ASCII;
      break;

    case fc::ASCII:
    case fc::UNKNOWN:
    case fc::NUM_OF_ENCODINGS:
      Fputchar = &FTerm::putchar_ASCII;
  }

  if ( isLinuxTerm() )
  {
    if ( enc == fc::VT100 || enc == fc::PC )
    {
      char* empty = 0;
      opti_move->set_tabular (empty);
    }
    else
      opti_move->set_tabular (TCAP(fc::t_tab));
  }
}

//----------------------------------------------------------------------
fc::encoding FTerm::getEncoding()
{
  return data->getTermEncoding();
}

//----------------------------------------------------------------------
std::string FTerm::getEncodingString()
{
  fc::encoding term_encoding = data->getTermEncoding();
  FTermData::encodingMap& encoding_list = data->getEncodingList();
  std::map<std::string, fc::encoding>::const_iterator it, end;
  end = encoding_list.end();

  for (it = encoding_list.begin(); it != end; ++it )
    if ( it->second == term_encoding )
      return it->first;

  return "";
}

//----------------------------------------------------------------------
bool FTerm::charEncodable (uInt c)
{
  uInt ch = charEncode(c);
  return bool(ch > 0 && ch != c);
}

//----------------------------------------------------------------------
uInt FTerm::charEncode (uInt c)
{
  return charEncode (c, data->getTermEncoding());
}

//----------------------------------------------------------------------
uInt FTerm::charEncode (uInt c, fc::encoding enc)
{
  for (uInt i = 0; i <= uInt(fc::lastCharItem); i++)
  {
    if ( fc::character[i][fc::UTF8] == c )
    {
      c = fc::character[i][enc];
      break;
    }
  }

  return c;
}

//----------------------------------------------------------------------
bool FTerm::scrollTermForward()
{
  if ( TCAP(fc::t_scroll_forward) )
  {
    putstring (TCAP(fc::t_scroll_forward));
    std::fflush(stdout);
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
bool FTerm::scrollTermReverse()
{
  if ( TCAP(fc::t_scroll_reverse) )
  {
    putstring (TCAP(fc::t_scroll_reverse));
    std::fflush(stdout);
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
void FTerm::putstringf (const char format[], ...)
{
  assert ( format != 0 );
  char  buf[512];
  char* buffer;
  va_list args;

  buffer = buf;
  va_start (args, format);
  vsnprintf (buffer, sizeof(buf), format, args);
  va_end (args);

  tputs (buffer, 1, FTerm::putchar_ASCII);
}

//----------------------------------------------------------------------
void FTerm::putstring (const char s[], int affcnt)
{
#if defined(__sun) && defined(__SVR4)
  tputs (C_STR(s), affcnt, FTerm::putchar_ASCII);
#else
  tputs (s, affcnt, FTerm::putchar_ASCII);
#endif
}

#if defined(__sun) && defined(__SVR4)
//----------------------------------------------------------------------
int FTerm::putchar_ASCII (char c)
{
  if ( std::putchar(c) == EOF )
    return 0;
  else
    return 1;
}
#endif  // defined(__sun) && defined(__SVR4)

//----------------------------------------------------------------------
int FTerm::putchar_ASCII (int c)
{
  if ( std::putchar(char(c)) == EOF )
    return 0;
  else
    return 1;
}

//----------------------------------------------------------------------
int FTerm::putchar_UTF8 (int c)
{
  if ( c < 0x80 )
  {
    // 1 Byte (7-bit): 0xxxxxxx
    std::putchar (c);
    return 1;
  }
  else if ( c < 0x800 )
  {
    // 2 byte (11-bit): 110xxxxx 10xxxxxx
    std::putchar (0xc0 | (c >> 6) );
    std::putchar (0x80 | (c & 0x3f) );
    return 2;
  }
  else if ( c < 0x10000 )
  {
    // 3 byte (16-bit): 1110xxxx 10xxxxxx 10xxxxxx
    std::putchar (0xe0 | (c >> 12) );
    std::putchar (0x80 | ((c >> 6) & 0x3f) );
    std::putchar (0x80 | (c & 0x3f) );
    return 3;
  }
  else if ( c < 0x200000 )
  {
    // 4 byte (21-bit): 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    std::putchar (0xf0 | (c >> 18) );
    std::putchar (0x80 | ((c >> 12) & 0x3f) );
    std::putchar (0x80 | ((c >> 6) & 0x3f) );
    std::putchar (0x80 | (c & 0x3f));
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
  linux->initCharMap (fc::character);
  data->supportShadowCharacter (linux->hasShadowCharacter());
  data->supportHalfBlockCharacter (linux->hasHalfBlockCharacter());
#elif defined(__FreeBSD__) || defined(__DragonFly__)
  freebsd->initCharMap (fc::character);
#endif

  // set xterm underline cursor
  xterm->setCursorStyle (fc::blinking_underline);

  // set xterm color settings to defaults
  xterm->setDefaults();
}

//----------------------------------------------------------------------
char* FTerm::changeAttribute ( charData*& term_attr
                             , charData*& next_attr )
{
  return opti_attr->changeAttribute (term_attr, next_attr);
}

//----------------------------------------------------------------------
void FTerm::exitWithMessage (const FString& message)
{
  // Exit the programm
  if ( init_term_object )
    init_term_object->finish();

  std::fflush (stderr);
  std::fflush (stdout);

  if ( ! message.isEmpty() )
    std::cerr << "Warning: " << message << std::endl;

  std::exit (EXIT_FAILURE);
}


// private methods of FTerm
//----------------------------------------------------------------------
void FTerm::init_global_values (bool disable_alt_screen)
{
  // Initialize global values

  // Preset to false
  data->setNewFont(false);

  // Sets alternative screen usage
  data->useAlternateScreen(! disable_alt_screen);

  // Initialize xterm object
  xterm->setFTermDetection(term_detection);

  if ( ! init_values.terminal_detection )
    term_detection->setTerminalDetection (false);

#if DEBUG
  debug_data->setFTermDetection(term_detection);
  debug_data->setFTermData(data);
#endif
}

//----------------------------------------------------------------------
void FTerm::init_terminal_device_path()
{
  char termfilename[256] = { };
  int  stdout_no = FTermios::getStdOut();

  if ( ttyname_r(stdout_no, termfilename, sizeof(termfilename)) )
    termfilename[0] = '\0';

  data->setTermFileName(termfilename);
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

  std::map <uChar,uChar> vt100_alt_char;

  if ( TCAP(fc::t_acs_chars) )
  {
    for (int n = 0; TCAP(fc::t_acs_chars)[n]; n += 2)
    {
      // insert the VT100 key/value pairs into a map
      uChar p1 = uChar(TCAP(fc::t_acs_chars)[n]);
      uChar p2 = uChar(TCAP(fc::t_acs_chars)[n + 1]);
      vt100_alt_char[p1] = p2;
    }
  }

  enum column
  {
    vt100_key = 0,
    utf8_char = 1
  };

  // Update array 'character' with discovered VT100 pairs
  for (int n = 0; n <= fc::lastKeyItem; n++ )
  {
    uChar keyChar = uChar(fc::vt100_key_to_utf8[n][vt100_key]);
    uChar altChar = uChar(vt100_alt_char[keyChar]);
    uInt utf8char = uInt(fc::vt100_key_to_utf8[n][utf8_char]);
    fc::encoding num = fc::NUM_OF_ENCODINGS;

    uInt* p = std::find ( fc::character[0]
                        , fc::character[fc::lastCharItem] + num
                        , utf8char );
    if ( p != fc::character[fc::lastCharItem] + num )  // found in character
    {
      int item = int(std::distance(fc::character[0], p) / num);

      if ( altChar )                 // update alternate character set
        fc::character[item][fc::VT100] = altChar;
      else                           // delete VT100 char in character
        fc::character[item][fc::VT100] = 0;
    }
  }
}

//----------------------------------------------------------------------
void FTerm::init_pc_charset()
{
  bool reinit = false;

  // rxvt does not support pc charset
  if ( isRxvtTerminal() || isUrxvtTerminal() )
    return;

  if ( isGnomeTerminal() || isLinuxTerm() )
  {
    // Fallback if tcap "S2" is not found
    if ( ! TCAP(fc::t_enter_pc_charset_mode) )
    {
      if ( data->hasUTF8Console() )
      {
        // Select iso8859-1 + null mapping
        TCAP(fc::t_enter_pc_charset_mode) = \
            C_STR(ESC "%@" ESC "(U");
      }
      else
      {
        // Select null mapping
        TCAP(fc::t_enter_pc_charset_mode) = \
            C_STR(ESC "(U");
      }

      opti_attr->set_enter_pc_charset_mode \
        (TCAP(fc::t_enter_pc_charset_mode));
      reinit = true;
    }

    // Fallback if tcap "S3" is not found
    if ( ! TCAP(fc::t_exit_pc_charset_mode) )
    {
      if ( data->hasUTF8Console() )
      {
        // Select ascii mapping + utf8
        TCAP(fc::t_exit_pc_charset_mode) = \
            C_STR(ESC "(B" ESC "%G");
      }
      else
      {
        // Select ascii mapping
        TCAP(fc::t_enter_pc_charset_mode) = \
            C_STR(ESC "(B");
      }

      opti_attr->set_exit_pc_charset_mode \
          (TCAP(fc::t_exit_pc_charset_mode));
      reinit = true;
    }
  }

  if ( reinit )
    opti_attr->initialize();
}

//----------------------------------------------------------------------
void FTerm::init_cygwin_charmap()
{
  // Replace don't printable characters in a Cygwin terminal

  if ( ! isCygwinTerminal() )
    return;

  for (int i = 0; i <= fc::lastCharItem; i++ )
  {
    if ( fc::character[i][fc::UTF8] == fc::BlackUpPointingTriangle    // ▲
      || fc::character[i][fc::UTF8] == fc::BlackDownPointingTriangle  // ▼
      || fc::character[i][fc::UTF8] == fc::SquareRoot )  // SquareRoot √
      fc::character[i][fc::PC] = fc::character[i][fc::ASCII];
  }
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

  for (int i = 0; i <= fc::lastCharItem; i++ )
    if ( fc::character[i][fc::PC] < 0x20 )
      fc::character[i][fc::PC] = fc::character[i][fc::ASCII];
}

//----------------------------------------------------------------------
void FTerm::init_keyboard()
{
#if defined(__linux__)
  keyboard->setFTermLinux (linux);
#endif
}

//----------------------------------------------------------------------
void FTerm::init_termcap()
{
  // Initialize the terminal capabilities

  FTermcap termcap;
  termcap.setFTermData(data);
  termcap.setFTermDetection(term_detection);
  termcap.init();

  // Share the terminal capabilities
  tcap = termcap.getTermcapMap();
}

//----------------------------------------------------------------------
void FTerm::init_quirks()
{
  // Initialize terminal quirks

  FTermcapQuirks quirks;
  quirks.setFTermData (data);
  quirks.setFTermDetection (term_detection);
  quirks.terminalFixup();  // Fix terminal quirks
}

//----------------------------------------------------------------------
void FTerm::init_optiMove()
{
  // Duration precalculation of the cursor movement strings

  FOptiMove::termEnv optimove_env =
  {
    FTermcap::automatic_left_margin,
    FTermcap::eat_nl_glitch,
    FTermcap::tabstop,
    TCAP(fc::t_cursor_home),
    TCAP(fc::t_carriage_return),
    TCAP(fc::t_cursor_to_ll),
    TCAP(fc::t_tab),
    TCAP(fc::t_back_tab),
    TCAP(fc::t_cursor_up),
    TCAP(fc::t_cursor_down),
    TCAP(fc::t_cursor_left),
    TCAP(fc::t_cursor_right),
    TCAP(fc::t_cursor_address),
    TCAP(fc::t_column_address),
    TCAP(fc::t_row_address),
    TCAP(fc::t_parm_up_cursor),
    TCAP(fc::t_parm_down_cursor),
    TCAP(fc::t_parm_left_cursor),
    TCAP(fc::t_parm_right_cursor),
    TCAP(fc::t_erase_chars),
    TCAP(fc::t_repeat_char),
    TCAP(fc::t_clr_bol),
    TCAP(fc::t_clr_eol)
  };

  opti_move->setTermEnvironment(optimove_env);
}

//----------------------------------------------------------------------
void FTerm::init_optiAttr()
{
  // Setting video attribute optimization

  FOptiAttr::termEnv optiattr_env =
  {
    FTermcap::ansi_default_color,
    FTermcap::max_color,
    FTermcap::attr_without_color,
    TCAP(fc::t_enter_bold_mode),
    TCAP(fc::t_exit_bold_mode),
    TCAP(fc::t_enter_dim_mode),
    TCAP(fc::t_exit_dim_mode),
    TCAP(fc::t_enter_italics_mode),
    TCAP(fc::t_exit_italics_mode),
    TCAP(fc::t_enter_underline_mode),
    TCAP(fc::t_exit_underline_mode),
    TCAP(fc::t_enter_blink_mode),
    TCAP(fc::t_exit_blink_mode),
    TCAP(fc::t_enter_reverse_mode),
    TCAP(fc::t_exit_reverse_mode),
    TCAP(fc::t_enter_standout_mode),
    TCAP(fc::t_exit_standout_mode),
    TCAP(fc::t_enter_secure_mode),
    TCAP(fc::t_exit_secure_mode),
    TCAP(fc::t_enter_protected_mode),
    TCAP(fc::t_exit_protected_mode),
    TCAP(fc::t_enter_crossed_out_mode),
    TCAP(fc::t_exit_crossed_out_mode),
    TCAP(fc::t_enter_dbl_underline_mode),
    TCAP(fc::t_exit_dbl_underline_mode),
    TCAP(fc::t_set_attributes),
    TCAP(fc::t_exit_attribute_mode),
    TCAP(fc::t_enter_alt_charset_mode),
    TCAP(fc::t_exit_alt_charset_mode),
    TCAP(fc::t_enter_pc_charset_mode),
    TCAP(fc::t_exit_pc_charset_mode),
    TCAP(fc::t_set_a_foreground),
    TCAP(fc::t_set_a_background),
    TCAP(fc::t_set_foreground),
    TCAP(fc::t_set_background),
    TCAP(fc::t_orig_pair),
    TCAP(fc::t_orig_pair),
    TCAP(fc::t_orig_colors)
  };

  opti_attr->setTermEnvironment(optiattr_env);
}

//----------------------------------------------------------------------
void FTerm::init_font()
{
  if ( init_values.vgafont )
  {
    bool ret = setVGAFont();

    if ( ! ret )
      exitWithMessage ("VGAfont is not supported by this terminal");
  }

  if ( init_values.newfont )
  {
    bool ret = setNewFont();

    if ( ! ret )
      exitWithMessage ("Newfont is not supported by this terminal");
  }
}

//----------------------------------------------------------------------
void FTerm::init_locale()
{
  // Init current locale

  char* locale_name;
  char* locale_xterm;
  const char* termtype = data->getTermType();
  locale_name = std::setlocale (LC_ALL, "");
  std::setlocale (LC_NUMERIC, "");

  // Get XTERM_LOCALE
  locale_xterm = std::getenv("XTERM_LOCALE");

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
  if ( std::strncmp(termtype, "sun", 3) == 0
    && ! std::strcmp(nl_langinfo(CODESET), "UTF-8") )
    locale_name = std::setlocale (LC_ALL, "C");

  // Try to found a meaningful content for locale_name
  if ( locale_name )
    locale_name = std::setlocale (LC_CTYPE, 0);
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
  // detect encoding and set the Fputchar function pointer

  bool force_vt100 = false;  // VT100 line drawing (G1 character set)
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

  if ( init_values.encoding != fc::UNKNOWN )
  {
    setEncoding(init_values.encoding);
  }
}

//----------------------------------------------------------------------
inline void FTerm::init_encoding_set()
{
  // Define the encoding set

  FTermData::encodingMap& encoding_list = data->getEncodingList();

  encoding_list["UTF8"]  = fc::UTF8;
  encoding_list["UTF-8"] = fc::UTF8;
  encoding_list["VT100"] = fc::VT100;  // VT100 line drawing
  encoding_list["PC"]    = fc::PC;     // CP-437
  encoding_list["ASCII"] = fc::ASCII;
}

//----------------------------------------------------------------------
void FTerm::init_term_encoding()
{
  int stdout_no = FTermios::getStdOut();
  const char* termtype = data->getTermType();

  if ( isatty(stdout_no)
    && ! std::strcmp(nl_langinfo(CODESET), "UTF-8") )
  {
    data->setUTF8Console(true);
    data->setTermEncoding (fc::UTF8);
    Fputchar = &FTerm::putchar_UTF8;  // function pointer
    data->setUTF8(true);
    setUTF8(true);
    keyboard->enableUTF8();
  }
  else if ( isatty(stdout_no)
         && (std::strlen(termtype) > 0)
         && (TCAP(fc::t_exit_alt_charset_mode) != 0) )
  {
    data->setVT100Console (true);
    data->setTermEncoding (fc::VT100);
    Fputchar = &FTerm::putchar_ASCII;  // function pointer
  }
  else
  {
    data->setASCIIConsole (true);
    data->setTermEncoding (fc::ASCII);
    Fputchar = &FTerm::putchar_ASCII;  // function pointer
  }
}

//----------------------------------------------------------------------
void FTerm::init_individual_term_encoding()
{
  if ( isLinuxTerm()
    || isCygwinTerminal()
    || isNewFont()
    || (isPuttyTerminal() && ! data->isUTF8())
    || (isTeraTerm() && ! data->isUTF8()) )
  {
    data->setTermEncoding (fc::PC);
    Fputchar = &FTerm::putchar_ASCII;  // function pointer

    if ( hasUTF8() )
    {
      if ( isLinuxTerm() )
        setUTF8(false);
      else if ( isXTerminal() )
        Fputchar = &FTerm::putchar_UTF8;  // function pointer
    }
  }
}

//----------------------------------------------------------------------
void FTerm::init_force_vt100_encoding()
{
  data->setVT100Console(true);
  data->setTermEncoding (fc::VT100);
  Fputchar = &FTerm::putchar_ASCII;  // function pointer
}

//----------------------------------------------------------------------
void FTerm::init_utf8_without_alt_charset()
{
  // Fall back to ascii for utf-8 terminals that
  // do not support VT100 line drawings

  if ( FTermcap::no_utf8_acs_chars && data->isUTF8()
    && data->getTermEncoding() == fc::VT100 )
  {
    data->setASCIIConsole(true);
    data->setTermEncoding (fc::ASCII);
    Fputchar = &FTerm::putchar_ASCII;  // function pointer
  }
}

//----------------------------------------------------------------------
void FTerm::init_tab_quirks()
{
  // In some alternative character sets, a tab character prints a '○'
  // on the terminal and does not move the cursor to the next tab stop
  // position

  fc::encoding enc = data->getTermEncoding();

  if ( enc == fc::VT100 || enc == fc::PC )
  {
    char* empty = 0;
    opti_move->set_tabular (empty);
  }
}

//----------------------------------------------------------------------
void FTerm::init_captureFontAndTitle()
{
  // Save the used xterm font and window title

  xterm->captureFontAndTitle();
  const FString* font = xterm->getFont();
  const FString* title = xterm->getTitle();

  if ( font )
    data->setXtermFont(*font);

  if ( title )
    data->setXtermTitle(*title);
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
inline bool FTerm::canChangeColorPalette()
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
void FTerm::redefineColorPalette()
{
  // Redefine the color palette

  if ( ! canChangeColorPalette() )
    return;

  resetColorMap();
  saveColorMap();

  if ( getMaxColor() >= 16 )
    FColorPalette::set16ColorPalette (FTerm::setPalette);
  else  // 8 colors
    FColorPalette::set8ColorPalette (FTerm::setPalette);
}

//----------------------------------------------------------------------
void FTerm::restoreColorPalette()
{
  if ( ! canChangeColorPalette() )
    return;

  // Reset screen settings
  if ( getMaxColor() >= 16 )
    FColorPalette::reset16ColorPalette (FTerm::setPalette);
  else  // 8 colors
    FColorPalette::reset8ColorPalette (FTerm::setPalette);

  xterm->resetColorMap();
  resetColorMap();
}

//----------------------------------------------------------------------
void FTerm::setInsertCursorStyle()
{
  xterm->setCursorStyle (fc::blinking_underline);
  setKDECursor(fc::UnderlineCursor);

#if defined(__linux__)
  char* cstyle;
  cstyle = linux->setCursorStyle ( fc::underscore_cursor
                                 , data->isCursorHidden() );
  putstring (cstyle);
  std::fflush(stdout);
#elif defined(__FreeBSD__) || defined(__DragonFly__)
  freebsd->setCursorStyle ( fc::destructive_cursor
                          , data->isCursorHidden() );
#endif

  if ( isUrxvtTerminal() )
    xterm->setCursorColor ("rgb:ffff/ffff/ffff");
}

//----------------------------------------------------------------------
void FTerm::setOverwriteCursorStyle()
{
  xterm->setCursorStyle (fc::steady_block);
  setKDECursor(fc::BlockCursor);

#if defined(__linux__)
  char* cstyle;
  cstyle = linux->setCursorStyle ( fc::full_block_cursor
                                 , data->isCursorHidden() );
  putstring (cstyle);
  std::fflush(stdout);
#elif defined(__FreeBSD__) || defined(__DragonFly__)
  freebsd->setCursorStyle ( fc::normal_cursor
                          , data->isCursorHidden() );
#endif

  if ( isUrxvtTerminal() )
    xterm->setCursorColor ("rgb:eeee/0000/0000");
}

//----------------------------------------------------------------------
void FTerm::enableMouse()
{
  // Enable the terminal mouse support

  if ( ! init_values.mouse_support )
    return;

  bool gpm_mouse = false;
  bool xterm_mouse = false;

#if defined(__linux__)
  if ( isLinuxTerm() && openConsole() == 0 )
  {
    if ( linux->isLinuxConsole() )
      gpm_mouse = true;

    closeConsole();
  }
#endif  // defined(__linux__)

  if ( TCAP(fc::t_key_mouse) && ! isLinuxTerm() )
    xterm_mouse = true;

  keyboard->enableMouseSequences();
  mouse->setMaxWidth (short(getColumnNumber()));
  mouse->setMaxHeight (short(getLineNumber()));
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

  keyboard->disableMouseSequences();
  mouse->disable();
}

//----------------------------------------------------------------------
inline void FTerm::enableKeypad()
{
  // Enter 'keyboard_transmit' mode

  if ( TCAP(fc::t_keypad_xmit) )
  {
    putstring (TCAP(fc::t_keypad_xmit));
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
inline void FTerm::disableKeypad()
{
  // Leave 'keyboard_transmit' mode

  if ( TCAP(fc::t_keypad_local) )
  {
    putstring (TCAP(fc::t_keypad_local));
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
inline void FTerm::enableAlternateCharset()
{
  // Enable alternate charset

  if ( TCAP(fc::t_enable_acs) )
  {
    putstring (TCAP(fc::t_enable_acs));
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
  if ( ! hasAlternateScreen() )
    return;

  // Save current cursor position
  if ( TCAP(fc::t_save_cursor) )
  {
    putstring (TCAP(fc::t_save_cursor));
    std::fflush(stdout);
  }

  // Saves the screen and the cursor position
  if ( TCAP(fc::t_enter_ca_mode) )
  {
    putstring (TCAP(fc::t_enter_ca_mode));
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTerm::useNormalScreenBuffer()
{
  if ( ! hasAlternateScreen() )
    return;

  // restores the screen and the cursor position
  if ( TCAP(fc::t_exit_ca_mode) )
  {
    putstring (TCAP(fc::t_exit_ca_mode));
    std::fflush(stdout);
  }

  // restore cursor to position of last save_cursor
  if ( TCAP(fc::t_restore_cursor) )
  {
    putstring (TCAP(fc::t_restore_cursor));
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
inline void FTerm::allocationValues()
{
  try
  {
    data           = new FTermData();
    opti_move      = new FOptiMove();
    opti_attr      = new FOptiAttr();
    term_detection = new FTermDetection();
    xterm          = new FTermXTerminal();
    keyboard       = new FKeyboard();
    mouse          = new FMouseControl();

#if defined(__linux__)
    linux          = new FTermLinux();
#elif defined(__FreeBSD__) || defined(__DragonFly__)
    freebsd        = new FTermFreeBSD();
#elif defined(__NetBSD__) || defined(__OpenBSD__)
    openbsd        = new FTermOpenBSD();
#endif

#if DEBUG
    debug_data     = new FTermDebugData();
#endif
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
    std::abort();
  }
}

//----------------------------------------------------------------------
inline void FTerm::deallocationValues()
{
#if DEBUG
  if ( debug_data )
    delete debug_data;
#endif

#if defined(__NetBSD__) || defined(__OpenBSD__)
  if ( openbsd )
    delete openbsd;
#elif defined(__FreeBSD__) || defined(__DragonFly__)
  if ( freebsd )
    delete freebsd;
#elif defined(__linux__)
  if ( linux )
    delete linux;
#endif

  if ( mouse )
    delete mouse;

  if ( keyboard )
    delete keyboard;

  if ( xterm )
    delete xterm;

  if ( term_detection )
    delete term_detection;

  if ( opti_attr )
    delete opti_attr;

  if ( opti_move )
    delete opti_move;

  if ( data )
    delete data;
}

//----------------------------------------------------------------------
void FTerm::init (bool disable_alt_screen)
{
  init_term_object = this;

  // Initialize global values for all objects
  allocationValues();
  init_global_values(disable_alt_screen);

  // Initialize termios
  FTermios::init();

  // Get pathname of the terminal device
  init_terminal_device_path();

  // Initialize Linux or *BSD console
  initOSspecifics();

  // Save termios settings
  FTermios::storeTTYsettings();

  // Get output baud rate
  initBaudRate();

  // Terminal detection
  term_detection->setFTermData(data);
  term_detection->detect();
  setTermType (term_detection->getTermType());

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
  keyboard->setTermcapMap (fc::Fkey);

  // Initializes locale information
  init_locale();

  // Detect environment and set encoding
  init_encoding();

  // Initializes keyboard settings
  init_keyboard();

  // Enable the terminal mouse support
  enableMouse();

  // Activate meta key sends escape
  if ( isXTerminal() )
    xterm->metaSendsESC(true);

  // switch to application escape key mode
  enableApplicationEscKey();

  // Enter 'keyboard_transmit' mode
  enableKeypad();

  useAlternateScreenBuffer();

  // Enable alternate charset
  enableAlternateCharset();

  // Save the used xterm font and window title
  init_captureFontAndTitle();

  // KDE terminal cursor and cygwin + teraterm charmap correction
  initTermspecifics();

  // Redefine the color palette
  if ( init_values.color_change )
    redefineColorPalette();

  // Set 200 Hz beep (100 ms)
  setBeep(200, 100);

  // Set FTerm signal handler
  setSignalHandler();

  if ( ! init_values.cursor_optimisation )
    data->supportCursorOptimisation(false);

  // Activate the VGA or the new graphic font
  // (depending on the initialization values)
  init_font();

  // Turn off hardware echo
  FTermios::unsetHardwareEcho();

  // Switch to the raw mode
  FTermios::setRawMode();

  // The terminal is now initialized
  term_initialized = true;
}

//----------------------------------------------------------------------
void FTerm::initOSspecifics()
{
#if defined(__linux__)
  linux->setFTermDetection(term_detection);
  linux->init();    // Initialize Linux console

#if DEBUG
  data->setFramebufferBpp (linux->getFramebufferBpp());
#endif

#endif  // defined(__linux__)

#if defined(__FreeBSD__) || defined(__DragonFly__)
  if ( init_values.meta_sends_escape )
    freebsd->enableMetaSendsEscape();
  else
    freebsd->disableMetaSendsEscape();

  if ( init_values.change_cursorstyle )
    freebsd->enableChangeCursorStyle();
  else
    freebsd->disableChangeCursorStyle();

  freebsd->init();  // Initialize BSD console
#elif defined(__NetBSD__) || defined(__OpenBSD__)
  if ( init_values.meta_sends_escape )
    openbsd->enableMetaSendsEscape();
  else
    openbsd->disableMetaSendsEscape();

  openbsd->init();  // Initialize wscons console
#endif
}

//----------------------------------------------------------------------
void FTerm::initTermspecifics()
{
  if ( isKdeTerminal() )
    setKDECursor(fc::UnderlineCursor);

  if ( isCygwinTerminal() )
    init_cygwin_charmap();

  if ( isTeraTerm() )
    init_teraterm_charmap();
}

//----------------------------------------------------------------------
void FTerm::initBaudRate()
{
  int stdout_no = FTermios::getStdOut();
  uInt baud = FTermios::getBaudRate();
  data->setBaudrate(baud);

  if ( isatty(stdout_no) )
    opti_move->setBaudRate(int(baud));
}

//----------------------------------------------------------------------
void FTerm::finish()
{
  // Set default signal handler

  const FString& title = data->getXtermTitle();
  resetSignalHandler();

  if ( title && isXTerminal() && ! isRxvtTerminal() )
    setTermTitle (title);

  // Restore the saved termios settings
  FTermios::restoreTTYsettings();

  // Turn off all attributes
  if ( TCAP(fc::t_exit_attribute_mode) )
  {
    putstring (TCAP(fc::t_exit_attribute_mode));
    std::fflush(stdout);
  }

  // Turn off pc charset mode
  if ( TCAP(fc::t_exit_pc_charset_mode) )
  {
    putstring (TCAP(fc::t_exit_pc_charset_mode));
    std::fflush(stdout);
  }

  // Reset xterm color settings to default values
  xterm->resetDefaults();

  // Set xterm full block cursor
  xterm->setCursorStyle (fc::steady_block);

  // Restore the color palette
  if ( init_values.color_change )
    restoreColorPalette();

  // Switch to normal escape key mode
  disableApplicationEscKey();

  finishOSspecifics1();

  if ( isKdeTerminal() )
    setKDECursor(fc::BlockCursor);

  resetBeep();

  // Disable the terminal mouse support
  if ( init_values.mouse_support )
    disableMouse();

  // Deactivate meta key sends escape
  if ( isXTerminal() )
    xterm->metaSendsESC(false);

  useNormalScreenBuffer();

  // leave 'keyboard_transmit' mode
  disableKeypad();

  finish_encoding();

  if ( data->isNewFont() || data->isVGAFont() )
    setOldFont();

  deallocationValues();
}

//----------------------------------------------------------------------
void FTerm::finishOSspecifics1()
{
#if defined(__linux__)
  linux->finish();
#elif defined(__FreeBSD__) || defined(__DragonFly__)
  freebsd->finish();
#elif defined(__NetBSD__) || defined(__OpenBSD__)
  openbsd->finish();
#endif
}

//----------------------------------------------------------------------
void FTerm::finish_encoding()
{
#if defined(__linux__)
  if ( isLinuxTerm() && data->hasUTF8Console() )
    setUTF8(true);
#endif
}

//----------------------------------------------------------------------
uInt FTerm::cp437_to_unicode (uChar c)
{
  uInt ucs = uInt(c);

  for (uInt i = 0; i <= fc::lastCP437Item; i++)
  {
    if ( fc::cp437_to_ucs[i][0] == c )  // found
    {
      ucs = fc::cp437_to_ucs[i][1];
      break;
    }
  }

  return ucs;
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
      if ( ! data )
        break;
      else if ( data->hasTermResized() )
        break;

      // initialize a resize event to the root element
      data->setTermResized(true);
      break;

    case SIGTERM:
    case SIGQUIT:
    case SIGINT:
    case SIGABRT:
    case SIGILL:
    case SIGSEGV:
      init_term_object->finish();
      std::fflush (stderr);
      std::fflush (stdout);
      std::fprintf ( stderr
                   , "\nProgram stopped: signal %d (%s)\n"
                   , signum
                   , strsignal(signum) );
      std::terminate();
  }

}

// FTerm non-member functions
//----------------------------------------------------------------------
uInt env2uint (const char* env)
{
  FString str(env);

  if ( str.isEmpty() )
    return 0;

  try
  {
    return str.toUInt();
  }
  catch (const std::exception&)
  {
    return 0;
  }
}

}  // namespace finalcut
