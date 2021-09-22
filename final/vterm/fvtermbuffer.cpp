/***********************************************************************
* fvtermbuffer.cpp - Buffer for virtual terminal strings               *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2017-2021 Markus Gans                                      *
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
FVTermBuffer::~FVTermBuffer() noexcept = default;  // destructor


// public methods of FVTermBuffer
//----------------------------------------------------------------------
FString FVTermBuffer::toString() const
{
  std::wstring wide_string{};
  wide_string.reserve(data.size());
  std::for_each ( data.begin()
                , data.end()
                , [&wide_string] (const FChar& fchar)
                  {
                    for (auto&& ch : fchar.ch)
                    {
                      if ( ch == L'\0' )
                        return;
                      else
                        wide_string.push_back(ch);
                    }
                  }
                );
  return wide_string;
}

//----------------------------------------------------------------------
int FVTermBuffer::write (const FString& string)
{
  data.reserve(data.size() + string.getLength());
  const auto last = string.end();
  auto begin = string.begin();
  auto iter = begin;
  int char_width{0};

  for (auto&& ch : string)
  {
    auto width = getColumnWidth(ch);
    auto wspace = std::iswspace(wint_t(ch));

    if ( width == 0 && ! wspace )  // zero-width character
    {
      if ( iter == begin)
        ++begin;

      ++iter;
    }
    else if ( iter != begin )
      add(begin, iter, char_width);

    if ( iter == begin && (width > 0 || is7bit(ch)) )  // 1st char
      ++iter;

    if ( width > 0 )
      char_width += width;

    if ( wspace )
      add(begin, iter, char_width);
  }

  if ( iter == last )
    add(begin, iter, char_width);

  return int(string.getLength());
}

//----------------------------------------------------------------------
int FVTermBuffer::write (wchar_t ch)
{
  FChar nc{FVTerm::getAttribute()};  // next character
  nc.ch[0] = ch;
  addColumnWidth(nc);  // add column width
  nc.attr.bit.no_changes = false;
  nc.attr.bit.printed = false;
  data.emplace_back(nc);
  return 1;
}

//----------------------------------------------------------------------
void FVTermBuffer::write (const FStyle& style) const
{
  Style attr = style.getStyle();

  if ( attr == Style::None )
    FVTerm::setNormal();
  else
  {
    if ( (attr & Style::Bold) != Style::None ) FVTerm::setBold();
    if ( (attr & Style::Dim) != Style::None ) FVTerm::setDim();
    if ( (attr & Style::Italic) != Style::None ) FVTerm::setItalic();
    if ( (attr & Style::Underline) != Style::None ) FVTerm::setUnderline();
    if ( (attr & Style::Blink) != Style::None ) FVTerm::setBlink();
    if ( (attr & Style::Reverse) != Style::None ) FVTerm::setReverse();
    if ( (attr & Style::Standout) != Style::None ) FVTerm::setStandout();
    if ( (attr & Style::Invisible) != Style::None ) FVTerm::setInvisible();
    if ( (attr & Style::Protected) != Style::None ) FVTerm::setProtected();
    if ( (attr & Style::CrossedOut) != Style::None ) FVTerm::setCrossedOut();
    if ( (attr & Style::DoubleUnderline) != Style::None ) FVTerm::setDoubleUnderline();
    if ( (attr & Style::Transparent) != Style::None ) FVTerm::setTransparent();
    if ( (attr & Style::ColorOverlay) != Style::None ) FVTerm::setColorOverlay();
    if ( (attr & Style::InheritBackground) != Style::None ) FVTerm::setInheritBackground();
  }
}

//----------------------------------------------------------------------
void FVTermBuffer::write (const FColorPair& pair) const
{
  FVTerm::setColor(pair.getForegroundColor(), pair.getBackgroundColor());
}


// private methods of FVTermBuffer
//----------------------------------------------------------------------
void FVTermBuffer::add ( FString::const_iterator& begin
                       , const FString::const_iterator& end
                       , int& char_width )
{
  static const auto& fterm_data = FTermData::getInstance();

  if ( begin == end )
    return;

  FChar nc{FVTerm::getAttribute()};  // next character
  nc.attr.byte[2] = 0;
  nc.attr.byte[3] = 0;


  if ( char_width == 2
    && fterm_data.getTermEncoding() != Encoding::UTF8 )
  {
    nc.ch[0] = '.';
    nc.attr.bit.char_width = 1;
  }
  else
    nc.attr.bit.char_width = char_width & 0x03;

  std::copy(begin, std::min(end, begin + UNICODE_MAX), nc.ch.begin());
  data.emplace_back(nc);
  begin = end;
  char_width = 0;
}


// FVTermBuffer non-member operators
//----------------------------------------------------------------------
FVTermBuffer::FCharVector& operator << ( FVTermBuffer::FCharVector& term_string
                                       , const FVTermBuffer& buf )
{
  if ( ! buf.data.empty() )
    term_string.assign(buf.data.begin(), buf.data.end());

  return term_string;
}

}  // namespace finalcut
