/***********************************************************************
* fwidget_flags.h - Struct to store widget properties in flags (⚑)     *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2022-2026 Markus Gans                                      *
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

inline bool containsFWidgetFlags ( const FWidgetFlags& subset
                                 , const FWidgetFlags& superset )
{
  auto check_feature = [] (const FWidgetFeature& a, const FWidgetFeature& b)
  {
    if ( a.active         && ! b.active         ) return false;
    if ( a.scrollable     && ! b.scrollable     ) return false;
    if ( a.resizeable     && ! b.resizeable     ) return false;
    if ( a.minimizable    && ! b.minimizable    ) return false;
    if ( a.flat           && ! b.flat           ) return false;
    if ( a.no_border      && ! b.no_border      ) return false;
    if ( a.no_underline   && ! b.no_underline   ) return false;
    if ( a.ignore_padding && ! b.ignore_padding ) return false;
    return true;
  };

  auto check_visibility = [] (const FWidgetVisibility& a, const FWidgetVisibility& b)
  {
    if ( a.visible        && ! b.visible        ) return false;
    if ( a.hidden         && ! b.hidden         ) return false;
    if ( a.shown          && ! b.shown          ) return false;
    if ( a.modal          && ! b.modal          ) return false;
    if ( a.always_on_top  && ! b.always_on_top  ) return false;
    if ( a.visible_cursor && ! b.visible_cursor ) return false;
    return true;
  };

  auto check_focus = [] (const FWidgetFocus& a, const FWidgetFocus& b)
  {
    if ( a.focus     && ! b.focus     ) return false;
    if ( a.focusable && ! b.focusable ) return false;
    return true;
  };

  auto check_shadow = [] (const FWidgetShadow& a, const FWidgetShadow& b)
  {
    if ( a.shadow       && ! b.shadow       ) return false;
    if ( a.trans_shadow && ! b.trans_shadow ) return false;
    return true;
  };

  auto check_type = [] (const FWidgetType& a, const FWidgetType& b)
  {
    if ( a.window_widget && ! b.window_widget ) return false;
    if ( a.dialog_widget && ! b.dialog_widget ) return false;
    if ( a.menu_widget   && ! b.menu_widget   ) return false;
    return true;
  };

  return check_feature (subset.feature, superset.feature)
      && check_visibility (subset.visibility, superset.visibility)
      && check_focus (subset.focus, superset.focus)
      && check_shadow (subset.shadow, superset.shadow)
      && check_type (subset.type, superset.type);
}

}  // namespace finalcut

#endif  // FWIDGET_FLAGS_H
