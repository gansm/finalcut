/***********************************************************************
* ftermbuffer.cpp - Buffer for virtual terminal strings                *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2017-2018 Markus Gans                                      *
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

#include "final/ftermbuffer.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FTermBuffer
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FTermBuffer::FTermBuffer()
  : data()
{ }

//----------------------------------------------------------------------
FTermBuffer::~FTermBuffer()  // destructor
{ }


// public methods of FTermBuffer
//----------------------------------------------------------------------
int FTermBuffer::writef (const FString format, ...)
{
  static const int BUFSIZE = 4096;
  wchar_t buffer[BUFSIZE];
  va_list args;

  if ( format.isEmpty() )
    return 0;

  va_start (args, format);
  std::vswprintf (buffer, BUFSIZE, format.wc_str(), args);
  va_end (args);

  FString str(buffer);
  return write(str);
}

//----------------------------------------------------------------------
int FTermBuffer::write (const FString& s)
{
  assert ( ! s.isNull() );
  int len = 0;
  const wchar_t* p = s.wc_str();

  if ( p )
  {
    while ( *p )
    {
      charData  nc;  // next character
      nc = FVTerm::getAttribute();
      nc.code = *p;
      nc.attr.bit.no_changes = false;
      nc.attr.bit.printed = false;

      data.push_back(nc);

      p++;
      len++;
    }  // end of while
  }

  return len;
}

//----------------------------------------------------------------------
int FTermBuffer::write (int c)
{
  charData nc;  // next character
  nc = FVTerm::getAttribute();
  nc.code = c;
  nc.attr.bit.no_changes = false;
  nc.attr.bit.printed = false;

  data.push_back(nc);
  return 1;
}


// FTermBuffer non-member operators
//----------------------------------------------------------------------
std::vector<FTermBuffer::charData>& operator << \
  ( std::vector<FTermBuffer::charData>& termString
  , const FTermBuffer& buf )
{
  if ( ! buf.data.empty() )
    termString.assign(buf.data.begin(), buf.data.end());

  return termString;
}

}  // namespace finalcut
