/***********************************************************************
* fterm_functions.cpp - FTerm helper functions                         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2019-2020 Markus Gans                                      *
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

#if defined(__CYGWIN__)
  #include "final/fconfig.h"  // includes _GNU_SOURCE for wcwidth()
#endif

#include <algorithm>
#include <array>
#include <limits>
#include <numeric>
#include <utility>

#include "final/fapplication.h"
#include "final/output/tty/fcharmap.h"
#include "final/output/tty/ftermdata.h"
#include "final/output/tty/fterm_functions.h"
#include "final/output/tty/fterm.h"
#include "final/output/tty/ftermios.h"
#include "final/util/flog.h"
#include "final/util/fpoint.h"
#include "final/vterm/fvtermbuffer.h"

namespace finalcut
{

// Using-declaration
using char_map = std::array<wchar_t, 2>;

// Enumeration
enum class FullWidthSupport
{
  Unknown = -1,
  No = 0,
  Yes = 1
};

// Constant
constexpr std::size_t NOT_FOUND = static_cast<std::size_t>(-1);

// Function prototypes
bool hasAmbiguousWidth (wchar_t);

// Data array
const wchar_t ambiguous_width_list[] =
{
  0x00a1, 0x00a4, 0x00a7, 0x00a8, 0x00aa, 0x00ad, 0x00ae, 0x00b0,
  0x00b1, 0x00b2, 0x00b3, 0x00b4, 0x00b6, 0x00b7, 0x00b8, 0x00b9,
  0x00ba, 0x00bc, 0x00bd, 0x00be, 0x00bf, 0x00c6, 0x00d0, 0x00d7,
  0x00d8, 0x00de, 0x00df, 0x00e0, 0x00e1, 0x00e6, 0x00e8, 0x00e9,
  0x00ea, 0x00ec, 0x00ed, 0x00f0, 0x00f2, 0x00f3, 0x00f7, 0x00f8,
  0x00f9, 0x00fa, 0x00fc, 0x00fe, 0x0101, 0x0111, 0x0113, 0x011b,
  0x0126, 0x0127, 0x012b, 0x0131, 0x0132, 0x0133, 0x0138, 0x013f,
  0x0140, 0x0141, 0x0142, 0x0144, 0x0148, 0x0149, 0x014a, 0x014b,
  0x014d, 0x0152, 0x0153, 0x0166, 0x0167, 0x016b, 0x01ce, 0x01d0,
  0x01d2, 0x01d4, 0x01d6, 0x01d8, 0x01da, 0x01dc, 0x0251, 0x0261,
  0x02c4, 0x02c7, 0x02c9, 0x02ca, 0x02cb, 0x02cd, 0x02d0, 0x02d8,
  0x02d9, 0x02da, 0x02db, 0x02dd, 0x02df, 0x0300, 0x0301, 0x0302,
  0x0303, 0x0304, 0x0305, 0x0306, 0x0307, 0x0308, 0x0309, 0x030a,
  0x030b, 0x030c, 0x030d, 0x030f, 0x0310, 0x0311, 0x0312, 0x0313,
  0x0314, 0x0315, 0x0316, 0x0317, 0x0318, 0x0319, 0x031a, 0x031b,
  0x031c, 0x031d, 0x031e, 0x031f, 0x0320, 0x0321, 0x0322, 0x0323,
  0x0324, 0x0325, 0x0326, 0x0327, 0x0328, 0x0329, 0x032a, 0x032b,
  0x032c, 0x032d, 0x032e, 0x032f, 0x0330, 0x0331, 0x0332, 0x0333,
  0x0334, 0x0335, 0x0336, 0x0337, 0x0338, 0x0339, 0x033a, 0x033b,
  0x033c, 0x033d, 0x033e, 0x033f, 0x0340, 0x0341, 0x0342, 0x0343,
  0x0344, 0x0345, 0x0346, 0x0347, 0x0348, 0x0349, 0x034a, 0x034b,
  0x034c, 0x034d, 0x034e, 0x034f, 0x0350, 0x0351, 0x0352, 0x0353,
  0x0354, 0x0355, 0x0356, 0x0357, 0x0358, 0x0359, 0x035a, 0x035b,
  0x035c, 0x035d, 0x035e, 0x035f, 0x0360, 0x0361, 0x0362, 0x0363,
  0x0364, 0x0365, 0x0366, 0x0367, 0x0368, 0x0369, 0x036a, 0x036b,
  0x036c, 0x036d, 0x036e, 0x036f, 0x0391, 0x0392, 0x0393, 0x0394,
  0x0395, 0x0396, 0x0397, 0x0398, 0x0399, 0x039a, 0x039b, 0x039c,
  0x039d, 0x039e, 0x039f, 0x03a0, 0x03a1, 0x03a3, 0x03a4, 0x03a5,
  0x03a6, 0x03a7, 0x03a8, 0x03a9, 0x03b1, 0x03b2, 0x03b3, 0x03b4,
  0x03b5, 0x03b6, 0x03b7, 0x03b8, 0x03b9, 0x03ba, 0x03bb, 0x03bc,
  0x03bd, 0x03be, 0x03bf, 0x03c0, 0x03c1, 0x03c3, 0x03c4, 0x03c5,
  0x03c6, 0x03c7, 0x03c8, 0x03c9, 0x0401, 0x0410, 0x0411, 0x0412,
  0x0413, 0x0414, 0x0415, 0x0416, 0x0417, 0x0418, 0x0419, 0x041a,
  0x041b, 0x041c, 0x041d, 0x041e, 0x041f, 0x0420, 0x0421, 0x0422,
  0x0423, 0x0424, 0x0425, 0x0426, 0x0427, 0x0428, 0x0429, 0x042a,
  0x042b, 0x042c, 0x042d, 0x042e, 0x042f, 0x0430, 0x0431, 0x0432,
  0x0433, 0x0434, 0x0435, 0x0436, 0x0437, 0x0438, 0x0439, 0x043a,
  0x043b, 0x043c, 0x043d, 0x043e, 0x043f, 0x0440, 0x0441, 0x0442,
  0x0443, 0x0444, 0x0445, 0x0446, 0x0447, 0x0448, 0x0449, 0x044a,
  0x044b, 0x044c, 0x044d, 0x044e, 0x044f, 0x0451, 0x2010, 0x2013,
  0x2014, 0x2015, 0x2016, 0x2018, 0x2019, 0x201c, 0x201d, 0x2020,
  0x2021, 0x2022, 0x2024, 0x2025, 0x2026, 0x2027, 0x2030, 0x2032,
  0x2033, 0x2035, 0x203b, 0x203e, 0x2074, 0x207f, 0x2081, 0x2082,
  0x2083, 0x2084, 0x20ac, 0x2103, 0x2105, 0x2109, 0x2113, 0x2116,
  0x2121, 0x2122, 0x2126, 0x212b, 0x2153, 0x2154, 0x215b, 0x215c,
  0x215d, 0x215e, 0x2160, 0x2162, 0x2162, 0x2163, 0x2164, 0x2165,
  0x2166, 0x2167, 0x2168, 0x2169, 0x216a, 0x216b, 0x2170, 0x2171,
  0x2172, 0x2173, 0x2174, 0x2175, 0x2176, 0x2177, 0x2178, 0x2179,
  0x2190, 0x2191, 0x2192, 0x2193, 0x2194, 0x2195, 0x2196, 0x2197,
  0x2198, 0x2199, 0x21b8, 0x21b9, 0x21d2, 0x21d4, 0x21e7, 0x2200,
  0x2202, 0x2203, 0x2207, 0x2208, 0x220b, 0x220f, 0x2211, 0x2215,
  0x221a, 0x221d, 0x221e, 0x221f, 0x2220, 0x2223, 0x2225, 0x2227,
  0x2228, 0x2229, 0x222a, 0x222b, 0x222c, 0x222e, 0x2234, 0x2235,
  0x2236, 0x2237, 0x223c, 0x223d, 0x2248, 0x224c, 0x2252, 0x2260,
  0x2261, 0x2264, 0x2265, 0x2266, 0x2267, 0x226a, 0x226b, 0x226e,
  0x226f, 0x2282, 0x2283, 0x2286, 0x2287, 0x2295, 0x2299, 0x22a5,
  0x22bf, 0x2312, 0x2460, 0x2461, 0x2462, 0x2463, 0x2464, 0x2465,
  0x2466, 0x2467, 0x2468, 0x2469, 0x246a, 0x246b, 0x246c, 0x246d,
  0x246e, 0x246f, 0x2470, 0x2471, 0x2472, 0x2473, 0x2474, 0x2475,
  0x2476, 0x2477, 0x2478, 0x2479, 0x247a, 0x247b, 0x247c, 0x247d,
  0x247e, 0x247f, 0x2480, 0x2481, 0x2482, 0x2483, 0x2484, 0x2485,
  0x2486, 0x2487, 0x2488, 0x2489, 0x248a, 0x248b, 0x248c, 0x248d,
  0x248e, 0x248f, 0x2490, 0x2491, 0x2492, 0x2493, 0x2494, 0x2495,
  0x2496, 0x2497, 0x2498, 0x2499, 0x249a, 0x249b, 0x249c, 0x249d,
  0x249e, 0x249f, 0x24a0, 0x24a1, 0x24a2, 0x24a3, 0x24a4, 0x24a5,
  0x24a6, 0x24a7, 0x24a8, 0x24a9, 0x24aa, 0x24ab, 0x24ac, 0x24ad,
  0x24ae, 0x24af, 0x24b0, 0x24b1, 0x24b2, 0x24b3, 0x24b4, 0x24b5,
  0x24b6, 0x24b7, 0x24b8, 0x24b9, 0x24ba, 0x24bb, 0x24bc, 0x24bd,
  0x24be, 0x24bf, 0x24c0, 0x24c1, 0x24c2, 0x24c3, 0x24c4, 0x24c5,
  0x24c6, 0x24c7, 0x24c8, 0x24c9, 0x24ca, 0x24cb, 0x24cc, 0x24cd,
  0x24ce, 0x24cf, 0x24d0, 0x24d1, 0x24d2, 0x24d3, 0x24d4, 0x24d5,
  0x24d6, 0x24d7, 0x24d8, 0x24d9, 0x24da, 0x24db, 0x24dc, 0x24dd,
  0x24de, 0x24df, 0x24e0, 0x24e1, 0x24e2, 0x24e3, 0x24e4, 0x24e5,
  0x24e6, 0x24e7, 0x24e8, 0x24e9, 0x24eb, 0x24ec, 0x24ed, 0x24ee,
  0x24ef, 0x24f0, 0x24f1, 0x24f2, 0x24f3, 0x24f4, 0x24f5, 0x24f6,
  0x24f7, 0x24f8, 0x24f9, 0x24fa, 0x24fb, 0x24fc, 0x24fd, 0x24fe,
  0x2500, 0x2501, 0x2502, 0x2503, 0x2504, 0x2505, 0x2506, 0x2507,
  0x2508, 0x2509, 0x250a, 0x250b, 0x250c, 0x250d, 0x250e, 0x250f,
  0x2510, 0x2511, 0x2512, 0x2513, 0x2514, 0x2515, 0x2516, 0x2517,
  0x2518, 0x2519, 0x251a, 0x251b, 0x251c, 0x251d, 0x251e, 0x251f,
  0x2520, 0x2521, 0x2522, 0x2523, 0x2524, 0x2525, 0x2526, 0x2527,
  0x2528, 0x2529, 0x252a, 0x252b, 0x252c, 0x252d, 0x252e, 0x252f,
  0x2530, 0x2531, 0x2532, 0x2533, 0x2534, 0x2535, 0x2536, 0x2537,
  0x2538, 0x2539, 0x253a, 0x253b, 0x253c, 0x253d, 0x253e, 0x253f,
  0x2540, 0x2541, 0x2542, 0x2543, 0x2544, 0x2545, 0x2546, 0x2547,
  0x2548, 0x2549, 0x254a, 0x254b, 0x2550, 0x2551, 0x2552, 0x2553,
  0x2554, 0x2555, 0x2556, 0x2557, 0x2558, 0x2559, 0x255a, 0x255b,
  0x255c, 0x255d, 0x255e, 0x255f, 0x2560, 0x2561, 0x2562, 0x2563,
  0x2564, 0x2565, 0x2566, 0x2567, 0x2568, 0x2569, 0x256a, 0x256b,
  0x256c, 0x256d, 0x256e, 0x256f, 0x2570, 0x2571, 0x2572, 0x2573,
  0x2580, 0x2581, 0x2582, 0x2583, 0x2584, 0x2585, 0x2586, 0x2587,
  0x2588, 0x2589, 0x258a, 0x258b, 0x258c, 0x258d, 0x258e, 0x258f,
  0x2590, 0x2592, 0x2593, 0x2594, 0x2595, 0x25a0, 0x25a1, 0x25a3,
  0x25a4, 0x25a5, 0x25a6, 0x25a7, 0x25a8, 0x25a9, 0x25ac, 0x25ae,
  0x25b2, 0x25b3, 0x25b6, 0x25b7, 0x25ba, 0x25bc, 0x25bd, 0x25c0,
  0x25c1, 0x25c4, 0x25c6, 0x25c7, 0x25c8, 0x25cb, 0x25ce, 0x25cf,
  0x25d0, 0x25d1, 0x25d8, 0x25d9, 0x25e2, 0x25e3, 0x25e4, 0x25e5,
  0x25ef, 0x2605, 0x2606, 0x2609, 0x260e, 0x260f, 0x2614, 0x2615,
  0x261c, 0x261e, 0x2640, 0x2642, 0x2660, 0x2661, 0x2663, 0x2664,
  0x2665, 0x2666, 0x2667, 0x2668, 0x2669, 0x266a, 0x266c, 0x266d,
  0x266f, 0x273d, 0x2776, 0x2777, 0x2778, 0x2779, 0x277a, 0x277b,
  0x277c, 0x277d, 0x277e, 0x277f, 0xfe00, 0xfe01, 0xfe02, 0xfe03,
  0xfe04, 0xfe05, 0xfe07, 0xfe09, 0xfe0a, 0xfe0b, 0xfe0c, 0xfe0d,
  0xfe0e, 0xfe0f, 0xfffd
#if !defined(__CYGWIN__)
, 0xe0100, 0xe0101, 0xe0102, 0xe0103, 0xe0104, 0xe0105, 0xe0106,
  0xe0107, 0xe0108, 0xe0109, 0xe010a, 0xe01ef
#endif
};

constexpr std::array<UniChar, 20> reverse_newfont_list =
{{
  UniChar::NF_rev_left_arrow2,
  UniChar::NF_rev_right_arrow2,
  UniChar::NF_rev_border_corner_upper_right,
  UniChar::NF_rev_border_line_right,
  UniChar::NF_rev_border_line_vertical_left,
  UniChar::NF_rev_border_corner_lower_right,
  UniChar::NF_rev_up_arrow2,
  UniChar::NF_rev_down_arrow2,
  UniChar::NF_rev_up_arrow1,
  UniChar::NF_rev_down_arrow1,
  UniChar::NF_rev_left_arrow1,
  UniChar::NF_rev_right_arrow1,
  UniChar::NF_rev_menu_button1,
  UniChar::NF_rev_menu_button2,
  UniChar::NF_rev_up_pointing_triangle1,
  UniChar::NF_rev_down_pointing_triangle1,
  UniChar::NF_rev_up_pointing_triangle2,
  UniChar::NF_rev_down_pointing_triangle2,
  UniChar::NF_rev_menu_button3,
  UniChar::NF_rev_border_line_right_and_left
}};


// FTerm non-member functions
//----------------------------------------------------------------------
uInt env2uint (const std::string& env)
{
  const FString str{getenv(env.data())};

  if ( str.isEmpty() )
    return 0;

  try
  {
    return str.toUInt();
  }
  catch (const std::invalid_argument&)
  {
    return 0;
  }
  catch (const std::underflow_error&)
  {
    return std::numeric_limits<uInt>::min();
  }
  catch (const std::overflow_error&)
  {
    return std::numeric_limits<uInt>::max();
  }
}

//----------------------------------------------------------------------
std::string& getExitMessage()
{
  static const auto& exit_message = std::make_unique<std::string>();
  return *exit_message;
}

//----------------------------------------------------------------------
void setExitMessage (const FString& message)
{
  getExitMessage().assign(message.toString());
}

//----------------------------------------------------------------------
FColor rgb2ColorIndex (uInt8 r, uInt8 g, uInt8 b)
{
  // Converts a 24-bit RGB color to a 256-color compatible approximation

  const uInt16 ri = (((r * 5) + 127) / 255) * 36;
  const uInt16 gi = (((g * 5) + 127) / 255) * 6;
  const uInt16 bi = (((b * 5) + 127) / 255);
  return FColor(16 + ri + gi + bi);
}

//----------------------------------------------------------------------
bool isReverseNewFontchar (wchar_t wchar)
{
  const auto& cbegin = std::cbegin(reverse_newfont_list);
  const auto& cend = std::cend(reverse_newfont_list);
  return ( std::any_of(cbegin, cend, [&wchar] (const auto& c)
                                     { return wchar_t(c) == wchar; }) );
}

//----------------------------------------------------------------------
inline bool hasAmbiguousWidth (wchar_t wchar)
{
  const auto& cbegin = std::cbegin(ambiguous_width_list);
  const auto& cend = std::cend(ambiguous_width_list);
  return ( std::any_of(cbegin, cend, [&wchar] (const auto& c)
                                     { return c == wchar; }) );
}

//----------------------------------------------------------------------
bool hasFullWidthSupports()
{
  // Checks if the terminal has full-width character support

  // global state
  static FullWidthSupport has_fullwidth_support = FullWidthSupport::Unknown;

  if ( has_fullwidth_support == FullWidthSupport::Unknown )
  {
    if ( ! FTerm::isInitialized() )
      return true;  // Assume that it is a modern terminal with full-width support

    static const auto& fterm_data = FTermData::getInstance();

    if ( fterm_data.isTermType ( FTermType::cygwin
                               | FTermType::tera_term
                               | FTermType::freebsd_con
                               | FTermType::netbsd_con
                               | FTermType::openbsd_con
                               | FTermType::sun_con
                               | FTermType::ansi ) )
      has_fullwidth_support = FullWidthSupport::No;
    else
      has_fullwidth_support = FullWidthSupport::Yes;
  }

  return has_fullwidth_support == FullWidthSupport::Yes;
}

//----------------------------------------------------------------------
wchar_t cp437_to_unicode (uChar c)
{
  constexpr std::size_t CP437 = 0;
  constexpr std::size_t UNICODE = 1;
  const auto& cp437_ucs = FCharMap::getCP437UCSMap();
  wchar_t ucs = c;
  const auto& found = std::find_if ( cp437_ucs.cbegin()
                                   , cp437_ucs.cend()
                                   , [&c] (const char_map& entry)
                                     {
                                       return entry[CP437] == c;
                                     } );

  if ( found != cp437_ucs.cend() )
    ucs = (*found)[UNICODE];

  return ucs;
}

//----------------------------------------------------------------------
uChar unicode_to_cp437 (wchar_t ucs)
{
  constexpr std::size_t CP437 = 0;
  constexpr std::size_t UNICODE = 1;
  const auto& cp437_ucs = FCharMap::getCP437UCSMap();
  uChar c{'?'};
  const auto& found = std::find_if ( cp437_ucs.cbegin()
                                   , cp437_ucs.cend()
                                   , [&ucs] (const char_map& entry)
                                     {
                                       return entry[UNICODE] == ucs;
                                     } );

  if ( found != cp437_ucs.cend() )
    c = static_cast<uChar>((*found)[CP437]);

  return c;
}

//----------------------------------------------------------------------
#if defined(__CYGWIN__)
std::string unicode_to_utf8 (wchar_t ucs)
{
  // 1 Byte (7-bit): 0xxxxxxx
  if ( ucs < 0x80 )
    return { char(ucs) };

  // 2 byte (11-bit): 110xxxxx 10xxxxxx
  if ( ucs < 0x800 )
    return { char(0xc0 | uChar(ucs >> 6u))
           , char(0x80 | uChar(ucs & 0x3f)) };

  // 3 byte (16-bit): 1110xxxx 10xxxxxx 10xxxxxx
  return { char(0xe0 | uChar(ucs >> 12u))
         , char(0x80 | uChar((ucs >> 6u) & 0x3f))
         , char(0x80 | uChar(ucs & 0x3f)) };
}

#else
std::string unicode_to_utf8 (wchar_t ucs)
{
  // 1 Byte (7-bit): 0xxxxxxx
  if ( ucs < 0x80 )
    return { char(ucs) };

  // 2 byte (11-bit): 110xxxxx 10xxxxxx
  if ( ucs < 0x800 )
    return { char(0xc0 | uChar(ucs >> 6u))
           , char(0x80 | uChar(ucs & 0x3f)) };

  // 3 byte (16-bit): 1110xxxx 10xxxxxx 10xxxxxx
  if ( ucs < 0x10000 )
    return { char(0xe0 | uChar(ucs >> 12u))
           , char(0x80 | uChar((ucs >> 6u) & 0x3f))
           , char(0x80 | uChar(ucs & 0x3f)) };

  // 4 byte (21-bit): 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
  if ( ucs < 0x200000 )
    return { char(0xf0 | uChar(ucs >> 18u))
           , char(0x80 | uChar((ucs >> 12u) & 0x3f))
           , char(0x80 | uChar((ucs >> 6u) & 0x3f))
           , char(0x80 | uChar(ucs & 0x3f)) };

  return unicode_to_utf8(L'�'); // Invalid character
}
#endif

//----------------------------------------------------------------------
FString getFullWidth (const FString& str)
{
  // Converts half-width to full-width characters

  FString s{str};
  auto table_search = [] (wchar_t& c)
  {
    const auto& halfwidth_fullwidth = FCharMap::getHalfFullWidthMap();
    constexpr std::size_t HALF = 0;
    constexpr std::size_t FULL = 1;
    const auto& found = std::find_if ( halfwidth_fullwidth.cbegin()
                                     , halfwidth_fullwidth.cend()
                                     , [&c] (const char_map& entry)
                                       {
                                         return entry[HALF] == c;
                                       } );

    if ( found != halfwidth_fullwidth.cend() )
      c = (*found)[FULL];
  };

  for (auto&& c : s)
  {
    if ( c > L'\x20' && c < L'\x7f' )  // half-width ASCII
      c += 0xfee0;
    else
      table_search(c);
  }

  return s;
}

//----------------------------------------------------------------------
FString getHalfWidth (const FString& str)
{
  // Converts full-width to half-width characters

  FString s{str};
  auto table_search = [] (wchar_t& c)
  {
    const auto& halfwidth_fullwidth = FCharMap::getHalfFullWidthMap();
    constexpr std::size_t HALF = 0;
    constexpr std::size_t FULL = 1;
    const auto& found = std::find_if ( halfwidth_fullwidth.cbegin()
                                     , halfwidth_fullwidth.cend()
                                     , [&c] (const char_map& entry)
                                       {
                                         return entry[FULL] == c;
                                       } );

    if ( found != halfwidth_fullwidth.cend() )
      c = (*found)[HALF];
  };

  for (auto&& c : s)
  {
    if ( c > L'\xff00' && c < L'\xff5f' )  // full-width ASCII
      c -= 0xfee0;
    else
      table_search(c);
  }

  return s;
}

//----------------------------------------------------------------------
FString getColumnSubString ( const FString& str
                           , std::size_t col_pos
                           , std::size_t col_len )
{
  FString s{str};
  std::size_t col_first{1};
  std::size_t col_num{0};
  std::size_t first{1};
  std::size_t num{0};

  if ( col_len == 0 || s.isEmpty() )
    return {L""};

  if ( col_pos == 0 )
    col_pos = 1;

  for (auto&& ch : s)
  {
    const auto& width = getColumnWidth(ch);

    if ( col_first < col_pos )
    {
      if ( col_first + width <= col_pos )
      {
        col_first += width;
        first++;
      }
      else
      {
        ch = wchar_t(UniChar::SingleLeftAngleQuotationMark);  // ‹
        num = col_num = 1;
        col_pos = col_first;
      }
    }
    else
    {
      if ( col_first == col_pos && width == 0 && num == 0 )
      {
        first++;
      }
      else if ( col_num + width <= col_len )
      {
        col_num += width;
        num++;
      }
      else if ( col_num < col_len )
      {
        ch = wchar_t(UniChar::SingleRightAngleQuotationMark);  // ›
        num++;
        break;
      }
      else
        break;
    }
  }

  if ( col_first < col_pos )  // String length < col_pos
    return {L""};

  return s.mid(first, num);
}

//----------------------------------------------------------------------
std::size_t getLengthFromColumnWidth ( const FString& str
                                     , std::size_t col_len )
{
  std::size_t column_width{0};
  std::size_t length{0};

  for (const auto& ch : str)
  {
    if ( column_width < col_len )
    {
      column_width += getColumnWidth(ch);
      length++;
    }
  }

  return length;
}

//----------------------------------------------------------------------
std::size_t getColumnWidth (const FString& s, std::size_t end_pos)
{
  if ( s.isEmpty() )
    return 0;

  std::size_t column_width{0};
  const auto& length = s.getLength();

  if ( end_pos > length )
    end_pos = length;

  for (std::size_t i{0}; i < end_pos; i++)
  {
    try
    {
      column_width += getColumnWidth(s[i]);
    }
    catch (const std::out_of_range& ex)
    {
      std::clog << FLog::LogLevel::Error
                << "Out of Range error: " << ex.what() << std::endl;
    }
  }

  return column_width;
}

//----------------------------------------------------------------------
std::size_t getColumnWidth (const FString& s)
{
  int column_width{0};

  if ( s.isEmpty() )
    return 0;

  for (const auto& wchar : s)
    column_width += getColumnWidth(wchar);

  return ( column_width == -1 ) ? 0 : std::size_t(column_width);
}

//----------------------------------------------------------------------
std::size_t getColumnWidth (const wchar_t wchar)
{
  int column_width{};

#if defined(__NetBSD__) || defined(__OpenBSD__) \
 || defined(__FreeBSD__) || defined(__DragonFly__) \
 || defined(__sun) && defined(__SVR4)
  if ( hasAmbiguousWidth(wchar) )
    column_width = 1;
  else
#endif

  column_width = wcwidth(wchar);
  static const auto& fterm_data = FTermData::getInstance();

  if ( (wchar >= UniChar::NF_rev_left_arrow2 && wchar <= UniChar::NF_check_mark)
    || fterm_data.getTerminalEncoding() != Encoding::UTF8 )
  {
    column_width = 1;
  }
  else if ( ! hasFullWidthSupports() )
  {
    column_width = std::min(column_width, 1);
  }

  return ( column_width == -1 ) ? 0 : std::size_t(column_width);
}

//----------------------------------------------------------------------
std::size_t getColumnWidth (const FChar& term_char)
{
  return std::size_t(term_char.attr.bit.char_width);
}

//----------------------------------------------------------------------
std::size_t getColumnWidth (const FVTermBuffer& vtbuf)
{
  return ( vtbuf.isEmpty() )
         ? 0
         : std::accumulate ( std::next(vtbuf.begin())
                           , vtbuf.end()
                           , vtbuf.front().attr.bit.char_width
                           , [] (std::size_t s, const FChar& c)
                             {
                               return s + c.attr.bit.char_width;
                             }
                           );
}

//----------------------------------------------------------------------
void addColumnWidth (FChar& term_char)
{
  const std::size_t char_width = getColumnWidth(term_char.ch[0]);
  static const auto& fterm_data = FTermData::getInstance();

  if ( char_width == 2
    && fterm_data.getTerminalEncoding() != Encoding::UTF8 )
  {
    term_char.ch[0] = '.';
    term_char.attr.bit.char_width = 1;
  }
  else
    term_char.attr.bit.char_width = char_width & 0x03;
}

//----------------------------------------------------------------------
inline int isWhitespace (const wchar_t ch) noexcept
{
  return std::iswspace(static_cast<wint_t>(ch));
}

//----------------------------------------------------------------------
int getCharLength (const FString& string, std::size_t pos)
{
  // Gets the number of characters of the combined character
  // at string position pos

  const auto& len = string.getLength();
  std::size_t n = pos;
  const auto& ch = string[n];
  std::size_t char_width = getColumnWidth(ch);

  if ( isWhitespace(ch) )
    return 1;

  if ( char_width == 0 || n >= len )
    return -1;

  do
  {
    n++;
    char_width = getColumnWidth(string[n]);
  }
  while ( n < len && char_width == 0 && ! isWhitespace(string[n]) );

  return static_cast<int>(n - pos);
}

//----------------------------------------------------------------------
int getPrevCharLength (const FString& string, std::size_t pos)
{
  // Gets the number of characters of the previous combined character
  // at string position pos

  const auto& len = string.getLength();
  std::size_t n = pos;
  const auto& ch = string[n];
  std::size_t char_width = getColumnWidth(ch);

  if ( (char_width == 0 || n == 0 || n >= len) && ! isWhitespace(ch) )
    return -1;

  do
  {
    n--;
    char_width = getColumnWidth(string[n]);
  }
  while ( n > 0 && char_width == 0 && ! isWhitespace(string[n]) );

  if ( char_width == 0 )
    return -1;

  return int(pos - n);
}

//----------------------------------------------------------------------
std::size_t searchLeftCharBegin (const FString& string, std::size_t pos)
{
  // Search for the next character position to the left of string position pos

  std::size_t n = pos;

  if ( n == 0 )
    return NOT_FOUND;

  std::size_t char_width{0};

  do
  {
    n--;
    char_width = getColumnWidth(string[n]);
  }
  while ( n > 0 && char_width == 0 && ! isWhitespace(string[n]) );

  if ( n == 0 && char_width == 0 )
    return NOT_FOUND;

  return n;
}

//----------------------------------------------------------------------
std::size_t searchRightCharBegin (const FString& string, std::size_t pos)
{
  // Search for the next character position to the right of string position pos

  const auto& len = string.getLength();
  std::size_t n = pos;

  if ( n >= len )
    return NOT_FOUND;

  std::size_t char_width{0};

  do
  {
    n++;
    char_width = getColumnWidth(string[n]);
  }
  while ( n < len && char_width == 0 && ! isWhitespace(string[n]) );

  if ( n == len && char_width == 0 )
    return NOT_FOUND;

  return n;
}

//----------------------------------------------------------------------
FPoint readCursorPos()
{
  int x{-1};
  int y{-1};
  const int stdin_no{FTermios::getStdIn()};
  const int stdout_no{FTermios::getStdOut()};
  fd_set ifds{};
  struct timeval tv{};
  const std::string DECXCPR{ESC "[6n"};

  // Report Cursor Position (DECXCPR)
  if ( write(stdout_no, DECXCPR.data(), DECXCPR.length()) < 1 )
    return {x, y};

  std::fflush(stdout);
  FD_ZERO(&ifds);
  FD_SET(stdin_no, &ifds);
  tv.tv_sec  = 0;
  tv.tv_usec = 100'000;  // 100 ms
  std::array<char, 20> temp{};
  std::size_t pos{0};

  // Read the answer
  if ( select (stdin_no + 1, &ifds, nullptr, nullptr, &tv) != 1 )
    return {x, y};

  do
  {
    std::size_t bytes_free = temp.size() - pos - 1;
    const ssize_t bytes = read(stdin_no, &temp[pos], bytes_free);

    if ( bytes <= 0 )
      break;

    pos += std::size_t(bytes);
  }
  while ( pos < temp.size() && ! std::strchr(temp.data(), 'R') );

  if ( pos > 4 )
  {
    constexpr auto parse = "\033[%4d;%4dR";
    std::sscanf(temp.data(), parse, &y, &x );
  }

  return {x, y};
}

}  // namespace finalcut
