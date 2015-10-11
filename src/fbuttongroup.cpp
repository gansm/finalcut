// File: fbuttongroup.cpp
// Provides: class FButtonGroup

#include "fapp.h"
#include "fbuttongroup.h"
#include "fstatusbar.h"
#include "ftogglebutton.h"


//----------------------------------------------------------------------
// class FButtonGroup
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FButtonGroup::FButtonGroup(FWidget* parent)
  : FWidget(parent)
  , text()
  , border(true)
  , buttonlist()
{
  init();
}

//----------------------------------------------------------------------
FButtonGroup::FButtonGroup (const FString& txt, FWidget* parent)
  : FWidget(parent)
  , text(txt)
  , border(true)
  , buttonlist()
{
  init();
  setText(txt);
}

//----------------------------------------------------------------------
FButtonGroup::~FButtonGroup()  // destructor
{
  FButtonGroup::FButtonList::iterator iter;

  if ( buttonlist.empty() )
    return;

  iter = buttonlist.begin();

  while ( iter != buttonlist.end() )
  {
    (*iter)->setGroup(0);
    iter = buttonlist.erase(iter);
  }
}

// private methods of FButtonGroup
//----------------------------------------------------------------------
void FButtonGroup::init()
{
  top_padding    = 1;
  left_padding   = 1;
  bottom_padding = 1;
  right_padding  = 1;

  if ( isEnabled() )
    flags |= ACTIVE;

  foregroundColor = wc.label_fg;
  backgroundColor = wc.label_bg;

  buttonlist.clear();  // no buttons yet
}

//----------------------------------------------------------------------
bool FButtonGroup::isRadioButton(FToggleButton* button) const
{
  return bool ( strcmp ( button->getClassName()
                       , const_cast<char*>("FRadioButton") ) == 0 );
}

//----------------------------------------------------------------------
void FButtonGroup::directFocus()
{
  if ( ! hasFocusedButton() )
  {
    bool found_checked = false;

    if ( hasCheckedButton() && ! buttonlist.empty() )
    {
      FButtonGroup::FButtonList::const_iterator iter, end;
      iter = buttonlist.begin();
      end = buttonlist.end();

      while ( iter != end )
      {
        if ( (*iter)->isChecked() )
        {
          if ( isRadioButton(*iter) )
          {
            found_checked = true;
            FWidget* focused_widget = getFocusWidget();
            FFocusEvent out (FocusOut_Event);
            FApplication::queueEvent(focused_widget, &out);
            (*iter)->setFocus();
            if ( focused_widget )
              focused_widget->redraw();
            getFocusWidget()->redraw();
          }
          break;
        }
        ++iter;
      }
    }

    if ( ! found_checked )
    {
      FWidget* focused_widget = getFocusWidget();
      FFocusEvent out (FocusOut_Event);
      FApplication::queueEvent(focused_widget, &out);
      focusFirstChild();
      if ( focused_widget )
        focused_widget->redraw();
      getFocusWidget()->redraw();
    }
  }

  if ( statusBar() )
  {
    statusBar()->drawMessage();
    updateTerminal();
    flush_out();
  }
}

// protected methods of FButtonGroup
//----------------------------------------------------------------------
void FButtonGroup::draw()
{
  setUpdateVTerm(false);
  if ( isMonochron() )
    setReverse(true);
  setColor (foregroundColor, backgroundColor);
  if ( border )
    drawBorder();
  drawLabel();
  if ( isMonochron() )
    setReverse(false);
  setUpdateVTerm(true);
}

//----------------------------------------------------------------------
uChar FButtonGroup::getHotkey()
{
  uInt length;

  if ( text.isEmpty() )
    return 0;

  length = text.getLength();

  for (uInt i=0; i < length; i++)
  {
    try
    {
      if ( (i+1 < length) && (text[i] == '&') )
        return uChar(text[++i]);
    }
    catch (const std::out_of_range&)
    {
      return 0;;
    }
  }
  return 0;
}

//----------------------------------------------------------------------
void FButtonGroup::setHotkeyAccelerator()
{
  int hotkey = getHotkey();

  if ( hotkey )
  {
    if ( isalpha(hotkey) || isdigit(hotkey) )
    {
      addAccelerator (tolower(hotkey), this);
      addAccelerator (toupper(hotkey), this);
      // Meta + hotkey
      addAccelerator (0x20000e0 + tolower(hotkey), this);
    }
    else
      addAccelerator (getHotkey(), this);
  }
  else
    delAccelerator(this);
}

