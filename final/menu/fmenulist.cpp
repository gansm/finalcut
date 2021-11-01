/***********************************************************************
* fmenulist.cpp - Menu item container base class                       *
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

#include <vector>

#include "final/fc.h"
#include "final/menu/fmenuitem.h"
#include "final/menu/fmenulist.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FMenuList
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FMenuList::~FMenuList()  // destructor
{
  // delete all items
  if ( item_list.empty() )
    return;

  auto iter = item_list.cbegin();

  while ( iter != item_list.cend() )
  {
    (*iter)->setSuperMenu(nullptr);
    iter = item_list.erase(iter);
  }
}


// public methods of FMenuList
//----------------------------------------------------------------------
void FMenuList::insert (FMenuItem* i)
{
  item_list.push_back(i);
}

//----------------------------------------------------------------------
void FMenuList::remove (FMenuItem* i)
{
  if ( item_list.empty() )
    return;

  auto iter = item_list.cbegin();

  while ( iter != item_list.cend() )
  {
    if ( (*iter) == i )
    {
      iter = item_list.erase(iter);
      i->setSuperMenu(nullptr);
      break;
    }

    ++iter;
  }
}

//----------------------------------------------------------------------
void FMenuList::remove (int pos)
{
  if ( int(getCount()) < pos )
    return;

  item_list.erase (item_list.cbegin() + pos - 1);
}

//----------------------------------------------------------------------
void FMenuList::clear()
{
  item_list.clear();
  item_list.shrink_to_fit();
}

//----------------------------------------------------------------------
auto FMenuList::findFirstSelectedItem() const
    -> FMenuList::FMenuItemList::const_iterator
{
  return std::find_if
  (
    item_list.cbegin(),
    item_list.cend(),
    [] (const auto& item)
    {
      return item->isSelected();
    }
  );
}

//----------------------------------------------------------------------
auto FMenuList::findLastSelectedItem() const
    -> FMenuList::FMenuItemList::const_reverse_iterator
{
  return std::find_if
  (
    item_list.crbegin(),
    item_list.crend(),
    [] (const auto& item)
    {
      return item->isSelected();
    }
  );
}

//----------------------------------------------------------------------
void FMenuList::selectFirstItem()
{
  auto iter = item_list.cbegin();
  auto end = item_list.cend();

  if ( item_list.empty() )
    return;

  if ( hasSelectedItem() )
    unselectItem();

  while ( iter != end )
  {
    if ( (*iter)->isEnabled() && ! (*iter)->isSeparator() )
    {
      // select first enabled item
      (*iter)->setSelected();
      setSelectedItem(*iter);
      break;
    }

    ++iter;
  }
}

//----------------------------------------------------------------------
void FMenuList::unselectItem()
{
  if ( hasSelectedItem() )
    getSelectedItem()->unsetSelected();

  setSelectedItem(nullptr);
}

//----------------------------------------------------------------------
bool FMenuList::selectNextItem()
{
  const auto& list = getItemList();
  auto iter = findFirstSelectedItem();

  if ( iter == list.cend() )  // not found
    return false;

  FMenuItem* next{};
  auto next_element = iter;

  do
  {
    ++next_element;

    if ( next_element == list.cend() )
      next_element = list.cbegin();

    next = *next_element;
  }
  while ( ! next->isEnabled()
       || ! next->acceptFocus()
       || ! next->isShown()
       || next->isSeparator() );

  if ( next == *iter )
    return false;

  // The code for post-processing is implemented in the inherited class
  selectItem_PostProcessing(next);
  return true;
}

//----------------------------------------------------------------------
bool FMenuList::selectPrevItem()
{
  const auto& list = getItemList();
  auto iter = findLastSelectedItem();

  if ( iter == list.crend() )  // not found
    return false;

  FMenuItem* prev;
  auto prev_element = iter;

  do
  {
    prev_element++;

    if ( prev_element == list.crend() )
      prev_element = list.crbegin();

    prev = *prev_element;
  }
  while ( ! prev->isEnabled()
       || ! prev->acceptFocus()
       || ! prev->isShown()
       || prev->isSeparator() );

  if ( prev == *iter )
    return false;

  // The code for post-processing is implemented in the inherited class
  selectItem_PostProcessing(prev);
  return true;
}

}  // namespace finalcut
