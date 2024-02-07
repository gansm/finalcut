/***********************************************************************
* fc.h - Implements global constants and enumerations                  *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2024 Markus Gans                                      *
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

#ifndef FC_H
#define FC_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fconfig.h"
#include "final/ftypes.h"


// ASCII sequences
#define ESC      "\033"     // Escape
#define CSI      ESC "["    // Control sequence introducer (7-bit)
#define ENQ      "\005"     // Enquiry
#define BEL      "\007"     // Bell (ctrl‐g)
#define BS       "\010"     // Backspace
#define SO       "\016"     // Shift out (alternative character set)
#define SI       "\017"     // Shift in  (regular character set)
#define OSC      ESC "]"    // Operating system command (7-bit)

namespace finalcut
{

constexpr char fc_release[] = F_VERSION;

//----------------------------------------------------------------------
// Global constants and enumerations
//----------------------------------------------------------------------

// Event types
enum class Event
{
  None,              // invalid event
  KeyPress,          // key pressed
  KeyUp,             // key released
  KeyDown,           // key pressed
  MouseDown,         // mouse button pressed
  MouseUp,           // mouse button released
  MouseDoubleClick,  // mouse button double click
  MouseWheel,        // mouse wheel rolled
  MouseMove,         // mouse move
  FocusIn,           // focus in
  FocusOut,          // focus out
  ChildFocusIn,      // child focus in
  ChildFocusOut,     // child focus out
  FailAtChildFocus,  // No further focusable child widgets
  TerminalFocusIn,   // terminal focus in
  TerminalFocusOut,  // terminal focus out
  WindowActive,      // activate window
  WindowInactive,    // deactivate window
  WindowRaised,      // raise window
  WindowLowered,     // lower window
  Accelerator,       // keyboard accelerator
  Resize,            // terminal resize
  Show,              // widget is shown
  Hide,              // widget is hidden
  Close,             // widget close
  Timer,             // timer event occur
  User               // user defined event
};

// Internal character encoding
enum class Encoding
{
  UTF8,
  VT100,
  PC,
  ASCII,
  NUM_OF_ENCODINGS,  // number of items
  Unknown
};

// VT100 line graphic keys
enum class VT100Key : char
{
  rarrow    = '+',   // ►  -  arrow pointing right
  larrow    = ',',   // ◄  -  arrow pointing left
  uarrow    = '-',   // ▲  -  arrow pointing up
  darrow    = '.',   // ▼  -  arrow pointing down
  block     = '0',   // █  -  solid square block
  nsup      = 'I',   // ⁿ  -  superscript letter n
  blackrect = '_',   // ▮  -  black vertical rectangle
  diamond   = '`',   // ◆  -  diamond
  ckboard   = 'a',   // ▒  -  checker board (stipple)
  htab      = 'b',   // ␉  -  horizontal tab symbol
  ff        = 'c',   // ␌  -  form feed symbol
  cr        = 'd',   // ␍  -  carriage return symbol
  lf        = 'e',   // ␊  -  line feed symbol
  degree    = 'f',   // °  -  degree symbol
  plminus   = 'g',   // ±  -  plus/minus
  board     = 'h',   // ␤  -  board of squares
  lantern   = 'i',   // ␋  -  lantern symbol
  lrcorner  = 'j',   // ┘  -  lower right corner
  urcorner  = 'k',   // ┐  -  upper right corner
  ulcorner  = 'l',   // ┌  -  upper left corner
  llcorner  = 'm',   // └  -  lower left corner
  plus      = 'n',   // ┼  -  large plus or crossover
  s1        = 'o',   // ⎺  -  scan line 1
  s3        = 'p',   // ⎻  -  scan line 3
  hline     = 'q',   // ─  -  horizontal line
  s7        = 'r',   // ⎼  -  scan line 7
  s9        = 's',   // ⎽  -  scan line 9
  ltee      = 't',   // ├  -  tee pointing right
  rtee      = 'u',   // ┤  -  tee pointing left
  btee      = 'v',   // ┴  -  tee pointing up
  ttee      = 'w',   // ┬  -  tee pointing down
  vline     = 'x',   // │  -  vertical line
  lequal    = 'y',   // ≤  -  less-than-or-equal-to
  gequal    = 'z',   // ≥  -  greater-than-or-equal-to
  pi        = '{',   // π  -  greek pi
  nequal    = '|',   // ≠  -  not-equal
  sterling  = '}',   // £  -  UK pound sign
  bullet    = '~'    // ·  -  bullet
};

// Unicode characters
enum class UniChar : wchar_t
{
  Euro                                = 0x20ac,  // €
  Pound                               = 0x00a3,  // £
  Section                             = 0x00a7,  // §
  Pi                                  = 0x03c0,  // π
  InverseBullet                       = 0x25d8,  // ◘
  InverseWhiteCircle                  = 0x25d9,  // ◙
  UpDownArrow                         = 0x2195,  // ↕
  LeftRightArrow                      = 0x2194,  // ↔
  BlackRectangle                      = 0x25ac,  // ▬
  UpwardsArrow                        = 0x2191,  // ↑
  DownwardsArrow                      = 0x2193,  // ↓
  RightwardsArrow                     = 0x2192,  // →
  LeftwardsArrow                      = 0x2190,  // ←
  SingleRightAngleQuotationMark       = 0x203a,  // › (1)
  SingleLeftAngleQuotationMark        = 0x2039,  // ‹ (1)
  HorizontalEllipsis                  = 0x2026,  // … (1)
  DoubleExclamationMark               = 0x203c,  // ‼
  SuperscriptLatinSmallLetterN        = 0x207f,  // ⁿ
  GreaterThanOrEqualTo                = 0x2265,  // ≥
  LessThanOrEqualTo                   = 0x2264,  // ≤
  NotEqualTo                          = 0x2260,  // ≠
  PlusMinus                           = 0x00b1,  // ±
  Times                               = 0x00d7,  // ×
  Degree                              = 0x00b0,  // °
  BlackVerticalRectangle              = 0x25ae,  // ▮ (1)
  SmallBullet                         = 0x00b7,  // ·
  BlackDiamondSuit                    = 0x2666,  // ◆
  BlackCircle                         = 0x25cf,  // ●
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
  NF_border_line_vertical             = 0xe1b3,  // │ (2)
  NF_rev_left_arrow2                  = 0xe1b4,  // ⊐ (2)
  NF_rev_right_arrow2                 = 0xe1b5,  // ► (2)
  NF_border_line_left_up              = 0xe1b6,  // ╵ (2)
  NF_radio_button3                    = 0xe1b7,  // ) (2)
  NF_rev_border_corner_upper_right    = 0xe1b8,  // ⎤ (2)
  NF_rev_border_line_right            = 0xe1b9,  // ⎹ (2)
  NF_rev_border_line_vertical_left    = 0xe1ba,  // ┤ (2)
  NF_rev_border_corner_lower_right    = 0xe1bb,  // ⎦ (2)
  NF_border_line_left                 = 0xe1bc,  // ⎸ (2)
  NF_rev_up_arrow2                    = 0xe1bd,  //⎹◣ (2)
  NF_rev_down_arrow2                  = 0xe1be,  //⎹◤ (2)
  NF_border_line_left_down            = 0xe1bf,  // ╷ (2)
  NF_border_corner_middle_lower_left  = 0xe1c0,  // └ (2)
  NF_rev_up_arrow1                    = 0xe1c1,  // ◢⎸ (2)
  NF_rev_down_arrow1                  = 0xe1c2,  // ◥⎸ (2)
  NF_border_line_vertical_right       = 0xe1c3,  // ├ (2)
  NF_border_line_horizontal           = 0xe1c4,  // │ (2)
  NF_border_line_up_and_down          = 0xe1c5,  // ニ (2)
  NF_shadow_box_middle                = 0xe1c6,  // ニ (2)
  NF_shadow_box_hdd                   = 0xe1c7,  // ニ (2)
  NF_rev_left_arrow1                  = 0xe1c8,  // ◄ (2)
  NF_rev_right_arrow1                 = 0xe1c9,  // ⊏ (2)
  NF_rev_menu_button1                 = 0xe1ca,  // [ (2)
  NF_rev_menu_button2                 = 0xe1cb,  // - (2)
  NF_border_corner_middle_upper_left  = 0xe1cc,  // ┌ (2)
  NF_shadow_box_cd                    = 0xe1cd,  // ニ (2)
  NF_shadow_box_left                  = 0xe1ce,  // [ (2)
  NF_border_line_middle_left_down     = 0xe1cf,  // ┌ (2)
  NF_border_line_middle_right_up      = 0xe1d0,  // └ (2)
  NF_shadow_box_net                   = 0xe1d1,  // ニ (2)
  NF_rev_up_pointing_triangle1        = 0xe1d2,  // ◢ (2)
  NF_border_corner_lower_left         = 0xe1d3,  // ⎣ (2)
  NF_border_line_bottom               = 0xe1d4,  // _ (2)
  NF_radio_button2                    = 0xe1d5,  // ⁐ (2)
  NF_radio_button2_checked            = 0xe1d6,  // ● (2)
  NF_rev_down_pointing_triangle1      = 0xe1d7,  // ◥ (2)
  NF_border_line_upper                = 0xe1d8,  // ¯ (2)
  NF_radio_button1                    = 0xe1d9,  // ( (2)
  NF_border_corner_upper_left         = 0xe1da,  // ⎡ (2)
  NF_shadow_box_checked               = 0xe1dc,  // ✓ (2)
  NF_rev_border_line_right_and_left   = 0xe1e7,  // || (2)
  NF_rev_up_pointing_triangle2        = 0xe1e8,  // ◣ (2)
  NF_rev_down_pointing_triangle2      = 0xe1e9,  // ◤ (2)
  NF_border_corner_middle_lower_right = 0xe1ea,  //  ┘ (2)
  NF_border_corner_middle_upper_right = 0xe1eb,  //  ┐ (2)
  NF_rev_menu_button3                 = 0xe1f4,  // ] (2)
  NF_shadow_box_right                 = 0xe1f5,  // ] (2)
  NF_Bullet                           = 0xe1f9,  // ● (2)
  NF_check_mark                       = 0xe1fb,  // ✓ (2)
  NF_x_superior                       = 0xe1fc,  // ˣ (2)
  SquareRoot                          = 0x221a   // √
};
/*
 * (1) Not defined in Windows Glyph List 4 (WGL4)
 * (2) Only supported in use with newfont
 */

