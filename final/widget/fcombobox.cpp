/***********************************************************************
* fcombobox.cpp - Widget FComboBox                                     *
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

#include <memory>

#include "final/fapplication.h"
#include "final/fevent.h"
#include "final/fwidgetcolors.h"
#include "final/input/fmouse.h"
#include "final/util/fpoint.h"
#include "final/util/fsize.h"
#include "final/vterm/fcolorpair.h"
#include "final/widget/fcombobox.h"
#include "final/widget/flabel.h"
#include "final/widget/flineedit.h"
#include "final/widget/flistbox.h"
#include "final/widget/fstatusbar.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FDropDownListBox
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FDropDownListBox::FDropDownListBox (FWidget* parent)
  : FWindow{parent}
{
  init();
}

//----------------------------------------------------------------------
FDropDownListBox::~FDropDownListBox()  // destructor
{
  if ( FApplication::isQuit() )
    return;

  FWindow* parent_win{nullptr};

  if ( const auto& parent = getParentWidget() )
    parent_win = getWindowWidget(parent);

  if ( parent_win )
    setActiveWindow (parent_win);
  else
    switchToPrevWindow(this);
}

//----------------------------------------------------------------------
void FDropDownListBox::setGeometry ( const FPoint& pos, const FSize& size
                                   , bool adjust )
{
  FWindow::setGeometry (pos, size, adjust);

  if ( FVTerm::getFOutput()->isNewFont() )
  {
    FSize new_size{size};
    new_size.scaleBy(-1, 0);
    list.setGeometry (FPoint{2, 1}, new_size, adjust);
  }
  else
    list.setGeometry (FPoint{1, 1}, size, adjust);
}

//----------------------------------------------------------------------
void FDropDownListBox::show()
{
  if ( ! isVisible() )
    return;

  FWindow::show();
}

//----------------------------------------------------------------------
void FDropDownListBox::hide()
{
  if ( ! isVisible() )
    return;

  FWindow::hide();
  setOpenMenu(nullptr);
  const auto& t_geometry = getTermGeometryWithShadow();
  restoreVTerm (t_geometry);
}


// private methods of FDropDownListBox
//----------------------------------------------------------------------
void FDropDownListBox::init()
{
  setAlwaysOnTop();
  ignorePadding();
  setShadow();
  // initialize geometry values
  FDropDownListBox::setGeometry (FPoint{1, 1}, FSize{3, 3}, false);
  setMinimumSize (FSize{3, 3});
  FDropDownListBox::hide();
  list.setGeometry (FPoint{1, 1}, FSize{3, 3}, false);
}

//----------------------------------------------------------------------
void FDropDownListBox::draw()
{
  // Fill the background

  const auto& wc = getColorTheme();
  setForegroundColor (wc->list_fg);
  setBackgroundColor (wc->list_bg);
  setColor();

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(true);

  clearArea();
  drawShadow();

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
void FDropDownListBox::drawShadow()
{
  const auto& wc = getColorTheme();
  finalcut::drawShadow(this);
  setColor (wc->shadow_fg, wc->shadow_bg);
  print() << FPoint{int(getWidth()) + 1, 1} << UniChar::FullBlock;  // █
}

//----------------------------------------------------------------------
auto FDropDownListBox::containsWidget (const FPoint& p) -> bool
{
  // Check mouse click position for item, menu and all sub menus

  const auto& parent = getParentWidget();

  if ( getTermGeometry().contains(p) )
    return true;

  if ( parent && parent->isInstanceOf("FComboBox") )
    return static_cast<FComboBox*>(parent)->getTermGeometry().contains(p);

  return false;
}


//----------------------------------------------------------------------
// class FComboBox
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FComboBox::FComboBox (FWidget* parent)
  : FWidget{parent}
{
  init();
}

//----------------------------------------------------------------------
FComboBox::~FComboBox() noexcept = default;  // destructor

// public methods of FComboBox
//----------------------------------------------------------------------
void FComboBox::setSize (const FSize& size, bool adjust)
{
  FWidget::setSize (size, adjust);
  FSize input_field_size{size};
  input_field_size.scaleBy(-(1 + nf), 0);
  input_field.setSize (input_field_size, adjust);
}

//----------------------------------------------------------------------
void FComboBox::setGeometry ( const FPoint& pos, const FSize& size
                            , bool adjust )
{
  FWidget::setGeometry (pos, size, adjust);
  FSize input_field_size{size};
  input_field_size.scaleBy(-(1 + nf), 0);
  input_field.setGeometry (FPoint{1, 1}, input_field_size, adjust);
}

//----------------------------------------------------------------------
void FComboBox::setEnable (bool enable)
{
  FWidget::setEnable(enable);
  input_field.setEnable(enable);
}

//----------------------------------------------------------------------
void FComboBox::setShadow (bool enable)
{
  setWidgetShadow(this, enable);
}

//----------------------------------------------------------------------
void FComboBox::setEditable (bool enable)
{
  if ( is_editable == enable )
    return;

  if ( enable )
    unsetVisibleCursor();
  else
    setVisibleCursor();

  input_field.setReadOnly(! enable);
}

//----------------------------------------------------------------------
void FComboBox::setCurrentItem (std::size_t index)
{
  if ( index > getCount() )
    index = getCount();
  else if ( index < 1 )
    index = 1;

  if ( index == list_window.list.currentItem() )
    return;

  list_window.list.setCurrentItem(index);
  input_field = list_window.list.getItem(index).getText();
  input_field.redraw();
  processRowChanged();
}

//----------------------------------------------------------------------
void FComboBox::setMaxVisibleItems (std::size_t items)
{
  // Sets the maximum height of the combo box in elements

  if ( items > getCount() )
    max_items = getCount();
  else
    max_items = items;
}

//----------------------------------------------------------------------
void FComboBox::insert (const FListBoxItem& listItem)
{
  list_window.list.insert(listItem);

  if ( getCount() == 1 )
    input_field = list_window.list.getItem(1).getText();
}

//----------------------------------------------------------------------
void FComboBox::remove (std::size_t item)
{
  list_window.list.remove(item);

  if ( ! list_window.isEmpty() )
  {
    const std::size_t index = list_window.list.currentItem();
    input_field = list_window.list.getItem(index).getText();
    input_field.redraw();
  }

  if ( list_window.isShown() )
  {
    // Adjusting the size of the drop-down list
    hideDropDown();
    showDropDown();
  }
}

//----------------------------------------------------------------------
void FComboBox::clear()
{
  if ( list_window.isShown() )
    hideDropDown();

  list_window.list.clear();
  input_field.clear();
  redraw();
}

//----------------------------------------------------------------------
void FComboBox::showDropDown()
{
  if ( list_window.isEmpty() )
    return;

  static constexpr std::size_t border = 2;  // Size of the top and bottom border
  setOpenMenu(&list_window);
  FPoint p{getTermPos()};
  p.move(0 - int(nf), 1);
  setClickedWidget(&list_window.list);
  const std::size_t w = getWidth();
  const std::size_t h = ( getCount() <= max_items ) ? getCount() : max_items;
  list_window.setGeometry(p, FSize{w + std::size_t(nf), h + border});
  list_window.show();
  list_window.list.setFocus();
  list_window.redraw();
}

//----------------------------------------------------------------------
void FComboBox::hideDropDown()
{
  if ( list_window.isHidden() )
     return;

  list_window.hide();
  input_field.setFocus();
  input_field.redraw();
}

//----------------------------------------------------------------------
void FComboBox::onKeyPress (FKeyEvent* ev)
{
  if ( ! isEnabled() )
    return;

  const auto key = ev->key();

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
    onePosUp();
    ev->accept();
  }
  else if ( key == FKey::Down )
  {
    onePosDown();
    ev->accept();
  }
  else if ( key == FKey::Meta_up
         || key == FKey::Ctrl_up
         || isEscapeKey(key) )
  {
    hideDropDown();
    ev->accept();
  }
  else if ( key == FKey::F4
         || key == FKey::Meta_down
         || key == FKey::Ctrl_down )
  {
    showDropDown();
    ev->accept();
  }
}

//----------------------------------------------------------------------
void FComboBox::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != MouseButton::Left )
    return;

  setWidgetFocus(this);
  const int mouse_x = ev->getX();
  const int mouse_y = ev->getY();

  if ( mouse_x >= int(getWidth()) - nf
    && mouse_x <= int(getWidth()) && mouse_y == 1 )
  {
    redraw();

    if ( list_window.isHidden() )
      showDropDown();
    else
      list_window.hide();
  }
}

//----------------------------------------------------------------------
void FComboBox::onMouseMove (FMouseEvent* ev)
{
  if ( ev->getButton() != MouseButton::Left )
    return;

  if ( isMouseOverListWindow(ev->getTermPos()) )
  {
    passEventToListWindow(*ev);  // Event handover to window list
    return;
  }
}

//----------------------------------------------------------------------
void FComboBox::onWheel (FWheelEvent* ev)
{
  if ( ev->getWheel() == MouseWheel::Up )
    onePosUp();
  else if ( ev->getWheel() == MouseWheel::Down )
    onePosDown();
}

//----------------------------------------------------------------------
void FComboBox::onFocusIn (FFocusEvent* in_ev)
{
  setWidgetFocus (&input_field);
  FWidget::onFocusIn(in_ev);
}

//----------------------------------------------------------------------
void FComboBox::onFocusOut (FFocusEvent* out_ev)
{
  hideDropDown();
  FWidget::onFocusOut(out_ev);
}

//----------------------------------------------------------------------
void FComboBox::onFailAtChildFocus (FFocusEvent* fail_ev)
{
  // Change the focus away from FComboBox to another widget

  if ( fail_ev->getFocusType() == FocusTypes::NextWidget )
  {
    fail_ev->accept();
    focusNextChild();
  }
  else if ( fail_ev->getFocusType() == FocusTypes::PreviousWidget )
  {
    fail_ev->accept();
    focusPrevChild();
  }
}


// private methods of FComboBox
//----------------------------------------------------------------------
auto FComboBox::isMouseOverListWindow (const FPoint& termpos) -> bool
{
  if ( list_window.isShown() )
  {
    const auto& list_geometry = list_window.getTermGeometry();
    return list_geometry.contains(termpos);
  }

  return false;
}

//----------------------------------------------------------------------
void FComboBox::init()
{
  setShadow();
  const auto& parent_widget = getParentWidget();
  auto label = input_field.getLabelObject();
  label->setParent(getParent());
  label->setForegroundColor (parent_widget->getForegroundColor());
  label->setBackgroundColor (parent_widget->getBackgroundColor());
  input_field.setLabelAssociatedWidget(this);
  input_field.unsetShadow();
  adjustSize();
  initCallbacks();

  if ( FVTerm::getFOutput()->isNewFont() )
    nf = 1;
}

//----------------------------------------------------------------------
void FComboBox::initCallbacks()
{
  input_field.addCallback
  (
    "mouse-press",
    this, &FComboBox::cb_inputFieldSwitch
  );

  input_field.addCallback
  (
    "mouse-move",
    this, &FComboBox::cb_inputFieldHandOver
  );

  list_window.list.addCallback
  (
    "row-changed",
    this, &FComboBox::cb_setInputField
  );

  for (const auto& signal : {"row-selected", "clicked"})
  {
    list_window.list.addCallback
    (
      signal,
      this, &FComboBox::cb_closeComboBox
    );
  }
}

//----------------------------------------------------------------------
void FComboBox::draw()
{
  const auto& wc = getColorTheme();

  const FColorPair button_color = [this, &wc] ()
  {
    if ( list_window.isEmpty() )
      return FColorPair { wc->scrollbar_button_inactive_fg
                        , wc->scrollbar_button_inactive_bg };

    return FColorPair { wc->scrollbar_button_fg
                      , wc->scrollbar_button_bg };
  }();

  print() << FPoint{int(getWidth()) - nf, 1}
          << button_color;

  if ( FVTerm::getFOutput()->isNewFont() )
    print() << NF_button_arrow_down;
  else
    print() << UniChar::BlackDownPointingTriangle;  // ▼

  if ( getFlags().shadow.shadow )
    drawShadow(this);
}

//----------------------------------------------------------------------
void FComboBox::onePosUp()
{
  std::size_t index = list_window.list.currentItem();

  if ( index > 1 )
    index--;
  else
    return;

  list_window.list.setCurrentItem(index);
  input_field = list_window.list.getItem(index).getText();
  input_field.redraw();
  processRowChanged();
}

//----------------------------------------------------------------------
void FComboBox::onePosDown()
{
  std::size_t index = list_window.list.currentItem();

  if ( index < getCount() )
    index++;
  else
    return;

  list_window.list.setCurrentItem(index);
  input_field = list_window.list.getItem(index).getText();
  input_field.redraw();
  processRowChanged();
}

//----------------------------------------------------------------------
void FComboBox::passEventToListWindow (const FMouseEvent& ev)
{
  // Mouse event handover to list window

  const auto& t = ev.getTermPos();
  const auto& p = list_window.list.termToWidgetPos(t);
  const auto b = ev.getButton();
  const auto& new_ev = \
      std::make_shared<FMouseEvent>(Event::MouseMove, p, t, b);
  setClickedWidget(&list_window.list);
  list_window.list.setFocus();
  list_window.list.onMouseMove(new_ev.get());
}

//----------------------------------------------------------------------
void FComboBox::processClick() const
{
  emitCallback("clicked");
}

//----------------------------------------------------------------------
void FComboBox::processRowChanged() const
{
  emitCallback("row-changed");
}

//----------------------------------------------------------------------
void FComboBox::cb_setInputField()
{
  auto& list = list_window.list;
  const std::size_t index = list.currentItem();
  input_field = list.getItem(index).getText();
  input_field.redraw();
  processRowChanged();
}

//----------------------------------------------------------------------
void FComboBox::cb_closeComboBox()
{
  hideDropDown();
  processClick();
}

//----------------------------------------------------------------------
void FComboBox::cb_inputFieldSwitch()
{
  const auto mouse_event = FMouseControl::getCurrentMouseEvent();

  if ( ! mouse_event || ! mouse_event->isLeftButtonPressed() )
    return;

  if ( list_window.isShown() )
  {
    hideDropDown();
  }
  else if ( ! is_editable )
  {
    setWidgetFocus(this);
    showDropDown();
  }
}

//----------------------------------------------------------------------
void FComboBox::cb_inputFieldHandOver()
{
  auto& mouse = FMouseControl::getInstance();

  if ( list_window.isHidden() )
    return;

  const auto& t = mouse.getPos();
  const auto& p = list_window.list.termToWidgetPos(t);
  const auto b = ( mouse.isLeftButtonPressed() ) ? MouseButton::Left : MouseButton::None;
  const auto& new_ev = \
      std::make_shared<FMouseEvent>(Event::MouseMove, p, t, b);
  setClickedWidget(&list_window.list);
  list_window.list.setFocus();
  list_window.list.onMouseMove(new_ev.get());
}

// non-member functions
//----------------------------------------------------------------------
void closeOpenComboBox()
{
  // Close open comboboxes
  auto openmenu = FWidget::getOpenMenu();

  if ( ! openmenu )
    return;

  if ( openmenu->isInstanceOf("FDropDownListBox") )
  {
    auto drop_down = static_cast<FDropDownListBox*>(openmenu);
    drop_down->hide();
  }
}

//----------------------------------------------------------------------
auto closeComboBox ( FDropDownListBox* list_window
                   , const FPoint& mouse_position ) -> bool
{
  // Close the drop down list box

  if ( ! list_window )
    return false;

  if ( list_window->containsWidget(mouse_position) )
    return false;

  list_window->hide();
  return true;
}

}  // namespace finalcut
