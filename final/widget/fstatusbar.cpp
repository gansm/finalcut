/***********************************************************************
* fstatusbar.cpp - Widget FStatusBar and FStatusKey                    *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2022 Markus Gans                                      *
*                                                                      *
* FINAL CUT is free software; you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as       *
* published by the Free Software Foundation; either version 3 of       *
* the License, or (at your option) any later version.                  *
*                                                                      *
* FINAL CUT is distributed in the hope that it will be useful, but     *
* WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU Lesser General Public License for more details.                  *
*                                                                      *
* You should have received a copy of the GNU Lesser General Public     *
* License along with this program.  If not, see                        *
* <http://www.gnu.org/licenses/>.                                      *
***********************************************************************/

#include <vector>

#include "final/fevent.h"
#include "final/fwidgetcolors.h"
#include "final/widget/fstatusbar.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FStatusKey
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FStatusKey::FStatusKey(FWidget* parent)
  : FWidget{parent}
{
  init();
}

//----------------------------------------------------------------------
FStatusKey::FStatusKey (FKey k, FString&& txt, FWidget* parent)
  : FWidget{parent}
  , text{std::move(txt)}
  , key{k}
{
  init();
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
auto FStatusKey::setMouseFocus(bool enable) -> bool
{
  if ( mouse_focus == enable )
    return true;

  return (mouse_focus = enable);
}


// private methods of FStatusKey
//----------------------------------------------------------------------
void FStatusKey::init()
{
  setGeometry (FPoint{1, 1}, FSize{1, 1});
  FWidget* parent = getParentWidget();

  if ( parent && parent->isInstanceOf("FStatusBar") )
  {
    setConnectedStatusbar (static_cast<FStatusBar*>(parent));

    if ( getConnectedStatusbar() )
      getConnectedStatusbar()->insert(this);
  }
}

//----------------------------------------------------------------------
void FStatusKey::processActivate() const
{
  emitCallback("activate");
}


//----------------------------------------------------------------------
// class FStatusBar
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FStatusBar::FStatusBar(FWidget* parent)
  : FWindow{parent}
{
  init();
}

//----------------------------------------------------------------------
FStatusBar::~FStatusBar()  // destructor
{
  // delete all keys
  if ( ! key_list.empty() )
  {
    auto iter = key_list.cbegin();

    while ( iter != key_list.cend() )
    {
      (*iter)->setConnectedStatusbar(nullptr);
      FWidget::delAccelerator (*iter);
      iter = key_list.erase(iter);
    }
  }

  FWidget::setStatusBar(nullptr);
}


// public methods of FStatusBar
//----------------------------------------------------------------------
void FStatusBar::setMessage (const FString& mgs)
{
  text.setString(mgs);
}

//----------------------------------------------------------------------
void FStatusBar::resetColors()
{
  const auto& wc = getColorTheme();
  setForegroundColor (wc->statusbar_fg);
  setBackgroundColor (wc->statusbar_bg);
  FWidget::resetColors();
}

//----------------------------------------------------------------------
auto FStatusBar::hasActivatedKey() const -> bool
{
  if ( ! key_list.empty() )
  {
    for (auto&& k : key_list)
      if ( k->isActivated() )
        return true;
  }

  return false;
}

//----------------------------------------------------------------------
void FStatusBar::hide()
{
  const auto& wc = getColorTheme();
  const auto& fg = wc->term_fg;
  const auto& bg = wc->term_bg;
  setColor (fg, bg);
  print() << FPoint{1, 1} << FString{getDesktopWidth(), L' '};
  FWindow::hide();
}

//----------------------------------------------------------------------
void FStatusBar::drawMessage()
{
  if ( ! (isVisible() ) )
    return;

  if ( x < 0 || x_msg < 0 )
    return;

  x = x_msg;
  int space_offset{1};
  bool isLastActiveFocus{false};
  const bool hasKeys( ! key_list.empty() );
  const std::size_t termWidth = getDesktopWidth();

  if ( hasKeys )
  {
    const auto& iter = key_list.cend();
    isLastActiveFocus = bool ( (*(iter - 1))->isActivated()
                            || (*(iter - 1))->hasMouseFocus() );
  }
  else
    isLastActiveFocus = false;

  if ( isLastActiveFocus )
    space_offset = 0;

  const auto& wc = getColorTheme();
  setColor (wc->statusbar_fg, wc->statusbar_bg);
  setPrintPos ({x, 1});

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(true);

  if ( x + space_offset + 3 < int(termWidth) && text )
  {
    if ( ! isLastActiveFocus )
    {
      x++;
      print (' ');
    }

    if ( hasKeys )
    {
      x += 2;
      print (UniChar::BoxDrawingsVertical);  // │
      print (' ');
    }

    const auto msg_length = getColumnWidth(getMessage());
    x += int(msg_length);

    if ( x - 1 <= int(termWidth) )
      print (getMessage());
    else
    {
      // Print ellipsis
      const std::size_t len = msg_length + termWidth - uInt(x) - 1;
      print() << getColumnSubString ( getMessage(), 1, len)
              << "..";
    }
  }

  for (auto i = x; i <= int(termWidth); i++)
    print (' ');

  if ( FVTerm::getFOutput()->isMonochron() )
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
    this,
    &FStatusBar::cb_statuskey_activated,
    skey
  );
}