constexpr auto operator < (const UniChar& c1, const wchar_t c2) noexcept -> bool
{
  return wchar_t(c1) < c2;
}

constexpr auto operator > (const UniChar& c1, const wchar_t c2) noexcept -> bool
{
  return wchar_t(c1) > c2;
}

constexpr auto operator < (const wchar_t c1, const UniChar& c2) noexcept -> bool
{
  return c1 < wchar_t(c2);
}

constexpr auto operator > (const wchar_t c1, const UniChar& c2) noexcept -> bool
{
  return c1 > wchar_t(c2);
}

constexpr auto operator <= (const UniChar& c1, const wchar_t c2) noexcept -> bool
{
  return wchar_t(c1) <= c2;
}

constexpr auto operator >= (const UniChar& c1, const wchar_t c2) noexcept -> bool
{
  return wchar_t(c1) >= c2;
}

constexpr auto operator <= (const wchar_t c1, const UniChar& c2) noexcept -> bool
{
  return c1 <= wchar_t(c2);
}

constexpr auto operator >= (const wchar_t c1, const UniChar& c2) noexcept -> bool
{
  return c1 >= wchar_t(c2);
}

constexpr auto operator == (const wchar_t c1, const UniChar& c2) noexcept -> bool
{
  return c1 == wchar_t(c2);
}

constexpr auto operator == (const UniChar& c1, const wchar_t c2) noexcept -> bool
{
  return wchar_t(c1) == c2;
}


