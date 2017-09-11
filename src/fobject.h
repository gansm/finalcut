// File: fobject.h
// Provides: class FObject
//
//  Base class
//  ══════════
//
// ▕▔▔▔▔▔▔▔▔▔▏
// ▕ FObject ▏
// ▕▁▁▁▁▁▁▁▁▁▏

#ifndef FOBJECT_H
#define FOBJECT_H

#include <stdint.h>
#include <sys/time.h>  // need for gettimeofday
#include <cstdlib>
#include <cstring>
#include <list>
#include <vector>

#include "fc.h"
#include "fevent.h"
#include "ftypes.h"


//----------------------------------------------------------------------
// class FObject
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FObject
{
  public:
    // Typedef
    typedef std::list<FObject*> FObjectList;

    // Constructor
    explicit FObject (FObject* = 0);

    // Destructor
    virtual ~FObject();

    // Accessors
    virtual const char* getClassName() const;
    FObject*            getParent() const;
    FObject*            getChild (int) const;
    const FObjectList&  getChildren() const;
    int                 numOfChildren() const;

    // Inquiries
    bool                hasParent() const;
    bool                hasChildren() const;
    bool                isChild (FObject*) const;
    bool                isDirectChild (FObject*) const;
    bool                isWidget() const;
    bool                isInstanceOf (const char*) const;
    bool                isTimerInUpdating() const;

    // Methods
    void                removeParent();
    void                addChild (FObject*);
    void                delChild (FObject*);

    // Timer methods
    static void         getCurrentTime (timeval*);
    int                 addTimer (int);
    bool                delTimer (int);
    bool                delOwnTimer();
    bool                delAllTimer();

  protected:
    struct timer_data
    {
      int       id;
      timeval   interval;
      timeval   timeout;
      FObject*  object;
    };

    // Typedef
    typedef std::vector<timer_data> TimerList;

    // Event handler
    virtual bool event (FEvent*);
    virtual void onTimer (FTimerEvent*);

    // Data Members
    static TimerList* timer_list;
    bool              widget_object;

  private:
    // Disable copy constructor
    FObject (const FObject&);

    // Disable assignment operator (=)
    FObject& operator = (const FObject&);

    // Data Members
    FObject*     parent_obj;
    FObjectList  children_list;
    bool         has_parent;
    static bool  timer_modify_lock;
};

#pragma pack(pop)

//----------------------------------------------------------------------
inline const char* FObject::getClassName() const
{ return "FObject"; }

//----------------------------------------------------------------------
inline FObject* FObject::getParent() const
{ return parent_obj; }

//----------------------------------------------------------------------
inline const FObject::FObjectList& FObject::getChildren() const
{ return children_list; }

//----------------------------------------------------------------------
inline int FObject::numOfChildren() const
{ return int(children_list.size()); }

//----------------------------------------------------------------------
inline bool FObject::hasParent() const
{ return has_parent; }

//----------------------------------------------------------------------
inline bool FObject::hasChildren() const
{ return bool( ! children_list.empty() ); }

//----------------------------------------------------------------------
inline bool FObject::isDirectChild (FObject* obj) const
{ return bool( obj->getParent() == this ); }

//----------------------------------------------------------------------
inline bool FObject::isWidget() const
{ return widget_object; }

//----------------------------------------------------------------------
inline bool FObject::isInstanceOf (const char* classname) const
{ return ( classname ) ? bool(strcmp(classname, getClassName()) == 0) : false; }

//----------------------------------------------------------------------
inline bool FObject::isTimerInUpdating() const
{ return timer_modify_lock; }

//----------------------------------------------------------------------
inline void FObject::removeParent()
{ parent_obj = 0; }


//----------------------------------------------------------------------
// Operator functions for timeval
//----------------------------------------------------------------------

static inline timeval operator + (const timeval& t1, const timeval& t2)
{
  timeval tmp;
  tmp.tv_sec = t1.tv_sec + t2.tv_sec;

  if ( (tmp.tv_usec = t1.tv_usec + t2.tv_usec) >= 1000000 )
  {
    tmp.tv_sec++;
    tmp.tv_usec -= 1000000;
  }

  return tmp;
}

//----------------------------------------------------------------------
static inline timeval operator - (const timeval& t1, const timeval& t2)
{
  timeval tmp;
  tmp.tv_sec = t1.tv_sec - t2.tv_sec;

  if ( (tmp.tv_usec = t1.tv_usec - t2.tv_usec) < 0 )
  {
    tmp.tv_sec--;
    tmp.tv_usec += 1000000;
  }

  return tmp;
}

//----------------------------------------------------------------------
static inline timeval& operator += (timeval& t1, const timeval& t2)
{
  t1.tv_sec += t2.tv_sec;

  if ( (t1.tv_usec += t2.tv_usec) >= 1000000 )
  {
    t1.tv_sec++;
    t1.tv_usec -= 1000000;
  }

  return t1;
}

//----------------------------------------------------------------------
static inline bool operator < (const timeval& t1, const timeval& t2)
{
  return (t1.tv_sec < t2.tv_sec)
      || (t1.tv_sec == t2.tv_sec && t1.tv_usec < t2.tv_usec);
}

#endif  // FOBJECT_H
