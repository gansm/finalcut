// File: ftogglebutton.cpp
// Provides: class FToggleButton

#include "fapplication.h"
#include "fbuttongroup.h"
#include "fstatusbar.h"
#include "ftogglebutton.h"


//----------------------------------------------------------------------
// class FToggleButton
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FToggleButton::FToggleButton (FWidget* parent)
  : FWidget(parent)
  , checked(false)
  , label_offset_pos(0)
  , button_width(0)
  , button_group(0)
  , focus_inside_group(true)
  , text()
{
  init();

  if ( parent && parent->isInstanceOf("FButtonGroup") )
  {
    setGroup( static_cast<FButtonGroup*>(parent) );

    if ( hasGroup() )
      getGroup()->insert(this);  // insert into button group
  }
}

//----------------------------------------------------------------------
FToggleButton::FToggleButton (const FString& txt, FWidget* parent)
  : FWidget(parent)
  , checked(false)
  , label_offset_pos(0)
  , button_width(0)
  , button_group(0)
  , focus_inside_group(true)
  , text()
{
  init();
  setText(txt);

  if ( parent && parent->isInstanceOf("FButtonGroup") )
  {
    setGroup( static_cast<FButtonGroup*>(parent) );

    if ( hasGroup() )
      getGroup()->insert(this);  // insert into button group
  }
}

//----------------------------------------------------------------------
FToggleButton::~FToggleButton()  // destructor
{
  delAccelerator();

  if ( hasGroup() )
    getGroup()->remove(this);
}


// public methods of FToggleButton
//----------------------------------------------------------------------
void FToggleButton::setGeometry (int x, int y, int w, int h, bool adjust)
{
  // Set the toggle button geometry

  int min_width = button_width + int(text.getLength());

  if ( w < min_width )
    w = min_width;

  const FRect geometry(x, y, w, h);

  if ( hasGroup() )
    getGroup()->checkScrollSize(geometry);

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
    setHotkeyAccelerator();

    if ( hasFocus() )
    {
      setForegroundColor (wc.toggle_button_active_focus_fg);
      setBackgroundColor (wc.toggle_button_active_focus_bg);
    }
    else
    {
      setForegroundColor (wc.toggle_button_active_fg);
      setBackgroundColor (wc.toggle_button_active_bg);
    }
  }
  else
  {
    delAccelerator();
    setForegroundColor (wc.toggle_button_inactive_fg);
    setBackgroundColor (wc.toggle_button_inactive_bg);
  }

  return on;
}

//----------------------------------------------------------------------
bool FToggleButton::setFocus (bool on)
{
  FWidget::setFocus(on);

  if ( on )
  {
    if ( isEnabled() )
    {
      if ( isRadioButton()  )
        focus_inside_group = false;

      setForegroundColor (wc.toggle_button_active_focus_fg);
      setBackgroundColor (wc.toggle_button_active_focus_bg);

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
      setForegroundColor (wc.toggle_button_active_fg);
      setBackgroundColor (wc.toggle_button_active_bg);

      if ( getStatusBar() )
        getStatusBar()->clearMessage();
    }
  }

  return on;
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
void FToggleButton::setText (const FString& txt)
{
  text = txt;
  setWidth(button_width + int(text.getLength()));

  if ( isEnabled() )
  {
    delAccelerator();
    setHotkeyAccelerator();
  }
}

//----------------------------------------------------------------------
void FToggleButton::hide()
{
  int size;
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
  size = getWidth();

  if ( size < 0 )
    return;

  try
  {
    blank = new char[size + 1];
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
    return;
  }

  std::memset(blank, ' ', uLong(size));
  blank[size] = '\0';
  setPrintPos (1, 1);
  print (blank);
  delete[] blank;
}

//----------------------------------------------------------------------
void FToggleButton::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton )
    return;

  if ( hasFocus() )
    return;

  FWidget* focused_widget = getFocusWidget();
  FFocusEvent out (fc::FocusOut_Event);
  FApplication::queueEvent(focused_widget, &out);
  setFocus();

  if ( focused_widget )
    focused_widget->redraw();

  redraw();

  if ( getStatusBar() )
  {
    getStatusBar()->drawMessage();
    updateTerminal();
    flush_out();
  }
}

//----------------------------------------------------------------------
void FToggleButton::onMouseUp (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton )
    return;

  if ( ! getTermGeometry().contains(ev->getTermPos()) )
    return;

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

//----------------------------------------------------------------------
void FToggleButton::onWheel (FWheelEvent* ev)
{
  if ( ! hasGroup() )
    return;

  getGroup()->onWheel(ev);
}

//----------------------------------------------------------------------
void FToggleButton::onAccel (FAccelEvent* ev)
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
    }
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

  if ( getStatusBar() )
  {
    getStatusBar()->drawMessage();
    updateTerminal();
    flush_out();
  }

  processClick();
  ev->accept();
}

