// fmenubar.cpp
// class FMenuBar

#include "fmenubar.h"

//----------------------------------------------------------------------
// class FMenuBar
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FMenuBar::FMenuBar(FWidget* parent) : FWindow(parent)
{
  this->init();
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
  // initialize geometry values
  setGeometry (1, 1, getColumnNumber(), 1, false);
  getRootWidget()->setTopPadding(1, true);
  x = -1;
  setMenuBar(this);
  foregroundColor = wc.menu_active_fg;
  backgroundColor = wc.menu_active_bg;
  window_object  = true;
  mouse_down = false;
  ignore_padding = true;
  unsetFocusable();
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
  bool isActive;
  bool isSelected;
  bool isNoUnderline;
  std::vector<FMenuItem*>::const_iterator iter, end;
  int screenWidth;

  x = 1;
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

    isActive = (*iter)->isActivated();
    isSelected = (*iter)->isSelected();
    isNoUnderline = (((*iter)->getFlags() & NO_UNDERLINE) != 0);

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
    setColor (foregroundColor, backgroundColor);
    x++;
    print (vmenubar, ' ');

    txt = (*iter)->getText();
    txt_length = int(txt.getLength());
    item_text = new wchar_t[txt_length+1];
    src  = const_cast<wchar_t*>(txt.wc_str());
    dest = const_cast<wchar_t*>(item_text);

    if ( x-1 <= screenWidth )
      to_char = int(txt_length);
    else
      to_char = txt_length - (screenWidth-x-1);

    hotkeypos = getHotkeyPos (src, dest, txt_length);

    if ( hotkeypos != -1 )
    {
      txt_length--;
      to_char--;
    }
    x += txt_length;

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

    if ( x > screenWidth )
    {
      print ( vmenubar,
              txt.left(uInt(txt_length+screenWidth-x-1)) );
      print ( vmenubar, ".." );
    }
    else
    {
      x++;
      print (vmenubar, ' ');
    }

    if ( isActive && isSelected )
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
void FMenuBar::onMouseDown (FMouseEvent* event)
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
      if ( (*iter)->hasHotkey() )
        txt_length--;
      x2 = x1 + txt_length;
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
      else
      {
        (*iter)->unsetSelected();
        this->redraw();
      }
      X = x2 + 1;
      ++iter;
    }
  }
}

//----------------------------------------------------------------------
void FMenuBar::onMouseUp (FMouseEvent* event)
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
        if ( (*iter)->hasHotkey() )
          txt_length--;
        int x2 = x1 + txt_length;

        if ( (*iter)->isSelected() )
        {
          int mouse_x = event->getX();
          int mouse_y = event->getY();
          if ( mouse_x < x1 || mouse_x > x2 || mouse_y != 1 )
            (*iter)->unsetSelected();
          else
          {
            (*iter)->processClicked();
          }
          this->redraw();
        }
        X = x2 + 1;
        ++iter;
      }
    }
  }
}

//----------------------------------------------------------------------
void FMenuBar::onMouseMove (FMouseEvent* event)
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
      if ( (*iter)->hasHotkey() )
        txt_length--;
      int x2 = x1 + txt_length;

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
      X = x2 + 1;
      ++iter;
    }
    if ( focus_changed )
      this->redraw();
  }
}

//----------------------------------------------------------------------
void FMenuBar::hide()
{
  int fg, bg, screenWidth;
  char* blank;

  FWidget::hide();

  fg = wc.term_fg;
  bg = wc.term_bg;
  setColor (fg, bg);

  screenWidth = getColumnNumber();
  blank = new char[screenWidth+1];
  memset(blank, ' ', uLong(screenWidth));
  blank[screenWidth] = '\0';

  gotoxy (1, 1);
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
    beep();
  }
}
