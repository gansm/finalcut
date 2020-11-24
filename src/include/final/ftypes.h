/***********************************************************************
* ftypes.h - Implements global data types                              *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2017-2020 Markus Gans                                      *
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

#ifndef FTYPES_H
#define FTYPES_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <stdint.h>
#include <sys/types.h>

#include <cstddef>
#include <cstring>

#include <array>
#include <functional>
#include <limits>
#include <memory>
#include <unordered_map>
#include <string>

#define null nullptr

#define badAllocOutput(object_name)              \
    std::clog << FLog::Error                     \
              << __FILE__  << ":" << __LINE__    \
              << ": Not enough memory to alloc " \
              << (object_name)                   \
              << " in "                          \
              << __func__ << std::endl;

using uChar      = unsigned char;
using uShort     = unsigned short;
using uInt       = unsigned int;
using uLong      = unsigned long;
using uInt8      = std::uint8_t;
using uInt16     = std::uint16_t;
using uInt32     = std::uint32_t;
using uInt64     = std::uint64_t;

using sInt       = signed int;
using sLong      = signed long;
using sInt8      = std::int8_t;
using sInt16     = std::int16_t;
using sInt32     = std::int32_t;
using sInt64     = std::int64_t;

using lDouble    = long double;

using FColor     = uInt16;
using FAttribute = uInt16;
using FKey       = uInt32;
using FCall      = std::function<void()>;

namespace finalcut
{

namespace internal
{

template <typename T
        , bool is_signed>
struct is_negative
{
  inline bool operator () (const T& x) const
  {
    return x < 0;
  }
};

template <typename T>
struct is_negative<T, false>
{
  inline bool operator () (const T&) const
  {
    return false;
  }
};

}  // namespace internal

template <typename T>
constexpr bool isNegative (const T& x)
{
  return internal::is_negative<T, std::numeric_limits<T>::is_signed>()(x);
}

template <typename T>
struct getPrecision
{
  constexpr operator int () const
  {
    return std::numeric_limits<T>::digits10;
  }
};

template <typename T, typename... Args>
std::unique_ptr<T> make_unique (Args&&... args)
{
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

using charSubstitution = std::unordered_map<wchar_t, wchar_t>;

struct FCharAttribute
{
  // Attribute byte #0
  uInt8 bold               : 1;  // bold
  uInt8 dim                : 1;  // dim
  uInt8 italic             : 1;  // italic
  uInt8 underline          : 1;  // underline
  uInt8 blink              : 1;  // blink
  uInt8 reverse            : 1;  // reverse
  uInt8 standout           : 1;  // standout
  uInt8 invisible          : 1;  // invisible
  // Attribute byte #1
  uInt8 protect            : 1;  // protect mode
  uInt8 crossed_out        : 1;  // crossed out
  uInt8 dbl_underline      : 1;  // double underline
  uInt8 alt_charset        : 1;  // alternate character set (vt100)
  uInt8 pc_charset         : 1;  // pc character set (CP437)
  uInt8 transparent        : 1;  // transparent
  uInt8 color_overlay      : 1;  // color overlay
  uInt8 inherit_background : 1;  // inherit background
  // Attribute byte #2
  uInt8 no_changes         : 1;  // no changes required
  uInt8 printed            : 1;  // is printed to VTerm
  uInt8 fullwidth_padding  : 1;  // padding char (after a full-width char)
  uInt8 char_width         : 2;  // number of character cells on screen
  uInt8                    : 3;  // padding bits
  // Attribute byte #3
  uInt8                    : 8;  // padding byte
};

union attribute
{
  FCharAttribute bit;
  uInt8 byte[4];
};

static constexpr uInt UNICODE_MAX = 5;

using FUnicode = std::array<wchar_t, UNICODE_MAX>;

struct FChar
{
  FUnicode  ch{};            // Character code
  FUnicode  encoded_char{};  // Encoded output character
  FColor    fg_color{};      // Foreground color
  FColor    bg_color{};      // Background color
  attribute attr{};          // Attributes
};


namespace fc
{

struct FKeyMap
{
  FKey  num;
  const char* string;
  char  tname[4];
};

struct FMetakeyMap
{
  FKey num;
  char string[8];
};

struct FKeyName
{
  FKey num;
  char string[25];
};

}  // namespace fc

// FChar operator functions
//----------------------------------------------------------------------
inline bool operator == (const FChar& lhs, const FChar& rhs)
{
  return operator == (lhs.ch, rhs.ch)  // Compare FUnicode
      && lhs.fg_color     == rhs.fg_color
      && lhs.bg_color     == rhs.bg_color
      && lhs.attr.byte[0] == rhs.attr.byte[0]
      && lhs.attr.byte[1] == rhs.attr.byte[1]
      && lhs.attr.bit.fullwidth_padding \
                          == rhs.attr.bit.fullwidth_padding;
}

//----------------------------------------------------------------------
inline bool operator != (const FChar& lhs, const FChar& rhs)
{
  return ! ( lhs == rhs );
}

}  // namespace finalcut

#endif  // FTYPES_H
