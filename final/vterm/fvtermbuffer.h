/***********************************************************************
* fvtermbuffer.h - Buffer for virtual terminal strings                 *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2017-2024 Markus Gans                                      *
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
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FVTermBuffer ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FVTERMBUFFER_H
#define FVTERMBUFFER_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "final/util/fstringstream.h"

namespace finalcut
{

// class forward declaration
class FStyle;
class FColorPair;

//----------------------------------------------------------------------
// class FVTermBuffer
//----------------------------------------------------------------------

class FVTermBuffer
{
  public:
    // Using-declarations
    using FCharVector     = std::vector<FChar>;
    using iterator        = FCharVector::iterator;
    using const_iterator  = FCharVector::const_iterator;
    using reference       = FCharVector::reference;
    using const_reference = FCharVector::const_reference;
    using difference_type = FCharVector::difference_type;

    // Constructor
    FVTermBuffer();

    template <typename Iterator>
    FVTermBuffer (Iterator, Iterator);

    // Overloaded operators
    auto operator [] (std::size_t) -> reference;
    auto operator [] (const std::size_t) const -> const_reference;

    template <typename NumT
            , enable_if_arithmetic_without_char_t<NumT> = nullptr>
    auto operator << (const NumT&) -> FVTermBuffer&;

    template <typename CharT
            , enable_if_CString_t<CharT> = nullptr>
    auto operator << (const CharT&) -> FVTermBuffer&;
    auto operator << (char) -> FVTermBuffer&;
    auto operator << (wchar_t) -> FVTermBuffer&;
    auto operator << (const wchar_t*) -> FVTermBuffer&;
    auto operator << (const UniChar&) -> FVTermBuffer&;
    auto operator << (const std::string&) -> FVTermBuffer&;
    auto operator << (const std::wstring&) -> FVTermBuffer&;
    auto operator << (const FString&) -> FVTermBuffer&;
    auto operator << (FChar&) -> FVTermBuffer&;
    auto operator << (const FCharVector&) -> FVTermBuffer&;
    auto operator << (const FStyle&) -> FVTermBuffer&;
    auto operator << (const FColorPair&) -> FVTermBuffer&;

    // Accessors
    auto getClassName() const -> FString;
    auto getLength() const -> std::size_t;
    auto getBuffer() const -> const FCharVector&;

    // Inquiry
    auto isEmpty() const -> bool;

    // Methods
    auto begin() -> iterator;
    auto end() -> iterator;
    auto begin() const -> const_iterator;
    auto end() const -> const_iterator;
    auto front() -> reference;
    auto back() -> reference;
    auto front() const -> const_reference;
    auto back() const -> const_reference;
    auto toString() const -> FString;
    template <typename Iterator>
    void assign (Iterator, Iterator);
    void clear();
    template <typename... Args>
    auto printf (const FString&, Args&&...) -> int;
    auto print (const FString&) -> int;
    auto print (wchar_t) -> int;
    void print (const FStyle&) const;
    void print (const FColorPair&) const;
    auto print () -> FVTermBuffer&;

  private:
    struct UnicodeBoundary
    {
      FString::const_iterator cbegin{};
      FString::const_iterator cend{};
      FString::const_iterator iter{};
      std::size_t char_width{0};
    };

    void getNextCharacterAttribute();
    void add (UnicodeBoundary&);

    // Data member
    FCharVector data{};
    FChar       nc{};  // next character

    // Non-member operators
    friend auto operator << ( FVTermBuffer::FCharVector& term_string
                            , const FVTermBuffer& buf ) -> FVTermBuffer::FCharVector&
    {
      if ( ! buf.data.empty() )
        term_string.assign(buf.data.cbegin(), buf.data.cend());

      return term_string;
    }
};

// non-member function forward declarations
template<typename T>
constexpr void checkCapacity (T&, std::size_t) noexcept;

// FVTermBuffer inline functions
//----------------------------------------------------------------------
template <typename Iterator>
inline FVTermBuffer::FVTermBuffer (Iterator first, Iterator last)
{
  assign(first, last);
}

//----------------------------------------------------------------------
inline auto FVTermBuffer::operator [] (std::size_t index) -> reference
{
  return data[index];
}

//----------------------------------------------------------------------
inline auto FVTermBuffer::operator [] (const std::size_t index) const -> const_reference
{
  return data[index];
}

//----------------------------------------------------------------------
template <typename NumT
        , enable_if_arithmetic_without_char_t<NumT>>
inline auto FVTermBuffer::operator << (const NumT& n) -> FVTermBuffer&
{
  print (FString(std::to_string(n)));
  return *this;
}

//----------------------------------------------------------------------
template <typename CharT
        , enable_if_CString_t<CharT>>
inline auto FVTermBuffer::operator << (const CharT& s) -> FVTermBuffer&
{
  print (FString(s));
  return *this;
}

//----------------------------------------------------------------------
inline auto FVTermBuffer::operator << (char c) -> FVTermBuffer&
{
  print (wchar_t(uChar(c)));
  return *this;
}

//----------------------------------------------------------------------
inline auto FVTermBuffer::operator << (wchar_t c) -> FVTermBuffer&
{
  print (c);
  return *this;
}

//----------------------------------------------------------------------
inline auto FVTermBuffer::operator << (const wchar_t* wide_string) -> FVTermBuffer&
{
  print (FString(wide_string));
  return *this;
}

//----------------------------------------------------------------------
inline auto FVTermBuffer::operator << (const UniChar& c) -> FVTermBuffer&
{
  print (static_cast<wchar_t>(c));
  return *this;
}

//----------------------------------------------------------------------
inline auto FVTermBuffer::operator << (const std::string& string) -> FVTermBuffer&
{
  print (FString(string));
  return *this;
}

//----------------------------------------------------------------------
inline auto FVTermBuffer::operator << (const std::wstring& wstring) -> FVTermBuffer&
{
  print (FString(wstring));
  return *this;
}

//----------------------------------------------------------------------
inline auto FVTermBuffer::operator << (const FString& fstring) -> FVTermBuffer&
{
  print (fstring);
  return *this;
}

//----------------------------------------------------------------------
inline auto FVTermBuffer::operator << (FChar& fchar) -> FVTermBuffer&
{
  data.emplace_back(fchar);
  return *this;
}

//----------------------------------------------------------------------
inline auto FVTermBuffer::operator << (const FCharVector& vec) -> FVTermBuffer&
{
  std::copy(vec.cbegin(), vec.cend(), std::back_inserter(data));
  return *this;
}

//----------------------------------------------------------------------
inline auto FVTermBuffer::operator << (const FStyle& style) -> FVTermBuffer&
{
  print (style);
  return *this;
}

//----------------------------------------------------------------------
inline auto FVTermBuffer::operator << (const FColorPair& pair) -> FVTermBuffer&
{
  print (pair);
  return *this;
}

//----------------------------------------------------------------------
inline auto FVTermBuffer::getClassName() const -> FString
{ return "FVTermBuffer"; }

//----------------------------------------------------------------------
inline auto FVTermBuffer::getLength() const -> std::size_t
{ return data.size(); }

//----------------------------------------------------------------------
inline auto FVTermBuffer::getBuffer() const -> const FCharVector&
{ return data; }

//----------------------------------------------------------------------
inline auto FVTermBuffer::isEmpty() const -> bool
{ return data.empty(); }

//----------------------------------------------------------------------
inline auto FVTermBuffer::begin() -> iterator
{ return data.begin(); }

//----------------------------------------------------------------------
inline auto FVTermBuffer::end() -> iterator
{ return data.end(); }

//----------------------------------------------------------------------
inline auto FVTermBuffer::begin() const -> const_iterator
{ return data.begin(); }

//----------------------------------------------------------------------
inline auto FVTermBuffer::end() const -> const_iterator
{ return data.end(); }

//----------------------------------------------------------------------
inline auto FVTermBuffer::front() -> reference
{ return data.front(); }

//----------------------------------------------------------------------
inline auto FVTermBuffer::back() -> reference
{ return data.back(); }

//----------------------------------------------------------------------
inline auto FVTermBuffer::front() const -> const_reference
{ return data.front(); }

//----------------------------------------------------------------------
inline auto FVTermBuffer::back() const -> const_reference
{ return data.back(); }

//----------------------------------------------------------------------
template <typename Iterator>
inline void FVTermBuffer::assign (Iterator first, Iterator last)
{
  if ( first >= last )
    return;

  checkCapacity (data, std::size_t(last - first));
  data.assign(first, last);
}

//----------------------------------------------------------------------
inline void FVTermBuffer::clear()
{
  data.clear();
}

//----------------------------------------------------------------------
template <typename... Args>
inline auto FVTermBuffer::printf (const FString& format, Args&&... args) -> int
{
  FString str{};
  str.sprintf (format, std::forward<Args>(args)...);
  return print(str);
}

//----------------------------------------------------------------------
inline auto FVTermBuffer::print() -> FVTermBuffer&
{ return *this; }

//----------------------------------------------------------------------
template<typename T>
constexpr void checkCapacity (T& buffer, std::size_t size) noexcept
{
  if ( size <= buffer.capacity() )
    return;

  const auto new_size = [&size] ()
  {
    return std::size_t(std::pow(2, std::ceil(std::log(size) / std::log(2.0))));
  }();

  buffer.reserve(new_size);
}

}  // namespace finalcut

#endif  // FVTERMBUFFER_H
