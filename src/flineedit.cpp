// flineedit.cpp
// class FLineEdit

#include "fapp.h"
#include "flineedit.h"
#include "fstatusbar.h"


//----------------------------------------------------------------------
// class FLineEdit
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FLineEdit::FLineEdit(FWidget* parent) : FWidget(parent)
{
  this->init();
}

//----------------------------------------------------------------------
FLineEdit::FLineEdit (const FString& txt, FWidget* parent) : FWidget(parent)
{
  this->init();
  setText(txt);
}

//----------------------------------------------------------------------
FLineEdit::~FLineEdit()  // destructor
{
  if ( ! insert_mode )
  {
    setXTermCursorStyle(fc::blinking_underline);
    setKDECursor(fc::UnderlineCursor);
    setConsoleCursor(fc::underscore_cursor);
    if ( isUrxvtTerminal() )
      setXTermCursor("rgb:ffff/ffff/ffff");
  }
  if ( hasFocus() )
    hideCursor();
}

// private methods of FLineEdit
//----------------------------------------------------------------------
void FLineEdit::init()
{
  flags = 0;
  scrollTimer = false;
  scrollRepeat = 100;
  dragScroll = FLineEdit::noScroll;
  insert_mode = true;
  cursor_pos = 0;
  offset = 0;
  this->text = "";
  this->label_text = "";
  this->label = new FLabel(label_text, parentWidget());
  label->setAccelWidget(this);
  label_orientation = FLineEdit::label_left;
  setVisibleCursor();

  if ( hasFocus() )
    this->flags |= FOCUS;

  if ( isEnabled() )
  {
    this->flags |= ACTIVE;

    if ( hasFocus() )
    {
      foregroundColor = wc.inputfield_active_focus_fg;
      backgroundColor = wc.inputfield_active_focus_bg;
    }
    else
    {
      foregroundColor = wc.inputfield_active_fg;
      backgroundColor = wc.inputfield_active_bg;
    }
  }
  else  // inactive
  {
    foregroundColor = wc.inputfield_inactive_fg;
    backgroundColor = wc.inputfield_inactive_bg;
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
  bool isFocus;

  if ( Encoding == fc::VT100 )
    unsetVT100altChar();

  drawInputField();

  isFocus = ((flags & FOCUS) != 0);
  if ( isFocus && statusBar() )
  {
    FString msg = getStatusbarMessage();
    FString curMsg = statusBar()->getMessage();
    if ( curMsg != msg )
    {
      statusBar()->setMessage(msg);
      statusBar()->drawMessage();
    }
  }
}

//----------------------------------------------------------------------
void FLineEdit::drawInputField()
{
  bool isActiveFocus, isActive, isShadow;
  int x;
  FString show_text;
  isActiveFocus = (flags & (ACTIVE+FOCUS)) == (ACTIVE+FOCUS);
  isActive = ((flags & ACTIVE) != 0);
  isShadow = ((flags & SHADOW) != 0 );

  setUpdateVTerm(false);
  gotoxy (xpos+xmin-1, ypos+ymin-1);
  if ( isMonochron() )
  {
    setReverse(true);
    print (' ');
  }
  else if ( isActiveFocus )
  {
    setColor (wc.inputfield_active_focus_bg, wc.dialog_bg);
    if ( this->isCygwinTerminal() )  // IBM Codepage 850
      print (fc::FullBlock); // █
    else
      print (fc::RightHalfBlock); // ▐
  }
  else if ( isActive )
  {
    setColor (wc.inputfield_active_bg, wc.dialog_bg);
    if ( this->isCygwinTerminal() )  // IBM Codepage 850
      print (fc::FullBlock); // █
    else
      print (fc::RightHalfBlock); // ▐
  }
  else // isInactive
  {
    setColor (wc.inputfield_inactive_bg, wc.dialog_bg);
    if ( this->isCygwinTerminal() )  // IBM Codepage 850
      print (fc::FullBlock); // █
    else
      print (fc::RightHalfBlock); // ▐
  }
  setColor (foregroundColor, backgroundColor);
  show_text = text.mid(uInt(1+offset), uInt(width-2));

  if ( isUTF8_linux_terminal() )
  {
    setUTF8(true);
    print (show_text);
    setUTF8(false);
  }
  else
    print (show_text);

  x = int(show_text.getLength());
  while ( x <= width-2 )
  {
    print (' ');
    x++;
  }
  if ( isMonochron() )
    setReverse(false);

  if ( isShadow )
    drawShadow ();

  setUpdateVTerm(true);

  // set the cursor to the first pos.
  setCursorPos (xpos+xmin+cursor_pos-offset, ypos+ymin-1);

  if ( isCursorInside() && hasFocus() && isHiddenCursor() )
    showCursor();
  else if ( ! isHiddenCursor() )
    hideCursor();
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


// protected methods of FListBox
//----------------------------------------------------------------------
void FLineEdit::adjustLabel()
{
  int label_length = int(label_text.getLength());

  if ( hasHotkey() )
    label_length--;

  assert (  label_orientation == label_above
         || label_orientation == label_left );

  switch ( label_orientation )
  {
    case label_above:
      label->setGeometry(xpos, ypos-1, label_length, 1);
      break;
    case label_left:
      label->setGeometry(xpos-label_length, ypos, label_length, 1);
      break;
  }
}

//----------------------------------------------------------------------
void FLineEdit::adjustSize()
{
  FWidget::adjustSize();
  adjustLabel();
}


// public methods of FLineEdit
//----------------------------------------------------------------------
void FLineEdit::hide()
{
  int fg, bg, s, size, lable_Length;
  char* blank;

  FWidget::hide();

  fg = parentWidget()->getForegroundColor();
  bg = parentWidget()->getBackgroundColor();
  setColor (fg, bg);
  s = hasShadow() ? 1 : 0;
  size = width + s;
  blank = new char[size+1];
  memset(blank, ' ', uLong(size));
  blank[size] = '\0';

  for (int y=0; y < height+s; y++)
  {
    gotoxy (xpos+xmin-1, ypos+ymin-1+y);
    print (blank);
  }
  delete[] blank;

  lable_Length = int(label_text.getLength());

  if ( lable_Length > 0 )
  {
    assert (  label_orientation == label_above
           || label_orientation == label_left );

    switch ( label_orientation )
    {
      case label_above:
        gotoxy (xpos+xmin-1, ypos+ymin-2);
        break;
      case label_left:
        gotoxy (xpos+xmin-int(lable_Length)-1, ypos+ymin-1);
        break;
    }
    blank = new char[lable_Length+1];
    memset(blank, ' ', uLong(size));
    blank[lable_Length] = '\0';
    print (blank);
    delete[] blank;
  }
}

//----------------------------------------------------------------------
bool FLineEdit::setEnable (bool on)
{
  FWidget::setEnable(on);

  if ( on )
  {
    this->flags |= ACTIVE;
    if ( hasFocus() )
    {
      foregroundColor = wc.inputfield_active_focus_fg;
      backgroundColor = wc.inputfield_active_focus_bg;
    }
    else
    {
      foregroundColor = wc.inputfield_active_fg;
      backgroundColor = wc.inputfield_active_bg;
    }
  }
  else
  {
    this->flags &= ~ACTIVE;
    foregroundColor = wc.inputfield_inactive_fg;
    backgroundColor = wc.inputfield_inactive_bg;
  }
  return on;
}

//----------------------------------------------------------------------
bool FLineEdit::setFocus (bool on)
{
  FWidget::setFocus(on);

  if ( on )
  {
    this->flags |= FOCUS;

    if ( isEnabled() )
    {
      foregroundColor = wc.inputfield_active_focus_fg;
      backgroundColor = wc.inputfield_active_focus_bg;

      if ( statusBar() )
      {
        FString msg = getStatusbarMessage();
        FString curMsg = statusBar()->getMessage();
        if ( curMsg != msg )
          statusBar()->setMessage(msg);
      }
    }
  }
  else
  {
    this->flags &= ~FOCUS;

    if ( isEnabled() )
    {
      foregroundColor = wc.inputfield_active_fg;
      backgroundColor = wc.inputfield_active_bg;
      if ( statusBar() )
        statusBar()->clearMessage();
    }
  }
  return on;
}

//----------------------------------------------------------------------
bool FLineEdit::setShadow (bool on)
{
  if ( on )
    this->flags |= SHADOW;
  else
    this->flags &= ~SHADOW;
  return on;
}

//----------------------------------------------------------------------
void FLineEdit::onKeyPress (FKeyEvent* event)
{
  int len = int(text.getLength());
  int key = event->key();

  switch ( key )
  {
    case fc::Fkey_left:
      cursor_pos--;
      if ( cursor_pos < 0 )
        cursor_pos=0;
      if ( cursor_pos < offset )
        offset--;
      event->accept();
      break;

    case fc::Fkey_right:
      cursor_pos++;
      if ( cursor_pos >= len )
        cursor_pos=len;
      if ( cursor_pos-offset >= width-2 && offset < len-width+2 )
        offset++;
      event->accept();
      break;

    case fc::Fkey_home:
      cursor_pos=0;
      offset=0;
      event->accept();
      break;

    case fc::Fkey_end:
      cursor_pos=len;
      if ( cursor_pos > width-2 )
        offset=len-width+2;
      event->accept();
      break;

    case fc::Fkey_dc: // del key
      if ( len > 0 && cursor_pos < len )
      {
        text.remove(uInt(cursor_pos), 1);
        processChanged();
      }
      if ( cursor_pos >= len )
        cursor_pos=len;
      if ( cursor_pos < 0 )
        cursor_pos=0;
      if ( offset > 0 && len-offset <= width-2 )
        offset--;
      event->accept();
      break;

    case fc::Fkey_erase:
    case fc::Fkey_backspace:
      if ( len > 0 && cursor_pos > 0 )
      {
        text.remove(uInt(cursor_pos-1), 1);
        processChanged();
        cursor_pos--;
        if ( offset > 0 )
          offset--;
      }
      event->accept();
      break;

    case fc::Fkey_ic: // insert key
      insert_mode = not insert_mode;
      if ( insert_mode )
      {
        setXTermCursorStyle(fc::blinking_underline);
        setKDECursor(fc::UnderlineCursor);
        setConsoleCursor(fc::underscore_cursor);
        if ( isUrxvtTerminal() )
          setXTermCursor("rgb:ffff/ffff/ffff");
      }
      else
      {
        setXTermCursorStyle(fc::steady_block);
        setKDECursor(fc::BlockCursor);
        setConsoleCursor(fc::full_block_cursor);
        if ( isUrxvtTerminal() )
          setXTermCursor("rgb:0000/0000/0000");
      }
      event->accept();
      break;

    case fc::Fkey_return:
    case fc::Fkey_enter:
      processActivate();
      event->accept();
      break;

    case fc::Fkey_tab:
      event->ignore();
      break;

    default:
      if ( key >= 0x20 && key <= 0x10fff )
      {
        if ( cursor_pos == len )
        {
          text += wchar_t(key);
          processChanged();
        }
        else if ( len > 0 )
        {
          if ( insert_mode )
            text.insert(wchar_t(key), uInt(cursor_pos));
          else
            text.overwrite(wchar_t(key), uInt(cursor_pos));
          processChanged();
        }
        else
        {
          text = wchar_t(key);
          processChanged();
        }
        cursor_pos++;
        if ( cursor_pos > width-2 )
          offset++;
        event->accept();
      }
      else
        event->ignore();
  }
  // end of switch

  if (  event->isAccepted()
     && key != fc::Fkey_return
     && key != fc::Fkey_enter )
  {
    drawInputField();
    updateTerminal();
  }
}

//----------------------------------------------------------------------
void FLineEdit::onMouseDown (FMouseEvent* event)
{
  int mouse_x, mouse_y;

  if ( event->getButton() != LeftButton )
    return;

  if ( ! hasFocus() )
  {
    FWidget* focused_widget = getFocusWidget();
    FFocusEvent out (FocusOut_Event);
    FApplication::queueEvent(focused_widget, &out);
    this->setFocus();
    if ( focused_widget )
      focused_widget->redraw();
    this->redraw();
    if ( statusBar() )
      statusBar()->drawMessage();
  }

  mouse_x = event->getX();
  mouse_y = event->getY();

  if ( mouse_x >= 2 && mouse_x <= width && mouse_y == 1 )
  {
    int len = int(text.getLength());
    cursor_pos = offset + mouse_x - 2;
    if ( cursor_pos >= len )
      cursor_pos = len;
    drawInputField();
    updateTerminal();
  }
}

//----------------------------------------------------------------------
void FLineEdit::onMouseUp (FMouseEvent*)
{
  if ( dragScroll != FLineEdit::noScroll )
  {
    delAllTimer();
    dragScroll = FLineEdit::noScroll;
    scrollTimer = false;
  }
}

//----------------------------------------------------------------------
void FLineEdit::onMouseMove (FMouseEvent* event)
{
  int len, mouse_x, mouse_y;

  if ( event->getButton() != LeftButton )
    return;

  len = int(text.getLength());
  mouse_x = event->getX();
  mouse_y = event->getY();

  if ( mouse_x >= 2 && mouse_x <= width && mouse_y == 1 )
  {

    cursor_pos = offset + mouse_x - 2;
    if ( cursor_pos >= len )
      cursor_pos=len;
    drawInputField();
    updateTerminal();
  }

  // auto-scrolling when dragging mouse outside the widget
  if ( mouse_x < 2 )
  {
    // drag left
    if ( ! scrollTimer && offset > 0 )
    {
      scrollTimer = true;
      addTimer(scrollRepeat);
      dragScroll = FLineEdit::scrollLeft;
    }
    if ( offset == 0 )
    {
      delAllTimer();
      dragScroll = FLineEdit::noScroll;
    }
  }
  else if ( mouse_x >= width )
  {
    // drag right
    if ( ! scrollTimer && offset < len-width+2 )
    {
      scrollTimer = true;
      addTimer(scrollRepeat);
      dragScroll = FLineEdit::scrollRight;
    }
    if ( offset == len-width+2 )
    {
      delAllTimer();
      dragScroll = FLineEdit::noScroll;
    }
  }
  else
  {
    // no dragging
    delAllTimer();
    scrollTimer = false;
    dragScroll = FLineEdit::noScroll;
  }
}

//----------------------------------------------------------------------
void FLineEdit::onTimer (FTimerEvent*)
{
  int len = int(text.getLength());

  switch ( dragScroll )
  {
    case FLineEdit::noScroll:
      return;

    case FLineEdit::scrollLeft:

      if ( offset == 0)
      {
        dragScroll = FLineEdit::noScroll;
        return;
      }
      offset--;
      if ( offset < 0 )
        offset = 0;
      cursor_pos--;
      if ( cursor_pos < 0 )
        cursor_pos = 0;
      break;

    case FLineEdit::scrollRight:
      if ( offset == len-width+2 )
      {
        dragScroll = FLineEdit::noScroll;
        return;
      }
      offset++;
      if ( offset > len-width+2 )
        offset = len-width+2;
      cursor_pos++;
      if ( cursor_pos > len )
        cursor_pos = len;
  }

  drawInputField();
  updateTerminal();
}

//----------------------------------------------------------------------
void FLineEdit::onAccel (FAccelEvent* event)
{
  if ( isEnabled() )
  {
    if ( ! hasFocus() )
    {
      FWidget* focused_widget = static_cast<FWidget*>(event->focusedWidget());
      FFocusEvent out (FocusOut_Event);
      FApplication::queueEvent(focused_widget, &out);
      this->setFocus();
      if ( focused_widget )
        focused_widget->redraw();
      this->redraw();
      if ( statusBar() )
      {
        statusBar()->drawMessage();
        updateTerminal();
        flush_out();
      }
    }
    event->accept();
  }
}

//----------------------------------------------------------------------
void FLineEdit::onHide (FHideEvent*)
{
  if ( ! insert_mode )
  {
    setXTermCursorStyle(fc::blinking_underline);
    setKDECursor(fc::UnderlineCursor);
    setConsoleCursor(fc::underscore_cursor);
    if ( isUrxvtTerminal() )
      setXTermCursor("rgb:ffff/ffff/ffff");
  }
  if ( hasFocus() )
    hideCursor();
}

//----------------------------------------------------------------------
void FLineEdit::onFocusIn (FFocusEvent*)
{
  if ( isCursorInside() )
    showCursor();

  if ( insert_mode )
  {
    setXTermCursorStyle(fc::blinking_underline);
    setKDECursor(fc::UnderlineCursor);
    setConsoleCursor(fc::underscore_cursor);
    if ( isUrxvtTerminal() )
      setXTermCursor("rgb:ffff/ffff/ffff");
  }
  else
  {
    setXTermCursorStyle(fc::steady_block);
    setKDECursor(fc::BlockCursor);
    setConsoleCursor(fc::full_block_cursor);
    if ( isUrxvtTerminal() )
      setXTermCursor("rgb:0000/0000/0000");
  }
  if ( statusBar() )
  {
    statusBar()->drawMessage();
    updateTerminal();
    flush_out();
  }
}

//----------------------------------------------------------------------
void FLineEdit::onFocusOut (FFocusEvent*)
{
  if ( statusBar() )
  {
    statusBar()->clearMessage();
    statusBar()->drawMessage();
  }

  if ( ! insert_mode )
  {
    setXTermCursorStyle(fc::blinking_underline);
    setKDECursor(fc::UnderlineCursor);
    setConsoleCursor(fc::underscore_cursor);
    if ( isUrxvtTerminal() )
      setXTermCursor("rgb:ffff/ffff/ffff");
  }

  hideCursor();
}

//----------------------------------------------------------------------
void FLineEdit::setText (FString txt)
{
  offset = 0;
  cursor_pos = 0;
  this->text = txt;
}

//----------------------------------------------------------------------
void FLineEdit::setLabelText (FString ltxt)
{
  label_text = ltxt;
  label->setText(label_text);
  adjustLabel();
}

//----------------------------------------------------------------------
void FLineEdit::setLabelOrientation(label_o o)
{
  label_orientation = o;
  adjustLabel();
}
