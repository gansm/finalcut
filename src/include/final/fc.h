/***********************************************************************
* fc.h - Implements global constants and enumerations                  *
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

#ifndef FC_H
#define FC_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <final/ftypes.h>

// Typecast to c-string
#define C_STR const_cast<char*>

// ASCII sequences
#define ESC    "\033"     // Escape
#define CSI    ESC "["    // Control sequence introducer (7-bit)
#define ENQ    "\005"     // Enquiry
#define BEL    "\007"     // Bell (ctrl‐g)
#define BS     "\010"     // Backspace
#define SO     "\016"     // Shift out (alternative character set)
#define SI     "\017"     // Shift in  (regular character set)
#define OSC    ESC "]"    // Operating system command (7-bit)
#define SECDA  ESC "[>c"  // Secondary Device Attributes

namespace finalcut
{

//----------------------------------------------------------------------
// Global constants and enumerations
//----------------------------------------------------------------------

namespace fc
{

// Event types
enum events
{
  None_Event,               // invalid event
  KeyPress_Event,           // key pressed
  KeyUp_Event,              // key released
  KeyDown_Event,            // key pressed
  MouseDown_Event,          // mouse button pressed
  MouseUp_Event,            // mouse button released
  MouseDoubleClick_Event,   // mouse button double click
  MouseWheel_Event,         // mouse wheel rolled
  MouseMove_Event,          // mouse move
  FocusIn_Event,            // focus in
  FocusOut_Event,           // focus out
  ChildFocusIn_Event,       // child focus in
  ChildFocusOut_Event,      // child focus out
  WindowActive_Event,       // activate window
  WindowInactive_Event,     // deactivate window
  WindowRaised_Event,       // raise window
  WindowLowered_Event,      // lower window
  Accelerator_Event,        // keyboard accelerator
  Resize_Event,             // terminal resize
  Show_Event,               // widget is shown
  Hide_Event,               // widget is hidden
  Close_Event,              // widget close
  Timer_Event               // timer event occur
};

// Internal character encoding
enum encoding
{
  UTF8,
  VT100,
  PC,
  ASCII,
  NUM_OF_ENCODINGS,  // number of items
  UNKNOWN
};

// VT100 line graphic keys
enum vt100_keys
{
  vt100_key_rarrow    = '+',   // ►  -  arrow pointing right
  vt100_key_larrow    = ',',   // ◄  -  arrow pointing left
  vt100_key_uarrow    = '-',   // ▲  -  arrow pointing up
  vt100_key_darrow    = '.',   // ▼  -  arrow pointing down
  vt100_key_block     = '0',   // █  -  solid square block
  vt100_key_nsup      = 'I',   // ⁿ  -  superscript letter n
  vt100_key_blackrect = '_',   // ▮  -  black vertical rectangle
  vt100_key_diamond   = '`',   // ◆  -  diamond
  vt100_key_ckboard   = 'a',   // ▒  -  checker board (stipple)
  vt100_key_htab      = 'b',   // ␉  -  horizontal tab symbol
  vt100_key_ff        = 'c',   // ␌  -  form feed symbol
  vt100_key_cr        = 'd',   // ␍  -  carriage return symbol
  vt100_key_lf        = 'e',   // ␊  -  line feed symbol
  vt100_key_degree    = 'f',   // °  -  degree symbol
  vt100_key_plminus   = 'g',   // ±  -  plus/minus
  vt100_key_board     = 'h',   // ␤  -  board of squares
  vt100_key_lantern   = 'i',   // ␋  -  lantern symbol
  vt100_key_lrcorner  = 'j',   // ┘  -  lower right corner
  vt100_key_urcorner  = 'k',   // ┐  -  upper right corner
  vt100_key_ulcorner  = 'l',   // ┌  -  upper left corner
  vt100_key_llcorner  = 'm',   // └  -  lower left corner
  vt100_key_plus      = 'n',   // ┼  -  large plus or crossover
  vt100_key_s1        = 'o',   // ⎺  -  scan line 1
  vt100_key_s3        = 'p',   // ⎻  -  scan line 3
  vt100_key_hline     = 'q',   // ─  -  horizontal line
  vt100_key_s7        = 'r',   // ⎼  -  scan line 7
  vt100_key_s9        = 's',   // ⎽  -  scan line 9
  vt100_key_ltee      = 't',   // ├  -  tee pointing right
  vt100_key_rtee      = 'u',   // ┤  -  tee pointing left
  vt100_key_btee      = 'v',   // ┴  -  tee pointing up
  vt100_key_ttee      = 'w',   // ┬  -  tee pointing down
  vt100_key_vline     = 'x',   // │  -  vertical line
  vt100_key_lequal    = 'y',   // ≤  -  less-than-or-equal-to
  vt100_key_gequal    = 'z',   // ≥  -  greater-than-or-equal-to
  vt100_key_pi        = '{',   // π  -  greek pi
  vt100_key_nequal    = '|',   // ≠  -  not-equal
  vt100_key_sterling  = '}',   // £  -  UK pound sign
  vt100_key_bullet    = '~'    // ·  -  bullet
};

// Unicode characters
enum SpecialCharacter
{
  Euro                                = 0x20ac,  // €
  Pound                               = 0x00a3,  // £
  Pi                                  = 0x03c0,  // π
  SuperscriptLatinSmallLetterN        = 0x207F,  // ⁿ
  GreaterThanOrEqualTo                = 0x2265,  // ≥
  LessThanOrEqualTo                   = 0x2264,  // ≤
  NotEqualTo                          = 0x2260,  // ≠
  PlusMinus                           = 0x00b1,  // ±
  Times                               = 0x00d7,  // ×
  Degree                              = 0x00b0,  // °
  BlackVerticalRectangle              = 0x25ae,  // ▮ (1)
  SmallBullet                         = 0x00b7,  // ·
  BlackDiamondSuit                    = 0x2666,  // ◆
  SymbolForNewline                    = 0x2424,  // ␤ (1)
  SymbolForVerticalTab                = 0x240b,  // ␋ (1)
  SymbolForHorizontalTab              = 0x2409,  // ␉ (1)
  SymbolForFormFeed                   = 0x240c,  // ␌ (1)
  SymbolForCarriageReturn             = 0x240d,  // ␍ (1)
  SymbolForLineFeed                   = 0x240a,  // ␊ (1)
  MediumShade                         = 0x2592,  // ▒
  BoxDrawingsHorizontal               = 0x2500,  // ─
  BoxDrawingsVertical                 = 0x2502,  // │
  BoxDrawingsDownAndRight             = 0x250c,  // ┌
  BoxDrawingsDownAndLeft              = 0x2510,  // ┐
  BoxDrawingsUpAndRight               = 0x2514,  // └
  BoxDrawingsUpAndLeft                = 0x2518,  // ┘
  BoxDrawingsCross                    = 0x253c,  // ┼
  BoxDrawingsDownAndHorizontal        = 0x252c,  // ┬
  BoxDrawingsVerticalAndLeft          = 0x2524,  // ┤
  BoxDrawingsVerticalAndRight         = 0x251c,  // ├
  BoxDrawingsUpAndHorizontal          = 0x2534,  // ┴
  HorizontalScanLine1                 = 0x23ba,  // ⎺ (1)
  HorizontalScanLine3                 = 0x23bb,  // ⎻ (1)
  HorizontalScanLine7                 = 0x23bc,  // ⎼ (1)
  HorizontalScanLine9                 = 0x23bd,  // ⎽ (1)
  BlackUpPointingTriangle             = 0x25b2,  // ▲
  BlackDownPointingTriangle           = 0x25bc,  // ▼
  BlackRightPointingTriangle          = 0x25b6,  // ▶ (1)
  BlackLeftPointingTriangle           = 0x25c0,  // ◀ (1)
  BlackRightPointingPointer           = 0x25ba,  // ►
  BlackLeftPointingPointer            = 0x25c4,  // ◄
  Bullet                              = 0x2022,  // •
  FullBlock                           = 0x2588,  // █
  UpperHalfBlock                      = 0x2580,  // ▀
  LowerHalfBlock                      = 0x2584,  // ▄
  LeftHalfBlock                       = 0x258c,  // ▌
  RightHalfBlock                      = 0x2590,  // ▐
  NF_rev_left_arrow2                  = 0x1ab4,  // ⊐ (2)
  NF_rev_right_arrow2                 = 0x1ab5,  // ► (2)
  NF_radio_button3                    = 0x1ab7,  // ) (2)
  NF_rev_border_corner_upper_right    = 0x1ab8,  // ⎤ (2)
  NF_rev_border_line_right            = 0x1ab9,  // ⎹ (2)
  NF_rev_border_line_vertical_left    = 0x1aba,  // ┤ (2)
  NF_rev_border_corner_lower_right    = 0x1abb,  // ⎦ (2)
  NF_border_line_left                 = 0x1abc,  // ⎸ (2)
  NF_rev_up_arrow2                    = 0x1abd,  //⎹◣ (2)
  NF_rev_down_arrow2                  = 0x1abe,  //⎹◤ (2)
  NF_border_corner_middle_lower_left  = 0x1ac0,  // └ (2)
  NF_rev_up_arrow1                    = 0x1ac1,  // ◢⎸ (2)
  NF_rev_down_arrow1                  = 0x1ac2,  // ◥⎸ (2)
  NF_border_line_vertical_right       = 0x1ac3,  // ├ (2)
  NF_border_line_up_and_down          = 0x1ac5,  // ニ (2)
  NF_shadow_box_middle                = 0x1ac6,  // ニ (2)
  NF_shadow_box_hdd                   = 0x1ac7,  // ニ (2)
  NF_rev_left_arrow1                  = 0x1ac8,  // ◄ (2)
  NF_rev_right_arrow1                 = 0x1ac9,  // ⊏ (2)
  NF_rev_menu_button1                 = 0x1aca,  // [ (2)
  NF_rev_menu_button2                 = 0x1acb,  // - (2)
  NF_border_corner_middle_upper_left  = 0x1acc,  // ┌ (2)
  NF_shadow_box_cd                    = 0x1acd,  // ニ (2)
  NF_shadow_box_left                  = 0x1ace,  // [ (2)
  NF_border_corner_middle_lower_right = 0x1acf,  //  ┘ (2)
  NF_border_corner_middle_upper_right = 0x1ad0,  //  ┐ (2)
  NF_shadow_box_net                   = 0x1ad1,  // ニ (2)
  NF_rev_up_pointing_triangle1        = 0x1ad2,  // ◢ (2)
  NF_border_corner_lower_left         = 0x1ad3,  // ⎣ (2)
  NF_border_line_bottom               = 0x1ad4,  // _ (2)
  NF_radio_button2                    = 0x1ad5,  // ⁐ (2)
  NF_radio_button2_checked            = 0x1ad6,  // ● (2)
  NF_rev_down_pointing_triangle1      = 0x1ad7,  // ◥ (2)
  NF_border_line_upper                = 0x1ad8,  // ¯ (2)
  NF_radio_button1                    = 0x1ad9,  // ( (2)
  NF_border_corner_upper_left         = 0x1ada,  // ⎡ (2)
  NF_shadow_box_checked               = 0x1adc,  // ✓ (2)
  NF_rev_border_line_right_and_left   = 0x1ae7,  // || (2)
  NF_rev_up_pointing_triangle2        = 0x1ae8,  // ◣ (2)
  NF_rev_down_pointing_triangle2      = 0x1ae9,  // ◤ (2)
  NF_rev_menu_button3                 = 0x1af4,  // ] (2)
  NF_shadow_box_right                 = 0x1af5,  // ] (2)
  NF_Bullet                           = 0x1af9,  // ● (2)
  NF_check_mark                       = 0x1afb,  // ✓ (2)
  SquareRoot                          = 0x221a   // √
};
/*
 * (1) Not defined in Windows Glyph List 4 (WGL4)
 * (2) Only supported in use with newfont
 */

