/***********************************************************************
* flabel.cpp - Widget FLabel                                           *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2014-2020 Markus Gans                                      *
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

#include <memory>
#include <utility>

#include "final/fapplication.h"
#include "final/fcolorpair.h"
#include "final/fevent.h"
#include "final/flabel.h"
#include "final/flog.h"
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
{
  init();
}

//----------------------------------------------------------------------
FLabel::FLabel (const FString& txt, FWidget* parent)
  : FWidget(parent)
  , text(txt)
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
FLabel& FLabel::operator << (fc::SpecialCharacter c)
{
  setText(text + static_cast<wchar_t>(c));
  return *this;
}

//----------------------------------------------------------------------
FLabel& FLabel::operator << (const wchar_t c)
{
  setText(text + c);
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
    F_METHOD_CALLBACK (this, &FLabel::cb_accelWidgetDestroyed)
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
bool FLabel::setEnable (bool enable)
{
  FWidget::setEnable(enable);

  if ( enable )
    setHotkeyAccelerator();
  else
    delAccelerator();

  return enable;
}

//----------------------------------------------------------------------
void FLabel::setText (const FString& txt)
{
  text.setString(txt);
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
  FWidget::hide();
  hideArea (getSize());
}

//----------------------------------------------------------------------
void FLabel::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton )
    return;

  if ( ! (isEnabled() && accel_widget) )
  {
    // send click to the parent widget
    if ( auto parent = getParentWidget() )
    {
      const int b = ev->getButton();
      const auto& tp = ev->getTermPos();
      const auto& p = parent->termToWidgetPos(tp);

      try
      {
        const auto& _ev = \
            std::make_shared<FMouseEvent>(fc::MouseDown_Event, p, tp, b);
        FApplication::sendEvent (parent, _ev.get());
      }
      catch (const std::bad_alloc&)
      {
        badAllocOutput ("FMouseEvent");
        return;
      }
    }

    return;
  }

  if ( ! accel_widget->hasFocus() )
  {
    // focus the accelerator widget
    auto focused_widget = getFocusWidget();
    accel_widget->setFocus();

    if ( focused_widget )
      focused_widget->redraw();

    accel_widget->redraw();

    if ( getStatusBar() )
    {
      accel_widget->getStatusBar()->drawMessage();
      updateTerminal();
      flush();
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
    auto focused_widget = static_cast<FWidget*>(ev->focusedWidget());

    if ( focused_widget && focused_widget->isWidget() )
    {
      accel_widget->setFocus();
      focused_widget->redraw();
      accel_widget->redraw();
      FFocusEvent in (fc::FocusIn_Event);
      FApplication::sendEvent(accel_widget, &in);

      if ( getStatusBar() )
      {
        accel_widget->getStatusBar()->drawMessage();
        updateTerminal();
        flush();
      }
    }
  }

  ev->accept();
}

//----------------------------------------------------------------------
void FLabel::cb_accelWidgetDestroyed (const FWidget*, const FDataPtr)
{
  accel_widget = nullptr;
  delAccelerator();
}


// private methods of FLabel
//----------------------------------------------------------------------
void FLabel::init()
{
  const auto& parent_widget = getParentWidget();
  unsetFocusable();

  if ( parent_widget )
  {
    setForegroundColor (parent_widget->getForegroundColor());
    setBackgroundColor (parent_widget->getBackgroundColor());
  }
  else
  {
    const auto& wc = getFWidgetColors();
    setForegroundColor (wc.dialog_fg);
    setBackgroundColor (wc.dialog_bg);
  }
}

//----------------------------------------------------------------------
void FLabel::setHotkeyAccelerator()
{
  setHotkeyViaString (this, text);
}

//----------------------------------------------------------------------
std::size_t FLabel::getAlignOffset (const std::size_t length)
{
  const std::size_t width(getWidth());
  assert ( alignment == fc::alignLeft
        || alignment == fc::alignCenter
        || alignment == fc::alignRight );

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
  if ( text.isEmpty() )
    return;

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
  std::size_t y{0};
  const std::size_t text_lines = multiline_text.size();
  bool hotkey_printed{false};

  while ( y < text_lines && y < std::size_t(getHeight()) )
  {
    FString label_text{};
    hotkeypos = NOT_SET;
    const auto length = multiline_text[y].getLength();
    column_width = getColumnWidth(multiline_text[y]);

    if ( ! hotkey_printed )
      hotkeypos = finalcut::getHotkeyPos (multiline_text[y], label_text);
    else
      label_text = multiline_text[y];

    print() << FPoint{1, 1 + int(y)};

    if ( hotkeypos != NOT_SET )
    {
      align_offset = getAlignOffset(length - 1);
      hotkey_printed = true;
    }
    else
      align_offset = getAlignOffset(length);

    printLine (label_text);
    y++;
  }
}

//----------------------------------------------------------------------
void FLabel::drawSingleLine()
{
  FString label_text{};
  column_width = getColumnWidth(text);
  hotkeypos = finalcut::getHotkeyPos (text, label_text);

  if ( hotkeypos != NOT_SET )
    column_width--;

  print() << FPoint{1, 1};
  align_offset = getAlignOffset(column_width);
  printLine (label_text);
}

//----------------------------------------------------------------------
void FLabel::printLine (FString& line)
{
  std::size_t to_char{};
  std::size_t to_column{};
  const std::size_t width(getWidth());

  if ( align_offset > 0 )
    print (FString{align_offset, ' '});  // leading spaces

  if ( column_width <= width )
  {
    to_char = line.getLength();
    to_column = column_width;
  }
  else
  {
    to_column = ( width >= 2 ) ? width - 2 : 0;
    to_char = getLengthFromColumnWidth(line, to_column);
  }

  if ( hasReverseMode() )
    setReverse(true);

  for (std::size_t z{0}; z < to_char; z++)
  {
    if ( ! std::iswprint(std::wint_t(line[z]))
      && ! isNewFont()
      && ( line[z] < fc::NF_rev_left_arrow2
        || line[z] > fc::NF_check_mark ) )
    {
      line[z] = L' ';
    }

    if ( z == hotkeypos && getFlags().active )
    {
      const auto& wc = getFWidgetColors();
      setColor (wc.label_hotkey_fg, wc.label_hotkey_bg);

      if ( ! getFlags().no_underline )
        setUnderline();

      print (line[z]);

      if ( ! getFlags().no_underline )
        unsetUnderline();

      if ( hasEmphasis() )
        setColor (emphasis_color, getBackgroundColor());
      else
        setColor();
    }
    else
      print (line[z]);
  }

  if ( column_width > width )
  {
    // Print ellipsis
    print() << FColorPair{ellipsis_color, getBackgroundColor()}
            << FString{".."}.left(width);
    setColor();
  }
  else if ( align_offset + to_column < width )
  {
    // Print trailing spaces
    const std::size_t len = width - align_offset - to_column;
    print (FString{len, ' '});
  }

  if ( hasReverseMode() )
    setReverse(false);
}

}  // namespace finalcut
