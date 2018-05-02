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
int      FTerm::fd_tty;
int      FTerm::stdin_status_flags;
int      FTerm::erase_ch_length;
int      FTerm::repeat_char_length;
int      FTerm::clr_bol_length;
int      FTerm::clr_eol_length;
int      FTerm::cursor_addres_lengths;
uInt     FTerm::baudrate;
long     FTerm::key_timeout;
bool     FTerm::resize_term;

bool     FTerm::input_data_pending;
bool     FTerm::non_blocking_stdin;
bool     FTerm::color256;
bool     FTerm::monochron;
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
char     FTerm::termtype[256] = {};
char     FTerm::termfilename[256] = {};
#if DEBUG
int      FTerm::framebuffer_bpp = -1;
#endif
char*    FTerm::locale_name  = 0;
char*    FTerm::locale_xterm = 0;
FRect*   FTerm::term         = 0;

char                   FTerm::exit_message[8192]        = "";
fc::encoding           FTerm::term_encoding;
const FString*         FTerm::xterm_font                = 0;
const FString*         FTerm::xterm_title               = 0;
FOptiMove*             FTerm::opti_move                 = 0;
FOptiAttr*             FTerm::opti_attr                 = 0;
FTermDetection*        FTerm::term_detection            = 0;
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
FTerm::colorEnv               FTerm::color_env;
FTerm::secondaryDA            FTerm::secondary_da;
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

//----------------------------------------------------------------------
void FTerm::setTermType (char term_name[])
{
  if ( ! term_name )
    return;

  std::strncpy ( termtype
               , term_name
               , std::strlen(term_name) + 1 );
}

