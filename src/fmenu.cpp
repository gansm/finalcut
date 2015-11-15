// File: fmenu.cpp
// Provides: class FMenu

#include "fapp.h"
#include "fmenu.h"
#include "fstatusbar.h"

//----------------------------------------------------------------------
// class FMenu
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FMenu::FMenu(FWidget* parent)
  : FWindow(parent)
  , item(0)
  , super_menu(0)
  , maxItemWidth(0)
  , mouse_down(false)
  , has_checkable_items(false)
{
  init(parent);
}

//----------------------------------------------------------------------
FMenu::FMenu (FString& txt, FWidget* parent)
  : FWindow(parent)
  , item(0)
  , super_menu(0)
  , maxItemWidth(0)
  , mouse_down(false)
  , has_checkable_items(false)
{
  item = new FMenuItem(txt, parent);
  init(parent);
}

//----------------------------------------------------------------------
FMenu::FMenu (const std::string& txt, FWidget* parent)
  : FWindow(parent)
  , item(0)
  , super_menu(0)
  , maxItemWidth(0)
  , mouse_down(false)
  , has_checkable_items(false)
{
  item = new FMenuItem(txt, parent);
  init(parent);
}

//----------------------------------------------------------------------
FMenu::FMenu (const char* txt, FWidget* parent)
  : FWindow(parent)
  , item(0)
  , super_menu(0)
  , maxItemWidth(0)
  , mouse_down(false)
  , has_checkable_items(false)
{
  item = new FMenuItem(txt, parent);
  init(parent);
}

//----------------------------------------------------------------------
FMenu::~FMenu()
{
  activatePrevWindow();
  delWindow(this);

  const FRect& geometry = getGeometryGlobalShadow();
  restoreVTerm (geometry);

  if ( vwin != 0 )
  {
    if ( vwin->changes != 0 )
      delete[] vwin->changes;
    if ( vwin->text != 0 )
      delete[] vwin->text;
    delete vwin;
  }
}


// private methods of FMenu
//----------------------------------------------------------------------
void FMenu::init(FWidget* parent)
{
  width  = 10;
  height = 2;
  xmin = 1;
  ymin = 1;
  xmax = width;
  ymax = height;
  client_xmin = 1;
  client_ymin = 1;
  client_xmax = width;
  client_ymax = height;
  top_padding    = 1;
  left_padding   = 1;
  bottom_padding = 1;
  right_padding  = 1;
  createArea (vwin);
  setGeometry (1, 1 , 10, 2, false);  // initialize geometry values
  setTransparentShadow();
  window_object  = true;
  addWindow(this);
  hide();

  foregroundColor = wc.menu_active_fg;
  backgroundColor = wc.menu_active_bg;

  item->setMenu(this);

  if ( parent )
  {
    if ( isMenuBar(parent) )
    {
      FMenuBar* mbar = dynamic_cast<FMenuBar*>(parent);
      if ( mbar )
        mbar->menu_dimension();
    }
    setSuperMenu(parent);
  }
  menu_dimension();
}

//----------------------------------------------------------------------
void FMenu::menu_dimension()
{
  int item_X, item_Y;
  std::vector<FMenuItem*>::const_iterator iter, end;
  iter = itemlist.begin();
  end = itemlist.end();
  maxItemWidth = 10; // minimum width

  // find the maximum item width
  while ( iter != end )
  {
    uInt item_width = (*iter)->getTextLength() + 2;
    int  accel_key  = (*iter)->accel_key;

    if ( accel_key )
    {
      uInt accel_len = getKeyName(accel_key).getLength();
      item_width += accel_len + 2;
    }

    if ( has_checkable_items )
      item_width++;

    if ( item_width > maxItemWidth )
      maxItemWidth = item_width;

    ++iter;
  }

  // set widget geometry
  setGeometry (xpos, ypos, int(maxItemWidth + 2), int(count() + 2));

  // set geometry of all items
  iter = itemlist.begin();
  item_X = 1;
  item_Y = 1;

  while ( iter != end )
  {
    (*iter)->setGeometry (item_X, item_Y, int(maxItemWidth), 1);
    item_Y++;

    ++iter;
  }
}

//----------------------------------------------------------------------
bool FMenu::isMenuBar (FWidget* w) const
{
  return bool ( strcmp ( w->getClassName()
                       , const_cast<char*>("FMenuBar") ) == 0 );
}

