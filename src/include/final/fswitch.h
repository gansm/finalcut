/***********************************************************************
* fswitch.h - Widget FSwitch                                           *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2015-2020 Markus Gans                                      *
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

#include "final/ftogglebutton.h"

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

    // Disable copy constructor
    FSwitch (const FSwitch&) = delete;

    // Destructor
    ~FSwitch() override;

    // Disable copy assignment operator (=)
    FSwitch& operator = (const FSwitch&) = delete;

    // Accessor
    const FString       getClassName() const override;

    // Mutator
    void                setText (const FString&) override;

    // Event handlers
    void                onKeyPress (FKeyEvent*) override;
    void                onMouseDown (FMouseEvent*) override;
    void                onMouseUp (FMouseEvent*) override;

  private:
    // Methods
    void                draw() override;
    void                drawCheckButton();
    void                drawChecked();
    void                drawUnchecked();

    // Data members
    std::size_t  switch_offset_pos{0};
    bool         button_pressed{false};
};

// FSwitch inline functions
//----------------------------------------------------------------------
inline const FString FSwitch::getClassName() const
{ return "FSwitch"; }

}  // namespace finalcut

#endif  // FSWITCH_H
