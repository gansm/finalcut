/***********************************************************************
* ftypes.h - Implements global data types                              *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2017-2020 Markus Gans                                      *
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

#ifndef FTYPES_H
#define FTYPES_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <stdint.h>
#include <sys/types.h>

#include <cstddef>
#include <limits>
#include <unordered_map>
#include <string>

#define null nullptr

namespace
{

typedef unsigned char  uChar;
typedef unsigned short uShort;
typedef unsigned int   uInt;
typedef unsigned long  uLong;
typedef std::uint8_t   uInt8;
typedef std::uint16_t  uInt16;
typedef std::uint32_t  uInt32;
typedef std::uint64_t  uInt64;

typedef signed int     sInt;
typedef signed long    sLong;
typedef std::int8_t    sInt8;
typedef std::int16_t   sInt16;
typedef std::int32_t   sInt32;
typedef std::int64_t   sInt64;

typedef long double    lDouble;

typedef uInt16         FColor;
typedef uInt16         FAttribute;
typedef uInt32         FKey;
typedef void*          FDataPtr;

}  // namespace


namespace finalcut
{

const char* const bad_alloc_str = \
    "not enough memory to alloc ";

template <typename T, bool is_signed>
struct is_negative
{
  inline bool operator () (const T& x)
  {
    return x < 0;
  }
};

template <typename T>
struct is_negative<T, false>
{
  inline bool operator () (const T&)
  {
    return false;
  }
};

template <typename T>
inline bool isNegative (const T& x)
{
  return is_negative<T, std::numeric_limits<T>::is_signed>()(x);
}

template <typename T>
struct getPrecision
{
  constexpr operator int () const
  {
    return std::numeric_limits<T>::digits10;
  }
};

typedef std::unordered_map<wchar_t, wchar_t> charSubstitution;

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

typedef struct
{
  wchar_t ch;            // Character code
  wchar_t encoded_char;  // Encoded output character
  FColor  fg_color;      // Foreground color
  FColor  bg_color;      // Background color
  attribute attr;        // Attributes
} FChar;


namespace fc
{

typedef struct
{
  FKey  num;
  const char* string;
  char  tname[4];
}
FKeyMap;

typedef struct
{
  FKey num;
  char string[8];
}
FMetakeyMap;

typedef struct
{
  FKey num;
  char string[25];
}
FKeyName;

}  // namespace fc

}  // namespace finalcut

#endif  // FTYPES_H

