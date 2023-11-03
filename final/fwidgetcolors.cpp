/***********************************************************************
* fwidgetcolors.cpp - Set widget color theme                           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2023 Markus Gans                                      *
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
  term =
  {
    FColor::LightGray,  // Foreground
    FColor::Blue        // Background
  };

  dialog =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::Red,        // Resize foreground
    FColor::Blue        // Emphasis foreground
  };

  error_box =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::Red         // Emphasis foreground
  };

  tooltip =
  {
    FColor::Black,      // Foreground
    FColor::Cyan        // Background
  };

  shadow =
  {
    FColor::LightGray,  // Foreground (only for transparent shadow)
    FColor::Black       // Background
  };

  current_element =
  {
    FColor::LightGray,  // Foreground
    FColor::Blue,       // Background
    FColor::LightGray,  // Focused foreground
    FColor::Red,        // Focused background
    FColor::Brown,      // Incremental search foreground
    FColor::Cyan,       // Selected foreground
    FColor::Blue,       // Selected background
    FColor::Blue,       // Selected focused foreground
    FColor::Red         // Selected focused background
  };

  list =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::Blue,       // Selected foreground
    FColor::LightGray   // Selected background
  };

  label =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::Cyan,       // Inactive foreground
    FColor::LightGray,  // Inactive background
    FColor::Red,        // Hotkey foreground
    FColor::LightGray,  // Hotkey background
    FColor::Blue,       // Emphasis foreground
    FColor::Black       // Ellipsis foreground
  };

  input_field =
  {
    FColor::Black,      // Foreground
    FColor::Cyan,       // Background
    FColor::LightGray,  // Focused foreground
    FColor::Blue,       // Focused background
    FColor::Black,      // Inactive foreground
    FColor::LightGray   // Inactive background
  };

  toggle_button =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::Black,      // Focused foreground
    FColor::Cyan,       // Focused background
    FColor::Cyan,       // Inactive foreground
    FColor::LightGray   // Inactive background
  };

  button =
  {
    FColor::LightGray,  // Foreground
    FColor::Blue,       // Background
    FColor::LightGray,  // Focused foreground
    FColor::Red,        // Focused background
    FColor::Black,      // Inactive foreground
    FColor::Blue,       // Inactive background
    FColor::LightGray   // Hotkey foreground
  };

  titlebar =
  {
    FColor::LightGray,  // Foreground
    FColor::Red,        // Background
    FColor::Black,      // Inactive foreground
    FColor::LightGray,  // Inactive background
    FColor::Black,      // Button foreground
    FColor::LightGray,  // Button background
    FColor::LightGray,  // Focused button foreground
    FColor::Black       // Focused button background
  };

  menu =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::LightGray,  // Focused foreground
    FColor::Blue,       // Focused background
    FColor::Cyan,       // Inactive foreground
    FColor::LightGray,  // Inactive background
    FColor::Red,        // Hotkey foreground
    FColor::LightGray   // Hotkey background
  };

  statusbar =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::LightGray,  // Focused foreground
    FColor::Black,      // Focused background
    FColor::Black,      // Separator foreground
    FColor::Red,        // Hotkey foreground
    FColor::LightGray,  // Hotkey background
    FColor::Red,        // Focused hotkey foreground
    FColor::Black       // Focused hotkey background
  };

  scrollbar =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::Black,      // Button foreground
    FColor::LightGray,  // Button background
    FColor::Cyan,       // Inactive button foreground
    FColor::LightGray   // Inactive button background
  };

  progressbar =
  {
    FColor::Blue,       // Foreground
    FColor::LightGray   // Background
  };
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
  term =
  {
    FColor::Black,      // Foreground
    FColor::LightBlue   // Background
  };

  dialog =
  {
    FColor::Black,      // Foreground
    FColor::White,      // Background
    FColor::Cyan,       // Resize foreground
    FColor::Blue        // Emphasis foreground
  };

  error_box =
  {
    FColor::White,      // Foreground
    FColor::LightRed,   // Background
    FColor::Yellow      // Emphasis foreground
  };

  tooltip =
  {
    FColor::Black,      // Foreground
    FColor::Yellow      // Background
  };

  shadow =
  {
    FColor::LightGray,  // Foreground (only for transparent shadow)
    FColor::Black       // Background
  };

  current_element =
  {
    FColor::LightGray,  // Foreground
    FColor::Blue,       // Background
    FColor::White,      // Focused foreground
    FColor::Blue,       // Focused background
    FColor::LightRed,   // Incremental search foreground
    FColor::LightBlue,  // Selected foreground
    FColor::Blue,       // Selected background
    FColor::LightCyan,  // Selected focused foreground
    FColor::Blue        // Selected focused background
  };

  list =
  {
    FColor::Black,      // Foreground
    FColor::White,      // Background
    FColor::Cyan,       // Selected foreground
    FColor::White       // Selected background
  };

  label =
  {
    FColor::Black,      // Foreground
    FColor::White,      // Background
    FColor::LightGray,  // Inactive foreground
    FColor::White,      // Inactive background
    FColor::Red,        // Hotkey foreground
    FColor::White,      // Hotkey background
    FColor::Blue,       // Emphasis foreground
    FColor::DarkGray    // Ellipsis foreground
  };

  input_field =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::White,      // Focused foreground
    FColor::Cyan,       // Focused background
    FColor::DarkGray,   // Inactive foreground
    FColor::LightGray   // Inactive background
  };

  toggle_button =
  {
    FColor::Black,      // Foreground
    FColor::White,      // Background
    FColor::White,      // Focused foreground
    FColor::Cyan,       // Focused background
    FColor::LightGray,  // Inactive foreground
    FColor::White       // Inactive background
  };

  button =
  {
    FColor::LightGray,  // Foreground
    FColor::DarkGray,   // Background
    FColor::LightGray,  // Focused foreground
    FColor::Blue,       // Focused background
    FColor::DarkGray,   // Inactive foreground
    FColor::LightGray,  // Inactive background
    FColor::White       // Hotkey foreground
  };

  titlebar =
  {
    FColor::White,      // Foreground
    FColor::Blue,       // Background
    FColor::LightGray,  // Inactive foreground
    FColor::DarkGray,   // Inactive background
    FColor::DarkGray,   // Button foreground
    FColor::LightGray,  // Button background
    FColor::LightGray,  // Focused button foreground
    FColor::Black       // Focused button background
  };

  menu =
  {
    FColor::Black,      // Foreground
    FColor::White,      // Background
    FColor::White,      // Focused foreground
    FColor::Blue,       // Focused background
    FColor::LightGray,  // Inactive foreground
    FColor::White,      // Inactive background
    FColor::Red,        // Hotkey foreground
    FColor::White       // Hotkey background
  };

  statusbar =
  {
    FColor::White,      // Foreground
    FColor::Blue,       // Background
    FColor::Blue,       // Focused foreground
    FColor::White,      // Focused background
    FColor::Black,      // Separator foreground
    FColor::LightRed,   // Hotkey foreground
    FColor::Blue,       // Hotkey background
    FColor::DarkGray,   // Focused hotkey foreground
    FColor::White       // Focused hotkey background
  };

  scrollbar =
  {
    FColor::DarkGray,   // Foreground
    FColor::LightBlue,  // Background
    FColor::Black,      // Button foreground
    FColor::LightGray,  // Button background
    FColor::DarkGray,   // Inactive button foreground
    FColor::LightGray   // Inactive button background
  };

  progressbar =
  {
    FColor::DarkGray,   // Foreground
    FColor::LightBlue   // Background
  };

  if ( ! FVTerm::getFOutput()->canChangeColorPalette()
    && FVTerm::getFOutput()->getMaxColor() > 16 )
    term.bg = FColor::SkyBlue2;
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
  term =
  {
    FColor::LightGray,  // Foreground
    FColor::Cyan        // Background
  };

  dialog =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::Blue,       // Resize foreground
    FColor::Blue        // Emphasis foreground
  };

  error_box =
  {
    FColor::LightGray,  // Foreground
    FColor::Red,        // Background
    FColor::Black       // Emphasis foreground
  };

  tooltip =
  {
    FColor::LightGray,  // Foreground
    FColor::Black       // Background
  };

  shadow =
  {
    FColor::LightGray,  // Foreground (only for transparent shadow)
    FColor::Black       // Background
  };

  current_element =
  {
    FColor::Black,      // Foreground
    FColor::Cyan,       // Background
    FColor::LightGray,  // Focused foreground
    FColor::Cyan,       // Focused background
    FColor::Red,        // Incremental search foreground
    FColor::Blue,       // Selected foreground
    FColor::Cyan,       // Selected background
    FColor::Blue,       // Selected focused foreground
    FColor::Cyan        // Selected focused background
  };

  list =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::Cyan,       // Selected foreground
    FColor::LightGray   // Selected background
  };

  label =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::Cyan,       // Inactive foreground
    FColor::LightGray,  // Inactive background
    FColor::Red,        // Hotkey foreground
    FColor::LightGray,  // Hotkey background
    FColor::Blue,       // Emphasis foreground
    FColor::Cyan        // Ellipsis foreground
  };

  input_field =
  {
    FColor::LightGray,  // Foreground
    FColor::Cyan,       // Background
    FColor::LightGray,  // Focused foreground
    FColor::Cyan,       // Focused background
    FColor::Cyan,       // Inactive foreground
    FColor::LightGray   // Inactive background
  };

  toggle_button =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::LightGray,  // Focused foreground
    FColor::Cyan,       // Focused background
    FColor::Cyan,       // Inactive foreground
    FColor::LightGray   // Inactive background
  };

  button =
  {
    FColor::LightGray,  // Foreground
    FColor::Black,      // Background
    FColor::LightGray,  // Focused foreground
    FColor::Cyan,       // Focused background
    FColor::Black,      // Inactive foreground
    FColor::Cyan,       // Inactive background
    FColor::LightGray   // Hotkey foreground
  };

  titlebar =
  {
    FColor::LightGray,  // Foreground
    FColor::Black,      // Background
    FColor::Black,      // Inactive foreground
    FColor::LightGray,  // Inactive background
    FColor::Black,      // Button foreground
    FColor::LightGray,  // Button background
    FColor::LightGray,  // Focused button foreground
    FColor::Black       // Focused button background
  };

  menu =
  {
    FColor::LightGray,  // Foreground
    FColor::Black,      // Background
    FColor::LightGray,  // Focused foreground
    FColor::Blue,       // Focused background
    FColor::LightGray,  // Inactive foreground
    FColor::Black,      // Inactive background
    FColor::Red,        // Hotkey foreground
    FColor::Black       // Hotkey background
  };

  statusbar =
  {
    FColor::LightGray,  // Foreground
    FColor::Black,      // Background
    FColor::LightGray,  // Focused foreground
    FColor::Blue,       // Focused background
    FColor::LightGray,  // Separator foreground
    FColor::Red,        // Hotkey foreground
    FColor::Black,      // Hotkey background
    FColor::Red,        // Focused hotkey foreground
    FColor::Blue        // Focused hotkey background
  };

  scrollbar =
  {
    FColor::Cyan,       // Foreground
    FColor::LightGray,  // Background
    FColor::Black,      // Button foreground
    FColor::LightGray,  // Button background
    FColor::Cyan,       // Inactive button foreground
    FColor::LightGray   // Inactive button background
  };

  progressbar =
  {
    FColor::Cyan,       // Foreground
    FColor::LightGray   // Background
  };
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
  term =
  {
    FColor::White,      // Foreground
    FColor::Cyan        // Background
  };

  dialog =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::LightBlue,  // Resize foreground
    FColor::Blue        // Emphasis foreground
  };

  error_box =
  {
    FColor::White,      // Foreground
    FColor::Red,        // Background
    FColor::Yellow      // Emphasis foreground
  };

  tooltip =
  {
    FColor::White,      // Foreground
    FColor::Black       // Background
  };

  shadow =
  {
    FColor::LightGray,  // Foreground (only for transparent shadow)
    FColor::Black       // Background
  };

  current_element =
  {
    FColor::LightBlue,  // Foreground
    FColor::Cyan,       // Background
    FColor::White,      // Focused foreground
    FColor::Cyan,       // Focused background
    FColor::LightRed,   // Incremental search foreground
    FColor::Red,        // Selected foreground
    FColor::Cyan,       // Selected background
    FColor::LightRed,   // Selected focused foreground
    FColor::Cyan        // Selected focused background
  };

  list =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::Red,        // Selected foreground
    FColor::LightGray   // Selected background
  };

  label =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::DarkGray,   // Inactive foreground
    FColor::LightGray,  // Inactive background
    FColor::Red,        // Hotkey foreground
    FColor::LightGray,  // Hotkey background
    FColor::Blue,       // Emphasis foreground
    FColor::DarkGray    // Ellipsis foreground
  };

  input_field =
  {
    FColor::White,      // Foreground
    FColor::DarkGray,   // Background
    FColor::White,      // Focused foreground
    FColor::Cyan,       // Focused background
    FColor::DarkGray,   // Inactive foreground
    FColor::LightGray   // Inactive background
  };

  toggle_button =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::White,      // Focused foreground
    FColor::Cyan,       // Focused background
    FColor::DarkGray,   // Inactive foreground
    FColor::LightGray   // Inactive background
  };

  button =
  {
    FColor::LightGray,  // Foreground
    FColor::DarkGray,   // Background
    FColor::LightGray,  // Focused foreground
    FColor::Cyan,       // Focused background
    FColor::DarkGray,   // Inactive foreground
    FColor::LightBlue,  // Inactive background
    FColor::White       // Hotkey foreground
  };

  titlebar =
  {
    FColor::White,      // Foreground
    FColor::DarkGray,   // Background
    FColor::DarkGray,   // Inactive foreground
    FColor::LightBlue,  // Inactive background
    FColor::DarkGray,   // Button foreground
    FColor::LightBlue,  // Button background
    FColor::LightGray,  // Focused button foreground
    FColor::Black       // Focused button background
  };

  menu =
  {
    FColor::White,      // Foreground
    FColor::DarkGray,   // Background
    FColor::White,      // Focused foreground
    FColor::Blue,       // Focused background
    FColor::LightGray,  // Inactive foreground
    FColor::DarkGray,   // Inactive background
    FColor::LightRed,   // Hotkey foreground
    FColor::DarkGray    // Hotkey background
  };

  statusbar =
  {
    FColor::White,      // Foreground
    FColor::DarkGray,   // Background
    FColor::White,      // Focused foreground
    FColor::Blue,       // Focused background
    FColor::LightGray,  // Separator foreground
    FColor::LightRed,   // Hotkey foreground
    FColor::DarkGray,   // Hotkey background
    FColor::LightRed,   // Focused hotkey foreground
    FColor::Blue        // Focused hotkey background
  };

  scrollbar =
  {
    FColor::DarkGray,   // Foreground
    FColor::LightBlue,  // Background
    FColor::Black,      // Button foreground
    FColor::LightBlue,  // Button background
    FColor::DarkGray,   // Inactive button foreground
    FColor::LightGray   // Inactive button background
  };

  progressbar =
  {
    FColor::DarkGray,   // Foreground
    FColor::LightBlue   // Background
  };
}

}  // namespace finalcut
