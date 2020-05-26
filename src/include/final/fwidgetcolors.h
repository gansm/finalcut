/***********************************************************************
* fwidgetcolors.h - Set widget color theme                             *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2018-2020 Markus Gans                                      *
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

class FWidgetColors
{
  public:
    // Constructor
    FWidgetColors();

    // Destructor
    virtual ~FWidgetColors();

    // Method
    virtual void setColorTheme() = 0;

    // Data members
    FColor term_fg{fc::Default};
    FColor term_bg{fc::Default};
    FColor list_fg{fc::Default};
    FColor list_bg{fc::Default};
    FColor selected_list_fg{fc::Default};
    FColor selected_list_bg{fc::Default};
    FColor current_element_focus_fg{fc::Default};
    FColor current_element_focus_bg{fc::Default};
    FColor current_element_fg{fc::Default};
    FColor current_element_bg{fc::Default};
    FColor current_inc_search_element_fg{fc::Default};
    FColor selected_current_element_focus_fg{fc::Default};
    FColor selected_current_element_focus_bg{fc::Default};
    FColor selected_current_element_fg{fc::Default};
    FColor selected_current_element_bg{fc::Default};
    FColor label_fg{fc::Default};
    FColor label_bg{fc::Default};
    FColor label_inactive_fg{fc::Default};
    FColor label_inactive_bg{fc::Default};
    FColor label_hotkey_fg{fc::Default};
    FColor label_hotkey_bg{fc::Default};
    FColor label_emphasis_fg{fc::Default};
    FColor label_ellipsis_fg{fc::Default};
    FColor inputfield_active_focus_fg{fc::Default};
    FColor inputfield_active_focus_bg{fc::Default};
    FColor inputfield_active_fg{fc::Default};
    FColor inputfield_active_bg{fc::Default};
    FColor inputfield_inactive_fg{fc::Default};
    FColor inputfield_inactive_bg{fc::Default};
    FColor dialog_fg{fc::Default};
    FColor dialog_resize_fg{fc::Default};
    FColor dialog_emphasis_fg{fc::Default};
    FColor dialog_bg{fc::Default};
    FColor error_box_fg{fc::Default};
    FColor error_box_emphasis_fg{fc::Default};
    FColor error_box_bg{fc::Default};
    FColor tooltip_fg{fc::Default};
    FColor tooltip_bg{fc::Default};
    FColor shadow_fg{fc::Default};
    FColor shadow_bg{fc::Default};
    FColor toggle_button_active_focus_fg{fc::Default};
    FColor toggle_button_active_focus_bg{fc::Default};
    FColor toggle_button_active_fg{fc::Default};
    FColor toggle_button_active_bg{fc::Default};
    FColor toggle_button_inactive_fg{fc::Default};
    FColor toggle_button_inactive_bg{fc::Default};
    FColor button_active_focus_fg{fc::Default};
    FColor button_active_focus_bg{fc::Default};
    FColor button_active_fg{fc::Default};
    FColor button_active_bg{fc::Default};
    FColor button_inactive_fg{fc::Default};
    FColor button_inactive_bg{fc::Default};
    FColor button_hotkey_fg{fc::Default};
    FColor titlebar_active_fg{fc::Default};
    FColor titlebar_active_bg{fc::Default};
    FColor titlebar_inactive_fg{fc::Default};
    FColor titlebar_inactive_bg{fc::Default};
    FColor titlebar_button_fg{fc::Default};
    FColor titlebar_button_bg{fc::Default};
    FColor titlebar_button_focus_fg{fc::Default};
    FColor titlebar_button_focus_bg{fc::Default};
    FColor menu_active_focus_fg{fc::Default};
    FColor menu_active_focus_bg{fc::Default};
    FColor menu_active_fg{fc::Default};
    FColor menu_active_bg{fc::Default};
    FColor menu_inactive_fg{fc::Default};
    FColor menu_inactive_bg{fc::Default};
    FColor menu_hotkey_fg{fc::Default};
    FColor menu_hotkey_bg{fc::Default};
    FColor statusbar_fg{fc::Default};
    FColor statusbar_bg{fc::Default};
    FColor statusbar_hotkey_fg{fc::Default};
    FColor statusbar_hotkey_bg{fc::Default};
    FColor statusbar_separator_fg{fc::Default};
    FColor statusbar_active_fg{fc::Default};
    FColor statusbar_active_bg{fc::Default};
    FColor statusbar_active_hotkey_fg{fc::Default};
    FColor statusbar_active_hotkey_bg{fc::Default};
    FColor scrollbar_fg{fc::Default};
    FColor scrollbar_bg{fc::Default};
    FColor scrollbar_button_fg{fc::Default};
    FColor scrollbar_button_bg{fc::Default};
    FColor scrollbar_button_inactive_fg{fc::Default};
    FColor scrollbar_button_inactive_bg{fc::Default};
    FColor progressbar_fg{fc::Default};
    FColor progressbar_bg{fc::Default};
};


//----------------------------------------------------------------------
// class default8ColorTheme
//----------------------------------------------------------------------

class default8ColorTheme final : public FWidgetColors
{
  public:
    // Constructor
    default8ColorTheme();

    // Destructor
    virtual ~default8ColorTheme() override;

    // Method
    void setColorTheme() override;
};


//----------------------------------------------------------------------
// class default16ColorTheme
//----------------------------------------------------------------------

class default16ColorTheme final : public FWidgetColors
{
  public:
    // Constructor
    default16ColorTheme();

    // Destructor
    virtual ~default16ColorTheme() override;

    // Method
    void setColorTheme() override;
};

}  // namespace finalcut

#endif  // FWIDGETCOLORS_H
