/***********************************************************************
* fbuttongroup.cpp - The FButtonGroup widget organizes FToggleButton   *
*                    widgets in a group.                               *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2014-2019 Markus Gans                                      *
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
#include "final/fbuttongroup.h"
#include "final/fstatusbar.h"
#include "final/ftogglebutton.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FButtonGroup
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FButtonGroup::FButtonGroup(FWidget* parent)
  : FScrollView(parent)
{
  init();
}

//----------------------------------------------------------------------
FButtonGroup::FButtonGroup (const FString& txt, FWidget* parent)
  : FScrollView(parent)
  , text{txt}
{
  init();
  setText(txt);
}

//----------------------------------------------------------------------
FButtonGroup::~FButtonGroup()  // destructor
{
  if ( buttonlist.empty() )
    return;

  auto iter = buttonlist.begin();

  while ( iter != buttonlist.end() )
  {
    auto toggle_button = static_cast<FToggleButton*>(*iter);
    toggle_button->setGroup(0);
    iter = buttonlist.erase(iter);
  }
}


// public methods of FButtonGroup
//----------------------------------------------------------------------
FToggleButton* FButtonGroup::getButton (int index) const
{
  if ( buttonlist.empty() )
    return 0;

  if ( index <= 0 || index > int(getCount()) )
    return 0;

  auto iter = buttonlist.begin();
  std::advance (iter, index - 1);
  return static_cast<FToggleButton*>(*iter);
}

//----------------------------------------------------------------------
FToggleButton* FButtonGroup::getFirstButton()
{
  auto widget = FWidget::getFirstFocusableWidget(buttonlist);
  auto toggle_button = static_cast<FToggleButton*>(widget);
  return toggle_button;
}

//----------------------------------------------------------------------
FToggleButton* FButtonGroup::getLastButton()
{
  auto widget = FWidget::getLastFocusableWidget(buttonlist);
  auto toggle_button = static_cast<FToggleButton*>(widget);
  return toggle_button;
}

//----------------------------------------------------------------------
bool FButtonGroup::setEnable (bool enable)
{
  FWidget::setEnable(enable);

  if ( enable )
    setHotkeyAccelerator();
  else
    delAccelerator();

  return enable;
}

//----------------------------------------------------------------------
void FButtonGroup::setText (const FString& txt)
{
  text = txt;

  if ( isEnabled() )
  {
    delAccelerator();
    setHotkeyAccelerator();
  }
}

//----------------------------------------------------------------------
bool FButtonGroup::isChecked (int index) const
{
  auto button = getButton(index);

  if ( button )
    return button->isChecked();
  else
    return false;
}

//----------------------------------------------------------------------
bool FButtonGroup::hasFocusedButton() const
{
  if ( buttonlist.empty() )
    return false;

  auto iter = buttonlist.begin();
  auto last = buttonlist.end();

  while ( iter != last )
  {
    auto toggle_button = static_cast<FToggleButton*>(*iter);

    if ( toggle_button->hasFocus() )
      return true;

    ++iter;
  }

  return false;
}

//----------------------------------------------------------------------
bool FButtonGroup::hasCheckedButton() const
{
  if ( buttonlist.empty() )
    return false;

  auto iter = buttonlist.begin();
  auto last = buttonlist.end();

  while ( iter != last )
  {
    auto toggle_button = static_cast<FToggleButton*>(*iter);

    if ( toggle_button->isChecked() )
      return true;

    ++iter;
  }

  return false;
}

//----------------------------------------------------------------------
void FButtonGroup::hide()
{
  std::size_t size;
  FColor fg, bg;
  FWidget::hide();
  auto parent_widget = getParentWidget();

  if ( ! buttonlist.empty() )
  {
    auto iter = buttonlist.begin();
    auto last = buttonlist.end();

    while ( iter != last )
    {
      auto toggle_button = static_cast<FToggleButton*>(*iter);
      toggle_button->hide();
      ++iter;
    }
  }

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

  auto blank = createBlankArray(size + 1);

  for (int y = 0; y < int(getHeight()); y++)
  {
    FWidget::setPrintPos (FPoint(1, 1 + y));
    print (blank);
  }

  destroyBlankArray (blank);
}

//----------------------------------------------------------------------
void FButtonGroup::insert (FToggleButton* button)
{
  if ( ! button )
    return;

  if ( button->getGroup() )
    button->getGroup()->remove(button);

  // setChecked the first FRadioButton
  if ( buttonlist.size() == 1 )
  {
    auto first_button = static_cast<FToggleButton*>(*buttonlist.begin());

    if ( isRadioButton(first_button) )
      first_button->setChecked();
  }

  button->setGroup(this);
  buttonlist.push_back(button);

  button->addCallback
  (
    "toggled",
    F_METHOD_CALLBACK (this, &FButtonGroup::cb_buttonToggled)
  );
}

//----------------------------------------------------------------------
void FButtonGroup::remove (FToggleButton* button)
{
  if ( ! button || buttonlist.empty() )
    return;

  auto iter = buttonlist.begin();

  while ( iter != buttonlist.end() )
  {
    auto toggle_button = static_cast<FToggleButton*>(*iter);

    if ( toggle_button == button )
    {
      iter = buttonlist.erase(iter);
      button->setGroup(0);
      button->delCallback(this);
      break;
    }
    else
      ++iter;
  }
}

//----------------------------------------------------------------------
void FButtonGroup::checkScrollSize (FToggleButton* button)
{
  // Check and adjust the scroll size

  checkScrollSize (button->getGeometry());
}

//----------------------------------------------------------------------
void FButtonGroup::checkScrollSize (const FRect& r)
{
  // Check and adjust the scroll size

  FRect scrollgeometry (1, 1, getScrollWidth(), getScrollHeight());

  if ( ! scrollgeometry.contains(r) )
  {
    FRect r_combined = scrollgeometry.combined(r);
    setScrollSize (r_combined.getSize());
  }
}

//----------------------------------------------------------------------
void FButtonGroup::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton )
    return;

  directFocus();
}

//----------------------------------------------------------------------
void FButtonGroup::onAccel (FAccelEvent*)
{
  directFocus();
}

//----------------------------------------------------------------------
void FButtonGroup::onFocusIn (FFocusEvent* in_ev)
{
  if ( hasCheckedButton() && ! buttonlist.empty() )
  {
    auto iter = buttonlist.begin();
    auto last = buttonlist.end();

    while ( iter != last )
    {
      auto toggle_button = static_cast<FToggleButton*>(*iter);

      if ( toggle_button->isChecked() )
      {
        if ( isRadioButton(toggle_button) )
        {
          auto prev_element = getFocusWidget();
          in_ev->ignore();
          toggle_button->setFocus();

          FFocusEvent cfi (fc::ChildFocusIn_Event);
          FApplication::sendEvent(this, &cfi);

          FFocusEvent in (fc::FocusIn_Event);
          FApplication::sendEvent(toggle_button, &in);

          if ( prev_element )
            prev_element->redraw();

          toggle_button->redraw();
        }

        break;
      }

      ++iter;
    }
  }

  if ( in_ev->isAccepted() )
  {
    in_ev->ignore();
    auto prev_element = getFocusWidget();

    if ( in_ev->getFocusType() == fc::FocusNextWidget )
      focusFirstChild();
    else if ( in_ev->getFocusType() == fc::FocusPreviousWidget )
      focusLastChild();

    if ( prev_element )
      prev_element->redraw();

    if ( getFocusWidget() )
      getFocusWidget()->redraw();
  }

  if ( getStatusBar() )
  {
    getStatusBar()->drawMessage();
    updateTerminal();
    flush_out();
  }
}


// protected methods of FButtonGroup
//----------------------------------------------------------------------
void FButtonGroup::setHotkeyAccelerator()
{
  FKey hotkey = getHotkey(text);

  if ( hotkey )
  {
    if ( std::isalpha(int(hotkey)) || std::isdigit(int(hotkey)) )
    {
      addAccelerator (FKey(std::tolower(int(hotkey))));
      addAccelerator (FKey(std::toupper(int(hotkey))));
      // Meta + hotkey
      addAccelerator (fc::Fmkey_meta + FKey(std::tolower(int(hotkey))));
    }
    else
      addAccelerator (hotkey);
  }
  else
    delAccelerator();
}

//----------------------------------------------------------------------
void FButtonGroup::draw()
{
  if ( isMonochron() )
    setReverse(true);

  setColor();
  clearArea();

  if ( isMonochron() )
    setReverse(false);

  FScrollView::draw();
  drawLabel();
}

//----------------------------------------------------------------------
void FButtonGroup::drawLabel()
{
  wchar_t* LabelText;

  if ( text.isNull() || text.isEmpty() )
    return;

  FString txt = " " + text + " ";
  std::size_t length = txt.getLength();

  try
  {
    LabelText = new wchar_t[length + 1]();
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
    return;
  }

  wchar_t* src = const_cast<wchar_t*>(txt.wc_str());
  wchar_t* dest = const_cast<wchar_t*>(LabelText);
  unsetViewportPrint();
  auto hotkeypos = getHotkeyPos(src, dest, length);

  if ( hotkeypos != NOT_SET )
    length--;

  if ( hasBorder() )
    FWidget::setPrintPos (FPoint(2, 1));
  else
    FWidget::setPrintPos (FPoint(0, 1));

  drawText (LabelText, hotkeypos, length);
  setViewportPrint();
  delete[] LabelText;
}


// private methods of FButtonGroup
//----------------------------------------------------------------------
bool FButtonGroup::isRadioButton (FToggleButton* button) const
{
  if ( ! button )
    return false;

  return bool ( std::strcmp ( button->getClassName()
                            , C_STR("FRadioButton") ) == 0 );
}

//----------------------------------------------------------------------
void FButtonGroup::init()
{
  setForegroundColor (wc.label_fg);
  setBackgroundColor (wc.label_bg);
  setMinimumSize (FSize(7, 4));
  buttonlist.clear();  // no buttons yet
}

//----------------------------------------------------------------------
std::size_t FButtonGroup::getHotkeyPos ( wchar_t src[]
                                       , wchar_t dest[]
                                       , std::size_t length )
{
  // find hotkey position in string
  // + generate a new string without the '&'-sign
  std::size_t pos = NOT_SET;
  wchar_t* txt = src;

  for (std::size_t i = 0; i < length; i++)
  {
    if ( i < length && txt[i] == L'&' && pos == NOT_SET )
    {
      pos = i;
      i++;
      src++;
    }

    *dest++ = *src++;
  }

  return pos;
}

//----------------------------------------------------------------------
void FButtonGroup::drawText ( wchar_t LabelText[]
                            , std::size_t hotkeypos
                            , std::size_t length )
{
  if ( isMonochron() )
    setReverse(true);

  if ( isEnabled() )
    setColor(wc.label_emphasis_fg, wc.label_bg);
  else
    setColor(wc.label_inactive_fg, wc.label_inactive_bg);

  for (std::size_t z = 0; z < length; z++)
  {
    if ( (z == hotkeypos) && flags.active )
    {
      setColor (wc.label_hotkey_fg, wc.label_hotkey_bg);

      if ( ! flags.no_underline )
        setUnderline();

      print (LabelText[z]);

      if ( ! flags.no_underline )
        unsetUnderline();

      setColor (wc.label_emphasis_fg, wc.label_bg);
    }
    else
      print (LabelText[z]);
  }

  if ( isMonochron() )
    setReverse(true);
}

//----------------------------------------------------------------------
void FButtonGroup::directFocus()
{
  if ( ! hasFocusedButton() )
  {
    bool found_checked = false;

    if ( hasCheckedButton() && ! buttonlist.empty() )
    {
      auto iter = buttonlist.begin();
      auto last = buttonlist.end();

      while ( iter != last )
      {
        auto toggle_button = static_cast<FToggleButton*>(*iter);

        if ( toggle_button->isChecked() )
        {
          if ( isRadioButton(toggle_button) )
          {
            found_checked = true;
            auto focused_widget = getFocusWidget();
            toggle_button->setFocus();

            if ( focused_widget )
              focused_widget->redraw();

            focused_widget = getFocusWidget();

            if ( focused_widget )
              focused_widget->redraw();
          }

          break;
        }

        ++iter;
      }
    }

    if ( ! found_checked )
    {
      auto focused_widget = getFocusWidget();
      focusFirstChild();

      if ( focused_widget )
        focused_widget->redraw();

      focused_widget = getFocusWidget();

      if ( focused_widget )
        focused_widget->redraw();
    }
  }

  if ( getStatusBar() )
  {
    getStatusBar()->drawMessage();
    updateTerminal();
    flush_out();
  }
}

//----------------------------------------------------------------------
void FButtonGroup::cb_buttonToggled (FWidget* widget, FDataPtr)
{
  auto button = static_cast<FToggleButton*>(widget);

  if ( ! button->isChecked() )
    return;

  if ( buttonlist.empty() )
    return;

  auto iter = buttonlist.begin();
  auto last = buttonlist.end();

  while ( iter != last )
  {
    auto toggle_button = static_cast<FToggleButton*>(*iter);

    if ( toggle_button != button
      && toggle_button->isChecked()
      && isRadioButton(toggle_button) )
    {
      toggle_button->unsetChecked();

      if ( toggle_button->isShown() )
        toggle_button->redraw();
    }

    ++iter;
  }
}

}  // namespace finalcut
