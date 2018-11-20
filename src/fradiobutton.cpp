/***********************************************************************
* fradiobutton.cpp - Widget FRadioButton                               *
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
  drawRadioButton();
  drawLabel();
  FToggleButton::draw();
}

//----------------------------------------------------------------------
void FRadioButton::drawRadioButton()
{
  if ( ! isVisible() )
    return;

  setPrintPos (1, 1);
  setColor();

  if ( isMonochron() )
  {
    if ( hasFocus() )
      setReverse(false);
    else
      setReverse(true);
  }

  if ( checked )
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
  else
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

  if ( isMonochron() )
    setReverse(false);
}

}  // namespace finalcut
