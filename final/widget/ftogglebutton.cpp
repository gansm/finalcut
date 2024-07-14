/***********************************************************************
* ftogglebutton.cpp - Intermediate base class for a toggle button      *
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

#include <utility>

#include "final/fapplication.h"
#include "final/fevent.h"
#include "final/fwidget.h"
#include "final/util/fpoint.h"
#include "final/widget/fbuttongroup.h"
#include "final/widget/fstatusbar.h"
#include "final/widget/ftogglebutton.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FToggleButton
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FToggleButton::FToggleButton (FWidget* parent)
  : FWidget{parent}
{
  init();

  if ( parent && parent->isInstanceOf("FButtonGroup") )
  {
    setGroup(static_cast<FButtonGroup*>(parent));

    if ( hasGroup() )
      getGroup()->insert(this);  // insert into button group
  }
}

//----------------------------------------------------------------------
FToggleButton::FToggleButton (const FString& txt, FWidget* parent)
  : FWidget{parent}
{
  FToggleButton::setText(txt);  // call own method
  init();

  if ( parent && parent->isInstanceOf("FButtonGroup") )
  {
    setGroup(static_cast<FButtonGroup*>(parent));

    if ( hasGroup() )
      getGroup()->insert(this);  // insert into button group
  }
}

//----------------------------------------------------------------------
FToggleButton::~FToggleButton()  // destructor
{
  FWidget::delAccelerator();

  if ( hasGroup() )
    getGroup()->remove(this);
}


// public methods of FToggleButton
//----------------------------------------------------------------------
void FToggleButton::setSize (const FSize& s, bool adjust)
{
  // Set the toggle button size

  FSize size{s};
  correctSize(size);
  const FRect geometry(getPos(), size);

  if ( hasGroup() )
    getGroup()->checkScrollSize(geometry);

  FWidget::setSize (size, adjust);
}

//----------------------------------------------------------------------
void FToggleButton::setGeometry ( const FPoint& pos, const FSize& s
                                , bool adjust )
{
  // Set the toggle button geometry

  FSize size{s};
  correctSize(size);
  const FRect geometry(pos, size);

  if ( hasGroup() )
    getGroup()->checkScrollSize(geometry);

  FWidget::setGeometry (pos, size, adjust);
}

//----------------------------------------------------------------------
void FToggleButton::resetColors()
{
  const auto& wc = getColorTheme();

  if ( isEnabled() )  // active
  {
    if ( hasFocus() )
    {
      FWidget::setForegroundColor (wc->toggle_button.focus_fg);
      FWidget::setBackgroundColor (wc->toggle_button.focus_bg);
    }
    else
    {
      FWidget::setForegroundColor (wc->toggle_button.fg);
      FWidget::setBackgroundColor (wc->toggle_button.bg);
    }
  }
  else  // inactive
  {
    FWidget::setForegroundColor (wc->label.inactive_fg);
    FWidget::setBackgroundColor (wc->label.inactive_bg);
  }

  FWidget::resetColors();
}

//----------------------------------------------------------------------
void FToggleButton::setNoUnderline (bool enable)
{
  setFlags().feature.no_underline = enable;
}

//----------------------------------------------------------------------
void FToggleButton::setEnable (bool enable)
{
  FWidget::setEnable(enable);
  resetColors();

  if ( enable )
    setHotkeyAccelerator();
  else
    delAccelerator();
}

//----------------------------------------------------------------------
void FToggleButton::setChecked (bool enable)
{
  if ( checked == enable )
    return;

  checked = enable;
  processToggle();
}

//----------------------------------------------------------------------
void FToggleButton::setText (const FString& txt)
{
  text.setString(txt);
  std::size_t hotkey_mark = ( getHotkey(text) != FKey::None ) ? 1 : 0;
  std::size_t column_width = getColumnWidth(text);
  FWidget::setWidth(button_width + column_width - hotkey_mark);

  if ( isEnabled() )
  {
    delAccelerator();
    setHotkeyAccelerator();
  }
}

//----------------------------------------------------------------------
void FToggleButton::hide()
{
  FWidget::hide();
  hideArea (getSize());
}

//----------------------------------------------------------------------
void FToggleButton::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != MouseButton::Left )
    return;

  setWidgetFocus(this);
}

//----------------------------------------------------------------------
void FToggleButton::onMouseUp (FMouseEvent* ev)
{
  if ( ev->getButton() != MouseButton::Left )
    return;

  if ( ! getTermGeometry().contains(ev->getTermPos()) )
    return;

  performButtonAction();
  redraw();
  processClick();
}

//----------------------------------------------------------------------
void FToggleButton::onWheel (FWheelEvent* ev)
{
  if ( ! hasGroup() )
    return;

  getGroup()->onWheel(ev);
}

//----------------------------------------------------------------------
void FToggleButton::onAccel (FAccelEvent* ev)
{
  if ( ! isEnabled() )
    return;

  if ( ! hasFocus() )
  {
    auto focused_widget = ev->focusedWidget();

    if ( focused_widget && focused_widget->isWidget() )
    {
      setFocus();
      focused_widget->redraw();
    }
  }

  performButtonAction();
  redraw();
  drawStatusBarMessage();
  processClick();
  ev->accept();
}

//----------------------------------------------------------------------
void FToggleButton::onFocusIn (FFocusEvent* in_ev)
{
  resetColors();

  if ( isEnabled() && hasFocus() && isRadioButton() )
    radiobutton_focus = true;  // Reactivate radio button focus

  FWidget::onFocusIn(in_ev);
}

//----------------------------------------------------------------------
void FToggleButton::onFocusOut (FFocusEvent* out_ev)
{
  resetColors();
  FWidget::onFocusOut(out_ev);
}


// protected methods of FToggleButton
//----------------------------------------------------------------------
void FToggleButton::setHotkeyAccelerator()
{
  setHotkeyViaString (this, text);
}

//----------------------------------------------------------------------
auto FToggleButton::isRadioButton() const -> bool
{
  return isInstanceOf("FRadioButton");
}

//----------------------------------------------------------------------
auto FToggleButton::isCheckboxButton() const -> bool
{
  return isInstanceOf("FCheckBox");
}

//----------------------------------------------------------------------
void FToggleButton::draw()
{
  if ( ! isVisible() )
    return;

  updateStatusbar (this);

  // set the cursor to the button
  if ( isRadioButton() || isCheckboxButton() )
    setCursorPos ({2, 1});
}

//----------------------------------------------------------------------
void FToggleButton::drawLabel()
{
  if ( text.isEmpty() )
    return;

  const FString txt(text);
  FString label_text{};
  auto hotkeypos = finalcut::getHotkeyPos(txt, label_text);
  print() << FPoint{1 + int(label_offset_pos), 1};
  drawText (label_text, hotkeypos);
}

//----------------------------------------------------------------------
void FToggleButton::processClick() const
{
  emitCallback("clicked");
}

//----------------------------------------------------------------------
void FToggleButton::processToggle() const
{
  emitCallback("toggled");
}

//----------------------------------------------------------------------
void FToggleButton::onKeyPress (FKeyEvent* ev)
{
  if ( ! isEnabled() )
    return;

  const auto key = ev->key();

  if ( isEnterKey(key) || key == FKey::Space )
  {
    performButtonAction();
    processClick();
    ev->accept();
  }
  else if ( key == FKey::Down || key == FKey::Right )
  {
    radiobutton_focus = false;  // Use normal focus
    focusNextChild();
    ev->accept();
  }
  else if ( key == FKey::Up || key == FKey::Left )
  {
    radiobutton_focus = false;  // Use normal focus
    focusPrevChild();
    ev->accept();
  }

  if ( ev->isAccepted() )
    draw();
}


// private methods of FToggleButton
//----------------------------------------------------------------------
void FToggleButton::setGroup (FButtonGroup* btngroup)
{
  button_group = btngroup;
}

//----------------------------------------------------------------------
void FToggleButton::init()
{
  FToggleButton::setGeometry (FPoint{1, 1}, FSize{4, 1}, false);  // initialize geometry values
  FToggleButton::resetColors();
}

//----------------------------------------------------------------------
void FToggleButton::drawText (const FString& label_text, std::size_t hotkeypos)
{
  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(true);

  const auto& wc = getColorTheme();

  if ( isEnabled() )
    setColor (wc->label.fg, wc->label.bg);
  else
    setColor (wc->label.inactive_fg, wc->label.inactive_bg);

  for (std::size_t z{0}; z < label_text.getLength(); z++)
  {
    if ( (z == hotkeypos) && getFlags().feature.active )
    {
      setColor (wc->label.hotkey_fg, wc->label.hotkey_bg);

      if ( ! getFlags().feature.no_underline )
        setUnderline();

      print ( label_text[z] );

      if ( ! getFlags().feature.no_underline )
        unsetUnderline();

      setColor (wc->label.fg, wc->label.bg);
    }
    else
      print (label_text[z]);
  }

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
void FToggleButton::performButtonAction()
{
  if ( isRadioButton() )
  {
    if ( ! checked )
    {
      checked = true;
      processToggle();
    }
  }
  else
  {
    checked = ! checked;
    processToggle();
  }
}

//----------------------------------------------------------------------
void FToggleButton::correctSize (FSize& size) const
{
  const std::size_t hotkey_mark = ( getHotkey(text) != FKey::None ) ? 1 : 0;
  const std::size_t column_width = getColumnWidth(text);
  const std::size_t min_width = button_width + column_width - hotkey_mark;

  if ( size.getWidth() < min_width )
    size.setWidth(min_width);
}

}  // namespace finalcut
