/************************************************************************
* ftcap_map.h - Internally used termcap capabilities                    *
*                                                                       *
* This file is part of the Final Cut widget toolkit                     *
*                                                                       *
* Copyright 2015-2017 Markus Gans                                       *
*                                                                       *
* The Final Cut is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by  *
* the Free Software Foundation; either version 3 of the License, or     *
* (at your option) any later version.                                   *
*                                                                       *
* The Final Cut is distributed in the hope that it will be useful,      *
* but WITHOUT ANY WARRANTY; without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
* GNU General Public License for more details.                          *
*                                                                       *
* You should have received a copy of the GNU General Public License     *
* along with this program.  If not, see <http://www.gnu.org/licenses/>. *
************************************************************************/

#ifndef FTCAPMAP_H
#define FTCAPMAP_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/ftermcap.h"

static FTermcap::tcap_map term_caps[] =
{
 // .------------- term string
 // |    .-------- Tcap-code
 // |    |      // variable name          -> description
 //-----------------------------------------------------------------------------
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

#endif  // FTCAPMAP_H
