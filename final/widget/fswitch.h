/***********************************************************************
* fswitch.h - Widget FSwitch                                           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2024 Markus Gans                                      *
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
 *    ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *    ▕ FToggleButton ▏
 *    ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *            ▲
 *            │
 *       ▕▔▔▔▔▔▔▔▔▔▏
 *       ▕ FSwitch ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FSWITCH_H
#define FSWITCH_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/widget/ftogglebutton.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FSwitch
//----------------------------------------------------------------------

class FSwitch : public FToggleButton
{
  public:
    // Constructors
    explicit FSwitch (FWidget* = nullptr);
    explicit FSwitch (const FString&, FWidget* = nullptr);

    // Destructor
    ~FSwitch() noexcept override;

    // Accessor
    auto getClassName() const -> FString override;

    // Mutator
    void setText (const FString&) override;

    // Event handlers
    void onKeyPress (FKeyEvent*) override;
    void onMouseDown (FMouseEvent*) override;
    void onMouseUp (FMouseEvent*) override;

  private:
    // Inquiries
    auto isMonoFocus() const -> bool;

    // Methods
    void draw() override;
    void drawCheckButton();
    void drawSwitch();
    void SetStyleForOn() const;
    void SetStyleForOff() const;
    auto createOnText() const -> FString;
    auto createOffText() const -> FString;

    // Data members
    std::size_t  switch_offset_pos{0};
    bool         button_pressed{false};
};

// FSwitch inline functions
//----------------------------------------------------------------------
inline auto FSwitch::getClassName() const -> FString
{ return "FSwitch"; }

}  // namespace finalcut

#endif  // FSWITCH_H
