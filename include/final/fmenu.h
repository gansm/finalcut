/***********************************************************************
* fmenu.h - Widget FMenu                                               *
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

#include "final/fwindow.h"
#include "final/fmenubar.h"
#include "final/fmenulist.h"
#include "final/fmenuitem.h"


//----------------------------------------------------------------------
// class FMenu
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FMenu : public FWindow, public FMenuList
{
  public:
    // Constructor
    explicit FMenu (FWidget* = 0);
    FMenu (const FString&, FWidget* = 0);

    // Destructor
    virtual ~FMenu();

    // Accessors
    virtual const char* getClassName() const;
    FString             getText() const;
    FMenuItem*          getItem() const;

    // Mutators
    bool                setEnable(bool);
    bool                setEnable();
    bool                unsetEnable();
    bool                setDisable();
    void                setSelected();
    void                unsetSelected();
    bool                setMenuWidget (bool);
    bool                setMenuWidget();
    bool                unsetMenuWidget();
    void                setStatusbarMessage (const FString&);
    void                setMenu (FMenu*);
    void                setText (const FString&);

    // Inquiries
    bool                isEnabled() const;
    bool                isSelected() const;
    bool                hasHotkey() const;
    bool                hasMenu() const;

    // Methods
    void                show();
    void                hide();

    // Event handlers
    void                onKeyPress (FKeyEvent*);
    void                onMouseDown (FMouseEvent*);
    void                onMouseUp (FMouseEvent*);
    void                onMouseMove (FMouseEvent*);
    void                onAccel (FAccelEvent*);

    // Callback method
    void                cb_menuitem_toggled (FWidget*, data_ptr);

  private:
    // Typedef
    typedef struct
    {
      uChar focus_changed        : 1;
      uChar hide_sub_menu        : 1;
      uChar mouse_over_menu      : 1;
      uChar mouse_over_submenu   : 1;
      uChar mouse_over_supermenu : 1;
      uChar mouse_over_menubar   : 1;
      uChar                      : 2;  // padding bits
    } mouseStates;

    typedef struct
    {
      wchar_t* text;
      int length;
      int hotkeypos;
      bool no_underline;
    } menuText;

    // Constants
    static const bool SELECT_ITEM = true;

    // Disable copy constructor
    FMenu (const FMenu&);

    // Disable assignment operator (=)
    FMenu& operator = (const FMenu&);

    // Accessors
    FWidget*     getSuperMenu() const;

    // Mutators
    void         setSuperMenu (FWidget*);

    // Inquiries
    bool         isWindowsMenu (FWidget*) const;
    bool         isMenuBar (FWidget*) const;
    bool         isMenu (FWidget*) const;
    bool         isRadioMenuItem (FWidget*) const;
    bool         isSubMenu() const;
    bool         isMouseOverMenu (const FPoint&);
    bool         isMouseOverSubMenu (const FPoint&);
    bool         isMouseOverSuperMenu (const FPoint&);
    bool         isMouseOverMenuBar (const FPoint&);

    // Methods
    void         init(FWidget*);
    void         calculateDimensions();
    void         adjustItems();
    int          adjustX(int);
    void         openSubMenu (FMenu*, bool = false);
    void         closeOpenedSubMenu();
    void         hideSubMenus();
    void         hideSuperMenus();
    bool         mouseDownOverList (FPoint);
    bool         mouseUpOverList (FPoint);
    bool         mouseMoveOverList (FPoint, mouseStates&);
    void         mouseUpOverBorder();
    void         mouseMoveOverBorder (mouseStates&);
    void         passEventToSubMenu (FMouseEvent*& ev);
    void         passEventToSuperMenu (FMouseEvent*& ev);
    void         passEventToMenuBar (FMouseEvent*& ev);
    bool         containsMenuStructure (const FPoint&);
    bool         containsMenuStructure (int, int);
    FMenu*       superMenuAt (const FPoint&);
    FMenu*       superMenuAt (int, int);
    bool         selectNextItem();
    bool         selectPrevItem();
    void         keypressMenuBar (FKeyEvent*&);
    bool         hotkeyMenu (FKeyEvent*&);
    int          getHotkeyPos (wchar_t[], wchar_t[], uInt);
    void         draw();
    void         drawItems();
    void         drawSeparator (int);
    void         drawMenuLine (FMenuItem*, int);
    void         drawCheckMarkPrefix (FMenuItem*);
    void         drawMenuText (menuText&);
    void         drawSubMenuIndicator (int&);
    void         drawAcceleratorKey (int&, int);
    void         drawTrailingSpaces (int);
    void         setLineAttributes (FMenuItem*, int);
    void         setCursorToHotkeyPosition (FMenuItem*);
    void         processActivate();

    // Friend classes
    friend class FApplication;
    friend class FCheckMenuItem;
    friend class FDialog;
    friend class FMenuBar;
    friend class FMenuItem;
    friend class FRadioMenuItem;

    // Data Members
    FMenuItem*   item;
    FWidget*     super_menu;
    FMenu*       opened_sub_menu;
    FMenu*       shown_sub_menu;
    uInt         max_item_width;
    int          hotkeypos;
    bool         mouse_down;
    bool         has_checkable_items;
};
#pragma pack(pop)


// FMenu inline functions
//----------------------------------------------------------------------
inline const char* FMenu::getClassName() const
{ return "FMenu"; }

//----------------------------------------------------------------------
inline FString FMenu::getText() const
{ return item->getText(); }

//----------------------------------------------------------------------
inline FMenuItem* FMenu::getItem() const
{ return item; }

//----------------------------------------------------------------------
inline bool FMenu::setEnable(bool on)
{ return item->setEnable(on); }

//----------------------------------------------------------------------
inline bool FMenu::setEnable()
{ return item->setEnable(); }

//----------------------------------------------------------------------
inline bool FMenu::unsetEnable()
{ return item->unsetEnable(); }

//----------------------------------------------------------------------
inline bool FMenu::setDisable()
{ return item->setDisable(); }

//----------------------------------------------------------------------
inline void FMenu::setSelected()
{ item->setSelected(); }

//----------------------------------------------------------------------
inline void FMenu::unsetSelected()
{ item->unsetSelected(); }

//----------------------------------------------------------------------
inline bool FMenu::setMenuWidget()
{ return setMenuWidget(true); }

//----------------------------------------------------------------------
inline bool FMenu::unsetMenuWidget()
{ return setMenuWidget(false); }

//----------------------------------------------------------------------
inline void FMenu::setMenu (FMenu* m)
{ item->setMenu(m); }

//----------------------------------------------------------------------
inline void FMenu::setText (const FString& txt)
{ item->setText(txt); }

//----------------------------------------------------------------------
inline bool FMenu::isEnabled() const
{ return item->isEnabled(); }

//----------------------------------------------------------------------
inline bool FMenu::isSelected() const
{ return item->isSelected(); }

//----------------------------------------------------------------------
inline bool FMenu::hasHotkey() const
{ return item->hasHotkey(); }

//----------------------------------------------------------------------
inline bool FMenu::hasMenu() const
{ return item->hasMenu(); }

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
{ item->onAccel(ev); }


#endif  // FMENU_H
