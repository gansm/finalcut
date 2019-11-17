/***********************************************************************
* fstring.h - Unicode string class with UTF-8 support                  *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2012-2019 Markus Gans                                      *
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
#include <stdint.h>

#include <cassert>
#include <cerrno>   // for read errno
#include <cfloat>
#include <climits>
#include <cmath>
#include <cstdarg>  // need for va_list, va_start and va_end
#include <cstdio>   // need for vsprintf
#include <cstring>
#include <cwchar>
#include <cwctype>

#include <limits>
#include <iostream>
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

// Global typedef
typedef std::vector<FString> FStringList;


//----------------------------------------------------------------------
// class FString
//----------------------------------------------------------------------

class FString
{
  public:
    // Typedef
    typedef const wchar_t* const_iterator;
    typedef wchar_t* iterator;

    // Constructors
    FString () = default;
    explicit FString (int);
    explicit FString (std::size_t);
    FString (std::size_t, wchar_t);
    FString (const FString&);        // implicit conversion copy constructor
    FString (FString&&);             // implicit conversion move constructor
    FString (const std::wstring&);   // implicit conversion constructor
    FString (const wchar_t[]);       // implicit conversion constructor
    FString (const std::string&);    // implicit conversion constructor
    FString (const char[]);          // implicit conversion constructor
    FString (fc::SpecialCharacter);  // implicit conversion constructor
    FString (const wchar_t);         // implicit conversion constructor
    FString (const char);            // implicit conversion constructor

    // Destructor
    virtual ~FString ();

    // Overloaded operators
    FString& operator = (const FString&);
    FString& operator = (FString&&);

    const FString& operator += (const FString&);

    const FString operator + (const FString&);
    const FString operator + (const wchar_t);
    const FString operator + (const char);

    FString& operator << (const FString&);
    FString& operator << (fc::SpecialCharacter);
    FString& operator << (const wchar_t);
    FString& operator << (const char);
    template <typename NumT
            , typename std::enable_if< std::is_integral<NumT>::value
                                    || std::is_floating_point<NumT>::value
                                     , NumT>::type* = nullptr >
    FString& operator << (const NumT);

    const FString& operator >> (FString&);
    const FString& operator >> (std::wstring&);
    const FString& operator >> (std::string&);
    const FString& operator >> (wchar_t&);
    const FString& operator >> (char&);
    const FString& operator >> (sInt16&);
    const FString& operator >> (uInt16&);
    const FString& operator >> (sInt32&);
    const FString& operator >> (uInt32&);
    const FString& operator >> (sInt64&);
    const FString& operator >> (uInt64&);
    const FString& operator >> (double&);
    const FString& operator >> (float&);

    template <typename IndexT>
    wchar_t&       operator [] (const IndexT);
    template <typename IndexT>
    const wchar_t& operator [] (const IndexT) const;
    const FString& operator () ();

    bool operator <  (const FString&) const;
    template <typename CharT>
    bool operator <  (const CharT&) const;
    bool operator <= (const FString&) const;
    template <typename CharT>
    bool operator <= (const CharT&) const;
    bool operator == (const FString&) const;
    template <typename CharT>
    bool operator == (const CharT&) const;
    bool operator != (const FString&) const;
    template <typename CharT>
    bool operator != (const CharT&) const;
    bool operator >= (const FString&) const;
    template <typename CharT>
    bool operator >= (const CharT&) const;
    bool operator >  (const FString&) const;
    template <typename CharT>
    bool operator >  (const CharT&) const;

    operator const char* () const { return c_str(); }

    // Non-member operators
    friend const FString operator + (const FString&, const FString&);
    friend const FString operator + (const FString&, const wchar_t);
    friend const FString operator + (const std::wstring&, const FString&);
    friend const FString operator + (const wchar_t[], const FString&);
    friend const FString operator + (const std::string&, const FString&);
    friend const FString operator + (const char[], const FString&);
    friend const FString operator + (const wchar_t, const FString&);
    friend const FString operator + (const char, const FString&);
    friend const FString operator + (const FString&, const char);

    friend std::ostream&  operator << (std::ostream&, const FString&);
    friend std::istream&  operator >> (std::istream&, FString& s);
    friend std::wostream& operator << (std::wostream&, const FString&);
    friend std::wistream& operator >> (std::wistream&, FString&);

    // Accessor
    virtual const FString getClassName();

    // inquiries
    bool isNull() const;
    bool isEmpty() const;

    // Methods
    std::size_t getLength() const;
    std::size_t getUTF8length() const;
    std::size_t capacity() const;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    wchar_t  front() const;
    wchar_t  back() const;

    template<typename... Args>
    FString& sprintf (const FString&, Args&&...);
    FString  clear();

    const wchar_t* wc_str() const;
    wchar_t* wc_str();
    const char* c_str() const;
    char* c_str();
    const std::string toString() const;

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

    FStringList split (const FString&);
    FString& setString (const FString&);

    template <typename NumT>
    FString& setNumber (NumT, int = getPrecision<NumT>());
    FString& setNumber (sInt64);
    FString& setNumber (uInt64);
    FString& setNumber (lDouble, int = getPrecision<lDouble>());

    template <typename NumT>
    FString& setFormatedNumber (NumT, char = nl_langinfo(THOUSEP)[0]);
    FString& setFormatedNumber (sInt64, char = nl_langinfo(THOUSEP)[0]);
    FString& setFormatedNumber (uInt64, char = nl_langinfo(THOUSEP)[0]);

    const FString& insert (const FString&, int);
    const FString& insert (const FString&, std::size_t);

    FString replace (const FString&, const FString&);

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
    static constexpr uInt FWDBUFFER = 15;
    static constexpr uInt INPBUFFER = 200;
    static constexpr uInt CHAR_SIZE = sizeof(wchar_t);  // bytes per character

    // Methods
    void     initLength (std::size_t);
    void     _assign (const wchar_t[]);
    void     _insert (std::size_t, const wchar_t[]);
    void     _insert (std::size_t, std::size_t, const wchar_t[]);
    void     _remove (std::size_t, std::size_t);
    char*    wc_to_c_str (const wchar_t[]) const;
    wchar_t* c_to_wc_str (const char[]) const;
    wchar_t* extractToken (wchar_t*[], const wchar_t[], const wchar_t[]);

    // Data members
    wchar_t*      string{nullptr};
    std::size_t   length{0};
    std::size_t   bufsize{0};
    mutable char* c_string{nullptr};
    static wchar_t null_char;
    static const wchar_t const_null_char;
};


// FString inline functions
//----------------------------------------------------------------------
template <typename NumT
        , typename std::enable_if< std::is_integral<NumT>::value
                                || std::is_floating_point<NumT>::value
                                 , NumT>::type* = nullptr >
inline FString& FString::operator << (const NumT val)
{
  FString numstr(FString().setNumber(val));
  _insert (length, numstr.length, numstr.string);
  return *this;
}

//----------------------------------------------------------------------
template <typename IndexT>
inline wchar_t& FString::operator [] (const IndexT pos)
{
  if ( isNegative(pos) || pos > IndexT(length) )
    throw std::out_of_range("");  // Invalid index position

  if ( std::size_t(pos) == length )
    return null_char;

  return string[std::size_t(pos)];
}

//----------------------------------------------------------------------
template <typename IndexT>
inline const wchar_t& FString::operator [] (const IndexT pos) const
{
  if ( isNegative(pos) || pos > IndexT(length) )
    throw std::out_of_range("");  // Invalid index position

  if ( std::size_t(pos) == length )
    return const_null_char;

  return string[std::size_t(pos)];
}

//----------------------------------------------------------------------
template <typename CharT>
inline bool FString::operator < (const CharT& s) const
{
  const FString tmp(s);
  return *this < tmp;
}

//----------------------------------------------------------------------
template <typename CharT>
inline bool FString::operator <= (const CharT& s) const
{
  const FString tmp(s);
  return *this <= tmp;
}

//----------------------------------------------------------------------
template <typename CharT>
inline bool FString::operator == (const CharT& s) const
{
  const FString tmp(s);
  return *this == tmp;
}

//----------------------------------------------------------------------
template <typename CharT>
inline bool FString::operator != (const CharT& s) const
{
  const FString tmp(s);
  return *this != tmp;
}

//----------------------------------------------------------------------
template <typename CharT>
inline bool FString::operator >= (const CharT& s) const
{
  const FString tmp(s);
  return *this >= tmp;
}

//----------------------------------------------------------------------
template <typename CharT>
inline bool FString::operator > (const CharT& s) const
{
  const FString tmp(s);
  return *this > tmp;
}

//----------------------------------------------------------------------
inline const FString FString::getClassName()
{ return "FString"; }

//----------------------------------------------------------------------
inline bool FString::isNull() const
{ return ! string; }

//----------------------------------------------------------------------
inline bool FString::isEmpty() const
{ return ( ! string ) || ( ! *string ); }

//----------------------------------------------------------------------
inline std::size_t FString::getLength() const
{ return length; }

//----------------------------------------------------------------------
inline std::size_t FString::capacity() const
{ return ( length > 0 ) ? bufsize - 1 : 0; }

//----------------------------------------------------------------------
inline FString::iterator FString::begin()
{ return string; }

//----------------------------------------------------------------------
inline FString::iterator FString::end()
{ return string + length; }

//----------------------------------------------------------------------
inline FString::const_iterator FString::begin() const
{ return string; }

//----------------------------------------------------------------------
inline FString::const_iterator FString::end() const
{ return string + length; }

//----------------------------------------------------------------------
inline wchar_t FString::front() const
{
  assert ( ! isEmpty() );
  return string[0];
}

//----------------------------------------------------------------------
inline wchar_t FString::back() const
{
  assert( ! isEmpty() );
  return string[length - 1];
}

//----------------------------------------------------------------------
template<typename... Args>
inline FString& FString::sprintf (const FString& format, Args&&... args)
{
  static constexpr int BUFSIZE = 4096;
  wchar_t buf[BUFSIZE]{};

  if ( format.isEmpty() )
  {
    clear();
    return *this;
  }

  std::swprintf ( buf, BUFSIZE, format.wc_str()
                , std::forward<Args>(args)... );
  _assign(buf);
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

#endif  // FSTRING_H
