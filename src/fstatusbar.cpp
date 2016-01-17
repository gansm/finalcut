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
  if ( statusbar() )
    statusbar()->remove(this);
  delAccelerator();
}


// private methods of FStatusKey
//----------------------------------------------------------------------
void FStatusKey::init (FWidget* parent)
{
  setGeometry (1,1,1,1);

  if ( parent && strcmp ( parent->getClassName()
                        , const_cast<char*>("FStatusBar") ) == 0 )
  {
    setStatusbar( static_cast<FStatusBar*>(parent) );
    statusbar()->insert(this);
  }
}

//----------------------------------------------------------------------
void FStatusKey::processActivate()
{
  emitCallback("activate");
}

//----------------------------------------------------------------------
void FStatusKey::onAccel (FAccelEvent* ev)
{
  if ( ! isActivated() )
  {
    setActive();
    statusbar()->redraw();
    ev->accept();
    // unset after get back from callback
    unsetActive();
    statusbar()->redraw();
  }
}

//----------------------------------------------------------------------
FStatusBar* FStatusKey::statusbar() const
{
  return bar;
}

//----------------------------------------------------------------------
void FStatusKey::setStatusbar (FStatusBar* sb)
{
  bar = sb;
}

// public methods of FStatusKey
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


//----------------------------------------------------------------------
// class FStatusBar
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FStatusBar::FStatusBar(FWidget* parent)
  : FWindow(parent)
  , keylist()
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
  if ( vstatusbar != 0 )
  {
    if ( vstatusbar->changes != 0 )
      delete[] vstatusbar->changes;
    if ( vstatusbar->text != 0 )
      delete[] vstatusbar->text;
    delete vstatusbar;
  }
  vstatusbar = 0;

  // delete all keys
  if ( ! keylist.empty() )
  {
    std::vector<FStatusKey*>::iterator iter;
    iter = keylist.begin();

    while ( iter != keylist.end() )
    {
      (*iter)->setStatusbar(0);
      delAccelerator (*iter);
      iter = keylist.erase(iter);
    }
  }
}


// private methods of FStatusBar
//----------------------------------------------------------------------
void FStatusBar::init()
{
  xmin = ymin = 1;
  xpos = 1;
  ypos = getLineNumber();
  createArea (vstatusbar);
  vstatusbar->visible = true;

  // initialize geometry values
  setGeometry (1, ypos, getColumnNumber(), 1, false);
  getRootWidget()->setBottomPadding(1, true);
  setStatusBar(this);
  foregroundColor = wc.statusbar_fg;
  backgroundColor = wc.statusbar_bg;
  window_object  = true;
  mouse_down = false;
  ignore_padding = true;
  unsetFocusable();
}

//----------------------------------------------------------------------
void FStatusBar::draw()
{
  xmin = ymin = 1;
  height = 1;
  xpos = 1;
  drawKeys();
  drawMessage();
}

