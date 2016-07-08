// File: fdialoglistmenu.cpp
// Provides: class FDialogListMenu


#include "fdialoglistmenu.h"

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
FDialogListMenu::FDialogListMenu (FString& txt, FWidget* parent)
  : FMenu(txt, parent)
{
  init();
}


//----------------------------------------------------------------------
FDialogListMenu::FDialogListMenu (const std::string& txt, FWidget* parent)
  : FMenu(txt, parent)
{
  init();
}


//----------------------------------------------------------------------
FDialogListMenu::FDialogListMenu (const char* txt, FWidget* parent)
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
    menuitem->dialog_list = true;
}
