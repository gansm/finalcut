/***********************************************************************
* fcharmap.h - Character mapping and encoding                          *
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

#ifndef FCHARMAP_H
#define FCHARMAP_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fc.h"
#include "final/ftypes.h"

namespace finalcut
{

namespace fc
{

static uInt character[][fc::NUM_OF_ENCODINGS] =
{
  //  .--------------------- Unicode (UTF-8)
  //  |     .--------------- VT100
  //  |     |     .--------- PC (IBM-437)
  //  |     |     |    .---- ASCII (7-Bit)
  //  |     |     |    |
  {0x20ac,   0, 0xee, 'E'},  // €  -  Euro
  {0x00a3, '}', 0x9c, 'P'},  // £  -  Pound
  {0x03c0, '{', 0xe3, 'n'},  // π  -  Pi
  {0x207F, 'I', 0xfc, ' '},  // ⁿ  -  SuperscriptLatinSmallLetterN
  {0x2265, 'z', 0xf2, '>'},  // ≥  -  GreaterThanOrEqualTo
  {0x2264, 'y', 0xf3, '<'},  // ≤  -  LessThanOrEqualTo
  {0x2260,   0, 0xd8, '#'},  // ≠  -  NotEqualTo
  {0x00b1, 'g', 0xf1, '#'},  // ±  -  PlusMinus
  {0x00f7, '/', 0xf6, '/'},  // ÷  -  Division sign
  {0x00d7,   0,  'x', 'x'},  // ×  -  Multiplication sign
  {0x02e3, '~', 0xfc, '`'},  // ˣ  -  Modifier letter small x
  {0x00b0, 'f', 0xb0, 'o'},  // °  -  Degree
  {0x2022, '`', 0x04, '*'},  // •  -  Bullet
  {0x00b7, '`', 0xfa, '.'},  // ·  -  small Bullet
  {0x2666, '`', 0x04, '*'},  // ◆  -  BlackDiamondSuit
  {0x2424, 'h',  ' ', ' '},  // ␤  -  SymbolForNewline (1)
  {0x240b, 'i',  ' ', ' '},  // ␋  -  SymbolForVerticalTab (1)
  {0x2409, 'b',  ' ', ' '},  // ␉  -  SymbolForHorizontalTab (1)
  {0x240c, 'c',  ' ', ' '},  // ␌  -  SymbolForFormFeed (1)
  {0x240d, 'd',  ' ', ' '},  // ␍  -  SymbolForCarriageReturn (1)
  {0x240a, 'e',  ' ', ' '},  // ␊  -  SymbolForLineFeed (1)
  {0x2592, 'a', 0xb0, '#'},  // ▒  -  MediumShade
  {0x2588, '0', 0xdb, '#'},  // █  -  FullBlock
  {0x25ae, '_', 0xfe, '#'},  // ▮  -  BlackVerticalRectangle (1)
  {0x258c,  0 , 0xdd, ' '},  // ▌  -  LeftHalfBlock
  {0x2590,  0 , 0xde, ' '},  // ▐  -  RightHalfBlock
  {0x2584,  0 , 0xdc, ' '},  // ▄  -  LowerHalfBlock
  {0x2580,  0 , 0xdf, ' '},  // ▀  -  UpperHalfBlock
  {0x2500, 'q', 0xc4, '-'},  // ─  -  BoxDrawingsHorizontal
  {0x2502, 'x', 0xb3, '|'},  // │  -  BoxDrawingsVertical
  {0x250c, 'l', 0xda, '.'},  // ┌  -  BoxDrawingsDownAndRight
  {0x2510, 'k', 0xbf, '.'},  // ┐  -  BoxDrawingsDownAndLeft
  {0x2514, 'm', 0xc0, '`'},  // └  -  BoxDrawingsUpAndRight
  {0x2518, 'j', 0xd9, '\''}, // ┘  -  BoxDrawingsUpAndLeft
  {0x253c, 'n', 0xc5, '+'},  // ┼  -  BoxDrawingsCross
  {0x252c, 'w', 0xc2, '+'},  // ┬  -  BoxDrawingsDownAndHorizontal
  {0x2524, 'u', 0xb4, '+'},  // ┤  -  BoxDrawingsVerticalAndLeft
  {0x251c, 't', 0xc3, '+'},  // ├  -  BoxDrawingsVerticalAndRight
  {0x2534, 'v', 0xc1, '+'},  // ┴  -  BoxDrawingsUpAndHorizontal
  {0x23ba, 'o',  '~', '~'},  // ⎺  -  HorizontalScanLine1 (1)
  {0x23bb, 'p', 0xc4, '-'},  // ⎻  -  HorizontalScanLine3 (1)
  {0x23bc, 'r', 0xc4, '-'},  // ⎼  -  HorizontalScanLine7 (1)
  {0x23bd, 's',  '_', '_'},  // ⎽  -  HorizontalScanLine9 (1)
  {0x25b2, '-', 0x1e, '^'},  // ▲  -  BlackUpPointingTriangle
  {0x25bc, '.', 0x1f, 'v'},  // ▼  -  BlackDownPointingTriangle
  {0x25b6, '+', 0x10, '>'},  // ▶  -  BlackRightPointingTriangle
  {0x25c0, ',', 0x11, '<'},  // ◀  -  BlackLeftPointingTriangle (1)
  {0x25ba, '+', 0x10, '>'},  // ►  -  BlackRightPointingPointer (1)
  {0x25c4, ',', 0x11, '<'},  // ◄  -  BlackLeftPointingPointer
  {0x1ab4,   0, 0xb4,   0},  // ⊐  -  NF_rev_left_arrow2 (2)
  {0x1ab5,   0, 0xb5,   0},  // ►  -  NF_rev_right_arrow2 (2)
  {0x1ab7,   0, 0xb7,   0},  // )  -  NF_radio_button3 (2)
  {0x1ab8,   0, 0xb8,   0},  // ⎤  -  NF_rev_border_corner_upper_right (2)
  {0x1ab9,   0, 0xb9,   0},  // ⎹    -  NF_rev_border_line_right (2)
  {0x1aba,   0, 0xba,   0},  // ┤  -  NF_rev_border_line_vertical_left (2)
  {0x1abb,   0, 0xbb,   0},  // ⎦  -  NF_rev_border_corner_lower_right (2)
  {0x1abc,   0, 0xbc,   0},  // ⎸    -  NF_border_line_left (2)
  {0x1abd,   0, 0xbd,   0},  //⎹◣ -  NF_rev_up_arrow2 (2)
  {0x1abe,   0, 0xbe,   0},  //⎹◤ -  NF_rev_down_arrow2 (2)
  {0x1ac0,   0, 0xc0,   0},  // └  -  NF_border_corner_middle_lower_left (2)
  {0x1ac1,   0, 0xc1,   0},  // ◢⎸ -  NF_rev_up_arrow1 (2)
  {0x1ac2,   0, 0xc2,   0},  // ◥⎸ -  NF_rev_down_arrow1 (2)
  {0x1ac3,   0, 0xc3,   0},  // ├  -  NF_border_line_vertical_right (2)
  {0x1ac5,   0, 0xc5,   0},  // =  -  NF_border_line_up_and_down (2)
  {0x1ac6,   0, 0xc6,   0},  // =  -  NF_shadow_box_middle (2)
  {0x1ac7,   0, 0xc7,   0},  // =  -  NF_shadow_box_hdd (2)
  {0x1ac8,   0, 0xc8,   0},  // ◄  -  NF_rev_left_arrow1 (2)
  {0x1ac9,   0, 0xc9,   0},  // ⊏  -  NF_rev_right_arrow1 (2)
  {0x1aca,   0, 0xca,   0},  // [  -  NF_rev_menu_button1 (2)
  {0x1acb,   0, 0xcb,   0},  // -  -  NF_rev_menu_button2 (2)
  {0x1acc,   0, 0xcc,   0},  // ┌  -  NF_border_corner_middle_upper_left (2)
  {0x1acd,   0, 0xcd,   0},  // =  -  NF_shadow_box_cd (2)
  {0x1ace,   0, 0xce,   0},  // [  -  NF_shadow_box_left (2)
  {0x1acf,   0, 0xcf,   0},  // ┘  -  NF_border_corner_middle_lower_right (2)
  {0x1ad0,   0, 0xd0,   0},  // ┐  -  NF_border_corner_middle_upper_right (2)
  {0x1ad1,   0, 0xd1,   0},  // =  -  NF_shadow_box_net (2)
  {0x1ad2,   0, 0xd2,   0},  // ◢  -  NF_rev_up_pointing_triangle1 (2)
  {0x1ad3,   0, 0xd3,   0},  // ⎣  -  NF_border_corner_lower_left (2)
  {0x1ad4,   0, 0xd4,   0},  // _  -  NF_border_line_bottom (2)
  {0x1ad5,   0, 0xd5,   0},  // O  -  NF_radio_button2 (2)
  {0x1ad6,   0, 0xd6,   0},  // ●  -  NF_radio_button2_checked (2)
  {0x1ad7,   0, 0xd7,   0},  // ◥  -  NF_rev_down_pointing_triangle1 (2)
  {0x1ad8,   0, 0xd8,   0},  // ¯  -  NF_border_line_upper (2)
  {0x1ad9,   0, 0xd9,   0},  // (  -  NF_radio_button1 (2)
  {0x1ada,   0, 0xda,   0},  // ⎡  -  NF_border_corner_upper_left (2)
  {0x1adc,   0, 0xdc,   0},  // ✓  -  NF_shadow_box_checked (2)
  {0x1ae7,   0, 0xe7,   0},  // ║  -  NF_rev_border_line_right_and_left (2)
  {0x1ae8,   0, 0xe8,   0},  // ◣  -  NF_rev_up_pointing_triangle2 (2)
  {0x1ae9,   0, 0xe9,   0},  // ◤  -  NF_rev_down_pointing_triangle2 (2)
  {0x1af4,   0, 0xf4,   0},  // ]  -  NF_rev_menu_button3 (2)
  {0x1af5,   0, 0xf5,   0},  // ]  -  NF_shadow_box_right (2)
  {0x1afb,   0, 0xfb,   0},  // ✓  -  NF_check_mark (2)
  {0x221a,   0, 0xfb, 'x'},  // √  -  square root
  {0x25cf, '`', 0x04, '*'}   // ●  -  black circle
};

/*
 * (1) Not defined in Windows Glyph List 4 (WGL4)
 * (2) Only supported in use with newfont
 */

