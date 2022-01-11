/***********************************************************************
* fstring.h - Unicode string class with UTF-8 support                  *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2012-2021 Markus Gans                                      *
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
    using iterator        = std::wstring::iterator;
    using const_iterator  = std::wstring::const_iterator;
    using reference       = std::wstring::reference;
    using const_reference = std::wstring::const_reference;
    using difference_type = std::wstring::difference_type;

    // Constructors
    FString () = default;
    explicit FString (int);
    explicit FString (std::size_t);
    FString (std::size_t, wchar_t);
    FString (std::size_t, const UniChar&);
    FString (const FString&);        // copy constructor
    FString (FString&&) noexcept;    // move constructor
    FString (const std::wstring&);   // implicit conversion constructor
    FString (std::wstring&&);        // implicit conversion constructor
    FString (const wchar_t[]);       // implicit conversion constructor
    FString (const std::string&);    // implicit conversion constructor
    FString (const char[]);          // implicit conversion constructor
    FString (const UniChar&);        // implicit conversion constructor
    FString (const wchar_t);         // implicit conversion constructor
    FString (const char);            // implicit conversion constructor

    // Destructor
    virtual ~FString ();

    // Overloaded operators
    FString& operator = (const FString&);
    FString& operator = (FString&&) noexcept;

    const FString& operator += (const FString&);

    FString& operator << (const FString&);
    FString& operator << (const UniChar&);
    FString& operator << (const wchar_t);
    FString& operator << (const char);
    template <typename NumT
            , std::enable_if_t< ( std::is_integral<NumT>::value
                             && ! std::is_same<NumT, bool>::value
                             && ! std::is_pointer<NumT>::value )
                             || ( std::is_floating_point<NumT>::value
                             && ! std::is_pointer<NumT>::value )
                              , int> = 0 >
    FString& operator << (const NumT);

    const FString& operator >> (FString&) const;
    const FString& operator >> (std::wstring&) const;
    const FString& operator >> (std::string&) const;
    const FString& operator >> (wchar_t&) const;
    const FString& operator >> (char&) const;
    const FString& operator >> (sInt16&) const;
    const FString& operator >> (uInt16&) const;
    const FString& operator >> (sInt32&) const;
    const FString& operator >> (uInt32&) const;
    const FString& operator >> (sInt64&) const;
    const FString& operator >> (uInt64&) const;
    const FString& operator >> (double&) const;
    const FString& operator >> (float&) const;

    template <typename IndexT>
    constexpr reference operator [] (const IndexT);
    template <typename IndexT>
    constexpr const_reference operator [] (const IndexT) const;
    explicit operator bool () const;
    const FString& operator () () const;

    bool operator < (const FString&) const;
    template <typename CharT
            , enable_if_char_ptr_t<CharT> = nullptr>
    bool operator < (const CharT&) const;
    template <typename CharT
            , enable_if_char_array_t<CharT> = nullptr>
    bool operator < (const CharT&) const;
    template <typename CharT
            , enable_if_wchar_ptr_t<CharT> = nullptr>
    bool operator < (const CharT&) const;
    template <typename CharT
            , enable_if_wchar_array_t<CharT> = nullptr>
    bool operator < (const CharT&) const;

    bool operator <= (const FString&) const;
    template <typename CharT
            , enable_if_char_ptr_t<CharT> = nullptr>
    bool operator <= (const CharT&) const;
    template <typename CharT
            , enable_if_char_array_t<CharT> = nullptr>
    bool operator <= (const CharT&) const;
    template <typename CharT
            , enable_if_wchar_ptr_t<CharT> = nullptr>
    bool operator <= (const CharT&) const;
    template <typename CharT
            , enable_if_wchar_array_t<CharT> = nullptr>
    bool operator <= (const CharT&) const;

    bool operator == (const FString&) const;
    template <typename CharT
            , enable_if_char_ptr_t<CharT> = nullptr>
    bool operator == (const CharT&) const;
    template <typename CharT
            , enable_if_char_array_t<CharT> = nullptr>
    bool operator == (const CharT&) const;
    template <typename CharT
            , enable_if_wchar_ptr_t<CharT> = nullptr>
    bool operator == (const CharT&) const;
    template <typename CharT
            , enable_if_wchar_array_t<CharT> = nullptr>
    bool operator == (const CharT&) const;

    bool operator != (const FString&) const;
    template <typename CharT
            , enable_if_char_ptr_t<CharT> = nullptr>
    bool operator != (const CharT&) const;
    template <typename CharT
            , enable_if_char_array_t<CharT> = nullptr>
    bool operator != (const CharT&) const;
    template <typename CharT
            , enable_if_wchar_ptr_t<CharT> = nullptr>
    bool operator != (const CharT&) const;
    template <typename CharT
            , enable_if_wchar_array_t<CharT> = nullptr>
    bool operator != (const CharT&) const;

    bool operator >= (const FString&) const;
    template <typename CharT
            , enable_if_char_ptr_t<CharT> = nullptr>
    bool operator >= (const CharT&) const;
    template <typename CharT
            , enable_if_char_array_t<CharT> = nullptr>
    bool operator >= (const CharT&) const;
    template <typename CharT
            , enable_if_wchar_ptr_t<CharT> = nullptr>
    bool operator >= (const CharT&) const;
    template <typename CharT
            , enable_if_wchar_array_t<CharT> = nullptr>
    bool operator >= (const CharT&) const;

    bool operator > (const FString&) const;
    template <typename CharT
            , enable_if_char_ptr_t<CharT> = nullptr>
    bool operator > (const CharT&) const;
    template <typename CharT
            , enable_if_char_array_t<CharT> = nullptr>
    bool operator > (const CharT&) const;
    template <typename CharT
            , enable_if_wchar_ptr_t<CharT> = nullptr>
    bool operator > (const CharT&) const;
    template <typename CharT
            , enable_if_wchar_array_t<CharT> = nullptr>
    bool operator > (const CharT&) const;

    // Accessor
    virtual FString getClassName() const;

    // inquiries
    [[deprecated("No longer needed")]] bool isNull() const noexcept;
    bool isEmpty() const noexcept;

    // Methods
    std::size_t getLength() const noexcept;
    std::size_t capacity() const noexcept;

    iterator begin() noexcept;
    iterator end() noexcept;
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;
    reference front();
    reference back();
    const_reference front() const;
    const_reference back() const;

    template <typename... Args>
    FString& sprintf (const FString&, Args&&...);
    FString  clear();

    const wchar_t* wc_str() const;
    wchar_t* wc_str();
    const char* c_str() const;
    char* c_str();
    std::wstring toWString() const;
    std::string toString() const;

    FString toLower()  const;
    FString toUpper()  const;

    sInt16  toShort()  const;
    uInt16  toUShort() const;
    int     toInt()    const;
    uInt    toUInt()   const;
    long    toLong()   const;
    uLong   toULong()  const;
    float   toFloat()  const;
    double  toDouble() const;

    FString ltrim() const;
    FString rtrim() const;
    FString trim()  const;

    FString left (std::size_t) const;
    FString right (std::size_t) const;
    FString mid (std::size_t, std::size_t) const;

    FStringList split (const FString&) const;
    FString& setString (const FString&);

    template <typename NumT>
    FString& setNumber (NumT, int = int(getPrecision<NumT>()));
    FString& setNumber (sInt64);
    FString& setNumber (uInt64);
    FString& setNumber (lDouble, int = int(getPrecision<lDouble>()));

    template <typename NumT>
    FString& setFormatedNumber (NumT, char = nl_langinfo(THOUSEP)[0]);
    FString& setFormatedNumber (sInt64, char = nl_langinfo(THOUSEP)[0]);
    FString& setFormatedNumber (uInt64, char = nl_langinfo(THOUSEP)[0]);

    const FString& insert (const FString&, int);
    const FString& insert (const FString&, std::size_t);

    FString replace (const FString&, const FString&) const;

    FString replaceControlCodes() const;
    FString expandTabs (int = 8) const;
    FString removeDel() const;
    FString removeBackspaces() const;

    const FString& overwrite (const FString&, int);
    const FString& overwrite (const FString&, std::size_t = 0);

    const FString& remove (std::size_t, std::size_t);
    bool  includes (const FString&) const;

  private:
    // Constants
    static constexpr uInt INPBUFFER = 200;

    // Methods
    void         internal_assign (std::wstring&);
    std::string  internal_toCharString (const std::wstring&) const;
    std::wstring internal_toWideString (const std::string&) const;

    // Data members
    std::wstring         string{};
    mutable std::string  char_string{};
    static wchar_t       null_char;
    static const wchar_t const_null_char;

    // Friend Non-member operator functions
    friend FString operator + (const FString&, const FString&);

    friend std::ostream&  operator << (std::ostream&, const FString&);
    friend std::istream&  operator >> (std::istream&, FString& s);
    friend std::wostream& operator << (std::wostream&, const FString&);
    friend std::wistream& operator >> (std::wistream&, FString&);

    // Friend struct
    friend struct std::hash<finalcut::FString>;
};

// non-member function forward declarations
//----------------------------------------------------------------------
int FStringCaseCompare (const FString&, const FString&);

// FString inline functions
//----------------------------------------------------------------------
template <typename NumT
        , std::enable_if_t< ( std::is_integral<NumT>::value
                         && ! std::is_same<NumT, bool>::value
                         && ! std::is_pointer<NumT>::value )
                         || ( std::is_floating_point<NumT>::value
                         && ! std::is_pointer<NumT>::value )
                          , int> >
inline FString& FString::operator << (const NumT val)
{
  const FString numstr(FString().setNumber(val));
  string.append(numstr.string);
  return *this;
}

//----------------------------------------------------------------------
template <typename IndexT>
constexpr FString::reference FString::operator [] (const IndexT pos)
{
  if ( isNegative(pos) || pos > IndexT(string.length()) )
    throw std::out_of_range("");  // Invalid index position

  if ( std::size_t(pos) == string.length() )
    return null_char;

  return string[std::size_t(pos)];
}

//----------------------------------------------------------------------
template <typename IndexT>
constexpr FString::const_reference FString::operator [] (const IndexT pos) const
{
  if ( isNegative(pos) || pos > IndexT(string.length()) )
    throw std::out_of_range("");  // Invalid index position

  if ( std::size_t(pos) == string.length() )
    return const_null_char;

  return string[std::size_t(pos)];
}

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_char_ptr_t<CharT>>
inline bool FString::operator < (const CharT& s) const
{
  char_string = internal_toCharString(string);
  return s ? char_string.compare(s) < 0 : char_string.compare("") < 0;
}

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_char_array_t<CharT>>
inline bool FString::operator < (const CharT& s) const
{
  char_string = internal_toCharString(string);
  return char_string.compare(s) < 0;
}

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_wchar_ptr_t<CharT>>
inline bool FString::operator < (const CharT& s) const
{
  return s ? string.compare(s) < 0 : string.compare(L"") < 0;
}

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_wchar_array_t<CharT>>
inline bool FString::operator < (const CharT& s) const
{
  return string.compare(s) < 0;
}

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_char_ptr_t<CharT>>
inline bool FString::operator <= (const CharT& s) const
{
  char_string = internal_toCharString(string);
  return s ? char_string.compare(s) <= 0 : char_string.compare("") <= 0;
}

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_char_array_t<CharT>>
inline bool FString::operator <= (const CharT& s) const
{
  char_string = internal_toCharString(string);
  return char_string.compare(s) <= 0;
}

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_wchar_ptr_t<CharT>>
inline bool FString::operator <= (const CharT& s) const
{
  return s ? string.compare(s) <= 0 : string.compare(L"") <= 0;
}

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_wchar_array_t<CharT>>
inline bool FString::operator <= (const CharT& s) const
{
  return string.compare(s) <= 0;
}

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_char_ptr_t<CharT>>
inline bool FString::operator == (const CharT& s) const
{
  char_string = internal_toCharString(string);
  return s ? char_string.compare(s) == 0 : char_string.compare("") == 0;
}

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_char_array_t<CharT>>
inline bool FString::operator == (const CharT& s) const
{
  char_string = internal_toCharString(string);
  return char_string.compare(s) == 0;
}

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_wchar_ptr_t<CharT>>
inline bool FString::operator == (const CharT& s) const
{
  return s ? string.compare(s) == 0 : string.compare(L"") == 0;
}

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_wchar_array_t<CharT>>
inline bool FString::operator == (const CharT& s) const
{
  return string.compare(s) == 0;
}

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_char_ptr_t<CharT>>
inline bool FString::operator != (const CharT& s) const
{
  char_string = internal_toCharString(string);
  return s ? char_string.compare(s) != 0 : char_string.compare("") != 0;
}

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_char_array_t<CharT>>
inline bool FString::operator != (const CharT& s) const
{
  char_string = internal_toCharString(string);
  return char_string.compare(s) != 0;
}

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_wchar_ptr_t<CharT>>
inline bool FString::operator != (const CharT& s) const
{
  return s ? string.compare(s) != 0 : string.compare(L"") != 0;
}

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_wchar_array_t<CharT>>
inline bool FString::operator != (const CharT& s) const
{
  return string.compare(s) != 0;
}

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_char_ptr_t<CharT>>
inline bool FString::operator >= (const CharT& s) const
{
  char_string = internal_toCharString(string);
  return s ? char_string.compare(s) >= 0 : char_string.compare("") >= 0;
}

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_char_array_t<CharT>>
inline bool FString::operator >= (const CharT& s) const
{
  char_string = internal_toCharString(string);
  return char_string.compare(s) >= 0;
}

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_wchar_ptr_t<CharT>>
inline bool FString::operator >= (const CharT& s) const
{
  return s ? string.compare(s) >= 0 : string.compare(L"") >= 0;
}

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_wchar_array_t<CharT>>
inline bool FString::operator >= (const CharT& s) const
{
  return string.compare(s) >= 0;
}

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_char_ptr_t<CharT>>
inline bool FString::operator > (const CharT& s) const
{
  char_string = internal_toCharString(string);
  return s ? char_string.compare(s) > 0 : char_string.compare("") > 0;
}

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_char_array_t<CharT>>
inline bool FString::operator > (const CharT& s) const
{
  char_string = internal_toCharString(string);
  return char_string.compare(s) > 0;
}

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_wchar_ptr_t<CharT>>
inline bool FString::operator > (const CharT& s) const
{
  return s ? string.compare(s) > 0 : string.compare(L"") > 0;
}

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_wchar_array_t<CharT>>
inline bool FString::operator > (const CharT& s) const
{
  return string.compare(s) > 0;
}

//----------------------------------------------------------------------
inline FString FString::getClassName() const
{ return "FString"; }

//----------------------------------------------------------------------
inline bool FString::isNull() const noexcept  // deprecated
{ return false; }

//----------------------------------------------------------------------
inline bool FString::isEmpty() const noexcept
{ return string.empty(); }

//----------------------------------------------------------------------
inline std::size_t FString::getLength() const noexcept
{ return string.length(); }

//----------------------------------------------------------------------
inline std::size_t FString::capacity() const noexcept
{ return string.capacity(); }

//----------------------------------------------------------------------
inline FString::iterator FString::begin() noexcept
{ return string.begin(); }

//----------------------------------------------------------------------
inline FString::iterator FString::end() noexcept
{ return string.end(); }

//----------------------------------------------------------------------
inline FString::const_iterator FString::begin() const
{ return this->string.begin(); }

//----------------------------------------------------------------------
inline FString::const_iterator FString::end() const
{ return this->string.end(); }

//----------------------------------------------------------------------
inline FString::const_iterator FString::cbegin() const noexcept
{ return this->string.cbegin(); }

//----------------------------------------------------------------------
inline FString::const_iterator FString::cend() const noexcept
{ return this->string.cend(); }

//----------------------------------------------------------------------
inline FString::reference FString::front()
{
  assert ( ! isEmpty() );
  return string.front();
}

//----------------------------------------------------------------------
inline FString::reference FString::back()
{
  assert( ! isEmpty() );
  return string.back();
}

//----------------------------------------------------------------------
inline FString::const_reference FString::front() const
{
  assert ( ! isEmpty() );
  return string.front();
}

//----------------------------------------------------------------------
inline FString::const_reference FString::back() const
{
  assert( ! isEmpty() );
  return string.back();
}

//----------------------------------------------------------------------
template <typename... Args>
inline FString& FString::sprintf (const FString& format, Args&&... args)
{
  std::array<wchar_t, 4096> buf{};

  if ( format.isEmpty() )
  {
    clear();
    return *this;
  }

  std::swprintf ( buf.data(), buf.size(), format.wc_str()
                , std::forward<Args>(args)... );
  setString(buf.data());
  return *this;
}

//----------------------------------------------------------------------
template <typename NumT>
inline FString& FString::setNumber (NumT num, int precision)
{
  if ( std::is_floating_point<NumT>::value )
    return setNumber (lDouble(num), precision);

  if ( isNegative(num) )
    return setNumber (sInt64(num));

  return setNumber (uInt64(num));
}

//----------------------------------------------------------------------
template <typename NumT>
inline FString& FString::setFormatedNumber (NumT num, char separator)
{
  if ( isNegative(num) )
    return setFormatedNumber (sInt64(num), separator);

  return setFormatedNumber (uInt64(num), separator);
}


}  // namespace finalcut

//----------------------------------------------------------------------
template <>
struct std::hash<finalcut::FString>
{
  std::size_t operator () (const finalcut::FString& p) const noexcept
  {
    return std::hash<std::wstring>{}(p.string);
  }
};

#endif  // FSTRING_H
