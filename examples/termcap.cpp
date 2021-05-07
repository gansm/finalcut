/***********************************************************************
* termcap.cpp - Show the used termcap variables                        *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2017-2021 Markus Gans                                      *
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

#include <array>
#include <iomanip>
#include <iostream>
#include <string>

#include <final/final.h>

using finalcut::Termcap;

// Function prototype
void tcapBoolean (const std::string&, bool);
void tcapNumeric (const std::string&, int);
void tcapString (const std::string&, const char[]);
void debug (const finalcut::FApplication&);
void booleans();
void numeric();
void string();


//----------------------------------------------------------------------
// struct data
//----------------------------------------------------------------------

struct Data
{
  struct alignas(alignof(std::string)) TermcapString
  {
    const std::string name;
    const Termcap cap;
  };

  static std::array<TermcapString, 85> strings;
};

//----------------------------------------------------------------------
// struct data - string data array
//----------------------------------------------------------------------
std::array<Data::TermcapString, 85> Data::strings =
{{
  { "t_bell", Termcap::t_bell },
  { "t_flash_screen", Termcap::t_flash_screen },
  { "t_erase_chars", Termcap::t_erase_chars },
  { "t_clear_screen", Termcap::t_clear_screen },
  { "t_clr_eos", Termcap::t_clr_eos },
  { "t_clr_eol", Termcap::t_clr_eol },
  { "t_clr_bol", Termcap::t_clr_bol },
  { "t_cursor_home", Termcap::t_cursor_home },
  { "t_cursor_to_ll", Termcap::t_cursor_to_ll },
  { "t_carriage_return", Termcap::t_carriage_return },
  { "t_tab", Termcap::t_tab },
  { "t_back_tab", Termcap::t_back_tab },
  { "t_pad_char", Termcap::t_pad_char },
  { "t_insert_padding", Termcap::t_insert_padding },
  { "t_insert_character", Termcap::t_insert_character },
  { "t_parm_ich", Termcap::t_parm_ich },
  { "t_repeat_char", Termcap::t_repeat_char },
  { "t_initialize_color", Termcap::t_initialize_color },
  { "t_initialize_pair", Termcap::t_initialize_pair },
  { "t_set_a_foreground", Termcap::t_set_a_foreground },
  { "t_set_a_background", Termcap::t_set_a_background },
  { "t_set_foreground", Termcap::t_set_foreground },
  { "t_set_background", Termcap::t_set_background },
  { "t_set_color_pair", Termcap::t_set_color_pair },
  { "t_orig_pair", Termcap::t_orig_pair },
  { "t_orig_colors", Termcap::t_orig_colors },
  { "t_no_color_video", Termcap::t_no_color_video },
  { "t_cursor_address", Termcap::t_cursor_address },
  { "t_column_address", Termcap::t_column_address },
  { "t_row_address", Termcap::t_row_address },
  { "t_cursor_visible", Termcap::t_cursor_visible },
  { "t_cursor_invisible", Termcap::t_cursor_invisible },
  { "t_cursor_normal", Termcap::t_cursor_normal },
  { "t_cursor_up", Termcap::t_cursor_up },
  { "t_cursor_down", Termcap::t_cursor_down },
  { "t_cursor_left", Termcap::t_cursor_left },
  { "t_cursor_right", Termcap::t_cursor_right },
  { "t_parm_up_cursor", Termcap::t_parm_up_cursor },
  { "t_parm_down_cursor", Termcap::t_parm_down_cursor },
  { "t_parm_left_cursor", Termcap::t_parm_left_cursor },
  { "t_parm_right_cursor", Termcap::t_parm_right_cursor },
  { "t_save_cursor", Termcap::t_save_cursor },
  { "t_restore_cursor", Termcap::t_restore_cursor },
  { "t_cursor_style", Termcap::t_cursor_style },
  { "t_scroll_forward", Termcap::t_scroll_forward },
  { "t_scroll_reverse", Termcap::t_scroll_reverse },
  { "t_enter_ca_mode", Termcap::t_enter_ca_mode },
  { "t_exit_ca_mode", Termcap::t_exit_ca_mode },
  { "t_enable_acs", Termcap::t_enable_acs },
  { "t_enter_bold_mode", Termcap::t_enter_bold_mode },
  { "t_exit_bold_mode", Termcap::t_exit_bold_mode },
  { "t_enter_dim_mode", Termcap::t_enter_dim_mode },
  { "t_exit_dim_mode", Termcap::t_exit_dim_mode },
  { "t_enter_italics_mode", Termcap::t_enter_italics_mode },
  { "t_exit_italics_mode", Termcap::t_exit_italics_mode },
  { "t_enter_underline_mode", Termcap::t_enter_underline_mode },
  { "t_exit_underline_mode", Termcap::t_exit_underline_mode },
  { "t_enter_blink_mode", Termcap::t_enter_blink_mode },
  { "t_exit_blink_mode", Termcap::t_exit_blink_mode },
  { "t_enter_reverse_mode", Termcap::t_enter_reverse_mode },
  { "t_exit_reverse_mode", Termcap::t_exit_reverse_mode },
  { "t_enter_standout_mode", Termcap::t_enter_standout_mode },
  { "t_exit_standout_mode", Termcap::t_exit_standout_mode },
  { "t_enter_secure_mode", Termcap::t_enter_secure_mode },
  { "t_exit_secure_mode", Termcap::t_exit_secure_mode },
  { "t_enter_protected_mode", Termcap::t_enter_protected_mode },
  { "t_exit_protected_mode", Termcap::t_exit_protected_mode },
  { "t_enter_crossed_out_mode", Termcap::t_enter_crossed_out_mode },
  { "t_exit_crossed_out_mode", Termcap::t_exit_crossed_out_mode },
  { "t_enter_dbl_underline_mode", Termcap::t_enter_dbl_underline_mode },
  { "t_exit_dbl_underline_mode", Termcap::t_exit_dbl_underline_mode },
  { "t_set_attributes", Termcap::t_set_attributes },
  { "t_exit_attribute_mode", Termcap::t_exit_attribute_mode },
  { "t_enter_alt_charset_mode", Termcap::t_enter_alt_charset_mode },
  { "t_exit_alt_charset_mode", Termcap::t_exit_alt_charset_mode },
  { "t_enter_pc_charset_mode", Termcap::t_enter_pc_charset_mode },
  { "t_exit_pc_charset_mode", Termcap::t_exit_pc_charset_mode },
  { "t_enter_insert_mode", Termcap::t_enter_insert_mode },
  { "t_exit_insert_mode", Termcap::t_exit_insert_mode },
  { "t_enter_am_mode", Termcap::t_enter_am_mode },
  { "t_exit_am_mode", Termcap::t_exit_am_mode },
  { "t_acs_chars", Termcap::t_acs_chars },
  { "t_keypad_xmit", Termcap::t_keypad_xmit },
  { "t_keypad_local", Termcap::t_keypad_local },
  { "t_key_mouse", Termcap::t_key_mouse }
}};


//----------------------------------------------------------------------
// Functions
//----------------------------------------------------------------------
void tcapBoolean (const std::string& name, bool cap_bool)
{
  std::cout << "FTermcap::" << name << ": ";

  if ( cap_bool )
    std::cout << "true\r\n";
  else
    std::cout << "false\r\n";
}

//----------------------------------------------------------------------
void tcapNumeric (const std::string& name, int cap_num)
{
  std::cout << "FTermcap::" << name << ": " << cap_num << "\r\n";
}

//----------------------------------------------------------------------
void tcapString (const std::string& name, const char cap_str[])
{
  std::string sequence{};
  std::cout << name << ": ";

  if ( cap_str == nullptr )
  {
    std::cout << "\r\n";
    return;
  }

  const auto len = uInt(std::strlen(cap_str));

  for (uInt i{0}; i < len; i++)
  {
    const auto c = uChar(cap_str[i]);

    if ( c > 127 )
    {
      std::ostringstream o;
      o << std::oct << int(c);
      sequence += "\\";
      sequence += o.str();
    }
    else if ( c < 32 )
    {
      if ( c == 27 )
        sequence += "\\E";
      else
      {
        sequence += '^';
        sequence += char(c + 64);
      }
    }
    else
      sequence += char(c);
  }

  std::cout << sequence << " ";
  std::cout << "\r\n";
}

//----------------------------------------------------------------------
#if DEBUG
void debug (const finalcut::FApplication& TermApp)
{
  const auto& fterm = TermApp.getFTerm();
  auto& debug_data = fterm.getFTermDebugData();
  const auto& ab_s = debug_data.getAnswerbackString();
  const auto& sec_da = debug_data.getSecDAString();
  std::cout << "\n.------------------- debug -------------------\r\n";

#if defined(__linux__)
  std::cout << "|               Framebuffer bpp: "
            << debug_data.getFramebufferBpp() << "\r\n";
#endif

  std::cout << "| after init_256colorTerminal(): "
            << debug_data.getTermType_256color() << "\r\n";
  std::cout << "|    after parseAnswerbackMsg(): "
            << debug_data.getTermType_Answerback() << "\r\n";
  std::cout << "|            after parseSecDA(): "
            << debug_data.getTermType_SecDA() << "\r\n";

  if ( ! ab_s.isEmpty() )
    tcapString ("|         The answerback String", ab_s.c_str());

  if ( ! sec_da.isEmpty() )
    tcapString ("|              The SecDA String", sec_da.c_str());

  std::cout << "`------------------- debug -------------------\r\n";
}
#else
void debug (const finalcut::FApplication&)
{
  // FINAL CUT was compiled without debug option
}
#endif

//----------------------------------------------------------------------
void booleans()
{
  std::cout << "\r\n[Booleans]\r\n";
  tcapBoolean ( "background_color_erase"
              , finalcut::FTermcap::background_color_erase );
  tcapBoolean ( "can_change_color_palette"
              , finalcut::FTermcap::can_change_color_palette );
  tcapBoolean ( "automatic_left_margin"
              , finalcut::FTermcap::automatic_left_margin );
  tcapBoolean ( "automatic_right_margin"
              , finalcut::FTermcap::automatic_right_margin );
  tcapBoolean ( "eat_nl_glitch"
              , finalcut::FTermcap::eat_nl_glitch );
  tcapBoolean ( "has_ansi_escape_sequences"
              , finalcut::FTermcap::has_ansi_escape_sequences );
  tcapBoolean ( "ansi_default_color"
              , finalcut::FTermcap::ansi_default_color );
  tcapBoolean ( "osc_support"
              , finalcut::FTermcap::osc_support );
  tcapBoolean ( "no_utf8_acs_chars"
              , finalcut::FTermcap::no_utf8_acs_chars );
  tcapBoolean ( "no_padding_char"
              , finalcut::FTermcap::no_padding_char );
  tcapBoolean ( "xon_xoff_flow_control"
              , finalcut::FTermcap::xon_xoff_flow_control );
}

//----------------------------------------------------------------------
void numeric()
{
  std::cout << "\r\n[Numeric]\r\n";
  tcapNumeric ("max_color"
              , finalcut::FTermcap::max_color);
  tcapNumeric ("tabstop"
              , finalcut::FTermcap::tabstop);
  tcapNumeric ("padding_baudrate"
              , finalcut::FTermcap::padding_baudrate);
  tcapNumeric ("attr_without_color"
              , finalcut::FTermcap::attr_without_color);
}

//----------------------------------------------------------------------
void string()
{
  std::cout << "\r\n[String]\r\n";
  const auto& tcap_strings = finalcut::FTermcap::strings;

  for (const auto& entry : Data::strings)
  {
    const std::string name = entry.name;
    const auto cap = std::size_t(entry.cap);
    tcapString (name, tcap_strings[cap].string);
  }
}

//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
{
  // Disabling the switch to the alternative screen
  finalcut::FTerm::useAlternateScreen(false);

  // Disable color palette changes and terminal data requests
  auto& start_options = finalcut::FStartOptions::getFStartOptions();
  start_options.color_change = false;
  start_options.terminal_data_request = false;

  // Create the application object as root widget
  finalcut::FApplication term_app {argc, argv};

  // Force terminal initialization without calling show()
  term_app.initTerminal();

  if ( finalcut::FApplication::isQuit() )
    return 0;

  std::cout << "--------\r\nFTermcap\r\n--------\r\n\n";
  std::cout << "Terminal: " << finalcut::FTerm::getTermType() << "\r\n";

  debug (term_app);

  booleans();
  numeric();
  string();
  return 0;
}
