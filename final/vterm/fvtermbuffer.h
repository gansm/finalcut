/***********************************************************************
* fvtermbuffer.h - Buffer for virtual terminal strings                 *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2017-2022 Markus Gans                                      *
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

    // Constructor
    FVTermBuffer();

    template <typename Iterator>
    FVTermBuffer (Iterator, Iterator);

    // Overloaded operators
    inline auto operator [] (std::size_t) -> reference;
    inline auto operator [] (const std::size_t) const -> const_reference;

    template <typename NumT
            , enable_if_arithmetic_without_char_t<NumT> = nullptr>
    inline auto operator << (const NumT&) -> FVTermBuffer&;

    template <typename CharT
            , enable_if_CString_t<CharT> = nullptr>
    inline auto operator << (const CharT&) -> FVTermBuffer&;
    inline auto operator << (char) -> FVTermBuffer&;
    inline auto operator << (wchar_t) -> FVTermBuffer&;
    inline auto operator << (const wchar_t*) -> FVTermBuffer&;
    inline auto operator << (const UniChar&) -> FVTermBuffer&;
    inline auto operator << (const std::string&) -> FVTermBuffer&;
    inline auto operator << (const std::wstring&) -> FVTermBuffer&;
    inline auto operator << (const FString&) -> FVTermBuffer&;
    inline auto operator << (FChar&) -> FVTermBuffer&;
    inline auto operator << (const FCharVector&) -> FVTermBuffer&;
    inline auto operator << (const FStyle&) -> FVTermBuffer&;
    inline auto operator << (const FColorPair&) -> FVTermBuffer&;

    // Accessors
    auto getClassName() const -> FString;
    inline auto getLength() const -> std::size_t;
    inline auto getBuffer() const -> const FCharVector&;

    // Inquiry
    inline auto isEmpty() const -> bool;

    // Methods
    inline auto begin() -> iterator;
    inline auto end() -> iterator;
    inline auto begin() const -> const_iterator;
    inline auto end() const -> const_iterator;
    inline auto front() -> reference;
    inline auto back() -> reference;
    inline auto front() const -> const_reference;
    inline auto back() const -> const_reference;
    auto toString() const -> FString;
    inline void clear();
    template <typename... Args>
    inline auto printf (const FString&, Args&&...) -> int;
    auto print (const FString&) -> int;
    auto print (wchar_t) -> int;
    void print (const FStyle&) const;
    void print (const FColorPair&) const;
    inline auto print () -> FVTermBuffer&;

  private:
    inline void checkCapacity (std::size_t);
    void add ( FString::const_iterator&
             , const FString::const_iterator&
             , int& );

    // Data member
    FCharVector data{};

    // Non-member operators
    friend auto operator << ( FCharVector&
                            , const FVTermBuffer& ) -> FCharVector&;
};


// FVTermBuffer inline functions
//----------------------------------------------------------------------
template <typename Iterator>
inline FVTermBuffer::FVTermBuffer(Iterator first, Iterator last)
{
  assert ( first < last );
  checkCapacity (std::size_t(last - first));
  data.assign(first, last);
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
inline void FVTermBuffer::clear()
{
  data.clear();
  data.shrink_to_fit();
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
void FVTermBuffer::checkCapacity (std::size_t size)
{
  if ( size <= data.capacity() )
    return;

  const auto new_size = [&size] ()
  {
    return std::size_t(std::pow(2, std::ceil(std::log(size) / std::log(2.0))));
  }();

  data.reserve(new_size);
}

}  // namespace finalcut

#endif  // FVTERMBUFFER_H
