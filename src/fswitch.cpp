// fswitch.cpp
// class FSwitch

#include "fswitch.h"


//----------------------------------------------------------------------
// class FSwitch
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FSwitch::FSwitch(FWidget* parent) : FToggleButton(parent)
{
  button_width = 11;
}

//----------------------------------------------------------------------
FSwitch::FSwitch ( const FString& txt,
                         FWidget* parent ) : FToggleButton(txt, parent)
{
  switch_offset_pos = int(txt.getLength()) + 1;
  button_width = 11;
}

//----------------------------------------------------------------------
FSwitch::~FSwitch()  // destructor
{
}


// private methods of FSwitch
//----------------------------------------------------------------------
void FSwitch::draw()
{
  setUpdateVTerm(false);
  drawLabel();
  drawCheckButton();
  setUpdateVTerm(true);

  FToggleButton::draw();
  updateTerminal();
  flush_out();
}

//----------------------------------------------------------------------
void FSwitch::drawCheckButton()
{
  if ( ! isVisible() )
    return;

 gotoxy (xpos+xmin-1+switch_offset_pos, ypos+ymin-1);

  if ( checked )
  {
    if ( hasFocus() )
      setColor (wc.button_hotkey_fg, wc.button_active_focus_bg);
    else
      setColor (wc.button_hotkey_fg, wc.button_active_bg);
    print ("  On ");
    setColor (wc.button_inactive_fg, wc.button_inactive_bg);
    print (" Off ");
    gotoxy (xpos+xmin+2+switch_offset_pos, ypos+ymin-1);
  }
  else
  {
    setColor (wc.button_inactive_fg, wc.button_inactive_bg);
    print ("  On ");
    if ( hasFocus() )
      setColor (wc.button_hotkey_fg, wc.button_active_focus_bg);
    else
      setColor (wc.button_hotkey_fg, wc.button_active_bg);
    print (" Off ");
    gotoxy (xpos+xmin+6+switch_offset_pos, ypos+ymin-1);
  }
}

// public methods of FSwitch
//----------------------------------------------------------------------
void FSwitch::setText (FString txt)
{
  FToggleButton::setText(txt);
  switch_offset_pos = int(txt.getLength()) + 1;
}

//----------------------------------------------------------------------
void FSwitch::onKeyPress (FKeyEvent* event)
{
  switch ( event->key() )
  {
    case fc::Fkey_home:
    case fc::Fkey_left:
      setChecked();
      event->accept();
      break;

    case fc::Fkey_end:
    case fc::Fkey_right:
      unsetChecked();
      event->accept();
      break;
  }

  if ( event->isAccepted() )
    draw();
  else
    FToggleButton::onKeyPress(event);
}
