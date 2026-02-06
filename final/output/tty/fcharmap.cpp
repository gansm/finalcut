/***********************************************************************
* fcharmap.cpp - Character mapping and encoding                        *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2026 Markus Gans                                      *
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
#include <memory>

#include "final/fc.h"
#include "final/ftypes.h"
#include "final/output/tty/fcharmap.h"

namespace finalcut
{

//----------------------------------------------------------------------
auto FCharMap::getInstance() -> FCharMap&
{
  static const auto& char_map = std::make_unique<FCharMap>();
  return *char_map;
}

//----------------------------------------------------------------------
auto FCharMap::getCharacter ( const CharEncodeMap& char_enc
                            , const Encoding& enc ) -> const wchar_t&
{
#if defined(__clang__)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif
  return char_enc.array[std::size_t(enc)];
#if defined(__clang__)
  #pragma clang diagnostic pop
#endif

}

//----------------------------------------------------------------------
auto FCharMap::setCharacter ( CharEncodeMap& char_enc
                            , const Encoding& enc ) -> wchar_t&
{
#if defined(__clang__)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif
  return char_enc.array[std::size_t(enc)];
#if defined(__clang__)
  #pragma clang diagnostic pop
#endif
}

//----------------------------------------------------------------------
auto FCharMap::getCharEncodeMap() -> CharEncodeType&
{
  return character;
}

//----------------------------------------------------------------------
auto FCharMap::getDECSpecialGraphics() -> const DECGraphicsType&
{
  return dec_special_graphics;
}

//----------------------------------------------------------------------
auto FCharMap::getCP437UCSMap() -> const Cp437UcsType&
{
  return cp437_ucs;
}

//----------------------------------------------------------------------
auto FCharMap::getHalfFullWidthMap() -> const HalfFullWidthType&
{
  return halfwidth_fullwidth;
}

//----------------------------------------------------------------------
FCharMap::CharEncodeType FCharMap::character =
{{
  //  .--------------------- Unicode (UTF-8)
  //  |     .--------------- VT100
  //  |     |     .--------- PC (IBM-437)
  //  |     |     |    .---- ASCII (7-Bit)
  //  |     |     |    |
  { {{0x20ac,   0, 0xee, 'E'}} },  // €  -  Euro
  { {{0x00a3, '}', 0x9c, 'P'}} },  // £  -  Pound
  { {{0x00a7, '$', 0x15, '$'}} },  // §  -  Section
  { {{0x25d8, '*', 0x08, '*'}} },  // ◘  -  InverseBullet
  { {{0x25d9, '*', 0x0a, '*'}} },  // ◙  -  InverseWhiteCircle
  { {{0x203c, '!', 0x13, '!'}} },  // ‼  -  DoubleExclamationMark
  { {{0x2195, 'I', 0x12, 'I'}} },  // ↕  -  UpDownArrow
  { {{0x2194, '-', 0x1d, '-'}} },  // ↔  -  LeftRightArrow
  { {{0x25ac, '_', 0x16, '_'}} },  // ▬  -  BlackRectangle
  { {{0x2191, '^', 0x18, '^'}} },  // ↑  -  UpwardsArrow
  { {{0x2193, 'v', 0x19, 'v'}} },  // ↓  -  DownwardsArrow
  { {{0x2192, '>', 0x1a, '>'}} },  // →  -  RightwardsArrow
  { {{0x2190, '<', 0x1b, '<'}} },  // ←  -  LeftwardsArrow
  { {{0x203a, '>', 0xaf, '>'}} },  // ›  -  SingleRightAngleQuotationMark
  { {{0x2039, '<', 0xae, '<'}} },  // ‹  -  SingleLeftAngleQuotationMark
  { {{0x2026, '.',  '.', '.'}} },  // …  -  HorizontalEllipsis
  { {{0x03c0, '{', 0xe3, 'n'}} },  // π  -  Pi
  { {{0x207F, 'I', 0xfc, ' '}} },  // ⁿ  -  SuperscriptLatinSmallLetterN
  { {{0x2265, 'z', 0xf2, '>'}} },  // ≥  -  GreaterThanOrEqualTo
  { {{0x2264, 'y', 0xf3, '<'}} },  // ≤  -  LessThanOrEqualTo
  { {{0x2260,   0, 0xd8, '#'}} },  // ≠  -  NotEqualTo
  { {{0x00b1, 'g', 0xf1, '#'}} },  // ±  -  PlusMinus
  { {{0x00f7, '/', 0xf6, '/'}} },  // ÷  -  Division sign
  { {{0x00d7,   0,  'x', 'x'}} },  // ×  -  Multiplication sign
  { {{0x02e3, '~', 0xfc, '`'}} },  // ˣ  -  Modifier letter small x
  { {{0x00b0, 'f', 0xb0, 'o'}} },  // °  -  Degree
  { {{0x2022, '`', 0x04, '*'}} },  // •  -  Bullet
  { {{0x00b7, '`', 0xfa, '.'}} },  // ·  -  small Bullet
  { {{0x25cf, '`', 0x04, '*'}} },  // ●  -  BlackCircle
  { {{0x2666, '`', 0x04, '*'}} },  // ◆  -  BlackDiamondSuit
  { {{0x2424, 'h',  ' ', ' '}} },  // ␤  -  SymbolForNewline (1)
  { {{0x240b, 'i',  ' ', ' '}} },  // ␋  -  SymbolForVerticalTab (1)
  { {{0x2409, 'b',  ' ', ' '}} },  // ␉  -  SymbolForHorizontalTab (1)
  { {{0x240c, 'c',  ' ', ' '}} },  // ␌  -  SymbolForFormFeed (1)
  { {{0x240d, 'd',  ' ', ' '}} },  // ␍  -  SymbolForCarriageReturn (1)
  { {{0x240a, 'e',  ' ', ' '}} },  // ␊  -  SymbolForLineFeed (1)
  { {{0x2592, 'a', 0xb0, '#'}} },  // ▒  -  MediumShade
  { {{0x2588, '0', 0xdb, '#'}} },  // █  -  FullBlock
  { {{0x25ae, '_', 0xfe, '#'}} },  // ▮  -  BlackVerticalRectangle (1)
  { {{0x258c,   0, 0xdd, ' '}} },  // ▌  -  LeftHalfBlock
  { {{0x2590,   0, 0xde, ' '}} },  // ▐  -  RightHalfBlock
  { {{0x2584,   0, 0xdc, ' '}} },  // ▄  -  LowerHalfBlock
  { {{0x2580,   0, 0xdf, ' '}} },  // ▀  -  UpperHalfBlock
  { {{0x2500, 'q', 0xc4, '-'}} },  // ─  -  BoxDrawingsHorizontal
  { {{0x2502, 'x', 0xb3, '|'}} },  // │  -  BoxDrawingsVertical
  { {{0x250c, 'l', 0xda, '.'}} },  // ┌  -  BoxDrawingsDownAndRight
  { {{0x2510, 'k', 0xbf, '.'}} },  // ┐  -  BoxDrawingsDownAndLeft
  { {{0x2514, 'm', 0xc0, '`'}} },  // └  -  BoxDrawingsUpAndRight
  { {{0x2518, 'j', 0xd9, '\''}} }, // ┘  -  BoxDrawingsUpAndLeft
  { {{0x253c, 'n', 0xc5, '+'}} },  // ┼  -  BoxDrawingsCross
  { {{0x252c, 'w', 0xc2, '+'}} },  // ┬  -  BoxDrawingsDownAndHorizontal
  { {{0x2524, 'u', 0xb4, '+'}} },  // ┤  -  BoxDrawingsVerticalAndLeft
  { {{0x251c, 't', 0xc3, '+'}} },  // ├  -  BoxDrawingsVerticalAndRight
  { {{0x2534, 'v', 0xc1, '+'}} },  // ┴  -  BoxDrawingsUpAndHorizontal
  { {{0x23ba, 'o',  '~', '~'}} },  // ⎺  -  HorizontalScanLine1 (1)
  { {{0x23bb, 'p', 0xc4, '-'}} },  // ⎻  -  HorizontalScanLine3 (1)
  { {{0x23bc, 'r', 0xc4, '-'}} },  // ⎼  -  HorizontalScanLine7 (1)
  { {{0x23bd, 's',  '_', '_'}} },  // ⎽  -  HorizontalScanLine9 (1)
  { {{0x25b2, '-', 0x1e, '^'}} },  // ▲  -  BlackUpPointingTriangle
  { {{0x25bc, '.', 0x1f, 'v'}} },  // ▼  -  BlackDownPointingTriangle
  { {{0x25b6, '+', 0x10, '>'}} },  // ▶  -  BlackRightPointingTriangle
  { {{0x25c0, ',', 0x11, '<'}} },  // ◀  -  BlackLeftPointingTriangle (1)
  { {{0x25ba, '+', 0x10, '>'}} },  // ►  -  BlackRightPointingPointer (1)
  { {{0x25c4, ',', 0x11, '<'}} },  // ◄  -  BlackLeftPointingPointer
  { {{0xe1b3, 'x', 0xb3, '|'}} },  // │  -  NF_border_line_vertical (2)
  { {{0xe1b4,   0, 0xb4,   0}} },  // ⊐  -  NF_rev_left_arrow2 (2)
  { {{0xe1b5,   0, 0xb5,   0}} },  // ►  -  NF_rev_right_arrow2 (2)
  { {{0xe1b6,   0, 0xb6,   0}} },  // ╵  -  NF_border_line_left_up (2)
  { {{0xe1b7,   0, 0xb7,   0}} },  // )  -  NF_radio_button3 (2)
  { {{0xe1b8,   0, 0xb8,   0}} },  // ⎤  -  NF_rev_border_corner_upper_right (2)
  { {{0xe1b9,   0, 0xb9,   0}} },  // ⎹    -  NF_rev_border_line_right (2)
  { {{0xe1ba,   0, 0xba,   0}} },  // ┤  -  NF_rev_border_line_vertical_left (2)
  { {{0xe1bb,   0, 0xbb,   0}} },  // ⎦  -  NF_rev_border_corner_lower_right (2)
  { {{0xe1bc,   0, 0xbc,   0}} },  // ⎸    -  NF_border_line_left (2)
  { {{0xe1bd,   0, 0xbd,   0}} },  //⎹◣ -  NF_rev_up_arrow2 (2)
  { {{0xe1be,   0, 0xbe,   0}} },  //⎹◤ -  NF_rev_down_arrow2 (2)
  { {{0xe1bf,   0, 0xbf,   0}} },  // ╷  -  NF_border_line_left_down (2)
  { {{0xe1c0,   0, 0xc0,   0}} },  // └  -  NF_border_corner_middle_lower_left (2)
  { {{0xe1c1,   0, 0xc1,   0}} },  // ◢⎸ -  NF_rev_up_arrow1 (2)
  { {{0xe1c2,   0, 0xc2,   0}} },  // ◥⎸ -  NF_rev_down_arrow1 (2)
  { {{0xe1c3,   0, 0xc3,   0}} },  // ├  -  NF_border_line_vertical_right (2)
  { {{0xe1c4, 'q', 0xc4, '-'}} },  // ─  -  NF_border_line_horizontal (2)
  { {{0xe1c5,   0, 0xc5,   0}} },  // =  -  NF_border_line_up_and_down (2)
  { {{0xe1c6,   0, 0xc6,   0}} },  // =  -  NF_shadow_box_middle (2)
  { {{0xe1c7,   0, 0xc7,   0}} },  // =  -  NF_shadow_box_hdd (2)
  { {{0xe1c8,   0, 0xc8,   0}} },  // ◄  -  NF_rev_left_arrow1 (2)
  { {{0xe1c9,   0, 0xc9,   0}} },  // ⊏  -  NF_rev_right_arrow1 (2)
  { {{0xe1ca,   0, 0xca,   0}} },  // [  -  NF_rev_menu_button1 (2)
  { {{0xe1cb,   0, 0xcb,   0}} },  // -  -  NF_rev_menu_button2 (2)
  { {{0xe1cc,   0, 0xcc,   0}} },  // ┌  -  NF_border_corner_middle_upper_left (2)
  { {{0xe1cd,   0, 0xcd,   0}} },  // =  -  NF_shadow_box_cd (2)
  { {{0xe1ce,   0, 0xce,   0}} },  // [  -  NF_shadow_box_left (2)
  { {{0xe1cf,   0, 0xcf,   0}} },  // ┌  -  NF_border_line_middle_left_down (2)
  { {{0xe1d0,   0, 0xd0,   0}} },  // └  -  NF_border_line_middle_right_up (2)
  { {{0xe1d1,   0, 0xd1,   0}} },  // =  -  NF_shadow_box_net (2)
  { {{0xe1d2,   0, 0xd2,   0}} },  // ◢  -  NF_rev_up_pointing_triangle1 (2)
  { {{0xe1d3,   0, 0xd3,   0}} },  // ⎣  -  NF_border_corner_lower_left (2)
  { {{0xe1d4,   0, 0xd4,   0}} },  // _  -  NF_border_line_bottom (2)
  { {{0xe1d5,   0, 0xd5,   0}} },  // O  -  NF_radio_button2 (2)
  { {{0xe1d6,   0, 0xd6,   0}} },  // ●  -  NF_radio_button2_checked (2)
  { {{0xe1d7,   0, 0xd7,   0}} },  // ◥  -  NF_rev_down_pointing_triangle1 (2)
  { {{0xe1d8,   0, 0xd8,   0}} },  // ¯  -  NF_border_line_upper (2)
  { {{0xe1d9,   0, 0xd9,   0}} },  // (  -  NF_radio_button1 (2)
  { {{0xe1da,   0, 0xda,   0}} },  // ⎡  -  NF_border_corner_upper_left (2)
  { {{0xe1dc,   0, 0xdc,   0}} },  // ✓  -  NF_shadow_box_checked (2)
  { {{0xe1e7,   0, 0xe7,   0}} },  // ║  -  NF_rev_border_line_right_and_left (2)
  { {{0xe1e8,   0, 0xe8,   0}} },  // ◣  -  NF_rev_up_pointing_triangle2 (2)
  { {{0xe1e9,   0, 0xe9,   0}} },  // ◤  -  NF_rev_down_pointing_triangle2 (2)
  { {{0xe1ea,   0, 0xea,   0}} },  // ┘  -  NF_border_corner_middle_lower_right (2)
  { {{0xe1eb,   0, 0xeb,   0}} },  // ┐  -  NF_border_corner_middle_upper_right (2)
  { {{0xe1f4,   0, 0xf4,   0}} },  // ]  -  NF_rev_menu_button3 (2)
  { {{0xe1f5,   0, 0xf5,   0}} },  // ]  -  NF_shadow_box_right (2)
  { {{0xe1fb,   0, 0xfb,   0}} },  // ✓  -  NF_check_mark (2)
  { {{0xe1fc, '~', 0xfc, '`'}} },  // ˣ  -  NF_xsuperior (2)
  { {{0x221a,   0, 0xfb, 'x'}} }   // √  -  square root
}};

/*
 * (1) Not defined in Windows Glyph List 4 (WGL4)
 * (2) Only supported in use with newfont
 */