//----------------------------------------------------------------------
bool FMenu::isMenu (FWidget* w) const
{
  return bool ( strcmp ( w->getClassName()
                       , const_cast<char*>("FMenu") ) == 0 );
}

//----------------------------------------------------------------------
bool FMenu::isRadioMenuItem (FWidget* w) const
{
  return bool ( strcmp ( w->getClassName()
                       , const_cast<char*>("FRadioMenuItem") ) == 0 );
}

//----------------------------------------------------------------------
void FMenu::hideSubMenus()
{
  // hide all sub-menus
  if ( hasSelectedItem() )
  {
    if ( getSelectedItem()->hasMenu() )
    {
      FMenu* m = getSelectedItem()->getMenu();
      m->hideSubMenus();
      m->hide();
    }
    unselectItem();
  }
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
  }
}

//----------------------------------------------------------------------
bool FMenu::containsMenuStructure (int x, int y) const
{
  // Check mouse click position for item, menu and all sub menus
  FMenuItem* si = getSelectedItem();

  if ( getGeometryGlobal().contains(x,y) )
    return true;
  else if ( si && si->hasMenu() )
    return si->getMenu()->getGeometryGlobal().contains(x,y);
  else if ( item && item->getGeometryGlobal().contains(x,y) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
bool FMenu::selectNextItem()
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
bool FMenu::selectPrevItem()
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
      setSelectedItem(prev);
      prev->setFocus();
      if ( statusBar() )
        statusBar()->drawMessage();
      redraw();
      break;
    }
  } while ( iter != begin );

  return true;
}

//----------------------------------------------------------------------
void FMenu::keypressMenuBar (FKeyEvent*& ev)
{
  FWidget* super = getSuperMenu();
  if ( super )
  {
    if ( isMenuBar(super) )
    {
      FMenuBar* mbar = reinterpret_cast<FMenuBar*>(super);
      mbar->onKeyPress(ev);
    }
  }
}

