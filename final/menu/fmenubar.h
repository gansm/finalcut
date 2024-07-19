/***********************************************************************
* fmenubar.h - Widget FMenuBar                                         *
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

#include "final/menu/fmenulist.h"
#include "final/widget/fwindow.h"

namespace finalcut
{

// class forward declaration
class FMenu;

//----------------------------------------------------------------------
// class FMenuBar
//----------------------------------------------------------------------

class FMenuBar : public FWindow
               , public FMenuList
{
  public:
    // Using-declaration
    using FWidget::addAccelerator;

    // Constructor
    explicit FMenuBar (FWidget* = nullptr);

    // Destructor
    ~FMenuBar() override;

    // Accessors
    auto getClassName() const -> FString override;

    // Methods
    void resetColors() override;
    void resetMenu();
    void hide() override;
    void adjustSize() override;

    // Event handlers
    void onKeyPress (FKeyEvent*) override;
    void onMouseDown (FMouseEvent*) override;
    void onMouseUp (FMouseEvent*) override;
    void onMouseMove (FMouseEvent*) override;
    void onAccel (FAccelEvent*) override;

    // Callback methods
    void cb_itemDeactivated (const FMenuItem*) const;

  private:
    struct menuText
    {
      FString text;
      std::size_t startpos;
      std::size_t hotkeypos;
      bool no_underline;
    };

    // Constants
    static constexpr auto NOT_SET = static_cast<std::size_t>(-1);

    // Inquiry
    auto isMenu (const FMenuItem*) const -> bool;

    // Methods
    void init();
    void calculateDimensions() const;
    void selectItem_PostProcessing (FMenuItem*) override;
    auto hotkeyMenu (FKeyEvent*&) -> bool;
    void draw() override;
    void drawItems();
    void drawItem (FMenuItem*, std::size_t&);
    void setLineAttributes (const FMenuItem*);
    void setCursorToHotkeyPosition (FMenuItem*, std::size_t) const;
    void drawMenuText (menuText&);
    void drawEllipsis (const menuText&, std::size_t);
    void drawLeadingSpace (std::size_t&);
    void drawTrailingSpace (std::size_t&);
    void adjustItems() const;
    void openMenu (const FMenuItem*);
    auto activateMenu (const FMenuItem*) -> bool;
    auto clickItem (FMenuItem*) -> bool;
    void unselectMenuItem (FMenuItem*);
    void selectMenuItem (FMenuItem*);
    auto isClickOnMenuEntry (const FMouseEvent*, const FMenuItem*) const -> bool;
    void mouseDownOverList (const FMouseEvent*);
    void mouseUpOverList (const FMouseEvent*);
    void mouseMoveOverList (const FMouseEvent&);
    void passEventToMenu (const FMouseEvent&) const;
    void leaveMenuBar();

    // Data members
    std::size_t  screenWidth{80};
    bool         mouse_down{false};
    bool         drop_down{false};
    bool         focus_changed{false};

    // Friend classes
    friend class FMenu;
    friend class FMenuItem;
};


// FMenuBar inline functions
//----------------------------------------------------------------------
inline auto FMenuBar::getClassName() const -> FString
{ return "FMenuBar"; }

//----------------------------------------------------------------------
inline auto FMenuBar::isMenu (const FMenuItem* mi) const -> bool
{ return mi->hasMenu(); }

}  // namespace finalcut

#endif  // FMENUBAR_H
