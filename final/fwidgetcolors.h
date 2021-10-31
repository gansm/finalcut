/***********************************************************************
* fwidgetcolors.h - Set widget color theme                             *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2021 Markus Gans                                      *
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
#include "final/util/fstring.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FWidgetColors
//----------------------------------------------------------------------

class FWidgetColors
{
  public:
    // Constructor
    FWidgetColors() = default;

    // Destructor
    virtual ~FWidgetColors() noexcept;

    // Method
    virtual FString getClassName() const;
    virtual void setColorTheme() = 0;

    // Data members
    FColor term_fg{FColor::Default};
    FColor term_bg{FColor::Default};
    FColor list_fg{FColor::Default};
    FColor list_bg{FColor::Default};
    FColor selected_list_fg{FColor::Default};
    FColor selected_list_bg{FColor::Default};
    FColor current_element_focus_fg{FColor::Default};
    FColor current_element_focus_bg{FColor::Default};
    FColor current_element_fg{FColor::Default};
    FColor current_element_bg{FColor::Default};
    FColor current_inc_search_element_fg{FColor::Default};
    FColor selected_current_element_focus_fg{FColor::Default};
    FColor selected_current_element_focus_bg{FColor::Default};
    FColor selected_current_element_fg{FColor::Default};
    FColor selected_current_element_bg{FColor::Default};
    FColor label_fg{FColor::Default};
    FColor label_bg{FColor::Default};
    FColor label_inactive_fg{FColor::Default};
    FColor label_inactive_bg{FColor::Default};
    FColor label_hotkey_fg{FColor::Default};
    FColor label_hotkey_bg{FColor::Default};
    FColor label_emphasis_fg{FColor::Default};
    FColor label_ellipsis_fg{FColor::Default};
    FColor inputfield_active_focus_fg{FColor::Default};
    FColor inputfield_active_focus_bg{FColor::Default};
    FColor inputfield_active_fg{FColor::Default};
    FColor inputfield_active_bg{FColor::Default};
    FColor inputfield_inactive_fg{FColor::Default};
    FColor inputfield_inactive_bg{FColor::Default};
    FColor dialog_fg{FColor::Default};
    FColor dialog_resize_fg{FColor::Default};
    FColor dialog_emphasis_fg{FColor::Default};
    FColor dialog_bg{FColor::Default};
    FColor error_box_fg{FColor::Default};
    FColor error_box_emphasis_fg{FColor::Default};
    FColor error_box_bg{FColor::Default};
    FColor tooltip_fg{FColor::Default};
    FColor tooltip_bg{FColor::Default};
    FColor shadow_fg{FColor::Default};
    FColor shadow_bg{FColor::Default};
    FColor toggle_button_active_focus_fg{FColor::Default};
    FColor toggle_button_active_focus_bg{FColor::Default};
    FColor toggle_button_active_fg{FColor::Default};
    FColor toggle_button_active_bg{FColor::Default};
    FColor toggle_button_inactive_fg{FColor::Default};
    FColor toggle_button_inactive_bg{FColor::Default};
    FColor button_active_focus_fg{FColor::Default};
    FColor button_active_focus_bg{FColor::Default};
    FColor button_active_fg{FColor::Default};
    FColor button_active_bg{FColor::Default};
    FColor button_inactive_fg{FColor::Default};
    FColor button_inactive_bg{FColor::Default};
    FColor button_hotkey_fg{FColor::Default};
    FColor titlebar_active_fg{FColor::Default};
    FColor titlebar_active_bg{FColor::Default};
    FColor titlebar_inactive_fg{FColor::Default};
    FColor titlebar_inactive_bg{FColor::Default};
    FColor titlebar_button_fg{FColor::Default};
    FColor titlebar_button_bg{FColor::Default};
    FColor titlebar_button_focus_fg{FColor::Default};
    FColor titlebar_button_focus_bg{FColor::Default};
    FColor menu_active_focus_fg{FColor::Default};
    FColor menu_active_focus_bg{FColor::Default};
    FColor menu_active_fg{FColor::Default};
    FColor menu_active_bg{FColor::Default};
    FColor menu_inactive_fg{FColor::Default};
    FColor menu_inactive_bg{FColor::Default};
    FColor menu_hotkey_fg{FColor::Default};
    FColor menu_hotkey_bg{FColor::Default};
    FColor statusbar_fg{FColor::Default};
    FColor statusbar_bg{FColor::Default};
    FColor statusbar_hotkey_fg{FColor::Default};
    FColor statusbar_hotkey_bg{FColor::Default};
    FColor statusbar_separator_fg{FColor::Default};
    FColor statusbar_active_fg{FColor::Default};
    FColor statusbar_active_bg{FColor::Default};
    FColor statusbar_active_hotkey_fg{FColor::Default};
    FColor statusbar_active_hotkey_bg{FColor::Default};
    FColor scrollbar_fg{FColor::Default};
    FColor scrollbar_bg{FColor::Default};
    FColor scrollbar_button_fg{FColor::Default};
    FColor scrollbar_button_bg{FColor::Default};
    FColor scrollbar_button_inactive_fg{FColor::Default};
    FColor scrollbar_button_inactive_bg{FColor::Default};
    FColor progressbar_fg{FColor::Default};
    FColor progressbar_bg{FColor::Default};
};

// FWidgetColors inline functions
//----------------------------------------------------------------------
inline FString FWidgetColors::getClassName() const
{ return "FWidgetColors"; }


/*  Inheritance diagram
 *  ═══════════════════
 *
 *    ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *    ▕ FWidgetColors ▏
 *    ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *            ▲
 *            │
 *  ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *  ▕ default8ColorTheme ▏
 *  ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

//----------------------------------------------------------------------
// class default8ColorTheme
//----------------------------------------------------------------------

class default8ColorTheme final : public FWidgetColors
{
  public:
    // Constructor
    default8ColorTheme();

    // Destructor
    ~default8ColorTheme() noexcept override;

    // Method
    FString getClassName() const override;
    void setColorTheme() override;
};

// default8ColorTheme inline functions
//----------------------------------------------------------------------
inline FString default8ColorTheme::getClassName() const
{ return "default8ColorTheme"; }


/*  Inheritance diagram
 *  ═══════════════════
 *
 *     ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *     ▕ FWidgetColors ▏
 *     ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *             ▲
 *             │
 *  ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *  ▕ default16ColorTheme ▏
 *  ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

//----------------------------------------------------------------------
// class default16ColorTheme
//----------------------------------------------------------------------

class default16ColorTheme final : public FWidgetColors
{
  public:
    // Constructor
    default16ColorTheme();

    // Destructor
    ~default16ColorTheme() noexcept override;

    // Method
    FString getClassName() const override;
    void setColorTheme() override;
};

// default16ColorTheme inline functions
//----------------------------------------------------------------------
inline FString default16ColorTheme::getClassName() const
{ return "default16ColorTheme"; }


/*  Inheritance diagram
 *  ═══════════════════
 *
 *      ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *      ▕ FWidgetColors ▏
 *      ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *              ▲
 *              │
 *  ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *  ▕ default8ColorDarkTheme ▏
 *  ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

//----------------------------------------------------------------------
// class default8ColorDarkTheme
//----------------------------------------------------------------------

class default8ColorDarkTheme final : public FWidgetColors
{
  public:
    // Constructor
    default8ColorDarkTheme();

    // Destructor
    ~default8ColorDarkTheme() noexcept override;

    // Method
    FString getClassName() const override;
    void setColorTheme() override;
};

// default8ColorDarkTheme inline functions
//----------------------------------------------------------------------
inline FString default8ColorDarkTheme::getClassName() const
{ return "default8ColorDarkTheme"; }


/*  Inheritance diagram
 *  ═══════════════════
 *
 *       ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *       ▕ FWidgetColors ▏
 *       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *               ▲
 *               │
 *  ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *  ▕ default16ColorDarkTheme ▏
 *  ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

//----------------------------------------------------------------------
// class default16ColorDarkTheme
//----------------------------------------------------------------------

class default16ColorDarkTheme final : public FWidgetColors
{
  public:
    // Constructor
    default16ColorDarkTheme();

    // Destructor
    ~default16ColorDarkTheme() noexcept override;

    // Method
    FString getClassName() const override;
    void setColorTheme() override;
};

// default16ColorDarkTheme inline functions
//----------------------------------------------------------------------
inline FString default16ColorDarkTheme::getClassName() const
{ return "default16ColorDarkTheme"; }

}  // namespace finalcut

#endif  // FWIDGETCOLORS_H
