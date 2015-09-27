// File: fmenu.cpp
// Provides: class FMenu

#include "fmenu.h"

//----------------------------------------------------------------------
// class FMenu
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FMenu::FMenu(FWidget* parent)
  : FWindow(parent)
  , item(0)
  , super_menu(0)
  , next_item_pos(1,1)
  , maxItemWidth(0)
  , current(0)
  , mouse_down(false)
{
  init();
}

//----------------------------------------------------------------------
FMenu::FMenu (FString& txt, FWidget* parent)
  : FWindow(parent)
  , item(0)
  , super_menu(0)
  , next_item_pos(1,1)
  , maxItemWidth(0)
  , current(0)
  , mouse_down(false)
{
  item = new FMenuItem(txt, parent);
  init();
}

//----------------------------------------------------------------------
FMenu::FMenu (const std::string& txt, FWidget* parent)
  : FWindow(parent)
  , item(0)
  , super_menu(0)
  , next_item_pos(1,1)
  , maxItemWidth(0)
  , current(0)
  , mouse_down(false)
{
  item = new FMenuItem(txt, parent);
  init();
}

//----------------------------------------------------------------------
FMenu::FMenu (const char* txt, FWidget* parent)
  : FWindow(parent)
  , item(0)
  , super_menu(0)
  , next_item_pos(1,1)
  , maxItemWidth(0)
  , current(0)
  , mouse_down(false)
{
  item = new FMenuItem(txt, parent);
  init();
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
void FMenu::init()
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
  window_object  = true;
  addWindow(this);

  foregroundColor = wc.menu_active_fg;
  backgroundColor = wc.menu_active_bg;

  FWidget* old_focus = FWidget::getFocusWidget();
  if ( old_focus )
  {
    setFocus();
    old_focus->redraw();
  }

  item->setMenu(this);
}

//----------------------------------------------------------------------
void FMenu::menu_dimension()
{
  std::vector<FMenuItem*>::const_iterator iter, end;
  iter = itemlist.begin();
  end = itemlist.end();
  maxItemWidth = 0;

  // find the max item width
  while ( iter != end )
  {
    FString item_text = (*iter)->getText();
    uInt len = item_text.getLength();

   if ( item_text.includes(L'&') )  // item has a hotkey '&'
     len--;

    if ( len > maxItemWidth )
      maxItemWidth = len;
    ++iter;
  }

  setGeometry (xpos, ypos, int(maxItemWidth + 4), int(count() + 2));
}

//----------------------------------------------------------------------
bool FMenu::isMenuBar (FWidget* w) const
{
  return bool ( strcmp ( w->getClassName()
                       , const_cast<char*>("FMenuBar") ) == 0 );
}

//----------------------------------------------------------------------
FMenuList* FMenu::superMenu() const
{
  return super_menu;
}

//----------------------------------------------------------------------
void FMenu::setSuperMenu (FMenuList* smenu)
{
  super_menu = smenu;
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
  if ( itemlist.empty() )
    return;

  if ( current < 1 )
  {
    current = 1;
    itemlist[0]->setSelected();
  }

  menu_dimension();

  // fill the background
  setColor (foregroundColor, backgroundColor);
  setUpdateVTerm(false);
  clrscr();
  drawBorder();

  drawItems();

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
    if ( (flags & SHADOW) == 0 )
    {
      gotoxy (x1, y2);
      // lower left corner border ⎣
      print (fc::NF_border_corner_lower_left);
      for (int x=1; x < width-1; x++) // low line _
        print (fc::NF_border_line_bottom);
      gotoxy (x2, y2);
      // lower right corner border ⎦
      print (fc::NF_rev_border_corner_lower_right);
    }
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
void FMenu::drawItems()
{
  std::vector<FMenuItem*>::const_iterator iter, end;
  int y = 0;

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
    bool is_Active = (*iter)->isActivated();
    bool is_Selected = (*iter)->isSelected();
    bool is_NoUnderline = (((*iter)->getFlags() & NO_UNDERLINE) != 0);

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
    gotoxy (xpos+xmin, ypos+ymin+y);
    setColor (foregroundColor, backgroundColor);
    print (' ');

    txt = (*iter)->getText();
    txt_length = uInt(txt.getLength());
    item_text = new wchar_t[txt_length+1];
    src  = const_cast<wchar_t*>(txt.wc_str());
    dest = const_cast<wchar_t*>(item_text);
    to_char = int(txt_length);
    hotkeypos = getHotkeyPos (src, dest, txt_length);

    if ( hotkeypos != -1 )
    {
      txt_length--;
      to_char--;
    }

    for (int z=0; z < to_char; z++)
    {
      if ( ! iswprint(wint_t(item_text[z])) )
        item_text[z] = L' ';
      if ( (z == hotkeypos) && is_Active && ! is_Selected )
      {
        setColor (wc.menu_hotkey_fg, wc.menu_hotkey_bg);
        if ( ! is_NoUnderline )
          setUnderline();
        print (item_text[z]);
        if ( ! is_NoUnderline )
          unsetUnderline();
        setColor (foregroundColor, backgroundColor);
      }
      else
        print (item_text[z]);
    }

    if ( is_Selected )
    {
      for (uInt i=uInt(to_char); i <= maxItemWidth; i++)
        print (' ');
    }

    if ( is_Active && is_Selected )
      setReverse(false);
    delete[] item_text;

    ++iter;
    y++;
  }
}

