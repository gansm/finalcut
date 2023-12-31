/***********************************************************************
* fwidget_flags.h - Struct to store widget properties in flags (⚑)     *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2022-2023 Markus Gans                                      *
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


#ifndef FWIDGET_FLAGS_H
#define FWIDGET_FLAGS_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/ftypes.h"

namespace finalcut
{

struct FWidgetFeature
{
  uInt16 active         : 1;
  uInt16 scrollable     : 1;
  uInt16 resizeable     : 1;
  uInt16 minimizable    : 1;
  uInt16 flat           : 1;
  uInt16 no_border      : 1;
  uInt16 no_underline   : 1;
  uInt16 ignore_padding : 1;
  uInt16                : 8;  // padding bits
};

struct FWidgetVisibility
{
  uInt16 visible        : 1;
  uInt16 hidden         : 1;
  uInt16 shown          : 1;
  uInt16 modal          : 1;
  uInt16 always_on_top  : 1;
  uInt16 visible_cursor : 1;
  uInt16                : 10;  // padding bits
};

struct FWidgetFocus
{
  uInt16 focus       : 1;
  uInt16 focusable   : 1;
  uInt16             : 14;  // padding bits
};

struct FWidgetShadow
{
  uInt16 shadow       : 1;
  uInt16 trans_shadow : 1;
  uInt16              : 14;  // padding bits
};

struct FWidgetType
{
  uInt16 window_widget : 1;
  uInt16 dialog_widget : 1;
  uInt16 menu_widget   : 1;
  uInt16               : 13;  // padding bits
};

// Main struct, which contains all the data
struct FWidgetFlags
{
  FWidgetFeature     feature;
  FWidgetVisibility  visibility;
  FWidgetFocus       focus;
  FWidgetShadow      shadow;
  FWidgetType        type;
};

}  // namespace finalcut

#endif  // FWIDGET_FLAGS_H
