/***********************************************************************
* fmenubar.cpp - Widget FMenuBar                                       *
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
#include "final/fevent.h"
#include "final/fmenu.h"
#include "final/fmenubar.h"
#include "final/fmenuitem.h"
#include "final/fstatusbar.h"
#include "final/fwidgetcolors.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FMenuBar
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FMenuBar::FMenuBar(FWidget* parent)
  : FWindow(parent)
{
  init();
}

//----------------------------------------------------------------------
FMenuBar::~FMenuBar()  // destructor
{
  setMenuBar(nullptr);
}


// public methods of FMenuBar
//----------------------------------------------------------------------
void FMenuBar::resetMenu()
{
  unselectItem();
  drop_down = false;
}

//----------------------------------------------------------------------
void FMenuBar::hide()
{
  const auto& wc = getFWidgetColors();
  FColor fg = wc.term_fg;
  FColor bg = wc.term_bg;
  setColor (fg, bg);
  print() << FPoint(1, 1) << FString(getDesktopWidth(), L' ');
  updateTerminal();
  FWindow::hide();
}

//----------------------------------------------------------------------
void FMenuBar::adjustSize()
{
  setGeometry (FPoint(1, 1), FSize(getDesktopWidth(), 1), false);
  adjustItems();
}

//----------------------------------------------------------------------
void FMenuBar::onKeyPress (FKeyEvent* ev)
{
  switch ( ev->key() )
  {
    case fc::Fkey_return:
    case fc::Fkey_enter:
    case fc::Fkey_up:
    case fc::Fkey_down:
      if ( hasSelectedItem() )
      {
        auto sel_item = getSelectedItem();

        if ( sel_item->hasMenu() )
        {
          auto menu = sel_item->getMenu();
          sel_item->openMenu();
          menu->selectFirstItem();
          auto first_item = menu->getSelectedItem();

          if ( first_item )
            first_item->setFocus();

          menu->redraw();

          if ( getStatusBar() )
            getStatusBar()->drawMessage();

          redraw();
          drop_down = true;
        }
        else if ( ev->key() == fc::Fkey_return
               || ev->key() == fc::Fkey_enter )
        {
          unselectItem();
          redraw();
          sel_item->processClicked();
        }
      }

      ev->accept();
      break;

    case fc::Fkey_left:
      selectPrevItem();
      ev->accept();
      break;

    case fc::Fkey_right:
      selectNextItem();
      ev->accept();
      break;

    case fc::Fkey_escape:
    case fc::Fkey_escape_mintty:
      leaveMenuBar();
      ev->accept();
      break;

    default:
      break;
  }
}

//----------------------------------------------------------------------
void FMenuBar::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton )
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
  if ( ev->getButton() != fc::LeftButton )
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
  if ( ev->getButton() != fc::LeftButton )
    return;

  if ( ! isWindowActive() )
    setActiveWindow(this);

  // Handle menu entries
  if ( mouse_down )
    mouseMoveOverList(ev);
}

//----------------------------------------------------------------------
void FMenuBar::onAccel (FAccelEvent* ev)
{
  unselectItem();
  selectFirstItem();
  getSelectedItem()->setFocus();

  if ( getStatusBar() )
    getStatusBar()->drawMessage();

  redraw();
  ev->accept();
}

//----------------------------------------------------------------------
void FMenuBar::cb_item_deactivated (FWidget* widget, FDataPtr)
{
  auto menuitem = static_cast<FMenuItem*>(widget);

  if ( menuitem->hasMenu() )
  {
    auto menu = menuitem->getMenu();
    menu->hide();
    menu->hideSubMenus();
  }
}


// private methods of FMenuBar
//----------------------------------------------------------------------
void FMenuBar::init()
{
  auto r = getRootWidget();
  auto w = r->getWidth();
  // initialize geometry values
  setGeometry (FPoint(1, 1), FSize(w, 1), false);
  setAlwaysOnTop();
  setMenuBar(this);
  ignorePadding();

  if ( getRootWidget() )
    getRootWidget()->setTopPadding(1, true);

  addAccelerator (fc::Fkey_f10);
  addAccelerator (fc::Fckey_space);
  const auto& wc = getFWidgetColors();
  setForegroundColor (wc.menu_active_fg);
  setBackgroundColor (wc.menu_active_bg);
  unsetFocusable();
}

//----------------------------------------------------------------------
void FMenuBar::calculateDimensions()
{
  FPoint item_pos (1, 1);

  // find the maximum item width
  for (auto&& item : getItemList())
  {
    int len = int(item->getTextWidth());
    int item_width = len + 2;

    // set item geometry
    item->setGeometry (item_pos, FSize(std::size_t(item_width), 1), false);

    // set menu position
    if ( item->hasMenu() )
      item->getMenu()->setPos (item_pos, false);

    item_pos.x_ref() += item_width;
  }
}

//----------------------------------------------------------------------
bool FMenuBar::selectNextItem()
{
  auto list = getItemList();
  auto iter = list.begin();

  while ( iter != list.end() )
  {
    if ( (*iter)->isSelected() )
    {
      FMenuItem* next;
      auto next_element = iter;

      do
      {
        ++next_element;

        if ( next_element == list.end() )
          next_element = list.begin();

        next = static_cast<FMenuItem*>(*next_element);
      } while ( ! next->isEnabled()
             || ! next->acceptFocus()
             || ! next->isShown()
             || next->isSeparator() );

      if ( next == *iter )
        return false;

      updateTerminal (FVTerm::stop_refresh);
      unselectItem();
      next->setSelected();
      setSelectedItem(next);
      next->setFocus();

      if ( drop_down && next->hasMenu() )
      {
        auto menu = next->getMenu();
        next->openMenu();
        menu->selectFirstItem();
        auto first_item = menu->getSelectedItem();

        if ( first_item )
          first_item->setFocus();

        menu->redraw();
      }

      if ( getStatusBar() )
        getStatusBar()->drawMessage();

      redraw();
      updateTerminal (FVTerm::start_refresh);
      break;
    }

    ++iter;
  }

  return true;
}

//----------------------------------------------------------------------
bool FMenuBar::selectPrevItem()
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

      updateTerminal (FVTerm::stop_refresh);
      unselectItem();
      prev->setSelected();
      prev->setFocus();

      if ( drop_down && prev->hasMenu() )
      {
        auto menu = prev->getMenu();
        prev->openMenu();
        menu->selectFirstItem();
        auto first_item = menu->getSelectedItem();

        if ( first_item )
          first_item->setFocus();

        menu->redraw();
      }

      if ( getStatusBar() )
        getStatusBar()->drawMessage();

      setSelectedItem(prev);
      redraw();
      updateTerminal (FVTerm::stop_refresh);
      break;
    }
  }
  while ( iter != list.begin() );

  return true;
}

//----------------------------------------------------------------------
bool FMenuBar::hotkeyMenu (FKeyEvent*& ev)
{
  for (auto&& item : getItemList())
  {
    if ( item->isEnabled() )
    {
      FKey hotkey = item->getHotkey();
      FKey key = ev->key();

      if ( hotkey > 0xff00 && hotkey < 0xff5f )  // full-width character
        hotkey -= 0xfee0;

      if ( fc::Fmkey_meta + FKey(std::tolower(int(hotkey))) == key )
      {
        auto sel_item = getSelectedItem();

        if ( sel_item && sel_item->hasMenu() )
          sel_item->getMenu()->unselectItem();

        unselectItem();

        if ( item->hasMenu() )
        {
          auto menu = item->getMenu();
          item->setSelected();
          setSelectedItem(item);
          item->setFocus();
          item->openMenu();
          menu->selectFirstItem();
          auto first_item = menu->getSelectedItem();

          if ( first_item )
            first_item->setFocus();

          menu->redraw();

          if ( getStatusBar() )
            getStatusBar()->drawMessage();

          redraw();
          drop_down = true;
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
  auto list = getItemList();

  if ( list.empty() )
    return;

  print() << FPoint(1, 1);

  if ( isMonochron() )
    setReverse(true);

  std::size_t x{1};

  for (auto&& item : list)
    drawItem (item, x);

  // Print spaces to end of line
  for (; x <= getDesktopWidth(); x++)
    print (' ');

  if ( isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
inline void FMenuBar::drawItem (FMenuItem* menuitem, std::size_t& x)
{
  menuText txtdata{};
  txtdata.startpos = x + 1;
  txtdata.no_underline = menuitem->getFlags().no_underline;
  FString txt(menuitem->getText());
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

  if ( ! is_enabled || is_selected )
    txtdata.hotkeypos = NOT_SET;
  else
    txtdata.hotkeypos = hotkeypos;

  setCursorToHotkeyPosition (menuitem, hotkeypos);
  drawMenuText (txtdata);
  drawEllipsis (txtdata, x);
  drawTrailingSpace (x);

  const auto& wc = getFWidgetColors();
  setColor (wc.menu_active_fg, wc.menu_active_bg);

  if ( isMonochron() && is_enabled && is_selected )
    setReverse(true);
}

//----------------------------------------------------------------------
inline void FMenuBar::setLineAttributes (FMenuItem* menuitem)
{
  bool is_enabled  = menuitem->isEnabled();
  bool is_selected = menuitem->isSelected();
  const auto& wc = getFWidgetColors();

  if ( is_enabled )
  {
    if ( is_selected )
    {
      if ( isMonochron() )
        setReverse(false);

      setForegroundColor (wc.menu_active_focus_fg);
      setBackgroundColor (wc.menu_active_focus_bg);
    }
    else
    {
      setForegroundColor (wc.menu_active_fg);
      setBackgroundColor (wc.menu_active_bg);
    }
  }
  else
  {
    setForegroundColor (wc.menu_inactive_fg);
    setBackgroundColor (wc.menu_inactive_bg);
  }

  setColor();
}

//----------------------------------------------------------------------
inline void FMenuBar::setCursorToHotkeyPosition ( FMenuItem* menuitem
                                                , std::size_t hotkeypos )
{
  if ( ! menuitem->isSelected() )
    return;

  if ( hotkeypos == NOT_SET )
  {
    // set cursor to the first character
    menuitem->setCursorPos (FPoint(2, 1));
    return;
  }

  // set cursor to the hotkey position
  std::size_t x = getColumnWidth (menuitem->getText(), hotkeypos);
  menuitem->setCursorPos (FPoint(2 + int(x), 1));
}

//----------------------------------------------------------------------
inline void FMenuBar::drawMenuText (menuText& data)
{
  // Print menu text

  for (std::size_t z{0}; z < data.text.getLength(); z++)
  {
    if ( data.startpos > screenWidth - z )
      break;

    if ( ! std::iswprint(std::wint_t(data.text[z])) )
    {
      if ( ! isNewFont()
        && ( data.text[z] < fc::NF_rev_left_arrow2
          || data.text[z] > fc::NF_check_mark ) )
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
inline void FMenuBar::drawEllipsis (const menuText& txtdata, std::size_t x)
{
  if ( x > screenWidth + 1 )
  {
    if ( txtdata.startpos < screenWidth )
    {
      // Print ellipsis
      print() << FPoint(int(screenWidth) - 1, 1) << "..";
    }
    else if ( txtdata.startpos - 1 <= screenWidth )
    {
      // Hide first character from text
      print() << FPoint(int(screenWidth), 1) << ' ';
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
void FMenuBar::adjustItems()
{
  int item_X = 1;
  int item_Y = 1;

  for (auto&& item : getItemList())
  {
    // get item width
    int item_width = int(item->getWidth());

    if ( item->hasMenu() )
    {
      auto menu = item->getMenu();

      // set menu position
      menu->setPos (FPoint(menu->adjustX(item_X), item_Y));

      // call menu adjustItems()
      menu->adjustItems();
    }

    item_X += item_width;
  }
}

//----------------------------------------------------------------------
void FMenuBar::selectMenuItem (FMenuItem* item)
{
  if ( ! item->isEnabled() || item->isSelected() )
    return;

  auto focused_widget = getFocusWidget();
  unselectItem();
  item->setSelected();
  item->setFocus();

  if ( focused_widget && ! focused_widget->isWindowWidget() )
    focused_widget->redraw();

  item->openMenu();
  setSelectedItem(item);
  focus_changed = true;

  if ( item->hasMenu() )
  {
    auto menu = item->getMenu();

    if ( menu->hasSelectedItem() )
    {
      menu->unselectItem();
      menu->redraw();
      drop_down = true;
    }
  }
}

//----------------------------------------------------------------------
bool FMenuBar::activateMenu (FMenuItem* item)
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

    if ( getStatusBar() )
      getStatusBar()->drawMessage();

    redraw();
    menu->redraw();
    drop_down = true;
  }

  return true;
}

//----------------------------------------------------------------------
bool FMenuBar::clickItem (FMenuItem* item)
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
void FMenuBar::mouseDownOverList (const FMouseEvent* ev)
{
  auto list = getItemList();

  if ( list.empty() )
    return;

  focus_changed = false;
  int mouse_x = ev->getX();
  int mouse_y = ev->getY();

  for (auto&& item : list)
  {
    int x1 = item->getX();
    int x2 = item->getX() + int(item->getWidth());

    if ( mouse_y == 1 )
    {
      if ( mouse_x >= x1 && mouse_x < x2 )
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
  {
    redraw();
    updateTerminal();
  }
}

//----------------------------------------------------------------------
void FMenuBar::mouseUpOverList (const FMouseEvent* ev)
{
  auto list = getItemList();

  if ( list.empty() )
    return;

  int mouse_x = ev->getX();
  int mouse_y = ev->getY();

  for (auto&& item : list)
  {
    int x1 = item->getX();
    int x2 = item->getX() + int(item->getWidth());

    if ( mouse_y == 1
      && mouse_x >= x1
      && mouse_x < x2
      && item->isEnabled()
      && item->isSelected() )
    {
      // Mouse pointer over item
      if ( ! activateMenu(item) )
      {
        if ( clickItem(item) )
          return;
      }
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
void FMenuBar::mouseMoveOverList (const FMouseEvent* ev)
{
  auto list = getItemList();

  if ( list.empty() )
    return;

  focus_changed = false;
  bool mouse_over_menubar{false};
  int mouse_x = ev->getX();
  int mouse_y = ev->getY();

  if ( getTermGeometry().contains(ev->getTermPos()) )
    mouse_over_menubar = true;

  for (auto&& item : list)
  {
    int x1 = item->getX();
    int x2 = item->getX() + int(item->getWidth());

    if ( mouse_x >= x1
      && mouse_x < x2
      && mouse_y == 1 )
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
    updateTerminal();
  }
}

//----------------------------------------------------------------------
void FMenuBar::passEventToMenu (const FMouseEvent*& ev)
{
  if ( ! hasSelectedItem() || ! getSelectedItem()->hasMenu() )
    return;

  // Mouse event handover to the menu
  const auto& menu = getSelectedItem()->getMenu();
  const auto& menu_geometry = menu->getTermGeometry();

  if ( menu->getCount() > 0
    && menu_geometry.contains(ev->getTermPos()) )
  {
    const auto& t = ev->getTermPos();
    const auto& p = menu->termToWidgetPos(t);
    int b = ev->getButton();

    try
    {
      const auto& _ev = \
          std::make_shared<FMouseEvent>(fc::MouseMove_Event, p, t, b);
      menu->mouse_down = true;
      setClickedWidget(menu);
      menu->onMouseMove(_ev.get());
    }
    catch (const std::bad_alloc& ex)
    {
      std::cerr << bad_alloc_str << ex.what() << std::endl;
    }
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

  if ( getStatusBar() )
    getStatusBar()->drawMessage();

  updateTerminal();
  flush();
  mouse_down = false;
}

}  // namespace finalcut
