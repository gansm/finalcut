/************************************************************************
* fcheckbox.h - Widget FCheckBox                                        *
*                                                                       *
* This file is part of the Final Cut widget toolkit                     *
*                                                                       *
* Copyright 2014-2017 Markus Gans                                       *
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
 *      ▕▔▔▔▔▔▔▔▔▔▔▔▏
 *      ▕ FCheckBox ▏
 *      ▕▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FCHECKBOX_H
#define FCHECKBOX_H

#include "final/ftogglebutton.h"


//----------------------------------------------------------------------
// class FCheckBox
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FCheckBox : public FToggleButton
{
  public:
    // Constructors
    explicit FCheckBox (FWidget* = 0);
    FCheckBox (const FString&, FWidget* = 0);

    // Destructor
    virtual ~FCheckBox();

    // Accessor
    const char* getClassName() const;

  private:
    // Disable copy constructor
    FCheckBox (const FCheckBox&);

    // Disable assignment operator (=)
    FCheckBox& operator = (const FCheckBox&);

    // Methods
    void init();
    void draw();
    void drawCheckButton();
};
#pragma pack(pop)


// FCheckBox inline functions
//----------------------------------------------------------------------
inline const char* FCheckBox::getClassName() const
{ return "FCheckBox"; }

#endif  // FCHECKBOX_H
