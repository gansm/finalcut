/***********************************************************************
* ftooltip.cpp - Widget FToolTip                                       *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2016-2019 Markus Gans                                      *
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
  auto fapp = FApplication::getApplicationObject();

  if ( fapp->isQuit() )
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
void FToolTip::draw()
{
  setColor();

  if ( getMaxColor() < 16 )
    setBold();

  clearArea();
  drawBorder();

  for (std::size_t i{0}; i < text_num_lines; i++)
  {
    print() << FPoint(3, 2 + int(i)) << text_components[i];
  }

  unsetBold();
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
  const FWidgetColors& wc = getFWidgetColors();
  setForegroundColor (wc.tooltip_fg);
  setBackgroundColor (wc.tooltip_bg);
  calculateDimensions();
}

//----------------------------------------------------------------------
void FToolTip::calculateDimensions()
{
  int x{}, y{};
  auto r = getRootWidget();
  max_line_width = 0;
  text_split = text.split("\n");
  text_num_lines = uInt(text_split.size());

  if ( text_num_lines == 0 )
    return;

  for (std::size_t i{0}; i < text_num_lines; i++)
  {
    text_components = &text_split[0];
    std::size_t len = text_components[i].getLength();

    if ( len > max_line_width )
      max_line_width = len;
  }

  std::size_t h = text_num_lines + 2;
  std::size_t w = max_line_width + 4;

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
