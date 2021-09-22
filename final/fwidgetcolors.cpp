/***********************************************************************
* fwidgetcolors.cpp - Set widget color theme                           *
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

#include "final/fc.h"
#include "final/fwidgetcolors.h"
#include "final/output/foutput.h"
#include "final/output/tty/fterm.h"
#include "final/vterm/fvterm.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FWidgetColors
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FWidgetColors::~FWidgetColors() noexcept = default;


//----------------------------------------------------------------------
// class default8ColorTheme
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
default8ColorTheme::default8ColorTheme()
{
  default8ColorTheme::setColorTheme();
}

//----------------------------------------------------------------------
default8ColorTheme::~default8ColorTheme() noexcept = default;


// public methods of default8ColorTheme
//----------------------------------------------------------------------
void default8ColorTheme::setColorTheme()
{
  term_fg                           = FColor::Black;
  term_bg                           = FColor::Blue;
  list_fg                           = FColor::Black;
  list_bg                           = FColor::LightGray;
  selected_list_fg                  = FColor::Blue;
  selected_list_bg                  = FColor::LightGray;
  dialog_fg                         = FColor::Black;
  dialog_resize_fg                  = FColor::Red;
  dialog_emphasis_fg                = FColor::Blue;
  dialog_bg                         = FColor::LightGray;
  error_box_fg                      = FColor::Black;
  error_box_emphasis_fg             = FColor::Red;
  error_box_bg                      = FColor::LightGray;
  tooltip_fg                        = FColor::Black;
  tooltip_bg                        = FColor::Cyan;
  shadow_fg                         = FColor::Black;
  shadow_bg                         = FColor::LightGray;  // only for transparent shadow
  current_element_focus_fg          = FColor::LightGray;
  current_element_focus_bg          = FColor::Red;
  current_element_fg                = FColor::LightGray;
  current_element_bg                = FColor::Blue;
  current_inc_search_element_fg     = FColor::Brown;
  selected_current_element_focus_fg = FColor::Blue;
  selected_current_element_focus_bg = FColor::Red;
  selected_current_element_fg       = FColor::Cyan;
  selected_current_element_bg       = FColor::Blue;
  label_fg                          = FColor::Black;
  label_bg                          = FColor::LightGray;
  label_inactive_fg                 = FColor::Cyan;
  label_inactive_bg                 = FColor::LightGray;
  label_hotkey_fg                   = FColor::Red;
  label_hotkey_bg                   = FColor::LightGray;
  label_emphasis_fg                 = FColor::Blue;
  label_ellipsis_fg                 = FColor::Black;
  inputfield_active_focus_fg        = FColor::LightGray;
  inputfield_active_focus_bg        = FColor::Blue;
  inputfield_active_fg              = FColor::Black;
  inputfield_active_bg              = FColor::Cyan;
  inputfield_inactive_fg            = FColor::Black;
  inputfield_inactive_bg            = FColor::LightGray;
  toggle_button_active_focus_fg     = FColor::Black;
  toggle_button_active_focus_bg     = FColor::Cyan;
  toggle_button_active_fg           = FColor::Black;
  toggle_button_active_bg           = FColor::LightGray;
  toggle_button_inactive_fg         = FColor::Cyan;
  toggle_button_inactive_bg         = FColor::LightGray;
  button_active_focus_fg            = FColor::LightGray;
  button_active_focus_bg            = FColor::Red;
  button_active_fg                  = FColor::LightGray;
  button_active_bg                  = FColor::Blue;
  button_inactive_fg                = FColor::Black;
  button_inactive_bg                = FColor::Blue;
  button_hotkey_fg                  = FColor::LightGray;
  titlebar_active_fg                = FColor::LightGray;
  titlebar_active_bg                = FColor::Red;
  titlebar_inactive_fg              = FColor::Black;
  titlebar_inactive_bg              = FColor::LightGray;
  titlebar_button_fg                = FColor::Black;
  titlebar_button_bg                = FColor::LightGray;
  titlebar_button_focus_fg          = FColor::LightGray;
  titlebar_button_focus_bg          = FColor::Black;
  menu_active_focus_fg              = FColor::LightGray;
  menu_active_focus_bg              = FColor::Blue;
  menu_active_fg                    = FColor::Black;
  menu_active_bg                    = FColor::LightGray;
  menu_inactive_fg                  = FColor::Cyan;
  menu_inactive_bg                  = FColor::LightGray;
  menu_hotkey_fg                    = FColor::Red;
  menu_hotkey_bg                    = FColor::LightGray;
  statusbar_fg                      = FColor::Black;
  statusbar_bg                      = FColor::LightGray;
  statusbar_hotkey_fg               = FColor::Red;
  statusbar_hotkey_bg               = FColor::LightGray;
  statusbar_separator_fg            = FColor::Black;
  statusbar_active_fg               = FColor::LightGray;
  statusbar_active_bg               = FColor::Black;
  statusbar_active_hotkey_fg        = FColor::Red;
  statusbar_active_hotkey_bg        = FColor::Black;
  scrollbar_fg                      = FColor::Black;
  scrollbar_bg                      = FColor::LightGray;
  scrollbar_button_fg               = FColor::Black;
  scrollbar_button_bg               = FColor::LightGray;
  scrollbar_button_inactive_fg      = FColor::Cyan;
  scrollbar_button_inactive_bg      = FColor::LightGray;
  progressbar_fg                    = FColor::Blue;
  progressbar_bg                    = FColor::LightGray;
}


//----------------------------------------------------------------------
// class default16ColorTheme
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
default16ColorTheme::default16ColorTheme()
{
  default16ColorTheme::setColorTheme();
}

//----------------------------------------------------------------------
default16ColorTheme::~default16ColorTheme() noexcept = default;


// public methods of default16ColorTheme
//----------------------------------------------------------------------
void default16ColorTheme::setColorTheme()
{
  term_fg                           = FColor::Black;
  term_bg                           = FColor::LightBlue;
  list_fg                           = FColor::Black;
  list_bg                           = FColor::White;
  selected_list_fg                  = FColor::Cyan;
  selected_list_bg                  = FColor::White;
  dialog_fg                         = FColor::Black;
  dialog_resize_fg                  = FColor::Cyan;
  dialog_emphasis_fg                = FColor::Blue;
  dialog_bg                         = FColor::White;
  error_box_fg                      = FColor::White;
  error_box_emphasis_fg             = FColor::Yellow;
  error_box_bg                      = FColor::LightRed;
  tooltip_fg                        = FColor::Black;
  tooltip_bg                        = FColor::Yellow;
  shadow_fg                         = FColor::Black;
  shadow_bg                         = FColor::LightGray;  // only for transparent shadow
  current_element_focus_fg          = FColor::White;
  current_element_focus_bg          = FColor::Blue;
  current_element_fg                = FColor::LightGray;
  current_element_bg                = FColor::Blue;
  current_inc_search_element_fg     = FColor::LightRed;
  selected_current_element_focus_fg = FColor::LightCyan;
  selected_current_element_focus_bg = FColor::Blue;
  selected_current_element_fg       = FColor::LightBlue;
  selected_current_element_bg       = FColor::Blue;
  label_fg                          = FColor::Black;
  label_bg                          = FColor::White;
  label_inactive_fg                 = FColor::LightGray;
  label_inactive_bg                 = FColor::White;
  label_hotkey_fg                   = FColor::Red;
  label_hotkey_bg                   = FColor::White;
  label_emphasis_fg                 = FColor::Blue;
  label_ellipsis_fg                 = FColor::DarkGray;
  inputfield_active_focus_fg        = FColor::White;
  inputfield_active_focus_bg        = FColor::Cyan;
  inputfield_active_fg              = FColor::Black;
  inputfield_active_bg              = FColor::LightGray;
  inputfield_inactive_fg            = FColor::DarkGray;
  inputfield_inactive_bg            = FColor::LightGray;
  toggle_button_active_focus_fg     = FColor::White;
  toggle_button_active_focus_bg     = FColor::Cyan;
  toggle_button_active_fg           = FColor::Black;
  toggle_button_active_bg           = FColor::White;
  toggle_button_inactive_fg         = FColor::LightGray;
  toggle_button_inactive_bg         = FColor::White;
  button_active_focus_fg            = FColor::LightGray;
  button_active_focus_bg            = FColor::Blue;
  button_active_fg                  = FColor::LightGray;
  button_active_bg                  = FColor::DarkGray;
  button_inactive_fg                = FColor::DarkGray;
  button_inactive_bg                = FColor::LightGray;
  button_hotkey_fg                  = FColor::White;
  titlebar_active_fg                = FColor::White;
  titlebar_active_bg                = FColor::Blue;
  titlebar_inactive_fg              = FColor::LightGray;
  titlebar_inactive_bg              = FColor::DarkGray;
  titlebar_button_fg                = FColor::DarkGray;
  titlebar_button_bg                = FColor::LightGray;
  titlebar_button_focus_fg          = FColor::LightGray;
  titlebar_button_focus_bg          = FColor::Black;
  menu_active_focus_fg              = FColor::White;
  menu_active_focus_bg              = FColor::Blue;
  menu_active_fg                    = FColor::Black;
  menu_active_bg                    = FColor::White;
  menu_inactive_fg                  = FColor::LightGray;
  menu_inactive_bg                  = FColor::White;
  menu_hotkey_fg                    = FColor::Red;
  menu_hotkey_bg                    = FColor::White;
  statusbar_fg                      = FColor::White;
  statusbar_bg                      = FColor::Blue;
  statusbar_hotkey_fg               = FColor::LightRed;
  statusbar_hotkey_bg               = FColor::Blue;
  statusbar_separator_fg            = FColor::Black;
  statusbar_active_fg               = FColor::Blue;
  statusbar_active_bg               = FColor::White;
  statusbar_active_hotkey_fg        = FColor::DarkGray;
  statusbar_active_hotkey_bg        = FColor::White;
  scrollbar_fg                      = FColor::DarkGray;
  scrollbar_bg                      = FColor::LightBlue;
  scrollbar_button_fg               = FColor::Black;
  scrollbar_button_bg               = FColor::LightGray;
  scrollbar_button_inactive_fg      = FColor::DarkGray;
  scrollbar_button_inactive_bg      = FColor::LightGray;
  progressbar_fg                    = FColor::DarkGray;
  progressbar_bg                    = FColor::LightBlue;

  if ( ! FVTerm::getFOutput()->canChangeColorPalette()
    && FVTerm::getFOutput()->getMaxColor() > 16 )
    term_bg = FColor::SkyBlue2;
}

//----------------------------------------------------------------------
// class default8ColorDarkTheme
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
default8ColorDarkTheme::default8ColorDarkTheme()
{
  default8ColorDarkTheme::setColorTheme();
}

//----------------------------------------------------------------------
default8ColorDarkTheme::~default8ColorDarkTheme() noexcept = default;


// public methods of default8ColorDarkTheme
//----------------------------------------------------------------------
void default8ColorDarkTheme::setColorTheme()
{
  term_fg                           = FColor::LightGray;
  term_bg                           = FColor::Cyan;
  list_fg                           = FColor::Black;
  list_bg                           = FColor::LightGray;
  selected_list_fg                  = FColor::Cyan;
  selected_list_bg                  = FColor::LightGray;
  dialog_fg                         = FColor::Black;
  dialog_resize_fg                  = FColor::Blue;
  dialog_emphasis_fg                = FColor::Blue;
  dialog_bg                         = FColor::LightGray;
  error_box_fg                      = FColor::LightGray;
  error_box_emphasis_fg             = FColor::Black;
  error_box_bg                      = FColor::Red;
  tooltip_fg                        = FColor::LightGray;
  tooltip_bg                        = FColor::Black;
  shadow_fg                         = FColor::Black;
  shadow_bg                         = FColor::LightGray;  // only for transparent shadow
  current_element_focus_fg          = FColor::LightGray;
  current_element_focus_bg          = FColor::Cyan;
  current_element_fg                = FColor::Black;
  current_element_bg                = FColor::Cyan;
  current_inc_search_element_fg     = FColor::Red;
  selected_current_element_focus_fg = FColor::Cyan;
  selected_current_element_focus_bg = FColor::Cyan;
  selected_current_element_fg       = FColor::Blue;
  selected_current_element_bg       = FColor::Cyan;
  label_fg                          = FColor::Black;
  label_bg                          = FColor::LightGray;
  label_inactive_fg                 = FColor::Cyan;
  label_inactive_bg                 = FColor::LightGray;
  label_hotkey_fg                   = FColor::Red;
  label_hotkey_bg                   = FColor::LightGray;
  label_emphasis_fg                 = FColor::Blue;
  label_ellipsis_fg                 = FColor::Cyan;
  inputfield_active_focus_fg        = FColor::LightGray;
  inputfield_active_focus_bg        = FColor::Cyan;
  inputfield_active_fg              = FColor::LightGray;
  inputfield_active_bg              = FColor::Cyan;
  inputfield_inactive_fg            = FColor::Cyan;
  inputfield_inactive_bg            = FColor::LightGray;
  toggle_button_active_focus_fg     = FColor::LightGray;
  toggle_button_active_focus_bg     = FColor::Cyan;
  toggle_button_active_fg           = FColor::Black;
  toggle_button_active_bg           = FColor::LightGray;
  toggle_button_inactive_fg         = FColor::Cyan;
  toggle_button_inactive_bg         = FColor::LightGray;
  button_active_focus_fg            = FColor::LightGray;
  button_active_focus_bg            = FColor::Cyan;
  button_active_fg                  = FColor::LightGray;
  button_active_bg                  = FColor::Black;
  button_inactive_fg                = FColor::Black;
  button_inactive_bg                = FColor::Cyan;
  button_hotkey_fg                  = FColor::LightGray;
  titlebar_active_fg                = FColor::LightGray;
  titlebar_active_bg                = FColor::Black;
  titlebar_inactive_fg              = FColor::Black;
  titlebar_inactive_bg              = FColor::LightGray;
  titlebar_button_fg                = FColor::Black;
  titlebar_button_bg                = FColor::LightGray;
  titlebar_button_focus_fg          = FColor::LightGray;
  titlebar_button_focus_bg          = FColor::Black;
  menu_active_focus_fg              = FColor::LightGray;
  menu_active_focus_bg              = FColor::Blue;
  menu_active_fg                    = FColor::LightGray;
  menu_active_bg                    = FColor::Black;
  menu_inactive_fg                  = FColor::LightGray;
  menu_inactive_bg                  = FColor::Black;
  menu_hotkey_fg                    = FColor::Red;
  menu_hotkey_bg                    = FColor::Black;
  statusbar_fg                      = FColor::LightGray;
  statusbar_bg                      = FColor::Black;
  statusbar_hotkey_fg               = FColor::Red;
  statusbar_hotkey_bg               = FColor::Black;
  statusbar_separator_fg            = FColor::LightGray;
  statusbar_active_fg               = FColor::LightGray;
  statusbar_active_bg               = FColor::Blue;
  statusbar_active_hotkey_fg        = FColor::Red;
  statusbar_active_hotkey_bg        = FColor::Blue;
  scrollbar_fg                      = FColor::Cyan;
  scrollbar_bg                      = FColor::LightGray;
  scrollbar_button_fg               = FColor::Black;
  scrollbar_button_bg               = FColor::LightGray;
  scrollbar_button_inactive_fg      = FColor::Cyan;
  scrollbar_button_inactive_bg      = FColor::LightGray;
  progressbar_fg                    = FColor::Cyan;
  progressbar_bg                    = FColor::LightGray;
}


//----------------------------------------------------------------------
// class default16ColorDarkTheme
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
default16ColorDarkTheme::default16ColorDarkTheme()
{
  default16ColorDarkTheme::setColorTheme();
}

//----------------------------------------------------------------------
default16ColorDarkTheme::~default16ColorDarkTheme() noexcept = default;


// public methods of default16ColorDarkTheme
//----------------------------------------------------------------------
void default16ColorDarkTheme::setColorTheme()
{
  term_fg                           = FColor::White;
  term_bg                           = FColor::Cyan;
  list_fg                           = FColor::Black;
  list_bg                           = FColor::LightGray;
  selected_list_fg                  = FColor::Red;
  selected_list_bg                  = FColor::LightGray;
  dialog_fg                         = FColor::Black;
  dialog_resize_fg                  = FColor::LightBlue;
  dialog_emphasis_fg                = FColor::Blue;
  dialog_bg                         = FColor::LightGray;
  error_box_fg                      = FColor::White;
  error_box_emphasis_fg             = FColor::Yellow;
  error_box_bg                      = FColor::Red;
  tooltip_fg                        = FColor::White;
  tooltip_bg                        = FColor::Black;
  shadow_fg                         = FColor::Black;
  shadow_bg                         = FColor::LightGray;  // only for transparent shadow
  current_element_focus_fg          = FColor::White;
  current_element_focus_bg          = FColor::Cyan;
  current_element_fg                = FColor::LightBlue;
  current_element_bg                = FColor::Cyan;
  current_inc_search_element_fg     = FColor::LightRed;
  selected_current_element_focus_fg = FColor::LightRed;
  selected_current_element_focus_bg = FColor::Cyan;
  selected_current_element_fg       = FColor::Red;
  selected_current_element_bg       = FColor::Cyan;
  label_fg                          = FColor::Black;
  label_bg                          = FColor::LightGray;
  label_inactive_fg                 = FColor::DarkGray;
  label_inactive_bg                 = FColor::LightGray;
  label_hotkey_fg                   = FColor::Red;
  label_hotkey_bg                   = FColor::LightGray;
  label_emphasis_fg                 = FColor::Blue;
  label_ellipsis_fg                 = FColor::DarkGray;
  inputfield_active_focus_fg        = FColor::White;
  inputfield_active_focus_bg        = FColor::Cyan;
  inputfield_active_fg              = FColor::White;
  inputfield_active_bg              = FColor::DarkGray;
  inputfield_inactive_fg            = FColor::DarkGray;
  inputfield_inactive_bg            = FColor::LightGray;
  toggle_button_active_focus_fg     = FColor::White;
  toggle_button_active_focus_bg     = FColor::Cyan;
  toggle_button_active_fg           = FColor::Black;
  toggle_button_active_bg           = FColor::LightGray;
  toggle_button_inactive_fg         = FColor::DarkGray;
  toggle_button_inactive_bg         = FColor::LightGray;
  button_active_focus_fg            = FColor::LightGray;
  button_active_focus_bg            = FColor::Cyan;
  button_active_fg                  = FColor::LightGray;
  button_active_bg                  = FColor::DarkGray;
  button_inactive_fg                = FColor::DarkGray;
  button_inactive_bg                = FColor::LightBlue;
  button_hotkey_fg                  = FColor::White;
  titlebar_active_fg                = FColor::White;
  titlebar_active_bg                = FColor::DarkGray;
  titlebar_inactive_fg              = FColor::DarkGray;
  titlebar_inactive_bg              = FColor::LightBlue;
  titlebar_button_fg                = FColor::DarkGray;
  titlebar_button_bg                = FColor::LightBlue;
  titlebar_button_focus_fg          = FColor::LightGray;
  titlebar_button_focus_bg          = FColor::Black;
  menu_active_focus_fg              = FColor::White;
  menu_active_focus_bg              = FColor::Blue;
  menu_active_fg                    = FColor::White;
  menu_active_bg                    = FColor::DarkGray;
  menu_inactive_fg                  = FColor::LightGray;
  menu_inactive_bg                  = FColor::DarkGray;
  menu_hotkey_fg                    = FColor::LightRed;
  menu_hotkey_bg                    = FColor::DarkGray;
  statusbar_fg                      = FColor::White;
  statusbar_bg                      = FColor::DarkGray;
  statusbar_hotkey_fg               = FColor::LightRed;
  statusbar_hotkey_bg               = FColor::DarkGray;
  statusbar_separator_fg            = FColor::LightGray;
  statusbar_active_fg               = FColor::White;
  statusbar_active_bg               = FColor::Blue;
  statusbar_active_hotkey_fg        = FColor::LightRed;
  statusbar_active_hotkey_bg        = FColor::Blue;
  scrollbar_fg                      = FColor::DarkGray;
  scrollbar_bg                      = FColor::LightBlue;
  scrollbar_button_fg               = FColor::Black;
  scrollbar_button_bg               = FColor::LightBlue;
  scrollbar_button_inactive_fg      = FColor::DarkGray;
  scrollbar_button_inactive_bg      = FColor::LightGray;
  progressbar_fg                    = FColor::DarkGray;
  progressbar_bg                    = FColor::LightBlue;
}

}  // namespace finalcut
