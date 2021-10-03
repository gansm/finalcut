/***********************************************************************
* fobject.h - Object container base class of all widget objects        *
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
#include <chrono>
#include <list>
#include <memory>
#include <vector>

#include "final/ftypes.h"
#include "final/util/fstring.h"

namespace finalcut
{

using std::chrono::duration_cast;
using std::chrono::seconds;
using std::chrono::milliseconds;
using std::chrono::microseconds;
using std::chrono::system_clock;
using std::chrono::time_point;

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
    // Using-declarations
    using FObjectList     = std::list<FObject*>;
    using iterator        = FObjectList::iterator;
    using const_iterator  = FObjectList::const_iterator;
    using reference       = FObjectList::reference;
    using const_reference = FObjectList::const_reference;

    // Constants
    static constexpr auto UNLIMITED = static_cast<std::size_t>(-1);

    // Constructor
    explicit FObject (FObject* = nullptr);

    // Disable copy constructor
    FObject (const FObject&) = delete;

    // Destructor
    virtual ~FObject();

    // Disable copy assignment operator (=)
    FObject& operator = (const FObject&) = delete;

    // Accessors
    virtual FString       getClassName() const;
    FObject*              getParent() const &;
    FObject*              getChild (int) const &;
    FObjectList&          getChildren() &;
    const FObjectList&    getChildren() const &;
    std::size_t           getMaxChildren() const &;
    std::size_t           numOfChildren() const &;
    iterator              begin();
    iterator              end();
    const_iterator        begin() const;
    const_iterator        end() const;
    reference             front();
    reference             back();
    const_reference       front() const;
    const_reference       back() const;

    // Mutator
    void                  setMaxChildren (std::size_t);

    // Inquiries
    bool                  hasParent() const & ;
    bool                  hasChildren() const &;
    bool                  isChild (const FObject*) const &;
    bool                  isDirectChild (const FObject*) const &;
    bool                  isWidget() const;
    bool                  isInstanceOf (const FString&) const;

    // Methods
    void                  removeParent() &;
    void                  addChild (FObject*) &;
    void                  delChild (FObject*) &;
    void                  setParent (FObject*) &;

    // Event handler
    virtual bool          event (FEvent*);

    // Timer methods
    static TimeValue      getCurrentTime();
    static bool           isTimeout (const TimeValue&, uInt64);
    int                   addTimer (int) &;
    bool                  delTimer (int) const &;
    bool                  delOwnTimers() const &;
    bool                  delAllTimers() const &;

  protected:
    struct FTimerData
    {
      int          id;
      milliseconds interval;
      TimeValue    timeout;
      FObject*     object;
    };

    // Using-declaration
    using FTimerList = std::vector<FTimerData>;
    using FTimerListUniquePtr = std::unique_ptr<FTimerList>;

    // Accessor
    FTimerList*           getTimerList() const;

    // Mutator
    void                  setWidgetProperty (bool = true);

    // Method
    uInt                  processTimerEvent();

    // Event handler
    virtual void          onTimer (FTimerEvent*);
    virtual void          onUserEvent (FUserEvent*);

  private:
    // Method
    virtual void          performTimerAction (FObject*, FEvent*);
    static auto           globalTimerList() -> const FTimerListUniquePtr&;

    // Data members
    FObject*              parent_obj{nullptr};
    FObjectList           children_list{};  // no children yet
    std::size_t           max_children{UNLIMITED};
    bool                  has_parent{false};
    bool                  widget_object{false};
};


//----------------------------------------------------------------------
inline FString FObject::getClassName() const
{ return "FObject"; }

//----------------------------------------------------------------------
inline FObject* FObject::getParent() const &
{ return parent_obj; }

//----------------------------------------------------------------------
inline FObject::FObjectList& FObject::getChildren() &
{ return children_list; }

//----------------------------------------------------------------------
inline const FObject::FObjectList& FObject::getChildren() const &
{ return children_list; }

//----------------------------------------------------------------------
inline std::size_t FObject::getMaxChildren() const &
{ return max_children; }

//----------------------------------------------------------------------
inline std::size_t FObject::numOfChildren() const &
{ return children_list.size(); }

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
inline FObject::reference FObject::front()
{ return children_list.front(); }

//----------------------------------------------------------------------
inline FObject::reference FObject::back()
{ return children_list.back(); }

//----------------------------------------------------------------------
inline FObject::const_reference FObject::front() const
{ return children_list.front(); }

//----------------------------------------------------------------------
inline FObject::const_reference FObject::back() const
{ return children_list.back(); }

//----------------------------------------------------------------------
inline void FObject::setMaxChildren (std::size_t max)
{ max_children = max; }

//----------------------------------------------------------------------
inline bool FObject::hasParent() const &
{ return has_parent; }

//----------------------------------------------------------------------
inline bool FObject::hasChildren() const &
{ return ! children_list.empty(); }

//----------------------------------------------------------------------
inline bool FObject::isDirectChild (const FObject* obj) const &
{ return obj->getParent() == this; }

//----------------------------------------------------------------------
inline bool FObject::isWidget() const
{ return widget_object; }

//----------------------------------------------------------------------
inline bool FObject::isInstanceOf (const FString& classname) const
{ return classname == getClassName(); }

//----------------------------------------------------------------------
inline FObject::FTimerList* FObject::getTimerList() const
{ return globalTimerList().get(); }

//----------------------------------------------------------------------
inline void FObject::setWidgetProperty (bool property)
{ widget_object = property; }

}  // namespace finalcut

#endif  // FOBJECT_H
