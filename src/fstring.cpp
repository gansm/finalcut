/***********************************************************************
* fstring.cpp - Unicode string class with UTF-8 support                *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2012-2020 Markus Gans                                      *
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

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "final/fapplication.h"
#include "final/flog.h"
#include "final/fstring.h"

namespace finalcut
{

// static class attributes
wchar_t       FString::null_char{L'\0'};
const wchar_t FString::const_null_char{L'\0'};

//----------------------------------------------------------------------
// class FString
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FString::FString (int len)
{
  if ( len > 0 )
    _initLength(std::size_t(len));
  else
    _initLength(0);
}

//----------------------------------------------------------------------
FString::FString (std::size_t len)
{
  _initLength(len);
}

//----------------------------------------------------------------------
FString::FString (std::size_t len, wchar_t c)
{
  _initLength(len);
  const wchar_t* ps = string;
  wchar_t* pe = string + len;

  while ( pe != ps )
    *--pe = c;
}

//----------------------------------------------------------------------
FString::FString (const FString& s)  // copy constructor
{
  if ( ! s.isNull() )
    _assign (s.string);
}

//----------------------------------------------------------------------
FString::FString (FString&& s) noexcept  // move constructor
  : string{std::move(s.string)}
  , length{s.length}
  , bufsize{s.bufsize}
  , c_string{std::move(s.c_string)}
{
  s.string = nullptr;
  s.length = 0;
  s.bufsize = 0;
  s.c_string = nullptr;
}

//----------------------------------------------------------------------
FString::FString (const std::wstring& s)
{
  if ( ! s.empty() )
    _assign (s.c_str());
}

//----------------------------------------------------------------------
FString::FString (const wchar_t s[])
{
  if ( s )
    _assign (s);
}

//----------------------------------------------------------------------
FString::FString (const std::string& s)
{
  if ( ! s.empty() )
  {
    const wchar_t* wc_string = _to_wcstring(s.c_str());
    _assign(wc_string);
    delete[] wc_string;
  }
}

//----------------------------------------------------------------------
FString::FString (const char s[])
{
  if ( s )
  {
    const wchar_t* wc_string = _to_wcstring(s);
    _assign( wc_string );
    delete[] wc_string;
  }
}

//----------------------------------------------------------------------
FString::FString (fc::SpecialCharacter c)
{
  if ( c )
  {
    wchar_t s[2];
    s[0] = static_cast<wchar_t>(c);
    s[1] = L'\0';
    _assign (s);
  }
}

//----------------------------------------------------------------------
FString::FString (const wchar_t c)
{
  if ( c )
  {
    wchar_t s[2];
    s[0] = c;
    s[1] = L'\0';
    _assign (s);
  }
}

//----------------------------------------------------------------------
FString::FString (const char c)
{
  if ( c )
  {
    wchar_t s[2];
    s[0] = wchar_t(c & 0xff);
    s[1] = L'\0';
    _assign (s);
  }
}

//----------------------------------------------------------------------
FString::~FString()  // destructor
{
  if ( string )
    delete[](string);

  if ( c_string )
    delete[](c_string);
}


// FString operators
//----------------------------------------------------------------------
FString& FString::operator = (const FString& s)
{
  if ( &s != this )
    _assign (s.string);

  return *this;
}

//----------------------------------------------------------------------
FString& FString::operator = (FString&& s) noexcept
{
  if ( &s != this )
  {
    if ( string )
      delete[](string);

    if ( c_string )
      delete[](c_string);

    string = std::move(s.string);
    length = s.length;
    bufsize = s.bufsize;
    c_string = std::move(s.c_string);

    s.string = nullptr;
    s.length = 0;
    s.bufsize = 0;
    s.c_string = nullptr;
  }

  return *this;
}

//----------------------------------------------------------------------
const FString& FString::operator += (const FString& s)
{
  _insert (length, s.length, s.string);
  return *this;
}


//----------------------------------------------------------------------
const FString FString::operator + (const FString& s)
{
  FString tmp{string};
  tmp._insert (length, s.length, s.string);
  return tmp;
}

//----------------------------------------------------------------------
const FString FString::operator + (const wchar_t c)
{
  wchar_t s[2]{};
  s[0] = c;
  FString tmp{string};
  tmp._insert (length, 1, s);
  return tmp;
}

//----------------------------------------------------------------------
const FString FString::operator + (const char c)
{
  wchar_t s[2]{};
  s[0] = wchar_t(c & 0xff);
  FString tmp{string};
  tmp._insert (length, 1, s);
  return tmp;
}

//----------------------------------------------------------------------
FString& FString::operator << (const FString& s)
{
  _insert (length, s.length, s.string);
  return *this;
}

//----------------------------------------------------------------------
FString& FString::operator << (fc::SpecialCharacter c)
{
  FString s{static_cast<wchar_t>(c)};
  _insert (length, s.length, s.string);
  return *this;
}

//----------------------------------------------------------------------
FString& FString::operator << (const wchar_t c)
{
  FString s{c};
  _insert (length, s.length, s.string);
  return *this;
}

//----------------------------------------------------------------------
FString& FString::operator << (const char c)
{
  FString s{c};
  _insert (length, s.length, s.string);
  return *this;
}

//----------------------------------------------------------------------
const FString& FString::operator >> (FString& s)
{
  s._insert (s.length, length, string);
  _assign(s.string);
  return *this;
}

//----------------------------------------------------------------------
const FString& FString::operator >> (std::wstring& s)
{
  s += std::wstring(string);
  return *this;
}

//----------------------------------------------------------------------
const FString& FString::operator >> (std::string& s)
{
  s += toString();
  return *this;
}

//----------------------------------------------------------------------
const FString& FString::operator >> (wchar_t& c)
{
  c = ( length > 0 ) ? string[0] : L'\0';
  return *this;
}

//----------------------------------------------------------------------
const FString& FString::operator >> (char& c)
{
  c = ( length > 0 ) ? char(string[0] & 0xff) : '\0';
  return *this;
}

//----------------------------------------------------------------------
const FString& FString::operator >> (sInt16& num)
{
  num = toShort();
  return *this;
}

//----------------------------------------------------------------------
const FString& FString::operator >> (uInt16& num)
{
  num = toUShort();
  return *this;
}

//----------------------------------------------------------------------
const FString& FString::operator >> (sInt32& num)
{
  num = toInt();
  return *this;
}

//----------------------------------------------------------------------
const FString& FString::operator >> (uInt32& num)
{
  num = toUInt();
  return *this;
}

//----------------------------------------------------------------------
const FString& FString::operator >> (sInt64& num)
{
  num = toLong();
  return *this;
}

//----------------------------------------------------------------------
const FString& FString::operator >> (uInt64& num)
{
  num = toULong();
  return *this;
}

//----------------------------------------------------------------------
const FString& FString::operator >> (double& num)
{
  num = toDouble();
  return *this;
}

//----------------------------------------------------------------------
const FString& FString::operator >> (float& num)
{
  num = toFloat();
  return *this;
}

//----------------------------------------------------------------------
const FString& FString::operator () ()
{
  return *this;
}


// public methods of FString
//----------------------------------------------------------------------
std::size_t FString::getUTF8length() const
{
  if ( ! string )
    return 0;

  std::size_t len{0};
  const char* s = c_str();

  while ( *s )
    len += std::size_t((*s++ & 0xc0) != 0x80);

  return len;
}

//----------------------------------------------------------------------
FString FString::clear()
{
  if ( string )
    delete[](string);

  length  = 0;
  bufsize = 0;
  string  = nullptr;
  return *this;
}

//----------------------------------------------------------------------
const wchar_t* FString::wc_str() const
{
  // Returns a constant wide character string

  return string;
}

//----------------------------------------------------------------------
wchar_t* FString::wc_str()
{
  // Returns a wide character string

  return string;
}

//----------------------------------------------------------------------
const char* FString::c_str() const
{
  // Returns a constant c-string

  if ( length > 0 )
    return _to_cstring(string);
  else if ( string )
    return "";
  else
    return nullptr;
}

//----------------------------------------------------------------------
char* FString::c_str()
{
  // Returns a c-string

  if ( length > 0 )
    return const_cast<char*>(_to_cstring(string));
  else if ( string )
    return const_cast<char*>("");
  else
    return nullptr;
}

//----------------------------------------------------------------------
const std::string FString::toString() const
{
  return std::string(c_str(), length);
}

//----------------------------------------------------------------------
const FString FString::toLower() const
{
  FString s{*this};
  auto to_lower = [] (wchar_t& c)
                  {
                    c = wchar_t(std::towlower(std::wint_t(c)));
                  };
  std::for_each (s.begin(), s.end(), to_lower);
  return s;
}

//----------------------------------------------------------------------
const FString FString::toUpper() const
{
  FString s{*this};
  auto to_upper = [] (wchar_t& c)
                  {
                    c = wchar_t(std::towupper(std::wint_t(c)));
                  };
  std::for_each (s.begin(), s.end(), to_upper);
  return s;
}

//----------------------------------------------------------------------
sInt16 FString::toShort() const
{
  const long num = toLong();

  if ( num > SHRT_MAX )
    throw std::overflow_error ("overflow");

  if ( num < SHRT_MIN )
    throw std::underflow_error ("underflow");

  return sInt16(num);
}

//----------------------------------------------------------------------
uInt16 FString::toUShort() const
{
  const uLong num = uLong(toULong());

  if ( num > USHRT_MAX )
    throw std::overflow_error ("overflow");

  return uInt16(num);
}

//----------------------------------------------------------------------
int FString::toInt() const
{
  long num = toLong();

  if ( num > INT_MAX )
    throw std::overflow_error ("overflow");

  if ( num < INT_MIN )
    throw std::underflow_error ("underflow");

  return int(num);
}

//----------------------------------------------------------------------
uInt FString::toUInt() const
{
  const uLong num = uLong(toULong());

  if ( num > UINT_MAX )
    throw std::overflow_error ("overflow");

  return uInt(num);
}

//----------------------------------------------------------------------
long FString::toLong() const
{
  bool neg{false};
  long num{0};
  long tenth_limit{LONG_MAX / 10};
  long tenth_limit_digit{LONG_MAX % 10};
  const FString s{trim()};
  const wchar_t* p = s.string;

  if ( ! p )
    throw std::invalid_argument ("null value");

  if ( ! *p )
    throw std::invalid_argument ("empty value");

  if ( *p == L'-' )
  {
    p++;
    neg = true;
    tenth_limit = -(LONG_MIN / 10);
    tenth_limit_digit += 1;
  }
  else if ( *p == L'+' )
  {
    p++;
  }

  while ( std::iswdigit(std::wint_t(*p)) )
  {
    uChar d = uChar(*p - L'0');

    if ( num > tenth_limit
      || (num == tenth_limit && d > tenth_limit_digit) )
    {
      if ( neg )
        throw std::underflow_error ("underflow");
      else
        throw std::overflow_error ("overflow");
    }

    num = (num << 3) + (num << 1) + d;  // (10 * num) + d
    p++;
  }

  if ( *p != L'\0' && ! std::iswdigit(std::wint_t(*p)) )
    throw std::invalid_argument ("no valid number");

  if ( neg )
    num = (~num) + 1;

  return num;
}

//----------------------------------------------------------------------
uLong FString::toULong() const
{
  uLong num{0};
  const uLong tenth_limit{ULONG_MAX / 10};
  const uLong tenth_limit_digit{ULONG_MAX % 10};
  const FString s{trim()};
  const wchar_t* p = s.string;

  if ( ! p )
    throw std::invalid_argument ("null value");

  if ( ! *p )
    throw std::invalid_argument ("empty value");

  if ( *p == L'-' )
  {
    throw std::underflow_error ("underflow");
  }
  else if ( *p == L'+' )
  {
    p++;
  }

  while ( std::iswdigit(std::wint_t(*p)) )
  {
    const uChar d = uChar(*p - L'0');

    if ( num > tenth_limit
      || (num == tenth_limit && d > tenth_limit_digit) )
    {
      throw std::overflow_error ("overflow");
    }

    num = (num << 3) + (num << 1) + d;  // (10 * num) + d
    p++;
  }

  if ( *p != L'\0' && ! std::iswdigit(std::wint_t(*p)) )
    throw std::invalid_argument ("no valid number");

  return num;
}

//----------------------------------------------------------------------
float FString::toFloat() const
{
  const double num = toDouble();

  if ( num > double(FLT_MAX) || num < double(-FLT_MAX) )
    throw std::overflow_error ("overflow");

  if ( std::fabs(num) < double(FLT_EPSILON) )  // num == 0.0f
    throw std::underflow_error ("underflow");

  return float(num);
}

//----------------------------------------------------------------------
double FString::toDouble() const
{
  if ( ! string )
    throw std::invalid_argument ("null value");

  if ( ! *string )
    throw std::invalid_argument ("empty value");

  wchar_t* p{};
  const double ret = std::wcstod(string, &p);

  if ( p != nullptr && *p != '\0' )
    throw std::invalid_argument ("no valid floating point value");

  if ( errno == ERANGE )
  {
    if ( ret >= HUGE_VAL || ret <= -HUGE_VAL )
      throw std::overflow_error ("overflow");

    if ( std::fabs(ret) < DBL_EPSILON )  // ret == 0.0l
      throw std::underflow_error ("underflow");
  }

  return ret;
}

//----------------------------------------------------------------------
const FString FString::ltrim() const
{
  const FString s{*this};

  // handle NULL and empty string
  if ( ! (string && *string) )
    return s;

  const wchar_t* p = s.string;

  while ( std::iswspace(std::wint_t(*p)) )
    p++;

  return p;
}

//----------------------------------------------------------------------
const FString FString::rtrim() const
{
  FString s{*this};

  // handle NULL and empty string
  if ( ! (string && *string) )
    return s;

  wchar_t* p = s.string;
  wchar_t* last = p + length;

  while ( std::iswspace(std::wint_t(*--last)) && last > p )
    s.length--;

  if ( last == p && std::iswspace(std::wint_t(*last)) )
    s = L"";
  else
    *(last + 1) = '\0';

  return s;
}

//----------------------------------------------------------------------
const FString FString::trim() const
{
  // handle NULL and empty string
  if ( ! (string && *string) )
    return *this;

  const FString s{ltrim()};
  return s.rtrim();
}

//----------------------------------------------------------------------
const FString FString::left (std::size_t len) const
{
  FString s{*this};

  // handle NULL and empty string
  if ( ! (string && *string) )
    return s;

  if ( len > length )
    return s;

  wchar_t* p = s.string;
  s.length = len;
  *(p + len) = '\0';
  return s;
}

//----------------------------------------------------------------------
const FString FString::right (std::size_t len) const
{
  const FString s{*this};

  // handle NULL and empty string
  if ( ! (string && *string) )
    return s;

  if ( len > length )
    return s;

  const wchar_t* p = s.string;
  p += (length - len);
  return p;
}

//----------------------------------------------------------------------
const FString FString::mid (std::size_t pos, std::size_t len) const
{
  const FString s{*this};

  // handle NULL and empty string
  if ( ! (string && *string) )
    return s;

  if ( pos == 0 )
    pos = 1;

  if ( pos <= length && pos + len > length )
    len = length - pos + 1;

  if ( pos > length || pos + len - 1 > length || len == 0 )
    return FString{L""};

  wchar_t* p = s.string;
  wchar_t* first = p + pos - 1;
  *(first + len) = '\0';
  return first;
}

//----------------------------------------------------------------------
FStringList FString::split (const FString& delimiter)
{
  const FString s{*this};
  FStringList string_list{};

  // handle NULL and empty string
  if ( ! (string && *string) )
    return string_list;

  wchar_t* rest{nullptr};
  const wchar_t* token = _extractToken(&rest, s.string, delimiter.wc_str());

  while ( token )
  {
    string_list.push_back (FString{token});
    token = _extractToken (&rest, nullptr, delimiter.wc_str());
  }

  return string_list;
}

//----------------------------------------------------------------------
FString& FString::setString (const FString& s)
{
  _assign (s.string);
  return *this;
}

//----------------------------------------------------------------------
FString& FString::setNumber (sInt64 num)
{
  wchar_t buf[30]{};
  wchar_t* s = &buf[29];  // Pointer to the last character
  uInt64 abs_num = static_cast<uInt64>(num);

  if ( num < 0 )
    abs_num = static_cast<uInt64>(-num);

  *s = '\0';

  do
  {
    *--s = L"0123456789"[abs_num % 10];
    abs_num /= 10;
  }
  while ( abs_num );

  if ( num < 0 )
    *--s = '-';

  _assign (s);
  return *this;
}

//----------------------------------------------------------------------
FString& FString::setNumber (uInt64 num)
{
  wchar_t buf[30]{};
  wchar_t* s = &buf[29];  // Pointer to the last character
  *s = '\0';

  do
  {
    *--s = L"0123456789"[num % 10];
    num /= 10;
  }
  while ( num );

  _assign (s);
  return *this;
}

//----------------------------------------------------------------------
FString& FString::setNumber (lDouble f_num, int precision)
{
  wchar_t format[20]{};  // = "%.<precision>Lg"
  wchar_t* s = &format[0];
  *s++ = L'%';
  *s++ = L'.';

  // The precision can not have more than 2 digits
  if ( precision > 99 )
    precision = 99;

  if ( precision >= 10 )
  {
    // The precision value is 2 digits long
    *s++ = precision / 10 + L'0';
    *s++ = precision % 10 + L'0';
  }
  else
  {
    // The precision value has only 1 digit
    *s++ = precision + L'0';
  }

  *s++ = L'L';
  *s++ = L'g';
  *s = L'\0';

  return sprintf(format, f_num);
}

//----------------------------------------------------------------------
FString& FString::setFormatedNumber (sInt64 num, char separator)
{
  int n{0};
  wchar_t buf[30]{};
  wchar_t* s = &buf[29];  // Pointer to the last character
  uInt64 abs_num = static_cast<uInt64>(num);

  if ( separator == 0 )
    separator = ' ';

  if ( num < 0 )
    abs_num = static_cast<uInt64>(-num);

  *s = L'\0';

  do
  {
    *--s = L"0123456789"[abs_num % 10];
    abs_num /= 10;
    n++;

    if ( abs_num && n % 3 == 0 )
      *--s = separator;
  }
  while ( abs_num );

  if ( num < 0 )
    *--s = '-';

  _assign (s);
  return *this;
}

//----------------------------------------------------------------------
FString& FString::setFormatedNumber (uInt64 num, char separator)
{
  int n{0};
  wchar_t buf[30]{};
  wchar_t* s = &buf[29];  // Pointer to the last character
  *s = L'\0';

  if ( separator == 0 )
    separator = ' ';

  do
  {
    *--s = L"0123456789"[num % 10];
    num /= 10;
    n++;

    if ( num && n % 3 == 0 )
      *--s = separator;
  }
  while ( num );

  _assign (s);
  return *this;
}

// FString operators
//----------------------------------------------------------------------
bool FString::operator < (const FString& s) const
{
  if ( string && ! s.string )
    return false;

  if ( ! string && s.string )
    return true;

  if ( ! (string || s.string) )
    return false;

  return ( std::wcscmp(string, s.string) < 0 );
}

//----------------------------------------------------------------------
bool FString::operator <= (const FString& s) const
{
  if ( ! (string || s.string) )
    return true;

  if ( string && ! s.string )
    return false;

  if ( ! string && s.string )
    return true;

  return ( std::wcscmp(string, s.string) <= 0 );
}

//----------------------------------------------------------------------
bool FString::operator == (const FString& s) const
{
  if ( ! (string || s.string) )
    return true;

  if ( bool(string) != bool(s.string) || length != s.length )
    return false;

  return ( std::wcscmp(string, s.string) == 0 );
}

//----------------------------------------------------------------------
bool FString::operator != (const FString& s) const
{
  if ( ! (string || s.string) )
    return false;

  if ( bool(string) != bool(s.string) || length != s.length )
    return true;

  return ( std::wcscmp(string, s.string) != 0 );
}

//----------------------------------------------------------------------
bool FString::operator >= (const FString& s) const
{
  if ( string && ! s.string )
    return true;

  if ( ! string && s.string )
    return false;

  if ( ! (string || s.string) )
    return true;

  return ( std::wcscmp(string, s.string) >= 0 );
}

//----------------------------------------------------------------------
bool FString::operator > (const FString& s) const
{
  if ( ! (string || s.string) )
    return false;

  if ( string && ! s.string )
    return true;

  if ( ! string && s.string )
    return false;

  return ( std::wcscmp(string, s.string) > 0 );
}

//----------------------------------------------------------------------
const FString& FString::insert (const FString& s, int pos)
{
  if ( isNegative(pos) || uInt(pos) > length )
    throw std::out_of_range("");

  _insert (uInt(pos), s.length, s.string);
  return *this;
}

//----------------------------------------------------------------------
const FString& FString::insert (const FString& s, std::size_t pos)
{
  if ( pos > length )
    throw std::out_of_range("");

  _insert (pos, s.length, s.string);
  return *this;
}

//----------------------------------------------------------------------
FString const FString::replace (const FString& from, const FString& to)
{
  FString s{*this};

  // handle NULL and empty string
  if ( ! (string && *string) )
    return s;

  if ( from.isNull() || to.isNull() )
    return s;

  if ( from.isEmpty() )
    return s;

  const wchar_t* p = s.string;
  const std::size_t from_length = from.getLength();
  const std::size_t to_length = to.getLength();
  std::size_t pos{0};

  while ( *p )
  {
    if ( std::wcsncmp(p, from.string, from_length) == 0 )
    {
      s._remove(pos, from_length);
      s._insert(pos, to_length, to.string);
      pos += to_length;
      p = s.string + pos;
    }
    else
    {
      pos++;
      p++;
    }
  }

  return s;
}

//----------------------------------------------------------------------
const FString FString::replaceControlCodes() const
{
  FString s{*this};

  for (auto&& c : s)
  {
    if ( c <= L'\x1f' )
    {
      c += L'\x2400';
    }
    else if ( c == L'\x7f' )
    {
      c = L'\x2421';
    }
    else if ( c >= L'\x80' && c <= L'\x9f' )
    {
      c = L' ';
    }
    else if ( ! std::iswprint(std::wint_t(c)) )
      c = L' ';
  }

  return s;
}

//----------------------------------------------------------------------
const FString FString::expandTabs (int tabstop) const
{
  FString instr{string};
  FString outstr{};

  if ( tabstop <= 0 )
    return instr;

  const FStringList tab_split = instr.split("\t");
  const uLong last = tab_split.size();

  for (std::size_t i{0}; i < last; i++)
  {
    const std::size_t len = tab_split[i].getLength();
    const std::size_t tab_len = std::size_t(tabstop);

    if ( i == last - 1 )
      outstr += tab_split[i];
    else
      outstr += tab_split[i] + FString(tab_len - (len % tab_len), L' ');
  }

  return outstr;
}

//----------------------------------------------------------------------
FString FString::removeDel() const
{
  FString s{*this};
  std::size_t i{0};
  std::size_t count{0};

  for (auto&& c : s)
  {
    if ( c == 0x7f )
    {
      count++;
    }
    else if ( count > 0 )
    {
      count--;
    }
    else  // count == 0
    {
      s.string[i] = c;
      i++;
    }
  }

  s.string[i] = L'\0';
  s.length = i;
  return s;
}


//----------------------------------------------------------------------
FString FString::removeBackspaces() const
{
  FString s{*this};
  std::size_t i{0};

  for (auto&& c : s)
  {
    if ( c != L'\b' )
    {
      s.string[i] = c;
      i++;
    }
    else if ( i > 0 )
    {
      i--;
    }
  }

  s.string[i] = L'\0';
  s.length = i;
  return s;
}

//----------------------------------------------------------------------
const FString& FString::overwrite (const FString& s, int pos)
{
  if ( pos < 0 )
    return overwrite (s, 0);

  return overwrite (s, std::size_t(pos));
}

//----------------------------------------------------------------------
const FString& FString::overwrite (const FString& s, std::size_t pos)
{
  if ( pos > length )
    pos = length;

  if ( length >= (pos + s.length) )
  {
    std::wcsncpy (string + pos, s.string, s.length);
  }
  else
  {
    std::wcsncpy (string + pos, s.string, length - pos);
    _insert (length, pos + s.length - length, s.string + length - pos);
  }

  return *this;
}

//----------------------------------------------------------------------
const FString& FString::remove (std::size_t pos, std::size_t len)
{
  if ( pos > length )
    return *this;

  if ( pos + len > length )
    len = length - pos;

  _remove (pos, len);

  return *this;
}

//----------------------------------------------------------------------
bool FString::includes (const FString& s) const
{
  if ( ! s )
    return false;

  if ( ! (string && s.string) )
    return false;

  return ( std::wcsstr(string, s.string) != nullptr );
}


// private methods of FString
//----------------------------------------------------------------------
inline void FString::_initLength (std::size_t len)
{
  if ( len == 0 )
    return;

  length  = len;
  bufsize = FWDBUFFER + len + 1;

  try
  {
    string = new wchar_t[bufsize]();
    std::wmemset (string, L'\0', bufsize);
  }
  catch (const std::bad_alloc&)
  {
    badAllocOutput ("wchar_t[bufsize]");
  }
}

//----------------------------------------------------------------------
void FString::_assign (const wchar_t s[])
{
  if ( ! s )
  {
    clear();
    return;
  }

  if ( string && std::wcscmp(string, s) == 0 )
    return;  // string == s

  std::size_t new_length{std::wcslen(s)};

  if ( ! string || new_length > capacity() )
  {
    if ( string )
      delete[](string);

    bufsize = FWDBUFFER + new_length + 1;

    try
    {
      string = new wchar_t[bufsize]();
    }
    catch (const std::bad_alloc&)
    {
      badAllocOutput ("wchar_t[bufsize]");
      return;
    }
  }

  std::wcsncpy (string, s, bufsize);
  length = new_length;
  string[capacity()] = L'\0';
}

//----------------------------------------------------------------------
void FString::_insert (std::size_t len, const wchar_t s[])
{
  if ( len == 0 )  // String s is a null or a empty string
    return;

  if ( string )
    delete[](string);

  length = len;
  bufsize = FWDBUFFER + length + 1;

  try
  {
    string = new wchar_t[bufsize]();
  }
  catch (const std::bad_alloc&)
  {
    badAllocOutput ("wchar_t[bufsize]");
    return;
  }

  std::wcsncpy (string, s, bufsize);
  string[capacity()] = L'\0';
}

//----------------------------------------------------------------------
void FString::_insert ( std::size_t pos
                      , std::size_t len
                      , const wchar_t s[] )
{
  if ( len == 0 )  // String s is a null or a empty string
    return;

  if ( ! string )  // string is null
  {
    _insert (len, s);
  }
  else
  {
    std::size_t x{};

    if ( length + len <= capacity() )
    {
      // output string <= bufsize
      for (x = length; x + 1 > pos; x--)  // shifting right side + '\0'
        string[x + len] = string[x];

      for (x = 0; x < len; x++)           // insert string
        string[x + pos] = s[x];

      length += len;
    }
    else
    {
      // output string > bufsize
      bufsize = FWDBUFFER + length + len + 1;
      wchar_t* sptr{};

      try
      {
        sptr = new wchar_t[bufsize]();  // generate new string
      }
      catch (const std::bad_alloc&)
      {
        badAllocOutput ("wchar_t[bufsize]");
        return;
      }

      std::size_t y{0};

      for (x = 0; x < pos; x++)           // left side
        sptr[y++] = string[x];

      for (x = 0 ; x < len; x++)          // insert string
        sptr[y++] = s[x];

      for (x = pos; x < length + 1; x++)  // right side + '\0'
        sptr[y++] = string[x];

      length += len;
      delete[](string);                   // delete old string
      string = sptr;
    }
  }
}

//----------------------------------------------------------------------
void FString::_remove (std::size_t pos, std::size_t len)
{
  if ( capacity() - length + len <= FWDBUFFER )
  {
    // shifting left side to pos
    for (std::size_t i = pos; i + len < length + 1; i++)
      string[i] = string[i + len];

    length -= len;
  }
  else
  {
    bufsize = length + 1 - len + FWDBUFFER;
    wchar_t* sptr{};

    try
    {
      sptr = new wchar_t[bufsize]();    // generate new string
    }
    catch (const std::bad_alloc&)
    {
      badAllocOutput ("wchar_t[bufsize]");
      return;
    }

    std::size_t x{};
    std::size_t y{};

    for (x = 0; x < pos; x++)             // left side
      sptr[y++] = string[x];

    for (x = pos + len; x < length + 1; x++)  // right side + '\0'
      sptr[y++] = string[x];

    delete[](string);                   // delete old string
    string = sptr;
    length -= len;
  }
}

//----------------------------------------------------------------------
inline const char* FString::_to_cstring (const wchar_t s[]) const
{
  if ( ! s )  // handle NULL string
    return nullptr;

  if ( ! *s )  // handle empty string
  {
    try
    {
      // Generate a empty string ("")
      c_string = new char[1]();
    }
    catch (const std::bad_alloc&)
    {
      badAllocOutput ("char[1]");
      return nullptr;
    }

    return c_string;
  }

  if ( c_string )
    delete[](c_string);

  const int size = int(std::wcslen(s)) + 1;
  const int dest_size = size * int(CHAR_SIZE);
  const wchar_t* src = s;
  std::mbstate_t state{};
  std::memset (&state, '\0', sizeof(mbstate_t));

  try
  {
    c_string = new char[std::size_t(dest_size)]();

    // pre-initialiaze the whole string with '\0'
    std::memset (c_string, '\0', std::size_t(dest_size));
  }
  catch (const std::bad_alloc&)
  {
    badAllocOutput ("char[std::size_t(dest_size)]");
    return nullptr;
  }

  const int mblength = \
      int(std::wcsrtombs (c_string, &src, std::size_t(dest_size), &state));

  if ( mblength == -1 && errno != EILSEQ )
  {
    delete[](c_string);
    c_string = nullptr;
    return "";
  }

  return c_string;
}

//----------------------------------------------------------------------
inline const wchar_t* FString::_to_wcstring (const char s[]) const
{
  if ( ! s )   // handle NULL string
    return nullptr;

  if ( ! *s )  // handle empty string
  {
    try
    {
      // Generate a empty wide string (L"")
      return new wchar_t[1]();
    }
    catch (const std::bad_alloc&)
    {
      badAllocOutput ("wchar_t[1]");
      return nullptr;
    }
  }

  const int size = int(std::strlen(s)) + 1;
  const int dest_size = size * int(CHAR_SIZE);
  const char* src = s;
  wchar_t* dest{};
  std::mbstate_t state{};
  std::memset (&state, '\0', sizeof(mbstate_t));

  try
  {
    dest = new wchar_t[std::size_t(size)]();
    // pre-initialiaze the whole string with '\0'
    std::wmemset (dest, L'\0', std::size_t(size));
  }
  catch (const std::bad_alloc&)
  {
    badAllocOutput ("wchar_t[std::size_t(size)]");
    return nullptr;
  }

  const int wclength = \
      int(std::mbsrtowcs (dest, &src, std::size_t(dest_size), &state));

  if ( wclength == -1 )
  {
    if ( src != s )
      return dest;
    else
    {
      delete[] dest;
      return nullptr;
    }
  }

  if ( wclength == size )
    dest[size - 1] = '\0';

  if ( wclength )
    return dest;
  else
  {
    delete[] dest;
    return nullptr;
  }
}

//----------------------------------------------------------------------
inline const wchar_t* FString::_extractToken ( wchar_t* rest[]
                                             , const wchar_t s[]
                                             , const wchar_t delim[] )
{
  wchar_t* token = ( s ) ? const_cast<wchar_t*>(s) : *rest;

  if ( ! token )
    return nullptr;

  if ( ! token[0] )
    return nullptr;

  *rest = std::wcspbrk(token, delim);

  if ( *rest )
    *(*rest)++ = '\0';
  else
    *rest = token + std::wcslen(token);
  return token;
}


// FString non-member operators
//----------------------------------------------------------------------
const FString operator + (const FString& s1, const FString& s2)
{
  FString tmp{s1};
  tmp._insert ( uInt(std::wcslen(s1.wc_str()))
              , uInt(std::wcslen(s2.wc_str()))
              , s2.wc_str() );
  return tmp;
}

//----------------------------------------------------------------------
const FString operator + (const FString& s, const wchar_t c)
{
  FString tmp{s};
  tmp._insert ( uInt(std::wcslen(s.wc_str())), 1, &c);
  return tmp;
}

//----------------------------------------------------------------------
const FString operator + (const std::wstring& s1, const FString& s2)
{
  FString tmp{s1};
  tmp._insert ( uInt(std::wcslen(s1.c_str()))
              , uInt(std::wcslen(s2.wc_str()))
              , s2.wc_str() );
  return tmp;
}

//----------------------------------------------------------------------
const FString operator + (const wchar_t s1[], const FString& s2)
{
  FString tmp{s1};
  tmp._insert ( uInt(std::wcslen(s1))
              , uInt(std::wcslen(s2.wc_str()))
              , s2.wc_str() );
  return tmp;
}

//----------------------------------------------------------------------
const FString operator + (const std::string& s1, const FString& s2)
{
  FString tmp{s1};
  tmp._insert ( tmp.getLength()
              , uInt(std::wcslen(s2.wc_str()))
              , s2.wc_str() );
  return tmp;
}

//----------------------------------------------------------------------
const FString operator + (const char s1[], const FString& s2)
{
  FString tmp{s1};
  tmp._insert ( tmp.getLength()
              , uInt(std::wcslen(s2.wc_str()))
              , s2.wc_str() );
  return tmp;
}

//----------------------------------------------------------------------
const FString operator + (const wchar_t c, const FString& s)
{
  FString tmp{c};
  tmp._insert (1, uInt(std::wcslen(s.wc_str())), s.wc_str());
  return tmp;
}

//----------------------------------------------------------------------
const FString operator + (const char c, const FString& s)
{
  FString tmp{c};
  tmp._insert (1, uInt(std::wcslen(s.wc_str())), s.wc_str());
  return tmp;
}

//----------------------------------------------------------------------
const FString operator + (const FString& s, const char c)
{
  FString tmp1{s};
  wchar_t tmp2[2];
  tmp2[0] = wchar_t(c & 0xff);
  tmp2[1] = L'\0';
  tmp1._insert (s.length, 1, tmp2);
  return tmp1;
}

//----------------------------------------------------------------------
std::ostream& operator << (std::ostream& outstr, const FString& s)
{
  const std::size_t width = std::size_t(outstr.width());

  if ( s.length > 0 )
  {
    outstr << s._to_cstring(s.string);
  }
  else if ( width > 0 )
  {
    const FString fill_str{width, wchar_t(outstr.fill())};
    outstr << s._to_cstring(fill_str.string);
  }

  return outstr;
}

//----------------------------------------------------------------------
std::istream& operator >> (std::istream& instr, FString& s)
{
  char buf[FString::INPBUFFER + 1]{};
  instr.getline (buf, FString::INPBUFFER);
  const wchar_t* wc_str = s._to_wcstring(buf);

  if ( wc_str )
  {
    s._assign (wc_str);
    delete[] wc_str;
  }

  return instr;
}

//----------------------------------------------------------------------
std::wostream& operator << (std::wostream& outstr, const FString& s)
{
  const std::size_t width = std::size_t(outstr.width());

  if ( s.length > 0 )
  {
    outstr << s.string;
  }
  else if ( width > 0 )
  {
    const FString fill_str{width, outstr.fill()};
    outstr << fill_str.string;
  }

  return outstr;
}

//----------------------------------------------------------------------
std::wistream& operator >> (std::wistream& instr, FString& s)
{
  wchar_t buf[FString::INPBUFFER + 1]{};
  instr.getline (buf, FString::INPBUFFER);
  s._assign (buf);
  return instr;
}

}  // namespace finalcut
