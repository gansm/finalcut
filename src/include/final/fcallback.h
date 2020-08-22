/***********************************************************************
* fcallback.h - Implements the callback functionality                  *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2020 Markus Gans                                           *
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

#include "final/fstring.h"
#include "final/ftypes.h"


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
  FCallbackData()
  { }

  template<typename FuncPtr>
  FCallbackData (const FString& s, FWidget* i, FuncPtr m, const FCall& c)
    : cb_signal(s)
    , cb_instance(i)
    , cb_function_ptr(m)
    , cb_function(c)
  { }

  FCallbackData (const FCallbackData&) = default;
  FCallbackData (FCallbackData&&) = default;

  FCallbackData& operator = (const FCallbackData&) = default;
  FCallbackData& operator = (FCallbackData&&) noexcept = default;

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
    template<typename T>
    using ObjectPointer =
        typename std::enable_if< ! std::is_member_function_pointer<T>::value
                              && ! std::is_function<typename std::remove_pointer<T>::type>::value
                              && ! std::is_function<T>::value
                              && std::is_pointer<T>::value
                              && std::is_object<T>::value
                              && ! std::is_class<T>::value
                              , std::nullptr_t >;
    template<typename T>
    using ClassObject =
        typename std::enable_if< ! std::is_member_function_pointer<T>::value
                              && ! std::is_function<typename std::remove_pointer<T>::type>::value
                              && ! std::is_function<T>::value
                              && ! std::is_pointer<T>::value
                              && std::is_object<T>::value
                              && std::is_class<T>::value
                              , std::nullptr_t >;
    template<typename T>
    using MemberFunctionPointer =
        typename std::enable_if< std::is_member_function_pointer<T>::value
                              && ! std::is_function<typename std::remove_pointer<T>::type>::value
                              && ! std::is_function<T>::value
                              && ! std::is_pointer<T>::value
                              && std::is_object<T>::value
                              && ! std::is_class<T>::value
                              , std::nullptr_t >;
    template<typename T>
    using FunctionPointer =
        typename std::enable_if< ! std::is_member_function_pointer<T>::value
                              && std::is_function<typename std::remove_pointer<T>::type>::value
                              && ! std::is_function<T>::value
                              && std::is_pointer<T>::value
                              && std::is_object<T>::value
                              && ! std::is_class<T>::value
                              , std::nullptr_t >;
    template<typename T>
    using FunctionReference =
        typename std::enable_if< ! std::is_member_function_pointer<T>::value
                              && std::is_function<typename std::remove_pointer<T>::type>::value
                              && std::is_function<T>::value
                              && ! std::is_pointer<T>::value
                              && ! std::is_object<T>::value
                              && ! std::is_class<T>::value
                              , std::nullptr_t >;

    // Constructors
    FCallback();

    // Disable copy constructor
    FCallback (const FCallback&) = delete;

    // Destructor
    ~FCallback();

    // Disable copy assignment operator (=)
    FCallback& operator = (const FCallback&) = delete;

    // Accessors
    const FString getClassName() const;
    std::size_t getCallbackCount() const;

    // Methods
    template<typename Object
           , typename Function
           , typename ObjectPointer<Object>::type = nullptr
           , typename MemberFunctionPointer<Function>::type = nullptr
           , typename... Args>
    void addCallback ( const FString& cb_signal
                     , Object&&       cb_instance
                     , Function&&     cb_member
                     , Args&&...      args);
    template<typename Object
           , typename Function
           , typename ObjectPointer<Object>::type = nullptr
           , typename ClassObject<Function>::type = nullptr
           , typename... Args>
    void addCallback ( const FString& cb_signal
                     , Object&&       cb_instance
                     , Function&&     cb_function
                     , Args&&...      args);
    template<typename Function
           , typename ClassObject<Function>::type = nullptr
           , typename... Args>
    void addCallback ( const FString& cb_signal
                     , Function&&     cb_function
                     , Args&&...      args);
    template<typename Function
           , typename ClassObject<Function>::type = nullptr
           , typename... Args>
    void addCallback ( const FString& cb_signal
                     , Function&      cb_function
                     , Args&&...      args);
    template<typename Function
           , typename FunctionReference<Function>::type = nullptr
           , typename... Args>
    void addCallback ( const FString& cb_signal
                     , Function&      cb_function
                     , Args&&...      args);
    template<typename Function
           , typename FunctionPointer<Function>::type = nullptr
           , typename... Args>
    void addCallback ( const FString& cb_signal
                     , Function&&     cb_function
                     , Args&&...      args);
    template<typename Object
           , typename ObjectPointer<Object>::type = nullptr>
    void delCallback (Object&& cb_instance);
    void delCallback (const FString& cb_signal);
    template<typename Object
           , typename ObjectPointer<Object>::type = nullptr>
    void delCallback (const FString& cb_signal, Object&& cb_instance);
    template<typename FunctionPtr
           , typename FunctionPointer<FunctionPtr>::type = nullptr>
    void delCallback (FunctionPtr&& cb_func_ptr);
    template<typename Function
           , typename FunctionReference<Function>::type = nullptr>
    void delCallback (const Function& cb_function);
    void delCallback();
    void emitCallback (const FString& emit_signal) const;

  private:
    // Typedefs
    typedef std::vector<FCallbackData>  FCallbackObjects;

    // Data members
    FCallbackObjects  callback_objects{};
};

// FCallback inline functions
//----------------------------------------------------------------------
inline const FString FCallback::getClassName() const
{ return "FCallback"; }

//----------------------------------------------------------------------
inline std::size_t FCallback::getCallbackCount() const
{ return callback_objects.size(); }

//----------------------------------------------------------------------
template<typename Object
       , typename Function
       , typename FCallback::ObjectPointer<Object>::type
       , typename FCallback::MemberFunctionPointer<Function>::type
       , typename... Args>
inline void FCallback::addCallback ( const FString& cb_signal
                                   , Object&&       cb_instance
                                   , Function&&     cb_member
                                   , Args&&...      args)
{
  // Add a member function pointer as callback

  Object instance = cb_instance;
  auto fn = std::bind ( std::forward<Function>(cb_member)
                      , std::forward<Object>(cb_instance)
                      , std::forward<Args>(args)... );
  FCallbackData obj{ cb_signal, instance, nullptr, fn };
  callback_objects.push_back(obj);
}

//----------------------------------------------------------------------
template<typename Object
       , typename Function
       , typename FCallback::ObjectPointer<Object>::type
       , typename FCallback::ClassObject<Function>::type
       , typename... Args>
inline void FCallback::addCallback ( const FString& cb_signal
                                   , Object&&       cb_instance
                                   , Function&&     cb_function
                                   , Args&&...      args)
{
  // Add a function object to an instance as callback

  auto fn = std::bind (std::forward<Function>(cb_function), std::forward<Args>(args)...);
  FCallbackData obj{ cb_signal, cb_instance, nullptr, fn };
  callback_objects.push_back(obj);
}

//----------------------------------------------------------------------
template<typename Function
       , typename FCallback::ClassObject<Function>::type
       , typename... Args>
inline void FCallback::addCallback ( const FString& cb_signal
                                   , Function&&     cb_function
                                   , Args&&...      args)
{
  // Add a function object as callback

  auto fn = std::bind ( std::forward<Function>(cb_function)
                      , std::forward<Args>(args)... );
  FCallbackData obj{ cb_signal, nullptr, nullptr, fn };
  callback_objects.push_back(obj);
}

//----------------------------------------------------------------------
template<typename Function
       , typename FCallback::ClassObject<Function>::type
       , typename... Args>
inline void FCallback::addCallback ( const FString& cb_signal
                                   , Function&      cb_function
                                   , Args&&...      args)
{
  // Add a function object reference as callback

  auto fn = std::bind (cb_function, std::forward<Args>(args)...);
  FCallbackData obj{ cb_signal, nullptr, nullptr, fn };
  callback_objects.push_back(obj);
}

//----------------------------------------------------------------------
template<typename Function
       , typename FCallback::FunctionReference<Function>::type
       , typename... Args>
inline void FCallback::addCallback ( const FString& cb_signal
                                   , Function&      cb_function
                                   , Args&&...      args)
{
  // Add a function reference as callback

  auto ptr = reinterpret_cast<void*>(&cb_function);
  auto fn = std::bind (cb_function, std::forward<Args>(args)...);
  FCallbackData obj{ cb_signal, nullptr, ptr, fn };
  callback_objects.push_back(obj);
}

//----------------------------------------------------------------------
template<typename Function
       , typename FCallback::FunctionPointer<Function>::type
       , typename... Args>
inline void FCallback::addCallback ( const FString& cb_signal
                                   , Function&&     cb_function
                                   , Args&&...      args)
{
  // Add a function pointer as callback

  auto ptr = reinterpret_cast<void*>(cb_function);
  auto fn = std::bind ( std::forward<Function>(cb_function)
                      , std::forward<Args>(args)... );
  FCallbackData obj{ cb_signal, nullptr, ptr, fn };
  callback_objects.push_back(obj);
}

//----------------------------------------------------------------------
template<typename Object
       , typename FCallback::ObjectPointer<Object>::type>
inline void FCallback::delCallback (Object&& cb_instance)
{
  // Deletes entries with the given instance from the callback list

  if ( callback_objects.empty() )
    return;

  auto iter = callback_objects.begin();

  while ( iter != callback_objects.end() )
  {
    if ( iter->cb_instance == cb_instance )
      iter = callback_objects.erase(iter);
    else
      ++iter;
  }
}

//----------------------------------------------------------------------
template<typename Object
       , typename FCallback::ObjectPointer<Object>::type>
inline void FCallback::delCallback (const FString& cb_signal, Object&& cb_instance)
{
  // Deletes entries with the given signal and instance
  // from the callback list

  if ( callback_objects.empty() )
    return;

  auto iter = callback_objects.begin();

  while ( iter != callback_objects.end() )
  {
    if ( iter->cb_signal == cb_signal
      && iter->cb_instance == cb_instance )
      iter = callback_objects.erase(iter);
    else
      ++iter;
  }
}

//----------------------------------------------------------------------
template<typename FunctionPtr
       , typename FCallback::FunctionPointer<FunctionPtr>::type>
inline void FCallback::delCallback (FunctionPtr&& cb_func_ptr)
{
  // Deletes entries with the given function pointer
  // from the callback list

  if ( callback_objects.empty() )
    return;

  auto ptr = reinterpret_cast<void*>(cb_func_ptr);
  auto iter = callback_objects.begin();

  while ( iter != callback_objects.end() )
  {
    if ( iter->cb_function_ptr == ptr )
      iter = callback_objects.erase(iter);
    else
      ++iter;
  }
}

//----------------------------------------------------------------------
template<typename Function
       , typename FCallback::FunctionReference<Function>::type>
inline void FCallback::delCallback (const Function& cb_function)
{
  // Deletes entries with the given function reference
  // from the callback list

  if ( callback_objects.empty() )
    return;

  auto ptr = reinterpret_cast<void*>(&cb_function);
  auto iter = callback_objects.begin();

  while ( iter != callback_objects.end() )
  {
    if ( iter->cb_function_ptr == ptr )
      iter = callback_objects.erase(iter);
    else
      ++iter;
  }
}

}  // namespace finalcut

#endif  // FCALLBACK_H
