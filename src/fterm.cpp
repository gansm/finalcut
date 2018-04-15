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
#include "final/fkey_map.h"
#include "final/ftcap_map.h"

#if defined(__linux__)
  #include "../fonts/newfont.h"
  #include "../fonts/unicodemap.h"
  #include "../fonts/vgafont.h"
#endif

// global FTerm object
static FTerm* init_term_object = 0;

// global init state
static bool term_initialized = false;

// function pointer
int (*FTerm::Fputchar)(int);

// static class attributes
int      FTerm::stdin_no;
int      FTerm::stdout_no;
int      FTerm::fd_tty;
int      FTerm::gnome_terminal_id;
int      FTerm::stdin_status_flags;
int      FTerm::erase_ch_length;
int      FTerm::repeat_char_length;
int      FTerm::clr_bol_length;
int      FTerm::clr_eol_length;
int      FTerm::cursor_addres_lengths;
uInt     FTerm::baudrate;
long     FTerm::key_timeout;
bool     FTerm::resize_term;
bool     FTerm::decscusr_support;
bool     FTerm::terminal_detection;
bool     FTerm::raw_mode;
bool     FTerm::input_data_pending;
bool     FTerm::non_blocking_stdin;
bool     FTerm::color256;
bool     FTerm::monochron;
bool     FTerm::xterm_terminal;
bool     FTerm::ansi_terminal;
bool     FTerm::rxvt_terminal;
bool     FTerm::urxvt_terminal;
bool     FTerm::mlterm_terminal;
bool     FTerm::putty_terminal;
bool     FTerm::kde_konsole;
bool     FTerm::gnome_terminal;
bool     FTerm::kterm_terminal;
bool     FTerm::tera_terminal;
bool     FTerm::sun_terminal;
bool     FTerm::cygwin_terminal;
bool     FTerm::mintty_terminal;
bool     FTerm::linux_terminal;
bool     FTerm::netbsd_terminal;
bool     FTerm::openbsd_terminal;
bool     FTerm::screen_terminal;
bool     FTerm::tmux_terminal;
bool     FTerm::pc_charset_console;
bool     FTerm::utf8_input;
bool     FTerm::utf8_state;
bool     FTerm::utf8_console;
bool     FTerm::utf8_linux_terminal;
bool     FTerm::force_vt100;
bool     FTerm::vt100_console;
bool     FTerm::ascii_console;
bool     FTerm::NewFont;
bool     FTerm::VGAFont;
bool     FTerm::no_shadow_character;
bool     FTerm::no_half_block_character;
bool     FTerm::cursor_optimisation;
bool     FTerm::xterm_default_colors;
bool     FTerm::use_alternate_screen = true;
termios  FTerm::term_init;
char     FTerm::termtype[256]  = {};
char     FTerm::termfilename[256] = {};

#if DEBUG
char     FTerm::termtype_256color[256]   = {};
char     FTerm::termtype_Answerback[256] = {};
char     FTerm::termtype_SecDA[256]      = {};
int      FTerm::framebuffer_bpp = -1;
#endif

char*    FTerm::locale_name  = 0;
char*    FTerm::locale_xterm = 0;
FRect*   FTerm::term         = 0;

char                   FTerm::exit_message[8192]        = "";
fc::encoding           FTerm::term_encoding;
const FString*         FTerm::xterm_font                = 0;
const FString*         FTerm::xterm_title               = 0;
const FString*         FTerm::answer_back               = 0;
const FString*         FTerm::sec_da                    = 0;
FOptiMove*             FTerm::opti_move                 = 0;
FOptiAttr*             FTerm::opti_attr                 = 0;
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
FTerm::secondaryDA            FTerm::secondary_da;
FTerm::colorEnv               FTerm::color_env;
FTerm::initializationValues   FTerm::init_values;
fc::linuxConsoleCursorStyle   FTerm::linux_console_cursor_style;
fc::freebsdConsoleCursorStyle FTerm::freebsd_console_cursor_style;

#if defined(__linux__)
  FTerm::modifier_key  FTerm::mod_key;
  console_font_op      FTerm::screen_font;
  unimapdesc           FTerm::screen_unicode_map;
#endif

#if defined(__FreeBSD__) || defined(__DragonFly__)
  uInt FTerm::bsd_alt_keymap = 0;
#endif

#if defined(__NetBSD__) || defined(__OpenBSD__)
  kbd_t FTerm::wscons_keyboard_encoding = 0;
#endif


//----------------------------------------------------------------------
// class FTerm
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FTerm::FTerm (bool disable_alt_screen)
  : color_map()
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
    finish();
}


// public methods of FTerm
//----------------------------------------------------------------------
termios FTerm::getTTY()
{
  struct termios t;
  tcgetattr (stdin_no, &t);
  return t;
}

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
  for (int i = 0; fc::FkeyName[i].string[0] != 0; i++)
    if ( fc::FkeyName[i].num && fc::FkeyName[i].num == keynum )
      return FString(fc::FkeyName[i].string);

  if ( keynum > 32 && keynum < 127 )
    return FString(char(keynum));

  return FString("");
}

//----------------------------------------------------------------------
FMouseControl* FTerm::getMouseControl()
{
  if ( mouse )
    return mouse;
  else
    return 0;
}

#if defined(__linux__)
//----------------------------------------------------------------------
FTerm::modifier_key& FTerm::getLinuxModifierKey()
{
  // Get Linux console shift state

  char subcode = 6;  // Shift state command + return value

  // fill bit field with 0
  std::memset (&mod_key, 0x00, sizeof(mod_key));

  // TIOCLINUX, subcode = 6
  if ( ioctl(0, TIOCLINUX, &subcode) >= 0 )
  {
    if ( subcode & (1 << KG_SHIFT) )
      mod_key.shift = true;

    if ( subcode & (1 << KG_ALTGR) )
      mod_key.alt_gr = true;

    if ( subcode & (1 << KG_CTRL) )
      mod_key.ctrl = true;

    if ( subcode & (1 << KG_ALT) )
      mod_key.alt = true;
  }

  return mod_key;
}

//----------------------------------------------------------------------
fc::linuxConsoleCursorStyle FTerm::getLinuxConsoleCursorStyle()
{
  return linux_console_cursor_style;
}
#endif

#if defined(__FreeBSD__) || defined(__DragonFly__)
//----------------------------------------------------------------------
fc::freebsdConsoleCursorStyle FTerm::getFreeBSDConsoleCursorStyle()
{
  return freebsd_console_cursor_style;
}
#endif

//----------------------------------------------------------------------
bool FTerm::isNormal (char_data*& ch)
{
  return opti_attr->isNormal(ch);
}

#if defined(__linux__)
//----------------------------------------------------------------------
void FTerm::setLinuxConsoleCursorStyle ( fc::linuxConsoleCursorStyle style
                                       , bool hidden )
{
  // Set cursor style in linux console

  if ( ! linux_terminal )
    return;

  linux_console_cursor_style = style;

  if ( hidden )
    return;

  putstringf (CSI "?%dc", style);
  std::fflush(stdout);
}
#endif

#if defined(__FreeBSD__) || defined(__DragonFly__)
//----------------------------------------------------------------------
void FTerm::setFreeBSDConsoleCursorStyle ( fc::freebsdConsoleCursorStyle style
                                         , bool hidden )
{
  // Set cursor style in a BSD console

  if ( ! isFreeBSDConsole() )
    return;

  freebsd_console_cursor_style = style;

  if ( hidden )
    return;

  ioctl(0, CONS_CURSORTYPE, &style);
}
#endif

//----------------------------------------------------------------------
void FTerm::setDblclickInterval (const long timeout)
{
  mouse->setDblclickInterval(timeout);
}

//----------------------------------------------------------------------
void FTerm::setTTY (const termios& t)
{
  tcsetattr (stdin_no, TCSADRAIN, &t);
}

//----------------------------------------------------------------------
void FTerm::noHardwareEcho()
{
  // Info under: man 3 termios
  struct termios t;
  tcgetattr (stdin_no, &t);

  // local mode
  t.c_lflag &= uInt(~(ECHO | ECHONL));

  // input mode
  t.c_iflag &= uInt(~(ICRNL | INLCR | IGNCR));

  // output mode
  t.c_oflag &= uInt(~ONLCR);

  // set the new termios settings
  setTTY (t);
}

//----------------------------------------------------------------------
bool FTerm::setRawMode (bool on)
{
  // set + unset flags for raw mode
  if ( on == raw_mode )
    return raw_mode;

  if ( on )
  {
    // Info under: man 3 termios
    struct termios t;
    tcgetattr (stdin_no, &t);

    // local mode
#if DEBUG
    // Exit with ctrl-c only if compiled with "DEBUG" option
    t.c_lflag &= uInt(~(ICANON | IEXTEN));
#else
    t.c_lflag &= uInt(~(ICANON | ISIG | IEXTEN));
#endif

    // input mode
    t.c_iflag &= uInt(~(IXON | BRKINT | PARMRK));

    // defines the terminal special characters for noncanonical read
    t.c_cc[VTIME] = 0;  // Timeout in deciseconds
    t.c_cc[VMIN]  = 1;  // Minimum number of characters

    // set the new termios settings
    setTTY (t);
    raw_mode = true;
  }
  else
  {
    struct termios t;
    tcgetattr (stdin_no, &t);

    // local mode
    t.c_lflag |= uInt(ISIG | ICANON | (term_init.c_lflag & IEXTEN));

    // input mode
    t.c_iflag |= uInt(IXON | BRKINT | PARMRK);

    // set the new termios settings
    setTTY (t);
    raw_mode = false;
  }

  return raw_mode;
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

  if ( linux_terminal )
  {
    if ( on )
      putstring (ESC "%G");
    else
      putstring (ESC "%@");
  }

  std::fflush(stdout);
  return utf8_state;
}

//----------------------------------------------------------------------
bool FTerm::setNonBlockingInput (bool on)
{
  if ( on == non_blocking_stdin )
    return non_blocking_stdin;

  if ( on )  // make stdin non-blocking
  {
    stdin_status_flags |= O_NONBLOCK;

    if ( fcntl (stdin_no, F_SETFL, stdin_status_flags) != -1 )
      non_blocking_stdin = true;
  }
  else
  {
    stdin_status_flags &= ~O_NONBLOCK;

    if ( fcntl (stdin_no, F_SETFL, stdin_status_flags) != -1 )
      non_blocking_stdin = false;
  }

  return non_blocking_stdin;
}

//----------------------------------------------------------------------
int FTerm::parseKeyString ( char buffer[]
                          , int buf_size
                          , timeval* time_keypressed )
{
  uChar firstchar = uChar(buffer[0]);

  if ( firstchar == ESC[0] )
  {
    int key = getMouseProtocolKey(buffer);

    if ( key > 0 )
      return key;

    key = getTermcapKey(buffer, buf_size);

    if ( key > 0 )
      return key;

    key = getMetaKey(buffer, buf_size, time_keypressed);

    if ( key > 0 )
      return key;

    if ( ! isKeypressTimeout(time_keypressed) )
      return NEED_MORE_DATA;
  }

  return getSingleKey(buffer, buf_size);
}

//----------------------------------------------------------------------
bool& FTerm::unprocessedInput()
{
  return input_data_pending;
}

//----------------------------------------------------------------------
bool FTerm::setVGAFont()
{
  if ( VGAFont )
    return VGAFont;

  if ( gnome_terminal
    || kde_konsole
    || putty_terminal
    || tera_terminal
    || cygwin_terminal
    || mintty_terminal )
    return false;

  VGAFont = true;

  if ( xterm_terminal || screen_terminal || FTermcap::osc_support )
  {
    // Set font in xterm to vga
    oscPrefix();
    putstring (OSC "50;vga" BEL);
    oscPostfix();
    std::fflush(stdout);
    NewFont = false;
    pc_charset_console = true;
    term_encoding = fc::PC;

    if ( xterm_terminal && utf8_console )
      Fputchar = &FTerm::putchar_UTF8;
    else
      Fputchar = &FTerm::putchar_ASCII;
  }
#if defined(__linux__)
  else if ( linux_terminal )
  {
    if ( openConsole() == 0 )
    {
      if ( isLinuxConsole() )
      {
        // standard vga font 8x16
        int ret = setScreenFont(fc::__8x16std, 256, 8, 16);

        if ( ret != 0 )
          VGAFont = false;

        // unicode character mapping
        struct unimapdesc unimap;
        unimap.entry_ct = uChar ( sizeof(fc::unicode_cp437_pairs)
                                / sizeof(unipair) );
        unimap.entries = &fc::unicode_cp437_pairs[0];
        setUnicodeMap(&unimap);
      }
      else
        VGAFont = false;

      detectTermSize();
      closeConsole();
    }
    else
      VGAFont = false;

    pc_charset_console = true;
    term_encoding = fc::PC;
    Fputchar = &FTerm::putchar_ASCII;
  }
#endif
  else
    VGAFont = false;

  return VGAFont;
}

