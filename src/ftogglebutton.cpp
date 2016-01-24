// File: ftogglebutton.cpp
// Provides: class FToggleButton

#include "fapp.h"
#include "fbuttongroup.h"
#include "fstatusbar.h"
#include "ftogglebutton.h"


//----------------------------------------------------------------------
// class FToggleButton
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FToggleButton::FToggleButton(FWidget* parent)
  : FWidget(parent)
  , focus_inside_group(true)
  , text()
  , checked(false)
  , label_offset_pos(0)
  , button_width(0)
  , button_group(0)
{
  init();

  if ( parent && strcmp ( parent->getClassName()
                        , const_cast<char*>("FButtonGroup") ) == 0 )
  {
    setGroup( static_cast<FButtonGroup*>(parent) );
    group()->insert(this);  // insert into button group
  }
}

//----------------------------------------------------------------------
FToggleButton::FToggleButton (const FString& txt, FWidget* parent)
  : FWidget(parent)
  , focus_inside_group(true)
  , text()
  , checked(false)
  , label_offset_pos(0)
  , button_width(0)
  , button_group(0)
{
  init();
  setText(txt);

  if ( parent && strcmp ( parent->getClassName()
                        , const_cast<char*>("FButtonGroup") ) == 0 )
  {
    setGroup( static_cast<FButtonGroup*>(parent) );
    group()->insert( this );  // insert into button group
  }
}

//----------------------------------------------------------------------
FToggleButton::~FToggleButton()  // destructor
{
  delAccelerator();
  if ( group() )
    group()->remove(this);
  if ( hasFocus() )
    hideCursor();
}


// private methods of FToggleButton
//----------------------------------------------------------------------
void FToggleButton::init()
{
  setGeometry (1, 1, 4, 1, false);  // initialize geometry values

  if ( hasFocus() )
    flags = fc::focus;

  if ( isEnabled() )
  {
    flags |= fc::active;

    if ( hasFocus() )
    {
      foregroundColor = wc.toggle_button_active_focus_fg;
      backgroundColor = wc.toggle_button_active_focus_bg;
    }
    else
    {
      foregroundColor = wc.toggle_button_active_fg;
      backgroundColor = wc.toggle_button_active_bg;
    }
  }
  else  // inactive
  {
    foregroundColor = wc.label_inactive_fg;
    backgroundColor = wc.label_inactive_bg;
  }
}

//----------------------------------------------------------------------
void FToggleButton::setGroup (FButtonGroup* btngroup)
{
  button_group = btngroup;
}


// protected methods of FToggleButton
//----------------------------------------------------------------------
uChar FToggleButton::getHotkey()
{
  uInt length;

  if ( text.isEmpty() )
    return 0;

  length = text.getLength();

  for (uInt i=0; i < length; i++)
  {
    try
    {
      if ( (i+1 < length) && (text[i] == '&') )
        return uChar(text[++i]);
    }
    catch (const std::out_of_range&)
    {
      return 0;;
    }
  }
  return 0;
}

//----------------------------------------------------------------------
void FToggleButton::setHotkeyAccelerator()
{
  int hotkey = getHotkey();

  if ( hotkey )
  {
    if ( isalpha(hotkey) || isdigit(hotkey) )
    {
      addAccelerator (tolower(hotkey));
      addAccelerator (toupper(hotkey));
      // Meta + hotkey
      addAccelerator (fc::Fmkey_meta + tolower(hotkey));
    }
    else
      addAccelerator (getHotkey());
  }
  else
    delAccelerator();
}

//----------------------------------------------------------------------
void FToggleButton::draw()
{
  bool isFocus = ((flags & fc::focus) != 0);
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

  // set the cursor to the button
  if ( isRadioButton() || isCheckboxButton() )
  {
    setCursorPos (xpos+xmin, ypos+ymin-1);

    if ( isCursorInside() && hasFocus() )
    {
      if ( isHiddenCursor() )
        showCursor();
    }
    else if ( ! isHiddenCursor() )
      hideCursor();
  }
}

