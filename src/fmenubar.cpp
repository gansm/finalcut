// File: fmenubar.cpp
// Provides: class FMenuBar

#include "fapp.h"
#include "fmenubar.h"
#include "fstatusbar.h"

//----------------------------------------------------------------------
// class FMenuBar
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FMenuBar::FMenuBar(FWidget* parent)
  : FWindow(parent)
  , mouse_down(false)
  , drop_down(false)
{
  init();
}

//----------------------------------------------------------------------
FMenuBar::~FMenuBar()
{
  if ( vmenubar != 0 )
  {
    if ( vmenubar->changes != 0 )
      delete[] vmenubar->changes;
    if ( vmenubar->text != 0 )
      delete[] vmenubar->text;
    delete vmenubar;
  }
  vmenubar = 0;
}


// private methods of FMenuBar
//----------------------------------------------------------------------
void FMenuBar::init()
{
  xmin = ymin = 1;
  xpos = 1;
  ypos = 1;
  createArea (vmenubar);
  vmenubar->visible = true;
  // initialize geometry values
  setGeometry (1, 1, getColumnNumber(), 1, false);
  getRootWidget()->setTopPadding(1, true);
  setMenuBar(this);
  addAccelerator (fc::Fkey_f10);
  addAccelerator (fc::Fckey_space);
  foregroundColor = wc.menu_active_fg;
  backgroundColor = wc.menu_active_bg;
  window_object  = true;
  ignore_padding = true;
  unsetFocusable();
}

//----------------------------------------------------------------------
void FMenuBar::menu_dimension()
{
  int item_X = 1;
  int item_Y = 1;
  std::vector<FMenuItem*>::const_iterator end, iter;
  iter = itemlist.begin();
  end = itemlist.end();

  // find the maximum item width
  while ( iter != end )
  {
    uInt len = (*iter)->getTextLength();
    int item_width = int(len + 2);

    // set item geometry
    (*iter)->setGeometry (item_X, item_Y, item_width, 1, false);

    // set menu position
    if ( (*iter)->hasMenu() )
      (*iter)->getMenu()->setPos (item_X, item_Y, false);

    item_X += item_width;

    ++iter;
  }
}

//----------------------------------------------------------------------
bool FMenuBar::isMenu (FMenuItem* mi) const
{
  return mi->hasMenu();
}

//----------------------------------------------------------------------
bool FMenuBar::selectNextItem()
{
  std::vector<FMenuItem*>::const_iterator iter, end;
  iter = itemlist.begin();
  end = itemlist.end();

  while ( iter != end )
  {
    if ( (*iter)->isSelected() )
    {
      FMenuItem* next;
      std::vector<FMenuItem*>::const_iterator next_element;

      next_element = iter;
      do
      {
        ++next_element;
        if ( next_element == itemlist.end() )
          next_element = itemlist.begin();
        next = static_cast<FMenuItem*>(*next_element);
      } while (  ! next->isEnabled()
              || ! next->acceptFocus()
              || ! next->isVisible()
              || next->isSeparator() );
      if ( next == *iter )
        return false;
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
      if ( statusBar() )
        statusBar()->drawMessage();
      redraw();
      break;
    }
    ++iter;
  }
  return true;
}

//----------------------------------------------------------------------
bool FMenuBar::selectPrevItem()
{
  std::vector<FMenuItem*>::const_iterator iter, begin;
  iter = itemlist.end();
  begin = itemlist.begin();

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
        if ( prev_element == itemlist.begin() )
          prev_element = itemlist.end();
        --prev_element;
        prev = static_cast<FMenuItem*>(*prev_element);
      } while (  ! prev->isEnabled()
              || ! prev->acceptFocus()
              || ! prev->isVisible()
              || prev->isSeparator() );
      if ( prev == *iter )
        return false;
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
      if ( statusBar() )
        statusBar()->drawMessage();
      setSelectedItem(prev);
      redraw();
      break;
    }
  } while ( iter != begin );

  return true;
}

