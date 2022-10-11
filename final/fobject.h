/***********************************************************************
* fobject.h - Object container base class of all widget objects        *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2022 Markus Gans                                      *
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
#elif __cplusplus > 1 && __cplusplus < 201402L
  #error "Your C++ compiler does not support the C++14 standard!"
#endif

#include <sys/time.h>  // need for gettimeofday
#include <cstdlib>
#include <cstring>
#include <chrono>
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
    using FObjectList     = std::vector<FObject*>;
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

    // Disable move constructor
    FObject (FObject&&) noexcept = delete;

    // Destructor
    virtual ~FObject();

    // Disable copy assignment operator (=)
    auto operator = (const FObject&) -> FObject& = delete;

    // Disable move assignment operator (=)
    auto operator = (FObject&&) noexcept -> FObject& = delete;

    // Accessors
    virtual auto getClassName() const -> FString;
    auto         getParent() const & -> FObject*;
    auto         getChild (int) const & -> FObject*;
    auto         getChildren() & -> FObjectList&;
    auto         getChildren() const & -> const FObjectList&;
    auto         getMaxChildren() const & noexcept -> std::size_t;
    auto         numOfChildren() const & -> std::size_t;
    auto         begin() -> iterator;
    auto         end() -> iterator;
    auto         begin() const -> const_iterator;
    auto         end() const -> const_iterator;
    auto         cbegin() const noexcept -> const_iterator;
    auto         cend() const noexcept -> const_iterator;
    auto         front() -> reference;
    auto         back() -> reference;
    auto         front() const -> const_reference;
    auto         back() const -> const_reference;

    // Mutator
    void         setMaxChildren (std::size_t) noexcept;

    // Inquiries
    auto         hasParent() const & noexcept -> bool;
    auto         hasChildren() const & -> bool;
    auto         isChild (const FObject*) const & -> bool;
    auto         isDirectChild (const FObject*) const & -> bool;
    auto         isWidget() const noexcept -> bool;
    auto         isInstanceOf (const FString&) const -> bool;

    // Methods
    void         removeParent() &;
    void         addChild (FObject*) &;
    void         delChild (FObject*) &;
    void         setParent (FObject*) &;

    // Event handler
    virtual auto event (FEvent*) -> bool;

    // Timer methods
    static auto  getCurrentTime() -> TimeValue;
    static auto  isTimeout (const TimeValue&, uInt64) -> bool;
    auto         addTimer (int) & -> int;
    auto         delTimer (int) const & -> bool;
    auto         delOwnTimers() const & -> bool;
    auto         delAllTimers() const & -> bool;

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
    auto         getTimerList() const -> FTimerList*;

    // Mutator
    void         setWidgetProperty (bool = true);

    // Method
    auto         processTimerEvent() -> uInt;

    // Event handler
    virtual void onTimer (FTimerEvent*);
    virtual void onUserEvent (FUserEvent*);

  private:
    // Method
    virtual void performTimerAction (FObject*, FEvent*);
    static auto  globalTimerList() -> const FTimerListUniquePtr&;

    // Data members
    FObject*     parent_obj{nullptr};
    FObjectList  children_list{};  // no children yet
    std::size_t  max_children{UNLIMITED};
    bool         has_parent{false};
    bool         widget_object{false};
};


//----------------------------------------------------------------------
inline auto FObject::getClassName() const -> FString
{ return "FObject"; }

//----------------------------------------------------------------------
inline auto FObject::getParent() const & -> FObject*
{ return parent_obj; }

//----------------------------------------------------------------------
inline auto FObject::getChildren() & -> FObjectList&
{ return children_list; }

//----------------------------------------------------------------------
inline auto FObject::getChildren() const & -> const FObjectList&
{ return children_list; }

//----------------------------------------------------------------------
inline auto FObject::getMaxChildren() const & noexcept -> std::size_t
{ return max_children; }

//----------------------------------------------------------------------
inline auto FObject::numOfChildren() const & -> std::size_t
{ return children_list.size(); }

//----------------------------------------------------------------------
inline auto FObject::begin() -> iterator
{ return children_list.begin(); }

//----------------------------------------------------------------------
inline auto FObject::end() -> iterator
{ return children_list.end(); }

//----------------------------------------------------------------------
inline auto FObject::begin() const -> const_iterator
{ return children_list.begin(); }

//----------------------------------------------------------------------
inline auto FObject::end() const -> const_iterator
{ return children_list.end(); }

//----------------------------------------------------------------------
inline auto FObject::cbegin() const noexcept -> const_iterator
{ return children_list.cbegin(); }

//----------------------------------------------------------------------
inline auto FObject::cend() const noexcept -> const_iterator
{ return children_list.cend(); }

//----------------------------------------------------------------------
inline auto FObject::front() -> reference
{ return children_list.front(); }

//----------------------------------------------------------------------
inline auto FObject::back() -> reference
{ return children_list.back(); }

//----------------------------------------------------------------------
inline auto FObject::front() const -> const_reference
{ return children_list.front(); }

//----------------------------------------------------------------------
inline auto FObject::back() const -> const_reference
{ return children_list.back(); }

//----------------------------------------------------------------------
inline void FObject::setMaxChildren (std::size_t max) noexcept
{ max_children = max; }

//----------------------------------------------------------------------
inline auto FObject::hasParent() const & noexcept -> bool
{ return has_parent; }

//----------------------------------------------------------------------
inline auto FObject::hasChildren() const & -> bool
{ return ! children_list.empty(); }

//----------------------------------------------------------------------
inline auto FObject::isDirectChild (const FObject* obj) const & -> bool
{ return obj->getParent() == this; }

//----------------------------------------------------------------------
inline auto FObject::isWidget() const noexcept -> bool
{ return widget_object; }

//----------------------------------------------------------------------
inline auto FObject::isInstanceOf (const FString& classname) const -> bool
{ return classname == getClassName(); }

//----------------------------------------------------------------------
inline auto FObject::getTimerList() const -> FTimerList*
{ return globalTimerList().get(); }

//----------------------------------------------------------------------
inline void FObject::setWidgetProperty (bool property)
{ widget_object = property; }

}  // namespace finalcut

#endif  // FOBJECT_H
