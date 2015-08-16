// fmenu.cpp
// class FMenu

#include "fmenu.h"

//----------------------------------------------------------------------
// class FMenu
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FMenu::FMenu(FWidget* parent) : FWindow(parent)
{
  item = 0;
  this->init();
}

//----------------------------------------------------------------------
FMenu::FMenu (FString& txt, FWidget* parent) : FWindow(parent)
{
  item = new FMenuItem(txt, parent);
  this->init();
}

//----------------------------------------------------------------------
FMenu::FMenu (const std::string& txt, FWidget* parent) : FWindow(parent)
{
  item = new FMenuItem(txt, parent);
  this->init();
}

//----------------------------------------------------------------------
FMenu::FMenu (const char* txt, FWidget* parent) : FWindow(parent)
{
  item = new FMenuItem(txt, parent);
  this->init();
}

//----------------------------------------------------------------------
FMenu::~FMenu()
{
}


// private methods of FMenu
//----------------------------------------------------------------------
void FMenu::init()
{
  item->setMenu(this);
  setGeometry (1,1,1,1);
}

//----------------------------------------------------------------------
bool FMenu::isMenuBar (FWidget* w) const
{
  return bool ( strcmp ( w->getClassName(),
                         const_cast<char*>("FMenuBar") ) == 0 );
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
  drawItems();
}

//----------------------------------------------------------------------
void FMenu::drawItems()
{
  std::vector<FMenuItem*>::const_iterator iter, end;
  int y = 1;

  if ( itemlist.empty() )
    return;

  setUpdateVTerm(false);

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
    bool isActive = (*iter)->isActivated();
    bool isSelected = (*iter)->isSelected();
    bool isNoUnderline = (((*iter)->getFlags() & NO_UNDERLINE) != 0);

    if ( isActive )
    {
      if ( isSelected )
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
    gotoxy (xpos+xmin+1, ypos+ymin+y);
    setColor (foregroundColor, backgroundColor);
    print (vmenubar, ' ');

    txt = (*iter)->getText();
    txt_length = int(txt.getLength());
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
      if ( (z == hotkeypos) && isActive && ! isSelected )
      {
        setColor (wc.menu_hotkey_fg, wc.menu_hotkey_bg);
        if ( ! isNoUnderline )
          setUnderline();
        print (vmenubar, item_text[z]);
        if ( ! isNoUnderline )
          unsetUnderline();
        setColor (foregroundColor, backgroundColor);
      }
      else
        print (vmenubar, item_text[z]);
    }

    if ( isActive && isSelected )
      setReverse(false);
    delete[] item_text;

    ++iter;
    y++;
  }
  setUpdateVTerm(true);
}

//----------------------------------------------------------------------
void FMenu::processActivate()
{
  emitCallback("activate");
}

// public methods of FMenu
//----------------------------------------------------------------------
void FMenu::onMouseDown (FMouseEvent* event)
{
  if ( event->getButton() != LeftButton )
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
    this->redraw();
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
      mouse_x = event->getX();
      mouse_y = event->getY();

      if (  mouse_x >= x1
         && mouse_x <= x2
         && mouse_y == 1
         && ! (*iter)->isSelected() )
      {
        (*iter)->setSelected();
        this->redraw();
      }
      X = x2 + 2;
      ++iter;
    }
  }
}

//----------------------------------------------------------------------
void FMenu::onMouseUp (FMouseEvent* event)
{
  if ( event->getButton() != LeftButton )
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
          int mouse_x = event->getX();
          int mouse_y = event->getY();
          if ( mouse_x < x1 || mouse_x > x2 || mouse_y != 1 )
            (*iter)->unsetSelected();
          this->redraw();
        }
        X = x2 + 2;
        ++iter;
      }
    }
  }
}

//----------------------------------------------------------------------
void FMenu::onMouseMove (FMouseEvent* event)
{
  if ( event->getButton() != LeftButton )
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

      int mouse_x = event->getX();
      int mouse_y = event->getY();
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
      this->redraw();
  }
}

//----------------------------------------------------------------------
void FMenu::hide()
{
}

//----------------------------------------------------------------------
void FMenu::setGeometry (int xx, int yy, int ww, int hh, bool adjust)
{
  int old_width = width;
  int old_height = height;
  FWidget::setGeometry (xx, yy, ww, hh, adjust);
  if ( vmenubar && (width != old_width || height != old_height) )
    resizeArea (vmenubar);
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
