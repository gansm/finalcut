/***********************************************************************
* fvtermbuffer.cpp - Buffer for virtual terminal strings               *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2017-2023 Markus Gans                                      *
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
#include <vector>

#include "final/fc.h"
#include "final/ftypes.h"
#include "final/output/foutput.h"
#include "final/output/tty/ftermdata.h"
#include "final/util/fstring.h"
#include "final/vterm/fcolorpair.h"
#include "final/vterm/fstyle.h"
#include "final/vterm/fvtermbuffer.h"
#include "final/vterm/fvterm.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FVTermBuffer
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FVTermBuffer::FVTermBuffer()  // constructor
{
  data.reserve(2048);  // Set initial data size to 2048 FChars
}

// public methods of FVTermBuffer
//----------------------------------------------------------------------
auto FVTermBuffer::toString() const -> FString
{
  std::wstring wide_string{};
  checkCapacity(wide_string, data.size());
  std::for_each ( data.cbegin()
                , data.cend()
                , [&wide_string] (const auto& fchar)
                  {
                    for (auto&& ch : fchar.ch)
                    {
                      if ( ch == L'\0' )
                        return;

                      wide_string.push_back(ch);
                    }
                  }
                );
  return FString(std::move(wide_string));
}

//----------------------------------------------------------------------
auto FVTermBuffer::print (const FString& string) -> int
{
  checkCapacity(data, data.size() + string.getLength());
  getNextCharacterAttribute();
  const auto last = string.cend();
  auto cbegin = string.cbegin();
  auto iter = cbegin;
  int char_width{0};

  for (auto&& ch : string)
  {
    auto width = getColumnWidth(ch);
    auto ctrl_char = std::iswcntrl(wint_t(ch));

    if ( width == 0 && ! ctrl_char )  // zero-width character
    {
      if ( iter == cbegin )
        ++cbegin;

      ++iter;
    }
    else if ( iter != cbegin )
      add(cbegin, iter, char_width);

    if ( iter == cbegin && (width > 0 || is7bit(ch)) )  // 1st char
      ++iter;

    if ( width > 0 )
      char_width += width;

    if ( ctrl_char )
      add(cbegin, iter, char_width);
  }

  if ( iter == last )
    add(cbegin, iter, char_width);

  return int(string.getLength());
}

//----------------------------------------------------------------------
auto FVTermBuffer::print (wchar_t ch) -> int
{
  getNextCharacterAttribute();
  nc.ch[0] = ch;
  nc.ch[1] = L'\0';
  const auto column_width = getColumnWidth(nc.ch[0]);
  addColumnWidth(nc, column_width);  // add column width
  data.emplace_back(nc);
  return 1;
}

//----------------------------------------------------------------------
void FVTermBuffer::print (const FStyle& style) const
{
  FVTermAttribute::print(style);
}

//----------------------------------------------------------------------
void FVTermBuffer::print (const FColorPair& pair) const
{
  FVTermAttribute::setColor( pair.getForegroundColor()
                           , pair.getBackgroundColor() );
}


// private methods of FVTermBuffer
//----------------------------------------------------------------------
inline void FVTermBuffer::getNextCharacterAttribute()
{
  static const auto& next_attribute = FVTermAttribute::getAttribute();
  nc.fg_color     = next_attribute.fg_color;
  nc.bg_color     = next_attribute.bg_color;
  nc.attr.byte[0] = next_attribute.attr.byte[0];
  nc.attr.byte[1] = next_attribute.attr.byte[1];
  nc.attr.byte[2] = 0;
  nc.attr.byte[3] = 0;
}

//----------------------------------------------------------------------
void FVTermBuffer::add ( FString::const_iterator& cbegin
                       , const FString::const_iterator& cend
                       , int& char_width )
{
  static const auto& fterm_data = FTermData::getInstance();

  if ( cbegin == cend )
    return;

  if ( char_width == 2
    && fterm_data.getTerminalEncoding() != Encoding::UTF8 )
  {
    nc.ch[0] = L'.';
    nc.ch[1] = L'\0';
    nc.attr.bit.char_width = 1;
  }
  else
  {
    const auto end = std::min(cend, cbegin + UNICODE_MAX);
    std::copy(cbegin, end, nc.ch.begin());
    nc.attr.bit.char_width = uInt8(char_width) & 0x03;
    const auto idx = std::size_t(end - cbegin);

    if ( idx < UNICODE_MAX )
      nc.ch[idx] = L'\0';
  }

  data.emplace_back(nc);
  cbegin = cend;
  char_width = 0;  // reset char width
}


// FVTermBuffer non-member operators
//----------------------------------------------------------------------
auto operator << ( FVTermBuffer::FCharVector& term_string
                 , const FVTermBuffer& buf ) -> FVTermBuffer::FCharVector&
{
  if ( ! buf.data.empty() )
    term_string.assign(buf.data.cbegin(), buf.data.cend());

  return term_string;
}

}  // namespace finalcut
