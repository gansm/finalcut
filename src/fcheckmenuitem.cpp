// File: fcheckmenuitem.cpp
// Provides: class FCheckMenuItem

#include "fcheckmenuitem.h"
#include "fmenu.h"

//----------------------------------------------------------------------
// class FCheckMenuItem
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FCheckMenuItem::FCheckMenuItem (FWidget* parent)
  : FMenuItem(parent)
{
  init (parent);
}

//----------------------------------------------------------------------
FCheckMenuItem::FCheckMenuItem (FString& txt, FWidget* parent)
  : FMenuItem(txt, parent)
{
  init (parent);
}

//----------------------------------------------------------------------
FCheckMenuItem::FCheckMenuItem (const std::string& txt, FWidget* parent)
  : FMenuItem(txt, parent)
{
  init (parent);
}

//----------------------------------------------------------------------
FCheckMenuItem::FCheckMenuItem (const char* txt, FWidget* parent)
  : FMenuItem(txt, parent)
{
  init (parent);
}

//----------------------------------------------------------------------
FCheckMenuItem::~FCheckMenuItem()  // destructor
{ }


// private methods of FCheckMenuItem
//----------------------------------------------------------------------
void FCheckMenuItem::init (FWidget* parent)
{
  checkable = true;

  if ( parent )
  {
    if ( isMenu(parent) ) // Parent is menu
    {
      FMenu* menu_ptr = dynamic_cast<FMenu*>(parent);
      if ( menu_ptr )
        menu_ptr->has_checkable_items = true;
    }
  }
}

//----------------------------------------------------------------------
void FCheckMenuItem::processToggle()
{
  emitCallback("toggled");
}

//----------------------------------------------------------------------
void FCheckMenuItem::processClicked()
{
  checked = not checked;
  processToggle();
  emitCallback("clicked");
}
