// fmenuitem.h
// class FMenuItem

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
   bool       active;
   bool       selected;
   bool       separator;
   bool       checked;
   int        hotkey;
 //int        accel_key;
   FMenu*     menu;
   FMenuList* super_menu;

 private:
   FMenuItem (const FMenuItem&);
   FMenuItem& operator = (const FMenuItem&);
   void       init (FWidget*);
   uChar      getHotkey();
   bool       isMenuBar (FWidget*) const;
   bool       isMenu (FWidget*) const;
   FMenuList* superMenu() const;
   void       setSuperMenu (FMenuList*);
   void       processActivate();
   void       processClicked();

 public:
   explicit FMenuItem (FWidget* parent=0);
   FMenuItem (FString&, FWidget* parent=0);
   FMenuItem (const std::string&, FWidget* parent=0);
   FMenuItem (const char*, FWidget* parent=0);
   virtual ~FMenuItem();

   void       onAccel (FAccelEvent*);
   FString    getText() const;
   void       setActive();
   void       unsetActive();
   bool       isActivated() const;
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
   void       setMenu(FMenu*);
   bool       hasMenu() const;
   void       setText (FString&);
   void       setText (const std::string&);
   void       setText (const char*);

 private:
   friend class FMenuList;
   friend class FMenuBar;
};
#pragma pack(pop)


// FMenuItem inline functions
//----------------------------------------------------------------------
inline FString FMenuItem::getText() const
{ return text; }

//----------------------------------------------------------------------
inline void FMenuItem::setActive()
{ active = true; }

//----------------------------------------------------------------------
inline void FMenuItem::unsetActive()
{ active = false; }

//----------------------------------------------------------------------
inline bool FMenuItem::isActivated() const
{ return active; }

//----------------------------------------------------------------------
inline void FMenuItem::unsetSelected()
{ selected = false; }

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
inline void FMenuItem::setMenu(FMenu* m)
{ menu = m; }

//----------------------------------------------------------------------
inline bool FMenuItem::hasMenu() const
{ return bool(menu != 0); }


#endif  // _FMENUITEM_H
