/***********************************************************************
* flineedit.cpp - Widget FLineEdit                                     *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2012-2024 Markus Gans                                      *
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

#include <array>
#include <regex>

#include "final/fapplication.h"
#include "final/fevent.h"
#include "final/fwidgetcolors.h"
#include "final/fwidget_functions.h"
#include "final/util/flog.h"
#include "final/util/fpoint.h"
#include "final/util/fsize.h"
#include "final/widget/flabel.h"
#include "final/widget/flineedit.h"
#include "final/widget/fstatusbar.h"

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
  if ( input_type == InputType::Password )  // Zero password in memory
    std::fill (text.begin(), text.end(), '\0');

  if ( ! insert_mode )
    FVTerm::getFOutput()->setCursor(CursorMode::Insert);
}

// FLineEdit operators
//----------------------------------------------------------------------
auto FLineEdit::operator = (const FString& s) -> FLineEdit&
{
  setText(s);
  return *this;
}

//----------------------------------------------------------------------
auto FLineEdit::operator << (UniChar c) -> FLineEdit&
{
  setText(text + static_cast<wchar_t>(c));
  return *this;
}

//----------------------------------------------------------------------
auto FLineEdit::operator << (const wchar_t c) -> FLineEdit&
{
  setText(text + c);
  return *this;
}


// public methods of FLineEdit
//----------------------------------------------------------------------
void FLineEdit::setEnable (bool enable)
{
  FWidget::setEnable(enable);
  resetColors();
}

//----------------------------------------------------------------------
void FLineEdit::setShadow (bool enable)
{
  setWidgetShadow(this, enable);
}

//----------------------------------------------------------------------
void FLineEdit::setReadOnly (bool enable)
{
  if ( enable )
    unsetVisibleCursor();
  else
    setVisibleCursor();

  read_only = enable;
}

//----------------------------------------------------------------------
void FLineEdit::setAlignment (Align align) noexcept
{
  if ( align != Align::Left
    && align != Align::Center
    && align != Align::Right )
    alignment = Align::Left;
  else
    alignment = align;
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
void FLineEdit::inputText (const FString& input)
{
  if ( insert_mode )
    text.insert(input, cursor_pos);
  else
    text.overwrite(input, cursor_pos);

  print_text = ( isPasswordField() ) ? getPasswordText() : text;

  if ( isShown() )
  {
    if ( ! isReadOnly() )
      cursor_pos += input.getLength();

    adjustTextOffset();
  }

  processChanged();
}

//----------------------------------------------------------------------
void FLineEdit::deletesCharacter()
{
  deleteCurrentCharacter();
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

  cursor_pos = ( pos != 0 ) ? pos - 1 : 1;
  cursor_pos = std::min(cursor_pos, text.getLength());

  if ( isShown() )
    adjustTextOffset();
}

//----------------------------------------------------------------------
void FLineEdit::moveCursorToBegin()
{
  cursorHome();
}

//----------------------------------------------------------------------
void FLineEdit::moveCursorToEnd()
{
  cursorEnd();
}

//----------------------------------------------------------------------
void FLineEdit::stepCursorForward (std::size_t steps)
{
  while ( steps-- )
    cursorRight();
}

//----------------------------------------------------------------------
void FLineEdit::stepCursorBackward (std::size_t steps)
{
  while ( steps-- )
    cursorLeft();
}

//----------------------------------------------------------------------
void FLineEdit::setLabelText (const FString& ltxt)
{
  label_text.setString(ltxt);
  label->setText(label_text);
  adjustLabel();
}

//----------------------------------------------------------------------
void FLineEdit::setOverwriteMode (bool overwrite)
{
  insert_mode = ! overwrite;

  if ( insert_mode )
    FVTerm::getFOutput()->setCursor(CursorMode::Insert);     // Insert mode
  else
    FVTerm::getFOutput()->setCursor(CursorMode::Overwrite);  // Overtype mode
}

//----------------------------------------------------------------------
void FLineEdit::setLabelOrientation (const LabelOrientation o)
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
      FWidget::setForegroundColor (wc->input_field.focus_fg);
      FWidget::setBackgroundColor (wc->input_field.focus_bg);
    }
    else
    {
      FWidget::setForegroundColor (wc->input_field.fg);
      FWidget::setBackgroundColor (wc->input_field.bg);
    }
  }
  else  // inactive
  {
    FWidget::setForegroundColor (wc->input_field.inactive_fg);
    FWidget::setBackgroundColor (wc->input_field.inactive_bg);
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
  const auto& shadow = hasShadow() ? FSize{1, 1} : FSize{0, 0};
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

  const auto key = ev->key();
  const auto& iter = key_map.find(key);

  if ( iter != key_map.end() )
  {
    iter->second();
    ev->accept();
  }
  else if ( key == FKey::Tab || key == FKey::Back_tab
         || key == FKey::Up || key == FKey::Down )
  {
    ev->ignore();
  }
  else if ( keyInput(key) )
  {
    ev->accept();
  }

  if ( ev->isAccepted() && ! isEnterKey(key) )
  {
    drawInputField();
    forceTerminalUpdate();
  }
}

//----------------------------------------------------------------------
void FLineEdit::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != MouseButton::Left || isReadOnly() )
    return;

  setWidgetFocus(this);
  const int mouse_x = ev->getX();
  const int mouse_y = ev->getY();
  const int xmin = 2 + int(char_width_offset);

  if ( mouse_x < xmin || mouse_x > int(getWidth()) || mouse_y != 1 )
    return;

  const std::size_t len = print_text.getLength();
  cursor_pos = clickPosToCursorPos (std::size_t(mouse_x) - 2);
  cursor_pos = std::min(cursor_pos, len);

  if ( mouse_x == int(getWidth()) )
    adjustTextOffset();

  drawInputField();
  forceTerminalUpdate();
}

//----------------------------------------------------------------------
void FLineEdit::onMouseUp (FMouseEvent*)
{
  if ( isDragging(drag_scroll) )
  {
    delOwnTimers();
    drag_scroll = DragScrollMode::None;
    scroll_timer = false;
  }
}

//----------------------------------------------------------------------
void FLineEdit::onMouseMove (FMouseEvent* ev)
{
  if ( ev->getButton() != MouseButton::Left || isReadOnly() )
    return;

  setCursorPositionByMouseClick(ev);
  handleAutoScroll(ev);
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

  if ( drag_scroll == DragScrollMode::Leftward )
  {
    if ( text_offset == 0 )
    {
      drag_scroll = DragScrollMode::None;
      return;
    }

    text_offset--;

    if ( cursor_pos > 0 )
      cursor_pos--;
  }
  else if ( drag_scroll == DragScrollMode::Rightward )
  {
    const auto& offset = endPosToOffset(len).first;

    if ( text_offset == offset )
    {
      drag_scroll = DragScrollMode::None;
      return;
    }

    if ( text_offset < offset )
      text_offset++;

    if ( cursor_pos < len )
      cursor_pos++;
  }

  adjustTextOffset();
  drawInputField();
  forceTerminalUpdate();
}

//----------------------------------------------------------------------
void FLineEdit::onAccel (FAccelEvent* ev)
{
  if ( ! isEnabled() )
    return;

  setWidgetFocus(this);
  ev->accept();
}

//----------------------------------------------------------------------
void FLineEdit::onHide (FHideEvent*)
{
  if ( ! insert_mode && ! isReadOnly() )
    FVTerm::getFOutput()->setCursor(CursorMode::Insert);
}

//----------------------------------------------------------------------
void FLineEdit::onFocusIn (FFocusEvent* in_ev)
{
  resetColors();

  if ( ! isReadOnly() )
  {
    if ( insert_mode )
      FVTerm::getFOutput()->setCursor(CursorMode::Insert);
    else
      FVTerm::getFOutput()->setCursor(CursorMode::Overwrite);
  }

  FWidget::onFocusIn(in_ev);
}

//----------------------------------------------------------------------
void FLineEdit::onFocusOut (FFocusEvent* out_ev)
{
  resetColors();

  if ( ! insert_mode && ! isReadOnly() )
    FVTerm::getFOutput()->setCursor(CursorMode::Insert);

  FWidget::onFocusOut(out_ev);
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

  assert ( label_orientation == LabelOrientation::Above
        || label_orientation == LabelOrientation::Left );

  if ( label_orientation == LabelOrientation::Above )
  {
    label->setGeometry ( FPoint{w->getX(), w->getY() - 1}
                       , FSize{label_width, 1} );
  }
  else if ( label_orientation == LabelOrientation::Left )
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
  FLineEdit::resetColors();
  mapKeyFunctions();

  if ( isReadOnly() )
    FLineEdit::setVisibleCursor(false);
  else
    FLineEdit::setVisibleCursor(true);
}

//----------------------------------------------------------------------
inline void FLineEdit::mapKeyFunctions()
{
  key_map =
  {
    { FKey::Left      , [this] { cursorLeft(); } },
    { FKey::Right     , [this] { cursorRight(); } },
    { FKey::Home      , [this] { cursorHome(); } },
    { FKey::End       , [this] { cursorEnd(); } },
    { FKey::Del_char  , [this] { deleteCurrentCharacter(); } },
    { FKey::Erase     , [this] { deletePreviousCharacter(); } },
    { FKey::Backspace , [this] { deletePreviousCharacter(); } },
    { FKey::Insert    , [this] { switchInsertMode(); } },
    { FKey::Return    , [this] { acceptInput(); } },
    { FKey::Enter     , [this] { acceptInput(); } }
  };
}

//----------------------------------------------------------------------
auto FLineEdit::hasHotkey() const -> bool
{
  if ( label_text.isEmpty() )
    return false;

  return label_text.includes('&');
}

//----------------------------------------------------------------------
auto FLineEdit::getAlignOffset (const std::size_t length) const -> std::size_t
{
  return finalcut::getAlignOffset (alignment, getWidth(), length);
}

//----------------------------------------------------------------------
void FLineEdit::draw()
{
  if ( cursor_pos == NOT_SET && ! isReadOnly() )
    cursorEnd();

  if ( ! isShown() )
    adjustTextOffset();

  drawInputField();
  updateStatusbar (this);
}

//----------------------------------------------------------------------
void FLineEdit::drawInputField()
{
  initializeDrawing();
  std::size_t text_offset_column;
  std::size_t x_pos;
  std::tie(text_offset_column, x_pos) = choosePrintMethod();
  printTrailingSpaces (x_pos);
  finalizingDrawing();
  drawShadow();

  // set the cursor to the insert pos.
  const auto cursor_pos_column = getCursorColumnPos();
  const auto xpos = int(2 + align_offset
                          + cursor_pos_column
                          - text_offset_column
                          + char_width_offset);
  setCursorPos ({xpos, 1});
}

//----------------------------------------------------------------------
inline void FLineEdit::initializeDrawing()
{
  const bool isActiveFocus = getFlags().feature.active
                          && getFlags().focus.focus;
  print() << FPoint{1, 1};

  if ( FVTerm::getFOutput()->isMonochron() )
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

  if ( isActiveFocus && FVTerm::getFOutput()->getMaxColor() < 16 )
    setBold();
}

//----------------------------------------------------------------------
inline void FLineEdit::finalizingDrawing() const
{
  const bool isActiveFocus = getFlags().feature.active
                          && getFlags().focus.focus;

  if ( isActiveFocus && FVTerm::getFOutput()->getMaxColor() < 16 )
    unsetBold();

  if ( ! FVTerm::getFOutput()->isMonochron() )
    return;

  setReverse(false);
  setUnderline(false);
}

//----------------------------------------------------------------------
inline void FLineEdit::printTrailingSpaces (std::size_t x_pos)
{
  if ( x_pos + align_offset + 1 >= getWidth() )
    return;

  const auto trailing_spaces = getWidth() - x_pos - align_offset - 1;
  print(FString{trailing_spaces, L' '});
}

//----------------------------------------------------------------------
inline auto FLineEdit::choosePrintMethod() -> std::tuple<std::size_t, std::size_t>
{
  switch ( input_type )
  {
    case InputType::Textfield:
      return printTextField();

    case InputType::Password:
      return printPassword();

    default:
      throw std::invalid_argument{"Invalid input type"};
  }
}

//----------------------------------------------------------------------
inline auto FLineEdit::printTextField() -> std::tuple<std::size_t, std::size_t>
{
  const std::size_t text_offset_column = getColumnWidth (print_text, text_offset);
  const std::size_t start_column = text_offset_column - char_width_offset + 1;
  const FString& show_text = \
      getColumnSubString(print_text, start_column, getWidth() - 2);
  const auto x_pos = getColumnWidth(show_text);
  align_offset = getAlignOffset(x_pos + 2);

  if ( align_offset > 0 )
    print (FString{align_offset, L' '});  // leading spaces

  if ( ! show_text.isEmpty() )
    print (show_text);

  return std::make_tuple (text_offset_column, x_pos);
}

//----------------------------------------------------------------------
inline auto FLineEdit::printPassword() -> std::tuple<std::size_t, std::size_t>
{
  const std::size_t text_offset_column = text_offset;
  const FString show_text{print_text.mid(1 + text_offset, getWidth() - 2)};
  const auto x_pos = show_text.getLength();
  align_offset = getAlignOffset(x_pos + 2);

  if ( align_offset > 0 )
    print (FString{align_offset, ' '});  // leading spaces

  if ( ! show_text.isEmpty() )
    print() << FString{x_pos, UniChar::Bullet};  // •

  return std::make_tuple (text_offset_column, x_pos);
}

//----------------------------------------------------------------------
inline void FLineEdit::drawShadow()
{
  if ( getFlags().shadow.shadow )
    finalcut::drawShadow(this);
}

//----------------------------------------------------------------------
inline auto FLineEdit::getCursorColumnPos() const -> std::size_t
{
  if ( input_type == InputType::Textfield )
    return getColumnWidth (print_text, cursor_pos);

  if ( input_type == InputType::Password )
    return cursor_pos;

  return 0;
}

//----------------------------------------------------------------------
inline auto FLineEdit::getPasswordText() const -> FString
{
  return {text.getLength(), UniChar::Bullet};  // •
}

//----------------------------------------------------------------------
inline auto FLineEdit::isPasswordField() const -> bool
{
  return input_type == InputType::Password;
}

//----------------------------------------------------------------------
inline auto FLineEdit::isFullwidthChar (std::size_t pos) const -> bool
{
  try
  {
    return getColumnWidth(print_text[pos]) == 2;  // pos is always > 0
  }
  catch (const std::out_of_range& ex)
  {
    handleOutOfRangeError(ex);
    return false;
  }
}

//----------------------------------------------------------------------
inline auto FLineEdit::getColumnWidthWithErrorHandling
                       ( FString::const_reference cref_string
                       , std::size_t fallback_size ) const -> std::size_t
{
  try
  {
    return getColumnWidth(cref_string);
  }
  catch (const std::out_of_range& ex)
  {
    handleOutOfRangeError(ex);
    return fallback_size;
  }
}

//----------------------------------------------------------------------
inline auto FLineEdit::endPosToOffset (std::size_t pos) -> offsetPair
{
  std::size_t input_width =  ( getWidth() > 2 ) ? getWidth() - 2 : 0;
  std::size_t fullwidth_char_offset{0};
  const std::size_t len = print_text.getLength();

  if ( pos >= len && len > 0 )
    pos = len - 1;

  while ( pos > 0 && input_width > 0 )
  {
    std::size_t char_width = \
        getColumnWidthWithErrorHandling (print_text[pos]);

    if ( input_width > char_width )
      input_width -= char_width;
    else
      break;

    if ( input_width == 1)
    {
      if ( char_width == 1 && isFullwidthChar(pos - 1) )
      {
        fullwidth_char_offset = 1;
        break;
      }

      if ( char_width == 2 )
      {
        fullwidth_char_offset = 1;
        break;
      }
    }

    pos--;
  }

  return {pos, fullwidth_char_offset};
}

//----------------------------------------------------------------------
auto FLineEdit::clickPosToCursorPos (std::size_t pos) -> std::size_t
{
  std::size_t click_width{align_offset};
  std::size_t idx = text_offset;
  const std::size_t len = print_text.getLength();
  pos -= char_width_offset;

  while ( click_width < pos && idx < len )
  {
    std::size_t char_width = \
        getColumnWidthWithErrorHandling (print_text[idx]);
    idx++;
    click_width += char_width;

    if ( char_width == 2 && click_width == pos + 1)
      idx--;
  }

  return idx;
}

//----------------------------------------------------------------------
void FLineEdit::setCursorPositionByMouseClick (const FMouseEvent* ev)
{
  const std::size_t len = print_text.getLength();
  const int mouse_x = ev->getX();
  const int mouse_y = ev->getY();

  if ( mouse_x < 2 || mouse_x > int(getWidth()) || mouse_y != 1 )
    return;

  cursor_pos = clickPosToCursorPos (std::size_t(mouse_x) - 2);

  if ( cursor_pos >= len )
    cursor_pos = len;

  adjustTextOffset();
  drawInputField();
  forceTerminalUpdate();
}

//----------------------------------------------------------------------
void FLineEdit::handleAutoScroll (const FMouseEvent* ev)
{
  // Auto-scrolling when dragging mouse outside the widget

  const int mouse_x = ev->getX();

  if ( mouse_x < 2 )  // drag left
  {
    handleLeftDragScroll();
  }
  else if ( mouse_x >= int(getWidth()) )  // drag right
  {
    handleRightDragScroll();
  }
  else
  {
    // no dragging
    delOwnTimers();
    scroll_timer = false;
    drag_scroll = DragScrollMode::None;
  }
}

//----------------------------------------------------------------------
inline void FLineEdit::handleLeftDragScroll()
{
  if ( ! scroll_timer && text_offset > 0 )
  {
    scroll_timer = true;
    addTimer(scroll_repeat);
    drag_scroll = DragScrollMode::Leftward;
  }

  if ( text_offset == 0 )
  {
    delOwnTimers();
    drag_scroll = DragScrollMode::None;
  }
}

//----------------------------------------------------------------------
inline void FLineEdit::handleRightDragScroll()
{
  const std::size_t len = print_text.getLength();

  if ( ! scroll_timer && cursor_pos < len )
  {
    scroll_timer = true;
    addTimer(scroll_repeat);
    drag_scroll = DragScrollMode::Rightward;
  }

  if ( cursor_pos == len )
  {
    delOwnTimers();
    drag_scroll = DragScrollMode::None;
  }
}

//----------------------------------------------------------------------
void FLineEdit::adjustTextOffset()
{
  const auto input_width = getWidth() - 2;
  const auto len = print_text.getLength();
  const auto len_column = getColumnWidth (print_text);
  auto text_offset_column = getColumnWidth (print_text, text_offset);
  const auto cursor_pos_column = getColumnWidth (print_text, cursor_pos);
  std::size_t first_char_width{0};
  std::size_t cursor_char_width{1};
  char_width_offset = 0;

  if ( cursor_pos < len )
  {
    cursor_char_width = \
        getColumnWidthWithErrorHandling (print_text[cursor_pos], 1);
  }

  if ( len > 0 )
  {
    first_char_width = \
        getColumnWidthWithErrorHandling (print_text[0]);
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
    const auto& offset_pair = endPosToOffset(cursor_pos);
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
  auto prev_char_len = getPrevCharLength(text, cursor_pos);

  if ( prev_char_len < 0 )
  {
    const auto pos = searchLeftCharBegin(text, cursor_pos);

    if ( pos != NOT_FOUND )
    {
      cursor_pos = pos;
      adjustTextOffset();
    }

    return;
  }

  if ( cursor_pos >= std::size_t(prev_char_len) )
  {
    cursor_pos -= std::size_t(prev_char_len);
    adjustTextOffset();
  }
}

//----------------------------------------------------------------------
inline void FLineEdit::cursorRight()
{
  const auto len = text.getLength();
  const auto char_len = getCharLength(text, cursor_pos);

  if ( char_len < 0 )
  {
    const auto pos = searchRightCharBegin(text, cursor_pos);

    if ( pos != NOT_FOUND )
    {
      cursor_pos = pos;
      adjustTextOffset();
    }

    return;
  }

  if ( cursor_pos + std::size_t(char_len) <= len )
  {
    cursor_pos += std::size_t(char_len);
    adjustTextOffset();
  }
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
  const auto char_len = getCharLength(text, cursor_pos);

  if ( char_len < 0 )
    return;

  if ( len >= std::size_t(char_len)
    && cursor_pos <= len - std::size_t(char_len) )
  {
    text.remove(cursor_pos, std::size_t(char_len));
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
  setOverwriteMode (insert_mode);
}

//----------------------------------------------------------------------
inline void FLineEdit::acceptInput()
{
  processActivate();
}

//----------------------------------------------------------------------
inline auto FLineEdit::keyInput (FKey key) -> bool
{
  if ( text.getLength() >= max_length )
  {
    FVTerm::getFOutput()->beep();
    return true;
  }

  if ( key >= 0x20 && key <= 0x10fff )
  {
    auto len = text.getLength();
    const auto ch = characterFilter(wchar_t(key));

    if ( ch == L'\0' )
      return false;

    if ( cursor_pos == len )
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

  return false;
}

//----------------------------------------------------------------------
inline auto FLineEdit::characterFilter (const wchar_t c) const -> wchar_t
{
  if ( input_filter.empty() )
    return c;

  if ( std::regex_match(std::wstring(1, c), std::wregex(input_filter)) )
    return c;

  return L'\0';
}

//----------------------------------------------------------------------
void FLineEdit::processActivate()
{
  setWidgetFocus(this);
  emitCallback("activate");
}

//----------------------------------------------------------------------
void FLineEdit::processChanged() const
{
  emitCallback("changed");
}

}  // namespace finalcut
