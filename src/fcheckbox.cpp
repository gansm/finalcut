// File: fcheckbox.cpp
// Provides: class FCheckBox

#include "final/fcheckbox.h"


//----------------------------------------------------------------------
// class FCheckBox
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FCheckBox::FCheckBox(FWidget* parent)
  : FToggleButton(parent)
{
  init();
}

//----------------------------------------------------------------------
FCheckBox::FCheckBox (const FString& txt, FWidget* parent)
  : FToggleButton(txt, parent)
{
  init();
}

//----------------------------------------------------------------------
FCheckBox::~FCheckBox()  // destructor
{ }


// private methods of FCheckBox
//----------------------------------------------------------------------
void FCheckBox::init()
{
  label_offset_pos = 4;
  button_width = 4;
  setVisibleCursor();
}

//----------------------------------------------------------------------
void FCheckBox::draw()
{
  drawCheckButton();
  drawLabel();
  FToggleButton::draw();
}

//----------------------------------------------------------------------
void FCheckBox::drawCheckButton()
{
  if ( ! isVisible() )
    return;

  setPrintPos (1,1);
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
      print (CHECKBOX_ON);
    else
    {
      print ('[');
      print (fc::Times);  // Times ×
      print (']');
    }
  }
  else
  {
    if ( isNewFont() )
      print (CHECKBOX);
    else
    {
      print ('[');
      print (' ');
      print (']');
    }
  }

  if ( isMonochron() )
    setReverse(false);
}
