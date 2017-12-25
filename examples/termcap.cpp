/***********************************************************************
* termcap.cpp - Show the used termcap variables                        *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2017 Markus Gans                                           *
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


// global FVTerm object
static FVTerm* terminal;

// function prototype
void tcapBooleans (const std::string&, bool);
void tcapNumeric (const std::string&, int);
void tcapString (const std::string&, const char[]);
void debug (FApplication&);
void booleans();
void numeric();
void string(FTermcap::tcap_map*&);

//----------------------------------------------------------------------
// functions
//----------------------------------------------------------------------
void tcapBooleans (const std::string& name, bool cap_bool)
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
void debug (FApplication& TermApp)
{
#if DEBUG
  std::cout << "\n.------------------- debug -------------------\r\n";
#if defined(__linux__)
  std::cout << "|               Framebuffer bpp: "
            << TermApp.framebuffer_bpp << "\r\n";
#endif
  std::cout << "| after init_256colorTerminal(): "
            << TermApp.termtype_256color << "\r\n";
  std::cout << "|    after parseAnswerbackMsg(): "
            << TermApp.termtype_Answerback << "\r\n";
  std::cout << "|            after parseSecDA(): "
            << TermApp.termtype_SecDA << "\r\n";

  if ( const FString& s = TermApp.getAnswerbackString() )
    tcapString ("|         The answerback String", s);

  if (  const FString& s = TermApp.getSecDAString() )
    tcapString ("|              The SecDA String", s);

  std::cout << "`------------------- debug -------------------\r\n";
#endif
}

//----------------------------------------------------------------------
void booleans()
{
  std::cout << "\r\n[Booleans]\r\n";
  tcapBooleans ( "background_color_erase"
               , FTermcap::background_color_erase );
  tcapBooleans ( "automatic_left_margin"
               , FTermcap::automatic_left_margin );
  tcapBooleans ( "automatic_right_margin"
               , FTermcap::automatic_right_margin );
  tcapBooleans ( "eat_nl_glitch"
               , FTermcap::eat_nl_glitch );
  tcapBooleans ( "ansi_default_color"
               , FTermcap::ansi_default_color );
  tcapBooleans ( "osc_support"
               , FTermcap::osc_support );
  tcapBooleans ( "no_utf8_acs_chars"
               , FTermcap::no_utf8_acs_chars );
}

//----------------------------------------------------------------------
void numeric()
{
  std::cout << "\r\n[Numeric]\r\n";
  tcapNumeric ("max_color"
              , FTermcap::max_color);
  tcapNumeric ("tabstop"
              , FTermcap::tabstop);
  tcapNumeric ("attr_without_color"
              , FTermcap::attr_without_color);
}

//----------------------------------------------------------------------
void string(FTermcap::tcap_map*& tcap)
{
  std::cout << "\r\n[String]\r\n";
  tcapString ("t_bell"
             , tcap[fc::t_bell].string);
  tcapString ("t_erase_chars"
             , tcap[fc::t_erase_chars].string);
  tcapString ("t_clear_screen"
             , tcap[fc::t_clear_screen].string);
  tcapString ("t_clr_eos"
             , tcap[fc::t_clr_eos].string);
  tcapString ("t_clr_eol"
             , tcap[fc::t_clr_eol].string);
  tcapString ("t_clr_bol"
             , tcap[fc::t_clr_bol].string);
  tcapString ("t_cursor_home"
             , tcap[fc::t_cursor_home].string);
  tcapString ("t_cursor_to_ll"
             , tcap[fc::t_cursor_to_ll].string);
  tcapString ("t_carriage_return"
             , tcap[fc::t_carriage_return].string);
  tcapString ("t_tab"
             , tcap[fc::t_tab].string);
  tcapString ("t_back_tab"
             , tcap[fc::t_back_tab].string);
  tcapString ("t_insert_padding"
             , tcap[fc::t_insert_padding].string);
  tcapString ("t_insert_character"
             , tcap[fc::t_insert_character].string);
  tcapString ("t_parm_ich"
             , tcap[fc::t_parm_ich].string);
  tcapString ("t_repeat_char"
             , tcap[fc::t_repeat_char].string);
  tcapString ("t_initialize_color"
             , tcap[fc::t_initialize_color].string);
  tcapString ("t_initialize_pair"
             , tcap[fc::t_initialize_pair].string);
  tcapString ("t_set_a_foreground"
             , tcap[fc::t_set_a_foreground].string);
  tcapString ("t_set_a_background"
             , tcap[fc::t_set_a_background].string);
  tcapString ("t_set_foreground"
             , tcap[fc::t_set_foreground].string);
  tcapString ("t_set_background"
             , tcap[fc::t_set_background].string);
  tcapString ("t_set_color_pair"
             , tcap[fc::t_set_color_pair].string);
  tcapString ("t_orig_pair"
             , tcap[fc::t_orig_pair].string);
  tcapString ("t_orig_colors"
             , tcap[fc::t_orig_colors].string);
  tcapString ("t_no_color_video"
             , tcap[fc::t_no_color_video].string);
  tcapString ("t_cursor_address"
             , tcap[fc::t_cursor_address].string);
  tcapString ("t_column_address"
             , tcap[fc::t_column_address].string);
  tcapString ("t_row_address"
             , tcap[fc::t_row_address].string);
  tcapString ("t_cursor_visible"
             , tcap[fc::t_cursor_visible].string);
  tcapString ("t_cursor_invisible"
             , tcap[fc::t_cursor_invisible].string);
  tcapString ("t_cursor_normal"
             , tcap[fc::t_cursor_normal].string);
  tcapString ("t_cursor_up"
             , tcap[fc::t_cursor_up].string);
  tcapString ("t_cursor_down"
             , tcap[fc::t_cursor_down].string);
  tcapString ("t_cursor_left"
             , tcap[fc::t_cursor_left].string);
  tcapString ("t_cursor_right"
             , tcap[fc::t_cursor_right].string);
  tcapString ("t_parm_up_cursor"
             , tcap[fc::t_parm_up_cursor].string);
  tcapString ("t_parm_down_cursor"
             , tcap[fc::t_parm_down_cursor].string);
  tcapString ("t_parm_left_cursor"
             , tcap[fc::t_parm_left_cursor].string);
  tcapString ("t_parm_right_cursor"
             , tcap[fc::t_parm_right_cursor].string);
  tcapString ("t_save_cursor"
             , tcap[fc::t_save_cursor].string);
  tcapString ("t_restore_cursor"
             , tcap[fc::t_restore_cursor].string);
  tcapString ("t_scroll_forward"
             , tcap[fc::t_scroll_forward].string);
  tcapString ("t_scroll_reverse"
             , tcap[fc::t_scroll_reverse].string);
  tcapString ("t_enter_ca_mode"
             , tcap[fc::t_enter_ca_mode].string);
  tcapString ("t_exit_ca_mode"
             , tcap[fc::t_exit_ca_mode].string);
  tcapString ("t_enable_acs"
             , tcap[fc::t_enable_acs].string);
  tcapString ("t_enter_bold_mode"
             , tcap[fc::t_enter_bold_mode].string);
  tcapString ("t_exit_bold_mode"
             , tcap[fc::t_exit_bold_mode].string);
  tcapString ("t_enter_dim_mode"
             , tcap[fc::t_enter_dim_mode].string);
  tcapString ("t_exit_dim_mode"
             , tcap[fc::t_exit_dim_mode].string);
  tcapString ("t_enter_italics_mode"
             , tcap[fc::t_enter_italics_mode].string);
  tcapString ("t_exit_italics_mode"
             , tcap[fc::t_exit_italics_mode].string);
  tcapString ("t_enter_underline_mode"
             , tcap[fc::t_enter_underline_mode].string);
  tcapString ("t_exit_underline_mode"
             , tcap[fc::t_exit_underline_mode].string);
  tcapString ("t_enter_blink_mode"
             , tcap[fc::t_enter_blink_mode].string);
  tcapString ("t_exit_blink_mode"
             , tcap[fc::t_exit_blink_mode].string);
  tcapString ("t_enter_reverse_mode"
             , tcap[fc::t_enter_reverse_mode].string);
  tcapString ("t_exit_reverse_mode"
             , tcap[fc::t_exit_reverse_mode].string);
  tcapString ("t_enter_standout_mode"
             , tcap[fc::t_enter_standout_mode].string);
  tcapString ("t_exit_standout_mode"
             , tcap[fc::t_exit_standout_mode].string);
  tcapString ("t_enter_secure_mode"
             , tcap[fc::t_enter_secure_mode].string);
  tcapString ("t_exit_secure_mode"
             , tcap[fc::t_exit_secure_mode].string);
  tcapString ("t_enter_protected_mode"
             , tcap[fc::t_enter_protected_mode].string);
  tcapString ("t_exit_protected_mode"
             , tcap[fc::t_exit_protected_mode].string);
  tcapString ("t_enter_crossed_out_mode"
             , tcap[fc::t_enter_crossed_out_mode].string);
  tcapString ("t_exit_crossed_out_mode"
             , tcap[fc::t_exit_crossed_out_mode].string);
  tcapString ("t_enter_dbl_underline_mode"
             , tcap[fc::t_enter_dbl_underline_mode].string);
  tcapString ("t_exit_dbl_underline_mode"
             , tcap[fc::t_exit_dbl_underline_mode].string);
  tcapString ("t_set_attributes"
             , tcap[fc::t_set_attributes].string);
  tcapString ("t_exit_attribute_mode"
             , tcap[fc::t_exit_attribute_mode].string);
  tcapString ("t_enter_alt_charset_mode"
             , tcap[fc::t_enter_alt_charset_mode].string);
  tcapString ("t_exit_alt_charset_mode"
             , tcap[fc::t_exit_alt_charset_mode].string);
  tcapString ("t_enter_pc_charset_mode"
             , tcap[fc::t_enter_pc_charset_mode].string);
  tcapString ("t_exit_pc_charset_mode"
             , tcap[fc::t_exit_pc_charset_mode].string);
  tcapString ("t_enter_insert_mode"
             , tcap[fc::t_enter_insert_mode].string);
  tcapString ("t_exit_insert_mode"
             , tcap[fc::t_exit_insert_mode].string);
  tcapString ("t_enter_am_mode"
             , tcap[fc::t_enter_am_mode].string);
  tcapString ("t_exit_am_mode"
             , tcap[fc::t_exit_am_mode].string);
  tcapString ("t_acs_chars"
             , tcap[fc::t_acs_chars].string);
  tcapString ("t_keypad_xmit"
             , tcap[fc::t_keypad_xmit].string);
  tcapString ("t_keypad_local"
             , tcap[fc::t_keypad_local].string);
  tcapString ("t_key_mouse"
             , tcap[fc::t_key_mouse].string);
}

//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
{
  bool disable_alt_screen = true;
  FApplication TermApp (argc, argv, disable_alt_screen);

  // Pointer to the global virtual terminal object
  terminal = static_cast<FVTerm*>(&TermApp);

  FTermcap::tcap_map* tcap = 0;
  tcap = FTermcap::getTermcapMap();

  std::cout << "--------\r\nFTermcap\r\n--------\r\n\n";
  std::cout << "Terminal: " << TermApp.getTermType() << "\r\n";

  debug (TermApp);

  booleans();
  numeric();
  string(tcap);
}