//----------------------------------------------------------------------
bool FMenu::hotkeyMenu (FKeyEvent*& ev)
{
  std::vector<FMenuItem*>::const_iterator iter, end;
  iter = itemlist.begin();
  end = itemlist.end();

  while ( iter != end )
  {
    if ( (*iter)->hasHotkey() )
    {
      bool found = false;
      int hotkey = (*iter)->getHotkey();
      int key = ev->key();

      if ( isalpha(hotkey) || isdigit(hotkey) )
      {
        if ( tolower(hotkey) == key || toupper(hotkey) == key )
          found = true;
      }
      else if ( hotkey == key )
        found = true;

      if ( found )
      {
        unselectItem();
        hide();
        hideSuperMenus();
        ev->accept();
        (*iter)->processClicked();
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
void FMenu::draw()
{
  // fill the background
  setColor (wc.menu_active_fg, wc.menu_active_bg);
  setUpdateVTerm(false);
  if ( isMonochron() )
    setReverse(true);
  clrscr();
  drawBorder();
  drawItems();
  if ( isMonochron() )
    setReverse(false);
  if ( (flags & SHADOW) != 0 )
    drawMenuShadow();
  setUpdateVTerm(true);
}

//----------------------------------------------------------------------
void FMenu::drawBorder()
{
  int x1, x2, y1, y2;
  x1 = xpos+xmin-1;
  x2 = xpos+xmin-2+width;
  y1 = ypos+ymin-1;
  y2 = ypos+ymin-2+height;

  if ( isNewFont() )
  {
    gotoxy (x1, y1);
    print (fc::NF_border_corner_upper_left); // ⎡
    for (int x=x1+1; x < x2; x++)
      print (fc::NF_border_line_upper); // ¯
    print (fc::NF_rev_border_corner_upper_right); // ⎤

    for (int y=y1+1; y <= y2; y++)
    {
      gotoxy (x1, y);
      // border left ⎸
      print (fc::NF_border_line_left);
      gotoxy (x2, y);
      // border right⎹
      print (fc::NF_rev_border_line_right);
    }
    gotoxy (x1, y2);
    // lower left corner border ⎣
    print (fc::NF_border_corner_lower_left);
    for (int x=1; x < width-1; x++) // low line _
      print (fc::NF_border_line_bottom);
    gotoxy (x2, y2);
    // lower right corner border ⎦
    print (fc::NF_rev_border_corner_lower_right);
  }
  else
  {
    gotoxy (x1, y1);
    print (fc::BoxDrawingsDownAndRight); // ┌
    for (int x=x1+1; x < x2; x++)
      print (fc::BoxDrawingsHorizontal); // ─
    print (fc::BoxDrawingsDownAndLeft);  // ┐

    gotoxy (x1, y2);
    print (fc::BoxDrawingsUpAndRight);   // └
    for (int x=x1+1; x < x2; x++)
      print (fc::BoxDrawingsHorizontal); // ─
    print (fc::BoxDrawingsUpAndLeft);    // ┘

    for (int y=y1+1; y < y2; y++)
    {
      gotoxy (x1, y);
      print (fc::BoxDrawingsVertical);   // │
      gotoxy (x2, y);
      print (fc::BoxDrawingsVertical);   // │
    }
  }
}

//----------------------------------------------------------------------
void FMenu::drawMenuShadow()
{
  drawShadow();
}

//----------------------------------------------------------------------
void FMenu::drawItems()
{
  std::vector<FMenuItem*>::const_iterator iter, end;
  int c = 0;
  int y = 0;
  iter = itemlist.begin();
  end = itemlist.end();

  if ( has_checkable_items )
    c = 1;

  while ( iter != end )
  {
    wchar_t* src;
    wchar_t* dest;
    wchar_t* item_text;
    FString txt;
    uInt txt_length;
    int  hotkeypos, to_char;
    int  accel_key = (*iter)->accel_key;
    bool is_enabled = (*iter)->isEnabled();
    bool is_checked = (*iter)->isChecked();
    bool is_checkable = (*iter)->checkable;
    bool is_radio_btn = (*iter)->radio_button;
    bool is_selected = (*iter)->isSelected();
    bool is_noUnderline = (((*iter)->getFlags() & NO_UNDERLINE) != 0);
    bool is_separator = (*iter)->isSeparator();

    if ( is_separator )
    {
      drawSeparator(y);
    }
    else
    {
      if ( is_enabled )
      {
        if ( is_selected )
        {
          foregroundColor = wc.menu_active_focus_fg;
          backgroundColor = wc.menu_active_focus_bg;
          if ( isMonochron() )
            setReverse(false);
        }
        else
        {
          foregroundColor = wc.menu_active_fg;
          backgroundColor = wc.menu_active_bg;
          if ( isMonochron() )
            setReverse(true);
        }
      }
      else
      {
        foregroundColor = wc.menu_inactive_fg;
        backgroundColor = wc.menu_inactive_bg;
        if ( isMonochron() )
          setReverse(true);
      }
      gotoxy (xpos+xmin, ypos+ymin+y);
      setColor (foregroundColor, backgroundColor);
      if ( has_checkable_items )
      {
        if ( is_checkable )
        {
          if ( is_checked )
          {
            if ( is_radio_btn )
            {
              print (fc::Bullet);
            }
            else
            {
              if ( isNewFont() )
                print (fc::NF_check_mark);
              else
                print (fc::SquareRoot);
            }
          }
          else
          {
            setColor (wc.menu_inactive_fg, backgroundColor);
            print ('-');
            setColor (foregroundColor, backgroundColor);
          }
        }
        else
          print (' ');
      }
      print (' ');

      txt = (*iter)->getText();
      txt_length = uInt(txt.getLength());
      item_text = new wchar_t[txt_length+1]();
      src  = const_cast<wchar_t*>(txt.wc_str());
      dest = const_cast<wchar_t*>(item_text);
      to_char = int(txt_length);
      hotkeypos = getHotkeyPos (src, dest, txt_length);

      if ( hotkeypos == -1 )
      {
        if ( is_selected )
          setCursorPos ( xpos+xmin+1
                       , ypos+ymin+y ); // first character
      }
      else
      {
        if ( is_selected )
          setCursorPos ( xpos+xmin+1+hotkeypos
                       , ypos+ymin+y ); // hotkey
        txt_length--;
        to_char--;
      }

      for (int z=0; z < to_char; z++)
      {
        if ( ! iswprint(wint_t(item_text[z])) )
        {
          if ( ! isNewFont() && (  int(item_text[z]) < fc::NF_rev_left_arrow2
                                || int(item_text[z]) > fc::NF_check_mark ) )
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
          setColor (foregroundColor, backgroundColor);
        }
        else
          print (item_text[z]);
      }

      if ( accel_key )
      {
        FString accel_name (getKeyName(accel_key));
        int accel_len = int(accel_name.getLength());
        int len = maxItemWidth - (to_char + accel_len + c + 2);
        if ( len > 0 )
        {
          FString spaces (len, wchar_t(' '));
          print (spaces + accel_name);
          to_char = maxItemWidth - (c + 2);
        }
      }

      if ( is_selected )
      {
        for (uInt i=uInt(to_char+c); i < maxItemWidth-1; i++)
          print (' ');
      }

      if ( isMonochron() && is_enabled && is_selected )
        setReverse(true);
      delete[] item_text;
    }
    ++iter;
    y++;
  }
  if ( hasFocus() )
    setCursor();
}

//----------------------------------------------------------------------
inline void FMenu::drawSeparator(int y)
{
  gotoxy (xpos+xmin-1, ypos+ymin+y);
  setColor (wc.menu_active_fg, wc.menu_active_bg);
  if ( isMonochron() )
    setReverse(true);
  if ( isNewFont() )
  {
    print (fc::NF_border_line_vertical_right);
    FString line(width-2, wchar_t(fc::BoxDrawingsHorizontal));
    print (line);
    print (fc::NF_rev_border_line_vertical_left);
  }
  else
  {
    print (fc::BoxDrawingsVerticalAndRight);
    FString line(width-2, wchar_t(fc::BoxDrawingsHorizontal));
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


// public methods of FMenu
//----------------------------------------------------------------------
void FMenu::onKeyPress (FKeyEvent* ev)
{
  // looking for menu hotkey
  if ( hotkeyMenu(ev) )
    return;

  // looking for menu bar hotkey
  FWidget* smenu = getSuperMenu();
  if ( smenu && isMenuBar(smenu))
  {
    FMenuBar* mbar = reinterpret_cast<FMenuBar*>(smenu);
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
        unselectItem();
        hide();
        hideSuperMenus();
        sel_item->processClicked();
      }
      ev->accept();
      break;

    case fc::Fkey_up:
      selectPrevItem();
      ev->accept();
      break;

    case fc::Fkey_down:
      selectNextItem();
      ev->accept();
      break;

    case fc::Fkey_left:
      if ( hasSelectedItem() && getSelectedItem()->hasMenu() )
      {
        FMenu* sub_menu = getSelectedItem()->getMenu();
        if ( sub_menu->isVisible() )
          hideSubMenus();
        else
          keypressMenuBar(ev);  // select previous menu
        ev->accept();
      }
      else
        keypressMenuBar(ev);  // select previous menu
      break;

    case fc::Fkey_right:
      if ( hasSelectedItem() && getSelectedItem()->hasMenu() )
      {
        FMenu* sub_menu = getSelectedItem()->getMenu();
        if ( ! sub_menu->isVisible() )
        {
          // open sub menu
          sub_menu->selectFirstItem();
          sub_menu->getSelectedItem()->setFocus();;
          sub_menu->setVisible();
          sub_menu->show();
          raiseWindow (sub_menu);
          sub_menu->redraw();
          updateTerminal();
          flush_out();
        }
        else
          keypressMenuBar(ev);  // select next menu
        ev->accept();
      }
      else
        keypressMenuBar(ev);  // select next menu
      break;

    case fc::Fkey_escape:
    case fc::Fkey_escape_mintty:
      unselectItem();
      hide();
      hideSubMenus();
      hideSuperMenus();
      activatePrevWindow();
      getActiveWindow()->getFocusWidget()->setFocus();
      getActiveWindow()->redraw();
      if ( statusBar() )
        statusBar()->drawMessage();
      updateTerminal();
      flush_out();
      ev->accept();
      break;

    default:
      break;
  }
}

//----------------------------------------------------------------------
void FMenu::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != LeftButton )
    return;

  if ( mouse_down )
    return;

  if ( ! isActiveWindow() )
    setActiveWindow(this);

  mouse_down = true;

  if ( ! itemlist.empty() )
  {
    std::vector<FMenuItem*>::const_iterator iter, end;
    bool focus_changed = false;
    FPoint mouse_pos;

    iter = itemlist.begin();
    end = itemlist.end();
    mouse_pos = ev->getPos();
    mouse_pos -= FPoint(getRightPadding(),getTopPadding());

    while ( iter != end )
    {
      int x1, x2, y, mouse_x, mouse_y;

      x1 = (*iter)->getX();
      x2 = (*iter)->getX() + (*iter)->getWidth() - 1;
      y  = (*iter)->getY();
      mouse_x = mouse_pos.getX();
      mouse_y = mouse_pos.getY();

      if (  mouse_x >= x1
         && mouse_x <= x2
         && mouse_y == y
         && ! (*iter)->isSelected() )
      {
        FWidget* focused_widget = getFocusWidget();
        FFocusEvent out (FocusOut_Event);
        FApplication::queueEvent(focused_widget, &out);
        if ( hasSelectedItem() )
          unselectItem();
        (*iter)->setSelected();
        setSelectedItem(*iter);
        (*iter)->setFocus();
        if ( focused_widget )
          focused_widget->redraw();
        if ( statusBar() )
          statusBar()->drawMessage();
        focus_changed = true;
      }
      ++iter;
    }
    if ( focus_changed )
      redraw();
  }
}

//----------------------------------------------------------------------
void FMenu::onMouseUp (FMouseEvent* ev)
{
  if ( ev->getButton() != LeftButton )
    return;

  if ( mouse_down )
  {
    mouse_down = false;
    if ( ! itemlist.empty() )
    {
      std::vector<FMenuItem*>::const_iterator iter, end;
      FPoint mouse_pos;

      iter = itemlist.begin();
      end = itemlist.end();
      mouse_pos = ev->getPos();
      mouse_pos -= FPoint(getRightPadding(),getTopPadding());

      while ( iter != end )
      {
        int x1, x2, y;

        x1 = (*iter)->getX();
        x2 = (*iter)->getX() + (*iter)->getWidth() - 1;
        y  = (*iter)->getY();

        if ( (*iter)->isSelected() )
        {
          int mouse_x = mouse_pos.getX();
          int mouse_y = mouse_pos.getY();

          if (  mouse_x >= x1
             && mouse_x <= x2
             && mouse_y == y )
          {
            unselectItem();
            hide();
            hideSuperMenus();
            (*iter)->processClicked();
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
  if ( ev->getButton() != LeftButton )
    return;

  if ( ! isActiveWindow() )
    setActiveWindow(this);

  if ( mouse_down && ! itemlist.empty() )
  {
    std::vector<FMenuItem*>::const_iterator iter, end;
    bool focus_changed = false;
    FPoint mouse_pos;

    iter = itemlist.begin();
    end = itemlist.end();
    mouse_pos = ev->getPos();
    mouse_pos -= FPoint(getRightPadding(),getTopPadding());

    while ( iter != end )
    {
      int x1, x2, y, mouse_x, mouse_y;

      x1 = (*iter)->getX();
      x2 = (*iter)->getX() + (*iter)->getWidth() - 1;
      y  = (*iter)->getY();
      mouse_x = mouse_pos.getX();
      mouse_y = mouse_pos.getY();

      if (  mouse_x >= x1
         && mouse_x <= x2
         && mouse_y == y )
      {
        if (    (*iter)->isEnabled()
           && ! (*iter)->isSelected()
           && ! (*iter)->isSeparator() )
        {
          FWidget* focused_widget = getFocusWidget();
          FFocusEvent out (FocusOut_Event);
          FApplication::queueEvent(focused_widget, &out);
          (*iter)->setSelected();
          setSelectedItem(*iter);
          (*iter)->setFocus();
          if ( focused_widget )
            focused_widget->redraw();
          if ( statusBar() )
            statusBar()->drawMessage();
          focus_changed = true;
        }
      }
      else
      {
        if ( getGeometryGlobal().contains(ev->getGlobalPos())
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

    // Mouse is over border or separator
    if ( ! hasSelectedItem() && statusBar()
       && getGeometryGlobal().contains(ev->getGlobalPos()) )
    {
      FString msg = getStatusbarMessage();
      FString curMsg = statusBar()->getMessage();
      if ( curMsg != msg )
      {
        statusBar()->setMessage(msg);
        statusBar()->drawMessage();
      }
    }

    // Mouse event handover to the menu bar
    FWidget* menubar = getSuperMenu();
    if (  menubar
       && isMenuBar(menubar)
       && menubar->getGeometryGlobal().contains(ev->getGlobalPos()) )
    {
      const FPoint& g = ev->getGlobalPos();
      const FPoint& p = menubar->globalToLocalPos(g);
      int b = ev->getButton();
      ev = new FMouseEvent (MouseMove_Event, p, g, b);
      setClickedWidget(menubar);
      FMenuBar* mbar = reinterpret_cast<FMenuBar*>(menubar);
      mbar->mouse_down = true;
      mbar->onMouseMove(ev);
      delete ev;
    }

    if ( focus_changed )
      redraw();
  }
}

//----------------------------------------------------------------------
void FMenu::show()
{
  if ( ! isVisible() )
    return;

  FWindow::show();

  // set the cursor to the focus widget
  if (  FWidget::getFocusWidget()
     && FWidget::getFocusWidget()->isVisible()
     && FWidget::getFocusWidget()->hasVisibleCursor()
     && FWidget::getFocusWidget()->isCursorInside() )
  {
    FWidget::getFocusWidget()->setCursor();
    showCursor();
    flush_out();
  }
}

//----------------------------------------------------------------------
void FMenu::hide()
{
  if ( isVisible() )
  {
    FWindow::hide();
    restoreVTerm (getGeometryGlobalShadow());
    updateTerminal();
    flush_out();

    FMenu* open_menu = static_cast<FMenu*>(getOpenMenu());
    if ( open_menu && open_menu != this )
      open_menu->hide();
    setOpenMenu(0);
    mouse_down = false;
  }
}

//----------------------------------------------------------------------
void FMenu::setGeometry (int xx, int yy, int ww, int hh, bool adjust)
{
  int old_width = width;
  int old_height = height;
  FWidget::setGeometry (xx, yy, ww, hh, adjust);
  if ( vwin && (width != old_width || height != old_height) )
    resizeArea (vwin);
}

//----------------------------------------------------------------------
void FMenu::setStatusbarMessage(FString msg)
{
  FWidget::setStatusbarMessage(msg);
  if ( item )
    item->setStatusbarMessage(msg);
}

//----------------------------------------------------------------------
void FMenu::cb_menuitem_activated (FWidget* widget, void*)
{
  FMenuItem* menuitem = static_cast<FMenuItem*>(widget);

  if ( menuitem->hasMenu() )
  {
    FMenu* menu = menuitem->getMenu();
    if ( ! menu->isVisible() )
    {
      FMenu* open_menu = static_cast<FMenu*>(getOpenMenu());
      if ( open_menu && open_menu != menu )
        open_menu->hide();
      setOpenMenu(menu);

      menu->setVisible();
      menu->show();
      raiseWindow (menu);
      menu->redraw();
      updateTerminal();
      flush_out();
      if ( ! isMenu(getSuperMenu()) )
        setOpenMenu(menu);
    }
  }
}

//----------------------------------------------------------------------
void FMenu::cb_menuitem_toggled (FWidget* widget, void*)
{
  FMenuItem* menuitem = static_cast<FMenuItem*>(widget);
  std::vector<FMenuItem*>::const_iterator iter, end;

  if ( ! has_checkable_items )
    return;
  if ( ! menuitem->isChecked() )
    return;
  if ( itemlist.empty() )
    return;

  iter = itemlist.begin();
  end = itemlist.end();

  while ( iter != end )
  {
    if (  (*iter) != menuitem
       && (*iter)->isChecked()
       && isRadioMenuItem(*iter) )
    {
      (*iter)->unsetChecked();
    }
    ++iter;
  }
}

//----------------------------------------------------------------------
bool FMenu::setTransparentShadow (bool on)
{
  if ( on )
  {
    flags |= SHADOW;
    flags |= TRANS_SHADOW;
    shadow.setPoint(2,1);
    adjustWidgetSizeShadow = getGeometry() + getShadow();
    adjustWidgetSizeGlobalShadow = getGeometryGlobal() + getShadow();
  }
  else
  {
    flags &= ~SHADOW;
    flags &= ~TRANS_SHADOW;
    shadow.setPoint(0,0);
    adjustWidgetSizeShadow = getGeometry() + getShadow();
    adjustWidgetSizeGlobalShadow = getGeometryGlobal() + getShadow();
  }
  resizeArea (vwin);
  return on;
}