//----------------------------------------------------------------------
void FToggleButton::onFocusIn (FFocusEvent*)
{
  if ( getStatusBar() )
    getStatusBar()->drawMessage();
}

//----------------------------------------------------------------------
void FToggleButton::onFocusOut (FFocusEvent* out_ev)
{
  if ( getStatusBar() )
  {
    getStatusBar()->clearMessage();
    getStatusBar()->drawMessage();
  }

  if ( ! hasGroup() )
    return;

  if ( ! focus_inside_group && isRadioButton()  )
  {
    focus_inside_group = true;
    out_ev->ignore();

    if ( out_ev->getFocusType() == fc::FocusNextWidget )
      getGroup()->focusNextChild();

    if ( out_ev->getFocusType() == fc::FocusPreviousWidget )
      getGroup()->focusPrevChild();

    redraw();
  }
  else if ( this == getGroup()->getLastButton()
           && out_ev->getFocusType() == fc::FocusNextWidget )
  {
    out_ev->ignore();
    getGroup()->focusNextChild();
    redraw();
  }
  else if ( this == getGroup()->getFirstButton()
           && out_ev->getFocusType() == fc::FocusPreviousWidget )
  {
    out_ev->ignore();
    getGroup()->focusPrevChild();
    redraw();
  }
}


// protected methods of FToggleButton
//----------------------------------------------------------------------
uChar FToggleButton::getHotkey()
{
  uInt length;

  if ( text.isEmpty() )
    return 0;

  length = text.getLength();

  for (uInt i = 0; i < length; i++)
  {
    try
    {
      if ( i + 1 < length && text[i] == '&' )
        return uChar(text[++i]);
    }
    catch (const std::out_of_range&)
    {
      return 0;
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
    if ( std::isalpha(hotkey) || std::isdigit(hotkey) )
    {
      addAccelerator (std::tolower(hotkey));
      addAccelerator (std::toupper(hotkey));
      // Meta + hotkey
      addAccelerator (fc::Fmkey_meta + std::tolower(hotkey));
    }
    else
      addAccelerator (getHotkey());
  }
  else
    delAccelerator();
}

//----------------------------------------------------------------------
bool FToggleButton::isRadioButton() const
{
  return isInstanceOf("FRadioButton");
}

//----------------------------------------------------------------------
bool FToggleButton::isCheckboxButton() const
{
  return isInstanceOf("FCheckBox");
}

//----------------------------------------------------------------------
void FToggleButton::draw()
{
  bool isFocus = ((flags & fc::focus) != 0);

  if ( isFocus && getStatusBar() )
  {
    const FString& msg = getStatusbarMessage();
    const FString& curMsg = getStatusBar()->getMessage();

    if ( curMsg != msg )
    {
      getStatusBar()->setMessage(msg);
      getStatusBar()->drawMessage();
    }
  }

  // set the cursor to the button
  if ( isRadioButton() || isCheckboxButton() )
    setCursorPos (2, 1);
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

  try
  {
    LabelText = new wchar_t[length + 1]();
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
    return;
  }

  txt = text;
  src = const_cast<wchar_t*>(txt.wc_str());
  dest = const_cast<wchar_t*>(LabelText);
  isActive = ((flags & fc::active) != 0);
  isNoUnderline = ((flags & fc::no_underline) != 0);

  // find hotkey position in string
  // + generate a new string without the '&'-sign
  for (i = 0; i < length; i++)
  {
    if ( i < length && txt[i] == '&' && hotkeypos == -1 )
    {
      hotkeypos = int(i);
      i++;
      src++;
    }

    *dest++ = *src++;
  }

  if ( hotkeypos != -1 )
    length--;

  setPrintPos (1 + label_offset_pos, 1);

  if ( isMonochron() )
    setReverse(true);

  if ( isEnabled() )
    setColor (wc.label_fg, wc.label_bg);
  else
    setColor (wc.label_inactive_fg, wc.label_inactive_bg);

  for (int z = 0; z < int(length); z++)
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


// private methods of FToggleButton
//----------------------------------------------------------------------
void FToggleButton::setGroup (FButtonGroup* btngroup)
{
  button_group = btngroup;
}

//----------------------------------------------------------------------
void FToggleButton::init()
{
  setGeometry (1, 1, 4, 1, false);  // initialize geometry values

  if ( isEnabled() )
  {
    if ( hasFocus() )
    {
      setForegroundColor (wc.toggle_button_active_focus_fg);
      setBackgroundColor (wc.toggle_button_active_focus_bg);
    }
    else
    {
      setForegroundColor (wc.toggle_button_active_fg);
      setBackgroundColor (wc.toggle_button_active_bg);
    }
  }
  else  // inactive
  {
    setForegroundColor (wc.label_inactive_fg);
    setBackgroundColor (wc.label_inactive_bg);
  }
}
