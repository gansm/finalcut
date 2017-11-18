/***********************************************************************
* fprogressbar.cpp - Widget FProgressbar                               *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2014-2017 Markus Gans                                      *
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


//----------------------------------------------------------------------
// class FProgressbar
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FProgressbar::FProgressbar(FWidget* parent)
  : FWidget(parent)
  , percentage(-1)
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
void FProgressbar::setPercentage (int percentage_value)
{
  if ( percentage_value <= percentage )
    return;

  if ( percentage_value > 100 )
    percentage = 100;
  else if ( percentage_value < 0 )
    percentage = 0;
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
void FProgressbar::setGeometry (int x, int y, int w, int h, bool adjust)
{
  // Set the progress bar geometry

  FWidget::setGeometry (x, y, w, h, adjust);
  bar_length = w;
}

//----------------------------------------------------------------------
bool FProgressbar::setShadow (bool on)
{
  if ( on
      && term_encoding != fc::VT100
      && term_encoding != fc::ASCII )
  {
    flags |= fc::shadow;
    setShadowSize(1,1);
  }
  else
  {
    flags &= ~fc::shadow;
    setShadowSize(0,0);
  }

  return on;
}

//----------------------------------------------------------------------
void FProgressbar::hide()
{
  int s, size;
  short fg, bg;
  char* blank;
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

  if ( size < 0 )
    return;

  try
  {
    blank = new char[size + 1];
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
    return;
  }

  std::memset(blank, ' ', uLong(size));
  blank[size] = '\0';

  for (int y = 0; y < getHeight() + s; y++)
  {
    setPrintPos (1, 1 + y);
    print (blank);
  }

  delete[] blank;
  setPrintPos (getWidth() - 4, 0);
  print ("     ");  // hide percentage
}

//----------------------------------------------------------------------
void FProgressbar::reset()
{
  percentage = -1;

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

  if ( (flags & fc::shadow) != 0 )
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

  setPrintPos (getWidth() - 3, 0);

  if ( percentage < 0 || percentage > 100 )
    print ("--- %");
  else
    printf ("%3d %%", percentage);

  if ( isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
void FProgressbar::drawBar()
{
  int i = 0;
  double length = double(bar_length * percentage) / 100;
  setPrintPos (1,1);
  setColor ( wc.progressbar_bg
           , wc.progressbar_fg );

  if ( isMonochron() )
    setReverse(false);

  for (; i < trunc(length); i++)
    print (' ');

  if ( isMonochron() )
    setReverse(true);

  if ( percentage > 0.0f && trunc(length) < bar_length )
  {
    if ( round(length) > trunc(length)
        || getMaxColor() < 16 )
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
