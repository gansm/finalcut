/***********************************************************************
* fcallback.h - Implements the callback functionality                  *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2020-2022 Markus Gans                                      *
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

/*  Standalone class
 *  ════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▔▔▏1     *▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FCallback ▏- - - -▕ FCallbackData ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▏       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FCALLBACK_H
#define FCALLBACK_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <utility>
#include <vector>

#include "final/ftypes.h"
#include "final/util/fstring.h"

namespace finalcut
{

// class forward declaration
class FWidget;

//----------------------------------------------------------------------
// struct FCallbackData
//----------------------------------------------------------------------

struct FCallbackData
{
  // Constructor
  FCallbackData() = default;

  template <typename FuncPtr>
  FCallbackData (FString&& s, FWidget* i, FuncPtr m, FCall&& c)
    : cb_signal(std::move(s))
    , cb_instance(i)
    , cb_function_ptr(m)
    , cb_function(std::move(c))
  { }

  FCallbackData (const FCallbackData&) = default;
  FCallbackData (FCallbackData&&) noexcept = default;

  auto operator = (const FCallbackData&) -> FCallbackData& = default;
  auto operator = (FCallbackData&&) noexcept -> FCallbackData& = default;

  // Data members
  FString   cb_signal{};
  FWidget*  cb_instance{};
  void*     cb_function_ptr{};
  FCall     cb_function{};
};


//----------------------------------------------------------------------
// class FCallback
//----------------------------------------------------------------------

class FCallback
{
  public:
    // Using-declaration
    template <typename T>
    using enable_if_ObjectPointer_t =
        std::enable_if_t< ! std::is_member_function_pointer<T>::value
                       && ! std::is_function<std::remove_pointer_t<T>>::value
                       && ! std::is_function<T>::value
                       && std::is_pointer<T>::value
                       && std::is_object<T>::value
                       && ! std::is_class<T>::value
                       , std::nullptr_t >;
    template <typename T>
    using enable_if_ClassObject_t =
        std::enable_if_t< ! std::is_member_function_pointer<T>::value
                       && ! std::is_function<std::remove_pointer_t<T>>::value
                       && ! std::is_function<T>::value
                       && ! std::is_pointer<T>::value
                       && std::is_object<T>::value
                       && std::is_class<T>::value
                       , std::nullptr_t >;
    template <typename T>
    using enable_if_MemberFunctionPointer_t =
        std::enable_if_t< std::is_member_function_pointer<T>::value
                       && ! std::is_function<std::remove_pointer_t<T>>::value
                       && ! std::is_function<T>::value
                       && ! std::is_pointer<T>::value
                       && std::is_object<T>::value
                       && ! std::is_class<T>::value
                       , std::nullptr_t >;
    template <typename T>
    using enable_if_FunctionPointer_t =
        std::enable_if_t< ! std::is_member_function_pointer<T>::value
                       && std::is_function<std::remove_pointer_t<T>>::value
                       && ! std::is_function<T>::value
                       && std::is_pointer<T>::value
                       && std::is_object<T>::value
                       && ! std::is_class<T>::value
                       , std::nullptr_t >;
    template <typename T>
    using enable_if_FunctionReference_t =
        std::enable_if_t< ! std::is_member_function_pointer<T>::value
                       && std::is_function<std::remove_pointer_t<T>>::value
                       && std::is_function<T>::value
                       && ! std::is_pointer<T>::value
                       && ! std::is_object<T>::value
                       && ! std::is_class<T>::value
                       , std::nullptr_t >;

    // Constructors
    FCallback() = default;

    // Disable copy constructor
    FCallback (const FCallback&) = delete;

    // Destructor
    ~FCallback() noexcept = default;

    // Disable copy assignment operator (=)
    auto operator = (const FCallback&) -> FCallback& = delete;

    // Accessors
    auto getClassName() const -> FString;
    auto getCallbackCount() const -> std::size_t;

    // Methods
    template <typename Object
            , typename Function
            , enable_if_ObjectPointer_t<Object> = nullptr
            , enable_if_MemberFunctionPointer_t<Function> = nullptr
            , typename... Args>
    void addCallback ( FString&&  cb_signal
                     , Object&&   cb_instance
                     , Function&& cb_member
                     , Args&&...  args) noexcept;
    template <typename Object
             , typename Function
             , enable_if_ObjectPointer_t<Object> = nullptr
             , enable_if_ClassObject_t<Function> = nullptr
             , typename... Args>
    void addCallback ( FString&&  cb_signal
                     , Object&&   cb_instance
                     , Function&& cb_function
                     , Args&&...  args) noexcept;
    template < typename Function
             , enable_if_ClassObject_t<Function> = nullptr
             , typename... Args>
    void addCallback ( FString&&  cb_signal
                     , Function&& cb_function
                     , Args&&...  args) noexcept;
    template <typename Function
            , enable_if_ClassObject_t<Function> = nullptr
            , typename... Args>
    void addCallback ( FString&& cb_signal
                     , Function& cb_function
                     , Args&&... args) noexcept;
    template <typename Function
            , enable_if_FunctionReference_t<Function> = nullptr
            , typename... Args>
    void addCallback ( FString&& cb_signal
                     , Function& cb_function
                     , Args&&... args) noexcept;
    template <typename Function
            , enable_if_FunctionPointer_t<Function> = nullptr
            , typename... Args>
    void addCallback ( FString&&  cb_signal
                     , Function&& cb_function
                     , Args&&...  args) noexcept;
    template <typename Object
            , enable_if_ObjectPointer_t<Object> = nullptr>
    void delCallback (Object&& cb_instance) noexcept;
    void delCallback (const FString& cb_signal);
    template <typename Object
            , enable_if_ObjectPointer_t<Object> = nullptr>
    void delCallback ( const FString& cb_signal
                     , Object&& cb_instance ) noexcept;
    template <typename FunctionPtr
            , enable_if_FunctionPointer_t<FunctionPtr> = nullptr>
    void delCallback (FunctionPtr&& cb_func_ptr) noexcept;
    template <typename Function
            , enable_if_FunctionReference_t<Function> = nullptr>
    void delCallback (const Function& cb_function);
    void delCallback();
    void emitCallback (const FString& emit_signal) const;

  private:
    // Using-declaration
    using FCallbackObjects = std::vector<FCallbackData>;

    // Data members
    FCallbackObjects  callback_objects{};
};

// FCallback inline functions
//----------------------------------------------------------------------
inline auto FCallback::getClassName() const -> FString
{ return "FCallback"; }

//----------------------------------------------------------------------
inline auto FCallback::getCallbackCount() const -> std::size_t
{ return callback_objects.size(); }

//----------------------------------------------------------------------
template <typename Object
        , typename Function
        , FCallback::enable_if_ObjectPointer_t<Object>
        , FCallback::enable_if_MemberFunctionPointer_t<Function>
        , typename... Args>
inline void FCallback::addCallback ( FString&&  cb_signal
                                   , Object&&   cb_instance
                                   , Function&& cb_member
                                   , Args&&...  args) noexcept
{
  // Add a member function pointer as callback

  Object instance = cb_instance;
  auto fn = std::bind ( std::forward<Function>(cb_member)
                      , std::forward<Object>(cb_instance)
                      , std::forward<Args>(args)... );
  callback_objects.emplace_back (std::move(cb_signal), instance, nullptr, fn);
}

//----------------------------------------------------------------------
template <typename Object
         , typename Function
         , FCallback::enable_if_ObjectPointer_t<Object>
         , FCallback::enable_if_ClassObject_t<Function>
         , typename... Args>
inline void FCallback::addCallback ( FString&&  cb_signal
                                   , Object&&   cb_instance
                                   , Function&& cb_function
                                   , Args&&...  args) noexcept
{
  // Add a function object to an instance as callback

  auto fn = std::bind (std::forward<Function>(cb_function), std::forward<Args>(args)...);
  callback_objects.emplace_back (std::move(cb_signal), cb_instance, nullptr, fn);
}

//----------------------------------------------------------------------
template <typename Function
        , FCallback::enable_if_ClassObject_t<Function>
        , typename... Args>
inline void FCallback::addCallback ( FString&&  cb_signal
                                   , Function&& cb_function
                                   , Args&&...  args) noexcept
{
  // Add a function object as callback

  auto fn = std::bind ( std::forward<Function>(cb_function)
                      , std::forward<Args>(args)... );
  callback_objects.emplace_back (std::move(cb_signal), nullptr, nullptr, fn);
}

//----------------------------------------------------------------------
template <typename Function
        , FCallback::enable_if_ClassObject_t<Function>
        , typename... Args>
inline void FCallback::addCallback ( FString&& cb_signal
                                   , Function& cb_function
                                   , Args&&... args) noexcept
{
  // Add a function object reference as callback

  auto fn = std::bind (cb_function, std::forward<Args>(args)...);
  callback_objects.emplace_back (std::move(cb_signal), nullptr, nullptr, fn);
}

//----------------------------------------------------------------------
template <typename Function
        , FCallback::enable_if_FunctionReference_t<Function>
        , typename... Args>
inline void FCallback::addCallback ( FString&& cb_signal
                                   , Function& cb_function
                                   , Args&&... args) noexcept
{
  // Add a function reference as callback

  auto ptr = reinterpret_cast<void*>(&cb_function);
  auto fn = std::bind (cb_function, std::forward<Args>(args)...);
  callback_objects.emplace_back (std::move(cb_signal), nullptr, ptr, fn);
}

//----------------------------------------------------------------------
template <typename Function
        , FCallback::enable_if_FunctionPointer_t<Function>
        , typename... Args>
inline void FCallback::addCallback ( FString&&  cb_signal
                                   , Function&& cb_function
                                   , Args&&...  args) noexcept
{
  // Add a function pointer as callback

  auto ptr = reinterpret_cast<void*>(cb_function);
  auto fn = std::bind ( std::forward<Function>(cb_function)
                      , std::forward<Args>(args)... );
  callback_objects.emplace_back(std::move(cb_signal), nullptr, ptr, fn);
}

//----------------------------------------------------------------------
template <typename Object
        , FCallback::enable_if_ObjectPointer_t<Object>>
inline void FCallback::delCallback (Object&& cb_instance) noexcept
{
  // Deletes entries with the given instance from the callback list

  if ( callback_objects.empty() )
    return;

  auto iter = callback_objects.cbegin();

  while ( iter != callback_objects.cend() )
  {
    if ( iter->cb_instance == cb_instance )
      iter = callback_objects.erase(iter);
    else
      ++iter;
  }
}

//----------------------------------------------------------------------
template <typename Object
        , FCallback::enable_if_ObjectPointer_t<Object>>
inline void FCallback::delCallback ( const FString& cb_signal
                                   , Object&& cb_instance ) noexcept
{
  // Deletes entries with the given signal and instance
  // from the callback list

  if ( callback_objects.empty() )
    return;

  auto iter = callback_objects.cbegin();

  while ( iter != callback_objects.cend() )
  {
    if ( iter->cb_signal == cb_signal
      && iter->cb_instance == cb_instance )
      iter = callback_objects.erase(iter);
    else
      ++iter;
  }
}

//----------------------------------------------------------------------
template <typename FunctionPtr
        , FCallback::enable_if_FunctionPointer_t<FunctionPtr>>
inline void FCallback::delCallback (FunctionPtr&& cb_func_ptr) noexcept
{
  // Deletes entries with the given function pointer
  // from the callback list

  if ( callback_objects.empty() )
    return;

  auto ptr = reinterpret_cast<void*>(cb_func_ptr);
  auto iter = callback_objects.cbegin();

  while ( iter != callback_objects.cend() )
  {
    if ( iter->cb_function_ptr == ptr )
      iter = callback_objects.erase(iter);
    else
      ++iter;
  }
}

//----------------------------------------------------------------------
template <typename Function
        , FCallback::enable_if_FunctionReference_t<Function>>
inline void FCallback::delCallback (const Function& cb_function)
{
  // Deletes entries with the given function reference
  // from the callback list

  if ( callback_objects.empty() )
    return;

  auto ptr = reinterpret_cast<void*>(&cb_function);
  auto iter = callback_objects.cbegin();

  while ( iter != callback_objects.cend() )
  {
    if ( iter->cb_function_ptr == ptr )
      iter = callback_objects.erase(iter);
    else
      ++iter;
  }
}

}  // namespace finalcut

#endif  // FCALLBACK_H