//----------------------------------------------------------------------
void FMenu::processActivate()
{
  emitCallback("activate");
}

// public methods of FMenu
//----------------------------------------------------------------------
void FMenu::onMouseDown (FMouseEvent* ev)
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
        ++iter;
      }
    }
    redraw();
    return;
  }
  if ( mouse_down )
    return;
  mouse_down = true;

  if ( ! itemlist.empty() )
  {
    std::vector<FMenuItem*>::const_iterator iter, end;
    int X = 1;

    iter = itemlist.begin();
    end = itemlist.end();

    while ( iter != end )
    {
      int x1, x2, mouse_x, mouse_y, txt_length;

      x1 = X;
      txt_length = int((*iter)->getText().getLength());
      x2 = x1 + txt_length + 1;
      mouse_x = ev->getX();
      mouse_y = ev->getY();

      if (  mouse_x >= x1
         && mouse_x <= x2
         && mouse_y == 1
         && ! (*iter)->isSelected() )
      {
        (*iter)->setSelected();
        redraw();
      }
      X = x2 + 2;
      ++iter;
    }
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
      int X = 1;

      iter = itemlist.begin();
      end = itemlist.end();

      while ( iter != end )
      {
        int x1 = X;
        int txt_length = int((*iter)->getText().getLength());
        int x2 = x1 + txt_length + 1;

        if ( (*iter)->isSelected() )
        {
          int mouse_x = ev->getX();
          int mouse_y = ev->getY();
          if ( mouse_x < x1 || mouse_x > x2 || mouse_y != 1 )
            (*iter)->unsetSelected();
          redraw();
        }
        X = x2 + 2;
        ++iter;
      }
    }
  }
}

//----------------------------------------------------------------------
void FMenu::onMouseMove (FMouseEvent* ev)
{
  if ( ev->getButton() != LeftButton )
    return;

  if ( mouse_down && ! itemlist.empty() )
  {
    std::vector<FMenuItem*>::const_iterator iter, end;
    bool focus_changed = false;
    int X=1;

    iter = itemlist.begin();
    end = itemlist.end();

    while ( iter != end )
    {
      int x1 = X;
      int txt_length = int((*iter)->getText().getLength());
      int x2 = x1 + txt_length + 1;

      int mouse_x = ev->getX();
      int mouse_y = ev->getY();
      if (  mouse_x >= x1
         && mouse_x <= x2
         && mouse_y == 1 )
      {
        if ( ! (*iter)->isSelected() )
        {
          (*iter)->setSelected();
          focus_changed = true;
        }
      }
      else
      {
        if ( (*iter)->isSelected() )
        {
          (*iter)->unsetSelected();
          focus_changed = true;
        }
      }
      X = x2 + 2;
      ++iter;
    }
    if ( focus_changed )
      redraw();
  }
}

//----------------------------------------------------------------------
void FMenu::hide()
{ }

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
void FMenu::insert (FMenuItem* i)
{
  FMenuList::insert(i);
}

//----------------------------------------------------------------------
void FMenu::remove (FMenuItem* i)
{
  FMenuList::remove(i);
}

//----------------------------------------------------------------------
void FMenu::cb_menuitem_activated (FWidget* widget, void*)
{
  FMenuItem* menuitem = static_cast<FMenuItem*>(widget);

  if ( menuitem->hasMenu() )
  {
    beep();
  }
}
