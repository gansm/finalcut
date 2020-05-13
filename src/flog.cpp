/***********************************************************************
* flog.cpp - Interface of the FINAL CUT logger                         *
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

#include "final/flog.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FLog
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FLog::FLog()
{ }

//----------------------------------------------------------------------
FLog::~FLog()  // destructor
{
  FLog::sync();
}


// public methods of FLog
//----------------------------------------------------------------------
FLog& FLog::operator << (LogLevel l)
{
  sync();

  switch ( l )
  {
    case Info:
      current_log = std::bind(&FLog::info, this, _1);
      break;

    case Warn:
      current_log = std::bind(&FLog::warn, this, _1);
      break;

    case Error:
      current_log = std::bind(&FLog::error, this, _1);
      break;

    case Debug:
      current_log = std::bind(&FLog::debug, this, _1);
      break;
  }

  return *this;
}


// protected methods of FLog
//----------------------------------------------------------------------
int FLog::sync()
{
  if ( ! str().empty() )
  {
    current_log (str());
    str("");
  }

  return 0;
}

}  // namespace finalcut

