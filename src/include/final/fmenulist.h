/***********************************************************************
* fmenulist.h - Menu item container base class                         *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2015-2018 Markus Gans                                      *
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

#include "final/fmenuitem.h"
#include "final/fwidget.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FMenuList
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FMenuList
{
  public:
    // Constructor
    FMenuList();

    // Destructor
    virtual ~FMenuList();

    // Accessors
    virtual const char* getClassName() const;
    std::size_t         getCount() const;
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
inline std::size_t FMenuList::getCount() const
{ return item_list.size(); }

//----------------------------------------------------------------------
inline FMenuItem* FMenuList::getItem (int index) const
{ return ( index > 0 ) ? item_list[uInt(index - 1)] : 0; }

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
{ return ( index > 0 ) ? item_list[uInt(index - 1)]->isSelected() : false; }

//----------------------------------------------------------------------
inline bool FMenuList::hasSelectedItem() const
{ return selected_item; }

}  // namespace finalcut

#endif  // FMENULIST_H
