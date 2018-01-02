/***********************************************************************
* fbutton.cpp - Widget FButton                                         *
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
#include "final/fbutton.h"
#include "final/fstatusbar.h"


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
  , indent(0)
  , space(int(' '))
  , center_offset(0)
  , vcenter_offset(0)
  , txtlength(0)
  , hotkeypos(-1)
  , button_fg(wc.button_active_fg)
  , button_bg(wc.button_active_bg)
  , button_hotkey_fg(wc.button_hotkey_fg)
  , button_focus_fg(wc.button_active_focus_fg)
  , button_focus_bg(wc.button_active_focus_bg)
  , button_inactive_fg(wc.button_inactive_fg)
  , button_inactive_bg(wc.button_inactive_bg)
  , is()
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
  , indent(0)
  , space(int(' '))
  , center_offset(0)
  , vcenter_offset(0)
  , txtlength(0)
  , hotkeypos(-1)
  , button_fg(wc.button_active_fg)
  , button_bg(wc.button_active_bg)
  , button_hotkey_fg(wc.button_hotkey_fg)
  , button_focus_fg(wc.button_active_focus_fg)
  , button_focus_bg(wc.button_active_focus_bg)
  , button_inactive_fg(wc.button_inactive_fg)
  , button_inactive_bg(wc.button_inactive_bg)
  , is()
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

//----------------------------------------------------------------------
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
    setHotkeyAccelerator();
  else
    delAccelerator();

  updateButtonColor();
  return on;
}

//----------------------------------------------------------------------
bool FButton::setFocus (bool on)
{
  FWidget::setFocus(on);

  if ( on )
  {
    if ( isEnabled() )
    {
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
    if ( isEnabled() && getStatusBar() )
      getStatusBar()->clearMessage();
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
    && term_encoding != fc::VT100
    && term_encoding != fc::ASCII )
  {
    flags |= fc::shadow;
    setShadowSize(1,1);
  }
  else
  {
    flags &= ~fc::shadow;
    setShadowSize(0,0);
  }

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
void FButton::setText (const FString& txt)
{
  if ( txt )
    text = txt;
  else
    text = "";

  detectHotkey();
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

  for (int y = 0; y < getHeight() + s + (f << 1); y++)
  {
    setPrintPos (1 - f, 1 + y - f);
    print (blank);
  }

  delete[] blank;
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

    if ( getStatusBar() )
      getStatusBar()->drawMessage();
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

      if ( click_animation )
        setDown();
      else
        redraw();

      if ( getStatusBar() )
        getStatusBar()->drawMessage();
    }
  }
  else if ( click_animation )
    setDown();

  if ( click_animation )
    addTimer(click_time);

  processClick();
  ev->accept();
}

//----------------------------------------------------------------------
void FButton::onFocusIn (FFocusEvent*)
{
  if ( getStatusBar() )
    getStatusBar()->drawMessage();
}

//----------------------------------------------------------------------
void FButton::onFocusOut (FFocusEvent*)
{
  if ( getStatusBar() )
  {
    getStatusBar()->clearMessage();
    getStatusBar()->drawMessage();
  }
}


// private methods of FButton
//----------------------------------------------------------------------
void FButton::init()
{
  setForegroundColor (wc.button_active_fg);
  setBackgroundColor (wc.button_active_bg);
  setShadow();
}

//----------------------------------------------------------------------
void FButton::getButtonState()
{
  int active_focus = fc::active + fc::focus;
  is.active_focus = ((flags & active_focus) == active_focus);
  is.active = ((flags & fc::active) != 0);
  is.focus = ((flags & fc::focus) != 0);
  is.flat = isFlat();
  is.non_flat_shadow = ((flags & (fc::shadow + fc::flat)) == fc::shadow);
  is.no_underline = ((flags & fc::no_underline) != 0);
}

//----------------------------------------------------------------------
uChar FButton::getHotkey()
{
  int length;

  if ( text.isEmpty() )
    return 0;

  length = int(text.getLength());

  for (int i = 0; i < length; i++)
  {
    try
    {
      if ( i + 1 < length && text[uInt(i)] == '&' )
        return uChar(text[uInt(++i)]);
    }
    catch (const std::out_of_range&)
    {
      return 0;
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
inline void FButton::detectHotkey()
{
  if ( isEnabled() )
  {
    delAccelerator();
    setHotkeyAccelerator();
  }
}

//----------------------------------------------------------------------
int FButton::getHotkeyPos (wchar_t src[], wchar_t dest[], uInt length)
{
  // find hotkey position in string
  // + generate a new string without the '&'-sign
  int pos = -1;
  wchar_t* txt = src;

  for (uInt i = 0; i < length; i++)
  {
    if ( i < length && txt[i] == L'&' && pos == -1 )
    {
      pos = int(i);
      i++;
      src++;
    }

    *dest++ = *src++;
  }

  return pos;
}

//----------------------------------------------------------------------
inline int FButton::clickAnimationIndent (FWidget* parent_widget)
{
  if ( ! button_down || ! click_animation )
    return 0;

  // noshadow + indent one character to the right
  if ( is.flat )
    clearFlatBorder();
  else
    clearShadow();

  if ( parent_widget )
    setColor ( parent_widget->getForegroundColor()
             , parent_widget->getBackgroundColor() );

  for (int y = 1; y <= getHeight(); y++)
  {
    setPrintPos (1, y);
    print (' ');  // clear one left █
  }

  return 1;
}

//----------------------------------------------------------------------
inline void FButton::clearRightMargin (FWidget* parent_widget)
{
  if ( button_down
    || isNewFont()
    || ( ! is.flat && hasShadow() && ! isMonochron()) )
    return;

  // Restore the right background after button down
  if ( parent_widget )
    setColor ( parent_widget->getForegroundColor()
             , parent_widget->getBackgroundColor() );

  for (int y = 1; y <= getHeight(); y++)
  {
    if ( isMonochron() )
      setReverse(true);  // Light background

    setPrintPos (1 + getWidth(), y);
    print (' ');  // clear right

    if ( is.active && isMonochron() )
      setReverse(false);  // Dark background
  }
}

//----------------------------------------------------------------------
inline void FButton::drawMarginLeft()
{
  // Print left margin

  setColor (getForegroundColor(), button_bg);

  for (int y = 0; y < getHeight(); y++)
  {
    setPrintPos (1 + indent, 1 + y);

    if ( isMonochron() && is.active_focus && y == vcenter_offset )
      print (fc::BlackRightPointingPointer);  // ►
    else
      print (space);  // full block █
  }
}

//----------------------------------------------------------------------
inline void FButton::drawMarginRight()
{
  // Print right margin

  for (int y = 0; y < getHeight(); y++)
  {
    setPrintPos (getWidth() + indent, 1 + y);

    if ( isMonochron() && is.active_focus && y == vcenter_offset )
      print (fc::BlackLeftPointingPointer);   // ◄
    else
      print (space);  // full block █
  }
}

//----------------------------------------------------------------------
inline void FButton::drawTopBottomBackground()
{
  // Print top and bottom button background

  if ( getHeight() < 2 )
    return;

  for (int y = 0; y < vcenter_offset; y++)
  {
    setPrintPos (2 + indent, 1 + y);

    for (int x = 1; x < getWidth() - 1; x++)
      print (space);  // █
  }

  for (int y = vcenter_offset + 1; y < getHeight(); y++)
  {
    setPrintPos (2 + indent, 1 + y);

    for (int x = 1; x < getWidth() - 1; x++)
      print (space);  // █
  }
}

//----------------------------------------------------------------------
inline void FButton::drawButtonTextLine (wchar_t button_text[])
{
  int pos;
  center_offset = int((getWidth() - txtlength - 1) / 2);
  setPrintPos (2 + indent, 1 + vcenter_offset);
  setColor (button_fg, button_bg);

  // Print button text line --------
  for (pos = 0; pos < center_offset; pos++)
    print (space);  // █

  if ( hotkeypos == -1 )
    setCursorPos ( 2 + center_offset
                 , 1 + vcenter_offset );  // first character
  else
    setCursorPos ( 2 + center_offset + hotkeypos
                 , 1 + vcenter_offset );  // hotkey

  if ( ! is.active && isMonochron() )
   setReverse(true);  // Light background

  if ( is.active_focus && (isMonochron() || getMaxColor() < 16) )
    setBold();

  for ( int z = 0
      ; pos < center_offset + txtlength && z < getWidth() - 2
      ; z++, pos++)
  {
    if ( (z == hotkeypos) && is.active )
    {
      setColor (button_hotkey_fg, button_bg);

      if ( ! is.active_focus && getMaxColor() < 16 )
        setBold();

      if ( ! is.no_underline )
        setUnderline();

      print (button_text[z]);

      if ( ! is.active_focus && getMaxColor() < 16 )
        unsetBold();

      if ( ! is.no_underline )
        unsetUnderline();

      setColor (button_fg, button_bg);
    }
    else
    {
      print (button_text[z]);
    }
  }

  if ( txtlength >= getWidth() - 1 )
  {
    // Print ellipsis
    setPrintPos (getWidth() + indent - 2, 1);
    print (L"..");
  }

  if ( is.active_focus && (isMonochron() || getMaxColor() < 16) )
    unsetBold();

  for (pos = center_offset + txtlength; pos < getWidth() - 2; pos++)
    print (space);  // █
}

//----------------------------------------------------------------------
void FButton::draw()
{
  wchar_t* button_text;
  FWidget* parent_widget = getParentWidget();
  txtlength = int(text.getLength());
  space = int(' ');
  getButtonState();

  try
  {
    button_text = new wchar_t[txtlength + 1]();
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
    return;
  }

  if ( isMonochron() )
    setReverse(true);  // Light background

  // Click animation preprocessing
  indent = clickAnimationIndent (parent_widget);

  // Clear right margin after animation
  clearRightMargin (parent_widget);

  if ( ! is.active && isMonochron() )
    space = fc::MediumShade;  // ▒ simulates greyed out at Monochron

  if ( isMonochron() && (is.active || is.focus) )
    setReverse(false);  // Dark background

  if ( is.flat && ! button_down )
    drawFlatBorder();

  hotkeypos = getHotkeyPos(text.wc_str(), button_text, uInt(txtlength));

  if ( hotkeypos != -1 )
    txtlength--;

  if ( getHeight() >= 2 )
    vcenter_offset = int((getHeight() - 1) / 2);
  else
    vcenter_offset = 0;

  // Print left margin
  drawMarginLeft();

  // Print button text line
  drawButtonTextLine(button_text);

  // Print right margin
  drawMarginRight();

  // Print top and bottom button background
  drawTopBottomBackground();

  // Draw button shadow
  if ( is.non_flat_shadow && ! button_down )
    drawShadow();

  if ( isMonochron() )
    setReverse(false);  // Dark background

  delete[] button_text;
  updateStatusBar();
}

//----------------------------------------------------------------------
void FButton::updateStatusBar()
{
  if ( ! is.focus || ! getStatusBar() )
    return;

  const FString& msg = getStatusbarMessage();
  const FString& curMsg = getStatusBar()->getMessage();

  if ( curMsg != msg )
  {
    getStatusBar()->setMessage(msg);
    getStatusBar()->drawMessage();
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
      button_bg = getBackgroundColor();
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
