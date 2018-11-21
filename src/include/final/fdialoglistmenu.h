/***********************************************************************
* fdialoglistmenu.h - Widget FDialogListMenu                           *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2016-2018 Markus Gans                                      *
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

/*  Inheritance diagram
 *  ═══════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▏
 * ▕  FTerm  ▏
 * ▕▁▁▁▁▁▁▁▁▁▏
 *      ▲
 *      │
 * ▕▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▏
 * ▕ FVTerm  ▏ ▕ FObject ▏
 * ▕▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▏
 *      ▲           ▲
 *      │           │
 *      └─────┬─────┘
 *            │
 *       ▕▔▔▔▔▔▔▔▔▔▏
 *       ▕ FWidget ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏
 *            ▲
 *            │
 *       ▕▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▔▔▏
 *       ▕ FWindow ▏ ▕ FMenuList ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▁▁▏
 *            ▲           ▲
 *            │           │
 *            └─────┬─────┘
 *                  │
 *              ▕▔▔▔▔▔▔▔▏
 *              ▕ FMenu ▏
 *              ▕▁▁▁▁▁▁▁▏
 *                  ▲
 *                  │
 *         ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏           ▕▔▔▔▔▔▔▔▔▔▔▔▏
 *         ▕ FDialogListMenu ▏- - - - - -▕ FMenuItem ▏
 *         ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏1         1▕▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FDIALOGLISTMENU_H
#define FDIALOGLISTMENU_H
#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fmenu.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FDialogListMenu
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FDialogListMenu : public FMenu
{
  public:
    // Constructors
    explicit FDialogListMenu (FWidget* = 0);
    explicit FDialogListMenu (const FString&, FWidget* = 0);

    // Destructor
    virtual ~FDialogListMenu();

    // Accessors
    virtual const char* getClassName() const;

  private:
    // Disable copy constructor
    FDialogListMenu (const FDialogListMenu&);

    // Disable assignment operator (=)
    FDialogListMenu& operator = (const FDialogListMenu&);

    // Method
    void init();
};
#pragma pack(pop)


// FDialogListMenu inline functions
//----------------------------------------------------------------------
inline const char* FDialogListMenu::getClassName() const
{ return "FDialogListMenu"; }

}  // namespace finalcut

#endif  // FDIALOGLISTMENU_H
