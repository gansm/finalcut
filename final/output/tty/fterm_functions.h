/***********************************************************************
* fterm_functions.h - FTerm helper functions                           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2021 Markus Gans                                           *
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

#include <string>

#include "final/ftypes.h"

namespace finalcut
{

// class forward declaration
class FPoint;
class FSize;
class FString;
class FVTermBuffer;

// non-member function forward declarations
uInt          env2uint (const std::string&);
std::string&  getExitMessage();
void          setExitMessage (const FString&);
FColor        rgb2ColorIndex (uInt8, uInt8, uInt8);
bool          isReverseNewFontchar (wchar_t);
bool          hasFullWidthSupports();
wchar_t       cp437_to_unicode (uChar);
uChar         unicode_to_cp437 (wchar_t);

#if defined(__CYGWIN__)
  std::string unicode_to_utf8 (wchar_t ucs);
#else
  std::string unicode_to_utf8 (wchar_t ucs);
#endif

FString       getFullWidth (const FString&);
FString       getHalfWidth (const FString&);
FString       getColumnSubString (const FString&, std::size_t, std::size_t);
std::size_t   getLengthFromColumnWidth (const FString&, std::size_t);
std::size_t   getColumnWidth (const FString&, std::size_t);
std::size_t   getColumnWidth (const FString&);
std::size_t   getColumnWidth (const wchar_t);
std::size_t   getColumnWidth (const FChar&);
std::size_t   getColumnWidth (const FVTermBuffer&);
void          addColumnWidth (FChar&);
int           getCharLength (const FString&, std::size_t);
int           getPrevCharLength (const FString&, std::size_t);
std::size_t   searchLeftCharBegin (const FString&, std::size_t);
std::size_t   searchRightCharBegin (const FString&, std::size_t);
FPoint        readCursorPos();

}  // namespace finalcut

#endif  // FTERM_FUNCTIONS_H

