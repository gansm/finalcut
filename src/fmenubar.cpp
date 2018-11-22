/***********************************************************************
* fmenubar.cpp - Widget FMenuBar                                       *
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
#include "final/fmenubar.h"
#include "final/fstatusbar.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FMenuBar
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FMenuBar::FMenuBar(FWidget* parent)
  : FWindow(parent)
  , mouse_down(false)
  , drop_down(false)
  , focus_changed(false)
  , screenWidth(80)
{
  init();
}

//----------------------------------------------------------------------
FMenuBar::~FMenuBar()  // destructor
{
  setMenuBar(0);
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
  FColor fg, bg;
  FWindow::hide();
  fg = wc.term_fg;
  bg = wc.term_bg;
  setColor (fg, bg);
  screenWidth = getDesktopWidth();
  char* blank = createBlankArray (screenWidth + 1);
  setPrintPos (1, 1);
  print (blank);
  destroyBlankArray (blank);
}

//----------------------------------------------------------------------
void FMenuBar::adjustSize()
{
  setGeometry (1, 1, getDesktopWidth(), 1, false);
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
        FMenuItem* sel_item = getSelectedItem();

        if ( sel_item->hasMenu() )
        {
          FMenuItem* first_item;
          FMenu* menu = sel_item->getMenu();
          sel_item->openMenu();
          menu->selectFirstItem();
          first_item = menu->getSelectedItem();

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

    if ( ! item_list.empty() && hasSelectedItem() )
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
void FMenuBar::cb_item_deactivated (FWidget* widget, data_ptr)
{
  FMenuItem* menuitem = static_cast<FMenuItem*>(widget);

  if ( menuitem->hasMenu() )
  {
    FMenu* menu = menuitem->getMenu();
    menu->hide();
    menu->hideSubMenus();
  }
}


// private methods of FMenuBar
//----------------------------------------------------------------------
void FMenuBar::init()
{
  FWidget* r = getRootWidget();
  std::size_t w = r->getWidth();
  // initialize geometry values
  setGeometry (1, 1, w, 1, false);
  setAlwaysOnTop();
  setMenuBar(this);
  ignorePadding();

  if ( getRootWidget() )
    getRootWidget()->setTopPadding(1, true);

  addAccelerator (fc::Fkey_f10);
  addAccelerator (fc::Fckey_space);
  setForegroundColor (wc.menu_active_fg);
  setBackgroundColor (wc.menu_active_bg);
  unsetFocusable();
}

//----------------------------------------------------------------------
void FMenuBar::calculateDimensions()
{
  int item_X = 1;
  int item_Y = 1;
  std::vector<FMenuItem*>::const_iterator last, iter;
  iter = item_list.begin();
  last = item_list.end();

  // find the maximum item width
  while ( iter != last )
  {
    std::size_t len = (*iter)->getTextLength();
    int item_width = int(len) + 2;

    // set item geometry
    (*iter)->setGeometry (item_X, item_Y, std::size_t(item_width), 1, false);

    // set menu position
    if ( (*iter)->hasMenu() )
      (*iter)->getMenu()->setPos (item_X, item_Y, false);

    item_X += item_width;

    ++iter;
  }
}

//----------------------------------------------------------------------
bool FMenuBar::selectNextItem()
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
      } while ( ! next->isEnabled()
             || ! next->acceptFocus()
             || ! next->isVisible()
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
        FMenuItem* first_item;
        FMenu* menu = next->getMenu();
        next->openMenu();
        menu->selectFirstItem();
        first_item = menu->getSelectedItem();

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

      updateTerminal (FVTerm::stop_refresh);
      unselectItem();
      prev->setSelected();
      prev->setFocus();

      if ( drop_down && prev->hasMenu() )
      {
        FMenuItem* first_item;
        FMenu* menu = prev->getMenu();
        prev->openMenu();
        menu->selectFirstItem();
        first_item = menu->getSelectedItem();

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
  while ( iter != first );

  return true;
}

//----------------------------------------------------------------------
bool FMenuBar::hotkeyMenu (FKeyEvent*& ev)
{
  std::vector<FMenuItem*>::const_iterator iter, last;
  iter = item_list.begin();
  last = item_list.end();

  while ( iter != last )
  {
    if ( (*iter)->isEnabled() )
    {
      uChar hotkey = (*iter)->getHotkey();
      FKey key = ev->key();

      if ( fc::Fmkey_meta + FKey(std::tolower(hotkey)) == key )
      {
        FMenuItem* sel_item = getSelectedItem();

        if ( sel_item && sel_item->hasMenu() )
          sel_item->getMenu()->unselectItem();

        unselectItem();

        if ( (*iter)->hasMenu() )
        {
          FMenuItem* first_item;
          FMenu* menu = (*iter)->getMenu();
          (*iter)->setSelected();
          setSelectedItem(*iter);
          (*iter)->setFocus();
          (*iter)->openMenu();
          menu->selectFirstItem();
          first_item = menu->getSelectedItem();

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
          setSelectedItem(0);
          redraw();
          drop_down = false;
          (*iter)->processClicked();
        }

        ev->accept();
        return true;
      }
    }

    ++iter;
  }

  return false;
}

//----------------------------------------------------------------------
std::size_t FMenuBar::getHotkeyPos ( wchar_t src[]
                                   , wchar_t dest[]
                                   , std::size_t length )
{
  // find hotkey position in string
  // + generate a new string without the '&'-sign
  std::size_t hotkeypos = NOT_SET;
  wchar_t* txt = src;

  for (std::size_t i = 0; i < length; i++)
  {
    if ( i < length && txt[i] == L'&' && hotkeypos == NOT_SET )
    {
      hotkeypos = i;
      i++;
      src++;
    }

    *dest++ = *src++;
  }

  return hotkeypos;
}

//----------------------------------------------------------------------
void FMenuBar::draw()
{
  drawItems();
}

//----------------------------------------------------------------------
void FMenuBar::drawItems()
{
  std::vector<FMenuItem*>::const_iterator iter, last;
  std::size_t x = 1;

  if ( item_list.empty() )
    return;

  setPrintPos (1, 1);

  if ( isMonochron() )
    setReverse(true);

  screenWidth = getDesktopWidth();
  iter = item_list.begin();
  last = item_list.end();

  while ( iter != last )
  {
    drawItem (*iter, x);
    ++iter;
  }

  // Print spaces to end of line
  for (; x <= screenWidth; x++)
    print (' ');

  if ( isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
inline void FMenuBar::drawItem (FMenuItem* menuitem, std::size_t& x)
{
  FString txt = menuitem->getText();
  menuText txtdata;
  std::size_t txt_length = txt.getLength();
  std::size_t to_char;
  std::size_t hotkeypos;
  bool is_enabled  = menuitem->isEnabled();
  bool is_selected = menuitem->isSelected();

  txtdata.startpos = x + 1;
  txtdata.no_underline = menuitem->getFlags().no_underline;

  // Set screen attributes
  setLineAttributes (menuitem);
  drawLeadingSpace (x);

  try
  {
    txtdata.text = new wchar_t[txt_length + 1]();
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
    return;
  }

  if ( x - 1 <= screenWidth )
    to_char = txt_length;
  else
    to_char = txt_length - screenWidth - x - 1;

  hotkeypos = getHotkeyPos (txt.wc_str(), txtdata.text, txt_length);

  if ( hotkeypos != NOT_SET )
  {
    txt_length--;
    to_char--;
  }

  txtdata.length = to_char;
  x += txt_length;

  if ( ! is_enabled || is_selected )
    txtdata.hotkeypos = NOT_SET;
  else
    txtdata.hotkeypos = hotkeypos;

  drawMenuText (txtdata);
  drawEllipsis (txtdata, x);
  drawTrailingSpace (x);

  setColor (wc.menu_active_fg, wc.menu_active_bg);

  if ( isMonochron() && is_enabled && is_selected )
    setReverse(true);

  delete[] txtdata.text;
}

//----------------------------------------------------------------------
inline void FMenuBar::setLineAttributes (FMenuItem* menuitem)
{
  bool is_enabled  = menuitem->isEnabled();
  bool is_selected = menuitem->isSelected();

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
inline void FMenuBar::drawMenuText (menuText& data)
{
  // Print menu text

  for (std::size_t z = 0; z < data.length; z++)
  {
    if ( data.startpos > screenWidth - z )
      break;

    if ( ! std::iswprint(wint_t(data.text[z])) )
    {
      if ( ! isNewFont()
        && ( int(data.text[z]) < fc::NF_rev_left_arrow2
          || int(data.text[z]) > fc::NF_check_mark ) )
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
inline void FMenuBar::drawEllipsis (menuText& txtdata, std::size_t x)
{
  if ( x > screenWidth + 1 )
  {
    if ( txtdata.startpos < screenWidth )
    {
      // Print ellipsis
      setPrintPos (int(screenWidth) - 1, 1);
      print ("..");
    }
    else if ( txtdata.startpos - 1 <= screenWidth )
    {
      // Hide first character from text
      setPrintPos (int(screenWidth), 1);
      print (' ');
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
  std::vector<FMenuItem*>::const_iterator last, iter;
  iter = item_list.begin();
  last = item_list.end();

  while ( iter != last )
  {
    // get item width
    int item_width = int((*iter)->getWidth());

    if ( (*iter)->hasMenu() )
    {
      FMenu* menu = (*iter)->getMenu();

      // set menu position
      menu->setPos (menu->adjustX(item_X), item_Y);

      // call menu adjustItems()
      menu->adjustItems();
    }

    item_X += item_width;
    ++iter;
  }
}

//----------------------------------------------------------------------
void FMenuBar::selectMenuItem (FMenuItem* item)
{
  if ( ! item->isEnabled() || item->isSelected() )
    return;

  FWidget* focused_widget = getFocusWidget();
  FFocusEvent out (fc::FocusOut_Event);
  FApplication::queueEvent(focused_widget, &out);
  item->setSelected();
  item->setFocus();

  if ( focused_widget && ! focused_widget->isWindowWidget() )
    focused_widget->redraw();

  item->openMenu();
  setSelectedItem(item);
  focus_changed = true;

  if ( item->hasMenu() )
  {
    FMenu* menu = item->getMenu();

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

  FMenu* menu = item->getMenu();

  if ( ! menu->hasSelectedItem() )
  {
    FMenuItem* first_item;
    menu->selectFirstItem();
    first_item = menu->getSelectedItem();

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
    setSelectedItem(0);
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
    setSelectedItem(0);
}

//----------------------------------------------------------------------
void FMenuBar::mouseDownOverList (FMouseEvent* ev)
{
  if ( item_list.empty() )
    return;

  std::vector<FMenuItem*>::const_iterator iter, last;
  int mouse_x, mouse_y;
  focus_changed = false;

  iter = item_list.begin();
  last = item_list.end();
  mouse_x = ev->getX();
  mouse_y = ev->getY();

  while ( iter != last )
  {
    int x1, x2;
    x1 = (*iter)->getX();
    x2 = (*iter)->getX() + int((*iter)->getWidth());

    if ( mouse_y == 1 )
    {
      if ( mouse_x >= x1 && mouse_x < x2 )
      {
        // Mouse pointer over item
        selectMenuItem (*iter);
      }
      else
      {
        unselectMenuItem (*iter);
      }
    }

    ++iter;
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
void FMenuBar::mouseUpOverList (FMouseEvent* ev)
{
  if ( item_list.empty() )
    return;

  int mouse_x, mouse_y;
  std::vector<FMenuItem*>::const_iterator iter, last;
  iter = item_list.begin();
  last = item_list.end();
  mouse_x = ev->getX();
  mouse_y = ev->getY();

  while ( iter != last )
  {
    int x1, x2;
    x1 = (*iter)->getX();
    x2 = (*iter)->getX() + int((*iter)->getWidth());

    if ( mouse_y == 1
      && mouse_x >= x1
      && mouse_x < x2
      && (*iter)->isEnabled()
      && (*iter)->isSelected() )
    {
      // Mouse pointer over item
      if ( ! activateMenu(*iter) )
      {
        if ( clickItem(*iter) )
          return;
      }
    }
    else
    {
      unselectMenuItem(*iter);
      redraw();
    }

    ++iter;
  }

  if ( ! hasSelectedItem() )
    leaveMenuBar();
}

//----------------------------------------------------------------------
void FMenuBar::mouseMoveOverList (FMouseEvent* ev)
{
  if ( item_list.empty() )
    return;

  std::vector<FMenuItem*>::const_iterator iter, last;
  int mouse_x, mouse_y;
  bool mouse_over_menubar = false;
  focus_changed = false;
  iter = item_list.begin();
  last = item_list.end();
  mouse_x = ev->getX();
  mouse_y = ev->getY();

  if ( getTermGeometry().contains(ev->getTermPos()) )
    mouse_over_menubar = true;

  while ( iter != last )
  {
    int x1, x2;
    x1 = (*iter)->getX();
    x2 = (*iter)->getX() + int((*iter)->getWidth());

    if ( mouse_x >= x1
      && mouse_x < x2
      && mouse_y == 1 )
    {
      // Mouse pointer over item
      selectMenuItem(*iter);
    }
    else
    {
      if ( mouse_over_menubar )
      {
        // Unselect selected item without mouse focus
        unselectMenuItem(*iter);
      }
      else
      {
        // Event handover to the menu
        passEventToMenu(ev);
      }
    }

    ++iter;
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
void FMenuBar::passEventToMenu (FMouseEvent*& ev)
{
  if ( ! hasSelectedItem() || ! getSelectedItem()->hasMenu() )
    return;

  // Mouse event handover to the menu
  FMenu* menu = getSelectedItem()->getMenu();
  const FRect& menu_geometry = menu->getTermGeometry();

  if ( menu->getCount() > 0
    && menu_geometry.contains(ev->getTermPos()) )
  {
    const FPoint& t = ev->getTermPos();
    const FPoint& p = menu->termToWidgetPos(t);
    int b = ev->getButton();

    try
    {
      FMouseEvent* _ev = new FMouseEvent (fc::MouseMove_Event, p, t, b);
      menu->mouse_down = true;
      setClickedWidget(menu);
      menu->onMouseMove(_ev);
      delete _ev;
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
  flush_out();
  mouse_down = false;
}

}  // namespace finalcut
