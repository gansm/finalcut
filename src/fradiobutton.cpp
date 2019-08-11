/***********************************************************************
* fradiobutton.cpp - Widget FRadioButton                               *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
e Copyright 2014-2019 Markus Gans                                      *
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
#include "final/fradiobutton.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FRadioButton
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FRadioButton::FRadioButton(FWidget* parent)
  : FToggleButton(parent)
{
  init();
}

//----------------------------------------------------------------------
FRadioButton::FRadioButton (const FString& txt, FWidget* parent)
  : FToggleButton(txt, parent)
{
  init();
}

//----------------------------------------------------------------------
FRadioButton::~FRadioButton()  // destructor
{ }


// private methods of FRadioButton
//----------------------------------------------------------------------
void FRadioButton::init()
{
  label_offset_pos = 4;
  button_width = 4;
  setVisibleCursor();
}

//----------------------------------------------------------------------
void FRadioButton::draw()
{
  if ( ! isVisible() )
    return;

  drawRadioButton();
  drawLabel();
  FToggleButton::draw();
}

//----------------------------------------------------------------------
void FRadioButton::drawRadioButton()
{
  print() << FPoint(1, 1);
  setColor();

  if ( isMonochron() )
  {
    if ( hasFocus() )
      setReverse(false);
    else
      setReverse(true);
  }

  if ( checked )
    drawChecked();
  else
    drawUnchecked();

  if ( isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
inline void FRadioButton::drawChecked()
{
  if ( isNewFont() )
    print (CHECKED_RADIO_BUTTON);
  else
  {
    print ('(');
    print (fc::Bullet);  // Bullet ●
    print (')');
  }
}

//----------------------------------------------------------------------
inline void FRadioButton::drawUnchecked()
{
  if ( isNewFont() )
    print (RADIO_BUTTON);
  else
  {
    print ('(');
    print (' ');
    print (')');
  }
}

}  // namespace finalcut
