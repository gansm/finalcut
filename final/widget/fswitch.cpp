/***********************************************************************
* fswitch.cpp - Widget FSwitch                                         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2023 Markus Gans                                      *
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
FSwitch::FSwitch (FWidget* parent)
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
inline auto FSwitch::isMonoFocus() const -> bool
{
  return hasFocus()
      && ! button_pressed
      && FVTerm::getFOutput()->isMonochron();
}

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

  setReverse(false);
  setBold(false);
  setCursorPos ({3 + int(switch_offset_pos), 1});
}

//----------------------------------------------------------------------
inline void FSwitch::drawChecked()
{
  const FString onText{createOnText()};
  const FString offText{createOffText()};

  SetStyleForOn();
  print (onText);
  SetStyleForOff();
  print(offText);
}

//----------------------------------------------------------------------
inline void FSwitch::drawUnchecked()
{
  const FString onText{createOnText()};
  const FString offText{createOffText()};

  SetStyleForOn();
  print (onText);
  SetStyleForOff();
  print (offText);
}

//----------------------------------------------------------------------
inline void FSwitch::SetStyleForOn()
{
  const auto& wc = getColorTheme();
  const auto& output = FVTerm::getFOutput();
  bool is_mono = output->isMonochron();
  bool less_than_16_colors = output->getMaxColor() < 16;
  auto fg = less_than_16_colors ? wc->button.focus_fg : wc->button.hotkey_fg;

  if ( isChecked() )
  {
    if ( hasFocus() && ! button_pressed )
    {
      setBold (is_mono || less_than_16_colors);
      setColor (fg, wc->button.focus_bg);
    }
    else
      setColor (fg, wc->button.bg);

    setReverse(false);
  }
  else
  {
    setColor(wc->button.inactive_fg, wc->button.inactive_bg);
    setReverse(is_mono);
  }
}

//----------------------------------------------------------------------
inline void FSwitch::SetStyleForOff()
{
  const auto& wc = getColorTheme();
  const auto& output = FVTerm::getFOutput();
  bool is_mono = output->isMonochron();
  bool less_than_16_colors = output->getMaxColor() < 16;
  auto fg = less_than_16_colors ? wc->button.focus_fg : wc->button.hotkey_fg;

  if ( isChecked() )
  {
    setReverse(is_mono);
    setBold (! is_mono && ! less_than_16_colors);
    setColor (wc->button.inactive_fg, wc->button.inactive_bg);
  }
  else
  {
    if ( hasFocus() && ! button_pressed )
    {
      setBold (is_mono || less_than_16_colors);
      setColor (fg, wc->button.focus_bg);
    }
    else
      setColor (fg, wc->button.bg);

    setReverse(false);
  }
}

//----------------------------------------------------------------------
inline auto FSwitch::createOnText() const -> FString
{
  return ( isChecked() && isMonoFocus() ) ? L" <On>" : L"  On ";
}

//----------------------------------------------------------------------
inline auto FSwitch::createOffText() const -> FString
{
  return ( ! isChecked() && isMonoFocus() ) ? L"<Off>" : L" Off ";
}

}  // namespace finalcut
