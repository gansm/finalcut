/***********************************************************************
* fmenuitem.h - Widget FMenuItem                                       *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2015-2019 Markus Gans                                      *
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
    explicit FMenuItem (const FString&, FWidget* = nullptr);
    FMenuItem (FKey, const FString&, FWidget* = nullptr);

    // Disable copy constructor
    FMenuItem (const FMenuItem&) = delete;

    // Destructor
    virtual ~FMenuItem();

    // Disable assignment operator (=)
    FMenuItem& operator = (const FMenuItem&) = delete;

    // Accessors
    const FString       getClassName() const override;
    FKey                getHotkey() const;
    FMenu*              getMenu() const;
    std::size_t         getTextLength() const;
    std::size_t         getTextWidth() const;
    FString             getText() const;

    // Mutators
    bool                setEnable (bool) override;
    bool                setFocus (bool) override;
    bool                setFocus() override;
    bool                unsetFocus() override;
    void                setSelected();
    void                unsetSelected();
    void                setSeparator();
    void                unsetSeparator();
    void                setCheckable();
    void                unsetCheckable();
    void                setChecked();
    void                unsetChecked();
    void                setRadioButton();
    void                unsetRadioButton();
    void                setMenu (FMenu*);
    void                setText (const FString&);

    // Inquiries
    bool                isSelected() const;
    bool                isSeparator() const;
    bool                isCheckable() const;
    bool                isChecked() const;
    bool                isRadioButton() const;
    bool                hasHotkey() const;
    bool                hasMenu() const;

    // Methods
    void                addAccelerator (FKey, FWidget*) override;
    void                delAccelerator (FWidget*) override;
    void                openMenu();

    // Event handlers
    void                onKeyPress (FKeyEvent*) override;
    void                onMouseDoubleClick (FMouseEvent*) override;
    void                onMouseDown (FMouseEvent*) override;
    void                onMouseUp (FMouseEvent*) override;
    void                onMouseMove (FMouseEvent*) override;
    void                onAccel (FAccelEvent*) override;
    void                onFocusIn (FFocusEvent*) override;
    void                onFocusOut (FFocusEvent*) override;

  protected:
    // Accessor
    FWidget*            getSuperMenu() const;

    // Mutator
    void                setSuperMenu (FWidget*);

    // Inquiries
    bool                isWindowsMenu (FWidget*) const;
    bool                isMenuBar (FWidget*) const;
    bool                isMenu (FWidget*) const;

  private:
    // Accessor
    FMenuList*          getFMenuList (FWidget&);

    // Methods
    void                init (FWidget*);
    void                updateSuperMenuDimensions();
    void                processActivate();
    void                processDeactivate();
    void                createDialogList (FMenu*);
    template <typename T>
    void                passMouseEvent (T, FMouseEvent*, fc::events);

    // Callback methods
    void                cb_switchToDialog (FWidget*, FDataPtr);
    void                cb_destroyDialog (FWidget*, FDataPtr);

    virtual void        processClicked();

    // Data members
    FString      text{};
    FMenu*       menu{nullptr};
    FWidget*     super_menu{nullptr};
    FDialog*     associated_window{nullptr};
    std::size_t  text_length{0};
    std::size_t  text_width{0};
    FKey         accel_key{0};
    FKey         hotkey{0};
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
inline const FString FMenuItem::getClassName() const
{ return "FMenuItem"; }

//----------------------------------------------------------------------
inline FKey FMenuItem::getHotkey() const
{ return hotkey; }

//----------------------------------------------------------------------
inline FMenu* FMenuItem::getMenu() const
{ return menu; }

//----------------------------------------------------------------------
inline std::size_t FMenuItem::getTextLength() const
{ return text_length; }

//----------------------------------------------------------------------
inline std::size_t FMenuItem::getTextWidth() const
{ return text_width; }

//----------------------------------------------------------------------
inline FString FMenuItem::getText() const
{ return text; }

//----------------------------------------------------------------------
inline bool FMenuItem::setFocus()
{ return setFocus(true); }

//----------------------------------------------------------------------
inline bool FMenuItem::unsetFocus()
{ return setFocus(false); }

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
inline void FMenuItem::setCheckable()
{ checkable = true; }

//----------------------------------------------------------------------
inline void FMenuItem::unsetCheckable()
{ checkable = false; }

//----------------------------------------------------------------------
inline void FMenuItem::setChecked()
{ checked = true; }

//----------------------------------------------------------------------
inline void FMenuItem::unsetChecked()
{ checked = false; }

//----------------------------------------------------------------------
inline void FMenuItem::setRadioButton()
{ radio_button = true; }

//----------------------------------------------------------------------
inline void FMenuItem::unsetRadioButton()
{ radio_button = false; }

//----------------------------------------------------------------------
inline void FMenuItem::setMenu(FMenu* m)
{ menu = m; }

//----------------------------------------------------------------------
inline bool FMenuItem::isSelected() const
{ return selected; }

//----------------------------------------------------------------------
inline bool FMenuItem::isSeparator() const
{ return separator; }

//----------------------------------------------------------------------
inline bool FMenuItem::isCheckable() const
{ return checkable; }

//----------------------------------------------------------------------
inline bool FMenuItem::isChecked() const
{ return checked; }

//----------------------------------------------------------------------
inline bool FMenuItem::isRadioButton() const
{ return radio_button; }

//----------------------------------------------------------------------
inline bool FMenuItem::hasHotkey() const
{ return bool(hotkey != 0); }

//----------------------------------------------------------------------
inline bool FMenuItem::hasMenu() const
{ return bool(menu != 0); }

//----------------------------------------------------------------------
inline FWidget* FMenuItem::getSuperMenu() const
{ return super_menu; }

//----------------------------------------------------------------------
inline void FMenuItem::setSuperMenu (FWidget* smenu)
{ super_menu = smenu; }

}  // namespace finalcut

#endif  // FMENUITEM_H