// keyboard - single keys and modifier key combinations
enum class FKey : uInt32
{
  None                       = 0x00000000,  // no key
  Ctrl_a                     = 0x00000001,  // control-a
  Ctrl_b                     = 0x00000002,  // control-b
  Ctrl_c                     = 0x00000003,  // control-c
  Ctrl_d                     = 0x00000004,  // control-d
  Ctrl_e                     = 0x00000005,  // control-e
  Ctrl_f                     = 0x00000006,  // control-f
  Ctrl_g                     = 0x00000007,  // control-g
  Ctrl_h                     = 0x00000008,  // control-h
  Erase                      = Ctrl_h,      // erase
  Ctrl_i                     = 0x00000009,  // control-i
  Tab                        = Ctrl_i,      // tabulator
  Ctrl_j                     = 0x0000000a,  // control-j
  Ctrl_k                     = 0x0000000b,  // control-k
  Ctrl_l                     = 0x0000000c,  // control-l
  Ctrl_m                     = 0x0000000d,  // control-m
  Return                     = Ctrl_m,      // return
  Ctrl_n                     = 0x0000000e,  // control-n
  Ctrl_o                     = 0x0000000f,  // control-o
  Ctrl_p                     = 0x00000010,  // control-p
  Ctrl_q                     = 0x00000011,  // control-q
  Ctrl_r                     = 0x00000012,  // control-r
  Ctrl_s                     = 0x00000013,  // control-s
  Ctrl_t                     = 0x00000014,  // control-t
  Ctrl_u                     = 0x00000015,  // control-u
  Ctrl_v                     = 0x00000016,  // control-v
  Ctrl_w                     = 0x00000017,  // control-w
  Ctrl_x                     = 0x00000018,  // control-x
  Ctrl_y                     = 0x00000019,  // control-y
  Ctrl_z                     = 0x0000001a,  // control-t
  Escape                     = 0x0000001b,  // control-[
  Ctrl_backslash             = 0x0000001c,  // control-'\'
  Ctrl_right_square_bracket  = 0x0000001d,  // control-]
  Ctrl_caret                 = 0x0000001e,  // control-^
  Ctrl_underscore            = 0x0000001f,  // control-_
  Space                      = 0x00000020,  // space
  Exclamation_mark           = 0x00000021,  // !
  Quotation_mark             = 0x00000022,  // "
  Number_sign                = 0x00000023,  // #
  Dollar_sign                = 0x00000024,  // $
  Percent_sign               = 0x00000025,  // %
  Ampersand                  = 0x00000026,  // &
  Apostrophe                 = 0x00000027,  // '
  Left_parentheses           = 0x00000028,  // (
  Right_parentheses          = 0x00000029,  // )
  Asterisk                   = 0x0000002a,  // *
  Plus_sign                  = 0x0000002b,  // +
  Comma                      = 0x0000002c,  // ,
  Minus_sign                 = 0x0000002d,  // -
  Full_stop                  = 0x0000002e,  // .
  Slash                      = 0x0000002f,  // /
  Digit_0                    = 0x00000030,  // 0
  Digit_1                    = 0x00000031,  // 1
  Digit_2                    = 0x00000032,  // 2
  Digit_3                    = 0x00000033,  // 3
  Digit_4                    = 0x00000034,  // 4
  Digit_5                    = 0x00000035,  // 5
  Digit_6                    = 0x00000036,  // 6
  Digit_7                    = 0x00000037,  // 7
  Digit_8                    = 0x00000038,  // 8
  Digit_9                    = 0x00000039,  // 9
  Colon                      = 0x0000003a,  // :
  Semicolon                  = 0x0000003b,  // ;
  Less_than                  = 0x0000003c,  // <
  Equals                     = 0x0000003d,  // =
  Greater_than               = 0x0000003e,  // >
  Question_mark              = 0x0000003f,  // ?
  At                         = 0x00000040,  // @
  A                          = 0x00000041,  // A
  B                          = 0x00000042,  // B
  C                          = 0x00000043,  // C
  D                          = 0x00000044,  // D
  E                          = 0x00000045,  // E
  F                          = 0x00000046,  // F
  G                          = 0x00000047,  // G
  H                          = 0x00000048,  // H
  I                          = 0x00000049,  // I
  J                          = 0x0000004a,  // J
  K                          = 0x0000004b,  // K
  L                          = 0x0000004c,  // L
  M                          = 0x0000004d,  // M
  N                          = 0x0000004e,  // N
  O                          = 0x0000004f,  // O
  P                          = 0x00000050,  // P
  Q                          = 0x00000051,  // Q
  R                          = 0x00000052,  // R
  S                          = 0x00000053,  // S
  T                          = 0x00000054,  // T
  U                          = 0x00000055,  // U
  V                          = 0x00000056,  // V
  W                          = 0x00000057,  // W
  X                          = 0x00000058,  // X
  Y                          = 0x00000059,  // Y
  Z                          = 0x0000005a,  // Z
  Left_square_bracket        = 0x0000005b,  // [
  Backslash                  = 0x0000005c,  // '\'
  Right_square_bracket       = 0x0000005d,  // ]
  Caret                      = 0x0000005e,  // ^
  Underscore                 = 0x0000005f,  // _
  Grave_accent               = 0x00000060,  // `
  a                          = 0x00000061,  // a
  b                          = 0x00000062,  // b
  c                          = 0x00000063,  // c
  d                          = 0x00000064,  // d
  e                          = 0x00000065,  // e
  f                          = 0x00000066,  // f
  g                          = 0x00000067,  // g
  h                          = 0x00000068,  // h
  i                          = 0x00000069,  // i
  j                          = 0x0000006a,  // j
  k                          = 0x0000006b,  // k
  l                          = 0x0000006c,  // l
  m                          = 0x0000006d,  // m
  n                          = 0x0000006e,  // n
  o                          = 0x0000006f,  // o
  p                          = 0x00000070,  // p
  q                          = 0x00000071,  // q
  r                          = 0x00000072,  // r
  s                          = 0x00000073,  // s
  t                          = 0x00000074,  // t
  u                          = 0x00000075,  // u
  v                          = 0x00000076,  // v
  w                          = 0x00000077,  // w
  x                          = 0x00000078,  // x
  y                          = 0x00000079,  // y
  z                          = 0x0000007a,  // z
  Left_curly_bracket         = 0x0000007b,  // {
  Vertical_bar               = 0x0000007c,  // |
  Right_curly_bracket        = 0x0000007d,  // }
  Tilde                      = 0x0000007e,  // ~
  Ctrl_space                 = 0x01000020,  // control-space
  Backspace                  = 0x01000100,
  Clear_all_tabs             = 0x01000101,
  Clear                      = 0x01000102,
  Clear_tab                  = 0x01000103,
  Del_char                   = 0x01000104,
  Del_line                   = 0x01000105,
  Down                       = 0x01000106,
  Exit_insert                = 0x01000107,
  Clear_eol                  = 0x01000108,
  Clear_eos                  = 0x01000109,
  F0                         = 0x0100010a,
  F1                         = 0x0100010b,
  F2                         = 0x0100010c,
  F3                         = 0x0100010d,
  F4                         = 0x0100010e,
  F5                         = 0x0100010f,
  F6                         = 0x01000110,
  F7                         = 0x01000111,
  F8                         = 0x01000112,
  F9                         = 0x01000113,
  F10                        = 0x01000114,
  Home                       = 0x01000115,
  Insert                     = 0x01000116,  // insert key
  Insert_line                = 0x01000117,
  Left                       = 0x01000118,
  Home_down                  = 0x01000119,
  Page_down                  = 0x0100011a,
  Page_up                    = 0x0100011b,
  Right                      = 0x0100011c,
  Scroll_forward             = 0x0100011d,
  Shift_down                 = Scroll_forward,
  Scroll_backward            = 0x0100011e,
  Shift_up                   = Scroll_backward,
  Set_tab                    = 0x0100011f,
  Up                         = 0x01000120,
  Upper_left                 = 0x01000121,
  Upper_right                = 0x01000122,
  Center                     = 0x01000123,
  Lower_left                 = 0x01000124,
  Lower_right                = 0x01000125,
  Back_tab                   = 0x01000126,
  Begin                      = 0x01000127,
  Cancel                     = 0x01000128,
  Close                      = 0x01000129,
  Command                    = 0x0100012a,
  Copy                       = 0x0100012b,
  Create                     = 0x0100012c,
  End                        = 0x0100012d,
  Enter                      = 0x0100012e,
  Exit                       = 0x0100012f,
  Find                       = 0x01000130,
  Help                       = 0x01000131,
  Mark                       = 0x01000132,
  Message                    = 0x01000133,
  Move                       = 0x01000134,
  Next                       = 0x01000135,
  Open                       = 0x01000136,
  Options                    = 0x01000137,
  Previous                   = 0x01000138,
  Print                      = 0x01000139,
  Redo                       = 0x0100013a,
  Reference                  = 0x0100013b,
  Refresh                    = 0x0100013c,
  Replace                    = 0x0100013d,
  Restart                    = 0x0100013e,
  Resume                     = 0x0100013f,
  Save                       = 0x01000140,
  Suspend                    = 0x01000141,
  Undo                       = 0x01000142,
  Select                     = 0x01000143,
  Shift_begin                = 0x01000144,
  Shift_cancel               = 0x01000145,
  Shift_command              = 0x01000146,
  Shift_copy                 = 0x01000147,
  Shift_create               = 0x01000148,
  Shift_del_char             = 0x01000149,
  Shift_dl                   = 0x0100014a,
  Shift_end                  = 0x0100014b,
  Shift_clear_eol            = 0x0100014c,
  Shift_exit                 = 0x0100014d,
  Shift_find                 = 0x0100014e,
  Shift_help                 = 0x0100014f,
  Shift_home                 = 0x01000150,
  Shift_insert               = 0x01000151,
  Shift_left                 = 0x01000152,
  Shift_message              = 0x01000153,
  Shift_move                 = 0x01000154,
  Shift_page_down            = 0x01000155,
  Shift_options              = 0x01000156,
  Shift_page_up              = 0x01000157,
  Shift_print                = 0x01000158,
  Shift_redo                 = 0x01000159,
  Shift_replace              = 0x0100015a,
  Shift_right                = 0x0100015b,
  Shift_rsume                = 0x0100015c,
  Shift_save                 = 0x0100015d,
  Shift_suspend              = 0x0100015e,
  Shift_undo                 = 0x0100015f,
  F11                        = 0x01000160,
  F12                        = 0x01000161,
  F13                        = 0x01000162,
  F14                        = 0x01000163,
  F15                        = 0x01000164,
  F16                        = 0x01000165,
  F17                        = 0x01000166,
  F18                        = 0x01000167,
  F19                        = 0x01000168,
  F20                        = 0x01000169,
  F21                        = 0x0100016a,
  F22                        = 0x0100016b,
  F23                        = 0x0100016c,
  F24                        = 0x0100016d,
  F25                        = 0x0100016e,
  F26                        = 0x0100016f,
  F27                        = 0x01000170,
  F28                        = 0x01000171,
  F29                        = 0x01000172,
  F30                        = 0x01000173,
  F31                        = 0x01000174,
  F32                        = 0x01000175,
  F33                        = 0x01000176,
  F34                        = 0x01000177,
  F35                        = 0x01000178,
  F36                        = 0x01000179,
  F37                        = 0x0100017a,
  F38                        = 0x0100017b,
  F39                        = 0x0100017c,
  F40                        = 0x0100017d,
  F41                        = 0x0100017e,
  F42                        = 0x0100017f,
  F43                        = 0x01000180,
  F44                        = 0x01000181,
  F45                        = 0x01000182,
  F46                        = 0x01000183,
  F47                        = 0x01000184,
  F48                        = 0x01000185,
  F49                        = 0x01000186,
  F50                        = 0x01000187,
  F51                        = 0x01000188,
  F52                        = 0x01000189,
  F53                        = 0x0100018a,
  F54                        = 0x0100018b,
  F55                        = 0x0100018c,
  F56                        = 0x0100018d,
  F57                        = 0x0100018e,
  F58                        = 0x0100018f,
  F59                        = 0x01000190,
  F60                        = 0x01000191,
  F61                        = 0x01000192,
  F62                        = 0x01000193,
  F63                        = 0x01000194,
  Meta_insert                = 0x01500100,  // M-Insert
  Meta_del_char              = 0x01500101,  // M-Delete
  Meta_home                  = 0x01500102,  // M-Home
  Meta_end                   = 0x01500103,  // M-End
  Meta_page_up               = 0x01500104,  // M-Page-up
  Meta_page_down             = 0x01500105,  // M-Page-down
  Meta_f1                    = 0x01500106,  // M-F1
  Meta_f2                    = 0x01500107,  // M-F2
  Meta_f3                    = 0x01500108,  // M-F3
  Meta_f4                    = 0x01500109,  // M-F4
  Meta_f5                    = 0x0150010a,  // M-F5
  Meta_f6                    = 0x0150010b,  // M-F6
  Meta_f7                    = 0x0150010c,  // M-F7
  Meta_f8                    = 0x0150010d,  // M-F8
  Meta_f9                    = 0x0150010e,  // M-F9
  Meta_f10                   = 0x0150010f,  // M-F10
  Meta_f11                   = 0x01500110,  // M-F11
  Meta_f12                   = 0x01500111,  // M-F12
  Meta_up                    = 0x01500112,  // M-Up
  Meta_down                  = 0x01500113,  // M-Down
  Meta_right                 = 0x01500114,  // M-Right
  Meta_left                  = 0x01500115,  // M-Left
  Shift_Meta_insert          = 0x01500116,  // shifted M-Insert
  Shift_Meta_del_char        = 0x01500117,  // shifted M-Delete
  Shift_Meta_home            = 0x01500118,  // shifted M-Home
  Shift_Meta_end             = 0x01500119,  // shifted M-End
  Shift_Meta_page_up         = 0x0150011a,  // shifted M-Page-up
  Shift_Meta_page_down       = 0x0150011b,  // shifted M-Page-down
  Shift_Meta_f1              = 0x0150011c,  // shifted M-F1
  Shift_Meta_f2              = 0x0150011d,  // shifted M-F2
  Shift_Meta_f3              = 0x0150011e,  // shifted M-F3
  Shift_Meta_f4              = 0x0150011f,  // shifted M-F4
  Shift_Meta_f5              = 0x01500120,  // shifted M-F5
  Shift_Meta_f6              = 0x01500121,  // shifted M-F6
  Shift_Meta_f7              = 0x01500122,  // shifted M-F7
  Shift_Meta_f8              = 0x01500123,  // shifted M-F8
  Shift_Meta_f9              = 0x01500124,  // shifted M-F9
  Shift_Meta_f10             = 0x01500125,  // shifted M-F10
  Shift_Meta_f11             = 0x01500126,  // shifted M-F11
  Shift_Meta_f12             = 0x01500127,  // shifted M-F12
  Shift_Meta_up              = 0x01500128,  // shifted M-Up
  Shift_Meta_down            = 0x01500129,  // shifted M-Down
  Shift_Meta_right           = 0x0150012a,  // shifted M-Right
  Shift_Meta_left            = 0x0150012b,  // shifted M-Left
  Ctrl_insert                = 0x0150012c,  // control-Insert
  Ctrl_del_char              = 0x0150012d,  // control-Delete
  Ctrl_home                  = 0x0150012e,  // control-Home
  Ctrl_end                   = 0x0150012f,  // control-End
  Ctrl_page_up               = 0x01500130,  // control-Page-up
  Ctrl_page_down             = 0x01500131,  // control-Page-down
  Ctrl_up                    = 0x01500132,  // control-Up
  Ctrl_down                  = 0x01500133,  // control-Down
  Ctrl_right                 = 0x01500134,  // control-Right
  Ctrl_left                  = 0x01500135,  // control-Left
  Shift_Ctrl_insert          = 0x01500136,  // shifted control-Insert
  Shift_Ctrl_del_char        = 0x01500137,  // shifted control-Delete
  Shift_Ctrl_home            = 0x01500138,  // shifted control-Home
  Shift_Ctrl_end             = 0x01500139,  // shifted control-End
  Shift_Ctrl_page_up         = 0x0150013a,  // shifted control-Page-up
  Shift_Ctrl_page_down       = 0x0150013b,  // shifted control-Page-down
  Shift_Ctrl_up              = 0x0150013c,  // shifted control-Up
  Shift_Ctrl_down            = 0x0150013d,  // shifted control-Down
  Shift_Ctrl_right           = 0x0150013e,  // shifted control-Right
  Shift_Ctrl_left            = 0x0150013f,  // shifted control-Left
  Ctrl_Meta_insert           = 0x01500140,  // control-M-Insert
  Ctrl_Meta_del_char         = 0x01500141,  // control-M-Delete
  Ctrl_Meta_home             = 0x01500142,  // control-M-Home
  Ctrl_Meta_end              = 0x01500143,  // control-M-End
  Ctrl_Meta_page_up          = 0x01500144,  // control-M-Page-up
  Ctrl_Meta_page_down        = 0x01500145,  // control-M-Page-down
  Ctrl_Meta_up               = 0x01500146,  // control-M-Up
  Ctrl_Meta_down             = 0x01500147,  // control-M-Down
  Ctrl_Meta_right            = 0x01500148,  // control-M-Right
  Ctrl_Meta_left             = 0x01500149,  // control-M-Left
  Shift_Ctrl_Meta_insert     = 0x0150014a,  // shifted control-M-Insert
  Shift_Ctrl_Meta_del_char   = 0x0150014b,  // shifted control-M-Delete
  Shift_Ctrl_Meta_home       = 0x0150014c,  // shifted control-M-Home
  Shift_Ctrl_Meta_end        = 0x0150014d,  // shifted control-M-End
  Shift_Ctrl_Meta_page_up    = 0x0150014e,  // shifted control-M-Page-up
  Shift_Ctrl_Meta_page_down  = 0x0150014f,  // shifted control-M-Page-down
  Shift_Ctrl_Meta_f1         = 0x01500150,  // shifted control-M-F1
  Shift_Ctrl_Meta_f2         = 0x01500151,  // shifted control-M-F2
  Shift_Ctrl_Meta_f3         = 0x01500152,  // shifted control-M-F3
  Shift_Ctrl_Meta_f4         = 0x01500153,  // shifted control-M-F4
  Shift_Ctrl_Meta_f5         = 0x01500154,  // shifted control-M-F5
  Shift_Ctrl_Meta_f6         = 0x01500155,  // shifted control-M-F6
  Shift_Ctrl_Meta_f7         = 0x01500156,  // shifted control-M-F7
  Shift_Ctrl_Meta_f8         = 0x01500157,  // shifted control-M-F8
  Shift_Ctrl_Meta_f9         = 0x01500158,  // shifted control-M-F9
  Shift_Ctrl_Meta_f10        = 0x01500159,  // shifted control-M-F10
  Shift_Ctrl_Meta_f11        = 0x0150015a,  // shifted control-M-F11
  Shift_Ctrl_Meta_f12        = 0x0150015b,  // shifted control-M-F12
  Shift_Ctrl_Meta_up         = 0x0150015c,  // shifted control-M-Up
  Shift_Ctrl_Meta_down       = 0x0150015d,  // shifted control-M-Down
  Shift_Ctrl_Meta_right      = 0x0150015e,  // shifted control-M-Right
  Shift_Ctrl_Meta_left       = 0x0150015f,  // shifted control-M-Left
  Menu                       = 0x01600000,  // menu
  Shift_menu                 = 0x01600001,  // shifted menu
  Ctrl_menu                  = 0x01600002,  // control-menu
  Shift_Ctrl_menu            = 0x01600003,  // shifted control-menu
  Meta_menu                  = 0x01600004,  // M-menu
  Shift_Meta_menu            = 0x01600005,  // shifted M-menu
  Ctrl_Meta_menu             = 0x01600006,  // control-M-menu
  Shift_Ctrl_Meta_menu       = 0x01600007,  // shifted control-M-menu
  Term_Focus_In              = 0x01900000,  // Terminal focus-in event
  Term_Focus_Out             = 0x01900001,  // Terminal focus-out event
  Escape_mintty              = 0x0200001b,  // mintty Esc
  X11mouse                   = 0x02000020,  // xterm mouse
  Extended_mouse             = 0x02000021,  // SGR extended mouse
  Urxvt_mouse                = 0x02000022,  // urxvt mouse extension
  Meta_offset                = 0x020000e0,  // meta key offset
  Meta_tab                   = 0x020000e9,  // M-tab
  Meta_enter                 = 0x020000ea,  // M-enter
  Meta_space                 = 0x02000100,  // M-' '
  Meta_bang                  = 0x02000101,  // M-!
  Meta_quotes                = 0x02000102,  // M-"
  Meta_hash                  = 0x02000103,  // M-#
  Meta_dollar                = 0x02000104,  // M-$
  Meta_percent               = 0x02000105,  // M-%
  Meta_ampersand             = 0x02000106,  // M-&
  Meta_apostrophe            = 0x02000107,  // M-'
  Meta_left_parenthesis      = 0x02000108,  // M-(
  Meta_right_parenthesis     = 0x02000109,  // M-)
  Meta_asterisk              = 0x0200010a,  // M-*
  Meta_plus                  = 0x0200010b,  // M-+
  Meta_comma                 = 0x0200010c,  // M-,
  Meta_minus                 = 0x0200010d,  // M-'-'
  Meta_full_stop             = 0x0200010e,  // M-.
  Meta_slash                 = 0x0200010f,  // M-/
  Meta_0                     = 0x02000110,  // M-0
  Meta_1                     = 0x02000111,  // M-1
  Meta_2                     = 0x02000112,  // M-2
  Meta_3                     = 0x02000113,  // M-3
  Meta_4                     = 0x02000114,  // M-4
  Meta_5                     = 0x02000115,  // M-5
  Meta_6                     = 0x02000116,  // M-6
  Meta_7                     = 0x02000117,  // M-7
  Meta_8                     = 0x02000118,  // M-8
  Meta_9                     = 0x02000119,  // M-9
  Meta_colon                 = 0x0200011a,  // M-:
  Meta_semicolon             = 0x0200011b,  // M-;
  Meta_less_than             = 0x0200011c,  // M-<
  Meta_equals                = 0x0200011d,  // M-=
  Meta_greater_than          = 0x0200011e,  // M->
  Meta_question_mark         = 0x0200011f,  // M-?
  Meta_at                    = 0x02000120,  // M-@
  Meta_A                     = 0x02000121,  // M-A
  Meta_B                     = 0x02000122,  // M-B
  Meta_C                     = 0x02000123,  // M-C
  Meta_D                     = 0x02000124,  // M-D
  Meta_E                     = 0x02000125,  // M-E
  Meta_F                     = 0x02000126,  // M-F
  Meta_G                     = 0x02000127,  // M-G
  Meta_H                     = 0x02000128,  // M-H
  Meta_I                     = 0x02000129,  // M-I
  Meta_J                     = 0x0200012a,  // M-J
  Meta_K                     = 0x0200012b,  // M-K
  Meta_L                     = 0x0200012c,  // M-L
  Meta_M                     = 0x0200012d,  // M-M
  Meta_N                     = 0x0200012e,  // M-N
  Meta_O                     = 0x0200012f,  // M-O
  Meta_P                     = 0x02000130,  // M-P
  Meta_Q                     = 0x02000131,  // M-Q
  Meta_R                     = 0x02000132,  // M-R
  Meta_S                     = 0x02000133,  // M-S
  Meta_T                     = 0x02000134,  // M-T
  Meta_U                     = 0x02000135,  // M-U
  Meta_V                     = 0x02000136,  // M-V
  Meta_W                     = 0x02000137,  // M-W
  Meta_X                     = 0x02000138,  // M-X
  Meta_Y                     = 0x02000139,  // M-Y
  Meta_Z                     = 0x0200013a,  // M-Z
  Meta_left_square_bracket   = 0x0200013b,  // M-[
  Meta_backslash             = 0x0200013c,  // M-'\'
  Meta_right_square_bracket  = 0x0200013d,  // M-]
  Meta_caret                 = 0x0200013e,  // M-^
  Meta_underscore            = 0x0200013f,  // M-_
  Meta_grave_accent          = 0x02000140,  // M-`
  Meta_a                     = 0x02000141,  // M-a
  Meta_b                     = 0x02000142,  // M-b
  Meta_c                     = 0x02000143,  // M-c
  Meta_d                     = 0x02000144,  // M-d
  Meta_e                     = 0x02000145,  // M-e
  Meta_f                     = 0x02000146,  // M-f
  Meta_g                     = 0x02000147,  // M-g
  Meta_h                     = 0x02000148,  // M-h
  Meta_i                     = 0x02000149,  // M-i
  Meta_j                     = 0x0200014a,  // M-j
  Meta_k                     = 0x0200014b,  // M-k
  Meta_l                     = 0x0200014c,  // M-l
  Meta_m                     = 0x0200014d,  // M-m
  Meta_n                     = 0x0200014e,  // M-n
  Meta_o                     = 0x0200014f,  // M-o
  Meta_p                     = 0x02000150,  // M-p
  Meta_q                     = 0x02000151,  // M-q
  Meta_r                     = 0x02000152,  // M-r
  Meta_s                     = 0x02000153,  // M-s
  Meta_t                     = 0x02000154,  // M-t
  Meta_u                     = 0x02000155,  // M-u
  Meta_v                     = 0x02000156,  // M-v
  Meta_w                     = 0x02000157,  // M-w
  Meta_x                     = 0x02000158,  // M-x
  Meta_y                     = 0x02000159,  // M-y
  Meta_z                     = 0x0200015a,  // M-z
  Meta_left_curly_bracket    = 0x0200015b,  // M-{
  Meta_vertical_bar          = 0x0200015c,  // M-|
  Meta_right_curly_bracket   = 0x0200015d,  // M-}
  Meta_tilde                 = 0x0200015e,  // M-~
  Incomplete                 = UINT32_MAX
};

