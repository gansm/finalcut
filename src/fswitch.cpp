/***********************************************************************
* fswitch.cpp - Widget FSwitch                                         *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2015-2018 Markus Gans                                      *
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

#include "final/fswitch.h"

namespace finalcut
{

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
FSwitch::FSwitch (const FString& txt, FWidget* parent)
  : FToggleButton(txt, parent)
  , switch_offset_pos(0)
  , button_pressed(false)
{
  switch_offset_pos = txt.getLength() + 1;
  button_width = 11;
}

//----------------------------------------------------------------------
FSwitch::~FSwitch()  // destructor
{ }


// public methods of FSwitch
//----------------------------------------------------------------------
void FSwitch::setText (const FString& txt)
{
  FToggleButton::setText(txt);
  switch_offset_pos = txt.getLength() + 1;
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

  if ( ev->getButton() != fc::LeftButton )
    return;

  button_pressed = true;
  draw();
}

//----------------------------------------------------------------------
void FSwitch::onMouseUp (FMouseEvent* ev)
{
  FToggleButton::onMouseUp(ev);

  if ( ev->getButton() != fc::LeftButton )
    return;

  button_pressed = false;
  draw();
}


// private methods of FSwitch
//----------------------------------------------------------------------
void FSwitch::draw()
{
  drawLabel();
  drawCheckButton();
  FToggleButton::draw();
  updateTerminal();
  flush_out();
}

//----------------------------------------------------------------------
void FSwitch::drawCheckButton()
{
  if ( ! isVisible() )
    return;

  setPrintPos (1 + int(switch_offset_pos), 1);

  if ( checked )
    drawChecked();
  else
    drawUnchecked();
}

//----------------------------------------------------------------------
void FSwitch::drawChecked()
{
  wchar_t on[6] = L"  On ";
  wchar_t off[6] = L" Off ";

  if ( hasFocus() && ! button_pressed )
  {
    if ( isMonochron() )
    {
      std::wcsncpy (on, L" <On>", 6);
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
  {
    if ( isMonochron() || getMaxColor() < 16 )
      setColor (wc.button_active_focus_fg, wc.button_active_bg);
    else
      setColor (wc.button_hotkey_fg, wc.button_active_bg);
  }

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

  setCursorPos (3 + int(switch_offset_pos), 1);
}

//----------------------------------------------------------------------
void FSwitch::drawUnchecked()
{
  wchar_t on[6] = L"  On ";
  wchar_t off[6] = L" Off ";

  setColor (wc.button_inactive_fg, wc.button_inactive_bg);

  if ( isMonochron() )
    setReverse(true);

  print (on);

  if ( hasFocus() && ! button_pressed )
  {
    if ( isMonochron() )
    {
      std::wcsncpy (off, L"<Off>", 6);
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
  {
    if ( isMonochron() || getMaxColor() < 16 )
      setColor (wc.button_active_focus_fg, wc.button_active_bg);
    else
      setColor (wc.button_hotkey_fg, wc.button_active_bg);
  }

  if ( isMonochron() )
    setReverse(false);

  print (off);

  if ( isMonochron() || getMaxColor() < 16 )
    setBold(false);

  setCursorPos (7 + int(switch_offset_pos), 1);
}

}  // namespace finalcut
