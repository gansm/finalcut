/***********************************************************************
* ftypes.h - Implements global data types                              *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2017-2024 Markus Gans                                      *
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
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cwctype>

#include <array>
#include <chrono>
#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <utility>

#include "final/eventloop/pipedata.h"

#if (defined(__APPLE__) && defined(__MACH__)) || defined(__OpenBSD__)
  #define USE_KQUEUE_TIMER
#endif

#if !(defined(__APPLE__) && defined(__MACH__)) && !(defined(__OpenBSD__))
  #define USE_POSIX_TIMER
#endif

#define null nullptr

#ifdef __has_builtin
  #define HAVE_BUILTIN(x) __has_builtin(x)
#else
  #define HAVE_BUILTIN(x) 0
#endif

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
  constexpr auto operator () (const T& x) const noexcept -> bool
  {
    return x < 0;
  }
};

template <typename T>
struct is_negative<T, false>
{
  constexpr auto operator () (const T&) const noexcept -> bool
  {
    return false;
  }
};

}  // namespace internal

// Check for 7-bit ASCII
template<typename CharT>
constexpr auto is7bit (CharT ch) noexcept -> bool
{
  using char_type = std::make_unsigned_t<CharT>;
  return static_cast<char_type>(ch) < 128;
}

// Printable character verification
inline auto isPrintable (char ch) noexcept -> bool
{
  return std::isprint(static_cast<unsigned char>(ch));
}

inline auto isPrintable (wchar_t ch) noexcept -> bool
{
  return std::iswprint(std::wint_t(ch));
}

// Typecast to c-string
template <typename StringT>
constexpr auto C_STR (StringT&& string) noexcept
{
  return const_cast<char*>(std::forward<StringT>(string));
}

template <typename T>
constexpr auto isNegative (const T& x) noexcept -> bool
{
  return internal::is_negative<T, std::numeric_limits<T>::is_signed>{}(x);
}

template <typename T>
struct getPrecision
{
  constexpr explicit operator int () const noexcept
  {
    return std::numeric_limits<T>::digits10;
  }
};

template <typename T>
struct EnumHash
{
  constexpr auto operator () (const T& mt) const noexcept -> std::size_t
  {
    using underlying_type = std::underlying_type_t<T>;
    return std::hash<underlying_type>{}(underlying_type(mt));
  }
};

template <typename CharT>
constexpr auto stringLength (const CharT* s) noexcept -> std::size_t
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
using is_wchar_based_ptr = std::is_same<wchar_t, remove_ptr_cv_t<CharT>>;

template <typename CharT>
using remove_ref_t = std::remove_reference_t<CharT>;

template <typename CharT>
using remove_ref_extent_t = std::remove_extent_t<remove_ref_t<CharT>>;

template <typename CharT>
using remove_ref_extent_cv_t = std::remove_cv_t<remove_ref_extent_t<CharT>>;

template <typename CharT>
using is_char_based_array = typename std::is_same<char, remove_ref_extent_cv_t<CharT>>;

template <typename CharT>
using is_wchar_based_array = typename std::is_same<wchar_t, remove_ref_extent_cv_t<CharT>>;

template <typename CharT>
using enable_if_char_ptr_t =
    std::enable_if_t<
        std::is_pointer<CharT>::value && is_char_based_ptr<CharT>::value
      , std::nullptr_t>;

template <typename CharT>
using enable_if_char_array_t =
    std::enable_if_t<
        std::is_array<remove_ref_t<CharT>>::value && is_char_based_array<CharT>::value
      , std::nullptr_t>;

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

template <typename CharT>
using enable_if_wchar_ptr_t =
    std::enable_if_t<
        std::is_pointer<CharT>::value && is_wchar_based_ptr<CharT>::value
      , std::nullptr_t>;

template <typename CharT>
using enable_if_wchar_array_t =
    std::enable_if_t<
        std::is_array<remove_ref_t<CharT>>::value && is_wchar_based_array<CharT>::value
      , std::nullptr_t>;

template <typename CharT>
using enable_if_WCString_t =
    std::enable_if_t<
       (std::is_pointer<CharT>::value && is_wchar_based_ptr<CharT>::value)
    || (std::is_array<remove_ref_t<CharT>>::value && is_wchar_based_array<CharT>::value)
      , std::nullptr_t>;

template <typename CharT>
struct isWCString
  : std::integral_constant<bool
    , (std::is_pointer<CharT>::value && is_wchar_based_ptr<CharT>::value)
   || (std::is_array<remove_ref_t<CharT>>::value && is_wchar_based_array<CharT>::value)>
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

#if HAVE_BUILTIN(__builtin_bit_cast)
constexpr auto getFAttributeByte ( const FCharAttribute& fchar_attr
                                 , std::size_t index ) noexcept -> uInt8
{
  return (__builtin_bit_cast(uInt32, fchar_attr) >> (index << 3)) & 0xff;
}
#else
inline auto getFAttributeByte ( const FCharAttribute& fchar_attr
                              , std::size_t index ) noexcept -> uInt8
{
  uInt8 byte{};
  std::memcpy (&byte, reinterpret_cast<const uInt8*>(&fchar_attr) + index, sizeof(uInt8));
  return byte;
}
#endif

#if HAVE_BUILTIN(__builtin_bit_cast)
constexpr auto setFAttributeByte ( FCharAttribute& fchar_attr
                                 , const std::size_t index
                                 , const uInt8 value ) noexcept
{
  if ( index >= sizeof(FCharAttribute) )
    return;  // index out of bounds

  const auto clear_value = __builtin_bit_cast(uInt32, fchar_attr) & ~(uInt32(0xff) << (index << 3));
  const auto ret = clear_value | (uInt32(value) << (index << 3));
  fchar_attr = __builtin_bit_cast(FCharAttribute, ret);
}
#else
inline auto setFAttributeByte ( FCharAttribute& fchar_attr
                              , std::size_t index
                              , uInt8 value ) noexcept
{
  assert ( index < sizeof(FCharAttribute) );
  std::memcpy(reinterpret_cast<uInt8*>(&fchar_attr) + index, &value, sizeof(uInt8));
}
#endif

#if HAVE_BUILTIN(__builtin_bit_cast)
constexpr auto getFAttributeWord (const FCharAttribute& fchar_attr) noexcept -> uInt32
{
  return __builtin_bit_cast(uInt32, fchar_attr);
}
#else
inline auto getFAttributeWord (const FCharAttribute& fchar_attr) noexcept -> uInt32
{
  uInt32 word{};
  std::memcpy(&word, &fchar_attr, sizeof(word));
  return word;
}
#endif

#if HAVE_BUILTIN(__builtin_bit_cast)
constexpr auto WordToFAttribute (uInt32 word) noexcept -> FCharAttribute
{
  return __builtin_bit_cast(FCharAttribute, word);
}
#else
inline auto WordToFAttribute (uInt32 word) noexcept -> FCharAttribute
{
  FCharAttribute fchar_attr{};
  std::memcpy(&fchar_attr, &word, sizeof(fchar_attr));
  return fchar_attr;
}
#endif

union FAttribute
{
  uInt8 byte[4];
  uInt32 word;
  FCharAttribute bit;
};

static constexpr std::size_t UNICODE_MAX = 5;

using FUnicode = std::array<wchar_t, UNICODE_MAX>;

enum class FColor : uInt16;   // forward declaration


// FChar operator functions
//----------------------------------------------------------------------
#if HAVE_BUILTIN(__builtin_bit_cast)
constexpr auto isFUnicodeEqual (const FUnicode& lhs, const FUnicode& rhs) noexcept -> bool
{
  // Check sizes first for early exit if sizes don't match
  if ( lhs.size() != rhs.size() )
    return false;

  // Perform a byte-wise comparison
  return std::memcmp(&lhs[0], &rhs[0], lhs.size() * sizeof(wchar_t)) == 0;
}
#else
inline auto isFUnicodeEqual (const FUnicode& lhs, const FUnicode& rhs) noexcept -> bool
{
  static_assert ( sizeof(lhs) == sizeof(rhs) , "Both sides are different sizes.");
  // Perform a byte-wise comparison
  return std::wmemcmp(lhs.data(), rhs.data(), lhs.size()) == 0;
}
#endif

//----------------------------------------------------------------------
#if HAVE_BUILTIN(__builtin_bit_cast)
constexpr auto getCompareBitMask() noexcept -> uInt32
{
  constexpr const FAttribute mask {{ 0xff, 0xff, 0x04, 0x00 }};
  return __builtin_bit_cast(uInt32, mask.byte);
}
#else
inline auto getCompareBitMask() noexcept -> uInt32
{
  const FAttribute mask {{ 0xff, 0xff, 0x04, 0x00 }};
  uInt32 word{};
  std::memcpy(&word, &mask, sizeof(word));
  return word;
}
#endif

//----------------------------------------------------------------------
struct FChar
{
  FUnicode   ch{};            // Character code
  FUnicode   encoded_char{};  // Encoded output character
  FColor     fg_color{};      // Foreground color
  FColor     bg_color{};      // Background color
  FAttribute attr{};          // Attributes

#if HAVE_BUILTIN(__builtin_bit_cast)
  friend constexpr
#else
  friend inline
#endif
  auto operator == (const FChar& lhs, const FChar& rhs) noexcept -> bool
  {
    if ( ! isFUnicodeEqual(lhs.ch, rhs.ch)
      || lhs.fg_color != rhs.fg_color
      || lhs.bg_color != rhs.bg_color )
      return false;

    const auto mask = getCompareBitMask();

    if ( (lhs.attr.word & mask) != (rhs.attr.word & mask) )
      return false;

    return true;
  }

#if HAVE_BUILTIN(__builtin_bit_cast)
  friend constexpr
#else
  friend inline
#endif
  auto operator != (const FChar& lhs, const FChar& rhs) noexcept -> bool
  {
    return ! ( lhs == rhs );
  }
};

}  // namespace finalcut

#endif  // FTYPES_H