constexpr auto operator >> (const FKey& k, const uInt32 n) noexcept -> FKey
{
  return FKey(uInt32(k) >> n);
}

constexpr auto operator << (const FKey& k, const uInt32 n) noexcept -> FKey
{
  return FKey(uInt32(k) << n);
}

constexpr auto operator < (const FKey& k1, const uInt32 k2) noexcept -> bool
{
  return uInt32(k1) < k2;
}

constexpr auto operator > (const FKey& k1, const uInt32 k2) noexcept -> bool
{
  return uInt32(k1) > k2;
}

constexpr auto operator <= (const FKey& k1, const uInt32 k2) noexcept -> bool
{
  return uInt32(k1) <= k2;
}

constexpr auto operator >= (const FKey& k1, const uInt32 k2) noexcept -> bool
{
  return uInt32(k1) >= k2;
}

constexpr auto operator | (const FKey& k1, const FKey& k2) noexcept -> FKey
{
  return FKey(uInt32(k1) | uInt32(k2));
}

constexpr auto operator & (const FKey& k1, const FKey& k2) noexcept -> FKey
{
  return FKey(uInt32(k1) & uInt32(k2));
}

constexpr auto operator + (const FKey& k1, const uInt32 k2) noexcept -> FKey
{
  return ( k1 < UINT32_MAX - k2 ) ? FKey(uInt32(k1) + k2) : FKey::None;
}

