// File: ftcap_map.h

#ifndef _FTCAPMAP_H
#define _FTCAPMAP_H

#pragma pack(push)
#pragma pack(1)

typedef struct
{
  char* string;
  char  tname[3];
}
tcap_map;

#pragma pack(pop)


static tcap_map tcap[] =
{
 // .------------- term string
 // |    .-------- Tcap-code
 // |    |      // variable name          -> description
 //-----------------------------------------------------------------------------
  { 0, "bl" },  // bell                   -> audible signal (bell) (P)
  { 0, "cl" },  // clear_screen           -> clear screen and home cursor (P*)
  { 0, "ho" },  // cursor_home            -> home cursor (if no cup)
  { 0, "ll" },  // cursor_to_ll           -> last line, first column (if no cup)
  { 0, "cr" },  // carriage_return        -> carriage return (P*)
  { 0, "ta" },  // tab                    -> tab to next 8-space hardware tab stop
  { 0, "bt" },  // back_tab               -> back tab (P)
  { 0, "ip" },  // insert_padding         -> insert padding after inserted character
  { 0, "ic" },  // insert_character       -> insert character (P)
  { 0, "IC" },  // parm_ich               -> insert #1 characters (P*)
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

enum termcaps
{
  t_bell,
  t_clear_screen,
  t_cursor_home,
  t_cursor_to_ll,
  t_carriage_return,
  t_tab,
  t_back_tab,
  t_insert_padding,
  t_insert_character,
  t_parm_ich,
  t_initialize_color,
  t_initialize_pair,
  t_set_a_foreground,
  t_set_a_background,
  t_set_foreground,
  t_set_background,
  t_set_color_pair,
  t_orig_pair,
  t_orig_colors,
  t_no_color_video,
  t_cursor_address,
  t_column_address,
  t_row_address,
  t_cursor_visible,
  t_cursor_invisible,
  t_cursor_normal,
  t_cursor_up,
  t_cursor_down,
  t_cursor_left,
  t_cursor_right,
  t_parm_up_cursor,
  t_parm_down_cursor,
  t_parm_left_cursor,
  t_parm_right_cursor,
  t_save_cursor,
  t_restore_cursor,
  t_scroll_forward,
  t_scroll_reverse,
  t_enter_ca_mode,
  t_exit_ca_mode,
  t_enable_acs,
  t_enter_bold_mode,
  t_exit_bold_mode,
  t_enter_dim_mode,
  t_exit_dim_mode,
  t_enter_italics_mode,
  t_exit_italics_mode,
  t_enter_underline_mode,
  t_exit_underline_mode,
  t_enter_blink_mode,
  t_exit_blink_mode,
  t_enter_reverse_mode,
  t_exit_reverse_mode,
  t_enter_standout_mode,
  t_exit_standout_mode,
  t_enter_secure_mode,
  t_exit_secure_mode,
  t_enter_protected_mode,
  t_exit_protected_mode,
  t_enter_crossed_out_mode,
  t_exit_crossed_out_mode,
  t_enter_dbl_underline_mode,
  t_exit_dbl_underline_mode,
  t_set_attributes,
  t_exit_attribute_mode,
  t_enter_alt_charset_mode,
  t_exit_alt_charset_mode,
  t_enter_pc_charset_mode,
  t_exit_pc_charset_mode,
  t_enter_insert_mode,
  t_exit_insert_mode,
  t_enter_am_mode,
  t_exit_am_mode,
  t_acs_chars,
  t_keypad_xmit,
  t_keypad_local,
  t_key_mouse
};

#endif  // _FTCAPMAP_H
