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
  init();
}

//----------------------------------------------------------------------
FSwitch::FSwitch ( const FString& txt,
                         FWidget* parent ) : FToggleButton(txt, parent)
{
  switch_offset_pos = int(txt.getLength()) + 1;
  init();
}

//----------------------------------------------------------------------
FSwitch::~FSwitch()  // destructor
{
}


// private methods of FSwitch
//----------------------------------------------------------------------
void FSwitch::init()
{
  button_width = 11;
  button_pressed = false;
}

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
    if ( hasFocus() && ! button_pressed )
      setColor (wc.button_hotkey_fg, wc.button_active_focus_bg);
    else
      setColor (wc.button_hotkey_fg, wc.button_active_bg);
    print ("  On ");
    setColor (wc.button_inactive_fg, wc.button_inactive_bg);
    print (" Off ");
    setCursorPos ( xpos + xmin + 1 + switch_offset_pos,
                   ypos + ymin - 1 );
  }
  else
  {
    setColor (wc.button_inactive_fg, wc.button_inactive_bg);
    print ("  On ");
    if ( hasFocus() && ! button_pressed )
      setColor (wc.button_hotkey_fg, wc.button_active_focus_bg);
    else
      setColor (wc.button_hotkey_fg, wc.button_active_bg);
    print (" Off ");
    setCursorPos ( xpos + xmin + 5 + switch_offset_pos,
                   ypos + ymin - 1 );
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

//----------------------------------------------------------------------
void FSwitch::onMouseDown (FMouseEvent* event)
{
  FToggleButton::onMouseDown(event);

  if ( event->getButton() != LeftButton )
    return;

  button_pressed = true;
  draw();
}

//----------------------------------------------------------------------
void FSwitch::onMouseUp (FMouseEvent* event)
{
  FToggleButton::onMouseUp(event);

  if ( event->getButton() != LeftButton )
    return;

  button_pressed = false;
  draw();
}