// keyboard - single keys
enum keys
{
  Fckey_a                    = 0x00000001,  // control-a
  Fckey_b                    = 0x00000002,  // control-b
  Fckey_c                    = 0x00000003,  // control-c
  Fckey_d                    = 0x00000004,  // control-d
  Fckey_e                    = 0x00000005,  // control-e
  Fckey_f                    = 0x00000006,  // control-f
  Fckey_g                    = 0x00000007,  // control-g
  Fkey_erase                 = 0x00000008,  // control-h
  Fkey_tab                   = 0x00000009,  // control-i
  Fckey_j                    = 0x0000000a,  // control-j
  Fckey_h                    = 0x0000000b,  // control-k
  Fckey_l                    = 0x0000000c,  // control-l
  Fkey_return                = 0x0000000d,  // control-m
  Fckey_n                    = 0x0000000e,  // control-n
  Fckey_o                    = 0x0000000f,  // control-o
  Fckey_p                    = 0x00000010,  // control-p
  Fckey_q                    = 0x00000011,  // control-q
  Fckey_r                    = 0x00000012,  // control-r
  Fckey_s                    = 0x00000013,  // control-s
  Fckey_t                    = 0x00000014,  // control-t
  Fckey_u                    = 0x00000015,  // control-u
  Fckey_v                    = 0x00000016,  // control-v
  Fckey_w                    = 0x00000017,  // control-w
  Fckey_x                    = 0x00000018,  // control-x
  Fckey_y                    = 0x00000019,  // control-y
  Fckey_z                    = 0x0000001a,  // control-t
  Fkey_escape                = 0x0000001b,  // control-[
  Fckey_backslash            = 0x0000001c,  // control-'\'
  Fckey_right_square_bracket = 0x0000001d,  // control-]
  Fckey_caret                = 0x0000001e,  // control-^
  Fckey_underscore           = 0x0000001f,  // control-_
  Fkey_space                 = 0x00000020,  // space
  Fkey_exclamation_mark      = 0x00000021,  // !
  Fkey_quotation_mark        = 0x00000022,  // "
  Fkey_number_sign           = 0x00000023,  // #
  Fkey_dollar_sign           = 0x00000024,  // $
  Fkey_percent_sign          = 0x00000025,  // %
  Fkey_ampersand             = 0x00000026,  // &
  Fkey_apostrophe            = 0x00000027,  // '
  Fkey_left_parentheses      = 0x00000028,  // (
  Fkey_right_parentheses     = 0x00000029,  // )
  Fkey_asterisk              = 0x0000002a,  // *
  Fkey_plus_sign             = 0x0000002b,  // +
  Fkey_comma                 = 0x0000002c,  // ,
  Fkey_minus_sign            = 0x0000002d,  // -
  Fkey_full_stop             = 0x0000002e,  // .
  Fkey_slash                 = 0x0000002f,  // /
  Fckey_space                = 0x01000020,  // control-space
  Fkey_backspace             = 0x01000100,
  Fkey_catab                 = 0x01000101,
  Fkey_clear                 = 0x01000102,
  Fkey_ctab                  = 0x01000103,
  Fkey_dc                    = 0x01000104,
  Fkey_dl                    = 0x01000105,
  Fkey_down                  = 0x01000106,
  Fkey_eic                   = 0x01000107,
  Fkey_eol                   = 0x01000108,
  Fkey_eos                   = 0x01000109,
  Fkey_f0                    = 0x0100010a,
  Fkey_f1                    = 0x0100010b,
  Fkey_f2                    = 0x0100010c,
  Fkey_f3                    = 0x0100010d,
  Fkey_f4                    = 0x0100010e,
  Fkey_f5                    = 0x0100010f,
  Fkey_f6                    = 0x01000110,
  Fkey_f7                    = 0x01000111,
  Fkey_f8                    = 0x01000112,
  Fkey_f9                    = 0x01000113,
  Fkey_f10                   = 0x01000114,
  Fkey_home                  = 0x01000115,
  Fkey_ic                    = 0x01000116,  // insert key
  Fkey_il                    = 0x01000117,
  Fkey_left                  = 0x01000118,
  Fkey_ll                    = 0x01000119,
  Fkey_npage                 = 0x0100011a,
  Fkey_ppage                 = 0x0100011b,
  Fkey_right                 = 0x0100011c,
  Fkey_sf                    = 0x0100011d,
  Fkey_sr                    = 0x0100011e,
  Fkey_stab                  = 0x0100011f,
  Fkey_up                    = 0x01000120,
  Fkey_a1                    = 0x01000121,
  Fkey_a3                    = 0x01000122,
  Fkey_b2                    = 0x01000123,
  Fkey_c1                    = 0x01000124,
  Fkey_c3                    = 0x01000125,
  Fkey_btab                  = 0x01000126,
  Fkey_beg                   = 0x01000127,
  Fkey_cancel                = 0x01000128,
  Fkey_close                 = 0x01000129,
  Fkey_command               = 0x0100012a,
  Fkey_copy                  = 0x0100012b,
  Fkey_create                = 0x0100012c,
  Fkey_end                   = 0x0100012d,
  Fkey_enter                 = 0x0100012e,
  Fkey_exit                  = 0x0100012f,
  Fkey_find                  = 0x01000130,
  Fkey_help                  = 0x01000131,
  Fkey_mark                  = 0x01000132,
  Fkey_message               = 0x01000133,
  Fkey_move                  = 0x01000134,
  Fkey_next                  = 0x01000135,
  Fkey_open                  = 0x01000136,
  Fkey_options               = 0x01000137,
  Fkey_previous              = 0x01000138,
  Fkey_print                 = 0x01000139,
  Fkey_redo                  = 0x0100013a,
  Fkey_reference             = 0x0100013b,
  Fkey_refresh               = 0x0100013c,
  Fkey_replace               = 0x0100013d,
  Fkey_restart               = 0x0100013e,
  Fkey_resume                = 0x0100013f,
  Fkey_save                  = 0x01000140,
  Fkey_suspend               = 0x01000141,
  Fkey_undo                  = 0x01000142,
  Fkey_sbeg                  = 0x01000143,
  Fkey_scancel               = 0x01000144,
  Fkey_scommand              = 0x01000145,
  Fkey_scopy                 = 0x01000146,
  Fkey_screate               = 0x01000147,
  Fkey_sdc                   = 0x01000148,
  Fkey_sdl                   = 0x01000149,
  Fkey_select                = 0x0100014a,
  Fkey_send                  = 0x0100014b,
  Fkey_seol                  = 0x0100014c,
  Fkey_sexit                 = 0x0100014d,
  Fkey_sfind                 = 0x0100014e,
  Fkey_shelp                 = 0x0100014f,
  Fkey_shome                 = 0x01000150,
  Fkey_sic                   = 0x01000151,
  Fkey_sleft                 = 0x01000152,
  Fkey_smessage              = 0x01000153,
  Fkey_smove                 = 0x01000154,
  Fkey_snext                 = 0x01000155,
  Fkey_soptions              = 0x01000156,
  Fkey_sprevious             = 0x01000157,
  Fkey_sprint                = 0x01000158,
  Fkey_sredo                 = 0x01000159,
  Fkey_sreplace              = 0x0100015a,
  Fkey_sright                = 0x0100015b,
  Fkey_srsume                = 0x0100015c,
  Fkey_ssave                 = 0x0100015d,
  Fkey_ssuspend              = 0x0100015e,
  Fkey_sundo                 = 0x0100015f,
  Fkey_f11                   = 0x01000160,
  Fkey_f12                   = 0x01000161,
  Fkey_f13                   = 0x01000162,
  Fkey_f14                   = 0x01000163,
  Fkey_f15                   = 0x01000164,
  Fkey_f16                   = 0x01000165,
  Fkey_f17                   = 0x01000166,
  Fkey_f18                   = 0x01000167,
  Fkey_f19                   = 0x01000168,
  Fkey_f20                   = 0x01000169,
  Fkey_f21                   = 0x0100016a,
  Fkey_f22                   = 0x0100016b,
  Fkey_f23                   = 0x0100016c,
  Fkey_f24                   = 0x0100016d,
  Fkey_f25                   = 0x0100016e,
  Fkey_f26                   = 0x0100016f,
  Fkey_f27                   = 0x01000170,
  Fkey_f28                   = 0x01000171,
  Fkey_f29                   = 0x01000172,
  Fkey_f30                   = 0x01000173,
  Fkey_f31                   = 0x01000174,
  Fkey_f32                   = 0x01000175,
  Fkey_f33                   = 0x01000176,
  Fkey_f34                   = 0x01000177,
  Fkey_f35                   = 0x01000178,
  Fkey_f36                   = 0x01000179,
  Fkey_f37                   = 0x0100017a,
  Fkey_f38                   = 0x0100017b,
  Fkey_f39                   = 0x0100017c,
  Fkey_f40                   = 0x0100017d,
  Fkey_f41                   = 0x0100017e,
  Fkey_f42                   = 0x0100017f,
  Fkey_f43                   = 0x01000180,
  Fkey_f44                   = 0x01000181,
  Fkey_f45                   = 0x01000182,
  Fkey_f46                   = 0x01000183,
  Fkey_f47                   = 0x01000184,
  Fkey_f48                   = 0x01000185,
  Fkey_f49                   = 0x01000186,
  Fkey_f50                   = 0x01000187,
  Fkey_f51                   = 0x01000188,
  Fkey_f52                   = 0x01000189,
  Fkey_f53                   = 0x0100018a,
  Fkey_f54                   = 0x0100018b,
  Fkey_f55                   = 0x0100018c,
  Fkey_f56                   = 0x0100018d,
  Fkey_f57                   = 0x0100018e,
  Fkey_f58                   = 0x0100018f,
  Fkey_f59                   = 0x01000190,
  Fkey_f60                   = 0x01000191,
  Fkey_f61                   = 0x01000192,
  Fkey_f62                   = 0x01000193,
  Fkey_f63                   = 0x01000194,
  need_more_data             = 0x80000000
};

