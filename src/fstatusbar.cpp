/***********************************************************************
* fstatusbar.cpp - Widget FStatusBar and FStatusKey                    *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2014-2018 Markus Gans                                      *
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

#include "final/fstatusbar.h"

namespace finalcut
{

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
FStatusKey::FStatusKey (FKey k, const FString& txt, FWidget* parent)
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

  return (mouse_focus = on);
}


// private methods of FStatusKey
//----------------------------------------------------------------------
void FStatusKey::init (FWidget* parent)
{
  setGeometry (1, 1, 1, 1);

  if ( parent && parent->isInstanceOf("FStatusBar") )
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
  , screenWidth(80)
  , keyname_len(0)
  , x(-1)
  , x_msg(-1)
{
  init();
}

//----------------------------------------------------------------------
FStatusBar::~FStatusBar()  // destructor
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
void FStatusBar::setMessage (const FString& mgs)
{
  text = mgs;
}

//----------------------------------------------------------------------
bool FStatusBar::hasActivatedKey()
{
  if ( ! key_list.empty() )
  {
    std::vector<FStatusKey*>::const_iterator iter, last;
    iter = key_list.begin();
    last = key_list.end();

    while ( iter != last )
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
  FColor fg, bg;
  FWindow::hide();
  fg = wc.term_fg;
  bg = wc.term_bg;
  setColor (fg, bg);
  screenWidth = getDesktopWidth();
  char* blank = createBlankArray(screenWidth + 1);
  setPrintPos (1, 1);
  print (blank);
  destroyBlankArray (blank);
}

//----------------------------------------------------------------------
void FStatusBar::drawMessage()
{
  std::size_t termWidth;
  int space_offset;
  bool isLastActiveFocus, hasKeys;

  if ( ! (isVisible() ) )
    return;

  if ( x < 0 || x_msg < 0 )
    return;

  x = x_msg;
  termWidth = getDesktopWidth();
  space_offset = 1;
  hasKeys = bool(! key_list.empty());

  if ( hasKeys )
  {
    std::vector<FStatusKey*>::const_iterator iter = key_list.end();
    isLastActiveFocus = bool ( (*(iter - 1))->isActivated()
                            || (*(iter - 1))->hasMouseFocus() );
  }
  else
    isLastActiveFocus = false;

  if ( isLastActiveFocus )
    space_offset = 0;

  setColor (wc.statusbar_fg, wc.statusbar_bg);
  setPrintPos (x, 1);

  if ( isMonochron() )
    setReverse(true);

  if ( x + space_offset + 3 < int(termWidth) )
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

      std::size_t msg_length = getMessage().getLength();
      x += int(msg_length);

      if ( x - 1 <= int(termWidth) )
        print (getMessage());
      else
      {
        // Print ellipsis
        print ( getMessage().left(msg_length + termWidth - uInt(x) - 1) );
        print ("..");
      }
    }
  }

  for (int i = x; i <= int(termWidth); i++)
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
    F_METHOD_CALLBACK (this, &FStatusBar::cb_statuskey_activated)
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

  key_list.erase (key_list.begin() + pos - 1);
}

//----------------------------------------------------------------------
void FStatusBar::clear()
{
  key_list.clear();
}

//----------------------------------------------------------------------
void FStatusBar::adjustSize()
{
  setGeometry (1, int(getDesktopHeight()), getDesktopWidth(), 1, false);
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
      std::vector<FStatusKey*>::const_iterator iter, last;
      iter = key_list.begin();
      last = key_list.end();

      while ( iter != last )
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
    std::vector<FStatusKey*>::const_iterator iter, last;
    int X = 1;
    iter = key_list.begin();
    last = key_list.end();

    while ( iter != last )
    {
      int x1 = X
        , kname_len = int(getKeyName((*iter)->getKey()).getLength())
        , txt_length = int((*iter)->getText().getLength())
        , x2 = x1 + kname_len + txt_length + 1
        , mouse_x = ev->getX()
        , mouse_y = ev->getY();

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
      std::vector<FStatusKey*>::const_iterator iter, last;
      int X = 1;
      iter = key_list.begin();
      last = key_list.end();

      while ( iter != last )
      {
        int x1 = X
          , kname_len = int(getKeyName((*iter)->getKey()).getLength())
          , txt_length = int((*iter)->getText().getLength())
          , x2 = x1 + kname_len + txt_length + 1;

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
    std::vector<FStatusKey*>::const_iterator iter, last;
    bool focus_changed = false;
    int X = 1;
    iter = key_list.begin();
    last = key_list.end();

    while ( iter != last )
    {
      int x1 = X
        , kname_len = int(getKeyName((*iter)->getKey()).getLength())
        , txt_length = int((*iter)->getText().getLength())
        , x2 = x1 + kname_len + txt_length + 1
        , mouse_x = ev->getX()
        , mouse_y = ev->getY();

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
void FStatusBar::cb_statuskey_activated (FWidget* widget, data_ptr)
{
  if ( ! key_list.empty() )
  {
    std::vector<FStatusKey*>::const_iterator iter, last;
    FStatusKey* statuskey = static_cast<FStatusKey*>(widget);

    iter = key_list.begin();
    last = key_list.end();

    while ( iter != last )
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
  std::size_t w = r->getWidth();
  int h = int(r->getHeight());
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
  keyList::const_iterator iter, last;

  screenWidth = getDesktopWidth();
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
  last = key_list.end();

  while ( iter != last )
  {
    keyname_len = int(getKeyName((*iter)->getKey()).getLength());

    if ( x + keyname_len + 2 < int(screenWidth) )
    {
      if ( (*iter)->isActivated() || (*iter)->hasMouseFocus() )
        drawActiveKey (iter);
      else
        drawKey (iter);
    }
    else
    {
      setColor (wc.statusbar_fg, wc.statusbar_bg);

      for (; x <= int(screenWidth); x++)
        print (' ');
    }

    ++iter;
  }

  if ( isMonochron() )
    setReverse(false);

  x_msg = x;
}

//----------------------------------------------------------------------
void FStatusBar::drawKey (keyList::const_iterator iter)
{
  // Draw not active key

  std::size_t txt_length;
  FStatusKey* item = *iter;

  setColor (wc.statusbar_hotkey_fg, wc.statusbar_hotkey_bg);
  x++;
  print (' ');
  x += keyname_len;
  print (getKeyName(item->getKey()));
  setColor (wc.statusbar_fg, wc.statusbar_bg);
  x++;
  print ('-');
  txt_length = item->getText().getLength();
  x += int(txt_length);

  if ( x - 1 <= int(screenWidth) )
    print (item->getText());
  else
  {
    // Print ellipsis
    print ( item->getText()
                 .left(txt_length + screenWidth - std::size_t(x) - 1) );
    print ("..");
  }

  if ( iter + 1 != key_list.end()
    && ( (*(iter + 1))->isActivated() || (*(iter + 1))->hasMouseFocus() )
    && x + int(getKeyName((*(iter + 1))->getKey()).getLength()) + 3
     < int(screenWidth) )
  {
    // Next element is active
    if ( isMonochron() )
      setReverse(false);

    if ( hasHalfBlockCharacter() )
    {
      setColor (wc.statusbar_active_fg, wc.statusbar_active_bg);
      print (fc::LeftHalfBlock);  // ▐
    }
    else
      print (' ');

    x++;

    if ( isMonochron() )
      setReverse(true);
  }
  else if ( iter + 1 != key_list.end() && x < int(screenWidth) )
  {
    // Not the last element
    setColor (wc.statusbar_separator_fg, wc.statusbar_bg);
    x++;
    print (fc::BoxDrawingsVertical);  // │
  }
}

//----------------------------------------------------------------------
void FStatusBar::drawActiveKey (keyList::const_iterator iter)
{
  // Draw active key

  std::size_t txt_length;
  FStatusKey* item = *iter;

  if ( isMonochron() )
    setReverse(false);

  setColor ( wc.statusbar_active_hotkey_fg
           , wc.statusbar_active_hotkey_bg );
  x++;
  print (' ');
  x += keyname_len;
  print (getKeyName(item->getKey()));
  setColor (wc.statusbar_active_fg, wc.statusbar_active_bg);
  x++;
  print ('-');
  txt_length = item->getText().getLength();
  x += int(txt_length);

  if ( x <= int(screenWidth) )
  {
    print (item->getText());
    x++;
    print (fc::RightHalfBlock);  // ▌
  }
  else
  {
    // Print ellipsis
    print ( item->getText()
                 .left(txt_length + screenWidth - std::size_t(x) - 1) );
    print ("..");
  }

  if ( isMonochron() )
    setReverse(true);
}

}  // namespace finalcut
