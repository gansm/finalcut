// File: flineedit.cpp
// Provides: class FLineEdit

#include "fapp.h"
#include "flineedit.h"
#include "fstatusbar.h"


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
  , drag_scroll(FLineEdit::noScroll)
  , scroll_timer(false)
  , scroll_repeat(100)
  , insert_mode(true)
  , cursor_pos(0)
  , text_offset(0)
  , label_orientation(FLineEdit::label_left)
{
  init();
}

//----------------------------------------------------------------------
FLineEdit::FLineEdit (const FString& txt, FWidget* parent)
  : FWidget(parent)
  , text(txt)
  , label_text("")
  , label(new FLabel("", parent))
  , drag_scroll(FLineEdit::noScroll)
  , scroll_timer(false)
  , scroll_repeat(100)
  , insert_mode(true)
  , cursor_pos(0)
  , text_offset(0)
  , label_orientation(FLineEdit::label_left)
{
  init();
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
      setXTermCursorColor("rgb:ffff/ffff/ffff");
  }
}

// private methods of FLineEdit
//----------------------------------------------------------------------
void FLineEdit::init()
{
  label->setAccelWidget(this);
  setVisibleCursor();
  setShadow();

  if ( hasFocus() )
    flags |= fc::focus;

  if ( isEnabled() )
  {
    flags |= fc::active;

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
  bool isFocus;
  drawInputField();
  isFocus = ((flags & fc::focus) != 0);

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
  int active_focus = fc::active + fc::focus;
  isActiveFocus = ((flags & active_focus) == active_focus);
  isActive = ((flags & fc::active) != 0);
  isShadow = ((flags & fc::shadow) != 0 );

  updateVTerm(false);
  printPos (1, 1);

  if ( isMonochron() )
  {
    setReverse(true);
    print (' ');

    if ( isActiveFocus )
      setReverse(false);
    else
      setUnderline(true);
  }
  else if ( isActiveFocus )
  {
    setColor (wc.inputfield_active_focus_bg, wc.dialog_bg);

    if ( isCygwinTerminal() )  // IBM Codepage 850
      print (fc::FullBlock); // █
    else if ( isTeraTerm() )
        print (0xdb);
    else
      print (fc::RightHalfBlock); // ▐
  }
  else if ( isActive )
  {
    setColor (wc.inputfield_active_bg, wc.dialog_bg);

    if ( isCygwinTerminal() )  // IBM Codepage 850
      print (fc::FullBlock); // █
    else if ( isTeraTerm() )
        print (0xdb);
    else
      print (fc::RightHalfBlock); // ▐
  }
  else // isInactive
  {
    setColor (wc.inputfield_inactive_bg, wc.dialog_bg);

    if ( isCygwinTerminal() )  // IBM Codepage 850
      print (fc::FullBlock); // █
    else if ( isTeraTerm() )
        print (0xdb);
    else
      print (fc::RightHalfBlock); // ▐
  }

  if ( isActiveFocus && getMaxColor() < 16 )
    setBold();

  setColor();
  show_text = text.mid(uInt(1+text_offset), uInt(getWidth()-2));

  if ( isUTF8_linux_terminal() )
  {
    setUTF8(true);

    if ( show_text )
      print (show_text);

    setUTF8(false);
  }
  else if ( show_text )
    print (show_text);

  x = int(show_text.getLength());

  while ( x < getWidth()-1 )
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

  if ( isShadow )
    drawShadow ();

  // set the cursor to the first pos.
  setCursorPos (2+cursor_pos-text_offset, 1);

  updateVTerm(true);
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
      label->setGeometry(getX(), getY()-1, label_length, 1);
      break;

    case label_left:
      label->setGeometry(getX()-label_length, getY(), label_length, 1);
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
  int s, size;
  short fg, bg;
  char* blank;
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

  if ( size < 0 )
    return;

  blank = new char[size+1];
  std::memset(blank, ' ', uLong(size));
  blank[size] = '\0';

  for (int y=0; y < getHeight()+s; y++)
  {
    printPos (1, 1+y);
    print (blank);
  }

  delete[] blank;

  if ( label )
    label->hide();
}

//----------------------------------------------------------------------
bool FLineEdit::setEnable (bool on)
{
  FWidget::setEnable(on);

  if ( on )
  {
    flags |= fc::active;

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
    flags &= ~fc::active;
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
    flags |= fc::focus;

    if ( isEnabled() )
    {
      setForegroundColor (wc.inputfield_active_focus_fg);
      setBackgroundColor (wc.inputfield_active_focus_bg);

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
    flags &= ~fc::focus;

    if ( isEnabled() )
    {
      setForegroundColor (wc.inputfield_active_fg);
      setBackgroundColor (wc.inputfield_active_bg);

      if ( statusBar() )
        statusBar()->clearMessage();
    }
  }

  return on;
}

//----------------------------------------------------------------------
bool FLineEdit::setShadow (bool on)
{
  if (  on
     && (Encoding != fc::VT100 || isTeraTerm() )
     && Encoding != fc::ASCII )
    flags |= fc::shadow;
  else
    flags &= ~fc::shadow;

  return on;
}

//----------------------------------------------------------------------
void FLineEdit::onKeyPress (FKeyEvent* ev)
{
  int len = int(text.getLength());
  int key = ev->key();

  switch ( key )
  {
    case fc::Fkey_left:
      cursor_pos--;

      if ( cursor_pos < 0 )
        cursor_pos=0;

      if ( cursor_pos < text_offset )
        text_offset--;

      ev->accept();
      break;

    case fc::Fkey_right:
      cursor_pos++;

      if ( cursor_pos >= len )
        cursor_pos=len;

      if ( cursor_pos-text_offset >= getWidth()-2 && text_offset <= len-getWidth()+1 )
        text_offset++;

      ev->accept();
      break;

    case fc::Fkey_home:
      cursor_pos=0;
      text_offset=0;
      ev->accept();
      break;

    case fc::Fkey_end:
      cursor_pos=len;
      if ( cursor_pos >= getWidth()-1 )
        text_offset=len-getWidth()+2;
      ev->accept();
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

      if ( text_offset > 0 && len-text_offset < getWidth()-1 )
        text_offset--;

      ev->accept();
      break;

    case fc::Fkey_erase:
    case fc::Fkey_backspace:
      if ( len > 0 && cursor_pos > 0 )
      {
        text.remove(uInt(cursor_pos-1), 1);
        processChanged();
        cursor_pos--;

        if ( text_offset > 0 )
          text_offset--;
      }

      ev->accept();
      break;

    case fc::Fkey_ic: // insert key
      insert_mode = not insert_mode;

      if ( insert_mode )
      {
        setXTermCursorStyle(fc::blinking_underline);
        setKDECursor(fc::UnderlineCursor);
        setConsoleCursor(fc::underscore_cursor);

        if ( isUrxvtTerminal() )
          setXTermCursorColor("rgb:ffff/ffff/ffff");
      }
      else
      {
        setXTermCursorStyle(fc::steady_block);
        setKDECursor(fc::BlockCursor);
        setConsoleCursor(fc::full_block_cursor);

        if ( isUrxvtTerminal() )
          setXTermCursorColor("rgb:eeee/0000/0000");
      }

      ev->accept();
      break;

    case fc::Fkey_return:
    case fc::Fkey_enter:
      processActivate();
      ev->accept();
      break;

    case fc::Fkey_tab:
      ev->ignore();
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

        if ( cursor_pos >= getWidth()-1 )
          text_offset++;

        ev->accept();
      }
      else
        ev->ignore();
  }
  // end of switch

  if (  ev->isAccepted()
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

    if ( statusBar() )
      statusBar()->drawMessage();
  }

  mouse_x = ev->getX();
  mouse_y = ev->getY();

  if ( mouse_x >= 2 && mouse_x <= getWidth() && mouse_y == 1 )
  {
    int len = int(text.getLength());
    cursor_pos = text_offset + mouse_x - 2;

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
  int len, mouse_x, mouse_y;

  if ( ev->getButton() != fc::LeftButton )
    return;

  len = int(text.getLength());
  mouse_x = ev->getX();
  mouse_y = ev->getY();

  if ( mouse_x >= 2 && mouse_x <= getWidth() && mouse_y == 1 )
  {
    cursor_pos = text_offset + mouse_x - 2;

    if ( cursor_pos >= len )
      cursor_pos=len;

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
  else if ( mouse_x >= getWidth() )
  {
    // drag right
    if ( ! scroll_timer && text_offset <= len-getWidth()+1 )
    {
      scroll_timer = true;
      addTimer(scroll_repeat);
      drag_scroll = FLineEdit::scrollRight;
    }

    if ( text_offset == len-getWidth()+2 )
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
  int len = int(text.getLength());

  switch ( int(drag_scroll) )
  {
    case FLineEdit::noScroll:
      return;

    case FLineEdit::scrollLeft:
      if ( text_offset == 0)
      {
        drag_scroll = FLineEdit::noScroll;
        return;
      }

      text_offset--;

      if ( text_offset < 0 )
        text_offset = 0;

      cursor_pos--;

      if ( cursor_pos < 0 )
        cursor_pos = 0;

      break;

    case FLineEdit::scrollRight:
      if ( text_offset == len-getWidth()+2 )
      {
        drag_scroll = FLineEdit::noScroll;
        return;
      }

      text_offset++;

      if ( text_offset > len-getWidth()+2 )
        text_offset = len-getWidth()+2;

      cursor_pos++;

      if ( cursor_pos > len )
        cursor_pos = len;

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
  if ( isEnabled() )
  {
    if ( ! hasFocus() )
    {
      FWidget* focused_widget = static_cast<FWidget*>(ev->focusedWidget());
      FFocusEvent out (fc::FocusOut_Event);
      FApplication::queueEvent(focused_widget, &out);
      setFocus();

      if ( focused_widget )
        focused_widget->redraw();

      redraw();

      if ( statusBar() )
      {
        statusBar()->drawMessage();
        updateTerminal();
        flush_out();
      }
    }

    ev->accept();
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
      setXTermCursorColor("rgb:ffff/ffff/ffff");
  }
}

//----------------------------------------------------------------------
void FLineEdit::onFocusIn (FFocusEvent*)
{
  if ( insert_mode )
  {
    setXTermCursorStyle(fc::blinking_underline);
    setKDECursor(fc::UnderlineCursor);
    setConsoleCursor(fc::underscore_cursor);
    if ( isUrxvtTerminal() )
      setXTermCursorColor("rgb:ffff/ffff/ffff");
  }
  else
  {
    setXTermCursorStyle(fc::steady_block);
    setKDECursor(fc::BlockCursor);
    setConsoleCursor(fc::full_block_cursor);
    if ( isUrxvtTerminal() )
      setXTermCursorColor("rgb:0000/0000/0000");
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
      setXTermCursorColor("rgb:ffff/ffff/ffff");
  }
}

//----------------------------------------------------------------------
void FLineEdit::clearText()
{
  text_offset = 0;
  cursor_pos = 0;
  text.clear();
}

//----------------------------------------------------------------------
void FLineEdit::setText (FString txt)
{
  text_offset = 0;
  cursor_pos = 0;

  if ( txt )
    text = txt;
  else
    text = "";
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
