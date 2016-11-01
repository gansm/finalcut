// File: fmenulist.cpp
// Provides: class FMenuList

#include "fmenulist.h"

//----------------------------------------------------------------------
// class FMenuList
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FMenuList::FMenuList()
  : selected_item()
  , item_list()
{ }

//----------------------------------------------------------------------
FMenuList::~FMenuList()  // destructor
{
  // delete all items
  if ( item_list.empty() )
    return;

  std::vector<FMenuItem*>::iterator iter;
  iter = item_list.begin();

  while ( iter != item_list.end() )
  {
    (*iter)->setSuperMenu(0);
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
  std::vector<FMenuItem*>::iterator iter;

  if ( item_list.empty() )
    return;

  iter = item_list.begin();

  while ( iter != item_list.end() )
  {
    if ( (*iter) == i )
    {
      iter = item_list.erase(iter);
      i->setSuperMenu(0);
      break;
    }
    else
      ++iter;
  }
}

//----------------------------------------------------------------------
void FMenuList::remove (int pos)
{
  if ( int(getCount()) < pos )
    return;

  item_list.erase (item_list.begin() + pos - 1);
}

//----------------------------------------------------------------------
void FMenuList::clear()
{
  item_list.clear();
}

//----------------------------------------------------------------------
void FMenuList::selectFirstItem()
{
  std::vector<FMenuItem*>::const_iterator iter, end;
  iter = item_list.begin();
  end = item_list.end();

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

  setSelectedItem(0);
}
