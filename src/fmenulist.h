// File: fmenulist.h
// Provides: class FMenuList
//
//  Base class
//  ══════════
//
// ▕▔▔▔▔▔▔▔▔▔▔▔▏1     *▕▔▔▔▔▔▔▔▔▔▔▔▏
// ▕ FMenuList ▏- - - -▕ FMenuItem ▏
// ▕▁▁▁▁▁▁▁▁▁▁▁▏       ▕▁▁▁▁▁▁▁▁▁▁▁▏


#ifndef _FMENULIST_H
#define _FMENULIST_H

#include "fmenuitem.h"
#include "fwidget.h"

/*
- FMenuBar muß die x + y Positionen des nächsten
  einzufügenden FMenuItem kennen

- FMenu muß die x + y Positionen des nächsten
  einzufügenden FMenuItem kennen

    ┌─────────┐           ┌──────────┐
    │ FWindow │◄───┐  ┌───┤ FMenuBar │
    └─────────┘    │  │   └──────────┘
                   ├──┤
  ┌───────────┐    │  │   ┌───────┐
  │ FMenuList │◄───┘  └───┤ FMenu │
  └─────┬─────┘           └───────┘ 
        :                 ┌───────────┐
        └-----------------┤ FMenuItem │
                          └───────────┘
*/

//----------------------------------------------------------------------
// class FMenuList
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FMenuList
{
 protected:
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
   void         activateItem (int);
   void         deactivateItem (int);
   bool         isSelected (int) const;
   bool         hasSelectedItem();

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
inline FMenuItem* FMenuList::item(int index) const
{ return itemlist[uInt(index-1)]; }

//----------------------------------------------------------------------
inline void FMenuList::activateItem (int index)
{ itemlist[uInt(index-1)]->setActive(); }

//----------------------------------------------------------------------
inline void FMenuList::deactivateItem (int index)
{ itemlist[uInt(index-1)]->unsetActive(); }

//----------------------------------------------------------------------
inline bool FMenuList::isSelected(int index) const
{ return itemlist[uInt(index-1)]->isSelected(); }


#endif  // _FMENULIST_H
