/***********************************************************************
* fprogressbar.cpp - Widget FProgressbar                               *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2014-2019 Markus Gans                                      *
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

  if ( isShown() )
  {
    drawProgressLabel();
    drawProgressBar();
  }

  updateTerminal();
}

//----------------------------------------------------------------------
void FProgressbar::setGeometry ( const FPoint& pos, const FSize& size
                               , bool adjust )
{
  // Set the progress bar geometry

  FWidget::setGeometry (pos, size, adjust);
  bar_length = size.getWidth();
}

//----------------------------------------------------------------------
bool FProgressbar::setShadow (bool enable)
{
  if ( enable
    && getEncoding() != fc::VT100
    && getEncoding() != fc::ASCII )
  {
    flags.shadow = true;
    setShadowSize(FSize(1, 1));
  }
  else
  {
    flags.shadow = false;
    setShadowSize(FSize(0, 0));
  }

  return enable;
}

//----------------------------------------------------------------------
void FProgressbar::hide()
{
  FWidget::hide();
  FSize shadow = hasShadow() ? FSize(1, 1) : FSize(0, 0);
  hideSize (getSize() + shadow);
  print() << FPoint(int(getWidth()) - 4, 0)
          << ("      ");  // hide percentage
}

//----------------------------------------------------------------------
void FProgressbar::reset()
{
  percentage = NOT_SET;

  if ( isShown() )
  {
    drawProgressLabel();
    drawProgressBar();
  }

  updateTerminal();
}


// private methods of FProgressbar
//----------------------------------------------------------------------
void FProgressbar::draw()
{
  drawProgressLabel();
  drawProgressBar();

  if ( flags.shadow )
    drawShadow ();

  flush_out();
}

//----------------------------------------------------------------------
void FProgressbar::drawProgressLabel()
{
  auto parent_widget = getParentWidget();

  if ( parent_widget )
    setColor ( parent_widget->getForegroundColor()
             , parent_widget->getBackgroundColor() );
  else
    setColor ( wc.dialog_fg, wc.dialog_bg );

  if ( isMonochron() )
    setReverse(true);

  print() << FPoint(int(getWidth()) - 3, 0);

  if ( percentage > 100 )
    print ("--- %");
  else
    printf ("%3d %%", percentage);

  if ( isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
void FProgressbar::drawProgressBar()
{
  std::size_t len = 0;
  print() << FPoint(1, 1);
  setColor (wc.progressbar_bg, wc.progressbar_fg);

  if ( percentage > 0 && percentage <= 100 )
    len = drawProgressIndicator();

  drawProgressBackground(len);

  if ( isMonochron() )
    setReverse(false);

  updateTerminal();
  flush_out();
}

//----------------------------------------------------------------------
std::size_t FProgressbar::drawProgressIndicator()
{
  // Draw the progress indicator

  if ( isMonochron() )
    setReverse(false);

  double length = double(bar_length * percentage) / 100;
  auto len = std::size_t(trunc(length));
  print() << FString (len, L' ');

  if ( isMonochron() )
    setReverse(true);

  if ( len >= bar_length )
    return len;

  if ( round(length) > len || getMaxColor() < 16 )
  {
    if ( isMonochron() )
      setReverse(false);

    print(' ');

    if ( isMonochron() )
      setReverse(true);
  }
  else
  {
    setColor (wc.progressbar_fg, wc.progressbar_bg);
    print (fc::LeftHalfBlock);  // ▌
  }

  len++;
  return len;
}

//----------------------------------------------------------------------
void FProgressbar::drawProgressBackground (std::size_t len)
{
  // Draw the progress background

  std::size_t bg_len = bar_length - len;
  setColor (wc.progressbar_fg, wc.progressbar_bg);

  if ( getMaxColor() < 16 )
    print() << FString (bg_len, fc::MediumShade);  // ▒
  else
    print() << FString (bg_len, L' ');
}

}  // namespace finalcut
