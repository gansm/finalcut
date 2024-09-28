/***********************************************************************
* fwidget_functions.h - FWidget helper functions                       *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2021-2024 Markus Gans                                      *
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
class FObject;
class FRect;
class FString;
class FWidget;
class FMouseEvent;

// Using-declarations
using FObjectList = std::vector<FObject*>;
using FWidgetList = std::vector<FWidget*>;

// non-member function forward declarations
void detectTerminalSize();
void initByte1PrintTransMask();
auto isFocusNextKey (const FKey) -> bool;
auto isFocusPrevKey (const FKey) -> bool;
auto isDialogMenuKey (const FKey) -> bool;
auto isEnterKey (const FKey) -> bool;
auto isEscapeKey (const FKey) -> bool;
auto isExpandComboBoxKey (const FKey) -> bool;
auto isCollapseComboBoxKey (const FKey) -> bool;
auto getFirstFocusableWidget (const FObjectList&) -> FWidget*;
auto getLastFocusableWidget (const FObjectList&) -> FWidget*;
auto isInFWidgetList (const FWidgetList*, const FWidget*) -> bool;
auto getAlignOffset (Align, const std::size_t, const std::size_t) -> std::size_t;
auto getHotkey (const FString&) -> FKey;
auto getHotkeyPos (const FString& src, FString& dest) -> std::size_t;
void setHotkeyViaString (FWidget*, const FString&);
void setWidgetFocus (FWidget*);
void setWidgetShadow (FWidget*, bool);
void passResizeCornerEventToDialog (const FWidget*, const FMouseEvent&);
void drawShadow (FWidget*);
void drawTransparentShadow (FWidget*);
void drawBlockShadow (FWidget*);
void clearBlockShadow (FWidget*);
void drawGenericBlockShadow (FWidget*, const std::array<FChar, 4>&);
void drawFlatBorder (FWidget*);
void clearFlatBorder (FWidget*);
void checkBorder (const FWidget*, FRect&);
void drawBorder (FWidget*, const FRect&);
void drawListBorder (FWidget*, const FRect&);
void drawBox (FWidget*, const FRect&);
void drawNewFontBox (FWidget*, const FRect&);
void drawNewFontUShapedBox (FWidget*, const FRect&);
void drawNewFontListBox (FWidget*, const FRect&);
void drawGenericBox (FWidget*, const FRect&, const std::array<wchar_t, 8>&);
void updateStatusbar (const FWidget* w, bool = true);
void drawStatusBarMessage();

}  // namespace finalcut

#endif  // FWIDGET_FUNCTIONS_H