//----------------------------------------------------------------------
constexpr FCharMap::DECGraphicsType FCharMap::dec_special_graphics =
{{
  {VT100Key::rarrow   , UniChar::BlackRightPointingPointer},     // ►
  {VT100Key::larrow   , UniChar::BlackLeftPointingPointer},      // ◄
  {VT100Key::uarrow   , UniChar::BlackUpPointingTriangle},       // ▲
  {VT100Key::darrow   , UniChar::BlackDownPointingTriangle},     // ▼
  {VT100Key::block    , UniChar::FullBlock},                     // █
  {VT100Key::nsup     , UniChar::SuperscriptLatinSmallLetterN},  // ⁿ
  {VT100Key::blackrect, UniChar::BlackVerticalRectangle},        // ▮
  {VT100Key::diamond  , UniChar::BlackDiamondSuit},              // ◆
  {VT100Key::ckboard  , UniChar::MediumShade},                   // ▒
  {VT100Key::htab     , UniChar::SymbolForHorizontalTab},        // ␉
  {VT100Key::ff       , UniChar::SymbolForFormFeed},             // ␌
  {VT100Key::cr       , UniChar::SymbolForCarriageReturn},       // ␍
  {VT100Key::lf       , UniChar::SymbolForLineFeed},             // ␊
  {VT100Key::degree   , UniChar::Degree},                        // °
  {VT100Key::plminus  , UniChar::PlusMinus},                     // ±
  {VT100Key::board    , UniChar::SymbolForNewline},              // ␤
  {VT100Key::lantern  , UniChar::SymbolForVerticalTab},          // ␋
  {VT100Key::lrcorner , UniChar::BoxDrawingsUpAndLeft},          // ┘
  {VT100Key::urcorner , UniChar::BoxDrawingsDownAndLeft},        // ┐
  {VT100Key::ulcorner , UniChar::BoxDrawingsDownAndRight},       // ┌
  {VT100Key::llcorner , UniChar::BoxDrawingsUpAndRight},         // └
  {VT100Key::plus     , UniChar::BoxDrawingsCross},              // ┼
  {VT100Key::s1       , UniChar::HorizontalScanLine1},           // ⎺
  {VT100Key::s3       , UniChar::HorizontalScanLine3},           // ⎻
  {VT100Key::hline    , UniChar::BoxDrawingsHorizontal},         // ─
  {VT100Key::s7       , UniChar::HorizontalScanLine7},           // ⎼
  {VT100Key::s9       , UniChar::HorizontalScanLine9},           // ⎽
  {VT100Key::ltee     , UniChar::BoxDrawingsVerticalAndRight},   // ├
  {VT100Key::rtee     , UniChar::BoxDrawingsVerticalAndLeft},    // ┤
  {VT100Key::btee     , UniChar::BoxDrawingsUpAndHorizontal},    // ┴
  {VT100Key::ttee     , UniChar::BoxDrawingsDownAndHorizontal},  // ┬
  {VT100Key::vline    , UniChar::BoxDrawingsVertical},           // │
  {VT100Key::lequal   , UniChar::LessThanOrEqualTo},             // ≤
  {VT100Key::gequal   , UniChar::GreaterThanOrEqualTo},          // ≥
  {VT100Key::pi       , UniChar::Pi},                            // π
  {VT100Key::nequal   , UniChar::NotEqualTo},                    // ≠
  {VT100Key::sterling , UniChar::Pound},                         // £
  {VT100Key::bullet   , UniChar::SmallBullet},                   // ·
  {VT100Key::diamond  , UniChar::Bullet}                         // ◆
}};

