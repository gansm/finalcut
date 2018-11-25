/***********************************************************************
* ftermcap.cpp - Provides access to terminal capabilities              *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2015-2018 Markus Gans                                      *
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
#include <vector>

#include "final/ftermcap.h"

namespace finalcut
{

// static class attributes
bool            FTermcap::background_color_erase   = false;
bool            FTermcap::can_change_color_palette = false;
bool            FTermcap::automatic_left_margin    = false;
bool            FTermcap::automatic_right_margin   = false;
bool            FTermcap::eat_nl_glitch            = false;
bool            FTermcap::ansi_default_color       = false;
bool            FTermcap::osc_support              = false;
bool            FTermcap::no_utf8_acs_chars        = false;
int             FTermcap::max_color                = 1;
int             FTermcap::tabstop                  = 8;
int             FTermcap::attr_without_color       = 0;
FTermData*      FTermcap::fterm_data               = 0;
FTermDetection* FTermcap::term_detection           = 0;


//----------------------------------------------------------------------
// class FTermcap
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FTermcap::FTermcap()
{ }

//----------------------------------------------------------------------
FTermcap::~FTermcap()  // destructor
{ }

/* Terminal capability data base
 * -----------------------------
 * Info under: man 5 terminfo
 *
 * Importent shell commands:
 *   captoinfo - convert all termcap descriptions into terminfo descriptions
 *   infocmp   - print out terminfo description from the current terminal
 */

// public methods of FTermcap
//----------------------------------------------------------------------
void FTermcap::setFTermData (FTermData* data)
{
  fterm_data = data;
}

//----------------------------------------------------------------------
void FTermcap::setFTermDetection (FTermDetection* td)
{
  term_detection = td;
}

//----------------------------------------------------------------------
void FTermcap::init()
{
  termcap();
}

// private methods of FTermcap
//----------------------------------------------------------------------
void FTermcap::termcap()
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

  // Open termcap file
#if defined(__sun) && defined(__SVR4)
  char* termtype = fterm_data->getTermType();
#else
  const char* termtype = fterm_data->getTermType();
#endif
  terminals.push_back(termtype);            // available terminal type

  if ( color256 )                           // 1st fallback if not found
    terminals.push_back("xterm-256color");

  terminals.push_back("xterm");             // 2nd fallback if not found
  terminals.push_back("ansi");              // 3rd fallback if not found
  terminals.push_back("vt100");             // 4th fallback if not found
  iter = terminals.begin();

  while ( iter != terminals.end() )
  {
    fterm_data->setTermType(iter->c_str());

    // Open the termcap file + load entry for termtype
    status = tgetent(term_buffer, termtype);

    if ( status == success || ! term_detection->hasTerminalDetection() )
      break;

    ++iter;
  }

  if ( std::strncmp(termtype, "ansi", 4) == 0 )
    term_detection->setAnsiTerminal (true);

  termcapError (status);
  termcapVariables (buffer);
}

