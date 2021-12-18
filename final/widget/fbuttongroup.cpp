/***********************************************************************
* fbuttongroup.cpp - The FButtonGroup widget organizes FToggleButton   *
*                    widgets in a group.                               *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2021 Markus Gans                                      *
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
    toggle_button->setGroup(nullptr);
    iter = buttonlist.erase(iter);
  }
}


// public methods of FButtonGroup
//----------------------------------------------------------------------
FToggleButton* FButtonGroup::getButton (int index) const
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
FToggleButton* FButtonGroup::getFirstButton()
{
  auto widget = getFirstFocusableWidget(buttonlist);
  auto toggle_button = static_cast<FToggleButton*>(widget);
  return toggle_button;
}

//----------------------------------------------------------------------
FToggleButton* FButtonGroup::getLastButton()
{
  auto widget = getLastFocusableWidget(buttonlist);
  auto toggle_button = static_cast<FToggleButton*>(widget);
  return toggle_button;
}

//----------------------------------------------------------------------
bool FButtonGroup::setEnable (bool enable)
{
  FWidget::setEnable(enable);

  if ( enable )
    setHotkeyAccelerator();
  else
    delAccelerator();

  return enable;
}

//----------------------------------------------------------------------
bool FButtonGroup::setFocus (bool)
{
  // This container widget cannot have its own focus
  return false;
}

//----------------------------------------------------------------------
bool FButtonGroup::isChecked (int index) const
{
  const auto& button = getButton(index);

  if ( button )
    return button->isChecked();

  return false;
}

//----------------------------------------------------------------------
bool FButtonGroup::hasFocusedButton() const
{
  if ( buttonlist.empty() )
    return false;

  for (auto&& item : buttonlist)
  {
    const auto& toggle_button = static_cast<FToggleButton*>(item);

    if ( toggle_button->hasFocus() )
      return true;
  }

  return false;
}

//----------------------------------------------------------------------
bool FButtonGroup::hasCheckedButton() const
{
  if ( buttonlist.empty() )
    return false;

  for (auto&& item : buttonlist)
  {
    const auto& toggle_button = static_cast<FToggleButton*>(item);

    if ( toggle_button->isChecked() )
      return true;
  }

  return false;
}

//----------------------------------------------------------------------
void FButtonGroup::hide()
{
  FColor fg{};
  FColor bg{};
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
    fg = parent_widget->getForegroundColor();
    bg = parent_widget->getBackgroundColor();
  }
  else
  {
    const auto& wc = getColorTheme();
    fg = wc->dialog_fg;
    bg = wc->dialog_bg;
  }

  setColor (fg, bg);
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

  if ( button->getGroup() )
    button->getGroup()->remove(button);

  // setChecked the first FRadioButton
  if ( buttonlist.size() == 1 )
  {
    auto first_button = static_cast<FToggleButton*>(*buttonlist.cbegin());

    if ( isRadioButton(first_button) )
      first_button->setChecked();
  }

  button->setGroup(this);
  buttonlist.push_back(button);

  button->addCallback
  (
    "toggled",
    this, &FButtonGroup::cb_buttonToggled,
    button
  );
}

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
      button->setGroup(nullptr);
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
  focusInRadioButton (in_ev);

  if ( ! in_ev->isAccepted() )
  {
    in_ev->accept();
    auto prev_element = getFocusWidget();

    if ( in_ev->getFocusType() == FocusTypes::NextWidget )
      focusFirstChild();
    else if ( in_ev->getFocusType() == FocusTypes::PreviousWidget )
      focusLastChild();

    if ( prev_element )
      prev_element->redraw();

    if ( getFocusWidget() )
      getFocusWidget()->redraw();
  }

  if ( getStatusBar() )
  {
    getStatusBar()->drawMessage();
  }
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
bool FButtonGroup::isRadioButton (const FToggleButton* button) const
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
bool FButtonGroup::directFocusCheckedRadioButton (FToggleButton* item) const
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
bool FButtonGroup::directFocusRadioButton() const
{
  if ( ! hasCheckedButton() || buttonlist.empty() )
    return false;

  for (auto&& item : buttonlist)
  {
    auto toggle_button = static_cast<FToggleButton*>(item);

    if ( toggle_button->isChecked() )
      return directFocusCheckedRadioButton (toggle_button);
  }

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

  if ( getStatusBar() )
  {
    getStatusBar()->drawMessage();
  }
}

//----------------------------------------------------------------------
void FButtonGroup::focusCheckedRadioButton ( FToggleButton* toggle_button
                                           , FFocusEvent* in_ev )
{
  auto prev_element = getFocusWidget();

  toggle_button->setFocus();

  FFocusEvent cfi (Event::ChildFocusIn);
  FApplication::sendEvent(this, &cfi);

  FFocusEvent in (Event::FocusIn);
  FApplication::sendEvent(toggle_button, &in);

  if ( in.isAccepted() )
    in_ev->accept();

  if ( prev_element )
    prev_element->redraw();

  toggle_button->redraw();
}

//----------------------------------------------------------------------
void FButtonGroup::focusInRadioButton (FFocusEvent* in_ev)
{
  if ( ! hasCheckedButton() || buttonlist.empty() )
    return;

  for (auto&& item : buttonlist)
  {
    auto toggle_button = static_cast<FToggleButton*>(item);

    if ( toggle_button->isChecked() )
    {
      if ( isRadioButton(toggle_button) )
        focusCheckedRadioButton (toggle_button, in_ev);

      break;
    }
  }
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

}  // namespace finalcut
