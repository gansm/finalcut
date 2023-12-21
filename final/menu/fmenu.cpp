/***********************************************************************
* fmenu.cpp - Widget FMenu                                             *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2023 Markus Gans                                      *
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
#include <tuple>
#include <utility>
#include <vector>

#include "final/dialog/fdialog.h"
#include "final/fapplication.h"
#include "final/fwidgetcolors.h"
#include "final/menu/fmenubar.h"
#include "final/menu/fmenu.h"
#include "final/menu/fmenuitem.h"
#include "final/util/flog.h"
#include "final/vterm/fcolorpair.h"
#include "final/widget/fstatusbar.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FMenu
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FMenu::FMenu(FWidget* parent)
  : FWindow{parent}
{
  init();
}

//----------------------------------------------------------------------
FMenu::FMenu (FString&& txt, FWidget* parent)
  : FWindow{parent}
  , menuitem{std::move(txt), parent}
{
  init();
}

//----------------------------------------------------------------------
FMenu::~FMenu()  // destructor
{
  if ( ! FApplication::isQuit() )
    switchToPrevWindow(this);  // Switch to previous window
}


// public methods of FMenu
//----------------------------------------------------------------------
void FMenu::setMenuWidget (bool enable)
{
  if ( isMenuWidget() == enable )
    return;

  setFlags().type.menu_widget = enable;
}

//----------------------------------------------------------------------
void FMenu::setStatusbarMessage (const FString& msg)
{
  FWidget::setStatusbarMessage(msg);
  menuitem.setStatusbarMessage(msg);
}

//----------------------------------------------------------------------
void FMenu::resetColors()
{
  const auto& wc = getColorTheme();
  setForegroundColor (wc->menu.fg);
  setBackgroundColor (wc->menu.bg);
  FWidget::resetColors();
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

  if ( menu_bar && menu_bar->hotkeyMenu(ev) )
    return;

  const auto key = ev->key();
  // Ignore these keys:
  //   Dialog Switch Accelerator Handling in FApplication
  std::array<FKey, 9> ignore_list =  //  Meta-1..9
  {{
    FKey::Meta_1, FKey::Meta_2, FKey::Meta_3,
    FKey::Meta_4, FKey::Meta_5, FKey::Meta_6,
    FKey::Meta_7, FKey::Meta_8, FKey::Meta_9
  }};

  if ( std::any_of( std::cbegin(ignore_list)
                  , std::cend(ignore_list)
                  , [&key] (const auto& k) { return key == k; } ) )
    return;

  if ( key == FKey::Up )
    selectPrevItem();
  else if ( key == FKey::Down )
    selectNextItem();
  else if ( key == FKey::Left )
    selectPrevMenu(ev);
  else if ( key == FKey::Right )
    selectNextMenu(ev);
  else if ( isEnterKey(key) )
    acceptSelection();
  else if ( isEscapeKey(key) )
    closeMenu();

  // always accept key event -> no forwarding to the parent widget
  ev->accept();
}

//----------------------------------------------------------------------
void FMenu::onMouseDown (FMouseEvent* ev)
{
  shown_sub_menu = nullptr;

  if ( ev->getButton() != MouseButton::Left )
  {
    if ( opened_sub_menu )
    {
      closeOpenedSubMenu();  // close opened sub menu

      if ( getSelectedItem() )
        getSelectedItem()->setFocus();

      redraw();
      drawStatusBarMessage();
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
  openSubMenu (shown_sub_menu, SelectItem::No);
}

//----------------------------------------------------------------------
void FMenu::onMouseUp (FMouseEvent* ev)
{
  if ( ev->getButton() != MouseButton::Left )
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
  if ( ev->getButton() != MouseButton::Left )
    return;

  if ( ! isWindowActive() )
    setActiveWindow(this);

  if ( !  mouse_down || getItemList().empty() )
    return;

  MouseStates ms = initializeMouseStates(ev);

  shown_sub_menu = nullptr;

  // Mouse pointer over an entry in the menu list
  mouseMoveOverList (ev->getPos(), ms);

  if ( handleSubMenuEvent(ms, *ev)    // Event handover to sub-menu
    || handleSuperMenuEvent(ms, *ev)  // Event handover to super-menu
    || handleMenuBarEvent(ms, *ev) )  // Event handover to the menu bar
  {
    return;
  }

  if ( ! hasSelectedItem() && ms.mouse_over_menu )
  {
    mouseMoveOverBorder(ms);  // Mouse is over border or separator
  }

  if ( ms.focus_changed )
    redraw();

  handleCloseSubMenu(ms);
}

//----------------------------------------------------------------------
void FMenu::cb_menuitemEnabled()
{
  setEnable();
}

//----------------------------------------------------------------------
void FMenu::cb_menuitemDisabled()
{
  setDisable();
}

//----------------------------------------------------------------------
void FMenu::cb_menuitemToggled (const FMenuItem* m_item) const
{
  if ( ! (has_checkable_items && m_item && m_item->isChecked()) )
    return;

  const auto& list = getItemList();

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
auto FMenu::isDialog (const FWidget* w) const -> bool
{
  return w->isDialogWidget();
}

//----------------------------------------------------------------------
auto FMenu::isMenuBar (const FWidget* w) const -> bool
{
  return w->isInstanceOf("FMenuBar");
}

//----------------------------------------------------------------------
auto FMenu::isMenu (const FWidget* w) const -> bool
{
  return w->isInstanceOf("FMenu");
}

//----------------------------------------------------------------------
auto FMenu::isRadioMenuItem (const FWidget* w) const -> bool
{
  return w->isInstanceOf("FRadioMenuItem");
}

//----------------------------------------------------------------------
auto FMenu::isSubMenu() const -> bool
{
  const auto& super = getSuperMenu();

  return ( super && isMenu(super) );
}

//----------------------------------------------------------------------
auto FMenu::isDialogMenu() const -> bool
{
  const auto& super = getSuperMenu();
  return super ? super->isDialogWidget() : false;
}

//----------------------------------------------------------------------
auto FMenu::isMouseOverMenu (const FPoint& termpos) -> bool
{
  return getTermGeometry().contains(termpos);
}

//----------------------------------------------------------------------
auto FMenu::isMouseOverSubMenu (const FPoint& termpos) -> bool
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
auto FMenu::isMouseOverSuperMenu (const FPoint& termpos) -> bool
{
  const auto& smenu = superMenuAt(termpos);
  return smenu;
}

//----------------------------------------------------------------------
auto FMenu::isMouseOverMenuBar (const FPoint& termpos) const -> bool
{
  return ( getMenuBar()
        && isMenuBar(getMenuBar())
        && getMenuBar()->getTermGeometry().contains(termpos) );
}

//----------------------------------------------------------------------
void FMenu::init()
{
  setTopPadding(1);
  setLeftPadding(1);
  setBottomPadding(1);
  setRightPadding(1);
  FWindow::setGeometry (FPoint{1, 1}, FSize{10, 2}, false);  // initialize geometry values
  setTransparentShadow();
  setMenuWidget();
  FMenu::hide();
  FMenu::resetColors();
  menuitem.setMenu(this);
  const auto& parent = getParentWidget();

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

  initCallbacks();
  calculateDimensions();
}

//----------------------------------------------------------------------
void FMenu::initCallbacks()
{
  menuitem.addCallback
  (
    "enable",
    this, &FMenu::cb_menuitemEnabled
  );

  menuitem.addCallback
  (
    "disable",
    this, &FMenu::cb_menuitemDisabled
  );
}

//----------------------------------------------------------------------
void FMenu::calculateDimensions()
{
  max_item_width = 10;  // minimum width

  // find the maximum item width
  for (auto&& item : getItemList())
  {
    std::size_t item_width = item->getTextWidth() + 2;
    const auto& accel_key = item->accel_key;
    const bool has_menu = item->hasMenu();

    if ( has_menu )
    {
      item_width += 3;
    }
    else if ( accel_key != FKey::None )
    {
      const std::size_t accel_len = FVTerm::getFOutput()->getKeyName(accel_key).getLength();
      item_width += accel_len + 2;
    }

    if ( has_checkable_items )
      item_width++;

    if ( item_width > max_item_width )
      max_item_width = item_width;
  }

  const int adjust_X = adjustX(getX());

  // set widget geometry
  setGeometry ( FPoint{adjust_X, getY()}
              , FSize{max_item_width + 2, getCount() + 2} );

  // set geometry of all items
  const int item_X = 1;
  int item_Y = 1;

  for (auto&& item : getItemList())
  {
    item->setGeometry (FPoint{item_X, item_Y}, FSize{max_item_width, 1});

    if ( item->hasMenu() )
    {
      const int menu_X = getTermX() + int(max_item_width) + 1;
      const int menu_Y = item->getTermY() - 2;
      // set sub-menu position
      item->getMenu()->setPos (FPoint{menu_X, menu_Y}, false);
    }

    item_Y++;
  }
}

//----------------------------------------------------------------------
void FMenu::adjustItems() const
{
  for (auto&& item : getItemList())
  {
    if ( item->hasMenu() )
    {
      const auto& menu = item->getMenu();
      int menu_X = getTermX() + int(max_item_width) + 1;
      menu_X = menu->adjustX(menu_X);
      const int menu_Y = item->getTermY() - 2;

      // set sub-menu position
      menu->setPos (FPoint{menu_X, menu_Y});

      // call sub-menu adjustItems()
      if ( menu->getCount() > 0 )
        menu->adjustItems();
    }
  }
}

//----------------------------------------------------------------------
auto FMenu::adjustX (int x_pos) const -> int
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
void FMenu::openSubMenu (FMenu* sub_menu, SelectItem select)
{
  // open sub menu

  if ( ! sub_menu || sub_menu->isShown() )
    return;

  if ( select == SelectItem::Yes )
  {
    sub_menu->selectFirstItem();

    if ( sub_menu->hasSelectedItem() )
      sub_menu->getSelectedItem()->setFocus();
  }

  sub_menu->show();
  opened_sub_menu = sub_menu;
  raiseWindow (sub_menu);
  sub_menu->redraw();
  drawStatusBarMessage();
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
void FMenu::hideSuperMenus() const
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
  else if ( isDialog(super) )
  {
    auto dgl = static_cast<FDialog*>(super);
    dgl->leaveMenu();
  }
}

//----------------------------------------------------------------------
auto FMenu::isMouseOverItem (const FPoint& mouse_pos, const FMenuItem* item) const -> bool
{
  const int x1 = item->getX();
  const int x2 = item->getX() + int(item->getWidth());
  const int y  = item->getY();
  const int mouse_x = mouse_pos.getX() - getRightPadding();
  const int mouse_y = mouse_pos.getY() - getTopPadding();
  return ( mouse_x >= x1 && mouse_x < x2 && mouse_y == y );
}

//----------------------------------------------------------------------
auto FMenu::mouseDownOverList (const FPoint& mouse_pos) -> bool
{
  bool focus_changed{false};

  for (auto&& item : getItemList())
  {
    if ( isMouseOverItem(mouse_pos, item) )
    {
      // Mouse pointer over item
      mouseDownSubmenu (item);
      mouseDownSelection (item, focus_changed);
    }
  }

  return focus_changed;
}

//----------------------------------------------------------------------
void FMenu::mouseDownSubmenu (const FMenuItem* m_item)
{
  if ( ! hasSelectedItem() )
    return;

  const auto& sel_item = getSelectedItem();

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
    drawStatusBarMessage();
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

  drawStatusBarMessage();

  if ( m_item->hasMenu() )
  {
    const auto& sub_menu = m_item->getMenu();

    if ( ! sub_menu->isShown() )
      shown_sub_menu = sub_menu;
  }
}

//----------------------------------------------------------------------
auto FMenu::mouseUpOverList (const FPoint& mouse_pos) -> bool
{
  FMenuItem* sel_item = nullptr;

  for (auto&& item : getItemList())
  {
    if ( item->isSelected() && isMouseOverItem(mouse_pos, item) )
      sel_item = item;  // Mouse pointer over item
  }

  if ( ! sel_item )
    return false;

  if ( sel_item->hasMenu() )
  {
    auto sub_menu = sel_item->getMenu();

    if ( ! sub_menu->isShown() )
      openSubMenu (sub_menu, SelectItem::Yes);
    else if ( opened_sub_menu )
    {
      opened_sub_menu->selectFirstItem();

      if ( opened_sub_menu->hasSelectedItem() )
        opened_sub_menu->getSelectedItem()->setFocus();

      opened_sub_menu->redraw();
      drawStatusBarMessage();
    }

    return true;
  }

  unselectItem();
  hide();
  hideSuperMenus();
  sel_item->processClicked();
  return true;
}

//----------------------------------------------------------------------
inline auto FMenu::initializeMouseStates (FMouseEvent* ev) -> MouseStates
{
  return
  {
    false,                                    // focus_changed
    false,                                    // hide_sub_menu
    isMouseOverMenu (ev->getTermPos()),       // mouse_over_menu
    isMouseOverSubMenu (ev->getTermPos()),    // mouse_over_submenu
    isMouseOverSuperMenu (ev->getTermPos()),  // mouse_over_supermenu
    isMouseOverMenuBar (ev->getTermPos())     // mouse_over_menubar
  };
}

//----------------------------------------------------------------------
inline void FMenu::handleCloseSubMenu (const MouseStates& ms)
{
  if ( shown_sub_menu )
  {
    closeOpenedSubMenu();
    openSubMenu (shown_sub_menu, SelectItem::No);
  }
  else if ( ms.hide_sub_menu )
  {
    closeOpenedSubMenu();
    forceTerminalUpdate();
  }
}

//----------------------------------------------------------------------
void FMenu::mouseMoveOverList (const FPoint& mouse_pos, MouseStates& ms)
{
  for (auto&& item : getItemList())
  {
    if ( isMouseOverItem(mouse_pos, item) )
      mouseMoveSelection (item, ms);
    else
      mouseMoveDeselection (item, ms);
  }
}

//----------------------------------------------------------------------
void FMenu::mouseMoveSelection (FMenuItem* m_item, MouseStates& ms)
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

  drawStatusBarMessage();

  // Sub menu handling
  if ( m_item->hasMenu() )
  {
    const auto& sub_menu = m_item->getMenu();

    if ( ! sub_menu->isShown() )
      shown_sub_menu = sub_menu;
  }
  else if ( opened_sub_menu )
    ms.hide_sub_menu = true;
}

//----------------------------------------------------------------------
void FMenu::mouseMoveDeselection (FMenuItem* m_item, MouseStates& ms)
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
void FMenu::mouseMoveOverBorder (MouseStates& ms) const
{
  // Mouse is moved over border or separator line

  updateStatusbar (this, false);

  if ( opened_sub_menu )
    ms.hide_sub_menu = true;
}

//----------------------------------------------------------------------
inline auto FMenu::handleSubMenuEvent ( const MouseStates& ms
                                      , const FMouseEvent& ev ) const -> bool
{
  if ( ms.mouse_over_submenu )
  {
    passEventToSubMenu(ev);  // Event handover to sub-menu
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
void FMenu::passEventToSubMenu (const FMouseEvent& ev) const
{
  // Mouse event handover to sub-menu

  passEventToWidget (opened_sub_menu, ev);
}

//----------------------------------------------------------------------
inline auto FMenu::handleSuperMenuEvent ( const MouseStates& ms
                                        , const FMouseEvent& ev ) -> bool
{
  if ( ! ms.mouse_over_menu && ms.mouse_over_supermenu )
  {
    passEventToSuperMenu(ev);  // Event handover to super-menu
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
void FMenu::passEventToSuperMenu (const FMouseEvent& ev)
{
  // Mouse event handover to super-menu

  auto smenu = superMenuAt (ev.getTermPos());
  passEventToWidget (smenu, ev);
}

//----------------------------------------------------------------------
inline auto FMenu::handleMenuBarEvent ( const MouseStates& ms
                                      , const FMouseEvent& ev ) const -> bool
{
  if ( ms.mouse_over_menubar )
  {
    passEventToMenuBar(ev);  // Event handover to the menu bar
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
void FMenu::passEventToMenuBar (const FMouseEvent& ev) const
{
  // Mouse event handover to the menu bar

  auto menu_bar = getMenuBar();
  passEventToWidget (menu_bar, ev);
}

//----------------------------------------------------------------------
auto FMenu::containsMenuStructure (int x, int y) -> bool
{
  // Check mouse click position for item, menu and all sub menus

  const auto& si = getSelectedItem();

  if ( getTermGeometry().contains(x, y) )
    return true;

  if ( si && si->hasMenu() && opened_sub_menu )
    return si->getMenu()->containsMenuStructure(x, y);

  if ( menuitem.getTermGeometry().contains(x, y) )
    return true;

  return false;
}

//----------------------------------------------------------------------
auto FMenu::superMenuAt (int x, int y) -> FMenu*
{
  // Check mouse click position for super menu

  if ( getTermGeometry().contains(x, y) )
    return nullptr;

  auto super = getSuperMenu();

  if ( super && isMenu(super) )
  {
    if ( super->getTermGeometry().contains(x, y) )
      return static_cast<FMenu*>(super);

    const auto& smenu = static_cast<FMenu*>(getSuperMenu());

    if ( smenu )
      return smenu->superMenuAt(x, y);
  }

  return nullptr;
}

//----------------------------------------------------------------------
void FMenu::selectItem_PostProcessing (FMenuItem* sel_item)
{
  setTerminalUpdates (FVTerm::TerminalUpdate::Stop);
  unselectItem();
  sel_item->setSelected();
  sel_item->setFocus();
  drawStatusBarMessage();
  setSelectedItem(sel_item);
  redraw();
  setTerminalUpdates (FVTerm::TerminalUpdate::Start);
  forceTerminalUpdate();
}

//----------------------------------------------------------------------
void FMenu::keypressMenuBar (FKeyEvent* ev) const
{
  auto mbar = getMenuBar();

  if ( mbar )
    mbar->onKeyPress(ev);
}

//----------------------------------------------------------------------
inline auto FMenu::hotkeyFound (FKey hotkey, const FKeyEvent& ev) const -> bool
{
  bool found{false};
  const auto& key = ev.key();

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

  return found;
}

//----------------------------------------------------------------------
auto FMenu::hotkeyMenu (FKeyEvent* ev) -> bool
{
  auto try_to_open_submenu = [this] (const auto& sub_menu)
  {
    if ( ! sub_menu->isShown() )
      openSubMenu (sub_menu, SelectItem::Yes);

    sub_menu->redraw();
  };

  for (auto&& item : getItemList())
  {
    if ( item->hasHotkey() && hotkeyFound(item->getHotkey(), *ev) )
    {
      if ( item->hasMenu() )
      {
        unselectItem();
        item->setSelected();
        setSelectedItem (item);
        redraw();
        try_to_open_submenu (item->getMenu());
      }
      else
      {
        unselectItem();
        hideSubMenus();
        hide();
        hideSuperMenus();
        ev->accept();
        item->processClicked();
      }

      ev->accept();
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------
void FMenu::draw()
{
  // Fill the background
  const auto& wc = getColorTheme();
  setColor (wc->menu.fg, wc->menu.bg);

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(true);

  clearArea();
  drawBorder();
  drawItems();
  drawShadow(this);

  if ( FVTerm::getFOutput()->isMonochron() )
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
  const auto& wc = getColorTheme();
  print() << FPoint{1, 2 + y}
          << FColorPair{wc->menu.fg, wc->menu.bg};

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(true);

  if ( FVTerm::getFOutput()->isNewFont() )
  {
    print (UniChar::NF_border_line_vertical_right);
    FString line { std::size_t(getWidth()) - 2
                 , UniChar::BoxDrawingsHorizontal };
    print (line);
    print (UniChar::NF_rev_border_line_vertical_left);
  }
  else
  {
    print (UniChar::BoxDrawingsVerticalAndRight);
    FString line { std::size_t(getWidth()) - 2
                 , UniChar::BoxDrawingsHorizontal};
    print (line);
    print (UniChar::BoxDrawingsVerticalAndLeft);
  }

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
inline void FMenu::drawMenuLine (FMenuItem* m_item, int y)
{
  FString txt{m_item->getText()};
  MenuText txtdata{};
  std::size_t column_width = getColumnWidth(txt);
  const FKey accel_key   = m_item->accel_key;
  const bool is_enabled  = m_item->isEnabled();
  const bool is_selected = m_item->isSelected();

  // Set screen position and attributes
  setLineAttributes (m_item, y);

  // Draw check mark prefix for checkable items
  drawCheckMarkPrefix (m_item);

  // Print leading blank space
  print (' ');

  hotkeypos = finalcut::getHotkeyPos(txt, txtdata.text);

  if ( hotkeypos != NOT_SET )
    column_width--;

  txtdata.no_underline = m_item->getFlags().feature.no_underline;
  setCursorToHotkeyPosition (m_item);

  if ( ! is_enabled || is_selected )
    txtdata.hotkeypos = NOT_SET;
  else
    txtdata.hotkeypos = hotkeypos;

  drawMenuText (txtdata);

  if ( m_item->hasMenu() )
    drawSubMenuIndicator (column_width);
  else if ( accel_key != FKey::None )
    drawAcceleratorKey (column_width, accel_key);

  // Draw the trailing spaces of the selected line
  if ( is_selected )
    drawTrailingSpaces (column_width);

  if ( FVTerm::getFOutput()->isMonochron() && is_enabled && is_selected )
    setReverse(true);
}

//----------------------------------------------------------------------
inline void FMenu::drawCheckMarkPrefix (const FMenuItem* m_item)
{
  const bool is_checked   = m_item->isChecked();
  const bool is_checkable = m_item->checkable;
  const bool is_radio_btn = m_item->radio_button;

  if ( ! has_checkable_items )
    return;

  auto print_bullet = [this] ()
  {
    if ( FVTerm::getFOutput()->isNewFont() )
      print (UniChar::NF_Bullet);      // NF_Bullet ●
    else
      print (UniChar::BlackCircle);    // BlackCircle ●
  };

  auto print_check_mark = [this] ()
  {
    if ( FVTerm::getFOutput()->isNewFont() )
      print (UniChar::NF_check_mark);  // NF_check_mark ✓
    else
      print (UniChar::SquareRoot);     // SquareRoot √
  };

  if ( is_checkable )
  {
    if ( is_checked )
    {
      if ( is_radio_btn )
        print_bullet();
      else
        print_check_mark();
    }
    else
    {
      const auto& wc = getColorTheme();
      setColor (wc->menu.inactive_fg, getBackgroundColor());

      if ( FVTerm::getFOutput()->getEncoding() == Encoding::ASCII )
        print ('-');
      else
        print (UniChar::SmallBullet);  // ·

      setColor();
    }
  }
  else
    print (' ');
}

//----------------------------------------------------------------------
inline void FMenu::drawMenuText (MenuText& data)
{
  // Print menu text

  for (std::size_t z{0}; z < data.text.getLength(); z++)
  {
    if ( ! std::iswprint(std::wint_t(data.text[z]))
      && ! FVTerm::getFOutput()->isNewFont()
      && ( data.text[z] < UniChar::NF_rev_left_arrow2
        || data.text[z] > UniChar::NF_check_mark )
      && ! FVTerm::getFOutput()->isEncodable(wchar_t(data.text[z])) )
    {
      data.text[z] = L' ';
    }

    if ( z == data.hotkeypos )
    {
      const auto& wc = getColorTheme();
      setColor (wc->menu.hotkey_fg, wc->menu.hotkey_bg);

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
  const std::size_t c = has_checkable_items ? 1 : 0;
  const std::size_t len = max_item_width - (startpos + c + 3);

  if ( len > 0 )
  {
    // Print filling blank spaces
    print (FString{len, L' '});
    // Print BlackRightPointingPointer ►
    print (UniChar::BlackRightPointingPointer);
    startpos = max_item_width - (c + 2);
  }
}

//----------------------------------------------------------------------
inline void FMenu::drawAcceleratorKey (std::size_t& startpos, FKey accel_key)
{
  const FString accel_name {FVTerm::getFOutput()->getKeyName(accel_key)};
  const std::size_t c = has_checkable_items ? 1 : 0;
  const std::size_t accel_len = accel_name.getLength();
  const std::size_t plain_text_length = startpos + accel_len + c + 2;

  if ( plain_text_length >= max_item_width )
    return;

  // Print filling blank spaces + accelerator key name
  const std::size_t len = max_item_width - plain_text_length;
  const FString spaces {len, L' '};
  print (spaces + accel_name);
  startpos = max_item_width - (c + 2);
}

//----------------------------------------------------------------------
inline void FMenu::drawTrailingSpaces (std::size_t startpos)
{
  const std::size_t c = has_checkable_items ? 1 : 0;

  // Print trailing blank space
  for (std::size_t i = startpos + c; i < max_item_width - 1; i++)
    print (' ');
}

//----------------------------------------------------------------------
inline void FMenu::setLineAttributes (const FMenuItem* m_item, int y)
{
  const bool is_enabled  = m_item->isEnabled();
  const bool is_selected = m_item->isSelected();
  const auto& wc = getColorTheme();

  if ( is_enabled )
  {
    if ( is_selected )
    {
      setForegroundColor (wc->menu.focus_fg);
      setBackgroundColor (wc->menu.focus_bg);

      if ( FVTerm::getFOutput()->isMonochron() )
        setReverse(false);
    }
    else
    {
      setForegroundColor (wc->menu.fg);
      setBackgroundColor (wc->menu.bg);

      if ( FVTerm::getFOutput()->isMonochron() )
        setReverse(true);
    }
  }
  else
  {
    setForegroundColor (wc->menu.inactive_fg);
    setBackgroundColor (wc->menu.inactive_bg);

    if ( FVTerm::getFOutput()->isMonochron() )
      setReverse(true);
  }

  print() << FPoint{2, 2 + y};
  setColor();
}

//----------------------------------------------------------------------
inline void FMenu::setCursorToHotkeyPosition (FMenuItem* m_item) const
{
  const bool is_checkable = m_item->checkable;
  const bool is_selected  = m_item->isSelected();

  if ( hotkeypos == NOT_SET )
  {
    // set cursor to the first character
    if ( is_selected )
    {
      if ( is_checkable )
        m_item->setCursorPos({3, 1});
      else
        m_item->setCursorPos({2, 1});
    }
  }
  else
  {
    if ( is_selected )
    {
      // set cursor to the hotkey position
      const auto x = getColumnWidth (m_item->getText(), hotkeypos);

      if ( is_checkable )
        m_item->setCursorPos({3 + int(x), 1});
      else
        m_item->setCursorPos({2 + int(x), 1});
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
    drawStatusBarMessage();
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
      openSubMenu (sub_menu, SelectItem::Yes);
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
    openSubMenu (sel_item->getMenu(), SelectItem::Yes);
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
    const auto& super = getSuperMenu();
    hideSuperMenus();

    if ( getStatusBar() )
      getStatusBar()->clearMessage();

    if ( ! (super && isDialog(super)) )
      switchToPrevWindow(this);
  }

  drawStatusBarMessage();
}

//----------------------------------------------------------------------
void FMenu::processActivate() const
{
  emitCallback("activate");
}


// non-member functions
//----------------------------------------------------------------------
auto closeOpenMenus ( FMenu* menu
                    , const FPoint& mouse_position ) -> std::tuple<bool, bool>
{
  // Close the open menu

  bool click_in_menu_structure{false};
  bool is_dialog_menu{false};

  if ( ! menu )
  {
    auto tuple = std::make_tuple(click_in_menu_structure, is_dialog_menu);
    return tuple;
  }

  if ( menu->containsMenuStructure(mouse_position) )
  {
    click_in_menu_structure = true;
    auto tuple = std::make_tuple(click_in_menu_structure, is_dialog_menu);
    return tuple;
  }

  if ( menu->isDialogMenu() )
    is_dialog_menu = true;

  menu->unselectItem();
  menu->hide();
  menu->hideSubMenus();
  menu->hideSuperMenus();
  return std::make_tuple (click_in_menu_structure, is_dialog_menu);
}

}  // namespace finalcut
