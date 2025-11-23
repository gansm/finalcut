/***********************************************************************
* fstring.h - Unicode string class with UTF-8 support                  *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2012-2025 Markus Gans                                      *
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

/*  Standalone class
 *  ════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▏
 * ▕ FString ▏
 * ▕▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FSTRING_H
#define FSTRING_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <langinfo.h>

#include <cassert>
#include <cerrno>   // for read errno
#include <cfloat>
#include <climits>
#include <cmath>
#include <cstdarg>  // need for va_list, va_start and va_end
#include <cstdint>
#include <cstdio>   // need for vsprintf
#include <cstring>
#include <cwchar>
#include <cwctype>

#include <algorithm>
#include <array>
#include <iostream>
#include <limits>
#include <memory>
#include <new>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "final/fc.h"
#include "final/ftypes.h"

namespace finalcut
{

// Class forward declaration
class FString;

// Global using-declaration
using FStringList = std::vector<FString>;


//----------------------------------------------------------------------
// class FString
//----------------------------------------------------------------------

class FString
{
  public:
    // Using-declarations - type aliases
    using size_type              = std::wstring::size_type;
    using difference_type        = std::wstring::difference_type;
    using iterator               = std::wstring::iterator;
    using const_iterator         = std::wstring::const_iterator;
    using reverse_iterator       = std::wstring::reverse_iterator;
    using const_reverse_iterator = std::wstring::const_reverse_iterator;
    using reference              = std::wstring::reference;
    using const_reference        = std::wstring::const_reference;
    using value_type             = wchar_t;

    // Constant
    static constexpr size_type npos = std::wstring::npos;

    // Constructors
    FString () = default;
    explicit FString (int);
    explicit FString (size_type);
    FString (size_type, wchar_t);
    FString (size_type, const UniChar&);
    FString (const FString&);            // copy constructor
    FString (FString&&) noexcept;        // move constructor
    FString (const std::wstring&);       // implicit conversion constructor
#if __cplusplus >= 201703L
    FString (const std::wstring_view&);  // implicit conversion constructor
#endif
    FString (std::wstring&&);            // implicit conversion constructor
    FString (const wchar_t[]);           // implicit conversion constructor
    FString (const std::string&);        // implicit conversion constructor
#if __cplusplus >= 201703L
    FString (const std::string_view&);   // implicit conversion constructor
#endif
    FString (const char[]);              // implicit conversion constructor
    FString (const UniChar&);            // implicit conversion constructor
    FString (const wchar_t);             // implicit conversion constructor
    FString (const char);                // implicit conversion constructor

    // Destructor
    virtual ~FString () noexcept;

    // Copy assignment operator (=)
    auto operator = (const FString&) -> FString&;

    // Move assignment operator (=)
    auto operator = (FString&&) noexcept -> FString&;

    // Compound assignment operators
    auto operator += (const FString&) -> const FString&;

    // Stream insertion operators
    auto operator << (const FString&) -> FString&;
    auto operator << (const UniChar&) -> FString&;
    auto operator << (const wchar_t) -> FString&;
    auto operator << (const char) -> FString&;
    template <typename NumT
            , std::enable_if_t< ( std::is_integral<NumT>::value
                             && ! std::is_same<NumT, bool>::value
                             && ! std::is_pointer<NumT>::value )
                             || ( std::is_floating_point<NumT>::value
                             && ! std::is_pointer<NumT>::value )
                              , int> = 0 >
    auto operator << (NumT) -> FString&;

    // Stream extraction operator friends
    friend inline auto operator >> (const FString& lhs, FString& rhs) -> const FString&
    {
      rhs.string.append(lhs.toWString());
      return lhs;
    }

    friend inline auto operator >> (const FString& lhs, std::wstring& rhs) -> const FString&
    {
      rhs.append(lhs.toWString());
      return lhs;
    }

    friend inline auto operator >> (const FString& lhs, std::string& rhs) -> const FString&
    {
      rhs.append(lhs.toString());
      return lhs;
    }

    friend inline auto operator >> (const FString& lhs, wchar_t& rhs) -> const FString&
    {
      rhs = ( ! lhs.isEmpty() ) ? lhs.string[0] : L'\0';
      return lhs;
    }

    friend inline auto operator >> (const FString& lhs, char& rhs) -> const FString&
    {
      rhs = ( ! lhs.isEmpty() ) ? char(uChar(lhs.string[0])) : '\0';
      return lhs;
    }

    friend inline auto operator >> (const FString& lhs, sInt16& rhs) -> const FString&
    {
      rhs = lhs.toShort();
      return lhs;
    }

    friend inline auto operator >> (const FString& lhs, uInt16& rhs) -> const FString&
    {
      rhs = lhs.toUShort();
      return lhs;
    }

    friend inline auto operator >> (const FString& lhs, sInt32& rhs) -> const FString&
    {
      rhs = lhs.toInt();
      return lhs;
    }

    friend inline auto operator >> (const FString& lhs, uInt32& rhs) -> const FString&
    {
      rhs = lhs.toUInt();
      return lhs;
    }

    friend inline auto operator >> (const FString& lhs, sInt64& rhs) -> const FString&
    {
      rhs = lhs.toLong();
      return lhs;
    }

    friend inline auto operator >> (const FString& lhs, uInt64& rhs) -> const FString&
    {
      rhs = lhs.toULong();
      return lhs;
    }

    friend inline auto operator >> (const FString& lhs, double& rhs) -> const FString&
    {
      rhs = lhs.toDouble();
      return lhs;
    }

    friend inline auto operator >> (const FString& lhs, float& rhs) -> const FString&
    {
      rhs = lhs.toFloat();
      return lhs;
    }

    // Index operators
    template <typename IndexT>
    constexpr auto operator [] (const IndexT) -> reference;
    template <typename IndexT>
    constexpr auto operator [] (const IndexT) const -> const_reference;

    // Function call operators
    explicit operator bool () const noexcept;
    auto operator () () const noexcept -> const FString&;

    // Comparison operator friends
    friend inline auto operator < (const FString& lhs, const FString& rhs) -> bool
    {
      return lhs.string < rhs.string;
    }

    template <typename CharT
            , enable_if_char_ptr_t<CharT> = nullptr>
    friend inline auto operator < (const FString& lhs, const CharT& rhs) -> bool
    {
      lhs.char_string = lhs.internal_toCharString(lhs.string);
      return rhs ? lhs.char_string.compare(rhs) < 0 : lhs.char_string.compare("") < 0;
    }

    template <typename CharT
            , enable_if_char_array_t<CharT> = nullptr>
    friend inline auto operator < (const FString& lhs, const CharT& rhs) -> bool
    {
      lhs.char_string = lhs.internal_toCharString(lhs.string);
      return lhs.char_string.compare(rhs) < 0;
    }

    template <typename CharT
            , enable_if_wchar_ptr_t<CharT> = nullptr>
    friend inline auto operator < (const FString& lhs, const CharT& rhs) -> bool
    {
      return rhs ? lhs.string.compare(rhs) < 0 : lhs.string.compare(L"") < 0;
    }

    template <typename CharT
            , enable_if_wchar_array_t<CharT> = nullptr>
    friend inline auto operator < (const FString& lhs, const CharT& rhs) -> bool
    {
      return lhs.string.compare(rhs) < 0;
    }

    friend inline auto operator <= (const FString& lhs, const FString& rhs) -> bool
    {
      return lhs.string <= rhs.string;
    }

    template <typename CharT
            , enable_if_char_ptr_t<CharT> = nullptr>
    friend inline auto operator <= (const FString& lhs, const CharT& rhs) -> bool
    {
      lhs.char_string = lhs.internal_toCharString(lhs.string);
      return rhs ? lhs.char_string.compare(rhs) <= 0 : lhs.char_string.compare("") <= 0;
    }

    template <typename CharT
            , enable_if_char_array_t<CharT> = nullptr>
    friend inline auto operator <= (const FString& lhs, const CharT& rhs) -> bool
    {
      lhs.char_string = lhs.internal_toCharString(lhs.string);
      return lhs.char_string.compare(rhs) <= 0;
    }

    template <typename CharT
            , enable_if_wchar_ptr_t<CharT> = nullptr>
    friend inline auto operator <= (const FString& lhs, const CharT& rhs) -> bool
    {
      return rhs ? lhs.string.compare(rhs) <= 0 : lhs.string.compare(L"") <= 0;
    }

    template <typename CharT
            , enable_if_wchar_array_t<CharT> = nullptr>
    friend inline auto operator <= (const FString& lhs, const CharT& rhs) -> bool
    {
      return lhs.string.compare(rhs) <= 0;
    }

    friend inline auto operator == (const FString& lhs, const FString& rhs) -> bool
    {
      return lhs.string == rhs.string;
    }

    template <typename CharT
            , enable_if_char_ptr_t<CharT> = nullptr>
    friend inline auto operator == (const FString& lhs, const CharT& rhs) -> bool
    {
      lhs.char_string = lhs.internal_toCharString(lhs.string);
      return rhs ? lhs.char_string.compare(rhs) == 0 : lhs.char_string.compare("") == 0;
    }

    template <typename CharT
            , enable_if_char_array_t<CharT> = nullptr>
    friend inline auto operator == (const FString& lhs, const CharT& rhs) -> bool
    {
      lhs.char_string = lhs.internal_toCharString(lhs.string);
      return lhs.char_string.compare(rhs) == 0;
    }

    template <typename CharT
            , enable_if_wchar_ptr_t<CharT> = nullptr>
    friend inline auto operator == (const FString& lhs, const CharT& rhs) -> bool
    {
      return rhs ? lhs.string.compare(rhs) == 0 : lhs.string.compare(L"") == 0;
    }

    template <typename CharT
            , enable_if_wchar_array_t<CharT> = nullptr>
    friend inline auto operator == (const FString& lhs, const CharT& rhs) -> bool
    {
      return lhs.string.compare(rhs) == 0;
    }

    friend inline auto operator != (const FString& lhs, const FString& rhs) -> bool
    {
      return ! ( lhs == rhs );
    }

    template <typename CharT
            , enable_if_char_ptr_t<CharT> = nullptr>
    friend inline auto operator != (const FString& lhs, const CharT& rhs) -> bool
    {
      lhs.char_string = lhs.internal_toCharString(lhs.string);
      return rhs ? lhs.char_string.compare(rhs) != 0 : lhs.char_string.compare("") != 0;
    }

    template <typename CharT
            , enable_if_char_array_t<CharT> = nullptr>
    friend inline auto operator != (const FString& lhs, const CharT& rhs) -> bool
    {
      lhs.char_string = lhs.internal_toCharString(lhs.string);
      return lhs.char_string.compare(rhs) != 0;
    }

    template <typename CharT
            , enable_if_wchar_ptr_t<CharT> = nullptr>
    friend inline auto operator != (const FString& lhs, const CharT& rhs) -> bool
    {
      return rhs ? lhs.string.compare(rhs) != 0 : lhs.string.compare(L"") != 0;
    }

    template <typename CharT
            , enable_if_wchar_array_t<CharT> = nullptr>
    friend inline auto operator != (const FString& lhs, const CharT& rhs) -> bool
    {
      return lhs.string.compare(rhs) != 0;
    }

    friend inline auto operator >= (const FString& lhs, const FString& rhs) -> bool
    {
      return lhs.string >= rhs.string;
    }

    template <typename CharT
            , enable_if_char_ptr_t<CharT> = nullptr>
    friend inline auto operator >= (const FString& lhs, const CharT& rhs) -> bool
    {
      lhs.char_string = lhs.internal_toCharString(lhs.string);
      return rhs ? lhs.char_string.compare(rhs) >= 0 : lhs.char_string.compare("") >= 0;
    }

    template <typename CharT
            , enable_if_char_array_t<CharT> = nullptr>
    friend inline auto operator >= (const FString& lhs, const CharT& rhs)  -> bool
    {
      lhs.char_string = lhs.internal_toCharString(lhs.string);
      return lhs.char_string.compare(rhs) >= 0;
    }

    template <typename CharT
            , enable_if_wchar_ptr_t<CharT> = nullptr>
    friend inline auto operator >= (const FString& lhs, const CharT& rhs) -> bool
    {
      return rhs ? lhs.string.compare(rhs) >= 0 : lhs.string.compare(L"") >= 0;
    }

    template <typename CharT
            , enable_if_wchar_array_t<CharT> = nullptr>
    friend inline auto operator >= (const FString& lhs, const CharT& rhs) -> bool
    {
      return lhs.string.compare(rhs) >= 0;
    }

    friend inline auto operator > (const FString& lhs, const FString& rhs) -> bool
    {
      return lhs.string > rhs.string;
    }

    template <typename CharT
            , enable_if_char_ptr_t<CharT> = nullptr>
    friend inline auto operator > (const FString& lhs, const CharT& rhs) -> bool
    {
      lhs.char_string = lhs.internal_toCharString(lhs.string);
      return rhs ? lhs.char_string.compare(rhs) > 0 : lhs.char_string.compare("") > 0;
    }

    template <typename CharT
            , enable_if_char_array_t<CharT> = nullptr>
    friend inline auto operator > (const FString& lhs, const CharT& rhs) -> bool
    {
      lhs.char_string = lhs.internal_toCharString(lhs.string);
      return lhs.char_string.compare(rhs) > 0;
    }

    template <typename CharT
            , enable_if_wchar_ptr_t<CharT> = nullptr>
    friend inline auto operator > (const FString& lhs, const CharT& rhs) -> bool
    {
      return rhs ? lhs.string.compare(rhs) > 0 : lhs.string.compare(L"") > 0;
    }

    template <typename CharT
            , enable_if_wchar_array_t<CharT> = nullptr>
    friend inline auto operator > (const FString& lhs, const CharT& rhs) -> bool
    {
      return lhs.string.compare(rhs) > 0;
    }

    // Accessors
    virtual auto getClassName() const -> FString;
    auto getLength() const noexcept -> size_type;
    auto capacity() const noexcept -> size_type;  // STL Compatibility
    auto size() const noexcept -> size_type;      // STL Compatibility
    auto length() const noexcept -> size_type;    // STL Compatibility
    auto max_size() const noexcept -> size_type;  // STL Compatibility

    // Inquiry
    auto isEmpty() const noexcept -> bool;

    // Iterators
    auto begin() noexcept -> iterator;
    auto end() noexcept -> iterator;
    auto begin() const -> const_iterator;
    auto end() const -> const_iterator;
    auto cbegin() const noexcept -> const_iterator;
    auto cend() const noexcept -> const_iterator;
    auto rbegin() noexcept -> reverse_iterator;
    auto rend() noexcept -> reverse_iterator;
    auto crbegin() const noexcept -> const_reverse_iterator;
    auto crend() const noexcept -> const_reverse_iterator;

    // Element access
    auto front() -> reference;
    auto back() -> reference;
    auto front() const -> const_reference;
    auto back() const -> const_reference;
    auto at (size_type) -> reference;              // STL Compatibility
    auto at (size_type) const -> const_reference;  // STL Compatibility

    // String operations
    template <typename... Args>
    auto sprintf (const FString&, Args&&...) -> FString&;
    auto clear() noexcept -> FString&;
    auto reserve (size_type) -> FString&;          // STL Compatibility
    auto shrink_to_fit() -> FString&;              // STL Compatibility
    auto resize (size_type) -> FString&;           // STL Compatibility
    auto resize (size_type, wchar_t) -> FString&;  // STL Compatibility

    // wc-string and c-string conversion methods
    auto wc_str() const noexcept -> const wchar_t*;
    auto wc_str() noexcept -> wchar_t*;
    auto c_str() const -> const char*;
    auto c_str() -> char*;
    auto data() const noexcept -> const wchar_t*;  // STL Compatibility
    auto data() noexcept -> wchar_t*;              // STL Compatibility
    auto toWString() const -> std::wstring;
    auto toString() const -> std::string;

    // Case conversion
    auto toLower() const -> FString;
    auto toUpper() const -> FString;

    // Numeric conversions
    auto toShort() const -> sInt16;
    auto toUShort() const -> uInt16;
    auto toInt() const -> int;
    auto toUInt() const -> uInt;
    auto toLong() const -> long;
    auto toULong() const -> uLong;
    auto toFloat() const -> float;
    auto toDouble() const -> double;

    // String manipulation
    auto ltrim() const -> FString;
    auto rtrim() const -> FString;
    auto trim() const -> FString;
    auto left (size_type) const -> FString;
    auto right (size_type) const -> FString;
    auto mid (size_type, size_type) const -> FString;
    auto split (const FString&) const -> FStringList;

    // Assignment methods
    auto setString (const FString&) -> FString&;
    template <typename NumT>
    auto setNumber (NumT, int = int(getPrecision<NumT>())) -> FString&;
    auto setNumber (sInt64) -> FString&;
    auto setNumber (uInt64) -> FString&;
    auto setNumber (lDouble, int = int(getPrecision<lDouble>())) -> FString&;

    template <typename NumT>
    auto setFormatedNumber (NumT, FString&& = nl_langinfo(THOUSEP)) -> FString&;
    auto setFormatedNumber (sInt64, FString = nl_langinfo(THOUSEP)) -> FString&;
    auto setFormatedNumber (uInt64, FString = nl_langinfo(THOUSEP)) -> FString&;

    // String modification
    auto insert (const FString&, int) -> const FString&;
    auto insert (const FString&, size_type) -> const FString&;

    auto replace (const FString&, const FString&) const -> FString;

    auto replaceControlCodes() const -> FString;
    auto expandTabs (int = 8) const -> FString;
    auto removeDel() const -> FString;
    auto removeBackspaces() const -> FString;

    auto overwrite (const FString&, int) -> FString&;
    auto overwrite (const FString&, size_type = 0) -> FString&;

    auto remove (size_type = 0, size_type = npos) -> FString&;
    auto erase (size_type pos = 0, size_type len = npos) -> FString&;  // STL Compatibility

    // Search operations
    auto includes (const FString&) const noexcept -> bool;
    auto contains (const FString&) const noexcept -> bool;  // STL Compatibility
    auto find (const FString&, size_type = 0) const noexcept -> size_type;  // STL Compatibility
    auto rfind (const FString&, size_type = npos) const noexcept -> size_type;  // STL Compatibility

    // Utility methods
    void swap (FString&) noexcept;

  private:
    // Enumeration
    enum class Sign : bool { Negative, Positive };

    // Constants
    static constexpr auto INPBUFFER = size_type(256);
    static constexpr auto SPRINTF_BUFFER_SIZE = size_type(4096);
    static constexpr auto NUMBER_BUFFER_SIZE = size_type(30);
    static constexpr auto NUMBER_BUFFER_LENGTH = NUMBER_BUFFER_SIZE - 1;
    static constexpr auto MALFORMED_STRING = size_type(-1);
    static constexpr auto LONG_LIMIT = long(LONG_MAX / 10);
    static constexpr auto LONG_MIN_LIMIT_DIGIT = long(-(LONG_MIN % 10) + 1);
    static constexpr auto LONG_MAX_LIMIT_DIGIT = long(LONG_MAX % 10);
    static constexpr auto ULONG_LIMIT = uLong(ULONG_MAX / 10);
    static constexpr auto ULONG_LIMIT_DIGIT = uLong(ULONG_MAX % 10);

    // Methods
    void internal_assign (std::wstring) noexcept;
    auto internal_toCharString (const std::wstring&) const -> std::string;
    auto internal_toWideString (const char[]) const -> std::wstring;
    void internal_skipLeadingWs (const wchar_t*&, const wchar_t*) const noexcept;
    void internal_skipTrailingWs (const wchar_t*, const wchar_t*&) const noexcept;
    auto internal_parseSign (const wchar_t*&) const noexcept -> Sign;
    auto internal_parseDigits (const wchar_t*&, const wchar_t*, Sign) const -> long;
    auto internal_parseDigits (const wchar_t*&, const wchar_t*) const -> uLong;
    auto internal_isOverflowed (Sign, long, long) const noexcept -> bool;
    auto internal_isOverflowed (uLong, uLong) const noexcept -> bool;

    // Data members
    std::wstring         string{};
    mutable std::string  char_string{};
    static wchar_t       null_char;
    static const wchar_t const_null_char;

    // Friend Non-member operator functions
    friend auto operator + (const FString& s1, const FString& s2) -> FString
    {
      const auto& tmp = s1.string + s2.string;
      return tmp;
    }

    friend auto operator << (std::ostream& outstr, const FString& s) -> std::ostream&
    {
      const auto width = size_type(outstr.width());

      if ( s.string.length() > 0 )
      {
        outstr << s.internal_toCharString(s.string);
      }
      else if ( width > 0 )
      {
        const std::string fill_str(width, outstr.fill());
        outstr << fill_str;
      }

      return outstr;
    }

    friend auto operator >> (std::istream& instr, FString& s) -> std::istream&
    {
      std::array<char, FString::INPBUFFER + 1> buf{};
      instr.getline (buf.data(), FString::INPBUFFER);
      auto wide_string = s.internal_toWideString(buf.data());

      if ( ! wide_string.empty() )
      {
        s.internal_assign (std::move(wide_string));
      }

      return instr;
    }

    friend auto operator << (std::wostream& outstr, const FString& s) -> std::wostream&
    {
      const auto width = size_type(outstr.width());

      if ( s.string.length() > 0 )
      {
        outstr << s.string;
      }
      else if ( width > 0 )
      {
        const std::wstring fill_str(width, outstr.fill());
        outstr << fill_str;
      }

      return outstr;
    }

    friend auto operator >> (std::wistream& instr, FString& s) -> std::wistream&
    {
      std::array<wchar_t, FString::INPBUFFER + 1> buf{};
      instr.getline (buf.data(), FString::INPBUFFER);
      std::wstring str(buf.data());
      s.internal_assign (std::move(str));
      return instr;
    }

    // Friend struct
    friend struct std::hash<finalcut::FString>;
};

// non-member function forward declarations
//----------------------------------------------------------------------
auto FStringCaseCompare (const FString&, const FString&) -> int;

// FString inline functions
//----------------------------------------------------------------------
template <typename NumT
        , std::enable_if_t< ( std::is_integral<NumT>::value
                         && ! std::is_same<NumT, bool>::value
                         && ! std::is_pointer<NumT>::value )
                         || ( std::is_floating_point<NumT>::value
                         && ! std::is_pointer<NumT>::value )
                          , int> >
inline auto FString::operator << (NumT value) -> FString&
{
  const FString numstr(FString().setNumber(value));
  string.append(numstr.string);
  return *this;
}

//----------------------------------------------------------------------
template <typename IndexT>
constexpr auto FString::operator [] (const IndexT pos) -> reference
{
  if ( isNegative(pos) || pos > IndexT(string.length()) )  // Invalid index position
    throw std::out_of_range("FString::operator[] index out of range");

  if ( size_type(pos) == string.length() )
    return null_char;

  return string[size_type(pos)];
}

//----------------------------------------------------------------------
template <typename IndexT>
constexpr auto FString::operator [] (const IndexT pos) const -> const_reference
{
  if ( isNegative(pos) || pos > IndexT(string.length()) )  // Invalid index position
    throw std::out_of_range("FString::operator[] index out of range");

  if ( size_type(pos) == string.length() )
    return const_null_char;

  return string[size_type(pos)];
}

//----------------------------------------------------------------------
inline auto FString::getClassName() const -> FString
{ return "FString"; }

//----------------------------------------------------------------------
inline auto FString::getLength() const noexcept -> size_type
{ return string.length(); }

//----------------------------------------------------------------------
inline auto FString::capacity() const noexcept -> size_type
{ return string.capacity(); }

//----------------------------------------------------------------------
inline auto FString::size() const noexcept -> size_type
{ return string.size(); }

//----------------------------------------------------------------------
inline auto FString::length() const noexcept -> size_type
{ return string.length(); }

//----------------------------------------------------------------------
inline auto FString::max_size() const noexcept -> size_type
{ return string.max_size(); }

//----------------------------------------------------------------------
inline auto FString::isEmpty() const noexcept -> bool
{ return string.empty(); }

//----------------------------------------------------------------------
inline auto FString::begin() noexcept -> iterator
{ return string.begin(); }

//----------------------------------------------------------------------
inline auto FString::end() noexcept -> iterator
{ return string.end(); }

//----------------------------------------------------------------------
inline auto FString::begin() const -> const_iterator
{ return this->string.begin(); }

//----------------------------------------------------------------------
inline auto FString::end() const -> const_iterator
{ return this->string.end(); }

//----------------------------------------------------------------------
inline auto FString::cbegin() const noexcept -> const_iterator
{ return this->string.cbegin(); }

//----------------------------------------------------------------------
inline auto FString::cend() const noexcept -> const_iterator
{ return this->string.cend(); }

//----------------------------------------------------------------------
inline auto FString::rbegin() noexcept -> reverse_iterator
{ return string.rbegin(); }

//----------------------------------------------------------------------
inline auto FString::rend() noexcept -> reverse_iterator
{ return string.rend(); }

//----------------------------------------------------------------------
inline auto FString::crbegin() const noexcept -> const_reverse_iterator
{ return string.crbegin(); }

//----------------------------------------------------------------------
inline auto FString::crend() const noexcept -> const_reverse_iterator
{ return string.crend(); }

//----------------------------------------------------------------------
inline auto FString::front() -> reference
{
  assert ( ! isEmpty() && "FString::front() called on empty string" );
  return string.front();
}

//----------------------------------------------------------------------
inline auto FString::back() -> reference
{
  assert( ! isEmpty() && "FString::back() called on empty string" );
  return string.back();
}

//----------------------------------------------------------------------
inline auto FString::front() const -> const_reference
{
  assert ( ! isEmpty() && "FString::front() called on empty string" );
  return string.front();
}

//----------------------------------------------------------------------
inline auto FString::back() const -> const_reference
{
  assert( ! isEmpty() && "FString::back() called on empty string" );
  return string.back();
}

//----------------------------------------------------------------------
inline auto FString::at (size_type pos) -> reference
{ return string.at(pos); }

//----------------------------------------------------------------------
inline auto FString::at (size_type pos) const -> const_reference
{ return string.at(pos); }

//----------------------------------------------------------------------
template <typename... Args>
inline auto FString::sprintf (const FString& format, Args&&... args) -> FString&
{
  std::array<wchar_t, SPRINTF_BUFFER_SIZE> buf{};

  if ( format.isEmpty() )
  {
    clear();
    return *this;
  }

  const auto written = std::swprintf ( buf.data()
                                     , buf.size()
                                     , format.wc_str()
                                     , std::forward<Args>(args)... );

  if ( written < 0 || size_type(written) >= buf.size() )
    throw std::runtime_error("FString::sprintf buffer overflow or encoding error");

  return setString(buf.data());
}

//----------------------------------------------------------------------
inline auto FString::data() const noexcept -> const wchar_t*
{ return string.data(); }

//----------------------------------------------------------------------
inline auto FString::data() noexcept -> wchar_t*
{ return const_cast<wchar_t*>(string.data()); }

//----------------------------------------------------------------------
template <typename NumT>
inline auto FString::setNumber (NumT value, int precision) -> FString&
{
  if ( std::is_floating_point<NumT>::value )
    return setNumber (lDouble(value), precision);

  if ( isNegative(value) )
    return setNumber (sInt64(value));

  return setNumber (uInt64(value));
}

//----------------------------------------------------------------------
template <typename NumT>
inline auto FString::setFormatedNumber (NumT value, FString&& separator) -> FString&
{
  if ( isNegative(value) )
    return setFormatedNumber (sInt64(value), std::move(separator));

  return setFormatedNumber (uInt64(value), std::move(separator));
}

//----------------------------------------------------------------------
inline void FString::swap (FString& s) noexcept
{
  s.string.swap(string);
}

//----------------------------------------------------------------------
inline void FString::internal_assign (std::wstring s) noexcept
{
  s.swap(string);
}

}  // namespace finalcut

//----------------------------------------------------------------------
// Specialization of std::hash for FString

namespace std
{

template <>
struct hash<finalcut::FString>
{
  auto operator () (const finalcut::FString& str) const noexcept -> std::size_t
  {
    return std::hash<std::wstring>{}(str.string);
  }
};

}  // namespace std

#endif  // FSTRING_H