// Keyboard - modifier key combinations
enum metakeys
{
  Fmkey_ic                   = 0x01500100,  // M-insert
  Fmkey_dc                   = 0x01500101,  // M-delete
  Fmkey_home                 = 0x01500102,  // M-home
  Fmkey_end                  = 0x01500103,  // M-end
  Fmkey_ppage                = 0x01500104,  // M-prev-page
  Fmkey_npage                = 0x01500105,  // M-next-page
  Fmkey_f1                   = 0x01500106,  // M-f1
  Fmkey_f2                   = 0x01500107,  // M-f2
  Fmkey_f3                   = 0x01500108,  // M-f3
  Fmkey_f4                   = 0x01500109,  // M-f4
  Fmkey_f5                   = 0x0150010a,  // M-f5
  Fmkey_f6                   = 0x0150010b,  // M-F6
  Fmkey_f7                   = 0x0150010c,  // M-f7
  Fmkey_f8                   = 0x0150010d,  // M-f8
  Fmkey_f9                   = 0x0150010e,  // M-f9
  Fmkey_f10                  = 0x0150010f,  // M-f10
  Fmkey_f11                  = 0x01500110,  // M-f11
  Fmkey_f12                  = 0x01500111,  // M-f12
  Fmkey_up                   = 0x01500112,  // M-up
  Fmkey_down                 = 0x01500113,  // M-down
  Fmkey_right                = 0x01500114,  // M-right
  Fmkey_left                 = 0x01500115,  // M-left
  Fmkey_sic                  = 0x01500116,  // shifted M-Insert
  Fmkey_sdc                  = 0x01500117,  // shifted M-Delete
  Fmkey_shome                = 0x01500118,  // shifted M-Home
  Fmkey_send                 = 0x01500119,  // shifted M-End
  Fmkey_sppage               = 0x0150011a,  // shifted M-Page_Up
  Fmkey_snpage               = 0x0150011b,  // shifted M-Page_Down
  Fmkey_sf1                  = 0x0150011c,  // shifted M-F1
  Fmkey_sf2                  = 0x0150011d,  // shifted M-F2
  Fmkey_sf3                  = 0x0150011e,  // shifted M-F3
  Fmkey_sf4                  = 0x0150011f,  // shifted M-F4
  Fmkey_sf5                  = 0x01500120,  // shifted M-F5
  Fmkey_sf6                  = 0x01500121,  // shifted M-F6
  Fmkey_sf7                  = 0x01500122,  // shifted M-F7
  Fmkey_sf8                  = 0x01500123,  // shifted M-F8
  Fmkey_sf9                  = 0x01500124,  // shifted M-F9
  Fmkey_sf10                 = 0x01500125,  // shifted M-F10
  Fmkey_sf11                 = 0x01500126,  // shifted M-F11
  Fmkey_sf12                 = 0x01500127,  // shifted M-F12
  Fmkey_sup                  = 0x01500128,  // shifted M-Up
  Fmkey_sdown                = 0x01500129,  // shifted M-Down
  Fmkey_sright               = 0x0150012a,  // shifted M-Right
  Fmkey_sleft                = 0x0150012b,  // shifted M-Left
  Fckey_ic                   = 0x0150012c,  // control-Insert
  Fckey_dc                   = 0x0150012d,  // control-Delete
  Fckey_home                 = 0x0150012e,  // control-Home
  Fckey_end                  = 0x0150012f,  // control-End
  Fckey_ppage                = 0x01500130,  // control-Page_Up
  Fckey_npage                = 0x01500131,  // control-Page_Down
  Fckey_up                   = 0x01500132,  // control-Up
  Fckey_down                 = 0x01500133,  // control-Down
  Fckey_right                = 0x01500134,  // control-Right
  Fckey_left                 = 0x01500135,  // control-Left
  Fckey_sic                  = 0x01500136,  // shifted control-M-Insert
  Fckey_sdc                  = 0x01500137,  // shifted control-M-Delete
  Fckey_shome                = 0x01500138,  // shifted control-M-Home
  Fckey_send                 = 0x01500139,  // shifted control-M-End
  Fckey_sppage               = 0x0150013a,  // shifted control-M-Page_Up
  Fckey_snpage               = 0x0150013b,  // shifted control-M-Page_Down
  Fckey_sup                  = 0x0150013c,  // shifted control-M-Up
  Fckey_sdown                = 0x0150013d,  // shifted control-M-Down
  Fckey_sright               = 0x0150013e,  // shifted control-M-Right
  Fckey_sleft                = 0x0150013f,  // shifted control-M-Left
  Fcmkey_ic                  = 0x01500140,  // control-M-Insert
  Fcmkey_dc                  = 0x01500141,  // control-M-Delete
  Fcmkey_home                = 0x01500142,  // control-M-Home
  Fcmkey_end                 = 0x01500143,  // control-M-End
  Fcmkey_ppage               = 0x01500144,  // control-M-Page_Up
  Fcmkey_npage               = 0x01500145,  // control-M-Page_Down
  Fcmkey_up                  = 0x01500146,  // control-M-Up
  Fcmkey_down                = 0x01500147,  // control-M-Down
  Fcmkey_right               = 0x01500148,  // control-M-Right
  Fcmkey_left                = 0x01500149,  // control-M-Left
  Fcmkey_sic                 = 0x0150014a,  // shifted control-M-Insert
  Fcmkey_sdc                 = 0x0150014b,  // shifted control-M-Delete
  Fcmkey_shome               = 0x0150014c,  // shifted control-M-Home
  Fcmkey_send                = 0x0150014d,  // shifted control-M-End
  Fcmkey_sppage              = 0x0150014e,  // shifted control-M-Page_Up
  Fcmkey_snpage              = 0x0150014f,  // shifted control-M-Page_Down
  Fcmkey_sf1                 = 0x01500150,  // shifted control-M-F1
  Fcmkey_sf2                 = 0x01500151,  // shifted control-M-F2
  Fcmkey_sf3                 = 0x01500152,  // shifted control-M-F3
  Fcmkey_sf4                 = 0x01500153,  // shifted control-M-F4
  Fcmkey_sf5                 = 0x01500154,  // shifted control-M-F5
  Fcmkey_sf6                 = 0x01500155,  // shifted control-M-F6
  Fcmkey_sf7                 = 0x01500156,  // shifted control-M-F7
  Fcmkey_sf8                 = 0x01500157,  // shifted control-M-F8
  Fcmkey_sf9                 = 0x01500158,  // shifted control-M-F9
  Fcmkey_sf10                = 0x01500159,  // shifted control-M-F10
  Fcmkey_sf11                = 0x0150015a,  // shifted control-M-F11
  Fcmkey_sf12                = 0x0150015b,  // shifted control-M-F12
  Fcmkey_sup                 = 0x0150015c,  // shifted control-M-Up
  Fcmkey_sdown               = 0x0150015d,  // shifted control-M-Down
  Fcmkey_sright              = 0x0150015e,  // shifted control-M-Right
  Fcmkey_sleft               = 0x0150015f,  // shifted control-M-Left
  Fkey_menu                  = 0x01600000,  // menu
  Fkey_smenu                 = 0x01600001,  // shifted menu
  Fckey_menu                 = 0x01600002,  // control-menu
  Fckey_smenu                = 0x01600003,  // shifted control-menu
  Fmkey_menu                 = 0x01600004,  // M-menu
  Fmkey_smenu                = 0x01600005,  // shifted M-menu
  Fcmkey_menu                = 0x01600006,  // control-M-menu
  Fcmkey_smenu               = 0x01600007,  // shifted control-M-menu
  Fkey_escape_mintty         = 0x0200001b,  // mintty Esc
  Fkey_mouse                 = 0x02000020,  // xterm mouse
  Fkey_extended_mouse        = 0x02000021,  // SGR extended mouse
  Fkey_urxvt_mouse           = 0x02000022,  // urxvt mouse extension
  Fmkey_meta                 = 0x020000e0,  // meta key offset
  Fmkey_tab                  = 0x020000e9,  // M-tab
  Fmkey_enter                = 0x020000ea,  // M-enter
  Fmkey_space                = 0x02000100,  // M-' '
  Fmkey_bang                 = 0x02000101,  // M-!
  Fmkey_quotes               = 0x02000102,  // M-"
  Fmkey_hash                 = 0x02000103,  // M-#
  Fmkey_dollar               = 0x02000104,  // M-$
  Fmkey_percent              = 0x02000105,  // M-%
  Fmkey_ampersand            = 0x02000106,  // M-&
  Fmkey_apostrophe           = 0x02000107,  // M-'
  Fmkey_left_parenthesis     = 0x02000108,  // M-(
  Fmkey_right_parenthesis    = 0x02000109,  // M-)
  Fmkey_asterisk             = 0x0200010a,  // M-*
  Fmkey_plus                 = 0x0200010b,  // M-+
  Fmkey_comma                = 0x0200010c,  // M-,
  Fmkey_minus                = 0x0200010d,  // M-'-'
  Fmkey_full_stop            = 0x0200010e,  // M-.
  Fmkey_slash                = 0x0200010f,  // M-/
  Fmkey_0                    = 0x02000110,  // M-0
  Fmkey_1                    = 0x02000111,  // M-1
  Fmkey_2                    = 0x02000112,  // M-2
  Fmkey_3                    = 0x02000113,  // M-3
  Fmkey_4                    = 0x02000114,  // M-4
  Fmkey_5                    = 0x02000115,  // M-5
  Fmkey_6                    = 0x02000116,  // M-6
  Fmkey_7                    = 0x02000117,  // M-7
  Fmkey_8                    = 0x02000118,  // M-8
  Fmkey_9                    = 0x02000119,  // M-9
  Fmkey_colon                = 0x0200011a,  // M-:
  Fmkey_semicolon            = 0x0200011b,  // M-;
  Fmkey_less_than            = 0x0200011c,  // M-<
  Fmkey_equals               = 0x0200011d,  // M-=
  Fmkey_greater_than         = 0x0200011e,  // M->
  Fmkey_question_mark        = 0x0200011f,  // M-?
  Fmkey_at                   = 0x02000120,  // M-@
  Fmkey_A                    = 0x02000121,  // M-A
  Fmkey_B                    = 0x02000122,  // M-B
  Fmkey_C                    = 0x02000123,  // M-C
  Fmkey_D                    = 0x02000124,  // M-D
  Fmkey_E                    = 0x02000125,  // M-E
  Fmkey_F                    = 0x02000126,  // M-F
  Fmkey_G                    = 0x02000127,  // M-G
  Fmkey_H                    = 0x02000128,  // M-H
  Fmkey_I                    = 0x02000129,  // M-I
  Fmkey_J                    = 0x0200012a,  // M-J
  Fmkey_K                    = 0x0200012b,  // M-K
  Fmkey_L                    = 0x0200012c,  // M-L
  Fmkey_M                    = 0x0200012d,  // M-M
  Fmkey_N                    = 0x0200012e,  // M-N
  Fmkey_O                    = 0x0200012f,  // M-O
  Fmkey_P                    = 0x02000130,  // M-P
  Fmkey_Q                    = 0x02000131,  // M-Q
  Fmkey_R                    = 0x02000132,  // M-R
  Fmkey_S                    = 0x02000133,  // M-S
  Fmkey_T                    = 0x02000134,  // M-T
  Fmkey_U                    = 0x02000135,  // M-U
  Fmkey_V                    = 0x02000136,  // M-V
  Fmkey_W                    = 0x02000137,  // M-W
  Fmkey_X                    = 0x02000138,  // M-X
  Fmkey_Y                    = 0x02000139,  // M-Y
  Fmkey_Z                    = 0x0200013a,  // M-Z
  Fmkey_left_square_bracket  = 0x0200013b,  // M-[
  Fmkey_backslash            = 0x0200013c,  // M-'\'
  Fmkey_right_square_bracket = 0x0200013d,  // M-]
  Fmkey_caret                = 0x0200013e,  // M-^
  Fmkey_underscore           = 0x0200013f,  // M-_
  Fmkey_grave_accent         = 0x02000140,  // M-`
  Fmkey_a                    = 0x02000141,  // M-a
  Fmkey_b                    = 0x02000142,  // M-b
  Fmkey_c                    = 0x02000143,  // M-c
  Fmkey_d                    = 0x02000144,  // M-d
  Fmkey_e                    = 0x02000145,  // M-e
  Fmkey_f                    = 0x02000146,  // M-f
  Fmkey_g                    = 0x02000147,  // M-g
  Fmkey_h                    = 0x02000148,  // M-h
  Fmkey_i                    = 0x02000149,  // M-i
  Fmkey_j                    = 0x0200014a,  // M-j
  Fmkey_k                    = 0x0200014b,  // M-k
  Fmkey_l                    = 0x0200014c,  // M-l
  Fmkey_m                    = 0x0200014d,  // M-m
  Fmkey_n                    = 0x0200014e,  // M-n
  Fmkey_o                    = 0x0200014f,  // M-o
  Fmkey_p                    = 0x02000150,  // M-p
  Fmkey_q                    = 0x02000151,  // M-q
  Fmkey_r                    = 0x02000152,  // M-r
  Fmkey_s                    = 0x02000153,  // M-s
  Fmkey_t                    = 0x02000154,  // M-t
  Fmkey_u                    = 0x02000155,  // M-u
  Fmkey_v                    = 0x02000156,  // M-v
  Fmkey_w                    = 0x02000157,  // M-w
  Fmkey_x                    = 0x02000158,  // M-x
  Fmkey_y                    = 0x02000159,  // M-y
  Fmkey_z                    = 0x0200015a,  // M-z
  Fmkey_left_curly_bracket   = 0x0200015b,  // M-{
  Fmkey_vertical_bar         = 0x0200015c,  // M-|
  Fmkey_right_curly_bracket  = 0x0200015d,  // M-}
  Fmkey_tilde                = 0x0200015e   // M-~
};

