/***********************************************************************
* fmenubar.h - Widget FMenuBar                                         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2020 Markus Gans                                      *
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

#include "final/fmenulist.h"
#include "final/fwindow.h"

namespace finalcut
{

// class forward declaration
class FMenu;

//----------------------------------------------------------------------
// class FMenuBar
//----------------------------------------------------------------------

class FMenuBar : public FWindow, public FMenuList
{
  public:
    // Constructor
    explicit FMenuBar (FWidget* = nullptr);

    // Disable copy constructor
    FMenuBar (const FMenuBar&) = delete;

    // Destructor
    ~FMenuBar() override;

    // Disable copy assignment operator (=)
    FMenuBar& operator = (const FMenuBar&) = delete;

    // Accessors
    FString       getClassName() const override;

    // Methods
    void          resetColors() override;
    void          resetMenu();
    void          hide() override;
    void          adjustSize() override;

    // Event handlers
    void          onKeyPress (FKeyEvent*) override;
    void          onMouseDown (FMouseEvent*) override;
    void          onMouseUp (FMouseEvent*) override;
    void          onMouseMove (FMouseEvent*) override;
    void          onAccel (FAccelEvent*) override;

    // Callback methods
    void          cb_itemDeactivated (const FMenuItem*) const;

  private:
    // Constants
    static constexpr auto NOT_SET = static_cast<std::size_t>(-1);

    // Typedef
    typedef struct
    {
      FString text;
      std::size_t startpos;
      std::size_t hotkeypos;
      bool no_underline;
    } menuText;

    // Inquiry
    bool          isMenu (const FMenuItem*) const;

    // Methods
    void          init();
    void          calculateDimensions() const;
    bool          selectNextItem();
    bool          selectPrevItem();
    bool          hotkeyMenu (FKeyEvent*&);
    void          draw() override;
    void          drawItems();
    void          drawItem (FMenuItem*, std::size_t&);
    void          setLineAttributes (const FMenuItem*);
    void          setCursorToHotkeyPosition (FMenuItem*, std::size_t) const;
    void          drawMenuText (menuText&);
    void          drawEllipsis (const menuText&, std::size_t);
    void          drawLeadingSpace (std::size_t&);
    void          drawTrailingSpace (std::size_t&);
    void          adjustItems() const;
    bool          activateMenu (const FMenuItem*);
    bool          clickItem (FMenuItem*);
    void          unselectMenuItem (FMenuItem*);
    void          selectMenuItem (FMenuItem*);
    void          mouseDownOverList (const FMouseEvent*);
    void          mouseUpOverList (const FMouseEvent*);
    void          mouseMoveOverList (const FMouseEvent*);
    void          passEventToMenu (const FMouseEvent* const&) const;
    void          leaveMenuBar();

    // Data members
    std::size_t   screenWidth{80};
    bool          mouse_down{false};
    bool          drop_down{false};
    bool          focus_changed{false};

    // Friend classes
    friend class FMenu;
    friend class FMenuItem;
};


// FMenuBar inline functions
//----------------------------------------------------------------------
inline FString FMenuBar::getClassName() const
{ return "FMenuBar"; }

//----------------------------------------------------------------------
inline bool FMenuBar::isMenu (const FMenuItem* mi) const
{ return mi->hasMenu(); }

}  // namespace finalcut

#endif  // FMENUBAR_H
