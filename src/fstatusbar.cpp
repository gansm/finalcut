// File: fstatusbar.cpp
// Provides: class FStatusKey
//           class FStatusBar

#include "fstatusbar.h"

//----------------------------------------------------------------------
// class FStatusKey
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FStatusKey::FStatusKey(FWidget* parent)
  : FWidget(parent)
  , key(0)
  , text()
  , active(false)
  , mouse_focus(false)
  , bar(0)
{
  init (parent);
}

//----------------------------------------------------------------------
FStatusKey::FStatusKey (int k, FString& txt, FWidget* parent)
  : FWidget(parent)
  , key(k)
  , text(txt)
  , active(false)
  , mouse_focus(false)
  , bar(0)
{
  init (parent);
}

//----------------------------------------------------------------------
FStatusKey::FStatusKey (int k, const std::string& txt, FWidget* parent)
  : FWidget(parent)
  , key(k)
  , text(txt)
  , active(false)
  , mouse_focus(false)
  , bar(0)
{
  init (parent);
}

//----------------------------------------------------------------------
FStatusKey::FStatusKey (int k, const char* txt, FWidget* parent)
  : FWidget(parent)
  , key(k)
  , text(txt)
  , active(false)
  , mouse_focus(false)
  , bar(0)
{
  init (parent);
}

//----------------------------------------------------------------------
FStatusKey::~FStatusKey()  // destructor
{
  if ( getConnectedStatusbar() )
    getConnectedStatusbar()->remove(this);

  delAccelerator();
}


// public methods of FStatusKey
//----------------------------------------------------------------------
void FStatusKey::onAccel (FAccelEvent* ev)
{
  if ( isActivated() )
    return;

  setActive();

  if ( getConnectedStatusbar() )
    getConnectedStatusbar()->redraw();

  ev->accept();
  // unset after get back from callback
  unsetActive();

  if ( getConnectedStatusbar() )
    getConnectedStatusbar()->redraw();
}

//----------------------------------------------------------------------
void FStatusKey::setActive()
{
  active = true;
  processActivate();
}

//----------------------------------------------------------------------
bool FStatusKey::setMouseFocus(bool on)
{
  if ( on == mouse_focus )
    return true;

  return mouse_focus = (on) ? true : false;
}


// private methods of FStatusKey
//----------------------------------------------------------------------
void FStatusKey::init (FWidget* parent)
{
  setGeometry (1,1,1,1);

  if ( parent && std::strcmp ( parent->getClassName()
                             , const_cast<char*>("FStatusBar") ) == 0 )
  {
    setConnectedStatusbar (static_cast<FStatusBar*>(parent));

    if ( getConnectedStatusbar() )
      getConnectedStatusbar()->insert(this);
  }
}

//----------------------------------------------------------------------
void FStatusKey::processActivate()
{
  emitCallback("activate");
}


//----------------------------------------------------------------------
// class FStatusBar
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FStatusBar::FStatusBar(FWidget* parent)
  : FWindow(parent)
  , key_list()
  , text("")
  , mouse_down()
  , x(-1)
  , x_msg(-1)
{
  init();
}

//----------------------------------------------------------------------
FStatusBar::~FStatusBar()
{
  // delete all keys
  if ( ! key_list.empty() )
  {
    std::vector<FStatusKey*>::iterator iter;
    iter = key_list.begin();

    while ( iter != key_list.end() )
    {
      (*iter)->setConnectedStatusbar(0);
      delAccelerator (*iter);
      iter = key_list.erase(iter);
    }
  }

  setStatusBar(0);
}


// public methods of FStatusBar
//----------------------------------------------------------------------
void FStatusBar::setMessage (FString& mgs)
{
  text = mgs;
}

//----------------------------------------------------------------------
void FStatusBar::setMessage (const std::string& mgs)
{
  FString s = FString(mgs);
  setMessage (s);
}

//----------------------------------------------------------------------
void FStatusBar::setMessage (const char* mgs)
{
  FString s = FString(mgs);
  setMessage (s);
}