//----------------------------------------------------------------------
constexpr FCharMap::Cp437UcsType FCharMap::cp437_ucs =
{{
  {{0x00, 0x0000}},  // null
  {{0x01, 0x263a}},  // white smiling face
  {{0x02, 0x263b}},  // black smiling face
  {{0x03, 0x2665}},  // black heart suit
  {{0x04, 0x2666}},  // black diamond suit
  {{0x05, 0x2663}},  // black club suit
  {{0x06, 0x2660}},  // black spade suit
  {{0x07, 0x2022}},  // bullet
  {{0x08, 0x25d8}},  // inverse bullet
  {{0x09, 0x25cb}},  // white circle
  {{0x0a, 0x25d9}},  // inverse white circle
  {{0x0b, 0x2642}},  // male sign
  {{0x0c, 0x2640}},  // female sign
  {{0x0d, 0x266a}},  // eighth note
  {{0x0e, 0x266b}},  // beamed eighth notes
  {{0x0f, 0x263c}},  // white sun with rays
  {{0x10, 0x25ba}},  // black right-pointing pointer
  {{0x11, 0x25c4}},  // black left-pointing pointer
  {{0x12, 0x2195}},  // up down arrow
  {{0x13, 0x203c}},  // double exclamation mark
  {{0x14, 0x00b6}},  // pilcrow sign
  {{0x15, 0x00a7}},  // section sign
  {{0x16, 0x25ac}},  // black rectangle
  {{0x17, 0x21a8}},  // up down arrow with base
  {{0x18, 0x2191}},  // upwards arrow
  {{0x19, 0x2193}},  // downwards arrow
  {{0x1a, 0x2192}},  // rightwards arrow
  {{0x1b, 0x2190}},  // leftwards arrow
  {{0x1c, 0x221f}},  // right angle
  {{0x1d, 0x2194}},  // left right arrow
  {{0x1e, 0x25b2}},  // black up-pointing triangle
  {{0x1f, 0x25bc}},  // black down-pointing triangle
  {{0x20, 0x0020}},  // space
  {{0x21, 0x0021}},  // exclamation mark
  {{0x22, 0x0022}},  // quotation mark
  {{0x23, 0x0023}},  // number sign
  {{0x24, 0x0024}},  // dollar sign
  {{0x25, 0x0025}},  // percent sign
  {{0x26, 0x0026}},  // ampersand
  {{0x27, 0x0027}},  // apostrophe
  {{0x28, 0x0028}},  // left parenthesis
  {{0x29, 0x0029}},  // right parenthesis
  {{0x2a, 0x002a}},  // asterisk
  {{0x2b, 0x002b}},  // plus sign
  {{0x2c, 0x002c}},  // comma
  {{0x2d, 0x002d}},  // hyphen-minus
  {{0x2e, 0x002e}},  // full stop
  {{0x2f, 0x002f}},  // solidus
  {{0x30, 0x0030}},  // digit zero
  {{0x31, 0x0031}},  // digit one
  {{0x32, 0x0032}},  // digit two
  {{0x33, 0x0033}},  // digit three
  {{0x34, 0x0034}},  // digit four
  {{0x35, 0x0035}},  // digit five
  {{0x36, 0x0036}},  // digit six
  {{0x37, 0x0037}},  // digit seven
  {{0x38, 0x0038}},  // digit eight
  {{0x39, 0x0039}},  // digit nine
  {{0x3a, 0x003a}},  // colon
  {{0x3b, 0x003b}},  // semicolon
  {{0x3c, 0x003c}},  // less-than sign
  {{0x3d, 0x003d}},  // equals sign
  {{0x3e, 0x003e}},  // greater-than sign
  {{0x3f, 0x003f}},  // question mark
  {{0x40, 0x0040}},  // commercial at
  {{0x41, 0x0041}},  // latin capital letter a
  {{0x42, 0x0042}},  // latin capital letter b
  {{0x43, 0x0043}},  // latin capital letter c
  {{0x44, 0x0044}},  // latin capital letter d
  {{0x45, 0x0045}},  // latin capital letter e
  {{0x46, 0x0046}},  // latin capital letter f
  {{0x47, 0x0047}},  // latin capital letter g
  {{0x48, 0x0048}},  // latin capital letter h
  {{0x49, 0x0049}},  // latin capital letter i
  {{0x4a, 0x004a}},  // latin capital letter j
  {{0x4b, 0x004b}},  // latin capital letter k
  {{0x4c, 0x004c}},  // latin capital letter l
  {{0x4d, 0x004d}},  // latin capital letter m
  {{0x4e, 0x004e}},  // latin capital letter n
  {{0x4f, 0x004f}},  // latin capital letter o
  {{0x50, 0x0050}},  // latin capital letter p
  {{0x51, 0x0051}},  // latin capital letter q
  {{0x52, 0x0052}},  // latin capital letter r
  {{0x53, 0x0053}},  // latin capital letter s
  {{0x54, 0x0054}},  // latin capital letter t
  {{0x55, 0x0055}},  // latin capital letter u
  {{0x56, 0x0056}},  // latin capital letter v
  {{0x57, 0x0057}},  // latin capital letter w
  {{0x58, 0x0058}},  // latin capital letter x
  {{0x59, 0x0059}},  // latin capital letter y
  {{0x5a, 0x005a}},  // latin capital letter z
  {{0x5b, 0x005b}},  // left square bracket
  {{0x5c, 0x005c}},  // reverse solidus
  {{0x5d, 0x005d}},  // right square bracket
  {{0x5e, 0x005e}},  // circumflex accent
  {{0x5f, 0x005f}},  // low line
  {{0x60, 0x0060}},  // grave accent
  {{0x61, 0x0061}},  // latin small letter a
  {{0x62, 0x0062}},  // latin small letter b
  {{0x63, 0x0063}},  // latin small letter c
  {{0x64, 0x0064}},  // latin small letter d
  {{0x65, 0x0065}},  // latin small letter e
  {{0x66, 0x0066}},  // latin small letter f
  {{0x67, 0x0067}},  // latin small letter g
  {{0x68, 0x0068}},  // latin small letter h
  {{0x69, 0x0069}},  // latin small letter i
  {{0x6a, 0x006a}},  // latin small letter j
  {{0x6b, 0x006b}},  // latin small letter k
  {{0x6c, 0x006c}},  // latin small letter l
  {{0x6d, 0x006d}},  // latin small letter m
  {{0x6e, 0x006e}},  // latin small letter n
  {{0x6f, 0x006f}},  // latin small letter o
  {{0x70, 0x0070}},  // latin small letter p
  {{0x71, 0x0071}},  // latin small letter q
  {{0x72, 0x0072}},  // latin small letter r
  {{0x73, 0x0073}},  // latin small letter s
  {{0x74, 0x0074}},  // latin small letter t
  {{0x75, 0x0075}},  // latin small letter u
  {{0x76, 0x0076}},  // latin small letter v
  {{0x77, 0x0077}},  // latin small letter w
  {{0x78, 0x0078}},  // latin small letter x
  {{0x79, 0x0079}},  // latin small letter y
  {{0x7a, 0x007a}},  // latin small letter z
  {{0x7b, 0x007b}},  // left curly bracket
  {{0x7c, 0x007c}},  // vertical line
  {{0x7d, 0x007d}},  // right curly bracket
  {{0x7e, 0x007e}},  // tilde
  {{0x7f, 0x007f}},  // house
  {{0x80, 0x00c7}},  // latin capital letter c with cedilla
  {{0x81, 0x00fc}},  // latin small letter u with diaeresis
  {{0x82, 0x00e9}},  // latin small letter e with acute
  {{0x83, 0x00e2}},  // latin small letter a with circumflex
  {{0x84, 0x00e4}},  // latin small letter a with diaeresis
  {{0x85, 0x00e0}},  // latin small letter a with grave
  {{0x86, 0x00e5}},  // latin small letter a with ring above
  {{0x87, 0x00e7}},  // latin small letter c with cedilla
  {{0x88, 0x00ea}},  // latin small letter e with circumflex
  {{0x89, 0x00eb}},  // latin small letter e with diaeresis
  {{0x8a, 0x00e8}},  // latin small letter e with grave
  {{0x8b, 0x00ef}},  // latin small letter i with diaeresis
  {{0x8c, 0x00ee}},  // latin small letter i with circumflex
  {{0x8d, 0x00ec}},  // latin small letter i with grave
  {{0x8e, 0x00c4}},  // latin capital letter a with diaeresis
  {{0x8f, 0x00c5}},  // latin capital letter a with ring above
  {{0x90, 0x00c9}},  // latin capital letter e with acute
  {{0x91, 0x00e6}},  // latin small ligature ae
  {{0x92, 0x00c6}},  // latin capital ligature ae
  {{0x93, 0x00f4}},  // latin small letter o with circumflex
  {{0x94, 0x00f6}},  // latin small letter o with diaeresis
  {{0x95, 0x00f2}},  // latin small letter o with grave
  {{0x96, 0x00fb}},  // latin small letter u with circumflex
  {{0x97, 0x00f9}},  // latin small letter u with grave
  {{0x98, 0x00ff}},  // latin small letter y with diaeresis
  {{0x99, 0x00d6}},  // latin capital letter o with diaeresis
  {{0x9a, 0x00dc}},  // latin capital letter u with diaeresis
  {{0x9b, 0x00a2}},  // cent sign
  {{0x9c, 0x00a3}},  // pound sign
  {{0x9d, 0x00a5}},  // yen sign
  {{0x9e, 0x20a7}},  // peseta sign
  {{0x9f, 0x0192}},  // latin small letter f with hook
  {{0xa0, 0x00e1}},  // latin small letter a with acute
  {{0xa1, 0x00ed}},  // latin small letter i with acute
  {{0xa2, 0x00f3}},  // latin small letter o with acute
  {{0xa3, 0x00fa}},  // latin small letter u with acute
  {{0xa4, 0x00f1}},  // latin small letter n with tilde
  {{0xa5, 0x00d1}},  // latin capital letter n with tilde
  {{0xa6, 0x00aa}},  // feminine ordinal indicator
  {{0xa7, 0x00ba}},  // masculine ordinal indicator
  {{0xa8, 0x00bf}},  // inverted question mark
  {{0xa9, 0x2310}},  // reversed not sign
  {{0xaa, 0x00ac}},  // not sign
  {{0xab, 0x00bd}},  // vulgar fraction one half
  {{0xac, 0x00bc}},  // vulgar fraction one quarter
  {{0xad, 0x00a1}},  // inverted exclamation mark
  {{0xae, 0x00ab}},  // left-pointing double angle quotation mark
  {{0xaf, 0x00bb}},  // right-pointing double angle quotation mark
  {{0xb0, 0x2591}},  // light shade
  {{0xb1, 0x2592}},  // medium shade
  {{0xb2, 0x2593}},  // dark shade
  {{0xb3, 0x2502}},  // box drawings light vertical
  {{0xb4, 0x2524}},  // box drawings light vertical and left
  {{0xb5, 0x2561}},  // box drawings vertical single and left double
  {{0xb6, 0x2562}},  // box drawings vertical double and left single
  {{0xb7, 0x2556}},  // box drawings down double and left single
  {{0xb8, 0x2555}},  // box drawings down single and left double
  {{0xb9, 0x2563}},  // box drawings double vertical and left
  {{0xba, 0x2551}},  // box drawings double vertical
  {{0xbb, 0x2557}},  // box drawings double down and left
  {{0xbc, 0x255d}},  // box drawings double up and left
  {{0xbd, 0x255c}},  // box drawings up double and left single
  {{0xbe, 0x255b}},  // box drawings up single and left double
  {{0xbf, 0x2510}},  // box drawings light down and left
  {{0xc0, 0x2514}},  // box drawings light up and right
  {{0xc1, 0x2534}},  // box drawings light up and horizontal
  {{0xc2, 0x252c}},  // box drawings light down and horizontal
  {{0xc3, 0x251c}},  // box drawings light vertical and right
  {{0xc4, 0x2500}},  // box drawings light horizontal
  {{0xc5, 0x253c}},  // box drawings light vertical and horizontal
  {{0xc6, 0x255e}},  // box drawings vertical single and right double
  {{0xc7, 0x255f}},  // box drawings vertical double and right single
  {{0xc8, 0x255a}},  // box drawings double up and right
  {{0xc9, 0x2554}},  // box drawings double down and right
  {{0xca, 0x2569}},  // box drawings double up and horizontal
  {{0xcb, 0x2566}},  // box drawings double down and horizontal
  {{0xcc, 0x2560}},  // box drawings double vertical and right
  {{0xcd, 0x2550}},  // box drawings double horizontal
  {{0xce, 0x256c}},  // box drawings double vertical and horizontal
  {{0xcf, 0x2567}},  // box drawings up single and horizontal double
  {{0xd0, 0x2568}},  // box drawings up double and horizontal single
  {{0xd1, 0x2564}},  // box drawings down single and horizontal double
  {{0xd2, 0x2565}},  // box drawings down double and horizontal single
  {{0xd3, 0x2559}},  // box drawings up double and right single
  {{0xd4, 0x2558}},  // box drawings up single and right double
  {{0xd5, 0x2552}},  // box drawings down single and right double
  {{0xd6, 0x2553}},  // box drawings down double and right single
  {{0xd7, 0x256b}},  // box drawings vertical double and horizontal single
  {{0xd8, 0x256a}},  // box drawings vertical single and horizontal double
  {{0xd9, 0x2518}},  // box drawings light up and left
  {{0xda, 0x250c}},  // box drawings light down and right
  {{0xdb, 0x2588}},  // full block
  {{0xdc, 0x2584}},  // lower half block
  {{0xdd, 0x258c}},  // left half block
  {{0xde, 0x2590}},  // right half block
  {{0xdf, 0x2580}},  // upper half block
  {{0xe0, 0x03b1}},  // greek small letter alpha
  {{0xe1, 0x00df}},  // latin small letter sharp s
  {{0xe2, 0x0393}},  // greek capital letter gamma
  {{0xe3, 0x03c0}},  // greek small letter pi
  {{0xe4, 0x03a3}},  // greek capital letter sigma
  {{0xe5, 0x03c3}},  // greek small letter sigma
  {{0xe6, 0x00b5}},  // micro sign
  {{0xe7, 0x03c4}},  // greek small letter tau
  {{0xe8, 0x03a6}},  // greek capital letter phi
  {{0xe9, 0x0398}},  // greek capital letter theta
  {{0xea, 0x03a9}},  // greek capital letter omega
  {{0xeb, 0x03b4}},  // greek small letter delta
  {{0xec, 0x221e}},  // infinity
  {{0xed, 0x03c6}},  // greek small letter phi
  {{0xee, 0x03b5}},  // greek small letter epsilon
  {{0xef, 0x2229}},  // intersection
  {{0xf0, 0x2261}},  // identical to
  {{0xf1, 0x00b1}},  // plus-minus sign
  {{0xf2, 0x2265}},  // greater-than or equal to
  {{0xf3, 0x2264}},  // less-than or equal to
  {{0xf4, 0x2320}},  // top half integral
  {{0xf5, 0x2321}},  // bottom half integral
  {{0xf6, 0x00f7}},  // division sign
  {{0xf7, 0x2248}},  // almost equal to
  {{0xf8, 0x00b0}},  // degree sign
  {{0xf9, 0x2219}},  // bullet operator
  {{0xfa, 0x00b7}},  // middle dot
  {{0xfb, 0x221a}},  // square root
  {{0xfc, 0x207f}},  // superscript latin small letter n
  {{0xfd, 0x00b2}},  // superscript two
  {{0xfe, 0x25a0}},  // black square
  {{0xff, 0x00a0}}   // no-break space
}};

