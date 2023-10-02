/***********************************************************************
* ftermcapquirks.cpp - Termcap quirks for some well-known terminals    *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2023 Markus Gans                                      *
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

#include <cstring>
#include <string>
#include <utility>
#include <vector>

#include "final/fc.h"
#include "final/input/fkey_map.h"
#include "final/output/tty/ftermcap.h"
#include "final/output/tty/ftermcapquirks.h"
#include "final/output/tty/ftermdata.h"
#include "final/output/tty/fterm.h"

namespace finalcut
{

//----------------------------------------------------------------------
template <typename CapabilityT, typename StringT>
constexpr void setTCapString (CapabilityT& cap, StringT&& string)
{
  cap = std::forward<StringT>(string);
}

//----------------------------------------------------------------------
template <typename CapabilityT, typename StringT>
constexpr void setTCapStringIfNotSet (CapabilityT& cap, StringT&& string)
{
  if ( ! cap )
    cap = std::forward<StringT>(string);
}

//----------------------------------------------------------------------
// class FTermcapQuirks
//----------------------------------------------------------------------

// public methods of FTermcapQuirks
//----------------------------------------------------------------------
void FTermcapQuirks::terminalFixup()
{
  static const auto& fterm_data = FTermData::getInstance();

  if ( fterm_data.isTermType(FTermType::cygwin) )
  {
    cygwin();
  }
  else if ( fterm_data.isTermType(FTermType::linux_con) )
  {
    linux();
  }
  else if ( fterm_data.isTermType(FTermType::rxvt) )
  {
    rxvt();
  }
  else if ( fterm_data.isTermType(FTermType::gnome_terminal) )
  {
    vte();
  }
  else if ( fterm_data.isTermType(FTermType::kitty) )
  {
    kitty();
  }
  else if ( fterm_data.isTermType(FTermType::tera_term) )
  {
    teraterm();
  }
  else if ( fterm_data.isTermType(FTermType::sun_con) )
  {
    sunConsole();
  }
  else if ( fterm_data.isTermType(FTermType::putty) )
  {
    putty();
  }
  else if ( fterm_data.isTermType(FTermType::screen) )
  {
    screen();
  }
#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  else if ( fterm_data.isTermType(FTermType::freebsd_con) )
  {
    freebsd();
  }
#endif  // defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)

  // xterm and compatible terminals
  if ( fterm_data.isTermType(FTermType::xterm)
    && ! fterm_data.isTermType(FTermType::putty) )
    xterm();

  // Fixes general quirks
  general();
  // ECMA-48 (ANSI X3.64) compatible terminal
  ecma48();
}


// private methods of FTermcapQuirks
//----------------------------------------------------------------------
#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
void FTermcapQuirks::freebsd()
{
  // FreeBSD console fixes

  setTCapString ( TCAP(t_acs_chars),
      "-\036.\0370\333"
      "a\260f\370g\361"
      "h\261j\331k\277"
      "l\332m\300n\305"
      "q\304t\303u\264"
      "v\301w\302x\263"
      "y\363z\362~\371" );

  setTCapString ( TCAP(t_set_attributes),
      CSI "0"
          "%?%p1%p6%|%t;1%;"
          "%?%p2%t;4%;"
          "%?%p1%p3%|%t;7%;"
          "%?%p4%t;5%;m"
          "%?%p9%t\016%e\017%;" );

  FTermcap::attr_without_color = 18;
}
#endif  // defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)

//----------------------------------------------------------------------
void FTermcapQuirks::cygwin()
{
  // Set invisible cursor for cygwin terminal
  setTCapStringIfNotSet (TCAP(t_cursor_invisible), CSI "?25l");

  // Set visible cursor for cygwin terminal
  setTCapStringIfNotSet (TCAP(t_cursor_visible), CSI "?25h");

  // Set ansi blink for cygwin terminal
  setTCapStringIfNotSet (TCAP(t_enter_blink_mode), CSI "5m");

  // Set enable alternate character set for cygwin terminal
  setTCapStringIfNotSet (TCAP(t_enable_acs), ESC "(B" ESC ")0");

  // Set background color erase for cygwin terminal
  FTermcap::background_color_erase = true;

  // Include the Linux console quirks
  linux();
}

//----------------------------------------------------------------------
void FTermcapQuirks::linux()
{
  /* Same settings are used by cygwin */

  // Set ansi foreground and background color
  if ( FTerm::getMaxColor() > 8 )
  {
    setTCapString ( TCAP(t_set_a_foreground),
        CSI "3%p1%{8}%m%d%?%p1%{7}%>%t;1%e;22%;m" );
    setTCapString ( TCAP(t_set_a_background),
        CSI "4%p1%{8}%m%d%?%p1%{7}%>%t;5%e;25%;m" );
    // Avoid underline, blink, dim mode and reverse
    FTermcap::attr_without_color = 30;
  }
  else
  {
    setTCapString (TCAP(t_set_a_foreground), CSI "3%p1%dm");
    setTCapString (TCAP(t_set_a_background), CSI "4%p1%dm");
    // Avoid underline and dim mode
    FTermcap::attr_without_color = 18;
  }

  // Set select graphic rendition attributes
  setTCapString ( TCAP(t_set_attributes),
      CSI "0"
          "%?%p6%t;1%;"
          "%?%p1%p3%|%t;7%;"
          "%?%p4%t;5%;m"
          "%?%p9%t\016%e\017%;" );

  setTCapString (TCAP(t_enter_alt_charset_mode), SO);
  setTCapString (TCAP(t_exit_alt_charset_mode), SI);
  setTCapString (TCAP(t_exit_attribute_mode), CSI "0m" SI);
  setTCapString (TCAP(t_exit_bold_mode), CSI "22m");
  setTCapString (TCAP(t_exit_blink_mode), CSI "25m");
  setTCapString (TCAP(t_exit_reverse_mode), CSI "27m");
  setTCapString (TCAP(t_exit_secure_mode), nullptr);
  setTCapString (TCAP(t_exit_protected_mode), nullptr);
  setTCapString (TCAP(t_exit_crossed_out_mode), nullptr);
  setTCapString (TCAP(t_orig_pair), CSI "39;49;25m");

  // Avoid underline and dim mode
  setTCapString (TCAP(t_enter_dim_mode), nullptr);
  setTCapString (TCAP(t_exit_dim_mode), nullptr);
  setTCapString (TCAP(t_enter_underline_mode), nullptr);
  setTCapString (TCAP(t_exit_underline_mode), nullptr);
}

