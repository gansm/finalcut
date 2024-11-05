/***********************************************************************
* fstatusbar.cpp - Widget FStatusBar and FStatusKey                    *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2024 Markus Gans                                      *
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

#include <utility>
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

  FStatusKey::delAccelerator(this);
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
void FStatusKey::setMouseFocus (bool enable)
{
  if ( mouse_focus == enable )
    return;

  mouse_focus = enable;
}


// private methods of FStatusKey
//----------------------------------------------------------------------
void FStatusKey::init()
{
  FWidget::setGeometry (FPoint{1, 1}, FSize{1, 1});
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
      FStatusBar::delAccelerator (*iter);
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
  setStatusBarColor();
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
  const auto& wc_term = getColorTheme()->term;
  const auto& fg = wc_term.fg;
  const auto& bg = wc_term.bg;
  setColor (fg, bg);
  print() << FPoint{1, 1} << FString{getDesktopWidth(), L' '};
  FWindow::hide();
}

//----------------------------------------------------------------------
void FStatusBar::drawMessage()
{
  if ( ! canDrawMessage() )
    return;

  x = x_msg;
  setStatusBarColor();
  setPrintPos ({x, 1});
  auto is_last_active_focus = isLastActiveFocus();
  int space_offset = is_last_active_focus ? 0 : 1;
  const std::size_t term_width = getDesktopWidth();

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(true);

  if ( x + space_offset + 3 < int(term_width) && text )
  {
    if ( ! is_last_active_focus )
    {
      x++;
      print (' ');
    }

    if ( ! key_list.empty() )
    {
      // Print key and message separators
      x += 2;
      print (UniChar::BoxDrawingsVertical);  // │
      print (' ');
    }

    printMessageWithEllipsis (term_width);
  }

  printPaddingSpaces (int(term_width));

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
    nonLeftButtonClick();
    return;
  }

  if ( mouse_down )
    return;

  leftButtonClick(ev);
}

//----------------------------------------------------------------------
void FStatusBar::onMouseUp (FMouseEvent* ev)
{
  if ( hasActivatedKey()
    || ev->getButton() != MouseButton::Left
    || mouse_down == false
    || key_list.empty() )
    return;

  mouse_down = false;
  int X{1};

  for (const auto& key : key_list)
  {
    const int x1 = X;
    const int kname_len = getKeyNameWidth(key);
    const int txt_length = getKeyTextWidth(key);
    const int x2 = x1 + kname_len + txt_length + 1;

    if ( key->hasMouseFocus() )
    {
      key->unsetMouseFocus();

      if ( isClickInsideRange(ev, x1, x2) )
        key->setActive();

      // unset after get back from callback
      key->unsetActive();
      redraw();
    }

    X = x2 + 2;
  }
}

//----------------------------------------------------------------------
void FStatusBar::onMouseMove (FMouseEvent* ev)
{
  if ( hasActivatedKey()
    || ev->getButton() != MouseButton::Left
    || mouse_down == false
    || key_list.empty() )
    return;

  bool focus_changed{false};
  int X{1};

  for (const auto& key : key_list)
  {
    const int x1 = X;
    const int kname_len = getKeyNameWidth(key);
    const int txt_length = getKeyTextWidth(key);
    const int x2 = x1 + kname_len + txt_length + 1;

    if ( isClickInsideRange(ev, x1, x2) )
    {
      if ( ! key->hasMouseFocus() )
      {
        key->setMouseFocus();
        focus_changed = true;
      }
    }
    else if ( key->hasMouseFocus() )
    {
      key->unsetMouseFocus();
      focus_changed = true;
    }

    X = x2 + 2;
  }

  if ( focus_changed )
    redraw();
}

//----------------------------------------------------------------------
void FStatusBar::cb_statuskey_activated (const FStatusKey* statuskey)
{
  if ( ! statuskey )
    return;

  if ( ! key_list.empty() )
  {
    for (const auto& key : key_list)
    {
      if ( key != statuskey && key->isActivated() )
        key->unsetActive();
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
  FWindow::setGeometry (FPoint{1, h}, FSize{w, 1}, false);
  setAlwaysOnTop();
  FWidget::setStatusBar(this);
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
      setStatusBarColor();

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

  const auto& wc_statusbar = getColorTheme()->statusbar;
  const FColorPair key_color{wc_statusbar.fg, wc_statusbar.bg};
  setColor (wc_statusbar.hotkey_fg, wc_statusbar.hotkey_bg);  // Hotkey color
  drawHotKeyName(iter, key_color);
  const auto& item = *iter;
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

  drawKeySeparator(iter);
}

//----------------------------------------------------------------------
void FStatusBar::drawKeySeparator (FKeyList::const_iterator iter)
{
  const auto& wc_statusbar = getColorTheme()->statusbar;

  if ( canPrintLeftActiveKeySeparator(iter) )
  {
    // Next element is active
    if ( FVTerm::getFOutput()->isMonochron() )
      setReverse(false);

    if ( FVTerm::getFOutput()->hasHalfBlockCharacter() )
    {
      setColor (wc_statusbar.bg, wc_statusbar.focus_hotkey_bg);
      print (UniChar::LeftHalfBlock);  // ▐
    }
    else
      print (' ');

    x++;

    if ( FVTerm::getFOutput()->isMonochron() )
      setReverse(true);
  }
  else if ( canPrintKeySeparator(iter) )
  {
    // Not the last element
    setColor (wc_statusbar.separator_fg, wc_statusbar.bg);
    x++;
    print (UniChar::BoxDrawingsVertical);  // │
  }
}

//----------------------------------------------------------------------
void FStatusBar::drawActiveKey (FKeyList::const_iterator iter)
{
  // Draw active key

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);

  const auto& wc_statusbar = getColorTheme()->statusbar;
  const FColorPair active_key_color (wc_statusbar.focus_hotkey_fg, wc_statusbar.focus_hotkey_bg);
  setColor (wc_statusbar.focus_hotkey_fg, wc_statusbar.focus_hotkey_bg);  // Hotkey color
  drawHotKeyName(iter, active_key_color);
  const auto& item = *iter;
  const auto column_width = getColumnWidth (item->getText());
  x += int(column_width);

  if ( x <= int(screenWidth) )
  {
    print (item->getText());
    x++;

    if ( FVTerm::getFOutput()->hasHalfBlockCharacter() )
    {
      // Print right active key separator
      setColor (wc_statusbar.bg, wc_statusbar.focus_hotkey_bg);
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

//----------------------------------------------------------------------
inline void FStatusBar::drawHotKeyName ( FKeyList::const_iterator iter
                                       , const FColorPair color )
{
  x++;
  print (' ');
  x += keyname_len;
  const auto& item = *iter;
  print (FVTerm::getFOutput()->getKeyName(item->getKey()));
  setColor(color);
  x++;
  print ('-');
}

//----------------------------------------------------------------------
inline auto FStatusBar::canPrintLeftActiveKeySeparator (FKeyList::const_iterator iter) const -> bool
{
  const auto next_iter = iter + 1;
  const auto next_key = *next_iter;
  return next_iter != key_list.cend()
      && ( next_key->isActivated() || next_key->hasMouseFocus() )
      && x + getKeyNameWidth(next_key) + 3 < int(screenWidth);
}

//----------------------------------------------------------------------
inline auto FStatusBar::canPrintKeySeparator (FKeyList::const_iterator iter) const -> bool
{
  const auto next_iter = iter + 1;
  return next_iter != key_list.cend()
      && x < int(screenWidth);
}

//----------------------------------------------------------------------
inline auto FStatusBar::canDrawMessage() const -> bool
{
  return isVisible() && x >= 0 && x_msg >= 0;
}

//----------------------------------------------------------------------
auto FStatusBar::isLastActiveFocus() const -> bool
{
  if ( key_list.empty() )
    return false;

  const auto& iter = key_list.cend();
  return (*(iter - 1))->isActivated()
      || (*(iter - 1))->hasMouseFocus();
}

//----------------------------------------------------------------------
inline auto FStatusBar::isClickInsideRange ( const FMouseEvent* ev
                                           , const int x1
                                           , const int x2 ) const -> bool
{
  const int mouse_x = ev->getX();
  const int mouse_y = ev->getY();

  return mouse_x >= x1
      && mouse_x <= x2
      && mouse_y == 1;
}

//----------------------------------------------------------------------
void FStatusBar::setStatusBarColor() const
{
  const auto& wc_statusbar = getColorTheme()->statusbar;
  setColor (wc_statusbar.fg, wc_statusbar.bg);
}

//----------------------------------------------------------------------
inline void FStatusBar::nonLeftButtonClick()
{
  mouse_down = false;

  if ( key_list.empty() )
    return;

  for (const auto& key : key_list)
    key->unsetMouseFocus();

  redraw();
}

//----------------------------------------------------------------------
inline void FStatusBar::leftButtonClick (const FMouseEvent* ev)
{
  mouse_down = true;

  if ( key_list.empty() )
    return;

  int X{1};

  for (const auto& key : key_list)
  {
    const int x1 = X;
    const int kname_len = getKeyNameWidth(key);
    const int txt_length = getKeyTextWidth(key);
    const int x2 = x1 + kname_len + txt_length + 1;

    if ( isClickInsideRange (ev, x1, x2) && ! key->hasMouseFocus() )
    {
      key->setMouseFocus();
      redraw();
    }

    X = x2 + 2;
  }
}

//----------------------------------------------------------------------
void FStatusBar::printMessageWithEllipsis (std::size_t term_width)
{
  const auto msg_length = getColumnWidth(getMessage());
  x += int(msg_length);

  if ( x - 1 <= int(term_width) )
    print (getMessage());
  else
  {
    // Print ellipsis
    const std::size_t len = msg_length + term_width - uInt(x) - 1;
    print() << getColumnSubString ( getMessage(), 1, len)
            << "..";
  }
}

//----------------------------------------------------------------------
void FStatusBar::printPaddingSpaces (int end)
{
  for (auto i = x; i <= end; i++)
    print (' ');
}

}  // namespace finalcut
