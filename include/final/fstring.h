// File: fstring.h
// Provides: class FString
//
//  Standalone class
//  ════════════════
//
// ▕▔▔▔▔▔▔▔▔▔▏
// ▕ FString ▏
// ▕▁▁▁▁▁▁▁▁▁▏

#ifndef FSTRING_H
#define FSTRING_H

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

#include <iostream>
#include <new>
#include <stdexcept>
#include <string>
#include <vector>

#include "final/ftypes.h"

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
    typedef const wchar_t* iterator;

    // Constructors
    FString ();
    explicit FString (int);
    explicit FString (uInt);
    FString (int, wchar_t);
    FString (uInt, wchar_t);
    FString (int, char);
    FString (uInt, char);
    FString (const FString&);       // implicit conversion copy constructor
    FString (const std::wstring&);  // implicit conversion constructor
    FString (const wchar_t*);       // implicit conversion constructor
    FString (const std::string&);   // implicit conversion constructor
    FString (const char*);          // implicit conversion constructor
    FString (const wchar_t);        // implicit conversion constructor
    FString (const char);           // implicit conversion constructor

    // Destructor
    virtual ~FString ();

    // Overloaded operators
    FString& operator = (const FString&);
    FString& operator = (const std::wstring&);
    const FString& operator = (const wchar_t*);
    FString& operator = (const std::string&);
    const FString& operator = (const char*);
    const FString& operator = (const wchar_t);
    const FString& operator = (const char);

    const FString& operator += (const FString&);
    const FString& operator += (const std::wstring&);
    const FString& operator += (const wchar_t*);
    const FString& operator += (const std::string&);
    const FString& operator += (const char*);
    const FString& operator += (const wchar_t);
    const FString& operator += (const char);

    const FString operator + (const FString&);
    const FString operator + (const std::wstring&);
    const FString operator + (const wchar_t*);
    const FString operator + (const std::string&);
    const FString operator + (const char*);
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

    wchar_t& operator [] (int);
    wchar_t& operator [] (uInt);
    const FString& operator () ();

    bool operator <  (const FString&) const;
    bool operator <  (const std::wstring&) const;
    bool operator <  (const wchar_t*) const;
    bool operator <  (const std::string&) const;
    bool operator <  (const char*) const;
    bool operator <  (const wchar_t) const;
    bool operator <  (const char) const;
    bool operator <= (const FString&) const;
    bool operator <= (const std::wstring&) const;
    bool operator <= (const wchar_t*) const;
    bool operator <= (const std::string&) const;
    bool operator <= (const char*) const;
    bool operator <= (const wchar_t) const;
    bool operator <= (const char) const;
    bool operator == (const FString&) const;
    bool operator == (const std::wstring&) const;
    bool operator == (const wchar_t*) const;
    bool operator == (const std::string&) const;
    bool operator == (const char*) const;
    bool operator == (const wchar_t) const;
    bool operator == (const char) const;
    bool operator != (const FString&) const;
    bool operator != (const std::wstring&) const;
    bool operator != (const wchar_t*) const;
    bool operator != (const std::string&) const;
    bool operator != (const char*) const;
    bool operator != (const wchar_t) const;
    bool operator != (const char) const;
    bool operator >= (const FString&) const;
    bool operator >= (const std::wstring&) const;
    bool operator >= (const wchar_t*) const;
    bool operator >= (const std::string&) const;
    bool operator >= (const char*) const;
    bool operator >= (const wchar_t) const;
    bool operator >= (const char) const;
    bool operator >  (const FString&) const;
    bool operator >  (const std::wstring&) const;
    bool operator >  (const wchar_t*) const;
    bool operator >  (const std::string&) const;
    bool operator >  (const char*) const;
    bool operator >  (const wchar_t) const;
    bool operator >  (const char) const;

    operator const char* () const { return c_str(); }

    // Non-member operators
    friend const FString operator + (const FString&, const FString&);
    friend const FString operator + (const FString&, const wchar_t);
    friend const FString operator + (const std::wstring&, const FString&);
    friend const FString operator + (const wchar_t*, const FString&);
    friend const FString operator + (const std::string&, const FString&);
    friend const FString operator + (const char*, const FString&);
    friend const FString operator + (const wchar_t, const FString&);
    friend const FString operator + (const char, const FString&);
    friend const FString operator + (const wchar_t, const std::wstring&);
    friend const FString operator + (const FString&, const char);

    friend std::ostream&  operator << (std::ostream& outstr, const FString& s);
    friend std::istream&  operator >> (std::istream& instr, FString& s);
    friend std::wostream& operator << (std::wostream& outstr, const FString& s);
    friend std::wistream& operator >> (std::wistream& instr, FString& s);

    // inquiries
    bool isNull() const;
    bool isEmpty() const;

    // Methods
    uInt getLength() const;
    uInt getUTF8length() const;

    iterator begin() const;
    iterator end()   const;
    wchar_t  front() const;
    wchar_t  back()  const;

    FString& sprintf (const wchar_t*, ...);
    FString& sprintf (const char*, ...)
#if defined(__clang__)
      __attribute__((__format__ (__printf__, 2, 3)))
#elif defined(__GNUC__)
      __attribute__ ((format (printf, 2, 3)))
