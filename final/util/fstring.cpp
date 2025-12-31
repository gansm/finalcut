/***********************************************************************
* fstring.cpp - Unicode string class with UTF-8 support                *
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

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "final/fapplication.h"
#include "final/util/flog.h"
#include "final/util/fstring.h"

namespace finalcut
{

// Herper functions
template <typename T>
constexpr auto isExactlyEqual (T rhs, T lhs) noexcept -> bool
{
#if defined(__clang__)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wfloat-equal"
#endif

  return rhs == lhs;

#if defined(__clang__)
  #pragma clang diagnostic pop
#endif
}

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
    string = std::wstring(size_type(len), L'\0');
}

//----------------------------------------------------------------------
FString::FString (size_type len)
  : string{std::wstring(len, L'\0')}
{ }

//----------------------------------------------------------------------
FString::FString (size_type len, wchar_t c)
  : string{std::wstring(len, c)}
{ }

//----------------------------------------------------------------------
FString::FString (size_type len, const UniChar& c)
  : FString(len, wchar_t(c))
{ }

//----------------------------------------------------------------------
FString::FString (const FString& s)  // copy constructor
  : string{s.string}
{ }

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
#if __cplusplus >= 201703L
FString::FString (const std::wstring_view& s)
{
  if ( ! s.empty() )
    internal_assign(std::wstring{s});
}
#endif

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
    internal_assign(internal_toWideString(s.c_str()));
}

//----------------------------------------------------------------------
#if __cplusplus >= 201703L
FString::FString (const std::string_view& s)
{
  if ( ! s.empty() )
    internal_assign(internal_toWideString(s.data()));
}
#endif

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
FString::~FString() noexcept = default;  // destructor


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
FString::operator bool () const noexcept
{
  return ! isEmpty();
}

//----------------------------------------------------------------------
auto FString::operator () () const noexcept -> const FString&
{
  return *this;
}


// public methods of FString
//----------------------------------------------------------------------
auto FString::clear() noexcept -> FString&
{
  string.clear();
  return *this;
}

//----------------------------------------------------------------------
auto FString::reserve (size_type new_capacity) -> FString&
{
  string.reserve(new_capacity);
  return *this;
}

//----------------------------------------------------------------------
auto FString::shrink_to_fit() -> FString&
{
  string.shrink_to_fit();
  return *this;
}

//----------------------------------------------------------------------
auto FString::resize (size_type count) -> FString&
{
  string.resize(count);
  return *this;
}

//----------------------------------------------------------------------
auto FString::resize (size_type count, wchar_t c) -> FString&
{
  string.resize(count, c);
  return *this;
}

//----------------------------------------------------------------------
auto FString::wc_str() const noexcept -> const wchar_t*
{
  // Returns a constant wide character string

  return string.c_str();
}

//----------------------------------------------------------------------
auto FString::wc_str() noexcept -> wchar_t*
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
  FString str;
  str.string.reserve(string.length());
  constexpr wchar_t offset = L'a' - L'A';

  for (wchar_t c : string)
  {
    if ( c < 128 )  // Fast path for ASCII
    {
      const bool is_upper = (c >= L'A') && (c <= L'Z');
      str.string.push_back(is_upper ? c + offset : c);
    }
    else  // Slow path for non-ASCII (uses locale)
    {
      str.string.push_back(wchar_t(std::towupper(std::wint_t(c))));
    }
  }

  return str;
}

//----------------------------------------------------------------------
auto FString::toUpper() const -> FString
{
  FString str;
  str.string.reserve(string.length());
  constexpr wchar_t offset = L'a' - L'A';

  for (wchar_t c : string)
  {
    if ( c < 128 )  // Fast path for ASCII
    {
      const bool is_lower = (c >= L'a') && (c <= L'z');
      str.string.push_back(is_lower ? c - offset : c);
    }
    else  // Slow path for non-ASCII (uses locale)
    {
      str.string.push_back(wchar_t(std::towupper(std::wint_t(c))));
    }
  }

  return str;
}

//----------------------------------------------------------------------
auto FString::toShort() const -> sInt16
{
  const long value = toLong();

  if ( value > SHRT_MAX )
    throw std::overflow_error ("overflow");

  if ( value < SHRT_MIN )
    throw std::underflow_error ("underflow");

  return static_cast<sInt16>(value);
}

//----------------------------------------------------------------------
auto FString::toUShort() const -> uInt16
{
  const uLong value = toULong();

  if ( value > USHRT_MAX )
    throw std::overflow_error ("overflow");

  return static_cast<uInt16>(value);
}

//----------------------------------------------------------------------
auto FString::toInt() const -> int
{
  const long value = toLong();

  if ( value > INT_MAX )
    throw std::overflow_error ("overflow");

  if ( value < INT_MIN )
    throw std::underflow_error ("underflow");

  return static_cast<int>(value);
}

//----------------------------------------------------------------------
auto FString::toUInt() const -> uInt
{
  const uLong value = toULong();

  if ( value > UINT_MAX )
    throw std::overflow_error ("overflow");

  return static_cast<uInt>(value);
}

//----------------------------------------------------------------------
auto FString::toLong() const -> long
{
  // Find actual start and end (skip whitespace)
  const wchar_t* s_ptr{string.data()};
  const wchar_t* end{s_ptr + string.length()};

  internal_skipLeadingWs(s_ptr, end);   // Skip leading whitespace
  internal_skipTrailingWs(s_ptr, end);  // Skip trailing whitespace

  if ( s_ptr == end )
    throw std::invalid_argument("empty value");

  const Sign sign = internal_parseSign(s_ptr);

  if ( s_ptr == end )
    throw std::invalid_argument ("no valid number");

  const long value = internal_parseDigits(s_ptr, end, sign);

  if ( s_ptr != end )
    throw std::invalid_argument("no valid number");

  return sign == Sign::Negative ? (~value) + 1u : value;
}

//----------------------------------------------------------------------
auto FString::toULong() const -> uLong
{
  // Find actual start and end (skip whitespace)
  const wchar_t* s_ptr{string.data()};
  const wchar_t* end{s_ptr + string.length()};

  internal_skipLeadingWs(s_ptr, end);   // Skip leading whitespace
  internal_skipTrailingWs(s_ptr, end);  // Skip trailing whitespace

  if ( s_ptr == end )
    throw std::invalid_argument("empty value");

  if ( internal_parseSign(s_ptr) == Sign::Negative )  // Handle '-' sign
    throw std::underflow_error ("underflow");

  if ( s_ptr == end  )
    throw std::invalid_argument("no valid number");

  const uLong value = internal_parseDigits(s_ptr, end);

  if ( s_ptr == string.data() + ((*string.data() == L'+') ? 1 : 0) )
    throw std::invalid_argument("no valid number");  // No digits parsed

  if ( s_ptr != end )
    throw std::invalid_argument("no valid number");  // Trailing non-digits

  return value;
}

//----------------------------------------------------------------------
auto FString::toFloat() const -> float
{
  const double value{toDouble()};

  if ( value > double(FLT_MAX) || value < double(-FLT_MAX) )
    throw std::overflow_error ("overflow");

  if ( std::fabs(value) < double(FLT_EPSILON) )  // value == 0.0f
    throw std::underflow_error ("underflow");

  return static_cast<float>(value);
}

//----------------------------------------------------------------------
auto FString::toDouble() const -> double
{
  if ( isEmpty() )
    throw std::invalid_argument ("empty value");

  errno = 0;
  wchar_t* p{nullptr};
  const double ret = std::wcstod(string.c_str(), &p);

  if ( p == string.c_str() )
    throw std::invalid_argument("no valid floating point value");

  if ( p != nullptr && *p != L'\0' )
    throw std::invalid_argument ("no valid floating point value");

  if ( errno == ERANGE )
  {
    if ( isExactlyEqual(ret, HUGE_VAL) || isExactlyEqual(ret, -HUGE_VAL) )
      throw std::overflow_error ("overflow");

    if ( std::fabs(ret) < DBL_EPSILON )  // ret == 0.0l
      throw std::underflow_error ("underflow");
  }

  return ret;
}

//----------------------------------------------------------------------
auto FString::ltrim() const -> FString
{
  if ( isEmpty() )  // Handle empty string
    return {};

  // Find last non-whitespace character
  const auto pos = string.find_first_not_of(L" \t\n\r\f\v");

  if ( pos == npos )  // All whitespace
    return {};

  return string.substr(pos);
}

//----------------------------------------------------------------------
auto FString::rtrim() const -> FString
{
  if ( isEmpty() )  // Handle empty string
    return {};

  // Find last non-whitespace character
  const auto pos = string.find_last_not_of(L" \t\n\r\f\v");

  if ( pos == npos )  // All whitespace
    return {};

  return string.substr(0, pos + 1);
}

//----------------------------------------------------------------------
auto FString::trim() const -> FString
{
  if ( isEmpty() )  // Handle empty string
    return {};

  const auto first = string.find_first_not_of(L" \t\n\r\f\v");

  if ( first == npos )  // All whitespace
    return {};

  const auto last = string.find_last_not_of(L" \t\n\r\f\v");
  return string.substr(first, last - first + 1);
}

//----------------------------------------------------------------------
auto FString::left (size_type len) const -> FString
{
  // Handle empty and too long strings
  if ( isEmpty() || len > getLength() )
    return *this;

  return string.substr(0, len);
}

//----------------------------------------------------------------------
auto FString::right (size_type len) const -> FString
{
  // Handle empty and too long strings
  if ( isEmpty() || len > getLength() )
    return *this;

  return string.substr(string.size() - len, len);
}

//----------------------------------------------------------------------
auto FString::mid (size_type pos, size_type len) const -> FString
{
  // Handle empty string
  if ( isEmpty() || len == 0 )
    return {};

  if ( pos == 0 )
    pos = 1;

  const size_type start_index{pos - 1};
  const auto length{string.length()};

  if ( start_index >= length )
    return {};

  FString str{};
  len = std::min(len, length - start_index);
  str.string.reserve (len);
  str.string.assign (string, start_index, len);
  return str;
}

//----------------------------------------------------------------------
auto FString::split (const FString& delimiter) const -> FStringList
{
  if ( isEmpty() )
    return {};

  if ( delimiter.isEmpty() )
    return {*this};

  FStringList string_list{};
  const auto delimiter_length{delimiter.getLength()};
  std::wstring::size_type start{0};
  std::wstring::size_type pos{0};

  // Calculate result size and reserve
  size_type count{1};

  while ( (pos = string.find(delimiter.string, pos)) != npos )
  {
    count++;
    pos += delimiter_length;
  }

  if ( count == 1 )  // 'delimiter' not found
  {
    string_list.emplace_back(std::wstring(string));
    return string_list;
  }

  string_list.reserve(count);

  // Perform split
  while ( (pos = string.find(delimiter.string, start)) != npos )
  {
    string_list.emplace_back(std::wstring(string, start, pos - start));
    start = pos + delimiter_length;
  }

  string_list.emplace_back(std::wstring(string, start));
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
auto FString::setNumber (sInt64 value) -> FString&
{
  std::array<wchar_t, NUMBER_BUFFER_SIZE> buf{};
  wchar_t* s = &buf[NUMBER_BUFFER_LENGTH];  // Pointer to the last character
  const auto is_negative{ bool( value < 0 ) };
  auto abs_value = is_negative ? ~static_cast<uInt64>(value) + 1
                               : static_cast<uInt64>(value);
  *s = '\0';

  do
  {
    *--s = L'0' + wchar_t(abs_value % 10);
    abs_value /= 10;
  }
  while ( abs_value );

  if ( is_negative )
    *--s = '-';

  std::wstring str{s};
  internal_assign (std::move(str));
  return *this;
}

//----------------------------------------------------------------------
auto FString::setNumber (uInt64 value) -> FString&
{
  std::array<wchar_t, NUMBER_BUFFER_SIZE> buf{};
  wchar_t* s = &buf[NUMBER_BUFFER_LENGTH];  // Pointer to the last character
  *s = '\0';

  do
  {
    *--s = L'0' + wchar_t(value % 10);
    value /= 10;
  }
  while ( value );

  std::wstring str{s};
  internal_assign (std::move(str));
  return *this;
}

//----------------------------------------------------------------------
auto FString::setNumber (lDouble f_value, int precision) -> FString&
{
  // The precision can not have more than 2 digits
  precision = std::min(precision, 99);

  std::array<wchar_t, 8> format{};  // = "%.<precision>Lg"
  wchar_t* s = &format[0];
  *s++ = L'%';
  *s++ = L'.';

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

  return sprintf(format.data(), f_value);
}

//----------------------------------------------------------------------
auto FString::setFormatedNumber (sInt64 value, FString separator) -> FString&
{
  int n{0};
  std::array<wchar_t, NUMBER_BUFFER_SIZE> buf{};
  wchar_t* s = &buf[NUMBER_BUFFER_LENGTH];  // Pointer to the last character
  auto abs_value{uInt64(value)};

  if ( separator[0] == 0 )
    separator = L" ";

  if ( value < 0 )
    abs_value = uInt64(-value);

  *s = L'\0';

  do
  {
    *--s = L'0' + wchar_t(abs_value % 10);
    abs_value /= 10;
    n++;

    if ( abs_value && n % 3 == 0 )
      *--s = separator[0];
  }
  while ( abs_value );

  if ( value < 0 )
    *--s = '-';

  std::wstring str{s};
  internal_assign (std::move(str));
  return *this;
}

//----------------------------------------------------------------------
auto FString::setFormatedNumber (uInt64 value, FString separator) -> FString&
{
  int n{0};
  std::array<wchar_t, NUMBER_BUFFER_SIZE> buf{};
  wchar_t* s = &buf[NUMBER_BUFFER_LENGTH];  // Pointer to the last character
  *s = L'\0';

  if ( separator[0] == 0 )
    separator = L" ";

  do
  {
    *--s = L'0' + wchar_t(value % 10);
    value /= 10;
    n++;

    if ( value && n % 3 == 0 )
      *--s = separator[0];
  }
  while ( value );

  std::wstring str{s};
  internal_assign (std::move(str));
  return *this;
}

// FString operators
//----------------------------------------------------------------------
auto FString::insert (const FString& s, int pos) -> const FString&
{
  if ( isNegative(pos) || uInt(pos) > string.length() )
    throw std::out_of_range("FString::insert index out of range");

  string.insert(uInt(pos), s.string, 0, s.getLength());
  return *this;
}

//----------------------------------------------------------------------
auto FString::insert (const FString& s, size_type pos) -> const FString&
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

  // Count occurrences to reserve exact capacity
  size_type count{0};
  std::wstring::size_type pos{0};

  while ( (pos = string.find(from.string, pos)) != npos )
  {
    count++;
    pos += from.getLength();
  }

  if ( count == 0 )  //  String 'from' not found
    return *this;

  // Calculate result size and reserve
  const size_type from_len{from.getLength()};
  const size_type to_len{to.getLength()};
  const size_type new_size{string.length() + count * (to_len - from_len)};

  FString str{};
  str.string.reserve(new_size);
  size_type last_pos{0};
  pos = 0;

  while ( (pos = string.find(from.string, pos)) != npos )
  {
    str.string.append (string, last_pos, pos - last_pos);  // Before
    str.string.append (to.string);  // The string replacement
    pos += from_len;
    last_pos = pos;
  }

  str.string.append (string, last_pos, npos);  // After
  return str;
}

//----------------------------------------------------------------------
auto FString::replaceControlCodes() const -> FString
{
  FString str{};
  str.string.reserve(string.length());

  for (auto c : string)
  {
    if ( c <= L'\x1f' )
    {
      str.string.push_back(c + L'\x2400');
    }
    else if ( c == L'\x7f' )
    {
      str.string.push_back(L'\x2421');
    }
    else if ( (c >= L'\x80' && c <= L'\x9f') || ! isPrintable(c) )
    {
      str.string.push_back(L' ');
    }
    else
      str.string.push_back(c);
  }

  return str;
}

//----------------------------------------------------------------------
auto FString::expandTabs (int tabstop) const -> FString
{
  if (tabstop <= 0)
    return *this;

  FString str{};
  auto tab_stop = size_type(tabstop);
  auto tab_count = size_type(std::count(string.begin(), string.end(), L'\t'));
  str.string.reserve(string.length() + (tab_count * tab_stop));
  size_type column{0};

  for (wchar_t c : string)
  {
    if ( c == L'\t' )
    {
      // Calculate spaces needed to reach next tab stop
      const size_type spaces{tab_stop - (column % tab_stop)};
      str.string.append(spaces, L' ');
      column += spaces;
    }
    else if ( c == L'\n' || c == L'\r' )
    {
      str.string.push_back(c);
      column = 0;  // Reset column on newline
    }
    else
    {
      str.string.push_back(c);
      ++column;
    }
  }

  return str;
}

//----------------------------------------------------------------------
auto FString::removeDel() const -> FString
{
  FString str{};
  str.string.reserve(string.length());
  size_type del_count{0};

  for (const auto c : string)
  {
    if ( c == L'\x7f' )
    {
      del_count++;
    }
    else if ( del_count > 0 )
    {
      del_count--;
    }
    else  // del_count == 0
    {
      str.string.push_back(c);
    }
  }

  return str;
}


//----------------------------------------------------------------------
auto FString::removeBackspaces() const -> FString
{
  FString str{};
  str.string.reserve(string.length());

  for (const auto c : string)
  {
    if ( c != L'\b' )
    {
      str.string.push_back(c);
    }
    else if ( ! str.string.empty() )
      str.string.pop_back();
  }

  return str;
}

//----------------------------------------------------------------------
auto FString::overwrite (const FString& s, int pos) -> FString&
{
  if ( pos < 0 )
    return overwrite (s, 0);

  return overwrite (s, size_type(pos));
}

//----------------------------------------------------------------------
auto FString::overwrite (const FString& s, size_type pos) -> FString&
{
  const auto length{string.length()};

  if ( pos > length )
    pos = length;

  string.replace(pos, s.getLength(), s.string);
  return *this;
}

//----------------------------------------------------------------------
auto FString::remove (size_type pos, size_type len) -> FString&
{
  const auto length{string.length()};

  if ( pos > length )
    return *this;

  if ( pos + len > length )
    len = length - pos;

  string.erase (pos, len);
  return *this;
}

//----------------------------------------------------------------------
auto FString::erase (size_type pos, size_type len) -> FString&
{
  string.erase (pos, len);
  return *this;
}

//----------------------------------------------------------------------
auto FString::includes (const FString& s) const noexcept -> bool
{
  if ( s.isEmpty() )
    return false;

  return string.find(s.string) != npos;
}

//----------------------------------------------------------------------
auto FString::contains (const FString& s) const noexcept -> bool
{
  return includes(s);
}

//----------------------------------------------------------------------
auto FString::find (const FString& s, size_type pos) const noexcept -> size_type
{
  return string.find(s.string, pos);
}

//----------------------------------------------------------------------
auto FString::rfind (const FString& s, size_type pos) const noexcept -> size_type
{
  return string.rfind(s.string, pos);
}


// private methods of FString
//----------------------------------------------------------------------
auto FString::internal_toCharString (const std::wstring& s) const -> std::string
{
  if ( s.empty() )
    return {};

  auto state{std::mbstate_t()};
  const wchar_t* src = s.c_str();
  const auto size = std::wcsrtombs(nullptr, &src, 0, &state);

  if ( size == MALFORMED_STRING )
    return {};

  std::string dest{};
  dest.resize(size);
  auto dest_data = const_cast<char*>(dest.data());
  const auto mblength = std::wcsrtombs (dest_data, &src, size + 1, &state);

  if ( mblength == MALFORMED_STRING && errno != EILSEQ )
    return {};

  return dest;
}

//----------------------------------------------------------------------
auto FString::internal_toWideString (const char src[]) const -> std::wstring
{
  if ( ! src || *src == '\0' )
    return {};

  auto state{std::mbstate_t()};
  auto size = std::mbsrtowcs(nullptr, &src, 0, &state);

  if ( size == MALFORMED_STRING )
    return {};

  std::wstring dest(size, L'\0');
  auto dest_data = const_cast<wchar_t*>(dest.data());
  const auto wide_length = std::mbsrtowcs (dest_data, &src, size + 1, &state);

  if ( wide_length == MALFORMED_STRING )
    return {};

  return dest;
}

//----------------------------------------------------------------------
inline void FString::internal_skipLeadingWs ( const wchar_t*& p
                                            , const wchar_t* end ) const noexcept
{
  // Skip over leading whitespace in a string

  while ( p != end && std::iswspace(static_cast<std::wint_t>(*p)) )
    ++p;
}

//----------------------------------------------------------------------
inline void FString::internal_skipTrailingWs ( const wchar_t* begin
                                             , const wchar_t*& p ) const noexcept
{
  // Skip over trailing whitespace in a string

  while ( p > begin && std::iswspace(static_cast<std::wint_t>(*(p - 1))) )
    --p;
}

//----------------------------------------------------------------------
inline auto FString::internal_parseSign (const wchar_t*& s) const noexcept -> Sign
{
  // Extract sign and increment pointer

  if ( *s == L'-' )  // Handle '-' sign
  {
    ++s;
    return Sign::Negative;
  }

  if ( *s == L'+' )  // Handle '+' sign
    ++s;

  return Sign::Positive;
}

//----------------------------------------------------------------------
inline auto FString::internal_parseDigits ( const wchar_t*& s_ptr
                                          , const wchar_t* end
                                          , Sign sign ) const -> long
{
  // Parse digits and detect overflow/underflow

  long value{0};

  while ( s_ptr != end )
  {
    const wchar_t c{*s_ptr};

    if ( c < L'0' || c > L'9' )  // Digit check
      break;

    const auto digit{long(c - L'0')};

    if ( internal_isOverflowed(sign, value, digit) )
    {
      if ( sign == Sign::Negative )
        throw std::underflow_error("underflow");

      throw std::overflow_error("overflow");
    }

    value = (value * 10) + digit;
    ++s_ptr;
  }

  return value;
}

//----------------------------------------------------------------------
inline auto FString::internal_parseDigits ( const wchar_t*& s_ptr
                                          , const wchar_t* end ) const -> uLong
{
  // Parse digits and detect overflow/underflow

  uLong value{0};

  while ( s_ptr != end )
  {
    const wchar_t c{*s_ptr};

    if ( c < L'0' || c > L'9' )  // Digit check
      break;

    const auto digit{uLong(c - L'0')};

    if ( internal_isOverflowed(value, digit) )
      throw std::overflow_error("overflow");

    value = (value * 10) + digit;
    ++s_ptr;
  }

  return value;
}

//----------------------------------------------------------------------
inline auto FString::internal_isOverflowed ( Sign sign
                                           , long value
                                           , long digit ) const noexcept -> bool
{
  const long limit_digit = ( sign == Sign::Negative ) ? LONG_MIN_LIMIT_DIGIT
                                                      : LONG_MAX_LIMIT_DIGIT;
  return ( value > LONG_LIMIT )
      || ( value == LONG_LIMIT && digit > limit_digit );
}

//----------------------------------------------------------------------
inline auto FString::internal_isOverflowed ( uLong value
                                           , uLong digit ) const noexcept -> bool
{
  return ( value > ULONG_LIMIT )
      || ( value == ULONG_LIMIT && digit > ULONG_LIMIT_DIGIT );
}


// FString non-member operators
//----------------------------------------------------------------------
auto FStringCaseCompare (const FString& s1, const FString& s2) -> int
{
  if ( &s1 == &s2 )
    return 0;

  auto iter1 = s1.cbegin();
  auto iter2 = s2.cbegin();
  const auto end1 = s1.cend();
  const auto end2 = s2.cend();

  while ( iter1 != end1 && iter2 != end2 )
  {
    auto c1 = *iter1;
    auto c2 = *iter2;

    // Convert to lowercase
    if ( c1 >= L'A' && c1 <= L'Z' )
      c1 += 32;

    if ( c2 >= L'A' && c2 <= L'Z' )
      c2 += 32;

    if ( c1 != c2 )
      return c1 - c2;

    ++iter1;
    ++iter2;
  }

  // Handle remaining characters
  if ( iter1 != end1 )
    return 1;  // s1 is longer

  if ( iter2 != end2 )
    return -1; // s2 is longer

  return 0;    // Equal
}


}  // namespace finalcut
