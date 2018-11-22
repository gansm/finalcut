/***********************************************************************
* fstring.h - Unicode string class with UTF-8 support                  *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2012-2018 Markus Gans                                      *
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
#include <vector>

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

#pragma pack(push)
#pragma pack(1)

class FString
{
  public:
    // Typedef
    typedef const wchar_t* iterator;

    // Constructors
    FString ();
    explicit FString (int);
    explicit FString (std::size_t);
    FString (std::size_t, wchar_t);
    FString (std::size_t, char);
    FString (const FString&);       // implicit conversion copy constructor
    FString (const std::wstring&);  // implicit conversion constructor
    FString (const wchar_t[]);      // implicit conversion constructor
    FString (const std::string&);   // implicit conversion constructor
    FString (const char[]);         // implicit conversion constructor
    FString (const wchar_t);        // implicit conversion constructor
    FString (const char);           // implicit conversion constructor

    // Destructor
    virtual ~FString ();

    // Overloaded operators
    FString& operator = (const FString&);

    const FString& operator += (const FString&);

    const FString operator + (const FString&);
    const FString operator + (const wchar_t);
    const FString operator + (const char);

    FString& operator << (const FString&);
    FString& operator << (const wchar_t);
    FString& operator << (const char);
    FString& operator << (const sInt16);
    FString& operator << (const uInt16);
    FString& operator << (const int);
    FString& operator << (const uInt);
    FString& operator << (const long);
    FString& operator << (const uLong);
    FString& operator << (const float);
    FString& operator << (const double);
    FString& operator << (const lDouble);

    const FString& operator >> (FString&);
    const FString& operator >> (std::wstring&);
    const FString& operator >> (std::string&);
    const FString& operator >> (wchar_t&);
    const FString& operator >> (char&);
    const FString& operator >> (sInt16&);
    const FString& operator >> (uInt16&);
    const FString& operator >> (int&);
    const FString& operator >> (uInt&);
    const FString& operator >> (long&);
    const FString& operator >> (uLong&);
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
    virtual const char* getClassName();

    // inquiries
    bool isNull() const;
    bool isEmpty() const;

    // Methods
    std::size_t getLength() const;
    std::size_t getUTF8length() const;
    std::size_t capacity() const;

    iterator begin() const;
    iterator end()   const;
    wchar_t  front() const;
    wchar_t  back()  const;

    FString& sprintf (const FString, ...);
    FString clear();

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

    FString& setNumber (sInt16);
    FString& setNumber (uInt16);
    FString& setNumber (int);
    FString& setNumber (uInt);
    FString& setNumber (long);
    FString& setNumber (uLong);
    FString& setNumber (float, int = FLT_DIG);
    FString& setNumber (double, int = DBL_DIG);
    FString& setNumber (lDouble, int = LDBL_DIG);

    FString& setFormatedNumber (sInt16, char = nl_langinfo(THOUSEP)[0]);
    FString& setFormatedNumber (uInt16, char = nl_langinfo(THOUSEP)[0]);
    FString& setFormatedNumber (int,    char = nl_langinfo(THOUSEP)[0]);
    FString& setFormatedNumber (uInt,   char = nl_langinfo(THOUSEP)[0]);
    FString& setFormatedNumber (long,   char = nl_langinfo(THOUSEP)[0]);
    FString& setFormatedNumber (uLong,  char = nl_langinfo(THOUSEP)[0]);

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
    static const uInt  FWDBUFFER = 15;
    static const uInt  INPBUFFER = 200;
    static const uInt  CHAR_SIZE = sizeof(wchar_t);  // bytes per character

    // Methods
    void     initLength (std::size_t);
    void     _assign (const wchar_t[]);
    void     _insert (std::size_t, const wchar_t[]);
    void     _insert (std::size_t, std::size_t, const wchar_t[]);
    void     _remove (std::size_t, std::size_t);
    char*    wc_to_c_str (const wchar_t[]) const;
    wchar_t* c_to_wc_str (const char[]) const;
    wchar_t* extractToken (wchar_t*[], const wchar_t[], const wchar_t[]);

    // Data Members
    wchar_t*      string;
    std::size_t   length;
    std::size_t   bufsize;
    mutable char* c_string;
};
#pragma pack(pop)

// FString inline functions
//----------------------------------------------------------------------
inline const char* FString::getClassName()
{ return "FString"; }

//----------------------------------------------------------------------
template <typename IndexT>
inline wchar_t& FString::operator [] (const IndexT pos)
{
  if ( isNegative(pos) || pos >= IndexT(length) )
    throw std::out_of_range("");  // Invalid index position

  return string[std::size_t(pos)];
}

//----------------------------------------------------------------------
template <typename IndexT>
inline const wchar_t& FString::operator [] (const IndexT pos) const
{
  if ( isNegative(pos) || pos >= IndexT(length) )
    throw std::out_of_range("");  // Invalid index position

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
inline FString::iterator FString::begin() const
{ return string; }

//----------------------------------------------------------------------
inline FString::iterator FString::end() const
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
inline FString& FString::setNumber (sInt16 num)
{ return setNumber (long(num)); }

//----------------------------------------------------------------------
inline FString& FString::setNumber (uInt16 num)
{ return setNumber (uLong(num)); }

//----------------------------------------------------------------------
inline FString& FString::setNumber (int num)
{ return setNumber (long(num)); }

//----------------------------------------------------------------------
inline FString& FString::setNumber (uInt num)
{ return setNumber (uLong(num)); }

//----------------------------------------------------------------------
inline FString& FString::setNumber (float num, int precision)
{ return setNumber (lDouble(num), precision); }

//----------------------------------------------------------------------
inline FString& FString::setNumber (double num, int precision)
{ return setNumber (lDouble(num), precision); }

//----------------------------------------------------------------------
inline FString& FString::setFormatedNumber (sInt16 num, char separator)
{ return setFormatedNumber (long(num), separator); }

//----------------------------------------------------------------------
inline FString& FString::setFormatedNumber (uInt16 num, char separator)
{ return setFormatedNumber (uLong(num), separator); }

//----------------------------------------------------------------------
inline FString& FString::setFormatedNumber (int num, char separator)
{ return setFormatedNumber (long(num), separator); }

//----------------------------------------------------------------------
inline FString& FString::setFormatedNumber (uInt num, char separator)
{ return setFormatedNumber (uLong(num), separator); }


}  // namespace finalcut

#endif  // FSTRING_H