//----------------------------------------------------------------------
void FButtonGroup::drawLabel()
{
  wchar_t* LabelText;
  register wchar_t* src;
  register wchar_t* dest;
  FString txt;
  uInt length;
  int hotkeypos;
  bool isActive, isNoUnderline;

  if ( text.isNull() || text.isEmpty() )
    return;

  txt = " " + text + " ";
  length = txt.getLength();
  hotkeypos = -1;
  LabelText = new wchar_t[length+1];
  src = const_cast<wchar_t*>(txt.wc_str());
  dest = const_cast<wchar_t*>(LabelText);

  isActive = ((flags & ACTIVE) != 0);
  isNoUnderline = ((flags & NO_UNDERLINE) != 0);

  // find hotkey position in string
  // + generate a new string without the '&'-sign
  for (uInt i=0; i < length; i++)
  {
    if ( (i < length) && (txt[i] == '&') && (hotkeypos == -1) )
    {
      hotkeypos = int(i);
      i++;
      src++;
    }
    *dest++ = *src++;
  }

  if ( hotkeypos != -1 )
    length--;

  if ( border )
    gotoxy (xpos+xmin, ypos+ymin-1);
  else
    gotoxy (xpos+xmin-2, ypos+ymin-1);

  if ( isEnabled() )
    setColor(wc.label_emphasis_fg, wc.label_bg);
  else
    setColor(wc.label_inactive_fg, wc.label_inactive_bg);

  for (int z=0; z < int(length); z++)
  {
    if ( (z == hotkeypos) && isActive )
    {
      setColor (wc.label_hotkey_fg, wc.label_hotkey_bg);
      if ( ! isNoUnderline )
        setUnderline();
      print ( LabelText[z] );
      if ( ! isNoUnderline )
        unsetUnderline();
      setColor (wc.label_emphasis_fg, wc.label_bg);
    }
    else
      print ( LabelText[z] );
  }
  delete[] LabelText;
}

// public methods of FButtonGroup
//----------------------------------------------------------------------
void FButtonGroup::hide()
{
  int fg, bg, size;
  char* blank;
  FWidget::hide();
  if ( ! buttonlist.empty() )
  {
    FButtonGroup::FButtonList::const_iterator iter, end;
    iter = buttonlist.begin();
    end = buttonlist.end();

    while ( iter != end )
    {
      (*iter)->hide();
      ++iter;
    }
  }

  fg = parentWidget()->getForegroundColor();
  bg = parentWidget()->getBackgroundColor();
  setColor (fg, bg);

  size = width;
  blank = new char[size+1];
  memset(blank, ' ', uLong(size));
  blank[size] = '\0';
  for (int y=0; y < height; y++)
  {
    gotoxy (xpos+xmin-1, ypos+ymin-1+y);
    print (blank);
  }
  delete[] blank;
}

//----------------------------------------------------------------------
void FButtonGroup::insert(FToggleButton* button)
{
  if ( button->group() )
    button->group()->remove(button);

  // setChecked the first FRadioButton
  if ( buttonlist.size() == 1 && isRadioButton(*buttonlist.begin()) )
    (*buttonlist.begin())->setChecked();

  button->setGroup(this);
  buttonlist.push_back(button);

  button->addCallback
  (
    "toggled",
    _METHOD_CALLBACK (this, &FButtonGroup::cb_buttonToggled)
  );
}