//----------------------------------------------------------------------
void FTermcap::termcapError (int status)
{
  static const int no_entry = 0;
  static const int db_not_found = -1;
  static const int uninitialized = -2;

  if ( status == no_entry || status == uninitialized )
  {
    const char* termtype = fterm_data->getTermType();
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
void FTermcap::termcapVariables (char*& buffer)
{
  // Get termcap booleans
  termcapBoleans();

  // Get termcap numerics
  termcapNumerics();

  // Get termcap strings
  termcapStrings (buffer);

  // Get termcap keys
  termcapKeys (buffer);
}

//----------------------------------------------------------------------
void FTermcap::termcapBoleans()
{
  // Get termcap booleans

  // Screen erased with the background color
  background_color_erase = tgetflag(C_STR("ut"));

  // Terminal is able to redefine existing colors
  can_change_color_palette = tgetflag(C_STR("cc"));

  // t_cursor_left wraps from column 0 to last column
  automatic_left_margin = tgetflag(C_STR("bw"));

  // Terminal has auto-matic margins
  automatic_right_margin = tgetflag(C_STR("am"));

  // NewLine ignored after 80 cols
  eat_nl_glitch = tgetflag(C_STR("xn"));

  // Terminal supports ANSI set default fg and bg color
  ansi_default_color = tgetflag(C_STR("AX"));

  // Terminal supports operating system commands (OSC)
  // OSC = Esc + ']'
  osc_support = tgetflag(C_STR("XT"));

  // U8 is nonzero for terminals with no VT100 line-drawing in UTF-8 mode
  no_utf8_acs_chars = bool(tgetnum(C_STR("U8")) != 0);
}

//----------------------------------------------------------------------
void FTermcap::termcapNumerics()
{
  // Get termcap numeric

  // Maximum number of colors on screen
  max_color = std::max(max_color, tgetnum(C_STR("Co")));

  if ( max_color < 0 )
    max_color = 1;

  if ( max_color < 8 )
    fterm_data->setMonochron(true);
  else
    fterm_data->setMonochron(false);

  // Get initial spacing for hardware tab stop
  tabstop = tgetnum(C_STR("it"));

  // Get video attributes that cannot be used with colors
  attr_without_color = tgetnum(C_STR("NC"));
}

//----------------------------------------------------------------------
void FTermcap::termcapStrings (char*& buffer)
{
  // Get termcap strings

  // Read termcap output strings
  for (int i = 0; tcap[i].tname[0] != 0; i++)
    tcap[i].string = tgetstr(tcap[i].tname, &buffer);
}

//----------------------------------------------------------------------
void FTermcap::termcapKeys (char*& buffer)
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

  // VT100 key codes for the arrow and function keys
  termcapKeysVt100 (buffer);
}

//----------------------------------------------------------------------
void FTermcap::termcapKeysVt100 (char*& buffer)
{
  // Some terminals (e.g. PuTTY) send vt100 key codes for
  // the arrow and function keys.

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


// private Data Member of FTermcap - termcap capabilities
//----------------------------------------------------------------------
FTermcap::tcap_map FTermcap::tcap[] =
{
//  .------------- term string
//  |    .-------- Tcap-code
//  |    |      // variable name          -> description
//------------------------------------------------------------------------------
  { 0, "bl" },  // bell                   -> audible signal (bell) (P)
  { 0, "ec" },  // erase_chars            -> erase #1 characters (P)
  { 0, "cl" },  // clear_screen           -> clear screen and home cursor (P*)
  { 0, "cd" },  // clr_eos                -> clear to end of screen (P*)
  { 0, "ce" },  // clr_eol                -> clear to end of line (P)
  { 0, "cb" },  // clr_bol                -> Clear to beginning of line
  { 0, "ho" },  // cursor_home            -> home cursor (if no cup)
  { 0, "ll" },  // cursor_to_ll           -> last line, first column (if no cup)
  { 0, "cr" },  // carriage_return        -> carriage return (P*)
  { 0, "ta" },  // tab                    -> tab to next 8-space hardware tab stop
  { 0, "bt" },  // back_tab               -> back tab (P)
  { 0, "ip" },  // insert_padding         -> insert padding after inserted character
  { 0, "ic" },  // insert_character       -> insert character (P)
  { 0, "IC" },  // parm_ich               -> insert #1 characters (P*)
  { 0, "rp" },  // repeat_char            -> repeat char #1 #2 times (P*)
  { 0, "Ic" },  // initialize_color       -> initialize color #1 to (#2,#3,#4)
  { 0, "Ip" },  // initialize_pair        -> Initialize color pair #1 to
                //                           fg=(#2,#3,#4), bg=(#5,#6,#7)
  { 0, "AF" },  // set_a_foreground       -> Set ANSI background color to #1
  { 0, "AB" },  // set_a_background       -> Set ANSI background color to #1
  { 0, "Sf" },  // set_foreground         -> Set foreground color #1
  { 0, "Sb" },  // set_background         -> Set background color #1
  { 0, "sp" },  // set_color_pair         -> Set current color pair to #1
  { 0, "op" },  // orig_pair              -> Set default pair to original value
  { 0, "oc" },  // orig_colors            -> Set all color pairs to the original
  { 0, "NC" },  // no_color_video         -> video attributes that cannot be used
                //                           with colors
  { 0, "cm" },  // cursor_address         -> move to row #1 columns #2
  { 0, "ch" },  // column_address         -> horizontal position #1, absolute (P)
  { 0, "cv" },  // row_address            -> vertical position #1 absolute (P)
  { 0, "vs" },  // cursor_visible         -> make cursor very visible
  { 0, "vi" },  // cursor_invisible       -> make cursor invisible
  { 0, "ve" },  // cursor_normal          -> make cursor appear normal (undo vi/vs)
  { 0, "up" },  // cursor_up              -> up one line
  { 0, "do" },  // cursor_down            -> down one line
  { 0, "le" },  // cursor_left            -> move left one space
  { 0, "nd" },  // cursor_right           -> non-destructive space (move right)
  { 0, "UP" },  // parm_up_cursor         -> up #1 lines (P*)
  { 0, "DO" },  // parm_down_cursor       -> down #1 lines (P*)
  { 0, "LE" },  // parm_left_cursor       -> move #1 characters to the left (P)
  { 0, "RI" },  // parm_right_cursor      -> move #1 characters to the right (P*)
  { 0, "sc" },  // save_cursor            -> save current cursor position (P)
  { 0, "rc" },  // restore_cursor         -> restore cursor to save_cursor
  { 0, "Ss" },  // set cursor style       -> Select the DECSCUSR cursor style
  { 0, "sf" },  // scroll_forward         -> scroll text up (P)
  { 0, "sr" },  // scroll_reverse         -> scroll text down (P)
  { 0, "ti" },  // enter_ca_mode          -> string to start programs using cup
  { 0, "te" },  // exit_ca_mode           -> strings to end programs using cup
  { 0, "eA" },  // enable_acs             -> enable alternate char set
  { 0, "md" },  // enter_bold_mode        -> turn on bold (double-bright) mode
  { 0, "me" },  // exit_bold_mode         -> turn off bold mode
  { 0, "mh" },  // enter_dim_mode         -> turn on half-bright
  { 0, "me" },  // exit_dim_mode          -> turn off half-bright
  { 0, "ZH" },  // enter_italics_mode     -> Enter italic mode
  { 0, "ZR" },  // exit_italics_mode      -> End italic mode
  { 0, "us" },  // enter_underline_mode   -> begin underline mode
  { 0, "ue" },  // exit_underline_mode    -> exit underline mode
  { 0, "mb" },  // enter_blink_mode       -> turn on blinking
  { 0, "me" },  // exit_blink_mode        -> turn off blinking
  { 0, "mr" },  // enter_reverse_mode     -> turn on reverse video mode
  { 0, "me" },  // exit_reverse_mode      -> turn off reverse video mode
  { 0, "so" },  // enter_standout_mode    -> begin standout mode
  { 0, "se" },  // exit_standout_mode     -> exit standout mode
  { 0, "mk" },  // enter_secure_mode      -> turn on blank mode (characters invisible)
  { 0, "me" },  // exit_secure_mode       -> turn off blank mode (characters visible)
  { 0, "mp" },  // enter_protected_mode   -> turn on protected mode
  { 0, "me" },  // exit_protected_mode    -> turn off protected mode
  { 0, "XX" },  // enter_crossed_out_mode -> turn on mark character as deleted
  { 0, "me" },  // exit_crossed_out_mode  -> turn off mark character as deleted
  { 0, "Us" },  // enter_dbl_underline_mode -> begin double underline mode
  { 0, "Ue" },  // exit_dbl_underline_mode  -> exit double underline mode
  { 0, "sa" },  // set_attributes         -> define videoattributes #1-#9 (PG9)
  { 0, "me" },  // exit_attribute_mode    -> turn off all attributes
  { 0, "as" },  // enter_alt_charset_mode -> start alternate character set (P)
  { 0, "ae" },  // exit_alt_charset_mode  -> end alternate character set (P)
  { 0, "S2" },  // enter_pc_charset_mode  -> Enter PC character display mode
  { 0, "S3" },  // exit_pc_charset_mode   -> Exit PC character display mode
  { 0, "im" },  // enter_insert_mode      -> enter insert mode
  { 0, "ei" },  // exit_insert_mode       -> exit insert mode
  { 0, "SA" },  // enter_am_mode          -> turn on automatic margins
  { 0, "RA" },  // exit_am_mode           -> turn off automatic margins
  { 0, "ac" },  // acs_chars              -> graphics charset pairs (vt100)
  { 0, "ks" },  // keypad_xmit            -> enter 'key-board_transmit' mode
  { 0, "ke" },  // keypad_local           -> leave 'key-board_transmit' mode
  { 0, "Km" },  // key_mouse              -> Mouse event has occurred
  { 0, "\0" }
};

/*
 * (P)    indicates that padding may be specified
 * #[1-9] in the description field indicates that the string
 *        is passed through tparm with parms as given (#i).
 * (P*)   indicates that padding may vary in proportion
 *        to the number of lines affected
 * (#i)   indicates the ith parameter.
 *
 * "XX", "Us" and "Ue" are unofficial and they are only used here.
 */

}  // namespace finalcut
