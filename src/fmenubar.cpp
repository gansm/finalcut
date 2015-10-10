// File: fmenubar.cpp
// Provides: class FMenuBar

#include "fapp.h"
#include "fmenubar.h"

//----------------------------------------------------------------------
// class FMenuBar
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FMenuBar::FMenuBar(FWidget* parent)
  : FWindow(parent)
  , mouse_down(false)
  , selectedMenuItem()
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

  // find the max item width
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

  iter = itemlist.begin();
  end = itemlist.end();

  while ( iter != end )
  {
    wchar_t* src;
    wchar_t* dest;
    wchar_t* item_text;
    FString txt;
    uInt txt_length;
    int  hotkeypos, to_char;
    bool is_Active, is_Selected, is_NoUnderline;

    is_Active = (*iter)->isActivated();
    is_Selected = (*iter)->isSelected();
    is_NoUnderline = (((*iter)->getFlags() & NO_UNDERLINE) != 0);

    if ( is_Active )
    {
      if ( is_Selected )
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
      }
    }
    else
    {
      foregroundColor = wc.menu_inactive_fg;
      backgroundColor = wc.menu_inactive_bg;
    }
    setColor (foregroundColor, backgroundColor);
    x++;
    print (vmenubar, ' ');

    txt = (*iter)->getText();
    txt_length = uInt(txt.getLength());
    item_text = new wchar_t[txt_length+1];
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
      if ( ! iswprint(wint_t(item_text[z])) )
        item_text[z] = L' ';
      if ( (z == hotkeypos) && is_Active && ! is_Selected )
      {
        setColor (wc.menu_hotkey_fg, wc.menu_hotkey_bg);
        if ( ! is_NoUnderline )
          setUnderline();
        print (vmenubar, item_text[z]);
        if ( ! is_NoUnderline )
          unsetUnderline();
        setColor (foregroundColor, backgroundColor);
      }
      else
        print (vmenubar, item_text[z]);
    }

    if ( x > screenWidth )
    {
      print (vmenubar, txt.left(uInt(int(txt_length)+screenWidth-x-1)));
      print (vmenubar, "..");
    }
    else
    {
      x++;
      print (vmenubar, ' ');
    }

    setColor (wc.menu_active_fg, wc.menu_active_bg);
    if ( is_Active && is_Selected )
      setReverse(false);
    delete[] item_text;

    ++iter;
  }
  for (; x <= screenWidth; x++)
    print (vmenubar, ' ');

  setUpdateVTerm(true);
}

//----------------------------------------------------------------------
void FMenuBar::adjustSize()
{
  xmin = ymin = 1;
  height = 1;
  xpos = 1;
  width = getColumnNumber();
  ypos = 1;
  FWidget::adjustSize();
}

