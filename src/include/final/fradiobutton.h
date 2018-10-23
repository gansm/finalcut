/***********************************************************************
* fradiobutton.h - Widget FRadioButton                                 *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2014-2018 Markus Gans                                      *
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
 *    ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *    ▕ FToggleButton ▏
 *    ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *            ▲
 *            │
 *     ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *     ▕ FRadioButton ▏
 *     ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FRADIOBUTTON_H
#define FRADIOBUTTON_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/ftogglebutton.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FRadioButton
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FRadioButton : public FToggleButton
{
  public:
    // Constructors
    explicit FRadioButton (FWidget* = 0);
    explicit FRadioButton (const FString&, FWidget* = 0);

    // Destructor
    virtual ~FRadioButton();

    // Accessor
    const char* getClassName() const;

  private:
    // Disable copy constructor
    FRadioButton (const FRadioButton&);

    // Disable assignment operator (=)
    FRadioButton& operator = (const FRadioButton&);

    // Methods
    void         init();
    virtual void draw();
    void         drawRadioButton();
};
#pragma pack(pop)


// FRadioButton inline functions
//----------------------------------------------------------------------
inline const char* FRadioButton::getClassName() const
{ return "FRadioButton"; }

}  // namespace finalcut

#endif  // FRADIOBUTTON_H
