// File: fobject.cpp
// Provides: class FObject

#include "fobject.h"

// static class attributes
bool FObject::timer_modify_lock;
FObject::TimerList* FObject::timer_list = 0;


//----------------------------------------------------------------------
// class FObject
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FObject::FObject (FObject* parent)
  : parent_obj(parent)
  , children_list()
  , has_parent(false)
{
  children_list.clear();         // no children yet

  if ( parent_obj )              // add object to parent
    parent_obj->addChild(this);

  if ( parent == 0 )
  {
    timer_modify_lock = false;
    timer_list   = new TimerList();
  }
  else
    has_parent = true;
}

//----------------------------------------------------------------------
FObject::~FObject()  // destructor
{
  if ( parent_obj )
    parent_obj->delChild(this);

  parent_obj = 0;
  delOwnTimer();  // delete all timers of this object

  if ( ! has_parent && timer_list )
  {
    delete timer_list;
    timer_list = 0;
  }

  // delete children objects
  FObject::object_list children = this->getChildren();

  if ( ! children.empty() )
  {
    FObject::object_list::const_iterator iter;
    iter = children.begin();

    while ( iter != children.end() )
    {
      delete (*iter);
      ++iter;
    }
  }


}

// public methods of FObject
//----------------------------------------------------------------------
void FObject::addChild (FObject* obj)
{
  if ( ! obj )
    return;

  if ( obj->parent_obj )
    obj->parent_obj->delChild(obj);

  obj->parent_obj = this;

  children_list.push_back(obj);
}

//----------------------------------------------------------------------
void FObject::delChild (FObject* obj)
{
  if ( ! obj )
    return;

  if ( ! children_list.empty() )
  {
    obj->removeParent();
    children_list.remove(obj);
  }
}

//----------------------------------------------------------------------
bool FObject::event (FEvent* ev)
{
  if ( ev->type() == fc::Timer_Event )
  {
    onTimer ( static_cast<FTimerEvent*>(ev) );
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
void FObject::getCurrentTime (timeval &time)
{
  gettimeofday(&time, 0);

  // NTP fix
  while ( time.tv_usec >= 1000000 )
  {
    time.tv_usec -= 1000000;
    time.tv_sec++;
  }

  while ( time.tv_usec < 0 )
  {
    if ( time.tv_sec > 0 )
    {
      time.tv_usec += 1000000;
      time.tv_sec--;
    }
    else
    {
      time.tv_usec = 0;
      break;
    }
  }
}

//----------------------------------------------------------------------
int FObject::addTimer (int interval)
{
  FObject::TimerList::iterator iter, end;
  timeval time_interval;
  timeval currentTime;
  int id = 1;

  timer_modify_lock = true;

  if ( ! timer_list )
    timer_list = new TimerList();

  // find an unused timer id
  if ( ! timer_list->empty() )
  {
    iter = timer_list->begin();
    end  = timer_list->end();

    while ( iter != end )
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
  getCurrentTime (currentTime);
  timeval timeout = currentTime + time_interval;
  timer_data t = { id, time_interval, timeout, this };

  // insert in list sorted by timeout
  iter = timer_list->begin();
  end  = timer_list->end();

  while ( iter != end && (*iter).timeout < t.timeout )
    ++iter;

  timer_list->insert (iter, t);
  timer_modify_lock = false;

  return id;
}

//----------------------------------------------------------------------
bool FObject::delTimer (int id)
{
  FObject::TimerList::iterator iter, end;

  if ( id <= 0 || id > int(timer_list->size()) )
    return false;

  timer_modify_lock = true;
  iter = timer_list->begin();
  end  = timer_list->end();

  while ( iter != end && (*iter).id != id )
    ++iter;

  if ( iter != end )
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
void FObject::onTimer (FTimerEvent*)
{ }