//----------------------------------------------------------------------
void FStatusBar::drawKeys()
{
  std::vector<FStatusKey*>::const_iterator iter, end;
  int screenWidth, lastLine;

  x = 1;
  screenWidth = getColumnNumber();
  lastLine = getLineNumber();
  width = screenWidth;
  ypos = lastLine;

  if ( keylist.empty() )
  {
    x_msg = 1;
    return;
  }

  setUpdateVTerm(false);
  gotoxy (1, lastLine);

  if ( isMonochron() )
    setReverse(true);

  iter = keylist.begin();
  end = keylist.end();

  while ( iter != end )
  {
    int kname_len = int(getKeyName((*iter)->getKey()).getLength());
    if ( x+kname_len+2 < screenWidth )
    {
      if ( (*iter)->isActivated() || (*iter)->hasMouseFocus() )
      {
        if ( isMonochron() )
          setReverse(false);
        setColor ( wc.statusbar_active_hotkey_fg
                 , wc.statusbar_active_hotkey_bg );
        x++;
        print (vstatusbar, ' ');
        x += kname_len;
        print (vstatusbar, getKeyName((*iter)->getKey()));

        setColor (wc.statusbar_active_fg, wc.statusbar_active_bg);
        x++;
        print (vstatusbar, '-');

        int txt_length = int((*iter)->getText().getLength());
        x += txt_length;
        if ( x <= screenWidth )
        {
          print (vstatusbar, (*iter)->getText());
          x++;
          print (vstatusbar, fc::RightHalfBlock);  // ▌
        }
        else
        {
          print ( vstatusbar
                , (*iter)->getText()
                          .left(uInt(txt_length+screenWidth-x-1)) );
          print (vstatusbar, "..");
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
        print (vstatusbar, ' ');

        x += kname_len;
        print (vstatusbar, getKeyName((*iter)->getKey()));

        setColor (wc.statusbar_fg, wc.statusbar_bg);
        x++;
        print (vstatusbar, '-');

        txt_length = int((*iter)->getText().getLength());
        x += txt_length;
        if ( x-1 <= screenWidth )
          print (vstatusbar, (*iter)->getText());
        else
        {
          print ( vstatusbar
                , (*iter)->getText()
                          .left(uInt(txt_length+screenWidth-x-1)) );
          print ( vstatusbar, ".." );
        }
        if (  iter+1 != keylist.end()
           && ( (*(iter+1))->isActivated() || (*(iter+1))->hasMouseFocus() )
           && x + int(getKeyName((*(iter+1))->getKey()).getLength()) + 3
              < screenWidth )
        {
          // next element is active
          if ( isMonochron() )
            setReverse(false);
          setColor (wc.statusbar_active_fg, wc.statusbar_active_bg);
          x++;
          print (vstatusbar, fc::LeftHalfBlock);  // ▐
          if ( isMonochron() )
            setReverse(true);
        }
        else if ( iter+1 != keylist.end() && x < screenWidth )
        {
          // not the last element
          setColor (wc.statusbar_separator_fg, wc.statusbar_bg);
          x++;
          print (vstatusbar, fc::BoxDrawingsVertical);  // │
        }
      }
    }
    else
    {
      setColor (wc.statusbar_fg, wc.statusbar_bg);
      for (; x <= screenWidth; x++)
        print (vstatusbar, ' ');
    }
    ++iter;
  }
  if ( isMonochron() )
    setReverse(false);

  setUpdateVTerm(true);
  x_msg = x;
}

//----------------------------------------------------------------------
void FStatusBar::adjustSize()
{
  xmin = ymin = 1;
  height = 1;
  xpos = 1;
  width = getColumnNumber();
  ypos = getLineNumber();
  FWidget::adjustSize();
}


// public methods of FStatusBar
//----------------------------------------------------------------------
void FStatusBar::onMouseDown (FMouseEvent* ev)
{
  if ( hasActivatedKey() )
    return;
  if ( ev->getButton() != fc::LeftButton )
  {
    mouse_down = false;
    if ( ! keylist.empty() )
    {
      std::vector<FStatusKey*>::const_iterator iter, end;
      iter = keylist.begin();
      end = keylist.end();

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

  if ( ! keylist.empty() )
  {
    std::vector<FStatusKey*>::const_iterator iter, end;
    int X = 1;

    iter = keylist.begin();
    end = keylist.end();

    while ( iter != end )
    {
      int x1, x2, mouse_x, mouse_y, kname_len, txt_length;

      x1 = X;
      kname_len = int(getKeyName((*iter)->getKey()).getLength());
      txt_length = int((*iter)->getText().getLength());
      x2 = x1 + kname_len + txt_length + 1;
      mouse_x = ev->getX();
      mouse_y = ev->getY();

      if (  mouse_x >= x1
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
    if ( ! keylist.empty() )
    {
      std::vector<FStatusKey*>::const_iterator iter, end;
      int X = 1;

      iter = keylist.begin();
      end = keylist.end();

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

  if ( mouse_down && ! keylist.empty() )
  {
    std::vector<FStatusKey*>::const_iterator iter, end;
    bool focus_changed = false;
    int X=1;

    iter = keylist.begin();
    end = keylist.end();

    while ( iter != end )
    {
      int x1, x2, mouse_x, mouse_y, kname_len, txt_length;

      x1 = X;
      kname_len = int(getKeyName((*iter)->getKey()).getLength());
      txt_length = int((*iter)->getText().getLength());
      x2 = x1 + kname_len + txt_length + 1;

      mouse_x = ev->getX();
      mouse_y = ev->getY();
      if (  mouse_x >= x1
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
void FStatusBar::hide()
{
  int lastLine, screenWidth;
  short fg, bg;
  char* blank;

  FWindow::hide();

  fg = wc.term_fg;
  bg = wc.term_bg;
  setColor (fg, bg);

  lastLine = getLineNumber();
  screenWidth = getColumnNumber();
  blank = new char[screenWidth+1];
  memset(blank, ' ', uLong(screenWidth));
  blank[screenWidth] = '\0';

  gotoxy (1, lastLine);
  print (vstatusbar, blank);
  delete[] blank;
}

//----------------------------------------------------------------------
void FStatusBar::setGeometry (int xx, int yy, int ww, int hh, bool adjust)
{
  int old_width = width;
  int old_height = height;
  FWidget::setGeometry (xx, yy, ww, hh, adjust);
  if ( vstatusbar && (width != old_width || height != old_height) )
    resizeArea (vstatusbar);
}

//----------------------------------------------------------------------
bool FStatusBar::hasActivatedKey()
{
  if ( ! keylist.empty() )
  {
    std::vector<FStatusKey*>::const_iterator iter, end;
    iter = keylist.begin();
    end = keylist.end();

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
void FStatusBar::drawMessage()
{
  int  termWidth, lastLine, space_offset;
  bool isLastActiveFocus, hasKeys;

  if ( ! isVisible() )
    return;
  if ( x < 0 || x_msg < 0 )
    return;

  x = x_msg;
  termWidth = getColumnNumber();
  lastLine = getLineNumber();
  space_offset = 1;
  hasKeys = bool(! keylist.empty());

  if ( hasKeys )
  {
    std::vector<FStatusKey*>::const_iterator iter = keylist.end();
    isLastActiveFocus = bool(  (*(iter-1))->isActivated()
                            || (*(iter-1))->hasMouseFocus() );
  }
  else
    isLastActiveFocus = false;

  if ( isLastActiveFocus )
    space_offset = 0;

  setUpdateVTerm(false);
  setColor (wc.statusbar_fg, wc.statusbar_bg);
  gotoxy (x, lastLine);
  if ( isMonochron() )
    setReverse(true);
  if ( x+space_offset+3 < termWidth )
  {
    if ( text )
    {
      if ( ! isLastActiveFocus )
      {
        x++;
        print (vstatusbar, ' ');
      }
      if ( hasKeys )
      {
        x += 2;
        print (vstatusbar, fc::BoxDrawingsVertical);  // │
        print (vstatusbar, ' ');
      }

      int msg_length = int(getMessage().getLength());
      x += msg_length;
      if ( x-1 <= termWidth )
        print (vstatusbar, getMessage());
      else
      {
        print ( vstatusbar
              , getMessage().left(uInt(msg_length+termWidth-x-1)) );
        print (vstatusbar, "..");
      }
    }
  }
  for (int i=x; i <= termWidth; i++)
    print (vstatusbar, ' ');

  if ( isMonochron() )
    setReverse(false);

  setUpdateVTerm(true);
}

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
void FStatusBar::insert (FStatusKey* skey)
{
  keylist.push_back (skey);

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

  if ( keylist.empty() )
    return;

  iter = keylist.begin();
  while ( iter != keylist.end() )
  {
    if ( (*iter) == skey )
    {
      iter = keylist.erase(iter);
      skey->setStatusbar(0);
      break;
    }
    else
      ++iter;
  }
}

//----------------------------------------------------------------------
void FStatusBar::remove (int pos)
{
  if ( int(count()) < pos )
    return;

  keylist.erase (keylist.begin()+pos-1);
}

//----------------------------------------------------------------------
void FStatusBar::clear()
{
  keylist.clear();
}

//----------------------------------------------------------------------
void FStatusBar::cb_statuskey_activated (FWidget* widget, void*)
{
  if ( ! keylist.empty() )
  {
    std::vector<FStatusKey*>::const_iterator iter, end;
    FStatusKey* statuskey = static_cast<FStatusKey*>(widget);

    iter = keylist.begin();
    end = keylist.end();

    while ( iter != end )
    {
      if ( (*iter) != statuskey && (*iter)->isActivated() )
        (*iter)->unsetActive();
      ++iter;
    }
  }
  if ( isVisible() && isShown() )
    redraw();
  if ( ! isHiddenCursor() )
    hideCursor();
}
