/***********************************************************************
* fdialoglistmenu.cpp - Widget FDialogListMenu                         *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2016-2017 Markus Gans                                      *
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

#include "final/fdialoglistmenu.h"

namespace finalcut
{

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
  menuitem->dialog_index = true;
}

}  // namespace finalcut
