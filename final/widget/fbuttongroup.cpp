/***********************************************************************
* fbuttongroup.cpp - The FButtonGroup widget organizes FToggleButton   *
*                    widgets in a group.                               *
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

#include "final/fapplication.h"
#include "final/fevent.h"
#include "final/util/fsize.h"
#include "final/widget/fbuttongroup.h"
#include "final/widget/fstatusbar.h"
#include "final/widget/ftogglebutton.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FButtonGroup
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FButtonGroup::FButtonGroup(FWidget* parent)
  : FScrollView{parent}
{
  init();
}

//----------------------------------------------------------------------
FButtonGroup::FButtonGroup (const FString& txt, FWidget* parent)
  : FScrollView{parent}
  , text{txt}
{
  init();
  setText(txt);
}

//----------------------------------------------------------------------
FButtonGroup::~FButtonGroup()  // destructor
{
  if ( buttonlist.empty() )
    return;

  auto iter = buttonlist.cbegin();

  while ( iter != buttonlist.cend() )
  {
    auto toggle_button = static_cast<FToggleButton*>(*iter);
    setGroup (*toggle_button, nullptr);
    iter = buttonlist.erase(iter);
  }
}


// public methods of FButtonGroup
//----------------------------------------------------------------------
auto FButtonGroup::getButton (int index) const -> FToggleButton*
{
  if ( buttonlist.empty() )
    return nullptr;

  if ( index <= 0 || index > int(getCount()) )
    return nullptr;

  auto iter = buttonlist.cbegin();
  std::advance (iter, index - 1);
  return static_cast<FToggleButton*>(*iter);
}

//----------------------------------------------------------------------
auto FButtonGroup::getFocusedButton() const -> FToggleButton*
{
  return findButtonIf ( [] (const auto* toggle_button)
                        { return toggle_button->hasFocus(); } );
}

//----------------------------------------------------------------------
auto FButtonGroup::getCheckedButton() const -> FToggleButton*
{
  return findButtonIf ( [] (const auto* toggle_button)
                        { return toggle_button->isChecked(); } );
}

//----------------------------------------------------------------------
void FButtonGroup::setEnable (bool enable)
{
  FWidget::setEnable(enable);

  if ( enable )
    setHotkeyAccelerator();
  else
    delAccelerator();
}

//----------------------------------------------------------------------
auto FButtonGroup::isChecked (int index) const -> bool
{
  const auto& button = getButton(index);

  if ( button )
    return button->isChecked();

  return false;
}

//----------------------------------------------------------------------
auto FButtonGroup::hasFocusedButton() const -> bool
{
  return getFocusedButton();
}

//----------------------------------------------------------------------
auto FButtonGroup::hasCheckedButton() const -> bool
{
  return getCheckedButton();
}

//----------------------------------------------------------------------
void FButtonGroup::hide()
{
  FWidget::hide();
  const auto& parent_widget = getParentWidget();

  if ( ! buttonlist.empty() )
  {
    for (auto&& item : buttonlist)
    {
      // Hide items
      auto toggle_button = static_cast<FToggleButton*>(item);
      toggle_button->hide();
    }
  }

  if ( parent_widget )
  {
    FColor fg = parent_widget->getForegroundColor();
    FColor bg = parent_widget->getBackgroundColor();
    setColor (fg, bg);
  }
  else
  {
    const auto& wc = getColorTheme();
    FColor fg = wc->dialog.fg;
    FColor bg = wc->dialog.bg;
    setColor (fg, bg);
  }

  const std::size_t size = getWidth();

  if ( size == 0 )
    return;

  // Hide border
  unsetViewportPrint();

  for (auto y{0}; y < int(getHeight()); y++)
    print() << FPoint{1, 1 + y} << FString{size, L' '};

  setViewportPrint();
}

//----------------------------------------------------------------------
void FButtonGroup::insert (FToggleButton* button)
{
  if ( ! button )
    return;

  if ( getGroup(*button) )
    getGroup(*button)->remove(button);

  // setChecked the first FRadioButton
  if ( buttonlist.size() == 1 )
  {
    auto first_button = static_cast<FToggleButton*>(*buttonlist.cbegin());

    if ( isRadioButton(first_button) )
      first_button->setChecked();
  }

  setGroup (*button, this);
  buttonlist.push_back(button);

  button->addCallback
  (
    "toggled",
    this, &FButtonGroup::cb_buttonToggled,
    button
  );
}

//----------------------------------------------------------------------
void FButtonGroup::remove (FToggleButton* button)
{
  if ( ! button || buttonlist.empty() )
    return;

  auto iter = buttonlist.cbegin();

  while ( iter != buttonlist.cend() )
  {
    const auto& toggle_button = static_cast<FToggleButton*>(*iter);

    if ( toggle_button == button )
    {
      iter = buttonlist.erase(iter);
      setGroup (*button, nullptr);
      button->delCallback(this);
      break;
    }

    ++iter;
  }
}

//----------------------------------------------------------------------
void FButtonGroup::checkScrollSize (const FToggleButton* button)
{
  // Check and adjust the scroll size

  checkScrollSize (button->getGeometry());
}

//----------------------------------------------------------------------
void FButtonGroup::checkScrollSize (const FRect& r)
{
  // Check and adjust the scroll size

  const FRect scrollgeometry (1, 1, getScrollWidth(), getScrollHeight());

  if ( ! scrollgeometry.contains(r) )
  {
    const FRect r_combined (scrollgeometry.combined(r));
    setScrollSize (r_combined.getSize());
  }
}

//----------------------------------------------------------------------
void FButtonGroup::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != MouseButton::Left )
    return;

  directFocus();
}

//----------------------------------------------------------------------
void FButtonGroup::onAccel (FAccelEvent*)
{
  directFocus();
}

//----------------------------------------------------------------------
void FButtonGroup::onFocusIn (FFocusEvent* in_ev)
{
  in_ev->ignore();  // Change default value to ignore
  auto checked_radio_button = getCheckedRadioButton();

  if ( checked_radio_button )
    checked_radio_button->setFocus();

  if ( ! hasFocusedButton() )
  {
    in_ev->accept();
    FScrollView::onFocusIn(in_ev);
  }
}

//----------------------------------------------------------------------
void FButtonGroup::onChildFocusOut (FFocusEvent* out_ev)
{
  // Change the focus away from FScrollView to another widget

  auto focused_widget = FWidget::getFocusWidget();
  auto focused_button = findButtonIf
  (
    [&focused_widget] (const auto* toggle_button)
    {
      return toggle_button == focused_widget;
    }
  );

  if ( focused_button && focused_button->hasRadiobuttonFocus()
    && isRadioButton(focused_button) )
  {
    if ( out_ev->getFocusType() == FocusTypes::NextWidget )
    {
      out_ev->accept();
      focusNextChild();
    }

    if ( out_ev->getFocusType() == FocusTypes::PreviousWidget )
    {
      out_ev->accept();
      focusPrevChild();
    }

    focused_button->redraw();
  }
  else
    FScrollView::onChildFocusOut(out_ev);
}


// protected methods of FButtonGroup
//----------------------------------------------------------------------
void FButtonGroup::draw()
{
  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(true);

  useParentWidgetColor();
  clearArea();

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);

  FScrollView::draw();
}


// private methods of FButtonGroup
//----------------------------------------------------------------------
auto FButtonGroup::isRadioButton (const FToggleButton* button) const -> bool
{
  if ( ! button )
    return false;

  return button->getClassName() == "FRadioButton";
}

//----------------------------------------------------------------------
void FButtonGroup::init()
{
  setMinimumSize (FSize{7, 3});
  buttonlist.clear();  // no buttons yet
}

//----------------------------------------------------------------------
auto FButtonGroup::directFocusCheckedRadioButton (FToggleButton* item) const -> bool
{
  if ( ! isRadioButton(item) )
    return false;

  auto focused_widget = getFocusWidget();
  item->setFocus();

  if ( focused_widget )
    focused_widget->redraw();

  focused_widget = getFocusWidget();

  if ( focused_widget )
    focused_widget->redraw();

  return true;
}

//----------------------------------------------------------------------
auto FButtonGroup::directFocusRadioButton() const -> bool
{
  if ( ! hasCheckedButton() || buttonlist.empty() )
    return false;

  auto checked_button = getCheckedButton();

  if ( checked_button )
    return directFocusCheckedRadioButton (checked_button);

  return false;
}

//----------------------------------------------------------------------
void FButtonGroup::directFocus()
{
  if ( ! hasFocusedButton() && ! directFocusRadioButton() )
  {
    auto focused_widget = getFocusWidget();
    focusFirstChild();

    if ( focused_widget )
      focused_widget->redraw();

    focused_widget = getFocusWidget();

    if ( focused_widget )
      focused_widget->redraw();
  }

  drawStatusBarMessage();
}

//----------------------------------------------------------------------
auto FButtonGroup::getCheckedRadioButton() -> FToggleButton*
{
  if ( ! hasCheckedButton() || buttonlist.empty() )
    return nullptr;

  auto checked_button = getCheckedButton();

  if ( checked_button && isRadioButton(checked_button) )
    return checked_button;

  return nullptr;
}

//----------------------------------------------------------------------
template <typename UnaryPredicate>
inline auto FButtonGroup::findButtonIf (UnaryPredicate p) const -> FToggleButton*
{
  if ( buttonlist.empty() )
    return nullptr;

  for (auto&& item : buttonlist)
  {
    const auto& toggle_button = static_cast<FToggleButton*>(item);

    if ( p(toggle_button) )
      return toggle_button;
  }

  return nullptr;
}

//----------------------------------------------------------------------
void FButtonGroup::cb_buttonToggled (const FToggleButton* button) const
{
  if ( (button && ! button->isChecked()) || buttonlist.empty() )
    return;

  for (auto&& item : buttonlist)
  {
    auto toggle_button = static_cast<FToggleButton*>(item);

    if ( toggle_button
      && toggle_button != button
      && toggle_button->isChecked()
      && isRadioButton(toggle_button) )
    {
      toggle_button->unsetChecked();

      if ( toggle_button->isShown() )
        toggle_button->redraw();
    }
  }
}

// FToggleButton friend function definition
//----------------------------------------------------------------------
auto getGroup (const FToggleButton& toggle_btn) -> FButtonGroup*
{
  return toggle_btn.button_group;
}

//----------------------------------------------------------------------
void setGroup (FToggleButton& toggle_btn, FButtonGroup* btngroup)
{
  toggle_btn.button_group = btngroup;
}

}  // namespace finalcut