//----------------------------------------------------------------------
void FToggleButton::drawLabel()
{
  uInt length, i;
  int hotkeypos;
  FString txt;
  wchar_t* LabelText;
  register wchar_t* src;
  register wchar_t* dest;
  bool isActive, isNoUnderline;

  if ( ! isVisible() )
    return;
  if ( text.isNull() || text.isEmpty() )
    return;

  length = text.getLength();
  hotkeypos = -1;

  LabelText = new wchar_t[length+1]();
  txt = text;
  src = const_cast<wchar_t*>(txt.wc_str());
  dest = const_cast<wchar_t*>(LabelText);
  isActive = ((flags & fc::active) != 0);
  isNoUnderline = ((flags & fc::no_underline) != 0);

  // find hotkey position in string
  // + generate a new string without the '&'-sign
  for (i=0; i < length; i++)
  {
    if ( (i < length) && (txt[i] == '&') && (hotkeypos == -1) )
    {
      hotkeypos = int(i);
      i++;
      src++;
    }
    *dest++ = *src++;
  }
  if ( hotkeypos != -1 )
    length--;

  gotoxy (xpos+xmin-1+label_offset_pos, ypos+ymin-1);

  if ( isMonochron() )
    setReverse(true);

  if ( isEnabled() )
    setColor (wc.label_fg, wc.label_bg);
  else
    setColor (wc.label_inactive_fg, wc.label_inactive_bg);

  for (int z=0; z < int(length); z++)
  {
    if ( (z == hotkeypos) && isActive )
    {
      setColor (wc.label_hotkey_fg, wc.label_hotkey_bg);
      if ( ! isNoUnderline )
        setUnderline();
      print ( LabelText[z] );
      if ( ! isNoUnderline )
        unsetUnderline();
      setColor (wc.label_fg, wc.label_bg);
    }
    else
      print (LabelText[z]);
  }

  if ( isMonochron() )
    setReverse(false);

  delete[] LabelText;
}

//----------------------------------------------------------------------
void FToggleButton::processClick()
{
  emitCallback("clicked");
}

//----------------------------------------------------------------------
void FToggleButton::processToggle()
{
  emitCallback("toggled");
}

//----------------------------------------------------------------------
FButtonGroup* FToggleButton::group() const
{
  return button_group;
}

//----------------------------------------------------------------------
bool FToggleButton::isRadioButton() const
{
  return ( strcmp ( getClassName()
                  , const_cast<char*>("FRadioButton") ) == 0 );
}

//----------------------------------------------------------------------
bool FToggleButton::isCheckboxButton() const
{
  return ( strcmp ( getClassName()
                  , const_cast<char*>("FCheckBox") ) == 0 );
}

//----------------------------------------------------------------------
void FToggleButton::onKeyPress (FKeyEvent* ev)
{
  int key;

  if ( ! isEnabled() )
    return;

  key = ev->key();

  switch ( key )
  {
    case fc::Fkey_return:
    case fc::Fkey_enter:
    case fc::Fkey_space:
      if ( isRadioButton() )
      {
        if ( ! checked )
        {
          checked = true;
          processToggle();
        }
      }
      else
      {
        checked = not checked;
        processToggle();
      }
      processClick();
      ev->accept();
      break;

    case fc::Fkey_down:
    case fc::Fkey_right:
      focus_inside_group = true;
      focusNextChild();
      ev->accept();
      break;

    case fc::Fkey_up:
    case fc::Fkey_left:
      focus_inside_group = true;
      focusPrevChild();
      ev->accept();
      break;

    default:
      break;
  }

  if ( ev->isAccepted() )
  {
    draw();
    updateTerminal();
  }
}


// public methods of FToggleButton
//----------------------------------------------------------------------
void FToggleButton::hide()
{
  int size;
  short fg, bg;
  char* blank;

  FWidget::hide();

  fg = parentWidget()->getForegroundColor();
  bg = parentWidget()->getBackgroundColor();
  setColor (fg, bg);

  size = width;
  blank = new char[size+1];
  memset(blank, ' ', uLong(size));
  blank[size] = '\0';
  gotoxy (xpos+xmin-1, ypos+ymin-1);
  print (blank);
  delete[] blank;
}

//----------------------------------------------------------------------
void FToggleButton::setGeometry (int x, int y, int w, int h, bool adjust)
{
  int min_width = button_width + int(text.getLength());
  if ( w < min_width )
    w = min_width;
  FWidget::setGeometry(x, y, w, h, adjust);
}

//----------------------------------------------------------------------
bool FToggleButton::setNoUnderline (bool on)
{
  if ( on )
    flags |= fc::no_underline;
  else
    flags &= ~fc::no_underline;
  return on;
}

