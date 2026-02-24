/***********************************************************************
* ftypes.h - Implements global data types                              *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2017-2026 Markus Gans                                      *
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

// UTF8_Char
//----------------------------------------------------------------------
struct FourByteData
{
  char byte1;  // First character
  char byte2;  // Second character
  char byte3;  // Third character
  char byte4;  // Fourth character
};

struct UTF8_Char
{
  FourByteData u8;
  uInt32 length;

  friend constexpr auto operator == (const UTF8_Char& lhs, const UTF8_Char& rhs) noexcept -> bool
  {
    return lhs.u8.byte1 == rhs.u8.byte1
        && lhs.u8.byte2 == rhs.u8.byte2
        && lhs.u8.byte3 == rhs.u8.byte3
        && lhs.u8.byte4 == rhs.u8.byte4
        && lhs.length   == rhs.length;
  }

  friend constexpr auto operator != (const UTF8_Char& lhs, const UTF8_Char& rhs) noexcept -> bool
  {
    return ! ( lhs == rhs );
  }
};

// FCharAttribute + FAttribute
//----------------------------------------------------------------------
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

constexpr auto FCharAttribute_to_uInt32 (const finalcut::FCharAttribute& fchar_attr) noexcept -> uInt32
{
  return uInt32(fchar_attr.bold)               << 0U
       | uInt32(fchar_attr.dim)                << 1U
       | uInt32(fchar_attr.italic)             << 2U
       | uInt32(fchar_attr.underline)          << 3U
       | uInt32(fchar_attr.blink)              << 4U
       | uInt32(fchar_attr.reverse)            << 5U
       | uInt32(fchar_attr.standout)           << 6U
       | uInt32(fchar_attr.invisible)          << 7U
       | uInt32(fchar_attr.protect)            << 8U
       | uInt32(fchar_attr.crossed_out)        << 9U
       | uInt32(fchar_attr.dbl_underline)      << 10U
       | uInt32(fchar_attr.alt_charset)        << 11U
       | uInt32(fchar_attr.pc_charset)         << 12U
       | uInt32(fchar_attr.transparent)        << 13U
       | uInt32(fchar_attr.color_overlay)      << 14U
       | uInt32(fchar_attr.inherit_background) << 15U
       | uInt32(fchar_attr.no_changes)         << 16U
       | uInt32(fchar_attr.printed)            << 17U
       | uInt32(fchar_attr.fullwidth_padding)  << 18U
       | uInt32(fchar_attr.char_width)         << 19U;
}

namespace internal
{

template<typename T>
constexpr auto createMask (T setter) noexcept -> uInt32
{
  FCharAttribute mask{};
  setter(mask);
  return FCharAttribute_to_uInt32(mask);
}

constexpr void setBoldMask (FCharAttribute& attr) noexcept
{
  attr.bold = true;
}

constexpr void setDimMask (FCharAttribute& attr) noexcept
{
  attr.dim = true;
}

constexpr void setItalicMask (FCharAttribute& attr) noexcept
{
  attr.italic = true;
}

constexpr void setUnderlineMask (FCharAttribute& attr) noexcept
{
  attr.underline = true;
}

constexpr void setBlinkMask (FCharAttribute& attr) noexcept
{
  attr.blink = true;
}

constexpr void setReverseMask (FCharAttribute& attr) noexcept
{
  attr.reverse = true;
}

constexpr void setStandoutMask (FCharAttribute& attr) noexcept
{
  attr.standout = true;
}

constexpr void setInvisibleMask (FCharAttribute& attr) noexcept
{
  attr.invisible = true;
}

constexpr void setProtectMask (FCharAttribute& attr) noexcept
{
  attr.protect = true;
}

constexpr void setCrossedOutMask (FCharAttribute& attr) noexcept
{
  attr.crossed_out = true;
}

constexpr void setDblUnderlineMask (FCharAttribute& attr) noexcept
{
  attr.dbl_underline = true;
}

constexpr void setAltCharsetMask (FCharAttribute& attr) noexcept
{
  attr.alt_charset = true;
}

constexpr void setPcCharsetMask (FCharAttribute& attr) noexcept
{
  attr.pc_charset = true;
}

constexpr void setTransparent (FCharAttribute& attr) noexcept
{
  attr.transparent = true;
}

constexpr void setColorOverlay (FCharAttribute& attr) noexcept
{
  attr.color_overlay = true;
}

constexpr void setInheritBackground (FCharAttribute& attr) noexcept
{
  attr.inherit_background = true;
}

constexpr void setNoChanges (FCharAttribute& attr) noexcept
{
  attr.no_changes = true;
}

constexpr void setPrinted (FCharAttribute& attr) noexcept
{
  attr.printed = true;
}

constexpr void setFullwidthPadding (FCharAttribute& attr) noexcept
{
  attr.fullwidth_padding = true;
}

constexpr void setCharWidth (FCharAttribute& attr) noexcept
{
  attr.char_width = 0x03;
}

struct attr
{
  static constexpr auto bold() noexcept
  {
    return createMask(setBoldMask);
  }

  static constexpr auto dim() noexcept
  {
    return createMask(setDimMask);
  }

  static constexpr auto italic() noexcept
  {
    return createMask(setItalicMask);
  }

  static constexpr auto underline() noexcept
  {
    return createMask(setUnderlineMask);
  }

  static constexpr auto blink() noexcept
  {
    return createMask(setBlinkMask);
  }

  static constexpr auto reverse() noexcept
  {
    return createMask(setReverseMask);
  }

  static constexpr auto standout() noexcept
  {
    return createMask(setStandoutMask);
  }

  static constexpr auto invisible() noexcept
  {
    return createMask(setInvisibleMask);
  }

  static constexpr auto protect() noexcept
  {
    return createMask(setProtectMask);
  }

  static constexpr auto crossed_out() noexcept
  {
    return createMask(setCrossedOutMask);
  }

  static constexpr auto dbl_underline() noexcept
  {
    return createMask(setDblUnderlineMask);
  }

  static constexpr auto alt_charset() noexcept
  {
    return createMask(setAltCharsetMask);
  }

  static constexpr auto pc_charset() noexcept
  {
    return createMask(setPcCharsetMask);
  }

  static constexpr auto transparent() noexcept
  {
    return createMask(setTransparent);
  }

  static constexpr auto color_overlay() noexcept
  {
    return createMask(setColorOverlay);
  }

  static constexpr auto inherit_background() noexcept
  {
    return createMask(setInheritBackground);
  }

  static constexpr auto no_changes() noexcept
  {
    return createMask(setNoChanges);
  }

  static constexpr auto printed() noexcept
  {
    return createMask(setPrinted);
  }

  static constexpr auto fullwidth_padding() noexcept
  {
    return createMask(setFullwidthPadding);
  }

  static constexpr auto char_width() noexcept
  {
    return createMask(setCharWidth);
  }

  static constexpr auto bold_reset() noexcept
  {
    return ~bold();
  }

  static constexpr auto dim_reset() noexcept
  {
    return ~dim();
  }

  static constexpr auto italic_reset() noexcept
  {
    return ~italic();
  }

  static constexpr auto underline_reset() noexcept
  {
    return ~underline();
  }

  static constexpr auto blink_reset() noexcept
  {
    return ~blink();
  }

  static constexpr auto reverse_reset() noexcept
  {
    return ~reverse();
  }

  static constexpr auto standout_reset() noexcept
  {
    return ~standout();
  }

  static constexpr auto invisible_reset() noexcept
  {
    return ~invisible();
  }

  static constexpr auto protect_reset() noexcept
  {
    return ~protect();
  }

  static constexpr auto crossed_out_reset() noexcept
  {
    return ~crossed_out();
  }

  static constexpr auto dbl_underline_reset() noexcept
  {
    return ~dbl_underline();
  }

  static constexpr auto alt_charset_reset() noexcept
  {
    return ~alt_charset();
  }

  static constexpr auto pc_charset_reset() noexcept
  {
    return ~pc_charset();
  }

  static constexpr auto transparent_reset() noexcept
  {
    return ~transparent();
  }

  static constexpr auto color_overlay_reset() noexcept
  {
    return ~color_overlay();
  }

  static constexpr auto inherit_background_reset() noexcept
  {
    return ~inherit_background();
  }

  static constexpr auto no_changes_reset() noexcept
  {
    return ~no_changes();
  }

  static constexpr auto printed_reset() noexcept
  {
    return ~printed();
  }

  static constexpr auto fullwidth_padding_reset() noexcept
  {
    return ~fullwidth_padding();
  }

  static constexpr auto char_width_reset() noexcept
  {
    return ~char_width();
  }
};

}  // namespace internal

constexpr auto getFAttributeByte ( const FCharAttribute& fchar_attr
                                 , std::size_t index ) noexcept -> uInt8
{
  return (FCharAttribute_to_uInt32(fchar_attr) >> (index << 3)) & 0xff;
}

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
constexpr auto getFAttributeData (const FCharAttribute& fchar_attr) noexcept -> uInt32
{
  return FCharAttribute_to_uInt32(fchar_attr);
}
#else
inline auto getFAttributeData (const FCharAttribute& fchar_attr) noexcept -> uInt32
{
  uInt32 data{};
  std::memcpy(&data, &fchar_attr, sizeof(data));
  return data;
}
#endif

#if HAVE_BUILTIN(__builtin_bit_cast)
constexpr auto WordToFAttribute (uInt32 data) noexcept -> FCharAttribute
{
  return __builtin_bit_cast(FCharAttribute, data);
}
#else
inline auto DataToFAttribute (uInt32 data) noexcept -> FCharAttribute
{
  FCharAttribute fchar_attr{};
  std::memcpy(&fchar_attr, &data, sizeof(fchar_attr));
  return fchar_attr;
}
#endif

struct FAttribute
{
  uInt32 data{0};

  auto getFCharAttribute() const noexcept -> FCharAttribute
  {
    FCharAttribute bit;
#if HAVE_BUILTIN(__builtin_bit_cast)
    bit = __builtin_bit_cast(FCharAttribute, data);
#else
    std::memcpy(&bit, &data, sizeof(uInt32));
#endif
    return bit;
  }

  void setFCharAttribute (const FCharAttribute& bit) noexcept
  {
#if HAVE_BUILTIN(__builtin_bit_cast)
    data = __builtin_bit_cast(uInt32, bit);
#else
    std::memcpy(&data, &bit, sizeof(uInt32));
#endif
  }

#if defined(UNIT_TEST)
  struct BitProxy
  {
    FAttribute& parent;
    FCharAttribute bits;

    BitProxy (FAttribute& p)
      : parent(p)
    {
      // Loads data during creation
      std::memcpy(&bits, &parent.data, sizeof(uInt32));
    }

    ~BitProxy()
    {
      // Writes data back when destroying (end of statement)
      std::memcpy(&parent.data, &bits, sizeof(uInt32));
    }

    auto operator -> () -> FCharAttribute*
    {
      // Allows access to bit fields via ->
      return &bits;
    }
  };

  struct ConstBitProxy
  {
    FCharAttribute bits;

    ConstBitProxy (const FAttribute& p)
    {
      // Loads data during creation
      std::memcpy(&bits, &p.data, sizeof(uInt32));
    }

    auto operator -> () const -> const FCharAttribute*
    {
      return &bits;
    }
  };

  BitProxy bit()
  {
    return BitProxy(*this);
  }

  ConstBitProxy bit() const
  {
    return ConstBitProxy(*this);
  }

  struct ByteProxy
  {
    FAttribute& parent;
    std::array<uInt8, 4> byte;

    ByteProxy (FAttribute& p)
      : parent(p)
    {
      // Loads data during creation
      std::memcpy(&byte, &parent.data, sizeof(uInt32));
    }

    ~ByteProxy()
    {
      // Writes data back when destroying (end of statement)
      std::memcpy(&parent.data, &byte, sizeof(uInt32));
    }

    auto operator -> () -> std::array<uInt8, 4>*
    {
      // Allows access to bit fields via ->
      return &byte;
    }
  };

  struct ConstByteProxy
  {
    std::array<uInt8, 4> byte;

    ConstByteProxy (const FAttribute& p)
    {
      std::memcpy(&byte, &p.data, sizeof(uInt32));
    }

    auto operator -> () const -> const std::array<uInt8, 4>*
    {
      return &byte;
    }
  };

  ByteProxy byte()
  {
    return ByteProxy(*this);
  }

  ConstByteProxy byte() const
  {
    return ConstByteProxy(*this);
  }
#endif  // defined(UNIT_TEST)
};

// FUnicode
//----------------------------------------------------------------------
static constexpr std::size_t UNICODE_MAX = 5;

struct FUnicode
{
  wchar_t unicode_data[UNICODE_MAX]{L'\0', L'\0', L'\0', L'\0', L'\0'};

  // Using-declarations
  using iterator        = wchar_t*;
  using const_iterator  = const wchar_t*;
  using pointer         = wchar_t*;
  using const_pointer   = const wchar_t*;
  using reference       = wchar_t&;
  using const_reference = const wchar_t&;
  using value_type      = wchar_t;

  // Overloaded operators
  constexpr auto operator [] (std::size_t index) noexcept -> reference
  {
#if defined(__clang__)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif
    return unicode_data[index];
#if defined(__clang__)
  #pragma clang diagnostic pop
#endif
  }

  constexpr auto operator [] (std::size_t index) const noexcept -> const_reference
  {
#if defined(__clang__)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif
    return unicode_data[index];
#if defined(__clang__)
  #pragma clang diagnostic pop
#endif
  }

  // Methods
  constexpr auto begin() noexcept -> iterator
  {
    return &unicode_data[0];
  }

  constexpr auto end() noexcept -> iterator
  {
#if defined(__clang__)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif
    return &unicode_data[UNICODE_MAX];
#if defined(__clang__)
  #pragma clang diagnostic pop
#endif
  }

  constexpr auto begin() const noexcept -> const_iterator
  {
    return &unicode_data[0];
  }

  constexpr auto end() const noexcept -> const_iterator
  {
#if defined(__clang__)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif
    return &unicode_data[UNICODE_MAX];
#if defined(__clang__)
  #pragma clang diagnostic pop
#endif
  }

  constexpr auto cbegin() const noexcept -> const_iterator
  {
    return &unicode_data[0];
  }

  constexpr auto cend() const noexcept -> const_iterator
  {
#if defined(__clang__)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif
    return &unicode_data[UNICODE_MAX];
#if defined(__clang__)
  #pragma clang diagnostic pop
#endif
  }

  constexpr auto data() noexcept -> pointer
  {
    return &unicode_data[0];
  }

  constexpr auto data() const noexcept -> const_pointer
  {
    return &unicode_data[0];
  }

  friend constexpr auto operator == (const FUnicode& lhs, const FUnicode& rhs) noexcept -> bool
  {
#if HAVE_BUILTIN(__builtin_memcmp)
    return __builtin_memcmp ( std::begin(lhs.unicode_data)
                            , std::begin(rhs.unicode_data)
                            , UNICODE_MAX * sizeof(wchar_t) ) == 0;
#else
    for (std::size_t i{0}; i < UNICODE_MAX; ++i)
    {
      if ( lhs.unicode_data[i] != rhs.unicode_data[i] )
        return false;
    }

    return true;
#endif
  }

  friend constexpr auto operator != (const FUnicode& lhs, const FUnicode& rhs) noexcept -> bool
  {
    return ! ( lhs == rhs );
  }
};

// FChar operator functions
//----------------------------------------------------------------------
#if HAVE_BUILTIN(__builtin_bit_cast)
constexpr auto isFUnicodeEqual (const FUnicode& lhs, const FUnicode& rhs) noexcept -> bool
{
  // Perform a byte-wise comparison
  return lhs == rhs;
}
#else
inline auto isFUnicodeEqual (const FUnicode& lhs, const FUnicode& rhs) noexcept -> bool
{
  // Perform a byte-wise comparison
  return std::equal ( std::begin(lhs.unicode_data)
                    , std::end(lhs.unicode_data)
                    , std::begin(rhs.unicode_data) );
}
#endif

// FCellColor
//----------------------------------------------------------------------
enum class FColor : uInt16;  // forward declaration

struct FColors
{
  FColor fg{};  // Foreground color
  FColor bg{};  // Background color
};

struct FCellColor
{
  uInt32  data{0};  // Color data

  FCellColor() = default;

  explicit constexpr FCellColor (uInt32 value) noexcept
    : data{value}
  { }

  constexpr FCellColor (FColor fg, FColor bg) noexcept
    : data{(uInt32(bg) << 16) | uInt32(fg)}
  { }

  explicit constexpr FCellColor (FColors color) noexcept
    : data{(uInt32(color.bg) << 16) | uInt32(color.fg)}
  { }

  constexpr auto getPair() const noexcept -> FColors
  {
    FColors pair;  // Foreground and background color
#if HAVE_BUILTIN(__builtin_bit_cast)
    pair = __builtin_bit_cast(FColors, data);
#else
    std::memcpy(&pair, &data, sizeof(uInt32));
#endif
    return pair;
  }

  constexpr auto getFgColor() const noexcept -> FColor
  {
    return FColor(data & 0x0000ffffU);
  }

  constexpr auto getBgColor() const noexcept -> FColor
  {
    return FColor(data >> 16);
  }

  constexpr void setPair (const FColors& pair) noexcept
  {
#if HAVE_BUILTIN(__builtin_bit_cast)
    data = __builtin_bit_cast(uInt32, pair);
#else
    std::memcpy(&data, &pair, sizeof(uInt32));
#endif
  }

  constexpr void setFgColor (const FColor& fg) noexcept
  {
    data = (data & 0xffff0000U) | uInt32(fg);
  }

  constexpr void setBgColor (const FColor& bg) noexcept
  {
    data = (data & 0x0000ffffU) | (uInt32(bg) << 16);
  }

  constexpr auto operator = (std::initializer_list<FColor> list) noexcept -> FCellColor&
  {
    if ( list.size() == 2 )
    {
      auto iter = list.begin();
      data = uInt32(*iter);           // Foreground color
      ++iter;
      data |= (uInt32(*iter) << 16);  // Background color
    }

    return *this;
  }

  friend constexpr auto operator == (const FCellColor& lhs, const FCellColor& rhs) noexcept -> bool
  {
    return lhs.data == rhs.data;
  }

  friend constexpr auto operator != (const FCellColor& lhs, const FCellColor& rhs) noexcept -> bool
  {
    return ! ( lhs == rhs );
  }

  friend constexpr void copyBgColor (const FCellColor& from, FCellColor& to) noexcept
  {
    to.data = (to.data & 0x0000ffffU) | (from.data & 0xffff0000U);
  }

  friend constexpr void copyFgColor (const FCellColor& from, FCellColor& to) noexcept
  {
    to.data = (to.data & 0xffff0000U) | (from.data & 0x0000ffffU);
  }
};


// FChar
//----------------------------------------------------------------------
constexpr auto getCompareBitMask() noexcept -> uInt32
{
  FCharAttribute mask{};
  mask.bold = true;
  mask.dim = true;
  mask.italic = true;
  mask.underline = true;
  mask.blink = true;
  mask.reverse = true;
  mask.standout = true;
  mask.invisible = true;
  mask.protect = true;
  mask.crossed_out = true;
  mask.dbl_underline = true;
  mask.alt_charset = true;
  mask.pc_charset = true;
  mask.transparent = true;
  mask.color_overlay = true;
  mask.inherit_background = true;
  mask.fullwidth_padding = true;
  return FCharAttribute_to_uInt32(mask);
}

struct alignas(std::max_align_t) FChar
{
  FUnicode   ch{};            // Character code
  FUnicode   encoded_char{};  // Encoded output character
  FCellColor color{};         // Foreground and background color
  FAttribute attr{};          // Attributes

  // Accessors
  constexpr auto getCharWidth() const noexcept -> uInt32
  {
    return (attr.data & internal::attr::char_width()) >> 19U;
  }

  // Mutators
  constexpr void setCharWidth (uInt32 width) noexcept
  {
    attr.data = (attr.data & internal::attr::char_width_reset()) \
              | ((width << 19U) & internal::attr::char_width());
  }

  constexpr void setBit (uInt32 mask, bool enable) noexcept
  {
    attr.data ^= (-(uInt32(enable)) ^ attr.data) & mask;
  }

  constexpr void setBit (uInt32 mask) noexcept
  {
    attr.data |= mask;
  }

  constexpr void unsetBit (uInt32 mask) noexcept
  {
    attr.data &= mask;
  }

  // Inquiries
  constexpr auto isBitSet (uInt32 mask) const noexcept -> bool
  {
    return attr.data & mask;
  }

  // Friend operator functions
#if HAVE_BUILTIN(__builtin_memcmp)
  friend constexpr
#else
  friend inline
#endif
  auto operator == (const FChar& lhs, const FChar& rhs) noexcept -> bool
  {
#if HAVE_BUILTIN(__builtin_memcmp)
    if ( __builtin_memcmp ( &lhs.color
                          , &rhs.color
                          , sizeof(uInt32) ) != 0 )
      return false;
#else
    if ( std::memcmp( &lhs.color
                    , &rhs.color
                    , sizeof(uInt32) ) != 0 )
      return false;
#endif

    constexpr auto mask = getCompareBitMask();

    if ( (lhs.attr.data & mask) != (rhs.attr.data & mask) )
      return false;

    return isFUnicodeEqual(lhs.ch, rhs.ch);
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
