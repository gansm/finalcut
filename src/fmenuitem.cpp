// File: fmenuitem.cpp
// Provides: class FMenuItem

#include "fmenu.h"
#include "fmenubar.h"
#include "fmenulist.h"
#include "fmenuitem.h"

//----------------------------------------------------------------------
// class FMenuItem
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FMenuItem::FMenuItem (FWidget* parent)
  : FWidget(parent)
  , text()
  , active(true)
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
  , active(true)
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
  , active(true)
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
  , active(true)
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
  hotkey = getHotkey();
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
      addAccelerator (0x20000e0+tolower(hotkey), this);

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

      //addAccelerator (item->getKey(), item);

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
}

//----------------------------------------------------------------------
uChar FMenuItem::getHotkey()
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
void FMenuItem::onAccel (FAccelEvent* ev)
{
  if ( isActivated() && ! isSelected() )
  {
    if ( super_menu && isMenuBar(super_menu) )
    {
      FMenuBar* mb = dynamic_cast<FMenuBar*>(super_menu);
      if ( mb )
      {
        setSelected();
        mb->selectedMenuItem = this;
        mb->redraw();
        ev->accept();
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
      FMenu* sm = dynamic_cast<FMenu*>(super_menu);
      if ( sm )
      {
        const FPoint& p2 = sm->globalToLocalPos(g);
        ev = new FMouseEvent (MouseMove_Event, p2, g, b);
        sm->onMouseDown(ev);
        delete ev;
      }
    }

    if ( isMenuBar(super_menu) )
    {
      FMenuBar* mb = dynamic_cast<FMenuBar*>(super_menu);
      if ( mb )
      {
        const FPoint& p2 = mb->globalToLocalPos(g);
        ev = new FMouseEvent (MouseMove_Event, p2, g, b);
        mb->onMouseDown(ev);
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
      FMenu* sm = dynamic_cast<FMenu*>(super_menu);
      if ( sm )
      {
        const FPoint& p2 = sm->globalToLocalPos(g);
        ev = new FMouseEvent (MouseMove_Event, p2, g, b);
        sm->onMouseUp(ev);
        delete ev;
      }
    }

    if ( isMenuBar(super_menu) )
    {
      FMenuBar* mb = dynamic_cast<FMenuBar*>(super_menu);
      if ( mb )
      {
        const FPoint& p2 = mb->globalToLocalPos(g);
        ev = new FMouseEvent (MouseMove_Event, p2, g, b);
        mb->onMouseUp(ev);
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
      FMenu* sm = dynamic_cast<FMenu*>(super_menu);
      if ( sm )
      {
        const FPoint& p2 = sm->globalToLocalPos(g);
        ev = new FMouseEvent (MouseMove_Event, p2, g, b);
        sm->onMouseMove(ev);
        delete ev;
      }
    }

    if ( isMenuBar(super_menu) )
    {
      FMenuBar* mb = dynamic_cast<FMenuBar*>(super_menu);
      if ( mb )
      {
        const FPoint& p2 = mb->globalToLocalPos(g);
        ev = new FMouseEvent (MouseMove_Event, p2, g, b);
        mb->onMouseMove(ev);
        delete ev;
      }
    }

  }
}

//----------------------------------------------------------------------
void FMenuItem::setSelected()
{
  if ( isActivated() )
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
  hotkey = getHotkey();
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
