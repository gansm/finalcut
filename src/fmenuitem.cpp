// File: fmenuitem.cpp
// Provides: class FMenuItem

#include "fmenu.h"
#include "fmenubar.h"
#include "fmenuitem.h"
#include "fmenulist.h"

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
  setGeometry (1,1,text_length+2,1, false);

  if ( parent )
  {
    FMenuList* super_menu_list = dynamic_cast<FMenuList*>(parent);
    setSuperMenu(super_menu_list);
    super_menu_list->insert(this);

    if ( isMenuBar(parent) ) // Parent is menubar
    {
      FMenuBar* menubar_ptr = dynamic_cast<FMenuBar*>(parent);
      if ( menubar_ptr )
        menubar_ptr->menu_dimension();

      //addAccelerator (item->getKey(), item);

      this->addCallback
      (
        "activate",
        _METHOD_CALLBACK (superMenu(), &FMenuBar::cb_item_activated)
      );
      this->addCallback
      (
        "deactivate",
        _METHOD_CALLBACK (superMenu(), &FMenuBar::cb_item_deactivated)
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
        _METHOD_CALLBACK (superMenu(), &FMenu::cb_menuitem_activated)
      );
      this->addCallback
      (
        "deactivate",
        _METHOD_CALLBACK (superMenu(), &FMenu::cb_menuitem_deactivated)
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
bool FMenuItem::isMenuBar (FMenuList* ml) const
{
  FWidget* menubar_ptr = dynamic_cast<FWidget*>(ml);

  if ( menubar_ptr )
    return isMenuBar(menubar_ptr);
  else
    return false;
}

//----------------------------------------------------------------------
bool FMenuItem::isMenuBar (FWidget* w) const
{
  return bool ( strcmp ( w->getClassName()
                       , const_cast<char*>("FMenuBar") ) == 0 );
}

//----------------------------------------------------------------------
bool FMenuItem::isMenu (FMenuList* ml) const
{
  FWidget* super_menu_ptr = dynamic_cast<FWidget*>(ml);

  if ( super_menu_ptr )
    return isMenu(super_menu_ptr);
  else
    return false;
}

//----------------------------------------------------------------------
bool FMenuItem::isMenu (FWidget* w) const
{
  return bool ( strcmp ( w->getClassName()
                       , const_cast<char*>("FMenu") ) == 0 );
}

//----------------------------------------------------------------------
FMenuList* FMenuItem::superMenu() const
{
  return super_menu;
}

//----------------------------------------------------------------------
void FMenuItem::setSuperMenu (FMenuList* smenu)
{
  super_menu = smenu;
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
  if ( isSelected() )
  {
    unsetSelected();
    FWidget* w = reinterpret_cast<FWidget*>(superMenu());
    if ( w && isMenuBar(w) )
      w->redraw();
    ev->accept();
  }
}

//----------------------------------------------------------------------
void FMenuItem::onMouseDown (FMouseEvent* ev)
{
  const FPoint& p1 = ev->getPos();
  const FPoint& g = ev->getGlobalPos();
  FPoint p2(p1);
  int b = ev->getButton();
  p2 = p1 + getPos() - FPoint(1,1);
  ev = new FMouseEvent (MouseMove_Event, p2, g, b);

  if ( super_menu )
  {
    if ( isMenu(super_menu) )
    {
      FMenu* sm = dynamic_cast<FMenu*>(super_menu);
      if ( sm )
        sm->onMouseDown(ev);
    }

    if ( isMenuBar(super_menu) )
    {
      FMenuBar* mb = dynamic_cast<FMenuBar*>(super_menu);
      if ( mb )
        mb->onMouseDown(ev);
    }
  }

  delete ev;
}

//----------------------------------------------------------------------
void FMenuItem::onMouseUp (FMouseEvent* ev)
{
  const FPoint& p1 = ev->getPos();
  const FPoint& g = ev->getGlobalPos();
  FPoint p2(p1);
  int b = ev->getButton();
  p2 = p1 + getPos() - FPoint(1,1);
  ev = new FMouseEvent (MouseMove_Event, p2, g, b);

  if ( super_menu )
  {
    if ( isMenu(super_menu) )
    {
      FMenu* sm = dynamic_cast<FMenu*>(super_menu);
      if ( sm )
        sm->onMouseUp(ev);
    }

    if ( isMenuBar(super_menu) )
    {
      FMenuBar* mb = dynamic_cast<FMenuBar*>(super_menu);
      if ( mb )
        mb->onMouseUp(ev);
    }
  }

  delete ev;
}

//----------------------------------------------------------------------
void FMenuItem::onMouseMove (FMouseEvent* ev)
{
  const FPoint& p1 = ev->getPos();
  const FPoint& g = ev->getGlobalPos();
  FPoint p2(p1);
  int b = ev->getButton();
  p2 = p1 + getPos() - FPoint(1,1);
  ev = new FMouseEvent (MouseMove_Event, p2, g, b);

  if ( super_menu )
  {
    if ( isMenu(super_menu) )
    {
      FMenu* sm = dynamic_cast<FMenu*>(super_menu);
      if ( sm )
        sm->onMouseMove(ev);
    }

    if ( isMenuBar(super_menu) )
    {
      FMenuBar* mb = dynamic_cast<FMenuBar*>(super_menu);
      if ( mb )
        mb->onMouseMove(ev);
    }
  }

  delete ev;
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
  setWidth(text_length);
}

//----------------------------------------------------------------------
inline void FMenuItem::setText (const std::string& txt)
{
  setText (FString(txt));
}

//----------------------------------------------------------------------
inline void FMenuItem::setText (const char* txt)
{
  setText (FString(txt));
}
