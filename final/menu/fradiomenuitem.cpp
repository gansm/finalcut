/***********************************************************************
* fradiomenuitem.cpp - Widget FRadioMenuItem                           *
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

#include <utility>

#include "final/fc.h"
#include "final/menu/fmenu.h"
#include "final/menu/fradiomenuitem.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FRadioMenuItem
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FRadioMenuItem::FRadioMenuItem (FWidget* parent)
  : FMenuItem{parent}
{
  init();
}

//----------------------------------------------------------------------
FRadioMenuItem::FRadioMenuItem (const FString& txt, FWidget* parent)
  : FMenuItem{txt, parent}
{
  init();
}

//----------------------------------------------------------------------
FRadioMenuItem::~FRadioMenuItem() noexcept = default;  // destructor


// private methods of FRadioMenuItem
//----------------------------------------------------------------------
void FRadioMenuItem::init()
{
  setCheckable();
  setRadioButton();
  FWidget* parent = getParentWidget();

  if ( ! parent )
    return;

  if ( isMenu(parent) )  // Parent is menu
  {
    auto menu_ptr = static_cast<FMenu*>(parent);
    menu_ptr->has_checkable_items = true;

    addCallback  // for this element
    (
      "toggled",
      std::move(menu_ptr), &FMenu::cb_menuitemToggled,
      this
    );
  }
}

//----------------------------------------------------------------------
void FRadioMenuItem::processToggle() const
{
  emitCallback("toggled");
}

//----------------------------------------------------------------------
void FRadioMenuItem::processClicked()
{
  if ( ! isChecked() )
  {
    setChecked();
    processToggle();
  }

  emitCallback("clicked");
}

}  // namespace finalcut
