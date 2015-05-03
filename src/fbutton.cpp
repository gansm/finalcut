// fbutton.cpp
// class FButton

#include "fapp.h"
#include "fbutton.h"
#include "fstatusbar.h"


//----------------------------------------------------------------------
// class FButton
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FButton::FButton(FWidget* parent) : FWidget(parent)
{
  this->init();
}

//----------------------------------------------------------------------
FButton::FButton (const FString& txt, FWidget* parent) : FWidget(parent)
{
  this->init();
  setText(txt);
}

//----------------------------------------------------------------------
FButton::~FButton()  // destructor
{
  delAccelerator (this);
}


// private methods of FButton
//----------------------------------------------------------------------
void FButton::init()
{
  flags = 0;
  button_down = false;
  this->text = "";

  if ( hasFocus() )
    this->flags = FOCUS;

  if ( isEnabled() )
  {
    this->flags |= ACTIVE;

    if ( hasFocus() )
    {
      foregroundColor = wc.button_active_focus_fg;
      backgroundColor = wc.button_active_focus_bg;
    }
    else
    {
      foregroundColor = wc.button_active_fg;
      backgroundColor = wc.button_active_bg;
    }
  }
  else  // inactive
  {
    foregroundColor = wc.button_inactive_fg;
    backgroundColor = wc.button_inactive_bg;
  }
}

//----------------------------------------------------------------------
uChar FButton::getHotkey()
{
  int length;

  if ( text.isEmpty() )
    return 0;

  length = int(text.getLength());

  for (int i=0; i < length; i++)
    if ( (i+1 < length) && (text[uInt(i)] == '&') )
      return uChar(text[uInt(++i)]);
  return 0;
}

//----------------------------------------------------------------------
void FButton::setHotkeyAccelerator()
{
  int hotkey = getHotkey();
  if ( hotkey )
  {
    if ( isalpha(hotkey) || isdigit(hotkey) )
    {
      addAccelerator (tolower(hotkey), this);
      addAccelerator (toupper(hotkey), this);
      // Meta + hotkey
      addAccelerator (0x20000e0+tolower(hotkey), this);
    }
    else
      addAccelerator (getHotkey(), this);
  }
  else
    delAccelerator(this);
}

