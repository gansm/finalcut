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

namespace finalcut
{

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
  FColor fg, bg;
  std::size_t size;
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

  if ( size == 0 )
    return;

  char* blank = createBlankArray(size + 1);
  setPrintPos (1, 1);
  print (blank);
  destroyBlankArray (blank);
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
        std::cerr << bad_alloc_str << ex.what() << std::endl;
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
  if ( text.isEmpty() )
    return 0;

  std::size_t length = text.getLength();

  for (std::size_t i = 0; i < length; i++)
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
std::size_t FLabel::getHotkeyPos ( wchar_t src[]
                                 , wchar_t dest[]
                                 , std::size_t length )
{
  // find hotkey position in string
  // + generate a new string without the '&'-sign
  std::size_t hotkeypos = NOT_SET;
  wchar_t* txt = src;

  for (std::size_t i = 0; i < length; i++)
  {
    if ( i < length && txt[i] == L'&' && hotkeypos == NOT_SET )
    {
      hotkeypos = i;
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
  uChar hotkey = getHotkey();

  if ( hotkey )
  {
    if ( std::isalpha(hotkey) || std::isdigit(hotkey) )
    {
      addAccelerator (FKey(std::tolower(hotkey)));
      addAccelerator (FKey(std::toupper(hotkey)));
      // Meta + hotkey
      addAccelerator (fc::Fmkey_meta + FKey(std::tolower(hotkey)));
    }
    else
      addAccelerator (getHotkey());
  }
  else
    delAccelerator();
}

//----------------------------------------------------------------------
std::size_t FLabel::getAlignOffset (std::size_t length)
{
  std::size_t width = std::size_t(getWidth());

  switch ( alignment )
  {
    case fc::alignLeft:
      return 0;

    case fc::alignCenter:
      if ( length < width )
        return (width - length) / 2;
      else
        return 0;

    case fc::alignRight:
      if ( length < width )
        return width - length;
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
  std::size_t y = 0;
  std::size_t text_lines = multiline_text.size();
  bool hotkey_printed = false;

  while ( y < text_lines && y < std::size_t(getHeight()) )
  {
    wchar_t* label_text;
    std::size_t hotkeypos = NOT_SET;
    std::size_t align_offset;
    std::size_t length = multiline_text[y].getLength();

    try
    {
      label_text = new wchar_t[length + 1]();
    }
    catch (const std::bad_alloc& ex)
    {
      std::cerr << bad_alloc_str << ex.what() << std::endl;
      return;
    }

    wchar_t* src  = const_cast<wchar_t*>(multiline_text[y].wc_str());
    wchar_t* dest = const_cast<wchar_t*>(label_text);

    if ( ! hotkey_printed )
      hotkeypos = getHotkeyPos(src, dest, length);
    else
      std::wcsncpy(dest, src, length);

    setPrintPos (1, 1 + int(y));

    if ( hotkeypos != NOT_SET )
    {
      align_offset = getAlignOffset(length - 1);
      printLine (label_text, length - 1, hotkeypos, align_offset);
      hotkey_printed = true;
    }
    else
    {
      align_offset = getAlignOffset(length);
      printLine (label_text, length, NOT_SET, align_offset);
    }

    y++;
    delete[] label_text;
  }
}

//----------------------------------------------------------------------
void FLabel::drawSingleLine()
{
  wchar_t* label_text;
  std::size_t hotkeypos = NOT_SET;
  std::size_t align_offset;
  std::size_t length = text.getLength();

  try
  {
    label_text = new wchar_t[length + 1]();
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
    return;
  }

  hotkeypos = getHotkeyPos (text.wc_str(), label_text, length);

  if ( hotkeypos != NOT_SET )
    length--;

  setPrintPos (1, 1);
  align_offset = getAlignOffset(length);
  printLine (label_text, length, hotkeypos, align_offset);
  delete[] label_text;
}

//----------------------------------------------------------------------
void FLabel::printLine ( wchar_t line[]
                       , std::size_t length
                       , std::size_t hotkeypos
                       , std::size_t align_offset )
{
  std::size_t to_char;
  std::size_t width = std::size_t(getWidth());

  if ( align_offset > 0 )
    print (FString(align_offset, ' '));  // leading spaces

  if ( length <= width )
    to_char = length;
  else
    to_char = width - 2;

  if ( hasReverseMode() )
    setReverse(true);

  for (std::size_t z = 0; z < to_char; z++)
  {
    if ( ! std::iswprint(wint_t(line[z])) )
    {
      if ( ! isNewFont() && ( int(line[z]) < fc::NF_rev_left_arrow2
                           || int(line[z]) > fc::NF_check_mark ) )
      {
        line[z] = L' ';
      }
    }

    if ( z == hotkeypos && flags.active )
    {
      setColor (wc.label_hotkey_fg, wc.label_hotkey_bg);

      if ( ! flags.no_underline )
        setUnderline();

      print (line[z]);

      if ( ! flags.no_underline )
        unsetUnderline();

      if ( hasEmphasis() )
        setColor (emphasis_color, getBackgroundColor());
      else
        setColor();
    }
    else
      print (line[z]);
  }

  if ( length > width )
  {
    // Print ellipsis
    setColor (ellipsis_color, getBackgroundColor());
    print ("..");
    setColor();
  }
  else if ( align_offset + to_char < width )
  {
    // Print trailing spaces
    std::size_t len = width - align_offset - to_char;
    print (FString(len, ' '));
  }

  if ( hasReverseMode() )
    setReverse(false);
}

}  // namespace finalcut
