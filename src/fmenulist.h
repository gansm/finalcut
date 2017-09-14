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

#ifndef FMENULIST_H
#define FMENULIST_H

#include <vector>

#include "fmenuitem.h"
#include "fwidget.h"


//----------------------------------------------------------------------
// class FMenuList
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FMenuList
{
  public:
    // Constructor
    explicit FMenuList();

    // Destructor
    virtual ~FMenuList();

    // Accessors
    virtual const char* getClassName() const;
    uInt                getCount() const;
    FMenuItem*          getItem (int) const;
    FMenuItem*          getSelectedItem() const;

    // Mutators
    void                enableItem (int);
    void                disableItem (int);
    void                setSelectedItem (FMenuItem*);

    // Inquiries
    bool                isSelected (int) const;
    bool                hasSelectedItem() const;

    // Methods
    virtual void        insert (FMenuItem*);
    virtual void        remove (FMenuItem*);
    void                remove (int);
    void                clear();
    void                selectFirstItem();
    void                unselectItem();

  protected:
    FMenuItem*              selected_item;
    std::vector<FMenuItem*> item_list;

  private:
    // Disable copy constructor
    FMenuList (const FMenuList&);

    // Disable assignment operator (=)
    FMenuList& operator = (const FMenuList&);
};
#pragma pack(pop)


// FMenuList inline functions
//----------------------------------------------------------------------
inline const char* FMenuList::getClassName() const
{ return "FMenuList"; }

//----------------------------------------------------------------------
inline uInt FMenuList::getCount() const
{ return uInt(item_list.size()); }

//----------------------------------------------------------------------
inline FMenuItem* FMenuList::getItem (int index) const
{ return (index > 0) ? item_list[uInt(index - 1)] : 0; }

//----------------------------------------------------------------------
inline FMenuItem* FMenuList::getSelectedItem() const
{ return selected_item; }

//----------------------------------------------------------------------
inline void FMenuList::enableItem (int index)
{ item_list[uInt(index - 1)]->setEnable(); }

//----------------------------------------------------------------------
inline void FMenuList::disableItem (int index)
{ item_list[uInt(index - 1)]->unsetEnable(); }

//----------------------------------------------------------------------
inline void FMenuList::setSelectedItem (FMenuItem* menuitem)
{ selected_item = menuitem; }

//----------------------------------------------------------------------
inline bool FMenuList::isSelected(int index) const
{ return (index > 0) ? item_list[uInt(index - 1)]->isSelected() : false; }

//----------------------------------------------------------------------
inline bool FMenuList::hasSelectedItem() const
{ return selected_item; }

#endif  // FMENULIST_H
