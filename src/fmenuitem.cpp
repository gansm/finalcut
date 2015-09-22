// fmenuitem.cpp
// class FMenuItem

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
  hotkey = getHotkey();
  setGeometry (1,1,1,1);

  if ( parent )
  {
    if ( isMenuBar(parent) ) // Parent is menubar
    {
      setSuperMenu( dynamic_cast<FMenuList*>(parent) );
      superMenu()->insert(this);

      //addAccelerator (item->getKey(), item);

      this->addCallback
      (
        "activate",
        (FWidget*)superMenu(),
        reinterpret_cast<FWidget::FMemberCallback>(&FMenuBar::cb_item_activated),
        null
      );
    }
    else if ( isMenu(parent) ) // Parent is menu
    {
      setSuperMenu( dynamic_cast<FMenuList*>(parent) );
      superMenu()->insert(this);

      //addAccelerator (item->getKey(), item);

      this->addCallback
      (
        "activate",
        (FWidget*)superMenu(),
        reinterpret_cast<FWidget::FMemberCallback>(&FMenu::cb_menuitem_activated),
        null
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
    if ( (i+1 < length) && (text[i] == '&') )
      return uChar(text[++i]);
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
    if ( isMenuBar(w) )
      w->redraw();
    ev->accept();
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
inline void FMenuItem::setText (FString& txt)
{
  text = txt;
  hotkey = getHotkey();
}

//----------------------------------------------------------------------
inline void FMenuItem::setText (const std::string& txt)
{
  text = txt;
  hotkey = getHotkey();
}

//----------------------------------------------------------------------
inline void FMenuItem::setText (const char* txt)
{
  text = txt;
  hotkey = getHotkey();
}