constexpr auto operator + (const FKey& k1, const FKey& k2) noexcept -> FKey
{
  return k1 + uInt32(k2);
}

constexpr auto operator - (const FKey& k1, const uInt32 k2) noexcept -> FKey
{
  return ( k1 > k2 ) ? FKey(uInt32(k1) - k2) : FKey::None;
}

constexpr auto operator - (const FKey& k1, const FKey& k2) noexcept -> FKey
{
  return k1 - uInt32(k2);
}

constexpr auto operator += (FKey& k1, uInt32 k2) noexcept -> FKey&
{
  k1 = ( k1 < UINT32_MAX - k2 ) ? FKey(uInt32(k1) + k2) : FKey::None;
  return k1;
}

constexpr auto operator -= (FKey& k1, uInt32 k2) noexcept -> FKey&
{
  k1 = ( k1 > k2 ) ? FKey(uInt32(k1) - k2) : FKey::None;
  return k1;
}


// Terminal color names
enum class FColor : uInt16
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
  Default           = static_cast<uInt16>(-1),
  Undefined         = static_cast<uInt16>(-2)
};

constexpr auto operator >> (const FColor& c, const uInt16 n) noexcept -> FColor
{
  return FColor(uInt16(c) >> n);
}

constexpr auto operator << (const FColor& c, const uInt16 n) noexcept -> FColor
{
  return FColor(uInt16(c) << n);
}

