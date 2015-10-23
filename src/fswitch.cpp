// File: fswitch.cpp
// Provides: class FSwitch

#include "fswitch.h"


//----------------------------------------------------------------------
// class FSwitch
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FSwitch::FSwitch(FWidget* parent)
  : FToggleButton(parent)
  , switch_offset_pos(0)
  , button_pressed(false)
{
  button_width = 11;
}

//----------------------------------------------------------------------
FSwitch::FSwitch ( const FString& txt, FWidget* parent )
  : FToggleButton(txt, parent)
  , switch_offset_pos(0)
  , button_pressed(false)
{
  switch_offset_pos = int(txt.getLength()) + 1;
  button_width = 11;
}

//----------------------------------------------------------------------
FSwitch::~FSwitch()  // destructor
{ }


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

  wchar_t on[6] = L"  On ";
  wchar_t off[6] = L" Off ";
  gotoxy (xpos+xmin-1+switch_offset_pos, ypos+ymin-1);

  if ( checked )
  {
    if ( hasFocus() && ! button_pressed )
    {
      if ( isMonochron() )
      {
        wcsncpy ( on, L" <On>", 6);
        setBold(true);
      }
      else if ( getMaxColor() < 16 )
      {
        setBold(true);
        setColor (wc.button_active_focus_fg, wc.button_active_focus_bg);
      }
      else
        setColor (wc.button_hotkey_fg, wc.button_active_focus_bg);
    }
    else
      if ( isMonochron() || getMaxColor() < 16 )
        setColor (wc.button_active_focus_fg, wc.button_active_bg);
      else
        setColor (wc.button_hotkey_fg, wc.button_active_bg);

    if ( isMonochron() )
      setReverse(false);

    print (on);

    if ( isMonochron() )
      setReverse(true);

    if ( isMonochron() || getMaxColor() < 16 )
      setBold(false);

    setColor (wc.button_inactive_fg, wc.button_inactive_bg);
    print (off);
    if ( isMonochron() )
      setReverse(false);
    setCursorPos ( xpos + xmin + 1 + switch_offset_pos
                 , ypos + ymin - 1 );
  }
  else
  {
    setColor (wc.button_inactive_fg, wc.button_inactive_bg);

    if ( isMonochron() )
      setReverse(true);

    print (on);

    if ( hasFocus() && ! button_pressed )
    {
      if ( isMonochron() )
      {
        wcsncpy ( off, L"<Off>", 6);
        setBold(true);
      }
      else if ( getMaxColor() < 16 )
      {
        setBold(true);
        setColor (wc.button_active_focus_fg, wc.button_active_focus_bg);
      }
      else
        setColor (wc.button_hotkey_fg, wc.button_active_focus_bg);
    }
    else
      if ( isMonochron() || getMaxColor() < 16 )
        setColor (wc.button_active_focus_fg, wc.button_active_bg);
      else
        setColor (wc.button_hotkey_fg, wc.button_active_bg);

    if ( isMonochron() )
      setReverse(false);

    print (off);

    if ( isMonochron() || getMaxColor() < 16 )
      setBold(false);

    setCursorPos ( xpos + xmin + 5 + switch_offset_pos
                 , ypos + ymin - 1 );
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
void FSwitch::onKeyPress (FKeyEvent* ev)
{
  switch ( ev->key() )
  {
    case fc::Fkey_home:
    case fc::Fkey_left:
      setChecked();
      ev->accept();
      break;

    case fc::Fkey_end:
    case fc::Fkey_right:
      unsetChecked();
      ev->accept();
      break;

    default:
      break;
  }

  if ( ev->isAccepted() )
    draw();
  else
    FToggleButton::onKeyPress(ev);
}

//----------------------------------------------------------------------
void FSwitch::onMouseDown (FMouseEvent* ev)
{
  FToggleButton::onMouseDown(ev);

  if ( ev->getButton() != LeftButton )
    return;

  button_pressed = true;
  draw();
}

//----------------------------------------------------------------------
void FSwitch::onMouseUp (FMouseEvent* ev)
{
  FToggleButton::onMouseUp(ev);

  if ( ev->getButton() != LeftButton )
    return;

  button_pressed = false;
  draw();
}
