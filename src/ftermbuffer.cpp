/***********************************************************************
* ftermbuffer.cpp - Buffer for virtual terminal strings                *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2017-2019 Markus Gans                                      *
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

#include <string>
#include <vector>

#include "final/fc.h"
#include "final/fcolorpair.h"
#include "final/fstring.h"
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
const FString FTermBuffer::toString() const
{
  std::wstring wide_string{};
  wide_string.reserve(data.size());

  for (auto&& tc : data)
    wide_string.push_back(tc.code);

  return FString(wide_string);
}

//----------------------------------------------------------------------
int FTermBuffer::write (const FString& string)
{
  assert ( ! string.isNull() );
  int len = int(string.getLength());

  for (auto&& c : string)
  {
    charData  nc;  // next character
    nc = FVTerm::getAttribute();
    nc.code = c;
    getColumnWidth(nc);  // add column width
    nc.attr.bit.no_changes = false;
    nc.attr.bit.printed = false;
    data.push_back(nc);
  }

  return len;
}

//----------------------------------------------------------------------
int FTermBuffer::write (wchar_t ch)
{
  charData nc = FVTerm::getAttribute();  // next character
  nc.code = ch;
  getColumnWidth(nc);  // add column width
  nc.attr.bit.no_changes = false;
  nc.attr.bit.printed = false;

  data.push_back(nc);
  return 1;
}

//----------------------------------------------------------------------
void FTermBuffer::write (const FColorPair& pair)
{
  FVTerm::setColor(pair.getForegroundColor(), pair.getBackgroundColor());
}


// FTermBuffer non-member operators
//----------------------------------------------------------------------
FTermBuffer::charDataVector& operator << ( FTermBuffer::charDataVector& termString
                                         , const FTermBuffer& buf )
{
  if ( ! buf.data.empty() )
    termString.assign(buf.data.begin(), buf.data.end());

  return termString;
}

}  // namespace finalcut
