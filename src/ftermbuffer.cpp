/***********************************************************************
* ftermbuffer.cpp - Buffer for virtual terminal strings                *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2017-2020 Markus Gans                                      *
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
#include "final/fcolorpair.h"
#include "final/fstring.h"
#include "final/fstyle.h"
#include "final/ftermbuffer.h"
#include "final/fvterm.h"
#include "final/ftypes.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FTermBuffer
//----------------------------------------------------------------------
FTermBuffer::~FTermBuffer() noexcept = default;  // destructor


// public methods of FTermBuffer
//----------------------------------------------------------------------
FString FTermBuffer::toString() const
{
  std::wstring wide_string{};
  wide_string.reserve(data.size());
  std::transform ( data.begin()
                 , data.end()
                 , std::back_inserter(wide_string)
                 , [] (const FChar& fchar)
                   {
                     return fchar.ch[0];
                   }
                 );
  return wide_string;
}

//----------------------------------------------------------------------
int FTermBuffer::write (const FString& string)
{
  assert ( ! string.isNull() );
  const auto len = int(string.getLength());

  for (auto&& c : string)
  {
    FChar nc{FVTerm::getAttribute()};  // next character
    nc.ch[0] = c;
    nc.attr.byte[2] = 0;
    nc.attr.byte[3] = 0;
    getColumnWidth(nc);  // add column width
    data.emplace_back(nc);
  }

  return len;
}

//----------------------------------------------------------------------
int FTermBuffer::write (wchar_t ch)
{
  FChar nc{FVTerm::getAttribute()};  // next character
  nc.ch[0] = ch;
  getColumnWidth(nc);  // add column width
  nc.attr.bit.no_changes = false;
  nc.attr.bit.printed = false;
  data.emplace_back(nc);
  return 1;
}

//----------------------------------------------------------------------
void FTermBuffer::write (const FStyle& style) const
{
  Style attr = style.getStyle();

  if ( attr == Style::None )
    FVTerm::setNormal();
  else if ( (attr & Style::Bold) != Style::None )
    FVTerm::setBold();
  else if ( (attr & Style::Dim) != Style::None )
    FVTerm::setDim();
  else if ( (attr & Style::Italic) != Style::None )
    FVTerm::setItalic();
  else if ( (attr & Style::Underline) != Style::None )
    FVTerm::setUnderline();
  else if ( (attr & Style::Blink) != Style::None )
    FVTerm::setBlink();
  else if ( (attr & Style::Reverse) != Style::None )
    FVTerm::setReverse();
  else if ( (attr & Style::Standout) != Style::None )
    FVTerm::setStandout();
  else if ( (attr & Style::Invisible) != Style::None )
    FVTerm::setInvisible();
  else if ( (attr & Style::Protected) != Style::None )
    FVTerm::setProtected();
  else if ( (attr & Style::CrossedOut) != Style::None )
    FVTerm::setCrossedOut();
  else if ( (attr & Style::DoubleUnderline) != Style::None )
    FVTerm::setDoubleUnderline();
  else if ( (attr & Style::Transparent) != Style::None )
    FVTerm::setTransparent();
  else if ( (attr & Style::ColorOverlay) != Style::None )
    FVTerm::setColorOverlay();
  else if ( (attr & Style::InheritBackground) != Style::None )
    FVTerm::setInheritBackground();
}

//----------------------------------------------------------------------
void FTermBuffer::write (const FColorPair& pair) const
{
  FVTerm::setColor(pair.getForegroundColor(), pair.getBackgroundColor());
}


// FTermBuffer non-member operators
//----------------------------------------------------------------------
FTermBuffer::FCharVector& operator << ( FTermBuffer::FCharVector& termString
                                      , const FTermBuffer& buf )
{
  if ( ! buf.data.empty() )
    termString.assign(buf.data.begin(), buf.data.end());

  return termString;
}

}  // namespace finalcut