//----------------------------------------------------------------------
bool FStatusBar::hasActivatedKey()
{
  if ( ! key_list.empty() )
  {
    std::vector<FStatusKey*>::const_iterator iter, end;
    iter = key_list.begin();
    end = key_list.end();

    while ( iter != end )
    {
      if ( (*iter)->isActivated() )
        return true;

      ++iter;
    }
  }

  return false;
}

//----------------------------------------------------------------------
void FStatusBar::hide()
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

  blank = new char[screenWidth+1];
  std::memset(blank, ' ', uLong(screenWidth));
  blank[screenWidth] = '\0';
  setPrintPos (1, 1);
  print (blank);
  delete[] blank;
}
//----------------------------------------------------------------------
void FStatusBar::drawMessage()
{
  int  termWidth, space_offset;
  bool isLastActiveFocus, hasKeys;

  if ( ! (isVisible() ) )
    return;

  if ( x < 0 || x_msg < 0 )
    return;

  x = x_msg;
  termWidth = getColumnNumber();
  space_offset = 1;
  hasKeys = bool(! key_list.empty());

  if ( hasKeys )
  {
    std::vector<FStatusKey*>::const_iterator iter = key_list.end();
    isLastActiveFocus = bool(  (*(iter-1))->isActivated()
                            || (*(iter-1))->hasMouseFocus() );
  }
  else
    isLastActiveFocus = false;

  if ( isLastActiveFocus )
    space_offset = 0;

  setColor (wc.statusbar_fg, wc.statusbar_bg);
  setPrintPos (x, 1);

  if ( isMonochron() )
    setReverse(true);

  if ( x+space_offset+3 < termWidth )
  {
    if ( text )
    {
      if ( ! isLastActiveFocus )
      {
        x++;
        print (' ');
      }

      if ( hasKeys )
      {
        x += 2;
        print (fc::BoxDrawingsVertical);  // │
        print (' ');
      }

      int msg_length = int(getMessage().getLength());
      x += msg_length;

      if ( x-1 <= termWidth )
        print (getMessage());
      else
      {
        print ( getMessage().left(uInt(msg_length+termWidth-x-1)) );
        print ("..");
      }
    }
  }

  for (int i=x; i <= termWidth; i++)
    print (' ');

  if ( isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
void FStatusBar::insert (FStatusKey* skey)
{
  key_list.push_back (skey);

  addAccelerator (skey->getKey(), skey);

  skey->addCallback
  (
    "activate",
    _METHOD_CALLBACK (this, &FStatusBar::cb_statuskey_activated)
  );
}

//----------------------------------------------------------------------
void FStatusBar::remove (FStatusKey* skey)
{
  std::vector<FStatusKey*>::iterator iter;

  delAccelerator (skey);

  if ( key_list.empty() )
    return;

  iter = key_list.begin();

  while ( iter != key_list.end() )
  {
    if ( (*iter) == skey )
    {
      iter = key_list.erase(iter);
      skey->setConnectedStatusbar(0);
      break;
    }
    else
      ++iter;
  }
}

//----------------------------------------------------------------------
void FStatusBar::remove (int pos)
{
  if ( int(getCount()) < pos )
    return;

  key_list.erase (key_list.begin()+pos-1);
}

//----------------------------------------------------------------------
void FStatusBar::clear()
{
  key_list.clear();
}

//----------------------------------------------------------------------
void FStatusBar::adjustSize()
{
  setGeometry (1, getLineNumber(), getColumnNumber(), 1, false);
}

//----------------------------------------------------------------------

void FStatusBar::onMouseDown (FMouseEvent* ev)
{
  if ( hasActivatedKey() )
    return;

  if ( ev->getButton() != fc::LeftButton )
  {
    mouse_down = false;

    if ( ! key_list.empty() )
    {
      std::vector<FStatusKey*>::const_iterator iter, end;
      iter = key_list.begin();
      end = key_list.end();

      while ( iter != end )
      {
        (*iter)->unsetMouseFocus();
        ++iter;
      }
    }

    redraw();
    return;
  }

  if ( mouse_down )
    return;

  mouse_down = true;

  if ( ! key_list.empty() )
  {
    std::vector<FStatusKey*>::const_iterator iter, end;
    int X = 1;
    iter = key_list.begin();
    end = key_list.end();

    while ( iter != end )
    {
      int x1, x2, mouse_x, mouse_y, kname_len, txt_length;

      x1 = X;
      kname_len = int(getKeyName((*iter)->getKey()).getLength());
      txt_length = int((*iter)->getText().getLength());
      x2 = x1 + kname_len + txt_length + 1;
      mouse_x = ev->getX();
      mouse_y = ev->getY();

      if ( mouse_x >= x1
          && mouse_x <= x2
          && mouse_y == 1
          && ! (*iter)->hasMouseFocus() )
      {
        (*iter)->setMouseFocus();
        redraw();
      }

      X = x2 + 2;
      ++iter;
    }
  }
}

//----------------------------------------------------------------------
void FStatusBar::onMouseUp (FMouseEvent* ev)
{
  if ( hasActivatedKey() )
    return;

  if ( ev->getButton() != fc::LeftButton )
    return;

  if ( mouse_down )
  {
    mouse_down = false;

    if ( ! key_list.empty() )
    {
      std::vector<FStatusKey*>::const_iterator iter, end;
      int X = 1;
      iter = key_list.begin();
      end = key_list.end();

      while ( iter != end )
      {
        int x1, x2, kname_len, txt_length;
        x1 = X;
        kname_len = int(getKeyName((*iter)->getKey()).getLength());
        txt_length = int((*iter)->getText().getLength());
        x2 = x1 + kname_len + txt_length + 1;

        if ( (*iter)->hasMouseFocus() )
        {
          int mouse_x, mouse_y;
          (*iter)->unsetMouseFocus();
          mouse_x = ev->getX();
          mouse_y = ev->getY();

          if ( mouse_x >= x1 && mouse_x <= x2 && mouse_y == 1 )
            (*iter)->setActive();

          // unset after get back from callback
          (*iter)->unsetActive();
          redraw();
        }

        X = x2 + 2;
        ++iter;
      }
    }
  }
}

//----------------------------------------------------------------------
void FStatusBar::onMouseMove (FMouseEvent* ev)
{
  if ( hasActivatedKey() )
    return;

  if ( ev->getButton() != fc::LeftButton )
    return;

  if ( mouse_down && ! key_list.empty() )
  {
    std::vector<FStatusKey*>::const_iterator iter, end;
    bool focus_changed = false;
    int X=1;
    iter = key_list.begin();
    end = key_list.end();

    while ( iter != end )
    {
      int x1, x2, mouse_x, mouse_y, kname_len, txt_length;
      x1 = X;
      kname_len = int(getKeyName((*iter)->getKey()).getLength());
      txt_length = int((*iter)->getText().getLength());
      x2 = x1 + kname_len + txt_length + 1;
      mouse_x = ev->getX();
      mouse_y = ev->getY();

      if ( mouse_x >= x1
          && mouse_x <= x2
          && mouse_y == 1 )
      {
        if ( ! (*iter)->hasMouseFocus() )
        {
          (*iter)->setMouseFocus();
          focus_changed = true;
        }
      }
      else
      {
        if ( (*iter)->hasMouseFocus() )
        {
          (*iter)->unsetMouseFocus();
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
void FStatusBar::cb_statuskey_activated (FWidget* widget, void*)
{
  if ( ! key_list.empty() )
  {
    std::vector<FStatusKey*>::const_iterator iter, end;
    FStatusKey* statuskey = static_cast<FStatusKey*>(widget);

    iter = key_list.begin();
    end = key_list.end();

    while ( iter != end )
    {
      if ( (*iter) != statuskey && (*iter)->isActivated() )
        (*iter)->unsetActive();
      ++iter;
    }
  }

  if ( isVisible() && isShown() )
    redraw();
}


// private methods of FStatusBar
//----------------------------------------------------------------------
void FStatusBar::init()
{
  FWidget* r = getRootWidget();
  int w = r->getWidth();
  int h = r->getHeight();
  // initialize geometry values
  setGeometry (1, h, w, 1, false);
  setAlwaysOnTop();
  setStatusBar(this);
  ignorePadding();
  mouse_down = false;

  if ( getRootWidget() )
    getRootWidget()->setBottomPadding(1, true);

  setForegroundColor (wc.statusbar_fg);
  setBackgroundColor (wc.statusbar_bg);
  unsetFocusable();
}

//----------------------------------------------------------------------
void FStatusBar::draw()
{
  drawKeys();
  drawMessage();
}

//----------------------------------------------------------------------
void FStatusBar::drawKeys()
{
  std::vector<FStatusKey*>::const_iterator iter, end;
  int screenWidth;

  screenWidth = getColumnNumber();
  x = 1;

  if ( key_list.empty() )
  {
    x_msg = 1;
    return;
  }

  setPrintPos (1, 1);

  if ( isMonochron() )
    setReverse(true);

  iter = key_list.begin();
  end = key_list.end();

  while ( iter != end )
  {
    int kname_len = int(getKeyName((*iter)->getKey()).getLength());

    if ( x+kname_len+2 < screenWidth )
    {
      if ( (*iter)->isActivated() || (*iter)->hasMouseFocus() )
      {
        int txt_length;
        if ( isMonochron() )
          setReverse(false);

        setColor ( wc.statusbar_active_hotkey_fg
                 , wc.statusbar_active_hotkey_bg );
        x++;
        print (' ');
        x += kname_len;
        print (getKeyName((*iter)->getKey()));
        setColor (wc.statusbar_active_fg, wc.statusbar_active_bg);
        x++;
        print ('-');
        txt_length = int((*iter)->getText().getLength());
        x += txt_length;

        if ( x <= screenWidth )
        {
          print ((*iter)->getText());
          x++;
          print (fc::RightHalfBlock);  // ▌
        }
        else
        {
          print ( (*iter)->getText()
                          .left(uInt(txt_length+screenWidth-x-1)) );
          print ("..");
        }

        if ( isMonochron() )
          setReverse(true);
      }
      else
      {
        int txt_length;

        // not active
        setColor (wc.statusbar_hotkey_fg, wc.statusbar_hotkey_bg);
        x++;
        print (' ');
        x += kname_len;
        print (getKeyName((*iter)->getKey()));
        setColor (wc.statusbar_fg, wc.statusbar_bg);
        x++;
        print ('-');
        txt_length = int((*iter)->getText().getLength());
        x += txt_length;

        if ( x-1 <= screenWidth )
          print ((*iter)->getText());
        else
        {
          print ( (*iter)->getText()
                          .left(uInt(txt_length+screenWidth-x-1)) );
          print ("..");
        }

        if ( iter+1 != key_list.end()
            && ( (*(iter+1))->isActivated() || (*(iter+1))->hasMouseFocus() )
            && x + int(getKeyName((*(iter+1))->getKey()).getLength()) + 3
             < screenWidth )
        {
          // next element is active
          if ( isMonochron() )
            setReverse(false);

          if ( no_half_block_character )
            print (' ');
          else
          {
            setColor (wc.statusbar_active_fg, wc.statusbar_active_bg);
            print (fc::LeftHalfBlock);  // ▐
          }

          x++;

          if ( isMonochron() )
            setReverse(true);
        }
        else if ( iter+1 != key_list.end() && x < screenWidth )
        {
          // not the last element
          setColor (wc.statusbar_separator_fg, wc.statusbar_bg);
          x++;
          print (fc::BoxDrawingsVertical);  // │
        }
      }
    }
    else
    {
      setColor (wc.statusbar_fg, wc.statusbar_bg);

      for (; x <= screenWidth; x++)
        print (' ');
    }
    ++iter;
  }
  if ( isMonochron() )
    setReverse(false);

  x_msg = x;
}
