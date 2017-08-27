// File: fmenubar.cpp
// Provides: class FMenuBar

#include "fapplication.h"
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
  setMenuBar(0);
}


// public methods of FMenuBar
//----------------------------------------------------------------------
void FMenuBar::hide()
{
  int screenWidth;
  short fg, bg;
  char* blank;

  FWindow::hide();
  fg = wc.term_fg;
  bg = wc.term_bg;
  setColor (fg, bg);
  screenWidth = getColumnNumber();

  if ( screenWidth < 0 )
    return;

  try
  {
    blank = new char[screenWidth + 1];
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
    return;
  }

  std::memset(blank, ' ', uLong(screenWidth));
  blank[screenWidth] = '\0';
  setPrintPos (1,1);
  print (blank);
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
  setGeometry (1, 1, getColumnNumber(), 1, false);
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

  if ( ! item_list.empty() )
  {
    std::vector<FMenuItem*>::const_iterator iter, end;
    int mouse_x, mouse_y;
    bool focus_changed = false;

    iter = item_list.begin();
    end = item_list.end();
    mouse_x = ev->getX();
    mouse_y = ev->getY();

    while ( iter != end )
    {
      int x1, x2;
      x1 = (*iter)->getX();
      x2 = (*iter)->getX() + (*iter)->getWidth();

      if ( mouse_y == 1 )
      {
        if ( mouse_x >= x1 && mouse_x < x2 )
        {
          // Mouse pointer over item
          if ( (*iter)->isEnabled() && ! (*iter)->isSelected() )
          {
            FWidget* focused_widget = getFocusWidget();
            FFocusEvent out (fc::FocusOut_Event);
            FApplication::queueEvent(focused_widget, &out);
            (*iter)->setSelected();
            (*iter)->setFocus();

            if ( focused_widget && ! focused_widget->isWindowWidget() )
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
        else if ( (*iter)->isEnabled() && (*iter)->isSelected() )
        {
          (*iter)->unsetSelected();

          if ( getSelectedItem() == *iter )
            setSelectedItem(0);

          focus_changed = true;
        }
      }

      ++iter;
    }

    if ( getStatusBar() )
      getStatusBar()->drawMessage();

    if ( focus_changed )
    {
      redraw();
      updateTerminal();
    }
  }
}

//----------------------------------------------------------------------
void FMenuBar::onMouseUp (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton )
    return;

  if ( mouse_down )
  {
    mouse_down = false;

    if ( ! item_list.empty() )
    {
      int mouse_x, mouse_y;
      std::vector<FMenuItem*>::const_iterator iter, end;
      iter = item_list.begin();
      end = item_list.end();
      mouse_x = ev->getX();
      mouse_y = ev->getY();

      while ( iter != end )
      {
        int x1, x2;
        x1 = (*iter)->getX();
        x2 = (*iter)->getX() + (*iter)->getWidth();

        if ( mouse_y == 1 && (*iter)->isEnabled() && (*iter)->isSelected() )
        {
          if ( mouse_x >= x1 && mouse_x < x2 )
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

                if ( getStatusBar() )
                  getStatusBar()->drawMessage();

                redraw();
                menu->redraw();
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
  if ( ev->getButton() != fc::LeftButton )
    return;

  if ( ! isWindowActive() )
    setActiveWindow(this);

  if ( mouse_down && ! item_list.empty() )
  {
    std::vector<FMenuItem*>::const_iterator iter, end;
    int mouse_x, mouse_y;
    bool mouse_over_menubar = false;
    bool focus_changed = false;
    iter = item_list.begin();
    end = item_list.end();
    mouse_x = ev->getX();
    mouse_y = ev->getY();

    if ( getTermGeometry().contains(ev->getTermPos()) )
      mouse_over_menubar = true;

    while ( iter != end )
    {
      int x1, x2;
      x1 = (*iter)->getX();
      x2 = (*iter)->getX() + (*iter)->getWidth();

      if ( mouse_x >= x1
          && mouse_x < x2
          && mouse_y == 1 )
      {
        // Mouse pointer over item
        if ( (*iter)->isEnabled() && ! (*iter)->isSelected() )
        {
          FWidget* focused_widget = getFocusWidget();
          FFocusEvent out (fc::FocusOut_Event);
          FApplication::queueEvent(focused_widget, &out);
          (*iter)->setSelected();
          (*iter)->setFocus();

          if ( focused_widget && ! focused_widget->isWindowWidget() )
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
        else if ( getStatusBar() )
          getStatusBar()->clearMessage();
      }
      else
      {
        if ( mouse_over_menubar
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
          const FRect& menu_geometry = menu->getTermGeometry();

          if ( menu->getCount() > 0
              && menu_geometry.contains(ev->getTermPos()) )
          {
            FMouseEvent* _ev;
            const FPoint& t = ev->getTermPos();
            const FPoint& p = menu->termToWidgetPos(t);
            int b = ev->getButton();

            try
            {
              _ev = new FMouseEvent (fc::MouseMove_Event, p, t, b);
              menu->mouse_down = true;
              setClickedWidget(menu);
              menu->onMouseMove(_ev);
              delete _ev;
            }
            catch (const std::bad_alloc& ex)
            {
              std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
            }
          }
        }
      }

      ++iter;
    }

    if ( getStatusBar() )
        getStatusBar()->drawMessage();

    if ( focus_changed )
    {
      redraw();
      updateTerminal();
    }
  }
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
  int w = r->getWidth();
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
  std::vector<FMenuItem*>::const_iterator end, iter;
  iter = item_list.begin();
  end = item_list.end();

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
bool FMenuBar::selectNextItem()
{
  std::vector<FMenuItem*>::const_iterator iter, end;
  iter = item_list.begin();
  end = item_list.end();

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

        if ( next_element == item_list.end() )
          next_element = item_list.begin();

        next = static_cast<FMenuItem*>(*next_element);
      } while ( ! next->isEnabled()
               || ! next->acceptFocus()
               || ! next->isVisible()
               || next->isSeparator() );

      if ( next == *iter )
        return false;

      updateTerminal(false);
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
      updateTerminal(true);
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
  iter = item_list.end();
  begin = item_list.begin();

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

      updateTerminal(false);
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
      updateTerminal(true);
      break;
    }
  }
  while ( iter != begin );

  return true;
}

//----------------------------------------------------------------------
bool FMenuBar::hotkeyMenu (FKeyEvent*& ev)
{
  std::vector<FMenuItem*>::const_iterator iter, end;
  iter = item_list.begin();
  end = item_list.end();

  while ( iter != end )
  {
    if ( (*iter)->isEnabled() )
    {
      int hotkey = (*iter)->getHotkey();
      int key = ev->key();

      if ( fc::Fmkey_meta + std::tolower(hotkey) == key )
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
int FMenuBar::getHotkeyPos (wchar_t*& src, wchar_t*& dest, uInt length)
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
void FMenuBar::draw()
{
  drawItems();
}

//----------------------------------------------------------------------
void FMenuBar::drawItems()
{
  std::vector<FMenuItem*>::const_iterator iter, end;
  int screenWidth;
  int x = 1;
  screenWidth = getColumnNumber();

  if ( item_list.empty() )
    return;

  setPrintPos (1,1);

  if ( isMonochron() )
    setReverse(true);

  iter = item_list.begin();
  end = item_list.end();

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
    is_noUnderline = (((*iter)->getFlags() & fc::no_underline) != 0);

    if ( is_active )
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

    if ( x < screenWidth )
    {
      x++;
      print (' ');
    }

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

    if ( x - 1 <= screenWidth )
      to_char = int(txt_length);
    else
      to_char = int(txt_length) - (screenWidth - x - 1);

    hotkeypos = getHotkeyPos (src, dest, txt_length);

    if ( hotkeypos != -1 )
    {
      txt_length--;
      to_char--;
    }

    x += int(txt_length);

    for (int z = 0; z < to_char; z++)
    {
      if ( startpos > screenWidth - z )
        break;

      if ( ! std::iswprint(wint_t(item_text[z])) )
      {
        if ( ! isNewFont()
            && ( int(item_text[z]) < fc::NF_rev_left_arrow2
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

        print (item_text[z]);

        if ( ! is_noUnderline )
          unsetUnderline();

        setColor();
      }
      else
        print (item_text[z]);
    }

    if ( x > screenWidth + 1 )
    {
      if ( startpos < screenWidth )
      {
        setPrintPos (screenWidth - 1, 1);
        print ("..");
      }
      else if ( startpos - 1 <= screenWidth )
      {
        setPrintPos (screenWidth, 1);
        print (' ');
      }
    }

    if ( x < screenWidth )
    {
      x++;
      print (' ');
    }

    setColor (wc.menu_active_fg, wc.menu_active_bg);

    if ( isMonochron() && is_active && is_selected )
      setReverse(true);

    delete[] item_text;
    ++iter;
  }

  for (; x <= screenWidth; x++)
    print (' ');

  if ( isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
void FMenuBar::adjustItems()
{
  int item_X = 1;
  int item_Y = 1;
  std::vector<FMenuItem*>::const_iterator end, iter;
  iter = item_list.begin();
  end = item_list.end();

  while ( iter != end )
  {
    // get item width
    int item_width = (*iter)->getWidth();

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
void FMenuBar::leaveMenuBar()
{
  resetMenu();
  redraw();

  if ( getStatusBar() )
    getStatusBar()->clearMessage();

  switchToPrevWindow();

  if ( getStatusBar() )
    getStatusBar()->drawMessage();

  updateTerminal();
  flush_out();
  mouse_down = false;
}
