/***********************************************************************
* fwidgetcolors.h - Set widget color theme                             *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2018 Markus Gans                                           *
*                                                                      *
* The Final Cut is free software; you can redistribute it and/or       *
* modify it under the terms of the GNU Lesser General Public License   *
* as published by the Free Software Foundation; either version 3 of    *
* the License, or (at your option) any later version.                  *
*                                                                      *
* The Final Cut is distributed in the hope that it will be useful,     *
* but WITHOUT ANY WARRANTY; without even the implied warranty of       *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU Lesser General Public License for more details.                  *
*                                                                      *
* You should have received a copy of the GNU Lesser General Public     *
* License along with this program.  If not, see                        *
* <http://www.gnu.org/licenses/>.                                      *
***********************************************************************/

/*  Standalone class
 *  ════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FWidgetColors ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FWIDGETCOLORS_H
#define FWIDGETCOLORS_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fc.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FWidgetColors
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FWidgetColors
{
  public:
    // Methods
    void set8ColorTheme();
    void set16ColorTheme();

    // Data Members
    FColor term_fg;
    FColor term_bg;
    FColor list_fg;
    FColor list_bg;
    FColor selected_list_fg;
    FColor selected_list_bg;
    FColor current_element_focus_fg;
    FColor current_element_focus_bg;
    FColor current_element_fg;
    FColor current_element_bg;
    FColor current_inc_search_element_fg;
    FColor selected_current_element_focus_fg;
    FColor selected_current_element_focus_bg;
    FColor selected_current_element_fg;
    FColor selected_current_element_bg;
    FColor label_fg;
    FColor label_bg;
    FColor label_inactive_fg;
    FColor label_inactive_bg;
    FColor label_hotkey_fg;
    FColor label_hotkey_bg;
    FColor label_emphasis_fg;
    FColor label_ellipsis_fg;
    FColor inputfield_active_focus_fg;
    FColor inputfield_active_focus_bg;
    FColor inputfield_active_fg;
    FColor inputfield_active_bg;
    FColor inputfield_inactive_fg;
    FColor inputfield_inactive_bg;
    FColor dialog_fg;
    FColor dialog_resize_fg;
    FColor dialog_emphasis_fg;
    FColor dialog_bg;
    FColor error_box_fg;
    FColor error_box_emphasis_fg;
    FColor error_box_bg;
    FColor tooltip_fg;
    FColor tooltip_bg;
    FColor shadow_fg;
    FColor shadow_bg;
    FColor toggle_button_active_focus_fg;
    FColor toggle_button_active_focus_bg;
    FColor toggle_button_active_fg;
    FColor toggle_button_active_bg;
    FColor toggle_button_inactive_fg;
    FColor toggle_button_inactive_bg;
    FColor button_active_focus_fg;
    FColor button_active_focus_bg;
    FColor button_active_fg;
    FColor button_active_bg;
    FColor button_inactive_fg;
    FColor button_inactive_bg;
    FColor button_hotkey_fg;
    FColor titlebar_active_fg;
    FColor titlebar_active_bg;
    FColor titlebar_inactive_fg;
    FColor titlebar_inactive_bg;
    FColor titlebar_button_fg;
    FColor titlebar_button_bg;
    FColor titlebar_button_focus_fg;
    FColor titlebar_button_focus_bg;
    FColor menu_active_focus_fg;
    FColor menu_active_focus_bg;
    FColor menu_active_fg;
    FColor menu_active_bg;
    FColor menu_inactive_fg;
    FColor menu_inactive_bg;
    FColor menu_hotkey_fg;
    FColor menu_hotkey_bg;
    FColor statusbar_fg;
    FColor statusbar_bg;
    FColor statusbar_hotkey_fg;
    FColor statusbar_hotkey_bg;
    FColor statusbar_separator_fg;
    FColor statusbar_active_fg;
    FColor statusbar_active_bg;
    FColor statusbar_active_hotkey_fg;
    FColor statusbar_active_hotkey_bg;
    FColor scrollbar_fg;
    FColor scrollbar_bg;
    FColor scrollbar_button_fg;
    FColor scrollbar_button_bg;
    FColor progressbar_fg;
    FColor progressbar_bg;
};
#pragma pack(pop)

}  // namespace finalcut

#endif  // FWIDGETCOLORS_H
