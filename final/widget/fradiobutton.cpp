/***********************************************************************
* fradiobutton.cpp - Widget FRadioButton                               *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2023 Markus Gans                                      *
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
#include "final/widget/fradiobutton.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FRadioButton
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FRadioButton::FRadioButton(FWidget* parent)
  : FToggleButton{parent}
{
  init();
}

//----------------------------------------------------------------------
FRadioButton::FRadioButton (const FString& txt, FWidget* parent)
  : FToggleButton{txt, parent}
{
  init();
}

//----------------------------------------------------------------------
FRadioButton::~FRadioButton() noexcept = default;  // destructor


// private methods of FRadioButton
//----------------------------------------------------------------------
void FRadioButton::init()
{
  setLabelOffsetPos(4);
  setButtonWidth(4);
  FWidget::setVisibleCursor();
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
  print() << FPoint{1, 1};
  setColor();

  if ( FVTerm::getFOutput()->isMonochron() )
  {
    if ( hasFocus() )
      setReverse(false);
    else
      setReverse(true);
  }

  if ( isChecked() )
    drawChecked();
  else
    drawUnchecked();

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
inline void FRadioButton::drawChecked()
{
  if ( FVTerm::getFOutput()->isNewFont() )
    print (CHECKED_RADIO_BUTTON);
  else
  {
    print ('(');
    print (UniChar::Bullet);  // Bullet ●
    print (')');
  }
}

//----------------------------------------------------------------------
inline void FRadioButton::drawUnchecked()
{
  if ( FVTerm::getFOutput()->isNewFont() )
    print (RADIO_BUTTON);
  else
  {
    print ('(');
    print (' ');
    print (')');
  }
}

}  // namespace finalcut
