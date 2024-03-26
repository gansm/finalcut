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
  setTermColors();
  setDialogColors();
  setTextColors();
  setErrorBoxColors();
  setTooltipColors();
  setShadowColors();
  setCurrentElementColors();
  setListColors();
  setLabelColors();
  setInputFieldColors();
  setToggleButtonColors();
  setButtonColors();
  setTitlebarColors();
  setMenuColors();
  setStatusbarColors();
  setScrollbarColors();
  setProgressbarColors();
}

// private methods of default8ColorTheme
//----------------------------------------------------------------------
inline void default8ColorTheme::setTermColors()
{
  term =
  {
    FColor::LightGray,  // Foreground
    FColor::Blue        // Background
  };
}

inline void default8ColorTheme::setDialogColors()
{
  dialog =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::Red,        // Resize foreground
    FColor::Blue        // Emphasis foreground
  };
}

inline void default8ColorTheme::setTextColors()
{
  text =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::LightGray,  // Selected foreground
    FColor::Black,      // Selected background
    FColor::LightGray,  // Selected focused foreground
    FColor::Blue        // Selected focused background
  };
}

inline void default8ColorTheme::setErrorBoxColors()
{
  error_box =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::Red         // Emphasis foreground
  };
}

inline void default8ColorTheme::setTooltipColors()
{
  tooltip =
  {
    FColor::Black,      // Foreground
    FColor::Cyan        // Background
  };
}

inline void default8ColorTheme::setShadowColors()
{
  shadow =
  {
    FColor::LightGray,  // Foreground (only for transparent shadow)
    FColor::Black       // Background
  };
}

inline void default8ColorTheme::setCurrentElementColors()
{
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
}

inline void default8ColorTheme::setListColors()
{
  list =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::Blue,       // Selected foreground
    FColor::LightGray   // Selected background
  };
}

inline void default8ColorTheme::setLabelColors()
{
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
}

inline void default8ColorTheme::setInputFieldColors()
{
  input_field =
  {
    FColor::Black,      // Foreground
    FColor::Cyan,       // Background
    FColor::LightGray,  // Focused foreground
    FColor::Blue,       // Focused background
    FColor::Black,      // Inactive foreground
    FColor::LightGray   // Inactive background
  };
}

inline void default8ColorTheme::setToggleButtonColors()
{
  toggle_button =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::Black,      // Focused foreground
    FColor::Cyan,       // Focused background
    FColor::Cyan,       // Inactive foreground
    FColor::LightGray   // Inactive background
  };
}

inline void default8ColorTheme::setButtonColors()
{
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
}

inline void default8ColorTheme::setTitlebarColors()
{
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
}

inline void default8ColorTheme::setMenuColors()
{
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
}

inline void default8ColorTheme::setStatusbarColors()
{
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
}

inline void default8ColorTheme::setScrollbarColors()
{
  scrollbar =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::Black,      // Button foreground
    FColor::LightGray,  // Button background
    FColor::Cyan,       // Inactive button foreground
    FColor::LightGray   // Inactive button background
  };
}

inline void default8ColorTheme::setProgressbarColors()
{
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
  setTermColors();
  setDialogColors();
  setTextColors();
  setErrorBoxColors();
  setTooltipColors();
  setShadowColors();
  setCurrentElementColors();
  setListColors();
  setLabelColors();
  setInputFieldColors();
  setToggleButtonColors();
  setButtonColors();
  setTitlebarColors();
  setMenuColors();
  setStatusbarColors();
  setScrollbarColors();
  setProgressbarColors();

  if ( ! FVTerm::getFOutput()->canChangeColorPalette()
    && FVTerm::getFOutput()->getMaxColor() > 16 )
    term.bg = FColor::SkyBlue2;
}

// private methods of default16ColorTheme
//----------------------------------------------------------------------
inline void default16ColorTheme::setTermColors()
{
  term =
  {
    FColor::Black,      // Foreground
    FColor::LightBlue   // Background
  };
}

inline void default16ColorTheme::setDialogColors()
{
  dialog =
  {
    FColor::Black,      // Foreground
    FColor::White,      // Background
    FColor::Cyan,       // Resize foreground
    FColor::Blue        // Emphasis foreground
  };
}

inline void default16ColorTheme::setTextColors()
{
  text =
  {
    FColor::Black,      // Foreground
    FColor::White,      // Background
    FColor::White,      // Selected foreground
    FColor::DarkGray,   // Selected background
    FColor::White,      // Selected focused foreground
    FColor::Blue        // Selected focused background
  };
}

