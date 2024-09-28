/***********************************************************************
* flog.h - Interface of the FINAL CUT logger                           *
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

#include <final/util/fstring.h>

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
    using IOManip = decltype(std::endl<char, std::char_traits<char>>);

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
    auto operator << (const T&) -> FLog&;
    auto operator << (IOManip) -> FLog&;
    auto operator << (LogLevel) -> FLog&;

    virtual auto getClassName() const -> FString;
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
    auto sync() -> int override;
    auto getLevel() const -> const LogLevel&;
    auto setLevel() -> LogLevel&;
    auto getEnding() const -> const LineEnding&;
    auto setEnding() -> LineEnding&;

  private:
    // Data member
    LogLevel     level{LogLevel::Info};
    LineEnding   end_of_line{LineEnding::CRLF};
    FLogPrint    current_log{ [this] (const auto& s) { info(s); } };
    std::mutex   current_log_mutex{};
    std::mutex   stream_mutex{};
    std::ostream stream{this};

    // Friend Non-member operator functions
    friend auto operator << (std::ostream&, LogLevel) -> std::ostream&;
};

// non-member function forward declarations
//----------------------------------------------------------------------
void handleOutOfRangeError (const std::out_of_range& ex);

// FLog inline functions
//----------------------------------------------------------------------
template <typename T>
inline auto FLog::operator << (const T& s) -> FLog&
{
  std::lock_guard<std::mutex> lock_guard(stream_mutex);
  stream << s;
  return *this;
}

//----------------------------------------------------------------------
inline auto FLog::operator << (IOManip pf) -> FLog&
{
  std::lock_guard<std::mutex> lock_guard(stream_mutex);
  pf(stream);
  return *this;
}

//----------------------------------------------------------------------
inline auto FLog::getClassName() const -> FString
{
  return "FLog";
}

//----------------------------------------------------------------------
inline auto FLog::getLevel() const -> const LogLevel&
{
  return level;
}

//----------------------------------------------------------------------
inline auto FLog::setLevel() -> LogLevel&
{
  return level;
}

//----------------------------------------------------------------------
inline auto FLog::getEnding() const -> const LineEnding&
{
  return end_of_line;
}

//----------------------------------------------------------------------
inline auto FLog::setEnding() -> LineEnding&
{
  return end_of_line;
}

}  // namespace finalcut

#endif  // FLOG_H
