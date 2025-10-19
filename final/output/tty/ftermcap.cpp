/***********************************************************************
* ftermcap.cpp - Provides access to terminal capabilities              *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2025 Markus Gans                                      *
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

#if defined(__sun) && defined(__SVR4)
  #include <termio.h>
  typedef struct termio SGTTY;
  typedef struct termios SGTTYS;

  #ifdef _LP64
    typedef unsigned int chtype;
  #else
    typedef unsigned long chtype;
  #endif  // _LP64

  #include <term.h>  // termcap
#else
  #include <term.h>  // termcap
#endif  // defined(__sun) && defined(__SVR4)

#ifdef OK
  #undef OK  // Need on NetBSD
#endif

#ifdef F_HAVE_LIBGPM
  #undef buttons  // from term.h
#endif

#include <algorithm>
#include <string>
#include <vector>

#include "final/fc.h"
#include "final/input/fkey_map.h"
#include "final/output/tty/ftermcap.h"
#include "final/output/tty/ftermdata.h"
#include "final/output/tty/ftermdetection.h"
#include "final/output/tty/fterm.h"
#include "final/util/emptyfstring.h"
#include "final/util/flog.h"
#include "final/util/fsystem.h"

namespace finalcut
{

namespace internal
{

// Constant
static constexpr std::size_t BUF_SIZE{2048};

// Function
static auto getStringBuffer() noexcept -> char*
{
  static std::array<char, BUF_SIZE> string_buf{};
  return string_buf.data();
}

}  // namespace internal

// Function prototypes
static void delDuplicateKeys();

// static class attributes
bool                    FTermcap::initialized              {false};
bool                    FTermcap::background_color_erase   {false};
bool                    FTermcap::can_change_color_palette {false};
bool                    FTermcap::automatic_left_margin    {false};
bool                    FTermcap::automatic_right_margin   {false};
bool                    FTermcap::eat_nl_glitch            {false};
bool                    FTermcap::has_ansi_escape_sequences{false};
bool                    FTermcap::ansi_default_color       {false};
bool                    FTermcap::osc_support              {false};
bool                    FTermcap::no_utf8_acs_chars        {false};
bool                    FTermcap::no_padding_char          {false};
bool                    FTermcap::xon_xoff_flow_control    {false};
int                     FTermcap::max_color                {1};
int                     FTermcap::tabstop                  {8};
int                     FTermcap::padding_baudrate         {0};
int                     FTermcap::attr_without_color       {0};
int                     FTermcap::baudrate                 {9600};
char                    FTermcap::PC                       {'\0'};
char*                   FTermcap::buffer                   {nullptr};
char**                  FTermcap::buffer_addr              {nullptr};
FTermcap::PutCharFunc   FTermcap::outc                     {};
FTermcap::PutStringFunc FTermcap::outs                     {};

//----------------------------------------------------------------------
inline auto getKeyEntry (FKey key) noexcept -> FKeyMap::KeyCapMap*
{
  auto& fkey_cap_table = FKeyMap::getKeyCapMap();
  std::size_t i{0};

  do
  {
    if ( fkey_cap_table[i].num == key )
      return &fkey_cap_table[i];

    i++;
  }
  while ( fkey_cap_table[i].num != FKey::F63 );

  return nullptr;
}

//----------------------------------------------------------------------
inline void del2ndKeyIfDuplicate ( const FKeyMap::KeyCapMap* first
                                 , FKeyMap::KeyCapMap* second ) noexcept
{
  if ( ! first || ! first->string || ! second || ! second->string )
    return;

  const auto len = std::min(first->length, second->length);

  if ( std::memcmp(first->string, second->string, len) == 0 )
  {
    second->string = nullptr;
    second->length = 0;
  }
}

//----------------------------------------------------------------------
static void delDuplicateKeys()
{
  // Fixes incorrect key detection caused by duplicate key sequences
  // (required e.g. for st - simple terminal)

  const auto* home_key = getKeyEntry(FKey::Home);
  const auto* end_key = getKeyEntry(FKey::End);
  const auto* ppage_key = getKeyEntry(FKey::Page_up);
  const auto* npage_key = getKeyEntry(FKey::Page_down);
  auto* a1_key = getKeyEntry(FKey::Upper_left);
  auto* c1_key = getKeyEntry(FKey::Lower_left);
  auto* a3_key = getKeyEntry(FKey::Upper_right);
  auto* c3_key = getKeyEntry(FKey::Lower_right);
  del2ndKeyIfDuplicate (home_key, a1_key);
  del2ndKeyIfDuplicate (end_key, c1_key);
  del2ndKeyIfDuplicate (ppage_key, a3_key);
  del2ndKeyIfDuplicate (npage_key, c3_key);
}

//----------------------------------------------------------------------
// class FTermcap
//----------------------------------------------------------------------

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
auto FTermcap::getFlag (const std::string& cap) -> bool
{
  return ::tgetflag(C_STR(cap.data())) == 1;
}

//----------------------------------------------------------------------
auto FTermcap::getNumber (const std::string& cap) -> int
{
  return ::tgetnum(C_STR(cap.data()));
}

//----------------------------------------------------------------------
auto FTermcap::getString (const std::string& cap) -> char*
{
  const auto string = ::tgetstr(C_STR(cap.data()), buffer_addr);
  return ( string && string[0] != '\0' ) ? string : nullptr;
}

//----------------------------------------------------------------------
auto FTermcap::encodeMotionParameter (const std::string& cap, int col, int row) -> std::string
{
  const auto str = ::tgoto(C_STR(cap.data()), col, row);
  return str ? str : std::string();
}

//----------------------------------------------------------------------
auto FTermcap::paddingPrint (const std::string& string, int affcnt) -> Status
{
  if ( string.empty() || ! outc )
    return Status::Error;

  bool has_delay = hasDelay(string);
  auto iter = string.cbegin();

  while ( iter != string.cend() )
  {
    if ( *iter != '$' )
    {
      outc (int(*iter));
      ++iter;
      continue;
    }

    ++iter;

    if ( iter == string.cend() || *iter != '<' )
    {
      outc (int('$'));

      if ( iter != string.cend() )
        outc (int(*iter));
      else
        break;

      ++iter;
      continue;
    }

    const int number = readNumber(iter, affcnt, has_delay);

    if ( number == -1 )
    {
      outc (int('$'));
      outc (int('<'));
      continue;
    }

    if ( has_delay && number > 0 )
      delayOutput(number / 10);

    ++iter;
  }

  return Status::OK;
}

//----------------------------------------------------------------------
auto FTermcap::stringPrint (const std::string& string) -> Status
{
  if ( string.empty() || ! outs )
    return Status::Error;

  return outs(string) >= 0 ? Status::OK : Status::Error;
}

//----------------------------------------------------------------------
void FTermcap::init()
{
  buffer = internal::getStringBuffer();
  buffer_addr = &buffer;
  termcap();
  setDefaultPutCharFunction();
  setDefaultPutStringFunction();
}

//----------------------------------------------------------------------
void FTermcap::setDefaultPutCharFunction()
{
  static const auto& fsys = FSystem::getInstance();
  auto put_char = [] (int ch) noexcept { return fsys->putchar(ch); };
  outc = put_char;
}

//----------------------------------------------------------------------
void FTermcap::setDefaultPutStringFunction()
{
  static const auto& fsys = FSystem::getInstance();
  auto put_string = \
      [] (const std::string& string) noexcept
      {
        return fsys->fputs(string.c_str(), stdout);
      };
  outs = put_string;
}


// private methods of FTermcap
//----------------------------------------------------------------------
void FTermcap::termcap()
{
  static constexpr int success = 1;
  static constexpr int uninitialized = -2;
  static std::array<char, internal::BUF_SIZE> term_buffer{};
  std::vector<std::string> terminals{};
  int status = uninitialized;
  static auto& fterm_data = FTermData::getInstance();
  static const auto& term_detection = FTermDetection::getInstance();
  const bool color256 = term_detection.canDisplay256Colors();
  baudrate = int(fterm_data.getBaudrate());

  // Open termcap file
  const auto& termtype = fterm_data.getTermType();
  terminals.emplace_back(termtype);         // available terminal type

  if ( color256 )                           // 1st fallback if not found
    terminals.emplace_back("xterm-256color");

  terminals.emplace_back("xterm");          // 2nd fallback if not found
  terminals.emplace_back("ansi");           // 3rd fallback if not found
  terminals.emplace_back("vt100");          // 4th fallback if not found
  auto iter = terminals.cbegin();

  while ( iter != terminals.cend() )
  {
    fterm_data.setTermType(*iter);

    // Open the termcap file + load entry for termtype
#if defined(__sun) && defined(__SVR4)
    status = tgetent(term_buffer.data(), const_cast<char*>(termtype.data()));
#else
    status = tgetent(term_buffer.data(), termtype.data());
#endif

    initialized = bool( status == success );

    if ( status == success || ! term_detection.hasTerminalDetection() )
      break;

    ++iter;
  }

  termcapError (status);
  termcapVariables();
}

//----------------------------------------------------------------------
void FTermcap::termcapError (int status)
{
  static constexpr int no_entry = 0;
  static constexpr int db_not_found = -1;
  static constexpr int uninitialized = -2;

  if ( status == no_entry || status == uninitialized )
  {
    const auto& termtype = FTermData::getInstance().getTermType();
    std::clog << FLog::LogLevel::Error
              << "Unknown terminal: \"" << termtype << "\". "
              << "Check the TERM environment variable. "
              << "Also make sure that the terminal "
              << "is defined in the termcap/terminfo database."
              << std::endl;
    std::abort();
  }
  else if ( status == db_not_found )
  {
    std::clog << "The termcap/terminfo database could not be found."
              << std::endl;
    std::abort();
  }
}

//----------------------------------------------------------------------
void FTermcap::termcapVariables()
{
  // Get termcap booleans
  termcapBoleans();

  // Get termcap numerics
  termcapNumerics();

  // Get termcap strings
  termcapStrings();

  // Get termcap keys
  termcapKeys();
}

//----------------------------------------------------------------------
void FTermcap::termcapBoleans()
{
  // Get termcap flags/booleans

  // Screen erased with the background color
  background_color_erase = getFlag("ut");

  // Terminal is able to redefine existing colors
  can_change_color_palette = getFlag("cc");

  // t_cursor_left wraps from column 0 to last column
  automatic_left_margin = getFlag("bw");

  // Terminal has auto-matic margins
  automatic_right_margin = getFlag("am");

  // NewLine ignored after 80 cols
  eat_nl_glitch = getFlag("xn");

  // Terminal supports ANSI set default fg and bg color
  ansi_default_color = getFlag("AX");

  // Terminal supports operating system commands (OSC)
  // OSC = Esc + ']'
  osc_support = getFlag("XT");

  // U8 is nonzero for terminals with no VT100 line-drawing in UTF-8 mode
  no_utf8_acs_chars = bool(getNumber("U8") != 0);

  // No padding character available
  no_padding_char = getFlag("NP");

  // Terminal uses software flow control (XON/XOFF) with (Ctrl-Q/Ctrl-S)
  xon_xoff_flow_control = getFlag("xo");
}

//----------------------------------------------------------------------
void FTermcap::termcapNumerics()
{
  // Get termcap numerics

  static auto& fterm_data = FTermData::getInstance();

  // Maximum number of colors on screen
  max_color = std::max(max_color, getNumber("Co"));

  if ( max_color < 0 )
    max_color = 1;

  fterm_data.setMonochron(max_color < 8);

  // Get initial spacing for hardware tab stop
  tabstop = getNumber("it");

  // Get the smallest baud rate where padding is required
  padding_baudrate = getNumber("pb");

  // Get video attributes that cannot be used with colors
  attr_without_color = getNumber("NC");
}

//----------------------------------------------------------------------
void FTermcap::termcapStrings()
{
  // Get termcap strings

  // Read termcap output strings

  for (auto&& entry : strings)
    entry.string = getString(entry.tname.data());

  const auto& ho = TCAP(t_cursor_home);

  if ( ho && std::memcmp(ho, "\033[H", 3) == 0 )
    has_ansi_escape_sequences = true;

  const auto& pc = TCAP(t_pad_char);

  if ( pc )
    PC = pc[0];
}

//----------------------------------------------------------------------
void FTermcap::termcapKeys()
{
  // Get termcap keys

  auto& cap_map = FKeyMap::getKeyCapMap();

  // Read termcap key sequences up to the self-defined values
  for (auto&& entry : cap_map)
  {
    if ( entry.string != nullptr )  // String is already set
      break;

    entry.string = getString(entry.tname.data());
    entry.length = entry.string ? uInt8(finalcut::stringLength(entry.string)) : 0;
  }

  delDuplicateKeys();

  // Sort key map list by string length (string length 0 at end)
  std::sort ( cap_map.begin(), cap_map.end()
            , [] (const auto& lhs, const auto& rhs) noexcept
              {
                if ( lhs.length == 0 && rhs.length > 0 ) return false;
                if ( lhs.length > 0 && rhs.length == 0 ) return true;
                return lhs.length < rhs.length;
              }
            );
}

//----------------------------------------------------------------------
auto FTermcap::encodeParams ( const std::string& cap
                            , const std::array<int, 9>& params ) -> std::string
{
  const auto str = ::tparm ( C_STR(cap.data()), params[0], params[1]
                           , params[2], params[3], params[4], params[5]
                           , params[6], params[7], params[8] );
  return str ? str : std::string();
}

//----------------------------------------------------------------------
inline auto FTermcap::hasDelay (const std::string& string) noexcept -> bool
{
  return (TCAP(t_bell) && string == std::string(TCAP(t_bell)))
      || (TCAP(t_flash_screen) && string == std::string(TCAP(t_flash_screen)))
      || ( ! xon_xoff_flow_control && padding_baudrate
        && (baudrate >= padding_baudrate) );
}

//----------------------------------------------------------------------
inline auto FTermcap::readNumber ( string_iterator& iter, int affcnt
                                 , bool& has_delay) noexcept -> int
{
  ++iter;
  const auto first_digit = iter;

  if ( ! std::isdigit(uChar(*iter)) && *iter != '.' )
    return -1;

  int number = 0;
  readDigits(iter, number);
  decimalPoint(iter, number);
  asteriskSlash(iter, number, affcnt, has_delay);

  if ( *iter != '>' )
  {
    iter = first_digit;
    return -1;
  }

  return number;
}

//----------------------------------------------------------------------
inline void FTermcap::readDigits (string_iterator& iter, int& number) noexcept
{
  int digit{};

  while ( (digit = uChar(*iter - '0')) < 10 && number < 1000 )
  {
    number = number * 10 + digit;
    ++iter;
  }

  number *= 10;  // Centisecond
}

//----------------------------------------------------------------------
inline void FTermcap::decimalPoint (string_iterator& iter, int& number) noexcept
{
  if ( *iter != '.' )
    return;

  ++iter;

  if ( std::isdigit(uChar(*iter)) )
  {
    number += (*iter - '0');  // Position after decimal point
    ++iter;
  }

  while ( std::isdigit(uChar(*iter)) )
    ++iter;
}

//----------------------------------------------------------------------
inline void FTermcap::asteriskSlash ( string_iterator& iter
                                    , int& number, int affcnt, bool& has_delay ) noexcept
{
  while ( *iter == '*' || *iter == '/' )
  {
    if ( *iter == '*' )
    {
      // Padding is proportional to the number of affected lines (suffix '*')
      number *= affcnt;
      ++iter;
    }
    else
    {
      // Padding is mandatory (suffix '/')
      has_delay = true;
      ++iter;
    }
  }
}


// private Data Member of FTermcap - termcap capabilities
//----------------------------------------------------------------------
FTermcap::TCapMapType FTermcap::strings =
{{
//  .------------- term string
//  |          .-------- Tcap-code
//  |          |        // variable name          -> description
//------------------------------------------------------------------------------
  { nullptr, {"bl"} },  // bell                   -> audible signal (bell) (P)
  { nullptr, {"vb"} },  // flash_screen           -> visible bell (may not move cursor)
  { nullptr, {"ec"} },  // erase_chars            -> erase #1 characters (P)
  { nullptr, {"cl"} },  // clear_screen           -> clear screen and home cursor (P*)
  { nullptr, {"cd"} },  // clr_eos                -> clear to end of screen (P*)
  { nullptr, {"ce"} },  // clr_eol                -> clear to end of line (P)
  { nullptr, {"cb"} },  // clr_bol                -> Clear to beginning of line
  { nullptr, {"ho"} },  // cursor_home            -> home cursor (if no cup)
  { nullptr, {"ll"} },  // cursor_to_ll           -> last line, first column (if no cup)
  { nullptr, {"cr"} },  // carriage_return        -> carriage return (P*)
  { nullptr, {"ta"} },  // tab                    -> tab to next 8-space hardware tab stop
  { nullptr, {"bt"} },  // back_tab               -> back tab (P)
  { nullptr, {"pc"} },  // pad_char               -> padding char (instead of null)
  { nullptr, {"ip"} },  // insert_padding         -> insert padding after inserted character
  { nullptr, {"ic"} },  // insert_character       -> insert character (P)
  { nullptr, {"IC"} },  // parm_ich               -> insert #1 characters (P*)
  { nullptr, {"rp"} },  // repeat_char            -> repeat char #1 #2 times (P*)
  { nullptr, {"lr"} },  // repeat_last_char       -> repeat last char #1 times (P*)
  { nullptr, {"Ic"} },  // initialize_color       -> initialize color #1 to (#2,#3,#4)
  { nullptr, {"Ip"} },  // initialize_pair        -> Initialize color pair #1 to
                        //                           fg=(#2,#3,#4), bg=(#5,#6,#7)
  { nullptr, {"AF"} },  // set_a_foreground       -> Set ANSI background color to #1
  { nullptr, {"AB"} },  // set_a_background       -> Set ANSI background color to #1
  { nullptr, {"Sf"} },  // set_foreground         -> Set foreground color #1
  { nullptr, {"Sb"} },  // set_background         -> Set background color #1
  { nullptr, {"sp"} },  // set_color_pair         -> Set current color pair to #1
  { nullptr, {"op"} },  // orig_pair              -> Set default pair to original value
  { nullptr, {"oc"} },  // orig_colors            -> Set all color pairs to the original
  { nullptr, {"NC"} },  // no_color_video         -> video attributes that cannot be used
                        //                           with colors
  { nullptr, {"cm"} },  // cursor_address         -> move to row #1 columns #2
  { nullptr, {"ch"} },  // column_address         -> horizontal position #1, absolute (P)
  { nullptr, {"cv"} },  // row_address            -> vertical position #1 absolute (P)
  { nullptr, {"vs"} },  // cursor_visible         -> make cursor very visible
  { nullptr, {"vi"} },  // cursor_invisible       -> make cursor invisible
  { nullptr, {"ve"} },  // cursor_normal          -> make cursor appear normal (undo vi/vs)
  { nullptr, {"up"} },  // cursor_up              -> up one line
  { nullptr, {"do"} },  // cursor_down            -> down one line
  { nullptr, {"le"} },  // cursor_left            -> move left one space
  { nullptr, {"nd"} },  // cursor_right           -> non-destructive space (move right)
  { nullptr, {"UP"} },  // parm_up_cursor         -> up #1 lines (P*)
  { nullptr, {"DO"} },  // parm_down_cursor       -> down #1 lines (P*)
  { nullptr, {"LE"} },  // parm_left_cursor       -> move #1 characters to the left (P)
  { nullptr, {"RI"} },  // parm_right_cursor      -> move #1 characters to the right (P*)
  { nullptr, {"sc"} },  // save_cursor            -> save current cursor position (P)
  { nullptr, {"rc"} },  // restore_cursor         -> restore cursor to save_cursor
  { nullptr, {"Ss"} },  // set cursor style       -> Select the DECSCUSR cursor style
  { nullptr, {"sf"} },  // scroll_forward         -> scroll text up (P)
  { nullptr, {"sr"} },  // scroll_reverse         -> scroll text down (P)
  { nullptr, {"ti"} },  // enter_ca_mode          -> string to start programs using cup
  { nullptr, {"te"} },  // exit_ca_mode           -> strings to end programs using cup
  { nullptr, {"eA"} },  // enable_acs             -> enable alternate char set
  { nullptr, {"md"} },  // enter_bold_mode        -> turn on bold (double-bright) mode
  { nullptr, {"me"} },  // exit_bold_mode         -> turn off bold mode
  { nullptr, {"mh"} },  // enter_dim_mode         -> turn on half-bright
  { nullptr, {"me"} },  // exit_dim_mode          -> turn off half-bright
  { nullptr, {"ZH"} },  // enter_italics_mode     -> Enter italic mode
  { nullptr, {"ZR"} },  // exit_italics_mode      -> End italic mode
  { nullptr, {"us"} },  // enter_underline_mode   -> begin underline mode
  { nullptr, {"ue"} },  // exit_underline_mode    -> exit underline mode
  { nullptr, {"mb"} },  // enter_blink_mode       -> turn on blinking
  { nullptr, {"me"} },  // exit_blink_mode        -> turn off blinking
  { nullptr, {"mr"} },  // enter_reverse_mode     -> turn on reverse video mode
  { nullptr, {"me"} },  // exit_reverse_mode      -> turn off reverse video mode
  { nullptr, {"so"} },  // enter_standout_mode    -> begin standout mode
  { nullptr, {"se"} },  // exit_standout_mode     -> exit standout mode
  { nullptr, {"mk"} },  // enter_secure_mode      -> turn on blank mode (characters invisible)
  { nullptr, {"me"} },  // exit_secure_mode       -> turn off blank mode (characters visible)
  { nullptr, {"mp"} },  // enter_protected_mode   -> turn on protected mode
  { nullptr, {"me"} },  // exit_protected_mode    -> turn off protected mode
  { nullptr, {"XX"} },  // enter_crossed_out_mode -> turn on mark character as deleted
  { nullptr, {"me"} },  // exit_crossed_out_mode  -> turn off mark character as deleted
  { nullptr, {"Us"} },  // enter_dbl_underline_mode -> begin double underline mode
  { nullptr, {"Ue"} },  // exit_dbl_underline_mode  -> exit double underline mode
  { nullptr, {"sa"} },  // set_attributes         -> define videoattributes #1-#9 (PG9)
  { nullptr, {"me"} },  // exit_attribute_mode    -> turn off all attributes
  { nullptr, {"as"} },  // enter_alt_charset_mode -> start alternate character set (P)
  { nullptr, {"ae"} },  // exit_alt_charset_mode  -> end alternate character set (P)
  { nullptr, {"S2"} },  // enter_pc_charset_mode  -> Enter PC character display mode
  { nullptr, {"S3"} },  // exit_pc_charset_mode   -> Exit PC character display mode
  { nullptr, {"im"} },  // enter_insert_mode      -> enter insert mode
  { nullptr, {"ei"} },  // exit_insert_mode       -> exit insert mode
  { nullptr, {"SA"} },  // enter_am_mode          -> turn on automatic margins
  { nullptr, {"RA"} },  // exit_am_mode           -> turn off automatic margins
  { nullptr, {"ac"} },  // acs_chars              -> graphics charset pairs (vt100)
  { nullptr, {"ks"} },  // keypad_xmit            -> enter 'key-board_transmit' mode
  { nullptr, {"ke"} },  // keypad_local           -> leave 'key-board_transmit' mode
  { nullptr, {"Km"} }   // key_mouse              -> Mouse event has occurred
}};

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
