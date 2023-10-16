/***********************************************************************
* fdialoglistmenu.cpp - Widget FDialogListMenu                         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2016-2023 Markus Gans                                      *
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

#include "final/fapplication.h"
#include "final/fc.h"
#include "final/menu/fdialoglistmenu.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FDialogListMenu
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FDialogListMenu::FDialogListMenu(FWidget* parent)
  : FMenu{parent}
{
  init();
}

//----------------------------------------------------------------------
FDialogListMenu::FDialogListMenu (FString&& txt, FWidget* parent)
  : FMenu{std::move(txt), parent}
{
  init();
}

//----------------------------------------------------------------------
FDialogListMenu::~FDialogListMenu()  // destructor
{
  auto fapp = FApplication::getApplicationObject();

  if ( fapp )
    fapp->delCallback(this);
}

// private methods of FMenu
//----------------------------------------------------------------------
void FDialogListMenu::init()
{
  auto m_item = getItem();
  m_item->dialog_index = true;

  if ( getDialogList()->empty() )
    m_item->setFlags().feature.active = false;

  auto fapp = FApplication::getApplicationObject();

  if ( ! fapp )
    return;

  // FApplication callback functions
  fapp->addCallback
  (
    "last-dialog-closed",
    this,
    &FDialogListMenu::cb_switchState,
    false
  );

  fapp->addCallback
  (
    "first-dialog-opened",
    this,
    &FDialogListMenu::cb_switchState,
    true
  );
}

//----------------------------------------------------------------------
void FDialogListMenu::cb_switchState (bool enable)
{
  auto m_item = getItem();

  if ( ! m_item )
    return;

  m_item->setFlags().feature.active = enable;
  auto super = m_item->getSuperMenu();

  if ( ! super )
    return;

  if ( isMenu(super) || isMenuBar(super) )
    super->redraw();
}

}  // namespace finalcut