const int lastCharItem = int(sizeof(character) / sizeof(character[0])) - 1;


static int vt100_key_to_utf8[][2] =
{
  {fc::vt100_key_rarrow   , fc::BlackRightPointingPointer},     // ►
  {fc::vt100_key_larrow   , fc::BlackLeftPointingPointer},      // ◄
  {fc::vt100_key_uarrow   , fc::BlackUpPointingTriangle},       // ▲
  {fc::vt100_key_darrow   , fc::BlackDownPointingTriangle},     // ▼
  {fc::vt100_key_block    , fc::FullBlock},                     // █
  {fc::vt100_key_nsup     , fc::SuperscriptLatinSmallLetterN},  // ⁿ
  {fc::vt100_key_blackrect, fc::BlackVerticalRectangle},        // ▮
  {fc::vt100_key_diamond  , fc::BlackDiamondSuit},              // ◆
  {fc::vt100_key_ckboard  , fc::MediumShade},                   // ▒
  {fc::vt100_key_htab     , fc::SymbolForHorizontalTab},        // ␉
  {fc::vt100_key_ff       , fc::SymbolForFormFeed},             // ␌
  {fc::vt100_key_cr       , fc::SymbolForCarriageReturn},       // ␍
  {fc::vt100_key_lf       , fc::SymbolForLineFeed},             // ␊
  {fc::vt100_key_degree   , fc::Degree},                        // °
  {fc::vt100_key_plminus  , fc::PlusMinus},                     // ±
  {fc::vt100_key_board    , fc::SymbolForNewline},              // ␤
  {fc::vt100_key_lantern  , fc::SymbolForVerticalTab},          // ␋
  {fc::vt100_key_lrcorner , fc::BoxDrawingsUpAndLeft},          // ┘
  {fc::vt100_key_urcorner , fc::BoxDrawingsDownAndLeft},        // ┐
  {fc::vt100_key_ulcorner , fc::BoxDrawingsDownAndRight},       // ┌
  {fc::vt100_key_llcorner , fc::BoxDrawingsUpAndRight},         // └
  {fc::vt100_key_plus     , fc::BoxDrawingsCross},              // ┼
  {fc::vt100_key_s1       , fc::HorizontalScanLine1},           // ⎺
  {fc::vt100_key_s3       , fc::HorizontalScanLine3},           // ⎻
  {fc::vt100_key_hline    , fc::BoxDrawingsHorizontal},         // ─
  {fc::vt100_key_s7       , fc::HorizontalScanLine7},           // ⎼
  {fc::vt100_key_s9       , fc::HorizontalScanLine9},           // ⎽
  {fc::vt100_key_ltee     , fc::BoxDrawingsVerticalAndRight},   // ├
  {fc::vt100_key_rtee     , fc::BoxDrawingsVerticalAndLeft},    // ┤
  {fc::vt100_key_btee     , fc::BoxDrawingsUpAndHorizontal},    // ┴
  {fc::vt100_key_ttee     , fc::BoxDrawingsDownAndHorizontal},  // ┬
  {fc::vt100_key_vline    , fc::BoxDrawingsVertical},           // │
  {fc::vt100_key_lequal   , fc::LessThanOrEqualTo},             // ≤
  {fc::vt100_key_gequal   , fc::GreaterThanOrEqualTo},          // ≥
  {fc::vt100_key_pi       , fc::Pi},                            // π
  {fc::vt100_key_nequal   , fc::NotEqualTo},                    // ≠
  {fc::vt100_key_sterling , fc::Pound},                         // £
  {fc::vt100_key_bullet   , fc::SmallBullet},                   // ·
  {fc::vt100_key_diamond  , fc::Bullet}                         // ◆
};

