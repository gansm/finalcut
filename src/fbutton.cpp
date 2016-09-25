// File: fbutton.cpp
// Provides: class FButton

#include "fapp.h"
#include "fbutton.h"
#include "fstatusbar.h"


//----------------------------------------------------------------------
// class FButton
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FButton::FButton(FWidget* parent)
  : FWidget(parent)
  , text()
  , button_down(false)
  , click_animation(true)
  , click_time(150)
  , button_fg(wc.button_active_fg)
  , button_bg(wc.button_active_bg)
  , button_hotkey_fg(wc.button_hotkey_fg)
  , button_focus_fg(wc.button_active_focus_fg)
  , button_focus_bg(wc.button_active_focus_bg)
  , button_inactive_fg(wc.button_inactive_fg)
  , button_inactive_bg(wc.button_inactive_bg)
{
  init();
}

//----------------------------------------------------------------------
FButton::FButton (const FString& txt, FWidget* parent)
  : FWidget(parent)
  , text(txt)
  , button_down(false)
  , click_animation(true)
  , click_time(150)
  , button_fg(wc.button_active_fg)
  , button_bg(wc.button_active_bg)
  , button_hotkey_fg(wc.button_hotkey_fg)
  , button_focus_fg(wc.button_active_focus_fg)
  , button_focus_bg(wc.button_active_focus_bg)
  , button_inactive_fg(wc.button_inactive_fg)
  , button_inactive_bg(wc.button_inactive_bg)
{
  init();
  detectHotkey();
}

//----------------------------------------------------------------------
FButton::~FButton()  // destructor
{
  delAccelerator();
  delOwnTimer();
}


// private methods of FButton
//----------------------------------------------------------------------
void FButton::init()
{
  setForegroundColor (wc.button_active_fg);
  setBackgroundColor (wc.button_active_bg);
  setShadow();

  if ( hasFocus() )
    flags = fc::focus;

  if ( isEnabled() )
    flags |= fc::active;
}

