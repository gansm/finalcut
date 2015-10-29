// File: fmenuitem.h
// Provides: class FMenuItem
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
//      ▕▔▔▔▔▔▔▔▔▔▔▔▏1       *▕▔▔▔▔▔▔▔▏
//      ▕ FMenuItem ▏-┬- - - -▕ FMenu ▏
//      ▕▁▁▁▁▁▁▁▁▁▁▁▏ :       ▕▁▁▁▁▁▁▁▏
//                    :
//                    :      *▕▔▔▔▔▔▔▔▔▔▔▔▏
//                    └- - - -▕ FMenuList ▏
//                            ▕▁▁▁▁▁▁▁▁▁▁▁▏

#ifndef _FMENUITEM_H
#define _FMENUITEM_H

#include "fwidget.h"


class FMenu;
class FMenuList;

//----------------------------------------------------------------------
// class FMenuItem
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FMenuItem : public FWidget
{
 private:
   FString    text;
   bool       selected;
   bool       separator;
   bool       checked;
   uInt       text_length;
   int        hotkey;
 //int        accel_key;
   FMenu*     menu;
   FWidget*   super_menu;

 private:
   FMenuItem (const FMenuItem&);
   FMenuItem& operator = (const FMenuItem&);
   void       init (FWidget*);
   uChar      getHotkey();
   bool       isMenuBar (FWidget*) const;
   bool       isMenu (FWidget*) const;
   FWidget*   getSuperMenu() const;
   void       setSuperMenu (FWidget*);
   void       processActivate();
   void       processDeactivate();
   void       processClicked();

 public:
   explicit FMenuItem (FWidget* = 0);
   FMenuItem (FString&, FWidget* = 0);
   FMenuItem (const std::string&, FWidget* = 0);
   FMenuItem (const char*, FWidget* = 0);
   virtual ~FMenuItem();

   void       onKeyPress (FKeyEvent*);
   void       onMouseDown (FMouseEvent*);
   void       onMouseUp (FMouseEvent*);
   void       onMouseMove (FMouseEvent*);
   void       onAccel (FAccelEvent*);
   void       onFocusIn (FFocusEvent*);
   void       onFocusOut (FFocusEvent*);
   FString    getText() const;
   // make every setEnable from FWidget available
   using FWidget::setEnable;
   bool       setEnable(bool);
   bool       setFocus(bool);
   bool       setFocus();
   bool       unsetFocus();
   void       setSelected();
   void       unsetSelected();
   bool       isSelected() const;
   void       setSeparator();
   void       unsetSeparator();
   bool       isSeparator() const;
   void       setChecked();
   void       unsetChecked();
   bool       isChecked() const;
   bool       hasHotkey() const;
   FMenu*     getMenu() const;
   void       setMenu(FMenu*);
   bool       hasMenu() const;
   uInt       getTextLength() const;
   void       setText (FString&);
   void       setText (const std::string&);
   void       setText (const char*);

 private:
   friend class FMenuList;
   friend class FMenuBar;
   friend class FMenu;
};
#pragma pack(pop)


// FMenuItem inline functions
//----------------------------------------------------------------------
inline FWidget* FMenuItem::getSuperMenu() const
{ return super_menu; }

//----------------------------------------------------------------------
inline void FMenuItem::setSuperMenu (FWidget* smenu)
{ super_menu = smenu; }

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
inline bool FMenuItem::isSelected() const
{ return selected; }

//----------------------------------------------------------------------
inline void FMenuItem::setSeparator()
{ separator = true; }

//----------------------------------------------------------------------
inline void FMenuItem::unsetSeparator()
{ separator = false; }

//----------------------------------------------------------------------
inline bool FMenuItem::isSeparator() const
{ return separator; }

//----------------------------------------------------------------------
inline void FMenuItem::setChecked()
{ checked = true; }

//----------------------------------------------------------------------
inline void FMenuItem::unsetChecked()
{ checked = false; }

//----------------------------------------------------------------------
inline bool FMenuItem::isChecked() const
{ return checked; }

//----------------------------------------------------------------------
inline bool FMenuItem::hasHotkey() const
{ return bool(hotkey != 0); }

//----------------------------------------------------------------------
inline FMenu* FMenuItem::getMenu() const
{ return menu; }

//----------------------------------------------------------------------
inline void FMenuItem::setMenu(FMenu* m)
{ menu = m; }

//----------------------------------------------------------------------
inline bool FMenuItem::hasMenu() const
{ return bool(menu != 0); }

//----------------------------------------------------------------------
inline uInt FMenuItem::getTextLength() const
{ return text_length; }

#endif  // _FMENUITEM_H
