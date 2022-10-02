/***********************************************************************
* fbutton.cpp - Widget FButton                                         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2012-2022 Markus Gans                                      *
*                                                                      *
* FINAL CUT is free software; you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as       *
* published by the Free Software Foundation; either version 3 of       *
* the License, or (at your option) any later version.                  *
*                                                                      *
* FINAL CUT is distributed in the hope that it will be useful, but     *
* WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU Lesser General Public License for more details.                  *
*                                                                      *
* You should have received a copy of the GNU Lesser General Public     *
* License along with this program.  If not, see                        *
* <http://www.gnu.org/licenses/>.                                      *
***********************************************************************/

#include "final/fapplication.h"
#include "final/fevent.h"
#include "final/vterm/fcolorpair.h"
#include "final/widget/fbutton.h"
#include "final/widget/fstatusbar.h"

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
FButton::FButton (FString&& txt, FWidget* parent)
  : FWidget{parent}
  , text{std::move(txt)}
{
  init();
}

//----------------------------------------------------------------------
FButton::~FButton()  // destructor
{
  delAccelerator();
  delOwnTimers();
}

// FButton operator
//----------------------------------------------------------------------
auto FButton::operator = (const FString& s) -> FButton&
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
  if ( color == FColor::Default || (color >> 8) == FColor::Black )
    button_hotkey_fg = color;
}

//----------------------------------------------------------------------
void FButton::setFocusForegroundColor (FColor color)
{
  // valid colors -1..254
  if ( color == FColor::Default || (color >> 8) == FColor::Black )
    button_focus_fg = color;

  updateButtonColor();
}

//----------------------------------------------------------------------
void FButton::setFocusBackgroundColor (FColor color)
{
  // valid colors -1..254
  if ( color == FColor::Default || (color >> 8) == FColor::Black )
    button_focus_bg = color;

  updateButtonColor();
}

//----------------------------------------------------------------------
void FButton::setInactiveForegroundColor (FColor color)
{
  // valid colors -1..254
  if ( color == FColor::Default || (color >> 8) == FColor::Black )
    button_inactive_fg = color;

  updateButtonColor();
}

//----------------------------------------------------------------------
void FButton::setInactiveBackgroundColor (FColor color)
{
  // valid colors -1..254
  if ( color == FColor::Default || (color >> 8) == FColor::Black )
    button_inactive_bg = color;

  updateButtonColor();
}

//----------------------------------------------------------------------
void FButton::resetColors()
{
  const auto& wc = getColorTheme();
  FButton::setForegroundColor (wc->button_active_fg);
  FButton::setBackgroundColor (wc->button_active_bg);
  FButton::setHotkeyForegroundColor (wc->button_hotkey_fg);
  FButton::setFocusForegroundColor (wc->button_active_focus_fg);
  FButton::setFocusBackgroundColor (wc->button_active_focus_bg);
  FButton::setInactiveForegroundColor (wc->button_inactive_fg);
  FButton::setInactiveBackgroundColor (wc->button_inactive_bg);
  FWidget::resetColors();
}

//----------------------------------------------------------------------
auto FButton::setNoUnderline (bool enable) -> bool
{
  return (setFlags().no_underline = enable);
}

//----------------------------------------------------------------------
auto FButton::setEnable (bool enable) -> bool
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
auto FButton::setFocus (bool enable) -> bool
{
  FWidget::setFocus(enable);
  updateButtonColor();
  return enable;
}

//----------------------------------------------------------------------
auto FButton::setFlat (bool enable) -> bool
{
  return (setFlags().flat = enable);
}

//----------------------------------------------------------------------
auto FButton::setShadow (bool enable) -> bool
{
  return setWidgetShadow(this, enable);
}

//----------------------------------------------------------------------
auto FButton::setDown (bool enable) -> bool
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
  text.setString(txt);
  detectHotkey();
}

