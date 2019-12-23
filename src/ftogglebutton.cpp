/***********************************************************************
* ftogglebutton.cpp - Intermediate base class for a toggle button      *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2014-2019 Markus Gans                                      *
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

#include <utility>

#include "final/fapplication.h"
#include "final/fbuttongroup.h"
#include "final/fevent.h"
#include "final/fpoint.h"
#include "final/fstatusbar.h"
#include "final/ftogglebutton.h"
#include "final/fwidget.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FToggleButton
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FToggleButton::FToggleButton (FWidget* parent)
  : FWidget(parent)
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
  : FWidget(parent)
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
  delAccelerator();

  if ( hasGroup() )
    getGroup()->remove(this);
}


// public methods of FToggleButton
//----------------------------------------------------------------------
void FToggleButton::setSize (const FSize& s, bool adjust)
{
  // Set the toggle button size

  FSize size(s);
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

  FSize size(s);
  correctSize(size);
  const FRect geometry(pos, size);

  if ( hasGroup() )
    getGroup()->checkScrollSize(geometry);

  FWidget::setGeometry (pos, size, adjust);
}

//----------------------------------------------------------------------
bool FToggleButton::setNoUnderline (bool enable)
{
  return (flags.no_underline = enable);
}

//----------------------------------------------------------------------
bool FToggleButton::setEnable (bool enable)
{
  FWidget::setEnable(enable);
  const auto& wc = getFWidgetColors();

  if ( enable )
  {
    setHotkeyAccelerator();

    if ( hasFocus() )
    {
      setForegroundColor (wc.toggle_button_active_focus_fg);
      setBackgroundColor (wc.toggle_button_active_focus_bg);
    }
    else
    {
      setForegroundColor (wc.toggle_button_active_fg);
      setBackgroundColor (wc.toggle_button_active_bg);
    }
  }
  else
  {
    delAccelerator();
    setForegroundColor (wc.toggle_button_inactive_fg);
    setBackgroundColor (wc.toggle_button_inactive_bg);
  }

  return enable;
}

//----------------------------------------------------------------------
bool FToggleButton::setFocus (bool enable)
{
  FWidget::setFocus(enable);

  if ( isEnabled() )
  {
    const auto& wc = getFWidgetColors();

    if ( enable )
    {
      if ( isRadioButton()  )
        focus_inside_group = false;

      setForegroundColor (wc.toggle_button_active_focus_fg);
      setBackgroundColor (wc.toggle_button_active_focus_bg);
    }
    else
    {
      setForegroundColor (wc.toggle_button_active_fg);
      setBackgroundColor (wc.toggle_button_active_bg);
    }
  }

  return enable;
}

//----------------------------------------------------------------------
bool FToggleButton::setChecked (bool enable)
{
  if ( checked != enable )
  {
    checked = enable;
    processToggle();
  }

  return checked;
}

//----------------------------------------------------------------------
void FToggleButton::setText (const FString& txt)
{
  text.setString(txt);
  std::size_t hotkey_mark = ( getHotkey(text) ) ? 1 : 0;
  std::size_t column_width = getColumnWidth(text);
  setWidth(button_width + column_width - hotkey_mark);

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
  if ( ev->getButton() != fc::LeftButton )
    return;

  if ( hasFocus() )
    return;

  auto focused_widget = getFocusWidget();
  setFocus();

  if ( focused_widget )
    focused_widget->redraw();

  redraw();

  if ( getStatusBar() )
  {
    getStatusBar()->drawMessage();
    updateTerminal();
    flush();
  }
}

//----------------------------------------------------------------------
void FToggleButton::onMouseUp (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton )
    return;

  if ( ! getTermGeometry().contains(ev->getTermPos()) )
    return;

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
    auto focused_widget = static_cast<FWidget*>(ev->focusedWidget());

    if ( focused_widget && focused_widget->isWidget() )
    {
      setFocus();
      focused_widget->redraw();
    }
  }

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

  redraw();

  if ( getStatusBar() )
  {
    getStatusBar()->drawMessage();
    updateTerminal();
    flush();
  }

  processClick();
  ev->accept();
}

//----------------------------------------------------------------------
void FToggleButton::onFocusIn (FFocusEvent*)
{
  if ( getStatusBar() )
    getStatusBar()->drawMessage();
}

//----------------------------------------------------------------------
void FToggleButton::onFocusOut (FFocusEvent* out_ev)
{
  if ( getStatusBar() )
  {
    getStatusBar()->clearMessage();
    getStatusBar()->drawMessage();
  }

  if ( ! hasGroup() )
    return;

  if ( ! focus_inside_group && isRadioButton()  )
  {
    focus_inside_group = true;
    out_ev->ignore();

    if ( out_ev->getFocusType() == fc::FocusNextWidget )
      getGroup()->focusNextChild();

    if ( out_ev->getFocusType() == fc::FocusPreviousWidget )
      getGroup()->focusPrevChild();

    redraw();
  }
  else if ( this == getGroup()->getLastButton()
         && out_ev->getFocusType() == fc::FocusNextWidget )
  {
    out_ev->ignore();
    getGroup()->focusNextChild();
    redraw();
  }
  else if ( this == getGroup()->getFirstButton()
         && out_ev->getFocusType() == fc::FocusPreviousWidget )
  {
    out_ev->ignore();
    getGroup()->focusPrevChild();
    redraw();
  }
}


// protected methods of FToggleButton
//----------------------------------------------------------------------
void FToggleButton::setHotkeyAccelerator()
{
  setHotkeyViaString (this, text);
}

//----------------------------------------------------------------------
bool FToggleButton::isRadioButton() const
{
  return isInstanceOf("FRadioButton");
}

//----------------------------------------------------------------------
bool FToggleButton::isCheckboxButton() const
{
  return isInstanceOf("FCheckBox");
}

//----------------------------------------------------------------------
void FToggleButton::draw()
{
  if ( ! isVisible() )
    return;

  if ( flags.focus && getStatusBar() )
  {
    const auto& msg = getStatusbarMessage();
    const auto& curMsg = getStatusBar()->getMessage();

    if ( curMsg != msg )
    {
      getStatusBar()->setMessage(msg);
      getStatusBar()->drawMessage();
    }
  }

  // set the cursor to the button
  if ( isRadioButton() || isCheckboxButton() )
    setCursorPos (FPoint(2, 1));
}

//----------------------------------------------------------------------
void FToggleButton::drawLabel()
{
  if ( text.isNull() || text.isEmpty() )
    return;

  FString txt(text);
  FString label_text{};
  auto hotkeypos = finalcut::getHotkeyPos(txt, label_text);
  print() << FPoint(1 + int(label_offset_pos), 1);
  drawText (std::move(label_text), hotkeypos);
}

//----------------------------------------------------------------------
void FToggleButton::processClick()
{
  emitCallback("clicked");
}

//----------------------------------------------------------------------
void FToggleButton::processToggle()
{
  emitCallback("toggled");
}

//----------------------------------------------------------------------
void FToggleButton::onKeyPress (FKeyEvent* ev)
{
  if ( ! isEnabled() )
    return;

  FKey key = ev->key();

  switch ( key )
  {
    case fc::Fkey_return:
    case fc::Fkey_enter:
    case fc::Fkey_space:
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
      processClick();
      ev->accept();
      break;

    case fc::Fkey_down:
    case fc::Fkey_right:
      focus_inside_group = true;
      focusNextChild();
      ev->accept();
      break;

    case fc::Fkey_up:
    case fc::Fkey_left:
      focus_inside_group = true;
      focusPrevChild();
      ev->accept();
      break;

    default:
      break;
  }

  if ( ev->isAccepted() )
  {
    draw();
    updateTerminal();
  }
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
  setGeometry (FPoint(1, 1), FSize(4, 1), false);  // initialize geometry values
  const auto& wc = getFWidgetColors();

  if ( isEnabled() )
  {
    if ( hasFocus() )
    {
      setForegroundColor (wc.toggle_button_active_focus_fg);
      setBackgroundColor (wc.toggle_button_active_focus_bg);
    }
    else
    {
      setForegroundColor (wc.toggle_button_active_fg);
      setBackgroundColor (wc.toggle_button_active_bg);
    }
  }
  else  // inactive
  {
    setForegroundColor (wc.label_inactive_fg);
    setBackgroundColor (wc.label_inactive_bg);
  }
}

//----------------------------------------------------------------------
void FToggleButton::drawText (FString&& label_text, std::size_t hotkeypos)
{
  if ( isMonochron() )
    setReverse(true);

  const auto& wc = getFWidgetColors();

  if ( isEnabled() )
    setColor (wc.label_fg, wc.label_bg);
  else
    setColor (wc.label_inactive_fg, wc.label_inactive_bg);

  for (std::size_t z{0}; z < label_text.getLength(); z++)
  {
    if ( (z == hotkeypos) && flags.active )
    {
      setColor (wc.label_hotkey_fg, wc.label_hotkey_bg);

      if ( ! flags.no_underline )
        setUnderline();

      print ( label_text[z] );

      if ( ! flags.no_underline )
        unsetUnderline();

      setColor (wc.label_fg, wc.label_bg);
    }
    else
      print (label_text[z]);
  }

  if ( isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
void FToggleButton::correctSize (FSize& size)
{
  std::size_t hotkey_mark = ( getHotkey(text) ) ? 1 : 0;
  std::size_t column_width = getColumnWidth(text);
  std::size_t min_width = button_width + column_width - hotkey_mark;

  if ( size.getWidth() < min_width )
    size.setWidth(min_width);
}

}  // namespace finalcut
