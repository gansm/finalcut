/***********************************************************************
* flogger.h - The FINAL CUT text logger                                *
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

#include <functional>
#include <iostream>
#include <sstream>
#include <string>

#include <chrono>
#include <iomanip>

#include "final/flog.h"

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
    FString getClassName() const override;
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
    void              newlineReplace (std::string&, const std::string&) const;
    std::string       getTimeString() const;
    std::string       getEOL();
    void              printLogLine (const std::string&);

    // Data member
    bool         timestamp{false};
    std::ostream output{std::cerr.rdbuf()};
};

// FLogger inline functions
//----------------------------------------------------------------------
inline FString FLogger::getClassName() const
{ return "FLogger"; }

//----------------------------------------------------------------------
inline void FLogger::info (const std::string& msg)
{
  std::lock_guard<std::mutex> lock_guard(getMutex());
  setLevel() = Info;
  printLogLine (msg);
}

//----------------------------------------------------------------------
inline void FLogger::warn (const std::string& msg)
{
  std::lock_guard<std::mutex> lock_guard(getMutex());
  setLevel() = Warn;
  printLogLine (msg);
}

//----------------------------------------------------------------------
inline void FLogger::error (const std::string& msg)
{
  std::lock_guard<std::mutex> lock_guard(getMutex());
  setLevel() = Error;
  printLogLine (msg);
}

//----------------------------------------------------------------------
inline void FLogger::debug (const std::string& msg)
{
  std::lock_guard<std::mutex> lock_guard(getMutex());
  setLevel() = Debug;
  printLogLine (msg);
}

//----------------------------------------------------------------------
inline void FLogger::flush()
{
  std::lock_guard<std::mutex> lock_guard(getMutex());
  output.flush();
}

//----------------------------------------------------------------------
inline void FLogger::setOutputStream (const std::ostream& os)
{
  std::lock_guard<std::mutex> lock_guard(getMutex());
  output.rdbuf(os.rdbuf());
}

//----------------------------------------------------------------------
inline void FLogger::setLineEnding (LineEnding eol)
{
  std::lock_guard<std::mutex> lock_guard(getMutex());
  setEnding() = eol;
}

//----------------------------------------------------------------------
inline void FLogger::enableTimestamp()
{
  std::lock_guard<std::mutex> lock_guard(getMutex());
  timestamp = true;
}

//----------------------------------------------------------------------
inline void FLogger::disableTimestamp()
{
  std::lock_guard<std::mutex> lock_guard(getMutex());
  timestamp = false;
}

}  // namespace finalcut

#endif  // FLOGGER_H
