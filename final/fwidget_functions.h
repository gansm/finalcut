/***********************************************************************
* fwidget_functions.h - FWidget helper functions                       *
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

#ifndef FWIDGET_FUNCTIONS_H
#define FWIDGET_FUNCTIONS_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <vector>

#include "final/fc.h"
#include "final/ftypes.h"

namespace finalcut
{

// class forward declaration
class FRect;
class FString;
class FWidget;

// Using-declarations
using FWidgetList = std::vector<FWidget*>;

// non-member function forward declarations
void          detectTerminalSize();
bool          isFocusNextKey (const FKey);
bool          isFocusPrevKey (const FKey);
bool          isMenuKey (const FKey);
bool          isEnterKey (const FKey);
bool          isEscapeKey (const FKey);
bool          isInFWidgetList (const FWidgetList*, const FWidget*);
FKey          getHotkey (const FString&);
std::size_t   getHotkeyPos (const FString& src, FString& dest);
void          setHotkeyViaString (FWidget*, const FString&);
void          setWidgetFocus (FWidget*);
bool          setWidgetShadow (FWidget*, bool);
void          drawShadow (FWidget*);
void          drawTransparentShadow (FWidget*);
void          drawBlockShadow (FWidget*);
void          clearShadow (FWidget*);
void          drawFlatBorder (FWidget*);
void          clearFlatBorder (FWidget*);
void          checkBorder (const FWidget*, FRect&);
void          drawBorder (FWidget*, const FRect&);
void          drawListBorder (FWidget*, const FRect&);
void          drawBox (FWidget*, const FRect&);
void          drawNewFontBox (FWidget*, const FRect&);
void          drawNewFontListBox (FWidget*, const FRect&);

}  // namespace finalcut

#endif  // FWIDGET_FUNCTIONS_H
