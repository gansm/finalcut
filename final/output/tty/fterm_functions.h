/***********************************************************************
* fterm_functions.h - FTerm helper functions                           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2021-2023 Markus Gans                                      *
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

#ifndef FTERM_FUNCTIONS_H
#define FTERM_FUNCTIONS_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#if defined(__CYGWIN__)
  #include <sys/select.h>  // need for FD_ZERO, FD_SET, FD_CLR, ...
#endif

#include <unistd.h>

#include <string>
#include <utility>

#include "final/ftypes.h"

namespace finalcut
{

// class forward declaration
class FPoint;
class FSize;
class FString;
class FVTermBuffer;

// non-member function forward declarations
auto env2uint (const std::string&) -> uInt;
auto getExitMessage() -> std::string&;
void setExitMessage (const FString&);
auto rgb2ColorIndex (uInt8, uInt8, uInt8) -> FColor;
auto isReverseNewFontchar (wchar_t) -> bool;
auto hasFullWidthSupports() -> bool;
auto cp437_to_unicode (uChar) -> wchar_t;
auto unicode_to_cp437 (wchar_t) -> uChar;

#if defined(__CYGWIN__)
  auto unicode_to_utf8 (wchar_t ucs) -> std::string;
#else
  auto unicode_to_utf8 (wchar_t ucs) -> std::string;
#endif

auto getFullWidth (const FString&) -> FString;
auto getHalfWidth (const FString&) -> FString;
auto getColumnSubString (const FString&, std::size_t, std::size_t) -> FString;
auto getLengthFromColumnWidth (const FString&, std::size_t) -> std::size_t;
auto getColumnWidth (const FString&, std::size_t) -> std::size_t;
auto getColumnWidth (const FString&) -> std::size_t;
auto getColumnWidth (const wchar_t) -> std::size_t;
auto getColumnWidth (const FChar&) -> std::size_t;
auto getColumnWidth (const FVTermBuffer&) -> std::size_t;
void addColumnWidth (FChar&, std::size_t);
auto getCharLength (const FString&, std::size_t) -> int;
auto getPrevCharLength (const FString&, std::size_t) -> int;
auto searchLeftCharBegin (const FString&, std::size_t) -> std::size_t;
auto searchRightCharBegin (const FString&, std::size_t) -> std::size_t;
auto readCursorPos() -> FPoint;

template<std::size_t size, typename UnaryPredicate>
auto captureTerminalInput ( std::array<char, size>& data
                          , uInt64 timeout_us
                          , UnaryPredicate p ) -> std::size_t
{
  const int stdin_no{0};
  fd_set ifds{};
  struct timeval tv{};
  FD_ZERO(&ifds);
  FD_SET(stdin_no, &ifds);
  tv.tv_sec  = 0;
  tv.tv_usec = suseconds_t(timeout_us);
  std::size_t pos{0};

  // Read data form stdin
  if ( select (stdin_no + 1, &ifds, nullptr, nullptr, &tv) < 1 )
    return 0;

  do
  {
    std::size_t bytes_free = data.size() - pos - 1;
    const ssize_t bytes = read(stdin_no, &data[pos], bytes_free);

    if ( bytes <= 0 )
      break;

    pos += std::size_t(bytes);
  }
  while ( pos < data.size() && p(data) );

  return pos;
}

template<typename Data>
auto captureTerminalInput (Data&& data, uInt64 timeout_us) -> std::size_t
{
  const auto isTrue = [] (auto) { return true; };
  return captureTerminalInput (std::forward<Data>(data), timeout_us, isTrue);
}

}  // namespace finalcut

#endif  // FTERM_FUNCTIONS_H