#endif
             ;
    FString clear();

    const wchar_t* wc_str() const;
    const char* c_str() const;
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

    FString left (int) const;
    FString left (uInt) const;
    FString right (int) const;
    FString right (uInt) const;
    FString mid (int, int) const;
    FString mid (uInt, uInt) const;

    FStringList split (const FString&);
    FStringList split (const std::wstring&);
    FStringList split (const wchar_t*);
    FStringList split (const std::string&);
    FStringList split (const char*);
    FStringList split (const wchar_t);
    FStringList split (const char);

    FString& setString (const wchar_t*);
    FString& setString (const char*);

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

    const FString& insert (const FString&, uInt);
    const FString& insert (const wchar_t*, uInt);
    const FString& insert (const char*, uInt);
    const FString& insert (const wchar_t, uInt);
    const FString& insert (const char, uInt);

    FString replace (const FString&, const FString&);
    FString replace (const FString&, const std::wstring&);
    FString replace (const FString&, const wchar_t*);
    FString replace (const FString&, const std::string&);
    FString replace (const FString&, const char*);
    FString replace (const FString&, const wchar_t);
    FString replace (const FString&, const char);
    FString replace (const std::wstring&, const FString&);
    FString replace (const std::wstring&, const std::wstring&);
    FString replace (const std::wstring&, const wchar_t*);
    FString replace (const std::wstring&, const std::string&);
    FString replace (const std::wstring&, const char*);
    FString replace (const std::wstring&, const wchar_t);
    FString replace (const std::wstring&, const char);
    FString replace (const std::string&, const FString&);
    FString replace (const std::string&, const std::wstring&);
    FString replace (const std::string&, const wchar_t*);
    FString replace (const std::string&, const std::string&);
    FString replace (const std::string&, const char*);
    FString replace (const std::string&, const wchar_t);
    FString replace (const std::string&, const char);
    FString replace (const wchar_t*, const FString&);
    FString replace (const wchar_t*, const std::wstring&);
    FString replace (const wchar_t*, const wchar_t*);
    FString replace (const wchar_t*, const std::string&);
    FString replace (const wchar_t*, const char*);
    FString replace (const wchar_t*, const wchar_t);
    FString replace (const wchar_t*, const char);
    FString replace (const char*, const FString&);
    FString replace (const char*, const std::wstring&);
    FString replace (const char*, const wchar_t*);
    FString replace (const char*, const std::string&);
    FString replace (const char*, const char*);
    FString replace (const char*, const wchar_t);
    FString replace (const char*, const char);
    FString replace (const wchar_t, const FString&);
    FString replace (const wchar_t, const std::wstring&);
    FString replace (const wchar_t, const wchar_t*);
    FString replace (const wchar_t, const std::string&);
    FString replace (const wchar_t, const char*);
    FString replace (const wchar_t, const wchar_t);
    FString replace (const wchar_t, const char);
    FString replace (const char, const FString&);
    FString replace (const char, const std::wstring&);
    FString replace (const char, const wchar_t*);
    FString replace (const char, const std::string&);
    FString replace (const char, const char*);
    FString replace (const char, const wchar_t);
    FString replace (const char, const char);

    FString replaceControlCodes() const;
    FString expandTabs (int = 8) const;
    FString removeDel() const;
    FString removeBackspaces() const;

    const FString& overwrite (const FString&, uInt);
    const FString& overwrite (const wchar_t*, uInt);
    const FString& overwrite (const wchar_t, uInt);

    const FString& remove (uInt, uInt);
    bool  includes (const FString&);
    bool  includes (const wchar_t*);
    bool  includes (const char*);
    bool  includes (const wchar_t);
    bool  includes (const char);

  private:
    // Constants
    static const uInt  FWDBUFFER = 15;
    static const uInt  INPBUFFER = 200;
    static const uInt  CHAR_SIZE = sizeof(wchar_t);  // bytes per character
    static const char* const bad_alloc_str;

    // Methods
    void     initLength (uInt);
    void     _assign (const wchar_t*);
    void     _insert (uInt, uInt, const wchar_t*);
    void     _remove (uInt, uInt);
    char*    wc_to_c_str (const wchar_t*) const;
    wchar_t* c_to_wc_str (const char*) const;
    wchar_t* extractToken (wchar_t**, const wchar_t*, const wchar_t*);

    // Data Members
    wchar_t*      string;
    uInt          length;
    uInt          bufsize;
    mutable char* c_string;
};


// FString inline functions
//----------------------------------------------------------------------
inline bool FString::isNull() const
{ return ! string; }

//----------------------------------------------------------------------
inline bool FString::isEmpty() const
{ return ( ! string ) || ( ! *string ); }

//----------------------------------------------------------------------
inline uInt FString::getLength() const
{ return length; }

//----------------------------------------------------------------------
inline FString::iterator FString::begin() const
{ return string; }

//----------------------------------------------------------------------
inline FString::iterator FString::end() const
{ return string + length; }

//----------------------------------------------------------------------
inline wchar_t FString::front() const
{
  assert( ! isEmpty() );
  return string[0];
}

//----------------------------------------------------------------------
inline wchar_t FString::back() const
{
  assert( ! isEmpty() );
  return string[length - 1];
}

//----------------------------------------------------------------------
inline FStringList FString::split (const std::wstring& s)
{ return split(FString(s)); }

//----------------------------------------------------------------------
inline FStringList FString::split (const wchar_t* s)
{ return split(FString(s)); }

//----------------------------------------------------------------------
inline FStringList FString::split (const std::string& s)
{ return split(FString(s)); }

//----------------------------------------------------------------------
inline FStringList FString::split (const char* s)
{ return split(FString(s)); }

//----------------------------------------------------------------------
inline FStringList FString::split (const wchar_t c)
{ return split(FString(c)); }

//----------------------------------------------------------------------
inline FStringList FString::split (const char c)
{ return split(FString(c)); }

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


#endif  // FSTRING_H
