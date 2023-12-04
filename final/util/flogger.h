/***********************************************************************
* flogger.h - The FINAL CUT text logger                                *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2020-2023 Markus Gans                                      *
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

/*  Inheritance diagram
 *  ═══════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ std::stringbuf ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *         ▲
 *         │
 *      ▕▔▔▔▔▔▔▏
 *      ▕ FLog ▏
 *      ▕▁▁▁▁▁▁▏
 *         ▲
 *         │
 *    ▕▔▔▔▔▔▔▔▔▔▏
 *    ▕ FLogger ▏
 *    ▕▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FLOGGER_H
#define FLOGGER_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "final/util/flog.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FLogger
//----------------------------------------------------------------------

class FLogger : public FLog
{
  public:
    // Constructor
    FLogger() = default;

    // Destructor
    ~FLogger() noexcept override;

    // Methods
    auto getClassName() const -> FString override;
    void info (const std::string&) override;
    void warn (const std::string&) override;
    void error (const std::string&) override;
    void debug (const std::string&) override;
    void flush() override;
    void setOutputStream (const std::ostream&) override;
    void setLineEnding (LineEnding) override;
    void enableTimestamp() override;
    void disableTimestamp() override;

  private:
    // Methods
    void newlineReplace (std::string&, const std::string&) const;
    auto getTimeString() const -> std::string;
    auto getEOL() const -> std::string;
    void printLogLine (const std::string&);
    auto getLogLevelString() const -> std::string;
    auto getPrefixString (const std::string&) -> std::string;

    // Data member
    bool         timestamp{false};
    std::mutex   print_mutex{};
    std::mutex   output_mutex{};
    std::ostream output{std::cerr.rdbuf()};
};

// FLogger inline functions
//----------------------------------------------------------------------
inline auto FLogger::getClassName() const -> FString
{ return "FLogger"; }

//----------------------------------------------------------------------
inline void FLogger::info (const std::string& msg)
{
  {
    std::lock_guard<std::mutex> lock_guard(print_mutex);
    setLevel() = LogLevel::Info;
    // Release mutex at end of scope
  }
  printLogLine (msg);
}

//----------------------------------------------------------------------
inline void FLogger::warn (const std::string& msg)
{
  {
    std::lock_guard<std::mutex> lock_guard(print_mutex);
    setLevel() = LogLevel::Warn;
    // Release mutex at end of scope
  }
  printLogLine (msg);
}

//----------------------------------------------------------------------
inline void FLogger::error (const std::string& msg)
{
  {
    std::lock_guard<std::mutex> lock_guard(print_mutex);
    setLevel() = LogLevel::Error;
    // Release mutex at end of scope
  }
  printLogLine (msg);
}

//----------------------------------------------------------------------
inline void FLogger::debug (const std::string& msg)
{
  {
    std::lock_guard<std::mutex> lock_guard(print_mutex);
    setLevel() = LogLevel::Debug;
    // Release mutex at end of scope
  }
  printLogLine (msg);
}

//----------------------------------------------------------------------
inline void FLogger::flush()
{
  std::lock_guard<std::mutex> lock_guard(output_mutex);
  output.flush();
}

//----------------------------------------------------------------------
inline void FLogger::setOutputStream (const std::ostream& os)
{
  std::lock_guard<std::mutex> lock_guard(output_mutex);
  output.rdbuf(os.rdbuf());
}

//----------------------------------------------------------------------
inline void FLogger::setLineEnding (LineEnding eol)
{
  std::lock_guard<std::mutex> lock_guard(print_mutex);
  setEnding() = eol;
}

//----------------------------------------------------------------------
inline void FLogger::enableTimestamp()
{
  std::lock_guard<std::mutex> lock_guard(print_mutex);
  timestamp = true;
}

//----------------------------------------------------------------------
inline void FLogger::disableTimestamp()
{
  std::lock_guard<std::mutex> lock_guard(print_mutex);
  timestamp = false;
}

}  // namespace finalcut

#endif  // FLOGGER_H
