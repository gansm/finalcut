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
FTermBuffer::~FTermBuffer()  // destructor
{ }

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
    FChar nc;  // next character
    nc = FVTerm::getAttribute();
    nc.ch[0] = c;
    nc.attr.byte[2] = 0;
    nc.attr.byte[3] = 0;
    getColumnWidth(nc);  // add column width
    data.push_back(std::move(nc));
  }

  return len;
}

//----------------------------------------------------------------------
int FTermBuffer::write (wchar_t ch)
{
  FChar nc = FVTerm::getAttribute();  // next character
  nc.ch[0] = ch;
  getColumnWidth(nc);  // add column width
  nc.attr.bit.no_changes = false;
  nc.attr.bit.printed = false;

  data.push_back(nc);
  return 1;
}

//----------------------------------------------------------------------
void FTermBuffer::write (const FStyle& style) const
{
  FAttribute attr = style.getStyle();

  if ( attr == 0 )
    FVTerm::setNormal();
  else if ( (attr & fc::Bold) != 0 )
    FVTerm::setBold();
  else if ( (attr & fc::Dim) != 0 )
    FVTerm::setDim();
  else if ( (attr & fc::Italic) != 0 )
    FVTerm::setItalic();
  else if ( (attr & fc::Underline) != 0 )
    FVTerm::setUnderline();
  else if ( (attr & fc::Blink) != 0 )
    FVTerm::setBlink();
  else if ( (attr & fc::Reverse) != 0 )
    FVTerm::setReverse();
  else if ( (attr & fc::Standout) != 0 )
    FVTerm::setStandout();
  else if ( (attr & fc::Invisible) != 0 )
    FVTerm::setInvisible();
  else if ( (attr & fc::Protected) != 0 )
    FVTerm::setProtected();
  else if ( (attr & fc::CrossedOut) != 0 )
    FVTerm::setCrossedOut();
  else if ( (attr & fc::DoubleUnderline) != 0 )
    FVTerm::setDoubleUnderline();
  else if ( (attr & fc::Transparent) != 0 )
    FVTerm::setTransparent();
  else if ( (attr & fc::ColorOverlay) != 0 )
    FVTerm::setColorOverlay();
  else if ( (attr & fc::InheritBackground) != 0 )
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
