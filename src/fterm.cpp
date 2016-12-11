// File: fterm.cpp
// Provides: class FTerm

#include "fterm.h"
#include "fcharmap.h"
#include "fkey_map.h"
#include "ftcap_map.h"

#include "fonts/newfont.h"
#include "fonts/unicodemap.h"
#include "fonts/vgafont.h"

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
int      FTerm::stdin_status_flags;
int      FTerm::clr_bol_length;
int      FTerm::clr_eol_length;
uInt     FTerm::baudrate;
bool     FTerm::resize_term;
bool     FTerm::mouse_support;
bool     FTerm::terminal_detection;
bool     FTerm::raw_mode;
bool     FTerm::input_data_pending;
bool     FTerm::non_blocking_stdin;
bool     FTerm::gpm_mouse_enabled;
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
bool     FTerm::cygwin_terminal;
bool     FTerm::mintty_terminal;
bool     FTerm::linux_terminal;
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
termios  FTerm::term_init;
char     FTerm::termtype[30] = "";
char*    FTerm::term_name    = 0;
char*    FTerm::locale_name  = 0;
char*    FTerm::locale_xterm = 0;
FPoint*  FTerm::mouse        = 0;
FRect*   FTerm::term         = 0;

char                   FTerm::exit_message[8192]        = "";
fc::encoding           FTerm::Encoding;
const FString*         FTerm::xterm_font                = 0;
const FString*         FTerm::xterm_title               = 0;
const FString*         FTerm::answer_back               = 0;
const FString*         FTerm::sec_da                    = 0;
FOptiMove*             FTerm::opti_move                 = 0;
FOptiAttr*             FTerm::opti_attr                 = 0;
FTerm::modifier_key    FTerm::mod_key;
std::map<uChar,uChar>* FTerm::vt100_alt_char            = 0;
std::map<std::string,fc::encoding>* \
                       FTerm::encoding_set              = 0;
FTerm::termcap_map*    FTerm::tcap                      = term_caps;
bool                   FTermcap::background_color_erase = false;
bool                   FTermcap::automatic_left_margin  = false;
bool                   FTermcap::automatic_right_margin = false;
bool                   FTermcap::eat_nl_glitch          = false;
bool                   FTermcap::ansi_default_color     = false;
bool                   FTermcap::osc_support            = false;
int                    FTermcap::max_color              = 1;
uInt                   FTermcap::tabstop                = 8;
uInt                   FTermcap::attr_without_color     = 0;
console_font_op        FTerm::screen_font;
unimapdesc             FTerm::screen_unicode_map;
fc::consoleCursorStyle FTerm::console_cursor_style;


//----------------------------------------------------------------------
// class FTerm
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FTerm::FTerm()
  : color_map()
{
  resize_term = false;

  if ( ! term_initialized )
    init();
}

