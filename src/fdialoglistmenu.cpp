// File: fdialoglistmenu.cpp
// Provides: class FDialogListMenu


#include "final/fdialoglistmenu.h"

//----------------------------------------------------------------------
// class FDialogListMenu
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FDialogListMenu::FDialogListMenu(FWidget* parent)
  : FMenu(parent)
{
  init();
}

//----------------------------------------------------------------------
FDialogListMenu::FDialogListMenu (const FString& txt, FWidget* parent)
  : FMenu(txt, parent)
{
  init();
}

//----------------------------------------------------------------------
FDialogListMenu::~FDialogListMenu()
{  }


// private methods of FMenu
//----------------------------------------------------------------------
void FDialogListMenu::init()
{
  FMenuItem* menuitem = getItem();

  if ( menuitem )
    menuitem->dialog_index = true;
}