//----------------------------------------------------------------------
void FTermcapQuirks::xterm()
{
  // Fallback if "Ic" is not found
  if ( ! TCAP(t_initialize_color) )
  {
    FTermcap::can_change_color_palette = true;
    setTCapString ( TCAP(t_initialize_color),
        OSC "4;%p1%d;rgb:"
            "%p2%{255}%*%{1000}%/%2.2X/"
            "%p3%{255}%*%{1000}%/%2.2X/"
            "%p4%{255}%*%{1000}%/%2.2X" ESC "\\");
  }

  // Fallback if "vi" is not found
  setTCapStringIfNotSet (TCAP(t_cursor_invisible), CSI "?25l");

  // Fallback if "ve" is not found
  setTCapStringIfNotSet (TCAP(t_cursor_normal), CSI "?12l" CSI "?25h");
}

//----------------------------------------------------------------------
void FTermcapQuirks::rxvt()
{
  // Set enter/exit alternative charset mode for rxvt terminal
  const auto& termtype = FTermData::getInstance().getTermType();

  if ( termtype.substr(0, 12) == "rxvt-16color" )
  {
    setTCapString (TCAP(t_enter_alt_charset_mode), ESC "(0");
    setTCapString (TCAP(t_exit_alt_charset_mode), ESC "(B");
  }

  // Set ansi foreground and background color
  if ( ! FTermData::getInstance().isTermType(FTermType::urxvt) )
  {
    setTCapString ( TCAP(t_set_a_foreground),
        CSI "%?%p1%{8}%<%t%p1%{30}%+%e%p1%'R'%+%;%dm");
    setTCapString ( TCAP(t_set_a_background),
        CSI "%?%p1%{8}%<%t%p1%'('%+%e%p1%{92}%+%;%dm");
  }
}

