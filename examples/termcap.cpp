/***********************************************************************
* termcap.cpp - Show the used termcap variables                        *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2017-2018 Markus Gans                                      *
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

#include <iomanip>
#include <iostream>
#include <string>

#include <final/final.h>


// Global FVTerm object
static finalcut::FVTerm* terminal;

// Function prototype
void tcapBoolean (const std::string&, bool);
void tcapNumeric (const std::string&, int);
void tcapString (const std::string&, const char[]);
void debug (finalcut::FApplication&);
void booleans();
void numeric();
void string(finalcut::FTermcap::tcap_map*&);


//----------------------------------------------------------------------
// struct data
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

struct data
{
  static int getNumberOfItems();

  struct termcap_string
  {
    const std::string name;
    const finalcut::fc::termcaps cap;
  };

  static termcap_string strings[];
};
#pragma pack(pop)

//----------------------------------------------------------------------
// struct data - string data array
//----------------------------------------------------------------------
data::termcap_string data::strings[] =
{
  { "t_bell", finalcut::fc::t_bell },
  { "t_erase_chars", finalcut::fc::t_erase_chars },
  { "t_clear_screen", finalcut::fc::t_clear_screen },
  { "t_clr_eos", finalcut::fc::t_clr_eos },
  { "t_clr_eol", finalcut::fc::t_clr_eol },
  { "t_clr_bol", finalcut::fc::t_clr_bol },
  { "t_cursor_home", finalcut::fc::t_cursor_home },
  { "t_cursor_to_ll", finalcut::fc::t_cursor_to_ll },
  { "t_carriage_return", finalcut::fc::t_carriage_return },
  { "t_tab", finalcut::fc::t_tab },
  { "t_back_tab", finalcut::fc::t_back_tab },
  { "t_insert_padding", finalcut::fc::t_insert_padding },
  { "t_insert_character", finalcut::fc::t_insert_character },
  { "t_parm_ich", finalcut::fc::t_parm_ich },
  { "t_repeat_char", finalcut::fc::t_repeat_char },
  { "t_initialize_color", finalcut::fc::t_initialize_color },
  { "t_initialize_pair", finalcut::fc::t_initialize_pair },
  { "t_set_a_foreground", finalcut::fc::t_set_a_foreground },
  { "t_set_a_background", finalcut::fc::t_set_a_background },
  { "t_set_foreground", finalcut::fc::t_set_foreground },
  { "t_set_background", finalcut::fc::t_set_background },
  { "t_set_color_pair", finalcut::fc::t_set_color_pair },
  { "t_orig_pair", finalcut::fc::t_orig_pair },
  { "t_orig_colors", finalcut::fc::t_orig_colors },
  { "t_no_color_video", finalcut::fc::t_no_color_video },
  { "t_cursor_address", finalcut::fc::t_cursor_address },
  { "t_column_address", finalcut::fc::t_column_address },
  { "t_row_address", finalcut::fc::t_row_address },
  { "t_cursor_visible", finalcut::fc::t_cursor_visible },
  { "t_cursor_invisible", finalcut::fc::t_cursor_invisible },
  { "t_cursor_normal", finalcut::fc::t_cursor_normal },
  { "t_cursor_up", finalcut::fc::t_cursor_up },
  { "t_cursor_down", finalcut::fc::t_cursor_down },
  { "t_cursor_left", finalcut::fc::t_cursor_left },
  { "t_cursor_right", finalcut::fc::t_cursor_right },
  { "t_parm_up_cursor", finalcut::fc::t_parm_up_cursor },
  { "t_parm_down_cursor", finalcut::fc::t_parm_down_cursor },
  { "t_parm_left_cursor", finalcut::fc::t_parm_left_cursor },
  { "t_parm_right_cursor", finalcut::fc::t_parm_right_cursor },
  { "t_save_cursor", finalcut::fc::t_save_cursor },
  { "t_restore_cursor", finalcut::fc::t_restore_cursor },
  { "t_scroll_forward", finalcut::fc::t_scroll_forward },
  { "t_scroll_reverse", finalcut::fc::t_scroll_reverse },
  { "t_enter_ca_mode", finalcut::fc::t_enter_ca_mode },
  { "t_exit_ca_mode", finalcut::fc::t_exit_ca_mode },
  { "t_enable_acs", finalcut::fc::t_enable_acs },
  { "t_enter_bold_mode", finalcut::fc::t_enter_bold_mode },
  { "t_exit_bold_mode", finalcut::fc::t_exit_bold_mode },
  { "t_enter_dim_mode", finalcut::fc::t_enter_dim_mode },
  { "t_exit_dim_mode", finalcut::fc::t_exit_dim_mode },
  { "t_enter_italics_mode", finalcut::fc::t_enter_italics_mode },
  { "t_exit_italics_mode", finalcut::fc::t_exit_italics_mode },
  { "t_enter_underline_mode", finalcut::fc::t_enter_underline_mode },
  { "t_exit_underline_mode", finalcut::fc::t_exit_underline_mode },
  { "t_enter_blink_mode", finalcut::fc::t_enter_blink_mode },
  { "t_exit_blink_mode", finalcut::fc::t_exit_blink_mode },
  { "t_enter_reverse_mode", finalcut::fc::t_enter_reverse_mode },
  { "t_exit_reverse_mode", finalcut::fc::t_exit_reverse_mode },
  { "t_enter_standout_mode", finalcut::fc::t_enter_standout_mode },
  { "t_exit_standout_mode", finalcut::fc::t_exit_standout_mode },
  { "t_enter_secure_mode", finalcut::fc::t_enter_secure_mode },
  { "t_exit_secure_mode", finalcut::fc::t_exit_secure_mode },
  { "t_enter_protected_mode", finalcut::fc::t_enter_protected_mode },
  { "t_exit_protected_mode", finalcut::fc::t_exit_protected_mode },
  { "t_enter_crossed_out_mode", finalcut::fc::t_enter_crossed_out_mode },
  { "t_exit_crossed_out_mode", finalcut::fc::t_exit_crossed_out_mode },
  { "t_enter_dbl_underline_mode", finalcut::fc::t_enter_dbl_underline_mode },
  { "t_exit_dbl_underline_mode", finalcut::fc::t_exit_dbl_underline_mode },
  { "t_set_attributes", finalcut::fc::t_set_attributes },
  { "t_exit_attribute_mode", finalcut::fc::t_exit_attribute_mode },
  { "t_enter_alt_charset_mode", finalcut::fc::t_enter_alt_charset_mode },
  { "t_exit_alt_charset_mode", finalcut::fc::t_exit_alt_charset_mode },
  { "t_enter_pc_charset_mode", finalcut::fc::t_enter_pc_charset_mode },
  { "t_exit_pc_charset_mode", finalcut::fc::t_exit_pc_charset_mode },
  { "t_enter_insert_mode", finalcut::fc::t_enter_insert_mode },
  { "t_exit_insert_mode", finalcut::fc::t_exit_insert_mode },
  { "t_enter_am_mode", finalcut::fc::t_enter_am_mode },
  { "t_exit_am_mode", finalcut::fc::t_exit_am_mode },
  { "t_acs_chars", finalcut::fc::t_acs_chars },
  { "t_keypad_xmit", finalcut::fc::t_keypad_xmit },
  { "t_keypad_local", finalcut::fc::t_keypad_local },
  { "t_key_mouse", finalcut::fc::t_key_mouse }
};

// data inline functions
//----------------------------------------------------------------------
inline int data::getNumberOfItems()
{
  return int ( sizeof(strings) / sizeof(strings[0]) ) - 1;
}


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
  uInt len;
  std::string sequence;
  std::cout << name << ": ";

  if ( cap_str == 0 )
  {
    std::cout << "\r\n";
    return;
  }

  len = uInt(std::strlen(cap_str));

  for (uInt i = 0; i < len; i++)
  {
    uChar c = uChar(cap_str[i]);

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
void debug (finalcut::FApplication& TermApp)
{
  finalcut::FTermDebugData& debug_data = TermApp.getFTermDebugData();
  const finalcut::FString& ab_s = debug_data.getAnswerbackString();
  const finalcut::FString& sec_da = debug_data.getSecDAString();
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
    tcapString ("|         The answerback String", ab_s);

  if ( ! sec_da.isEmpty() )
    tcapString ("|              The SecDA String", sec_da);

  std::cout << "`------------------- debug -------------------\r\n";
}
#else
void debug (finalcut::FApplication&)
{ }
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
  tcapBoolean ( "ansi_default_color"
              , finalcut::FTermcap::ansi_default_color );
  tcapBoolean ( "osc_support"
              , finalcut::FTermcap::osc_support );
  tcapBoolean ( "no_utf8_acs_chars"
              , finalcut::FTermcap::no_utf8_acs_chars );
}

//----------------------------------------------------------------------
void numeric()
{
  std::cout << "\r\n[Numeric]\r\n";
  tcapNumeric ("max_color"
              , finalcut::FTermcap::max_color);
  tcapNumeric ("tabstop"
              , finalcut::FTermcap::tabstop);
  tcapNumeric ("attr_without_color"
              , finalcut::FTermcap::attr_without_color);
}

//----------------------------------------------------------------------
void string(finalcut::FTermcap::tcap_map*& tcap)
{
  std::cout << "\r\n[String]\r\n";

  for (int n = 0; n <= data::getNumberOfItems(); n++ )
  {
    const std::string name = data::strings[n].name;
    const finalcut::fc::termcaps cap = data::strings[n].cap;
    tcapString (name, tcap[cap].string);
  }
}

//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
{
  bool disable_alt_screen = true;
  finalcut::FApplication TermApp (argc, argv, disable_alt_screen);

  // Pointer to the global virtual terminal object
  terminal = static_cast<finalcut::FVTerm*>(&TermApp);

  finalcut::FTermcap::tcap_map* tcap = 0;
  tcap = finalcut::FTermcap::getTermcapMap();

  std::cout << "--------\r\nFTermcap\r\n--------\r\n\n";
  std::cout << "Terminal: " << TermApp.getTermType() << "\r\n";

  debug (TermApp);

  booleans();
  numeric();
  string(tcap);
}