// Console color names
enum colornames
{
  Black             = 0,
  Blue              = 1,
  Green             = 2,
  Cyan              = 3,
  Red               = 4,
  Magenta           = 5,
  Brown             = 6,
  LightGray         = 7,
  DarkGray          = 8,
  LightBlue         = 9,
  LightGreen        = 10,
  LightCyan         = 11,
  LightRed          = 12,
  LightMagenta      = 13,
  Yellow            = 14,
  White             = 15,
/* 256 color terminals */
  Grey0             = 16,   // #000000
  NavyBlue          = 17,   // #00005f
  DarkBlue          = 18,   // #000087
  Blue3             = 19,   // #0000af
  Blue2             = 20,   // #0000d7
  Blue1             = 21,   // #0000ff
  DarkGreen         = 22,   // #005f00
  DarkCyan2         = 23,   // #005f5f
  DeepSkyBlue6      = 24,   // #005f87
  DeepSkyBlue5      = 25,   // #005faf
  DodgerBlue3       = 26,   // #005fd7
  DodgerBlue2       = 27,   // #005fff
  Green4            = 28,   // #008700
  SpringGreen6      = 29,   // #00875f
  Turquoise4        = 30,   // #008787
  DeepSkyBlue4      = 31,   // #0087af
  DeepSkyBlue3      = 32,   // #0087d7
  DodgerBlue1       = 33,   // #0087ff
  Green2            = 34,   // #00af00
  SpringGreen5      = 35,   // #00af5f
  DarkCyan          = 36,   // #00af87
  LightSeaGreen     = 37,   // #00afaf
  DeepSkyBlue2      = 38,   // #00afd7
  DeepSkyBlue1      = 39,   // #00afff
  Green3            = 40,   // #00d700
  SpringGreen4      = 41,   // #00d75f
  SpringGreen3      = 42,   // #00d787
  Cyan3             = 43,   // #00d7af
  DarkTurquoise     = 44,   // #00d7d7
  Turquoise2        = 45,   // #00d7ff
  Green1            = 46,   // #00ff00
  SpringGreen2      = 47,   // #00ff5f
  SpringGreen1      = 48,   // #00ff87
  MediumSpringGreen = 49,   // #00ffaf
  Cyan2             = 50,   // #00ffd7
  Cyan1             = 51,   // #00ffff
  DarkRed           = 52,   // #5f0000
  DeepPink7         = 53,   // #5f005f
  Purple4           = 54,   // #5f0087
  Purple3           = 55,   // #5f00af
  Purple2           = 56,   // #5f00d7
  BlueViolet        = 57,   // #5f00ff
  Orange4           = 58,   // #5f5f00
  Grey37            = 59,   // #5f5f5f
  MediumPurple6     = 60,   // #5f5f87
  SlateBlue3        = 61,   // #5f5faf
  SlateBlue2        = 62,   // #5f5fd7
  RoyalBlue1        = 63,   // #5f5fff
  Chartreuse5       = 64,   // #5f8700
  DarkSeaGreen8     = 65,   // #5f875f
  PaleTurquoise4    = 66,   // #5f8787
  SteelBlue         = 67,   // #5f87af
  SteelBlue3        = 68,   // #5f87d7
  CornflowerBlue    = 69,   // #5f87ff
  Chartreuse4       = 70,   // #5faf00
  DarkSeaGreen7     = 71,   // #5faf5f
  CadetBlue2        = 72,   // #5faf87
  CadetBlue1        = 73,   // #5fafaf
  SkyBlue3          = 74,   // #5fafd7
  SteelBlue2        = 75,   // #5fafff
  Chartreuse3       = 76,   // #5fd700
  PaleGreen3        = 77,   // #5fd75f
  SeaGreen4         = 78,   // #5fd787
  Aquamarine3       = 79,   // #5fd7af
  MediumTurquoise   = 80,   // #5fd7d7
  SteelBlue1        = 81,   // #5fd7ff
  Chartreuse        = 82,   // #5fff00
  SeaGreen3         = 83,   // #5fff5f
  SeaGreen2         = 84,   // #5fff87
  SeaGreen1         = 85,   // #5fffaf
  Aquamarine2       = 86,   // #5fffd7
  DarkSlateGray2    = 87,   // #5fffff
  DarkRed2          = 88,   // #870000
  DeepPink6         = 89,   // #87005f
  DarkMagenta2      = 90,   // #870087
  DarkMagenta       = 91,   // #8700af
  DarkViolet1       = 92,   // #8700d7
  Purple1           = 93,   // #8700ff
  Orange3           = 94,   // #875f00
  LightPink4        = 95,   // #875f5f
  Plum4             = 96,   // #875f87
  MediumPurple5     = 97,   // #875faf
  MediumPurple4     = 98,   // #875fd7
  SlateBlue1        = 99,   // #875fff
  Yellow6           = 100,  // #878700
  Wheat4            = 101,  // #87875f
  Grey53            = 102,  // #878787
  LightSlateGrey    = 103,  // #8787af
  MediumPurple      = 104,  // #8787d7
  LightSlateBlue    = 105,  // #8787ff
  Yellow5           = 106,  // #87af00
  DarkOliveGreen5   = 107,  // #87af5f
  DarkSeaGreen6     = 108,  // #87af87
  LightSkyBlue3     = 109,  // #87afaf
  LightSkyBlue2     = 110,  // #87afd7
  SkyBlue2          = 111,  // #87afff
  Chartreuse2       = 112,  // #87d700
  DarkOliveGreen4   = 113,  // #87d75f
  PaleGreen2        = 114,  // #87d787
  DarkSeaGreen5     = 115,  // #87d7af
  DarkSlateGray3    = 116,  // #87d7d7
  SkyBlue1          = 117,  // #87d7ff
  Chartreuse1       = 118,  // #87ff00
  LightGreen3       = 119,  // #87ff5f
  LightGreen2       = 120,  // #87ff87
  PaleGreen1        = 121,  // #87ffaf
  Aquamarine1       = 122,  // #87ffd7
  DarkSlateGray1    = 123,  // #87ffff
  Red3              = 124,  // #af0000
  DeepPink5         = 125,  // #af005f
  MediumVioletRed   = 126,  // #af0087
  Magenta6          = 127,  // #af00af
  DarkViolet        = 128,  // #af00d7
  Purple            = 129,  // #af00ff
  DarkOrange3       = 130,  // #af5f00
  IndianRed3        = 131,  // #af5f5f
  HotPink4          = 132,  // #af5f87
  MediumOrchid3     = 133,  // #af5faf
  MediumOrchid      = 134,  // #af5fd7
  MediumPurple3     = 135,  // #af5fff
  DarkGoldenrod     = 136,  // #af8700
  LightSalmon3      = 137,  // #af875f
  RosyBrown         = 138,  // #af8787
  Grey63            = 139,  // #af87af
  MediumPurple2     = 140,  // #af87d7
  MediumPurple1     = 141,  // #af87ff
  Gold3             = 142,  // #afaf00
  DarkKhaki         = 143,  // #afaf5f
  NavajoWhite3      = 144,  // #afaf87
  Grey69            = 145,  // #afafaf
  LightSteelBlue3   = 146,  // #afafd7
  LightSteelBlue    = 147,  // #afafff
  Yellow4           = 148,  // #afd700
  DarkOliveGreen3   = 149,  // #afd75f
  DarkSeaGreen4     = 150,  // #afd787
  DarkSeaGreen3     = 151,  // #afd7af
  LightCyan3        = 152,  // #afd7d7
  LightSkyBlue1     = 153,  // #afd7ff
  GreenYellow       = 154,  // #afff00
  DarkOliveGreen2   = 155,  // #afff5f
  PaleGreen         = 156,  // #afff87
  DarkSeaGreen2     = 157,  // #afffaf
  DarkSeaGreen1     = 158,  // #afffd7
  PaleTurquoise1    = 159,  // #afffff
  Red2              = 160,  // #d70000
  DeepPink4         = 161,  // #d7005f
  DeepPink3         = 162,  // #d70087
  Magenta5          = 163,  // #d700af
  Magenta4          = 164,  // #d700d7
  Magenta3          = 165,  // #d700ff
  DarkOrange2       = 166,  // #d75f00
  IndianRed         = 167,  // #d75f5f
  HotPink3          = 168,  // #d75f87
  HotPink2          = 169,  // #d75faf
  Orchid            = 170,  // #d75fd7
  MediumOrchid2     = 171,  // #d75fff
  Orange2           = 172,  // #d78700
  LightSalmon2      = 173,  // #d7875f
  LightPink3        = 174,  // #d78787
  Pink3             = 175,  // #d787af
  Plum3             = 176,  // #d787d7
  Violet            = 177,  // #d787ff
  Gold2             = 178,  // #d7af00
  LightGoldenrod5   = 179,  // #d7af5f
  Tan               = 180,  // #d7af87
  MistyRose3        = 181,  // #d7afaf
  Thistle3          = 182,  // #d7afd7
  Plum2             = 183,  // #d7afff
  Yellow3           = 184,  // #d7d700
  Khaki3            = 185,  // #d7d75f
  LightGoldenrod4   = 186,  // #d7d787
  LightYellow3      = 187,  // #d7d7af
  Grey84            = 188,  // #d7d7d7
  LightSteelBlue1   = 189,  // #d7d7ff
  Yellow2           = 190,  // #d7ff00
  DarkOliveGreen1   = 191,  // #d7ff5f
  DarkOliveGreen    = 192,  // #d7ff87
  DarkSeaGreen      = 193,  // #d7ffaf
  Honeydew2         = 194,  // #d7ffd7
  LightCyan1        = 195,  // #d7ffff
  Red1              = 196,  // #ff0000
  DeepPink2         = 197,  // #ff005f
  DeepPink1         = 198,  // #ff0087
  DeepPink          = 199,  // #ff00af
  Magenta2          = 200,  // #ff00d7
  Magenta1          = 201,  // #ff00ff
  OrangeRed1        = 202,  // #ff5f00
  IndianRed2        = 203,  // #ff5f5f
  IndianRed1        = 204,  // #ff5f87
  HotPink1          = 205,  // #ff5faf
  HotPink           = 206,  // #ff5fd7
  MediumOrchid1     = 207,  // #ff5fff
  DarkOrange        = 208,  // #ff8700
  Salmon1           = 209,  // #ff875f
  LightCoral        = 210,  // #ff8787
  PaleVioletRed1    = 211,  // #ff87af
  Orchid2           = 212,  // #ff87d7
  Orchid1           = 213,  // #ff87ff
  Orange1           = 214,  // #ffaf00
  SandyBrown        = 215,  // #ffaf5f
  LightSalmon1      = 216,  // #ffaf87
  LightPink1        = 217,  // #ffafaf
  Pink1             = 218,  // #ffafd7
  Plum1             = 219,  // #ffafff
  Gold1             = 220,  // #ffd700
  LightGoldenrod3   = 221,  // #ffd75f
  LightGoldenrod2   = 222,  // #ffd787
  NavajoWhite1      = 223,  // #ffd7af
  MistyRose1        = 224,  // #ffd7d7
  Thistle1          = 225,  // #ffd7ff
  Yellow1           = 226,  // #ffff00
  LightGoldenrod    = 227,  // #ffff5f
  Khaki1            = 228,  // #ffff87
  Wheat1            = 229,  // #ffffaf
  Cornsilk1         = 230,  // #ffffd7
  Grey100           = 231,  // #ffffff
  Grey3             = 232,  // #080808
  Grey7             = 233,  // #121212
  Grey11            = 234,  // #1c1c1c
  Grey15            = 235,  // #262626
  Grey19            = 236,  // #303030
  Grey23            = 237,  // #3a3a3a
  Grey27            = 238,  // #444444
  Grey30            = 239,  // #4e4e4e
  Grey35            = 240,  // #585858
  Grey39            = 241,  // #626262
  Grey42            = 242,  // #6c6c6c
  Grey46            = 243,  // #767676
  Grey50            = 244,  // #808080
  Grey54            = 245,  // #8a8a8a
  Grey58            = 246,  // #949494
  Grey62            = 247,  // #9e9e9e
  Grey66            = 248,  // #a8a8a8
  Grey70            = 249,  // #b2b2b2
  Grey74            = 250,  // #bcbcbc
  Grey78            = 251,  // #c6c6c6
  Grey82            = 252,  // #d0d0d0
  Grey85            = 253,  // #dadada
  Grey89            = 254,  // #e4e4e4
  Grey93            = 255,  // #eeeeee
  Default           = static_cast<FColor>(-1)
};

