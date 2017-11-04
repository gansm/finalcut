/***********************************************************************
* fmenubar.h - Widget FMenuBar                                         *
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
 *       ▕▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▔▔▏1     *▕▔▔▔▔▔▔▔▔▔▔▔▏
 *       ▕ FWindow ▏ ▕ FMenuList ▏- - - -▕ FMenuItem ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▁▁▏       ▕▁▁▁▁▁▁▁▁▁▁▁▏
 *            ▲           ▲
 *            │           │
 *            └─────┬─────┘
 *                  │
 *            ▕▔▔▔▔▔▔▔▔▔▔▏
 *            ▕ FMenuBar ▏
 *            ▕▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FMENUBAR_H
#define FMENUBAR_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fmenu.h"
#include "final/fmenulist.h"
#include "final/fwindow.h"


//----------------------------------------------------------------------
// class FMenuBar
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FMenuBar : public FWindow, public FMenuList
{
  public:
    // Constructor
    explicit FMenuBar (FWidget* = 0);

    // Destructor
    virtual ~FMenuBar();

    // Accessors
    virtual const char* getClassName() const;

    // Methods
    void                hide();
    void                resetMenu();
    void                adjustSize();

    // Event handlers
    void                onKeyPress (FKeyEvent*);
    void                onMouseDown (FMouseEvent*);
    void                onMouseUp (FMouseEvent*);
    void                onMouseMove (FMouseEvent*);
    void                onAccel (FAccelEvent*);

    // Callback methods
    void                cb_item_deactivated (FWidget*, data_ptr);

  private:
    // Disable copy constructor
    FMenuBar (const FMenuBar&);

    // Disable assignment operator (=)
    FMenuBar& operator  = (const FMenuBar&);

    // Inquiry
    bool                isMenu (FMenuItem*) const;

    // Methods
    void                init();
    void                calculateDimensions();
    bool                selectNextItem();
    bool                selectPrevItem();
    bool                hotkeyMenu (FKeyEvent*&);
    int                 getHotkeyPos (wchar_t*&, wchar_t*&, uInt);
    void                draw();
    void                drawItems();
    void                adjustItems();
    void                leaveMenuBar();

    // Friend classes
    friend class FMenu;
    friend class FMenuItem;

    // Data Members
    bool   mouse_down;
    bool   drop_down;
};
#pragma pack(pop)


// FMenuBar inline functions
//----------------------------------------------------------------------
inline const char* FMenuBar::getClassName() const
{ return "FMenuBar"; }

//----------------------------------------------------------------------
inline bool FMenuBar::isMenu (FMenuItem* mi) const
{ return mi->hasMenu(); }

#endif  // FMENUBAR_H
