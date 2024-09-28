/***********************************************************************
* fdialoglistmenu.h - Widget FDialogListMenu                           *
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

/*  Inheritance diagram
 *  ═══════════════════
 *
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

#include "final/menu/fmenu.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FDialogListMenu
//----------------------------------------------------------------------

class FDialogListMenu : public FMenu
{
  public:
    // Constructors
    explicit FDialogListMenu (FWidget* = nullptr);
    explicit FDialogListMenu (FString&&, FWidget* = nullptr);

    // Destructor
    ~FDialogListMenu() override;

    // Accessors
    auto getClassName() const -> FString override;

  private:
    // Enumeration
    enum class Enable { No, Yes };

    // Method
    void init();

    // Callback method
    void cb_switchState (Enable);
};


// FDialogListMenu inline functions
//----------------------------------------------------------------------
inline auto FDialogListMenu::getClassName() const -> FString
{ return "FDialogListMenu"; }

}  // namespace finalcut

#endif  // FDIALOGLISTMENU_H
