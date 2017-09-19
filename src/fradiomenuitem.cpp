// File: fradiomenuitem.cpp
// Provides: class FRadioMenuItem

#include "final/fradiomenuitem.h"
#include "final/fmenu.h"

//----------------------------------------------------------------------
// class FRadioMenuItem
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FRadioMenuItem::FRadioMenuItem (FWidget* parent)
  : FMenuItem(parent)
{
  init (parent);
}

//----------------------------------------------------------------------
FRadioMenuItem::FRadioMenuItem (const FString& txt, FWidget* parent)
  : FMenuItem(txt, parent)
{
  init (parent);
}

//----------------------------------------------------------------------
FRadioMenuItem::~FRadioMenuItem()  // destructor
{ }


// private methods of FRadioMenuItem
//----------------------------------------------------------------------
void FRadioMenuItem::init (FWidget* parent)
{
  checkable = true;
  radio_button = true;

  if ( ! parent )
    return;

  if ( isMenu(parent) )  // Parent is menu
  {
    FMenu* menu_ptr = static_cast<FMenu*>(parent);
    menu_ptr->has_checkable_items = true;

    addCallback  // for this element
    (
      "toggled",
      F_METHOD_CALLBACK (parent, &FMenu::cb_menuitem_toggled)
    );
  }
}

//----------------------------------------------------------------------
void FRadioMenuItem::processToggle()
{
  emitCallback("toggled");
}

//----------------------------------------------------------------------
void FRadioMenuItem::processClicked()
{
  if ( ! checked )
  {
    checked = true;
    processToggle();
  }

  emitCallback("clicked");
}
