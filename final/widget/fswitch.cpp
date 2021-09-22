/***********************************************************************
* fswitch.cpp - Widget FSwitch                                         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2021 Markus Gans                                      *
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

#include "final/fevent.h"
#include "final/fwidgetcolors.h"
#include "final/vterm/fcolorpair.h"
#include "final/widget/fswitch.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FSwitch
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FSwitch::FSwitch(FWidget* parent)
  : FToggleButton{parent}
{
  setButtonWidth(11);
}

//----------------------------------------------------------------------
FSwitch::FSwitch (const FString& txt, FWidget* parent)
  : FToggleButton{txt, parent}
  , switch_offset_pos(txt.getLength() + 1)
{
  setButtonWidth(11);
}

//----------------------------------------------------------------------
FSwitch::~FSwitch() noexcept = default;  // destructor


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
  const auto key = ev->key();

  if ( key == FKey::Home || key == FKey::Left )
  {
    setChecked();
    ev->accept();
  }
  else if ( key == FKey::End || key == FKey::Right )
  {
    unsetChecked();
    ev->accept();
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

  if ( ev->getButton() != MouseButton::Left )
    return;

  button_pressed = true;
  draw();
}

//----------------------------------------------------------------------
void FSwitch::onMouseUp (FMouseEvent* ev)
{
  FToggleButton::onMouseUp(ev);

  if ( ev->getButton() != MouseButton::Left )
    return;

  button_pressed = false;
  draw();
}


// private methods of FSwitch
//----------------------------------------------------------------------
void FSwitch::draw()
{
  if ( ! isVisible() )
    return;

  drawLabel();
  drawCheckButton();
  FToggleButton::draw();
}

//----------------------------------------------------------------------
void FSwitch::drawCheckButton()
{
  print() << FPoint{1 + int(switch_offset_pos), 1};

  if ( isChecked() )
    drawChecked();
  else
    drawUnchecked();
}

//----------------------------------------------------------------------
inline void FSwitch::drawChecked()
{
  FString on{L"  On "};
  const FString off{L" Off "};
  const auto& wc = getColorTheme();

  if ( hasFocus() && ! button_pressed )
  {
    if ( FVTerm::getFOutput()->isMonochron() )
    {
      on.setString(L" <On>");
      setBold(true);
    }
    else if ( FVTerm::getFOutput()->getMaxColor() < 16 )
    {
      setBold(true);
      setColor (wc->button_active_focus_fg, wc->button_active_focus_bg);
    }
    else
      setColor (wc->button_hotkey_fg, wc->button_active_focus_bg);
  }
  else
  {
    if ( FVTerm::getFOutput()->isMonochron()
      || FVTerm::getFOutput()->getMaxColor() < 16 )
      setColor (wc->button_active_focus_fg, wc->button_active_bg);
    else
      setColor (wc->button_hotkey_fg, wc->button_active_bg);
  }

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);

  print (on);

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(true);

  if ( FVTerm::getFOutput()->isMonochron()
    || FVTerm::getFOutput()->getMaxColor() < 16 )
    setBold(false);

  print() << FColorPair{wc->button_inactive_fg, wc->button_inactive_bg}
          << off;

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);

  setCursorPos ({3 + int(switch_offset_pos), 1});
}

//----------------------------------------------------------------------
inline void FSwitch::drawUnchecked()
{
  const FString on{L"  On "};
  FString off{L" Off "};

  const auto& wc = getColorTheme();
  setColor (wc->button_inactive_fg, wc->button_inactive_bg);

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(true);

  print (on);

  if ( hasFocus() && ! button_pressed )
  {
    if ( FVTerm::getFOutput()->isMonochron() )
    {
      off.setString(L"<Off>");
      setBold(true);
    }
    else if ( FVTerm::getFOutput()->getMaxColor() < 16 )
    {
      setBold(true);
      setColor (wc->button_active_focus_fg, wc->button_active_focus_bg);
    }
    else
      setColor (wc->button_hotkey_fg, wc->button_active_focus_bg);
  }
  else
  {
    if ( FVTerm::getFOutput()->isMonochron()
      || FVTerm::getFOutput()->getMaxColor() < 16 )
      setColor (wc->button_active_focus_fg, wc->button_active_bg);
    else
      setColor (wc->button_hotkey_fg, wc->button_active_bg);
  }

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);

  print (off);

  if ( FVTerm::getFOutput()->isMonochron()
    || FVTerm::getFOutput()->getMaxColor() < 16 )
    setBold(false);

  setCursorPos ({7 + int(switch_offset_pos), 1});
}

}  // namespace finalcut