//----------------------------------------------------------------------
void FStatusBar::remove (FStatusKey* skey)
{
  delAccelerator (skey);

  if ( key_list.empty() )
    return;

  auto iter = key_list.cbegin();

  while ( iter != key_list.cend() )
  {
    if ( (*iter) == skey )
    {
      iter = key_list.erase(iter);
      skey->setConnectedStatusbar(nullptr);
      break;
    }

    ++iter;
  }
}

//----------------------------------------------------------------------
void FStatusBar::remove (int pos)
{
  if ( int(getCount()) < pos )
    return;

  key_list.erase (key_list.cbegin() + pos - 1);
}

//----------------------------------------------------------------------
void FStatusBar::clear()
{
  key_list.clear();
  key_list.shrink_to_fit();
}

//----------------------------------------------------------------------
void FStatusBar::adjustSize()
{
  setGeometry ( FPoint{1, int(getDesktopHeight())}
              , FSize{getDesktopWidth(), 1}, false );
}

//----------------------------------------------------------------------

void FStatusBar::onMouseDown (FMouseEvent* ev)
{
  if ( hasActivatedKey() )
    return;

  if ( ev->getButton() != MouseButton::Left )
  {
    mouse_down = false;

    if ( ! key_list.empty() )
    {
      auto iter = key_list.cbegin();
      const auto& last = key_list.cend();

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
    int X{1};
    auto iter = key_list.cbegin();
    const auto& last = key_list.cend();

    while ( iter != last )
    {
      const int x1 = X;
      const int kname_len = getKeyNameWidth(*iter);
      const int txt_length = getKeyTextWidth(*iter);
      const int x2 = x1 + kname_len + txt_length + 1;
      const int mouse_x = ev->getX();
      const int mouse_y = ev->getY();

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

  if ( ev->getButton() != MouseButton::Left )
    return;

  if ( mouse_down )
  {
    mouse_down = false;

    if ( ! key_list.empty() )
    {
      int X{1};
      auto iter = key_list.cbegin();
      const auto& last = key_list.cend();

      while ( iter != last )
      {
        const int x1 = X;
        const int kname_len = getKeyNameWidth(*iter);
        const int txt_length = getKeyTextWidth(*iter);
        const int x2 = x1 + kname_len + txt_length + 1;

        if ( (*iter)->hasMouseFocus() )
        {
          (*iter)->unsetMouseFocus();
          const int mouse_x = ev->getX();
          const int mouse_y = ev->getY();

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

  if ( ev->getButton() != MouseButton::Left )
    return;

  if ( mouse_down && ! key_list.empty() )
  {
    bool focus_changed{false};
    int X{1};
    auto iter = key_list.cbegin();
    const auto& last = key_list.cend();

    while ( iter != last )
    {
      const int x1 = X;
      const int kname_len = getKeyNameWidth(*iter);
      const int txt_length = getKeyTextWidth(*iter);
      const int x2 = x1 + kname_len + txt_length + 1;
      const int mouse_x = ev->getX();
      const int mouse_y = ev->getY();

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
void FStatusBar::cb_statuskey_activated (const FStatusKey* statuskey)
{
  if ( ! statuskey )
    return;

  if ( ! key_list.empty() )
  {
    auto iter = key_list.cbegin();
    const auto& last = key_list.cend();

    while ( iter != last )
    {
      if ( (*iter) != statuskey && (*iter)->isActivated() )
        (*iter)->unsetActive();
      ++iter;
    }
  }

  redraw();
}


// private methods of FStatusBar
//----------------------------------------------------------------------
void FStatusBar::init()
{
  const auto& r = getRootWidget();
  const std::size_t w = r->getWidth();
  const auto h = int(r->getHeight());
  // initialize geometry values
  setGeometry (FPoint{1, h}, FSize{w, 1}, false);
  setAlwaysOnTop();
  setStatusBar(this);
  ignorePadding();
  mouse_down = false;

  if ( getRootWidget() )
    getRootWidget()->setBottomPadding(1, true);

  FStatusBar::resetColors();
  unsetFocusable();
}

//----------------------------------------------------------------------
auto FStatusBar::getKeyNameWidth (const FStatusKey* key) const -> int
{
  const FString& key_name = FVTerm::getFOutput()->getKeyName(key->getKey());
  return int(getColumnWidth(key_name));
}

//----------------------------------------------------------------------
auto FStatusBar::getKeyTextWidth (const FStatusKey* key) const -> int
{
  const FString& key_text = key->getText();
  return int(getColumnWidth(key_text));
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
  screenWidth = getDesktopWidth();
  x = 1;

  if ( key_list.empty() )
  {
    x_msg = 1;
    return;
  }

  print() << FPoint{1, 1};

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(true);

  auto iter = key_list.cbegin();

  while ( iter != key_list.cend() )
  {
    const auto& item = *iter;
    keyname_len = getKeyNameWidth(item);

    if ( x + keyname_len + 2 < int(screenWidth) )
    {
      if ( item->isActivated() || item->hasMouseFocus() )
        drawActiveKey (iter);
      else
        drawKey (iter);
    }
    else
    {
      const auto& wc = getColorTheme();
      setColor (wc->statusbar_fg, wc->statusbar_bg);

      for (; x <= int(screenWidth); x++)
        print (' ');
    }

    ++iter;
  }

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);

  x_msg = x;
}

//----------------------------------------------------------------------
void FStatusBar::drawKey (FKeyList::const_iterator iter)
{
  // Draw not active key

  const auto& item = *iter;
  const auto& wc = getColorTheme();
  setColor (wc->statusbar_hotkey_fg, wc->statusbar_hotkey_bg);
  x++;
  print (' ');
  x += keyname_len;
  print (FVTerm::getFOutput()->getKeyName(item->getKey()));
  setColor (wc->statusbar_fg, wc->statusbar_bg);
  x++;
  print ('-');
  const auto column_width = getColumnWidth (item->getText());
  x += int(column_width);

  if ( x - 1 <= int(screenWidth) )
    print (item->getText());
  else
  {
    // Print ellipsis
    const std::size_t len = column_width + screenWidth - std::size_t(x) - 1;
    print() << getColumnSubString(item->getText(), 1, len)
            << "..";
  }

  if ( iter + 1 != key_list.cend()
    && ( (*(iter + 1))->isActivated() || (*(iter + 1))->hasMouseFocus() )
    && x + getKeyNameWidth(*(iter + 1)) + 3 < int(screenWidth) )
  {
    // Next element is active
    if ( FVTerm::getFOutput()->isMonochron() )
      setReverse(false);

    if ( FVTerm::getFOutput()->hasHalfBlockCharacter() )
    {
      setColor (wc->statusbar_bg, wc->statusbar_active_hotkey_bg);
      print (UniChar::LeftHalfBlock);  // ▐
    }
    else
      print (' ');

    x++;

    if ( FVTerm::getFOutput()->isMonochron() )
      setReverse(true);
  }
  else if ( iter + 1 != key_list.cend() && x < int(screenWidth) )
  {
    // Not the last element
    setColor (wc->statusbar_separator_fg, wc->statusbar_bg);
    x++;
    print (UniChar::BoxDrawingsVertical);  // │
  }
}

//----------------------------------------------------------------------
void FStatusBar::drawActiveKey (FKeyList::const_iterator iter)
{
  // Draw active key

  const auto& item = *iter;

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);

  const auto& wc = getColorTheme();
  setColor ( wc->statusbar_active_hotkey_fg
           , wc->statusbar_active_hotkey_bg );
  x++;
  print (' ');
  x += keyname_len;
  print (FVTerm::getFOutput()->getKeyName(item->getKey()));
  setColor (wc->statusbar_active_fg, wc->statusbar_active_bg);
  x++;
  print ('-');
  const auto column_width = getColumnWidth (item->getText());
  x += int(column_width);

  if ( x <= int(screenWidth) )
  {
    print (item->getText());
    x++;

    if ( FVTerm::getFOutput()->hasHalfBlockCharacter() )
    {
      setColor (wc->statusbar_bg, wc->statusbar_active_hotkey_bg);
      print (UniChar::RightHalfBlock);  // ▌
    }
    else
      print (' ');
  }
  else
  {
    // Print ellipsis
    std::size_t len = column_width + screenWidth - std::size_t(x) - 1;
    print() << getColumnSubString(item->getText(), 1, len)
            << "..";
  }

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(true);
}

}  // namespace finalcut
