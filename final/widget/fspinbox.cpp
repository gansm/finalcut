/***********************************************************************
* fspinbox.cpp - Widget FSpinBox                                       *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2019-2023 Markus Gans                                      *
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

#include <regex>

#include "final/fevent.h"
#include "final/fwidgetcolors.h"
#include "final/util/fpoint.h"
#include "final/util/fsize.h"
#include "final/vterm/fcolorpair.h"
#include "final/widget/flabel.h"
#include "final/widget/flineedit.h"
#include "final/widget/fspinbox.h"
#include "final/widget/fstatusbar.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FSpinBox
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FSpinBox::FSpinBox (FWidget* parent)
  : FWidget{parent}
{
  init();
}

//----------------------------------------------------------------------
FSpinBox::~FSpinBox() noexcept = default;  // destructor


// public methods of FSpinBox
//----------------------------------------------------------------------
void FSpinBox::setSize (const FSize& size, bool adjust)
{
  FWidget::setSize (size, adjust);
  FSize input_field_size{size};
  input_field_size.scaleBy(-2, 0);
  input_field.setSize (input_field_size, adjust);
}

//----------------------------------------------------------------------
void FSpinBox::setGeometry ( const FPoint& pos, const FSize& size
                           , bool adjust )
{
  FWidget::setGeometry (pos, size, adjust);
  FSize input_field_size{size};
  input_field_size.scaleBy(-2, 0);
  input_field.setGeometry (FPoint{1, 1}, input_field_size, adjust);
}

//----------------------------------------------------------------------
auto FSpinBox::setEnable (bool enable) -> bool
{
  FWidget::setEnable(enable);
  input_field.setEnable(enable);
  return enable;
}

//----------------------------------------------------------------------
auto FSpinBox::setShadow (bool enable) -> bool
{
  return setWidgetShadow(this, enable);
}

//----------------------------------------------------------------------
void FSpinBox::setValue (sInt64 n)
{
  if ( n > max )
    value = max;
  else if ( n < min )
    value = min;
  else
    value = n;

  updateInputField();
}

//----------------------------------------------------------------------
void FSpinBox::setMinValue (sInt64 n)
{
  if ( n <= max )
    value = min = n;
}

//----------------------------------------------------------------------
void FSpinBox::setMaxValue (sInt64 n)
{
  if ( n >= min )
    max = n;
}

//----------------------------------------------------------------------
void FSpinBox::setRange (sInt64 m, sInt64 n)
{
  if ( m <= n )
  {
    value = min = m;
    max = n;
  }
}

//----------------------------------------------------------------------
void FSpinBox::setPrefix (const FString& text)
{
  pfix = text;
  updateInputField();
}

//----------------------------------------------------------------------
void FSpinBox::setSuffix (const FString& text)
{
  sfix = text;
  updateInputField();
}

//----------------------------------------------------------------------
void FSpinBox::hide()
{
  input_field.hide();
  FWidget::hide();
  const auto& shadow = hasShadow() ? FSize{1, 1} : FSize{0, 0};
  hideArea (getSize() + shadow);
}

//----------------------------------------------------------------------
void FSpinBox::onKeyPress (FKeyEvent* ev)
{
  if ( ! isEnabled() )
    return;

  const FKey key = ev->key();

  if ( key == FKey::Tab )
  {
    focusNextChild();
  }
  else if ( key == FKey::Back_tab )
  {
    focusPrevChild();
  }
  else if ( key == FKey::Up )
  {
    increaseValue();
    ev->accept();
  }
  else if ( key == FKey::Down )
  {
    decreaseValue();
    ev->accept();
  }
  else if ( key == FKey::Page_up )
  {
    increaseValue(10);
    ev->accept();
  }
  else if ( key == FKey::Page_down )
  {
    decreaseValue(10);
    ev->accept();
  }

  if ( ev->isAccepted() )
    updateInputField();
}

//----------------------------------------------------------------------
void FSpinBox::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != MouseButton::Left
    && ev->getButton() != MouseButton::Middle )
    return;

  forceFocus();

  if ( min == max )
    return;

  const int mouse_x = ev->getX();
  const int mouse_y = ev->getY();

  if ( mouse_x == int(getWidth()) - 1 && mouse_y == 1 )
  {
    spining_state = SpiningState::Down;
    decreaseValue();
    updateInputField();
    threshold_reached = false;
    addTimer(threshold_time);
  }
  else if ( mouse_x == int(getWidth()) && mouse_y == 1 )
  {
    spining_state = SpiningState::Up;
    increaseValue();
    updateInputField();
    threshold_reached = false;
    addTimer(threshold_time);
  }
  else
    delOwnTimers();
}

//----------------------------------------------------------------------
void FSpinBox::onMouseUp (FMouseEvent*)
{
  delOwnTimers();
  spining_state = SpiningState::None;
}

//----------------------------------------------------------------------
void FSpinBox::onWheel (FWheelEvent* ev)
{
  const auto& wheel = ev->getWheel();

  delOwnTimers();
  forceFocus();
  spining_state = SpiningState::None;

  if ( wheel == MouseWheel::Up )
  {
    increaseValue();
    updateInputField();
  }
  else if ( wheel == MouseWheel::Down )
  {
    decreaseValue();
    updateInputField();
  }
}

//----------------------------------------------------------------------
void FSpinBox::onTimer (FTimerEvent*)
{
  if ( ! threshold_reached )
  {
    threshold_reached = true;
    delOwnTimers();
    addTimer(repeat_time);
  }

  switch ( spining_state )
  {
    case SpiningState::None:
      break;

    case SpiningState::Up:
      increaseValue();
      updateInputField();
      break;

    case SpiningState::Down:
      decreaseValue();
      updateInputField();
      break;

    default:
      throw std::invalid_argument{"Invalid spining state"};
  }
}

//----------------------------------------------------------------------
void FSpinBox::onFocusIn (FFocusEvent* in_ev)
{
  setWidgetFocus (&input_field);
  FWidget::onFocusIn(in_ev);
}


// private methods of FSpinBox
//----------------------------------------------------------------------
void FSpinBox::init()
{
  setShadow();
  auto parent_widget = getParentWidget();
  auto label = input_field.getLabelObject();
  label->setParent(getParent());
  label->setForegroundColor (parent_widget->getForegroundColor());
  label->setBackgroundColor (parent_widget->getBackgroundColor());
  input_field.setLabelAssociatedWidget(this);
  input_field.setInputFilter("[-[:digit:]]");  // Only numbers
  input_field.setAlignment (Align::Right);
  input_field.unsetShadow();
  input_field << value;
  input_field.addCallback
  (
    "activate",
    this, &FSpinBox::cb_inputFieldActivate
  );
  input_field.addCallback
  (
    "changed",
    this, &FSpinBox::cb_inputFieldChange,
    std::cref(input_field)
  );
}

//----------------------------------------------------------------------
void FSpinBox::draw()
{
  const auto& wc = getColorTheme();

  const FColorPair inc_button_color = [this, &wc] ()
  {
    if ( value == max )
      return FColorPair { wc->scrollbar_button_inactive_fg
                        , wc->scrollbar_button_inactive_bg };

    return FColorPair { wc->scrollbar_button_fg
                      , wc->scrollbar_button_bg };
  }();

  const FColorPair dec_button_color = [this, &wc] ()
  {
    if ( value == min )
      return FColorPair { wc->scrollbar_button_inactive_fg
                        , wc->scrollbar_button_inactive_bg };

    return FColorPair { wc->scrollbar_button_fg
                      , wc->scrollbar_button_bg };
  }();

  print() << FPoint{int(getWidth()) - 1, 1}
          << dec_button_color
          << UniChar::BlackDownPointingTriangle  // ▼
          << inc_button_color
          << UniChar::BlackUpPointingTriangle;   // ▲

  if ( getFlags().shadow.shadow )
    drawShadow(this);
}

//----------------------------------------------------------------------
inline void FSpinBox::updateInputField()
{
  input_field.clear();
  input_field << pfix << value << sfix;
  input_field.redraw();
  redraw();
}

//----------------------------------------------------------------------
inline void FSpinBox::increaseValue (sInt64 n)
{
  if ( value < max )
  {
    if ( value < max - n )
      value += n;
    else
      value = max;

    processChanged();
  }
  else
    delOwnTimers();
}

//----------------------------------------------------------------------
inline void FSpinBox::decreaseValue (sInt64 n)
{
  if ( value > min )
  {
    if ( value > min + n )
      value -= n;
    else
      value = min;

    processChanged();
  }
  else
    delOwnTimers();
}

//----------------------------------------------------------------------
void FSpinBox::processActivate() const
{
  emitCallback("activate");
}

//----------------------------------------------------------------------
void FSpinBox::processChanged() const
{
  emitCallback("changed");
}

//----------------------------------------------------------------------
void FSpinBox::forceFocus()
{
  setWidgetFocus(this);
}

//----------------------------------------------------------------------
void FSpinBox::cb_inputFieldActivate() const
{
  processActivate();
}

//----------------------------------------------------------------------
void FSpinBox::cb_inputFieldChange (const FLineEdit& lineedit)
{
  if ( lineedit.getText().isEmpty() )
    value = 0;
  else
  {
    std::wsmatch match;
    const auto& regex = std::wregex(LR"([-]?[[:xdigit:]]+)");
    const auto& text = lineedit.getText().toWString();

    if ( std::regex_search(text, match, regex) )
      value = stoll(match[0]);
    else
      value = 0;
  }

  if ( value > max )
    value = max;
  else if ( value < min )
    value = min;

  updateInputField();
  processChanged();
}

}  // namespace finalcut