//----------------------------------------------------------------------
void FButton::hide()
{
  const auto& parent_widget = getParentWidget();
  FWidget::hide();

  if ( parent_widget )
  {
    FColor fg = parent_widget->getForegroundColor();
    FColor bg = parent_widget->getBackgroundColor();
    setColor (fg, bg);
  }
  else
  {
    const auto& wc = getColorTheme();
    FColor fg = wc->dialog_fg;
    FColor bg = wc->dialog_bg;
    setColor (fg, bg);
  }

  const std::size_t s = hasShadow() ? 1 : 0;
  const std::size_t f = isFlat() ? 1 : 0;
  const std::size_t size = getWidth() + s + (f << 1u);

  if ( size == 0 )
    return;

  for (std::size_t y{0}; y < getHeight() + s + (f << 1u); y++)
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

  if ( isEnterKey(key) || key == FKey::Space )
  {
    if ( click_animation )
    {
      setDown();
      addTimer(click_time);
    }

    processClick();
    ev->accept();
  }
}

//----------------------------------------------------------------------
void FButton::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != MouseButton::Left )
  {
    setUp();
    return;
  }

  setWidgetFocus(this);
  const FPoint tPos{ev->getTermPos()};

  if ( getTermGeometry().contains(tPos) )
    setDown();
}

//----------------------------------------------------------------------
void FButton::onMouseUp (FMouseEvent* ev)
{
  if ( ev->getButton() != MouseButton::Left )
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
  if ( ev->getButton() != MouseButton::Left )
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
void FButton::onWheel (FWheelEvent*)
{
  setUp();
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
    auto focused_widget = ev->focusedWidget();

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
  button_fg = wc->button_active_fg;
  button_bg = wc->button_active_bg;
  FButton::resetColors();
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
inline auto FButton::clickAnimationIndent (const FWidget* parent_widget) -> std::size_t
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

  for (auto y{1}; y <= int(getHeight()); y++)
  {
    print() << FPoint{1, y} << ' ';  // clear one left █
  }

  return 1;
}

//----------------------------------------------------------------------
inline void FButton::clearRightMargin (const FWidget* parent_widget)
{
  if ( button_down || FVTerm::getFOutput()->isNewFont() )
    return;

  // Restore the right background after button down
  if ( parent_widget )
    setColor ( parent_widget->getForegroundColor()
             , parent_widget->getBackgroundColor() );

  for (auto y{1}; y <= int(getHeight()); y++)
  {
    if ( FVTerm::getFOutput()->isMonochron() )
      setReverse(true);  // Light background

    print() << FPoint{1 + int(getWidth()), y} << ' ';  // clear right

    if ( getFlags().active && FVTerm::getFOutput()->isMonochron() )
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

    if ( FVTerm::getFOutput()->isMonochron() && active_focus && y == vcenter_offset )
      print (UniChar::BlackRightPointingPointer);  // ►
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

    if ( FVTerm::getFOutput()->isMonochron() && active_focus && y == vcenter_offset )
      print (UniChar::BlackLeftPointingPointer);   // ◄
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

  if ( ! getFlags().active && FVTerm::getFOutput()->isMonochron() )
    setReverse(true);  // Light background

  if ( active_focus && (FVTerm::getFOutput()->isMonochron()
                     || FVTerm::getFOutput()->getMaxColor() < 16) )
    setBold();

  while ( pos < center_offset + column_width && columns + 2 < getWidth() )
  {
    if ( z == hotkeypos && getFlags().active )
    {
      setColor (button_hotkey_fg, button_bg);

      if ( ! active_focus && FVTerm::getFOutput()->getMaxColor() < 16 )
        setBold();

      if ( ! getFlags().no_underline )
        setUnderline();

      print (button_text[z]);

      if ( ! active_focus && FVTerm::getFOutput()->getMaxColor() < 16 )
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

  if ( active_focus && (FVTerm::getFOutput()->isMonochron()
                     || FVTerm::getFOutput()->getMaxColor() < 16) )
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
  space_char = L' ';
  active_focus = getFlags().active && getFlags().focus;

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(true);  // Light background

  // Click animation preprocessing
  indent = clickAnimationIndent (parent_widget);

  // Clear right margin after animation
  clearRightMargin (parent_widget);

  if ( ! getFlags().active && FVTerm::getFOutput()->isMonochron() )
    space_char = wchar_t(UniChar::MediumShade);  // ▒ simulates greyed out at Monochron

  if ( FVTerm::getFOutput()->isMonochron() && (getFlags().active || getFlags().focus) )
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

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);  // Dark background

  updateStatusBar();
}

//----------------------------------------------------------------------
void FButton::updateStatusBar() const
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
void FButton::processClick() const
{
  emitCallback("clicked");
}

}  // namespace finalcut