constexpr auto operator < (const FColor& c, const uInt16 n) noexcept -> bool
{
  return uInt16(c) < n;
}

constexpr auto operator > (const FColor& c, const uInt16 n) noexcept -> bool
{
  return uInt16(c) > n;
}

constexpr auto operator == (const FColor& c, const uInt16 n) noexcept -> bool
{
  return uInt16(c) == n;
}

constexpr auto operator <= (const FColor& c, const uInt16 n) noexcept -> bool
{
  return uInt16(c) <= n;
}

constexpr auto operator >= (const FColor& c, const uInt16 n) noexcept -> bool
{
  return uInt16(c) >= n;
}

constexpr auto operator + (const FColor& c, const uInt16 n) noexcept -> FColor
{
  return FColor(uInt16(c) + n);
}

constexpr auto operator - (const FColor& c, const uInt16 n) noexcept -> FColor
{
  return FColor(uInt16(c) - n);
}

constexpr auto operator % (const FColor& c, const uInt16 n) noexcept -> FColor
{
  return FColor(uInt16(c) % n);
}

constexpr auto operator %= (FColor& c, uInt16 n) noexcept -> FColor&
{
  c = FColor(uInt16(c) % n);
  return c;
}

constexpr auto operator ++ (FColor& c) noexcept -> FColor&  // prefix
{
  c = ( uInt16(c) < 255 ) ? FColor(uInt16(c) + 1) : FColor::Default;
  return c;
}

