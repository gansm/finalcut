/***********************************************************************
* flog.cpp - Interface of the FINAL CUT logger                         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2020-2021 Markus Gans                                      *
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
FLog::~FLog()  // destructor
{
  FLog::sync();
}


// public methods of FLog
//----------------------------------------------------------------------
FLog& FLog::operator << (LogLevel log_level)
{
  assert ( log_level == LogLevel::Info
        || log_level == LogLevel::Warn
        || log_level == LogLevel::Error
        || log_level == LogLevel::Debug );

  sync();
  std::lock_guard<std::mutex> lock_guard(current_log_mutex);

  switch ( log_level )
  {
    case LogLevel::Info:
      current_log = [this] (const std::string& s) { info(s); };
      break;

    case LogLevel::Warn:
      current_log = [this] (const std::string& s) { warn(s); };
      break;

    case LogLevel::Error:
      current_log = [this] (const std::string& s) { error(s); };
      break;

    case LogLevel::Debug:
      current_log = [this] (const std::string& s) { debug(s); };
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

