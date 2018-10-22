/***********************************************************************
* fmenubar.h - Widget FMenuBar                                         *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2015-2018 Markus Gans                                      *
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

namespace finalcut
{

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
    void         resetMenu();
    virtual void hide();
    virtual void adjustSize();

    // Event handlers
    virtual void onKeyPress (FKeyEvent*);
    virtual void onMouseDown (FMouseEvent*);
    virtual void onMouseUp (FMouseEvent*);
    virtual void onMouseMove (FMouseEvent*);
    virtual void onAccel (FAccelEvent*);

    // Callback methods
    void         cb_item_deactivated (FWidget*, data_ptr);

  private:
    // Constants
    static const std::size_t NOT_SET = static_cast<std::size_t>(-1);

    // Typedef
    typedef struct
    {
      wchar_t* text;
      std::size_t length;
      std::size_t startpos;
      std::size_t hotkeypos;
      bool no_underline;
    } menuText;

    // Disable copy constructor
    FMenuBar (const FMenuBar&);

    // Disable assignment operator (=)
    FMenuBar& operator = (const FMenuBar&);

    // Inquiry
    bool         isMenu (FMenuItem*) const;

    // Methods
    void         init();
    void         calculateDimensions();
    bool         selectNextItem();
    bool         selectPrevItem();
    bool         hotkeyMenu (FKeyEvent*&);
    std::size_t  getHotkeyPos (wchar_t[], wchar_t[], std::size_t);
    virtual void draw();
    void         drawItems();
    void         drawItem (FMenuItem*, std::size_t&);
    void         setLineAttributes (FMenuItem*);
    void         drawMenuText (menuText&);
    void         drawEllipsis (menuText&, std::size_t);
    void         drawLeadingSpace (std::size_t&);
    void         drawTrailingSpace (std::size_t&);
    void         adjustItems();
    bool         activateMenu (FMenuItem*);
    bool         clickItem (FMenuItem*);
    void         unselectMenuItem (FMenuItem*);
    void         selectMenuItem (FMenuItem*);
    void         mouseDownOverList (FMouseEvent*);
    void         mouseUpOverList (FMouseEvent*);
    void         mouseMoveOverList (FMouseEvent*);
    void         passEventToMenu (FMouseEvent*&);
    void         leaveMenuBar();

    // Friend classes
    friend class FMenu;
    friend class FMenuItem;

    // Data Members
    bool        mouse_down;
    bool        drop_down;
    bool        focus_changed;
    std::size_t screenWidth;
};
#pragma pack(pop)


// FMenuBar inline functions
//----------------------------------------------------------------------
inline const char* FMenuBar::getClassName() const
{ return "FMenuBar"; }

//----------------------------------------------------------------------
inline bool FMenuBar::isMenu (FMenuItem* mi) const
{ return mi->hasMenu(); }

}  // namespace finalcut

#endif  // FMENUBAR_H
