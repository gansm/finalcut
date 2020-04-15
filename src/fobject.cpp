/***********************************************************************
* fobject.cpp - Object container base class of all widget objects      *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2015-2020 Markus Gans                                      *
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

#include <memory>

#include "final/emptyfstring.h"
#include "final/fevent.h"
#include "final/fc.h"
#include "final/fobject.h"

namespace finalcut
{

// static class attributes
bool FObject::timer_modify_lock;
FObject::FTimerList* FObject::timer_list{nullptr};
const FString* fc::emptyFString::empty_string{nullptr};


//----------------------------------------------------------------------
// class FObject
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FObject::FObject (FObject* parent)
  : parent_obj(parent)
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
        timer_list = new FTimerList;
      }
      catch (const std::bad_alloc& ex)
      {
        std::cerr << bad_alloc_str << ex.what() << std::endl;
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
    timer_list = nullptr;
  }

  if ( ! has_parent && ! fc::emptyFString::isNull() )
    fc::emptyFString::clear();

  // Delete children objects
  if ( hasChildren() )
  {
    auto delete_list = children_list;

    for (auto&& obj : delete_list)
      delete obj;
  }

  if ( parent_obj )
    parent_obj->delChild(this);

  parent_obj = nullptr;
}

// public methods of FObject
//----------------------------------------------------------------------
FObject* FObject::getChild (int index) const
{
  // returns the child for the index number

  if ( ! hasChildren() )
    return nullptr;

  if ( index <= 0 || index > numOfChildren() )
    return nullptr;

  auto iter = begin();
  std::advance (iter, index - 1);
  return *iter;
}

//----------------------------------------------------------------------
bool FObject::isChild (const FObject* obj) const
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
  if ( ! hasParent() )
    return;

  getParent()->delChild(this);
}

//----------------------------------------------------------------------
void FObject::addChild (FObject* obj)
{
  // Adds an object obj to the children list

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
  // Deletes the child object obj from children list

  if ( ! obj )
    return;

  if ( hasChildren() )
  {
    obj->parent_obj = nullptr;
    obj->has_parent = false;
    children_list.remove(obj);
  }
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
bool FObject::event (FEvent* ev)
{
  // Receives events on this object

  if ( ev->type() == fc::Timer_Event )
  {
    onTimer ( static_cast<FTimerEvent*>(ev) );
    return true;
  }

  if ( ev->type() == fc::User_Event )
  {
    onUserEvent ( static_cast<FUserEvent*>(ev) );
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
void FObject::getCurrentTime (timeval* time)
{
  // Get the current time as timeval struct

  gettimeofday(time, nullptr);

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
bool FObject::isTimeout (const timeval* time, uInt64 timeout)
{
  // Checks whether the specified time span (timeout in µs) has elapse

  struct timeval now{};
  struct timeval diff{};

  FObject::getCurrentTime(&now);
  diff.tv_sec = now.tv_sec - time->tv_sec;
  diff.tv_usec = now.tv_usec - time->tv_usec;

  if ( diff.tv_usec < 0 )
  {
    diff.tv_sec--;
    diff.tv_usec += 1000000;
  }

  const uInt64 diff_usec = uInt64((diff.tv_sec * 1000000) + diff.tv_usec);
  return ( diff_usec > timeout );
}

//----------------------------------------------------------------------
int FObject::addTimer (int interval)
{
  // Create a timer and returns the timer identifier number
  // (interval in ms)

  timeval time_interval{};
  timeval currentTime{};
  int id{1};
  timer_modify_lock = true;

  // find an unused timer id
  if ( ! timer_list->empty() )
  {
    auto iter = timer_list->begin();
    const auto& last = timer_list->end();

    while ( iter != last )
    {
      if ( iter->id == id )
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
  FTimerData t{ id, time_interval, timeout, this };

  // insert in list sorted by timeout
  auto iter = timer_list->begin();
  const auto& last = timer_list->end();

  while ( iter != last && iter->timeout < t.timeout )
    ++iter;

  timer_list->insert (iter, t);
  timer_modify_lock = false;
  return id;
}

//----------------------------------------------------------------------
bool FObject::delTimer (int id)
{
  // Deletes a timer by using the timer identifier number

  if ( id <= 0 )
    return false;

  timer_modify_lock = true;
  auto iter = timer_list->begin();
  const auto& last = timer_list->end();

  while ( iter != last && iter->id != id )
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
  // Deletes all timers of this object

  if ( ! timer_list )
    return false;

  if ( timer_list->empty() )
    return false;

  timer_modify_lock = true;
  auto iter = timer_list->begin();

  while ( iter != timer_list->end() )
  {
    if ( iter->object == this )
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
  // Deletes all timers of all objects

  if ( ! timer_list )
    return false;

  if ( timer_list->empty() )
    return false;

  timer_modify_lock = true;
  timer_list->clear();
  timer_list->shrink_to_fit();
  timer_modify_lock = false;
  return true;
}


// protected methods of FObject
//----------------------------------------------------------------------
void FObject::onTimer (FTimerEvent*)
{ }

//----------------------------------------------------------------------
void FObject::onUserEvent (FUserEvent*)
{ }

//----------------------------------------------------------------------
uInt FObject::processTimerEvent()
{
  timeval currentTime{};
  uInt activated{0};

  getCurrentTime (&currentTime);

  if ( isTimerInUpdating() )
    return 0;

  if ( ! timer_list )
    return 0;

  if ( timer_list->empty() )
    return 0;

  for (auto&& timer : *timer_list)
  {
    if ( ! timer.id
      || ! timer.object
      || currentTime < timer.timeout )  // no timer expired
      break;

    timer.timeout += timer.interval;

    if ( timer.timeout < currentTime )
      timer.timeout = currentTime + timer.interval;

    if ( timer.interval.tv_usec > 0 || timer.interval.tv_sec > 0 )
      activated++;

    FTimerEvent t_ev(fc::Timer_Event, timer.id);
    performTimerAction (timer.object, &t_ev);
  }

  return activated;
}

//----------------------------------------------------------------------
void FObject::performTimerAction (FObject*, FEvent*)
{ }

}  // namespace finalcut
