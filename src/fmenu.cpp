/***********************************************************************
* fmenu.cpp - Widget FMenu                                             *
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

#include <vector>

#include "final/fapplication.h"
#include "final/fdialog.h"
#include "final/fmenu.h"
#include "final/fstatusbar.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FMenu
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FMenu::FMenu(FWidget* parent)
  : FWindow(parent)
  , item()
  , super_menu(0)
  , opened_sub_menu(0)
  , shown_sub_menu(0)
  , max_item_width(0)
  , hotkeypos(NOT_SET)
  , mouse_down(false)
  , has_checkable_items(false)
{
  init(parent);
}

//----------------------------------------------------------------------
FMenu::FMenu (const FString& txt, FWidget* parent)
  : FWindow(parent)
  , item(txt, parent)
  , super_menu(0)
  , opened_sub_menu(0)
  , shown_sub_menu(0)
  , max_item_width(0)
  , hotkeypos(NOT_SET)
  , mouse_down(false)
  , has_checkable_items(false)
{
  init(parent);
}

//----------------------------------------------------------------------
FMenu::~FMenu()  // destructor
{
  FApplication* fapp = static_cast<FApplication*>(getRootWidget());

  if ( ! fapp->isQuit() )
    switchToPrevWindow(this);  // Switch to previous window
}


// public methods of FMenu
//----------------------------------------------------------------------
bool FMenu::setMenuWidget (bool on)
{
  if ( isMenuWidget() == on )
    return true;

  return (flags.menu_widget = on);
}

//----------------------------------------------------------------------
void FMenu::setStatusbarMessage (const FString& msg)
{
  FWidget::setStatusbarMessage(msg);
  item.setStatusbarMessage(msg);
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
  if ( ! isVisible() )
    return;

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

//----------------------------------------------------------------------
void FMenu::onKeyPress (FKeyEvent* ev)
{
  FWidget* menu_bar;

  // looking for menu hotkey
  if ( hotkeyMenu(ev) )
    return;

  // looking for menu bar hotkey
  menu_bar = getMenuBar();

  if ( menu_bar )
  {
    FMenuBar* mbar = static_cast<FMenuBar*>(menu_bar);

    if ( mbar->hotkeyMenu(ev) )
      return;
  }

  switch ( ev->key() )
  {
    case fc::Fkey_up:
      keyUp();
      break;

    case fc::Fkey_down:
      keyDown();
      break;

    case fc::Fkey_left:
      keyLeft(ev);
      break;

    case fc::Fkey_right:
      keyRight(ev);
      break;

    case fc::Fkey_return:
    case fc::Fkey_enter:
      keyEnter();
      break;

    case fc::Fkey_escape:
    case fc::Fkey_escape_mintty:
      keyEscape();
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
  shown_sub_menu = 0;

  if ( ev->getButton() != fc::LeftButton )
  {
    if ( opened_sub_menu )
    {
      closeOpenedSubMenu();  // close opened sub menu

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

  if ( item_list.empty() )
    return;

  // Mouse pointer over menu list changed focus
  if ( mouseDownOverList (ev->getPos()) )
    redraw();

  // Open the sub menu to be opened
  openSubMenu(shown_sub_menu);
}

//----------------------------------------------------------------------
void FMenu::onMouseUp (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton )
    return;

  if ( mouse_down )
  {
    mouse_down = false;

    if ( item_list.empty() )
      return;

    // Mouse pointer over an entry in the menu list
    if ( mouseUpOverList (ev->getPos()) )
      return;

    // Click on a non-FMenuItem (border or separator line)
    mouseUpOverBorder();
  }
}

//----------------------------------------------------------------------
void FMenu::onMouseMove (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton )
    return;

  if ( ! isWindowActive() )
    setActiveWindow(this);

  if ( !  mouse_down || item_list.empty() )
    return;

  mouseStates ms =
  {
    false,  // focus_changed
    false,  // hide_sub_menu
    isMouseOverMenu (ev->getTermPos()),
    isMouseOverSubMenu (ev->getTermPos()),
    isMouseOverSuperMenu (ev->getTermPos()),
    isMouseOverMenuBar (ev->getTermPos())
  };

  shown_sub_menu = 0;

  // Mouse pointer over an entry in the menu list
  mouseMoveOverList (ev->getPos(), ms);

  if ( ms.mouse_over_submenu )
  {
    passEventToSubMenu(ev);  // Event handover to sub-menu
    return;
  }

  if ( ! ms.mouse_over_menu && ms.mouse_over_supermenu )
  {
    passEventToSuperMenu(ev);  // Event handover to super-menu
    return;
  }

  if ( ms.mouse_over_menubar )
  {
    passEventToMenuBar(ev);  // Event handover to the menu bar
    return;
  }

  if ( ! hasSelectedItem() && ms.mouse_over_menu )
  {
    mouseMoveOverBorder(ms);  // Mouse is over border or separator
  }

  if ( ms.focus_changed )
    redraw();

  if ( shown_sub_menu )
  {
    closeOpenedSubMenu();
    openSubMenu (shown_sub_menu);
  }
  else if ( ms.hide_sub_menu )
  {
    closeOpenedSubMenu();
    updateTerminal();
    flush_out();
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
bool FMenu::isMouseOverMenu (const FPoint& termpos)
{
  if ( getTermGeometry().contains(termpos) )
    return true;

  return false;
}

//----------------------------------------------------------------------
bool FMenu::isMouseOverSubMenu (const FPoint& termpos)
{
  if ( opened_sub_menu )
  {
    const FRect& submenu_geometry = opened_sub_menu->getTermGeometry();

    if ( submenu_geometry.contains(termpos) )
      return true;
  }

  return false;
}

//----------------------------------------------------------------------
bool FMenu::isMouseOverSuperMenu (const FPoint& termpos)
{
  FMenu* smenu = superMenuAt (termpos);

  if ( smenu )
    return true;

  return false;
}

//----------------------------------------------------------------------
bool FMenu::isMouseOverMenuBar (const FPoint& termpos)
{
  if ( getMenuBar()
    && isMenuBar(getMenuBar())
    && getMenuBar()->getTermGeometry().contains(termpos) )
  {
    return true;
  }

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
  item.setMenu(this);

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
  int item_X, item_Y, adjust_X;
  std::vector<FMenuItem*>::const_iterator iter, last;
  iter = item_list.begin();
  last = item_list.end();
  max_item_width = 10;  // minimum width

  // find the maximum item width
  while ( iter != last )
  {
    std::size_t item_width = (*iter)->getTextLength() + 2;
    FKey        accel_key  = (*iter)->accel_key;
    bool        has_menu   = (*iter)->hasMenu();

    if ( has_menu )
    {
      item_width += 3;
    }
    else if ( accel_key )
    {
      std::size_t accel_len = getKeyName(accel_key).getLength();
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
  setGeometry (adjust_X, getY(), max_item_width + 2, getCount() + 2);

  // set geometry of all items
  iter = item_list.begin();
  item_X = 1;
  item_Y = 1;

  while ( iter != last )
  {
    (*iter)->setGeometry (item_X, item_Y, max_item_width, 1);

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
  if ( x_pos + int(max_item_width) >= int(getDesktopWidth() - 1) )
  {
    x_pos = int(getDesktopWidth() - max_item_width + 1);
    // Menu to large for the screen
    if ( x_pos < 1 )
      x_pos = 1;
  }

  return x_pos;
}

//----------------------------------------------------------------------
void FMenu::openSubMenu (FMenu* sub_menu, bool select)
{
  // open sub menu

  if ( ! sub_menu )
    return;

  if ( sub_menu->isVisible() )
    return;

  if ( select )
  {
    sub_menu->selectFirstItem();

    if ( sub_menu->hasSelectedItem() )
      sub_menu->getSelectedItem()->setFocus();
  }

  sub_menu->setVisible();
  sub_menu->show();
  opened_sub_menu = sub_menu;
  raiseWindow (sub_menu);
  sub_menu->redraw();

  if ( getStatusBar() )
    getStatusBar()->drawMessage();

  updateTerminal();
  flush_out();
}

//----------------------------------------------------------------------
void FMenu::closeOpenedSubMenu()
{
  // close sub menu
  if ( ! opened_sub_menu )
    return;

  opened_sub_menu->hideSubMenus();
  opened_sub_menu->hide();
  opened_sub_menu = 0;
}

//----------------------------------------------------------------------
void FMenu::hideSubMenus()
{
  // hide all sub-menus
  if ( opened_sub_menu )
  {
    opened_sub_menu->hideSubMenus();
    opened_sub_menu->hide();
    opened_sub_menu = 0;
  }

  unselectItem();
}

//----------------------------------------------------------------------
void FMenu::hideSuperMenus()
{
  // hide all menus to the top
  FWidget* super = getSuperMenu();

  if ( ! super )
    return;

  if ( isMenuBar(super) )
  {
    FMenuBar* mbar = static_cast<FMenuBar*>(super);

    if ( mbar->hasSelectedItem() )
      mbar->leaveMenuBar();
  }
  else if ( isMenu(super) )
  {
    FMenu* m = static_cast<FMenu*>(super);
    m->hide();
    m->hideSuperMenus();
  }
  else if ( isWindowsMenu(super) )
  {
    FDialog* dgl = static_cast<FDialog*>(super);
    dgl->leaveMenu();
  }
}

//----------------------------------------------------------------------
bool FMenu::mouseDownOverList (FPoint mouse_pos)
{
  std::vector<FMenuItem*>::const_iterator iter, last;
  bool focus_changed = false;
  iter = item_list.begin();
  last = item_list.end();
  mouse_pos -= FPoint(getRightPadding(), getTopPadding());

  while ( iter != last )
  {
    int x1 = (*iter)->getX()
      , x2 = (*iter)->getX() + int((*iter)->getWidth())
      , y  = (*iter)->getY()
      , mouse_x = mouse_pos.getX()
      , mouse_y = mouse_pos.getY();

    if ( mouse_x >= x1
      && mouse_x < x2
      && mouse_y == y )
    {
      // Mouse pointer over item
      mouseDownSubmenu (*iter);
      mouseDownSelection (*iter, focus_changed);
    }

    ++iter;
  }

  return focus_changed;
}

//----------------------------------------------------------------------
void FMenu::mouseDownSubmenu (FMenuItem* m_item)
{
  if ( ! hasSelectedItem() )
    return;

  FMenuItem* sel_item = getSelectedItem();

  if ( ! sel_item
    || ! sel_item->hasMenu()
    || sel_item->getMenu() != opened_sub_menu )
  return;

  if ( sel_item != m_item )
    hideSubMenus();
  else
  {
    // unselect sub menu items
    opened_sub_menu->unselectItem();
    raiseWindow (opened_sub_menu);
    opened_sub_menu->redraw();
    sel_item->setFocus();

    if ( getStatusBar() )
      getStatusBar()->drawMessage();

    updateTerminal();
    flush_out();
  }
}

//----------------------------------------------------------------------
void FMenu::mouseDownSelection (FMenuItem* m_item, bool& focus_changed)
{
  if ( m_item->isSelected() )
    return;

  unselectItem();
  FWidget* focused_widget = getFocusWidget();
  FFocusEvent out (fc::FocusOut_Event);
  FApplication::queueEvent(focused_widget, &out);
  m_item->setSelected();
  setSelectedItem(m_item);
  m_item->setFocus();
  focus_changed = true;

  if ( focused_widget )
    focused_widget->redraw();

  if ( getStatusBar() )
    getStatusBar()->drawMessage();

  if ( m_item->hasMenu() )
  {
    FMenu* sub_menu = m_item->getMenu();
    if ( ! sub_menu->isVisible() )
      shown_sub_menu = sub_menu;
  }
}

//----------------------------------------------------------------------
bool FMenu::mouseUpOverList (FPoint mouse_pos)
{
  std::vector<FMenuItem*>::const_iterator iter, last;
  iter = item_list.begin();
  last = item_list.end();
  mouse_pos -= FPoint(getRightPadding(), getTopPadding());

  while ( iter != last )
  {
    int x1 = (*iter)->getX()
      , x2 = (*iter)->getX() + int((*iter)->getWidth())
      , y  = (*iter)->getY()
      , mouse_x = mouse_pos.getX()
      , mouse_y = mouse_pos.getY();

    if ( (*iter)->isSelected()
      && mouse_x >= x1
      && mouse_x < x2
      && mouse_y == y )
    {
      // Mouse pointer over item
      if ( (*iter)->hasMenu() )
      {
        FMenu* sub_menu = (*iter)->getMenu();
        if ( ! sub_menu->isVisible() )
          openSubMenu (sub_menu, SELECT_ITEM);
        else if ( opened_sub_menu )
        {
          opened_sub_menu->selectFirstItem();

          if ( opened_sub_menu->hasSelectedItem() )
            opened_sub_menu->getSelectedItem()->setFocus();

          opened_sub_menu->redraw();

          if ( getStatusBar() )
            getStatusBar()->drawMessage();

          updateTerminal();
          flush_out();
        }

        return true;
      }
      else
      {
        unselectItem();
        hide();
        hideSuperMenus();
        (*iter)->processClicked();
      }
    }

    ++iter;
  }

  return false;
}

//----------------------------------------------------------------------
void FMenu::mouseMoveOverList (FPoint mouse_pos, mouseStates& ms)
{
  std::vector<FMenuItem*>::const_iterator iter, last;
  iter = item_list.begin();
  last = item_list.end();
  mouse_pos -= FPoint(getRightPadding(), getTopPadding());

  while ( iter != last )
  {
    int x1 = (*iter)->getX()
      , x2 = (*iter)->getX() + int((*iter)->getWidth())
      , y  = (*iter)->getY()
      , mouse_x = mouse_pos.getX()
      , mouse_y = mouse_pos.getY();

    if ( mouse_x >= x1 && mouse_x < x2 && mouse_y == y )
      mouseMoveSelection (*iter, ms);
    else
      mouseMoveDeselection (*iter, ms);

    ++iter;
  }
}

//----------------------------------------------------------------------
void FMenu::mouseMoveSelection (FMenuItem* m_item, mouseStates& ms)
{
  if ( ! m_item->isEnabled()
    || m_item->isSelected()
    || m_item->isSeparator() )
    return;

  // Mouse pointer over item
  FWidget* focused_widget = getFocusWidget();
  FFocusEvent out (fc::FocusOut_Event);
  FApplication::queueEvent(focused_widget, &out);
  m_item->setSelected();
  setSelectedItem(m_item);
  m_item->setFocus();
  ms.focus_changed = true;

  if ( focused_widget )
    focused_widget->redraw();

  if ( getStatusBar() )
    getStatusBar()->drawMessage();

  // Sub menu handling
  if ( m_item->hasMenu() )
  {
    FMenu* sub_menu = m_item->getMenu();

    if ( ! sub_menu->isVisible() )
      shown_sub_menu = sub_menu;
  }
  else if ( opened_sub_menu )
    ms.hide_sub_menu = true;
}

//----------------------------------------------------------------------
void FMenu::mouseMoveDeselection (FMenuItem* m_item, mouseStates& ms)
{
  if ( ! ms.mouse_over_menu
    || ! m_item->isEnabled()
    || ! m_item->isSelected()
    || ms.mouse_over_submenu )
    return;

  // Unselect selected item without mouse focus
  m_item->unsetSelected();
  m_item->unsetFocus();

  if ( getSelectedItem() == m_item )
    setSelectedItem(0);

  ms.focus_changed = true;
}

//----------------------------------------------------------------------
void FMenu::mouseUpOverBorder()
{
  // Mouse button up over border or separator line

  unselectItem();
  hide();
  hideSuperMenus();
}

//----------------------------------------------------------------------
void FMenu::mouseMoveOverBorder (mouseStates& ms)
{
  // Mouse is moved over border or separator line

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

  if ( opened_sub_menu )
    ms.hide_sub_menu = true;
}

//----------------------------------------------------------------------
void FMenu::passEventToSubMenu (FMouseEvent*& ev)
{
  // Mouse event handover to sub-menu

  const FPoint& t = ev->getTermPos();
  const FPoint& p = opened_sub_menu->termToWidgetPos(t);
  int b = ev->getButton();

  try
  {
    FMouseEvent* _ev = new FMouseEvent (fc::MouseMove_Event, p, t, b);
    opened_sub_menu->mouse_down = true;
    setClickedWidget(opened_sub_menu);
    opened_sub_menu->onMouseMove(_ev);
    delete _ev;
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
  }
}

//----------------------------------------------------------------------
void FMenu::passEventToSuperMenu (FMouseEvent*& ev)
{
  // Mouse event handover to super-menu

  FMenu* smenu = superMenuAt (ev->getTermPos());
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
    std::cerr << bad_alloc_str << ex.what() << std::endl;
  }
}

//----------------------------------------------------------------------
void FMenu::passEventToMenuBar (FMouseEvent*& ev)
{
  // Mouse event handover to the menu bar

  FWidget* menu_bar = getMenuBar();
  const FPoint& t = ev->getTermPos();
  const FPoint& p = menu_bar->termToWidgetPos(t);
  int b = ev->getButton();

  try
  {
    FMouseEvent* _ev = new FMouseEvent (fc::MouseMove_Event, p, t, b);
    setClickedWidget(menu_bar);
    FMenuBar* mbar = static_cast<FMenuBar*>(menu_bar);
    mbar->mouse_down = true;
    mbar->onMouseMove(_ev);
    delete _ev;
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
  }
}

//----------------------------------------------------------------------
bool FMenu::containsMenuStructure (int x, int y)
{
  // Check mouse click position for item, menu and all sub menus

  FMenuItem* si = getSelectedItem();

  if ( getTermGeometry().contains(x, y) )
    return true;
  else if ( si && si->hasMenu() && opened_sub_menu )
    return si->getMenu()->containsMenuStructure(x, y);
  else if ( item.getTermGeometry().contains(x, y) )
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
      while ( ! next->isEnabled()
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
      while ( ! prev->isEnabled()
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
void FMenu::keypressMenuBar (FKeyEvent* ev)
{
  FMenuBar* mbar = getMenuBar();

  if ( mbar )
    mbar->onKeyPress(ev);
}

//----------------------------------------------------------------------
bool FMenu::hotkeyMenu (FKeyEvent* ev)
{
  std::vector<FMenuItem*>::const_iterator iter, last;
  iter = item_list.begin();
  last = item_list.end();

  while ( iter != last )
  {
    if ( (*iter)->hasHotkey() )
    {
      bool found = false;
      uChar hotkey = (*iter)->getHotkey();
      FKey key = ev->key();

      if ( std::isalpha(hotkey) || std::isdigit(hotkey) )
      {
        if ( FKey(std::tolower(hotkey)) == key
          || FKey(std::toupper(hotkey)) == key )
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
          openSubMenu (sub_menu, SELECT_ITEM);
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
std::size_t FMenu::getHotkeyPos ( wchar_t src[]
                                , wchar_t dest[]
                                , std::size_t length )
{
  // Find hotkey position in string
  // + generate a new string without the '&'-sign
  std::size_t pos = NOT_SET;
  wchar_t* txt = src;

  for (std::size_t i = 0; i < length; i++)
  {
    if ( i < length && txt[i] == L'&' && pos == NOT_SET )
    {
      pos = i;
      i++;
      src++;
    }

    *dest++ = *src++;
  }

  return pos;
}

//----------------------------------------------------------------------
void FMenu::draw()
{
  // Fill the background
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
  int y = 0;
  iter = item_list.begin();
  last = item_list.end();

  while ( iter != last )
  {
    if ( (*iter)->isSeparator() )
      drawSeparator (y);
    else
      drawMenuLine (*iter, y);

    ++iter;
    y++;
  }
}

//----------------------------------------------------------------------
inline void FMenu::drawSeparator (int y)
{
  setPrintPos (1, 2 + y);
  setColor (wc.menu_active_fg, wc.menu_active_bg);

  if ( isMonochron() )
    setReverse(true);

  if ( isNewFont() )
  {
    print (fc::NF_border_line_vertical_right);
    FString line ( std::size_t(getWidth()) - 2
                 , wchar_t(fc::BoxDrawingsHorizontal) );
    print (line);
    print (fc::NF_rev_border_line_vertical_left);
  }
  else
  {
    print (fc::BoxDrawingsVerticalAndRight);
    FString line ( std::size_t(getWidth()) - 2
                 , wchar_t(fc::BoxDrawingsHorizontal));
    print (line);
    print (fc::BoxDrawingsVerticalAndLeft);
  }

  if ( isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
inline void FMenu::drawMenuLine (FMenuItem* menuitem, int y)
{
  FString txt = menuitem->getText();
  menuText txtdata;
  std::size_t txt_length = txt.getLength();
  std::size_t to_char = txt_length;
  FKey accel_key   = menuitem->accel_key;
  bool is_enabled  = menuitem->isEnabled();
  bool is_selected = menuitem->isSelected();

  // Set screen position and attributes
  setLineAttributes (menuitem, y);

  // Draw check mark prefix for checkable items
  drawCheckMarkPrefix (menuitem);

  // Print leading blank space
  print (' ');

  try
  {
    txtdata.text = new wchar_t[txt_length + 1]();
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
    return;
  }

  hotkeypos = getHotkeyPos(txt.wc_str(), txtdata.text, txt_length);

  if ( hotkeypos != NOT_SET )
    to_char--;

  txtdata.length = to_char;
  txtdata.no_underline = menuitem->getFlags().no_underline;
  setCursorToHotkeyPosition (menuitem);

  if ( ! is_enabled || is_selected )
    txtdata.hotkeypos = NOT_SET;
  else
    txtdata.hotkeypos = hotkeypos;

  drawMenuText (txtdata);

  if ( menuitem->hasMenu() )
    drawSubMenuIndicator (to_char);
  else if ( accel_key )
    drawAcceleratorKey (to_char, accel_key);

  // Draw the trailing spaces of the selected line
  if ( is_selected )
    drawTrailingSpaces (to_char);

  if ( isMonochron() && is_enabled && is_selected )
    setReverse(true);

  delete[] txtdata.text;
}

//----------------------------------------------------------------------
inline void FMenu::drawCheckMarkPrefix (FMenuItem* menuitem)
{
  bool is_checked   = menuitem->isChecked();
  bool is_checkable = menuitem->checkable;
  bool is_radio_btn = menuitem->radio_button;

  if ( ! has_checkable_items )
    return;

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

//----------------------------------------------------------------------
inline void FMenu::drawMenuText (menuText& data)
{
  // Print menu text

  for (std::size_t z = 0; z < data.length; z++)
  {
    if ( ! std::iswprint(wint_t(data.text[z])) )
    {
      if ( ! isNewFont()
        && ( int(data.text[z]) < fc::NF_rev_left_arrow2
          || int(data.text[z]) > fc::NF_check_mark )
        && ! charEncodable(uInt(data.text[z])) )
      {
        data.text[z] = L' ';
      }
    }

    if ( z == data.hotkeypos )
    {
      setColor (wc.menu_hotkey_fg, wc.menu_hotkey_bg);

      if ( ! data.no_underline )
        setUnderline();

      print (data.text[z]);

      if ( ! data.no_underline )
        unsetUnderline();

      setColor();
    }
    else
      print (data.text[z]);
  }
}

//----------------------------------------------------------------------
inline void FMenu::drawSubMenuIndicator (std::size_t& startpos)
{
  std::size_t c = ( has_checkable_items ) ? 1 : 0;
  std::size_t len = max_item_width - (startpos + c + 3);

  if ( len > 0 )
  {
    // Print filling blank spaces
    print (FString(len, wchar_t(' ')));
    // Print BlackRightPointingPointer ►
    print (wchar_t(fc::BlackRightPointingPointer));
    startpos = max_item_width - (c + 2);
  }
}

//----------------------------------------------------------------------
inline void FMenu::drawAcceleratorKey (std::size_t& startpos, FKey accel_key)
{
  FString accel_name (getKeyName(accel_key));
  std::size_t c = ( has_checkable_items ) ? 1 : 0;
  std::size_t accel_len = accel_name.getLength();
  std::size_t len = max_item_width - (startpos + accel_len + c + 2);

  if ( len > 0 )
  {
    // Print filling blank spaces + accelerator key name
    FString spaces (len, wchar_t(' '));
    print (spaces + accel_name);
    startpos = max_item_width - (c + 2);
  }
}

//----------------------------------------------------------------------
inline void FMenu::drawTrailingSpaces (std::size_t startpos)
{
  std::size_t c = ( has_checkable_items ) ? 1 : 0;

  // Print trailing blank space
  for (std::size_t i = startpos + c; i < max_item_width - 1; i++)
    print (' ');
}

//----------------------------------------------------------------------
inline void FMenu::setLineAttributes (FMenuItem* menuitem, int y)
{
  bool is_enabled  = menuitem->isEnabled();
  bool is_selected = menuitem->isSelected();

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
}

//----------------------------------------------------------------------
inline void FMenu::setCursorToHotkeyPosition (FMenuItem* menuitem)
{
  bool is_checkable = menuitem->checkable;
  bool is_selected  = menuitem->isSelected();

  if ( hotkeypos == NOT_SET )
  {
    // set cursor to the first character
    if ( is_selected )
    {
      if ( is_checkable )
        menuitem->setCursorPos (3, 1);
      else
        menuitem->setCursorPos (2, 1);
    }
  }
  else
  {
    if ( is_selected )
    {
      // set cursor to the hotkey position
      if ( is_checkable )
        menuitem->setCursorPos (3 + int(hotkeypos), 1);
      else
        menuitem->setCursorPos (2 + int(hotkeypos), 1);
    }
  }
}

//----------------------------------------------------------------------
inline void FMenu::keyUp()
{
  selectPrevItem();
}

//----------------------------------------------------------------------
inline void FMenu::keyDown()
{
  selectNextItem();
}

//----------------------------------------------------------------------
inline void FMenu::keyLeft (FKeyEvent* ev)
{
  if ( isSubMenu() )
  {
    FMenu* smenu = static_cast<FMenu*>(getSuperMenu());
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
}

//----------------------------------------------------------------------
inline void FMenu::keyRight (FKeyEvent* ev)
{
  if ( hasSelectedItem() && getSelectedItem()->hasMenu() )
  {
    FMenu* sub_menu = getSelectedItem()->getMenu();

    if ( ! sub_menu->isVisible() )
      openSubMenu (sub_menu, SELECT_ITEM);
    else
      keypressMenuBar(ev);  // select next menu
  }
  else
    keypressMenuBar(ev);  // select next menu
}

//----------------------------------------------------------------------
inline void FMenu::keyEnter()
{
  if ( ! hasSelectedItem() )
    return;

  FMenuItem* sel_item = getSelectedItem();

  if ( sel_item->hasMenu() )
    openSubMenu (sel_item->getMenu(), SELECT_ITEM);
  else
  {
    unselectItem();
    hide();
    hideSuperMenus();
    sel_item->processClicked();
  }
}

//----------------------------------------------------------------------
inline void FMenu::keyEscape()
{
  unselectItem();
  hideSubMenus();
  hide();

  if ( isSubMenu() )
  {
    FMenu* smenu = static_cast<FMenu*>(getSuperMenu());

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
      switchToPrevWindow(this);
  }

  if ( getStatusBar() )
    getStatusBar()->drawMessage();

  updateTerminal();
  flush_out();
}

//----------------------------------------------------------------------
void FMenu::processActivate()
{
  emitCallback("activate");
}

}  // namespace finalcut
