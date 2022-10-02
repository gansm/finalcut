/***********************************************************************
* fmenuitem.cpp - Widget FMenuItem                                     *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2022 Markus Gans                                      *
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
#include <utility>

#include "final/dialog/fdialog.h"
#include "final/fapplication.h"
#include "final/fevent.h"
#include "final/menu/fmenubar.h"
#include "final/menu/fmenu.h"
#include "final/menu/fmenuitem.h"
#include "final/menu/fmenulist.h"
#include "final/util/flog.h"
#include "final/widget/fstatusbar.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FMenuItem
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FMenuItem::FMenuItem (FWidget* parent)
  : FWidget{parent}
{
  init();
}

//----------------------------------------------------------------------
FMenuItem::FMenuItem (FString&& txt, FWidget* parent)
  : FWidget{parent}
  , text{std::move(txt)}
{
  init();
}

//----------------------------------------------------------------------
FMenuItem::FMenuItem (FKey k, FString&& txt, FWidget* parent)
  : FWidget{parent}
  , text{std::move(txt)}
  , accel_key{k}
{
  init();
}

//----------------------------------------------------------------------
FMenuItem::~FMenuItem()  // destructor
{
  if ( super_menu && (isMenu(super_menu) || isMenuBar(super_menu)) )
  {
    auto menu_list = getFMenuList(*super_menu);

    if ( menu_list )
      menu_list->remove(this);
  }

  delAccelerator();

  // remove dialog list item callback from the dialog
  if ( associated_window )
    associated_window->delCallback(this);
}


// public methods of FMenuItem
//----------------------------------------------------------------------
auto FMenuItem::setEnable (bool enable) -> bool
{
  FWidget::setEnable(enable);
  auto super = getSuperMenu();

  if ( enable )
  {
    if ( super && isMenuBar(super) )
    {
      // Meta + hotkey
      super->addAccelerator ( FKey::Meta_offset + FKey(std::tolower(int(hotkey)))
                            , this );
    }
  }
  else
  {
    if ( super && isMenuBar(super) )
      super->delAccelerator (this);
  }

  return enable;
}

//----------------------------------------------------------------------
auto FMenuItem::setFocus (bool enable) -> bool
{
  FWidget::setFocus(enable);

  if ( ! enable || ! isEnabled() || selected )
    return enable;

  auto menu_list = getFMenuList(*getSuperMenu());
  setSelected();

  if ( menu_list )
  {
    menu_list->unselectItem();
    menu_list->setSelectedItem(this);
  }

  if ( getStatusBar() )
    getStatusBar()->drawMessage();

  auto parent = getSuperMenu();

  if ( isMenuBar(parent) )
  {
    auto menubar_ptr = static_cast<FMenuBar*>(parent);

    if ( menubar_ptr )
      menubar_ptr->redraw();
  }
  else if ( isMenu(parent) )
  {
    auto menu_ptr = static_cast<FMenu*>(parent);

    if ( menu_ptr )
      menu_ptr->redraw();
  }

  return enable;
}

//----------------------------------------------------------------------
void FMenuItem::setSelected()
{
  if ( isEnabled() )
  {
    selected = true;
    processActivate();
  }
}

//----------------------------------------------------------------------
void FMenuItem::unsetSelected()
{
  selected = false;
  unsetCursorPos();
  processDeactivate();
}

//----------------------------------------------------------------------
void FMenuItem::setText (const FString& txt)
{
  text.setString(txt);
  calculateTextDimensions();
}

//----------------------------------------------------------------------
void FMenuItem::addAccelerator (FKey key, FWidget* obj) &
{
  const auto& root = getRootWidget();
  const FAccelerator accel{ key, obj };

  if ( root )
  {
    accel_key = key;
    root->setAcceleratorList().push_back(accel);
  }

  updateSuperMenuDimensions();
}

//----------------------------------------------------------------------
void FMenuItem::delAccelerator (FWidget* obj) &
{
  auto root = getRootWidget();

  if ( root && ! root->setAcceleratorList().empty() )
  {
    auto& list = root->setAcceleratorList();
    auto iter = list.cbegin();

    while ( iter != list.cend() )
    {
      if ( iter->object == obj )
      {
        accel_key = FKey::None;
        iter = list.erase(iter);
      }
      else
        ++iter;
    }
  }

  updateSuperMenuDimensions();
}

//----------------------------------------------------------------------
void FMenuItem::openMenu() const
{
  if ( ! hasMenu() )
    return;

  auto dd_menu = getMenu();  // Drop-down menu

  if ( dd_menu->isShown() )
    return;

  auto openmenu = static_cast<FMenu*>(getOpenMenu());

  if ( openmenu && openmenu != dd_menu )
  {
    openmenu->hide();
    openmenu->hideSubMenus();
  }

  if ( dialog_index )
    createDialogList (dd_menu);

  setOpenMenu(dd_menu);
  dd_menu->show();
  dd_menu->raiseWindow();
  dd_menu->redraw();
}

//----------------------------------------------------------------------
void FMenuItem::onKeyPress (FKeyEvent* ev)
{
  if ( ! super_menu )
    return;

  if ( isMenu(super_menu) )
  {
    auto smenu = static_cast<FMenu*>(super_menu);
    smenu->onKeyPress(ev);
  }

  if ( isMenuBar(super_menu) )
  {
    auto mbar = static_cast<FMenuBar*>(super_menu);

    if ( mbar )
    {
      if ( mbar->hotkeyMenu(ev) )
        return;

      mbar->onKeyPress(ev);
    }
  }
}

//----------------------------------------------------------------------
void FMenuItem::onMouseDoubleClick (FMouseEvent* ev)
{
  if ( ! super_menu )
    return;

  if ( isMenu(super_menu) )
  {
    auto smenu = static_cast<FMenu*>(super_menu);
    passMouseEvent (smenu, ev, Event::MouseDoubleClick);
  }

  if ( isMenuBar(super_menu) )
  {
    auto mbar = static_cast<FMenuBar*>(super_menu);
    passMouseEvent (mbar, ev, Event::MouseDoubleClick);
  }

  if ( isDialog(super_menu) )
  {
    auto dgl = static_cast<FDialog*>(super_menu);
    passMouseEvent (dgl, ev, Event::MouseDoubleClick);
  }
}

//----------------------------------------------------------------------
void FMenuItem::onMouseDown (FMouseEvent* ev)
{
  if ( ! super_menu )
    return;

  if ( isMenu(super_menu) )
  {
    auto smenu = static_cast<FMenu*>(super_menu);
    passMouseEvent (smenu, ev, Event::MouseDown);
  }

  if ( isMenuBar(super_menu) )
  {
    auto mbar = static_cast<FMenuBar*>(super_menu);
    passMouseEvent (mbar, ev, Event::MouseDown);
  }

  if ( isDialog(super_menu) )
  {
    auto dgl = static_cast<FDialog*>(super_menu);
    passMouseEvent (dgl, ev, Event::MouseDown);
  }
}

//----------------------------------------------------------------------
void FMenuItem::onMouseUp (FMouseEvent* ev)
{
  if ( ! super_menu )
    return;

  if ( isMenu(super_menu) )
  {
    auto smenu = static_cast<FMenu*>(super_menu);
    passMouseEvent (smenu, ev, Event::MouseUp);
  }

  if ( isMenuBar(super_menu) )
  {
    auto mbar = static_cast<FMenuBar*>(super_menu);
    passMouseEvent (mbar, ev, Event::MouseUp);
  }

  if ( isDialog(super_menu) )
  {
    auto dgl = static_cast<FDialog*>(super_menu);
    passMouseEvent (dgl, ev, Event::MouseUp);
  }
}

//----------------------------------------------------------------------
void FMenuItem::onMouseMove (FMouseEvent* ev)
{
  if ( ! super_menu )
    return;

  if ( isMenu(super_menu) )
  {
    auto smenu = static_cast<FMenu*>(super_menu);
    passMouseEvent (smenu, ev, Event::MouseMove);
  }

  if ( isMenuBar(super_menu) )
  {
    auto mbar = static_cast<FMenuBar*>(super_menu);
    passMouseEvent (mbar, ev, Event::MouseMove);
  }

  if ( isDialog(super_menu) )
  {
    auto dgl = static_cast<FDialog*>(super_menu);
    passMouseEvent (dgl, ev, Event::MouseMove);
  }
}

//----------------------------------------------------------------------
void FMenuItem::onAccel (FAccelEvent* ev)
{
  if ( ! isEnabled() || isSelected() )
    return;

  if ( ! super_menu || ! isMenuBar(super_menu) )
  {
    processClicked();
    return;
  }

  auto mbar = static_cast<FMenuBar*>(super_menu);

  if ( menu )
  {
    if ( mbar->getSelectedItem() )
      mbar->getSelectedItem()->unsetSelected();

    setSelected();
    mbar->setSelectedItem(this);
    openMenu();
    auto focused_widget = ev->focusedWidget();
    menu->unselectItem();
    menu->selectFirstItem();

    if ( menu->getSelectedItem() )
      menu->getSelectedItem()->setFocus();

    if ( focused_widget && focused_widget->isWidget() )
      focused_widget->redraw();

    menu->redraw();

    if ( getStatusBar() )
      getStatusBar()->drawMessage();

    mbar->redraw();
    mbar->drop_down = true;
  }
  else
  {
    unsetSelected();
    mbar->unsetSelectedItem();
    mbar->redraw();
    processClicked();
    mbar->drop_down = false;
  }

  forceTerminalUpdate();
  ev->accept();
}

//----------------------------------------------------------------------
void FMenuItem::onFocusIn (FFocusEvent*)
{
  if ( getStatusBar() )
    getStatusBar()->drawMessage();
}

//----------------------------------------------------------------------
void FMenuItem::onFocusOut (FFocusEvent*)
{
  unsetSelected();

  if ( super_menu && isMenuBar(super_menu) )
  {
    auto mbar = static_cast<FMenuBar*>(super_menu);
    mbar->redraw();
  }

  if ( getStatusBar() )
  {
    getStatusBar()->clearMessage();
    getStatusBar()->drawMessage();
  }
}


// protected methods of FMenuItem
//----------------------------------------------------------------------
auto FMenuItem::isDialog (const FWidget* w) const -> bool
{
  return w ? w->isDialogWidget() : false;
}

//----------------------------------------------------------------------
auto FMenuItem::isMenuBar (const FWidget* w) const -> bool
{
  return w ? w->isInstanceOf("FMenuBar") : false;
}

//----------------------------------------------------------------------
auto FMenuItem::isMenu (const FWidget* w) const -> bool
{
  if ( ! w )
    return false;

  const bool m1 = w->isInstanceOf("FMenu");
  const bool m2 = w->isInstanceOf("FDialogListMenu");
  return m1 || m2;
}

//----------------------------------------------------------------------
void FMenuItem::initLayout()
{
  calculateTextDimensions();
}

// private methods of FMenuItem
//----------------------------------------------------------------------
auto FMenuItem::getFMenuList (FWidget& widget) -> FMenuList*
{
  FMenuList* menu_list{nullptr};

  if ( isMenu(&widget) )
  {
    auto Menu = static_cast<FMenu*>(&widget);
    menu_list = static_cast<FMenuList*>(Menu);
  }
  else if ( isMenuBar(&widget) )
  {
    auto Menubar = static_cast<FMenuBar*>(&widget);
    menu_list = static_cast<FMenuList*>(Menubar);
  }

  return menu_list;
}

//----------------------------------------------------------------------
void FMenuItem::init()
{
  text_length = text.getLength();
  text_width = getColumnWidth(text);
  hotkey = finalcut::getHotkey(text);

  if ( hotkey > 0xff00 && hotkey < 0xff5f )  // full-width character
    hotkey -= 0xfee0;

  if ( hotkey != FKey::None )
  {
    text_length--;
    text_width--;
  }

  setGeometry (FPoint{1, 1}, FSize{text_width + 2, 1}, false);
  const auto& parent = getParentWidget();

  if ( ! parent )
    return;

  setSuperMenu (parent);

  if ( accel_key != FKey::None )
    addAccelerator (accel_key);

  auto menu_list = getFMenuList(*parent);

  if ( menu_list )
    menu_list->insert(this);

  if ( isMenuBar(parent) )  // Parent is menubar
  {
    auto& menubar_widget = *static_cast<FMenuBar*>(parent);
    menubar_widget.calculateDimensions();

    if ( hotkey != FKey::None )  // Meta + hotkey
      menubar_widget.addAccelerator ( FKey::Meta_offset + FKey(std::tolower(int(hotkey)))
                                    , this );

    addCallback  // for this element
    (
      "deactivate",
      &menubar_widget, &FMenuBar::cb_itemDeactivated,
      this
    );
  }
  else if ( isMenu(parent) )  // Parent is menu
  {
    auto menu_ptr = static_cast<FMenu*>(parent);
    menu_ptr->calculateDimensions();
  }
}

//----------------------------------------------------------------------
void FMenuItem::calculateTextDimensions()
{
  text_length = text.getLength();
  text_width = getColumnWidth(text);
  hotkey = finalcut::getHotkey(text);

  if ( hotkey != FKey::None )
  {
    text_length--;
    text_width--;
  }

  updateMenubarDimensions();
  updateSuperMenuDimensions();
}

//----------------------------------------------------------------------
void FMenuItem::updateSuperMenuDimensions()
{
  if ( ! super_menu || ! isMenu(super_menu) )
    return;

  auto menu_ptr = static_cast<FMenu*>(super_menu);
  menu_ptr->calculateDimensions();
}

//----------------------------------------------------------------------
void FMenuItem::updateMenubarDimensions() const
{
  const auto& parent = getParentWidget();

  if ( ! parent || ! isMenuBar(parent) )
    return;

  auto menubar_ptr = static_cast<FMenuBar*>(parent);
  menubar_ptr->calculateDimensions();
}

//----------------------------------------------------------------------
void FMenuItem::processEnable() const
{
  emitCallback("enable");
}

//----------------------------------------------------------------------
void FMenuItem::processDisable() const
{
  emitCallback("disable");
}

//----------------------------------------------------------------------
void FMenuItem::processActivate() const
{
  emitCallback("activate");
}

//----------------------------------------------------------------------
void FMenuItem::processDeactivate() const
{
  emitCallback("deactivate");
}

//----------------------------------------------------------------------
void FMenuItem::createDialogList (FMenu* winmenu) const
{
  winmenu->clear();

  if ( getDialogList() && ! getDialogList()->empty() )
  {
    const auto& first = getDialogList()->cbegin();
    auto iter = first;

    while ( iter != getDialogList()->cend() && *iter )
    {
      auto win = static_cast<FDialog*>(*iter);
      FMenuItem* win_item{};
      const auto n = uInt32(std::distance(first, iter));
      const auto& name = win->getText();  // get the dialog title
      FString state = ( win->isMinimized() ) ? L" (minimized)" : L"";

      try
      {
        // create a new dialog list item
        win_item = new FMenuItem (name + state, winmenu);
      }
      catch (const std::bad_alloc&)
      {
        badAllocOutput ("FMenuItem");
        return;
      }

      if ( n < 9 )
        win_item->addAccelerator (FKey::Meta_1 + n);  // Meta + 1..9

      win_item->addCallback
      (
        "clicked",
        static_cast<std::remove_reference_t<decltype(win_item)>>(win_item),
        &FMenuItem::cb_switchToDialog,
        win
      );

      win->addCallback
      (
        "destroy",
        static_cast<std::remove_reference_t<decltype(win_item)>>(win_item),
        &FMenuItem::cb_destroyDialog,
        win
      );

      win_item->associated_window = win;
      ++iter;
    }
  }

  winmenu->calculateDimensions();
}

//----------------------------------------------------------------------
template <typename T>
void FMenuItem::passMouseEvent ( T widget, const FMouseEvent* ev
                               , Event ev_type ) const
{
  if ( ! widget )
    return;

  const auto& t = ev->getTermPos();
  const auto& p2 = widget->termToWidgetPos(t);
  const auto b = ev->getButton();
  auto _ev = std::make_shared<FMouseEvent>(ev_type, p2, t, b);

  if ( ev_type == Event::MouseDoubleClick )
  {
    widget->onMouseDoubleClick(_ev.get());
  }
  else if ( ev_type == Event::MouseDown )
  {
    widget->onMouseDown(_ev.get());
  }
  else if ( ev_type == Event::MouseUp )
  {
    widget->onMouseUp(_ev.get());
  }
  else if ( ev_type == Event::MouseMove )
  {
    widget->onMouseMove(_ev.get());
  }
}

//----------------------------------------------------------------------
void FMenuItem::cb_switchToDialog (FDialog* win) const
{
  if ( ! win )
    return;

  if ( win->isMinimized() )
    win->minimizeWindow();  // unminimize window

  auto focus = getFocusWidget();
  FAccelEvent a_ev (Event::Accelerator, focus);
  FApplication::sendEvent (win, &a_ev);
}

//----------------------------------------------------------------------
void FMenuItem::cb_destroyDialog (FDialog* win)
{
  const auto& fapp = FApplication::getApplicationObject();

  if ( ! win || ! fapp )
    return;

  delAccelerator(win);
  delCallback(std::move(win));
  associated_window = nullptr;
}

//----------------------------------------------------------------------
void FMenuItem::processClicked()
{
  emitCallback("clicked");
}

}  // namespace finalcut
