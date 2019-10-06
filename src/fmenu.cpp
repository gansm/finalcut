/***********************************************************************
* fmenu.cpp - Widget FMenu                                             *
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
#include <vector>

#include "final/fapplication.h"
#include "final/fcolorpair.h"
#include "final/fdialog.h"
#include "final/fevent.h"
#include "final/fmenu.h"
#include "final/fmenubar.h"
#include "final/fmenuitem.h"
#include "final/fstatusbar.h"
#include "final/fwidgetcolors.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FMenu
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FMenu::FMenu(FWidget* parent)
  : FWindow(parent)
{
  init(parent);
}

//----------------------------------------------------------------------
FMenu::FMenu (const FString& txt, FWidget* parent)
  : FWindow(parent)
  , menuitem(txt, parent)
{
  init(parent);
}

//----------------------------------------------------------------------
FMenu::~FMenu()  // destructor
{
  auto fapp = FApplication::getApplicationObject();

  if ( ! fapp->isQuit() )
    switchToPrevWindow(this);  // Switch to previous window
}


// public methods of FMenu
//----------------------------------------------------------------------
bool FMenu::setMenuWidget (bool enable)
{
  if ( isMenuWidget() == enable )
    return true;

  return (setFlags().menu_widget = enable);
}

//----------------------------------------------------------------------
void FMenu::setStatusbarMessage (const FString& msg)
{
  FWidget::setStatusbarMessage(msg);
  menuitem.setStatusbarMessage(msg);
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
  const auto& t_geometry = getTermGeometryWithShadow();
  restoreVTerm (t_geometry);
  updateTerminal();
  flush_out();

  if ( ! isSubMenu() )
  {
    auto openmenu = static_cast<FMenu*>(getOpenMenu());

    if ( openmenu && openmenu != this )
      openmenu->hide();

    setOpenMenu(nullptr);
  }

  mouse_down = false;
}

//----------------------------------------------------------------------
void FMenu::onKeyPress (FKeyEvent* ev)
{
  // looking for menu hotkey
  if ( hotkeyMenu(ev) )
    return;

  // looking for menu bar hotkey
  auto menu_bar = getMenuBar();

  if ( menu_bar )
  {
    auto mbar = static_cast<FMenuBar*>(menu_bar);

    if ( mbar->hotkeyMenu(ev) )
      return;
  }

  switch ( ev->key() )
  {
    case fc::Fkey_up:
      selectPrevItem();
      break;

    case fc::Fkey_down:
      selectNextItem();
      break;

    case fc::Fkey_left:
      selectPrevMenu(ev);
      break;

    case fc::Fkey_right:
      selectNextMenu(ev);
      break;

    case fc::Fkey_return:
    case fc::Fkey_enter:
      acceptSelection();
      break;

    case fc::Fkey_escape:
    case fc::Fkey_escape_mintty:
      closeMenu();
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
  shown_sub_menu = nullptr;

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

  if ( getItemList().empty() )
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

    if ( getItemList().empty() )
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

  if ( !  mouse_down || getItemList().empty() )
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

  shown_sub_menu = nullptr;

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
void FMenu::cb_menuitem_toggled (FWidget* widget, FDataPtr)
{
  auto m_item = static_cast<FMenuItem*>(widget);

  if ( ! has_checkable_items )
    return;

  if ( ! m_item->isChecked() )
    return;

  auto list = getItemList();

  if ( list.empty() )
    return;

  for (auto&& item : list)
  {
    if ( item != m_item
      && item->isChecked()
      && isRadioMenuItem(item) )
    {
      item->unsetChecked();
    }
  }
}


// private methods of FMenu
//----------------------------------------------------------------------
bool FMenu::isWindowsMenu (const FWidget* w) const
{
  return w->isDialogWidget();
}

//----------------------------------------------------------------------
bool FMenu::isMenuBar (const FWidget* w) const
{
  return w->isInstanceOf("FMenuBar");
}

//----------------------------------------------------------------------
bool FMenu::isMenu (const FWidget* w) const
{
  return w->isInstanceOf("FMenu");
}

//----------------------------------------------------------------------
bool FMenu::isRadioMenuItem (const FWidget* w) const
{
  return w->isInstanceOf("FRadioMenuItem");
}

//----------------------------------------------------------------------
bool FMenu::isSubMenu() const
{
  auto super = getSuperMenu();

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
    const auto& submenu_geometry = opened_sub_menu->getTermGeometry();

    if ( submenu_geometry.contains(termpos) )
      return true;
  }

  return false;
}

//----------------------------------------------------------------------
bool FMenu::isMouseOverSuperMenu (const FPoint& termpos)
{
  const auto smenu = superMenuAt (termpos);

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
  setGeometry (FPoint(1, 1), FSize(10, 2), false);  // initialize geometry values
  setTransparentShadow();
  setMenuWidget();
  hide();

  const auto& wc = getFWidgetColors();
  setForegroundColor (wc.menu_active_fg);
  setBackgroundColor (wc.menu_active_bg);
  menuitem.setMenu(this);

  if ( parent )
  {
    if ( isMenuBar(parent) )
    {
      auto mbar = static_cast<FMenuBar*>(parent);
      mbar->calculateDimensions();
    }
    else if ( isMenu(parent) )
    {
      auto smenu = static_cast<FMenu*>(parent);
      smenu->calculateDimensions();
    }

    setSuperMenu(parent);
  }

  calculateDimensions();
}

//----------------------------------------------------------------------
void FMenu::calculateDimensions()
{
  max_item_width = 10;  // minimum width

  // find the maximum item width
  for (auto&& item : getItemList())
  {
    std::size_t item_width = item->getTextWidth() + 2;
    FKey        accel_key  = item->accel_key;
    bool        has_menu   = item->hasMenu();

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
  }

  int adjust_X = adjustX(getX());

  // set widget geometry
  setGeometry ( FPoint(adjust_X, getY())
              , FSize(max_item_width + 2, getCount() + 2) );

  // set geometry of all items
  int item_X = 1;
  int item_Y = 1;

  for (auto&& item : getItemList())
  {
    item->setGeometry (FPoint(item_X, item_Y), FSize(max_item_width, 1));

    if ( item->hasMenu() )
    {
      int menu_X = getTermX() + int(max_item_width) + 1;
      int menu_Y = item->getTermY() - 2;
      // set sub-menu position
      item->getMenu()->setPos (FPoint(menu_X, menu_Y), false);
    }

    item_Y++;
  }
}

//----------------------------------------------------------------------
void FMenu::adjustItems()
{
  for (auto&& item : getItemList())
  {
    if ( item->hasMenu() )
    {
      auto menu = item->getMenu();
      int menu_X = getTermX() + int(max_item_width) + 1;
      menu_X = menu->adjustX(menu_X);
      int menu_Y = item->getTermY() - 2;

      // set sub-menu position
      menu->setPos (FPoint(menu_X, menu_Y));

      // call sub-menu adjustItems()
      if ( menu->getCount() > 0 )
        menu->adjustItems();
    }
  }
}

//----------------------------------------------------------------------
int FMenu::adjustX (int x_pos)
{
  // Is menu outside on the right of the screen?
  if ( x_pos + int(max_item_width) >= int(getDesktopWidth() - 1) )
  {
    x_pos = int(getDesktopWidth() - max_item_width - 1);
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

  if ( sub_menu->isShown() )
    return;

  if ( select )
  {
    sub_menu->selectFirstItem();

    if ( sub_menu->hasSelectedItem() )
      sub_menu->getSelectedItem()->setFocus();
  }

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
  opened_sub_menu = nullptr;
}

//----------------------------------------------------------------------
void FMenu::hideSubMenus()
{
  // hide all sub-menus
  if ( opened_sub_menu )
  {
    opened_sub_menu->hideSubMenus();
    opened_sub_menu->hide();
    opened_sub_menu = nullptr;
  }

  unselectItem();
}

//----------------------------------------------------------------------
void FMenu::hideSuperMenus()
{
  // hide all menus to the top
  auto super = getSuperMenu();

  if ( ! super )
    return;

  if ( isMenuBar(super) )
  {
    auto mbar = static_cast<FMenuBar*>(super);

    if ( mbar->hasSelectedItem() )
      mbar->leaveMenuBar();
  }
  else if ( isMenu(super) )
  {
    auto m = static_cast<FMenu*>(super);
    m->hide();
    m->hideSuperMenus();
  }
  else if ( isWindowsMenu(super) )
  {
    auto dgl = static_cast<FDialog*>(super);
    dgl->leaveMenu();
  }
}

//----------------------------------------------------------------------
bool FMenu::mouseDownOverList (FPoint mouse_pos)
{
  bool focus_changed{false};
  mouse_pos -= FPoint(getRightPadding(), getTopPadding());

  for (auto&& item : getItemList())
  {
    int x1 = item->getX()
      , x2 = item->getX() + int(item->getWidth())
      , y  = item->getY()
      , mouse_x = mouse_pos.getX()
      , mouse_y = mouse_pos.getY();

    if ( mouse_x >= x1
      && mouse_x < x2
      && mouse_y == y )
    {
      // Mouse pointer over item
      mouseDownSubmenu (item);
      mouseDownSelection (item, focus_changed);
    }
  }

  return focus_changed;
}

//----------------------------------------------------------------------
void FMenu::mouseDownSubmenu (FMenuItem* m_item)
{
  if ( ! hasSelectedItem() )
    return;

  auto sel_item = getSelectedItem();

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
  auto focused_widget = getFocusWidget();
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
    auto sub_menu = m_item->getMenu();
    if ( ! sub_menu->isShown() )
      shown_sub_menu = sub_menu;
  }
}

//----------------------------------------------------------------------
bool FMenu::mouseUpOverList (FPoint mouse_pos)
{
  mouse_pos -= FPoint(getRightPadding(), getTopPadding());

  for (auto&& item : getItemList())
  {
    int x1 = item->getX()
      , x2 = item->getX() + int(item->getWidth())
      , y  = item->getY()
      , mouse_x = mouse_pos.getX()
      , mouse_y = mouse_pos.getY();

    if ( item->isSelected()
      && mouse_x >= x1
      && mouse_x < x2
      && mouse_y == y )
    {
      // Mouse pointer over item
      if ( item->hasMenu() )
      {
        auto sub_menu = item->getMenu();
        if ( ! sub_menu->isShown() )
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
        item->processClicked();
      }
    }
  }

  return false;
}

//----------------------------------------------------------------------
void FMenu::mouseMoveOverList (FPoint mouse_pos, mouseStates& ms)
{
  mouse_pos -= FPoint(getRightPadding(), getTopPadding());

  for (auto&& item : getItemList())
  {
    int x1 = item->getX()
      , x2 = item->getX() + int(item->getWidth())
      , y  = item->getY()
      , mouse_x = mouse_pos.getX()
      , mouse_y = mouse_pos.getY();

    if ( mouse_x >= x1 && mouse_x < x2 && mouse_y == y )
      mouseMoveSelection (item, ms);
    else
      mouseMoveDeselection (item, ms);
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
  auto focused_widget = getFocusWidget();
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
    auto sub_menu = m_item->getMenu();

    if ( ! sub_menu->isShown() )
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
    setSelectedItem(nullptr);

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
    const auto& msg = getStatusbarMessage();
    const auto& curMsg = getStatusBar()->getMessage();

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

  const auto& t = ev->getTermPos();
  const auto& p = opened_sub_menu->termToWidgetPos(t);
  int b = ev->getButton();

  try
  {
    const auto& _ev = \
        std::make_shared<FMouseEvent>(fc::MouseMove_Event, p, t, b);
    opened_sub_menu->mouse_down = true;
    setClickedWidget(opened_sub_menu);
    opened_sub_menu->onMouseMove(_ev.get());
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

  const auto& smenu = superMenuAt (ev->getTermPos());
  const auto& t = ev->getTermPos();
  const auto& p = smenu->termToWidgetPos(t);
  int b = ev->getButton();

  try
  {
    const auto& _ev = \
        std::make_shared<FMouseEvent>(fc::MouseMove_Event, p, t, b);
    smenu->mouse_down = true;
    setClickedWidget(smenu);
    smenu->onMouseMove(_ev.get());
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

  const auto& menu_bar = getMenuBar();
  const auto& t = ev->getTermPos();
  const auto& p = menu_bar->termToWidgetPos(t);
  int b = ev->getButton();

  try
  {
    const auto& _ev = \
        std::make_shared<FMouseEvent>(fc::MouseMove_Event, p, t, b);
    setClickedWidget(menu_bar);
    auto& mbar = *(static_cast<FMenuBar*>(menu_bar));
    mbar.mouse_down = true;
    mbar.onMouseMove(_ev.get());
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

  auto si = getSelectedItem();

  if ( getTermGeometry().contains(x, y) )
    return true;
  else if ( si && si->hasMenu() && opened_sub_menu )
    return si->getMenu()->containsMenuStructure(x, y);
  else if ( menuitem.getTermGeometry().contains(x, y) )
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

  auto super = getSuperMenu();

  if ( super && isMenu(super) )
  {
    if ( super->getTermGeometry().contains(x, y) )
      return static_cast<FMenu*>(super);
    else
    {
      auto smenu = static_cast<FMenu*>(getSuperMenu());

      if ( smenu )
        return smenu->superMenuAt(x, y);
    }
  }

  return 0;
}

//----------------------------------------------------------------------
bool FMenu::selectNextItem()
{
  auto list = getItemList();
  auto iter = list.begin();

  while ( iter != list.end() )
  {
    if ( (*iter)->isSelected() )
    {
      FMenuItem* next{};
      auto next_element = iter;

      do
      {
        ++next_element;
        if ( next_element == list.end() )
          next_element = list.begin();
        next = static_cast<FMenuItem*>(*next_element);
      }
      while ( ! next->isEnabled()
           || ! next->acceptFocus()
           || ! next->isShown()
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
  auto list = getItemList();
  auto iter = list.end();

  do
  {
    --iter;

    if ( (*iter)->isSelected() )
    {
      FMenuItem* prev;
      auto prev_element = iter;

      do
      {
        if ( prev_element == list.begin() )
          prev_element = list.end();
        --prev_element;
        prev = static_cast<FMenuItem*>(*prev_element);
      }
      while ( ! prev->isEnabled()
           || ! prev->acceptFocus()
           || ! prev->isShown()
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
  while ( iter != list.begin() );

  return true;
}

//----------------------------------------------------------------------
void FMenu::keypressMenuBar (FKeyEvent* ev)
{
  auto mbar = getMenuBar();

  if ( mbar )
    mbar->onKeyPress(ev);
}

//----------------------------------------------------------------------
bool FMenu::hotkeyMenu (FKeyEvent* ev)
{
  for (auto&& item : getItemList())
  {
    if ( item->hasHotkey() )
    {
      bool found{false};
      FKey hotkey = item->getHotkey();
      FKey key = ev->key();

      if ( hotkey > 0xff00 && hotkey < 0xff5f )  // full-width character
        hotkey -= 0xfee0;

      if ( std::isalpha(int(hotkey)) || std::isdigit(int(hotkey)) )
      {
        if ( FKey(std::tolower(int(hotkey))) == key
          || FKey(std::toupper(int(hotkey))) == key )
          found = true;
      }
      else if ( hotkey == key )
        found = true;

      if ( found )
      {
        if ( item->hasMenu() )
        {
          auto sub_menu = item->getMenu();
          unselectItem();
          item->setSelected();
          setSelectedItem (item);
          redraw();

          if ( ! sub_menu->isShown() )
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
          item->processClicked();
        }

        ev->accept();
        return true;
      }
    }
  }

  return false;
}

//----------------------------------------------------------------------
void FMenu::draw()
{
  // Fill the background
  const auto& wc = getFWidgetColors();
  setColor (wc.menu_active_fg, wc.menu_active_bg);

  if ( isMonochron() )
    setReverse(true);

  clearArea();
  drawBorder();
  drawItems();
  drawShadow(this);

  if ( isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
void FMenu::drawItems()
{
  int y = 0;

  for (auto&& item : getItemList())
  {
    if ( item->isSeparator() )
      drawSeparator (y);
    else
      drawMenuLine (item, y);

    y++;
  }
}

//----------------------------------------------------------------------
inline void FMenu::drawSeparator (int y)
{
  const auto& wc = getFWidgetColors();
  print() << FPoint(1, 2 + y)
          << FColorPair(wc.menu_active_fg, wc.menu_active_bg);

  if ( isMonochron() )
    setReverse(true);

  if ( isNewFont() )
  {
    print (fc::NF_border_line_vertical_right);
    FString line ( std::size_t(getWidth()) - 2
                 , fc::BoxDrawingsHorizontal );
    print (line);
    print (fc::NF_rev_border_line_vertical_left);
  }
  else
  {
    print (fc::BoxDrawingsVerticalAndRight);
    FString line ( std::size_t(getWidth()) - 2
                 , fc::BoxDrawingsHorizontal);
    print (line);
    print (fc::BoxDrawingsVerticalAndLeft);
  }

  if ( isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
inline void FMenu::drawMenuLine (FMenuItem* m_item, int y)
{
  FString txt(m_item->getText());
  menuText txtdata{};
  std::size_t column_width = getColumnWidth(txt);
  FKey accel_key   = m_item->accel_key;
  bool is_enabled  = m_item->isEnabled();
  bool is_selected = m_item->isSelected();

  // Set screen position and attributes
  setLineAttributes (m_item, y);

  // Draw check mark prefix for checkable items
  drawCheckMarkPrefix (m_item);

  // Print leading blank space
  print (' ');

  hotkeypos = finalcut::getHotkeyPos(txt, txtdata.text);

  if ( hotkeypos != NOT_SET )
    column_width--;

  txtdata.no_underline = m_item->getFlags().no_underline;
  setCursorToHotkeyPosition (m_item);

  if ( ! is_enabled || is_selected )
    txtdata.hotkeypos = NOT_SET;
  else
    txtdata.hotkeypos = hotkeypos;

  drawMenuText (txtdata);

  if ( m_item->hasMenu() )
    drawSubMenuIndicator (column_width);
  else if ( accel_key )
    drawAcceleratorKey (column_width, accel_key);

  // Draw the trailing spaces of the selected line
  if ( is_selected )
    drawTrailingSpaces (column_width);

  if ( isMonochron() && is_enabled && is_selected )
    setReverse(true);
}

//----------------------------------------------------------------------
inline void FMenu::drawCheckMarkPrefix (FMenuItem* m_item)
{
  bool is_checked   = m_item->isChecked();
  bool is_checkable = m_item->checkable;
  bool is_radio_btn = m_item->radio_button;

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
          print (fc::BlackCircle);     // BlackCircle ●
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
      const auto& wc = getFWidgetColors();
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

  for (std::size_t z{0}; z < data.text.getLength(); z++)
  {
    if ( ! std::iswprint(std::wint_t(data.text[z])) )
    {
      if ( ! isNewFont()
        && ( int(data.text[z]) < fc::NF_rev_left_arrow2
          || int(data.text[z]) > fc::NF_check_mark )
        && ! charEncodable(wchar_t(data.text[z])) )
      {
        data.text[z] = L' ';
      }
    }

    if ( z == data.hotkeypos )
    {
      const auto& wc = getFWidgetColors();
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
    print (FString(len, L' '));
    // Print BlackRightPointingPointer ►
    print (fc::BlackRightPointingPointer);
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
    FString spaces (len, L' ');
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
inline void FMenu::setLineAttributes (FMenuItem* m_item, int y)
{
  bool is_enabled  = m_item->isEnabled();
  bool is_selected = m_item->isSelected();
  const auto& wc = getFWidgetColors();

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

  print() << FPoint(2, 2 + y);
  setColor();
}

//----------------------------------------------------------------------
inline void FMenu::setCursorToHotkeyPosition (FMenuItem* m_item)
{
  bool is_checkable = m_item->checkable;
  bool is_selected  = m_item->isSelected();

  if ( hotkeypos == NOT_SET )
  {
    // set cursor to the first character
    if ( is_selected )
    {
      if ( is_checkable )
        m_item->setCursorPos (FPoint(3, 1));
      else
        m_item->setCursorPos (FPoint(2, 1));
    }
  }
  else
  {
    if ( is_selected )
    {
      // set cursor to the hotkey position
      auto x = getColumnWidth (m_item->getText(), hotkeypos);

      if ( is_checkable )
        m_item->setCursorPos (FPoint(3 + int(x), 1));
      else
        m_item->setCursorPos (FPoint(2 + int(x), 1));
    }
  }
}

//----------------------------------------------------------------------
inline void FMenu::selectPrevMenu (FKeyEvent* ev)
{
  if ( isSubMenu() )
  {
    auto smenu = static_cast<FMenu*>(getSuperMenu());
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
inline void FMenu::selectNextMenu (FKeyEvent* ev)
{
  if ( hasSelectedItem() && getSelectedItem()->hasMenu() )
  {
    auto sub_menu = getSelectedItem()->getMenu();

    if ( ! sub_menu->isShown() )
      openSubMenu (sub_menu, SELECT_ITEM);
    else
      keypressMenuBar(ev);  // select next menu
  }
  else
    keypressMenuBar(ev);  // select next menu
}

//----------------------------------------------------------------------
inline void FMenu::acceptSelection()
{
  if ( ! hasSelectedItem() )
    return;

  auto sel_item = getSelectedItem();

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
inline void FMenu::closeMenu()
{
  unselectItem();
  hideSubMenus();
  hide();

  if ( isSubMenu() )
  {
    auto smenu = static_cast<FMenu*>(getSuperMenu());

    if ( smenu->getSelectedItem() )
      smenu->getSelectedItem()->setFocus();

    smenu->redraw();
  }
  else
  {
    auto super = getSuperMenu();
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
