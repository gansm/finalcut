/***********************************************************************
* fmenulist.h - Menu item container base class                         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2021 Markus Gans                                      *
*                                                                      *
* FINAL CUT is free software; you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as       *
* published by the Free Software Foundation; either version 3 of       *
* the License, or (at your option) any later version.                  *
*                                                                      *
* FINAL CUT is distributed in the hope that it will be useful, but     *
* WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU Lesser General Public License for more details.                  *
*                                                                      *
* You should have received a copy of the GNU Lesser General Public     *
* License along with this program.  If not, see                        *
* <http://www.gnu.org/licenses/>.                                      *
***********************************************************************/

/*  Base class
 *  ══════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▔▔▏1     *▕▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FMenuList ▏- - - -▕ FMenuItem ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▏       ▕▁▁▁▁▁▁▁▁▁▁▁▏
 *       :1
 *       :            *▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *       ├- - - - - - -▕ FRadioMenuItem ▏
 *       :             ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *       :
 *       :            *▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *       └- - - - - - -▕ FCheckMenuItem ▏
 *                     ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FMENULIST_H
#define FMENULIST_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <vector>

#include "final/fwidget.h"
#include "final/menu/fmenuitem.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FMenuList
//----------------------------------------------------------------------

class FMenuList
{
  public:
    // Using-declarations
    using FMenuItemList = std::vector<FMenuItem*>;

    // Constructor
    FMenuList() = default;

    // Disable copy constructor
    FMenuList (const FMenuList&) = delete;

    // Destructor
    virtual ~FMenuList();

    // Disable copy assignment operator (=)
    FMenuList& operator = (const FMenuList&) = delete;

    // Accessors
    virtual FString          getClassName() const;
    std::size_t              getCount() const;
    FMenuItem*               getItem (int) const;
    FMenuItem*               getSelectedItem() const;
    const FMenuItemList&     getItemList() const;

    // Mutators
    void                     enableItem (int);
    void                     disableItem (int);
    void                     setSelectedItem (FMenuItem*);
    void                     unsetSelectedItem();

    // Inquiries
    bool                     isSelected (int) const;
    bool                     hasSelectedItem() const;

    // Methods
    virtual void             insert (FMenuItem*);
    virtual void             remove (FMenuItem*);
    void                     remove (int);
    void                     clear();
    auto                     findFirstSelectedItem() const
                                 -> FMenuItemList::const_iterator;
    auto                     findLastSelectedItem () const
                                 -> FMenuItemList::const_reverse_iterator;
    void                     selectFirstItem();
    void                     unselectItem();

  protected:
    bool                     selectNextItem();
    bool                     selectPrevItem();
    virtual void             selectItem_PostProcessing (FMenuItem*) = 0;

  private:
    // Data members
    FMenuItem*               selected_item{};
    FMenuItemList            item_list{};
};


// FMenuList inline functions
//----------------------------------------------------------------------
inline FString FMenuList::getClassName() const
{ return "FMenuList"; }

//----------------------------------------------------------------------
inline std::size_t FMenuList::getCount() const
{ return item_list.size(); }

//----------------------------------------------------------------------
inline FMenuItem* FMenuList::getItem (int index) const
{ return ( index > 0 ) ? item_list[uInt(index - 1)] : nullptr; }

//----------------------------------------------------------------------
inline FMenuItem* FMenuList::getSelectedItem() const
{ return selected_item; }

//----------------------------------------------------------------------
inline const FMenuList::FMenuItemList& FMenuList::getItemList() const
{ return item_list; }

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
inline void FMenuList::unsetSelectedItem()
{ selected_item = nullptr; }

//----------------------------------------------------------------------
inline bool FMenuList::isSelected(int index) const
{ return ( index > 0 ) ? item_list[uInt(index - 1)]->isSelected() : false; }

//----------------------------------------------------------------------
inline bool FMenuList::hasSelectedItem() const
{ return selected_item; }

}  // namespace finalcut

#endif  // FMENULIST_H