//----------------------------------------------------------------------
void FTermcapQuirks::vte()
{
  static const auto& fterm_data = FTermData::getInstance();

  // gnome-terminal has NC=16 however, it can use the dim attribute
  FTermcap::attr_without_color = 0;

  // set exit underline for gnome terminal
  setTCapString (TCAP(t_exit_underline_mode), CSI "24m");

  if ( fterm_data.getGnomeTerminalID() >= 5300 )  // vte >= 0.53.0
    caModeExtension();
}

//----------------------------------------------------------------------
void FTermcapQuirks::kitty()
{
  caModeExtension();
}

//----------------------------------------------------------------------
void FTermcapQuirks::putty()
{
  FTermcap::background_color_erase = true;
  FTermcap::osc_support = true;

  // PuTTY has NC=22 however, it can show underline and reverse
  // and since version 0.71 is the dim attribute is also supported
  FTermcap::attr_without_color = 0;

  // Set ansi foreground and background color
  setTCapString ( TCAP(t_set_a_foreground),
      CSI "%?%p1%{8}%<"
          "%t3%p1%d"
          "%e%p1%{16}%<"
          "%t9%p1%{8}%-%d"
          "%e38;5;%p1%d%;m" );

  setTCapString ( TCAP(t_set_a_background),
      CSI "%?%p1%{8}%<"
          "%t4%p1%d"
          "%e%p1%{16}%<"
          "%t10%p1%{8}%-%d"
          "%e48;5;%p1%d%;m" );

  setTCapString ( TCAP(t_set_attributes),
      CSI "0"
          "%?%p1%p6%|%t;1%;"
          "%?%p5%t;2%;"  // since putty 0.71
          "%?%p2%t;4%;"
          "%?%p1%p3%|%t;7%;"
          "%?%p4%t;5%;m"
          "%?%p9%t\016%e\017%;" );
  // PuTTY 0.71 or higher
  setTCapString (TCAP(t_enter_dim_mode), CSI "2m");

  // PuTTY 0.71 or higher
  setTCapString (TCAP(t_exit_dim_mode), CSI "22m");
  setTCapStringIfNotSet (TCAP(t_clr_bol), CSI "1K");
  setTCapStringIfNotSet (TCAP(t_orig_pair), CSI "39;49m");
  setTCapStringIfNotSet (TCAP(t_orig_colors), OSC "R");
  setTCapStringIfNotSet (TCAP(t_column_address), CSI "%i%p1%dG");
  setTCapStringIfNotSet (TCAP(t_row_address), CSI "%i%p1%dd");
  setTCapStringIfNotSet (TCAP(t_enable_acs), ESC "(B" ESC ")0");
  setTCapStringIfNotSet (TCAP(t_enter_am_mode), CSI "?7h");
  setTCapStringIfNotSet (TCAP(t_exit_am_mode), CSI "?7l");
  setTCapStringIfNotSet (TCAP(t_enter_pc_charset_mode), CSI "11m");
  setTCapStringIfNotSet (TCAP(t_exit_pc_charset_mode), CSI "10m");
  setTCapStringIfNotSet (TCAP(t_keypad_xmit), CSI "?1h" ESC "=");
  setTCapStringIfNotSet (TCAP(t_keypad_local), CSI "?1l" ESC ">");
  setTCapStringIfNotSet (TCAP(t_key_mouse), CSI "M");
}

