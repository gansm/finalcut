/***********************************************************************
* fcheckmenuitem.cpp - Widget FCheckMenuItem                           *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2015-2017 Markus Gans                                      *
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

#include "final/fcheckmenuitem.h"
#include "final/fmenu.h"

namespace finalcut
{

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
FCheckMenuItem::FCheckMenuItem (const FString& txt, FWidget* parent)
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

  if ( ! parent )
    return;

  if ( isMenu(parent) )  // Parent is menu
  {
    FMenu* menu_ptr = static_cast<FMenu*>(parent);
    menu_ptr->has_checkable_items = true;
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
  checked = ! checked;
  processToggle();
  emitCallback("clicked");
}

}  // namespace finalcut