// Mouse/keyboard state values
enum ButtonState
{
  NoButton        = 0x00,
  LeftButton      = 0x01,
  RightButton     = 0x02,
  MiddleButton    = 0x04,
  MouseButtonMask = 0x07,
  ShiftButton     = 0x08,
  ControlButton   = 0x10,
  MetaButton      = 0x20,
  KeyButtonMask   = 0x38
};

// Wheel state values
enum WheelState
{
  NoWheel   = 0x00,
  WheelUp   = 0x01,
  WheelDown = 0x02,
  WheelMask = 0x03
};

// Type of focus
enum FocusTypes
{
  FocusNextWidget     = 0x00,
  FocusPreviousWidget = 0x01,
  FocusDefiniteWidget = 0x03  // event default
};

// Drag scrolling mode
enum dragScroll
{
  noScroll         = 0,
  scrollUp         = 1,
  scrollDown       = 2,
  scrollUpSelect   = 3,
  scrollDownSelect = 4
};

// Scroll bar visibility mode
enum scrollBarMode
{
  Auto   = 0,  // Shows a scroll bar when area is larger than viewport
  Hidden = 1,  // Never shows a scroll bar
  Scroll = 2   // Always shows a scroll bar
};

// Xterm cursor style
enum xtermCursorStyle
{
  unknown_cursor_style   = -1,
  blinking_block         = 0,
  blinking_block_default = 1,
  steady_block           = 2,
  blinking_underline     = 3,
  steady_underline       = 4,
  blinking_bar_xterm     = 5,
  steady_bar_xterm       = 6
};

