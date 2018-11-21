/***********************************************************************
* fprogressbar.cpp - Widget FProgressbar                               *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2014-2018 Markus Gans                                      *
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

#include "final/fprogressbar.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FProgressbar
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FProgressbar::FProgressbar(FWidget* parent)
  : FWidget(parent)
  , percentage(NOT_SET)
  , bar_length(getWidth())
{
  unsetFocusable();
  setShadow();
}

//----------------------------------------------------------------------
FProgressbar::~FProgressbar()  // destructor
{ }


// public methods of FProgressbar
//----------------------------------------------------------------------
void FProgressbar::setPercentage (std::size_t percentage_value)
{
  if ( percentage_value == NOT_SET )
    percentage = NOT_SET;
  else if ( percentage_value > 100 )
    percentage = 100;
  else if ( percentage_value <= percentage && percentage != NOT_SET )
    return;
  else
    percentage = percentage_value;

  if ( isVisible() )
  {
    drawPercentage();
    drawBar();
  }

  updateTerminal();
}

//----------------------------------------------------------------------
void FProgressbar::setGeometry ( int x, int y
                               , std::size_t w, std::size_t h
                               , bool adjust )
{
  // Set the progress bar geometry

  FWidget::setGeometry (x, y, w, h, adjust);
  bar_length = w;
}

//----------------------------------------------------------------------
bool FProgressbar::setShadow (bool on)
{
  if ( on
    && getEncoding() != fc::VT100
    && getEncoding() != fc::ASCII )
  {
    flags.shadow = true;
    setShadowSize(1, 1);
  }
  else
  {
    flags.shadow = false;
    setShadowSize(0, 0);
  }

  return on;
}

//----------------------------------------------------------------------
void FProgressbar::hide()
{
  std::size_t s, size;
  FColor fg, bg;
  FWidget* parent_widget = getParentWidget();

  FWidget::hide();

  if ( parent_widget )
  {
    fg = parent_widget->getForegroundColor();
    bg = parent_widget->getBackgroundColor();
  }
  else
  {
    fg = wc.dialog_fg;
    bg = wc.dialog_bg;
  }

  setColor (fg, bg);
  s = hasShadow() ? 1 : 0;
  size = getWidth() + s;

  if ( size == 0 )
    return;

  char* blank = createBlankArray(size + 1);

  for (std::size_t y = 0; y < getHeight() + s; y++)
  {
    setPrintPos (1, 1 + int(y));
    print (blank);
  }

  destroyBlankArray (blank);
  setPrintPos (int(getWidth()) - 4, 0);
  print ("     ");  // hide percentage
}

//----------------------------------------------------------------------
void FProgressbar::reset()
{
  percentage = NOT_SET;

  if ( isVisible() )
  {
    drawPercentage();
    drawBar();
  }

  updateTerminal();
}


// private methods of FProgressbar
//----------------------------------------------------------------------
void FProgressbar::draw()
{
  drawPercentage();
  drawBar();

  if ( flags.shadow )
    drawShadow ();

  flush_out();
}

//----------------------------------------------------------------------
void FProgressbar::drawPercentage()
{
  FWidget* parent_widget = getParentWidget();

  if ( parent_widget )
    setColor ( parent_widget->getForegroundColor()
             , parent_widget->getBackgroundColor() );
  else
    setColor ( wc.dialog_fg, wc.dialog_bg );

  if ( isMonochron() )
    setReverse(true);

  setPrintPos (int(getWidth()) - 3, 0);

  if ( percentage > 100 )
    print ("--- %");
  else
    printf ("%3d %%", percentage);

  if ( isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
void FProgressbar::drawBar()
{
  std::size_t i = 0;
  double length;
  setPrintPos (1, 1);
  setColor ( wc.progressbar_bg
           , wc.progressbar_fg );

  if ( percentage == NOT_SET )
    length = double(-0/100);
  else
    length = double(bar_length * percentage) / 100;

  if ( isMonochron() )
    setReverse(false);

  for (; i < trunc(length); i++)
    print (' ');

  if ( isMonochron() )
    setReverse(true);

  if ( percentage > 0 && percentage <= 100 && trunc(length) < bar_length )
  {
    if ( round(length) > trunc(length) || getMaxColor() < 16 )
    {
      if ( isMonochron() )
      {
        setReverse(false);
        print (' ');
        setReverse(true);
      }
      else
        print (' ');
    }
    else
    {
      setColor (wc.progressbar_fg, wc.progressbar_bg);
      print (fc::LeftHalfBlock);  // ▌
    }

    i++;
  }

  setColor (wc.progressbar_fg, wc.progressbar_bg);

  if ( getMaxColor() < 16 )
  {
    for (; i < bar_length; i++)
      print (fc::MediumShade);  // ▒
  }
  else
  {
    for (; i < bar_length; i++)
      print (' ');
  }

  if ( isMonochron() )
    setReverse(false);

  updateTerminal();
  flush_out();
}

}  // namespace finalcut
