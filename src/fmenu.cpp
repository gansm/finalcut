/***********************************************************************
* fmenu.cpp - Widget FMenu                                             *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2015-2017 Markus Gans                                      *
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

#include <vector>

#include "final/fapplication.h"
#include "final/fdialog.h"
#include "final/fmenu.h"
#include "final/fstatusbar.h"

//----------------------------------------------------------------------
// class FMenu
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FMenu::FMenu(FWidget* parent)
  : FWindow(parent)
  , item(0)
  , super_menu(0)
  , open_sub_menu(0)
  , max_item_width(0)
  , mouse_down(false)
  , has_checkable_items(false)
{
  init(parent);
}

//----------------------------------------------------------------------
FMenu::FMenu (const FString& txt, FWidget* parent)
  : FWindow(parent)
  , item(0)
  , super_menu(0)
  , open_sub_menu(0)
  , max_item_width(0)
  , mouse_down(false)
  , has_checkable_items(false)
{
  try
  {
    item = new FMenuItem(txt, parent);
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
    return;
  }

  init(parent);
}

//----------------------------------------------------------------------
FMenu::~FMenu()  // destructor
{
  FApplication* fapp = static_cast<FApplication*>(getRootWidget());

  if ( ! fapp->isQuit() )
    switchToPrevWindow();
}


// public methods of FMenu
//----------------------------------------------------------------------
bool FMenu::setMenuWidget (bool on)
{
  if ( isMenuWidget() == on )
    return true;

  if ( on )
    flags |= fc::menu_widget;
  else
    flags &= ~fc::menu_widget;

  return on;
}

//----------------------------------------------------------------------
void FMenu::setStatusbarMessage (const FString& msg)
{
  FWidget::setStatusbarMessage(msg);

  if ( item )
    item->setStatusbarMessage(msg);
}

//----------------------------------------------------------------------
void FMenu::show()
{
  if ( ! isVisible() )
    return;

  FWindow::show();
}

//----------------------------------------------------------------------
void FMenu::hide()
{
  if ( isVisible() )
  {
    FWindow::hide();
    const FRect& t_geometry = getTermGeometryWithShadow();
    restoreVTerm (t_geometry);
    updateTerminal();
    flush_out();

    if ( ! isSubMenu() )
    {
      FMenu* open_menu = static_cast<FMenu*>(getOpenMenu());

      if ( open_menu && open_menu != this )
        open_menu->hide();

      setOpenMenu(0);
    }

    mouse_down = false;
  }
}

//----------------------------------------------------------------------
void FMenu::onKeyPress (FKeyEvent* ev)
{
  FWidget* menubar;

  // looking for menu hotkey
  if ( hotkeyMenu(ev) )
    return;

  // looking for menu bar hotkey
  menubar = getMenuBar();

  if ( menubar )
  {
    FMenuBar* mbar = reinterpret_cast<FMenuBar*>(menubar);

    if ( mbar->hotkeyMenu(ev) )
      return;
  }

  switch ( ev->key() )
  {
    case fc::Fkey_return:
    case fc::Fkey_enter:
      if ( hasSelectedItem() )
      {
        FMenuItem* sel_item = getSelectedItem();

        if ( sel_item->hasMenu() )
          openSubMenu (sel_item->getMenu());
        else
        {
          unselectItem();
          hide();
          hideSuperMenus();
          sel_item->processClicked();
        }
      }
      break;

    case fc::Fkey_up:
      selectPrevItem();
      break;

    case fc::Fkey_down:
      selectNextItem();
      break;

    case fc::Fkey_left:
      if ( isSubMenu() )
      {
        FMenu* smenu = reinterpret_cast<FMenu*>(getSuperMenu());
        hideSubMenus();
        hide();

        if ( smenu->getSelectedItem() )
          smenu->getSelectedItem()->setFocus();

        smenu->redraw();

        if ( getStatusBar() )
          getStatusBar()->drawMessage();

        updateTerminal();
        flush_out();
      }
      else
        keypressMenuBar(ev);  // select previous menu
      break;

    case fc::Fkey_right:
      if ( hasSelectedItem() && getSelectedItem()->hasMenu() )
      {
        FMenu* sub_menu = getSelectedItem()->getMenu();

        if ( ! sub_menu->isVisible() )
          openSubMenu (sub_menu);
        else
          keypressMenuBar(ev);  // select next menu
      }
      else
        keypressMenuBar(ev);  // select next menu
      break;

    case fc::Fkey_escape:
    case fc::Fkey_escape_mintty:
      unselectItem();
      hideSubMenus();
      hide();

      if ( isSubMenu() )
      {
        FMenu* smenu = reinterpret_cast<FMenu*>(getSuperMenu());

        if ( smenu->getSelectedItem() )
          smenu->getSelectedItem()->setFocus();

        smenu->redraw();
      }
      else
      {
        FWidget* super = getSuperMenu();
        hideSuperMenus();

        if ( getStatusBar() )
          getStatusBar()->clearMessage();

        if ( ! (super && isWindowsMenu(super)) )
          switchToPrevWindow();
      }

      if ( getStatusBar() )
        getStatusBar()->drawMessage();

      updateTerminal();
      flush_out();
      break;

    case fc::Fmkey_1:
    case fc::Fmkey_2:
    case fc::Fmkey_3:
    case fc::Fmkey_4:
    case fc::Fmkey_5:
    case fc::Fmkey_6:
    case fc::Fmkey_7:
    case fc::Fmkey_8:
    case fc::Fmkey_9:
      // do nothing:
      //   handle the dialog switch accelerator in FApplication
      return;

    default:
      break;
  }

  // always accept key event -> no forwarding to the parent widget
  ev->accept();
}

//----------------------------------------------------------------------
void FMenu::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton )
  {
    if ( open_sub_menu )
    {
      // close open sub menu
      open_sub_menu->hideSubMenus();
      open_sub_menu->hide();
      open_sub_menu = 0;

      if ( getSelectedItem() )
        getSelectedItem()->setFocus();

      redraw();

      if ( getStatusBar() )
        getStatusBar()->drawMessage();

      updateTerminal();
      flush_out();
    }

    return;
  }

  if ( mouse_down )
    return;

  if ( ! isWindowActive() )
    setActiveWindow(this);

  mouse_down = true;

  if ( ! item_list.empty() )
  {
    std::vector<FMenuItem*>::const_iterator iter, last;
    FMenu* show_sub_menu = 0;
    bool focus_changed = false;
    FPoint mouse_pos;

    iter = item_list.begin();
    last = item_list.end();
    mouse_pos = ev->getPos();
    mouse_pos -= FPoint(getRightPadding(), getTopPadding());

    while ( iter != last )
    {
      int x1 = (*iter)->getX()
        , x2 = (*iter)->getX() + (*iter)->getWidth()
        , y  = (*iter)->getY()
        , mouse_x = mouse_pos.getX()
        , mouse_y = mouse_pos.getY();

      if ( mouse_x >= x1
          && mouse_x < x2
          && mouse_y == y )
      {
        // Mouse pointer over item
        if ( hasSelectedItem() )
        {
          FMenuItem* sel_item = getSelectedItem();
          if ( sel_item
              && sel_item->hasMenu()
              && sel_item->getMenu() == open_sub_menu )
          {
            if ( sel_item != *iter )
              hideSubMenus();
            else
            {
              open_sub_menu->unselectItem();
              raiseWindow (open_sub_menu);
              open_sub_menu->redraw();
              sel_item->setFocus();

              if ( getStatusBar() )
                getStatusBar()->drawMessage();

              updateTerminal();
              flush_out();
            }
          }
        }

        if ( ! (*iter)->isSelected() )
        {
          unselectItem();
          FWidget* focused_widget = getFocusWidget();
          FFocusEvent out (fc::FocusOut_Event);
          FApplication::queueEvent(focused_widget, &out);
          (*iter)->setSelected();
          setSelectedItem(*iter);
          (*iter)->setFocus();

          if ( focused_widget )
            focused_widget->redraw();

          if ( getStatusBar() )
            getStatusBar()->drawMessage();

          if ( (*iter)->hasMenu() )
          {
            FMenu* sub_menu = (*iter)->getMenu();
            if ( ! sub_menu->isVisible() )
              show_sub_menu = sub_menu;
          }

          focus_changed = true;
        }
      }

      ++iter;
    }

    if ( focus_changed )
      redraw();

    if ( show_sub_menu )
    {
      // open sub menu
      show_sub_menu->setVisible();
      show_sub_menu->show();
      open_sub_menu = show_sub_menu;
      raiseWindow (show_sub_menu);
      show_sub_menu->redraw();
      updateTerminal();
      flush_out();
    }
  }
}

//----------------------------------------------------------------------
void FMenu::onMouseUp (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton )
    return;

  if ( mouse_down )
  {
    mouse_down = false;

    if ( ! item_list.empty() )
    {
      std::vector<FMenuItem*>::const_iterator iter, last;
      FPoint mouse_pos;
      iter = item_list.begin();
      last = item_list.end();
      mouse_pos = ev->getPos();
      mouse_pos -= FPoint(getRightPadding(), getTopPadding());

      while ( iter != last )
      {
        int x1 = (*iter)->getX()
          , x2 = (*iter)->getX() + (*iter)->getWidth()
          , y  = (*iter)->getY();

        if ( (*iter)->isSelected() )
        {
          int mouse_x = mouse_pos.getX();
          int mouse_y = mouse_pos.getY();

          if ( mouse_x >= x1
              && mouse_x < x2
              && mouse_y == y )
          {
            // Mouse pointer over item
            if ( (*iter)->hasMenu() )
            {
              FMenu* sub_menu = (*iter)->getMenu();
              if ( ! sub_menu->isVisible() )
                openSubMenu (sub_menu);
              else if ( open_sub_menu )
              {
                open_sub_menu->selectFirstItem();

                if ( open_sub_menu->hasSelectedItem() )
                  open_sub_menu->getSelectedItem()->setFocus();

                open_sub_menu->redraw();

                if ( getStatusBar() )
                  getStatusBar()->drawMessage();

                updateTerminal();
                flush_out();
              }

              return;
            }
            else
            {
              unselectItem();
              hide();
              hideSuperMenus();
              (*iter)->processClicked();
            }
          }
        }

        ++iter;
      }

      // Click on a non-FMenuItem (border or separator line)
      unselectItem();
      hide();
      hideSuperMenus();
    }
  }
}

//----------------------------------------------------------------------
void FMenu::onMouseMove (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton )
    return;

  if ( ! isWindowActive() )
    setActiveWindow(this);

  if ( mouse_down && ! item_list.empty() )
  {
    std::vector<FMenuItem*>::const_iterator iter, last;
    FMenu* smenu = 0;
    bool focus_changed = false
      , mouse_over_menu = false
      , mouse_over_submenu = false
      , mouse_over_supermenu = false
      , mouse_over_menubar = false
      , hide_sub_menu = false;
    FMenu* show_sub_menu = 0;
    FPoint mouse_pos;

    iter = item_list.begin();
    last = item_list.end();
    mouse_pos = ev->getPos();
    mouse_pos -= FPoint(getRightPadding(), getTopPadding());

    if ( getTermGeometry().contains(ev->getTermPos()) )
      mouse_over_menu = true;

    if ( open_sub_menu )
    {
      const FRect& submenu_geometry = open_sub_menu->getTermGeometry();
      if ( submenu_geometry.contains(ev->getTermPos()) )
        mouse_over_submenu = true;
    }

    if ( isSubMenu() )
    {
      smenu = superMenuAt (ev->getTermPos());

      if ( smenu )
        mouse_over_supermenu = true;
    }

    if ( getMenuBar()
        && isMenuBar(getMenuBar())
        && getMenuBar()->getTermGeometry().contains(ev->getTermPos()) )
    {
      mouse_over_menubar = true;
    }

    while ( iter != last )
    {
      int x1 = (*iter)->getX()
        , x2 = (*iter)->getX() + (*iter)->getWidth()
        , y  = (*iter)->getY()
        , mouse_x = mouse_pos.getX()
        , mouse_y = mouse_pos.getY();

      if ( mouse_x >= x1
          && mouse_x < x2
          && mouse_y == y )
      {
        if ( (*iter)->isEnabled()
            && ! (*iter)->isSelected()
            && ! (*iter)->isSeparator() )
        {
          // Mouse pointer over item
          FWidget* focused_widget = getFocusWidget();
          FFocusEvent out (fc::FocusOut_Event);
          FApplication::queueEvent(focused_widget, &out);
          (*iter)->setSelected();
          setSelectedItem(*iter);
          (*iter)->setFocus();

          if ( focused_widget )
            focused_widget->redraw();

          if ( getStatusBar() )
            getStatusBar()->drawMessage();

          // sub menu handling
          if ( (*iter)->hasMenu() )
          {
            FMenu* sub_menu = (*iter)->getMenu();

            if ( ! sub_menu->isVisible() )
              show_sub_menu = sub_menu;
          }
          else if ( open_sub_menu )
            hide_sub_menu = true;

          focus_changed = true;
        }
      }
      else
      {
        if ( mouse_over_menu
            && (*iter)->isEnabled()
            && (*iter)->isSelected()
            && ! mouse_over_submenu )
        {
          // Unselect selected item without mouse focus
          (*iter)->unsetSelected();
          (*iter)->unsetFocus();

          if ( getSelectedItem() == *iter )
            setSelectedItem(0);

          focus_changed = true;
        }
      }

      ++iter;
    }

    if ( mouse_over_submenu )
    {
      // Mouse event handover to sub-menu
      const FPoint& t = ev->getTermPos();
      const FPoint& p = open_sub_menu->termToWidgetPos(t);
      int b = ev->getButton();

      try
      {
        FMouseEvent* _ev = new FMouseEvent (fc::MouseMove_Event, p, t, b);
        open_sub_menu->mouse_down = true;
        setClickedWidget(open_sub_menu);
        open_sub_menu->onMouseMove(_ev);
        delete _ev;
      }
      catch (const std::bad_alloc& ex)
      {
        std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
      }

      return;
    }
    else if ( ! mouse_over_menu && mouse_over_supermenu )
    {
      // Mouse event handover to super-menu
      const FPoint& t = ev->getTermPos();
      const FPoint& p = smenu->termToWidgetPos(t);
      int b = ev->getButton();

      try
      {
        FMouseEvent* _ev = new FMouseEvent (fc::MouseMove_Event, p, t, b);
        smenu->mouse_down = true;
        setClickedWidget(smenu);
        smenu->onMouseMove(_ev);
        delete _ev;
      }
      catch (const std::bad_alloc& ex)
      {
        std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
      }

      return;
    }
    else if ( mouse_over_menubar )
    {
      // Mouse event handover to the menu bar
      FWidget* menubar = getMenuBar();
      const FPoint& t = ev->getTermPos();
      const FPoint& p = menubar->termToWidgetPos(t);
      int b = ev->getButton();

      try
      {
        FMouseEvent* _ev = new FMouseEvent (fc::MouseMove_Event, p, t, b);
        setClickedWidget(menubar);
        FMenuBar* mbar = reinterpret_cast<FMenuBar*>(menubar);
        mbar->mouse_down = true;
        mbar->onMouseMove(_ev);
        delete _ev;
      }
      catch (const std::bad_alloc& ex)
      {
        std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
      }

      return;
    }
    else if ( ! hasSelectedItem() && mouse_over_menu )
    {
      // Mouse is over border or separator
      if ( getStatusBar() )
      {
        const FString& msg = getStatusbarMessage();
        const FString& curMsg = getStatusBar()->getMessage();

        if ( curMsg != msg )
        {
          getStatusBar()->setMessage(msg);
          getStatusBar()->drawMessage();
        }
      }

      if ( open_sub_menu )
        hide_sub_menu = true;
    }

    if ( focus_changed )
      redraw();

    if ( show_sub_menu )
    {
      // close open sub menu
      if ( open_sub_menu )
      {
        open_sub_menu->hideSubMenus();
        open_sub_menu->hide();
      }

      // open sub menu
      show_sub_menu->setVisible();
      show_sub_menu->show();
      open_sub_menu = show_sub_menu;
      raiseWindow (show_sub_menu);
      show_sub_menu->redraw();
      updateTerminal();
      flush_out();
    }
    else if ( hide_sub_menu )
    {
      open_sub_menu->hideSubMenus();
      open_sub_menu->hide();
      open_sub_menu = 0;
      updateTerminal();
      flush_out();
    }
  }
}

//----------------------------------------------------------------------
void FMenu::cb_menuitem_toggled (FWidget* widget, data_ptr)
{
  FMenuItem* menuitem = static_cast<FMenuItem*>(widget);
  std::vector<FMenuItem*>::const_iterator iter, last;

  if ( ! has_checkable_items )
    return;

  if ( ! menuitem->isChecked() )
    return;

  if ( item_list.empty() )
    return;

  iter = item_list.begin();
  last = item_list.end();

  while ( iter != last )
  {
    if ( (*iter) != menuitem
        && (*iter)->isChecked()
        && isRadioMenuItem(*iter) )
    {
      (*iter)->unsetChecked();
    }

    ++iter;
  }
}


// private methods of FMenu
//----------------------------------------------------------------------
bool FMenu::isWindowsMenu (FWidget* w) const
{
  return w->isDialogWidget();
}

//----------------------------------------------------------------------
bool FMenu::isMenuBar (FWidget* w) const
{
  return w->isInstanceOf("FMenuBar");
}

//----------------------------------------------------------------------
bool FMenu::isMenu (FWidget* w) const
{
  return w->isInstanceOf("FMenu");
}

//----------------------------------------------------------------------
bool FMenu::isRadioMenuItem (FWidget* w) const
{
  return w->isInstanceOf("FRadioMenuItem");
}

//----------------------------------------------------------------------
bool FMenu::isSubMenu() const
{
  FWidget* super = getSuperMenu();

  if ( super && isMenu(super) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
void FMenu::init(FWidget* parent)
{
  setTopPadding(1);
  setLeftPadding(1);
  setBottomPadding(1);
  setRightPadding(1);
  setGeometry (1, 1, 10, 2, false);  // initialize geometry values
  setTransparentShadow();
  setMenuWidget();
  hide();

  setForegroundColor (wc.menu_active_fg);
  setBackgroundColor (wc.menu_active_bg);

  if ( item )
    item->setMenu(this);

  if ( parent )
  {
    if ( isMenuBar(parent) )
    {
      FMenuBar* mbar = static_cast<FMenuBar*>(parent);
      mbar->calculateDimensions();
    }
    else if ( isMenu(parent) )
    {
      FMenu* smenu = static_cast<FMenu*>(parent);
      smenu->calculateDimensions();
    }

    setSuperMenu(parent);
  }

  calculateDimensions();
}

//----------------------------------------------------------------------
void FMenu::calculateDimensions()
{
  int item_X
    , item_Y
    , adjust_X;
  std::vector<FMenuItem*>::const_iterator iter, last;
  iter = item_list.begin();
  last = item_list.end();
  max_item_width = 10;  // minimum width

  // find the maximum item width
  while ( iter != last )
  {
    uInt item_width = (*iter)->getTextLength() + 2;
    int  accel_key  = (*iter)->accel_key;
    bool has_menu   = (*iter)->hasMenu();

    if ( has_menu )
    {
      item_width += 3;
    }
    else if ( accel_key )
    {
      uInt accel_len = getKeyName(accel_key).getLength();
      item_width += accel_len + 2;
    }

    if ( has_checkable_items )
      item_width++;

    if ( item_width > max_item_width )
      max_item_width = item_width;

    ++iter;
  }

  adjust_X = adjustX(getX());

  // set widget geometry
  setGeometry (adjust_X, getY(), int(max_item_width + 2), int(getCount() + 2));

  // set geometry of all items
  iter = item_list.begin();
  item_X = 1;
  item_Y = 1;

  while ( iter != last )
  {
    (*iter)->setGeometry (item_X, item_Y, int(max_item_width), 1);

    if ( (*iter)->hasMenu() )
    {
      int menu_X = getTermX() + int(max_item_width) + 1;
      int menu_Y = (*iter)->getTermY() - 2;
      // set sub-menu position
      (*iter)->getMenu()->setPos (menu_X, menu_Y, false);
    }

    item_Y++;
    ++iter;
  }
}

//----------------------------------------------------------------------
void FMenu::adjustItems()
{
  std::vector<FMenuItem*>::const_iterator last, iter;
  iter = item_list.begin();
  last = item_list.end();

  while ( iter != last )
  {
    if ( (*iter)->hasMenu() )
    {
      int menu_X, menu_Y;
      FMenu* menu = (*iter)->getMenu();

      menu_X = getTermX() + int(max_item_width) + 1;
      menu_X = menu->adjustX(menu_X);
      menu_Y = (*iter)->getTermY() - 2;

      // set sub-menu position
      menu->setPos (menu_X, menu_Y);

      // call sub-menu adjustItems()
      if ( menu->getCount() > 0 )
        menu->adjustItems();
    }

    ++iter;
  }
}

//----------------------------------------------------------------------
int FMenu::adjustX (int x_pos)
{
  // Is menu outside on the right of the screen?
  if ( x_pos + int(max_item_width) >= getColumnNumber() - 1 )
  {
    x_pos = getColumnNumber() - int(max_item_width + 1);
    // Menu to large for the screen
    if ( x_pos < 1 )
      x_pos = 1;
  }

  return x_pos;
}

//----------------------------------------------------------------------
void FMenu::openSubMenu (FMenu* sub_menu)
{
  if ( sub_menu->isVisible() )
    return;

  // open sub menu
  sub_menu->selectFirstItem();

  if ( sub_menu->hasSelectedItem() )
    sub_menu->getSelectedItem()->setFocus();

  sub_menu->setVisible();
  sub_menu->show();
  open_sub_menu = sub_menu;
  raiseWindow (sub_menu);
  sub_menu->redraw();

  if ( getStatusBar() )
    getStatusBar()->drawMessage();

  updateTerminal();
  flush_out();
}

//----------------------------------------------------------------------
void FMenu::hideSubMenus()
{
  // hide all sub-menus
  if ( open_sub_menu )
  {
    open_sub_menu->hideSubMenus();
    open_sub_menu->hide();
    open_sub_menu = 0;
  }

  unselectItem();
}

//----------------------------------------------------------------------
void FMenu::hideSuperMenus()
{
  // hide all menus to the top
  FWidget* super = getSuperMenu();

  if ( super )
  {
    if ( isMenuBar(super) )
    {
      FMenuBar* mbar = reinterpret_cast<FMenuBar*>(super);

      if ( mbar->hasSelectedItem() )
        mbar->leaveMenuBar();
    }
    else if ( isMenu(super) )
    {
      FMenu* m = reinterpret_cast<FMenu*>(super);
      m->hide();
      m->hideSuperMenus();
    }
    else if ( isWindowsMenu(super) )
    {
      FDialog* dgl = reinterpret_cast<FDialog*>(super);
      dgl->leaveMenu();
    }
  }
}

//----------------------------------------------------------------------
bool FMenu::containsMenuStructure (int x, int y)
{
  // Check mouse click position for item, menu and all sub menus
  FMenuItem* si = getSelectedItem();

  if ( getTermGeometry().contains(x, y) )
    return true;
  else if ( si && si->hasMenu() && open_sub_menu )
    return si->getMenu()->containsMenuStructure(x, y);
  else if ( item && item->getTermGeometry().contains(x, y) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
FMenu* FMenu::superMenuAt (int x, int y)
{
  // Check mouse click position for super menu
  if ( getTermGeometry().contains(x, y) )
    return 0;

  FWidget* super = getSuperMenu();

  if ( super && isMenu(super) )
  {
    if ( super->getTermGeometry().contains(x, y) )
      return static_cast<FMenu*>(super);
    else
    {
      FMenu* smenu = static_cast<FMenu*>(getSuperMenu());

      if ( smenu )
        return smenu->superMenuAt(x, y);
    }
  }

  return 0;
}

//----------------------------------------------------------------------
bool FMenu::selectNextItem()
{
  std::vector<FMenuItem*>::const_iterator iter, last;
  iter = item_list.begin();
  last = item_list.end();

  while ( iter != last )
  {
    if ( (*iter)->isSelected() )
    {
      FMenuItem* next;
      std::vector<FMenuItem*>::const_iterator next_element;
      next_element = iter;

      do
      {
        ++next_element;
        if ( next_element == item_list.end() )
          next_element = item_list.begin();
        next = static_cast<FMenuItem*>(*next_element);
      }
      while (   ! next->isEnabled()
             || ! next->acceptFocus()
             || ! next->isVisible()
             || next->isSeparator() );

      if ( next == *iter )
        return false;

      unselectItem();
      next->setSelected();
      setSelectedItem(next);
      next->setFocus();

      if ( getStatusBar() )
        getStatusBar()->drawMessage();

      redraw();
      updateTerminal();
      flush_out();
      break;
    }

    ++iter;
  }

  return true;
}

//----------------------------------------------------------------------
bool FMenu::selectPrevItem()
{
  std::vector<FMenuItem*>::const_iterator iter, first;
  iter = item_list.end();
  first = item_list.begin();

  do
  {
    --iter;

    if ( (*iter)->isSelected() )
    {
      FMenuItem* prev;
      std::vector<FMenuItem*>::const_iterator prev_element;
      prev_element = iter;

      do
      {
        if ( prev_element == item_list.begin() )
          prev_element = item_list.end();
        --prev_element;
        prev = static_cast<FMenuItem*>(*prev_element);
      }
      while (   ! prev->isEnabled()
             || ! prev->acceptFocus()
             || ! prev->isVisible()
             || prev->isSeparator() );

      if ( prev == *iter )
        return false;

      unselectItem();
      prev->setSelected();
      setSelectedItem(prev);
      prev->setFocus();

      if ( getStatusBar() )
        getStatusBar()->drawMessage();

      redraw();
      updateTerminal();
      flush_out();
      break;
    }
  }
  while ( iter != first );

  return true;
}

//----------------------------------------------------------------------
void FMenu::keypressMenuBar (FKeyEvent*& ev)
{
  FMenuBar* mbar = getMenuBar();

  if ( mbar )
    mbar->onKeyPress(ev);
}

//----------------------------------------------------------------------
bool FMenu::hotkeyMenu (FKeyEvent*& ev)
{
  std::vector<FMenuItem*>::const_iterator iter, last;
  iter = item_list.begin();
  last = item_list.end();

  while ( iter != last )
  {
    if ( (*iter)->hasHotkey() )
    {
      bool found = false;
      int hotkey = (*iter)->getHotkey();
      int key = ev->key();

      if ( std::isalpha(hotkey) || std::isdigit(hotkey) )
      {
        if ( std::tolower(hotkey) == key || std::toupper(hotkey) == key )
          found = true;
      }
      else if ( hotkey == key )
        found = true;

      if ( found )
      {
        if ( (*iter)->hasMenu() )
        {
          FMenu* sub_menu = (*iter)->getMenu();
          unselectItem();
          (*iter)->setSelected();
          setSelectedItem (*iter);
          redraw();
          openSubMenu (sub_menu);
          sub_menu->redraw();
        }
        else
        {
          unselectItem();
          hideSubMenus();
          hide();
          hideSuperMenus();
          updateTerminal();
          flush_out();
          ev->accept();
          (*iter)->processClicked();
        }

        return true;
      }
    }

    ++iter;
  }

  return false;
}

//----------------------------------------------------------------------
int FMenu::getHotkeyPos (wchar_t*& src, wchar_t*& dest, uInt length)
{
  // find hotkey position in string
  // + generate a new string without the '&'-sign
  int hotkeypos = -1;
  wchar_t* txt = src;

  for (uInt i = 0; i < length; i++)
  {
    if ( i < length && txt[i] == L'&' && hotkeypos == -1 )
    {
      hotkeypos = int(i);
      i++;
      src++;
    }

    *dest++ = *src++;
  }

  return hotkeypos;
}

//----------------------------------------------------------------------
void FMenu::draw()
{
  // fill the background
  setColor (wc.menu_active_fg, wc.menu_active_bg);

  if ( isMonochron() )
    setReverse(true);

  clearArea();
  drawBorder();
  drawItems();
  drawShadow();

  if ( isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
void FMenu::drawItems()
{
  std::vector<FMenuItem*>::const_iterator iter, last;
  int c = 0;
  int y = 0;
  iter = item_list.begin();
  last = item_list.end();

  if ( has_checkable_items )
    c = 1;

  while ( iter != last )
  {
    if ( (*iter)->isSeparator() )
    {
      drawSeparator(y);
    }
    else
    {
      wchar_t* src;
      wchar_t* dest;
      wchar_t* item_text;
      FString txt;
      uInt txt_length;
      int  hotkeypos, to_char;
      int  accel_key     = (*iter)->accel_key;
      bool has_menu      = (*iter)->hasMenu()
        , is_enabled     = (*iter)->isEnabled()
        , is_checked     = (*iter)->isChecked()
        , is_checkable   = (*iter)->checkable
        , is_radio_btn   = (*iter)->radio_button
        , is_selected    = (*iter)->isSelected()
        , is_noUnderline = (((*iter)->getFlags() & fc::no_underline) != 0);

      if ( is_enabled )
      {
        if ( is_selected )
        {
          setForegroundColor (wc.menu_active_focus_fg);
          setBackgroundColor (wc.menu_active_focus_bg);

          if ( isMonochron() )
            setReverse(false);
        }
        else
        {
          setForegroundColor (wc.menu_active_fg);
          setBackgroundColor (wc.menu_active_bg);

          if ( isMonochron() )
            setReverse(true);
        }
      }
      else
      {
        setForegroundColor (wc.menu_inactive_fg);
        setBackgroundColor (wc.menu_inactive_bg);

        if ( isMonochron() )
          setReverse(true);
      }

      setPrintPos (2, 2 + y);
      setColor();

      if ( has_checkable_items )
      {
        if ( is_checkable )
        {
          if ( is_checked )
          {
            if ( is_radio_btn )
            {
              if ( isNewFont() )
                print (fc::NF_Bullet);  // NF_Bullet ●
              else
                print (fc::Bullet);     // Bullet ●
            }
            else
            {
              if ( isNewFont() )
                print (fc::NF_check_mark);  // NF_check_mark ✓
              else
                print (fc::SquareRoot);     // SquareRoot √
            }
          }
          else
          {
            setColor (wc.menu_inactive_fg, getBackgroundColor());

            if ( getEncoding() == fc::ASCII )
              print ('-');
            else
              print (fc::SmallBullet);  // ·

            setColor();
          }
        }
        else
          print (' ');
      }

      print (' ');
      txt = (*iter)->getText();
      txt_length = uInt(txt.getLength());

      try
      {
        item_text = new wchar_t[txt_length + 1]();
      }
      catch (const std::bad_alloc& ex)
      {
        std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
        return;
      }

      src  = const_cast<wchar_t*>(txt.wc_str());
      dest = const_cast<wchar_t*>(item_text);
      to_char = int(txt_length);
      hotkeypos = getHotkeyPos (src, dest, txt_length);

      if ( hotkeypos == -1 )
      {
        // set cursor to the first character
        if ( is_selected )
        {
          if ( is_checkable )
            (*iter)->setCursorPos (3, 1);
          else
            (*iter)->setCursorPos (2, 1);
        }
      }
      else
      {
        if ( is_selected )
        {
          // set cursor to the hotkey position
          if ( is_checkable )
            (*iter)->setCursorPos (3 + hotkeypos, 1);
          else
            (*iter)->setCursorPos (2 + hotkeypos, 1);
        }

        txt_length--;
        to_char--;
      }

      for (int z = 0; z < to_char; z++)
      {
        if ( ! std::iswprint(wint_t(item_text[z])) )
        {
          if ( ! isNewFont()
              && ( int(item_text[z]) < fc::NF_rev_left_arrow2
                  || int(item_text[z]) > fc::NF_check_mark )
              && ! charEncodable(uInt(item_text[z])) )
          {
            item_text[z] = L' ';
          }
        }

        if ( (z == hotkeypos) && is_enabled && ! is_selected )
        {
          setColor (wc.menu_hotkey_fg, wc.menu_hotkey_bg);

          if ( ! is_noUnderline )
            setUnderline();

          print (item_text[z]);

          if ( ! is_noUnderline )
            unsetUnderline();

          setColor();
        }
        else
          print (item_text[z]);
      }

      if ( has_menu )
      {
        int len = int(max_item_width) - (to_char + c + 3);

        if ( len > 0 )
        {
          print (FString(len, wchar_t(' ')));
          // BlackRightPointingPointer ►
          print (wchar_t(fc::BlackRightPointingPointer));
          to_char = int(max_item_width) - (c + 2);
        }
      }
      else if ( accel_key )
      {
        FString accel_name (getKeyName(accel_key));
        int accel_len = int(accel_name.getLength());
        int len = int(max_item_width) - (to_char + accel_len + c + 2);

        if ( len > 0 )
        {
          FString spaces (len, wchar_t(' '));
          print (spaces + accel_name);
          to_char = int(max_item_width) - (c + 2);
        }
      }

      if ( is_selected )
      {
        for (uInt i = uInt(to_char + c); i < max_item_width - 1; i++)
          print (' ');
      }

      if ( isMonochron() && is_enabled && is_selected )
        setReverse(true);

      delete[] item_text;
    }

    ++iter;
    y++;
  }
}

//----------------------------------------------------------------------
inline void FMenu::drawSeparator(int y)
{
  setPrintPos (1, 2 + y);
  setColor (wc.menu_active_fg, wc.menu_active_bg);

  if ( isMonochron() )
    setReverse(true);

  if ( isNewFont() )
  {
    print (fc::NF_border_line_vertical_right);
    FString line(getWidth() - 2, wchar_t(fc::BoxDrawingsHorizontal));
    print (line);
    print (fc::NF_rev_border_line_vertical_left);
  }
  else
  {
    print (fc::BoxDrawingsVerticalAndRight);
    FString line(getWidth() - 2, wchar_t(fc::BoxDrawingsHorizontal));
    print (line);
    print (fc::BoxDrawingsVerticalAndLeft);
  }

  if ( isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
void FMenu::processActivate()
{
  emitCallback("activate");
}