// public methods of FMenuBar
//----------------------------------------------------------------------
void FMenuBar::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != LeftButton )
  {
    mouse_down = false;

    if ( ! itemlist.empty() )
    {
      std::vector<FMenuItem*>::const_iterator iter, end;
      iter = itemlist.begin();
      end = itemlist.end();

      while ( iter != end )
      {
        (*iter)->unsetSelected();
        if ( selectedMenuItem == *iter )
          selectedMenuItem = 0;
        ++iter;
      }
    }
    redraw();
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
//FMessageBox::info (this, "Info", FString().sprintf("local(%d,%d) global(%d,%d)", ev->getX(),ev->getY(),ev->getGlobalX(), ev->getGlobalY()));
// #include "fmessagebox.h"
    while ( iter != end )
    {
      int x1, x2;

      x1 = (*iter)->getX();
      x2 = (*iter)->getX() + (*iter)->getWidth() - 1;

      if (  mouse_x >= x1
         && mouse_x <= x2
         && mouse_y == 1 )
      {
        if ( (*iter)->isActivated() && ! (*iter)->isSelected() )
        {
          (*iter)->setSelected();
          selectedMenuItem = *iter;
          focus_changed = true;
        }
        if ( (*iter)->hasMenu() )
        {
           FMenu* menu = (*iter)->getMenu();
           if ( menu->hasSelectedListItem() )
           {
             menu->unselectItemInList();
             menu->redraw();
           }
        }
      }
      else
      {
        if ( mouse_y == 1
           && (*iter)->isActivated()
           && (*iter)->isSelected() )
        {
          (*iter)->unsetSelected();
          if ( selectedMenuItem == *iter )
            selectedMenuItem = 0;
          focus_changed = true;
        }
      }
      ++iter;
    }
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
           && (*iter)->isActivated()
           && (*iter)->isSelected() )
        {
          if ( (*iter)->hasMenu() )
          {
            FMenu* menu = (*iter)->getMenu();
            if ( ! menu->hasSelectedListItem() )
            {
              menu->selectFirstItemInList();
              menu->redraw();
            }
          }
          else
          {
            (*iter)->unsetSelected();
            if ( selectedMenuItem == *iter )
              selectedMenuItem = 0;
            redraw();
            (*iter)->processClicked();
          }
        }
        ++iter;
      }
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
        if ( (*iter)->isActivated() && ! (*iter)->isSelected() )
        {
          (*iter)->setSelected();
          selectedMenuItem = *iter;
          focus_changed = true;
        }
        if ( (*iter)->hasMenu() )
        {
           FMenu* menu = (*iter)->getMenu();
           if ( menu->hasSelectedListItem() )
           {
             menu->unselectItemInList();
             menu->redraw();
           }
        }
      }
      else
      {
        if ( getGeometryGlobal().contains(ev->getGlobalPos())
           && (*iter)->isActivated()
           && (*iter)->isSelected() )
        {
          (*iter)->unsetSelected();
          if ( selectedMenuItem == *iter )
            selectedMenuItem = 0;
          focus_changed = true;
        }
        else if ( hasSelectedMenuItem() && selectedMenuItem->hasMenu() )
        {
          FMenu* menu = selectedMenuItem->getMenu();
          const FRect& menu_geometry = menu->getGeometryGlobal();

          if ( menu_geometry.contains(ev->getGlobalPos()) )
          {
            const FPoint& g = ev->getGlobalPos();
            const FPoint& p = menu->globalToLocalPos(g);
            int b = ev->getButton();
            ev = new FMouseEvent (MouseMove_Event, p, g, b);
            setClickedWidget(menu);
            menu->onMouseDown(ev);
          }
        }
      }
      ++iter;
    }
    if ( focus_changed )
      redraw();
  }
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
void FMenuBar::setGeometry (int xx, int yy, int ww, int hh, bool adjust)
{
  int old_width = width;
  int old_height = height;
  FWidget::setGeometry (xx, yy, ww, hh, adjust);
  if ( vmenubar && (width != old_width || height != old_height) )
    resizeArea (vmenubar);
}

//----------------------------------------------------------------------
void FMenuBar::cb_item_activated (FWidget* widget, void*)
{
  FMenuItem* menuitem = static_cast<FMenuItem*>(widget);

  if ( menuitem->hasMenu() )
  {
    FMenu* menu = menuitem->getMenu();
    if ( ! menu->isVisible() )
    {
      menu->setVisible();
      menu->show();
      raiseWindow(menu);
      menu->redraw();
    }

    updateTerminal();
    flush_out();
  }
}

//----------------------------------------------------------------------
void FMenuBar::cb_item_deactivated (FWidget* widget, void*)
{
  FMenuItem* menuitem = static_cast<FMenuItem*>(widget);

  if ( menuitem->hasMenu() )
  {
    FMenu* menu = menuitem->getMenu();
    if ( menu->isVisible() )
      menu->hide();

    restoreVTerm (menu->getGeometryGlobalShadow());

    updateTerminal();
    flush_out();
  }
}
