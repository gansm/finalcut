/***********************************************************************
* fmenubar.cpp - Widget FMenuBar                                       *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2024 Markus Gans                                      *
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
#include <vector>

#include "final/fapplication.h"
#include "final/fevent.h"
#include "final/fwidgetcolors.h"
#include "final/menu/fmenubar.h"
#include "final/menu/fmenu.h"
#include "final/menu/fmenuitem.h"
#include "final/util/flog.h"
#include "final/widget/fstatusbar.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FMenuBar
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FMenuBar::FMenuBar(FWidget* parent)
  : FWindow{parent}
{
  init();
}

//----------------------------------------------------------------------
FMenuBar::~FMenuBar()  // destructor
{
  FWidget::setMenuBar(nullptr);
}


// public methods of FMenuBar
//----------------------------------------------------------------------
void FMenuBar::resetColors()
{
  const auto& wc_menu = getColorTheme()->menu;
  FWidget::setForegroundColor (wc_menu.fg);
  FWidget::setBackgroundColor (wc_menu.bg);
  FWidget::resetColors();
}

//----------------------------------------------------------------------
void FMenuBar::resetMenu()
{
  unselectItem();
  drop_down = false;
}

//----------------------------------------------------------------------
void FMenuBar::hide()
{
  const auto& wc_term = getColorTheme()->term;
  const auto& fg = wc_term.fg;
  const auto& bg = wc_term.bg;
  setColor (fg, bg);
  print() << FPoint{1, 1} << FString{getDesktopWidth(), L' '};
  FWindow::hide();
}

//----------------------------------------------------------------------
void FMenuBar::adjustSize()
{
  setGeometry (FPoint{1, 1}, FSize{getDesktopWidth(), 1}, false);
  adjustItems();
}

//----------------------------------------------------------------------
void FMenuBar::onKeyPress (FKeyEvent* ev)
{
  const auto& key = ev->key();

  if ( isEnterKey(key)
    || key == FKey::Up
    || key == FKey::Down )
  {
    if ( hasSelectedItem() )
    {
      auto sel_item = getSelectedItem();

      if ( sel_item->hasMenu() )
      {
        openMenu (sel_item);
      }
      else if ( isEnterKey(key) )
      {
        unselectItem();
        redraw();
        sel_item->processClicked();
      }
    }

    ev->accept();
  }
  else if ( isFocusNextKey(key) )
  {
    selectNextItem();
    ev->accept();
  }
  else if ( isFocusPrevKey(key) )
  {
    selectPrevItem();
    ev->accept();
  }
  else if ( isEscapeKey(key) )
  {
    leaveMenuBar();
    ev->accept();
  }
}

//----------------------------------------------------------------------
void FMenuBar::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != MouseButton::Left )
  {
    mouse_down = false;

    if ( ! getItemList().empty() && hasSelectedItem() )
      leaveMenuBar();
    else
      return;

    if ( getStatusBar() )
      getStatusBar()->clearMessage();

    return;
  }

  if ( mouse_down )
    return;

  mouse_down = true;

  if ( ! isWindowActive() )
    setActiveWindow(this);

  // Handle menu entries
  mouseDownOverList(ev);
}

//----------------------------------------------------------------------
void FMenuBar::onMouseUp (FMouseEvent* ev)
{
  if ( ev->getButton() != MouseButton::Left )
    return;

  if ( mouse_down )
  {
    mouse_down = false;

    // Handle menu entries
    mouseUpOverList(ev);
  }
}

//----------------------------------------------------------------------
void FMenuBar::onMouseMove (FMouseEvent* ev)
{
  if ( ev->getButton() != MouseButton::Left )
    return;

  if ( ! isWindowActive() )
    setActiveWindow(this);

  // Handle menu entries
  if ( mouse_down )
    mouseMoveOverList(*ev);
}

//----------------------------------------------------------------------
void FMenuBar::onAccel (FAccelEvent* ev)
{
  unselectItem();
  selectFirstItem();
  getSelectedItem()->setFocus();
  drawStatusBarMessage();
  redraw();
  forceTerminalUpdate();
  ev->accept();
}

//----------------------------------------------------------------------
void FMenuBar::cb_itemDeactivated (const FMenuItem* menuitem) const
{
  if ( ! menuitem->hasMenu() )
    return;

  auto menu = menuitem->getMenu();
  menu->hide();
  menu->hideSubMenus();
}


// private methods of FMenuBar
//----------------------------------------------------------------------
void FMenuBar::init()
{
  auto r = getRootWidget();
  auto w = r->getWidth();
  // initialize geometry values
  FWindow::setGeometry (FPoint{1, 1}, FSize{w, 1}, false);
  setAlwaysOnTop();
  FWidget::setMenuBar(this);
  ignorePadding();

  if ( getRootWidget() )
    getRootWidget()->setTopPadding(1, true);

  FMenuBar::addAccelerator (FKey::F10, this);
  FMenuBar::addAccelerator (FKey::Ctrl_space, this);
  FMenuBar::addAccelerator (FKey::Menu, this);
  FMenuBar::resetColors();
  unsetFocusable();
}

//----------------------------------------------------------------------
void FMenuBar::calculateDimensions() const
{
  FPoint item_pos{1, 1};

  // find the maximum item width
  for (auto&& item : getItemList())
  {
    auto len = item->getTextWidth();
    auto item_width = len + 2;

    // set item geometry
    item->setGeometry (item_pos, FSize{item_width, 1}, false);

    // set menu position
    if ( item->hasMenu() )
      item->getMenu()->setPos (item_pos, false);

    item_pos.x_ref() += int(item_width);
  }
}

//----------------------------------------------------------------------
void FMenuBar::selectItem_PostProcessing (FMenuItem* sel_item)
{
  setTerminalUpdates (FVTerm::TerminalUpdate::Stop);
  unselectItem();
  sel_item->setSelected();
  sel_item->setFocus();

  if ( drop_down && sel_item->hasMenu() )
    openMenu(sel_item);

  drawStatusBarMessage();
  setSelectedItem(sel_item);
  redraw();
  setTerminalUpdates (FVTerm::TerminalUpdate::Start);
  forceTerminalUpdate();
}

//----------------------------------------------------------------------
auto FMenuBar::hotkeyMenu (FKeyEvent*& ev) -> bool
{
  for (auto&& item : getItemList())
  {
    if ( ! item->isEnabled() )
      continue;

    auto hotkey = item->getHotkey();

    if ( hotkey > 0xff00 && hotkey < 0xff5f )  // full-width character
      hotkey -= 0xfee0;

    if ( FKey::Meta_offset + FKey(std::tolower(int(hotkey))) == ev->key() )
    {
      const auto& sel_item = getSelectedItem();

      if ( sel_item && sel_item->hasMenu() )
        sel_item->getMenu()->unselectItem();

      unselectItem();

      if ( item->hasMenu() )
      {
        setTerminalUpdates (FVTerm::TerminalUpdate::Stop);
        item->setSelected();
        setSelectedItem(item);
        item->setFocus();
        openMenu(item);
        setTerminalUpdates (FVTerm::TerminalUpdate::Start);
      }
      else
      {
        setSelectedItem(nullptr);
        redraw();
        drop_down = false;
        item->processClicked();
      }

      ev->accept();
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------
void FMenuBar::draw()
{
  drawItems();
}

//----------------------------------------------------------------------
void FMenuBar::drawItems()
{
  const auto& list = getItemList();

  if ( list.empty() )
    return;

  print() << FPoint{1, 1};

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(true);

  std::size_t x{1};

  for (auto&& item : list)
    drawItem (item, x);

  // Print spaces to end of line
  for (; x <= getDesktopWidth(); x++)
    print (' ');

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
inline void FMenuBar::drawItem (FMenuItem* menuitem, std::size_t& x)
{
  menuText txtdata =
  {
    {},
    x + 1,
    NOT_SET,
    bool(menuitem->getFlags().feature.no_underline)
  };

  FString txt{menuitem->getText()};
  std::size_t column_width = getColumnWidth(txt);
  bool is_enabled  = menuitem->isEnabled();
  bool is_selected = menuitem->isSelected();

  // Set screen attributes
  setLineAttributes (menuitem);
  drawLeadingSpace (x);

  if ( x - 1 > screenWidth )
    txt = txt.left(getColumnWidth(txt) - screenWidth - x - 1);

  std::size_t hotkeypos = finalcut::getHotkeyPos(txt, txtdata.text);

  if ( hotkeypos != NOT_SET )
    column_width--;

  x += column_width;

  if ( is_enabled && ! is_selected )
    txtdata.hotkeypos = hotkeypos;

  setCursorToHotkeyPosition (menuitem, hotkeypos);
  drawMenuText (txtdata);
  drawEllipsis (txtdata, x);
  drawTrailingSpace (x);

  const auto& wc_menu = getColorTheme()->menu;
  setColor (wc_menu.fg, wc_menu.bg);

  if ( FVTerm::getFOutput()->isMonochron() && is_enabled && is_selected )
    setReverse(true);
}

//----------------------------------------------------------------------
inline void FMenuBar::setLineAttributes (const FMenuItem* menuitem)
{
  bool is_enabled  = menuitem->isEnabled();
  bool is_selected = menuitem->isSelected();
  const auto& wc_menu = getColorTheme()->menu;

  if ( is_enabled )
  {
    if ( is_selected )
    {
      if ( FVTerm::getFOutput()->isMonochron() )
        setReverse(false);

      setForegroundColor (wc_menu.focus_fg);
      setBackgroundColor (wc_menu.focus_bg);
    }
    else
    {
      setForegroundColor (wc_menu.fg);
      setBackgroundColor (wc_menu.bg);
    }
  }
  else
  {
    setForegroundColor (wc_menu.inactive_fg);
    setBackgroundColor (wc_menu.inactive_bg);
  }

  setColor();
}

//----------------------------------------------------------------------
inline void FMenuBar::setCursorToHotkeyPosition ( FMenuItem* menuitem
                                                , std::size_t hotkeypos ) const
{
  if ( ! menuitem->isSelected() )
    return;

  if ( hotkeypos == NOT_SET )
  {
    // set cursor to the first character
    menuitem->setCursorPos({2, 1});
    return;
  }

  // set cursor to the hotkey position
  std::size_t x = getColumnWidth (menuitem->getText(), hotkeypos);
  menuitem->setCursorPos({2 + int(x), 1});
}

//----------------------------------------------------------------------
inline void FMenuBar::drawMenuText (menuText& data)
{
  // Print menu text

  for (std::size_t z{0}; z < data.text.getLength(); z++)
  {
    if ( data.startpos > screenWidth - z )
      break;

    if ( ! isPrintable(data.text[z])
      && ! FVTerm::getFOutput()->isNewFont()
      && ( data.text[z] < UniChar::NF_rev_left_arrow2
        || data.text[z] > UniChar::NF_check_mark ) )
    {
      data.text[z] = L' ';
    }

    if ( z == data.hotkeypos )
    {
      const auto& wc_menu = getColorTheme()->menu;
      setColor (wc_menu.hotkey_fg, wc_menu.hotkey_bg);

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
inline void FMenuBar::drawEllipsis (const menuText& txtdata, std::size_t x)
{
  if ( x > screenWidth + 1 )
  {
    if ( txtdata.startpos < screenWidth )
    {
      // Print ellipsis
      print() << FPoint{int(screenWidth) - 1, 1} << "..";
    }
    else if ( txtdata.startpos - 1 <= screenWidth )
    {
      // Hide first character from text
      print() << FPoint{int(screenWidth), 1} << ' ';
    }
  }
}

//----------------------------------------------------------------------
inline void FMenuBar::drawLeadingSpace (std::size_t& x)
{
  // Print a leading blank space

  if ( x < screenWidth )
  {
    x++;
    print (' ');
  }
}

//----------------------------------------------------------------------
inline void FMenuBar::drawTrailingSpace (std::size_t& x)
{
  // Print a trailing blank space

  if ( x < screenWidth )
  {
    x++;
    print (' ');
  }
}

//----------------------------------------------------------------------
void FMenuBar::adjustItems() const
{
  int item_X = 1;
  int item_Y = 1;

  for (auto&& item : getItemList())
  {
    // get item width
    auto item_width = item->getWidth();

    if ( item->hasMenu() )
    {
      auto menu = item->getMenu();

      // set menu position
      menu->setPos (FPoint{menu->adjustX(item_X), item_Y});

      // call menu adjustItems()
      menu->adjustItems();
    }

    item_X += int(item_width);
  }
}

//----------------------------------------------------------------------
void FMenuBar::selectMenuItem (FMenuItem* item)
{
  auto focused_widget = getFocusWidget();

  if ( ! item->isEnabled() && focused_widget )
    focused_widget->unsetFocus();

  if ( ! item->isEnabled() || item->isSelected() )
    return;

  unselectItem();
  item->setSelected();
  item->setFocus();

  if ( focused_widget && ! focused_widget->isWindowWidget() )
    focused_widget->redraw();

  item->openMenu();
  setSelectedItem(item);
  focus_changed = true;

  if ( ! item->hasMenu() )
    return;

  auto menu = item->getMenu();

  if ( ! menu->hasSelectedItem() )
    return;

  menu->unselectItem();
  menu->redraw();
  drop_down = true;
}

//----------------------------------------------------------------------
void FMenuBar::openMenu (const FMenuItem* sel_item)
{
  auto menu = sel_item->getMenu();
  sel_item->openMenu();
  menu->selectFirstItem();
  auto first_item = menu->getSelectedItem();

  if ( first_item )
    first_item->setFocus();

  menu->redraw();
  drawStatusBarMessage();
  redraw();
  drop_down = true;
}

//----------------------------------------------------------------------
auto FMenuBar::activateMenu (const FMenuItem* item) -> bool
{
  if ( ! item->hasMenu() )
    return false;

  auto menu = item->getMenu();

  if ( ! menu->hasSelectedItem() )
  {
    menu->selectFirstItem();
    auto first_item = menu->getSelectedItem();

    if ( first_item )
      first_item->setFocus();

    drawStatusBarMessage();
    redraw();
    menu->redraw();
    drop_down = true;
  }

  return true;
}

//----------------------------------------------------------------------
auto FMenuBar::clickItem (FMenuItem* item) -> bool
{
  if ( item->hasMenu() )
    return false;

  item->unsetSelected();

  if ( getSelectedItem() == item )
  {
    setSelectedItem(nullptr);
    leaveMenuBar();
    drop_down = false;
    item->processClicked();
  }

  return true;
}

//----------------------------------------------------------------------
void FMenuBar::unselectMenuItem (FMenuItem* item)
{
  if ( ! item->isEnabled() || ! item->isSelected() )
    return;

  item->unsetSelected();
  focus_changed = true;
  drop_down = false;

  if ( getSelectedItem() == item )
    setSelectedItem(nullptr);
}

//----------------------------------------------------------------------
inline auto FMenuBar::isClickOnMenuEntry ( const FMouseEvent* ev
                                         , const FMenuItem* item ) const -> bool
{
  const int mouse_x = ev->getX();
  const int mouse_y = ev->getY();
  const int x1 = item->getX();
  const int x2 = item->getX() + int(item->getWidth());
  return mouse_x >= x1
      && mouse_x < x2
      && mouse_y == 1;
}

//----------------------------------------------------------------------
void FMenuBar::mouseDownOverList (const FMouseEvent* ev)
{
  const auto& list = getItemList();

  if ( list.empty() )
    return;

  focus_changed = false;

  for (auto&& item : list)
  {
    if ( ev->getY() == 1 )
    {
      if ( isClickOnMenuEntry(ev, item) )
        selectMenuItem (item);  // Mouse pointer over item
      else
        unselectMenuItem (item);
    }
  }

  if ( getStatusBar() )
  {
    if ( ! hasSelectedItem() )
      getStatusBar()->clearMessage();

    getStatusBar()->drawMessage();
  }

  if ( focus_changed )
    redraw();
}

//----------------------------------------------------------------------
void FMenuBar::mouseUpOverList (const FMouseEvent* ev)
{
  const auto& list = getItemList();

  if ( list.empty() )
    return;

  for (auto&& item : list)
  {
    if ( isClickOnMenuEntry(ev, item)
      && item->isEnabled()
      && item->isSelected() )
    {
      // Mouse pointer over item
      if ( ! activateMenu(item) && clickItem(item) )
        return;
    }
    else
    {
      unselectMenuItem(item);
      redraw();
    }
  }

  if ( ! hasSelectedItem() )
    leaveMenuBar();
}

//----------------------------------------------------------------------
void FMenuBar::mouseMoveOverList (const FMouseEvent& ev)
{
  const auto& list = getItemList();

  if ( list.empty() )
    return;

  focus_changed = false;
  bool mouse_over_menubar{false};

  if ( getTermGeometry().contains(ev.getTermPos()) )
    mouse_over_menubar = true;

  for (auto&& item : list)
  {
    if ( isClickOnMenuEntry(&ev, item) )
    {
      // Mouse pointer over item
      selectMenuItem(item);
    }
    else
    {
      if ( mouse_over_menubar )
      {
        // Unselect selected item without mouse focus
        unselectMenuItem(item);
      }
      else
      {
        // Event handover to the menu
        passEventToMenu(ev);
      }
    }
  }

  if ( getStatusBar() )
  {
    if ( ! hasSelectedItem() )
      getStatusBar()->clearMessage();

    getStatusBar()->drawMessage();
  }

  if ( focus_changed )
  {
    redraw();
    forceTerminalUpdate();
  }
}

//----------------------------------------------------------------------
void FMenuBar::passEventToMenu (const FMouseEvent& ev) const
{
  if ( ! hasSelectedItem() || ! getSelectedItem()->hasMenu() )
    return;

  // Mouse event handover to the menu
  auto menu = getSelectedItem()->getMenu();
  const auto& menu_geometry = menu->getTermGeometry();

  if ( menu->getCount() > 0
    && menu_geometry.contains(ev.getTermPos()) )
  {
    const auto& t = ev.getTermPos();
    const auto& p = menu->termToWidgetPos(t);
    const MouseButton b = ev.getButton();
    const auto& _ev = \
        std::make_shared<FMouseEvent>(Event::MouseMove, p, t, b);
    menu->mouse_down = true;
    setClickedWidget(menu);
    menu->onMouseMove(_ev.get());
  }
}

//----------------------------------------------------------------------
void FMenuBar::leaveMenuBar()
{
  resetMenu();
  redraw();

  if ( getStatusBar() )
    getStatusBar()->clearMessage();

  switchToPrevWindow(this);
  drawStatusBarMessage();
  mouse_down = false;
}

}  // namespace finalcut