#if defined(__linux__)
//----------------------------------------------------------------------
void FTerm::setLinuxConsoleCursorStyle ( fc::linuxConsoleCursorStyle style
                                       , bool hidden )
{
  // Set cursor style in linux console

  if ( ! isLinuxTerm() )
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
bool FTerm::setUTF8 (bool on)  // UTF-8 (Unicode)
{
  if ( on == utf8_state )
    return utf8_state;

  if ( on )
    utf8_state = true;
  else
    utf8_state = false;

  if ( isLinuxTerm() )
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

  if ( isGnomeTerminal()
    || isKdeTerminal()
    || isPuttyTerminal()
    || isTeraTerm()
    || isCygwinTerminal()
    || isMinttyTerm() )
    return false;

  VGAFont = true;

  if ( isXTerminal() || isScreenTerm() || FTermcap::osc_support )
  {
    // Set font in xterm to vga
    oscPrefix();
    putstring (OSC "50;vga" BEL);
    oscPostfix();
    std::fflush(stdout);
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
    oscPrefix();
    putstring (OSC "50;8x16graph" BEL);
    oscPostfix();
    std::fflush(stdout);
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

  if ( isXTerminal() || isScreenTerm()
    || isUrxvtTerminal() || FTermcap::osc_support )
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
  else if ( isLinuxTerm() )
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

  if ( isXTerminal() )
  {
    putstringf (CSI "8;%d;%dt", term_height, term_width);
    std::fflush(stdout);
  }
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
const FString* FTerm::getXTermFont()
{
  if ( isXTerminal() || isScreenTerm() || FTermcap::osc_support )
  {
    fd_set ifds;
    struct timeval tv;
    int stdin_no = FTermios::getStdIn();

    oscPrefix();
    putstring (OSC "50;?" BEL);  // get font
    oscPostfix();
    std::fflush(stdout);

    FD_ZERO(&ifds);
    FD_SET(stdin_no, &ifds);
    tv.tv_sec  = 0;
    tv.tv_usec = 150000;  // 150 ms

    // Read the terminal answer
    if ( select(stdin_no + 1, &ifds, 0, 0, &tv) > 0 )
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
  if ( isKdeTerminal() )
    return 0;

  fd_set ifds;
  struct timeval tv;
  int stdin_no = FTermios::getStdIn();

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
void FTerm::setXTermCursorStyle (fc::xtermCursorStyle style)
{
  // Set the xterm cursor style

  if ( (isGnomeTerminal() && ! term_detection->hasSetCursorStyleSupport())
    || isKdeTerminal() )
    return;

#if defined(__FreeBSD__) || defined(__DragonFly__)
  if ( isFreeBSDConsole() )
    return;
#endif

  if ( isXTerminal() || isMinttyTerm()
    || term_detection->hasSetCursorStyleSupport() )
  {
    putstringf (CSI "%d q", style);
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTerm::setXTermTitle (const FString& title)
{
  // Set the xterm title
  if ( isXTerminal() || isScreenTerm()
    || isMinttyTerm() || isPuttyTerminal()
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
  if ( isXTerminal() || isScreenTerm()
    || isMinttyTerm() || isMltermTerminal()
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
  if ( isXTerminal() || isScreenTerm()
    || isMinttyTerm() || isMltermTerminal()
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
  if ( isXTerminal() || isScreenTerm()
    || isMinttyTerm() || isUrxvtTerminal()
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
  if ( isXTerminal() || isScreenTerm()
    || isUrxvtTerminal() || FTermcap::osc_support )
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
  if ( isXTerminal() || isScreenTerm() || FTermcap::osc_support )
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
  if ( isXTerminal() || isScreenTerm()
    || isUrxvtTerminal() || FTermcap::osc_support )
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
  if ( isPuttyTerminal() )
    return;

  setXTermMouseBackground("rgb:ffff/ffff/ffff");
  setXTermMouseForeground ("rgb:0000/0000/0000");

  if ( ! isGnomeTerminal() )
    setXTermCursorColor("rgb:ffff/ffff/ffff");

  if ( xterm_default_colors
    && ! (isMinttyTerm() || isRxvtTerminal() || isScreenTerm()) )
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

  if ( isGnomeTerminal() && term_detection->getGnomeTerminalID() < 3502 )
    return;

  if ( isPuttyTerminal() )
    return;

  if ( isXTerminal() || isScreenTerm() || FTermcap::osc_support )
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

  if ( isGnomeTerminal() && term_detection->getGnomeTerminalID() < 3502 )
    return;

  if ( isPuttyTerminal() )
    return;

  if ( isXTerminal() || isScreenTerm() || FTermcap::osc_support )
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

  if ( isGnomeTerminal() && term_detection->getGnomeTerminalID() < 3502 )
    return;

  if ( isPuttyTerminal() )
    return;

  if ( isXTerminal() || isScreenTerm() || FTermcap::osc_support )
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

  if ( isGnomeTerminal() && term_detection->getGnomeTerminalID() < 3502 )
    return;

  if ( isPuttyTerminal() )
    return;

  if ( isXTerminal() || isScreenTerm() || FTermcap::osc_support )
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

  if ( isGnomeTerminal() && term_detection->getGnomeTerminalID() < 3502 )
    return;

  if ( isPuttyTerminal() )
    return;

  if ( isXTerminal() || isScreenTerm() || FTermcap::osc_support )
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

  if ( isGnomeTerminal() && term_detection->getGnomeTerminalID() < 3502 )
    return;

  if ( isPuttyTerminal() )
    return;

  if ( isXTerminal() || isScreenTerm() || FTermcap::osc_support )
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

  if ( isGnomeTerminal() && term_detection->getGnomeTerminalID() < 3502 )
    return;

  if ( isPuttyTerminal() )
    return;

  if ( isXTerminal() || isScreenTerm()
    || isUrxvtTerminal() || FTermcap::osc_support )
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
  if ( isPuttyTerminal() )
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
  else if ( isLinuxTerm() )
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
  if ( ! isLinuxTerm() )
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
  if ( ! isLinuxTerm() )
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
    if ( fc::character[i][fc::PC] < 0x1c )
      fc::character[i][fc::PC] = fc::character[i][fc::ASCII];
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
    term_detection->setLinuxTerm (isLinuxConsole());

    if ( isLinuxTerm() )
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
void FTerm::init_global_values()
{
  // Initialize global values

  // Teletype (tty) file descriptor is still undefined
  fd_tty = -1;

  // Set default timeout for keypress
  key_timeout = 100000;  // 100 ms

  // Preset to true
  cursor_optimisation     = true;

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
  force_vt100             = \
  xterm_default_colors    = false;

  // Assertion: programm start in cooked mode
  input_data_pending      = \
  non_blocking_stdin      = false;

  // Init arrays with '\0'
  std::fill_n (exit_message, sizeof(exit_message), '\0');

  // Initialize the structs
  color_env.setDefault();
  secondary_da.setDefault();

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
  FTermcapQuirks termcap_quirks;
  termcap_quirks.setTermcapMap (tcap);  // Parameter
  termcap_quirks.setFTermDetection (term_detection);
  termcap_quirks.setTerminalType (termtype);
  termcap_quirks.terminalFixup();       // Fix terminal quirks

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

  if ( isRxvtTerminal() )
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
  (*encoding_set)["VT100"] = fc::VT100;
  (*encoding_set)["PC"]    = fc::PC;
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

  if ( isCygwinTerminal()
    || isKdeTerminal()
    || isTeraTerm()
    || isNetBSDTerm()
    || isOpenBSDTerm()
    || isSunTerminal()
    || isAnsiTerminal() )
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
  if ( isCygwinTerminal()
    || isKdeTerminal()
    || isTeraTerm()
    || isNetBSDTerm()
    || isOpenBSDTerm()
    || isSunTerminal()
    || isAnsiTerminal() )
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
  if ( isLinuxTerm() && openConsole() == 0 )
  {
    if ( isLinuxConsole() )
      gpm_mouse = true;

    closeConsole();
  }
#endif

  if ( TCAP(fc::t_key_mouse) && ! isLinuxTerm() )
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
  if ( (isXTerminal() || isUrxvtTerminal()) && ! isRxvtTerminal() )
  {
    FTermios::setCaptureSendCharacters();
    xterm_font  = getXTermFont();
    xterm_title = getXTermTitle();
    FTermios::unsetCaptureSendCharacters();
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

  if ( xterm_title )
    delete xterm_title;

  if ( xterm_font )
    delete xterm_font;

  if ( term )
    delete term;

  if ( mouse )
    delete mouse;

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

  // Get the stdin file status flags
  stdin_status_flags = fcntl(FTermios::getStdIn(), F_GETFL);

  if ( stdin_status_flags == -1 )
    std::abort();

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

  // Initializes locale information
  init_locale();

  // Detect environment and set encoding
  init_encoding();

  // Enable the terminal mouse support
  enableMouse();

  // Activate meta key sends escape
  if ( isXTerminal() )
    xtermMetaSendsESC(true);

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
  captureXTermFontAndTitle();

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

  if ( xterm_title && isXTerminal() && ! isRxvtTerminal() )
    setXTermTitle (*xterm_title);

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
  resetXTermDefaults();

  // Set xterm full block cursor
  setXTermCursorStyle(fc::steady_block);

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
  disableMouse();

  // Deactivate meta key sends escape
  if ( isXTerminal() )
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
  if ( isLinuxTerm() )
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
  if ( isLinuxTerm() && utf8_console )
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
