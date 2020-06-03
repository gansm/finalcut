/***********************************************************************
* fbutton.cpp - Widget FButton                                         *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2012-2020 Markus Gans                                      *
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
#include "final/fcolorpair.h"
#include "final/fevent.h"
#include "final/fbutton.h"
#include "final/fstatusbar.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FButton
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FButton::FButton(FWidget* parent)
  : FWidget{parent}
{
  init();
}

//----------------------------------------------------------------------
FButton::FButton (const FString& txt, FWidget* parent)
  : FWidget{parent}
  , text{txt}
{
  init();
}

//----------------------------------------------------------------------
FButton::~FButton()  // destructor
{
  delAccelerator();
  delOwnTimer();
}

// FButton operator
//----------------------------------------------------------------------
FButton& FButton::operator = (const FString& s)
{
  setText(s);
  return *this;
}

// public methods of FButton
//----------------------------------------------------------------------
void FButton::setForegroundColor (FColor color)
{
  FWidget::setForegroundColor(color);
  updateButtonColor();
}

//----------------------------------------------------------------------
void FButton::setBackgroundColor (FColor color)
{
  FWidget::setBackgroundColor(color);
  updateButtonColor();
}

//----------------------------------------------------------------------
void FButton::setHotkeyForegroundColor (FColor color)
{
  // valid colors -1..254
  if ( color == fc::Default || color >> 8 == 0 )
    button_hotkey_fg = color;
}

//----------------------------------------------------------------------
void FButton::setFocusForegroundColor (FColor color)
{
  // valid colors -1..254
  if ( color == fc::Default || color >> 8 == 0 )
    button_focus_fg = color;

  updateButtonColor();
}

//----------------------------------------------------------------------
void FButton::setFocusBackgroundColor (FColor color)
{
  // valid colors -1..254
  if ( color == fc::Default || color >> 8 == 0 )
    button_focus_bg = color;

  updateButtonColor();
}

//----------------------------------------------------------------------
void FButton::setInactiveForegroundColor (FColor color)
{
  // valid colors -1..254
  if ( color == fc::Default || color >> 8 == 0 )
    button_inactive_fg = color;

  updateButtonColor();
}

//----------------------------------------------------------------------
void FButton::setInactiveBackgroundColor (FColor color)
{
  // valid colors -1..254
  if ( color == fc::Default || color >> 8 == 0 )
    button_inactive_bg = color;

  updateButtonColor();
}

//----------------------------------------------------------------------
void FButton::resetColors()
{
  const auto& wc = getColorTheme();
  setForegroundColor (wc->button_active_fg);
  setBackgroundColor (wc->button_active_bg);
  setHotkeyForegroundColor (wc->button_hotkey_fg);
  setFocusForegroundColor (wc->button_active_focus_fg);
  setFocusBackgroundColor (wc->button_active_focus_bg);
  setInactiveForegroundColor (wc->button_inactive_fg);
  setInactiveBackgroundColor (wc->button_inactive_bg);
  FWidget::resetColors();
}

//----------------------------------------------------------------------
bool FButton::setNoUnderline (bool enable)
{
  return (setFlags().no_underline = enable);
}

//----------------------------------------------------------------------
bool FButton::setEnable (bool enable)
{
  FWidget::setEnable(enable);

  if ( enable )
    setHotkeyAccelerator();
  else
    delAccelerator();

  updateButtonColor();
  return enable;
}

//----------------------------------------------------------------------
bool FButton::setFocus (bool enable)
{
  FWidget::setFocus(enable);
  updateButtonColor();
  return enable;
}

//----------------------------------------------------------------------
bool FButton::setFlat (bool enable)
{
  return (setFlags().flat = enable);
}

//----------------------------------------------------------------------
bool FButton::setShadow (bool enable)
{
  if ( enable
    && FTerm::getEncoding() != fc::VT100
    && FTerm::getEncoding() != fc::ASCII )
  {
    setFlags().shadow = true;
    setShadowSize(FSize{1, 1});
  }
  else
  {
    setFlags().shadow = false;
    setShadowSize(FSize{0, 0});
  }

  return getFlags().shadow;
}

//----------------------------------------------------------------------
bool FButton::setDown (bool enable)
{
  if ( button_down != enable )
  {
    button_down = enable;
    redraw();
  }

  return enable;
}

//----------------------------------------------------------------------
void FButton::setText (const FString& txt)
{
  if ( txt.isNull() )
    text.setString("");
  else
    text.setString(txt);

  detectHotkey();
}

//----------------------------------------------------------------------
void FButton::hide()
{
  FColor fg{};
  FColor bg{};
  auto parent_widget = getParentWidget();
  FWidget::hide();

  if ( parent_widget )
  {
    fg = parent_widget->getForegroundColor();
    bg = parent_widget->getBackgroundColor();
  }
  else
  {
    const auto& wc = getColorTheme();
    fg = wc->dialog_fg;
    bg = wc->dialog_bg;
  }

  setColor (fg, bg);
  const std::size_t s = hasShadow() ? 1 : 0;
  const std::size_t f = isFlat() ? 1 : 0;
  const std::size_t size = getWidth() + s + (f << 1);

  if ( size == 0 )
    return;

  for (std::size_t y{0}; y < getHeight() + s + (f << 1); y++)
  {
    print() << FPoint{1 - int(f), 1 + int(y - f)}
            << FString{size, L' '};
  }
}

//----------------------------------------------------------------------
void FButton::onKeyPress (FKeyEvent* ev)
{
  if ( ! isEnabled() )
    return;

  const FKey key = ev->key();

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
    auto focused_widget = getFocusWidget();
    setFocus();

    if ( focused_widget )
      focused_widget->redraw();

    if ( getStatusBar() )
      getStatusBar()->drawMessage();
  }

  const FPoint tPos{ev->getTermPos()};

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

  const FPoint tPos{ev->getTermPos()};

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
  delTimer(ev->getTimerId());
  setUp();
}

//----------------------------------------------------------------------
void FButton::onAccel (FAccelEvent* ev)
{
  if ( ! isEnabled() )
    return;

  if ( ! hasFocus() )
  {
    auto focused_widget = static_cast<FWidget*>(ev->focusedWidget());

    if ( focused_widget && focused_widget->isWidget() )
    {
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
  const auto& wc = getColorTheme();
  setForegroundColor (wc->button_active_fg);
  setBackgroundColor (wc->button_active_bg);
  setShadow();

  if ( ! text.isEmpty() )
    detectHotkey();
}

//----------------------------------------------------------------------
void FButton::setHotkeyAccelerator()
{
  setHotkeyViaString (this, text);
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
inline std::size_t FButton::clickAnimationIndent (const FWidget* parent_widget)
{
  if ( ! button_down || ! click_animation )
    return 0;

  // noshadow + indent one character to the right
  if ( getFlags().flat )
    clearFlatBorder(this);
  else if ( hasShadow() )
    clearShadow(this);

  if ( parent_widget )
    setColor ( parent_widget->getForegroundColor()
             , parent_widget->getBackgroundColor() );

  for (int y{1}; y <= int(getHeight()); y++)
  {
    print() << FPoint{1, y} << ' ';  // clear one left █
  }

  return 1;
}

//----------------------------------------------------------------------
inline void FButton::clearRightMargin (const FWidget* parent_widget)
{
  if ( button_down || FTerm::isNewFont() )
    return;

  // Restore the right background after button down
  if ( parent_widget )
    setColor ( parent_widget->getForegroundColor()
             , parent_widget->getBackgroundColor() );

  for (int y{1}; y <= int(getHeight()); y++)
  {
    if ( FTerm::isMonochron() )
      setReverse(true);  // Light background

    print() << FPoint{1 + int(getWidth()), y} << ' ';  // clear right

    if ( getFlags().active && FTerm::isMonochron() )
      setReverse(false);  // Dark background
  }
}

//----------------------------------------------------------------------
inline void FButton::drawMarginLeft()
{
  // Print left margin

  setColor (getForegroundColor(), button_bg);

  for (std::size_t y{0}; y < getHeight(); y++)
  {
    print() << FPoint{1 + int(indent), 1 + int(y)};

    if ( FTerm::isMonochron() && active_focus && y == vcenter_offset )
      print (fc::BlackRightPointingPointer);  // ►
    else
      print (space_char);  // full block █
  }
}

//----------------------------------------------------------------------
inline void FButton::drawMarginRight()
{
  // Print right margin

  for (std::size_t y{0}; y < getHeight(); y++)
  {
    print() << FPoint{int(getWidth() + indent), 1 + int(y)};

    if ( FTerm::isMonochron() && active_focus && y == vcenter_offset )
      print (fc::BlackLeftPointingPointer);   // ◄
    else
      print (space_char);  // full block █
  }
}

//----------------------------------------------------------------------
inline void FButton::drawTopBottomBackground()
{
  // Print top and bottom button background

  if ( getHeight() < 2 )
    return;

  for (std::size_t y{0}; y < vcenter_offset; y++)
  {
    print() << FPoint{2 + int(indent), 1 + int(y)};

    for (std::size_t x{1}; x < getWidth() - 1; x++)
      print (space_char);  // █
  }

  for (std::size_t y{vcenter_offset + 1}; y < getHeight(); y++)
  {
    print() << FPoint{2 + int(indent), 1 + int(y)};

    for (std::size_t x{1}; x < getWidth() - 1; x++)
      print (space_char);  // █
  }
}

//----------------------------------------------------------------------
inline void FButton::drawButtonTextLine (const FString& button_text)
{
  std::size_t z{0};
  std::size_t pos{};
  std::size_t columns{0};
  print() << FPoint{2 + int(indent), 1 + int(vcenter_offset)}
          << FColorPair{button_fg, button_bg};

  if ( getWidth() < column_width + 1 )
    center_offset = 0;
  else
    center_offset = (getWidth() - column_width - 1) / 2;

  // Print button text line
  for (pos = 0; pos < center_offset; pos++)
    print (space_char);  // █

  if ( hotkeypos == NOT_SET )
    setCursorPos ({ 2 + int(center_offset)
                  , 1 + int(vcenter_offset) });  // first character
  else
    setCursorPos ({ 2 + int(center_offset + hotkeypos)
                  , 1 + int(vcenter_offset) });  // hotkey

  if ( ! getFlags().active && FTerm::isMonochron() )
    setReverse(true);  // Light background

  if ( active_focus && (FTerm::isMonochron() || FTerm::getMaxColor() < 16) )
    setBold();

  while ( pos < center_offset + column_width && columns + 2 < getWidth() )
  {
    if ( z == hotkeypos && getFlags().active )
    {
      setColor (button_hotkey_fg, button_bg);

      if ( ! active_focus && FTerm::getMaxColor() < 16 )
        setBold();

      if ( ! getFlags().no_underline )
        setUnderline();

      print (button_text[z]);

      if ( ! active_focus && FTerm::getMaxColor() < 16 )
        unsetBold();

      if ( ! getFlags().no_underline )
        unsetUnderline();

      setColor (button_fg, button_bg);
    }
    else
    {
      print (button_text[z]);
    }

    const auto char_width = getColumnWidth(button_text[z]);
    columns += char_width;
    pos += char_width;
    z++;
  }

  if ( column_width + 1 >= getWidth() )
  {
    // Print ellipsis
    print() << FPoint{int(getWidth() + indent) - 2, 1} << "..";
  }

  if ( active_focus && (FTerm::isMonochron() || FTerm::getMaxColor() < 16) )
    unsetBold();

  for (pos = center_offset + column_width; pos < getWidth() - 2; pos++)
    print (space_char);  // █
}

//----------------------------------------------------------------------
void FButton::draw()
{
  FString button_text{};
  const auto& parent_widget = getParentWidget();
  column_width = getColumnWidth(text);
  space_char = int(' ');
  active_focus = getFlags().active && getFlags().focus;

  if ( FTerm::isMonochron() )
    setReverse(true);  // Light background

  // Click animation preprocessing
  indent = clickAnimationIndent (parent_widget);

  // Clear right margin after animation
  clearRightMargin (parent_widget);

  if ( ! getFlags().active && FTerm::isMonochron() )
    space_char = fc::MediumShade;  // ▒ simulates greyed out at Monochron

  if ( FTerm::isMonochron() && (getFlags().active || getFlags().focus) )
    setReverse(false);  // Dark background

  if ( getFlags().flat && ! button_down )
    drawFlatBorder(this);

  hotkeypos = finalcut::getHotkeyPos(text, button_text);

  if ( hotkeypos != NOT_SET )
    column_width--;

  if ( getHeight() >= 2 )
    vcenter_offset = (getHeight() - 1) / 2;
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
  if ( ! getFlags().flat && getFlags().shadow && ! button_down )
    drawShadow(this);

  if ( FTerm::isMonochron() )
    setReverse(false);  // Dark background

  updateStatusBar();
}

//----------------------------------------------------------------------
void FButton::updateStatusBar()
{
  if ( ! getFlags().focus || ! getStatusBar() )
    return;

  const auto& msg = getStatusbarMessage();
  const auto& curMsg = getStatusBar()->getMessage();

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

}  // namespace finalcut