//----------------------------------------------------------------------
bool FMenuBar::hotkeyMenu (FKeyEvent*& ev)
{
  std::vector<FMenuItem*>::const_iterator iter, end;
  iter = itemlist.begin();
  end = itemlist.end();

  while ( iter != end )
  {
    if ( (*iter)->isEnabled() )
    {
      int hotkey = (*iter)->getHotkey();
      int key = ev->key();

      if ( fc::Fmkey_meta + tolower(hotkey) == key )
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
           if ( statusBar() )
             statusBar()->drawMessage();
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
int FMenuBar::getHotkeyPos (wchar_t*& src, wchar_t*& dest, uInt length)
{
  // find hotkey position in string
  // + generate a new string without the '&'-sign
  int hotkeypos = -1;
  wchar_t* txt = src;

  for (uInt i=0; i < length; i++)
  {
    if ( (i < length) && (txt[i] == L'&') && (hotkeypos == -1) )
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
void FMenuBar::draw()
{
  xmin = ymin = 1;
  height = 1;
  xpos = 1;
  drawItems();
}

//----------------------------------------------------------------------
void FMenuBar::drawItems()
{
  std::vector<FMenuItem*>::const_iterator iter, end;
  int screenWidth;

  int x = 1;
  screenWidth = getColumnNumber();
  width = screenWidth;
  ypos = 1;

  if ( itemlist.empty() )
    return;

  setUpdateVTerm(false);
  gotoxy (1,1);

  if ( isMonochron() )
    setReverse(true);

  iter = itemlist.begin();
  end = itemlist.end();

  while ( iter != end )
  {
    wchar_t* src;
    wchar_t* dest;
    wchar_t* item_text;
    FString txt;
    uInt txt_length;
    int  hotkeypos, startpos, to_char;
    bool is_active, is_selected, is_noUnderline;

    startpos = x + 1;
    is_active = (*iter)->isEnabled();
    is_selected = (*iter)->isSelected();
    is_noUnderline = (((*iter)->getFlags() & NO_UNDERLINE) != 0);

    if ( is_active )
    {
      if ( is_selected )
      {
        if ( isMonochron() )
          setReverse(false);
        foregroundColor = wc.menu_active_focus_fg;
        backgroundColor = wc.menu_active_focus_bg;
      }
      else
      {
        foregroundColor = wc.menu_active_fg;
        backgroundColor = wc.menu_active_bg;
      }
    }
    else
    {
      foregroundColor = wc.menu_inactive_fg;
      backgroundColor = wc.menu_inactive_bg;
    }
    setColor (foregroundColor, backgroundColor);

    if ( x < screenWidth )
    {
      x++;
      print (vmenubar, ' ');
    }

    txt = (*iter)->getText();
    txt_length = uInt(txt.getLength());
    item_text = new wchar_t[txt_length+1]();
    src  = const_cast<wchar_t*>(txt.wc_str());
    dest = const_cast<wchar_t*>(item_text);

    if ( x-1 <= screenWidth )
      to_char = int(txt_length);
    else
      to_char = int(txt_length) - (screenWidth-x-1);

    hotkeypos = getHotkeyPos (src, dest, txt_length);

    if ( hotkeypos != -1 )
    {
      txt_length--;
      to_char--;
    }

    x += int(txt_length);

    for (int z=0; z < to_char; z++)
    {
      if ( startpos > screenWidth-z )
        break;
      if ( ! iswprint(wint_t(item_text[z])) )
      {
        if ( ! isNewFont() && (  int(item_text[z]) < fc::NF_rev_left_arrow2
                              || int(item_text[z]) > fc::NF_check_mark ) )
        {
          item_text[z] = L' ';
        }
      }
      if ( (z == hotkeypos) && is_active && ! is_selected )
      {
        setColor (wc.menu_hotkey_fg, wc.menu_hotkey_bg);
        if ( ! is_noUnderline )
          setUnderline();
        print (vmenubar, item_text[z]);
        if ( ! is_noUnderline )
          unsetUnderline();
        setColor (foregroundColor, backgroundColor);
      }
      else
        print (vmenubar, item_text[z]);
    }

    if ( x > screenWidth+1 )
    {
      if ( startpos < screenWidth )
      {
        gotoxy(screenWidth-1,1);
        print (vmenubar, "..");
      }
      else if ( startpos-2 < screenWidth )
      {
        gotoxy(screenWidth,1);
        print (vmenubar, ' ');
      }
    }

    if ( x < screenWidth )
    {
      x++;
      print (vmenubar, ' ');
    }

    setColor (wc.menu_active_fg, wc.menu_active_bg);
    if ( isMonochron() && is_active && is_selected )
      setReverse(true);
    delete[] item_text;

    ++iter;
  }

  for (; x <= screenWidth; x++)
    print (vmenubar, ' ');

  if ( isMonochron() )
    setReverse(false);

  setUpdateVTerm(true);
}

//----------------------------------------------------------------------
void FMenuBar::adjustItems()
{
  int item_X = 1;
  int item_Y = 1;
  std::vector<FMenuItem*>::const_iterator end, iter;
  iter = itemlist.begin();
  end = itemlist.end();

  while ( iter != end )
  {
    // get item width
    int item_width = (*iter)->getWidth();

    if ( (*iter)->hasMenu() )
    {
      FMenu* menu = (*iter)->getMenu();

      // set menu position
      menu->move (menu->adjustX(item_X), item_Y);

      // call menu adjustItems()
      menu->adjustItems();
    }
    item_X += item_width;

    ++iter;
  }
}

//----------------------------------------------------------------------
void FMenuBar::leaveMenuBar()
{
  resetMenu();
  redraw();
  activatePrevWindow();
  raiseWindow (getActiveWindow());
  getActiveWindow()->getFocusWidget()->setFocus();
  getActiveWindow()->redraw();
  if ( statusBar() )
    statusBar()->drawMessage();
  updateTerminal();
  flush_out();
  mouse_down = false;
}

// public methods of FMenuBar
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
          if ( statusBar() )
            statusBar()->drawMessage();
          redraw();
          drop_down = true;
        }
        else if (  ev->key() == fc::Fkey_return
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
  if ( ev->getButton() != LeftButton )
  {
    mouse_down = false;

    if ( ! itemlist.empty() && hasSelectedItem() )
      leaveMenuBar();
    else
      return;

    if ( statusBar() )
          statusBar()->clearMessage();
    return;
  }

  if ( mouse_down )
    return;

  mouse_down = true;

  if ( ! isActiveWindow() )
    setActiveWindow(this);

  if ( ! itemlist.empty() )
  {
    std::vector<FMenuItem*>::const_iterator iter, end;
    int mouse_x, mouse_y;
    bool focus_changed = false;

    iter = itemlist.begin();
    end = itemlist.end();
    mouse_x = ev->getX();
    mouse_y = ev->getY();

    while ( iter != end )
    {
      int x1, x2;
      x1 = (*iter)->getX();
      x2 = (*iter)->getX() + (*iter)->getWidth() - 1;

      if (  mouse_x >= x1
         && mouse_x <= x2
         && mouse_y == 1 )
      {
        // Mouse pointer over item
        if ( (*iter)->isEnabled() && ! (*iter)->isSelected() )
        {
          FWidget* focused_widget = getFocusWidget();
          FFocusEvent out (FocusOut_Event);
          FApplication::queueEvent(focused_widget, &out);
          (*iter)->setSelected();
          (*iter)->setFocus();
          if ( focused_widget )
            focused_widget->redraw();
          (*iter)->openMenu();
          setSelectedItem(*iter);
          focus_changed = true;

          if ( (*iter)->hasMenu() )
          {
            FMenu* menu = (*iter)->getMenu();
            if ( menu->hasSelectedItem() )
            {
              menu->unselectItem();
              menu->redraw();
              drop_down = true;
            }
          }
        }
      }
      else
      {
        if ( mouse_y == 1
           && (*iter)->isEnabled()
           && (*iter)->isSelected() )
        {
          (*iter)->unsetSelected();
          if ( getSelectedItem() == *iter )
            setSelectedItem(0);
          focus_changed = true;
        }
      }
      ++iter;
    }
    if ( statusBar() )
      statusBar()->drawMessage();
    if ( focus_changed )
      redraw();
  }
}

//----------------------------------------------------------------------
void FMenuBar::onMouseUp (FMouseEvent* ev)
{
  if ( ev->getButton() != LeftButton )
    return;

  if ( mouse_down )
  {
    mouse_down = false;
    if ( ! itemlist.empty() )
    {
      std::vector<FMenuItem*>::const_iterator iter, end;
      int mouse_x, mouse_y;

      iter = itemlist.begin();
      end = itemlist.end();
      mouse_x = ev->getX();
      mouse_y = ev->getY();

      while ( iter != end )
      {
        int x1, x2;

        x1 = (*iter)->getX();
        x2 = (*iter)->getX() + (*iter)->getWidth() - 1;

        if (  mouse_x >= x1
           && mouse_x <= x2
           && mouse_y == 1
           && (*iter)->isEnabled()
           && (*iter)->isSelected() )
        {
          // Mouse pointer over item
          if ( (*iter)->hasMenu() )
          {
            FMenu* menu = (*iter)->getMenu();
            if ( ! menu->hasSelectedItem() )
            {
              FMenuItem* first_item;
              menu->selectFirstItem();
              first_item = menu->getSelectedItem();
              if ( first_item )
                first_item->setFocus();
              menu->redraw();
              if ( statusBar() )
                statusBar()->drawMessage();
              redraw();
              drop_down = true;
            }
          }
          else
          {
            (*iter)->unsetSelected();
            if ( getSelectedItem() == *iter )
            {
              setSelectedItem(0);
              leaveMenuBar();
              drop_down = false;
              (*iter)->processClicked();
              return;
            }
          }
        }
        else
        {
          if ( mouse_y == 1
             && (*iter)->isEnabled()
             && (*iter)->isSelected() )
          {
            (*iter)->unsetSelected();
            if ( getSelectedItem() == *iter )
              setSelectedItem(0);
            redraw();
          }
        }
        ++iter;
      }
      if ( ! hasSelectedItem() )
        leaveMenuBar();
    }
  }
}

//----------------------------------------------------------------------
void FMenuBar::onMouseMove (FMouseEvent* ev)
{
  if ( ev->getButton() != LeftButton )
    return;

  if ( ! isActiveWindow() )
    setActiveWindow(this);

  if ( mouse_down && ! itemlist.empty() )
  {
    std::vector<FMenuItem*>::const_iterator iter, end;
    int mouse_x, mouse_y;
    bool mouse_over_menubar = false;
    bool focus_changed = false;

    iter = itemlist.begin();
    end = itemlist.end();
    mouse_x = ev->getX();
    mouse_y = ev->getY();

    if ( getGeometryGlobal().contains(ev->getGlobalPos()) )
      mouse_over_menubar = true;

    while ( iter != end )
    {
      int x1, x2;

      x1 = (*iter)->getX();
      x2 = (*iter)->getX() + (*iter)->getWidth() - 1;

      if (  mouse_x >= x1
         && mouse_x <= x2
         && mouse_y == 1 )
      {
        // Mouse pointer over item
        if ( (*iter)->isEnabled() && ! (*iter)->isSelected() )
        {
          FWidget* focused_widget = getFocusWidget();
          FFocusEvent out (FocusOut_Event);
          FApplication::queueEvent(focused_widget, &out);
          (*iter)->setSelected();
          (*iter)->setFocus();
          if ( focused_widget )
            focused_widget->redraw();
          (*iter)->openMenu();
          setSelectedItem(*iter);
          focus_changed = true;

          if ( (*iter)->hasMenu() )
          {
             FMenu* menu = (*iter)->getMenu();
             if ( menu->hasSelectedItem() )
             {
               menu->unselectItem();
               menu->redraw();
               drop_down = true;
             }
          }
        }
        else if ( statusBar() )
          statusBar()->clearMessage();
      }
      else
      {
        if (  mouse_over_menubar
           && (*iter)->isEnabled()
           && (*iter)->isSelected() )
        {
          // Unselect selected item without mouse focus
          (*iter)->unsetSelected();
          if ( getSelectedItem() == *iter )
            setSelectedItem(0);
          focus_changed = true;
          drop_down = false;
        }
        else if ( hasSelectedItem() && getSelectedItem()->hasMenu() )
        {
          // Mouse event handover to the menu
          FMenu* menu = getSelectedItem()->getMenu();
          const FRect& menu_geometry = menu->getGeometryGlobal();

          if (  menu->count() > 0
             && menu_geometry.contains(ev->getGlobalPos()) )
          {
            const FPoint& g = ev->getGlobalPos();
            const FPoint& p = menu->globalToLocalPos(g);
            int b = ev->getButton();
            FMouseEvent* _ev = new FMouseEvent (MouseMove_Event, p, g, b);
            menu->mouse_down = true;
            setClickedWidget(menu);
            menu->onMouseMove(_ev);
            delete _ev;
          }
        }
      }
      ++iter;
    }
    if ( statusBar() )
        statusBar()->drawMessage();
    if ( focus_changed )
      redraw();
  }
}

//----------------------------------------------------------------------
void FMenuBar::onAccel (FAccelEvent* ev)
{
  unselectItem();
  selectFirstItem();
  getSelectedItem()->setFocus();
  if ( statusBar() )
    statusBar()->drawMessage();
  redraw();
  ev->accept();
}

//----------------------------------------------------------------------
void FMenuBar::hide()
{
  int fg, bg, screenWidth;
  char* blank;

  FWindow::hide();

  fg = wc.term_fg;
  bg = wc.term_bg;
  setColor (fg, bg);

  screenWidth = getColumnNumber();
  blank = new char[screenWidth+1];
  memset(blank, ' ', uLong(screenWidth));
  blank[screenWidth] = '\0';

  gotoxy (1,1);
  print (vmenubar, blank);
  delete[] blank;
}

//----------------------------------------------------------------------
void FMenuBar::resetMenu()
{
  unselectItem();
  drop_down = false;
}

//----------------------------------------------------------------------
void FMenuBar::adjustSize()
{
  xmin = ymin = 1;
  height = 1;
  xpos = 1;
  width = getColumnNumber();
  ypos = 1;
  adjustItems();
}

//----------------------------------------------------------------------
void FMenuBar::setGeometry (int xx, int yy, int ww, int hh, bool adjust)
{
  int old_width = width;
  int old_height = height;
  FWidget::setGeometry (xx, yy, ww, hh, adjust);
  if ( vmenubar && (width != old_width || height != old_height) )
    resizeArea (vmenubar);
}

//----------------------------------------------------------------------
void FMenuBar::cb_item_deactivated (FWidget* widget, void*)
{
  FMenuItem* menuitem = static_cast<FMenuItem*>(widget);

  if ( menuitem->hasMenu() )
  {
    FMenu* menu = menuitem->getMenu();
    menu->hide();
    menu->hideSubMenus();
  }
}
