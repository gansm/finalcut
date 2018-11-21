/***********************************************************************
* flineedit.cpp - Widget FLineEdit                                     *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2012-2018 Markus Gans                                      *
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

#include "final/fapplication.h"
#include "final/flineedit.h"
#include "final/fstatusbar.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FLineEdit
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FLineEdit::FLineEdit(FWidget* parent)
  : FWidget(parent)
  , text("")
  , label_text("")
  , label(new FLabel("", parent))
  , label_orientation(FLineEdit::label_left)
  , drag_scroll(FLineEdit::noScroll)
  , scroll_timer(false)
  , scroll_repeat(100)
  , insert_mode(true)
  , cursor_pos(0)
  , text_offset(0)
{
  init();
}

//----------------------------------------------------------------------
FLineEdit::FLineEdit (const FString& txt, FWidget* parent)
  : FWidget(parent)
  , text(txt)
  , label_text("")
  , label(new FLabel("", parent))
  , label_orientation(FLineEdit::label_left)
  , drag_scroll(FLineEdit::noScroll)
  , scroll_timer(false)
  , scroll_repeat(100)
  , insert_mode(true)
  , cursor_pos(0)
  , text_offset(0)
{
  init();
  setText(txt);
}

//----------------------------------------------------------------------
FLineEdit::~FLineEdit()  // destructor
{
  if ( ! insert_mode )
    setInsertCursor();
}

// FLineEdit operators
//----------------------------------------------------------------------
FLineEdit& FLineEdit::operator = (const FString& s)
{
  setText(s);
  return *this;
}

//----------------------------------------------------------------------
FLineEdit& FLineEdit::operator << (const FString& s)
{
  setText(text + s);
  return *this;
}

//----------------------------------------------------------------------
FLineEdit& FLineEdit::operator << (const wchar_t c)
{
  setText(text + c);
  return *this;
}

//----------------------------------------------------------------------
FLineEdit& FLineEdit::operator << (const uInt num)
{
  FString num_str;
  num_str << num;
  setText(text + num_str);
  return *this;
}

//----------------------------------------------------------------------
FLineEdit& FLineEdit::operator << (const int num)
{
  FString num_str;
  num_str << num;
  setText(text + num_str);
  return *this;
}

//----------------------------------------------------------------------
FLineEdit& FLineEdit::operator << (const uLong num)
{
  FString num_str;
  num_str << num;
  setText(text + num_str);
  return *this;
}

//----------------------------------------------------------------------
FLineEdit& FLineEdit::operator << (const long num)
{
  FString num_str;
  num_str << num;
  setText(text + num_str);
  return *this;
}

//----------------------------------------------------------------------
FLineEdit& FLineEdit::operator << (const float num)
{
  FString num_str;
  num_str << num;
  setText(text + num_str);
  return *this;
}

//----------------------------------------------------------------------
FLineEdit& FLineEdit::operator << (const double num)
{
  FString num_str;
  num_str << num;
  setText(text + num_str);
  return *this;
}

//----------------------------------------------------------------------
FLineEdit& FLineEdit::operator << (const lDouble num)
{
  FString num_str;
  num_str << num;
  setText(text + num_str);
  return *this;
}

//----------------------------------------------------------------------
const FLineEdit& FLineEdit::operator >> (FString& s)
{
  s += text;
  return *this;
}


// public methods of FLineEdit
//----------------------------------------------------------------------
bool FLineEdit::setEnable (bool on)
{
  FWidget::setEnable(on);

  if ( on )
  {
    if ( hasFocus() )
    {
      setForegroundColor (wc.inputfield_active_focus_fg);
      setBackgroundColor (wc.inputfield_active_focus_bg);
    }
    else
    {
      setForegroundColor (wc.inputfield_active_fg);
      setBackgroundColor (wc.inputfield_active_bg);
    }
  }
  else
  {
    setForegroundColor (wc.inputfield_inactive_fg);
    setBackgroundColor (wc.inputfield_inactive_bg);
  }

  return on;
}

//----------------------------------------------------------------------
bool FLineEdit::setFocus (bool on)
{
  FWidget::setFocus(on);

  if ( on )
  {
    if ( isEnabled() )
    {
      setForegroundColor (wc.inputfield_active_focus_fg);
      setBackgroundColor (wc.inputfield_active_focus_bg);

      if ( getStatusBar() )
      {
        const FString& msg = getStatusbarMessage();
        const FString& curMsg = getStatusBar()->getMessage();

        if ( curMsg != msg )
          getStatusBar()->setMessage(msg);
      }
    }
  }
  else
  {
    if ( isEnabled() )
    {
      setForegroundColor (wc.inputfield_active_fg);
      setBackgroundColor (wc.inputfield_active_bg);

      if ( getStatusBar() )
        getStatusBar()->clearMessage();
    }
  }

  return on;
}

//----------------------------------------------------------------------
bool FLineEdit::setShadow (bool on)
{
  if ( on
    && getEncoding() != fc::VT100
    && getEncoding() != fc::ASCII )
  {
    flags.shadow = true;
    setShadowSize(1, 1);
  }
  else
  {
    flags.shadow = false;
    setShadowSize(0, 0);
  }

  return flags.shadow;
}

//----------------------------------------------------------------------
void FLineEdit::setText (const FString& txt)
{
  text_offset = 0;
  cursor_pos = 0;

  if ( txt )
    text = txt;
  else
    text = "";
}

//----------------------------------------------------------------------
void FLineEdit::setLabelText (const FString& ltxt)
{
  label_text = ltxt;
  label->setText(label_text);
  adjustLabel();
}

//----------------------------------------------------------------------
void FLineEdit::setLabelOrientation(const label_o o)
{
  label_orientation = o;
  adjustLabel();
}

//----------------------------------------------------------------------
void FLineEdit::hide()
{
  std::size_t s, size;
  FColor fg, bg;
  FWidget* parent_widget = getParentWidget();

  FWidget::hide();

  if ( parent_widget )
  {
    fg = parent_widget->getForegroundColor();
    bg = parent_widget->getBackgroundColor();
  }
  else
  {
    fg = wc.dialog_fg;
    bg = wc.dialog_bg;
  }

  setColor (fg, bg);
  s = hasShadow() ? 1 : 0;
  size = getWidth() + s;

  if ( size == 0 )
    return;

  char* blank = createBlankArray(size + 1);

  for (std::size_t y = 0; y < getHeight() + s; y++)
  {
    setPrintPos (1, 1 + int(y));
    print (blank);
  }

  destroyBlankArray(blank);

  if ( label )
    label->hide();
}

//----------------------------------------------------------------------
void FLineEdit::clear()
{
  text_offset = 0;
  cursor_pos = 0;
  text.clear();
}

//----------------------------------------------------------------------
void FLineEdit::onKeyPress (FKeyEvent* ev)
{
  FKey key = ev->key();

  switch ( key )
  {
    case fc::Fkey_left:
      keyLeft();
      ev->accept();
      break;

    case fc::Fkey_right:
      keyRight();
      ev->accept();
      break;

    case fc::Fkey_home:
      keyHome();
      ev->accept();
      break;

    case fc::Fkey_end:
      keyEnd();
      ev->accept();
      break;

    case fc::Fkey_dc:  // del key
      keyDel();
      ev->accept();
      break;

    case fc::Fkey_erase:
    case fc::Fkey_backspace:
      keyBackspace();
      ev->accept();
      break;

    case fc::Fkey_ic:  // insert key
      keyInsert();
      ev->accept();
      break;

    case fc::Fkey_return:
    case fc::Fkey_enter:
      keyEnter();
      ev->accept();
      break;

    case fc::Fkey_tab:
      ev->ignore();
      break;

    default:
      if ( keyInput(key) )
        ev->accept();
  }
  // end of switch

  if ( ev->isAccepted()
    && key != fc::Fkey_return
    && key != fc::Fkey_enter )
  {
    drawInputField();
    updateTerminal();
  }
}

//----------------------------------------------------------------------
void FLineEdit::onMouseDown (FMouseEvent* ev)
{
  int mouse_x, mouse_y;

  if ( ev->getButton() != fc::LeftButton )
    return;

  if ( ! hasFocus() )
  {
    FWidget* focused_widget = getFocusWidget();
    FFocusEvent out (fc::FocusOut_Event);
    FApplication::queueEvent(focused_widget, &out);
    setFocus();

    if ( focused_widget )
      focused_widget->redraw();

    redraw();

    if ( getStatusBar() )
      getStatusBar()->drawMessage();
  }

  mouse_x = ev->getX();
  mouse_y = ev->getY();

  if ( mouse_x >= 2 && mouse_x <= int(getWidth()) && mouse_y == 1 )
  {
    std::size_t len = text.getLength();
    cursor_pos = text_offset + std::size_t(mouse_x) - 2;

    if ( cursor_pos >= len )
      cursor_pos = len;

    drawInputField();
    updateTerminal();
  }
}

//----------------------------------------------------------------------
void FLineEdit::onMouseUp (FMouseEvent*)
{
  if ( drag_scroll != FLineEdit::noScroll )
  {
    delOwnTimer();
    drag_scroll = FLineEdit::noScroll;
    scroll_timer = false;
  }
}

//----------------------------------------------------------------------
void FLineEdit::onMouseMove (FMouseEvent* ev)
{
  std::size_t len;
  int mouse_x, mouse_y;

  if ( ev->getButton() != fc::LeftButton )
    return;

  len = text.getLength();
  mouse_x = ev->getX();
  mouse_y = ev->getY();

  if ( mouse_x >= 2 && mouse_x <= int(getWidth()) && mouse_y == 1 )
  {
    cursor_pos = text_offset + std::size_t(mouse_x) - 2;

    if ( cursor_pos >= len )
      cursor_pos = len;

    drawInputField();
    updateTerminal();
  }

  // auto-scrolling when dragging mouse outside the widget
  if ( mouse_x < 2 )
  {
    // drag left
    if ( ! scroll_timer && text_offset > 0 )
    {
      scroll_timer = true;
      addTimer(scroll_repeat);
      drag_scroll = FLineEdit::scrollLeft;
    }

    if ( text_offset == 0 )
    {
      delOwnTimer();
      drag_scroll = FLineEdit::noScroll;
    }
  }
  else if ( mouse_x >= int(getWidth()) )
  {
    // drag right
    if ( ! scroll_timer && text_offset <= len - getWidth() + 1 )
    {
      scroll_timer = true;
      addTimer(scroll_repeat);
      drag_scroll = FLineEdit::scrollRight;
    }

    if ( text_offset == len - getWidth() + 2 )
    {
      delOwnTimer();
      drag_scroll = FLineEdit::noScroll;
    }
  }
  else
  {
    // no dragging
    delOwnTimer();
    scroll_timer = false;
    drag_scroll = FLineEdit::noScroll;
  }
}

//----------------------------------------------------------------------
void FLineEdit::onTimer (FTimerEvent*)
{
  std::size_t len = text.getLength();

  switch ( int(drag_scroll) )
  {
    case FLineEdit::noScroll:
      return;

    case FLineEdit::scrollLeft:
      if ( text_offset == 0 )
      {
        drag_scroll = FLineEdit::noScroll;
        return;
      }

      text_offset--;

      if ( cursor_pos > 0 )
        cursor_pos--;

      break;

    case FLineEdit::scrollRight:
      if ( text_offset == len - getWidth() + 2 )
      {
        drag_scroll = FLineEdit::noScroll;
        return;
      }

      if ( text_offset <= len - getWidth() + 1 )
        text_offset++;

      if ( cursor_pos < len )
        cursor_pos++;

      break;

    default:
      break;
  }

  drawInputField();
  updateTerminal();
}

//----------------------------------------------------------------------
void FLineEdit::onAccel (FAccelEvent* ev)
{
  if ( ! isEnabled() )
    return;

  if ( ! hasFocus() )
  {
    FWidget* focused_widget = static_cast<FWidget*>(ev->focusedWidget());

    if ( focused_widget && focused_widget->isWidget() )
    {
      FFocusEvent out (fc::FocusOut_Event);
      FApplication::queueEvent(focused_widget, &out);
      setFocus();
      focused_widget->redraw();
      redraw();

      if ( getStatusBar() )
      {
        getStatusBar()->drawMessage();
        updateTerminal();
        flush_out();
      }
    }
  }

  ev->accept();
}

//----------------------------------------------------------------------
void FLineEdit::onHide (FHideEvent*)
{
  if ( ! insert_mode )
    setInsertCursor();
}

//----------------------------------------------------------------------
void FLineEdit::onFocusIn (FFocusEvent*)
{
  if ( insert_mode )
    setInsertCursor();
  else
    unsetInsertCursor();

  if ( getStatusBar() )
  {
    getStatusBar()->drawMessage();
    updateTerminal();
    flush_out();
  }
}

//----------------------------------------------------------------------
void FLineEdit::onFocusOut (FFocusEvent*)
{
  if ( getStatusBar() )
  {
    getStatusBar()->clearMessage();
    getStatusBar()->drawMessage();
  }

  if ( ! insert_mode )
    setInsertCursor();
}


// protected methods of FListBox
//----------------------------------------------------------------------
void FLineEdit::adjustLabel()
{
  std::size_t label_length = label_text.getLength();

  if ( hasHotkey() )
    label_length--;

  assert ( label_orientation == label_above
        || label_orientation == label_left );

  switch ( label_orientation )
  {
    case label_above:
      label->setGeometry(getX(), getY() - 1, label_length, 1);
      break;

    case label_left:
      label->setGeometry(getX() - int(label_length) - 1, getY()
                        , label_length, 1);
      break;
  }
}

//----------------------------------------------------------------------
void FLineEdit::adjustSize()
{
  FWidget::adjustSize();
  adjustLabel();
}


// private methods of FLineEdit
//----------------------------------------------------------------------
void FLineEdit::init()
{
  label->setAccelWidget(this);
  setVisibleCursor();
  setShadow();

  if ( isEnabled() )
  {
    if ( hasFocus() )
    {
      setForegroundColor (wc.inputfield_active_focus_fg);
      setBackgroundColor (wc.inputfield_active_focus_bg);
    }
    else
    {
      setForegroundColor (wc.inputfield_active_fg);
      setBackgroundColor (wc.inputfield_active_bg);
    }
  }
  else  // inactive
  {
    setForegroundColor (wc.inputfield_inactive_fg);
    setBackgroundColor (wc.inputfield_inactive_bg);
  }
}

//----------------------------------------------------------------------
bool FLineEdit::hasHotkey()
{
  if ( label_text.isEmpty() )
    return 0;

  return label_text.includes('&');
}

//----------------------------------------------------------------------
void FLineEdit::draw()
{
  drawInputField();

  if ( flags.focus && getStatusBar() )
  {
    const FString& msg = getStatusbarMessage();
    const FString& curMsg = getStatusBar()->getMessage();

    if ( curMsg != msg )
    {
      getStatusBar()->setMessage(msg);
      getStatusBar()->drawMessage();
    }
  }
}

//----------------------------------------------------------------------
void FLineEdit::drawInputField()
{
  std::size_t x;
  FString show_text;
  bool isActiveFocus = flags.active && flags.focus;
  setPrintPos (1, 1);

  if ( isMonochron() )
  {
    setReverse(true);
    print (' ');

    if ( isActiveFocus )
      setReverse(false);
    else
      setUnderline(true);
  }
  else
  {
    setColor();
    print (' ');
  }

  if ( isActiveFocus && getMaxColor() < 16 )
    setBold();

  show_text = text.mid(1 + text_offset, getWidth() - 2);

  if ( isLinuxTerm() && hasUTF8() )
  {
    setUTF8(true);

    if ( show_text )
      print (show_text);

    setUTF8(false);
  }
  else if ( show_text )
    print (show_text);

  x = show_text.getLength();

  while ( x < getWidth() - 1 )
  {
    print (' ');
    x++;
  }

  if ( isActiveFocus && getMaxColor() < 16 )
    unsetBold();

  if ( isMonochron() )
  {
    setReverse(false);
    setUnderline(false);
  }

  if ( flags.shadow )
    drawShadow ();

  // set the cursor to the first pos.
  setCursorPos (int(2 + cursor_pos - text_offset), 1);
}

//----------------------------------------------------------------------
inline void FLineEdit::keyLeft()
{
  if ( cursor_pos > 0 )
    cursor_pos--;

  if ( cursor_pos < text_offset )
    text_offset--;
}

//----------------------------------------------------------------------
inline void FLineEdit::keyRight()
{
  std::size_t len = text.getLength();

  if ( cursor_pos < len )
    cursor_pos++;

  if ( cursor_pos - text_offset >= getWidth() - 2
    && text_offset <= len - getWidth() + 1 )
    text_offset++;
}

//----------------------------------------------------------------------
inline void FLineEdit::keyHome()
{
  cursor_pos = 0;
  text_offset = 0;
}

//----------------------------------------------------------------------
inline void FLineEdit::keyEnd()
{
  std::size_t len = text.getLength();
  cursor_pos = len;

  if ( cursor_pos >= getWidth() - 1 )
    text_offset = len - getWidth() + 2;
}

//----------------------------------------------------------------------
inline void FLineEdit::keyDel()
{
  std::size_t len = text.getLength();

  if ( len > 0 && cursor_pos < len )
  {
    text.remove(cursor_pos, 1);
    processChanged();
  }

  if ( cursor_pos >= len )
    cursor_pos = len;

  if ( text_offset > 0 && len - text_offset < getWidth() - 1 )
    text_offset--;
}

//----------------------------------------------------------------------
inline void FLineEdit::keyBackspace()
{
  if ( text.getLength() > 0 && cursor_pos > 0 )
  {
    text.remove(cursor_pos - 1, 1);
    processChanged();
    cursor_pos--;

    if ( text_offset > 0 )
      text_offset--;
  }
}

//----------------------------------------------------------------------
inline void FLineEdit::keyInsert()
{
  insert_mode = ! insert_mode;

  if ( insert_mode )
    setInsertCursor();
  else
    unsetInsertCursor();
}

//----------------------------------------------------------------------
inline void FLineEdit::keyEnter()
{
  processActivate();
}

//----------------------------------------------------------------------
inline bool FLineEdit::keyInput (FKey key)
{
  if ( key >= 0x20 && key <= 0x10fff )
  {
    std::size_t len = text.getLength();

    if ( cursor_pos == len )
    {
      text += wchar_t(key);
      processChanged();
    }
    else if ( len > 0 )
    {
      if ( insert_mode )
        text.insert(wchar_t(key), cursor_pos);
      else
        text.overwrite(wchar_t(key), cursor_pos);

      processChanged();
    }
    else
    {
      text = wchar_t(key);
      processChanged();
    }

    cursor_pos++;

    if ( cursor_pos >= getWidth() - 1 )
      text_offset++;

    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
void FLineEdit::processActivate()
{
  if ( ! hasFocus() )
  {
    setFocus();
    redraw();
  }

  emitCallback("activate");
}

//----------------------------------------------------------------------
void FLineEdit::processChanged()
{
  emitCallback("changed");
}

}  // namespace finalcut
