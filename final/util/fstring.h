/***********************************************************************
* fstring.h - Unicode string class with UTF-8 support                  *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2012-2024 Markus Gans                                      *
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

#include <array>
#include <iostream>
#include <limits>
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

// class forward declaration
class FString;

// Global using-declaration
using FStringList = std::vector<FString>;


//----------------------------------------------------------------------
// class FString
//----------------------------------------------------------------------

class FString
{
  public:
    // Using-declarations
    using size_type       = std::wstring::size_type;
    using difference_type = std::wstring::difference_type;
    using iterator        = std::wstring::iterator;
    using const_iterator  = std::wstring::const_iterator;
    using reference       = std::wstring::reference;
    using const_reference = std::wstring::const_reference;

    // Constructors
    FString () = default;
    explicit FString (int);
    explicit FString (std::size_t);
    FString (std::size_t, wchar_t);
    FString (std::size_t, const UniChar&);
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
    FString (const std::string_view&);  // implicit conversion constructor
#endif
    FString (const char[]);             // implicit conversion constructor
    FString (const UniChar&);           // implicit conversion constructor
    FString (const wchar_t);            // implicit conversion constructor
    FString (const char);               // implicit conversion constructor

    // Destructor
    virtual ~FString ();

    // Overloaded operators
    auto operator = (const FString&) -> FString&;
    auto operator = (FString&&) noexcept -> FString&;

    auto operator += (const FString&) -> const FString&;

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
    auto operator << (const NumT) -> FString&;

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

    template <typename IndexT>
    constexpr auto operator [] (const IndexT) -> reference;
    template <typename IndexT>
    constexpr auto operator [] (const IndexT) const -> const_reference;
    explicit operator bool () const;
    auto operator () () const -> const FString&;

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

    // Accessor
    virtual auto getClassName() const -> FString;

    // inquiries
    auto isEmpty() const noexcept -> bool;

    // Methods
    auto getLength() const noexcept -> std::size_t;
    auto capacity() const noexcept -> std::size_t;

    auto begin() noexcept -> iterator;
    auto end() noexcept -> iterator;
    auto begin() const -> const_iterator;
    auto end() const -> const_iterator;
    auto cbegin() const noexcept -> const_iterator;
    auto cend() const noexcept -> const_iterator;
    auto front() -> reference;
    auto back() -> reference;
    auto front() const -> const_reference;
    auto back() const -> const_reference;

    template <typename... Args>
    auto sprintf (const FString&, Args&&...) -> FString&;
    auto clear() -> FString&;

    auto wc_str() const -> const wchar_t*;
    auto wc_str() -> wchar_t*;
    auto c_str() const -> const char*;
    auto c_str() -> char*;
    auto toWString() const -> std::wstring;
    auto toString() const -> std::string;

    auto toLower() const -> FString;
    auto toUpper() const -> FString;

    auto toShort() const -> sInt16;
    auto toUShort() const -> uInt16;
    auto toInt() const -> int;
    auto toUInt() const -> uInt;
    auto toLong() const -> long;
    auto toULong() const -> uLong;
    auto toFloat() const -> float;
    auto toDouble() const -> double;

    auto ltrim() const -> FString;
    auto rtrim() const -> FString;
    auto trim() const -> FString;

    auto left (std::size_t) const -> FString;
    auto right (std::size_t) const -> FString;
    auto mid (std::size_t, std::size_t) const -> FString;

    auto split (const FString&) const -> FStringList;
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

    auto insert (const FString&, int) -> const FString&;
    auto insert (const FString&, std::size_t) -> const FString&;

    auto replace (const FString&, const FString&) const -> FString;

    auto replaceControlCodes() const -> FString;
    auto expandTabs (int = 8) const -> FString;
    auto removeDel() const -> FString;
    auto removeBackspaces() const -> FString;

    auto overwrite (const FString&, int) -> const FString&;
    auto overwrite (const FString&, std::size_t = 0) -> const FString&;

    auto remove (std::size_t, std::size_t) -> const FString&;
    auto includes (const FString&) const -> bool;

  private:
    // Constants
    static constexpr auto INPBUFFER = uInt(200);
    static constexpr auto MALFORMED_STRING = static_cast<std::size_t>(-1);

    // Methods
    void internal_assign (std::wstring);
    auto internal_toCharString (const std::wstring&) const -> std::string;
    auto internal_toWideString (const char[]) const -> std::wstring;

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
      const auto& width = std::size_t(outstr.width());

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
      const auto& width = std::size_t(outstr.width());

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
inline auto FString::operator << (const NumT val) -> FString&
{
  const FString numstr(FString().setNumber(val));
  string.append(numstr.string);
  return *this;
}

//----------------------------------------------------------------------
template <typename IndexT>
constexpr auto FString::operator [] (const IndexT pos) -> reference
{
  if ( isNegative(pos) || pos > IndexT(string.length()) )
    throw std::out_of_range("");  // Invalid index position

  if ( std::size_t(pos) == string.length() )
    return null_char;

  return string[std::size_t(pos)];
}

//----------------------------------------------------------------------
template <typename IndexT>
constexpr auto FString::operator [] (const IndexT pos) const -> const_reference
{
  if ( isNegative(pos) || pos > IndexT(string.length()) )
    throw std::out_of_range("");  // Invalid index position

  if ( std::size_t(pos) == string.length() )
    return const_null_char;

  return string[std::size_t(pos)];
}

//----------------------------------------------------------------------
inline auto FString::getClassName() const -> FString
{ return "FString"; }

//----------------------------------------------------------------------
inline auto FString::isEmpty() const noexcept -> bool
{ return string.empty(); }

//----------------------------------------------------------------------
inline auto FString::getLength() const noexcept -> std::size_t
{ return string.length(); }

//----------------------------------------------------------------------
inline auto FString::capacity() const noexcept -> std::size_t
{ return string.capacity(); }

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
inline auto FString::front() -> reference
{
  assert ( ! isEmpty() );
  return string.front();
}

//----------------------------------------------------------------------
inline auto FString::back() -> reference
{
  assert( ! isEmpty() );
  return string.back();
}

//----------------------------------------------------------------------
inline auto FString::front() const -> const_reference
{
  assert ( ! isEmpty() );
  return string.front();
}

//----------------------------------------------------------------------
inline auto FString::back() const -> const_reference
{
  assert( ! isEmpty() );
  return string.back();
}

//----------------------------------------------------------------------
template <typename... Args>
inline auto FString::sprintf (const FString& format, Args&&... args) -> FString&
{
  std::array<wchar_t, 4096> buf{};

  if ( format.isEmpty() )
  {
    clear();
    return *this;
  }

  std::swprintf ( buf.data(), buf.size(), format.wc_str()
                , std::forward<Args>(args)... );
  return setString(buf.data());
}

//----------------------------------------------------------------------
template <typename NumT>
inline auto FString::setNumber (NumT num, int precision) -> FString&
{
  if ( std::is_floating_point<NumT>::value )
    return setNumber (lDouble(num), precision);

  if ( isNegative(num) )
    return setNumber (sInt64(num));

  return setNumber (uInt64(num));
}

//----------------------------------------------------------------------
template <typename NumT>
inline auto FString::setFormatedNumber (NumT num, FString&& separator) -> FString&
{
  if ( isNegative(num) )
    return setFormatedNumber (sInt64(num), std::move(separator));

  return setFormatedNumber (uInt64(num), std::move(separator));
}

//----------------------------------------------------------------------
inline void FString::internal_assign (std::wstring s)
{
  s.swap(string);
}

}  // namespace finalcut

//----------------------------------------------------------------------
template <>
struct std::hash<finalcut::FString>
{
  auto operator () (const finalcut::FString& p) const noexcept -> std::size_t
  {
    return std::hash<std::wstring>{}(p.string);
  }
};

#endif  // FSTRING_H