//----------------------------------------------------------------------
void FButton::draw()
{
  register wchar_t* src;
  register wchar_t* dest;
  wchar_t* ButtonText;
  FString txt;
  int d, i, j, x, mono_offset, margin;
  int length, hotkeypos, hotkey_offset, space;
  int button_fg, button_bg;
  bool isActiveFocus, isActive, isFocus, isFlat;
  bool isNonFlatShadow, isNoUnderline;

  if ( text.isNull() || text.isEmpty() )
    return;

  length = int(text.getLength());
  hotkeypos = -1;
  hotkey_offset = 0;
  space = int(' ');

  if ( isMonochron() )
    ButtonText = new wchar_t[length+3];
  else
    ButtonText = new wchar_t[length+1];

  txt  = this->text;
  src  = const_cast<wchar_t*>(txt.wc_str());
  dest = const_cast<wchar_t*>(ButtonText);

  isActiveFocus = (flags & (ACTIVE+FOCUS)) == (ACTIVE+FOCUS);
  isActive = ((flags & ACTIVE) != 0);
  isFocus = ((flags & FOCUS) != 0);
  isFlat = ((flags & FLAT) != 0);
  isNonFlatShadow = ((flags & (SHADOW+FLAT)) == SHADOW);
  isNoUnderline = ((flags & NO_UNDERLINE) != 0);

  setUpdateVTerm(false);
  if ( button_down )
  {
    // noshadow + indent one character to the right
    if ( isFlat )
      clearFlatBorder();
    clearShadow();
    setColor ( parentWidget()->getForegroundColor(),
               parentWidget()->getBackgroundColor() );
    for (int y=1; y <= height; y++)
    {
      gotoxy (xpos+xmin-1, ypos+ymin-2+y);
      print (' '); // clear one left █
    }
    d = 1;
  }
  else
    d = 0;

  if ( ! isActive && (isMonochron() || getMaxColor() < 16) )
    space = fc::MediumShade;  // ▒

  if ( isMonochron() && isActiveFocus )
  {
    txt = "<" + txt + ">";
    length = int(txt.getLength());
    src = const_cast<wchar_t*>(txt.wc_str());
    mono_offset = 2;
  }
  else
    mono_offset = 0;

  // find hotkey position in string
  // + generate a new string without the '&'-sign
  for (i=0; i < length; i++)
  {
    if ( (i < length) && (txt[uInt(i)] == '&') && (hotkeypos == -1) )
    {
      hotkeypos = i;
      i++;
      src++;
    }
    *dest++ = *src++;
  }
  if ( hotkeypos != -1 )
    hotkey_offset = 1;

  if ( (length - hotkey_offset + mono_offset - hotkey_offset) <= width )
    margin = 1;
  else
    margin = 0;

  button_fg = foregroundColor;
  button_bg = backgroundColor;

  if ( isMonochron() && (isActive || isFocus) )
    setReverse(true);

  if ( isFlat )
  {
    if ( margin == 1 )
    {
      setColor (wc.button_active_fg, button_bg);
      for (int y=0; y <= height-1; y++)
      {
        gotoxy (xpos+xmin-1+d, ypos+ymin-1+y);
        print (space); // full block █
      }
    }
    if ( ! button_down )
      drawFlatBorder();
  }
  else if ( ! isMonochron() )
  {
    if ( ! isActive && getMaxColor() < 16 )
      setColor (button_fg, button_bg);
    else
      setColor (button_bg, parentWidget()->getBackgroundColor());
    gotoxy (xpos+xmin-1+d, ypos+ymin-1);
    for (int y=1; y <= height; y++)
    {
       // Cygwin terminal use IBM Codepage 850
      if ( this->isCygwinTerminal() )
        print (fc::FullBlock); // █
      else
        print (fc::RightHalfBlock); // ▐
      gotoxy (xpos+xmin-1+d, ypos+ymin-1+y);
    }
  }

  if (  ! button_down
     && ! isNewFont()
     && (isFlat || ! hasShadow() || isMonochron()) )
  {
    // clear the right █ from button down
    setColor ( parentWidget()->getForegroundColor(),
               parentWidget()->getBackgroundColor() );
    for (int y=1; y <= height; y++)
    {
      if ( isMonochron() )
        setReverse(false);
      gotoxy (xpos+xmin-1+width, ypos+ymin-2+y);
      print (' '); // clear right
      if ( isMonochron() )
        setReverse(true);
    }
  }

  if ( hotkeypos != -1 )
    length--;

  i = width - length - 1;
  i = int(i / 2);

  if ( height > 1 )
    j = int((height-1) / 2);
  else
    j=0;

  gotoxy (xpos+xmin-1+margin+d, ypos+ymin-1+j);
  setColor (button_fg, button_bg);

  for (x=0; x < i; x++)
    print (space); // █

  if ( hotkeypos == -1 )
    setCursorPos ( xpos+xmin-1+margin+i,
                   ypos+ymin-1+j ); // first character
  else
    setCursorPos ( xpos+xmin-1+margin+i+hotkeypos,
                   ypos+ymin-1+j ); // hotkey

  if ( isMonochron() && isActiveFocus )
    setBold();
  for (int z=0; x < i+length && z < width; z++,x++)
  {
    if ( (z == hotkeypos) && isActive )
    {
      setColor (wc.button_hotkey_fg, button_bg);
      if ( ! isNoUnderline )
        setUnderline();
      print ( ButtonText[z] );
      if ( ! isNoUnderline )
        unsetUnderline();
      setColor (button_fg, button_bg);
    }
    else
    {
      print ( ButtonText[z] );
    }
  }
  if ( isMonochron() && isActiveFocus )
    unsetBold();
  for (x=i+length; x < width-1; x++)
    print (space); // █

  if ( height > 1 )
  {
    for (i=0; i < j; i++)
    {
      gotoxy (xpos+xmin+d, ypos+ymin-1+i);
      for (int z=1; z <= width-1; z++)
        print (space); // █
    }
    for (i=j+1; i < height; i++)
    {
      gotoxy (xpos+xmin+d, ypos+ymin-1+i);
      for (int z=1; z <= width-1; z++)
        print (space); // █
    }
  }

  if ( isMonochron() )
    setReverse(false);

  if ( isNonFlatShadow && ! button_down )
  {
    setColor ( parentWidget()->getForegroundColor(),
               parentWidget()->getBackgroundColor() );
    print(' '); // restore background after button down
    drawShadow();
  }

  setUpdateVTerm(true);
  delete[] ButtonText;

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
void FButton::processClick()
{
  emitCallback("clicked");
}


// public methods of FButton
//----------------------------------------------------------------------
void FButton::hide()
{
  int fg, bg, s, f, size;
  char* blank;

  FWidget::hide();

  fg = parentWidget()->getForegroundColor();
  bg = parentWidget()->getBackgroundColor();
  setColor (fg, bg);

  s = hasShadow() ? 1 : 0;
  f = isFlat() ? 1 : 0;
  size = width + s + (f << 1);
  blank = new char[size+1];
  memset(blank, ' ', uLong(size));
  blank[size] = '\0';
  for (int y=0; y < height+s+(f << 1); y++)
  {
    gotoxy (xpos+xmin-1-f, ypos+ymin-1+y-f);
    print (blank);
  }
  delete[] blank;
}

//----------------------------------------------------------------------
bool FButton::setNoUnderline(bool on)
{
  if ( on )
    this->flags |= NO_UNDERLINE;
  else
    this->flags &= ~NO_UNDERLINE;
  return on;
}

//----------------------------------------------------------------------
bool FButton::setEnable(bool on)
{
  FWidget::setEnable(on);

  if ( on )
  {
    this->flags |= ACTIVE;
    setHotkeyAccelerator();
    if ( hasFocus() )
    {
      foregroundColor = wc.button_active_focus_fg;
      backgroundColor = wc.button_active_focus_bg;
    }
    else
    {
      foregroundColor = wc.button_active_fg;
      backgroundColor = wc.button_active_bg;
    }
  }
  else
  {
    this->flags &= ~ACTIVE;
    delAccelerator (this);
    foregroundColor = wc.button_inactive_fg;
    backgroundColor = wc.button_inactive_bg;
  }
  return on;
}

//----------------------------------------------------------------------
bool FButton::setFocus(bool on)
{
  FWidget::setFocus(on);

  if ( on )
  {
    this->flags |= FOCUS;

    if ( isEnabled() )
    {
      foregroundColor = wc.button_active_focus_fg;
      backgroundColor = wc.button_active_focus_bg;
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
      foregroundColor = wc.button_active_fg;
      backgroundColor = wc.button_active_bg;
      if ( statusBar() )
        statusBar()->clearMessage();
    }
  }
  return on;
}

//----------------------------------------------------------------------
bool FButton::setFlat(bool on)
{
  if ( on )
    this->flags |= FLAT;
  else
    this->flags &= ~FLAT;
  return on;
}

//----------------------------------------------------------------------
bool FButton::setShadow(bool on)
{
  if ( on && Encoding != fc::VT100 && Encoding != fc::ASCII )
    this->flags |= SHADOW;
  else
    this->flags &= ~SHADOW;
  return on;
}

//----------------------------------------------------------------------
bool FButton::setDown(bool on)
{
  if ( button_down != on )
  {
    button_down = on;
    redraw();
  }
  return on;
}

//----------------------------------------------------------------------
void FButton::onKeyPress (FKeyEvent* event)
{
  int key;

  if ( ! isEnabled() )
    return;

  key = event->key();

  switch ( key )
  {
    case fc::Fkey_return:
    case fc::Fkey_enter:
    case fc::Fkey_space:
      setDown();
      usleep(150000);
      setUp();
      processClick();
      event->accept();
      break;
  }
}

//----------------------------------------------------------------------
void FButton::onMouseDown (FMouseEvent* event)
{
  if ( event->getButton() != LeftButton )
  {
    setUp();
    return;
  }

  if ( ! hasFocus() )
  {
    FWidget* focused_widget = getFocusWidget();
    FFocusEvent out (FocusOut_Event);
    FApplication::queueEvent(focused_widget, &out);
    this->setFocus();
    if ( focused_widget )
      focused_widget->redraw();
    if ( statusBar() )
      statusBar()->drawMessage();
  }
  FPoint gPos = event->getGlobalPos();
  if ( getGeometryGlobal().contains(gPos) )
    setDown();
}

//----------------------------------------------------------------------
void FButton::onMouseUp (FMouseEvent* event)
{
  if ( event->getButton() != LeftButton )
    return;

  if ( button_down )
  {
    setUp();
    if ( getGeometryGlobal().contains(event->getGlobalPos()) )
      processClick();
  }
}

//----------------------------------------------------------------------
void FButton::onMouseMove (FMouseEvent* event)
{
  if ( event->getButton() != LeftButton )
    return;

  FPoint gPos = event->getGlobalPos();
  if ( getGeometryGlobal().contains(gPos) )
    setDown();
  else
    setUp();
}

//----------------------------------------------------------------------
void FButton::onAccel (FAccelEvent* event)
{
  if ( isEnabled() )
  {
    if ( ! hasFocus() )
    {
      FWidget* focused_widget = static_cast<FWidget*>(event->focusedWidget());
      FFocusEvent out (FocusOut_Event);
      FApplication::queueEvent(focused_widget, &out);
      setFocus();
      if ( focused_widget )
        focused_widget->redraw();
      setDown();
      if ( statusBar() )
        statusBar()->drawMessage();
    }
    else
      setDown();
    usleep(150000);
    setUp();
    processClick();
    event->accept();
  }
}

//----------------------------------------------------------------------
void FButton::onFocusIn (FFocusEvent*)
{
  if ( statusBar() )
    statusBar()->drawMessage();
}

//----------------------------------------------------------------------
void FButton::onFocusOut (FFocusEvent*)
{
  if ( statusBar() )
  {
    statusBar()->clearMessage();
    statusBar()->drawMessage();
  }
}

//----------------------------------------------------------------------
void FButton::setText (const FString& txt)
{
  this->text = txt;
  if ( isEnabled() )
  {
    delAccelerator (this);
    setHotkeyAccelerator();
  }
}
