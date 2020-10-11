/***********************************************************************
* flog.cpp - Interface of the FINAL CUT logger                         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2020 Markus Gans                                           *
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
  using std::placeholders::_1;
  sync();
  std::lock_guard<std::mutex> lock_guard(mut);

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
    std::lock_guard<std::mutex> lock_guard(mut);
    current_log (str());
    str("");
  }

  return 0;
}

}  // namespace finalcut

