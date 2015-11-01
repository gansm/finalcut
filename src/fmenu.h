// File: fmenubar.h
// Provides: class FMenu
//
//  Inheritance diagram
//  ═══════════════════
//
// ▕▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▏
// ▕ FObject ▏ ▕  FTerm  ▏
// ▕▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▏
//      ▲           ▲
//      │           │
//      └─────┬─────┘
//            │
//       ▕▔▔▔▔▔▔▔▔▔▏
//       ▕ FWidget ▏
//       ▕▁▁▁▁▁▁▁▁▁▏
//            ▲
//            │
//       ▕▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▔▔▏       ▕▔▔▔▔▔▔▔▔▔▔▔▏
//       ▕ FWindow ▏ ▕ FMenuList ▏- - - -▕ FMenuItem ▏
//       ▕▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▁▁▏1     *▕▁▁▁▁▁▁▁▁▁▁▁▏
//            ▲           ▲   1:
//            │           │    :
//            └─────┬─────┘    :
//                  │    1     :
//              ▕▔▔▔▔▔▔▔▏- - - ┘        ▕▔▔▔▔▔▔▔▔▔▔▔▏
//              ▕ FMenu ▏- - - - - - - -▕ FMenuItem ▏
//              ▕▁▁▁▁▁▁▁▏1             1▕▁▁▁▁▁▁▁▁▁▁▁▏

#ifndef _FMENU_H
#define _FMENU_H

#include "fwindow.h"
#include "fmenubar.h"
#include "fmenulist.h"
#include "fmenuitem.h"


//----------------------------------------------------------------------
// class FMenu
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FMenu : public FWindow, public FMenuList
{
 private:
   FMenuItem* item;
   FMenuItem* selectedListItem;
   FWidget*   super_menu;
   uInt       maxItemWidth;
   bool       mouse_down;

 private:
   FMenu (const FMenu&);
   FMenu& operator = (const FMenu&);
   void       init(FWidget*);
   void       menu_dimension();
   bool       isMenuBar (FWidget*) const;
   bool       isMenu (FWidget*) const;
   FWidget*   getSuperMenu() const;
   void       setSuperMenu (FWidget*);
   void       hideSubMenus();
   void       hideSuperMenus();
   bool       containsMenuStructure (const FPoint&) const;
   bool       containsMenuStructure (int, int) const;
   bool       selectNextItem();
   bool       selectPrevItem();
   void       keypressMenuBar (FKeyEvent*);
   int        getHotkeyPos (wchar_t*&, wchar_t*&, uInt);
   void       draw();
   void       drawBorder();
   void       drawMenuShadow();
   void       drawItems();
   void       drawSeparator(int);
   void       processActivate();

 public:
   explicit FMenu (FWidget* = 0);  // constructor
   FMenu (FString&, FWidget* = 0);
   FMenu (const std::string&, FWidget* = 0);
   FMenu (const char*, FWidget* = 0);
   virtual ~FMenu();  // destructor
   virtual const char* getClassName() const;

   void       onKeyPress (FKeyEvent*);
   void       onMouseDown (FMouseEvent*);
   void       onMouseUp (FMouseEvent*);
   void       onMouseMove (FMouseEvent*);
   void       onAccel (FAccelEvent*);
   void       show();
   void       hide();
   // make every setGeometry from FWidget available
   using FWidget::setGeometry;
   void       setGeometry (int, int, int, int, bool = true);
   void       setStatusbarMessage (FString);
   FMenuItem* getItem() const;
   FString    getText() const;
   bool       setEnable(bool);
   bool       setEnable();
   bool       unsetEnable();
   bool       setDisable();
   bool       isEnabled() const;
   void       setSelected();
   void       unsetSelected();
   bool       isSelected() const;
   void       selectFirstItemInList();
   void       unselectItemInList();
   FMenuItem* getSelectedListItem() const;
   bool       hasSelectedListItem() const;
   bool       hasHotkey() const;
   void       setMenu (FMenu*);
   bool       hasMenu() const;
   bool       setTransparentShadow (bool);
   bool       setTransparentShadow();
   bool       unsetTransparentShadow();
   bool       hasTransparentShadow();
   void       setText (FString&);
   void       setText (const std::string&);
   void       setText (const char*);
   void       cb_menuitem_activated (FWidget*, void*);
   void       cb_menuitem_deactivated (FWidget*, void*);

 private:
   friend class FApplication;
   friend class FMenuItem;
};
#pragma pack(pop)


// FMenu inline functions
//----------------------------------------------------------------------
inline FWidget* FMenu::getSuperMenu() const
{ return super_menu; }

//----------------------------------------------------------------------
inline void FMenu::setSuperMenu (FWidget* smenu)
{ super_menu = smenu; }

//----------------------------------------------------------------------
inline bool FMenu::containsMenuStructure (const FPoint& p) const
{ return containsMenuStructure (p.getX(), p.getY()); }

//----------------------------------------------------------------------
inline const char* FMenu::getClassName() const
{ return "FMenu"; }

//----------------------------------------------------------------------
inline void FMenu::onAccel (FAccelEvent* ev)
{ item->onAccel(ev); }

//----------------------------------------------------------------------
inline FMenuItem* FMenu::getItem() const
{ return item; }

//----------------------------------------------------------------------
inline FString FMenu::getText() const
{ return item->getText(); }

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
inline bool FMenu::isEnabled() const
{ return item->isEnabled(); }

//----------------------------------------------------------------------
inline void FMenu::setSelected()
{ item->setSelected(); }

//----------------------------------------------------------------------
inline void FMenu::unsetSelected()
{ item->unsetSelected(); }

//----------------------------------------------------------------------
inline bool FMenu::isSelected() const
{ return item->isSelected(); }

//----------------------------------------------------------------------
inline FMenuItem* FMenu::getSelectedListItem() const
{ return selectedListItem; }

//----------------------------------------------------------------------
inline bool FMenu::hasSelectedListItem() const
{ return selectedListItem; }

//----------------------------------------------------------------------
inline bool FMenu::hasHotkey() const
{ return item->hasHotkey(); }

//----------------------------------------------------------------------
inline void FMenu::setMenu (FMenu* m)
{ item->setMenu(m); }

//----------------------------------------------------------------------
inline bool FMenu::hasMenu() const
{ return item->hasMenu(); }

//----------------------------------------------------------------------
inline bool FMenu::setTransparentShadow()
{ return setTransparentShadow(true); }

//----------------------------------------------------------------------
inline bool FMenu::unsetTransparentShadow()
{ return setTransparentShadow(false); }

//----------------------------------------------------------------------
inline bool FMenu::hasTransparentShadow()
{ return ((flags & TRANS_SHADOW) != 0); }

//----------------------------------------------------------------------
inline void FMenu::setText (FString& txt)
{ item->setText(txt); }

//----------------------------------------------------------------------
inline void FMenu::setText (const std::string& txt)
{ item->setText(txt); }

//----------------------------------------------------------------------
inline void FMenu::setText (const char* txt)
{ item->setText(txt); }


#endif  // _FMENU_H
