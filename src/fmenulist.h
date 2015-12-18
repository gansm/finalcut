// File: fmenulist.h
// Provides: class FMenuList
//
//  Base class
//  ══════════
//
// ▕▔▔▔▔▔▔▔▔▔▔▔▏1     *▕▔▔▔▔▔▔▔▔▔▔▔▏
// ▕ FMenuList ▏- - - -▕ FMenuItem ▏
// ▕▁▁▁▁▁▁▁▁▁▁▁▏       ▕▁▁▁▁▁▁▁▁▁▁▁▏
//       :1
//       :            *▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
//       ├- - - - - - -▕ FRadioMenuItem ▏
//       :             ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
//       :
//       :            *▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
//       └- - - - - - -▕ FCheckMenuItem ▏
//                     ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏

#ifndef _FMENULIST_H
#define _FMENULIST_H

#include "fmenuitem.h"
#include "fwidget.h"


//----------------------------------------------------------------------
// class FMenuList
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FMenuList
{
 protected:
   FMenuItem* selectedItem;
   std::vector<FMenuItem*> itemlist;

 private:
   FMenuList (const FMenuList&);
   FMenuList& operator = (const FMenuList&);

 public:
   explicit FMenuList();
   virtual ~FMenuList();
   virtual const char* getClassName() const;

   uInt         count() const;
   FMenuItem*   item (int) const;
   void         enableItem (int);
   void         disableItem (int);
   bool         isSelected (int) const;
   void         selectFirstItem();
   void         unselectItem();
   FMenuItem*   getSelectedItem() const;
   void         setSelectedItem (FMenuItem*);
   bool         hasSelectedItem() const;

   virtual void insert (FMenuItem*);
   virtual void remove (FMenuItem*);
   void         remove (int);
   void         clear();
};
#pragma pack(pop)


// FMenuList inline functions
//----------------------------------------------------------------------
inline const char* FMenuList::getClassName() const
{ return "FMenuList"; }

//----------------------------------------------------------------------
inline uInt FMenuList::count() const
{ return uInt(itemlist.size()); }

//----------------------------------------------------------------------
inline FMenuItem* FMenuList::item (int index) const
{ return (index > 0) ? itemlist[uInt(index-1)] : 0; }

//----------------------------------------------------------------------
inline void FMenuList::enableItem (int index)
{ itemlist[uInt(index-1)]->setEnable(); }

//----------------------------------------------------------------------
inline void FMenuList::disableItem (int index)
{ itemlist[uInt(index-1)]->unsetEnable(); }

//----------------------------------------------------------------------
inline bool FMenuList::isSelected(int index) const
{ return (index > 0) ? itemlist[uInt(index-1)]->isSelected() : false; }

//----------------------------------------------------------------------
inline FMenuItem* FMenuList::getSelectedItem() const
{ return selectedItem; }

//----------------------------------------------------------------------
inline void FMenuList::setSelectedItem (FMenuItem* menuitem)
{ selectedItem = menuitem; }

//----------------------------------------------------------------------
inline bool FMenuList::hasSelectedItem() const
{ return selectedItem; }

#endif  // _FMENULIST_H