//----------------------------------------------------------------------
bool FTerm::setNewFont()
{
  if ( NewFont )
    return true;

  if ( gnome_terminal
    || kde_konsole
    || putty_terminal
    || tera_terminal
    || cygwin_terminal
    || mintty_terminal )
    return false;

  if ( xterm_terminal || screen_terminal
    || urxvt_terminal || FTermcap::osc_support )
  {
    NewFont = true;
    // Set font in xterm to 8x16graph
    oscPrefix();
    putstring (OSC "50;8x16graph" BEL);
    oscPostfix();
    std::fflush(stdout);
    pc_charset_console = true;
    term_encoding = fc::PC;

    if ( xterm_terminal && utf8_console )
      Fputchar = &FTerm::putchar_UTF8;
    else
      Fputchar = &FTerm::putchar_ASCII;
  }
#if defined(__linux__)
  else if ( linux_terminal )
  {
    NewFont = true;

    if ( openConsole() == 0 )
    {
      if ( isLinuxConsole() )
      {
        struct unimapdesc unimap;
        int ret;

        // Set the graphical font 8x16
        ret = setScreenFont(fc::__8x16graph, 256, 8, 16);

        if ( ret != 0 )
          NewFont = false;

        // unicode character mapping
        unimap.entry_ct = uInt16 ( sizeof(fc::unicode_cp437_pairs)
                                 / sizeof(unipair) );
        unimap.entries = &fc::unicode_cp437_pairs[0];
        setUnicodeMap(&unimap);
      }

      detectTermSize();
      closeConsole();
    }

    pc_charset_console = true;
    term_encoding = fc::PC;
    Fputchar = &FTerm::putchar_ASCII;  // function pointer
  }
#endif
  else
    NewFont = false;

  return NewFont;
}

//----------------------------------------------------------------------
bool FTerm::setOldFont()
{
  bool retval;

  if ( ! (NewFont || VGAFont) )
    return false;

  retval  = \
  NewFont = \
  VGAFont = false;

  if ( xterm_terminal || screen_terminal
    || urxvt_terminal || FTermcap::osc_support )
  {
    if ( xterm_font && xterm_font->getLength() > 2 )
    {
      // restore saved xterm font
      oscPrefix();
      putstringf (OSC "50;%s" BEL, xterm_font->c_str() );
      oscPostfix();
    }
    else
    {
      // Set font in xterm to vga
      oscPrefix();
      putstring (OSC "50;vga" BEL);
      oscPostfix();
    }

    std::fflush(stdout);
    retval = true;
  }
#if defined(__linux__)
  else if ( linux_terminal )
  {
    if ( openConsole() == 0 )
    {
      if ( isLinuxConsole() )
      {
        if ( screen_font.data )
        {
          int ret = setScreenFont ( screen_font.data
                                  , screen_font.charcount
                                  , screen_font.width
                                  , screen_font.height
                                  , true );
          delete[] screen_font.data;

          if ( ret == 0 )
            retval = true;
        }

        if ( screen_unicode_map.entries )
        {
          setUnicodeMap (&screen_unicode_map);
          delete[] screen_unicode_map.entries;
        }
      }

      detectTermSize();
      closeConsole();
    }
  }
#endif

  return retval;
}

//----------------------------------------------------------------------
char* FTerm::moveCursor (int xold, int yold, int xnew, int ynew)
{
  // returns the cursor move string
  if ( cursor_optimisation )
    return opti_move->moveCursor (xold, yold, xnew, ynew);
  else
    return tgoto(TCAP(fc::t_cursor_address), xnew, ynew);
}

//----------------------------------------------------------------------
void FTerm::printMoveDurations()
{
  opti_move->printDurations();
}

//----------------------------------------------------------------------
char* FTerm::enableCursor()
{
  char*& vs = TCAP(fc::t_cursor_visible);
  char*& ve = TCAP(fc::t_cursor_normal);

  if ( ve )
    return ve;
  else if ( vs )
    return vs;

  return 0;
}

//----------------------------------------------------------------------
char* FTerm::disableCursor()
{
  char*& vi = TCAP(fc::t_cursor_invisible);

  if ( vi )
    return vi;

  return 0;
}

