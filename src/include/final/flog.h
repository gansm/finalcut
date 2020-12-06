/***********************************************************************
* flog.h - Interface of the FINAL CUT logger                           *
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
 */

#ifndef FLOG_H
#define FLOG_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <functional>
#include <iostream>
#include <mutex>
#include <ostream>
#include <sstream>
#include <string>

#include <final/fstring.h>

namespace finalcut
{

//----------------------------------------------------------------------
// class FLog
//----------------------------------------------------------------------

class FLog : public std::stringbuf
{
  public:
    // Using-declaration
    using FLogPrint = std::function<void(const std::string&)>;
    using IOManip = std::ostream& (*)(std::ostream&);

    // Enumerations
    enum class LogLevel
    {
      Info, Warn, Error, Debug
    };

    enum class LineEnding
    {
      LF, CR, CRLF
    };

    // Constructor
    FLog() = default;

    // Destructor
    ~FLog() override;

    template <typename T>
    FLog& operator << (const T& s);
    FLog& operator << (IOManip);
    FLog& operator << (LogLevel);

    virtual FString getClassName() const;
    virtual void info (const std::string&) = 0;
    virtual void warn (const std::string&) = 0;
    virtual void error (const std::string&) = 0;
    virtual void debug (const std::string&) = 0;
    virtual void flush() = 0;
    virtual void setOutputStream (const std::ostream&) = 0;
    virtual void setLineEnding (LineEnding) = 0;
    virtual void enableTimestamp() = 0;
    virtual void disableTimestamp() = 0;

  protected:
    int               sync() override;
    const LogLevel&   getLevel() const;
    LogLevel&         setLevel();
    const LineEnding& getEnding();
    LineEnding&       setEnding();
    std::mutex&       getMutex();

  private:
    // Data member
    LogLevel     level{LogLevel::Info};
    LineEnding   end_of_line{LineEnding::CRLF};
    std::mutex   mut{};
    FLogPrint    current_log{std::bind(&FLog::info, this, std::placeholders::_1)};
    std::ostream stream{this};

    // Friend Non-member operator functions
    friend std::ostream& operator << (std::ostream&, LogLevel);
};

// FLog inline functions
//----------------------------------------------------------------------
template <typename T>
inline FLog& FLog::operator << (const T& s)
{
  std::lock_guard<std::mutex> lock_guard(mut);
  stream << s;
  return *this;
}

//----------------------------------------------------------------------
inline FLog& FLog::operator << (IOManip pf)
{
  std::lock_guard<std::mutex> lock_guard(mut);
  pf(stream);
  return *this;
}

//----------------------------------------------------------------------
inline FString FLog::getClassName() const
{ return "FLog"; }

//----------------------------------------------------------------------
inline const FLog::LogLevel& FLog::getLevel() const
{
  return level;
}

//----------------------------------------------------------------------
inline FLog::LogLevel& FLog::setLevel()
{
  return level;
}

//----------------------------------------------------------------------
inline const FLog::LineEnding& FLog::getEnding()
{
  std::lock_guard<std::mutex> lock_guard(mut);
  return end_of_line;
}

//----------------------------------------------------------------------
inline FLog::LineEnding& FLog::setEnding()
{
  return end_of_line;
}

//----------------------------------------------------------------------
inline std::mutex& FLog::getMutex()
{ return mut; }

}  // namespace finalcut

#endif  // FLOG_H
