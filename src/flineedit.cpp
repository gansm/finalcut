/***********************************************************************
* flineedit.cpp - Widget FLineEdit                                     *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2012-2020 Markus Gans                                      *
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

#include <regex>

#include "final/fapplication.h"
#include "final/fevent.h"
#include "final/flabel.h"
#include "final/flog.h"
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
  : FWidget{parent}
  , label{new FLabel("", parent)}
{
  init();
}

//----------------------------------------------------------------------
FLineEdit::FLineEdit (const FString& txt, FWidget* parent)
  : FWidget{parent}
  , text{txt}
  , label{new FLabel("", parent)}
{
  init();
  setText(txt);
}

//----------------------------------------------------------------------
FLineEdit::~FLineEdit()  // destructor
{
  if ( ! insert_mode )
    FTerm::setInsertCursor();
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
const FLineEdit& FLineEdit::operator >> (FString& s) const
{
  s += text;
  return *this;
}


// public methods of FLineEdit
//----------------------------------------------------------------------
bool FLineEdit::setEnable (bool enable)
{
  FWidget::setEnable(enable);
  resetColors();
  return enable;
}

//----------------------------------------------------------------------
bool FLineEdit::setFocus (bool enable)
{
  FWidget::setFocus(enable);
  resetColors();
  return enable;
}

//----------------------------------------------------------------------
bool FLineEdit::setShadow (bool enable)
{
  if ( enable
    && FTerm::getEncoding() != fc::VT100
    && FTerm::getEncoding() != fc::ASCII )
  {
    setFlags().shadow = true;
    setShadowSize(FSize{1, 1});
  }
  else
  {
    setFlags().shadow = false;
    setShadowSize(FSize{0, 0});
  }

  return getFlags().shadow;
}

//----------------------------------------------------------------------
bool FLineEdit::setReadOnly (bool enable)
{
  if ( enable )
    unsetVisibleCursor();
  else
    setVisibleCursor();

  return (read_only = enable);
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
    if ( ! isReadOnly() )
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
    if ( ! isReadOnly() )
      cursorEnd();

    adjustTextOffset();
  }
}

//----------------------------------------------------------------------
void FLineEdit::setCursorPosition (std::size_t pos)
{
  if ( isReadOnly() )
    return;

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
void FLineEdit::resetColors()
{
  const auto& wc = getColorTheme();

  if ( isEnabled() )  // active
  {
    if ( hasFocus() )
    {
      setForegroundColor (wc->inputfield_active_focus_fg);
      setBackgroundColor (wc->inputfield_active_focus_bg);
    }
    else
    {
      setForegroundColor (wc->inputfield_active_fg);
      setBackgroundColor (wc->inputfield_active_bg);
    }
  }
  else  // inactive
  {
    setForegroundColor (wc->inputfield_inactive_fg);
    setBackgroundColor (wc->inputfield_inactive_bg);
  }

  FWidget::resetColors();
}


//----------------------------------------------------------------------
void FLineEdit::setSize (const FSize& size, bool adjust)
{
  FWidget::setSize (size, adjust);

  if ( isShown() )
    adjustTextOffset();
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
  const FSize shadow = hasShadow() ? FSize{1, 1} : FSize{0, 0};
  hideArea (getSize() + shadow);
}

//----------------------------------------------------------------------
void FLineEdit::clear()
{
  if ( ! isReadOnly() )
    cursor_pos = 0;

  text_offset = 0;
  char_width_offset = 0;
  text.clear();
  print_text.clear();
}

//----------------------------------------------------------------------
void FLineEdit::onKeyPress (FKeyEvent* ev)
{
  if ( isReadOnly() )
    return;

  const FKey key = ev->key();

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
  if ( ev->getButton() != fc::LeftButton || isReadOnly() )
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

  const int mouse_x = ev->getX();
  const int mouse_y = ev->getY();
  const int xmin = 2 + int(char_width_offset);

  if ( mouse_x >= xmin && mouse_x <= int(getWidth()) && mouse_y == 1 )
  {
    const std::size_t len = print_text.getLength();
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
    delOwnTimers();
    drag_scroll = FLineEdit::noScroll;
    scroll_timer = false;
  }
}

//----------------------------------------------------------------------
void FLineEdit::onMouseMove (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton || isReadOnly() )
    return;

  const std::size_t len = print_text.getLength();
  const int mouse_x = ev->getX();
  const int mouse_y = ev->getY();

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
      delOwnTimers();
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
      delOwnTimers();
      drag_scroll = FLineEdit::noScroll;
    }
  }
  else
  {
    // no dragging
    delOwnTimers();
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
  const auto len = print_text.getLength();

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
        flush();
      }
    }
  }

  ev->accept();
}

//----------------------------------------------------------------------
void FLineEdit::onHide (FHideEvent*)
{
  if ( ! insert_mode && ! isReadOnly() )
    FTerm::setInsertCursor();
}

//----------------------------------------------------------------------
void FLineEdit::onFocusIn (FFocusEvent*)
{
  if ( ! isReadOnly() )
  {
    if ( insert_mode )
      FTerm::setInsertCursor();
    else
      FTerm::unsetInsertCursor();
  }

  if ( getStatusBar() )
  {
    getStatusBar()->drawMessage();
    updateTerminal();
    flush();
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

  if ( ! insert_mode && ! isReadOnly() )
    FTerm::setInsertCursor();
}


// protected methods of FListBox
//----------------------------------------------------------------------
void FLineEdit::adjustLabel()
{
  auto label_width = getColumnWidth(label_text);
  const auto& w = label_associated_widget;

  if ( ! w )
    return;

  if ( hasHotkey() )
    label_width--;

  assert ( label_orientation == label_above
        || label_orientation == label_left );

  if ( label_orientation == label_above )
  {
    label->setGeometry ( FPoint{w->getX(), w->getY() - 1}
                       , FSize{label_width, 1} );
  }
  else if ( label_orientation == label_left )
  {
    label->setGeometry ( FPoint{w->getX() - int(label_width) - 1, w->getY()}
                       , FSize{label_width, 1} );
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
  label->setAccelWidget(this);
  setShadow();
  resetColors();

  if ( isReadOnly() )
    unsetVisibleCursor();
  else
    setVisibleCursor();
}

//----------------------------------------------------------------------
bool FLineEdit::hasHotkey() const
{
  if ( label_text.isEmpty() )
    return false;

  return label_text.includes('&');
}

//----------------------------------------------------------------------
void FLineEdit::draw()
{
  if ( cursor_pos == NOT_SET && ! isReadOnly() )
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
  const bool isActiveFocus = getFlags().active && getFlags().focus;
  print() << FPoint{1, 1};

  if ( FTerm::isMonochron() )
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

  if ( isActiveFocus && FTerm::getMaxColor() < 16 )
    setBold();

  const std::size_t text_offset_column = [this] ()
  {
    assert ( input_type == FLineEdit::textfield
          || input_type == FLineEdit::password );

    switch ( input_type )
    {
      case FLineEdit::textfield:
        return printTextField();

      case FLineEdit::password:
        return printPassword();
    }

    return std::size_t(0);
  }();

  while ( x_pos + 1 < getWidth() )
  {
    print (' ');
    x_pos++;
  }

  if ( isActiveFocus && FTerm::getMaxColor() < 16 )
    unsetBold();

  if ( FTerm::isMonochron() )
  {
    setReverse(false);
    setUnderline(false);
  }

  if ( getFlags().shadow )
    drawShadow(this);

  // set the cursor to the insert pos.
  const auto cursor_pos_column = getCursorColumnPos();
  const int xpos = int(2 + cursor_pos_column
                         - text_offset_column
                         + char_width_offset);
  setCursorPos ({xpos, 1});
}

//----------------------------------------------------------------------
inline std::size_t FLineEdit::printTextField()
{
  const std::size_t text_offset_column = getColumnWidth (print_text, text_offset);
  const std::size_t start_column = text_offset_column - char_width_offset + 1;
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
  const std::size_t text_offset_column = text_offset;
  const FString show_text{print_text.mid(1 + text_offset, getWidth() - 2)};

  if ( ! show_text.isEmpty() )
    print() << FString{show_text.getLength(), fc::Bullet};  // •

  x_pos = show_text.getLength();
  return text_offset_column;
}

//----------------------------------------------------------------------
inline std::size_t FLineEdit::getCursorColumnPos() const
{
  if ( input_type == FLineEdit::textfield )
  {
    return getColumnWidth (print_text, cursor_pos);
  }
  else if ( input_type == FLineEdit::password )
  {
    return cursor_pos;
  }

  return 0;
}

//----------------------------------------------------------------------
inline const FString FLineEdit::getPasswordText() const
{
  return FString{text.getLength(), fc::Bullet};  // •
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
  const std::size_t len = print_text.getLength();

  if ( pos >= len )
    pos = len - 1;

  while ( pos > 0 && input_width > 0 )
  {
    std::size_t char_width{};

    try
    {
      char_width = getColumnWidth(print_text[pos]);
    }
    catch (const std::out_of_range& ex)
    {
      *FApplication::getLog() << FLog::Error
          << "Out of Range error: " << ex.what() << std::endl;
    }

    if ( input_width >= char_width )
      input_width -= char_width;

    if ( input_width == 0 )
      break;

    if ( input_width == 1)
    {
      if ( char_width == 1 )
      {
        try
        {
          if ( getColumnWidth(print_text[pos - 1]) == 2 )  // pos is always > 0
          {
            fullwidth_char_offset = 1;
            break;
          }
        }
        catch (const std::out_of_range& ex)
        {
          *FApplication::getLog() << FLog::Error
              << "Out of Range error: " << ex.what() << std::endl;
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
  const std::size_t len = print_text.getLength();
  pos -= char_width_offset;

  while ( click_width < pos && idx < len )
  {
    std::size_t char_width{};

    try
    {
      char_width = getColumnWidth(print_text[idx]);
    }
    catch (const std::out_of_range& ex)
    {
      *FApplication::getLog() << FLog::Error
          << "Out of Range error: " << ex.what() << std::endl;
    }

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
  const std::size_t input_width = getWidth() - 2;
  const std::size_t len = print_text.getLength();
  const std::size_t len_column = getColumnWidth (print_text);
  std::size_t text_offset_column = getColumnWidth (print_text, text_offset);
  const std::size_t cursor_pos_column = getColumnWidth (print_text, cursor_pos);
  std::size_t first_char_width{0};
  std::size_t cursor_char_width{1};
  char_width_offset = 0;

  if ( cursor_pos < len )
  {
    try
    {
      cursor_char_width = getColumnWidth(print_text[cursor_pos]);
    }
    catch (const std::out_of_range& ex)
    {
      *FApplication::getLog() << FLog::Error
          << "Out of Range error: " << ex.what() << std::endl;
    }
  }

  if ( len > 0 )
  {
    try
    {
      first_char_width = getColumnWidth(print_text[0]);
    }
    catch (const std::out_of_range& ex)
    {
      *FApplication::getLog() << FLog::Error
          << "Out of Range error: " << ex.what() << std::endl;
    }
  }

  // Text alignment right for long lines
  while ( text_offset > 0 && len_column - text_offset_column < input_width )
  {
    text_offset--;
    text_offset_column = getColumnWidth (print_text, text_offset);
  }

  // Right cursor overflow
  if ( cursor_pos_column + 1 > text_offset_column + input_width )
  {
    const offsetPair offset_pair = endPosToOffset(cursor_pos);
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
  const auto& len = text.getLength();

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
  const auto& len = text.getLength();

  if ( cursor_pos == len )
    return;

  cursor_pos = len;
  adjustTextOffset();
}

//----------------------------------------------------------------------
inline void FLineEdit::deleteCurrentCharacter()
{
  // Delete key functionality

  const auto& len = text.getLength();

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
    FTerm::setInsertCursor();    // Insert mode
  else
    FTerm::unsetInsertCursor();  // Overtype mode
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
    FTerm::beep();
    return true;
  }

  if ( key >= 0x20 && key <= 0x10fff )
  {
    auto len = text.getLength();
    const auto ch = characterFilter(wchar_t(key));

    if ( ch == L'\0' )
      return false;
    else if ( cursor_pos == len )
      text += ch;
    else if ( len > 0 )
    {
      if ( insert_mode )
        text.insert(ch, cursor_pos);
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
inline wchar_t FLineEdit::characterFilter (const wchar_t c) const
{
  if ( input_filter.empty() )
    return c;

  const wchar_t character[2]{c, L'\0'};

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
void FLineEdit::processChanged() const
{
  emitCallback("changed");
}

}  // namespace finalcut
