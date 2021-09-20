/***********************************************************************
* fmenu.h - Widget FMenu                                               *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2021 Markus Gans                                      *
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
 *      │           │                   *▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *      └─────┬─────┘               ┌ - -▕ FRadioMenuItem ▏
 *            │                     :    ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *       ▕▔▔▔▔▔▔▔▔▔▏                :
 *       ▕ FWidget ▏                :   *▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *       ▕▁▁▁▁▁▁▁▁▁▏                ├ - -▕ FCheckMenuItem ▏
 *            ▲                     :    ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *            │                   1 :
 *       ▕▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▔▔▏- ┘    ▕▔▔▔▔▔▔▔▔▔▔▔▏
 *       ▕ FWindow ▏ ▕ FMenuList ▏- - - -▕ FMenuItem ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▁▁▏1     *▕▁▁▁▁▁▁▁▁▁▁▁▏
 *            ▲           ▲   1:
 *            │           │    :
 *            └─────┬─────┘    :
 *                  │    1     :
 *              ▕▔▔▔▔▔▔▔▏- - - ┘        ▕▔▔▔▔▔▔▔▔▔▔▔▏
 *              ▕ FMenu ▏- - - - - - - -▕ FMenuItem ▏
 *              ▕▁▁▁▁▁▁▁▏1             1▕▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FMENU_H
#define FMENU_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <tuple>

#include "final/fwindow.h"
#include "final/fmenulist.h"

namespace finalcut
{

// class forward declaration
class FMenuBar;
class FMenuItem;

//----------------------------------------------------------------------
// class FMenu
//----------------------------------------------------------------------

class FMenu : public FWindow, public FMenuList
{
  public:
    // Using-declaration
    using FMenuList::getItem;
    using FMenuList::isSelected;

    // Constructor
    explicit FMenu (FWidget* = nullptr);
    explicit FMenu (const FString&, FWidget* = nullptr);

    // Disable copy constructor
    FMenu (const FMenu&) = delete;

    // Destructor
    ~FMenu() override;

    // Disable copy assignment operator (=)
    FMenu& operator = (const FMenu&) = delete;

    // Accessors
    FString             getClassName() const override;
    FString             getText() const;
    FMenuItem*          getItem();

    // Mutators
    bool                setEnable (bool = true) override;
    bool                unsetEnable() override;
    bool                setDisable() override;
    void                setSelected();
    void                unsetSelected();
    bool                setMenuWidget (bool = true);
    bool                unsetMenuWidget();
    void                setStatusbarMessage (const FString&) override;
    void                setMenu (FMenu*);
    void                setText (const FString&);
    void                resetColors() override;

    // Inquiries
    bool                isSelected() const;
    bool                hasHotkey() const;
    bool                hasMenu() const;

    // Methods
    void                show() override;
    void                hide() override;

    // Event handlers
    void                onKeyPress (FKeyEvent*) override;
    void                onMouseDown (FMouseEvent*) override;
    void                onMouseUp (FMouseEvent*) override;
    void                onMouseMove (FMouseEvent*) override;
    void                onAccel (FAccelEvent*) override;

    // Callback method
    void                cb_menuitemEnabled();
    void                cb_menuitemDisabled();
    void                cb_menuitemToggled (const FMenuItem*) const;

  private:
    struct MouseStates
    {
      uChar focus_changed        : 1;
      uChar hide_sub_menu        : 1;
      uChar mouse_over_menu      : 1;
      uChar mouse_over_submenu   : 1;
      uChar mouse_over_supermenu : 1;
      uChar mouse_over_menubar   : 1;
      uChar                      : 2;  // padding bits
    };

    struct MenuText
    {
      FString text;
      std::size_t hotkeypos;
      bool no_underline;
    };

    // Constants
    static constexpr auto NOT_SET = static_cast<std::size_t>(-1);
    static constexpr bool SELECT_ITEM = true;

    // Accessors
    FWidget*     getSuperMenu() const;

    // Mutators
    void         setSuperMenu (FWidget*);

    // Inquiries
    bool         isDialog (const FWidget*) const;
    bool         isMenuBar (const FWidget*) const;
    bool         isMenu (const FWidget*) const;
    bool         isRadioMenuItem (const FWidget*) const;
    bool         isSubMenu() const;
    bool         isDialogMenu() const;
    bool         isMouseOverMenu (const FPoint&);
    bool         isMouseOverSubMenu (const FPoint&);
    bool         isMouseOverSuperMenu (const FPoint&);
    bool         isMouseOverMenuBar (const FPoint&) const;

    // Methods
    void         init();
    void         initCallbacks();
    void         calculateDimensions();
    void         adjustItems() const;
    int          adjustX(int) const;
    void         openSubMenu (FMenu*, bool = false);
    void         closeOpenedSubMenu();
    void         hideSubMenus();
    void         hideSuperMenus() const;
    bool         mouseDownOverList (const FPoint&);
    void         mouseDownSubmenu (const FMenuItem*);
    void         mouseDownSelection (FMenuItem*, bool&);
    bool         mouseUpOverList (const FPoint&);
    void         mouseMoveOverList (const FPoint&, MouseStates&);
    void         mouseMoveSelection (FMenuItem*, MouseStates&);
    void         mouseMoveDeselection (FMenuItem*, MouseStates&);
    void         mouseUpOverBorder();
    void         mouseMoveOverBorder (MouseStates&) const;
    void         passEventToSubMenu (const FMouseEvent&);
    void         passEventToSuperMenu (const FMouseEvent&);
    void         passEventToMenuBar (const FMouseEvent&) const;
    bool         containsMenuStructure (const FPoint&);
    bool         containsMenuStructure (int, int);
    FMenu*       superMenuAt (const FPoint&);
    FMenu*       superMenuAt (int, int);
    void         selectItem_PostProcessing (FMenuItem*) override;
    void         keypressMenuBar (FKeyEvent*) const;
    bool         hotkeyFound (FKey, const FKeyEvent&) const;
    bool         hotkeyMenu (FKeyEvent*);
    void         draw() override;
    void         drawItems();
    void         drawSeparator (int);
    void         drawMenuLine (FMenuItem*, int);
    void         drawCheckMarkPrefix (const FMenuItem*);
    void         drawMenuText (MenuText&);
    void         drawSubMenuIndicator (std::size_t&);
    void         drawAcceleratorKey (std::size_t&, FKey);
    void         drawTrailingSpaces (std::size_t);
    void         setLineAttributes (const FMenuItem*, int);
    void         setCursorToHotkeyPosition (FMenuItem*) const;
    void         selectPrevMenu (FKeyEvent*);
    void         selectNextMenu (FKeyEvent*);
    void         acceptSelection();
    void         closeMenu();
    void         processActivate() const;

    // Data members
    FMenuItem    menuitem{};
    FWidget*     super_menu{nullptr};
    FMenu*       opened_sub_menu{nullptr};
    FMenu*       shown_sub_menu{nullptr};
    std::size_t  max_item_width{0};
    std::size_t  hotkeypos{NOT_SET};
    bool         mouse_down{false};
    bool         has_checkable_items{false};

    // Friend functions
    friend std::tuple<bool, bool> closeOpenMenus (FMenu*, const FPoint&);

    // Friend classes
    friend class FCheckMenuItem;
    friend class FDialog;
    friend class FMenuBar;
    friend class FMenuItem;
    friend class FRadioMenuItem;
};

// non-member function forward declarations
//----------------------------------------------------------------------
std::tuple<bool, bool> closeOpenMenus (FMenu*, const FPoint&);


// FMenu inline functions
//----------------------------------------------------------------------
inline FString FMenu::getClassName() const
{ return "FMenu"; }

//----------------------------------------------------------------------
inline FString FMenu::getText() const
{ return menuitem.getText(); }

//----------------------------------------------------------------------
inline FMenuItem* FMenu::getItem()
{ return &menuitem; }

//----------------------------------------------------------------------
inline bool FMenu::setEnable (bool enable)
{ return menuitem.setEnable(enable); }

//----------------------------------------------------------------------
inline bool FMenu::unsetEnable()
{ return menuitem.unsetEnable(); }

//----------------------------------------------------------------------
inline bool FMenu::setDisable()
{ return menuitem.setDisable(); }

//----------------------------------------------------------------------
inline void FMenu::setSelected()
{ menuitem.setSelected(); }

//----------------------------------------------------------------------
inline void FMenu::unsetSelected()
{ menuitem.unsetSelected(); }

//----------------------------------------------------------------------
inline bool FMenu::unsetMenuWidget()
{ return setMenuWidget(false); }

//----------------------------------------------------------------------
inline void FMenu::setMenu (FMenu* m)
{ menuitem.setMenu(m); }

//----------------------------------------------------------------------
inline void FMenu::setText (const FString& txt)
{ menuitem.setText(txt); }

//----------------------------------------------------------------------
inline bool FMenu::isSelected() const
{ return menuitem.isSelected(); }

//----------------------------------------------------------------------
inline bool FMenu::hasHotkey() const
{ return menuitem.hasHotkey(); }

//----------------------------------------------------------------------
inline bool FMenu::hasMenu() const
{ return menuitem.hasMenu(); }

//----------------------------------------------------------------------
inline FWidget* FMenu::getSuperMenu() const
{ return super_menu; }

//----------------------------------------------------------------------
inline void FMenu::setSuperMenu (FWidget* smenu)
{ super_menu = smenu; }

//----------------------------------------------------------------------
inline bool FMenu::containsMenuStructure (const FPoint& p)
{ return containsMenuStructure (p.getX(), p.getY()); }

//----------------------------------------------------------------------
inline FMenu* FMenu::superMenuAt (const FPoint& p)
{ return superMenuAt (p.getX(), p.getY()); }

//----------------------------------------------------------------------
inline void FMenu::onAccel (FAccelEvent* ev)
{ menuitem.onAccel(ev); }

}  // namespace finalcut

#endif  // FMENU_H
