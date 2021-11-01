/***********************************************************************
* ftypes.h - Implements global data types                              *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2017-2021 Markus Gans                                      *
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

#include <sys/types.h>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include <array>
#include <chrono>
#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <utility>

#define null nullptr

#define badAllocOutput(object_name)              \
    std::clog << FLog::LogLevel::Error           \
              << __FILE__  << ":" << __LINE__    \
              << ": Not enough memory to alloc " \
              << (object_name)                   \
              << " in "                          \
              << __func__ << std::endl  // ;

using uChar     = unsigned char;
using uShort    = unsigned short;
using uInt      = unsigned int;
using uLong     = unsigned long;
using uInt8     = std::uint8_t;
using uInt16    = std::uint16_t;
using uInt32    = std::uint32_t;
using uInt64    = std::uint64_t;

using sInt      = signed int;
using sLong     = signed long;
using sInt8     = std::int8_t;
using sInt16    = std::int16_t;
using sInt32    = std::int32_t;
using sInt64    = std::int64_t;

using lDouble   = long double;
using TimeValue = std::chrono::time_point<std::chrono::system_clock>;
using FCall     = std::function<void()>;

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

// Check for 7-bit ASCII
template<typename CharT>
constexpr bool is7bit (CharT ch)
{
  using char_type = std::make_unsigned_t<CharT>;
  return static_cast<char_type>(ch) < 128;
}


// Typecast to c-string
template <typename StringT>
constexpr auto C_STR (StringT&& string)
{
  return const_cast<char*>(std::forward<StringT>(string));
}

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

template <typename T>
struct EnumHash
{
  constexpr std::size_t operator () (const T& mt) const noexcept
  {
    using underlying_type = std::underlying_type_t<T>;
    return std::hash<underlying_type>()(underlying_type(mt));
  }
};

template <typename CharT>
constexpr std::size_t stringLength (const CharT* s)
{
  return std::char_traits<CharT>::length(s);
}

template <typename CharT>
using remove_ptr_t = std::remove_pointer_t<CharT>;

template <typename CharT>
using remove_ptr_cv_t = std::remove_cv_t<remove_ptr_t<CharT>>;

template <typename CharT>
using is_char_based_ptr = std::is_same<char, remove_ptr_cv_t<CharT>>;

template <typename CharT>
using remove_ref_t = std::remove_reference_t<CharT>;

template <typename CharT>
using remove_ref_extent_t = std::remove_extent_t<remove_ref_t<CharT>>;

template <typename CharT>
using remove_ref_extent_cv_t = std::remove_cv_t<remove_ref_extent_t<CharT>>;

template <typename CharT>
using is_char_based_array = typename std::is_same<char, remove_ref_extent_cv_t<CharT>>;

template <typename CharT>
using enable_if_CString_t =
    std::enable_if_t<
       (std::is_pointer<CharT>::value && is_char_based_ptr<CharT>::value)
    || (std::is_array<remove_ref_t<CharT>>::value && is_char_based_array<CharT>::value)
      , std::nullptr_t>;

template <typename CharT>
struct isCString
  : std::integral_constant<bool
    , (std::is_pointer<CharT>::value && is_char_based_ptr<CharT>::value)
   || (std::is_array<remove_ref_t<CharT>>::value && is_char_based_array<CharT>::value)>
{ };

template <typename NumT>
using enable_if_arithmetic_without_char_t =
  std::enable_if_t< std::is_arithmetic<NumT>::value
                 && ! std::is_same<char, NumT>::value
                 , std::nullptr_t>;

struct TCapAttributes
{
  uInt8 p1 : 1;  // Standout
  uInt8 p2 : 1;  // Underline
  uInt8 p3 : 1;  // Reverse
  uInt8 p4 : 1;  // Blink
  uInt8 p5 : 1;  // Dim
  uInt8 p6 : 1;  // Bold
  uInt8 p7 : 1;  // Invisible
  uInt8 p8 : 1;  // Protected
  uInt8 p9 : 1;  // Alternate charset
  uInt8    : 7;  // padding bits
};

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

union FAttribute
{
  FCharAttribute bit;
  uInt8 byte[4];
};

static constexpr std::size_t UNICODE_MAX = 5;

using FUnicode = std::array<wchar_t, UNICODE_MAX>;

enum class FColor : uInt16;   // forward declaration

struct FChar
{
  FUnicode   ch{};            // Character code
  FUnicode   encoded_char{};  // Encoded output character
  FColor     fg_color{};      // Foreground color
  FColor     bg_color{};      // Background color
  FAttribute attr{};          // Attributes
};

// FChar operator functions
//----------------------------------------------------------------------
inline bool isFUnicodeEqual (const FUnicode& lhs, const FUnicode& rhs) noexcept
{
  auto l_iter = lhs.cbegin();
  auto r_iter = rhs.cbegin();
  const auto& l_last = lhs.cend();

  while ( l_iter != l_last )
  {
    if ( *l_iter == *r_iter )
    {
      if ( *l_iter == L'\0' )
        return true;

      ++l_iter;
      ++r_iter;
    }
    else
      return false;
  }

  return true;
}

//----------------------------------------------------------------------
inline bool operator == (const FChar& lhs, const FChar& rhs)
{
  return isFUnicodeEqual(lhs.ch, rhs.ch)
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
