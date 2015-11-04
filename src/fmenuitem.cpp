// File: fmenuitem.cpp
// Provides: class FMenuItem

#include "fapp.h"
#include "fmenu.h"
#include "fmenubar.h"
#include "fmenulist.h"
#include "fmenuitem.h"
#include "fstatusbar.h"

//----------------------------------------------------------------------
// class FMenuItem
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FMenuItem::FMenuItem (FWidget* parent)
  : FWidget(parent)
  , text()
  , selected(false)
  , separator(false)
  , checked(false)
  , text_length(0)
  , hotkey(0)
//, accel_key(0)
  , menu(0)
  , super_menu(0)
{
  init (parent);
}

//----------------------------------------------------------------------
FMenuItem::FMenuItem (FString& txt, FWidget* parent)
  : FWidget(parent)
  , text(txt)
  , selected(false)
  , separator(false)
  , checked(false)
  , text_length(0)
  , hotkey(0)
//, accel_key(0)
  , menu(0)
  , super_menu(0)
{
  init (parent);
}

//----------------------------------------------------------------------
FMenuItem::FMenuItem (const std::string& txt, FWidget* parent)
  : FWidget(parent)
  , text(txt)
  , selected(false)
  , separator(false)
  , checked(false)
  , text_length(0)
  , hotkey(0)
//, accel_key(0)
  , menu(0)
  , super_menu(0)
{
  init (parent);
}

//----------------------------------------------------------------------
FMenuItem::FMenuItem (const char* txt, FWidget* parent)
  : FWidget(parent)
  , text(txt)
  , selected(false)
  , separator(false)
  , checked(false)
  , text_length(0)
  , hotkey(0)
//, accel_key(0)
  , menu(0)
  , super_menu(0)
{
  init (parent);
}

//----------------------------------------------------------------------
FMenuItem::~FMenuItem()  // destructor
{ }


// private methods of FMenuItem
//----------------------------------------------------------------------
void FMenuItem::init (FWidget* parent)
{
  text_length = text.getLength();
  hotkey = hotKey();
  if ( hotkey )
    text_length--;
  setGeometry (1,1,int(text_length+2),1, false);

  if ( parent )
  {
    setSuperMenu(parent);
    FMenuList* sm_list = dynamic_cast<FMenuList*>(parent);
    if ( sm_list )
      sm_list->insert(this);

    if ( isMenuBar(parent) ) // Parent is menubar
    {
      FMenuBar* menubar_ptr = dynamic_cast<FMenuBar*>(parent);
      if ( menubar_ptr )
        menubar_ptr->menu_dimension();

      // Meta + hotkey
      menubar_ptr->addAccelerator (0x20000e0+tolower(hotkey), this);

      this->addCallback
      (
        "activate",
        _METHOD_CALLBACK (getSuperMenu(), &FMenuBar::cb_item_activated)
      );
      this->addCallback
      (
        "deactivate",
        _METHOD_CALLBACK (getSuperMenu(), &FMenuBar::cb_item_deactivated)
      );
    }
    else if ( isMenu(parent) ) // Parent is menu
    {
      FMenu* super_menu_ptr = dynamic_cast<FMenu*>(parent);
      if ( super_menu_ptr )
        super_menu_ptr->menu_dimension();

      //addAccelerator (accel_key, this);

      this->addCallback
      (
        "activate",
        _METHOD_CALLBACK (getSuperMenu(), &FMenu::cb_menuitem_activated)
      );
      this->addCallback
      (
        "deactivate",
        _METHOD_CALLBACK (getSuperMenu(), &FMenu::cb_menuitem_deactivated)
      );
    }
  }
  if ( hasFocus() )
    flags = FOCUS;

  if ( isEnabled() )
    flags |= ACTIVE;
}

//----------------------------------------------------------------------
uChar FMenuItem::hotKey()
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
bool FMenuItem::isMenuBar (FWidget* w) const
{
  return bool ( strcmp ( w->getClassName()
                       , const_cast<char*>("FMenuBar") ) == 0 );
}

//----------------------------------------------------------------------
bool FMenuItem::isMenu (FWidget* w) const
{
  return bool ( strcmp ( w->getClassName()
                       , const_cast<char*>("FMenu") ) == 0 );
}

//----------------------------------------------------------------------
void FMenuItem::processActivate()
{
  emitCallback("activate");
}

//----------------------------------------------------------------------
void FMenuItem::processDeactivate()
{
  emitCallback("deactivate");
}

//----------------------------------------------------------------------
void FMenuItem::processClicked()
{
  emitCallback("clicked");
}

// public methods of FMenuItem
//----------------------------------------------------------------------
void FMenuItem::onKeyPress (FKeyEvent* ev)
{
  if ( super_menu )
  {
    if ( isMenu(super_menu) )
    {
      FMenu* smenu = dynamic_cast<FMenu*>(super_menu);
      if ( smenu )
        smenu->onKeyPress(ev);
    }

    if ( isMenuBar(super_menu) )
    {
      FMenuBar* mbar = dynamic_cast<FMenuBar*>(super_menu);
      if ( mbar )
      {
        if ( mbar->hotkeyMenu(ev) )
          return;
        mbar->onKeyPress(ev);
      }
    }
  }
}

//----------------------------------------------------------------------
void FMenuItem::onMouseDown (FMouseEvent* ev)
{
  if ( super_menu )
  {
    const FPoint& g = ev->getGlobalPos();
    int b = ev->getButton();

    if ( isMenu(super_menu) )
    {
      FMenu* smenu = dynamic_cast<FMenu*>(super_menu);
      if ( smenu )
      {
        const FPoint& p2 = smenu->globalToLocalPos(g);
        ev = new FMouseEvent (MouseMove_Event, p2, g, b);
        smenu->onMouseDown(ev);
        delete ev;
      }
    }

    if ( isMenuBar(super_menu) )
    {
      FMenuBar* mbar = dynamic_cast<FMenuBar*>(super_menu);
      if ( mbar )
      {
        const FPoint& p2 = mbar->globalToLocalPos(g);
        ev = new FMouseEvent (MouseMove_Event, p2, g, b);
        mbar->onMouseDown(ev);
        delete ev;
      }
    }
  }
}

