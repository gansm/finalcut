/***********************************************************************
* fmenuitem.h - Widget FMenuItem                                       *
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

#pragma pack(push)
#pragma pack(1)

class FMenuItem : public FWidget
{
  public:
    // Using-declarations
    using FWidget::addAccelerator;
    using FWidget::delAccelerator;
    using FWidget::setEnable;

    // Constructor
    explicit FMenuItem (FWidget* = 0);
    explicit FMenuItem (const FString&, FWidget* = 0);
    FMenuItem (FKey, const FString&, FWidget* = 0);

    // Destructor
    virtual ~FMenuItem();

    // Accessors
    const char*  getClassName() const;
    uChar        getHotkey() const;
    FMenu*       getMenu() const;
    std::size_t  getTextLength() const;
    FString      getText() const;

    // Mutators
    virtual bool setEnable (bool);
    virtual bool setFocus (bool);
    virtual bool setFocus();
    virtual bool unsetFocus();
    void         setSelected();
    void         unsetSelected();
    void         setSeparator();
    void         unsetSeparator();
    void         setChecked();
    void         unsetChecked();
    void         setMenu (FMenu*);
    void         setText (const FString&);

    // Inquiries
    bool         isSelected() const;
    bool         isSeparator() const;
    bool         isChecked() const;
    bool         hasHotkey() const;
    bool         hasMenu() const;

    // Methods
    virtual void addAccelerator (FKey, FWidget*);
    virtual void delAccelerator (FWidget*);
    void         openMenu();

    // Event handlers
    virtual void onKeyPress (FKeyEvent*);
    virtual void onMouseDoubleClick (FMouseEvent*);
    virtual void onMouseDown (FMouseEvent*);
    virtual void onMouseUp (FMouseEvent*);
    virtual void onMouseMove (FMouseEvent*);
    virtual void onAccel (FAccelEvent*);
    virtual void onFocusIn (FFocusEvent*);
    virtual void onFocusOut (FFocusEvent*);

  protected:
    // Accessor
    FWidget*     getSuperMenu() const;

    // Mutator
    void         setSuperMenu (FWidget*);

    // Inquiries
    bool         isWindowsMenu (FWidget*) const;
    bool         isMenuBar (FWidget*) const;
    bool         isMenu (FWidget*) const;

    // Data Members
    FString      text;
    bool         selected;
    bool         separator;
    bool         checkable;
    bool         checked;
    bool         radio_button;
    bool         dialog_index;
    std::size_t  text_length;
    uChar        hotkey;
    FKey         accel_key;
    FMenu*       menu;
    FWidget*     super_menu;
    FDialog*     associated_window;

  private:
    // Disable copy constructor
    FMenuItem (const FMenuItem&);

    // Disable assignment operator (=)
    FMenuItem& operator = (const FMenuItem&);

    // Accessor
    FMenuList*   getFMenuList (FWidget&);

    // Methods
    void         init (FWidget*);
    uChar        hotKey();
    void         processActivate();
    void         processDeactivate();
    void         createDialogList (FMenu*);
    template <typename T>
    void         passMouseEvent (T, FMouseEvent*, fc::events);

    // Callback methods
    void         cb_switchToDialog (FWidget*, data_ptr);
    void         cb_destroyDialog (FWidget*, data_ptr);

    virtual void processClicked();

    // Friend classes
    friend class FDialogListMenu;
    friend class FMenuList;
    friend class FMenuBar;
    friend class FMenu;
};
#pragma pack(pop)


// FMenuItem inline functions
//----------------------------------------------------------------------
inline const char* FMenuItem::getClassName() const
{ return "FMenuItem"; }

//----------------------------------------------------------------------
inline uChar FMenuItem::getHotkey() const
{ return hotkey; }

//----------------------------------------------------------------------
inline FMenu* FMenuItem::getMenu() const
{ return menu; }

//----------------------------------------------------------------------
inline std::size_t FMenuItem::getTextLength() const
{ return text_length; }

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
inline void FMenuItem::setChecked()
{ checked = true; }

//----------------------------------------------------------------------
inline void FMenuItem::unsetChecked()
{ checked = false; }

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
inline bool FMenuItem::isChecked() const
{ return checked; }

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
