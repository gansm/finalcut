/***********************************************************************
* fwidgetcolors.cpp - Set widget color theme                           *
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

#include "final/fwidgetcolors.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FWidgetColors
//----------------------------------------------------------------------

// public methods of FWidgetColors
//----------------------------------------------------------------------
void FWidgetColors::set8ColorTheme()
{
  term_fg                           = fc::Black;
  term_bg                           = fc::Blue;
  list_fg                           = fc::Black;
  list_bg                           = fc::LightGray;
  selected_list_fg                  = fc::Blue;
  selected_list_bg                  = fc::LightGray;
  dialog_fg                         = fc::Black;
  dialog_resize_fg                  = fc::Red;
  dialog_emphasis_fg                = fc::Blue;
  dialog_bg                         = fc::LightGray;
  error_box_fg                      = fc::Black;
  error_box_emphasis_fg             = fc::Red;
  error_box_bg                      = fc::LightGray;
  tooltip_fg                        = fc::LightGray;
  tooltip_bg                        = fc::Cyan;
  shadow_fg                         = fc::Black;
  shadow_bg                         = fc::LightGray;  // only for transparent shadow
  current_element_focus_fg          = fc::LightGray;
  current_element_focus_bg          = fc::Red;
  current_element_fg                = fc::LightGray;
  current_element_bg                = fc::Blue;
  current_inc_search_element_fg     = fc::Brown;
  selected_current_element_focus_fg = fc::Blue;
  selected_current_element_focus_bg = fc::Red;
  selected_current_element_fg       = fc::Cyan;
  selected_current_element_bg       = fc::Blue;
  label_fg                          = fc::Black;
  label_bg                          = fc::LightGray;
  label_inactive_fg                 = fc::Cyan;
  label_inactive_bg                 = fc::LightGray;
  label_hotkey_fg                   = fc::Red;
  label_hotkey_bg                   = fc::LightGray;
  label_emphasis_fg                 = fc::Blue;
  label_ellipsis_fg                 = fc::Black;
  inputfield_active_focus_fg        = fc::LightGray;
  inputfield_active_focus_bg        = fc::Blue;
  inputfield_active_fg              = fc::Black;
  inputfield_active_bg              = fc::Cyan;
  inputfield_inactive_fg            = fc::Black;
  inputfield_inactive_bg            = fc::LightGray;
  toggle_button_active_focus_fg     = fc::LightGray;
  toggle_button_active_focus_bg     = fc::Red;
  toggle_button_active_fg           = fc::Black;
  toggle_button_active_bg           = fc::LightGray;
  toggle_button_inactive_fg         = fc::Cyan;
  toggle_button_inactive_bg         = fc::LightGray;
  button_active_focus_fg            = fc::LightGray;
  button_active_focus_bg            = fc::Red;
  button_active_fg                  = fc::LightGray;
  button_active_bg                  = fc::Blue;
  button_inactive_fg                = fc::Black;
  button_inactive_bg                = fc::Blue;
  button_hotkey_fg                  = fc::LightGray;
  titlebar_active_fg                = fc::LightGray;
  titlebar_active_bg                = fc::Red;
  titlebar_inactive_fg              = fc::Black;
  titlebar_inactive_bg              = fc::LightGray;
  titlebar_button_fg                = fc::Black;
  titlebar_button_bg                = fc::LightGray;
  titlebar_button_focus_fg          = fc::LightGray;
  titlebar_button_focus_bg          = fc::Black;
  menu_active_focus_fg              = fc::LightGray;
  menu_active_focus_bg              = fc::Blue;
  menu_active_fg                    = fc::Black;
  menu_active_bg                    = fc::LightGray;
  menu_inactive_fg                  = fc::Cyan;
  menu_inactive_bg                  = fc::LightGray;
  menu_hotkey_fg                    = fc::Red;
  menu_hotkey_bg                    = fc::LightGray;
  statusbar_fg                      = fc::Black;
  statusbar_bg                      = fc::LightGray;
  statusbar_hotkey_fg               = fc::Red;
  statusbar_hotkey_bg               = fc::LightGray;
  statusbar_separator_fg            = fc::Black;
  statusbar_active_fg               = fc::LightGray;
  statusbar_active_bg               = fc::Black;
  statusbar_active_hotkey_fg        = fc::Red;
  statusbar_active_hotkey_bg        = fc::Black;
  scrollbar_fg                      = fc::Black;
  scrollbar_bg                      = fc::LightGray;
  scrollbar_button_fg               = fc::Black;
  scrollbar_button_bg               = fc::LightGray;
  progressbar_fg                    = fc::Blue;
  progressbar_bg                    = fc::LightGray;
}

//----------------------------------------------------------------------
void FWidgetColors::set16ColorTheme()
{
  term_fg                           = fc::Black;
  term_bg                           = fc::LightBlue;
  list_fg                           = fc::Black;
  list_bg                           = fc::White;
  selected_list_fg                  = fc::Cyan;
  selected_list_bg                  = fc::White;
  dialog_fg                         = fc::Black;
  dialog_resize_fg                  = fc::Cyan;
  dialog_emphasis_fg                = fc::Blue;
  dialog_bg                         = fc::White;
  error_box_fg                      = fc::White;
  error_box_emphasis_fg             = fc::Yellow;
  error_box_bg                      = fc::LightRed;
  tooltip_fg                        = fc::Black;
  tooltip_bg                        = fc::Yellow;
  shadow_fg                         = fc::Black;
  shadow_bg                         = fc::LightGray;  // only for transparent shadow
  current_element_focus_fg          = fc::White;
  current_element_focus_bg          = fc::Blue;
  current_element_fg                = fc::LightGray;
  current_element_bg                = fc::Blue;
  current_inc_search_element_fg     = fc::LightRed;
  selected_current_element_focus_fg = fc::LightCyan;
  selected_current_element_focus_bg = fc::Blue;
  selected_current_element_fg       = fc::LightBlue;
  selected_current_element_bg       = fc::Blue;
  label_fg                          = fc::Black;
  label_bg                          = fc::White;
  label_inactive_fg                 = fc::LightGray;
  label_inactive_bg                 = fc::White;
  label_hotkey_fg                   = fc::Red;
  label_hotkey_bg                   = fc::White;
  label_emphasis_fg                 = fc::Blue;
  label_ellipsis_fg                 = fc::DarkGray;
  inputfield_active_focus_fg        = fc::White;
  inputfield_active_focus_bg        = fc::Cyan;
  inputfield_active_fg              = fc::Black;
  inputfield_active_bg              = fc::LightGray;
  inputfield_inactive_fg            = fc::DarkGray;
  inputfield_inactive_bg            = fc::LightGray;
  toggle_button_active_focus_fg     = fc::White;
  toggle_button_active_focus_bg     = fc::Cyan;
  toggle_button_active_fg           = fc::Black;
  toggle_button_active_bg           = fc::White;
  toggle_button_inactive_fg         = fc::LightGray;
  toggle_button_inactive_bg         = fc::White;
  button_active_focus_fg            = fc::LightGray;
  button_active_focus_bg            = fc::Blue;
  button_active_fg                  = fc::LightGray;
  button_active_bg                  = fc::DarkGray;
  button_inactive_fg                = fc::DarkGray;
  button_inactive_bg                = fc::LightGray;
  button_hotkey_fg                  = fc::White;
  titlebar_active_fg                = fc::White;
  titlebar_active_bg                = fc::Blue;
  titlebar_inactive_fg              = fc::LightGray;
  titlebar_inactive_bg              = fc::DarkGray;
  titlebar_button_fg                = fc::DarkGray;
  titlebar_button_bg                = fc::LightGray;
  titlebar_button_focus_fg          = fc::LightGray;
  titlebar_button_focus_bg          = fc::Black;
  menu_active_focus_fg              = fc::White;
  menu_active_focus_bg              = fc::Blue;
  menu_active_fg                    = fc::Black;
  menu_active_bg                    = fc::White;
  menu_inactive_fg                  = fc::LightGray;
  menu_inactive_bg                  = fc::White;
  menu_hotkey_fg                    = fc::Red;
  menu_hotkey_bg                    = fc::White;
  statusbar_fg                      = fc::White;
  statusbar_bg                      = fc::Blue;
  statusbar_hotkey_fg               = fc::LightRed;
  statusbar_hotkey_bg               = fc::Blue;
  statusbar_separator_fg            = fc::Black;
  statusbar_active_fg               = fc::Blue;
  statusbar_active_bg               = fc::White;
  statusbar_active_hotkey_fg        = fc::DarkGray;
  statusbar_active_hotkey_bg        = fc::White;
  scrollbar_fg                      = fc::DarkGray;
  scrollbar_bg                      = fc::LightBlue;
  scrollbar_button_fg               = fc::Black;
  scrollbar_button_bg               = fc::LightGray;
  progressbar_fg                    = fc::DarkGray;
  progressbar_bg                    = fc::LightBlue;
}

}  // namespace finalcut