//----------------------------------------------------------------------
void FMenuItem::onMouseUp (FMouseEvent* ev)
{
  if ( super_menu )
  {
    const FPoint& g = ev->getGlobalPos();
    int b = ev->getButton();

    if ( isMenu(super_menu) )
    {
      FMenu* smenu = dynamic_cast<FMenu*>(super_menu);
      if ( smenu )
      {
        const FPoint& p2 = smenu->globalToLocalPos(g);
        ev = new FMouseEvent (MouseMove_Event, p2, g, b);
        smenu->onMouseUp(ev);
        delete ev;
      }
    }

    if ( isMenuBar(super_menu) )
    {
      FMenuBar* mbar = dynamic_cast<FMenuBar*>(super_menu);
      if ( mbar )
      {
        const FPoint& p2 = mbar->globalToLocalPos(g);
        ev = new FMouseEvent (MouseMove_Event, p2, g, b);
        mbar->onMouseUp(ev);
        delete ev;
      }
    }
  }
}

//----------------------------------------------------------------------
void FMenuItem::onMouseMove (FMouseEvent* ev)
{
  if ( super_menu )
  {
    const FPoint& g = ev->getGlobalPos();
    int b = ev->getButton();

    if ( isMenu(super_menu) )
    {
      FMenu* smenu = dynamic_cast<FMenu*>(super_menu);
      if ( smenu )
      {
        const FPoint& p2 = smenu->globalToLocalPos(g);
        ev = new FMouseEvent (MouseMove_Event, p2, g, b);
        smenu->onMouseMove(ev);
        delete ev;
      }
    }

    if ( isMenuBar(super_menu) )
    {
      FMenuBar* mbar = dynamic_cast<FMenuBar*>(super_menu);
      if ( mbar )
      {
        const FPoint& p2 = mbar->globalToLocalPos(g);
        ev = new FMouseEvent (MouseMove_Event, p2, g, b);
        mbar->onMouseMove(ev);
        delete ev;
      }
    }

  }
}

//----------------------------------------------------------------------
void FMenuItem::onAccel (FAccelEvent* ev)
{
  if ( isEnabled() && ! isSelected() )
  {
    if ( super_menu && isMenuBar(super_menu) )
    {
      FMenuBar* mbar = dynamic_cast<FMenuBar*>(super_menu);
      if ( mbar )
      {
        if ( menu && ! menu->hasSelectedListItem() )
        {
          FWidget* focused_widget;

          if ( mbar->getSelectedMenuItem() )
            mbar->getSelectedMenuItem()->unsetSelected();
          setSelected();
          mbar->selectedMenuItem = this;


          focused_widget = static_cast<FWidget*>(ev->focusedWidget());
          FFocusEvent out (FocusOut_Event);
          FApplication::queueEvent(focused_widget, &out);
          menu->selectFirstItemInList();
          menu->selectedListItem->setFocus();
          if ( focused_widget )
            focused_widget->redraw();
          menu->redraw();
          if ( statusBar() )
            statusBar()->drawMessage();
          mbar->redraw();
        }
        else
        {
          unsetSelected();
          mbar->selectedMenuItem = 0;
          mbar->redraw();
          processClicked();
        }
        ev->accept();
      }
    }
  }
}

//----------------------------------------------------------------------
void FMenuItem::onFocusIn (FFocusEvent*)
{
  if ( statusBar() )
    statusBar()->drawMessage();
}

//----------------------------------------------------------------------
void FMenuItem::onFocusOut (FFocusEvent*)
{
  if ( statusBar() )
  {
    statusBar()->clearMessage();
    statusBar()->drawMessage();
  }
}

//----------------------------------------------------------------------
bool FMenuItem::setEnable (bool on)
{
  FWidget::setEnable(on);

  FWidget* super = getSuperMenu();

  if ( on )
  {
    flags |= ACTIVE;

    if ( super && isMenuBar(super) )
    {
      // Meta + hotkey
      super->addAccelerator (0x20000e0+tolower(hotkey), this);
    }
  }
  else
  {
    flags &= ~ACTIVE;

    if ( super && isMenuBar(super) )
      super->delAccelerator (this);
  }
  return on;
}

//----------------------------------------------------------------------
bool FMenuItem::setFocus (bool on)
{
  FWidget::setFocus(on);

  if ( on )
  {
    flags |= FOCUS;

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
    flags &= ~FOCUS;

    if ( isEnabled() && statusBar() )
      statusBar()->clearMessage();
  }
  return on;
}

//----------------------------------------------------------------------
void FMenuItem::setSelected()
{
  if ( isEnabled() )
  {
    selected = true;
    processActivate();
  }
}

//----------------------------------------------------------------------
void FMenuItem::unsetSelected()
{
  selected = false;
  processDeactivate();
}

//----------------------------------------------------------------------
void FMenuItem::setText (FString& txt)
{
  text = txt;
  text_length = text.getLength();
  hotkey = hotKey();
  if ( hotkey )
    text_length--;
  setWidth(int(text_length));
}

//----------------------------------------------------------------------
void FMenuItem::setText (const std::string& txt)
{
  FString s = FString(txt);
  setText (s);
}

//----------------------------------------------------------------------
void FMenuItem::setText (const char* txt)
{
  FString s = FString(txt);
  setText (s);
}
