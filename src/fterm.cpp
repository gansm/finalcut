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
#include "final/ftcap_map.h"

namespace finalcut
{

// global FTerm object
static FTerm* init_term_object = 0;

// global init state
static bool term_initialized = false;

// function pointer
int (*FTerm::Fputchar)(int);

// static class attributes
int      FTerm::fd_tty;
uInt     FTerm::baudrate;
bool     FTerm::resize_term;
bool     FTerm::monochron;
bool     FTerm::pc_charset_console;
bool     FTerm::utf8_state;
bool     FTerm::utf8_console;
bool     FTerm::utf8_linux_terminal;
bool     FTerm::force_vt100;  // VT100 line drawing
bool     FTerm::vt100_console;
bool     FTerm::ascii_console;
bool     FTerm::NewFont;
bool     FTerm::VGAFont;
bool     FTerm::shadow_character;
bool     FTerm::half_block_character;
bool     FTerm::cursor_optimisation;
bool     FTerm::hidden_cursor;
bool     FTerm::use_alternate_screen = true;
char     FTerm::termtype[256] = { };
char     FTerm::termfilename[256] = { };
#if DEBUG
int      FTerm::framebuffer_bpp = -1;
#endif
char*    FTerm::locale_name  = 0;
char*    FTerm::locale_xterm = 0;
FRect*   FTerm::term         = 0;

char                   FTerm::exit_message[8192]        = "";
fc::encoding           FTerm::term_encoding;
const FString*         FTerm::save_xterm_font           = 0;
const FString*         FTerm::save_xterm_title          = 0;
FOptiMove*             FTerm::opti_move                 = 0;
FOptiAttr*             FTerm::opti_attr                 = 0;
FTermDetection*        FTerm::term_detection            = 0;
FTermXTerminal*        FTerm::xterm                     = 0;
FKeyboard*             FTerm::keyboard                  = 0;
FMouseControl*         FTerm::mouse                     = 0;
std::map<uChar,uChar>* FTerm::vt100_alt_char            = 0;
std::map<std::string,fc::encoding>* \
                       FTerm::encoding_set              = 0;
FTerm::termcap_map*    FTerm::tcap                      = fc::term_caps;
bool                   FTermcap::background_color_erase = false;
bool                   FTermcap::automatic_left_margin  = false;
bool                   FTermcap::automatic_right_margin = false;
bool                   FTermcap::eat_nl_glitch          = false;
bool                   FTermcap::ansi_default_color     = false;
bool                   FTermcap::osc_support            = false;
bool                   FTermcap::no_utf8_acs_chars      = false;
int                    FTermcap::max_color              = 1;
int                    FTermcap::tabstop                = 8;
int                    FTermcap::attr_without_color     = 0;
FTerm::initializationValues   FTerm::init_values;
fc::linuxConsoleCursorStyle   FTerm::linux_console_cursor_style;

#if defined(__linux__)
  FTermLinux* FTerm::linux = 0;
#endif

#if defined(__FreeBSD__) || defined(__DragonFly__)
  FTermFreeBSD* FTerm::freebsd = 0;
#endif

#if defined(__NetBSD__) || defined(__OpenBSD__)
  FTermOpenBSD* FTerm::openbsd = 0;
#endif


//----------------------------------------------------------------------
// class FTerm
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FTerm::FTerm (bool disable_alt_screen)
{
  resize_term = false;

  if ( ! term_initialized )
  {
    use_alternate_screen = ! disable_alt_screen;
    init();
  }
}

//----------------------------------------------------------------------
FTerm::~FTerm()  // destructor
{
  if ( init_term_object == this )
    finish();  // Resetting console settings
}


// public methods of FTerm
//----------------------------------------------------------------------
int FTerm::getLineNumber()
{
  if ( term->getHeight() == 0 )
    detectTermSize();

  return term->getHeight();
}

//----------------------------------------------------------------------
int FTerm::getColumnNumber()
{
  if ( term->getWidth() == 0 )
    detectTermSize();

  return term->getWidth();
}

//----------------------------------------------------------------------
const FString FTerm::getKeyName (int keynum)
{
  return keyboard->getKeyName (keynum);
}

//----------------------------------------------------------------------
bool FTerm::isNormal (charData*& ch)
{
  return opti_attr->isNormal(ch);
}

//----------------------------------------------------------------------
void FTerm::setTermType (const char term_name[])
{
  if ( ! term_name )
    return;

  std::strncpy (termtype, term_name, sizeof(termtype));
  termtype[sizeof(termtype) - 1] = '\0';
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
  if ( on == utf8_state )
    return utf8_state;

  if ( on )
    utf8_state = true;
  else
    utf8_state = false;

#if defined(__linux__)
  linux->setUTF8 (on);
#endif

  return utf8_state;
}

//----------------------------------------------------------------------
bool FTerm::setVGAFont()
{
  if ( VGAFont )
    return VGAFont;

  if ( isGnomeTerminal()
    || isKdeTerminal()
    || isPuttyTerminal()
    || isTeraTerm()
    || isCygwinTerminal()
    || isMinttyTerm() )
    return false;


  if ( isXTerminal() || isScreenTerm()
    || isUrxvtTerminal() || FTermcap::osc_support )
  {
    VGAFont = true;
    // Set font in xterm to vga
    xterm->setFont("vga");
    NewFont = false;
    pc_charset_console = true;
    term_encoding = fc::PC;

    if ( isXTerminal() && utf8_console )
      Fputchar = &FTerm::putchar_UTF8;
    else
      Fputchar = &FTerm::putchar_ASCII;
  }
#if defined(__linux__)
  else if ( isLinuxTerm() )
  {
    VGAFont = linux->loadVGAFont();
    pc_charset_console = true;
    term_encoding = fc::PC;
    Fputchar = &FTerm::putchar_ASCII;
  }
#endif  // defined(__linux__)
  else
    VGAFont = false;

  if ( VGAFont )
    shadow_character = half_block_character = true;

  return VGAFont;
}

//----------------------------------------------------------------------
bool FTerm::setNewFont()
{
  if ( NewFont )
    return true;

  if ( isGnomeTerminal()
    || isKdeTerminal()
    || isPuttyTerminal()
    || isTeraTerm()
    || isCygwinTerminal()
    || isMinttyTerm() )
    return false;

  if ( isXTerminal() || isScreenTerm()
    || isUrxvtTerminal() || FTermcap::osc_support )
  {
    NewFont = true;
    // Set font in xterm to 8x16graph
    xterm->setFont("8x16graph");
    pc_charset_console = true;
    term_encoding = fc::PC;

    if ( isXTerminal() && utf8_console )
      Fputchar = &FTerm::putchar_UTF8;
    else
      Fputchar = &FTerm::putchar_ASCII;
  }
#if defined(__linux__)
  else if ( isLinuxTerm() )
  {
    NewFont = linux->loadNewFont();
    pc_charset_console = true;
    term_encoding = fc::PC;
    Fputchar = &FTerm::putchar_ASCII;  // function pointer
  }
#endif  // defined(__linux__)
  else
    NewFont = false;

  if ( NewFont )
    shadow_character = half_block_character = true;

  return NewFont;
}

//----------------------------------------------------------------------
bool FTerm::setOldFont()
{
  bool retval = false;

  if ( ! (NewFont || VGAFont) )
    return false;

  retval  = \
  NewFont = \
  VGAFont = false;

  if ( isXTerminal() || isScreenTerm()
    || isUrxvtTerminal() || FTermcap::osc_support )
  {
    if ( save_xterm_font && save_xterm_font->getLength() > 2 )
    {
      // restore saved xterm font
      xterm->setFont(*save_xterm_font);
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
      shadow_character = linux->hasShadowCharacter();
      half_block_character = linux->hasHalfBlockCharacter();
    }
  }
#endif  // defined(__linux__)

  if ( retval )
    VGAFont = NewFont = false;

  return retval;
}

//----------------------------------------------------------------------
int FTerm::openConsole()
{
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

  if ( fd_tty >= 0 )  // console is already opened
    return 0;

  if ( ! *termfilename )
    return 0;

  for (int i = 0; terminal_devices[i] != 0; i++)
    if ( (fd_tty = open(terminal_devices[i], O_RDWR, 0)) >= 0 )
      return 0;

  return -1;  // No file descriptor referring to the console
}

//----------------------------------------------------------------------
int FTerm::closeConsole()
{
  if ( fd_tty < 0 )  // console is already closed
    return 0;

  int ret = ::close (fd_tty);  // use 'close' from the global namespace
  fd_tty = -1;

  if ( ret == 0 )
    return 0;
  else
    return -1;
}

//----------------------------------------------------------------------
char* FTerm::moveCursor (int xold, int yold, int xnew, int ynew)
{
  // Returns the cursor move string

  if ( cursor_optimisation )
    return opti_move->moveCursor (xold, yold, xnew, ynew);
  else
    return tgoto(TCAP(fc::t_cursor_address), xnew, ynew);
}

//----------------------------------------------------------------------
char* FTerm::cursorsVisibility (bool on)
{
  // Hides or shows the input cursor on the terminal

  char* visibility_str = 0;

  if ( on == hidden_cursor )
    return 0;

  if ( on )
  {
    visibility_str = disableCursor();

    if ( visibility_str )
      hidden_cursor = true;  // global state
  }
  else
  {
    visibility_str = enableCursor();

    if ( visibility_str )
      hidden_cursor = false;  // global state
  }

  return visibility_str;
}

//----------------------------------------------------------------------
void FTerm::printMoveDurations()
{
  opti_move->printDurations();
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
    std::strncpy (enable_str, ve, SIZE);
  else if ( vs )
    std::strncpy (enable_str, vs, SIZE);

#if defined(__linux__)
  if ( isLinuxTerm() )
  {
    // Restore the last used Linux console cursor style
    char* cstyle;
    cstyle = linux->restoreCursorStyle();
    std::strncat (enable_str, cstyle, SIZE - std::strlen(enable_str));
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
  int ret;

  if ( fd_tty < 0 )  // console is already closed
  {
    if ( openConsole() != 0 )
      return;

    close_after_detect = true;
  }

  ret = ioctl (fd_tty, TIOCGWINSZ, &win_size);

  if ( ret != 0 || win_size.ws_col == 0 || win_size.ws_row == 0 )
  {
    char* str;
    term->setPos(1,1);
    // Use COLUMNS or fallback to the xterm default width of 80 characters
    str = std::getenv("COLUMNS");
    term->setWidth(str ? std::atoi(str) : 80);
    // Use LINES or fallback to the xterm default height of 24 characters
    str = std::getenv("LINES");
    term->setHeight(str ? std::atoi(str) : 24);
  }
  else
  {
    term->setRect(1, 1, win_size.ws_col, win_size.ws_row);
  }

  opti_move->setTermSize (term->getWidth(), term->getHeight());

  if ( close_after_detect )
    closeConsole();
}

//----------------------------------------------------------------------
void FTerm::setTermSize (int term_width, int term_height)
{
  // Set xterm size to {term_width} x {term_height}

  if ( isXTerminal() )
  {
    putstringf (CSI "8;%d;%dt", term_height, term_width);
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTerm::setTermTitle (const FString& title)
{
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
void FTerm::setPalette (short index, int r, int g, int b)
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
  term_encoding = enc;

  assert ( term_encoding == fc::UTF8
        || term_encoding == fc::VT100  // VT100 line drawing
        || term_encoding == fc::PC     // CP-437
        || term_encoding == fc::ASCII );

  // Set the new Fputchar function pointer
  switch ( term_encoding )
  {
    case fc::UTF8:
      Fputchar = &FTerm::putchar_UTF8;
      break;

    case fc::VT100:
    case fc::PC:
      if ( isXTerminal() && utf8_console )
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
    if ( term_encoding == fc::VT100 || term_encoding == fc::PC )
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
  return term_encoding;
}

//----------------------------------------------------------------------
std::string FTerm::getEncodingString()
{
  std::map<std::string,fc::encoding>::const_iterator it, end;
  end = encoding_set->end();

  for (it = encoding_set->begin(); it != end; ++it )
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
  return charEncode (c, term_encoding);
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
  shadow_character = linux->hasShadowCharacter();
  half_block_character = linux->hasHalfBlockCharacter();
#endif

#if defined(__FreeBSD__) || defined(__DragonFly__)
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
void FTerm::exitWithMessage (std::string message)
{
  // Set the exit_message for the atexit-handler
  snprintf ( exit_message
           , sizeof(exit_message)
           , "%s"
           , message.c_str() );

  // Exit the programm
  if ( init_term_object )
    init_term_object->finish();

  std::fflush (stderr);
  std::fflush (stdout);

  if ( exit_message[0] )
    std::fprintf (stderr, "Warning: %s\n", exit_message);

  std::exit (EXIT_FAILURE);
}


// private methods of FTerm
//----------------------------------------------------------------------
void FTerm::init_global_values()
{
  // Initialize global values

  // Teletype (tty) file descriptor is still undefined
  fd_tty = -1;

  // Preset to true
  shadow_character     = \
  half_block_character = \
  cursor_optimisation  = true;

  // Preset to false
  hidden_cursor        = \
  utf8_console         = \
  utf8_state           = \
  utf8_linux_terminal  = \
  pc_charset_console   = \
  vt100_console        = \
  NewFont              = \
  VGAFont              = \
  ascii_console        = \
  force_vt100          = false;

  // Init arrays with '\0'
  std::fill_n (exit_message, sizeof(exit_message), '\0');

  // Initialize xterm object
  xterm->setTermcapMap(tcap);
  xterm->setFTermDetection(term_detection);

  if ( ! init_values.terminal_detection )
    term_detection->setTerminalDetection (false);
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

  if ( TCAP(fc::t_acs_chars) )
  {
    for (int n = 0; TCAP(fc::t_acs_chars)[n]; n += 2)
    {
      // insert the VT100 key/value pairs into a map
      uChar p1 = uChar(TCAP(fc::t_acs_chars)[n]);
      uChar p2 = uChar(TCAP(fc::t_acs_chars)[n + 1]);
      (*vt100_alt_char)[p1] = p2;
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
    uChar altChar = uChar((*vt100_alt_char)[ keyChar ]);
    uInt utf8char = uInt(fc::vt100_key_to_utf8[n][utf8_char]);
    fc::encoding num = fc::NUM_OF_ENCODINGS;

    uInt* p = std::find ( fc::character[0]
                        , fc::character[fc::lastCharItem] + num
                        , utf8char );
    if ( p != fc::character[fc::lastCharItem] + num )  // found in character
    {
      int item = int(std::distance(fc::character[0], p) / num);

      if ( altChar )
        fc::character[item][fc::VT100] = altChar;  // update alternate character set
      else
        fc::character[item][fc::VT100] = 0;  // delete VT100 char in character
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
      if ( utf8_console )
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
      if ( utf8_console )
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
/* Terminal capability data base
 * -----------------------------
 * Info under: man 5 terminfo
 *
 * Importent shell commands:
 *   captoinfo - convert all termcap descriptions into terminfo descriptions
 *   infocmp   - print out terminfo description from the current terminal
 */
void FTerm::init_termcap()
{
  std::vector<std::string> terminals;
  std::vector<std::string>::iterator iter;
  static const int success = 1;
  static const int uninitialized = -2;
  static char term_buffer[2048];
  static char string_buf[2048];
  char* buffer = string_buf;
  int status = uninitialized;
  bool color256 = term_detection->canDisplay256Colors();

  // Share the terminal capabilities
  FTermcap().setTermcapMap(tcap);

  // Open termcap file
  terminals.push_back(termtype);            // available terminal type

  if ( color256 )                           // 1st fallback if not found
    terminals.push_back("xterm-256color");

  terminals.push_back("xterm");             // 2nd fallback if not found
  terminals.push_back("ansi");              // 3rd fallback if not found
  terminals.push_back("vt100");             // 4th fallback if not found
  iter = terminals.begin();

  while ( iter != terminals.end() )
  {
    // Copy c-string + terminating null-character ('\0')
    std::strncpy (termtype, iter->c_str(), iter->length() + 1);

    // Open the termcap file + load entry for termtype
    status = tgetent(term_buffer, termtype);

    if ( status == success || ! term_detection->hasTerminalDetection() )
      break;

    ++iter;
  }

  if ( std::strncmp(termtype, "ansi", 4) == 0 )
    term_detection->setAnsiTerminal (true);

  init_termcap_error (status);
  init_termcap_variables (buffer);
}

//----------------------------------------------------------------------
void FTerm::init_termcap_error (int status)
{
  static const int no_entry = 0;
  static const int db_not_found = -1;
  static const int uninitialized = -2;

  if ( status == no_entry || status == uninitialized )
  {
    std::cerr << "Unknown terminal: "  << termtype << "\n"
              << "Check the TERM environment variable\n"
              << "Also make sure that the terminal\n"
              << "is defined in the termcap/terminfo database.\n";
    std::abort();
  }
  else if ( status == db_not_found )
  {
    std::cerr << "The termcap/terminfo database could not be found.\n";
    std::abort();
  }
}

//----------------------------------------------------------------------
void FTerm::init_termcap_variables (char*& buffer)
{
  // Get termcap booleans
  init_termcap_booleans();

  // Get termcap numerics
  init_termcap_numerics();

  // Get termcap strings
  init_termcap_strings(buffer);

  // Terminal quirks
  FTermcapQuirks quirks;
  quirks.setTermcapMap (tcap);
  quirks.setFTermDetection (term_detection);
  quirks.setTerminalType (termtype);
  quirks.terminalFixup();       // Fix terminal quirks

  // Get termcap keys
  init_termcap_keys(buffer);

  // Initialize cursor movement optimization
  init_OptiMove();

  // Initialize video attributes optimization
  init_OptiAttr();
}

//----------------------------------------------------------------------
void FTerm::init_termcap_booleans()
{
  // Get termcap booleans

  // Screen erased with the background color
  FTermcap::background_color_erase = tgetflag(C_STR("ut"));

  // t_cursor_left wraps from column 0 to last column
  FTermcap::automatic_left_margin = tgetflag(C_STR("bw"));

  // Terminal has auto-matic margins
  FTermcap::automatic_right_margin = tgetflag(C_STR("am"));

  // NewLine ignored after 80 cols
  FTermcap::eat_nl_glitch = tgetflag(C_STR("xn"));

  // Terminal supports ANSI set default fg and bg color
  FTermcap::ansi_default_color = tgetflag(C_STR("AX"));

  // Terminal supports operating system commands (OSC)
  // OSC = Esc + ']'
  FTermcap::osc_support = tgetflag(C_STR("XT"));

  // U8 is nonzero for terminals with no VT100 line-drawing in UTF-8 mode
  FTermcap::no_utf8_acs_chars = bool(tgetnum(C_STR("U8")) != 0);
}

//----------------------------------------------------------------------
void FTerm::init_termcap_numerics()
{
  // Get termcap numeric

  // Maximum number of colors on screen
  FTermcap::max_color = std::max( FTermcap::max_color
                                , tgetnum(C_STR("Co")) );

  if ( getMaxColor() < 0 )
    FTermcap::max_color = 1;

  if ( getMaxColor() < 8 )
    monochron = true;
  else
    monochron = false;

  // Get initial spacing for hardware tab stop
  FTermcap::tabstop = tgetnum(C_STR("it"));

  // Get video attributes that cannot be used with colors
  FTermcap::attr_without_color = tgetnum(C_STR("NC"));
}

//----------------------------------------------------------------------
void FTerm::init_termcap_strings (char*& buffer)
{
  // Get termcap strings

  // Read termcap output strings
  for (int i = 0; tcap[i].tname[0] != 0; i++)
    tcap[i].string = tgetstr(tcap[i].tname, &buffer);
}

//----------------------------------------------------------------------
void FTerm::init_termcap_keys (char*& buffer)
{
  // Read termcap key strings

  for (int i = 0; fc::Fkey[i].tname[0] != 0; i++)
  {
    fc::Fkey[i].string = tgetstr(fc::Fkey[i].tname, &buffer);

    // Fallback for rxvt with TERM=xterm
    if ( std::strncmp(fc::Fkey[i].tname, "khx", 3) == 0 )
      fc::Fkey[i].string = C_STR(CSI "7~");  // Home key

    if ( std::strncmp(fc::Fkey[i].tname, "@7x", 3) == 0 )
      fc::Fkey[i].string = C_STR(CSI "8~");  // End key

    if ( std::strncmp(fc::Fkey[i].tname, "k1x", 3) == 0 )
      fc::Fkey[i].string = C_STR(CSI "11~");  // F1

    if ( std::strncmp(fc::Fkey[i].tname, "k2x", 3) == 0 )
      fc::Fkey[i].string = C_STR(CSI "12~");  // F2

    if ( std::strncmp(fc::Fkey[i].tname, "k3x", 3) == 0 )
      fc::Fkey[i].string = C_STR(CSI "13~");  // F3

    if ( std::strncmp(fc::Fkey[i].tname, "k4x", 3) == 0 )
      fc::Fkey[i].string = C_STR(CSI "14~");  // F4

    // Fallback for TERM=ansi
    if ( std::strncmp(fc::Fkey[i].tname, "@7X", 3) == 0 )
      fc::Fkey[i].string = C_STR(CSI "K");  // End key

    // Keypad keys
    if ( std::strncmp(fc::Fkey[i].tname, "@8x", 3) == 0 )
      fc::Fkey[i].string = C_STR(ESC "OM");  // Enter key

    if ( std::strncmp(fc::Fkey[i].tname, "KP1", 3) == 0 )
      fc::Fkey[i].string = C_STR(ESC "Oo");  // Keypad slash

    if ( std::strncmp(fc::Fkey[i].tname, "KP2", 3) == 0 )
      fc::Fkey[i].string = C_STR(ESC "Oj");  // Keypad asterisk

    if ( std::strncmp(fc::Fkey[i].tname, "KP3", 3) == 0 )
      fc::Fkey[i].string = C_STR(ESC "Om");  // Keypad minus sign

    if ( std::strncmp(fc::Fkey[i].tname, "KP4", 3) == 0 )
      fc::Fkey[i].string = C_STR(ESC "Ok");  // Keypad plus sign
  }

  // Some terminals (e.g. PuTTY) send the wrong code for the arrow keys
  // http://www.unix.com/shell-programming-scripting/..
  //      ..110380-using-arrow-keys-shell-scripts.html
  char* key_up_string = tgetstr(C_STR("ku"), &buffer);

  if ( (key_up_string && (std::strcmp(key_up_string, CSI "A") == 0))
    || ( TCAP(fc::t_cursor_up)
      && (std::strcmp(TCAP(fc::t_cursor_up), CSI "A") == 0) ) )
  {
    for (int i = 0; fc::Fkey[i].tname[0] != 0; i++)
    {
      if ( std::strncmp(fc::Fkey[i].tname, "kux", 3) == 0 )
        fc::Fkey[i].string = C_STR(CSI "A");  // Key up

      if ( std::strncmp(fc::Fkey[i].tname, "kdx", 3) == 0 )
        fc::Fkey[i].string = C_STR(CSI "B");  // Key down

      if ( std::strncmp(fc::Fkey[i].tname, "krx", 3) == 0 )
        fc::Fkey[i].string = C_STR(CSI "C");  // Key right

      if ( std::strncmp(fc::Fkey[i].tname, "klx", 3) == 0 )
        fc::Fkey[i].string = C_STR(CSI "D");  // Key left

      if ( std::strncmp(fc::Fkey[i].tname, "k1X", 3) == 0 )
        fc::Fkey[i].string = C_STR(ESC "OP");  // PF1

      if ( std::strncmp(fc::Fkey[i].tname, "k2X", 3) == 0 )
        fc::Fkey[i].string = C_STR(ESC "OQ");  // PF2

      if ( std::strncmp(fc::Fkey[i].tname, "k3X", 3) == 0 )
        fc::Fkey[i].string = C_STR(ESC "OR");  // PF3

      if ( std::strncmp(fc::Fkey[i].tname, "k4X", 3) == 0 )
        fc::Fkey[i].string = C_STR(ESC "OS");  // PF4
    }
  }
}

//----------------------------------------------------------------------
void FTerm::init_OptiMove()
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
void FTerm::init_OptiAttr()
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
  locale_name = std::setlocale (LC_ALL, "");
  locale_name = std::setlocale (LC_NUMERIC, "");

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
    locale_name = C_STR("C");
}

//----------------------------------------------------------------------
void FTerm::init_encoding()
{
  // detect encoding and set the Fputchar function pointer

  init_encoding_set();

  if ( isRxvtTerminal() && ! isUrxvtTerminal() )
    force_vt100 = true;  // This rxvt terminal does not support utf-8

  init_term_encoding();
  init_pc_charset();
  init_individual_term_encoding();

  if ( ! init_force_vt100_encoding() )
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

  (*encoding_set)["UTF8"]  = fc::UTF8;
  (*encoding_set)["UTF-8"] = fc::UTF8;
  (*encoding_set)["VT100"] = fc::VT100;  // VT100 line drawing
  (*encoding_set)["PC"]    = fc::PC;     // CP-437
  (*encoding_set)["ASCII"] = fc::ASCII;
}

//----------------------------------------------------------------------
void FTerm::init_term_encoding()
{
  int stdout_no = FTermios::getStdOut();

  if ( isatty(stdout_no)
    && ! std::strcmp(nl_langinfo(CODESET), "UTF-8") )
  {
    utf8_console  = true;
    term_encoding = fc::UTF8;
    Fputchar      = &FTerm::putchar_UTF8;  // function pointer
    utf8_state    = true;
    setUTF8(true);
    keyboard->enableUTF8();
  }
  else if ( isatty(stdout_no)
         && (std::strlen(termtype) > 0)
         && (TCAP(fc::t_exit_alt_charset_mode) != 0) )
  {
    vt100_console = true;
    term_encoding = fc::VT100;
    Fputchar      = &FTerm::putchar_ASCII;  // function pointer
  }
  else
  {
    ascii_console = true;
    term_encoding = fc::ASCII;
    Fputchar      = &FTerm::putchar_ASCII;  // function pointer
  }
}

//----------------------------------------------------------------------
void FTerm::init_individual_term_encoding()
{
  if ( isLinuxTerm()
    || isCygwinTerminal()
    || isNewFont()
    || (isPuttyTerminal() && ! isUTF8())
    || (isTeraTerm() && ! isUTF8()) )
  {
    pc_charset_console = true;
    term_encoding      = fc::PC;
    Fputchar           = &FTerm::putchar_ASCII;  // function pointer

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
bool FTerm::init_force_vt100_encoding()
{
  if ( force_vt100 )  // Force VT100 line drawing
  {
    vt100_console = true;
    term_encoding = fc::VT100;
    Fputchar      = &FTerm::putchar_ASCII;  // function pointer
  }

  return force_vt100;
}

//----------------------------------------------------------------------
void FTerm::init_utf8_without_alt_charset()
{
  // Fall back to ascii for utf-8 terminals that
  // do not support VT100 line drawings

  if ( FTermcap::no_utf8_acs_chars && isUTF8()
    && term_encoding == fc::VT100 )
  {
    ascii_console = true;
    term_encoding = fc::ASCII;
    Fputchar      = &FTerm::putchar_ASCII;  // function pointer
  }
}

//----------------------------------------------------------------------
void FTerm::init_tab_quirks()
{
  // In some alternative character sets, a tab character prints a '○'
  // on the terminal and does not move the cursor to the next tab stop
  // position

  if ( term_encoding == fc::VT100 || term_encoding == fc::PC )
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
    save_xterm_font = new FString(*font);

  if ( title )
    save_xterm_title = new FString(*title);
}

//----------------------------------------------------------------------
void FTerm::redefineColorPalette()
{
  // Redefine the color palette

  if ( isCygwinTerminal()
    || isKdeTerminal()
    || isTeraTerm()
    || isMltermTerminal()
    || isNetBSDTerm()
    || isOpenBSDTerm()
    || isSunTerminal()
    || isAnsiTerminal() )
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
  if ( isCygwinTerminal()
    || isKdeTerminal()
    || isTeraTerm()
    || isMltermTerminal()
    || isNetBSDTerm()
    || isOpenBSDTerm()
    || isSunTerminal()
    || isAnsiTerminal() )
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
  cstyle = linux->setCursorStyle (fc::underscore_cursor, isCursorHidden());
  putstring (cstyle);
  std::fflush(stdout);
#endif

#if defined(__FreeBSD__) || defined(__DragonFly__)
  freebsd->setCursorStyle (fc::destructive_cursor, isCursorHidden());
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
  cstyle = linux->setCursorStyle (fc::full_block_cursor, isCursorHidden());
  putstring (cstyle);
  std::fflush(stdout);
#endif

#if defined(__FreeBSD__) || defined(__DragonFly__)
  freebsd->setCursorStyle (fc::normal_cursor, isCursorHidden());
#endif

  if ( isUrxvtTerminal() )
    xterm->setCursorColor ("rgb:eeee/0000/0000");
}

//----------------------------------------------------------------------
void FTerm::enableMouse()
{
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
void FTerm::disableMouse()
{
  keyboard->disableMouseSequences();
  mouse->disable();
}

//----------------------------------------------------------------------
void FTerm::useAlternateScreenBuffer()
{
  if ( ! use_alternate_screen )
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
  if ( ! use_alternate_screen )
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
    opti_move      = new FOptiMove();
    opti_attr      = new FOptiAttr();
    term_detection = new FTermDetection();
    xterm          = new FTermXTerminal();

#if defined(__linux__)
    linux          = new FTermLinux();
#endif

#if defined(__FreeBSD__) || defined(__DragonFly__)
    freebsd        = new FTermFreeBSD();
#endif

#if defined(__NetBSD__) || defined(__OpenBSD__)
    openbsd        = new FTermOpenBSD();
#endif

    keyboard       = new FKeyboard();
    mouse          = new FMouseControl();
    term           = new FRect(0, 0, 0, 0);
    vt100_alt_char = new std::map<uChar, uChar>;
    encoding_set   = new std::map<std::string, fc::encoding>;
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
    std::abort();
  }
}

//----------------------------------------------------------------------
inline void FTerm::deallocationValues()
{
  if ( encoding_set )
    delete encoding_set;

  if ( vt100_alt_char )
    delete vt100_alt_char;

  if ( save_xterm_title )
    delete save_xterm_title;

  if ( save_xterm_font )
    delete save_xterm_font;

  if ( term )
    delete term;

  if ( mouse )
    delete mouse;

  if ( keyboard )
    delete keyboard;

#if defined(__NetBSD__) || defined(__OpenBSD__)
  if ( openbsd )
    delete openbsd;
#endif

#if defined(__FreeBSD__) || defined(__DragonFly__)
  if ( freebsd )
    delete freebsd;
#endif

#if defined(__linux__)
  if ( linux )
    delete linux;
#endif

  if ( xterm )
    delete xterm;

  if ( term_detection )
    delete term_detection;

  if ( opti_attr )
    delete opti_attr;

  if ( opti_move )
    delete opti_move;
}

//----------------------------------------------------------------------
void FTerm::init()
{
  int stdout_no = FTermios::getStdOut();
  init_term_object = this;

  // Initialize global values for all objects
  allocationValues();
  init_global_values();

  // Initialize termios
  FTermios::init();

  // Get pathname of the terminal device
  if ( ttyname_r(stdout_no, termfilename, sizeof(termfilename)) )
    termfilename[0] = '\0';

  term_detection->setTermFileName(termfilename);

  initOSspecifics();

  // Save termios settings
  FTermios::storeTTYsettings();

  // Get output baud rate
  baudrate = FTermios::getBaudRate();

  if ( isatty(stdout_no) )
    opti_move->setBaudRate(int(baudrate));

  // Terminal detection
  term_detection->detect();
  setTermType (term_detection->getTermType());

  // Set maximum number of colors for detected terminals
  init_fixed_max_color();

  // Initializes variables for the current terminal
  init_termcap();
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
  if ( init_values.mouse_support )
    enableMouse();

  // Activate meta key sends escape
  if ( isXTerminal() )
    xterm->metaSendsESC(true);

  // switch to application escape key mode
  if ( isMinttyTerm() )
    FTerm::putstring (CSI "?7727h");

  // Enter 'keyboard_transmit' mode
  if ( TCAP(fc::t_keypad_xmit) )
  {
    putstring (TCAP(fc::t_keypad_xmit));
    std::fflush(stdout);
  }

  useAlternateScreenBuffer();

  // Enable alternate charset
  if ( TCAP(fc::t_enable_acs) )
  {
    putstring (TCAP(fc::t_enable_acs));
    std::fflush(stdout);
  }

  // Save the used xterm font and window title
  init_captureFontAndTitle();

  if ( isKdeTerminal() )
    setKDECursor(fc::UnderlineCursor);

  if ( isCygwinTerminal() )
    init_cygwin_charmap();

  if ( isTeraTerm() )
    init_teraterm_charmap();

  // Redefine the color palette
  if ( init_values.color_change )
    redefineColorPalette();

  // Set 200 Hz beep (100 ms)
  setBeep(200, 100);

  // Set FTerm signal handler
  setSignalHandler();

  if ( ! init_values.cursor_optimisation )
    setCursorOptimisation (false);

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
  framebuffer_bpp = linux->getFramebufferBpp();
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
#endif  // defined(__FreeBSD__) || defined(__DragonFly__)

#if defined(__NetBSD__) || defined(__OpenBSD__)
  if ( init_values.meta_sends_escape )
    openbsd->enableMetaSendsEscape();
  else
    openbsd->disableMetaSendsEscape();

  openbsd->init();  // Initialize wscons console
#endif
}

//----------------------------------------------------------------------
void FTerm::finish()
{
  // Set default signal handler
  resetSignalHandler();

  if ( save_xterm_title && isXTerminal() && ! isRxvtTerminal() )
    setTermTitle (*save_xterm_title);

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

  if ( isMinttyTerm() )
  {
    // Switch to normal escape key mode
    putstring (CSI "?7727l");
    std::fflush(stdout);
  }

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
  if ( TCAP(fc::t_keypad_local) )
  {
    putstring (TCAP(fc::t_keypad_local));
    std::fflush(stdout);
  }

  finish_encoding();

  if ( NewFont || VGAFont )
    setOldFont();

  deallocationValues();
}

//----------------------------------------------------------------------
void FTerm::finishOSspecifics1()
{
#if defined(__linux__)
  linux->finish();
#endif

#if defined(__FreeBSD__) || defined(__DragonFly__)
  freebsd->finish();
#endif

#if defined(__NetBSD__) || defined(__OpenBSD__)
  openbsd->finish();
#endif
}

//----------------------------------------------------------------------
void FTerm::finish_encoding()
{
#if defined(__linux__)
  if ( isLinuxTerm() && utf8_console )
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
      if ( resize_term )
        break;
      // initialize a resize event to the root element
      resize_term = true;
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

}  // namespace finalcut
