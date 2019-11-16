/***********************************************************************
* flineedit.cpp - Widget FLineEdit                                     *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2012-2019 Markus Gans                                      *
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

#include <regex>

#include "final/fapplication.h"
#include "final/fevent.h"
#include "final/flabel.h"
#include "final/flineedit.h"
#include "final/fpoint.h"
#include "final/fsize.h"
#include "final/fstatusbar.h"
#include "final/fwidgetcolors.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FLineEdit
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FLineEdit::FLineEdit (FWidget* parent)
  : FWidget(parent)
  , label{new FLabel("", parent)}
{
  init();
}

//----------------------------------------------------------------------
FLineEdit::FLineEdit (const FString& txt, FWidget* parent)
  : FWidget(parent)
  , text(txt)
  , label{new FLabel("", parent)}
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
FLineEdit& FLineEdit::operator << (fc::SpecialCharacter c)
{
  setText(text + static_cast<wchar_t>(c));
  return *this;
}

//----------------------------------------------------------------------
FLineEdit& FLineEdit::operator << (const wchar_t c)
{
  setText(text + c);
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
bool FLineEdit::setEnable (bool enable)
{
  const auto& wc = getFWidgetColors();
  FWidget::setEnable(enable);

  if ( enable )
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

  return enable;
}

//----------------------------------------------------------------------
bool FLineEdit::setFocus (bool enable)
{
  FWidget::setFocus(enable);

  if ( isEnabled() )
  {
    const auto& wc = getFWidgetColors();

    if ( enable )
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

  return enable;
}

//----------------------------------------------------------------------
bool FLineEdit::setShadow (bool enable)
{
  if ( enable
    && getEncoding() != fc::VT100
    && getEncoding() != fc::ASCII )
  {
    setFlags().shadow = true;
    setShadowSize(FSize(1, 1));
  }
  else
  {
    setFlags().shadow = false;
    setShadowSize(FSize(0, 0));
  }

  return getFlags().shadow;
}

//----------------------------------------------------------------------
void FLineEdit::setText (const FString& txt)
{
  if ( txt )
  {
    if ( txt.getLength() > max_length )
      text.setString(txt.left(max_length));
    else
      text.setString(txt);
  }
  else
    text.setString("");

  print_text = ( isPasswordField() ) ? getPasswordText() : text;

  if ( isShown() )
  {
    cursorEnd();
    adjustTextOffset();
  }
}

//----------------------------------------------------------------------
void FLineEdit::setMaxLength (std::size_t max)
{
  max_length = max;

  if ( text.getLength() > max_length )
  {
    text.setString(text.left(max_length));
    print_text = ( isPasswordField() ) ? getPasswordText() : text;
  }

  if ( isShown() )
  {
    cursorEnd();
    adjustTextOffset();
  }
}

//----------------------------------------------------------------------
void FLineEdit::setCursorPosition (std::size_t pos)
{
  if ( pos == 0 )
    cursor_pos = 1;
  else
    cursor_pos = pos - 1;

  if ( cursor_pos > text.getLength() )
    cursor_pos = text.getLength();

  if ( isShown() )
    adjustTextOffset();
}

//----------------------------------------------------------------------
void FLineEdit::setLabelText (const FString& ltxt)
{
  label_text.setString(ltxt);
  label->setText(label_text);
  adjustLabel();
}

//----------------------------------------------------------------------
void FLineEdit::setLabelOrientation (const label_o o)
{
  label_orientation = o;
  adjustLabel();
}

//----------------------------------------------------------------------
void FLineEdit::setGeometry ( const FPoint& pos, const FSize& size
                            , bool adjust )
{
  FWidget::setGeometry(pos, size, adjust);

  if ( isShown() )
    adjustTextOffset();
}

//----------------------------------------------------------------------
void FLineEdit::hide()
{
  if ( label )
    label->hide();

  FWidget::hide();
  FSize shadow = hasShadow() ? FSize(1, 1) : FSize(0, 0);
  hideArea (getSize() + shadow);
}

//----------------------------------------------------------------------
void FLineEdit::clear()
{
  cursor_pos = 0;
  text_offset = 0;
  char_width_offset = 0;
  text.clear();
  print_text.clear();
}

//----------------------------------------------------------------------
void FLineEdit::onKeyPress (FKeyEvent* ev)
{
  FKey key = ev->key();

  switch ( key )
  {
    case fc::Fkey_left:
      cursorLeft();
      ev->accept();
      break;

    case fc::Fkey_right:
      cursorRight();
      ev->accept();
      break;

    case fc::Fkey_home:
      cursorHome();
      ev->accept();
      break;

    case fc::Fkey_end:
      cursorEnd();
      ev->accept();
      break;

    case fc::Fkey_dc:  // del key
      deleteCurrentCharacter();
      ev->accept();
      break;

    case fc::Fkey_erase:
    case fc::Fkey_backspace:
      deletePreviousCharacter();
      ev->accept();
      break;

    case fc::Fkey_ic:  // insert key
      switchInsertMode();
      ev->accept();
      break;

    case fc::Fkey_return:
    case fc::Fkey_enter:
      acceptInput();
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
  if ( ev->getButton() != fc::LeftButton )
    return;

  if ( ! hasFocus() )
  {
    auto focused_widget = getFocusWidget();
    setFocus();

    if ( focused_widget )
      focused_widget->redraw();

    redraw();

    if ( getStatusBar() )
      getStatusBar()->drawMessage();
  }

  int mouse_x = ev->getX();
  int mouse_y = ev->getY();
  int xmin = 2 + int(char_width_offset);

  if ( mouse_x >= xmin && mouse_x <= int(getWidth()) && mouse_y == 1 )
  {
    std::size_t len = print_text.getLength();
    cursor_pos = clickPosToCursorPos (std::size_t(mouse_x) - 2);

    if ( cursor_pos >= len )
      cursor_pos = len;

    if ( mouse_x == int(getWidth()) )
      adjustTextOffset();

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
  if ( ev->getButton() != fc::LeftButton )
    return;

  std::size_t len = print_text.getLength();
  int mouse_x = ev->getX();
  int mouse_y = ev->getY();

  if ( mouse_x >= 2 && mouse_x <= int(getWidth()) && mouse_y == 1 )
  {
    cursor_pos = clickPosToCursorPos (std::size_t(mouse_x) - 2);

    if ( cursor_pos >= len )
      cursor_pos = len;

    adjustTextOffset();
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
    if ( ! scroll_timer && cursor_pos < len )
    {
      scroll_timer = true;
      addTimer(scroll_repeat);
      drag_scroll = FLineEdit::scrollRight;
    }

    if ( cursor_pos == len )
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
void FLineEdit::onWheel (FWheelEvent* ev)
{
  // Sends the wheel event to the parent widget

  auto widget = getParentWidget();

  if ( widget )
  {
    FApplication::sendEvent(widget, ev);
  }
}

//----------------------------------------------------------------------
void FLineEdit::onTimer (FTimerEvent*)
{
  auto len = print_text.getLength();

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
      if ( text_offset == endPosToOffset(len).first )
      {
        drag_scroll = FLineEdit::noScroll;
        return;
      }

      if ( text_offset < endPosToOffset(len).first )
        text_offset++;

      if ( cursor_pos < len )
        cursor_pos++;

      break;

    default:
      break;
  }

  adjustTextOffset();
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
    auto focused_widget = static_cast<FWidget*>(ev->focusedWidget());

    if ( focused_widget && focused_widget->isWidget() )
    {
      setFocus();
      focused_widget->redraw();
      redraw();

      if ( getStatusBar() )
      {
        getStatusBar()->drawMessage();
        updateTerminal();
        flushOutputBuffer();
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
    flushOutputBuffer();
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
  auto label_width = getColumnWidth(label_text);
  auto w = label_associated_widget;

  if ( ! w )
    return;

  if ( hasHotkey() )
    label_width--;

  assert ( label_orientation == label_above
        || label_orientation == label_left );

  switch ( label_orientation )
  {
    case label_above:
      label->setGeometry ( FPoint(w->getX(), w->getY() - 1)
                         , FSize(label_width, 1) );
      break;

    case label_left:
      label->setGeometry ( FPoint(w->getX() - int(label_width) - 1, w->getY())
                         , FSize(label_width, 1) );
      break;
  }
}

//----------------------------------------------------------------------
void FLineEdit::adjustSize()
{
  FWidget::adjustSize();
  adjustLabel();

  if ( isShown() )
    adjustTextOffset();
}


// private methods of FLineEdit
//----------------------------------------------------------------------
void FLineEdit::init()
{
  const auto& wc = getFWidgetColors();
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
  if ( cursor_pos == NOT_SET )
    cursorEnd();

  if ( ! isShown() )
    adjustTextOffset();

  drawInputField();

  if ( getFlags().focus && getStatusBar() )
  {
    const auto& msg = getStatusbarMessage();
    const auto& curMsg = getStatusBar()->getMessage();

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
  bool isActiveFocus = getFlags().active && getFlags().focus;
  print() << FPoint(1, 1);

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

  std::size_t text_offset_column = [&] () -> std::size_t
  {
    switch ( input_type )
    {
      case FLineEdit::textfield:
        return printTextField();

      case FLineEdit::password:
        return printPassword();
    }

    return 0;
  }();

  while ( x_pos + 1 < getWidth() )
  {
    print (' ');
    x_pos++;
  }

  if ( isActiveFocus && getMaxColor() < 16 )
    unsetBold();

  if ( isMonochron() )
  {
    setReverse(false);
    setUnderline(false);
  }

  if ( getFlags().shadow )
    drawShadow(this);

  // set the cursor to the insert pos.
  auto cursor_pos_column = getCursorColumnPos();
  int xpos = int(2 + cursor_pos_column
                   - text_offset_column
                   + char_width_offset);
  setCursorPos (FPoint(xpos, 1));
}

//----------------------------------------------------------------------
inline std::size_t FLineEdit::printTextField()
{
  std::size_t text_offset_column = getColumnWidth (print_text, text_offset);
  std::size_t start_column = text_offset_column - char_width_offset + 1;
  const FString& show_text = \
      getColumnSubString(print_text, start_column, getWidth() - 2);

  if ( ! show_text.isEmpty() )
    print (show_text);

  x_pos = getColumnWidth(show_text);
  return text_offset_column;
}

//----------------------------------------------------------------------
inline std::size_t FLineEdit::printPassword()
{
  std::size_t text_offset_column = text_offset;
  FString show_text(print_text.mid(1 + text_offset, getWidth() - 2));

  if ( ! show_text.isEmpty() )
    print() << FString(show_text.getLength(), fc::Bullet);  // •

  x_pos = show_text.getLength();
  return text_offset_column;
}

//----------------------------------------------------------------------
inline std::size_t FLineEdit::getCursorColumnPos()
{
  switch ( input_type )
  {
    case FLineEdit::textfield:
      return getColumnWidth (print_text, cursor_pos);

    case FLineEdit::password:
      return cursor_pos;
  }

  return 0;
}

//----------------------------------------------------------------------
inline const FString FLineEdit::getPasswordText() const
{
  return FString(text.getLength(), fc::Bullet);  // •
}

//----------------------------------------------------------------------
inline bool FLineEdit::isPasswordField() const
{
  return bool( input_type == FLineEdit::password );
}

//----------------------------------------------------------------------
inline FLineEdit::offsetPair FLineEdit::endPosToOffset (std::size_t pos)
{
  std::size_t input_width = getWidth() - 2;
  std::size_t fullwidth_char_offset{0};
  std::size_t len = print_text.getLength();

  if ( pos >= len )
    pos = len - 1;

  while ( pos > 0 && input_width > 0 )
  {
    std::size_t char_width = getColumnWidth(print_text[pos]);

    if ( input_width >= char_width )
      input_width -= char_width;

    if ( input_width == 0 )
      break;

    if ( input_width == 1)
    {
      if ( char_width == 1 )
      {
        if ( getColumnWidth(print_text[pos - 1]) == 2 )  // pos is always > 0
        {
          fullwidth_char_offset = 1;
          break;
        }
      }

      if ( char_width == 2 )
      {
        fullwidth_char_offset = 1;
        break;
      }
    }

    pos--;
  }

  return offsetPair(pos, fullwidth_char_offset);
}

//----------------------------------------------------------------------
std::size_t FLineEdit::clickPosToCursorPos (std::size_t pos)
{
  std::size_t click_width{0};
  std::size_t idx = text_offset;
  std::size_t len = print_text.getLength();
  pos -= char_width_offset;

  while ( click_width < pos && idx < len )
  {
    std::size_t char_width = getColumnWidth(print_text[idx]);
    idx++;
    click_width += char_width;

    if ( char_width == 2 && click_width == pos + 1)
      idx--;
  }

  return idx;
}

//----------------------------------------------------------------------
void FLineEdit::adjustTextOffset()
{
  std::size_t input_width = getWidth() - 2;
  std::size_t len = print_text.getLength();
  std::size_t len_column = getColumnWidth (print_text);
  std::size_t text_offset_column = getColumnWidth (print_text, text_offset);
  std::size_t cursor_pos_column = getColumnWidth (print_text, cursor_pos);
  std::size_t first_char_width{0};
  std::size_t cursor_char_width{1};
  char_width_offset = 0;

  if ( cursor_pos < len )
    cursor_char_width = getColumnWidth(print_text[cursor_pos]);

  if ( len > 0 )
    first_char_width = getColumnWidth(print_text[0]);

  // Text alignment right for long lines
  while ( text_offset > 0 && len_column - text_offset_column < input_width )
  {
    text_offset--;
    text_offset_column = getColumnWidth (print_text, text_offset);
  }

  // Right cursor overflow
  if ( cursor_pos_column + 1 > text_offset_column + input_width )
  {
    offsetPair offset_pair = endPosToOffset(cursor_pos);
    text_offset = offset_pair.first;
    char_width_offset = offset_pair.second;
    text_offset_column = getColumnWidth (print_text, text_offset);
  }

  // Right full-width cursor overflow
  if ( cursor_pos_column + 2 > text_offset_column + input_width
    && cursor_char_width == 2 )
  {
    text_offset++;

    if ( first_char_width == 2 )
      char_width_offset = 1;  // Deletes a half character at the beginning
  }

  // Left cursor underflow
  if ( text_offset > cursor_pos )
    text_offset = cursor_pos;
}

//----------------------------------------------------------------------
inline void FLineEdit::cursorLeft()
{
  if ( cursor_pos > 0 )
    cursor_pos--;

  adjustTextOffset();
}

//----------------------------------------------------------------------
inline void FLineEdit::cursorRight()
{
  auto len = text.getLength();

  if ( cursor_pos < len )
    cursor_pos++;

  adjustTextOffset();
}

//----------------------------------------------------------------------
inline void FLineEdit::cursorHome()
{
  cursor_pos = 0;
  text_offset = 0;
  char_width_offset = 0;
}

//----------------------------------------------------------------------
inline void FLineEdit::cursorEnd()
{
  auto len = text.getLength();

  if ( cursor_pos == len )
    return;

  cursor_pos = len;
  adjustTextOffset();
}

//----------------------------------------------------------------------
inline void FLineEdit::deleteCurrentCharacter()
{
  // Delete key functionality

  auto len = text.getLength();

  if ( len > 0 && cursor_pos < len )
  {
    text.remove(cursor_pos, 1);
    print_text = ( isPasswordField() ) ? getPasswordText() : text;
    processChanged();
  }

  if ( cursor_pos >= len )
    cursor_pos = len;

  adjustTextOffset();
}

//----------------------------------------------------------------------
inline void FLineEdit::deletePreviousCharacter()
{
  // Backspace functionality

  if ( text.getLength() == 0 || cursor_pos == 0 )
    return;

  cursorLeft();
  deleteCurrentCharacter();
}

//----------------------------------------------------------------------
inline void FLineEdit::switchInsertMode()
{
  insert_mode = ! insert_mode;

  if ( insert_mode )
    setInsertCursor();    // Insert mode
  else
    unsetInsertCursor();  // Overtype mode
}

//----------------------------------------------------------------------
inline void FLineEdit::acceptInput()
{
  processActivate();
}

//----------------------------------------------------------------------
inline bool FLineEdit::keyInput (FKey key)
{
  if ( text.getLength() >= max_length )
  {
    beep();
    return true;
  }

  if ( key >= 0x20 && key <= 0x10fff )
  {
    auto len = text.getLength();
    auto ch = characterFilter(wchar_t(key));

    if ( ch == L'\0' )
      return false;
    else if ( cursor_pos == len )
      text += ch;
    else if ( len > 0 )
    {
      if ( insert_mode )
      {
        text.insert(ch, cursor_pos);
        len++;
      }
      else
        text.overwrite(ch, cursor_pos);
    }
    else
      text.setString(ch);

    cursor_pos++;
    print_text = ( isPasswordField() ) ? getPasswordText() : text;
    adjustTextOffset();
    processChanged();
    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
inline wchar_t FLineEdit::characterFilter (const wchar_t c)
{
  if ( input_filter.empty() )
    return c;

  wchar_t character[2]{c, L'\0'};

  if ( regex_match(character, std::wregex(input_filter)) )
    return c;
  else
    return L'\0';
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
