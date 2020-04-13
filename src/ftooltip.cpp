/***********************************************************************
* ftooltip.cpp - Widget FToolTip                                       *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2016-2020 Markus Gans                                      *
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

#include "final/fapplication.h"
#include "final/ftooltip.h"
#include "final/fwidgetcolors.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FToolTip
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FToolTip::FToolTip (FWidget* parent)
  : FWindow(parent)
{
  init();
}

//----------------------------------------------------------------------
FToolTip::FToolTip (const FString& txt, FWidget* parent)
  : FWindow(parent)
  , text(txt)
{
  init();
}

//----------------------------------------------------------------------
FToolTip::~FToolTip()  // destructor
{
  if ( FApplication::isQuit() )
    return;

  FWindow* parent_win{nullptr};

  if ( auto parent = getParentWidget() )
    parent_win = getWindowWidget(parent);

  if ( parent_win )
    setActiveWindow (parent_win);
  else
    switchToPrevWindow(this);
}


// public methods of FToolTip
//----------------------------------------------------------------------
void FToolTip::setText (const FString& txt)
{
  text.setString(txt);
  calculateDimensions();
}

//----------------------------------------------------------------------
bool FToolTip::setBorder (bool enable)
{
  setFlags().no_border = ! enable;
  calculateDimensions();
  return hasBorder();
}

//----------------------------------------------------------------------
void FToolTip::show()
{
  if ( ! isVisible() )
    return;

  FWindow::show();
}

//----------------------------------------------------------------------
void FToolTip::hide()
{
  FWindow::hide();
}

//----------------------------------------------------------------------
void FToolTip::onMouseDown (FMouseEvent*)
{
  setClickedWidget(nullptr);
  close();
}


// private methods of FToolTip
//----------------------------------------------------------------------
void FToolTip::init()
{
  setAlwaysOnTop();
  ignorePadding();
  // initialize geometry values
  setGeometry (FPoint(1, 1), FSize(3, 3), false);
  setMinimumSize (FSize(3, 3));
  const auto& wc = getFWidgetColors();
  setForegroundColor (wc.tooltip_fg);
  setBackgroundColor (wc.tooltip_bg);
  calculateDimensions();
}

//----------------------------------------------------------------------
void FToolTip::draw()
{
  bool border = hasBorder();
  int y{( border ) ? 2 : 1};
  int x{( border ) ? 3 : 2};
  setColor();
  clearArea();

  if ( border )
    drawBorder();

  for (auto&& line : text_components)
  {
    print() << FPoint(x, y) << line;
    y++;
  }
}

//----------------------------------------------------------------------
void FToolTip::calculateDimensions()
{
  max_line_width = 0;
  text_components = text.split("\n");
  text_num_lines = std::size_t(text_components.size());

  if ( text_num_lines == 0 )
    return;

  for (auto&& line : text_components)
  {
    std::size_t column_width = getColumnWidth(line);

    if ( column_width > max_line_width )
      max_line_width = column_width;
  }

  int x{};
  int y{};
  const std::size_t h = ( hasBorder() ) ? text_num_lines + 2 : text_num_lines;
  const std::size_t w = ( hasBorder() ) ? max_line_width + 4 : max_line_width + 2;
  const auto& r = getRootWidget();

  if ( r )
  {
    x = 1 + int((r->getWidth() - w) / 2);
    y = 1 + int((r->getHeight() - h) / 2);
  }
  else
    x = y = 1;

  setGeometry (FPoint(x, y), FSize(w, h));
}

//----------------------------------------------------------------------
void FToolTip::adjustSize()
{
  calculateDimensions();
  FWindow::adjustSize();
}

}  // namespace finalcut
