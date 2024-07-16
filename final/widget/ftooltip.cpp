/***********************************************************************
* ftooltip.cpp - Widget FToolTip                                       *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2016-2024 Markus Gans                                      *
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

#include "final/fapplication.h"
#include "final/fwidgetcolors.h"
#include "final/widget/ftooltip.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FToolTip
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FToolTip::FToolTip (FWidget* parent)
  : FWindow{parent}
{
  init();
}

//----------------------------------------------------------------------
FToolTip::FToolTip (const FString& txt, FWidget* parent)
  : FWindow{parent}
  , text{txt.trim()}
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
  text.setString(text_auto_trim ? txt.trim() : txt);
  calculateDimensions();
}

//----------------------------------------------------------------------
void FToolTip::resetColors()
{
  const auto& wc = getColorTheme();
  FWidget::setForegroundColor (wc->tooltip.fg);
  FWidget::setBackgroundColor (wc->tooltip.bg);
  FWidget::resetColors();
}

//----------------------------------------------------------------------
void FToolTip::setBorder (bool enable)
{
  setFlags().feature.no_border = ! enable;
  calculateDimensions();
}

//----------------------------------------------------------------------
void FToolTip::show()
{
  if ( ! isVisible() )
    return;

  FWindow::show();
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
  FWindow::setGeometry (FPoint{1, 1}, FSize{3, 3}, false);
  setMinimumSize (FSize{3, 3});
  FToolTip::resetColors();
  calculateDimensions();
}

//----------------------------------------------------------------------
void FToolTip::draw()
{
  bool border = hasBorder();
  int y{border ? 2 : 1};
  int x{border ? 3 : 2};
  setColor();
  clearArea();

  if ( border )
    drawBorder();

  for (const auto& line : text_components)
  {
    print() << FPoint{x, y} << line;
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

  for (const auto& line : text_components)
  {
    std::size_t column_width = getColumnWidth(line);
    max_line_width = std::max(column_width, max_line_width);
  }

  bool border = hasBorder();
  const std::size_t h = border ? text_num_lines + 2 : text_num_lines;
  const std::size_t w = border ? max_line_width + 4 : max_line_width + 2;
  const auto& r = getRootWidget();
  int x = r ? 1 + int((r->getWidth() - w) / 2) : 1;
  int y = r ? 1 + int((r->getHeight() - h) / 2) : 1;
  FWidget::setGeometry (FPoint{x, y}, FSize{w, h});
}

//----------------------------------------------------------------------
void FToolTip::initLayout()
{
  calculateDimensions();
}

//----------------------------------------------------------------------
void FToolTip::adjustSize()
{
  FWindow::adjustSize();
}

}  // namespace finalcut
