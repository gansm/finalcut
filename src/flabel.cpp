/***********************************************************************
* flabel.cpp - Widget FLabel                                           *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2014-2018 Markus Gans                                      *
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
#include "final/flabel.h"
#include "final/fstatusbar.h"


//----------------------------------------------------------------------
// class FLabel
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FLabel::FLabel(FWidget* parent)
  : FWidget(parent)
  , multiline_text()
  , multiline(false)
  , text()
  , alignment(fc::alignLeft)
  , emphasis_color(wc.label_emphasis_fg)
  , ellipsis_color(wc.label_ellipsis_fg)
  , emphasis(false)
  , reverse_mode(false)
  , accel_widget(0)
{
  init();
}

//----------------------------------------------------------------------
FLabel::FLabel (const FString& txt, FWidget* parent)
  : FWidget(parent)
  , multiline_text()
  , multiline(false)
  , text(txt)
  , alignment(fc::alignLeft)
  , emphasis_color(wc.label_emphasis_fg)
  , ellipsis_color(wc.label_ellipsis_fg)
  , emphasis(false)
  , reverse_mode(false)
  , accel_widget(0)
{
  init();
  setText(txt);
}

//----------------------------------------------------------------------
FLabel::~FLabel()  // destructor
{
  delAccelerator();
}

// FLabel operators
//----------------------------------------------------------------------
FLabel& FLabel::operator = (const FString& s)
{
  setText(s);
  return *this;
}

//----------------------------------------------------------------------
FLabel& FLabel::operator << (const FString& s)
{
  setText(text + s);
  return *this;
}

//----------------------------------------------------------------------
FLabel& FLabel::operator << (const wchar_t c)
{
  setText(text + c);
  return *this;
}

//----------------------------------------------------------------------
FLabel& FLabel::operator << (const uInt num)
{
  FString num_str;
  num_str << num;
  setText(text + num_str);
  return *this;
}

//----------------------------------------------------------------------
FLabel& FLabel::operator << (const int num)
{
  FString num_str;
  num_str << num;
  setText(text + num_str);
  return *this;
}

//----------------------------------------------------------------------
FLabel& FLabel::operator << (const uLong num)
{
  FString num_str;
  num_str << num;
  setText(text + num_str);
  return *this;
}

//----------------------------------------------------------------------
FLabel& FLabel::operator << (const long num)
{
  FString num_str;
  num_str << num;
  setText(text + num_str);
  return *this;
}

//----------------------------------------------------------------------
FLabel& FLabel::operator << (const float num)
{
  FString num_str;
  num_str << num;
  setText(text + num_str);
  return *this;
}

//----------------------------------------------------------------------
FLabel& FLabel::operator << (const double num)
{
  FString num_str;
  num_str << num;
  setText(text + num_str);
  return *this;
}

//----------------------------------------------------------------------
FLabel& FLabel::operator << (const lDouble num)
{
  FString num_str;
  num_str << num;
  setText(text + num_str);
  return *this;
}

//----------------------------------------------------------------------
const FLabel& FLabel::operator >> (FString& s)
{
  s += text;
  return *this;
}


// public methods of FLabel
//----------------------------------------------------------------------
void FLabel::setAccelWidget (FWidget* widget)
{
  if ( widget )
    accel_widget = widget;

  accel_widget->addCallback
  (
    "destroy",
    F_METHOD_CALLBACK (this, &FLabel::cb_accel_widget_destroyed)
  );
}

//----------------------------------------------------------------------
void FLabel::setAlignment (fc::text_alignment align)
{
  if ( align != fc::alignLeft
    && align != fc::alignCenter
    && align != fc::alignRight )
    alignment = fc::alignLeft;
  else
    alignment = align;
}

//----------------------------------------------------------------------
bool FLabel::setEmphasis (bool on)
{
  if ( emphasis != on )
    emphasis = on;

  return on;
}

//----------------------------------------------------------------------
bool FLabel::setReverseMode (bool on)
{
  if ( reverse_mode != on )
    reverse_mode = on;

  return on;
}

//----------------------------------------------------------------------
bool FLabel::setEnable (bool on)
{
  FWidget::setEnable(on);

  if ( on )
    setHotkeyAccelerator();
  else
    delAccelerator();

  return on;
}

//----------------------------------------------------------------------
void FLabel::setText (const FString& txt)
{
  text = txt;
  multiline_text = text.split("\r\n");

  if ( int(multiline_text.size()) > 1 )
    multiline = true;
  else
    multiline = false;

  if ( isEnabled() )
  {
    delAccelerator();
    setHotkeyAccelerator();
  }
}

//----------------------------------------------------------------------
void FLabel::hide()
{
  short fg, bg;
  int size;
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
    blank = new char[uInt(size) + 1];
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
    return;
  }

  std::memset(blank, ' ', uLong(size));
  blank[getWidth()] = '\0';
  setPrintPos (1,1);
  print (blank);
  delete[] blank;
}

//----------------------------------------------------------------------
void FLabel::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton )
    return;

  if ( ! (isEnabled() && accel_widget) )
  {
    // send click to the parent widget
    if ( FWidget* parent = getParentWidget() )
    {
      int b = ev->getButton();
      const FPoint& tp = ev->getTermPos();
      const FPoint& p = parent->termToWidgetPos(tp);

      try
      {
        FMouseEvent* _ev = new FMouseEvent (fc::MouseDown_Event, p, tp, b);
        FApplication::sendEvent (parent, _ev);
        delete _ev;
      }
      catch (const std::bad_alloc& ex)
      {
        std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
        return;
      }
    }

    return;
  }

  if ( ! accel_widget->hasFocus() )
  {
    // focus the accelerator widget
    FWidget* focused_widget = getFocusWidget();
    FFocusEvent out (fc::FocusOut_Event);
    FApplication::queueEvent(focused_widget, &out);
    accel_widget->setFocus();

    if ( focused_widget )
      focused_widget->redraw();

    accel_widget->redraw();

    if ( getStatusBar() )
    {
      accel_widget->getStatusBar()->drawMessage();
      updateTerminal();
      flush_out();
    }
  }
}

//----------------------------------------------------------------------
void FLabel::onAccel (FAccelEvent* ev)
{
  if ( ! (isEnabled() && accel_widget) )
    return;

  if ( ! accel_widget->hasFocus() )
  {
    FWidget* focused_widget = static_cast<FWidget*>(ev->focusedWidget());

    if ( focused_widget && focused_widget->isWidget() )
    {
      FFocusEvent out (fc::FocusOut_Event);
      FApplication::queueEvent(focused_widget, &out);
      accel_widget->setFocus();
      focused_widget->redraw();
      accel_widget->redraw();
      FFocusEvent in (fc::FocusIn_Event);
      FApplication::sendEvent(accel_widget, &in);

      if ( getStatusBar() )
      {
        accel_widget->getStatusBar()->drawMessage();
        updateTerminal();
        flush_out();
      }
    }
  }

  ev->accept();
}

//----------------------------------------------------------------------
void FLabel::cb_accel_widget_destroyed (FWidget*, data_ptr)
{
  accel_widget = 0;
  delAccelerator();
}


// private methods of FLabel
//----------------------------------------------------------------------
void FLabel::init()
{
  FWidget* parent_widget = getParentWidget();
  unsetFocusable();

  if ( parent_widget )
  {
    setForegroundColor (parent_widget->getForegroundColor());
    setBackgroundColor (parent_widget->getBackgroundColor());
  }
  else
  {
    setForegroundColor (wc.dialog_fg);
    setBackgroundColor (wc.dialog_bg);
  }
}

//----------------------------------------------------------------------
uChar FLabel::getHotkey()
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
int FLabel::getHotkeyPos (wchar_t src[], wchar_t dest[], uInt length)
{
  // find hotkey position in string
  // + generate a new string without the '&'-sign
  int hotkeypos = -1;
  wchar_t* txt = src;

  for (uInt i = 0; i < length; i++)
  {
    if ( i < length && txt[i] == L'&' && hotkeypos == -1 )
    {
      hotkeypos = int(i);
      i++;
      src++;
    }

    *dest++ = *src++;
  }

  return hotkeypos;
}

//----------------------------------------------------------------------
void FLabel::setHotkeyAccelerator()
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
int FLabel::getAlignOffset (int length)
{
  switch ( alignment )
  {
    case fc::alignLeft:
      return 0;

    case fc::alignCenter:
      if ( length < getWidth() )
        return int((getWidth() - length) / 2);
      else
        return 0;

    case fc::alignRight:
      if ( length < getWidth() )
        return getWidth() - length;
      else
        return 0;
  }

  return 0;
}

//----------------------------------------------------------------------
void FLabel::draw()
{
  if ( isMonochron() )
  {
    setReverse(true);

    if ( hasEmphasis() )
      setBold(true);
  }

  if ( hasEmphasis() )
    setColor (emphasis_color, getBackgroundColor());
  else
    setColor();

  // Draw the text
  if ( multiline && getHeight() >= 2 )
    drawMultiLine();
  else
    drawSingleLine();

  if ( isMonochron() )
  {
    setReverse(false);

    if ( hasEmphasis() )
      setBold(false);
  }
}

//----------------------------------------------------------------------
void FLabel::drawMultiLine()
{
  uInt y = 0;
  uInt text_lines = uInt(multiline_text.size());
  bool hotkey_printed = false;

  while ( y < text_lines && y < uInt(getHeight()) )
  {
    wchar_t* label_text;
    int  align_offset, hotkeypos = -1;
    uInt length = multiline_text[y].getLength();

    try
    {
      label_text = new wchar_t[length + 1]();
    }
    catch (const std::bad_alloc& ex)
    {
      std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
      return;
    }

    wchar_t* src  = const_cast<wchar_t*>(multiline_text[y].wc_str());
    wchar_t* dest = const_cast<wchar_t*>(label_text);

    if ( ! hotkey_printed )
      hotkeypos = getHotkeyPos(src, dest, length);
    else
      std::wcsncpy(dest, src, length);

    setPrintPos (1, 1 + int(y));

    if ( hotkeypos != -1 )
    {
      align_offset = getAlignOffset (int(length - 1));
      printLine (label_text, length - 1, hotkeypos, align_offset);
      hotkey_printed = true;
    }
    else
    {
      align_offset = getAlignOffset (int(length));
      printLine (label_text, length, -1, align_offset);
    }

    y++;
    delete[] label_text;
  }
}

//----------------------------------------------------------------------
void FLabel::drawSingleLine()
{
  wchar_t* label_text;
  int hotkeypos = -1, align_offset;
  uInt length = text.getLength();

  try
  {
    label_text = new wchar_t[length + 1]();
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
    return;
  }

  hotkeypos = getHotkeyPos (text.wc_str(), label_text, length);

  if ( hotkeypos != -1 )
    length--;

  setPrintPos (1,1);
  align_offset = getAlignOffset (int(length));
  printLine (label_text, length, hotkeypos, align_offset);
  delete[] label_text;
}

//----------------------------------------------------------------------
void FLabel::printLine ( wchar_t line[]
                       , uInt length
                       , int  hotkeypos
                       , int  align_offset )
{
  int to_char;
  bool isActive, isNoUnderline;
  isActive = ((flags & fc::active) != 0);
  isNoUnderline = ((flags & fc::no_underline) != 0);

  if ( align_offset > 0 )
    print (FString(align_offset, ' '));  // leading spaces

  if ( length <= uInt(getWidth()) )
    to_char = int(length);
  else
    to_char = getWidth() - 2;

  if ( hasReverseMode() )
    setReverse(true);

  for (int z = 0; z < to_char; z++)
  {
    if ( ! std::iswprint(wint_t(line[z])) )
    {
      if ( ! isNewFont() && ( int(line[z]) < fc::NF_rev_left_arrow2
                           || int(line[z]) > fc::NF_check_mark ) )
      {
        line[z] = L' ';
      }
    }

    if ( (z == hotkeypos) && isActive )
    {
      setColor (wc.label_hotkey_fg, wc.label_hotkey_bg);

      if ( ! isNoUnderline )
        setUnderline();

      print ( line[z] );

      if ( ! isNoUnderline )
        unsetUnderline();

      if ( hasEmphasis() )
        setColor (emphasis_color, getBackgroundColor());
      else
        setColor();
    }
    else
      print ( line[z] );
  }

  if ( length > uInt(getWidth()) )
  {
    // Print ellipsis
    setColor (ellipsis_color, getBackgroundColor());
    print ("..");
    setColor();
  }
  else if ( align_offset + to_char < getWidth() )
  {
    // Print trailing spaces
    int len = getWidth() - align_offset - to_char;
    print (FString(len, ' '));
  }

  if ( hasReverseMode() )
    setReverse(false);
}
