/***********************************************************************
* fmenuitem.cpp - Widget FMenuItem                                     *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2015-2018 Markus Gans                                      *
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

#include "final/fapplication.h"
#include "final/fdialog.h"
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
  , text()
  , selected(false)
  , separator(false)
  , checkable(false)
  , checked(false)
  , radio_button(false)
  , dialog_index(false)
  , text_length(0)
  , hotkey(0)
  , accel_key(0)
  , menu(0)
  , super_menu(0)
  , associated_window(0)
{
  init (parent);
}

//----------------------------------------------------------------------
FMenuItem::FMenuItem (const FString& txt, FWidget* parent)
  : FWidget(parent)
  , text(txt)
  , selected(false)
  , separator(false)
  , checkable(false)
  , checked(false)
  , radio_button(false)
  , dialog_index(false)
  , text_length(0)
  , hotkey(0)
  , accel_key(0)
  , menu(0)
  , super_menu(0)
  , associated_window(0)
{
  init (parent);
}

//----------------------------------------------------------------------
FMenuItem::FMenuItem (FKey k, const FString& txt, FWidget* parent)
  : FWidget(parent)
  , text(txt)
  , selected(false)
  , separator(false)
  , checkable(false)
  , checked(false)
  , radio_button(false)
  , dialog_index(false)
  , text_length(0)
  , hotkey(0)
  , accel_key(k)
  , menu(0)
  , super_menu(0)
  , associated_window(0)
{
  init (parent);
}

//----------------------------------------------------------------------
FMenuItem::~FMenuItem()  // destructor
{
  if ( super_menu && (isMenu(super_menu) || isMenuBar(super_menu)) )
  {
    FMenuList* menu_list = getFMenuList(*super_menu);

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
bool FMenuItem::setEnable (bool on)
{
  FWidget::setEnable(on);
  FWidget* super = getSuperMenu();

  if ( on )
  {
    if ( super && isMenuBar(super) )
    {
      // Meta + hotkey
      super->addAccelerator ( fc::Fmkey_meta + FKey(std::tolower(hotkey))
                            , this );
    }
  }
  else
  {
    if ( super && isMenuBar(super) )
      super->delAccelerator (this);
  }

  return on;
}

//----------------------------------------------------------------------
bool FMenuItem::setFocus (bool on)
{
  FWidget::setFocus(on);

  if ( on )
  {
    if ( isEnabled() )
    {
      if ( ! selected )
      {
        FMenuList* menu_list = getFMenuList(*getSuperMenu());
        setSelected();

        if ( menu_list )
        {
          menu_list->unselectItem();
          menu_list->setSelectedItem(this);
        }

        if ( getStatusBar() )
          getStatusBar()->drawMessage();

        FWidget* parent = getSuperMenu();

        if ( isMenuBar(parent) )
        {
          FMenuBar* menubar_ptr = static_cast<FMenuBar*>(parent);

          if ( menubar_ptr )
            menubar_ptr->redraw();
        }
        else if ( isMenu(parent) )
        {
          FMenu* menu_ptr = static_cast<FMenu*>(parent);

          if ( menu_ptr )
            menu_ptr->redraw();
        }
      }

      if ( getStatusBar() )
      {
        const FString& msg = getStatusbarMessage();
        const FString& curMsg = getStatusBar()->getMessage();

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

  return on;
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
  text = txt;
  text_length = text.getLength();
  hotkey = hotKey();

  if ( hotkey )
    text_length--;

  setWidth(text_length);
}

//----------------------------------------------------------------------
void FMenuItem::addAccelerator (FKey key, FWidget* obj)
{
  FWidget* root = getRootWidget();
  accelerator accel = { key, obj };

  if ( root && root->accelerator_list )
  {
    accel_key = key;
    root->accelerator_list->push_back(accel);
  }

  if ( isMenu(super_menu) )
  {
    FMenu* menu_ptr = static_cast<FMenu*>(super_menu);

    if ( menu_ptr )
      menu_ptr->calculateDimensions();
  }
}

//----------------------------------------------------------------------
void FMenuItem::delAccelerator (FWidget* obj)
{
  FWidget* root = getRootWidget();

  if ( root
    && root->accelerator_list
    && ! root->accelerator_list->empty() )
  {
    FWidget::Accelerators::iterator iter;
    iter = root->accelerator_list->begin();

    while ( iter != root->accelerator_list->end() )
    {
      if ( iter->object == obj )
      {
        accel_key = 0;
        iter = root->accelerator_list->erase(iter);
      }
      else
        ++iter;
    }
  }

  if ( isMenu(super_menu) )
  {
    FMenu* menu_ptr = static_cast<FMenu*>(super_menu);

    if ( menu_ptr )
      menu_ptr->calculateDimensions();
  }
}

//----------------------------------------------------------------------
void FMenuItem::openMenu()
{
  FMenu* dd_menu;  // Drop-down menu
  FMenu* open_menu;

  if ( ! hasMenu() )
    return;

  dd_menu = getMenu();  // Drop-down menu

  if ( dd_menu->isVisible() )
    return;

  open_menu = static_cast<FMenu*>(getOpenMenu());

  if ( open_menu && open_menu != dd_menu )
  {
    open_menu->hide();
    open_menu->hideSubMenus();
  }

  if ( dialog_index )
    createDialogList (dd_menu);

  setOpenMenu(dd_menu);
  dd_menu->setVisible();
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
    FMenu* smenu = static_cast<FMenu*>(super_menu);
    smenu->onKeyPress(ev);
  }

  if ( isMenuBar(super_menu) )
  {
    FMenuBar* mbar = static_cast<FMenuBar*>(super_menu);

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
    FMenu* smenu = static_cast<FMenu*>(super_menu);
    passMouseEvent (smenu, ev, fc::MouseDoubleClick_Event);
  }

  if ( isMenuBar(super_menu) )
  {
    FMenuBar* mbar = static_cast<FMenuBar*>(super_menu);
    passMouseEvent (mbar, ev, fc::MouseDoubleClick_Event);
  }

  if ( isWindowsMenu(super_menu) )
  {
    FDialog* dgl = static_cast<FDialog*>(super_menu);
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
    FMenu* smenu = static_cast<FMenu*>(super_menu);
    passMouseEvent (smenu, ev, fc::MouseDown_Event);
  }

  if ( isMenuBar(super_menu) )
  {
    FMenuBar* mbar = static_cast<FMenuBar*>(super_menu);
    passMouseEvent (mbar, ev, fc::MouseDown_Event);
  }

  if ( isWindowsMenu(super_menu) )
  {
    FDialog* dgl = static_cast<FDialog*>(super_menu);
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
    FMenu* smenu = static_cast<FMenu*>(super_menu);
    passMouseEvent (smenu, ev, fc::MouseUp_Event);
  }

  if ( isMenuBar(super_menu) )
  {
    FMenuBar* mbar = static_cast<FMenuBar*>(super_menu);
    passMouseEvent (mbar, ev, fc::MouseUp_Event);
  }

  if ( isWindowsMenu(super_menu) )
  {
    FDialog* dgl = static_cast<FDialog*>(super_menu);
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
    FMenu* smenu = static_cast<FMenu*>(super_menu);
    passMouseEvent (smenu, ev, fc::MouseMove_Event);
  }

  if ( isMenuBar(super_menu) )
  {
    FMenuBar* mbar = static_cast<FMenuBar*>(super_menu);
    passMouseEvent (mbar, ev, fc::MouseMove_Event);
  }

  if ( isWindowsMenu(super_menu) )
  {
    FDialog* dgl = static_cast<FDialog*>(super_menu);
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

  FMenuBar* mbar = static_cast<FMenuBar*>(super_menu);

  if ( menu )
  {
    FWidget* focused_widget;

    if ( mbar->getSelectedItem() )
      mbar->getSelectedItem()->unsetSelected();

    setSelected();
    mbar->selected_item = this;
    openMenu();

    focused_widget = static_cast<FWidget*>(ev->focusedWidget());

    if ( focused_widget && focused_widget->isWidget() )
    {
      FFocusEvent out (fc::FocusOut_Event);
      FApplication::queueEvent(focused_widget, &out);
    }

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
    mbar->selected_item = 0;
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
    FMenuBar* mbar = static_cast<FMenuBar*>(super_menu);
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
  FMenuList* menu_list;

  if ( isMenu(&widget) )
  {
    FMenu* Menu = static_cast<FMenu*>(&widget);
    menu_list = static_cast<FMenuList*>(Menu);
  }
  else if ( isMenuBar(&widget) )
  {
    FMenuBar* Menubar = static_cast<FMenuBar*>(&widget);
    menu_list = static_cast<FMenuList*>(Menubar);
  }
  else
    menu_list = 0;

  return menu_list;
}

//----------------------------------------------------------------------
void FMenuItem::init (FWidget* parent)
{
  text_length = text.getLength();
  hotkey = hotKey();

  if ( hotkey )
    text_length--;

  setGeometry (1, 1, text_length + 2, 1, false);

  if ( ! parent )
    return;

  setSuperMenu (parent);

  if ( accel_key )
    addAccelerator (accel_key);

  FMenuList* menu_list = getFMenuList(*parent);

  if ( menu_list )
    menu_list->insert(this);

  if ( isMenuBar(parent) )  // Parent is menubar
  {
    FMenuBar* menubar_ptr = static_cast<FMenuBar*>(parent);
    menubar_ptr->calculateDimensions();

    if ( hotkey )  // Meta + hotkey
      menubar_ptr->addAccelerator ( fc::Fmkey_meta + FKey(std::tolower(hotkey))
                                  , this );

    addCallback  // for this element
    (
      "deactivate",
      F_METHOD_CALLBACK (parent, &FMenuBar::cb_item_deactivated)
    );
  }
  else if ( isMenu(parent) )  // Parent is menu
  {
    FMenu* menu_ptr = static_cast<FMenu*>(parent);
    menu_ptr->calculateDimensions();
  }
}

//----------------------------------------------------------------------
uChar FMenuItem::hotKey()
{
  std::size_t length;

  if ( text.isEmpty() )
    return 0;

  length = text.getLength();

  for (std::size_t i = 0; i < length; i++)
  {
    try
    {
      if ( i + 1 < length && text[i] == '&' )
        return uChar(text[++i]);
    }
    catch (const std::out_of_range&)
    {
      return 0;
    }
  }

  return 0;
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

  if ( dialog_list && ! dialog_list->empty() )
  {
    widgetList::const_iterator iter, first;
    iter = first = dialog_list->begin();

    while ( iter != dialog_list->end() && *iter )
    {
      FDialog* win = static_cast<FDialog*>(*iter);

      if ( win )
      {
        FMenuItem* win_item;
        uInt32 n = uInt32(std::distance(first, iter));
        // get the dialog title
        const FString& name = win->getText();

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
          static_cast<FWidget::data_ptr>(win)
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

  FMouseEvent* _ev;
  const FPoint& t = ev->getTermPos();
  const FPoint& p2 = widget->termToWidgetPos(t);
  int b = ev->getButton();

  try
  {
    _ev = new FMouseEvent (ev_type, p2, t, b);
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
    return;
  }

  switch ( int(ev_type) )
  {
    case fc::MouseDoubleClick_Event:
      widget->onMouseDoubleClick(_ev);
      break;

    case fc::MouseDown_Event:
      widget->onMouseDown(_ev);
      break;

    case fc::MouseUp_Event:
      widget->onMouseUp(_ev);
      break;

    case fc::MouseMove_Event:
      widget->onMouseMove(_ev);
      break;
  }

  delete _ev;
}

//----------------------------------------------------------------------
void FMenuItem::cb_switchToDialog (FWidget*, data_ptr data)
{
  FDialog* win = static_cast<FDialog*>(data);

  if ( win )
  {
    FWidget* focus_widget = getFocusWidget();
    FAccelEvent a_ev (fc::Accelerator_Event, focus_widget);
    FApplication::sendEvent (win, &a_ev);
  }
}

//----------------------------------------------------------------------
void FMenuItem::cb_destroyDialog (FWidget* widget, data_ptr)
{
  FDialog* win = static_cast<FDialog*>(widget);
  FApplication* fapp = static_cast<FApplication*>(getRootWidget());

  if ( win && fapp )
  {
    delAccelerator(win);
    delCallback(win);
    associated_window = 0;
  }
}

//----------------------------------------------------------------------
void FMenuItem::processClicked()
{
  emitCallback("clicked");
}

}  // namespace finalcut