//----------------------------------------------------------------------
void FTerm::detectTermSize()
{
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

  if ( xterm_terminal )
  {
    putstringf (CSI "8;%d;%dt", term_height, term_width);
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTerm::setKDECursor (fc::kdeKonsoleCursorShape style)
{
  // Set cursor style in KDE konsole

  if ( kde_konsole )
  {
    oscPrefix();
    putstringf (OSC "50;CursorShape=%d" BEL, style);
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
const FString* FTerm::getXTermFont()
{
  if ( xterm_terminal || screen_terminal || FTermcap::osc_support )
  {
    fd_set ifds;
    struct timeval tv;

    oscPrefix();
    putstring (OSC "50;?" BEL);  // get font
    oscPostfix();
    std::fflush(stdout);

    FD_ZERO(&ifds);
    FD_SET(stdin_no, &ifds);
    tv.tv_sec  = 0;
    tv.tv_usec = 150000;  // 150 ms

    // read the terminal answer
    if ( select (stdin_no + 1, &ifds, 0, 0, &tv) > 0 )
    {
      char temp[150] = {};

      if ( std::scanf("\033]50;%148[^\n]s", temp) == 1 )
      {
        FString* xtermfont;
        std::size_t n = std::strlen(temp);

        // BEL + '\0' = string terminator
        if ( n >= 5 && temp[n - 1] == BEL[0] && temp[n] == '\0' )
          temp[n - 1] = '\0';

        try
        {
          xtermfont = new FString(temp);
        }
        catch (const std::bad_alloc& ex)
        {
          std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
          return 0;
        }

        return xtermfont;
      }
    }
  }

  return 0;
}

//----------------------------------------------------------------------
const FString* FTerm::getXTermTitle()
{
  if ( kde_konsole )
    return 0;

  fd_set ifds;
  struct timeval tv;

  putstring (CSI "21t");  // get title
  std::fflush(stdout);

  FD_ZERO(&ifds);
  FD_SET(stdin_no, &ifds);
  tv.tv_sec  = 0;
  tv.tv_usec = 150000;  // 150 ms

  // read the terminal answer
  if ( select (stdin_no + 1, &ifds, 0, 0, &tv) > 0 )
  {
    char temp[512] = {};

    if ( std::scanf("\033]l%509[^\n]s", temp) == 1 )
    {
      std::size_t n = std::strlen(temp);

      // Esc + \ = OSC string terminator
      if ( n >= 2 && temp[n - 2] == ESC[0] && temp[n - 1] == '\\' )
      {
        FString* xtermtitle;

        if ( n < 4 )
          return 0;

        temp[n - 2] = '\0';

        try
        {
          xtermtitle = new FString(temp);
        }
        catch (const std::bad_alloc& ex)
        {
          std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
          return 0;
        }

        return xtermtitle;
      }
    }
  }

  return 0;
}

//----------------------------------------------------------------------
const FString FTerm::getXTermColorName (int color)
{
  FString color_str("");

  fd_set ifds;
  struct timeval tv;

  char temp[512] = {};
  putstringf (OSC "4;%d;?" BEL, color);  // get color
  std::fflush(stdout);

  FD_ZERO(&ifds);
  FD_SET(stdin_no, &ifds);
  tv.tv_sec  = 0;
  tv.tv_usec = 150000;  // 150 ms

  // read the terminal answer
  if ( select (stdin_no + 1, &ifds, 0, 0, &tv) > 0 )
  {
    if ( std::scanf("\033]4;%10d;%509[^\n]s", &color, temp) == 2 )
    {
      std::size_t n = std::strlen(temp);

      // BEL + '\0' = string terminator
      if ( n >= 6 && temp[n - 1] == BEL[0] && temp[n] == '\0' )
        temp[n - 1] = '\0';

      // Esc + \ = OSC string terminator (mintty)
      if ( n >= 6 && temp[n - 2] == ESC[0] && temp[n - 1] == '\\' )
        temp[n - 2] = '\0';

      color_str = temp;
    }
  }

  return color_str;
}

//----------------------------------------------------------------------
void FTerm::setXTermCursorStyle (fc::xtermCursorStyle style)
{
  // Set the xterm cursor style

  if ( (gnome_terminal && ! decscusr_support) || kde_konsole )
    return;

#if defined(__FreeBSD__) || defined(__DragonFly__)
  if ( isFreeBSDConsole() )
    return;
#endif

  if ( xterm_terminal || mintty_terminal || decscusr_support )
  {
    putstringf (CSI "%d q", style);
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTerm::setXTermTitle (const FString& title)
{
  // Set the xterm title
  if ( xterm_terminal || screen_terminal
    || mintty_terminal || putty_terminal
    || FTermcap::osc_support )
  {
    oscPrefix();
    putstringf (OSC "0;%s" BEL, title.c_str());
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTerm::setXTermForeground (const FString& fg)
{
  // Set the VT100 text foreground color
  if ( xterm_terminal || screen_terminal
    || mintty_terminal || mlterm_terminal
    || FTermcap::osc_support )
  {
    oscPrefix();
    putstringf (OSC "10;%s" BEL, fg.c_str());
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTerm::setXTermBackground (const FString& bg)
{
  // Set the VT100 text background color
  if ( xterm_terminal || screen_terminal
    || mintty_terminal || mlterm_terminal
    || FTermcap::osc_support )
  {
    oscPrefix();
    putstringf (OSC "11;%s" BEL, bg.c_str());
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTerm::setXTermCursorColor (const FString& cc)
{
  // Set the text cursor color
  if ( xterm_terminal || screen_terminal
    || mintty_terminal || urxvt_terminal
    || FTermcap::osc_support )
  {
    oscPrefix();
    putstringf (OSC "12;%s" BEL, cc.c_str());
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTerm::setXTermMouseForeground (const FString& mfg)
{
  // Set the mouse foreground color
  if ( xterm_terminal || screen_terminal
    || urxvt_terminal || FTermcap::osc_support )
  {
    oscPrefix();
    putstringf (OSC "13;%s" BEL, mfg.c_str());
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTerm::setXTermMouseBackground (const FString& mbg)
{
  // Set the mouse background color
  if ( xterm_terminal || screen_terminal || FTermcap::osc_support )
  {
    oscPrefix();
    putstringf (OSC "14;%s" BEL, mbg.c_str());
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTerm::setXTermHighlightBackground (const FString& hbg)
{
  // Set the highlight background color
  if ( xterm_terminal || screen_terminal
    || urxvt_terminal || FTermcap::osc_support )
  {
    oscPrefix();
    putstringf (OSC "17;%s" BEL, hbg.c_str());
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTerm::setXTermDefaults()
{
  if ( putty_terminal )
    return;

  setXTermMouseBackground("rgb:ffff/ffff/ffff");
  setXTermMouseForeground ("rgb:0000/0000/0000");

  if ( ! gnome_terminal )
    setXTermCursorColor("rgb:ffff/ffff/ffff");

  if ( xterm_default_colors
    && ! (mintty_terminal || rxvt_terminal || screen_terminal) )
  {
    // mintty and rxvt can't reset these settings
    setXTermBackground("rgb:8080/a4a4/ecec");
    setXTermForeground("rgb:0000/0000/0000");
    setXTermHighlightBackground("rgb:8686/8686/8686");
  }
}

//----------------------------------------------------------------------
void FTerm::resetXTermColors()
{
  // Reset the entire color table

  if ( gnome_terminal && gnome_terminal_id < 3502 )
    return;

  if ( putty_terminal )
    return;

  if ( xterm_terminal || screen_terminal || FTermcap::osc_support )
  {
    oscPrefix();
    putstringf (OSC "104" BEL);
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTerm::resetXTermForeground()
{
  // Reset the VT100 text foreground color

  if ( gnome_terminal && gnome_terminal_id < 3502 )
    return;

  if ( putty_terminal )
    return;

  if ( xterm_terminal || screen_terminal || FTermcap::osc_support )
  {
    oscPrefix();
    putstring (OSC "110" BEL);
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTerm::resetXTermBackground()
{
  // Reset the VT100 text background color

  if ( gnome_terminal && gnome_terminal_id < 3502 )
    return;

  if ( putty_terminal )
    return;

  if ( xterm_terminal || screen_terminal || FTermcap::osc_support )
  {
    oscPrefix();
    putstring (OSC "111" BEL);
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTerm::resetXTermCursorColor()
{
  // Reset the text cursor color

  if ( gnome_terminal && gnome_terminal_id < 3502 )
    return;

  if ( putty_terminal )
    return;

  if ( xterm_terminal || screen_terminal || FTermcap::osc_support )
  {
    oscPrefix();
    putstring (OSC "112" BEL);
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTerm::resetXTermMouseForeground()
{
  // Reset the mouse foreground color

  if ( gnome_terminal && gnome_terminal_id < 3502 )
    return;

  if ( putty_terminal )
    return;

  if ( xterm_terminal || screen_terminal || FTermcap::osc_support )
  {
    oscPrefix();
    putstring (OSC "113" BEL);
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTerm::resetXTermMouseBackground()
{
  // Reset the mouse background color

  if ( gnome_terminal && gnome_terminal_id < 3502 )
    return;

  if ( putty_terminal )
    return;

  if ( xterm_terminal || screen_terminal || FTermcap::osc_support )
  {
    oscPrefix();
    putstring (OSC "114" BEL);
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTerm::resetXTermHighlightBackground()
{
  // Reset the highlight background color

  if ( gnome_terminal && gnome_terminal_id < 3502 )
    return;

  if ( putty_terminal )
    return;

  if ( xterm_terminal || screen_terminal
    || urxvt_terminal || FTermcap::osc_support )
  {
    oscPrefix();
    putstringf (OSC "117" BEL);
    oscPostfix();
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTerm::resetXTermDefaults()
{
  if ( putty_terminal )
    return;

  setXTermCursorColor("rgb:b1b1/b1b1/b1b1");
  resetXTermMouseForeground();
  resetXTermMouseBackground();
  resetXTermCursorColor();

  if ( xterm_default_colors )
  {
    resetXTermForeground();
    resetXTermBackground();
    resetXTermHighlightBackground();
  }
}

//----------------------------------------------------------------------
void FTerm::saveColorMap()
{
  //ioctl (0, GIO_CMAP, &color_map);
}

//----------------------------------------------------------------------
void FTerm::resetColorMap()
{
  char*& op = TCAP(fc::t_orig_pair);
  char*& oc = TCAP(fc::t_orig_colors);

  if ( op )
    putstring (op);
  else if ( oc )
    putstring (oc);
/*else
  {
    dacreg CurrentColors[16] =
    {
      {0x00, 0x00, 0x00}, {0xAA, 0x00, 0x00},
      {0x00, 0xAA, 0x00}, {0xAA, 0x55, 0x00},
      {0x00, 0x00, 0xAA}, {0xAA, 0x00, 0xAA},
      {0x00, 0xAA, 0xAA}, {0xAA, 0xAA, 0xAA},
      {0x55, 0x55, 0x55}, {0xFF, 0x55, 0x55},
      {0x55, 0xFF, 0x55}, {0xFF, 0xFF, 0x55},
      {0x55, 0x55, 0xFF}, {0xFF, 0x55, 0xFF},
      {0x55, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF}
    };
    for (int x = 0; x < 16; x++)
    {
      color_map.d[x].red = CurrentColors[x].red;
      color_map.d[x].green = CurrentColors[x].green;
      color_map.d[x].blue = CurrentColors[x].blue;
    }
    ioctl (0, PIO_CMAP, &color_map);
  }*/

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
  else if ( linux_terminal )
  {
/*  // direct vga-register set
    if ( r>=0 && r<256
      && g>=0 && g<256
      && b>=0 && b<256 )
    {
      map.d[index].red = r;
      map.d[index].green = g;
      map.d[index].blue = b;
    }
    ioctl (0, PIO_CMAP, &map); */
  }

  std::fflush(stdout);
}

//----------------------------------------------------------------------
void FTerm::setBeep (int Hz, int ms)
{
  if ( ! linux_terminal )
    return;

  // range for frequency: 21-32766
  if ( Hz < 21 || Hz > 32766 )
    return;

  // range for duration:  0-1999
  if ( ms < 0 || ms > 1999 )
    return;

  putstringf ( CSI "10;%d]"
               CSI "11;%d]"
             , Hz, ms );
  std::fflush(stdout);
}

//----------------------------------------------------------------------
void FTerm::resetBeep()
{
  if ( ! linux_terminal )
    return;

  // default frequency: 750 Hz
  // default duration:  125 ms
  putstring ( CSI "10;750]"
              CSI "11;125]" );
  std::fflush(stdout);
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
        || term_encoding == fc::VT100
        || term_encoding == fc::PC
        || term_encoding == fc::ASCII );

  // Set the new Fputchar function pointer
  switch ( term_encoding )
  {
    case fc::UTF8:
      Fputchar = &FTerm::putchar_UTF8;
      break;

    case fc::VT100:
    case fc::PC:
      if ( xterm_terminal && utf8_console )
        Fputchar = &FTerm::putchar_UTF8;
      else
        Fputchar = &FTerm::putchar_ASCII;
      break;

    case fc::ASCII:
    case fc::UNKNOWN:
    case fc::NUM_OF_ENCODINGS:
      Fputchar = &FTerm::putchar_ASCII;
  }

  if ( linux_terminal )
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
const FString FTerm::getAnswerbackMsg()
{
  FString answerback = "";

  fd_set ifds;
  struct timeval tv;
  char temp[10] = {};

  std::putchar (ENQ[0]);  // Send enquiry character
  std::fflush(stdout);

  FD_ZERO(&ifds);
  FD_SET(stdin_no, &ifds);
  tv.tv_sec  = 0;
  tv.tv_usec = 150000;  // 150 ms

  // Read the answerback message
  if ( select (stdin_no + 1, &ifds, 0, 0, &tv) > 0 )
    if ( std::fgets (temp, sizeof(temp) - 1, stdin) != 0 )
      answerback = temp;

  return answerback;
}

//----------------------------------------------------------------------
const FString FTerm::getSecDA()
{
  FString sec_da_str = "";

  int a = 0
    , b = 0
    , c = 0;
  fd_set ifds;
  struct timeval tv;

  // Get the secondary device attributes
#if defined(__CYGWIN__)
  puts (SECDA);
#else
  putstring (SECDA);
#endif
  std::fflush(stdout);

  FD_ZERO(&ifds);
  FD_SET(stdin_no, &ifds);
  tv.tv_sec  = 0;
  tv.tv_usec = 600000;  // 600 ms

  // Read the answer
  if ( select (stdin_no + 1, &ifds, 0, 0, &tv) == 1 )
    if ( std::scanf("\033[>%10d;%10d;%10dc", &a, &b, &c) == 3 )
      sec_da_str.sprintf("\033[>%d;%d;%dc", a, b, c);

  return sec_da_str;
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
int FTerm::putchar_ASCII (register char c)
{
  if ( std::putchar(c) == EOF )
    return 0;
  else
    return 1;
}
#endif

//----------------------------------------------------------------------
int FTerm::putchar_ASCII (register int c)
{
  if ( std::putchar(char(c)) == EOF )
    return 0;
  else
    return 1;
}

//----------------------------------------------------------------------
int FTerm::putchar_UTF8 (register int c)
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

//----------------------------------------------------------------------
int FTerm::UTF8decode (const char utf8[])
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


// protected methods of FTerm
//----------------------------------------------------------------------
#if defined(__linux__)
void FTerm::initLinuxConsoleCharMap()
{
  uInt c1, c2, c3, c4, c5;

  if ( NewFont || VGAFont )
    return;

  if ( screen_unicode_map.entry_ct != 0 )
  {
    for (int i = 0; i <= fc::lastCharItem; i++ )
    {
      bool found = false;

      for (uInt n = 0; n < screen_unicode_map.entry_ct; n++)
      {
        if ( fc::character[i][fc::UTF8] == screen_unicode_map.entries[n].unicode )
        {
          found = true;
          break;
        }
      }

      if ( ! found )
        fc::character[i][fc::PC] = fc::character[i][fc::ASCII];
    }
  }

  c1 = fc::UpperHalfBlock;
  c2 = fc::LowerHalfBlock;
  c3 = fc::FullBlock;

  if ( charEncode(c1, fc::PC) == charEncode(c1, fc::ASCII)
    || charEncode(c2, fc::PC) == charEncode(c2, fc::ASCII)
    || charEncode(c3, fc::PC) == charEncode(c3, fc::ASCII) )
  {
    no_shadow_character = true;
  }

  c4 = fc::RightHalfBlock;
  c5 = fc::LeftHalfBlock;

  if ( charEncode(c4, fc::PC) == charEncode(c4, fc::ASCII)
    || charEncode(c5, fc::PC) == charEncode(c5, fc::ASCII) )
  {
    no_half_block_character = true;
  }
}
#endif

#if defined(__FreeBSD__) || defined(__DragonFly__)
//----------------------------------------------------------------------
void FTerm::initFreeBSDConsoleCharMap()
{
  // A FreeBSD console can't show ASCII codes from 0x00 to 0x1b

  if ( ! isFreeBSDConsole() )
    return;

  for (int i = 0; i <= fc::lastCharItem; i++ )
    if ( character[i][fc::PC] < 0x1c )
      character[i][fc::PC] = character[i][fc::ASCII];
}
#endif

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
    if ( fc::character[i][0] == c )
    {
      c = fc::character[i][enc];
      break;
    }
  }

  return c;
}

//----------------------------------------------------------------------
char* FTerm::changeAttribute ( char_data*& term_attr
                             , char_data*& next_attr )
{
  return opti_attr->changeAttribute (term_attr, next_attr);
}

//----------------------------------------------------------------------
void FTerm::xtermMetaSendsESC (bool on)
{
  // activate/deactivate the xterm meta key sends escape prefix

  if ( on )
    putstring (CSI "?1036s"    // save meta key sends escape
               CSI "?1036h");  // enable meta key sends escape
  else
    putstring (CSI "?1036r");  // restore meta key sends escape

  std::fflush(stdout);
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
#if defined(__linux__)
int FTerm::isLinuxConsole()
{
  char arg = 0;
  // get keyboard type an compare
  return ( isatty (fd_tty)
        && ioctl(fd_tty, KDGKBTYPE, &arg) == 0
        && ((arg == KB_101) || (arg == KB_84)) );
}
#endif

#if defined(__FreeBSD__) || defined(__DragonFly__)
//----------------------------------------------------------------------
bool FTerm::isFreeBSDConsole()
{
  keymap_t keymap;

  if ( ioctl(0, GIO_KEYMAP, &keymap) == 0 )
    return true;
  else
    return false;
}
#endif

//----------------------------------------------------------------------
#if defined(__NetBSD__) || defined(__OpenBSD__)
bool FTerm::isWSConsConsole()
{
  static kbd_t kbdencoding;

  if ( ioctl(0, WSKBDIO_GETENCODING, &kbdencoding) == 0 )
    return true;
  else
    return false;
}
#endif

#if defined(__linux__)
#if defined(__x86_64__) || defined(__i386) || defined(__arm__)
//----------------------------------------------------------------------
inline uInt16 FTerm::getInputStatusRegisterOne()
{
  // Gets the VGA input-status-register-1

  // Miscellaneous output (read port)
  static const uInt16 misc_read = 0x3cc;
  const uInt16 io_base = ( inb(misc_read) & 0x01 ) ? 0x3d0 : 0x3b0;
  // 0x3ba : Input status 1 MDA (read port)
  // 0x3da : Input status 1 CGA (read port)
  return io_base + 0x0a;
}

//----------------------------------------------------------------------
uChar FTerm::readAttributeController (uChar index)
{
  // Reads a byte from the attribute controller from a given index

  uChar res;
  // Attribute controller (write port)
  static const uInt16 attrib_cntlr_write = 0x3c0;
  // Attribute controller (read port)
  static const uInt16 attrib_cntlr_read  = 0x3c1;
  const uInt16 input_status_1 = getInputStatusRegisterOne();

  inb (input_status_1);  // switch to index mode
  outb (index & 0x1f, attrib_cntlr_write);
  res = inb (attrib_cntlr_read);

  inb (input_status_1);  // switch to data mode
  index = (index & 0x1f) | 0x20;  // set bit 5 (enable display)
  outb (index, attrib_cntlr_write);
  inb (attrib_cntlr_read);
  return res;
}

//----------------------------------------------------------------------
void FTerm::writeAttributeController (uChar index, uChar data)
{
  // Writes a byte from the attribute controller from a given index

  // Attribute controller (write port)
  static const uInt16 attrib_cntlr_write = 0x3c0;
  const uInt16 input_status_1 = getInputStatusRegisterOne();

  inb (input_status_1);  // switch to index mode
  outb (index & 0x1f, attrib_cntlr_write);
  outb (data, attrib_cntlr_write);

  inb (input_status_1);  // switch to data mode
  index = (index & 0x1f) | 0x20;  // set bit 5 (enable display)
  outb (index, attrib_cntlr_write);
  outb (data, attrib_cntlr_write);
}

//----------------------------------------------------------------------
inline uChar FTerm::getAttributeMode()
{
  // Gets the attribute mode value from the vga attribute controller
  static const uChar attrib_mode = 0x10;
  return readAttributeController(attrib_mode);
}

//----------------------------------------------------------------------
inline void FTerm::setAttributeMode (uChar data)
{
  // Sets the attribute mode value from the vga attribute controller
  static const uChar attrib_mode = 0x10;
  writeAttributeController (attrib_mode, data);
}

//----------------------------------------------------------------------
int FTerm::setBlinkAsIntensity (bool on)
{
  // Uses blink-bit as background intensity.
  // That permits 16 colors for background

  // Test if the blink-bit is used by the screen font (512 characters)
  if ( screen_font.charcount > 256 )
    return -1;

  if ( getuid() != 0 )  // Direct hardware access requires root privileges
    return -2;

  if ( fd_tty < 0 )
    return -1;

  // Enable access to VGA I/O ports  (from 0x3B4 with num = 0x2C)
  if ( ioctl(fd_tty, KDENABIO, 0) < 0 )
    return -1;  // error on KDENABIO

  if ( on )
    setAttributeMode (getAttributeMode() & 0xF7);  // clear bit 3
  else
    setAttributeMode (getAttributeMode() | 0x08);  // set bit 3

  // Disable access to VGA I/O ports
  if ( ioctl(fd_tty, KDDISABIO, 0) < 0 )
    return -1;  // error on KDDISABIO

  return 0;
}
#endif

//----------------------------------------------------------------------
int FTerm::getFramebuffer_bpp ()
{
  int fd = -1;
  struct fb_var_screeninfo fb_var;
  struct fb_fix_screeninfo fb_fix;

  const char* fb = C_STR("/dev/fb/0");

  if ( (fd = open(fb, O_RDWR)) < 0 )
  {
    if ( errno != ENOENT && errno != ENOTDIR )
      return -1;

    fb = C_STR("/dev/fb0");

    if ( (fd = open(fb, O_RDWR)) < 0 )
      return -1;
  }

  if ( ! ioctl(fd, FBIOGET_VSCREENINFO, &fb_var)
    && ! ioctl(fd, FBIOGET_FSCREENINFO, &fb_fix) )
  {
    ::close(fd);
    return int(fb_var.bits_per_pixel);
  }
  else
  {
    ::close(fd);
  }

  return -1;
}
#endif

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
void FTerm::getSystemTermType()
{
  // Import the untrusted environment variable TERM
  const char* const& term_env = std::getenv(C_STR("TERM"));

  if ( term_env )
  {
    // Save name in termtype
    std::strncpy (termtype, term_env, sizeof(termtype) - 1);
    return;
  }
  else if ( *termfilename )  // 1st fallback: use the teminal file name
  {
    getTTYtype();  // Look into /etc/ttytype

#if F_HAVE_GETTTYNAM
    if ( getTTYSFileEntry() )  // Look into /etc/ttys
      return;
#endif
  }

  // 2nd fallback: use vt100 if not found
  std::strncpy (termtype, C_STR("vt100"), 6);
}

//----------------------------------------------------------------------
void FTerm::getTTYtype()
{
  // Analyse /etc/ttytype and get the term name
  // ------------------------------------------
  // file format:
  // <terminal type> <whitespace> <tty name>
  //
  // Example:
  // linux  tty1
  // vt100  ttys0

  // Get term basename
  const char* term_basename = std::strrchr(termfilename, '/');

  if ( term_basename == 0 )
    term_basename = termfilename;
  else
    term_basename++;

  std::FILE *fp;

  if ( (fp = std::fopen("/etc/ttytype", "r")) != 0 )
  {
    char* p;
    char  str[BUFSIZ];

    // Read and parse the file
    while ( fgets(str, sizeof(str) - 1, fp) != 0 )
    {
      char* name;
      char* type;
      type = name = 0;  // 0 == not found
      p = str;

      while ( *p )
      {
        if ( std::isspace(uChar(*p)) )
          *p = '\0';
        else if ( type == 0 )
          type = p;
        else if ( name == 0 && p != str && p[-1] == '\0' )
          name = p;

        p++;
      }

      if ( type != 0 && name != 0 && ! std::strcmp(name, term_basename) )
      {
        // Save name in termtype
        std::strncpy (termtype, type, sizeof(termtype) - 1);
        std::fclose(fp);
        return;
      }
    }

    std::fclose(fp);
  }
}

#if F_HAVE_GETTTYNAM
//----------------------------------------------------------------------
bool FTerm::getTTYSFileEntry()
{
  // Analyse /etc/ttys and get the term name

  // get term basename
  const char* term_basename = std::strrchr(termfilename, '/');

  if ( term_basename == 0 )
    term_basename = termfilename;
  else
    term_basename++;

  struct ttyent* ttys_entryt;
  ttys_entryt = getttynam(term_basename);

  if ( ttys_entryt )
  {
    char* type = ttys_entryt->ty_type;

    if ( type != 0 )
    {
      // Save name in termtype
      std::strncpy (termtype, type, sizeof(termtype) - 1);
      endttyent();
      return true;
    }
  }

  endttyent();
  return false;
}
#endif

//----------------------------------------------------------------------
void FTerm::storeTTYsettings()
{
  // store termios settings
  term_init = getTTY();
}

//----------------------------------------------------------------------
void FTerm::restoreTTYsettings()
{
  // restore termios settings
  setTTY (term_init);
}

#if defined(__linux__)
//----------------------------------------------------------------------
int FTerm::getScreenFont()
{
  static const std::size_t data_size = 4 * 32 * 512;
  struct console_font_op font;

  int ret;

  if ( fd_tty < 0 )
    return -1;

  // initialize unused padding bytes in struct
  std::memset (&font, 0, sizeof(console_font_op));

  font.op = KD_FONT_OP_GET;
  font.flags = 0;
  font.width = 32;
  font.height = 32;
  font.charcount = 512;

  // initialize with 0
  try
  {
    font.data = new uChar[data_size]();
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
    return -1;
  }

  // font operation
  ret = ioctl (fd_tty, KDFONTOP, &font);

  if ( ret == 0 )
  {
    screen_font.width = font.width;
    screen_font.height = font.height;
    screen_font.charcount = font.charcount;
    screen_font.data = font.data;
    return 0;
  }
  else
    return -1;
}

//----------------------------------------------------------------------
int FTerm::setScreenFont ( uChar fontdata[], uInt count
                         , uInt fontwidth, uInt fontheight
                         , bool direct)
{
  struct console_font_op font;
  int ret;

  if ( fd_tty < 0 )
    return -1;

  // initialize unused padding bytes in struct
  std::memset (&font, 0x00, sizeof(console_font_op));

  font.op = KD_FONT_OP_SET;
  font.flags = 0;
  font.width = fontwidth;
  font.height = fontheight;
  font.charcount = count;

  if ( direct )
    font.data = fontdata;
  else
  {
    const uInt bytes_per_line = font.width / 8;
    const std::size_t data_size = bytes_per_line * 32 * font.charcount;

    try
    {
      font.data = new uChar[data_size]();  // initialize with 0
    }
    catch (const std::bad_alloc& ex)
    {
      std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
      return -1;
    }

    for (uInt i = 0; i < count; i++)
      std::memcpy ( const_cast<uChar*>(font.data + bytes_per_line * 32 * i)
                  , &fontdata[i * font.height]
                  , font.height);
  }
  // font operation
  ret = ioctl (fd_tty, KDFONTOP, &font);

  if ( ret != 0 && errno != ENOSYS && errno != EINVAL )
  {
    if ( ! direct )
      delete[] font.data;

    return -1;
  }

  if ( ! direct )
    delete[] font.data;

  if ( ret == 0 )
    return 0;
  else
    return -1;
}

//----------------------------------------------------------------------
int FTerm::getUnicodeMap()
{
  struct unimapdesc unimap;
  int ret;

  if ( fd_tty < 0 )
    return -1;

  unimap.entry_ct = 0;
  unimap.entries = 0;

  // get count
  ret = ioctl (fd_tty, GIO_UNIMAP, &unimap);

  if ( ret != 0 )
  {
    int count;

    if ( errno != ENOMEM || unimap.entry_ct == 0 )
      return -1;

    count = unimap.entry_ct;

    try
    {
      unimap.entries = new struct unipair[count]();
    }
    catch (const std::bad_alloc& ex)
    {
      std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
      return -1;
    }

    // get unicode-to-font mapping from kernel
    ret = ioctl(fd_tty, GIO_UNIMAP, &unimap);

    if ( ret == 0 )
    {
      screen_unicode_map.entry_ct = unimap.entry_ct;
      screen_unicode_map.entries = unimap.entries;
    }
    else
      return -1;
  }

  return 0;
}

//----------------------------------------------------------------------
int FTerm::setUnicodeMap (struct unimapdesc* unimap)
{
  struct unimapinit advice;
  int ret;

  if ( fd_tty < 0 )
    return -1;

  advice.advised_hashsize = 0;
  advice.advised_hashstep = 0;
  advice.advised_hashlevel = 0;

  do
  {
    // clear the unicode-to-font table
    ret = ioctl(fd_tty, PIO_UNIMAPCLR, &advice);

    if ( ret != 0 )
      return -1;

    // put the new unicode-to-font mapping in kernel
    ret = ioctl(fd_tty, PIO_UNIMAP, unimap);

    if ( ret != 0 )
      advice.advised_hashlevel++;
  }
  while ( ret != 0 && errno == ENOMEM && advice.advised_hashlevel < 100);

  if ( ret == 0 )
    return 0;
  else
    return -1;
}

//----------------------------------------------------------------------
void FTerm::initLinuxConsole()
{
  // initialize Linux console

  screen_unicode_map.entries = 0;
  screen_font.data = 0;

  if ( openConsole() == 0 )
  {
    linux_terminal = isLinuxConsole();

    if ( linux_terminal )
    {
      getUnicodeMap();
      getScreenFont();

#if defined(__x86_64__) || defined(__i386) || defined(__arm__)
      // Enable 16 background colors
      if ( setBlinkAsIntensity(true) == 0 )
        FTermcap::max_color = 16;
      else
        FTermcap::max_color = 8;
#endif
      // Underline cursor
      setLinuxConsoleCursorStyle (fc::underscore_cursor, true);

      // Framebuffer color depth in bits per pixel
      int bpp = getFramebuffer_bpp();

      // More than 4 bits per pixel and the font uses the blink-bit
      if ( bpp >= 4 && screen_font.charcount <= 256 )
        FTermcap::max_color = 16;

#if DEBUG
    framebuffer_bpp = bpp;
#endif
    }

    detectTermSize();
    closeConsole();
  }
  else
  {
    std::cerr << "can not open the console.\n";
    std::abort();
  }
}
#endif

#if defined(__FreeBSD__) || defined(__DragonFly__)
//----------------------------------------------------------------------
bool FTerm::saveFreeBSDAltKey()
{
  static const int left_alt = 0x38;
  int ret;
  keymap_t keymap;

  ret = ioctl(0, GIO_KEYMAP, &keymap);

  if ( ret < 0 )
    return false;

  // save current mapping
  bsd_alt_keymap = keymap.key[left_alt].map[0];
  return true;
}

//----------------------------------------------------------------------
bool FTerm::setFreeBSDAltKey (uInt key)
{
  static const int left_alt = 0x38;
  int ret;
  keymap_t keymap;

  ret = ioctl(0, GIO_KEYMAP, &keymap);

  if ( ret < 0 )
    return false;

  // map to meta key
  keymap.key[left_alt].map[0] = key;

  if ( (keymap.n_keys > 0) && (ioctl(0, PIO_KEYMAP, &keymap) < 0) )
    return false;
  else
    return true;
}

//----------------------------------------------------------------------
bool FTerm::setFreeBSDAlt2Meta()
{
  return setFreeBSDAltKey (META);
}

//----------------------------------------------------------------------
bool FTerm::resetFreeBSDAlt2Meta()
{
  return setFreeBSDAltKey (bsd_alt_keymap);
}

//----------------------------------------------------------------------
void FTerm::initFreeBSDConsole()
{
  // initialize BSD console

  if ( isFreeBSDConsole() )
  {
    // save current left alt key mapping
    saveFreeBSDAltKey();

    // map meta key to left alt key
    setFreeBSDAlt2Meta();

    // Initialize FreeBSD console cursor
    setFreeBSDConsoleCursorStyle (fc::destructive_cursor, true);
  }
}
#endif

#if defined(__NetBSD__) || defined(__OpenBSD__)
//----------------------------------------------------------------------
bool FTerm::saveWSConsEncoding()
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
bool FTerm::setWSConsEncoding (kbd_t k_encoding)
{
  if ( ioctl(0, WSKBDIO_SETENCODING, &k_encoding) < 0 )
    return false;
  else
    return true;
}

//----------------------------------------------------------------------
bool FTerm::setWSConsMetaEsc()
{
  static const kbd_t meta_esc = 0x20;  // generate ESC prefix on ALT-key

  return setWSConsEncoding (wscons_keyboard_encoding | meta_esc);
}

//----------------------------------------------------------------------
bool FTerm::resetWSConsEncoding()
{
  return setWSConsEncoding (wscons_keyboard_encoding);
}

//----------------------------------------------------------------------
void FTerm::initWSConsConsole()
{
  // initialize wscons console

  if ( isWSConsConsole() )
  {
    // save current left alt key mapping
    saveWSConsEncoding();

    // alt key generate ESC prefix
    setWSConsMetaEsc();
  }
}
#endif

//----------------------------------------------------------------------
uInt FTerm::getBaudRate (const struct termios* termios_p)
{
  std::map<speed_t,uInt> outspeed;
  outspeed[B0]      = 0;       // hang up
  outspeed[B50]     = 50;      //      50 baud
  outspeed[B75]     = 75;      //      75 baud
  outspeed[B110]    = 110;     //     110 baud
  outspeed[B134]    = 134;     //     134.5 baud
  outspeed[B150]    = 150;     //     150 baud
  outspeed[B200]    = 200;     //     200 baud
  outspeed[B300]    = 300;     //     300 baud
  outspeed[B600]    = 600;     //     600 baud
  outspeed[B1200]   = 1200;    //   1,200 baud
  outspeed[B1800]   = 1800;    //   1,800 baud
  outspeed[B2400]   = 2400;    //   2,400 baud
  outspeed[B4800]   = 4800;    //   4,800 baud
  outspeed[B9600]   = 9600;    //   9,600 baud
  outspeed[B19200]  = 19200;   //  19,200 baud
  outspeed[B38400]  = 38400;   //  38,400 baud
  outspeed[B57600]  = 57600;   //  57,600 baud
  outspeed[B115200] = 115200;  // 115,200 baud
  outspeed[B230400] = 230400;  // 230,400 baud

  return outspeed[cfgetospeed(termios_p)];
}

//----------------------------------------------------------------------
void FTerm::init_global_values()
{
  // Initialize global values

  // Teletype (tty) file descriptor is still undefined
  fd_tty = -1;

  // Gnome terminal id from SecDA
  // Example: vte version 0.40.0 = 0 * 100 + 40 * 100 + 0 = 4000
  //                      a.b.c  = a * 100 +  b * 100 + c
  gnome_terminal_id = 0;

  // Set default timeout for keypress
  key_timeout = 100000;  // 100 ms

  // Preset to false
  utf8_console            = \
  utf8_input              = \
  utf8_state              = \
  utf8_linux_terminal     = \
  pc_charset_console      = \
  vt100_console           = \
  NewFont                 = \
  VGAFont                 = \
  no_shadow_character     = \
  no_half_block_character = \
  ascii_console           = \
  decscusr_support        = \
  force_vt100             = \
  tera_terminal           = \
  sun_terminal            = \
  kterm_terminal          = \
  gnome_terminal          = \
  kde_konsole             = \
  ansi_terminal           = \
  rxvt_terminal           = \
  urxvt_terminal          = \
  mlterm_terminal         = \
  mintty_terminal         = \
  openbsd_terminal        = \
  screen_terminal         = \
  tmux_terminal           = \
  xterm_default_colors    = false;

  // Preset to true
  cursor_optimisation     = \
  terminal_detection      = true;

  // Assertion: programm start in cooked mode
  raw_mode                = \
  input_data_pending      = \
  non_blocking_stdin      = false;

  // Init arrays with '\0'
  std::fill_n (exit_message, sizeof(exit_message), '\0');

  // Initialize the structs
  color_env.setDefault();
  secondary_da.setDefault();

  if ( ! init_values.terminal_detection )
    terminal_detection = false;
}

//----------------------------------------------------------------------
void FTerm::detectTerminal()
{
  // Terminal detection

  char* new_termtype = 0;

  if ( ! terminal_detection )
  {
    struct termios t;
    tcgetattr (stdin_no, &t);
    t.c_cc[VTIME] = 1;  // Timeout in deciseconds
    t.c_cc[VMIN]  = 0;  // Minimum number of characters
    tcsetattr (stdin_no, TCSANOW, &t);
  }
  else
  {
    struct termios t;
    tcgetattr (stdin_no, &t);
    t.c_lflag &= uInt(~(ICANON | ECHO));
    t.c_cc[VTIME] = 1;  // Timeout in deciseconds
    t.c_cc[VMIN]  = 0;  // Minimum number of characters
    tcsetattr (stdin_no, TCSANOW, &t);

    // Initialize 256 colors terminals
    new_termtype = init_256colorTerminal();

    // Identify the terminal via the answerback-message
    new_termtype = parseAnswerbackMsg (new_termtype);

    // Identify the terminal via the secondary device attributes (SEC_DA)
    new_termtype = parseSecDA (new_termtype);

    // Determines the maximum number of colors
    new_termtype = determineMaxColor(new_termtype);

    if ( cygwin_terminal
      || putty_terminal
      || tera_terminal
      || rxvt_terminal )
    {
      FTermcap::max_color = 16;
    }

    t.c_lflag |= uInt(ICANON | ECHO);
    tcsetattr(stdin_no, TCSADRAIN, &t);
  }

  //
  // Additional termtype analysis
  //

  // Test if the terminal is a xterm
  if ( std::strncmp(termtype, C_STR("xterm"), 5) == 0
    || std::strncmp(termtype, C_STR("Eterm"), 5) == 0 )
  {
    xterm_terminal = true;

    // Each xterm should be able to use at least 16 colors
    if ( ! new_termtype && std::strlen(termtype) == 5 )
      new_termtype = C_STR("xterm-16color");
  }
  else
    xterm_terminal = false;

  // set the new environment variable TERM
  if ( new_termtype )
  {
    setenv(C_STR("TERM"), new_termtype, 1);
    std::strncpy (termtype, new_termtype, std::strlen(new_termtype) + 1);
  }
}

//----------------------------------------------------------------------
void FTerm::termtypeAnalysis()
{
  // Cygwin console
  if ( std::strncmp(termtype, "cygwin", 6) == 0 )
    cygwin_terminal = true;
  else
    cygwin_terminal = false;

  // rxvt terminal emulator (native MS Window System port) on cygwin
  if ( std::strncmp(termtype, "rxvt-cygwin-native", 18) == 0 )
    rxvt_terminal = true;

  // Ansi terminal
  if ( std::strncmp(termtype, "ansi", 4) == 0 )
  {
    terminal_detection = false;
    ansi_terminal = true;
  }

  // Sun Microsystems workstation console
  if ( std::strncmp(termtype, "sun", 3) == 0 )
  {
    terminal_detection = false;
    sun_terminal = true;
  }

  // Kterm
  if ( std::strncmp(termtype, "kterm", 5) == 0 )
  {
    terminal_detection = false;
    kterm_terminal = true;
  }

  // Linux console
  if ( std::strncmp(termtype, C_STR("linux"), 5) == 0
    || std::strncmp(termtype, C_STR("con"), 3) == 0 )
    linux_terminal = true;
  else
    linux_terminal = false;

  // NetBSD workstation console
  if ( std::strncmp(termtype, C_STR("wsvt25"), 6) == 0 )
    netbsd_terminal = true;
  else
    netbsd_terminal = false;
}

//----------------------------------------------------------------------
bool FTerm::get256colorEnvString()
{
  // Enable 256 color capabilities
  color_env.string1 = std::getenv("COLORTERM");
  color_env.string2 = std::getenv("VTE_VERSION");
  color_env.string3 = std::getenv("XTERM_VERSION");
  color_env.string4 = std::getenv("ROXTERM_ID");
  color_env.string5 = std::getenv("KONSOLE_DBUS_SESSION");
  color_env.string6 = std::getenv("KONSOLE_DCOP");

  if ( color_env.string1 != 0 )
    return true;

  if ( color_env.string2 != 0 )
    return true;

  if ( color_env.string3 != 0 )
    return true;

  if ( color_env.string4 != 0 )
    return true;

  if ( color_env.string5 != 0 )
    return true;

  if ( color_env.string6 != 0 )
    return true;

  return false;
}

//----------------------------------------------------------------------
char* FTerm::termtype_256color_quirks()
{
  char* new_termtype = 0;

  if ( ! color256 )
    return new_termtype;

  if ( std::strncmp(termtype, "xterm", 5) == 0 )
    new_termtype = C_STR("xterm-256color");

  if ( std::strncmp(termtype, "screen", 6) == 0 )
  {
    new_termtype = C_STR("screen-256color");
    screen_terminal = true;
    char* tmux = std::getenv("TMUX");

    if ( tmux && std::strlen(tmux) != 0 )
      tmux_terminal = true;
  }

  if ( std::strncmp(termtype, "Eterm", 5) == 0 )
    new_termtype = C_STR("Eterm-256color");

  if ( std::strncmp(termtype, "mlterm", 6) == 0 )
  {
    new_termtype = C_STR("mlterm-256color");
    mlterm_terminal = true;
  }

  if ( std::strncmp(termtype, "rxvt", 4) != 0
    && color_env.string1
    && std::strncmp(color_env.string1, "rxvt-xpm", 8) == 0 )
  {
    new_termtype = C_STR("rxvt-256color");
    rxvt_terminal = true;
  }

  if ( (color_env.string5 && std::strlen(color_env.string5) > 0)
    || (color_env.string6 && std::strlen(color_env.string6) > 0) )
    kde_konsole = true;

  if ( (color_env.string1 && std::strncmp(color_env.string1, "gnome-terminal", 14) == 0)
    || color_env.string2 )
  {
    gnome_terminal = true;
    // Each gnome-terminal should be able to use 256 colors
    color256 = true;

    if ( ! screen_terminal )
      new_termtype = C_STR("gnome-256color");
  }

  return new_termtype;
}

//----------------------------------------------------------------------
char* FTerm::init_256colorTerminal()
{
  char* new_termtype = 0;

  if ( get256colorEnvString() )
    color256 = true;
  else if ( std::strstr (termtype, "256color") )
    color256 = true;
  else
    color256 = false;

  new_termtype = termtype_256color_quirks();

#if DEBUG
  if ( new_termtype )
    std::strncpy ( termtype_256color
                 , new_termtype
                 , std::strlen(new_termtype) + 1 );
#endif

  return new_termtype;
}

//----------------------------------------------------------------------
char* FTerm::determineMaxColor (char current_termtype[])
{
  // Determine xterm maximum number of colors via OSC 4

  char* new_termtype = current_termtype;

  if ( ! color256
    && ! cygwin_terminal
    && ! tera_terminal
    && ! linux_terminal
    && ! netbsd_terminal
    && ! getXTermColorName(0).isEmpty() )
  {
    if ( ! getXTermColorName(256).isEmpty() )
    {
      if ( putty_terminal )
        new_termtype = C_STR("putty-256color");
      else
        new_termtype = C_STR("xterm-256color");
    }
    else if ( ! getXTermColorName(87).isEmpty() )
    {
      new_termtype = C_STR("xterm-88color");
    }
    else if ( ! getXTermColorName(15).isEmpty() )
    {
      new_termtype = C_STR("xterm-16color");
    }
  }

  return new_termtype;
}

//----------------------------------------------------------------------
char* FTerm::parseAnswerbackMsg (char current_termtype[])
{
  char* new_termtype = current_termtype;

  // send ENQ and read the answerback message
  try
  {
    answer_back = new FString(getAnswerbackMsg());
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
    return 0;
  }

  if ( *answer_back == "PuTTY" )
  {
    putty_terminal = true;

    if ( color256 )
      new_termtype = C_STR("putty-256color");
    else
      new_termtype = C_STR("putty");
  }
  else
    putty_terminal = false;

  // cygwin needs a backspace to delete the '♣' char
  if ( cygwin_terminal )
    putstring (BS " " BS);

#if DEBUG
  if ( new_termtype )
    std::strncpy ( termtype_Answerback
                 , new_termtype
                 , std::strlen(new_termtype) + 1 );
#endif

  return new_termtype;
}

//----------------------------------------------------------------------
char* FTerm::parseSecDA (char current_termtype[])
{
  // The Linux console and older cygwin terminals knows no Sec_DA
  if ( linux_terminal || cygwin_terminal )
    return current_termtype;

  try
  {
    // Secondary device attributes (SEC_DA) <- decTerminalID string
    sec_da = new FString(getSecDA());
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
    return current_termtype;
  }

  if ( sec_da->getLength() < 6 )
    return current_termtype;

  // remove the first 3 bytes ("\033[>")
  FString temp = sec_da->right(sec_da->getLength() - 3);
  // remove the last byte ("c")
  temp.remove(temp.getLength() - 1, 1);
  // split into components
  FStringList sec_da_list = temp.split(';');

  uLong num_components = sec_da_list.size();

  // The second device attribute (SEC_DA) always has 3 parameters,
  // otherwise it usually has a copy of the device attribute (primary DA)
  if ( num_components < 3 )
    return current_termtype;

  const FString* sec_da_components = &sec_da_list[0];

  if ( sec_da_components[0].isEmpty() )
    return current_termtype;

  // Read the terminal type
  try
  {
    secondary_da.terminal_id_type = sec_da_components[0].toInt();
  }
  catch (const std::exception&)
  {
    secondary_da.terminal_id_type = -1;
  }

  // Read the terminal (firmware) version
  try
  {
    if ( sec_da_components[1] )
      secondary_da.terminal_id_version = sec_da_components[1].toInt();
    else
      secondary_da.terminal_id_version = -1;
  }
  catch (const std::exception&)
  {
    secondary_da.terminal_id_version = -1;
  }

  // Read the terminal hardware option
  try
  {
    if ( sec_da_components[2] )
      secondary_da.terminal_id_hardware = sec_da_components[2].toInt();
    else
      secondary_da.terminal_id_hardware = -1;
  }
  catch (const std::exception&)
  {
    secondary_da.terminal_id_hardware = -1;
  }

  char* new_termtype = secDA_Analysis(current_termtype);

#if DEBUG
  if ( new_termtype )
    std::strncpy ( termtype_SecDA
                 , new_termtype
                 , std::strlen(new_termtype) + 1 );
#endif

  return new_termtype;
}

//----------------------------------------------------------------------
char* FTerm::secDA_Analysis (char current_termtype[])
{
  char* new_termtype = current_termtype;

  switch ( secondary_da.terminal_id_type )
  {
    case 0:  // DEC VT100
      new_termtype = secDA_Analysis_0(current_termtype);
      break;

    case 1:  // DEC VT220
      new_termtype = secDA_Analysis_1(current_termtype);
      break;

    case 2:   // DEC VT240
    case 18:  // DEC VT330
    case 19:  // DEC VT340

    case 24:  // DEC VT320
      new_termtype = secDA_Analysis_24(current_termtype);
      break;

    case 32:  // Tera Term
      new_termtype = secDA_Analysis_32(current_termtype);
      break;

    case 41:  // DEC VT420
    case 61:  // DEC VT510
    case 64:  // DEC VT520
    case 65:  // DEC VT525
    case 67:  // Cygwin
      break;

    case 77:  // mintty
      new_termtype = secDA_Analysis_77(current_termtype);
      break;

    case 82:  // rxvt
      new_termtype = secDA_Analysis_82(current_termtype);
      break;

    case 83:  // screen
      new_termtype = secDA_Analysis_83(current_termtype);
      break;

    case 85:  // rxvt-unicode
      new_termtype = secDA_Analysis_85(current_termtype);
      break;

    default:
      break;
  }

  // Correct false assumptions
  if ( gnome_terminal && secondary_da.terminal_id_type != 1 )
    gnome_terminal = false;

  if ( kde_konsole && secondary_da.terminal_id_type != 0 )
    kde_konsole = false;

  return new_termtype;
}

//----------------------------------------------------------------------
inline char* FTerm::secDA_Analysis_0 (char current_termtype[])
{
  // Terminal ID 0 - DEC VT100

  char* new_termtype = current_termtype;

  if ( secondary_da.terminal_id_version == 115 )
    kde_konsole = true;
  else if ( secondary_da.terminal_id_version == 136 )
    putty_terminal = true;  // PuTTY

  return new_termtype;
}

//----------------------------------------------------------------------
inline char* FTerm::secDA_Analysis_1 (char current_termtype[])
{
  // Terminal ID 1 - DEC VT220

  char* new_termtype = current_termtype;

  if ( secondary_da.terminal_id_version > 1000 )
  {
    gnome_terminal = true;
    // Each gnome-terminal should be able to use 256 colors
    color256 = true;
    new_termtype = C_STR("gnome-256color");
    gnome_terminal_id = secondary_da.terminal_id_version;

    // VTE 0.40.0 or higher and gnome-terminal 3.16 or higher
    if ( gnome_terminal_id >= 4000 )
      decscusr_support = true;
  }

  return new_termtype;
}

//----------------------------------------------------------------------
inline char* FTerm::secDA_Analysis_24 (char current_termtype[])
{
  // Terminal ID 24 - DEC VT320

  char* new_termtype = current_termtype;

#if defined(__NetBSD__) || defined(__OpenBSD__)

  if ( secondary_da.terminal_id_version == 20 && isWSConsConsole() )
  {
    // NetBSD/OpenBSD workstation console
    if ( std::strncmp(termtype, C_STR("wsvt25"), 6) == 0 )
      netbsd_terminal = true;
    else if ( std::strncmp(termtype, C_STR("vt220"), 5) == 0 )
    {
      openbsd_terminal = true;
      new_termtype = C_STR("pccon");
    }
  }

#endif

  return new_termtype;
}

//----------------------------------------------------------------------
inline char* FTerm::secDA_Analysis_32 (char[])
{
  // Terminal ID 32 - Tera Term

  char* new_termtype;
  tera_terminal = true;
  new_termtype = C_STR("teraterm");
  return new_termtype;
}

//----------------------------------------------------------------------
inline char* FTerm::secDA_Analysis_77 (char[])
{
  // Terminal ID 77 - mintty

  char* new_termtype;
  mintty_terminal = true;
  new_termtype = C_STR("xterm-256color");
  // switch to application escape key mode
  putstring (CSI "?7727h");
  std::fflush(stdout);
  return new_termtype;
}

//----------------------------------------------------------------------
inline char* FTerm::secDA_Analysis_82 (char current_termtype[])
{
  // Terminal ID 82 - rxvt

  char* new_termtype = current_termtype;
  rxvt_terminal = true;
  force_vt100 = true;  // This rxvt terminal does not support utf-8

  if ( std::strncmp(termtype, "rxvt-", 5) != 0
    && std::strncmp(termtype, "rxvt-cygwin-native", 18) == 0 )
    new_termtype = C_STR("rxvt-16color");
  else
    new_termtype = termtype;

  return new_termtype;
}

//----------------------------------------------------------------------
inline char* FTerm::secDA_Analysis_83 (char current_termtype[])
{
  // Terminal ID 83 - screen

  char* new_termtype = current_termtype;
  screen_terminal = true;
  return new_termtype;
}

//----------------------------------------------------------------------
inline char* FTerm::secDA_Analysis_85 (char current_termtype[])
{
  // Terminal ID 85 - rxvt-unicode

  char* new_termtype = current_termtype;
  rxvt_terminal = true;
  urxvt_terminal = true;

  if ( std::strncmp(termtype, "rxvt-", 5) != 0 )
  {
    if ( color256 )
      new_termtype = C_STR("rxvt-256color");
    else
      new_termtype = C_STR("rxvt");
  }
  else
    new_termtype = termtype;

  return new_termtype;
}

//----------------------------------------------------------------------
void FTerm::oscPrefix()
{
  if ( tmux_terminal )
  {
    // tmux device control string
    putstring (ESC "Ptmux;" ESC);
  }
  else if ( screen_terminal )
  {
    // GNU Screen device control string
    putstring (ESC "P");
  }
}

//----------------------------------------------------------------------
void FTerm::oscPostfix()
{
  if ( screen_terminal || tmux_terminal )
  {
    // GNU Screen/tmux string terminator
    putstring (ESC "\\");
  }
}

//----------------------------------------------------------------------
void FTerm::init_alt_charset()
{
  // Read the used vt100 pairs

  if ( TCAP(fc::t_acs_chars) )
  {
    for (int n = 0; TCAP(fc::t_acs_chars)[n]; n += 2)
    {
      // insert the vt100 key/value pairs into a map
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

  // Update array 'character' with discovered vt100 pairs
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
        fc::character[item][fc::VT100] = 0;  // delete vt100 char in character
    }
  }
}

//----------------------------------------------------------------------
void FTerm::init_pc_charset()
{
  bool reinit = false;

  // rxvt does not support pc charset
  if ( rxvt_terminal || urxvt_terminal )
    return;

  if ( gnome_terminal || linux_terminal )
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

  if ( ! cygwin_terminal )
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
void FTerm::init_teraterm_charmap()
{
  // Tera Term can't print ascii characters < 0x20

  if ( ! tera_terminal )
    return;

  for (int i = 0; i <= fc::lastCharItem; i++ )
    if ( fc::character[i][fc::PC] < 0x20 )
      fc::character[i][fc::PC] = fc::character[i][fc::ASCII];
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
void FTerm::init_termcaps()
{
  std::vector<std::string> terminals;
  std::vector<std::string>::iterator iter;
  static const int success = 1;
  static const int uninitialized = -2;
  static char term_buffer[2048];
  static char string_buf[2048];
  char* buffer = string_buf;
  int status = uninitialized;

  // share the terminal capabilities
  FTermcap().setTermcapMap(tcap);

  // open termcap file
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

    if ( status == success || ! terminal_detection )
      break;

    ++iter;
  }

  if ( std::strncmp(termtype, "ansi", 4) == 0 )
    ansi_terminal = true;

  init_termcaps_error (status);
  init_termcaps_variables (buffer);
}

//----------------------------------------------------------------------
void FTerm::init_termcaps_error (int status)
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
void FTerm::init_termcaps_variables (char*& buffer)
{
  // Get termcap booleans
  init_termcaps_booleans();

  // Get termcap numerics
  init_termcaps_numerics();

  // Get termcap strings
  init_termcaps_strings(buffer);

  // Fix terminal quirks
  init_termcaps_quirks();

  // Get termcap keys
  init_termcaps_keys(buffer);

  // Initialize cursor movement optimization
  init_OptiMove();

  // Initialize video attributes optimization
  init_OptiAttr();
}

//----------------------------------------------------------------------
void FTerm::init_termcaps_booleans()
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
void FTerm::init_termcaps_numerics()
{
  // Get termcap numeric

  // Maximum number of colors on screen
  FTermcap::max_color = std::max( FTermcap::max_color
                                , tgetnum(C_STR("Co")) );

  if ( FTermcap::max_color < 0 )
    FTermcap::max_color = 1;

  if ( FTermcap::max_color < 8 )
    monochron = true;
  else
    monochron = false;

  // Get initial spacing for hardware tab stop
  FTermcap::tabstop = tgetnum(C_STR("it"));

  // Get video attributes that cannot be used with colors
  FTermcap::attr_without_color = tgetnum(C_STR("NC"));
}

//----------------------------------------------------------------------
void FTerm::init_termcaps_strings (char*& buffer)
{
  // Get termcap strings

  // Read termcap output strings
  for (int i = 0; tcap[i].tname[0] != 0; i++)
    tcap[i].string = tgetstr(tcap[i].tname, &buffer);
}

//----------------------------------------------------------------------
void FTerm::init_termcaps_quirks()
{
  if ( cygwin_terminal )
  {
    init_termcaps_cygwin_quirks();
  }
  else if ( linux_terminal )
  {
    init_termcaps_linux_quirks();
  }
  else if ( rxvt_terminal )
  {
    init_termcaps_rxvt_quirks();
  }
  else if ( gnome_terminal )
  {
    init_termcaps_vte_quirks();
  }
  else if ( tera_terminal )
  {
    init_termcaps_teraterm_quirks();
  }
  else if ( sun_terminal )
  {
    init_termcaps_sun_quirks();
  }
  else if ( putty_terminal )
  {
    init_termcaps_putty_quirks();
  }
  else if ( screen_terminal )
  {
    init_termcaps_screen_quirks();
  }
#if defined(__FreeBSD__) || defined(__DragonFly__)
  else if ( isFreeBSDConsole() )
  {
    init_termcaps_freebsd_quirks()
  }
#endif

  // xterm and compatible terminals
  if ( xterm_terminal && ! putty_terminal )
    init_termcaps_xterm_quirks();

  // Fixes general quirks
  init_termcaps_general_quirks();
}

#if defined(__FreeBSD__) || defined(__DragonFly__)
//----------------------------------------------------------------------
static void FTerm::init_termcaps_freebsd_quirks()
{
  // FreeBSD console fixes

  TCAP(fc::t_acs_chars) = \
      C_STR("-\036.\0370\333"
            "a\260f\370g\361"
            "h\261j\331k\277"
            "l\332m\300n\305"
            "q\304t\303u\264"
            "v\301w\302x\263"
            "y\363z\362~\371");

    TCAP(fc::t_set_attributes) = \
        C_STR(CSI "0"
                  "%?%p1%p6%|%t;1%;"
                  "%?%p2%t;4%;"
                  "%?%p1%p3%|%t;7%;"
                  "%?%p4%t;5%;m"
                  "%?%p9%t\016%e\017%;");

  FTermcap::attr_without_color = 18;
}
#endif

//----------------------------------------------------------------------
void FTerm::init_termcaps_cygwin_quirks()
{
  // Set invisible cursor for cygwin terminal
  if ( ! TCAP(fc::t_cursor_invisible) )
    TCAP(fc::t_cursor_invisible) = \
        C_STR(CSI "?25l");

  // Set visible cursor for cygwin terminal
  if ( ! TCAP(fc::t_cursor_visible) )
    TCAP(fc::t_cursor_visible) = \
        C_STR(CSI "?25h");

  // Set ansi blink for cygwin terminal
  if ( ! TCAP(fc::t_enter_blink_mode) )
    TCAP(fc::t_enter_blink_mode) = \
        C_STR(CSI "5m");

  // Set enable alternate character set for cygwin terminal
  if ( ! TCAP(fc::t_enable_acs) )
    TCAP(fc::t_enable_acs) = \
        C_STR(ESC "(B" ESC ")0");

  // Set background color erase for cygwin terminal
  FTermcap::background_color_erase = true;

  // Include the Linux console quirks
  init_termcaps_linux_quirks();
}

//----------------------------------------------------------------------
void FTerm::init_termcaps_linux_quirks()
{
  /* Same settings are used by cygwin */

  // Set ansi foreground and background color
  if ( FTermcap::max_color > 8 )
  {
    TCAP(fc::t_set_a_foreground) = \
        C_STR(CSI "3%p1%{8}%m%d%?%p1%{7}%>%t;1%e;22%;m");
    TCAP(fc::t_set_a_background) = \
        C_STR(CSI "4%p1%{8}%m%d%?%p1%{7}%>%t;5%e;25%;m");
    // Avoid underline, blink and dim mode
    FTermcap::attr_without_color = 26;
  }
  else
  {
    TCAP(fc::t_set_a_foreground) = \
        C_STR(CSI "3%p1%dm");
    TCAP(fc::t_set_a_background) = \
        C_STR(CSI "4%p1%dm");
    // Avoid underline and dim mode
    FTermcap::attr_without_color = 18;
  }

  // Set select graphic rendition attributes
  TCAP(fc::t_set_attributes) = \
      C_STR(CSI "0"
                "%?%p6%|%t;1%;"
                "%?%p1%p3%|%t;7%;"
                "%?%p4%t;5%;m"
                "%?%p9%t\016%e\017%;");

  TCAP(fc::t_enter_alt_charset_mode) = C_STR("\016");
  TCAP(fc::t_exit_alt_charset_mode) = C_STR("\017");
  TCAP(fc::t_exit_attribute_mode) = C_STR(CSI "0m\017");
  TCAP(fc::t_exit_bold_mode) = C_STR(CSI "22m");
  TCAP(fc::t_exit_blink_mode) = C_STR(CSI "25m");
  TCAP(fc::t_exit_reverse_mode) = C_STR(CSI "27m");
  TCAP(fc::t_exit_secure_mode) = 0;
  TCAP(fc::t_exit_protected_mode) = 0;
  TCAP(fc::t_exit_crossed_out_mode) = 0;
  TCAP(fc::t_orig_pair) = C_STR(CSI "39;49;25m");

  // Avoid underline and dim mode
  TCAP(fc::t_enter_dim_mode)       = 0;
  TCAP(fc::t_exit_dim_mode)        = 0;
  TCAP(fc::t_enter_underline_mode) = 0;
  TCAP(fc::t_exit_underline_mode)  = 0;
}

//----------------------------------------------------------------------
void FTerm::init_termcaps_xterm_quirks()
{
  // Fallback if "Ic" is not found
  if ( ! TCAP(fc::t_initialize_color) )
  {
    TCAP(fc::t_initialize_color) = \
        C_STR(OSC "4;%p1%d;rgb:"
                  "%p2%{255}%*%{1000}%/%2.2X/"
                  "%p3%{255}%*%{1000}%/%2.2X/"
                  "%p4%{255}%*%{1000}%/%2.2X" ESC "\\");
  }

  // Fallback if "vi" is not found
  if ( ! TCAP(fc::t_cursor_invisible) )
    TCAP(fc::t_cursor_invisible) = \
        C_STR(CSI "?25l");

  // Fallback if "ve" is not found
  if ( ! TCAP(fc::t_cursor_normal) )
    TCAP(fc::t_cursor_normal) = \
        C_STR(CSI "?12l" CSI "?25h");
}

//----------------------------------------------------------------------
void FTerm::init_termcaps_rxvt_quirks()
{
  // Set enter/exit alternative charset mode for rxvt terminal
  if ( std::strncmp(termtype, "rxvt-16color", 12) == 0 )
  {
    TCAP(fc::t_enter_alt_charset_mode) = \
        C_STR(ESC "(0");
    TCAP(fc::t_exit_alt_charset_mode)  = \
        C_STR(ESC "(B");
  }

  // Set ansi foreground and background color
  if ( ! urxvt_terminal )
  {
    TCAP(fc::t_set_a_foreground) = \
        C_STR(CSI "%?%p1%{8}%<%t%p1%{30}%+%e%p1%'R'%+%;%dm");
    TCAP(fc::t_set_a_background) = \
        C_STR(CSI "%?%p1%{8}%<%t%p1%'('%+%e%p1%{92}%+%;%dm");
  }
}

//----------------------------------------------------------------------
void FTerm::init_termcaps_vte_quirks()
{
  // gnome-terminal has NC=16 however, it can use the dim attribute
  FTermcap::attr_without_color = 0;

  // set exit underline for gnome terminal
  TCAP(fc::t_exit_underline_mode) = \
      C_STR(CSI "24m");
}

//----------------------------------------------------------------------
void FTerm::init_termcaps_putty_quirks()
{
  FTermcap::background_color_erase = true;
  FTermcap::osc_support = true;

  // PuTTY has NC=22 however, it can show underline and reverse
  // and since version 0.71 is the dim attribute is also supported
  FTermcap::attr_without_color = 0;

  // Set ansi foreground and background color
  TCAP(fc::t_set_a_foreground) = \
      C_STR(CSI "%?%p1%{8}%<"
                "%t3%p1%d"
                "%e%p1%{16}%<"
                "%t9%p1%{8}%-%d"
                "%e38;5;%p1%d%;m");

  TCAP(fc::t_set_a_background) = \
      C_STR(CSI "%?%p1%{8}%<"
                "%t4%p1%d"
                "%e%p1%{16}%<"
                "%t10%p1%{8}%-%d"
                "%e48;5;%p1%d%;m");

  TCAP(fc::t_set_attributes) = \
      C_STR(CSI "0"
                "%?%p1%p6%|%t;1%;"
                "%?%p5%t;2%;"  // since putty 0.71
                "%?%p2%t;4%;"
                "%?%p1%p3%|%t;7%;"
                "%?%p4%t;5%;m"
                "%?%p9%t\016%e\017%;");
  // PuTTY 0.71 or higher
  TCAP(fc::t_enter_dim_mode) = \
      C_STR(CSI "2m");

  // PuTTY 0.71 or higher
  TCAP(fc::t_exit_dim_mode) = \
      C_STR(CSI "22m");

  if ( ! TCAP(fc::t_clr_bol) )
    TCAP(fc::t_clr_bol) = \
        C_STR(CSI "1K");

  if ( ! TCAP(fc::t_orig_pair) )
    TCAP(fc::t_orig_pair) = \
        C_STR(CSI "39;49m");

  if ( ! TCAP(fc::t_orig_colors) )
    TCAP(fc::t_orig_colors) = \
        C_STR(OSC "R");

  if ( ! TCAP(fc::t_column_address) )
    TCAP(fc::t_column_address) = \
        C_STR(CSI "%i%p1%dG");

  if ( ! TCAP(fc::t_row_address) )
    TCAP(fc::t_row_address) = \
        C_STR(CSI "%i%p1%dd");

  if ( ! TCAP(fc::t_enable_acs) )
    TCAP(fc::t_enable_acs) = \
        C_STR(ESC "(B" ESC ")0");

  if ( ! TCAP(fc::t_enter_am_mode) )
    TCAP(fc::t_enter_am_mode) = \
        C_STR(CSI "?7h");

  if ( ! TCAP(fc::t_exit_am_mode) )
    TCAP(fc::t_exit_am_mode) = \
        C_STR(CSI "?7l");

  if ( ! TCAP(fc::t_enter_pc_charset_mode) )
    TCAP(fc::t_enter_pc_charset_mode) = \
        C_STR(CSI "11m");

  if ( ! TCAP(fc::t_exit_pc_charset_mode) )
    TCAP(fc::t_exit_pc_charset_mode) = \
        C_STR(CSI "10m");

  if ( ! TCAP(fc::t_key_mouse) )
    TCAP(fc::t_key_mouse) = \
        C_STR(CSI "M");
}

//----------------------------------------------------------------------
void FTerm::init_termcaps_teraterm_quirks()
{
  // Tera Term eat_nl_glitch fix
  FTermcap::eat_nl_glitch = true;

  // Tera Term color settings
  TCAP(fc::t_set_a_foreground) = \
      C_STR(CSI "38;5;%p1%dm");
  TCAP(fc::t_set_a_background) = \
      C_STR(CSI "48;5;%p1%dm");
  TCAP(fc::t_exit_attribute_mode) = \
      C_STR(CSI "0m" SI);
  TCAP(fc::t_orig_pair) = \
      C_STR(CSI "39;49m");
}

//----------------------------------------------------------------------
void FTerm::init_termcaps_sun_quirks()
{
  // Sun Microsystems workstation console eat_nl_glitch fix
  FTermcap::eat_nl_glitch = true;
}

//----------------------------------------------------------------------
void FTerm::init_termcaps_screen_quirks()
{
  // Fallback if "Ic" is not found
  if ( ! TCAP(fc::t_initialize_color) )
  {
    if ( tmux_terminal )
    {
      TCAP(fc::t_initialize_color) = \
          C_STR(ESC "Ptmux;" ESC OSC "4;%p1%d;rgb:"
                "%p2%{255}%*%{1000}%/%2.2X/"
                "%p3%{255}%*%{1000}%/%2.2X/"
                "%p4%{255}%*%{1000}%/%2.2X" BEL ESC "\\");
    }
    else
    {
      TCAP(fc::t_initialize_color) = \
          C_STR(ESC "P" OSC "4;%p1%d;rgb:"
                "%p2%{255}%*%{1000}%/%2.2X/"
                "%p3%{255}%*%{1000}%/%2.2X/"
                "%p4%{255}%*%{1000}%/%2.2X" BEL ESC "\\");
    }
  }
}

//----------------------------------------------------------------------
void FTerm::init_termcaps_general_quirks()
{
  static const int not_available = -1;

  if ( FTermcap::tabstop == not_available )
    FTermcap::tabstop = 8;

  if ( FTermcap::attr_without_color == not_available )
    FTermcap::attr_without_color = 0;

  // Fallback if "AF" is not found
  if ( ! TCAP(fc::t_set_a_foreground) )
    TCAP(fc::t_set_a_foreground) = \
        C_STR(CSI "3%p1%dm");

  // Fallback if "AB" is not found
  if ( ! TCAP(fc::t_set_a_background) )
    TCAP(fc::t_set_a_background) = \
        C_STR(CSI "4%p1%dm");

  // Fallback if "Ic" is not found
  if ( ! TCAP(fc::t_initialize_color) )
  {
    TCAP(fc::t_initialize_color) = \
        C_STR(OSC "P%p1%x"
                  "%p2%{255}%*%{1000}%/%02x"
                  "%p3%{255}%*%{1000}%/%02x"
                  "%p4%{255}%*%{1000}%/%02x");
  }

  // Fallback if "ti" is not found
  if ( ! TCAP(fc::t_enter_ca_mode) )
    TCAP(fc::t_enter_ca_mode) = \
        C_STR(ESC "7" CSI "?47h");

  // Fallback if "te" is not found
  if ( ! TCAP(fc::t_exit_ca_mode) )
    TCAP(fc::t_exit_ca_mode) = \
        C_STR(CSI "?47l" ESC "8" CSI "m");

  // Set ansi move if "cm" is not found
  if ( ! TCAP(fc::t_cursor_address) )
    TCAP(fc::t_cursor_address) = \
        C_STR(CSI "%i%p1%d;%p2%dH");

  // Test for standard ECMA-48 (ANSI X3.64) terminal
  if ( TCAP(fc::t_exit_underline_mode)
    && std::strncmp(TCAP(fc::t_exit_underline_mode), CSI "24m", 5) == 0 )
  {
    // Seems to be a ECMA-48 (ANSI X3.64) compatible terminal
    TCAP(fc::t_enter_dbl_underline_mode) = \
        C_STR(CSI "21m");  // Exit single underline, too

    TCAP(fc::t_exit_dbl_underline_mode) = \
        C_STR(CSI "24m");

    TCAP(fc::t_exit_bold_mode) = \
        C_STR(CSI "22m");  // Exit dim, too

    TCAP(fc::t_exit_dim_mode) = \
        C_STR(CSI "22m");

    TCAP(fc::t_exit_underline_mode) = \
        C_STR(CSI "24m");

    TCAP(fc::t_exit_blink_mode) = \
        C_STR(CSI "25m");

    TCAP(fc::t_exit_reverse_mode) = \
        C_STR(CSI "27m");

    TCAP(fc::t_exit_secure_mode) = \
        C_STR(CSI "28m");

    TCAP(fc::t_enter_crossed_out_mode) = \
        C_STR(CSI "9m");

    TCAP(fc::t_exit_crossed_out_mode) = \
        C_STR(CSI "29m");
  }
}

//----------------------------------------------------------------------
void FTerm::init_termcaps_keys (char*& buffer)
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

  opti_move->setTabStop(int(FTermcap::tabstop));
  opti_move->set_cursor_home (TCAP(fc::t_cursor_home));
  opti_move->set_cursor_to_ll (TCAP(fc::t_cursor_to_ll));
  opti_move->set_carriage_return (TCAP(fc::t_carriage_return));
  opti_move->set_tabular (TCAP(fc::t_tab));
  opti_move->set_back_tab (TCAP(fc::t_back_tab));
  opti_move->set_cursor_up (TCAP(fc::t_cursor_up));
  opti_move->set_cursor_down (TCAP(fc::t_cursor_down));
  opti_move->set_cursor_left (TCAP(fc::t_cursor_left));
  opti_move->set_cursor_right (TCAP(fc::t_cursor_right));
  cursor_addres_lengths = \
      opti_move->set_cursor_address (TCAP(fc::t_cursor_address));
  opti_move->set_column_address (TCAP(fc::t_column_address));
  opti_move->set_row_address (TCAP(fc::t_row_address));
  opti_move->set_parm_up_cursor (TCAP(fc::t_parm_up_cursor));
  opti_move->set_parm_down_cursor (TCAP(fc::t_parm_down_cursor));
  opti_move->set_parm_left_cursor (TCAP(fc::t_parm_left_cursor));
  opti_move->set_parm_right_cursor (TCAP(fc::t_parm_right_cursor));
  opti_move->set_auto_left_margin (FTermcap::automatic_left_margin);
  opti_move->set_eat_newline_glitch (FTermcap::eat_nl_glitch);
  erase_ch_length = \
      opti_move->set_erase_chars (TCAP(fc::t_erase_chars));
  repeat_char_length = \
      opti_move->set_repeat_char (TCAP(fc::t_repeat_char));
  clr_bol_length = opti_move->set_clr_bol (TCAP(fc::t_clr_bol));
  clr_eol_length = opti_move->set_clr_eol (TCAP(fc::t_clr_eol));
}

//----------------------------------------------------------------------
void FTerm::init_OptiAttr()
{
  // Setting video attribute optimization

  opti_attr->setNoColorVideo (int(FTermcap::attr_without_color));
  opti_attr->set_enter_bold_mode (TCAP(fc::t_enter_bold_mode));
  opti_attr->set_exit_bold_mode (TCAP(fc::t_exit_bold_mode));
  opti_attr->set_enter_dim_mode (TCAP(fc::t_enter_dim_mode));
  opti_attr->set_exit_dim_mode (TCAP(fc::t_exit_dim_mode));
  opti_attr->set_enter_italics_mode (TCAP(fc::t_enter_italics_mode));
  opti_attr->set_exit_italics_mode (TCAP(fc::t_exit_italics_mode));
  opti_attr->set_enter_underline_mode (TCAP(fc::t_enter_underline_mode));
  opti_attr->set_exit_underline_mode (TCAP(fc::t_exit_underline_mode));
  opti_attr->set_enter_blink_mode (TCAP(fc::t_enter_blink_mode));
  opti_attr->set_exit_blink_mode (TCAP(fc::t_exit_blink_mode));
  opti_attr->set_enter_reverse_mode (TCAP(fc::t_enter_reverse_mode));
  opti_attr->set_exit_reverse_mode (TCAP(fc::t_exit_reverse_mode));
  opti_attr->set_enter_standout_mode (TCAP(fc::t_enter_standout_mode));
  opti_attr->set_exit_standout_mode (TCAP(fc::t_exit_standout_mode));
  opti_attr->set_enter_secure_mode (TCAP(fc::t_enter_secure_mode));
  opti_attr->set_exit_secure_mode (TCAP(fc::t_exit_secure_mode));
  opti_attr->set_enter_protected_mode (TCAP(fc::t_enter_protected_mode));
  opti_attr->set_exit_protected_mode (TCAP(fc::t_exit_protected_mode));
  opti_attr->set_enter_crossed_out_mode (TCAP(fc::t_enter_crossed_out_mode));
  opti_attr->set_exit_crossed_out_mode (TCAP(fc::t_exit_crossed_out_mode));
  opti_attr->set_enter_dbl_underline_mode (TCAP(fc::t_enter_dbl_underline_mode));
  opti_attr->set_exit_dbl_underline_mode (TCAP(fc::t_exit_dbl_underline_mode));
  opti_attr->set_set_attributes (TCAP(fc::t_set_attributes));
  opti_attr->set_exit_attribute_mode (TCAP(fc::t_exit_attribute_mode));
  opti_attr->set_enter_alt_charset_mode (TCAP(fc::t_enter_alt_charset_mode));
  opti_attr->set_exit_alt_charset_mode (TCAP(fc::t_exit_alt_charset_mode));
  opti_attr->set_enter_pc_charset_mode (TCAP(fc::t_enter_pc_charset_mode));
  opti_attr->set_exit_pc_charset_mode (TCAP(fc::t_exit_pc_charset_mode));
  opti_attr->set_a_foreground_color (TCAP(fc::t_set_a_foreground));
  opti_attr->set_a_background_color (TCAP(fc::t_set_a_background));
  opti_attr->set_foreground_color (TCAP(fc::t_set_foreground));
  opti_attr->set_background_color (TCAP(fc::t_set_background));
  opti_attr->set_term_color_pair (TCAP(fc::t_set_color_pair));
  opti_attr->set_orig_pair (TCAP(fc::t_orig_pair));
  opti_attr->set_orig_orig_colors (TCAP(fc::t_orig_colors));
  opti_attr->setMaxColor (FTermcap::max_color);

  if ( FTermcap::ansi_default_color )
    opti_attr->setDefaultColorSupport();

  opti_attr->initialize();
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
  if ( tera_terminal && ! std::strcmp(nl_langinfo(CODESET), "UTF-8") )
    locale_name = std::setlocale (LC_ALL, "C");

  // Kterm
  if ( kterm_terminal && ! std::strcmp(nl_langinfo(CODESET), "UTF-8") )
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
  init_term_encoding();
  init_pc_charset();
  init_individual_term_encoding();

  if ( ! init_force_vt100_encoding() )
    init_utf8_without_alt_charset();

  init_tab_quirks();
}

//----------------------------------------------------------------------
inline void FTerm::init_encoding_set()
{
  // Define the encoding set

  (*encoding_set)["UTF8"]  = fc::UTF8;
  (*encoding_set)["UTF-8"] = fc::UTF8;
  (*encoding_set)["VT100"] = fc::VT100;
  (*encoding_set)["PC"]    = fc::PC;
  (*encoding_set)["ASCII"] = fc::ASCII;
}

//----------------------------------------------------------------------
void FTerm::init_term_encoding()
{
  if ( isatty(stdout_no)
    && ! std::strcmp(nl_langinfo(CODESET), "UTF-8") )
  {
    utf8_console  = true;
    term_encoding = fc::UTF8;
    Fputchar      = &FTerm::putchar_UTF8;  // function pointer
    utf8_state    = true;
    utf8_input    = true;
    setUTF8(true);
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
  if ( linux_terminal
    || cygwin_terminal
    || NewFont
    || (putty_terminal && ! utf8_state)
    || (tera_terminal && ! utf8_state) )
  {
    pc_charset_console = true;
    term_encoding      = fc::PC;
    Fputchar           = &FTerm::putchar_ASCII;  // function pointer

    if ( linux_terminal && utf8_console )
    {
      utf8_linux_terminal = true;
      setUTF8(false);
    }
    else if ( xterm_terminal && utf8_console )
    {
      Fputchar = &FTerm::putchar_UTF8;  // function pointer
    }
  }
}

//----------------------------------------------------------------------
bool FTerm::init_force_vt100_encoding()
{
  if ( force_vt100 )
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
void FTerm::redefineColorPalette()
{
  // Redefine the color palette

  if ( cygwin_terminal
    || kde_konsole
    || tera_terminal
    || sun_terminal
    || ansi_terminal )
    return;

  resetColorMap();
  saveColorMap();

  if ( FTermcap::max_color >= 16 )
    FColorPalette::set16ColorPalette (FTerm::setPalette);
  else  // 8 colors
    FColorPalette::set8ColorPalette (FTerm::setPalette);
}

//----------------------------------------------------------------------
void FTerm::restoreColorPalette()
{
  if ( kde_konsole
    || tera_terminal
    || sun_terminal
    || ansi_terminal )
    return;

  // Reset screen settings
  if ( FTermcap::max_color >= 16 )
    FColorPalette::reset16ColorPalette (FTerm::setPalette);
  else  // 8 colors
    FColorPalette::reset8ColorPalette (FTerm::setPalette);

  resetXTermColors();
  resetColorMap();
}

//----------------------------------------------------------------------
void FTerm::enableMouse()
{
  bool gpm_mouse = false;
  bool xterm_mouse = false;

#if defined(__linux__)
  if ( linux_terminal && openConsole() == 0 )
  {
    if ( isLinuxConsole() )
      gpm_mouse = true;

    closeConsole();
  }
#endif

  if ( TCAP(fc::t_key_mouse) && ! linux_terminal )
    xterm_mouse = true;

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
void FTerm::captureXTermFontAndTitle()
{
  if ( (xterm_terminal || urxvt_terminal) && ! rxvt_terminal )
  {
    struct termios t;
    tcgetattr (stdin_no, &t);
    t.c_lflag &= uInt(~(ICANON | ECHO));
    tcsetattr (stdin_no, TCSANOW, &t);

    xterm_font  = getXTermFont();
    xterm_title = getXTermTitle();

    t.c_lflag |= uInt(ICANON | ECHO);
    tcsetattr (stdin_no, TCSADRAIN, &t);
  }
}

//----------------------------------------------------------------------
inline void FTerm::allocationValues()
{
  try
  {
    opti_move      = new FOptiMove();
    opti_attr      = new FOptiAttr();
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

  if ( sec_da )
    delete sec_da;

  if ( answer_back )
    delete answer_back;

  if ( xterm_title )
    delete xterm_title;

  if ( xterm_font )
    delete xterm_font;

  if ( term )
    delete term;

  if ( mouse )
    delete mouse;

  if ( opti_attr )
    delete opti_attr;

  if ( opti_move )
    delete opti_move;
}

//----------------------------------------------------------------------
void FTerm::init()
{
  init_term_object = this;

  // Initialize global values for all objects
  allocationValues();
  init_global_values();

  // Get file descriptor for standard input and standard output
  stdin_no  = fileno(stdin);
  stdout_no = fileno(stdout);

  // Get the stdin file status flags
  stdin_status_flags = fcntl(stdin_no, F_GETFL);

  if ( stdin_status_flags == -1 )
    std::abort();

  // Get pathname of the terminal device
  if ( ttyname_r(stdout_no, termfilename, sizeof(termfilename)) )
    termfilename[0] = '\0';

  initOSspecifics();

  // Save termios settings
  storeTTYsettings();

  // Get output baud rate
  baudrate = getBaudRate(&term_init);

  if ( isatty(stdout_no) )
    opti_move->setBaudRate(int(baudrate));

  // Set the variable 'termtype' to the predefined type of the terminal
  getSystemTermType();

  // Analysis the termtype
  termtypeAnalysis();

  // Terminal detection
  detectTerminal();

  // Initializes variables for the current terminal
  init_termcaps();
  init_alt_charset();

  // Initializes locale information
  init_locale();

  // Detect environment and set encoding
  init_encoding();

  if ( init_values.encoding != fc::UNKNOWN )
  {
    setEncoding(init_values.encoding);
  }

  // Enable the terminal mouse support
  enableMouse();

  // Activate meta key sends escape
  if ( xterm_terminal )
    xtermMetaSendsESC(true);

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
  captureXTermFontAndTitle();

  if ( kde_konsole )
    setKDECursor(fc::UnderlineCursor);

  if ( cygwin_terminal )
    init_cygwin_charmap();

  if ( tera_terminal )
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
  noHardwareEcho();

  // Switch to the raw mode
  setRawMode();

  // The terminal is now initialized
  term_initialized = true;
}

//----------------------------------------------------------------------
void FTerm::initOSspecifics()
{
#if defined(__linux__)
  // initialize Linux console
  initLinuxConsole();
#endif

#if defined(__FreeBSD__) || defined(__DragonFly__)
  // Initialize BSD console
  initFreeBSDConsole();
#endif

#if defined(__NetBSD__) || defined(__OpenBSD__)
  // Initialize wscons console
  initWSConsConsole();
#endif
}

//----------------------------------------------------------------------
void FTerm::finish()
{
  // Set default signal handler
  resetSignalHandler();

  if ( xterm_title && xterm_terminal && ! rxvt_terminal )
    setXTermTitle (*xterm_title);

  // Restore the saved termios settings
  restoreTTYsettings();

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
  resetXTermDefaults();

  // Set xterm full block cursor
  setXTermCursorStyle(fc::steady_block);

  // Restore the color palette
  if ( init_values.color_change )
    restoreColorPalette();

  if ( mintty_terminal )
  {
    // Switch to normal escape key mode
    putstring (CSI "?7727l");
    std::fflush(stdout);
  }

  finishOSspecifics1();

  if ( kde_konsole )
    setKDECursor(fc::BlockCursor);

  resetBeep();

  // Disable the terminal mouse support
  disableMouse();

  // Deactivate meta key sends escape
  if ( xterm_terminal )
    xtermMetaSendsESC(false);

  useNormalScreenBuffer();

  // leave 'keyboard_transmit' mode
  if ( TCAP(fc::t_keypad_local) )
  {
    putstring (TCAP(fc::t_keypad_local));
    std::fflush(stdout);
  }

  finishOSspecifics2();
  deallocationValues();
}

//----------------------------------------------------------------------
void FTerm::finishOSspecifics1()
{
#if defined(__linux__)
  if ( linux_terminal )
  {
#if defined(__x86_64__) || defined(__i386) || defined(__arm__)
    setBlinkAsIntensity (false);
#endif
    setLinuxConsoleCursorStyle (fc::default_cursor, false);
  }
#endif

#if defined(__FreeBSD__) || defined(__DragonFly__)
  resetFreeBSDAlt2Meta();
  setFreeBSDConsoleCursorStyle (fc::normal_cursor, false);
#endif

#if defined(__NetBSD__) || defined(__OpenBSD__)
  resetWSConsEncoding();
#endif
}

//----------------------------------------------------------------------
void FTerm::finishOSspecifics2()
{
#if defined(__linux__)
  if ( linux_terminal && utf8_console )
    setUTF8(true);

  if ( NewFont || VGAFont )
    setOldFont();
  else
  {
    if ( screen_font.data != 0 )
      delete[] screen_font.data;

    if ( screen_unicode_map.entries != 0 )
      delete[] screen_unicode_map.entries;
  }
#endif
}

//----------------------------------------------------------------------
uInt FTerm::cp437_to_unicode (uChar c)
{
  register uInt ucs = uInt(c);

  for (register uInt i = 0; i <= fc::lastCP437Item; i++)
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
inline int FTerm::getMouseProtocolKey (char buffer[])
{
  // Looking for mouse string in the key buffer
  register std::size_t buf_len = std::strlen(buffer);

  // x11 mouse tracking
  if ( buf_len >= 6 && buffer[1] == '[' && buffer[2] == 'M' )
    return fc::Fkey_mouse;

  // SGR mouse tracking
  if ( buffer[1] == '[' && buffer[2] == '<' && buf_len >= 9
    && (buffer[buf_len - 1] == 'M' || buffer[buf_len - 1] == 'm') )
    return fc::Fkey_extended_mouse;

  // urxvt mouse tracking
  if ( buffer[1] == '[' && buffer[2] >= '1' && buffer[2] <= '9'
    && buffer[3] >= '0' && buffer[3] <= '9' && buf_len >= 9
    && buffer[buf_len - 1] == 'M' )
    return fc::Fkey_urxvt_mouse;

  return -1;
}

//----------------------------------------------------------------------
inline int FTerm::getTermcapKey (char buffer[], int buf_size)
{
  // Looking for termcap key strings in the buffer
  assert ( buf_size > 0 );

  for (int i = 0; fc::Fkey[i].tname[0] != 0; i++)
  {
    char* k = fc::Fkey[i].string;
    register int len = ( k ) ? int(std::strlen(k)) : 0;

    if ( k && std::strncmp(k, buffer, uInt(len)) == 0 )  // found
    {
      register int n;

      for (n = len; n < buf_size; n++)  // Remove founded entry
        buffer[n - len] = buffer[n];

      for (; n - len < len; n++)  // Fill rest with '\0'
        buffer[n - len] = '\0';

      input_data_pending = bool(buffer[0] != '\0');
      return fc::Fkey[i].num;
    }
  }

  return -1;
}

//----------------------------------------------------------------------
inline int FTerm::getMetaKey ( char buffer[]
                             , int buf_size
                             , timeval* time_keypressed )
{
  // Looking for meta key strings in the buffer
  assert ( buf_size > 0 );

  for (int i = 0; fc::Fmetakey[i].string[0] != 0; i++)
  {
    char* kmeta = fc::Fmetakey[i].string;  // The string is never null
    register int len = int(std::strlen(kmeta));

    if ( std::strncmp(kmeta, buffer, uInt(len)) == 0 )  // found
    {
      register int n;

      if ( len == 2 && ( buffer[1] == 'O'
                      || buffer[1] == '['
                      || buffer[1] == ']' ) )
      {
        if ( ! isKeypressTimeout(time_keypressed) )
          return NEED_MORE_DATA;
      }

      for (n = len; n < buf_size; n++)  // Remove founded entry
        buffer[n - len] = buffer[n];

      for (; n - len < len; n++)  // Fill rest with '\0'
        buffer[n - len] = '\0';

      input_data_pending = bool(buffer[0] != '\0');
      return fc::Fmetakey[i].num;
    }
  }

  return -1;
}

//----------------------------------------------------------------------
inline int FTerm::getSingleKey (char buffer[], int buf_size)
{
  register uChar firstchar = uChar(buffer[0]);
  int key, n, len;
  len = 1;

  // Look for a utf-8 character
  if ( utf8_input && (firstchar & 0xc0) == 0xc0 )
  {
    char utf8char[4] = {};  // Init array with '\0'

    if ( (firstchar & 0xe0) == 0xc0 )
      len = 2;
    else if ( (firstchar & 0xf0) == 0xe0 )
      len = 3;
    else if ( (firstchar & 0xf8) == 0xf0 )
      len = 4;

    for (int i = 0; i < len ; i++)
      utf8char[i] = char(buffer[i] & 0xff);

    key = UTF8decode(utf8char);
  }
  else
    key = uChar(buffer[0] & 0xff);

  for (n = len; n < buf_size; n++)  // Remove the key from the buffer front
    buffer[n - len] = buffer[n];

  for (n = n - len; n < buf_size; n++)   // Fill the rest with '\0' bytes
    buffer[n] = '\0';

  input_data_pending = bool(buffer[0] != '\0');

  if ( key == 0 )  // Ctrl+Space or Ctrl+@
    key = fc::Fckey_space;

  return int(key == 127 ? fc::Fkey_backspace : key);
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
