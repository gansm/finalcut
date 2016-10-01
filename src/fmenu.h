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
//      │           │                   *▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
//      └─────┬─────┘               ┌ - -▕ FRadioMenuItem ▏
//            │                     :    ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
//       ▕▔▔▔▔▔▔▔▔▔▏                :
//       ▕ FWidget ▏                :   *▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
//       ▕▁▁▁▁▁▁▁▁▁▏                ├ - -▕ FCheckMenuItem ▏
//            ▲                     :    ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
//            │                   1 :
//       ▕▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▔▔▏- ┘    ▕▔▔▔▔▔▔▔▔▔▔▔▏
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
   FMenuItem*   item;
   FWidget*     super_menu;
   FMenu*       open_sub_menu;
   uInt         max_item_width;
   bool         mouse_down;
   bool         has_checkable_items;

 private:
   // Disable copy constructor
   FMenu (const FMenu&);
   // Disable assignment operator (=)
   FMenu& operator = (const FMenu&);

   void         init(FWidget*);
   void         calculateDimensions();
   void         adjustItems();
   int          adjustX(int);
   bool         isWindowsMenu (FWidget*) const;
   bool         isMenuBar (FWidget*) const;
   bool         isMenu (FWidget*) const;
   bool         isRadioMenuItem (FWidget*) const;
   FWidget*     getSuperMenu() const;
   void         setSuperMenu (FWidget*);
   bool         isSubMenu() const;
   void         openSubMenu (FMenu*);
   void         hideSubMenus();
   void         hideSuperMenus();
   bool         containsMenuStructure (const FPoint&);
   bool         containsMenuStructure (int, int);
   FMenu*       superMenuAt (const FPoint&);
   FMenu*       superMenuAt (int, int);
   bool         selectNextItem();
   bool         selectPrevItem();
   void         keypressMenuBar (FKeyEvent*&);
   bool         hotkeyMenu (FKeyEvent*&);
   int          getHotkeyPos (wchar_t*&, wchar_t*&, uInt);
   void         draw();
   // make every drawBorder from FWidget available
   using FWidget::drawBorder;
   virtual void drawBorder();
   void         drawMenuShadow();
   void         drawItems();
   void         drawSeparator(int);
   void         processActivate();

 public:
   // Constructor
   explicit FMenu (FWidget* = 0);
   FMenu (FString&, FWidget* = 0);
   FMenu (const std::string&, FWidget* = 0);
   FMenu (const char*, FWidget* = 0);
   // Destructor
   virtual ~FMenu();

   virtual const char* getClassName() const;

   // Event handlers
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
   bool       setMenuWidget (bool);
   bool       setMenuWidget();
   bool       unsetMenuWidget();
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

   // Callback method
   void       cb_menuitem_toggled (FWidget*, void*);

 private:
   friend class FApplication;
   friend class FCheckMenuItem;
   friend class FDialog;
   friend class FMenuBar;
   friend class FMenuItem;
   friend class FRadioMenuItem;
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
inline bool FMenu::containsMenuStructure (const FPoint& p)
{ return containsMenuStructure (p.getX(), p.getY()); }

//----------------------------------------------------------------------
inline FMenu* FMenu::superMenuAt (const FPoint& p)
{ return superMenuAt (p.getX(), p.getY()); }

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
inline bool FMenu::setMenuWidget()
{ return setMenuWidget(true); }

//----------------------------------------------------------------------
inline bool FMenu::unsetMenuWidget()
{ return setMenuWidget(false); }

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
{ return ((flags & fc::trans_shadow) != 0); }

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
