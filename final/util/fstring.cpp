/***********************************************************************
* fstring.cpp - Unicode string class with UTF-8 support                *
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

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "final/fapplication.h"
#include "final/util/flog.h"
#include "final/util/fstring.h"

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
    string = std::wstring(std::size_t(len), L'\0');
}

//----------------------------------------------------------------------
FString::FString (std::size_t len)
  : string{std::wstring(len, L'\0')}
{ }

//----------------------------------------------------------------------
FString::FString (std::size_t len, wchar_t c)
  : string{std::wstring(len, c)}
{ }

//----------------------------------------------------------------------
FString::FString (std::size_t len, const UniChar& c)
  : FString(len, wchar_t(c))
{ }

//----------------------------------------------------------------------
FString::FString (const FString& s)  // copy constructor
{
  internal_assign(std::wstring{s.string});
}

//----------------------------------------------------------------------
FString::FString (FString&& s) noexcept  // move constructor
  : string{std::move(s.string)}
{ }

//----------------------------------------------------------------------
FString::FString (const std::wstring& s)
{
  internal_assign(std::wstring{s});
}

//----------------------------------------------------------------------
FString::FString (std::wstring&& s)
  : string{std::move(s)}
{ }

//----------------------------------------------------------------------
FString::FString (const wchar_t s[])
{
  if ( s )
    internal_assign(std::wstring{s});
}

//----------------------------------------------------------------------
FString::FString (const std::string& s)
{
  if ( ! s.empty() )
    internal_assign(internal_toWideString(s));
}

//----------------------------------------------------------------------
FString::FString (const char s[])
{
  if ( s )
    internal_assign(internal_toWideString(s));
}

//----------------------------------------------------------------------
FString::FString (const UniChar& c)
{
  internal_assign(std::wstring{static_cast<wchar_t>(c)});
}

//----------------------------------------------------------------------
FString::FString (const wchar_t c)
{
  if ( c )
    internal_assign(std::wstring{c});
}

//----------------------------------------------------------------------
FString::FString (const char c)
{
  if ( c )
    internal_assign(std::wstring{wchar_t(uChar(c))});
}

//----------------------------------------------------------------------
FString::~FString() = default;  // destructor


// FString operators
//----------------------------------------------------------------------
auto FString::operator = (const FString& s) -> FString&
{
  if ( &s != this )
    internal_assign(std::wstring{s.string});

  return *this;
}

//----------------------------------------------------------------------
auto FString::operator = (FString&& s) noexcept -> FString&
{
  if ( &s != this )
    string = s.string;

  return *this;
}

//----------------------------------------------------------------------
auto FString::operator += (const FString& s) -> const FString&
{
  string.append(s.string);
  return *this;
}

//----------------------------------------------------------------------
auto FString::operator << (const FString& s) -> FString&
{
  string.append(s.string);
  return *this;
}

//----------------------------------------------------------------------
auto FString::operator << (const UniChar& c) -> FString&
{
  FString s{static_cast<wchar_t>(c)};
  string.append(s.string);
  return *this;
}

//----------------------------------------------------------------------
auto FString::operator << (const wchar_t c) -> FString&
{
  FString s{c};
  string.append(s.string);
  return *this;
}

//----------------------------------------------------------------------
auto FString::operator << (const char c) -> FString&
{
  FString s{c};
  string.append(s.string);
  return *this;
}

//----------------------------------------------------------------------
FString::operator bool () const
{
  return ! isEmpty();
}

//----------------------------------------------------------------------
auto FString::operator () () const -> const FString&
{
  return *this;
}


// public methods of FString
//----------------------------------------------------------------------
auto FString::clear() -> FString&
{
  string.clear();
  return *this;
}

//----------------------------------------------------------------------
auto FString::wc_str() const -> const wchar_t*
{
  // Returns a constant wide character string

  return string.c_str();
}

//----------------------------------------------------------------------
auto FString::wc_str() -> wchar_t*
{
  // Returns a wide character string

  return const_cast<wchar_t*>(string.c_str());
}

//----------------------------------------------------------------------
auto FString::c_str() const -> const char*
{
  // Returns a constant c-string

  if ( isEmpty() )
    return "";

  char_string = internal_toCharString(string);
  return char_string.c_str();
}

//----------------------------------------------------------------------
auto FString::c_str() -> char*
{
  // Returns a c-string

  if ( isEmpty() )
    return const_cast<char*>("");

  char_string = internal_toCharString(string);
  return const_cast<char*>(char_string.c_str());
}

//----------------------------------------------------------------------
auto FString::toWString() const -> std::wstring
{
  return string;
}

//----------------------------------------------------------------------
auto FString::toString() const -> std::string
{
  return internal_toCharString(string);
}

//----------------------------------------------------------------------
auto FString::toLower() const -> FString
{
  FString s{*this};

  auto to_lower = \
      [] (auto& c)
      {
        c = wchar_t(std::towlower(std::wint_t(c)));
      };

  std::for_each (s.begin(), s.end(), to_lower);
  return s;
}

//----------------------------------------------------------------------
auto FString::toUpper() const -> FString
{
  FString s{*this};

  auto to_upper = \
      [] (auto& c)
      {
        c = wchar_t(std::towupper(std::wint_t(c)));
      };

  std::for_each (s.begin(), s.end(), to_upper);
  return s;
}

//----------------------------------------------------------------------
auto FString::toShort() const -> sInt16
{
  const long num = toLong();

  if ( num > SHRT_MAX )
    throw std::overflow_error ("overflow");

  if ( num < SHRT_MIN )
    throw std::underflow_error ("underflow");

  return static_cast<sInt16>(num);
}

//----------------------------------------------------------------------
auto FString::toUShort() const -> uInt16
{
  const uLong num = toULong();

  if ( num > USHRT_MAX )
    throw std::overflow_error ("overflow");

  return static_cast<uInt16>(num);
}

//----------------------------------------------------------------------
auto FString::toInt() const -> int
{
  const long num = toLong();

  if ( num > INT_MAX )
    throw std::overflow_error ("overflow");

  if ( num < INT_MIN )
    throw std::underflow_error ("underflow");

  return static_cast<int>(num);
}

//----------------------------------------------------------------------
auto FString::toUInt() const -> uInt
{
  const uLong num = toULong();

  if ( num > UINT_MAX )
    throw std::overflow_error ("overflow");

  return static_cast<uInt>(num);
}

//----------------------------------------------------------------------
auto FString::toLong() const -> long
{
  bool neg{false};
  long num{0};
  long tenth_limit{LONG_MAX / 10};
  long tenth_limit_digit{LONG_MAX % 10};
  const FString s{trim()};
  auto iter = s.string.cbegin();

  if ( s.isEmpty() )
    throw std::invalid_argument ("empty value");

  if ( *iter == L'-' )
  {
    ++iter;
    neg = true;
    tenth_limit = -(LONG_MIN / 10);
    tenth_limit_digit += 1;
  }
  else if ( *iter == L'+' )
  {
    ++iter;
  }

  while ( std::iswdigit(std::wint_t(*iter)) )
  {
    const auto d = uChar(*iter - L'0');

    if ( num > tenth_limit
      || (num == tenth_limit && d > tenth_limit_digit) )
    {
      if ( neg )
        throw std::underflow_error ("underflow");

      throw std::overflow_error ("overflow");
    }

    num = (10 * num) + d;
    ++iter;
  }

  if ( *iter != L'\0' && ! std::iswdigit(std::wint_t(*iter)) )
    throw std::invalid_argument ("no valid number");

  if ( neg )
    num = (~num) + 1u;

  return num;
}

//----------------------------------------------------------------------
auto FString::toULong() const -> uLong
{
  uLong num{0};
  const uLong tenth_limit{ULONG_MAX / 10};
  const uLong tenth_limit_digit{ULONG_MAX % 10};
  const FString s{trim()};
  auto iter = s.string.cbegin();

  if ( s.isEmpty() )
    throw std::invalid_argument ("empty value");

  if ( *iter == L'-' )
    throw std::underflow_error ("underflow");

  if ( *iter == L'+' )
    ++iter;

  while ( std::iswdigit(std::wint_t(*iter)) )
  {
    const auto d = uChar(*iter - L'0');

    if ( num > tenth_limit
      || (num == tenth_limit && d > tenth_limit_digit) )
    {
      throw std::overflow_error ("overflow");
    }

    num = (10 * num) + d;
    ++iter;
  }

  if ( *iter != L'\0' && ! std::iswdigit(std::wint_t(*iter)) )
    throw std::invalid_argument ("no valid number");

  return num;
}

//----------------------------------------------------------------------
auto FString::toFloat() const -> float
{
  const double num = toDouble();

  if ( num > double(FLT_MAX) || num < double(-FLT_MAX) )
    throw std::overflow_error ("overflow");

  if ( std::fabs(num) < double(FLT_EPSILON) )  // num == 0.0f
    throw std::underflow_error ("underflow");

  return static_cast<float>(num);
}

//----------------------------------------------------------------------
auto FString::toDouble() const -> double
{
  if ( isEmpty() )
    throw std::invalid_argument ("null value");

  wchar_t* p{};
  const double ret = std::wcstod(string.c_str(), &p);

  if ( p != nullptr && *p != L'\0' )
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
auto FString::ltrim() const -> FString
{
  // Handle empty string
  if ( isEmpty() )
    return *this;

  auto iter = string.cbegin();
  const auto last = string.cend();

  while ( iter != last && std::iswspace(std::wint_t(*iter)) )
    ++iter;

  if ( iter != last )
    return std::wstring(iter, last);

  return {};
}

//----------------------------------------------------------------------
auto FString::rtrim() const -> FString
{
  // Handle empty string
  if ( isEmpty() )
    return *this;

  const auto r_end = string.crend();
  auto r_iter = string.crbegin();

  while ( r_iter != r_end && std::iswspace(std::wint_t(*r_iter)) )
    ++r_iter;

  if ( r_iter != r_end )
    return std::wstring( std::make_reverse_iterator(r_end)
                       , std::make_reverse_iterator(r_iter) );

  return {};
}

//----------------------------------------------------------------------
auto FString::trim() const -> FString
{
  // Handle NULL and empty string
  if ( isEmpty() )
    return *this;

  const FString s{ltrim()};
  return s.rtrim();
}

//----------------------------------------------------------------------
auto FString::left (std::size_t len) const -> FString
{
  // Handle empty and too long strings
  if ( isEmpty() || len > getLength() )
    return *this;

  return string.substr(0, len);
}

//----------------------------------------------------------------------
auto FString::right (std::size_t len) const -> FString
{
  // Handle empty and too long strings
  if ( isEmpty() || len > getLength() )
    return *this;

  return string.substr(string.size() - len, len);
}

//----------------------------------------------------------------------
auto FString::mid (std::size_t pos, std::size_t len) const -> FString
{
  // Handle empty string
  if ( isEmpty() )
    return *this;

  if ( pos == 0 )
    pos = 1;

  const auto& length = string.length();

  if ( pos <= length && pos + len > length )
    len = length - pos + 1;

  if ( pos > length || pos + len - 1 > length || len == 0 )
    return {};

  auto first = string.cbegin() + static_cast<difference_type>(pos) - 1;
  auto last = first + static_cast<difference_type>(len);
  return std::wstring(first, last);
}

//----------------------------------------------------------------------
auto FString::split (const FString& delimiter) const -> FStringList
{
  if ( isEmpty() )
    return {};

  const FString s{*this};
  FStringList string_list{};
  const auto& delimiter_length = delimiter.getLength();
  std::wstring::size_type first = 0;
  std::wstring::size_type last;

  while ( (last = s.string.find(delimiter.string, first)) != std::wstring::npos )
  {
    string_list.emplace_back(std::wstring(s.string, first, last - first));
    first = last + delimiter_length;
  }

  string_list.emplace_back(std::wstring(s.string, first));
  return string_list;
}

//----------------------------------------------------------------------
auto FString::setString (const FString& s) -> FString&
{
  std::wstring str{s.string};
  internal_assign (std::move(str));
  return *this;
}

//----------------------------------------------------------------------
auto FString::setNumber (sInt64 num) -> FString&
{
  std::array<wchar_t, 30> buf{};
  wchar_t* s = &buf[29];  // Pointer to the last character
  auto abs_num = ( num >= 0 )
               ? static_cast<uInt64>(num)
               : ~static_cast<uInt64>(num) + 1;
  *s = '\0';

  do
  {
    *--s = L"0123456789"[abs_num % 10];
    abs_num /= 10;
  }
  while ( abs_num );

  if ( num < 0 )
    *--s = '-';

  std::wstring str{s};
  internal_assign (std::move(str));
  return *this;
}

//----------------------------------------------------------------------
auto FString::setNumber (uInt64 num) -> FString&
{
  std::array<wchar_t, 30> buf{};
  wchar_t* s = &buf[29];  // Pointer to the last character
  *s = '\0';

  do
  {
    *--s = L"0123456789"[num % 10];
    num /= 10;
  }
  while ( num );

  std::wstring str{s};
  internal_assign (std::move(str));
  return *this;
}

//----------------------------------------------------------------------
auto FString::setNumber (lDouble f_num, int precision) -> FString&
{
  std::array<wchar_t, 20> format{};  // = "%.<precision>Lg"
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

  return sprintf(format.data(), f_num);
}

//----------------------------------------------------------------------
auto FString::setFormatedNumber (sInt64 num, FString separator) -> FString&
{
  int n{0};
  std::array<wchar_t, 30> buf{};
  wchar_t* s = &buf[29];  // Pointer to the last character
  auto abs_num = static_cast<uInt64>(num);

  if ( separator[0] == 0 )
    separator = L" ";

  if ( num < 0 )
    abs_num = static_cast<uInt64>(-num);

  *s = L'\0';

  do
  {
    *--s = L"0123456789"[abs_num % 10];
    abs_num /= 10;
    n++;

    if ( abs_num && n % 3 == 0 )
      *--s = separator[0];
  }
  while ( abs_num );

  if ( num < 0 )
    *--s = '-';

  std::wstring str{s};
  internal_assign (std::move(str));
  return *this;
}

//----------------------------------------------------------------------
auto FString::setFormatedNumber (uInt64 num, FString separator) -> FString&
{
  int n{0};
  std::array<wchar_t, 30> buf{};
  wchar_t* s = &buf[29];  // Pointer to the last character
  *s = L'\0';

  if ( separator[0] == 0 )
    separator = L" ";

  do
  {
    *--s = L"0123456789"[num % 10];
    num /= 10;
    n++;

    if ( num && n % 3 == 0 )
      *--s = separator[0];
  }
  while ( num );

  std::wstring str{s};
  internal_assign (std::move(str));
  return *this;
}

// FString operators
//----------------------------------------------------------------------
auto FString::insert (const FString& s, int pos) -> const FString&
{
  if ( isNegative(pos) || uInt(pos) > string.length() )
    throw std::out_of_range("");

  string.insert(uInt(pos), s.string, 0, s.getLength());
  return *this;
}

//----------------------------------------------------------------------
auto FString::insert (const FString& s, std::size_t pos) -> const FString&
{
  if ( pos > string.length() )
    throw std::out_of_range("");

  string.insert(uInt(pos), s.string, 0, s.getLength());
  return *this;
}

//----------------------------------------------------------------------
auto FString::replace (const FString& from, const FString& to) const -> FString
{
  // Handle empty strings
  if ( isEmpty() || from.isEmpty() )
    return *this;

  FString s{*this};
  std::wstring::size_type pos{};

  while ( (pos = s.string.find(from.string, pos)) != std::wstring::npos )
  {
     s.string.replace (pos, from.getLength(), to.string);
     pos += to.getLength();
  }

  return s;
}

//----------------------------------------------------------------------
auto FString::replaceControlCodes() const -> FString
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
    else if ( (c >= L'\x80' && c <= L'\x9f') || ! isPrintable(c) )
      c = L' ';
  }

  return s;
}

//----------------------------------------------------------------------
auto FString::expandTabs (int tabstop) const -> FString
{
  if ( tabstop <= 0 )
    return *this;

  FString outstr{};
  const auto& tab_split = split(L"\t");
  const auto& last = tab_split.cend() - 1;
  auto iter = tab_split.cbegin();

  while ( iter != tab_split.cend() )
  {
    if ( iter != last )
    {
      const auto& len = iter->getLength();
      const auto& tab_len = std::size_t(tabstop);
      outstr += *iter + std::wstring(tab_len - (len % tab_len), L' ');
    }
    else
      outstr += *iter;

    ++iter;
  }

  return outstr;
}

//----------------------------------------------------------------------
auto FString::removeDel() const -> FString
{
  FString s{*this};
  std::size_t i{0};
  std::size_t count{0};

  for (const auto& c : s)
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

  s.string.erase(i);
  return s;
}


//----------------------------------------------------------------------
auto FString::removeBackspaces() const -> FString
{
  FString s{*this};
  std::size_t i{0};

  for (const auto& c : s)
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

  s.string.erase(i);
  return s;
}

//----------------------------------------------------------------------
auto FString::overwrite (const FString& s, int pos) -> const FString&
{
  if ( pos < 0 )
    return overwrite (s, 0);

  return overwrite (s, std::size_t(pos));
}

//----------------------------------------------------------------------
auto FString::overwrite (const FString& s, std::size_t pos) -> const FString&
{
  if ( pos > string.length() )
    pos = string.length();

  string.replace(pos, s.getLength(), s.string);
  return *this;
}

//----------------------------------------------------------------------
auto FString::remove (std::size_t pos, std::size_t len) -> const FString&
{
  const auto& length = string.length();

  if ( pos > length )
    return *this;

  if ( pos + len > length )
    len = length - pos;

  string.erase (pos, len);
  return *this;
}

//----------------------------------------------------------------------
auto FString::includes (const FString& s) const -> bool
{
  if ( s.isEmpty() )
    return false;

  return string.find(s.string) != std::wstring::npos;
}


// private methods of FString
//----------------------------------------------------------------------
inline void FString::internal_assign (std::wstring s)
{
  s.swap(string);
}

//----------------------------------------------------------------------
auto FString::internal_toCharString (const std::wstring& s) const -> std::string
{
  if ( s.empty() )
    return {};

  auto src = s.c_str();
  auto state = std::mbstate_t();
  const auto size = std::wcsrtombs(nullptr, &src, 0, &state);

  if ( size == MALFORMED_STRING )
    return {};

  std::vector<char> dest(size + 1);  // Contains terminator "\0"
  const auto mblength = std::wcsrtombs (dest.data(), &src, dest.size(), &state);

  if ( mblength == MALFORMED_STRING && errno != EILSEQ )
    return {};

  return dest.data();
}

//----------------------------------------------------------------------
inline auto FString::internal_toWideString (const std::string& s) const -> std::wstring
{
  if ( s.empty() )
    return {};

  auto src = s.c_str();
  auto state = std::mbstate_t();
  auto size = std::mbsrtowcs(nullptr, &src, 0, &state);

  if ( size == MALFORMED_STRING )
    return {};

  size++;
  std::vector<wchar_t> dest(size);
  const auto wide_length = std::mbsrtowcs (dest.data(), &src, size, &state);

  if ( wide_length == MALFORMED_STRING )
    return {};

  if ( wide_length == size )
    dest[size - 1] = '\0';

  if ( wide_length != 0 )
    return {dest.data(), wide_length};

  return {};
}


// FString non-member operators
//----------------------------------------------------------------------
auto FStringCaseCompare (const FString& s1, const FString& s2) -> int
{
  if ( &s1 == &s2 )
    return 0;

  auto iter1 = s1.cbegin();
  auto iter2 = s2.cbegin();

  while ( iter1 != s1.cend() )
  {
    if ( iter2 != s2.cend() )
    {
      int cmp = int(std::tolower(*iter1)) - int(std::tolower(*iter2));

      if ( cmp != 0 )
        return cmp;

      ++iter2;
    }
    else
      return int(std::tolower(*iter1));

    ++iter1;
  }

  return -int(std::tolower(*iter2));
}


}  // namespace finalcut