inline void default16ColorTheme::setErrorBoxColors()
{
  error_box =
  {
    FColor::White,      // Foreground
    FColor::LightRed,   // Background
    FColor::Yellow      // Emphasis foreground
  };
}

inline void default16ColorTheme::setTooltipColors()
{
  tooltip =
  {
    FColor::Black,      // Foreground
    FColor::Yellow      // Background
  };
}

inline void default16ColorTheme::setShadowColors()
{
  shadow =
  {
    FColor::LightGray,  // Foreground (only for transparent shadow)
    FColor::Black       // Background
  };
}

inline void default16ColorTheme::setCurrentElementColors()
{
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
}

inline void default16ColorTheme::setListColors()
{
  list =
  {
    FColor::Black,      // Foreground
    FColor::White,      // Background
    FColor::Cyan,       // Selected foreground
    FColor::White       // Selected background
  };
}

inline void default16ColorTheme::setLabelColors()
{
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
}

inline void default16ColorTheme::setInputFieldColors()
{
  input_field =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::White,      // Focused foreground
    FColor::Cyan,       // Focused background
    FColor::DarkGray,   // Inactive foreground
    FColor::LightGray   // Inactive background
  };
}

inline void default16ColorTheme::setToggleButtonColors()
{
  toggle_button =
  {
    FColor::Black,      // Foreground
    FColor::White,      // Background
    FColor::White,      // Focused foreground
    FColor::Cyan,       // Focused background
    FColor::LightGray,  // Inactive foreground
    FColor::White       // Inactive background
  };
}

inline void default16ColorTheme::setButtonColors()
{
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
}

inline void default16ColorTheme::setTitlebarColors()
{
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
}

inline void default16ColorTheme::setMenuColors()
{
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
}

inline void default16ColorTheme::setStatusbarColors()
{
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
}

inline void default16ColorTheme::setScrollbarColors()
{
  scrollbar =
  {
    FColor::DarkGray,   // Foreground
    FColor::LightBlue,  // Background
    FColor::Black,      // Button foreground
    FColor::LightGray,  // Button background
    FColor::DarkGray,   // Inactive button foreground
    FColor::LightGray   // Inactive button background
  };
}

inline void default16ColorTheme::setProgressbarColors()
{
  progressbar =
  {
    FColor::DarkGray,   // Foreground
    FColor::LightBlue   // Background
  };
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
  setTermColors();
  setDialogColors();
  setTextColors();
  setErrorBoxColors();
  setTooltipColors();
  setShadowColors();
  setCurrentElementColors();
  setListColors();
  setLabelColors();
  setInputFieldColors();
  setToggleButtonColors();
  setButtonColors();
  setTitlebarColors();
  setMenuColors();
  setStatusbarColors();
  setScrollbarColors();
  setProgressbarColors();
}

// private methods of default8ColorDarkTheme
//----------------------------------------------------------------------
inline void default8ColorDarkTheme::setTermColors()
{
  term =
  {
    FColor::LightGray,  // Foreground
    FColor::Cyan        // Background
  };
}

inline void default8ColorDarkTheme::setDialogColors()
{
  dialog =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::Blue,       // Resize foreground
    FColor::Blue        // Emphasis foreground
  };
}

inline void default8ColorDarkTheme::setTextColors()
{
  text =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::LightGray,  // Selected foreground
    FColor::Black,      // Selected background
    FColor::LightGray,  // Selected focused foreground
    FColor::Blue        // Selected focused background
  };
}

inline void default8ColorDarkTheme::setErrorBoxColors()
{
  error_box =
  {
    FColor::LightGray,  // Foreground
    FColor::Red,        // Background
    FColor::Black       // Emphasis foreground
  };
}

inline void default8ColorDarkTheme::setTooltipColors()
{
  tooltip =
  {
    FColor::LightGray,  // Foreground
    FColor::Black       // Background
  };
}

inline void default8ColorDarkTheme::setShadowColors()
{
  shadow =
  {
    FColor::LightGray,  // Foreground (only for transparent shadow)
    FColor::Black       // Background
  };
}

inline void default8ColorDarkTheme::setCurrentElementColors()
{
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
}

inline void default8ColorDarkTheme::setListColors()
{
  list =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::Cyan,       // Selected foreground
    FColor::LightGray   // Selected background
  };
}

inline void default8ColorDarkTheme::setLabelColors()
{
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
}

inline void default8ColorDarkTheme::setInputFieldColors()
{
  input_field =
  {
    FColor::LightGray,  // Foreground
    FColor::Cyan,       // Background
    FColor::LightGray,  // Focused foreground
    FColor::Cyan,       // Focused background
    FColor::Cyan,       // Inactive foreground
    FColor::LightGray   // Inactive background
  };
}