//----------------------------------------------------------------------
// Based on http://www.unicode.org/charts/PDF/UFF00.pdf
constexpr FCharMap::HalfFullWidthType FCharMap::halfwidth_fullwidth =
{{
  // Fullwidth ASCII variants
  {{0x0020, 0x3000}},  // ' ' -> '　'
  {{0x0021, 0xff01}},  // ! -> ！
  {{0x0022, 0xff02}},  // " -> ＂
  {{0x0023, 0xff03}},  // # -> ＃
  {{0x0024, 0xff04}},  // $ -> ＄
  {{0x0025, 0xff05}},  // % -> ％
  {{0x0026, 0xff06}},  // & -> ＆
  {{0x0027, 0xff07}},  // ' -> ＇
  {{0x0028, 0xff08}},  // ( -> （
  {{0x0029, 0xff09}},  // ) -> ）
  {{0x002a, 0xff0a}},  // * -> ＊
  {{0x002b, 0xff0b}},  // + -> ＋
  {{0x002c, 0xff0c}},  // , -> ，
  {{0x002d, 0xff0d}},  // - -> －
  {{0x002e, 0xff0e}},  // . -> ．
  {{0x002f, 0xff0f}},  // / -> ／
  {{0x0030, 0xff10}},  // 0 -> ０
  {{0x0031, 0xff11}},  // 1 -> １
  {{0x0032, 0xff12}},  // 2 -> ２
  {{0x0033, 0xff13}},  // 3 -> ３
  {{0x0034, 0xff14}},  // 4 -> ４
  {{0x0035, 0xff15}},  // 5 -> ５
  {{0x0036, 0xff16}},  // 6 -> ６
  {{0x0037, 0xff17}},  // 7 -> ７
  {{0x0038, 0xff18}},  // 8 -> ８
  {{0x0039, 0xff19}},  // 9 -> ９
  {{0x003a, 0xff1a}},  // : -> ：
  {{0x003b, 0xff1b}},  // ; -> ；
  {{0x003c, 0xff1c}},  // < -> ＜
  {{0x003d, 0xff1d}},  // = -> ＝
  {{0x003e, 0xff1e}},  // > -> ＞
  {{0x003f, 0xff1f}},  // ? -> ？
  {{0x0040, 0xff20}},  // @ -> ＠
  {{0x0041, 0xff21}},  // A -> Ａ
  {{0x0042, 0xff22}},  // B -> Ｂ
  {{0x0043, 0xff23}},  // C -> Ｃ
  {{0x0044, 0xff24}},  // D -> Ｄ
  {{0x0045, 0xff25}},  // E -> Ｅ
  {{0x0046, 0xff26}},  // F -> Ｆ
  {{0x0047, 0xff27}},  // G -> Ｇ
  {{0x0048, 0xff28}},  // H -> Ｈ
  {{0x0049, 0xff29}},  // I -> Ｉ
  {{0x004a, 0xff2a}},  // J -> Ｊ
  {{0x004b, 0xff2b}},  // K -> Ｋ
  {{0x004c, 0xff2c}},  // L -> Ｌ
  {{0x004d, 0xff2d}},  // M -> Ｍ
  {{0x004e, 0xff2e}},  // N -> Ｎ
  {{0x004f, 0xff2f}},  // O -> Ｏ
  {{0x0050, 0xff30}},  // P -> Ｐ
  {{0x0051, 0xff31}},  // Q -> Ｑ
  {{0x0052, 0xff32}},  // R -> Ｒ
  {{0x0053, 0xff33}},  // S -> Ｓ
  {{0x0054, 0xff34}},  // T -> Ｔ
  {{0x0055, 0xff35}},  // U -> Ｕ
  {{0x0056, 0xff36}},  // V -> Ｖ
  {{0x0057, 0xff37}},  // W -> Ｗ
  {{0x0058, 0xff38}},  // X -> Ｘ
  {{0x0059, 0xff39}},  // Y -> Ｙ
  {{0x005a, 0xff3a}},  // Z -> Ｚ
  {{0x005b, 0xff3b}},  // [ -> ［
  {{0x005c, 0xff3c}},  // \ -> ＼
  {{0x005d, 0xff3c}},  // ] -> ］
  {{0x005e, 0xff3e}},  // ^ -> ＾
  {{0x005f, 0xff3f}},  // _ -> ＿
  {{0x0060, 0xff40}},  // ` -> ｀
  {{0x0061, 0xff41}},  // a -> ａ
  {{0x0062, 0xff42}},  // b -> ｂ
  {{0x0063, 0xff43}},  // c -> ｃ
  {{0x0064, 0xff44}},  // d -> ｄ
  {{0x0065, 0xff45}},  // e -> ｅ
  {{0x0066, 0xff46}},  // f -> ｆ
  {{0x0067, 0xff47}},  // g -> ｇ
  {{0x0068, 0xff48}},  // h -> ｈ
  {{0x0069, 0xff49}},  // i -> ｉ
  {{0x006a, 0xff4a}},  // j -> ｊ
  {{0x006b, 0xff4b}},  // k -> ｋ
  {{0x006c, 0xff4c}},  // l -> ｌ
  {{0x006d, 0xff4d}},  // m -> ｍ
  {{0x006e, 0xff4e}},  // n -> ｎ
  {{0x006f, 0xff4f}},  // o -> ｏ
  {{0x0070, 0xff50}},  // p -> ｐ
  {{0x0071, 0xff51}},  // q -> ｑ
  {{0x0072, 0xff52}},  // r -> ｒ
  {{0x0073, 0xff53}},  // s -> ｓ
  {{0x0074, 0xff54}},  // t -> ｔ
  {{0x0075, 0xff55}},  // u -> ｕ
  {{0x0076, 0xff56}},  // v -> ｖ
  {{0x0077, 0xff57}},  // w -> ｗ
  {{0x0078, 0xff58}},  // x -> ｘ
  {{0x0079, 0xff59}},  // y -> ｙ
  {{0x007a, 0xff5a}},  // z -> ｚ
  {{0x007b, 0xff5b}},  // { -> ｛
  {{0x007c, 0xff5c}},  // | -> ｜
  {{0x007d, 0xff5d}},  // } -> ｝
  {{0x007e, 0xff5e}},  // ~ -> ～
  {{0x007e, 0x0301}},  // ~ -> 〜
  // Fullwidth brackets
  {{0xff5f, 0x2e28}},  // ｟ -> ⸨
  {{0xff60, 0x2e29}},  // ｠ -> ⸩
  // Halfwidth CJK punctuation
  {{0xff61, 0x3002}},  // ｡ -> 。
  {{0xff62, 0x300c}},  // ｢ -> 「
  {{0xff63, 0x300d}},  // ｣ -> 」
  {{0xff64, 0x3001}},  // ､ -> 、
  // Halfwidth Katakana variants
  {{0xff65, 0x30fb}},  // ･ -> ・
  {{0xff66, 0x30f2}},  // ｦ -> ヲ
  {{0xff67, 0x30a1}},  // ｧ -> ァ
  {{0xff68, 0x30a3}},  // ｨ -> ィ
  {{0xff69, 0x30a5}},  // ｩ -> ゥ
  {{0xff6a, 0x30a7}},  // ｪ -> ェ
  {{0xff6b, 0x30a9}},  // ｫ -> ォ
  {{0xff6c, 0x30e3}},  // ｬ -> ャ
  {{0xff6d, 0x30e5}},  // ｭ -> ュ
  {{0xff6e, 0x30e7}},  // ｮ -> ョ
  {{0xff6f, 0x30c3}},  // ｯ -> ッ
  {{0xff70, 0x30fc}},  // ｰ -> ー
  {{0xff71, 0x30a2}},  // ｱ -> ア
  {{0xff72, 0x30a4}},  // ｲ -> イ
  {{0xff73, 0x30a6}},  // ｳ -> ウ
  {{0xff74, 0x30a8}},  // ｴ -> エ
  {{0xff75, 0x30aa}},  // ｵ -> オ
  {{0xff76, 0x30ab}},  // ｶ -> カ
  {{0xff77, 0x30ad}},  // ｷ -> キ
  {{0xff78, 0x30af}},  // ｸ -> ク
  {{0xff79, 0x30b1}},  // ｹ -> ケ
  {{0xff7a, 0x30b3}},  // ｺ -> コ
  {{0xff7b, 0x30b5}},  // ｻ -> サ
  {{0xff7c, 0x30b7}},  // ｼ -> シ
  {{0xff7d, 0x30b9}},  // ｽ -> ス
  {{0xff7e, 0x30bb}},  // ｾ -> セ
  {{0xff7f, 0x30bd}},  // ｿ -> ソ
  {{0xff80, 0x30bf}},  // ﾀ -> タ
  {{0xff81, 0x30c1}},  // ﾁ -> チ
  {{0xff82, 0x30c4}},  // ﾂ -> ツ
  {{0xff83, 0x30c6}},  // ﾃ -> テ
  {{0xff84, 0x30c8}},  // ﾄ -> ト
  {{0xff85, 0x30ca}},  // ﾅ -> ナ
  {{0xff86, 0x30cb}},  // ﾆ -> ニ
  {{0xff87, 0x30cc}},  // ﾇ -> ヌ
  {{0xff88, 0x30cd}},  // ﾈ -> ネ
  {{0xff89, 0x30ce}},  // ﾉ -> ノ
  {{0xff8a, 0x30cf}},  // ﾊ -> ハ
  {{0xff8b, 0x30d2}},  // ﾋ -> ヒ
  {{0xff8c, 0x30d5}},  // ﾌ -> フ
  {{0xff8d, 0x30d8}},  // ﾍ -> ヘ
  {{0xff8e, 0x30db}},  // ﾎ -> ホ
  {{0xff8f, 0x30de}},  // ﾏ -> マ
  {{0xff90, 0x30df}},  // ﾐ -> ミ
  {{0xff91, 0x30e0}},  // ﾑ -> ム
  {{0xff92, 0x30e1}},  // ﾒ -> メ
  {{0xff93, 0x30e2}},  // ﾓ -> モ
  {{0xff94, 0x30e4}},  // ﾔ -> ヤ
  {{0xff95, 0x30e6}},  // ﾕ -> ユ
  {{0xff96, 0x30e8}},  // ﾖ -> ヨ
  {{0xff97, 0x30e9}},  // ﾗ -> ラ
  {{0xff98, 0x30ea}},  // ﾘ -> リ
  {{0xff99, 0x30eb}},  // ﾙ -> ル
  {{0xff9a, 0x30ec}},  // ﾚ -> レ
  {{0xff9b, 0x30ed}},  // ﾛ -> ロ
  {{0xff9c, 0x30ef}},  // ﾜ -> ワ
  {{0xff9d, 0x30f3}},  // ﾝ -> ン
  {{0xff9e, 0x3099}},  // ﾞ -> ゙
  {{0xff9f, 0x309a}},  // ﾟ -> ゚
  // Halfwidth Hangul variants
  {{0xffa0, 0x3164}},  // ﾠ-> ᅠ
  {{0xffa1, 0x3131}},  // ﾡ -> ㄱ
  {{0xffa2, 0x3132}},  // ﾢ -> ㄲ
  {{0xffa3, 0x3133}},  // ﾣ -> ㄳ
  {{0xffa4, 0x3134}},  // ﾤ -> ㄴ
  {{0xffa5, 0x3135}},  // ﾥ -> ㄵ
  {{0xffa6, 0x3136}},  // ﾦ -> ㄶ
  {{0xffa7, 0x3137}},  // ﾧ -> ㄷ
  {{0xffa8, 0x3138}},  // ﾨ -> ㄸ
  {{0xffa9, 0x3139}},  // ﾩ -> ㄹ
  {{0xffaa, 0x313a}},  // ﾪ -> ㄺ
  {{0xffab, 0x313b}},  // ﾫ -> ㄻ
  {{0xffac, 0x313c}},  // ﾬ -> ㄼ
  {{0xffad, 0x313d}},  // ﾭ -> ㄽ
  {{0xffae, 0x313e}},  // ﾮ -> ㄾ
  {{0xffaf, 0x313f}},  // ﾯ -> ㄿ
  {{0xffb0, 0x3140}},  // ﾰ -> ㅀ
  {{0xffb1, 0x3141}},  // ﾱ -> ㅁ
  {{0xffb2, 0x3142}},  // ﾲ -> ㅂ
  {{0xffb3, 0x3143}},  // ﾳ -> ㅃ
  {{0xffb4, 0x3144}},  // ﾴ -> ㅄ
  {{0xffb5, 0x3145}},  // ﾵ -> ㅅ
  {{0xffb6, 0x3146}},  // ﾶ -> ㅆ
  {{0xffb7, 0x3147}},  // ﾷ -> ㅇ
  {{0xffb8, 0x3148}},  // ﾸ -> ㅈ
  {{0xffb9, 0x3149}},  // ﾹ -> ㅉ
  {{0xffba, 0x314a}},  // ﾺ -> ㅊ
  {{0xffbb, 0x314b}},  // ﾻ -> ㅋ
  {{0xffbc, 0x314c}},  // ﾼ -> ㅌ
  {{0xffbd, 0x314d}},  // ﾽ -> ㅍ
  {{0xffbe, 0x314e}},  // ﾾ -> ㅎ
  {{0xffc2, 0x314f}},  // ￂ -> ㅏ
  {{0xffc3, 0x3150}},  // ￃ -> ㅐ
  {{0xffc4, 0x3151}},  // ￄ -> ㅑ
  {{0xffc5, 0x3152}},  // ￅ -> ㅒ
  {{0xffc6, 0x3153}},  // ￆ -> ㅓ
  {{0xffc7, 0x3154}},  // ￇ -> ㅔ
  {{0xffca, 0x3155}},  // ￊ -> ㅕ
  {{0xffcb, 0x3156}},  // ￋ -> ㅖ
  {{0xffcc, 0x3157}},  // ￌ -> ㅗ
  {{0xffcd, 0x3158}},  // ￍ -> ㅘ
  {{0xffce, 0x3159}},  // ￎ -> ㅙ
  {{0xffcf, 0x315a}},  // ￏ -> ㅚ
  {{0xffd2, 0x315b}},  // ￒ -> ㅛ
  {{0xffd3, 0x315c}},  // ￓ -> ㅜ
  {{0xffd4, 0x315d}},  // ￔ -> ㅝ
  {{0xffd5, 0x315e}},  // ￕ -> ㅞ
  {{0xffd6, 0x315f}},  // ￖ -> ㅟ
  {{0xffd7, 0x3160}},  // ￗ -> ㅠ
  {{0xffda, 0x3161}},  // ￚ -> ㅡ
  {{0xffdb, 0x3162}},  // ￛ -> ㅢ
  {{0xffdc, 0x3163}},  // ￜ -> ㅣ
  // Fullwidth symbol variants
  {{0x00a2, 0xffe0}},  // ¢ -> ￠
  {{0x00a3, 0xffe1}},  // £ -> ￡
  {{0x00ac, 0xffe2}},  // ¬ -> ￢
  {{0x00af, 0xffe3}},  // ¯ -> ￣
  {{0x00a6, 0xffe4}},  // ¦ -> ￤
  {{0x00a5, 0xffe5}},  // ¥ -> ￥
  {{0x20a9, 0xffe6}},  // ₩ -> ￦
  // Halfwidth symbol variants
  {{0xffe8, 0x2502}},  // ￨ -> │
  {{0xffe9, 0x2190}},  // ￩ -> ←
  {{0xffea, 0x2191}},  // ￪ -> ↑
  {{0xffeb, 0x2192}},  // ￫ -> →
  {{0xffec, 0x2193}},  // ￬ -> ↓
  {{0xffed, 0x25a0}},  // ￭ -> ■
  {{0xffee, 0x25cb}}   // ￮ -> ○
}};

}  // namespace finalcut
