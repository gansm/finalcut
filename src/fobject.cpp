/***********************************************************************
* fobject.cpp - Object container base class of all widget objects      *
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

#include "final/fobject.h"

// static class attributes
bool                FObject::timer_modify_lock;
FObject::TimerList* FObject::timer_list = 0;
const FString* fc::emptyFString::empty_string = 0;

//----------------------------------------------------------------------
// class FObject
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FObject::FObject (FObject* parent)
  : widget_object(false)
  , parent_obj(parent)
  , children_list()            // no children yet
  , has_parent(false)
{
  if ( parent )                // add object to parent
  {
    parent->addChild(this);
    has_parent = true;
  }
  else
  {
    timer_modify_lock = false;

    if ( ! timer_list )
    {
      try
      {
        timer_list = new TimerList();
      }
      catch (const std::bad_alloc& ex)
      {
        std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
        return;
      }
    }
  }
}

//----------------------------------------------------------------------
FObject::~FObject()  // destructor
{
  delOwnTimer();  // Delete all timers of this object

  if ( ! has_parent && timer_list )
  {
    delete timer_list;
    timer_list = 0;
  }

  if ( ! has_parent && ! fc::emptyFString::isNull() )
    fc::emptyFString::clear();

  // Delete children objects
  if ( hasChildren() )
  {
    constFObjectIterator iter, last;
    FObjectList delete_list = children_list;
    iter = delete_list.begin();
    last = delete_list.end();

    while ( iter != last )
    {
      delete (*iter);
      ++iter;
    }
  }

  if ( parent_obj )
    parent_obj->delChild(this);

  parent_obj = 0;
}

// public methods of FObject
//----------------------------------------------------------------------
FObject* FObject::getChild (int index) const
{
  if ( ! hasChildren() )
    return 0;

  if ( index <= 0 || index > numOfChildren() )
    return 0;

  constFObjectIterator iter;
  iter = begin();
  std::advance (iter, index - 1);
  return *iter;
}

//----------------------------------------------------------------------
bool FObject::isChild (FObject* obj) const
{
  FObject* p_obj = 0;

  while ( obj && (p_obj = obj->getParent()) )
  {
    obj = p_obj;

    if ( obj == this )
      return true;
  }

  return false;
}

//----------------------------------------------------------------------
void FObject::addChild (FObject* obj)
{
  if ( ! obj )
    return;

  if ( obj->parent_obj )
    obj->parent_obj->delChild(obj);

  obj->parent_obj = this;
  obj->has_parent = true;
  children_list.push_back(obj);
}

//----------------------------------------------------------------------
void FObject::delChild (FObject* obj)
{
  if ( ! obj )
    return;

  if ( hasChildren() )
  {
    obj->removeParent();
    children_list.remove(obj);
  }
}

//----------------------------------------------------------------------
void FObject::getCurrentTime (timeval* time)
{
  gettimeofday(time, 0);

  // NTP fix
  while ( time->tv_usec >= 1000000 )
  {
    time->tv_usec -= 1000000;
    time->tv_sec++;
  }

  while ( time->tv_usec < 0 )
  {
    if ( time->tv_sec > 0 )
    {
      time->tv_usec += 1000000;
      time->tv_sec--;
    }
    else
    {
      time->tv_usec = 0;
      break;
    }
  }
}

//----------------------------------------------------------------------
bool FObject::isTimeout (timeval* time, register long timeout)
{
  register long diff_usec;
  struct timeval now;
  struct timeval diff;

  FObject::getCurrentTime(&now);
  diff.tv_sec = now.tv_sec - time->tv_sec;
  diff.tv_usec = now.tv_usec - time->tv_usec;

  if ( diff.tv_usec < 0 )
  {
    diff.tv_sec--;
    diff.tv_usec += 1000000;
  }

  diff_usec = (diff.tv_sec * 1000000) + diff.tv_usec;
  return ( diff_usec > timeout );
}

//----------------------------------------------------------------------
int FObject::addTimer (int interval)
{
  FObject::TimerList::iterator iter, last;
  timeval time_interval;
  timeval currentTime;
  int id = 1;

  timer_modify_lock = true;

  if ( ! timer_list )
  {
    try
    {
      timer_list = new TimerList();
    }
    catch (const std::bad_alloc& ex)
    {
      std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
      return -1;
    }
  }

  // find an unused timer id
  if ( ! timer_list->empty() )
  {
    iter = timer_list->begin();
    last = timer_list->end();

    while ( iter != last )
    {
      if ( (*iter).id == id )
      {
        iter = timer_list->begin();
        id++;
        continue;
      }
      ++iter;
    }
  }

  if ( id <= 0 || id > int(timer_list->size() + 1) )
    return 0;


  time_interval.tv_sec  =  interval / 1000;
  time_interval.tv_usec = (interval % 1000) * 1000;
  getCurrentTime (&currentTime);
  timeval timeout = currentTime + time_interval;
  timer_data t = { id, time_interval, timeout, this };

  // insert in list sorted by timeout
  iter = timer_list->begin();
  last = timer_list->end();

  while ( iter != last && (*iter).timeout < t.timeout )
    ++iter;

  timer_list->insert (iter, t);
  timer_modify_lock = false;

  return id;
}

//----------------------------------------------------------------------
bool FObject::delTimer (int id)
{
  FObject::TimerList::iterator iter, last;

  if ( id <= 0 || id > int(timer_list->size()) )
    return false;

  timer_modify_lock = true;
  iter = timer_list->begin();
  last = timer_list->end();

  while ( iter != last && (*iter).id != id )
    ++iter;

  if ( iter != last )
  {
    timer_list->erase(iter);
    timer_modify_lock = false;
    return true;
  }

  timer_modify_lock = false;
  return false;
}

//----------------------------------------------------------------------
bool FObject::delOwnTimer()
{
  FObject::TimerList::iterator iter;

  if ( ! timer_list )
    return false;

  if ( timer_list->empty() )
    return false;

  timer_modify_lock = true;
  iter = timer_list->begin();

  while ( iter != timer_list->end() )
  {
    if ( (*iter).object == this )
      iter = timer_list->erase(iter);
    else
      ++iter;
  }

  timer_modify_lock = false;
  return true;
}

//----------------------------------------------------------------------
bool FObject::delAllTimer()
{
  if ( ! timer_list )
    return false;

  if ( timer_list->empty() )
    return false;

  timer_modify_lock = true;
  timer_list->clear();
  timer_modify_lock = false;
  return true;
}


// protected methods of FObject
//----------------------------------------------------------------------
bool FObject::event (FEvent* ev)
{
  if ( ev->type() == fc::Timer_Event )
  {
    onTimer ( const_cast<FTimerEvent*>(static_cast<const FTimerEvent*>(ev)) );
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
void FObject::onTimer (FTimerEvent*)
{ }
