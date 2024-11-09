/***********************************************************************
* fmenu.h - Widget FMenu                                               *
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

#include <memory>
#include <tuple>

#include "final/fevent.h"
#include "final/menu/fmenulist.h"
#include "final/widget/fwindow.h"

namespace finalcut
{

// class forward declaration
class FMenuBar;
class FMenuItem;

//----------------------------------------------------------------------
// class FMenu
//----------------------------------------------------------------------

class FMenu : public FWindow
            , public FMenuList
{
  public:
    // Using-declaration
    using FMenuList::getItem;
    using FMenuList::isSelected;

    // Constructor
    explicit FMenu (FWidget* = nullptr);
    explicit FMenu (FString&&, FWidget* = nullptr);

    // Disable copy constructor
    FMenu (const FMenu&) = delete;

    // Disable move constructor
    FMenu (FMenu&&) noexcept = delete;

    // Destructor
    ~FMenu() override;

    // Disable copy assignment operator (=)
    auto operator = (const FMenu&) -> FMenu& = delete;

    // Disable move assignment operator (=)
    auto operator = (FMenu&&) noexcept -> FMenu& = delete;

    // Accessors
    auto getClassName() const -> FString override;
    auto getText() const -> FString;
    auto getItem() -> FMenuItem*;

    // Mutators
    void setEnable (bool = true) override;
    void unsetEnable() override;
    void setDisable() override;
    void setSelected();
    void unsetSelected();
    void setMenuWidget (bool = true);
    void unsetMenuWidget();
    void setStatusbarMessage (const FString&) override;
    void setMenu (FMenu*);
    void setText (const FString&);
    void resetColors() override;

    // Inquiries
    auto isSelected() const -> bool;
    auto hasHotkey() const -> bool;
    auto hasMenu() const -> bool;

    // Methods
    void show() override;
    void hide() override;

    // Event handlers
    void onKeyPress (FKeyEvent*) override;
    void onMouseDown (FMouseEvent*) override;
    void onMouseUp (FMouseEvent*) override;
    void onMouseMove (FMouseEvent*) override;
    void onAccel (FAccelEvent*) override;

    // Callback method
    void cb_menuitemEnabled();
    void cb_menuitemDisabled();
    void cb_menuitemToggled (const FMenuItem*) const;

  private:
    // Using-declaration
    using KeyMap = std::unordered_map<FKey, std::function<void(FKeyEvent*)>, EnumHash<FKey>>;

    // Enumeration
    enum class SelectItem { No, Yes };

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

    // Accessors
    auto getSuperMenu() const -> FWidget*;

    // Mutators
    void setSuperMenu (FWidget*);

    // Inquiries
    auto isDialog (const FWidget*) const -> bool;
    auto isMenuBar (const FWidget*) const -> bool;
    auto isMenu (const FWidget*) const -> bool;
    auto isRadioMenuItem (const FWidget*) const -> bool;
    auto isSubMenu() const -> bool;
    auto isDialogMenu() const -> bool;
    auto isMouseOverMenu (const FPoint&) -> bool;
    auto isMouseOverSubMenu (const FPoint&) -> bool;
    auto isMouseOverSuperMenu (const FPoint&) -> bool;
    auto isMouseOverMenuBar (const FPoint&) const -> bool;
    auto isMetaNumberKey (const FKey&) const -> bool;

    // Methods
    void init();
    void handleParentWidget();
    void initCallbacks();
    void mapKeyFunctions();
    void calculateDimensions();
    void adjustItems() const;
    auto adjustX(int) const -> int;
    void openSubMenu (FMenu*, SelectItem);
    void closeOpenedSubMenu();
    void hideSubMenus();
    void hideSuperMenus() const;
    auto isMouseOverItem (const FPoint&, const FMenuItem*) const -> bool;
    auto mouseDownOverList (const FPoint&) -> bool;
    void mouseDownSubmenu (const FMenuItem*);
    void mouseDownSelection (FMenuItem*, bool&);
    auto mouseUpOverList (const FPoint&) -> bool;
    auto initializeMouseStates (const FMouseEvent*) -> MouseStates;
    void handleCloseSubMenu (const MouseStates& ms);
    void mouseMoveOverList (const FPoint&, MouseStates&);
    void mouseMoveSelection (FMenuItem*, MouseStates&);
    void mouseMoveDeselection (FMenuItem*, MouseStates&);
    void mouseUpOverBorder();
    void mouseMoveOverBorder (MouseStates&) const;
    auto handleSubMenuEvent (const MouseStates&, const FMouseEvent&) const -> bool;
    void passEventToSubMenu (const FMouseEvent&) const;
    auto handleSuperMenuEvent (const MouseStates&, const FMouseEvent&) -> bool;
    void passEventToSuperMenu (const FMouseEvent&);
    auto handleMenuBarEvent (const MouseStates&, const FMouseEvent&) const -> bool;
    void passEventToMenuBar (const FMouseEvent&) const;
    template <typename WidgetT>
    void passEventToWidget (WidgetT, const FMouseEvent&) const;
    auto containsMenuStructure (const FPoint&) -> bool;
    auto containsMenuStructure (int, int) -> bool;
    auto superMenuAt (const FPoint&) -> FMenu*;
    auto superMenuAt (int, int) -> FMenu*;
    void selectItem_PostProcessing (FMenuItem*) override;
    void keypressMenuBar (FKeyEvent*) const;
    auto hotkeyFound (FKey, const FKeyEvent&) const -> bool;
    auto hotkeyMenu (FKeyEvent*) -> bool;
    void draw() override;
    void drawItems();
    void drawSeparator (int);
    void drawMenuLine (FMenuItem*, int);
    void drawCheckMarkPrefix (const FMenuItem*);
    void drawMenuText (MenuText&);
    void drawSubMenuIndicator (std::size_t&);
    void drawAcceleratorKey (std::size_t&, FKey);
    void drawTrailingSpaces (std::size_t);
    void setLineAttributes (const FMenuItem*, int);
    void setCursorToHotkeyPosition (FMenuItem*) const;
    void selectPrevMenu (FKeyEvent*);
    void selectNextMenu (FKeyEvent*);
    void acceptSelection();
    void closeMenu();
    void processActivate() const;

    // Data members
    KeyMap       key_map{};
    FMenuItem    menuitem{};
    FWidget*     super_menu{nullptr};
    FMenu*       opened_sub_menu{nullptr};
    FMenu*       shown_sub_menu{nullptr};
    std::size_t  max_item_width{0};
    std::size_t  hotkeypos{NOT_SET};
    bool         mouse_down{false};
    bool         has_checkable_items{false};

    // Friend functions
    friend auto closeOpenMenus (FMenu*, const FPoint&) -> std::tuple<bool, bool>;

    // Friend classes
    friend class FCheckMenuItem;
    friend class FDialog;
    friend class FDialogListMenu;
    friend class FMenuBar;
    friend class FMenuItem;
    friend class FRadioMenuItem;
};

// non-member function forward declarations
//----------------------------------------------------------------------
auto closeOpenMenus (FMenu*, const FPoint&) -> std::tuple<bool, bool>;


// FMenu inline functions
//----------------------------------------------------------------------
inline auto FMenu::getClassName() const -> FString
{ return "FMenu"; }

//----------------------------------------------------------------------
inline auto FMenu::getText() const -> FString
{ return menuitem.getText(); }

//----------------------------------------------------------------------
inline auto FMenu::getItem() -> FMenuItem*
{ return &menuitem; }

//----------------------------------------------------------------------
inline void FMenu::setEnable (bool enable)
{ menuitem.setEnable(enable); }

//----------------------------------------------------------------------
inline void FMenu::unsetEnable()
{ menuitem.unsetEnable(); }

//----------------------------------------------------------------------
inline void FMenu::setDisable()
{ menuitem.setDisable(); }

//----------------------------------------------------------------------
inline void FMenu::setSelected()
{ menuitem.setSelected(); }

//----------------------------------------------------------------------
inline void FMenu::unsetSelected()
{ menuitem.unsetSelected(); }

//----------------------------------------------------------------------
inline void FMenu::unsetMenuWidget()
{ setMenuWidget(false); }

//----------------------------------------------------------------------
inline void FMenu::setMenu (FMenu* m)
{ menuitem.setMenu(m); }

//----------------------------------------------------------------------
inline void FMenu::setText (const FString& txt)
{ menuitem.setText(txt); }

//----------------------------------------------------------------------
inline auto FMenu::isSelected() const -> bool
{ return menuitem.isSelected(); }

//----------------------------------------------------------------------
inline auto FMenu::hasHotkey() const -> bool
{ return menuitem.hasHotkey(); }

//----------------------------------------------------------------------
inline auto FMenu::hasMenu() const -> bool
{ return menuitem.hasMenu(); }

//----------------------------------------------------------------------
inline auto FMenu::getSuperMenu() const -> FWidget*
{ return super_menu; }

//----------------------------------------------------------------------
inline void FMenu::setSuperMenu (FWidget* smenu)
{ super_menu = smenu; }

//----------------------------------------------------------------------
template <typename WidgetT>
inline void FMenu::passEventToWidget (WidgetT widget, const FMouseEvent& ev) const
{
  // Mouse event handover to given widget

  const auto& type = ev.getType();
  const auto& tpos = ev.getTermPos();
  const auto& par = widget->termToWidgetPos(tpos);
  const MouseButton btn = ev.getButton();
  const auto& new_ev = \
      std::make_shared<FMouseEvent>(type, par, tpos, btn);
  setClickedWidget(widget);
  widget->mouse_down = true;
  widget->onMouseMove(new_ev.get());
}

//----------------------------------------------------------------------
inline auto FMenu::containsMenuStructure (const FPoint& p) -> bool
{ return containsMenuStructure (p.getX(), p.getY()); }

//----------------------------------------------------------------------
inline auto FMenu::superMenuAt (const FPoint& p) -> FMenu*
{ return superMenuAt (p.getX(), p.getY()); }

//----------------------------------------------------------------------
inline void FMenu::onAccel (FAccelEvent* ev)
{ menuitem.onAccel(ev); }

}  // namespace finalcut

#endif  // FMENU_H