constexpr auto operator ++ (FColor& c, int) noexcept -> FColor  // postfix
{
  const FColor tmp = c;
  ++c;
  return tmp;
}

constexpr auto operator -- (FColor& c) noexcept -> FColor&  // prefix
{
  if ( uInt16(c) > 0 )
    return (c = FColor(uInt16(c) - 1));

  if ( c == FColor::Black )        // value 0
    return (c = FColor::Default);  // value uInt16(-1)

  return (c = FColor(255));        // --(-1) = 255
}

constexpr auto operator -- (FColor& c, int) noexcept -> FColor  // postfix
{
  const FColor tmp = c;
  --c;
  return tmp;
}


// Terminal attribute style names
enum class Style : uInt16
{
  None              = 0U,
  Bold              = 1U << 0U,
  Dim               = 1U << 1U,
  Italic            = 1U << 2U,
  Underline         = 1U << 3U,
  Blink             = 1U << 4U,
  Reverse           = 1U << 5U,
  Standout          = 1U << 6U,
  Invisible         = 1U << 7U,
  Protected         = 1U << 8U,
  CrossedOut        = 1U << 9U,
  DoubleUnderline   = 1U << 10U,
  Transparent       = 1U << 11U,
  ColorOverlay      = 1U << 12U,
  InheritBackground = 1U << 13U
};

constexpr auto operator + (const Style& a1, const Style& a2) noexcept -> Style
{
  return Style(uInt16(a1) + uInt16(a2));
}

constexpr auto operator - (const Style& a) noexcept -> Style
{
  return Style(-(uInt16(a)));
}

constexpr auto operator | (const Style& a1, const Style& a2) noexcept -> Style
{
  return Style(uInt16(a1) | uInt16(a2));
}

constexpr auto operator & (const Style& a1, const Style& a2) noexcept -> Style
{
  return Style(uInt16(a1) & uInt16(a2));
}

