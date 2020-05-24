/***********************************************************************
* fstringstream.cpp - I/O operations on FString based streams          *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2020 Markus Gans                                           *
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

#include "final/fstring.h"
#include "final/fstringstream.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FStringStream
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FStringStream::FStringStream (openmode mode)
  : std::wiostream{&buffer}
  , buffer{mode}
{ }

//----------------------------------------------------------------------
FStringStream::FStringStream (const FString& str, openmode mode)
  : std::wiostream{&buffer}
  , buffer{str.wc_str(), mode}
{ }

//----------------------------------------------------------------------
FStringStream::FStringStream (FStringStream&& sstream) noexcept
  : std::wiostream{std::move(sstream)}
  , buffer{std::move(sstream.buffer)}
{
  std::wiostream::set_rdbuf(&buffer);
}

//----------------------------------------------------------------------
FStringStream::~FStringStream()  // destructor
{ }

// public methods of FStringStream
//----------------------------------------------------------------------
FStringStream& FStringStream::operator = (FStringStream&& sstream)
{
  std::wiostream::operator = (std::move(sstream));
  buffer = std::move(sstream.buffer);
  return *this;
}

//----------------------------------------------------------------------
void FStringStream::swap (FStringStream& sstream) noexcept
{
  std::wiostream::swap(sstream);
  buffer.swap(sstream.buffer);
}

}  // namespace finalcut