const int lastKeyItem = int ( sizeof(vt100_key_to_utf8)
                            / sizeof(vt100_key_to_utf8[0]) ) - 1;


static uInt cp437_to_ucs[][2] =
{
  {0x80, 0x00c7},  // latin capital letter c with cedilla
  {0x81, 0x00fc},  // latin small letter u with diaeresis
  {0x82, 0x00e9},  // latin small letter e with acute
  {0x83, 0x00e2},  // latin small letter a with circumflex
  {0x84, 0x00e4},  // latin small letter a with diaeresis
  {0x85, 0x00e0},  // latin small letter a with grave
  {0x86, 0x00e5},  // latin small letter a with ring above
  {0x87, 0x00e7},  // latin small letter c with cedilla
  {0x88, 0x00ea},  // latin small letter e with circumflex
  {0x89, 0x00eb},  // latin small letter e with diaeresis
  {0x8a, 0x00e8},  // latin small letter e with grave
  {0x8b, 0x00ef},  // latin small letter i with diaeresis
  {0x8c, 0x00ee},  // latin small letter i with circumflex
  {0x8d, 0x00ec},  // latin small letter i with grave
  {0x8e, 0x00c4},  // latin capital letter a with diaeresis
  {0x8f, 0x00c5},  // latin capital letter a with ring above
  {0x90, 0x00c9},  // latin capital letter e with acute
  {0x91, 0x00e6},  // latin small ligature ae
  {0x92, 0x00c6},  // latin capital ligature ae
  {0x93, 0x00f4},  // latin small letter o with circumflex
  {0x94, 0x00f6},  // latin small letter o with diaeresis
  {0x95, 0x00f2},  // latin small letter o with grave
  {0x96, 0x00fb},  // latin small letter u with circumflex
  {0x97, 0x00f9},  // latin small letter u with grave
  {0x98, 0x00ff},  // latin small letter y with diaeresis
  {0x99, 0x00d6},  // latin capital letter o with diaeresis
  {0x9a, 0x00dc},  // latin capital letter u with diaeresis
  {0x9b, 0x00a2},  // cent sign
  {0x9c, 0x00a3},  // pound sign
  {0x9d, 0x00a5},  // yen sign
  {0x9e, 0x20a7},  // peseta sign
  {0x9f, 0x0192},  // latin small letter f with hook
  {0xa0, 0x00e1},  // latin small letter a with acute
  {0xa1, 0x00ed},  // latin small letter i with acute
  {0xa2, 0x00f3},  // latin small letter o with acute
  {0xa3, 0x00fa},  // latin small letter u with acute
  {0xa4, 0x00f1},  // latin small letter n with tilde
  {0xa5, 0x00d1},  // latin capital letter n with tilde
  {0xa6, 0x00aa},  // feminine ordinal indicator
  {0xa7, 0x00ba},  // masculine ordinal indicator
  {0xa8, 0x00bf},  // inverted question mark
  {0xa9, 0x2310},  // reversed not sign
  {0xaa, 0x00ac},  // not sign
  {0xab, 0x00bd},  // vulgar fraction one half
  {0xac, 0x00bc},  // vulgar fraction one quarter
  {0xad, 0x00a1},  // inverted exclamation mark
  {0xae, 0x00ab},  // left-pointing double angle quotation mark
  {0xaf, 0x00bb},  // right-pointing double angle quotation mark
  {0xb0, 0x2591},  // light shade
  {0xb1, 0x2592},  // medium shade
  {0xb2, 0x2593},  // dark shade
  {0xb3, 0x2502},  // box drawings light vertical
  {0xb4, 0x2524},  // box drawings light vertical and left
  {0xb5, 0x2561},  // box drawings vertical single and left double
  {0xb6, 0x2562},  // box drawings vertical double and left single
  {0xb7, 0x2556},  // box drawings down double and left single
  {0xb8, 0x2555},  // box drawings down single and left double
  {0xb9, 0x2563},  // box drawings double vertical and left
  {0xba, 0x2551},  // box drawings double vertical
  {0xbb, 0x2557},  // box drawings double down and left
  {0xbc, 0x255d},  // box drawings double up and left
  {0xbd, 0x255c},  // box drawings up double and left single
  {0xbe, 0x255b},  // box drawings up single and left double
  {0xbf, 0x2510},  // box drawings light down and left
  {0xc0, 0x2514},  // box drawings light up and right
  {0xc1, 0x2534},  // box drawings light up and horizontal
  {0xc2, 0x252c},  // box drawings light down and horizontal
  {0xc3, 0x251c},  // box drawings light vertical and right
  {0xc4, 0x2500},  // box drawings light horizontal
  {0xc5, 0x253c},  // box drawings light vertical and horizontal
  {0xc6, 0x255e},  // box drawings vertical single and right double
  {0xc7, 0x255f},  // box drawings vertical double and right single
  {0xc8, 0x255a},  // box drawings double up and right
  {0xc9, 0x2554},  // box drawings double down and right
  {0xca, 0x2569},  // box drawings double up and horizontal
  {0xcb, 0x2566},  // box drawings double down and horizontal
  {0xcc, 0x2560},  // box drawings double vertical and right
  {0xcd, 0x2550},  // box drawings double horizontal
  {0xce, 0x256c},  // box drawings double vertical and horizontal
  {0xcf, 0x2567},  // box drawings up single and horizontal double
  {0xd0, 0x2568},  // box drawings up double and horizontal single
  {0xd1, 0x2564},  // box drawings down single and horizontal double
  {0xd2, 0x2565},  // box drawings down double and horizontal single
  {0xd3, 0x2559},  // box drawings up double and right single
  {0xd4, 0x2558},  // box drawings up single and right double
  {0xd5, 0x2552},  // box drawings down single and right double
  {0xd6, 0x2553},  // box drawings down double and right single
  {0xd7, 0x256b},  // box drawings vertical double and horizontal single
  {0xd8, 0x256a},  // box drawings vertical single and horizontal double
  {0xd9, 0x2518},  // box drawings light up and left
  {0xda, 0x250c},  // box drawings light down and right
  {0xdb, 0x2588},  // full block
  {0xdc, 0x2584},  // lower half block
  {0xdd, 0x258c},  // left half block
  {0xde, 0x2590},  // right half block
  {0xdf, 0x2580},  // upper half block
  {0xe0, 0x03b1},  // greek small letter alpha
  {0xe1, 0x00df},  // latin small letter sharp s
  {0xe2, 0x0393},  // greek capital letter gamma
  {0xe3, 0x03c0},  // greek small letter pi
  {0xe4, 0x03a3},  // greek capital letter sigma
  {0xe5, 0x03c3},  // greek small letter sigma
  {0xe6, 0x00b5},  // micro sign
  {0xe7, 0x03c4},  // greek small letter tau
  {0xe8, 0x03a6},  // greek capital letter phi
  {0xe9, 0x0398},  // greek capital letter theta
  {0xea, 0x03a9},  // greek capital letter omega
  {0xeb, 0x03b4},  // greek small letter delta
  {0xec, 0x221e},  // infinity
  {0xed, 0x03c6},  // greek small letter phi
  {0xee, 0x03b5},  // greek small letter epsilon
  {0xef, 0x2229},  // intersection
  {0xf0, 0x2261},  // identical to
  {0xf1, 0x00b1},  // plus-minus sign
  {0xf2, 0x2265},  // greater-than or equal to
  {0xf3, 0x2264},  // less-than or equal to
  {0xf4, 0x2320},  // top half integral
  {0xf5, 0x2321},  // bottom half integral
  {0xf6, 0x00f7},  // division sign
  {0xf7, 0x2248},  // almost equal to
  {0xf8, 0x00b0},  // degree sign
  {0xf9, 0x2219},  // bullet operator
  {0xfa, 0x00b7},  // middle dot
  {0xfb, 0x221a},  // square root
  {0xfc, 0x207f},  // superscript latin small letter n
  {0xfd, 0x00b2},  // superscript two
  {0xfe, 0x25a0},  // black square
  {0xff, 0x00a0}   // no-break space
};

const uInt lastCP437Item = uInt ( sizeof(cp437_to_ucs)
                                / sizeof(cp437_to_ucs[0]) ) - 1;
}  // namespace fc

}  // namespace finalcut

#endif  // FCHARMAP_H