//----------------------------------------------------------------------
uChar FButton::getHotkey()
{
  int length;

  if ( text.isEmpty() )
    return 0;

  length = int(text.getLength());

  for (int i=0; i < length; i++)
  {
    try
    {
      if ( (i+1 < length) && (text[uInt(i)] == '&') )
        return uChar(text[uInt(++i)]);
    }
    catch (const std::out_of_range&)
    {
      return 0;;
    }
  }

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
inline void FButton::detectHotkey()
{
  if ( isEnabled() )
  {
    delAccelerator();
    setHotkeyAccelerator();
  }
}

//----------------------------------------------------------------------
void FButton::draw()
{
  register wchar_t* src;
  register wchar_t* dest;
  wchar_t* ButtonText;
  FString txt;
  FWidget* parent_widget = getParentWidget();
  int active_focus;
  int d, i, j, x, mono_offset, margin;
  int length, hotkeypos, hotkey_offset, space;
  bool is_ActiveFocus, is_Active, is_Focus, is_Flat;
  bool is_NonFlatShadow, is_NoUnderline;

  length = int(text.getLength());
  hotkeypos = -1;
  hotkey_offset = 0;
  space = int(' ');

  if ( isMonochron() || getMaxColor() < 16 )
    ButtonText = new wchar_t[length+3]();
  else
    ButtonText = new wchar_t[length+1]();

  txt  = FString(text);
  src  = const_cast<wchar_t*>(txt.wc_str());
  dest = const_cast<wchar_t*>(ButtonText);

  active_focus = fc::active + fc::focus;
  is_ActiveFocus = ((flags & active_focus) == active_focus);
  is_Active = ((flags & fc::active) != 0);
  is_Focus = ((flags & fc::focus) != 0);
  is_Flat = isFlat();
  is_NonFlatShadow = ((flags & (fc::shadow+fc::flat)) == fc::shadow);
  is_NoUnderline = ((flags & fc::no_underline) != 0);
  updateVTerm(false);

  if ( isMonochron() )
    setReverse(true);

  if ( button_down && click_animation )
  {
    // noshadow + indent one character to the right
    if ( is_Flat )
      clearFlatBorder();
    else
      clearShadow();

    if ( parent_widget )
      setColor ( parent_widget->getForegroundColor()
               , parent_widget->getBackgroundColor() );

    for (int y=1; y <= getHeight(); y++)
    {
      printPos (1, y);
      print (' '); // clear one left █
    }

    d = 1;
  }
  else
    d = 0;

  if ( ! is_Active && isMonochron() )
    space = fc::MediumShade;  // ▒

  if ( isMonochron() && is_ActiveFocus )
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

  if ( (length - hotkey_offset + mono_offset - hotkey_offset) <= getWidth() )
    margin = 1;
  else
    margin = 0;

  if ( isMonochron() && (is_Active || is_Focus) )
    setReverse(false);

  if ( is_Flat )
  {
    if ( margin == 1 )
    {
      setColor (getForegroundColor(), button_bg);

      for (int y=0; y < getHeight(); y++)
      {
        printPos (1+d, 1+y);
        print (space); // full block █
      }
    }

    if ( ! button_down )
      drawFlatBorder();
  }
  else if ( ! isMonochron() )
  {
    if ( parent_widget )
      setColor (button_bg, parent_widget->getBackgroundColor());

    for (int y=0; y < getHeight(); y++)
    {
      printPos (1+d, 1+y);

      // Cygwin terminal use IBM Codepage 850
      if ( isCygwinTerminal() )
        print (fc::FullBlock); // █
      else if ( isTeraTerm() )
        print (0xdb);
      else
        print (fc::RightHalfBlock); // ▐
    }
  }

  if (  ! button_down
     && ! isNewFont()
     && (is_Flat || ! hasShadow() || isMonochron()) )
  {
    // clear the right █ from button down
    if ( parent_widget )
      setColor ( parent_widget->getForegroundColor()
               , parent_widget->getBackgroundColor() );

    for (int y=1; y <= getHeight(); y++)
    {
      if ( isMonochron() )
        setReverse(true);

      printPos (1+getWidth(), y);
      print (' '); // clear right

      if ( isMonochron() )
        setReverse(false);
    }
  }

  if ( hotkeypos != -1 )
    length--;

  i = getWidth() - length - 1;
  i = int(i / 2);

  if ( getHeight() > 1 )
    j = int((getHeight()-1) / 2);
  else
    j=0;

  printPos (1+margin+d, 1+j);
  setColor (button_fg, button_bg);

  for (x=0; x < i; x++)
   print (space); // █

  if ( hotkeypos == -1 )
    setCursorPos (1+margin+i, 1+j ); // first character
  else
    setCursorPos (1+margin+i+hotkeypos, 1+j ); // hotkey

  if ( is_ActiveFocus && (isMonochron() || getMaxColor() < 16) )
    setBold();

  for (int z=0; x < i+length && z < getWidth(); z++,x++)
  {
    if ( (z == hotkeypos) && is_Active )
    {
      setColor (button_hotkey_fg, button_bg);

      if ( ! is_ActiveFocus && getMaxColor() < 16 )
        setBold();

      if ( ! is_NoUnderline )
        setUnderline();

      print ( ButtonText[z] );

      if ( ! is_ActiveFocus && getMaxColor() < 16 )
        unsetBold();

      if ( ! is_NoUnderline )
        unsetUnderline();

      setColor (button_fg, button_bg);
    }
    else
    {
      print ( ButtonText[z] );
    }
  }

  if ( is_ActiveFocus && (isMonochron() || getMaxColor() < 16) )
    unsetBold();

  for (x=i+length; x < getWidth()-1; x++)
    print (space); // █

  if ( getHeight() > 1 )
  {
    for (i=0; i < j; i++)
    {
      printPos (2+d, 1+i);

      for (int z=1; z < getWidth(); z++)
        print (space); // █
    }
    for (i=j+1; i < getHeight(); i++)
    {
      printPos (2+d, 1+i);

      for (int z=1; z < getWidth(); z++)
        print (space); // █
    }
  }

  if ( isMonochron() )
    setReverse(true);

  if ( is_NonFlatShadow && ! button_down )
  {
    if ( parent_widget )
      setColor ( parent_widget->getForegroundColor()
               , parent_widget->getBackgroundColor() );

    print(' '); // restore background after button down
    drawShadow();
  }

  if ( isMonochron() )
    setReverse(false);

  updateVTerm(true);
  delete[] ButtonText;

  if ( is_Focus && statusBar() )
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
void FButton::updateButtonColor()
{
  if ( isEnabled() )
  {
    if ( hasFocus() )
    {
      button_fg = button_focus_fg;
      button_bg = button_focus_bg;
    }
    else
    {
      button_fg = getForegroundColor();
      button_bg = getBackgroundColor() ;
    }
  }
  else  // inactive
  {
    button_fg = button_inactive_fg;
    button_bg = button_inactive_bg;
  }
}

//----------------------------------------------------------------------
void FButton::processClick()
{
  emitCallback("clicked");
}


// public methods of FButton
//----------------------------------------------------------------------
void FButton::setForegroundColor (short color)
{
  FWidget::setForegroundColor(color);
  updateButtonColor();
}

//----------------------------------------------------------------------
void FButton::setBackgroundColor (short color)
{
  FWidget::setBackgroundColor(color);
  updateButtonColor();
}

//----------------------------------------------------------------------
void FButton::setHotkeyForegroundColor (short color)
{
  // valid colors -1..254
  if ( color == fc::Default || color >> 8 == 0 )
    button_hotkey_fg = color;
}

void FButton::setFocusForegroundColor (short color)
{
  // valid colors -1..254
  if ( color == fc::Default || color >> 8 == 0 )
    button_focus_fg = color;

  updateButtonColor();
}

//----------------------------------------------------------------------
void FButton::setFocusBackgroundColor (short color)
{
  // valid colors -1..254
  if ( color == fc::Default || color >> 8 == 0 )
    button_focus_bg = color;

  updateButtonColor();
}

//----------------------------------------------------------------------
void FButton::setInactiveForegroundColor (short color)
{
  // valid colors -1..254
  if ( color == fc::Default || color >> 8 == 0 )
    button_inactive_fg = color;

  updateButtonColor();
}

//----------------------------------------------------------------------
void FButton::setInactiveBackgroundColor (short color)
{
  // valid colors -1..254
  if ( color == fc::Default || color >> 8 == 0 )
    button_inactive_bg = color;

  updateButtonColor();
}

//----------------------------------------------------------------------
void FButton::hide()
{
  int s, f, size;
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
  f = isFlat() ? 1 : 0;
  size = getWidth() + s + (f << 1);

  if ( size < 0 )
    return;

  blank = new char[size+1];
  memset(blank, ' ', uLong(size));
  blank[size] = '\0';

  for (int y=0; y < getHeight()+s+(f << 1); y++)
  {
    printPos (1-f, 1+y-f);
    print (blank);
  }

  delete[] blank;
}

//----------------------------------------------------------------------
bool FButton::setNoUnderline (bool on)
{
  if ( on )
    flags |= fc::no_underline;
  else
    flags &= ~fc::no_underline;

  return on;
}

//----------------------------------------------------------------------
bool FButton::setEnable (bool on)
{
  FWidget::setEnable(on);

  if ( on )
  {
    flags |= fc::active;
    setHotkeyAccelerator();
  }
  else
  {
    flags &= ~fc::active;
    delAccelerator();
  }

  updateButtonColor();
  return on;
}

//----------------------------------------------------------------------
bool FButton::setFocus (bool on)
{
  FWidget::setFocus(on);

  if ( on )
  {
    flags |= fc::focus;

    if ( isEnabled() )
    {
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

    if ( isEnabled() && statusBar() )
      statusBar()->clearMessage();
  }

  updateButtonColor();
  return on;
}

//----------------------------------------------------------------------
bool FButton::setFlat (bool on)
{
  if ( on )
    flags |= fc::flat;
  else
    flags &= ~fc::flat;
  return on;
}

//----------------------------------------------------------------------
bool FButton::setShadow (bool on)
{
  if ( on
     && (Encoding != fc::VT100 || isTeraTerm() )
     && Encoding != fc::ASCII )
    flags |= fc::shadow;
  else
    flags &= ~fc::shadow;

  return on;
}

//----------------------------------------------------------------------
bool FButton::setDown (bool on)
{
  if ( button_down != on )
  {
    button_down = on;
    redraw();
  }

  return on;
}

//----------------------------------------------------------------------
void FButton::onKeyPress (FKeyEvent* ev)
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
      if ( click_animation )
      {
        setDown();
        addTimer(click_time);
      }
      processClick();
      ev->accept();
      break;

    default:
      break;
  }
}

//----------------------------------------------------------------------
void FButton::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton )
  {
    setUp();
    return;
  }

  if ( ! hasFocus() )
  {
    FWidget* focused_widget = getFocusWidget();
    FFocusEvent out (fc::FocusOut_Event);
    FApplication::queueEvent(focused_widget, &out);
    setFocus();

    if ( focused_widget )
      focused_widget->redraw();

    if ( statusBar() )
      statusBar()->drawMessage();
  }

  FPoint tPos = ev->getTermPos();

  if ( getTermGeometry().contains(tPos) )
    setDown();
}

//----------------------------------------------------------------------
void FButton::onMouseUp (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton )
    return;

  if ( button_down )
  {
    setUp();

    if ( getTermGeometry().contains(ev->getTermPos()) )
      processClick();
  }
}

//----------------------------------------------------------------------
void FButton::onMouseMove (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton )
    return;

  FPoint tPos = ev->getTermPos();

  if ( click_animation )
  {
    if ( getTermGeometry().contains(tPos) )
      setDown();
    else
      setUp();
  }
}

//----------------------------------------------------------------------
void FButton::onTimer (FTimerEvent* ev)
{
  delTimer(ev->timerId());
  setUp();
}

//----------------------------------------------------------------------
void FButton::onAccel (FAccelEvent* ev)
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

      if ( click_animation )
        setDown();
      else
        redraw();

      if ( statusBar() )
        statusBar()->drawMessage();
    }
    else if ( click_animation )
      setDown();

    if ( click_animation )
      addTimer(click_time);

    processClick();
    ev->accept();
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
  if ( txt )
    text = txt;
  else
    text = "";

  detectHotkey();
}