//----------------------------------------------------------------------
void FTermcapQuirks::teraterm()
{
  // Tera Term eat_nl_glitch fix
  FTermcap::eat_nl_glitch = true;

  // Tera Term color settings
  setTCapString (TCAP(t_set_a_foreground), CSI "38;5;%p1%dm");
  setTCapString (TCAP(t_set_a_background), CSI "48;5;%p1%dm");
  setTCapString (TCAP(t_exit_attribute_mode), CSI "0m" SI);
  setTCapString (TCAP(t_orig_pair), CSI "39;49m");
}

//----------------------------------------------------------------------
void FTermcapQuirks::sunConsole()
{
  // Sun Microsystems workstation console eat_nl_glitch fix
  FTermcap::eat_nl_glitch = true;

  // Sun Microsystems workstation console parameter cursor control
  setTCapString (TCAP(t_parm_up_cursor), CSI "%p1%dA");
  setTCapString (TCAP(t_parm_down_cursor), CSI "%p1%dB");
  setTCapString (TCAP(t_parm_right_cursor), CSI "%p1%dC");
  setTCapString (TCAP(t_parm_left_cursor), CSI "%p1%dD");
  auto& fkey_cap_table = FKeyMap::getKeyCapMap();

  // Sun Microsystems workstation console keys
  std::vector<std::pair<std::string, std::string>> sun_console_keys = \
  {
    {"K2", CSI "218z"},  // center of keypad
    {"kb", "\b"},  // backspace key
    {"kD", "\177"},  // delete-character key
    {"@7", CSI "220z"},  // end key
    {"k;", CSI "233z"},  // F10 function key
    {"F1", CSI "234z"},  // F11 function key
    {"F2", CSI "235z"},  // F12 function key
    {"kh", CSI "214z"},  // home key
    {"kI", CSI "247z"},  // insert-character key
    {"kN", CSI "222z"},  // next-page key
    {"%7", CSI "194z"},  // options key
    {"kP", CSI "216z"},  // prev-page key
    {"&5", CSI "193z"},  // resume key
    {"&8", CSI "195z"},  // undo key
    {"K2", CSI "218z"},  // center of keypad
    {"kDx", CSI "249z"},  // keypad delete
    {"@8x", CSI "250z"},  // enter/send key
    {"KP1", CSI "212z"},  // keypad slash
    {"KP2", CSI "213z"},  // keypad asterisk
    {"KP3", CSI "254z"},  // keypad minus sign
    {"KP4", CSI "253z"},  // keypad plus sign
  };

  for (std::size_t i{0}; fkey_cap_table[i].tname[0] != 0; i++)
  {
    for (const auto& key : sun_console_keys)
    {
      const std::string& tname = key.first;
      const std::string& string = key.second;

      if ( std::memcmp(fkey_cap_table[i].tname.data(), tname.c_str(), tname.size()) == 0
        && stringLength(fkey_cap_table[i].tname.data()) == tname.size() )
      {
        fkey_cap_table[i].string = string.c_str();
      }
    }
  }
}

//----------------------------------------------------------------------
void FTermcapQuirks::screen()
{
  // Fallback if "Ic" is not found
  if ( ! TCAP(t_initialize_color) )
  {
    FTermcap::can_change_color_palette = true;

    if ( FTermData::getInstance().isTermType(FTermType::tmux) )
    {
      setTCapString ( TCAP(t_initialize_color),
          ESC "Ptmux;" ESC OSC "4;%p1%d;rgb:"
          "%p2%{255}%*%{1000}%/%2.2X/"
          "%p3%{255}%*%{1000}%/%2.2X/"
          "%p4%{255}%*%{1000}%/%2.2X" BEL ESC "\\" );
    }
    else
    {
      setTCapString ( TCAP(t_initialize_color),
          ESC "P" OSC "4;%p1%d;rgb:"
          "%p2%{255}%*%{1000}%/%2.2X/"
          "%p3%{255}%*%{1000}%/%2.2X/"
          "%p4%{255}%*%{1000}%/%2.2X" BEL ESC "\\" );
    }
  }
}