// Linux console and framebuffer cursor style
enum linuxConsoleCursorStyle
{
  default_cursor     = 0,
  invisible_cursor   = 1,
  underscore_cursor  = 2,
  lower_third_cursor = 3,
  lower_half_cursor  = 4,
  two_thirds_cursor  = 5,
  full_block_cursor  = 6
};

// BSD console cursor style
enum freebsdConsoleCursorStyle
{
  normal_cursor      = 0,
  blink_cursor       = 1,
  destructive_cursor = 3
};

// KDE konsole cursor style
enum kdeKonsoleCursorShape
{
  BlockCursor     = 0,
  IBeamCursor     = 1,
  UnderlineCursor = 2
};

enum text_alignment
{
  alignLeft   = 1,
  alignCenter = 2,
  alignRight  = 3
};

enum orientation
{
  vertical   = 0,
  horizontal = 1
};

enum sides
{
  top    = 0,
  right  = 1,
  bottom = 2,
  left   = 3
};

enum sorting_type
{
  by_name,
  by_number,
  user_defined,
  unknown
};

enum sorting_order
{
  ascending,
  descending,
  unsorted
};

enum brackets_type
{
  NoBrackets     = 0,
  SquareBrackets = 1,  // [ ]
  Parenthesis    = 2,  // ( )
  CurlyBrackets  = 3,  // { }
  AngleBrackets  = 4   // < >
};

enum termcaps
{
  t_bell,
  t_erase_chars,
  t_clear_screen,
  t_clr_eos,
  t_clr_eol,
  t_clr_bol,
  t_cursor_home,
  t_cursor_to_ll,
  t_carriage_return,
  t_tab,
  t_back_tab,
  t_insert_padding,
  t_insert_character,
  t_parm_ich,
  t_repeat_char,
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
  t_cursor_style,
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

}  // namespace fc

}  // namespace finalcut

#endif  // FC_H