constexpr auto operator ^= (Style& a, const Style& b) noexcept -> Style&
{
  a = Style(uInt16(a) ^ uInt16(b));
  return a;
}


// Mouse/keyboard state values
enum class MouseButton : uInt
{
  None    = 0x00,
  Left    = 0x01,
  Right   = 0x02,
  Middle  = 0x04,
  Shift   = 0x08,
  Control = 0x10,
  Meta    = 0x20
};

constexpr auto operator | (const MouseButton& b1, const MouseButton& b2) noexcept -> MouseButton
{
  return MouseButton(uInt(b1) | uInt(b2));
}

constexpr auto operator |= (MouseButton& b1, const MouseButton& b2) noexcept -> MouseButton&
{
  b1 = b1 | b2;
  return b1;
}

constexpr auto operator & (const MouseButton& b1, const MouseButton& b2) noexcept -> MouseButton
{
  return MouseButton(uInt(b1) & uInt(b2));
}

constexpr auto operator &= (MouseButton& b1, const MouseButton& b2) noexcept -> MouseButton&
{
  b1 = b1 & b2;
  return b1;
}


// Mouse wheel state values
enum class MouseWheel
{
  None  = 0x00,
  Up    = 0x01,
  Down  = 0x02,
  Left  = 0x03,
  Right = 0x04,
};

// Terminal type
enum class FTermType : uInt32
{
  ansi           = 1U << 0U,   // ANSI X3.64 terminal
  xterm          = 1U << 1U,   // Xterm
  rxvt           = 1U << 2U,   // Rxvt
  urxvt          = 1U << 3U,   // Rxvt-unicode
  kde_konsole    = 1U << 4U,   // KDE Konsole
  gnome_terminal = 1U << 5U,   // Gnome Terminal
  putty          = 1U << 6U,   // PuTTY
  win_terminal   = 1U << 7U,   // Windows Terminal
  tera_term      = 1U << 8U,   // Tera Term
  cygwin         = 1U << 9U,   // Cygwin
  mintty         = 1U << 10U,  // Mintty
  stterm         = 1U << 11U,  // st - simple terminal
  linux_con      = 1U << 12U,  // Linux Console
  freebsd_con    = 1U << 13U,  // FreeBSD workstation Console
  netbsd_con     = 1U << 14U,  // NetBSD workstation console
  openbsd_con    = 1U << 15U,  // OpenBSD workstation Console
  sun_con        = 1U << 16U,  // Sun Microsystems workstation console
  screen         = 1U << 17U,  // GNU Screen
  tmux           = 1U << 18U,  // TMux (terminal multiplexer)
  kterm          = 1U << 19U,  // Kterm (multi-lingual terminal emulator)
  mlterm         = 1U << 20U,  // MLterm (multi-lingual terminal emulator)
  kitty          = 1U << 21U   // kitty (GPU based terminal emulator)
};

using FTermTypeT = std::underlying_type_t<FTermType>;

constexpr auto operator | (const FTermType& t1, const FTermType& t2) noexcept -> FTermTypeT
{
  return FTermTypeT(t1) | FTermTypeT(t2);
}

constexpr auto operator | (FTermTypeT t1, const FTermType& t2) noexcept -> FTermTypeT
{
  return t1 | FTermTypeT(t2);
}

constexpr auto operator | (const FTermType& t1, FTermTypeT t2) noexcept -> FTermTypeT
{
  return FTermTypeT(t1) | t2;
}

// Type of focus
enum class FocusTypes
{
  NextWidget     = 0x00,
  PreviousWidget = 0x01,
  DefiniteWidget = 0x03  // Focus event default value
};

// Mouse drag scrolling mode
enum class DragScrollMode
{
  None            = 0,
  Upward          = 1,
  Downward        = 2,
  Leftward        = 3,
  Rightward       = 4,
  SelectUpward    = 5,
  SelectDownward  = 6,
  SelectLeftward  = 7,
  SelectRightward = 8
};

constexpr auto isDragging (const DragScrollMode& drag_scroll) noexcept -> bool
{
  return drag_scroll != DragScrollMode::None;
}

// Scroll bar visibility mode
enum class ScrollBarMode
{
  Auto   = 0,  // Shows a scroll bar when area is larger than viewport
  Hidden = 1,  // Never shows a scroll bar
  Scroll = 2   // Always shows a scroll bar
};

// Cursor mode
enum class CursorMode
{
  Insert,
  Overwrite
};

// Xterm cursor style
enum class XTermCursorStyle
{
  UnknownCursorStyle   = -1,
  BlinkingBlock        = 0,
  BlinkingBlockDefault = 1,
  SteadyBlock          = 2,
  BlinkingUnderline    = 3,
  SteadyUnderline      = 4,
  BlinkingBarXterm     = 5,
  SteadyBarXterm       = 6
};

// Linux console and framebuffer cursor style
enum class LinuxConsoleCursorStyle
{
  Default    = 0,
  Invisible  = 1,
  Underscore = 2,
  LowerThird = 3,
  LowerHalf  = 4,
  TwoThirds  = 5,
  FullBlock  = 6
};

// BSD console cursor style
enum class FreeBSDConsoleCursorStyle
{
  Normal      = 0,
  Blink       = 1,
  Destructive = 3
};

// KDE konsole cursor style
enum class KdeKonsoleCursorShape
{
  Block     = 0,
  IBeam     = 1,
  Underline = 2
};

enum class Align
{
  Left   = 1,
  Center = 2,
  Right  = 3
};

enum class Orientation
{
  Vertical   = 0,
  Horizontal = 1
};

enum class Side
{
  Top    = 0,
  Right  = 1,
  Bottom = 2,
  Left   = 3
};

enum class SortType
{
  Name,
  Number,
  UserDefined,
  Unknown
};

enum class SortOrder
{
  Ascending,
  Descending,
  Unsorted
};

enum class BracketType
{
  None        = 0,
  Brackets    = 1,  // [ ]
  Parentheses = 2,  // ( )
  Braces      = 3,  // { }
  Chevrons    = 4   // < >
};

enum class Termcap
{
  t_bell,
  t_flash_screen,
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
  t_pad_char,
  t_insert_padding,
  t_insert_character,
  t_parm_ich,
  t_repeat_char,
  t_repeat_last_char,
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

}  // namespace finalcut

#endif  // FC_H
