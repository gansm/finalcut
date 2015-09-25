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
//       ▕▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▔▔▏1     *▕▔▔▔▔▔▔▔▔▔▔▔▏
//       ▕ FWindow ▏ ▕ FMenuList ▏- - - -▕ FMenuItem ▏
//       ▕▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▁▁▏       ▕▁▁▁▁▁▁▁▁▁▁▁▏
//            ▲           ▲
//            │           │
//            └─────┬─────┘
//                  │
//              ▕▔▔▔▔▔▔▔▏
//              ▕ FMenu ▏
//              ▕▁▁▁▁▁▁▁▏

#ifndef _FMENU_H
#define _FMENU_H

#include "fwindow.h"
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
   FMenuList* super_menu;
   uInt       maxItemWidth;
   int        current;
   bool       mouse_down;

 private:
   FMenu (const FMenu&);
   FMenu& operator = (const FMenu&);
   void       init();
   void       menu_dimension();
   bool       isMenuBar (FWidget*) const;

   FMenuList* superMenu() const;
   void       setSuperMenu (FMenuList*);
   int        getHotkeyPos (wchar_t*&, wchar_t*&, uInt);
   void       draw();
   void       drawBorder();
   void       drawItems();
   void       processActivate();

 public:
   explicit FMenu (FWidget* parent=0);  // constructor
   FMenu (FString&, FWidget* parent=0);
   FMenu (const std::string&, FWidget* parent=0);
   FMenu (const char*, FWidget* parent=0);
   virtual ~FMenu();  // destructor
   virtual const char* getClassName() const;

   void       onMouseDown (FMouseEvent*);
   void       onMouseUp (FMouseEvent*);
   void       onMouseMove (FMouseEvent*);
   void       hide();
   // make every setGeometry from FWidget available
   using FWidget::setGeometry;
   void       setGeometry (int, int, int, int, bool adjust=true);
   FMenuItem* getItem() const;

   void       onAccel (FAccelEvent*);
   FString    getText() const;
   void       setActive();
   void       unsetActive();
   bool       isActivated() const;
   void       setSelected();
   void       unsetSelected();
   bool       isSelected() const;
   bool       hasHotkey() const;
   void       setMenu (FMenu*);
   bool       hasMenu() const;
   void       setText (FString&);
   void       setText (const std::string&);
   void       setText (const char*);
   void       insert (FMenuItem*);
   void       remove (FMenuItem*);
   void       cb_menuitem_activated (FWidget*, void*);
};
#pragma pack(pop)


// FMenu inline functions
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
inline void FMenu::setActive()
{ item->setActive(); }

//----------------------------------------------------------------------
inline void FMenu::unsetActive()
{ item->unsetActive(); }

//----------------------------------------------------------------------
inline bool FMenu::isActivated() const
{ return item->isActivated(); }

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
inline bool FMenu::hasHotkey() const
{ return item->hasHotkey(); }

//----------------------------------------------------------------------
inline void FMenu::setMenu (FMenu* m)
{ item->setMenu(m); }

//----------------------------------------------------------------------
inline bool FMenu::hasMenu() const
{ return item->hasMenu(); }

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
