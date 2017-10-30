/************************************************************************
* fswitch.h - Widget FSwitch                                            *
*                                                                       *
* This file is part of the Final Cut widget toolkit                     *
*                                                                       *
* Copyright 2015-2017 Markus Gans                                       *
*                                                                       *
* The Final Cut is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by  *
* the Free Software Foundation; either version 3 of the License, or     *
* (at your option) any later version.                                   *
*                                                                       *
* The Final Cut is distributed in the hope that it will be useful,      *
* but WITHOUT ANY WARRANTY; without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
* GNU General Public License for more details.                          *
*                                                                       *
* You should have received a copy of the GNU General Public License     *
* along with this program.  If not, see <http://www.gnu.org/licenses/>. *
************************************************************************/

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


//----------------------------------------------------------------------
// class FSwitch
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FSwitch : public FToggleButton
{
  public:
    // Constructors
    explicit FSwitch (FWidget* = 0);
    FSwitch (const FString&, FWidget* = 0);

    // Destructor
    virtual ~FSwitch();

    // Accessor
    const char* getClassName() const;

    // Mutator
    void setText (const FString&);

    // Event handlers
    void onKeyPress (FKeyEvent*);
    void onMouseDown (FMouseEvent*);
    void onMouseUp (FMouseEvent*);

  private:
    // Disable copy constructor
    FSwitch (const FSwitch&);

    // Disable assignment operator (=)
    FSwitch& operator = (const FSwitch&);

    // Methods
    void draw();
    void drawCheckButton();

    // Data Members
    int  switch_offset_pos;
    bool button_pressed;
};
#pragma pack(pop)


// FSwitch inline functions
//----------------------------------------------------------------------
inline const char* FSwitch::getClassName() const
{ return "FSwitch"; }

#endif  // FSWITCH_H