inline void default8ColorDarkTheme::setToggleButtonColors()
{
  toggle_button =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::LightGray,  // Focused foreground
    FColor::Cyan,       // Focused background
    FColor::Cyan,       // Inactive foreground
    FColor::LightGray   // Inactive background
  };
}

inline void default8ColorDarkTheme::setButtonColors()
{
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
}

inline void default8ColorDarkTheme::setTitlebarColors()
{
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
}

inline void default8ColorDarkTheme::setMenuColors()
{
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
}

inline void default8ColorDarkTheme::setStatusbarColors()
{
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
}

inline void default8ColorDarkTheme::setScrollbarColors()
{
  scrollbar =
  {
    FColor::Cyan,       // Foreground
    FColor::LightGray,  // Background
    FColor::Black,      // Button foreground
    FColor::LightGray,  // Button background
    FColor::Cyan,       // Inactive button foreground
    FColor::LightGray   // Inactive button background
  };
}

inline void default8ColorDarkTheme::setProgressbarColors()
{
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
  setTermColors();
  setDialogColors();
  setTextColors();
  setErrorBoxColors();
  setTooltipColors();
  setShadowColors();
  setCurrentElementColors();
  setListColors();
  setLabelColors();
  setInputFieldColors();
  setToggleButtonColors();
  setButtonColors();
  setTitlebarColors();
  setMenuColors();
  setStatusbarColors();
  setScrollbarColors();
  setProgressbarColors();
}

// private methods of default16ColorDarkTheme
//----------------------------------------------------------------------
inline void default16ColorDarkTheme::setTermColors()
{
  term =
  {
    FColor::White,      // Foreground
    FColor::Cyan        // Background
  };
}

inline void default16ColorDarkTheme::setDialogColors()
{
  dialog =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::LightBlue,  // Resize foreground
    FColor::Blue        // Emphasis foreground
  };
}

inline void default16ColorDarkTheme::setTextColors()
{
  text =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::LightGray,  // Selected foreground
    FColor::DarkGray,   // Selected background
    FColor::LightGray,  // Selected focused foreground
    FColor::Blue        // Selected focused background
  };
}

inline void default16ColorDarkTheme::setErrorBoxColors()
{
  error_box =
  {
    FColor::White,      // Foreground
    FColor::Red,        // Background
    FColor::Yellow      // Emphasis foreground
  };
}

inline void default16ColorDarkTheme::setTooltipColors()
{
  tooltip =
  {
    FColor::White,      // Foreground
    FColor::Black       // Background
  };
}

inline void default16ColorDarkTheme::setShadowColors()
{
  shadow =
  {
    FColor::LightGray,  // Foreground (only for transparent shadow)
    FColor::Black       // Background
  };
}

inline void default16ColorDarkTheme::setCurrentElementColors()
{
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
}

inline void default16ColorDarkTheme::setListColors()
{
  list =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::Red,        // Selected foreground
    FColor::LightGray   // Selected background
  };
}

inline void default16ColorDarkTheme::setLabelColors()
{
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
}

inline void default16ColorDarkTheme::setInputFieldColors()
{
  input_field =
  {
    FColor::White,      // Foreground
    FColor::DarkGray,   // Background
    FColor::White,      // Focused foreground
    FColor::Cyan,       // Focused background
    FColor::DarkGray,   // Inactive foreground
    FColor::LightGray   // Inactive background
  };
}

inline void default16ColorDarkTheme::setToggleButtonColors()
{
  toggle_button =
  {
    FColor::Black,      // Foreground
    FColor::LightGray,  // Background
    FColor::White,      // Focused foreground
    FColor::Cyan,       // Focused background
    FColor::DarkGray,   // Inactive foreground
    FColor::LightGray   // Inactive background
  };
}

inline void default16ColorDarkTheme::setButtonColors()
{
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
}

inline void default16ColorDarkTheme::setTitlebarColors()
{
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
}

inline void default16ColorDarkTheme::setMenuColors()
{
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
}

inline void default16ColorDarkTheme::setStatusbarColors()
{
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
}

inline void default16ColorDarkTheme::setScrollbarColors()
{
  scrollbar =
  {
    FColor::DarkGray,   // Foreground
    FColor::LightBlue,  // Background
    FColor::Black,      // Button foreground
    FColor::LightBlue,  // Button background
    FColor::DarkGray,   // Inactive button foreground
    FColor::LightGray   // Inactive button background
  };
}

inline void default16ColorDarkTheme::setProgressbarColors()
{
  progressbar =
  {
    FColor::DarkGray,   // Foreground
    FColor::LightBlue   // Background
  };
}

}  // namespace finalcut
