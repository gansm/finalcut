/***********************************************************************
* fmenuitem.cpp - Widget FMenuItem                                     *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2015-2019 Markus Gans                                      *
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

#include <memory>

#include "final/fapplication.h"
#include "final/fdialog.h"
#include "final/fevent.h"
#include "final/fmenu.h"
#include "final/fmenubar.h"
#include "final/fmenulist.h"
#include "final/fmenuitem.h"
#include "final/fstatusbar.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FMenuItem
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FMenuItem::FMenuItem (FWidget* parent)
  : FWidget(parent)
{
  init (parent);
}

//----------------------------------------------------------------------
FMenuItem::FMenuItem (const FString& txt, FWidget* parent)
  : FWidget(parent)
  , text(txt)
{
  init (parent);
}

//----------------------------------------------------------------------
FMenuItem::FMenuItem (FKey k, const FString& txt, FWidget* parent)
  : FWidget(parent)
  , text(txt)
  , accel_key(k)
{
  init (parent);
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
bool FMenuItem::setEnable (bool enable)
{
  FWidget::setEnable(enable);
  auto super = getSuperMenu();

  if ( enable )
  {
    if ( super && isMenuBar(super) )
    {
      // Meta + hotkey
      super->addAccelerator ( fc::Fmkey_meta + FKey(std::tolower(int(hotkey)))
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
bool FMenuItem::setFocus (bool enable)
{
  FWidget::setFocus(enable);

  if ( enable )
  {
    if ( isEnabled() )
    {
      if ( ! selected )
      {
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
      }

      if ( getStatusBar() )
      {
        const auto& msg = getStatusbarMessage();
        const auto& curMsg = getStatusBar()->getMessage();

        if ( curMsg != msg )
          getStatusBar()->setMessage(msg);
      }
    }
  }
  else
  {
    if ( isEnabled() && getStatusBar() )
      getStatusBar()->clearMessage();
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
  text_length = text.getLength();
  text_width = getColumnWidth(txt);
  hotkey = finalcut::getHotkey(text);

  if ( hotkey )
  {
    text_length--;
    text_width--;
  }

  updateSuperMenuDimensions();
}

//----------------------------------------------------------------------
void FMenuItem::addAccelerator (FKey key, FWidget* obj)
{
  auto root = getRootWidget();
  accelerator accel = { key, obj };

  if ( root && root->getAcceleratorList() )
  {
    accel_key = key;
    root->getAcceleratorList()->push_back(accel);
  }

  updateSuperMenuDimensions();
}

//----------------------------------------------------------------------
void FMenuItem::delAccelerator (FWidget* obj)
{
  auto root = getRootWidget();

  if ( root
    && root->getAcceleratorList()
    && ! root->getAcceleratorList()->empty() )
  {
    auto iter = root->getAcceleratorList()->begin();

    while ( iter != root->getAcceleratorList()->end() )
    {
      if ( iter->object == obj )
      {
        accel_key = 0;
        iter = root->getAcceleratorList()->erase(iter);
      }
      else
        ++iter;
    }
  }

  updateSuperMenuDimensions();
}

//----------------------------------------------------------------------
void FMenuItem::openMenu()
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
  updateTerminal();
  flush_out();
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
    passMouseEvent (smenu, ev, fc::MouseDoubleClick_Event);
  }

  if ( isMenuBar(super_menu) )
  {
    auto mbar = static_cast<FMenuBar*>(super_menu);
    passMouseEvent (mbar, ev, fc::MouseDoubleClick_Event);
  }

  if ( isWindowsMenu(super_menu) )
  {
    auto dgl = static_cast<FDialog*>(super_menu);
    passMouseEvent (dgl, ev, fc::MouseDoubleClick_Event);
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
    passMouseEvent (smenu, ev, fc::MouseDown_Event);
  }

  if ( isMenuBar(super_menu) )
  {
    auto mbar = static_cast<FMenuBar*>(super_menu);
    passMouseEvent (mbar, ev, fc::MouseDown_Event);
  }

  if ( isWindowsMenu(super_menu) )
  {
    auto dgl = static_cast<FDialog*>(super_menu);
    passMouseEvent (dgl, ev, fc::MouseDown_Event);
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
    passMouseEvent (smenu, ev, fc::MouseUp_Event);
  }

  if ( isMenuBar(super_menu) )
  {
    auto mbar = static_cast<FMenuBar*>(super_menu);
    passMouseEvent (mbar, ev, fc::MouseUp_Event);
  }

  if ( isWindowsMenu(super_menu) )
  {
    auto dgl = static_cast<FDialog*>(super_menu);
    passMouseEvent (dgl, ev, fc::MouseUp_Event);
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
    passMouseEvent (smenu, ev, fc::MouseMove_Event);
  }

  if ( isMenuBar(super_menu) )
  {
    auto mbar = static_cast<FMenuBar*>(super_menu);
    passMouseEvent (mbar, ev, fc::MouseMove_Event);
  }

  if ( isWindowsMenu(super_menu) )
  {
    auto dgl = static_cast<FDialog*>(super_menu);
    passMouseEvent (dgl, ev, fc::MouseMove_Event);
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
    auto focused_widget = static_cast<FWidget*>(ev->focusedWidget());
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
bool FMenuItem::isWindowsMenu (FWidget* w) const
{
  return ( w ) ? w->isDialogWidget() : false;
}

//----------------------------------------------------------------------
bool FMenuItem::isMenuBar (FWidget* w) const
{
  return ( w ) ? w->isInstanceOf("FMenuBar") : false;
}

//----------------------------------------------------------------------
bool FMenuItem::isMenu (FWidget* w) const
{
  if ( ! w )
    return false;

  bool m1 = w->isInstanceOf("FMenu");
  bool m2 = w->isInstanceOf("FDialogListMenu");
  return bool( m1 || m2 );
}


// private methods of FMenuItem
//----------------------------------------------------------------------
FMenuList* FMenuItem::getFMenuList (FWidget& widget)
{
  FMenuList* menu_list{};

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
  else
    menu_list = nullptr;

  return menu_list;
}

//----------------------------------------------------------------------
void FMenuItem::init (FWidget* parent)
{
  text_length = text.getLength();
  text_width = getColumnWidth(text);
  hotkey = finalcut::getHotkey(text);

  if ( hotkey > 0xff00 && hotkey < 0xff5f )  // full-width character
    hotkey -= 0xfee0;

  if ( hotkey )
  {
    text_length--;
    text_width--;
  }

  setGeometry (FPoint(1, 1), FSize(text_width + 2, 1), false);

  if ( ! parent )
    return;

  setSuperMenu (parent);

  if ( accel_key )
    addAccelerator (accel_key);

  auto menu_list = getFMenuList(*parent);

  if ( menu_list )
    menu_list->insert(this);

  if ( isMenuBar(parent) )  // Parent is menubar
  {
    auto menubar_ptr = static_cast<FMenuBar*>(parent);
    menubar_ptr->calculateDimensions();

    if ( hotkey )  // Meta + hotkey
      menubar_ptr->addAccelerator ( fc::Fmkey_meta + FKey(std::tolower(int(hotkey)))
                                  , this );

    addCallback  // for this element
    (
      "deactivate",
      F_METHOD_CALLBACK (parent, &FMenuBar::cb_item_deactivated)
    );
  }
  else if ( isMenu(parent) )  // Parent is menu
  {
    auto menu_ptr = static_cast<FMenu*>(parent);
    menu_ptr->calculateDimensions();
  }
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
void FMenuItem::processActivate()
{
  emitCallback("activate");
}

//----------------------------------------------------------------------
void FMenuItem::processDeactivate()
{
  emitCallback("deactivate");
}

//----------------------------------------------------------------------
void FMenuItem::createDialogList (FMenu* winmenu)
{
  winmenu->clear();

  if ( getDialogList() && ! getDialogList()->empty() )
  {
    auto first = getDialogList()->begin();
    auto iter = first;

    while ( iter != getDialogList()->end() && *iter )
    {
      auto win = static_cast<FDialog*>(*iter);

      if ( win )
      {
        FMenuItem* win_item{};
        uInt32 n = uInt32(std::distance(first, iter));
        // get the dialog title
        const auto& name = win->getText();

        try
        {
          // create a new dialog list item
          win_item = new FMenuItem (name, winmenu);
        }
        catch (const std::bad_alloc& ex)
        {
          std::cerr << bad_alloc_str << ex.what() << std::endl;
          return;
        }

        if ( n < 9 )
          win_item->addAccelerator (fc::Fmkey_1 + n);  // Meta + 1..9

        win_item->addCallback
        (
          "clicked",
          F_METHOD_CALLBACK (win_item, &FMenuItem::cb_switchToDialog),
          static_cast<FDataPtr>(win)
        );

        win->addCallback
        (
          "destroy",
          F_METHOD_CALLBACK (win_item, &FMenuItem::cb_destroyDialog)
        );

        win_item->associated_window = win;
      }

      ++iter;
    }
  }

  winmenu->calculateDimensions();
}

//----------------------------------------------------------------------
template <typename T>
void FMenuItem::passMouseEvent ( T widget, FMouseEvent* ev
                               , fc::events ev_type )
{
  if ( ! widget )
    return;

  const auto& t = ev->getTermPos();
  const auto& p2 = widget->termToWidgetPos(t);
  int b = ev->getButton();
  std::shared_ptr<FMouseEvent> _ev;

  try
  {
    _ev = std::make_shared<FMouseEvent>(ev_type, p2, t, b);
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
    return;
  }

  switch ( int(ev_type) )
  {
    case fc::MouseDoubleClick_Event:
      widget->onMouseDoubleClick(_ev.get());
      break;

    case fc::MouseDown_Event:
      widget->onMouseDown(_ev.get());
      break;

    case fc::MouseUp_Event:
      widget->onMouseUp(_ev.get());
      break;

    case fc::MouseMove_Event:
      widget->onMouseMove(_ev.get());
      break;
  }
}

//----------------------------------------------------------------------
void FMenuItem::cb_switchToDialog (FWidget*, FDataPtr data)
{
  auto win = static_cast<FDialog*>(data);

  if ( win )
  {
    auto focus = getFocusWidget();
    FAccelEvent a_ev (fc::Accelerator_Event, focus);
    FApplication::sendEvent (win, &a_ev);
  }
}

//----------------------------------------------------------------------
void FMenuItem::cb_destroyDialog (FWidget* widget, FDataPtr)
{
  auto win = static_cast<FDialog*>(widget);
  auto fapp = FApplication::getApplicationObject();

  if ( win && fapp )
  {
    delAccelerator(win);
    delCallback(win);
    associated_window = nullptr;
  }
}

//----------------------------------------------------------------------
void FMenuItem::processClicked()
{
  emitCallback("clicked");
}

}  // namespace finalcut
