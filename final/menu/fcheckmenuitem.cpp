/***********************************************************************
* fcheckmenuitem.cpp - Widget FCheckMenuItem                           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2021 Markus Gans                                      *
*                                                                      *
* FINAL CUT is free software; you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as       *
* published by the Free Software Foundation; either version 3 of       *
* the License, or (at your option) any later version.                  *
*                                                                      *
* FINAL CUT is distributed in the hope that it will be useful, but     *
* WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU Lesser General Public License for more details.                  *
*                                                                      *
* You should have received a copy of the GNU Lesser General Public     *
* License along with this program.  If not, see                        *
* <http://www.gnu.org/licenses/>.                                      *
***********************************************************************/

#include "final/fc.h"
#include "final/menu/fcheckmenuitem.h"
#include "final/menu/fmenu.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FCheckMenuItem
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FCheckMenuItem::FCheckMenuItem (FWidget* parent)
  : FMenuItem{parent}
{
  init();
}

//----------------------------------------------------------------------
FCheckMenuItem::FCheckMenuItem (const FString& txt, FWidget* parent)
  : FMenuItem{txt, parent}
{
  init();
}

//----------------------------------------------------------------------
FCheckMenuItem::~FCheckMenuItem() noexcept = default;  // destructor


// private methods of FCheckMenuItem
//----------------------------------------------------------------------
void FCheckMenuItem::init()
{
  setCheckable();
  FWidget* parent = getParentWidget();

  if ( ! parent )
    return;

  if ( isMenu(parent) )  // Parent is menu
  {
    auto menu_ptr = static_cast<FMenu*>(parent);
    menu_ptr->has_checkable_items = true;
  }
}

//----------------------------------------------------------------------
void FCheckMenuItem::processToggle() const
{
  emitCallback("toggled");
}

//----------------------------------------------------------------------
void FCheckMenuItem::processClicked()
{
  if ( isChecked() )
    unsetChecked();
  else
    setChecked();

  processToggle();
  emitCallback("clicked");
}

}  // namespace finalcut