//----------------------------------------------------------------------
bool FToggleButton::setEnable (bool on)
{
  FWidget::setEnable(on);

  if ( on )
  {
    flags |= fc::active;
    setHotkeyAccelerator();
    if ( hasFocus() )
    {
      foregroundColor = wc.toggle_button_active_focus_fg;
      backgroundColor = wc.toggle_button_active_focus_bg;
    }
    else
    {
      foregroundColor = wc.toggle_button_active_fg;
      backgroundColor = wc.toggle_button_active_bg;
    }
  }
  else
  {
    flags &= ~fc::active;
    delAccelerator();
    foregroundColor = wc.toggle_button_inactive_fg;
    backgroundColor = wc.toggle_button_inactive_bg;
  }
  return on;
}

//----------------------------------------------------------------------
bool FToggleButton::setFocus (bool on)
{
  FWidget::setFocus(on);

  if ( on )
  {
    flags |= fc::focus;

    if ( isEnabled() )
    {
      if ( isRadioButton()  )
        focus_inside_group = false;

      foregroundColor = wc.toggle_button_active_focus_fg;
      backgroundColor = wc.toggle_button_active_focus_bg;


      if ( isCursorInside() && (isRadioButton() || isCheckboxButton()) )
        showCursor();

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
      foregroundColor = wc.toggle_button_active_fg;
      backgroundColor = wc.toggle_button_active_bg;
      hideCursor();

      if ( statusBar() )
        statusBar()->clearMessage();
    }
  }
  return on;
}

//----------------------------------------------------------------------
void FToggleButton::onMouseDown (FMouseEvent* ev)
{
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
    {
      statusBar()->drawMessage();
      updateTerminal();
      flush_out();
    }
  }
}

//----------------------------------------------------------------------
void FToggleButton::onMouseUp (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton )
    return;

  if ( getGeometryGlobal().contains(ev->getGlobalPos()) )
  {
    if ( isRadioButton() )
    {
      if ( ! checked )
      {
        checked = true;
        processToggle();
      }
    }
    else
    {
      checked = not checked;
      processToggle();
    }
    redraw();
    processClick();
  }
}

//----------------------------------------------------------------------
void FToggleButton::onAccel (FAccelEvent* ev)
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
    }

    if ( isRadioButton() )
    {
      if ( ! checked )
      {
        checked = true;
        processToggle();
      }
    }
    else
    {
      checked = not checked;
      processToggle();
    }
    redraw();
    if ( statusBar() )
    {
      statusBar()->drawMessage();
      updateTerminal();
      flush_out();
    }
    processClick();
    ev->accept();
  }
}

//----------------------------------------------------------------------
void FToggleButton::onHide (FHideEvent*)
{
  if ( hasFocus() )
    hideCursor();
}

//----------------------------------------------------------------------
void FToggleButton::onFocusIn (FFocusEvent*)
{
  if ( isCursorInside() && (isRadioButton() || isCheckboxButton()) )
    showCursor();

  if ( statusBar() )
    statusBar()->drawMessage();
}

//----------------------------------------------------------------------
void FToggleButton::onFocusOut (FFocusEvent* out_ev)
{
  if ( statusBar() )
  {
    statusBar()->clearMessage();
    statusBar()->drawMessage();
  }

  hideCursor();

  if ( group() )
  {
    if ( ! focus_inside_group && isRadioButton()  )
    {
      focus_inside_group = true;
      out_ev->ignore();
      if ( out_ev->getFocusType() == fc::FocusNextWidget )
        group()->focusNextChild();
      if ( out_ev->getFocusType() == fc::FocusPreviousWidget )
        group()->focusPrevChild();
      redraw();
    }
    else if (  this == group()->getLastButton()
       && out_ev->getFocusType() == fc::FocusNextWidget )
    {
      out_ev->ignore();
      group()->focusNextChild();
      redraw();
    }
    else if (  this == group()->getFirstButton()
            && out_ev->getFocusType() == fc::FocusPreviousWidget )
    {
      out_ev->ignore();
      group()->focusPrevChild();
      redraw();
    }
  }
}

//----------------------------------------------------------------------
bool FToggleButton::setChecked (bool on)
{
  if ( checked != on )
  {
    checked = on;
    processToggle();
  }
  return checked;
}

//----------------------------------------------------------------------
void FToggleButton::setText (FString txt)
{
  text = txt;
  setWidth(button_width + int(text.getLength()));
  if ( isEnabled() )
  {
    delAccelerator();
    setHotkeyAccelerator();
  }
}
