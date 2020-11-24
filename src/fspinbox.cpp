/***********************************************************************
* fspinbox.cpp - Widget FSpinBox                                       *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2019-2020 Markus Gans                                      *
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

#include "final/fcolorpair.h"
#include "final/fevent.h"
#include "final/flabel.h"
#include "final/flineedit.h"
#include "final/fpoint.h"
#include "final/fsize.h"
#include "final/fspinbox.h"
#include "final/fstatusbar.h"
#include "final/fwidgetcolors.h"

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
bool FSpinBox::setEnable (bool enable)
{
  FWidget::setEnable(enable);
  input_field.setEnable(enable);
  return enable;
}

//----------------------------------------------------------------------
bool FSpinBox::setFocus (bool enable)
{
  FWidget::setFocus(enable);
  input_field.setFocus(enable);
  return enable;
}

//----------------------------------------------------------------------
bool FSpinBox::setShadow (bool enable)
{
  if ( enable
    && FTerm::getEncoding() != fc::VT100
    && FTerm::getEncoding() != fc::ASCII )
  {
    setFlags().shadow = true;
    setShadowSize(FSize{1, 1});
  }
  else
  {
    setFlags().shadow = false;
    setShadowSize(FSize{0, 0});
  }

  return getFlags().shadow;
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
  const FSize shadow = hasShadow() ? FSize{1, 1} : FSize{0, 0};
  hideArea (getSize() + shadow);
}

//----------------------------------------------------------------------
void FSpinBox::onKeyPress (FKeyEvent* ev)
{
  if ( ! isEnabled() )
    return;

  const FKey key = ev->key();

  switch ( key )
  {
    case fc::Fkey_tab:
      focusNextChild();
      break;

    case fc::Fkey_btab:
      focusPrevChild();
      break;

    case fc::Fkey_up:
      increaseValue();
      ev->accept();
      break;

    case fc::Fkey_down:
      decreaseValue();
      ev->accept();
      break;

    default:
      break;
  }

  if ( ev->isAccepted() )
    updateInputField();
}

//----------------------------------------------------------------------
void FSpinBox::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton
    && ev->getButton() != fc::MiddleButton )
    return;

  forceFocus();

  if ( min == max )
    return;

  const int mouse_x = ev->getX();
  const int mouse_y = ev->getY();

  if ( mouse_x == int(getWidth()) - 1 && mouse_y == 1 )
  {
    spining_state = FSpinBox::spinDown;
    decreaseValue();
    updateInputField();
    threshold_reached = false;
    addTimer(threshold_time);
  }
  else if ( mouse_x == int(getWidth()) && mouse_y == 1 )
  {
    spining_state = FSpinBox::spinUp;
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
  spining_state = FSpinBox::noSpin;
}

//----------------------------------------------------------------------
void FSpinBox::onWheel (FWheelEvent* ev)
{
  const int wheel = ev->getWheel();

  delOwnTimers();
  forceFocus();
  spining_state = FSpinBox::noSpin;

  switch ( wheel )
  {
    case fc::WheelUp:
      increaseValue();
      updateInputField();
      break;

    case fc::WheelDown:
      decreaseValue();
      updateInputField();
      break;

    default:
      break;
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

  assert ( spining_state == FSpinBox::noSpin
        || spining_state == FSpinBox::spinUp
        || spining_state == FSpinBox::spinDown );

  switch ( spining_state )
  {
    case FSpinBox::noSpin:
      break;

    case FSpinBox::spinUp:
      increaseValue();
      updateInputField();
      break;

    case FSpinBox::spinDown:
      decreaseValue();
      updateInputField();
      break;
  }
}

// private methods of FSpinBox
//----------------------------------------------------------------------
void FSpinBox::init()
{
  setShadow();
  auto parent_widget = getParentWidget();
  FLabel* label = input_field.getLabelObject();
  label->setParent(getParent());
  label->setForegroundColor (parent_widget->getForegroundColor());
  label->setBackgroundColor (parent_widget->getBackgroundColor());
  input_field.setLabelAssociatedWidget(this);
  input_field.setInputFilter("[-[:digit:]]");  // Only numbers
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
    else
      return FColorPair { wc->scrollbar_button_fg
                        , wc->scrollbar_button_bg };
  }();

  const FColorPair dec_button_color = [this, &wc] ()
  {
    if ( value == min )
      return FColorPair { wc->scrollbar_button_inactive_fg
                        , wc->scrollbar_button_inactive_bg };
    else
      return FColorPair { wc->scrollbar_button_fg
                        , wc->scrollbar_button_bg };
  }();

  print() << FPoint{int(getWidth()) - 1, 1}
          << dec_button_color
          << fc::BlackDownPointingTriangle  // ▼
          << inc_button_color
          << fc::BlackUpPointingTriangle;   // ▲

  if ( getFlags().shadow )
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
inline void FSpinBox::increaseValue()
{
  if ( value < max )
  {
    value++;
    processChanged();
  }
  else
    delOwnTimers();
}

//----------------------------------------------------------------------
inline void FSpinBox::decreaseValue()
{
  if ( value > min )
  {
    value--;
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
  if ( hasFocus() )
    return;

  auto focused_widget = getFocusWidget();
  setFocus();

  if ( focused_widget )
    focused_widget->redraw();

  redraw();

  if ( getStatusBar() )
    getStatusBar()->drawMessage();
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
    std::wregex regex(L"[-]?[[:xdigit:]]+");
    std::wsmatch match;
    std::wstring text = lineedit.getText().wc_str();

    if ( std::regex_search(text, match, regex) )
    {
      const FString tmp(match[0]);
      value = tmp.toLong();
    }
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