//----------------------------------------------------------------------
void FTermcapQuirks::general()
{
  static constexpr int not_available = -1;

  if ( FTermcap::tabstop == not_available )
    FTermcap::tabstop = 8;

  if ( FTermcap::attr_without_color == not_available )
    FTermcap::attr_without_color = 0;

  // Fallback if "AF" is not found
  setTCapStringIfNotSet (TCAP(t_set_a_foreground), CSI "3%p1%dm");

  // Fallback if "AB" is not found
  setTCapStringIfNotSet (TCAP(t_set_a_background), CSI "4%p1%dm");

  // Fallback if "Ic" is not found
  if ( ! TCAP(t_initialize_color) )
  {
    FTermcap::can_change_color_palette = true;
    setTCapString ( TCAP(t_initialize_color),
        OSC "P%p1%x"
            "%p2%{255}%*%{1000}%/%02x"
            "%p3%{255}%*%{1000}%/%02x"
            "%p4%{255}%*%{1000}%/%02x" );
  }

  // Fallback if "ti" is not found
  setTCapStringIfNotSet (TCAP(t_enter_ca_mode), ESC "7" CSI "?47h");

  // Fallback if "te" is not found
  setTCapStringIfNotSet (TCAP(t_exit_ca_mode), CSI "?47l" ESC "8" CSI "m");

  // Set ansi move if "cm" is not found
  setTCapStringIfNotSet (TCAP(t_cursor_address), CSI "%i%p1%d;%p2%dH");
}

//----------------------------------------------------------------------
inline void FTermcapQuirks::caModeExtension()
{
  if ( TCAP(t_enter_ca_mode)
    && ! std::strstr(TCAP(t_enter_ca_mode), "\033[22;0;0t") )
  {
    // Save the cursor position, enter alternate screen buffer
    // and save xterm icon and window title on stack
    setTCapString (TCAP(t_enter_ca_mode), CSI "?1049h" CSI "22;0;0t");
  }

  if ( TCAP(t_exit_ca_mode)
    && ! std::strstr(TCAP(t_exit_ca_mode), "\033[23;0;0t") )
  {
    // Use normal screen buffer, restore the cursor position
    // and restore xterm icon and window title from stack
    setTCapString (TCAP(t_exit_ca_mode), CSI "?1049l" CSI "23;0;0t");
  }
}

//----------------------------------------------------------------------
void FTermcapQuirks::ecma48()
{
  // Test for standard ECMA-48 (ANSI X3.64) terminal
  if ( ! TCAP(t_exit_underline_mode)
    || std::memcmp(TCAP(t_exit_underline_mode), CSI "24m", 5) != 0 )
    return;

  // Seems to be a ECMA-48 (ANSI X3.64) compatible terminal
  setTCapString (TCAP(t_enter_dbl_underline_mode), CSI "21m");  // Leaves single underlined too
  setTCapString (TCAP(t_exit_dbl_underline_mode), CSI "24m");
  setTCapString (TCAP(t_exit_bold_mode), CSI "22m");  // Exit dim, too
  setTCapString (TCAP(t_exit_dim_mode), CSI "22m");
  setTCapString (TCAP(t_exit_underline_mode), CSI "24m");
  setTCapString (TCAP(t_exit_blink_mode), CSI "25m");
  setTCapString (TCAP(t_exit_reverse_mode), CSI "27m");
  setTCapString (TCAP(t_exit_secure_mode), CSI "28m");
  setTCapString (TCAP(t_enter_crossed_out_mode), CSI "9m");
  setTCapString (TCAP(t_exit_crossed_out_mode), CSI "29m");
}

}  // namespace finalcut
