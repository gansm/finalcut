/***********************************************************************
* fmenuitem.h - Widget FMenuItem                                       *
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
 *      ▕▔▔▔▔▔▔▔▔▔▔▔▏*       1▕▔▔▔▔▔▔▔▏
 *      ▕ FMenuItem ▏-┬- - - -▕ FMenu ▏
 *      ▕▁▁▁▁▁▁▁▁▁▁▁▏ :       ▕▁▁▁▁▁▁▁▏
 *                    :
 *                    :      1▕▔▔▔▔▔▔▔▔▔▔▔▏
 *                    └- - - -▕ FMenuList ▏
 *                            ▕▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FMENUITEM_H
#define FMENUITEM_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fwidget.h"

namespace finalcut
{

// class forward declaration
class FDialog;
class FMenu;
class FMenuList;

//----------------------------------------------------------------------
// class FMenuItem
//----------------------------------------------------------------------

class FMenuItem : public FWidget
{
  public:
    // Using-declarations
    using FWidget::addAccelerator;
    using FWidget::delAccelerator;
    using FWidget::setEnable;

    // Constructor
    explicit FMenuItem (FWidget* = nullptr);
    explicit FMenuItem (FString&&, FWidget* = nullptr);
    FMenuItem (FKey, FString&&, FWidget* = nullptr);

    // Disable copy constructor
    FMenuItem (const FMenuItem&) = delete;

    // Disable move constructor
    FMenuItem (FMenuItem&&) noexcept = delete;

    // Destructor
    ~FMenuItem() override;

    // Disable copy assignment operator (=)
    auto operator = (const FMenuItem&) -> FMenuItem& = delete;

    // Disable move assignment operator (=)
    auto operator = (FMenuItem&&) noexcept -> FMenuItem& = delete;
    // Accessors
    auto getClassName() const -> FString override;
    auto getHotkey() const noexcept -> FKey;
    auto getMenu() const -> FMenu*;
    auto getTextLength() const noexcept -> std::size_t;
    auto getTextWidth() const noexcept -> std::size_t;
    auto getText() const -> FString;

    // Mutators
    void setEnable (bool = true) override;
    void setSelected();
    void unsetSelected();
    void setSeparator();
    void unsetSeparator();
    void setCheckable() noexcept;
    void unsetCheckable() noexcept;
    void setChecked() noexcept;
    void unsetChecked() noexcept;
    void setRadioButton() noexcept;
    void unsetRadioButton() noexcept;
    void setMenu (FMenu*);
    void setText (const FString&);

    // Inquiries
    auto isSelected() const -> bool;
    auto isSeparator() const -> bool;
    auto isCheckable() const -> bool;
    auto isChecked() const -> bool;
    auto isRadioButton() const -> bool;
    auto hasHotkey() const -> bool;
    auto hasMenu() const -> bool;

    // Methods
    void addAccelerator (FKey, FWidget*) & final;
    void delAccelerator (FWidget*) & final;
    void openMenu() const;

    // Event handlers
    void onKeyPress (FKeyEvent*) override;
    void onMouseDoubleClick (FMouseEvent*) override;
    void onMouseDown (FMouseEvent*) override;
    void onMouseUp (FMouseEvent*) override;
    void onMouseMove (FMouseEvent*) override;
    void onAccel (FAccelEvent*) override;
    void onFocusIn (FFocusEvent*) override;
    void onFocusOut (FFocusEvent*) override;

  protected:
    // Accessor
    auto getSuperMenu() const -> FWidget*;

    // Mutator
    void setSuperMenu (FWidget*);

    // Inquiries
    auto isDialog (const FWidget*) const -> bool;
    auto isMenuBar (const FWidget*) const -> bool;
    auto isMenu (const FWidget*) const -> bool;

    // Method
    void initLayout() override;

  private:
    // Accessor
    auto getFMenuList (FWidget&) -> FMenuList*;

    // Methods
    void init();
    void calculateTextDimensions();
    void updateSuperMenuDimensions();
    void updateMenubarDimensions() const;
    void processEnable() const;
    void processDisable() const;
    void processActivate() const;
    void processDeactivate() const;
    void createDialogList (FMenu*) const;
    template <typename T>
    void passMouseEvent (T, const FMouseEvent*, Event) const;
    void resetSelectedItem (const FMenuList*) const;

    // Callback methods
    void cb_switchToDialog (FDialog*) const;
    void cb_destroyDialog (FDialog*);

    virtual void processClicked();

    // Data members
    FString      text{};
    FMenu*       menu{nullptr};
    FWidget*     super_menu{nullptr};
    FDialog*     associated_window{nullptr};
    std::size_t  text_length{0};
    std::size_t  text_width{0};
    FKey         accel_key{FKey::None};
    FKey         hotkey{FKey::None};
    bool         selected{false};
    bool         separator{false};
    bool         checkable{false};
    bool         checked{false};
    bool         radio_button{false};
    bool         dialog_index{false};

    // Friend classes
    friend class FDialogListMenu;
    friend class FMenuList;
    friend class FMenuBar;
    friend class FMenu;
};


// FMenuItem inline functions
//----------------------------------------------------------------------
inline auto FMenuItem::getClassName() const -> FString
{ return "FMenuItem"; }

//----------------------------------------------------------------------
inline auto FMenuItem::getHotkey() const noexcept -> FKey
{ return hotkey; }

//----------------------------------------------------------------------
inline auto FMenuItem::getMenu() const -> FMenu*
{ return menu; }

//----------------------------------------------------------------------
inline auto FMenuItem::getTextLength() const noexcept -> std::size_t
{ return text_length; }

//----------------------------------------------------------------------
inline auto FMenuItem::getTextWidth() const noexcept -> std::size_t
{ return text_width; }

//----------------------------------------------------------------------
inline auto FMenuItem::getText() const -> FString
{ return text; }

//----------------------------------------------------------------------
inline void FMenuItem::setSeparator()
{
  separator = true;
  unsetFocusable();
}

//----------------------------------------------------------------------
inline void FMenuItem::unsetSeparator()
{
  separator = false;
  setFocusable();
}

//----------------------------------------------------------------------
inline void FMenuItem::setCheckable() noexcept
{ checkable = true; }

//----------------------------------------------------------------------
inline void FMenuItem::unsetCheckable() noexcept
{ checkable = false; }

//----------------------------------------------------------------------
inline void FMenuItem::setChecked() noexcept
{ checked = true; }

//----------------------------------------------------------------------
inline void FMenuItem::unsetChecked() noexcept
{ checked = false; }

//----------------------------------------------------------------------
inline void FMenuItem::setRadioButton() noexcept
{ radio_button = true; }

//----------------------------------------------------------------------
inline void FMenuItem::unsetRadioButton() noexcept
{ radio_button = false; }

//----------------------------------------------------------------------
inline void FMenuItem::setMenu(FMenu* m)
{ menu = m; }

//----------------------------------------------------------------------
inline auto FMenuItem::isSelected() const -> bool
{ return selected; }

//----------------------------------------------------------------------
inline auto FMenuItem::isSeparator() const -> bool
{ return separator; }

//----------------------------------------------------------------------
inline auto FMenuItem::isCheckable() const -> bool
{ return checkable; }

//----------------------------------------------------------------------
inline auto FMenuItem::isChecked() const -> bool
{ return checked; }

//----------------------------------------------------------------------
inline auto FMenuItem::isRadioButton() const -> bool
{ return radio_button; }

//----------------------------------------------------------------------
inline auto FMenuItem::hasHotkey() const -> bool
{ return hotkey != FKey::None; }

//----------------------------------------------------------------------
inline auto FMenuItem::hasMenu() const -> bool
{ return menu != nullptr; }

//----------------------------------------------------------------------
inline auto FMenuItem::getSuperMenu() const -> FWidget*
{ return super_menu; }

//----------------------------------------------------------------------
inline void FMenuItem::setSuperMenu (FWidget* smenu)
{ super_menu = smenu; }

}  // namespace finalcut

#endif  // FMENUITEM_H
