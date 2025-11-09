/***********************************************************************
* fobject.cpp - Object container base class of all widget objects      *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2025 Markus Gans                                      *
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

#include <algorithm>
#include <memory>

#include "final/fc.h"
#include "final/fevent.h"
#include "final/fobject.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FObject
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FObject::FObject (FObject* parent)
  : parent_obj{parent}
{
  if ( parent )  // Add object to parent
  {
    parent->addChild(this);
    has_parent = true;
  }
}

//----------------------------------------------------------------------
FObject::~FObject()  // destructor
{
  delOwnTimers();  // Delete all timers of this object

  // Delete children objects
  if ( ! children_list.empty() )
  {
    // Make a copy to avoid iterator invalidation during deletion
    auto delete_list = children_list;

    for (auto obj : delete_list)
      delete obj;
  }

  if ( parent_obj )
    parent_obj->delChild(this);

  parent_obj = nullptr;
}

// public methods of FObject
//----------------------------------------------------------------------
auto FObject::getChild (int index) const -> FObject*
{
  // returns the child for the index number

  if ( children_list.empty() )
    return nullptr;

  if ( index <= 0 || index > int(children_list.size()) )
    return nullptr;

  auto iter = children_list.cbegin();
  std::advance (iter, index - 1);
  return *iter;
}

//----------------------------------------------------------------------
auto FObject::isChild (const FObject* obj) const -> bool
{
  // Find out if obj is a child object of mine

  while ( obj )
  {
    obj = obj->getParent();

    if ( obj == this )
      return true;
  }

  return false;
}

//----------------------------------------------------------------------
void FObject::removeParent()
{
  if ( ! has_parent )
    return;

  getParent()->delChild(this);
}

//----------------------------------------------------------------------
void FObject::addChild (FObject* obj)
{
  // Adds an object obj to the children list

  if ( ! obj )
    return;

  if ( max_children != UNLIMITED && children_list.size() >= max_children )
    throw std::length_error ("max. child objects reached");

  if ( obj->parent_obj )
    obj->parent_obj->delChild(obj);

  obj->parent_obj = this;
  obj->has_parent = true;
  children_list.push_back(obj);
}

//----------------------------------------------------------------------
void FObject::delChild (FObject* obj)
{
  // Deletes the child object obj from children list

  if ( ! obj || children_list.empty() )
    return;

  obj->parent_obj = nullptr;
  obj->has_parent = false;

  const auto list_end = children_list.end();
  const auto last = std::remove (children_list.begin(), list_end, obj);
  children_list.erase(last, list_end);
}

//----------------------------------------------------------------------
void FObject::setParent (FObject* parent)
{
  // Sets a new parent object

  if ( ! parent )
    return;

  removeParent();
  parent_obj = parent;
  has_parent = true;
  parent->children_list.push_back(this);
}

//----------------------------------------------------------------------
auto FObject::event (FEvent* ev) -> bool
{
  // Receives events on this object

  const auto event_type = ev->getType();

  if ( event_type == Event::Timer )
  {
    onTimer ( static_cast<FTimerEvent*>(ev) );
    return true;
  }

  if ( event_type == Event::User )
  {
    onUserEvent ( static_cast<FUserEvent*>(ev) );
    return true;
  }

  return false;
}


// protected methods of FObject
//----------------------------------------------------------------------
void FObject::onTimer (FTimerEvent*)
{
  // This event handler can be reimplemented in a subclass
  // to receive timer events for this object
}

//----------------------------------------------------------------------
void FObject::onUserEvent (FUserEvent*)
{
  // This event handler can be reimplemented in a subclass
  // to receive user events for this object
}


}  // namespace finalcut
