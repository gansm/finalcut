/***********************************************************************
* flog.cpp - Interface of the FINAL CUT logger                         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2020-2026 Markus Gans                                      *
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

#include "final/util/flog.h"

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

  std::lock_guard<std::mutex> lock(current_log_mutex);
  current_log = [] (const std::string&) {};

  log_callback = nullptr;
}


// public methods of FLog
//----------------------------------------------------------------------
auto FLog::operator << (LogLevel log_level) -> FLog&
{
  sync();
  std::lock_guard<std::mutex> lock_guard(current_log_mutex);

  switch ( log_level )
  {
    case LogLevel::Info:
      current_log = [this] (const auto& s) { info(s); };
      break;

    case LogLevel::Warn:
      current_log = [this] (const auto& s) { warn(s); };
      break;

    case LogLevel::Error:
      current_log = [this] (const auto& s) { error(s); };
      break;

    case LogLevel::Debug:
      current_log = [this] (const auto& s) { debug(s); };
      break;

    default:
      throw std::invalid_argument{"Invalid log level"};
  }

  return *this;
}


// protected methods of FLog
//----------------------------------------------------------------------
auto FLog::sync() -> int
{
  int result = std::stringbuf::sync();

  if ( ! str().empty() )
  {
    current_log (str());
    str("");

    if ( log_callback )
      log_callback();
  }

  return result;
}

//----------------------------------------------------------------------
auto FLog::xsputn (const char* str, std::streamsize n) -> std::streamsize
{
  std::streamsize result = std::stringbuf::xsputn(str, n);

  if ( result > 0 && log_callback )
    log_callback();

  return result;
}

//----------------------------------------------------------------------
auto FLog::overflow (int c) -> int
{
  int result = std::stringbuf::overflow(c);

  if ( result != EOF && log_callback )
    log_callback();

  return result;
}


// FLog non-member operators
//----------------------------------------------------------------------
void handleOutOfRangeError (const std::out_of_range& ex)
{
  std::clog << FLog::LogLevel::Error
            << "Out of Range error: " << ex.what() << std::endl;
}

}  // namespace finalcut
