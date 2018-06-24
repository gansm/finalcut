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
int FTermBuffer::writef (const wchar_t format[], ...)
{
  assert ( format != 0 );
  static const int BufSize = 1024;
  wchar_t buffer[BufSize];
  va_list args;

  va_start (args, format);
  std::vswprintf (buffer, BufSize, format, args);
  va_end (args);

  FString str(buffer);
  return write(str);
}

//----------------------------------------------------------------------
int FTermBuffer::writef (const char format[], ...)
{
  assert ( format != 0 );
  int   len;
  char  buf[512];
  char* buffer;
  va_list args;

  buffer = buf;
  va_start (args, format);
  len = vsnprintf (buffer, sizeof(buf), format, args);
  va_end (args);

  if ( len >= int(sizeof(buf)) )
  {
    try
    {
      buffer = new char[len + 1]();
    }
    catch (const std::bad_alloc& ex)
    {
      std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
      return -1;
    }

    va_start (args, format);
    vsnprintf (buffer, uLong(len + 1), format, args);
    va_end (args);
  }

  FString str(buffer);
  int ret = write(str);

  if ( buffer != buf )
    delete[] buffer;

  return ret;
}

//----------------------------------------------------------------------
int FTermBuffer::write (const std::wstring& s)
{
  assert ( ! s.empty() );
  return write (FString(s));
}

//----------------------------------------------------------------------
int FTermBuffer::write (const wchar_t s[])
{
  assert ( s != 0 );
  return write (FString(s));
}

//----------------------------------------------------------------------
int FTermBuffer::write (const char s[])
{
  assert ( s != 0 );
  FString str(s);
  return write(str);
}

//----------------------------------------------------------------------
int FTermBuffer::write (const std::string& s)
{
  assert ( ! s.empty() );
  return write (FString(s));
}

//----------------------------------------------------------------------
int FTermBuffer::write (const FString& s)
{
  assert ( ! s.isNull() );
  register int len = 0;
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
int FTermBuffer::write (register int c)
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
