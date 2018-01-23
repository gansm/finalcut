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

#include "final/fc.h"


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
    short term_fg;
    short term_bg;
    short list_fg;
    short list_bg;
    short selected_list_fg;
    short selected_list_bg;
    short current_element_focus_fg;
    short current_element_focus_bg;
    short current_element_fg;
    short current_element_bg;
    short current_inc_search_element_fg;
    short selected_current_element_focus_fg;
    short selected_current_element_focus_bg;
    short selected_current_element_fg;
    short selected_current_element_bg;
    short label_fg;
    short label_bg;
    short label_inactive_fg;
    short label_inactive_bg;
    short label_hotkey_fg;
    short label_hotkey_bg;
    short label_emphasis_fg;
    short label_ellipsis_fg;
    short inputfield_active_focus_fg;
    short inputfield_active_focus_bg;
    short inputfield_active_fg;
    short inputfield_active_bg;
    short inputfield_inactive_fg;
    short inputfield_inactive_bg;
    short dialog_fg;
    short dialog_resize_fg;
    short dialog_emphasis_fg;
    short dialog_bg;
    short error_box_fg;
    short error_box_emphasis_fg;
    short error_box_bg;
    short tooltip_fg;
    short tooltip_bg;
    short shadow_fg;
    short shadow_bg;
    short toggle_button_active_focus_fg;
    short toggle_button_active_focus_bg;
    short toggle_button_active_fg;
    short toggle_button_active_bg;
    short toggle_button_inactive_fg;
    short toggle_button_inactive_bg;
    short button_active_focus_fg;
    short button_active_focus_bg;
    short button_active_fg;
    short button_active_bg;
    short button_inactive_fg;
    short button_inactive_bg;
    short button_hotkey_fg;
    short titlebar_active_fg;
    short titlebar_active_bg;
    short titlebar_inactive_fg;
    short titlebar_inactive_bg;
    short titlebar_button_fg;
    short titlebar_button_bg;
    short titlebar_button_focus_fg;
    short titlebar_button_focus_bg;
    short menu_active_focus_fg;
    short menu_active_focus_bg;
    short menu_active_fg;
    short menu_active_bg;
    short menu_inactive_fg;
    short menu_inactive_bg;
    short menu_hotkey_fg;
    short menu_hotkey_bg;
    short statusbar_fg;
    short statusbar_bg;
    short statusbar_hotkey_fg;
    short statusbar_hotkey_bg;
    short statusbar_separator_fg;
    short statusbar_active_fg;
    short statusbar_active_bg;
    short statusbar_active_hotkey_fg;
    short statusbar_active_hotkey_bg;
    short scrollbar_fg;
    short scrollbar_bg;
    short scrollbar_button_fg;
    short scrollbar_button_bg;
    short progressbar_fg;
    short progressbar_bg;
};
#pragma pack(pop)


#endif  // FWIDGETCOLORS_H