//----------------------------------------------------------------------
void FButtonGroup::remove(FToggleButton* button)
{
  FButtonGroup::FButtonList::iterator iter;

  if ( buttonlist.empty() )
    return;

  iter = buttonlist.begin();

  while ( iter != buttonlist.end() )
  {
    if ( (*iter) == button )
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
void FButtonGroup::cb_buttonToggled (FWidget* widget, void*)
{
  FToggleButton* button = static_cast<FToggleButton*>(widget);
  FButtonGroup::FButtonList::const_iterator iter, end;

  if ( ! button->isChecked() )
    return;
  if ( buttonlist.empty() )
    return;

  iter = buttonlist.begin();
  end = buttonlist.end();

  while ( iter != end )
  {
    if (  (*iter) != button
       && (*iter)->isChecked()
       && isRadioButton(*iter) )
    {
      (*iter)->unsetChecked();
      if ( (*iter)->isVisible() && (*iter)->isShown() )
        (*iter)->redraw();
    }
    ++iter;
  }
}

//----------------------------------------------------------------------
FToggleButton* FButtonGroup::getFirstButton()
{
  if ( ! buttonlist.empty() )
  {
    FButtonGroup::FButtonList::const_iterator iter, end;
    iter = buttonlist.begin();
    end = buttonlist.end();

    while ( iter != end )
    {
      if ( (*iter)->isEnabled() && (*iter)->acceptFocus() )
        return (*iter);
      ++iter;
    }
  }
  return 0;
}

//----------------------------------------------------------------------
FToggleButton* FButtonGroup::getLastButton()
{
  if ( ! buttonlist.empty() )
  {
    FButtonGroup::FButtonList::const_iterator iter, begin;
    begin = buttonlist.begin();
    iter = buttonlist.end();
    do
    {
      --iter;
      if ( (*iter)->isEnabled() && (*iter)->acceptFocus() )
        return (*iter);
    }
    while ( iter != begin );
  }
  return 0;
}

//----------------------------------------------------------------------
bool FButtonGroup::hasFocusedButton()
{
  if ( ! buttonlist.empty() )
  {
    FButtonGroup::FButtonList::const_iterator iter, end;
    iter = buttonlist.begin();
    end = buttonlist.end();

    while ( iter != end )
    {
      if ( (*iter)->hasFocus() )
        return true;
      ++iter;
    }
  }
  return false;
}

//----------------------------------------------------------------------
bool FButtonGroup::hasCheckedButton()
{
  if ( ! buttonlist.empty() )
  {
    FButtonGroup::FButtonList::const_iterator iter, end;
    iter = buttonlist.begin();
    end = buttonlist.end();

    while ( iter != end )
    {
      if ( (*iter)->isChecked() )
        return true;
      ++iter;
    }
  }
  return false;
}

//----------------------------------------------------------------------
void FButtonGroup::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != LeftButton )
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
    FButtonGroup::FButtonList::const_iterator iter, end;
    iter = buttonlist.begin();
    end = buttonlist.end();

    while ( iter != end )
    {
      if ( (*iter)->isChecked() )
      {
        if ( isRadioButton(*iter) )
        {
          in_ev->ignore();
          FWidget* prev_element = getFocusWidget();
          (*iter)->setFocus();
          prev_element->redraw();
          (*iter)->redraw();
        }
        break;
      }
      ++iter;
    }
  }
  if ( in_ev->isAccepted() )
  {
    if ( in_ev->getFocusType() == FocusNextWidget )
    {
      in_ev->ignore();
      FWidget* prev_element = getFocusWidget();
      focusFirstChild();
      prev_element->redraw();
      getFocusWidget()->redraw();
    }
    else if ( in_ev->getFocusType() == FocusPreviousWidget )
    {
      in_ev->ignore();
      FWidget* prev_element = getFocusWidget();
      focusLastChild();
      prev_element->redraw();
      getFocusWidget()->redraw();
    }
  }
  if ( statusBar() )
  {
    statusBar()->drawMessage();
    updateTerminal();
    flush_out();
  }
}

//----------------------------------------------------------------------
void FButtonGroup::onFocusOut (FFocusEvent*)
{ }

//----------------------------------------------------------------------
bool FButtonGroup::setEnable (bool on)
{
  FWidget::setEnable(on);

  if ( on )
  {
    flags |= ACTIVE;
    setHotkeyAccelerator();
  }
  else
  {
    flags &= ~ACTIVE;
    delAccelerator (this);
  }
  return on;
}

//----------------------------------------------------------------------
bool FButtonGroup::setBorder(bool on)
{
  if ( on )
    border = true;
  else
    border = false;
  return on;
}

//----------------------------------------------------------------------
void FButtonGroup::setText (const FString& txt)
{
  text = txt;
  if ( isEnabled() )
  {
    delAccelerator (this);
    setHotkeyAccelerator();
  }
}
