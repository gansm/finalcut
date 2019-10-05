/***********************************************************************
* fobject.h - Object container base class of all widget objects        *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2015-2019 Markus Gans                                      *
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
 * ▕▔▔▔▔▔▔▔▔▔▏
 * ▕ FObject ▏
 * ▕▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FOBJECT_H
#define FOBJECT_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#if !defined (__cplusplus)
  #error "You need a C++ compiler like g++ or clang++"
#elif __cplusplus > 1 && __cplusplus < 201103
  #error "Your C++ compiler does not support the C++11 standard!"
#endif

#include <sys/time.h>  // need for gettimeofday
#include <cstdlib>
#include <cstring>
#include <list>
#include <memory>
#include <vector>

namespace finalcut
{

// class forward declaration
class FEvent;
class FKeyEvent;
class FMouseEvent;
class FWheelEvent;
class FFocusEvent;
class FAccelEvent;
class FShowEvent;
class FHideEvent;
class FCloseEvent;
class FTimerEvent;
class FUserEvent;

//----------------------------------------------------------------------
// class FObject
//----------------------------------------------------------------------

class FObject
{
  public:
    // Typedef
    typedef std::list<FObject*>         FObjectList;
    typedef FObjectList::iterator       iterator;
    typedef FObjectList::const_iterator const_iterator;

    // Constructor
    explicit FObject (FObject* = nullptr);

    // Disable copy constructor
    FObject (const FObject&) = delete;

    // Destructor
    virtual ~FObject();

    // Disable assignment operator (=)
    FObject& operator = (const FObject&) = delete;

    // Accessors
    virtual const FString getClassName() const;
    FObject*              getParent() const;
    FObject*              getChild (int) const;
    FObjectList&          getChildren();
    const FObjectList&    getChildren() const;
    int                   numOfChildren() const;
    iterator              begin();
    iterator              end();
    const_iterator        begin() const;
    const_iterator        end() const;

    // Inquiries
    bool                  hasParent() const;
    bool                  hasChildren() const;
    bool                  isChild (const FObject*) const;
    bool                  isDirectChild (const FObject*) const;
    bool                  isWidget() const;
    bool                  isInstanceOf (const FString&) const;
    bool                  isTimerInUpdating() const;

    // Methods
    void                  removeParent();
    void                  addChild (FObject*);
    void                  delChild (FObject*);

    // Event handler
    virtual bool          event (FEvent*);

    // Timer methods
    static void           getCurrentTime (timeval*);
    static bool           isTimeout (timeval*, uInt64);
    int                   addTimer (int);
    bool                  delTimer (int);
    bool                  delOwnTimer();
    bool                  delAllTimer();

  protected:
    struct timer_data
    {
      int       id;
      timeval   interval;
      timeval   timeout;
      FObject*  object;
    };

    // Typedefs
    typedef std::vector<timer_data> TimerList;

    // Accessor
    TimerList*            getTimerList() const;

    // Mutator
    void                  setWidgetProperty (bool);

    // Method
    uInt                  processTimerEvent();

    // Event handler
    virtual void          onTimer (FTimerEvent*);
    virtual void          onUserEvent (FUserEvent*);

  private:
    // Method
    virtual void          performTimerAction ( const FObject*
                                             , const FEvent* );

    // Data members
    FObject*              parent_obj{nullptr};
    FObjectList           children_list{};  // no children yet
    bool                  has_parent{false};
    bool                  widget_object{false};
    static bool           timer_modify_lock;
    static TimerList*     timer_list;
};


//----------------------------------------------------------------------
inline const FString FObject::getClassName() const
{ return "FObject"; }

//----------------------------------------------------------------------
inline FObject* FObject::getParent() const
{ return parent_obj; }

//----------------------------------------------------------------------
inline FObject::FObjectList& FObject::getChildren()
{ return children_list; }

//----------------------------------------------------------------------
inline const FObject::FObjectList& FObject::getChildren() const
{ return children_list; }

//----------------------------------------------------------------------
inline int FObject::numOfChildren() const
{ return int(children_list.size()); }

//----------------------------------------------------------------------
inline FObject::iterator FObject::begin()
{ return children_list.begin(); }

//----------------------------------------------------------------------
inline FObject::iterator FObject::end()
{ return children_list.end(); }

//----------------------------------------------------------------------
inline FObject::const_iterator FObject::begin() const
{ return children_list.begin(); }

//----------------------------------------------------------------------
inline FObject::const_iterator FObject::end() const
{ return children_list.end(); }

//----------------------------------------------------------------------
inline bool FObject::hasParent() const
{ return has_parent; }

//----------------------------------------------------------------------
inline bool FObject::hasChildren() const
{ return bool( ! children_list.empty() ); }

//----------------------------------------------------------------------
inline bool FObject::isDirectChild (const FObject* obj) const
{ return bool( obj->getParent() == this ); }

//----------------------------------------------------------------------
inline bool FObject::isWidget() const
{ return widget_object; }

//----------------------------------------------------------------------
inline bool FObject::isInstanceOf (const FString& classname) const
{ return bool( classname == getClassName() ); }

//----------------------------------------------------------------------
inline bool FObject::isTimerInUpdating() const
{ return timer_modify_lock; }

//----------------------------------------------------------------------
inline FObject::TimerList* FObject::getTimerList() const
{ return timer_list; }

//----------------------------------------------------------------------
inline void FObject::setWidgetProperty (bool property)
{ widget_object = property; }


//----------------------------------------------------------------------
// Operator functions for timeval
//----------------------------------------------------------------------

static inline timeval operator + (const timeval& t1, const timeval& t2)
{
  timeval tmp{};
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
  timeval tmp{};
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

}  // namespace finalcut

#endif  // FOBJECT_H
