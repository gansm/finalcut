// fmenulist.cpp
// class FMenuList

#include "fmenulist.h"

//----------------------------------------------------------------------
// class FMenuList
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FMenuList::FMenuList()
{
}

//----------------------------------------------------------------------
FMenuList::~FMenuList()  // destructor
{
  // delete all items
  if ( ! itemlist.empty() )
  {
    std::vector<FMenuItem*>::iterator iter;
    iter = itemlist.begin();

    while ( iter != itemlist.end() )
    {
      (*iter)->setSuperMenu(0);
      //delAccelerator (*iter);
      iter = itemlist.erase(iter);
    }
  }
}


// private methods of FMenuList
//----------------------------------------------------------------------



// public methods of FMenuList
//----------------------------------------------------------------------
bool FMenuList::hasSelectedItem()
{
  if ( ! itemlist.empty() )
  {
    std::vector<FMenuItem*>::const_iterator iter, end;
    iter = itemlist.begin();
    end = itemlist.end();

    while ( iter != end )
    {
      if ( (*iter)->isSelected() )
        return true;
      ++iter;
    }
  }
  return false;
}

//----------------------------------------------------------------------
void FMenuList::insert (FMenuItem* obj)
{
  itemlist.push_back(obj);

  //addAccelerator (obj->getKey(), obj);
/*
  obj->addCallback
  (
    "activate",
    this,
    reinterpret_cast<FWidget::FMemberCallback>(&FMenuBar::cb_statuskey_activated),
    null
  );
*/
}

//----------------------------------------------------------------------
void FMenuList::remove (FMenuItem* obj)
{
  std::vector<FMenuItem*>::iterator iter;

  //delAccelerator (obj);

  if ( itemlist.empty() )
    return;

  iter = itemlist.begin();
  while ( iter != itemlist.end() )
  {
    if ( (*iter) == obj )
    {
      iter = itemlist.erase(iter);
      obj->setSuperMenu(0);
      break;
    }
    else
      ++iter;
  }
}

//----------------------------------------------------------------------
void FMenuList::remove (int pos)
{
  if ( int(count()) < pos )
    return;

  itemlist.erase (itemlist.begin()+pos-1);
}

//----------------------------------------------------------------------
void FMenuList::clear()
{
  itemlist.clear();
}