//----------------------------------------------------------------------
FTerm::~FTerm()  // destructor
{
  if ( init_term_object == this )
  {
    finish();

    if ( exit_message[0] )
    {
      std::fprintf (stderr, "Warning: %s\n", exit_message);
    }
  }
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
FString FTerm::getKeyName (int keynum)
{
  for (int i=0; FkeyName[i].string[0] != 0; i++)
    if ( FkeyName[i].num && FkeyName[i].num == keynum )
      return FString(FkeyName[i].string);

  if ( keynum > 32 && keynum < 127 )
    return FString(char(keynum));

  return FString("");
}

//----------------------------------------------------------------------
FTerm::modifier_key& FTerm::getModifierKey()
{
  char subcode = 6;
  // fill bit field with 0
  std::memset (&mod_key, 0x00, sizeof(mod_key));

  // TIOCLINUX, subcode=6
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
fc::consoleCursorStyle FTerm::getConsoleCursor()
{
  return console_cursor_style;
}

//----------------------------------------------------------------------
bool FTerm::isKeyTimeout (timeval* time, register long timeout)
{
  register long diff_usec;
  struct timeval now;
  struct timeval diff;

  FObject::getCurrentTime(now);
  diff.tv_sec = now.tv_sec - time->tv_sec;
  diff.tv_usec = now.tv_usec - time->tv_usec;

  if ( diff.tv_usec < 0 )
  {
    diff.tv_sec--;
    diff.tv_usec += 1000000;
  }

  diff_usec = (diff.tv_sec * 1000000) + diff.tv_usec;
  return (diff_usec > timeout);
}

//----------------------------------------------------------------------
bool FTerm::isNormal (char_data*& ch)
{
  return opti_attr->isNormal(ch);
}

//----------------------------------------------------------------------
void FTerm::setConsoleCursor (fc::consoleCursorStyle style, bool hidden)
{
  // Set cursor style in linux console
  if ( ! linux_terminal )
    return;

  console_cursor_style = style;

  if ( hidden )
    return;

  putstringf (CSI "?%dc", style);
  std::fflush(stdout);
}

//----------------------------------------------------------------------
void FTerm::setTTY (termios& t)
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
    t.c_cc[VTIME] = 0; // Timeout in deciseconds
    t.c_cc[VMIN]  = 1; // Minimum number of characters

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
bool FTerm::setUTF8 (bool on) // UTF-8 (Unicode)
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
  register uChar firstchar = uChar(buffer[0]);
  register size_t buf_len = std::strlen(buffer);
  const long key_timeout = 100000;  // 100 ms
  int key, len, n;

  if ( firstchar == ESC[0] )
  {
    // x11 mouse tracking
    if ( buf_len >= 6 && buffer[1] == '[' && buffer[2] == 'M' )
      return fc::Fkey_mouse;

    // SGR mouse tracking
    if (  buffer[1] == '[' && buffer[2] == '<' && buf_len >= 9
       && (buffer[buf_len-1] == 'M' || buffer[buf_len-1] == 'm') )
     return fc::Fkey_extended_mouse;

    // urxvt mouse tracking
    if (  buffer[1] == '[' && buffer[2] >= '1' && buffer[2] <= '9'
       && buffer[3] >= '0' && buffer[3] <= '9' && buf_len >= 9
       && buffer[buf_len-1] == 'M' )
     return fc::Fkey_urxvt_mouse;

    // look for termcap keys
    for (int i=0; Fkey[i].tname[0] != 0; i++)
    {
      char* k = Fkey[i].string;
      len = (k) ? int(std::strlen(k)) : 0;

      if ( k && std::strncmp(k, buffer, uInt(len)) == 0 ) // found
      {
        n = len;

        for (; n < buf_size; n++)   // Remove founded entry
          buffer[n-len] = buffer[n];

        for (; n-len < len; n++)    // Fill rest with '\0'
          buffer[n-len] = '\0';

        input_data_pending = bool(buffer[0] != '\0');
        return Fkey[i].num;
      }
    }

    // look for meta keys
    for (int i=0; Fmetakey[i].string[0] != 0; i++)
    {
      char* kmeta = Fmetakey[i].string;  // The string is never null
      len = int(std::strlen(kmeta));

      if ( std::strncmp(kmeta, buffer, uInt(len)) == 0 ) // found
      {
        if ( len == 2 && (  buffer[1] == 'O'
                         || buffer[1] == '['
                         || buffer[1] == ']') )
        {
          if ( ! isKeyTimeout(time_keypressed, key_timeout) )
            return NEED_MORE_DATA;
        }
        n = len;

        for (; n < buf_size; n++)    // Remove founded entry
          buffer[n-len] = buffer[n];

        for (; n-len < len; n++)     // Fill rest with '\0'
          buffer[n-len] = '\0';

        input_data_pending = bool(buffer[0] != '\0');
        return Fmetakey[i].num;
      }
    }

    if ( ! isKeyTimeout(time_keypressed, key_timeout) )
      return NEED_MORE_DATA;
  }

  // look for utf-8 character

  len = 1;

  if ( utf8_input && (firstchar & 0xc0) == 0xc0 )
  {
    char utf8char[4] = {};  // init array with '\0'

    if ((firstchar & 0xe0) == 0xc0)
      len = 2;
    else if ((firstchar & 0xf0) == 0xe0)
      len = 3;
    else if ((firstchar & 0xf8) == 0xf0)
      len = 4;

    for (n=0; n < len ; n++)
      utf8char[n] = char(buffer[n] & 0xff);

    key = UTF8decode(utf8char);
  }
  else
    key = uChar(buffer[0] & 0xff);

  n = len;

  for (; n < buf_size; n++)  // remove the key from the buffer front
    buffer[n-len] = buffer[n];

  for (n=n-len; n < buf_size; n++)   // fill the rest with '\0' bytes
    buffer[n] = '\0';

  input_data_pending = bool(buffer[0] != '\0');

  if ( key == 0 )  // Ctrl+Space or Ctrl+@
    key = fc::Fckey_space;

  return int(key == 127 ? fc::Fkey_backspace : key);
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
    Encoding = fc::PC;

    if ( xterm_terminal && utf8_console )
      Fputchar = &FTerm::putchar_UTF8;
    else
      Fputchar = &FTerm::putchar_ASCII;
  }
  else if ( linux_terminal )
  {
    if ( openConsole() == 0 )
    {
      if ( isConsole() )
      {
        // standard vga font 8x16
        int ret = setScreenFont(__8x16std, 256, 8, 16);

        if ( ret != 0 )
          VGAFont = false;

        // unicode character mapping
        struct unimapdesc unimap;
        unimap.entry_ct = uChar ( sizeof(unicode_cp437_pairs)
                                / sizeof(unipair) );
        unimap.entries = &unicode_cp437_pairs[0];
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
    Encoding = fc::PC;
    Fputchar = &FTerm::putchar_ASCII;
  }
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
    Encoding = fc::PC;

   if ( xterm_terminal && utf8_console )
      Fputchar = &FTerm::putchar_UTF8;
    else
      Fputchar = &FTerm::putchar_ASCII;
  }
  else if ( linux_terminal )
  {
    NewFont = true;

    if ( openConsole() == 0 )
    {
      if ( isConsole() )
      {
        struct unimapdesc unimap;
        int ret;

        // Set the graphical font 8x16
        ret = setScreenFont(__8x16graph, 256, 8, 16);

        if ( ret != 0 )
          NewFont = false;

        // unicode character mapping
        unimap.entry_ct = uInt16 ( sizeof(unicode_cp437_pairs)
                                 / sizeof(unipair) );
        unimap.entries = &unicode_cp437_pairs[0];
        setUnicodeMap(&unimap);
      }

      detectTermSize();
      closeConsole();
    }

    pc_charset_console = true;
    Encoding = fc::PC;
    Fputchar = &FTerm::putchar_ASCII;  // function pointer
  }
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
  else if ( linux_terminal )
  {
    if ( openConsole() == 0 )
    {
      if ( isConsole() )
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

  return retval;
}

//----------------------------------------------------------------------
char* FTerm::moveCursor (int xold, int yold, int xnew, int ynew)
{
  // returns the cursor move string
  if ( cursor_optimisation )
    return opti_move->moveCursor (xold, yold, xnew, ynew);
  else
    return tgoto(tcap[fc::t_cursor_address].string, xnew, ynew);
}

//----------------------------------------------------------------------
void FTerm::printMoveDurations()
{
  opti_move->printDurations();
}

//----------------------------------------------------------------------
char* FTerm::enableCursor()
{
  char*& vs = tcap[fc::t_cursor_visible].string;
  char*& ve = tcap[fc::t_cursor_normal].string;

  if ( ve )
    return ve;
  else if ( vs )
    return vs;

  return 0;
}

//----------------------------------------------------------------------
char* FTerm::disableCursor()
{
  char*& vi = tcap[fc::t_cursor_invisible].string;

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
    str = std::getenv("COLUMNS");
    term->setWidth(str ? std::atoi(str) : 80);
    str = std::getenv("LINES");
    term->setHeight(str ? std::atoi(str) : 25);
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
FString* FTerm::getXTermFont()
{
  if ( xterm_terminal || screen_terminal || FTermcap::osc_support )
  {
    fd_set ifds;
    struct timeval tv;
    char temp[150] = {};

    oscPrefix();
    putstring (OSC "50;?" BEL);  // get font
    oscPostfix();
    std::fflush(stdout);

    FD_ZERO(&ifds);
    FD_SET(stdin_no, &ifds);
    tv.tv_sec  = 0;
    tv.tv_usec = 150000;  // 150 ms

    // read the terminal answer
    if ( select (stdin_no+1, &ifds, 0, 0, &tv) > 0)
    {
      if ( std::scanf("\033]50;%[^\n]s", temp) == 1 )
      {
        size_t n = std::strlen(temp);

        // BEL + '\0' = string terminator
        if ( n >= 5 && temp[n-1] == BEL[0] && temp[n] == '\0' )
          temp[n-1] = '\0';

        return new FString(temp);
      }
    }
  }

  return 0;
}

//----------------------------------------------------------------------
FString* FTerm::getXTermTitle()
{
  if ( kde_konsole )
    return 0;

  fd_set ifds;
  struct timeval tv;
  char temp[512] = {};

  putstring (CSI "21t");  // get title
  std::fflush(stdout);

  FD_ZERO(&ifds);
  FD_SET(stdin_no, &ifds);
  tv.tv_sec  = 0;
  tv.tv_usec = 150000;  // 150 ms

  // read the terminal answer
  if ( select (stdin_no+1, &ifds, 0, 0, &tv) > 0)
  {
    if ( std::scanf("\033]l%[^\n]s", temp) == 1 )
    {
      size_t n = std::strlen(temp);

      // Esc + \ = OSC string terminator
      if ( n >= 2 && temp[n-2] == ESC[0] && temp[n-1] == '\\' )
      {
        if ( n < 4 )
          return 0;

        temp[n-2] = '\0';
        return new FString(temp);
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
  int c = color;
  int digits = 0;

  while ( c /= 10 )
    digits++;

  char temp[512] = {};
  putstringf (OSC "4;%d;?" BEL, color);  // get color
  std::fflush(stdout);

  FD_ZERO(&ifds);
  FD_SET(stdin_no, &ifds);
  tv.tv_sec  = 0;
  tv.tv_usec = 150000;  // 150 ms

  // read the terminal answer
  if ( select (stdin_no+1, &ifds, 0, 0, &tv) > 0)
  {
    if ( std::scanf("\033]4;%d;%[^\n]s", &color, temp) == 2 )
    {
      size_t n = std::strlen(temp);

      // BEL + '\0' = string terminator
      if ( n >= 6 && temp[n-1] == BEL[0] && temp[n] == '\0' )
        temp[n-1] = '\0';

      // Esc + \ = OSC string terminator (mintty)
      if ( n >= 6 && temp[n-2] == ESC[0] && temp[n-1] == '\\' )
        temp[n-2] = '\0';

      color_str = temp;
    }
  }

  return color_str;
}

//----------------------------------------------------------------------
void FTerm::setXTermCursorStyle (fc::xtermCursorStyle style)
{
  // Set the xterm cursor style
  if ( (xterm_terminal || mintty_terminal) && ! (gnome_terminal || kde_konsole) )
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
 // ioctl (0, GIO_CMAP, &color_map);
}

//----------------------------------------------------------------------
void FTerm::resetColorMap()
{
  char*& op = tcap[fc::t_orig_pair].string;
  char*& oc = tcap[fc::t_orig_colors].string;

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
    for (int x=0; x<16; x++)
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
  char*& Ic = tcap[fc::t_initialize_color].string;
  char*& Ip = tcap[fc::t_initialize_pair].string;

  index = FOptiAttr::vga2ansi(index);

  if ( Ic || Ip )
  {
    int rr, gg, bb;
    const char* color_str = "";

    rr = (r * 1001) / 256;
    gg = (g * 1001) / 256;
    bb = (b * 1001) / 256;

    if ( Ic )
      color_str = tparm(Ic, index, rr, gg, bb, 0, 0, 0, 0, 0);
    else if ( Ip )
      color_str = tparm(Ip, index, 0, 0, 0, rr, gg, bb, 0, 0);

    putstring (color_str);
  }
  else if ( linux_terminal )
  {
/*  // direct vga-register set
    if (  r>=0 && r<256
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
  if ( tcap[fc::t_bell].string )
  {
    putstring (tcap[fc::t_bell].string);
    std::fflush(stdout);
  }
}

//----------------------------------------------------------------------
void FTerm::setEncoding (std::string enc)
{
  std::map<std::string,fc::encoding>::const_iterator it;

  // available encodings: "UTF8", "VT100", "PC" and "ASCII"
  it = encoding_set->find(enc);

  if ( it == encoding_set->end() )  // not found
    return;

  Encoding = it->second;

  assert (  Encoding == fc::UTF8
         || Encoding == fc::VT100
         || Encoding == fc::PC
         || Encoding == fc::ASCII );

  // set the new Fputchar function pointer
  switch ( int(Encoding) )
  {
    case fc::UTF8:
      Fputchar = &FTerm::putchar_UTF8;
      break;

    case fc::VT100:
    case fc::PC:
      if ( xterm_terminal && utf8_console )
        Fputchar = &FTerm::putchar_UTF8;
      // fall through
    case fc::ASCII:
    default:
      Fputchar = &FTerm::putchar_ASCII;
  }
}

//----------------------------------------------------------------------
std::string FTerm::getEncoding()
{
  std::map<std::string,fc::encoding>::const_iterator it, end;
  end = encoding_set->end();

  for (it = encoding_set->begin(); it != end; ++it )
    if ( it->second == Encoding )
      return it->first;

  return "";
}

//----------------------------------------------------------------------
bool FTerm::scrollTermForward()
{
  if ( tcap[fc::t_scroll_forward].string )
  {
    putstring (tcap[fc::t_scroll_forward].string);
    std::fflush(stdout);
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
bool FTerm::scrollTermReverse()
{
  if ( tcap[fc::t_scroll_reverse].string )
  {
    putstring (tcap[fc::t_scroll_reverse].string);
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

  std::putchar (ENQ[0]);  // send enquiry character
  std::fflush(stdout);

  FD_ZERO(&ifds);
  FD_SET(stdin_no, &ifds);
  tv.tv_sec  = 0;
  tv.tv_usec = 150000;  // 150 ms

  // read the answerback message
  if ( select (stdin_no+1, &ifds, 0, 0, &tv) > 0)
    if ( std::fgets (temp, sizeof(temp)-1, stdin) != 0 )
      answerback = temp;

  return answerback;
}

//----------------------------------------------------------------------
const FString FTerm::getSecDA()
{
  FString sec_da_str = "";

  int a=0, b=0, c=0;
  fd_set ifds;
  struct timeval tv;

  // get the secondary device attributes
  putstring (SECDA);
  std::fflush(stdout);

  FD_ZERO(&ifds);
  FD_SET(stdin_no, &ifds);
  tv.tv_sec  = 0;
  tv.tv_usec = 600000;;  // 600 ms

  // read the answer
  if ( select (stdin_no+1, &ifds, 0, 0, &tv) == 1 )
    if ( std::scanf("\033[>%d;%d;%dc", &a, &b, &c) == 3 )
      sec_da_str.sprintf("\033[>%d;%d;%dc", a, b, c);

  return sec_da_str;
}

//----------------------------------------------------------------------
void FTerm::putstringf (const char* format, ...)
{
  assert ( format != 0 );
  char  buf[512];
  char* buffer;
  va_list args;

  buffer = buf;
  va_start (args, format);
  std::vsnprintf (buffer, sizeof(buf), format, args);
  va_end (args);

  tputs (buffer, 1, std::putchar);
}

//----------------------------------------------------------------------
inline void FTerm::putstring (const char* s, int affcnt)
{
  tputs (s, affcnt, std::putchar);
}

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
  if (c < 0x80)
  {
    // 1 Byte (7-bit): 0xxxxxxx
    std::putchar (c);
    return 1;
  }
  else if (c < 0x800)
  {
    // 2 byte (11-bit): 110xxxxx 10xxxxxx
    std::putchar (0xc0 | (c >> 6) );
    std::putchar (0x80 | (c & 0x3f) );
    return 2;
  }
  else if (c < 0x10000)
  {
    // 3 byte (16-bit): 1110xxxx 10xxxxxx 10xxxxxx
    std::putchar (0xe0 | (c >> 12) );
    std::putchar (0x80 | ((c >> 6) & 0x3f) );
    std::putchar (0x80 | (c & 0x3f) );
    return 3;
  }
  else if (c < 0x200000)
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
  register int ucs=0;

  for (register int i=0; i < int(std::strlen(utf8)); ++i)
  {
    register uChar ch = uChar(utf8[i]);

    if ((ch & 0xc0) == 0x80)
    {
      // byte 2..4 = 10xxxxxx
      ucs = (ucs << 6) | (ch & 0x3f);
    }
    else if (ch < 128)
    {
      // byte 1 = 0xxxxxxx (1 byte mapping)
      ucs = ch & 0xff;
    }
    else if ((ch & 0xe0) == 0xc0)
    {
      // byte 1 = 110xxxxx (2 byte mapping)
      ucs = ch & 0x1f;
    }
    else if ((ch & 0xf0) == 0xe0)
    {
      // byte 1 = 1110xxxx (3 byte mapping)
      ucs = ch & 0x0f;
    }
    else if ((ch & 0xf8) == 0xf0)
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
void FTerm::init_consoleCharMap()
{
  uInt c1, c2, c3, c4, c5;

  if ( NewFont || VGAFont )
    return;

  if ( screen_unicode_map.entry_ct != 0 )
  {
    for (int i=0; i <= lastCharItem; i++ )
    {
      bool found = false;

      for (uInt n=0; n < screen_unicode_map.entry_ct; n++)
      {
        if ( character[i][fc::UTF8] == screen_unicode_map.entries[n].unicode )
        {
          found = true;
          break;
        }
      }

      if ( ! found )
        character[i][fc::PC] = character[i][fc::ASCII];
    }
  }

  c1 = fc::UpperHalfBlock;
  c2 = fc::LowerHalfBlock;
  c3 = fc::FullBlock;

  if (  charEncode(c1, fc::PC) == charEncode(c1, fc::ASCII)
     || charEncode(c2, fc::PC) == charEncode(c2, fc::ASCII)
     || charEncode(c3, fc::PC) == charEncode(c3, fc::ASCII) )
  {
     no_shadow_character = true;
  }

  c4 = fc::RightHalfBlock;
  c5 = fc::LeftHalfBlock;

  if (  charEncode(c4, fc::PC) == charEncode(c4, fc::ASCII)
     || charEncode(c5, fc::PC) == charEncode(c5, fc::ASCII) )
  {
     no_half_block_character = true;
  }
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
  return charEncode (c, Encoding);
}

//----------------------------------------------------------------------
uInt FTerm::charEncode (uInt c, fc::encoding enc)
{
  for (uInt i=0; i <= uInt(lastCharItem); i++)
  {
    if ( character[i][0] == c )
    {
      c = character[i][enc];
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
void FTerm::xtermMouse (bool on)
{
  // activate/deactivate the xterm mouse support
  if ( ! mouse_support )
    return;

  if ( on )
    putstring (CSI "?1001s"   // save old highlight mouse tracking
               CSI "?1000h"   // enable x11 mouse tracking
               CSI "?1002h"   // enable cell motion mouse tracking
               CSI "?1015h"   // enable urxvt mouse mode
               CSI "?1006h"); // enable SGR mouse mode
  else
    putstring (CSI "?1006l"   // disable SGR mouse mode
               CSI "?1015l"   // disable urxvt mouse mode
               CSI "?1002l"   // disable cell motion mouse tracking
               CSI "?1000l"   // disable x11 mouse tracking
               CSI "?1001r"); // restore old highlight mouse tracking

  std::fflush(stdout);
}


#ifdef F_HAVE_LIBGPM
//----------------------------------------------------------------------
bool FTerm::gpmMouse (bool on)
{
  // activate/deactivate the gpm mouse support
  if ( ! linux_terminal )
    return false;

  if ( openConsole() == 0 )
  {
    if ( ! isConsole() )
      return false;

    closeConsole();
  }

  if ( on )
  {
    Gpm_Connect conn;
    conn.eventMask   = uInt16(~GPM_MOVE);
    conn.defaultMask = GPM_MOVE;
    conn.maxMod      = uInt16(~0);
    conn.minMod      = 0;
    Gpm_Open(&conn, 0);

    switch ( gpm_fd )
    {
      case -1:
        return false;

      case -2:
        Gpm_Close();
        return false;

      default:
        break;
    }
  }
  else
  {
    Gpm_Close();
  }

  return on;
}
#endif  // F_HAVE_LIBGPM


// private methods of FTerm
//----------------------------------------------------------------------
int FTerm::isConsole()
{
  char arg = 0;
  // get keyboard type an compare
  return (  isatty (fd_tty)
         && ioctl(fd_tty, KDGKBTYPE, &arg) == 0
         && ((arg == KB_101) || (arg == KB_84)) );
}

//----------------------------------------------------------------------
inline uInt16 FTerm::getInputStatusRegisterOne()
{
  // Gets the VGA input-status-register-1
  uInt16 misc_read = 0x3cc;  // Miscellaneous output (read port)
  uInt16 io_base = (inb(misc_read) & 0x01) ? 0x3d0 : 0x3b0;
  // 0x3ba : Input status 1 MDA (read port)
  // 0x3da : Input status 1 CGA (read port)
  return io_base + 0x0a;
}

//----------------------------------------------------------------------
uChar FTerm::readAttributeController (uChar index)
{
  // Reads a byte from the attribute controller from a given index
  uChar res;
  uInt16 attrib_cntlr_write = 0x3c0; // Attribute controller (write port)
  uInt16 attrib_cntlr_read  = 0x3c1; // Attribute controller (read port)
  uInt16 input_status_1 = getInputStatusRegisterOne();

  inb (input_status_1);  // switch to index mode
  outb (index & 0x1f, attrib_cntlr_write);
  res = inb (attrib_cntlr_read);

  inb (input_status_1);  // switch to data mode
  index = (index & 0x1f) | 0x20; // set bit 5 (enable display)
  outb (index, attrib_cntlr_write);
  inb (attrib_cntlr_read);
  return res;
}

//----------------------------------------------------------------------
void FTerm::writeAttributeController (uChar index, uChar data)
{
  // Writes a byte from the attribute controller from a given index
  uInt16 attrib_cntlr_write = 0x3c0;  // Attribute controller (write port)
  uInt16 input_status_1 = getInputStatusRegisterOne();

  inb (input_status_1);  // switch to index mode
  outb (index & 0x1f, attrib_cntlr_write);
  outb (data, attrib_cntlr_write);

  inb (input_status_1);  // switch to data mode
  index = (index & 0x1f) | 0x20; // set bit 5 (enable display)
  outb (index, attrib_cntlr_write);
  outb (data, attrib_cntlr_write);
}

//----------------------------------------------------------------------
inline uChar FTerm::getAttributeMode()
{
  // Gets the attribute mode value from the vga attribute controller
  uChar attrib_mode = 0x10;
  return readAttributeController(attrib_mode);
}

//----------------------------------------------------------------------
inline void FTerm::setAttributeMode(uChar data)
{
  // Sets the attribute mode value from the vga attribute controller
  uChar attrib_mode = 0x10;
  writeAttributeController (attrib_mode, data);
}

//----------------------------------------------------------------------
int FTerm::setBlinkAsIntensity (bool on)
{
  // Uses blink-bit as background intensity.
  // That permits 16 colors for background

  if ( getuid() != 0 ) // Direct hardware access requires root privileges
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

//----------------------------------------------------------------------
int FTerm::getFramebuffer_bpp ()
{
  int fd = -1;
  struct fb_var_screeninfo fb_var;
  struct fb_fix_screeninfo fb_fix;

  const char* fb = const_cast<char*>("/dev/fb/0");

  if ( (fd = open(fb, O_RDWR)) < 0 )
  {
    if ( errno != ENOENT && errno != ENOTDIR )
      return -1;

    fb = const_cast<char*>("/dev/fb0");

    if ( (fd = open(fb, O_RDWR)) < 0 )
      return -1;
  }

  if (fd >= 0)
  {
    if (  ! ioctl(fd, FBIOGET_VSCREENINFO, &fb_var)
       && ! ioctl(fd, FBIOGET_FSCREENINFO, &fb_fix) )
    {
      ::close(fd);
      return int(fb_var.bits_per_pixel);
    }
    else
    {
      ::close(fd);
    }
  }

  return -1;
}

//----------------------------------------------------------------------
int FTerm::openConsole()
{
  if ( fd_tty >= 0 )  // console is already opened
    return 0;

  if ( term_name && (fd_tty = open (term_name, O_RDWR, 0)) < 0)
    if ( (fd_tty = open("/proc/self/fd/0", O_RDWR, 0)) < 0)
      if ( (fd_tty = open("/dev/tty", O_RDWR, 0)) < 0)
        if ( (fd_tty = open("/dev/tty0", O_RDWR, 0)) < 0)
          if ( (fd_tty = open("/dev/vc/0", O_RDWR, 0)) < 0)
            if ( (fd_tty = open("/dev/systty", O_RDWR, 0)) < 0)
              if ( (fd_tty = open("/dev/console", O_RDWR, 0)) < 0)
                return -1;  // No file descriptor referring to the console
  return 0;
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
void FTerm::identifyTermType()
{
  // Import the untrusted environment variable TERM
  const char* term_env = std::getenv(const_cast<char*>("TERM"));

  if ( term_env )
  {
    std::strncpy (termtype, term_env, sizeof(termtype) - 1);
    return;
  }
  else if ( term_name )
  {
    // fallback: look into /etc/ttytype or /etc/ttys
    //
    // file format:
    // <terminal type> <whitespace> <tty name>
    //
    // Example:
    // linux	tty1
    // vt100	ttys0

    std::FILE *fp;

    if (  (fp = std::fopen("/etc/ttytype", "r")) != 0
       || (fp = std::fopen("/etc/ttys", "r"))    != 0 )
    {
      char* p;
      char* type;
      char* name;
      char  str[BUFSIZ];

      // get term basename
      const char* term_basename = std::strrchr(term_name, '/');

      if ( term_basename == 0 )
        term_basename = term_name;
      else
        term_basename++;

      // read and parse the file
      while ( fgets(str, sizeof(str)-1, fp) != 0 )
      {
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
          std::strncpy (termtype, type, sizeof(termtype) - 1);
          std::fclose(fp);
          return;
        }
      }

      std::fclose(fp);
    }
  }

  // use vt100 if not found
  std::strncpy (termtype, const_cast<char*>("vt100"), 6);
}

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

//----------------------------------------------------------------------
int FTerm::getScreenFont()
{
  struct console_font_op font;
  const size_t data_size = 4 * 32 * 512;
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
  font.data = new uChar[data_size]();

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
int FTerm::setScreenFont ( uChar* fontdata, uInt count
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
    const size_t data_size = bytes_per_line * 32 * font.charcount;

    try
    {
      font.data = new uChar[data_size](); // initialize with 0
    }
    catch (const std::bad_alloc& ex)
    {
      std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
      return -1;
    }

    for (uInt i=0; i < count; i++)
      std::memcpy ( const_cast<uChar*>(font.data + bytes_per_line*32*i)
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
void FTerm::init_console()
{
  // initialize terminal and Linux console
  fd_tty = -1;
  screen_unicode_map.entries = 0;
  screen_font.data = 0;

  if ( openConsole() == 0 )
  {
    if ( isConsole() )
    {
      getUnicodeMap();
      getScreenFont();
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

//----------------------------------------------------------------------
uInt FTerm::getBaudRate (const struct termios* termios_p)
{
  std::map<speed_t,uInt> ospeed;
  ospeed[B0]      = 0;       // hang up
  ospeed[B50]     = 50;      //      50 baud
  ospeed[B75]     = 75;      //      75 baud
  ospeed[B110]    = 110;     //     110 baud
  ospeed[B134]    = 134;     //     134.5 baud
  ospeed[B150]    = 150;     //     150 baud
  ospeed[B200]    = 200;     //     200 baud
  ospeed[B300]    = 300;     //     300 baud
  ospeed[B600]    = 600;     //     600 baud
  ospeed[B1200]   = 1200;    //   1,200 baud
  ospeed[B1800]   = 1800;    //   1,800 baud
  ospeed[B2400]   = 2400;    //   2,400 baud
  ospeed[B4800]   = 4800;    //   4,800 baud
  ospeed[B9600]   = 9600;    //   9,600 baud
  ospeed[B19200]  = 19200;   //  19,200 baud
  ospeed[B38400]  = 38400;   //  38,400 baud
  ospeed[B57600]  = 57600;   //  57,600 baud
  ospeed[B115200] = 115200;  // 115,200 baud
  ospeed[B230400] = 230400;  // 230,400 baud

  return ospeed[cfgetospeed(termios_p)];
}

//----------------------------------------------------------------------
char* FTerm::init_256colorTerminal()
{
  char local256[80] = "";
  char* new_termtype = 0;
  char *s1, *s2, *s3, *s4, *s5, *s6;

  // Enable 256 color capabilities
  s1 = std::getenv("COLORTERM");
  s2 = std::getenv("VTE_VERSION");
  s3 = std::getenv("XTERM_VERSION");
  s4 = std::getenv("ROXTERM_ID");
  s5 = std::getenv("KONSOLE_DBUS_SESSION");
  s6 = std::getenv("KONSOLE_DCOP");

  if ( s1 != 0 )
    std::strncat (local256, s1, sizeof(local256) - std::strlen(local256) - 1);

  if ( s2 != 0 )
    std::strncat (local256, s2, sizeof(local256) - std::strlen(local256) - 1);

  if ( s3 != 0 )
    std::strncat (local256, s3, sizeof(local256) - std::strlen(local256) - 1);

  if ( s4 != 0 )
    std::strncat (local256, s4, sizeof(local256) - std::strlen(local256) - 1);

  if ( s5 != 0 )
    std::strncat (local256, s5, sizeof(local256) - std::strlen(local256) - 1);

  if ( s6 != 0 )
    std::strncat (local256, s6, sizeof(local256) - std::strlen(local256) - 1);

  if ( std::strlen(local256) > 0 )
  {
    if ( std::strncmp(termtype, "xterm", 5) == 0 )
      new_termtype = const_cast<char*>("xterm-256color");

    if ( std::strncmp(termtype, "screen", 6) == 0 )
    {
      new_termtype = const_cast<char*>("screen-256color");
      screen_terminal = true;
      char* tmux = std::getenv("TMUX");

      if ( tmux && std::strlen(tmux) != 0 )
        tmux_terminal = true;
    }

    if ( std::strncmp(termtype, "Eterm", 5) == 0 )
      new_termtype = const_cast<char*>("Eterm-256color");

    if ( std::strncmp(termtype, "mlterm", 6) == 0 )
    {
      new_termtype = const_cast<char*>("mlterm-256color");
      mlterm_terminal = true;
    }

    if ( std::strncmp(termtype, "rxvt", 4) != 0
       && s1
       && std::strncmp(s1, "rxvt-xpm", 8) == 0 )
    {
      new_termtype = const_cast<char*>("rxvt-256color");
      rxvt_terminal = true;
    }

    color256 = true;
  }
  else if ( std::strstr (termtype, "256color") )
    color256 = true;
  else
    color256 = false;

  if ( (s5 && std::strlen(s5) > 0) || (s6 && std::strlen(s6) > 0) )
    kde_konsole = true;

  if ( (s1 && std::strncmp(s1, "gnome-terminal", 14) == 0) || s2 )
  {
    gnome_terminal = true;
    // Each gnome-terminal should be able to use 256 colors
    color256 = true;
    new_termtype = const_cast<char*>("gnome-256color");
  }

  return new_termtype;
}

//----------------------------------------------------------------------
char* FTerm::parseAnswerbackMsg (char*& current_termtype)
{
  char* new_termtype = current_termtype;

  // send ENQ and read the answerback message
  answer_back = new FString(getAnswerbackMsg());

  if ( answer_back && *answer_back == FString("PuTTY") )
  {
    putty_terminal = true;

    if ( color256 )
      new_termtype = const_cast<char*>("putty-256color");
    else
      new_termtype = const_cast<char*>("putty");
  }
  else
    putty_terminal = false;

  // cygwin needs a backspace to delete the '♣' char
  if ( cygwin_terminal )
    putstring (BS " " BS);

  return new_termtype;
}

//----------------------------------------------------------------------
char* FTerm::parseSecDA (char*& current_termtype)
{
  char* new_termtype = current_termtype;
  bool  sec_da_supported = false;


  // The Linux console knows no Sec_DA
  if ( linux_terminal )
    return new_termtype;

  // secondary device attributes (SEC_DA) <- decTerminalID string
  sec_da = new FString(getSecDA());

  if ( sec_da && sec_da->getLength() > 5 )
  {
    uLong num_components;

    // remove the first 3 bytes ("\033[>")
    FString temp = sec_da->right(sec_da->getLength() - 3);
    // remove the last byte ("c")
    temp.remove(temp.getLength()-1, 1);
    // split into components
    std::vector<FString> sec_da_split = temp.split(';');

    num_components = sec_da_split.size();

    if ( num_components == 3 )
      sec_da_supported = true;

    if ( num_components >= 2 )
    {
      FString* sec_da_components = &sec_da_split[0];

      if ( ! sec_da_components[0].isEmpty() )
      {
        int terminal_id_type, terminal_id_version;

        // Read the terminal type
        try
        {
          terminal_id_type = sec_da_components[0].toInt();
        }
        catch (const std::exception&)
        {
          terminal_id_type = -1;
        }

        // Read the terminal (firmware) version
        try
        {
          if ( sec_da_components[1] )
            terminal_id_version = sec_da_components[1].toInt();
          else
            terminal_id_version = -1;
        }
        catch (const std::exception&)
        {
          terminal_id_version = -1;
        }
        switch ( terminal_id_type )
        {
          case 0: // DEC VT100
            if ( terminal_id_version == 115 )
              kde_konsole = true;
            else if ( terminal_id_version == 136 )
              putty_terminal = true;  // PuTTY
            break;

          case 1: // DEC VT220
            if ( ! sec_da_supported )
            {
              if ( terminal_id_version ==  2 )  // also used by apple terminal
                kterm_terminal = true;  // kterm
            }
            else if ( terminal_id_version > 1000 )
            {
              gnome_terminal = true;
              // Each gnome-terminal should be able to use 256 colors
              color256 = true;
              new_termtype = const_cast<char*>("gnome-256color");
            }
            break;

          case 2:  // DEC VT240
          case 18: // DEC VT330
          case 19: // DEC VT340
          case 24: // DEC VT320
          case 41: // DEC VT420
          case 61: // DEC VT510
          case 64: // DEC VT520
          case 65: // DEC VT525
            break;

          case 32:  // Tera Term
            tera_terminal = true;
            new_termtype = const_cast<char*>("teraterm");
            break;

          case 77:  // mintty
            mintty_terminal = true;
            new_termtype = const_cast<char*>("xterm-256color");
            // switch to application escape key mode
            putstring (CSI "?7727h");
            std::fflush(stdout);
            break;

          case 83:  // screen
            screen_terminal = true;
            break;

          case 82:  // rxvt
            rxvt_terminal = true;
            force_vt100 = true;  // this rxvt terminal support on utf-8

            if (  std::strncmp(termtype, "rxvt-", 5) != 0
               || std::strncmp(termtype, "rxvt-cygwin-native", 5) == 0 )
              new_termtype = const_cast<char*>("rxvt-16color");
            break;

          case 85:  // rxvt-unicode
            rxvt_terminal = true;
            urxvt_terminal = true;

            if ( std::strncmp(termtype, "rxvt-", 5) != 0 )
            {
              if ( color256 )
                new_termtype = const_cast<char*>("rxvt-256color");
              else
                new_termtype = const_cast<char*>("rxvt");
            }
            break;

          default:
            break;
        }
      }
    }
  }

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
  // read the used vt100 pairs
  if ( tcap[fc::t_acs_chars].string )
  {
    for (int n=0; tcap[fc::t_acs_chars].string[n]; tcap[fc::t_acs_chars].string += 2)
    {
      // insert the vt100 key/value pairs into a map
      uChar p1 = uChar(tcap[fc::t_acs_chars].string[n]);
      uChar p2 = uChar(tcap[fc::t_acs_chars].string[n+1]);
      (*vt100_alt_char)[p1] = p2;
    }
  }

  enum column
  {
    vt100_key = 0,
    utf8_char = 1
  };

  // update array 'character' with discovered vt100 pairs
  for (int n=0; n <= lastKeyItem; n++ )
  {
    uChar keyChar = uChar(vt100_key_to_utf8[n][vt100_key]);
    uChar altChar = uChar((*vt100_alt_char)[ keyChar ]);
    uInt utf8char = uInt(vt100_key_to_utf8[n][utf8_char]);
    fc::encoding num = fc::NUM_OF_ENCODINGS;

    uInt* p = std::find ( character[0]
                        , character[lastCharItem] + num
                        , utf8char );
    if ( p != character[lastCharItem] + num ) // found in character
    {
      int item = int(std::distance(character[0], p) / num);

      if ( altChar )
        character[item][fc::VT100] = altChar; // update alternate character set
      else
        character[item][fc::VT100] = 0; // delete vt100 char in character
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
    // fallback if tcap "S2" is not found
    if ( ! tcap[fc::t_enter_pc_charset_mode].string )
    {
      if ( utf8_console )
      {
        // Select iso8859-1 + null mapping
        tcap[fc::t_enter_pc_charset_mode].string = \
          const_cast<char*>(ESC "%@" ESC "(U");
      }
      else
      {
        // Select null mapping
        tcap[fc::t_enter_pc_charset_mode].string = \
          const_cast<char*>(ESC "(U");
      }

      opti_attr->set_enter_pc_charset_mode (tcap[fc::t_enter_pc_charset_mode].string);
      reinit = true;
    }

    // fallback if tcap "S3" is not found
    if ( ! tcap[fc::t_exit_pc_charset_mode].string )
    {
      if ( utf8_console )
      {
        // Select ascii mapping + utf8
        tcap[fc::t_exit_pc_charset_mode].string = \
          const_cast<char*>(ESC "(B" ESC "%G");
      }
      else
      {
        // Select ascii mapping
        tcap[fc::t_enter_pc_charset_mode].string = \
          const_cast<char*>(ESC "(B");
      }

      opti_attr->set_exit_pc_charset_mode (tcap[fc::t_exit_pc_charset_mode].string);
      reinit = true;
    }
  }

  if ( reinit )
    opti_attr->init();
}

//----------------------------------------------------------------------
void FTerm::init_termcaps()
{
  /* Terminal capability data base
   * -----------------------------
   * Info under: man 5 terminfo
   *
   * Importent shell commands:
   *   captoinfo - convert all termcap descriptions into terminfo descriptions
   *   infocmp   - print out terminfo description from the current terminal
   */
  static char term_buffer[2048];
  static char string_buf[2048];
  char* buffer = string_buf;

  // share the terminal capabilities
  FTermcap().setTermcapMap(tcap);

  // open the termcap file + load entry for termtype
  if ( tgetent(term_buffer, termtype) == 1 )
  {
    char* key_up_string;

    // screen erased with the background color
    FTermcap::background_color_erase = tgetflag(const_cast<char*>("ut"));

    // t_cursor_left wraps from column 0 to last column
    FTermcap::automatic_left_margin = tgetflag(const_cast<char*>("bw"));

    // terminal has auto-matic margins
    FTermcap::automatic_right_margin = tgetflag(const_cast<char*>("am"));

    // newline ignored after 80 cols
    FTermcap::eat_nl_glitch = tgetflag(const_cast<char*>("xn"));

    // terminal supports ANSI set default fg and bg color
    FTermcap::ansi_default_color = tgetflag(const_cast<char*>("AX"));

    // terminal supports operating system commands (OSC)
    // OSC = Esc + ']'
    FTermcap::osc_support = tgetflag(const_cast<char*>("XT"));

    if ( isTeraTerm() )
      FTermcap::eat_nl_glitch = true;

    // maximum number of colors on screen
    FTermcap::max_color = std::max( FTermcap::max_color
                                  , tgetnum(const_cast<char*>("Co")) );

    if ( FTermcap::max_color < 0 )
      FTermcap::max_color = 1;

    if ( FTermcap::max_color < 8 )
      monochron = true;
    else
      monochron = false;

    FTermcap::tabstop = uInt(tgetnum(const_cast<char*>("it")));
    FTermcap::attr_without_color = uInt(tgetnum(const_cast<char*>("NC")));

    // gnome-terminal has NC=16 however, it can use the dim attribute
    if ( gnome_terminal )
      FTermcap::attr_without_color = 0;

    // PuTTY has NC=22 however, it can show underline and reverse
    if ( putty_terminal )
      FTermcap::attr_without_color = 16;

    // read termcap output strings
    for (int i=0; tcap[i].tname[0] != 0; i++)
      tcap[i].string = tgetstr(tcap[i].tname, &buffer);

    // set invisible cursor for cygwin terminal
    if ( cygwin_terminal && ! tcap[fc::t_cursor_invisible].string )
      tcap[fc::t_cursor_invisible].string = \
        const_cast<char*>(CSI "?25l");

    // set visible cursor for cygwin terminal
    if ( cygwin_terminal && ! tcap[fc::t_cursor_visible].string )
      tcap[fc::t_cursor_visible].string = \
        const_cast<char*>(CSI "?25h");

    // set ansi blink for cygwin terminal
    if ( cygwin_terminal && ! tcap[fc::t_enter_blink_mode].string )
      tcap[fc::t_enter_blink_mode].string = \
        const_cast<char*>(CSI "5m");

    // set enter/exit alternative charset mode for rxvt terminal
    if ( rxvt_terminal && std::strncmp(termtype, "rxvt-16color", 12) == 0 )
    {
      tcap[fc::t_enter_alt_charset_mode].string = \
        const_cast<char*>(ESC "(0");
      tcap[fc::t_exit_alt_charset_mode].string  = \
        const_cast<char*>(ESC "(B");
    }

    // set exit underline for gnome terminal
    if ( gnome_terminal )
      tcap[fc::t_exit_underline_mode].string = \
        const_cast<char*>(CSI "24m");

    // set background color erase for cygwin terminal
    if ( cygwin_terminal )
      FTermcap::background_color_erase = true;

    // set ansi foreground and background color
    if ( linux_terminal || cygwin_terminal )
    {
      if ( FTermcap::max_color > 8 )
      {
        tcap[fc::t_set_a_foreground].string = \
          const_cast<char*>(CSI "3%p1%{8}%m%d%?%p1%{7}%>%t;1%e;21%;m");
        tcap[fc::t_set_a_background].string = \
          const_cast<char*>(CSI "4%p1%{8}%m%d%?%p1%{7}%>%t;5%e;25%;m");
      }
      else
      {
        tcap[fc::t_set_a_foreground].string = \
          const_cast<char*>(CSI "3%p1%dm");
        tcap[fc::t_set_a_background].string = \
          const_cast<char*>(CSI "4%p1%dm");
      }

      tcap[fc::t_orig_pair].string = \
        const_cast<char*>(CSI "39;49;25m");

      // avoid dim + underline
      tcap[fc::t_enter_dim_mode].string       = 0;
      tcap[fc::t_exit_dim_mode].string        = 0;
      tcap[fc::t_enter_underline_mode].string = 0;
      tcap[fc::t_exit_underline_mode].string  = 0;
      FTermcap::attr_without_color            = 18;
    }
    else if ( rxvt_terminal && ! urxvt_terminal )
    {
      tcap[fc::t_set_a_foreground].string = \
        const_cast<char*>(CSI "%?%p1%{8}%<%t%p1%{30}%+%e%p1%'R'%+%;%dm");
      tcap[fc::t_set_a_background].string = \
        const_cast<char*>(CSI "%?%p1%{8}%<%t%p1%'('%+%e%p1%{92}%+%;%dm");
    }
    else if ( tera_terminal )
    {
      tcap[fc::t_set_a_foreground].string = \
        const_cast<char*>(CSI "38;5;%p1%dm");
      tcap[fc::t_set_a_background].string = \
        const_cast<char*>(CSI "48;5;%p1%dm");
      tcap[fc::t_exit_attribute_mode].string = \
        const_cast<char*>(CSI "0m" SI);
    }
    else if ( putty_terminal )
    {
      tcap[fc::t_set_a_foreground].string = \
        const_cast<char*>(CSI "%?%p1%{8}%<"
                          "%t3%p1%d"
                          "%e%p1%{16}%<"
                          "%t9%p1%{8}%-%d"
                          "%e38;5;%p1%d%;m");
      tcap[fc::t_set_a_background].string = \
        const_cast<char*>(CSI "%?%p1%{8}%<"
                          "%t4%p1%d"
                          "%e%p1%{16}%<"
                          "%t10%p1%{8}%-%d"
                          "%e48;5;%p1%d%;m");
    }

    // fallback if "AF" is not found
    if ( ! tcap[fc::t_set_a_foreground].string )
      tcap[fc::t_set_a_foreground].string = \
        const_cast<char*>(CSI "3%p1%dm");

    // fallback if "AB" is not found
    if ( ! tcap[fc::t_set_a_background].string )
      tcap[fc::t_set_a_background].string = \
        const_cast<char*>(CSI "4%p1%dm");

    // fallback if "Ic" is not found
    if ( ! tcap[fc::t_initialize_color].string )
    {
      if ( screen_terminal )
      {
        if ( tmux_terminal )
        {
          tcap[fc::t_initialize_color].string = \
            const_cast<char*>(ESC "Ptmux;" ESC OSC "4;%p1%d;rgb:"
                              "%p2%{255}%*%{1000}%/%2.2X/"
                              "%p3%{255}%*%{1000}%/%2.2X/"
                              "%p4%{255}%*%{1000}%/%2.2X" BEL ESC "\\");
        }
        else
        {
          tcap[fc::t_initialize_color].string = \
            const_cast<char*>(ESC "P" OSC "4;%p1%d;rgb:"
                              "%p2%{255}%*%{1000}%/%2.2X/"
                              "%p3%{255}%*%{1000}%/%2.2X/"
                              "%p4%{255}%*%{1000}%/%2.2X" BEL ESC "\\");
        }
      }
      else
      {
        tcap[fc::t_initialize_color].string = \
          const_cast<char*>(OSC "P%p1%x"
                            "%p2%{255}%*%{1000}%/%02x"
                            "%p3%{255}%*%{1000}%/%02x"
                            "%p4%{255}%*%{1000}%/%02x");
      }
   }

    // fallback if "ti" is not found
    if ( ! tcap[fc::t_enter_ca_mode].string )
      tcap[fc::t_enter_ca_mode].string = \
        const_cast<char*>(ESC "7" CSI "?47h");

    // fallback if "te" is not found
    if ( ! tcap[fc::t_exit_ca_mode].string )
      tcap[fc::t_exit_ca_mode].string = \
        const_cast<char*>(CSI "?47l" ESC "8" CSI "m");

    // set ansi move if "cm" is not found
    if ( ! tcap[fc::t_cursor_address].string )
      tcap[fc::t_cursor_address].string = \
        const_cast<char*>(CSI "%i%p1%d;%p2%dH");

    // test for standard ECMA-48 (ANSI X3.64) terminal
    if ( tcap[fc::t_exit_underline_mode].string
       && std::strncmp(tcap[fc::t_exit_underline_mode].string, CSI "24m", 5) == 0 )
    {
      // seems to be a ECMA-48 (ANSI X3.64) compatible terminal
      tcap[fc::t_enter_dbl_underline_mode].string = \
        const_cast<char*>(CSI "21m");  // Exit single underline, too

      tcap[fc::t_exit_dbl_underline_mode].string = \
        const_cast<char*>(CSI "24m");

      tcap[fc::t_exit_bold_mode].string = \
        const_cast<char*>(CSI "22m");  // Exit dim, too

      tcap[fc::t_exit_dim_mode].string = \
        const_cast<char*>(CSI "22m");

      tcap[fc::t_exit_underline_mode].string = \
        const_cast<char*>(CSI "24m");

      tcap[fc::t_exit_blink_mode].string = \
        const_cast<char*>(CSI "25m");

      tcap[fc::t_exit_reverse_mode].string = \
        const_cast<char*>(CSI "27m");

      tcap[fc::t_exit_secure_mode].string = \
        const_cast<char*>(CSI "28m");

      tcap[fc::t_enter_crossed_out_mode].string = \
        const_cast<char*>(CSI "9m");

      tcap[fc::t_exit_crossed_out_mode].string = \
        const_cast<char*>(CSI "29m");
    }

    // read termcap key strings
    for (int i=0; Fkey[i].tname[0] != 0; i++)
    {
      Fkey[i].string = tgetstr(Fkey[i].tname, &buffer);

      // fallback for rxvt with TERM=xterm
      if ( std::strncmp(Fkey[i].tname, "khx", 3) == 0 )
        Fkey[i].string = const_cast<char*>(CSI "7~");  // home key

      if ( std::strncmp(Fkey[i].tname, "@7x", 3) == 0 )
        Fkey[i].string = const_cast<char*>(CSI "8~");  // end key

      if ( std::strncmp(Fkey[i].tname, "k1x", 3) == 0 )
        Fkey[i].string = const_cast<char*>(CSI "11~");  // F1

      if ( std::strncmp(Fkey[i].tname, "k2x", 3) == 0 )
        Fkey[i].string = const_cast<char*>(CSI "12~");  // F2

      if ( std::strncmp(Fkey[i].tname, "k3x", 3) == 0 )
        Fkey[i].string = const_cast<char*>(CSI "13~");  // F3

      if ( std::strncmp(Fkey[i].tname, "k4x", 3) == 0 )
        Fkey[i].string = const_cast<char*>(CSI "14~");  // F4

      // fallback for TERM=ansi
      if ( std::strncmp(Fkey[i].tname, "@7X", 3) == 0 )
        Fkey[i].string = const_cast<char*>(CSI "K");  // end key
    }

    // Some terminals (e.g. PuTTY) send the wrong code for the arrow keys
    // http://www.unix.com/shell-programming-scripting/..
    //      ..110380-using-arrow-keys-shell-scripts.html
    key_up_string = tgetstr(const_cast<char*>("ku"), &buffer);

    if (  (key_up_string && (std::strcmp(key_up_string, CSI "A") == 0))
       || (  tcap[fc::t_cursor_up].string
          && (std::strcmp(tcap[fc::t_cursor_up].string, CSI "A") == 0)) )
    {
      for (int i=0; Fkey[i].tname[0] != 0; i++)
      {
        if ( std::strncmp(Fkey[i].tname, "kux", 3) == 0 )
          Fkey[i].string = const_cast<char*>(CSI "A");  // key up

        if ( std::strncmp(Fkey[i].tname, "kdx", 3) == 0 )
          Fkey[i].string = const_cast<char*>(CSI "B");  // key down

        if ( std::strncmp(Fkey[i].tname, "krx", 3) == 0 )
          Fkey[i].string = const_cast<char*>(CSI "C");  // key right

        if ( std::strncmp(Fkey[i].tname, "klx", 3) == 0 )
          Fkey[i].string = const_cast<char*>(CSI "D");  // key left

        if ( std::strncmp(Fkey[i].tname, "k1X", 3) == 0 )
          Fkey[i].string = const_cast<char*>(ESC "OP");  // PF1

        if ( std::strncmp(Fkey[i].tname, "k2X", 3) == 0 )
          Fkey[i].string = const_cast<char*>(ESC "OQ");  // PF2

        if ( std::strncmp(Fkey[i].tname, "k3X", 3) == 0 )
          Fkey[i].string = const_cast<char*>(ESC "OR");  // PF3

        if ( std::strncmp(Fkey[i].tname, "k4X", 3) == 0 )
          Fkey[i].string = const_cast<char*>(ESC "OS");  // PF4
      }
    }
  }
  else
  {
    std::cerr << "Unknown terminal: "  << termtype << "\n"
              << "Check the TERM environment variable\n"
              << "Also make sure that the terminal"
              << "is defined in the terminfo database.\n";
    std::abort();
  }

  // duration precalculation of the cursor movement strings
  opti_move->setTabStop(int(FTermcap::tabstop));
  opti_move->set_cursor_home (tcap[fc::t_cursor_home].string);
  opti_move->set_cursor_to_ll (tcap[fc::t_cursor_to_ll].string);
  opti_move->set_carriage_return (tcap[fc::t_carriage_return].string);
  opti_move->set_tabular (tcap[fc::t_tab].string);
  opti_move->set_back_tab (tcap[fc::t_back_tab].string);
  opti_move->set_cursor_up (tcap[fc::t_cursor_up].string);
  opti_move->set_cursor_down (tcap[fc::t_cursor_down].string);
  opti_move->set_cursor_left (tcap[fc::t_cursor_left].string);
  opti_move->set_cursor_right (tcap[fc::t_cursor_right].string);
  opti_move->set_cursor_address (tcap[fc::t_cursor_address].string);
  opti_move->set_column_address (tcap[fc::t_column_address].string);
  opti_move->set_row_address (tcap[fc::t_row_address].string);
  opti_move->set_parm_up_cursor (tcap[fc::t_parm_up_cursor].string);
  opti_move->set_parm_down_cursor (tcap[fc::t_parm_down_cursor].string);
  opti_move->set_parm_left_cursor (tcap[fc::t_parm_left_cursor].string);
  opti_move->set_parm_right_cursor (tcap[fc::t_parm_right_cursor].string);
  opti_move->set_auto_left_margin (FTermcap::automatic_left_margin);
  opti_move->set_eat_newline_glitch (FTermcap::eat_nl_glitch);
  clr_bol_length = opti_move->set_clr_bol (tcap[fc::t_clr_bol].string);
  clr_eol_length = opti_move->set_clr_eol (tcap[fc::t_clr_eol].string);

  // attribute settings
  opti_attr->setNoColorVideo (int(FTermcap::attr_without_color));
  opti_attr->set_enter_bold_mode (tcap[fc::t_enter_bold_mode].string);
  opti_attr->set_exit_bold_mode (tcap[fc::t_exit_bold_mode].string);
  opti_attr->set_enter_dim_mode (tcap[fc::t_enter_dim_mode].string);
  opti_attr->set_exit_dim_mode (tcap[fc::t_exit_dim_mode].string);
  opti_attr->set_enter_italics_mode (tcap[fc::t_enter_italics_mode].string);
  opti_attr->set_exit_italics_mode (tcap[fc::t_exit_italics_mode].string);
  opti_attr->set_enter_underline_mode (tcap[fc::t_enter_underline_mode].string);
  opti_attr->set_exit_underline_mode (tcap[fc::t_exit_underline_mode].string);
  opti_attr->set_enter_blink_mode (tcap[fc::t_enter_blink_mode].string);
  opti_attr->set_exit_blink_mode (tcap[fc::t_exit_blink_mode].string);
  opti_attr->set_enter_reverse_mode (tcap[fc::t_enter_reverse_mode].string);
  opti_attr->set_exit_reverse_mode (tcap[fc::t_exit_reverse_mode].string);
  opti_attr->set_enter_standout_mode (tcap[fc::t_enter_standout_mode].string);
  opti_attr->set_exit_standout_mode (tcap[fc::t_exit_standout_mode].string);
  opti_attr->set_enter_secure_mode (tcap[fc::t_enter_secure_mode].string);
  opti_attr->set_exit_secure_mode (tcap[fc::t_exit_secure_mode].string);
  opti_attr->set_enter_protected_mode (tcap[fc::t_enter_protected_mode].string);
  opti_attr->set_exit_protected_mode (tcap[fc::t_exit_protected_mode].string);
  opti_attr->set_enter_crossed_out_mode (tcap[fc::t_enter_crossed_out_mode].string);
  opti_attr->set_exit_crossed_out_mode (tcap[fc::t_exit_crossed_out_mode].string);
  opti_attr->set_enter_dbl_underline_mode (tcap[fc::t_enter_dbl_underline_mode].string);
  opti_attr->set_exit_dbl_underline_mode (tcap[fc::t_exit_dbl_underline_mode].string);
  opti_attr->set_set_attributes (tcap[fc::t_set_attributes].string);
  opti_attr->set_exit_attribute_mode (tcap[fc::t_exit_attribute_mode].string);
  opti_attr->set_enter_alt_charset_mode (tcap[fc::t_enter_alt_charset_mode].string);
  opti_attr->set_exit_alt_charset_mode (tcap[fc::t_exit_alt_charset_mode].string);
  opti_attr->set_enter_pc_charset_mode (tcap[fc::t_enter_pc_charset_mode].string);
  opti_attr->set_exit_pc_charset_mode (tcap[fc::t_exit_pc_charset_mode].string);
  opti_attr->set_a_foreground_color (tcap[fc::t_set_a_foreground].string);
  opti_attr->set_a_background_color (tcap[fc::t_set_a_background].string);
  opti_attr->set_foreground_color (tcap[fc::t_set_foreground].string);
  opti_attr->set_background_color (tcap[fc::t_set_background].string);
  opti_attr->set_term_color_pair (tcap[fc::t_set_color_pair].string);
  opti_attr->set_orig_pair (tcap[fc::t_orig_pair].string);
  opti_attr->set_orig_orig_colors (tcap[fc::t_orig_colors].string);
  opti_attr->setMaxColor (FTermcap::max_color);

  if ( FTermcap::ansi_default_color )
    opti_attr->setDefaultColorSupport();

  if ( cygwin_terminal )
    opti_attr->setCygwinTerminal();

  opti_attr->init();
}

//----------------------------------------------------------------------
void FTerm::init_encoding()
{
  // detect encoding and set the Fputchar function pointer

  if ( isatty(stdout_no) && ! std::strcmp(nl_langinfo(CODESET), "UTF-8") )
  {
    utf8_console = true;
    Encoding = fc::UTF8;
    Fputchar = &FTerm::putchar_UTF8;  // function pointer
    utf8_state = true;
    utf8_input = true;
    setUTF8(true);
  }
  else if (  isatty(stdout_no)
          && (std::strlen(termtype) > 0)
          && (tcap[fc::t_exit_alt_charset_mode].string != 0) )
  {
    vt100_console = true;
    Encoding = fc::VT100;
    Fputchar = &FTerm::putchar_ASCII;  // function pointer
  }
  else
  {
    ascii_console = true;
    Encoding = fc::ASCII;
    Fputchar = &FTerm::putchar_ASCII;  // function pointer
  }

  init_pc_charset();

  if (  linux_terminal
     || cygwin_terminal
     || NewFont
     || (putty_terminal && ! utf8_state) )
  {
    pc_charset_console = true;
    Encoding = fc::PC;
    Fputchar = &FTerm::putchar_ASCII;  // function pointer

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

  if ( force_vt100 )
  {
    vt100_console = true;
    Encoding = fc::VT100;
    Fputchar = &FTerm::putchar_ASCII;  // function pointer
  }
}

//----------------------------------------------------------------------
void FTerm::init()
{
  char* new_termtype = 0;
  term_initialized = true;
  init_term_object = this;
  fd_tty      = -1;
  opti_move   = new FOptiMove();
  opti_attr   = new FOptiAttr();
  term        = new FRect(0,0,0,0);
  mouse       = new FPoint(0,0);

  vt100_alt_char = new std::map<uChar,uChar>;
  encoding_set   = new std::map<std::string,fc::encoding>;

  // Define the encoding set
  (*encoding_set)["UTF8"]  = fc::UTF8;
  (*encoding_set)["UTF-8"] = fc::UTF8;
  (*encoding_set)["VT100"] = fc::VT100;
  (*encoding_set)["PC"]    = fc::PC;
  (*encoding_set)["ASCII"] = fc::ASCII;

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
  mouse_support           = \
  force_vt100             = \
  tera_terminal           = \
  kterm_terminal          = \
  gnome_terminal          = \
  kde_konsole             = \
  ansi_terminal           = \
  rxvt_terminal           = \
  urxvt_terminal          = \
  mlterm_terminal         = \
  mintty_terminal         = \
  screen_terminal         = \
  tmux_terminal           = \
  xterm_default_colors    = false;

  // Preset to true
  cursor_optimisation     = \
  terminal_detection      = true;

  // assertion: programm start in cooked mode
  raw_mode                = \
  input_data_pending      = \
  non_blocking_stdin      = false;

  // init arrays with '\0'
  std::fill_n (exit_message, sizeof(exit_message), '\0');

  stdin_no  = fileno(stdin);
  stdout_no = fileno(stdout);
  stdin_status_flags = fcntl(stdin_no, F_GETFL);

  if ( stdin_status_flags == -1 )
    std::abort();

  term_name = ttyname(stdout_no);

  // initialize terminal and Linux console
  init_console();

  // save termios settings
  storeTTYsettings();

  // get output baud rate
  baudrate = getBaudRate(&term_init);

  if ( isatty(stdout_no) )
    opti_move->setBaudRate(int(baudrate));

  // detect the type of the terminal
  identifyTermType();

  // initialize 256 colors terminals
  new_termtype = init_256colorTerminal();

  if ( std::strncmp(termtype, "cygwin", 6) == 0 )
    cygwin_terminal = true;
  else
    cygwin_terminal = false;

  if ( std::strncmp(termtype, "rxvt-cygwin-native", 18) == 0 )
    rxvt_terminal = true;

  if ( std::strncmp(termtype, "ansi", 4) == 0 )
  {
    terminal_detection = false;
    ansi_terminal = true;
  }

  // Test for Linux console
  if (  std::strncmp(termtype, const_cast<char*>("linux"), 5) == 0
     || std::strncmp(termtype, const_cast<char*>("con"), 3) == 0 )
    linux_terminal = true;
  else
    linux_terminal = false;

  // terminal detection
  if ( terminal_detection )
  {
    struct termios t;
    tcgetattr (stdin_no, &t);
    t.c_lflag &= uInt(~(ICANON | ECHO));
    t.c_cc[VTIME] = 1; // Timeout in deciseconds
    t.c_cc[VMIN]  = 0; // Minimum number of characters
    tcsetattr (stdin_no, TCSANOW, &t);


    // Identify the terminal via the answerback-message
    new_termtype = parseAnswerbackMsg (new_termtype);

    // Identify the terminal via the secondary device attributes (SEC_DA)
    new_termtype = parseSecDA (new_termtype);

    // Determine xterm maximum number of colors via OSC 4
    if ( ! color256
       && ! cygwin_terminal
       && ! tera_terminal
       && ! linux_terminal
       && getXTermColorName(0) != "" )
    {
      if ( getXTermColorName(256) != "" )
      {
        new_termtype = const_cast<char*>("xterm-256color");
      }
      else if ( getXTermColorName(87) != "" )
      {
        new_termtype = const_cast<char*>("xterm-88color");
      }
      else if ( getXTermColorName(15) != "" )
      {
        new_termtype = const_cast<char*>("xterm-16color");
      }
    }

    if (  cygwin_terminal
       || putty_terminal
       || tera_terminal
       || rxvt_terminal )
    {
      FTermcap::max_color = 16;
    }

    if ( linux_terminal && openConsole() == 0 )
    {
      if ( isConsole() )
      {
        if ( setBlinkAsIntensity(true) == 0 )
          FTermcap::max_color = 16;
        else
          FTermcap::max_color = 8;
      }

      closeConsole();
      setConsoleCursor(fc::underscore_cursor, true);
    }

    if ( linux_terminal && getFramebuffer_bpp() >= 4 )
      FTermcap::max_color = 16;

    t.c_lflag |= uInt(ICANON | ECHO);
    tcsetattr(stdin_no, TCSADRAIN, &t);
  }

  // Test if the terminal is a xterm
  if (  std::strncmp(termtype, const_cast<char*>("xterm"), 5) == 0
     || std::strncmp(termtype, const_cast<char*>("Eterm"), 4) == 0 )
  {
    xterm_terminal = true;

    // Each xterm should be able to use at least 16 colors
    if ( ! new_termtype && std::strlen(termtype) == 5 )
      new_termtype = const_cast<char*>("xterm-16color");
  }
  else
    xterm_terminal = false;

  // set the new environment variable TERM
  if ( new_termtype )
  {
    setenv(const_cast<char*>("TERM"), new_termtype, 1);
    std::strncpy (termtype, new_termtype, std::strlen(new_termtype)+1);
  }

  // Initializes variables for the current terminal
  init_termcaps();
  init_alt_charset();

  // init current locale
  locale_name = std::setlocale (LC_ALL, "");
  locale_name = std::setlocale (LC_NUMERIC, "");

  // get XTERM_LOCALE
  locale_xterm = std::getenv("XTERM_LOCALE");

  // set LC_ALL to XTERM_LOCALE
  if ( locale_xterm )
    locale_name = std::setlocale (LC_ALL, locale_xterm);

  // TeraTerm can not show UTF-8 character
  if ( tera_terminal && ! std::strcmp(nl_langinfo(CODESET), "UTF-8") )
    locale_name = std::setlocale (LC_ALL, "C");

  // try to found a meaningful content for locale_name
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
    locale_name = const_cast<char*>("C");

  // Detect environment and set encoding
  init_encoding();

#ifdef F_HAVE_LIBGPM

  // Enable the linux general purpose mouse (gpm) server
  gpm_mouse_enabled = enableGpmMouse();

#endif

  // xterm mouse support
  if ( tcap[fc::t_key_mouse].string != 0 )
  {
    mouse_support = true;
    enableXTermMouse();
  }

  // enter 'keyboard_transmit' mode
  if ( tcap[fc::t_keypad_xmit].string )
  {
    putstring (tcap[fc::t_keypad_xmit].string);
    std::fflush(stdout);
  }

  // save current cursor position
  if ( tcap[fc::t_save_cursor].string )
  {
    putstring (tcap[fc::t_save_cursor].string);
    std::fflush(stdout);
  }

  // saves the screen and the cursor position
  if ( tcap[fc::t_enter_ca_mode].string )
  {
    putstring (tcap[fc::t_enter_ca_mode].string);
    std::fflush(stdout);
  }

  // enable alternate charset
  if ( tcap[fc::t_enable_acs].string )
  {
    putstring (tcap[fc::t_enable_acs].string);
    std::fflush(stdout);
  }

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

  if ( kde_konsole )
    setKDECursor(fc::UnderlineCursor);

  if ( FTermcap::max_color >= 16
     && ! cygwin_terminal
     && ! kde_konsole
     && ! tera_terminal )
  {
    resetColorMap();
    saveColorMap();

    setPalette (fc::Black, 0x00, 0x00, 0x00);
    setPalette (fc::Blue, 0x22, 0x22, 0xb2);
    setPalette (fc::Green, 0x18, 0xb2, 0x18);
    setPalette (fc::Cyan, 0x4a, 0x4a, 0xe4);
    setPalette (fc::Red, 0xb2, 0x18, 0x18);
    setPalette (fc::Magenta, 0xb2, 0x18, 0xb2);
    setPalette (fc::Brown, 0xe8, 0x87, 0x1f);
    setPalette (fc::LightGray, 0xbc, 0xbc, 0xbc);
    setPalette (fc::DarkGray, 0x50, 0x50, 0x50);
    setPalette (fc::LightBlue, 0x80, 0xa4, 0xec);
    setPalette (fc::LightGreen, 0x54, 0xff, 0x54);
    setPalette (fc::LightCyan, 0x49, 0xc9, 0xe3);
    setPalette (fc::LightRed, 0xff, 0x54, 0x54);
    setPalette (fc::LightMagenta, 0xff, 0x54, 0xff);
    setPalette (fc::Yellow, 0xff, 0xff, 0x54);
    setPalette (fc::White, 0xff, 0xff, 0xff);
  }

  // set 200 Hz beep (100 ms)
  setBeep(200, 100);

  signal(SIGTERM,  FTerm::signal_handler); // Termination signal
  signal(SIGQUIT,  FTerm::signal_handler); // Quit from keyboard (Ctrl-\)
  signal(SIGINT,   FTerm::signal_handler); // Keyboard interrupt (Ctrl-C)
  signal(SIGABRT,  FTerm::signal_handler); // Abort signal from abort(3)
  signal(SIGILL,   FTerm::signal_handler); // Illegal Instruction
  signal(SIGSEGV,  FTerm::signal_handler); // Invalid memory reference
  signal(SIGWINCH, FTerm::signal_handler); // Window resize signal

  // turn off hardware echo
  noHardwareEcho();

  // switch to the raw mode
  setRawMode();
}

//----------------------------------------------------------------------
void FTerm::finish()
{
  // set default signal handler
  signal(SIGWINCH, SIG_DFL); // Window resize signal
  signal(SIGSEGV,  SIG_DFL); // Invalid memory reference
  signal(SIGILL,   SIG_DFL); // Illegal Instruction
  signal(SIGABRT,  SIG_DFL); // Abort signal from abort(3)
  signal(SIGINT,   SIG_DFL); // Keyboard interrupt (Ctrl-C)
  signal(SIGQUIT,  SIG_DFL); // Quit from keyboard (Ctrl-\)
  signal(SIGTERM,  SIG_DFL); // Termination signal

  if ( xterm_title && xterm_terminal && ! rxvt_terminal )
    setXTermTitle (*xterm_title);

  // restore the saved termios settings
  restoreTTYsettings();

  // turn off all attributes
  if ( tcap[fc::t_exit_attribute_mode].string )
  {
    putstring (tcap[fc::t_exit_attribute_mode].string);
    std::fflush(stdout);
  }

  // turn off pc charset mode
  if ( tcap[fc::t_exit_pc_charset_mode].string )
  {
    putstring (tcap[fc::t_exit_pc_charset_mode].string);
    std::fflush(stdout);
  }

  // reset xterm color settings to default values
  resetXTermDefaults();

  // set xterm full block cursor
  setXTermCursorStyle(fc::steady_block);

  if ( FTermcap::max_color >= 16 && ! (kde_konsole || tera_terminal) )
  {
    // reset screen settings
    setPalette (fc::Cyan, 0x18, 0xb2, 0xb2);
    setPalette (fc::LightGray, 0xb2, 0xb2, 0xb2);
    setPalette (fc::DarkGray, 0x68, 0x68, 0x68);
    setPalette (fc::LightBlue, 0x54, 0x54, 0xff);
    setPalette (fc::LightGreen, 0x54, 0xff, 0x54);

    resetXTermColors();
    resetColorMap();
  }

  if ( mintty_terminal )
  {
    // switch to normal escape key mode
    putstring (CSI "?7727l");
    std::fflush(stdout);
  }

  if ( linux_terminal )
  {
    setBlinkAsIntensity (false);
    setConsoleCursor(fc::default_cursor, false);
  }

  if ( kde_konsole )
    setKDECursor(fc::BlockCursor);

  resetBeep();

  if ( mouse_support )
    disableXTermMouse();

#ifdef F_HAVE_LIBGPM

  if ( gpm_mouse_enabled )
    disableGpmMouse();  // Disable gpm server

#endif

  // restores the screen and the cursor position
  if ( tcap[fc::t_exit_ca_mode].string )
  {
    putstring (tcap[fc::t_exit_ca_mode].string);
    std::fflush(stdout);
  }

  // restore cursor to position of last save_cursor
  if ( tcap[fc::t_restore_cursor].string )
  {
    putstring (tcap[fc::t_restore_cursor].string);
    std::fflush(stdout);
  }

  // leave 'keyboard_transmit' mode
  if ( tcap[fc::t_keypad_local].string )
  {
    putstring (tcap[fc::t_keypad_local].string);
    std::fflush(stdout);
  }

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

  if ( mouse )
    delete mouse;

  if ( term )
    delete term;

  if ( opti_attr )
    delete opti_attr;

  if ( opti_move )
    delete opti_move;
}

//----------------------------------------------------------------------
uInt FTerm::cp437_to_unicode (uChar c)
{
  register uInt ucs = uInt(c);

  for (register uInt i=0; i <= lastCP437Item; i++)
  {
    if ( cp437_to_ucs[i][0] == c ) // found
    {
      ucs = cp437_to_ucs[i][1];
      break;
    }
  }

  return ucs;
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
