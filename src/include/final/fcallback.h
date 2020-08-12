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

  FCallbackData (const FString& s, FWidget* i, void* m, const FCall& c)
    : cb_signal(s)
    , cb_instance(i)
    , cb_function_ptr(m)
    , cb_function(c)
  { }

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
    // Constructors
    FCallback();

    // Disable copy constructor
    FCallback (const FCallback&) = delete;

    // Destructor
    ~FCallback();

    // Disable copy assignment operator (=)
    FCallback& operator = (const FCallback&) = delete;

    // Accessors
    const FString getClassName() const
    {
      return "FCallback";
    }

    std::size_t getCallbackCount() const
    {
      return callback_objects.size();
    }

    // Methods
    template<typename Object
           , typename Function
           , typename std::enable_if< ! std::is_member_function_pointer<Object>::value
                                   && ! std::is_function<typename std::remove_pointer<Object>::type>::value
                                   && ! std::is_function<Object>::value
                                   && std::is_pointer<Object>::value
                                   && std::is_object<Object>::value
                                   && ! std::is_class<Object>::value
                                   && std::is_member_function_pointer<Function>::value
                                   && ! std::is_function<typename std::remove_pointer<Function>::type>::value
                                   && ! std::is_function<Function>::value
                                   && ! std::is_pointer<Function>::value
                                   && std::is_object<Function>::value
                                   && ! std::is_class<Function>::value
                                   , std::nullptr_t >::type = nullptr
           , typename... Args>
    void addCallback ( const FString& cb_signal
                     , Object&&       cb_instance
                     , Function&&     cb_member
                     , Args&&...      args)
    {
      // Add a member function pointer as callback

      Object instance = cb_instance;
      auto member = reinterpret_cast<void*>(std::addressof(cb_member));
      auto fn = std::bind ( std::forward<Function>(cb_member)
                          , std::forward<Object>(cb_instance)
                          , std::forward<Args>(args)... );
      FCallbackData obj{ cb_signal, instance, member, fn };
      callback_objects.push_back(obj);
    }

    template<typename Object
           , typename Function
           , typename std::enable_if< ! std::is_member_function_pointer<Object>::value
                                   && ! std::is_function<typename std::remove_pointer<Object>::type>::value
                                   && ! std::is_function<Object>::value
                                   && std::is_pointer<Object>::value
                                   && std::is_object<Object>::value
                                   && ! std::is_class<Object>::value
                                   && ! std::is_member_function_pointer<Function>::value
                                   && ! std::is_function<typename std::remove_pointer<Function>::type>::value
                                   && ! std::is_function<Function>::value
                                   && ! std::is_pointer<Function>::value
                                   && std::is_object<Function>::value
                                   && std::is_class<Function>::value
                                   , std::nullptr_t >::type = nullptr
           , typename... Args>
    void addCallback ( const FString& cb_signal
                     , Object&&       cb_instance
                     , Function&&     cb_function
                     , Args&&...      args)
    {
      // Add a function object to an instance as callback

      auto fn = std::bind (std::forward<Function>(cb_function), std::forward<Args>(args)...);
      FCallbackData obj{ cb_signal, cb_instance, nullptr, fn };
      callback_objects.push_back(obj);
    }

    template<typename Function
           , typename std::enable_if< ! std::is_member_function_pointer<Function>::value
                                   && ! std::is_function<typename std::remove_pointer<Function>::type>::value
                                   && ! std::is_function<Function>::value
                                   && ! std::is_pointer<Function>::value
                                   && std::is_object<Function>::value
                                   && std::is_class<Function>::value
                                   , std::nullptr_t >::type = nullptr
           , typename... Args>
    void addCallback ( const FString& cb_signal
                     , Function&&     cb_function
                     , Args&&...      args)
    {
      // Add a function object as callback

      auto fn = std::bind ( std::forward<Function>(cb_function)
                          , std::forward<Args>(args)... );
      FCallbackData obj{ cb_signal, nullptr, nullptr, fn };
      callback_objects.push_back(obj);
    }

    template<typename Function
           , typename std::enable_if< ! std::is_member_function_pointer<Function>::value
                                   && std::is_function<typename std::remove_pointer<Function>::type>::value
                                   && std::is_function<Function>::value
                                   && ! std::is_pointer<Function>::value
                                   && ! std::is_object<Function>::value
                                   && ! std::is_class<Function>::value
                                   , std::nullptr_t >::type = nullptr
           , typename... Args>
    void addCallback ( const FString& cb_signal
                     , Function&      cb_function
                     , Args&&...      args)
    {
      // Add a function as callback

      auto ptr = reinterpret_cast<void*>(&cb_function);
      auto fn = std::bind (cb_function, std::forward<Args>(args)...);
      FCallbackData obj{ cb_signal, nullptr, ptr, fn };
      callback_objects.push_back(obj);
    }

    template<typename Function
           , typename std::enable_if< ! std::is_member_function_pointer<Function>::value
                                   && std::is_function<typename std::remove_pointer<Function>::type>::value
                                   && ! std::is_function<Function>::value
                                   && std::is_pointer<Function>::value
                                   && std::is_object<Function>::value
                                   && ! std::is_class<Function>::value
                                   , std::nullptr_t >::type = nullptr
               , typename... Args>
    void addCallback ( const FString& cb_signal
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

    template<typename Function
           , typename std::enable_if< ! std::is_member_function_pointer<Function>::value
                                   && ! std::is_function<typename std::remove_pointer<Function>::type>::value
                                   && ! std::is_function<Function>::value
                                   && ! std::is_pointer<Function>::value
                                   && std::is_object<Function>::value
                                   && std::is_class<Function>::value
                                   , std::nullptr_t >::type = nullptr
           , typename... Args>
    void addCallback ( const FString& cb_signal
                     , Function&      cb_function
                     , Args&&...      args)
    {
      // Add a non-union class type as callback

      auto fn = std::bind (cb_function, std::forward<Args>(args)...);
      FCallbackData obj{ cb_signal, nullptr, nullptr, fn };
      callback_objects.push_back(obj);
    }

    template<typename Object
           , typename std::enable_if< ! std::is_member_function_pointer<Object>::value
                                   && ! std::is_function<typename std::remove_pointer<Object>::type>::value
                                   && ! std::is_function<Object>::value
                                   && std::is_pointer<Object>::value
                                   && std::is_object<Object>::value
                                   && ! std::is_class<Object>::value
                                   , std::nullptr_t >::type = nullptr >
    void delCallback (Object&& cb_instance)
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

    void delCallback (const FString& cb_signal)
    {
      // Deletes entries with the given signal from the callback list

      if ( callback_objects.empty() )
        return;

      auto iter = callback_objects.begin();

      while ( iter != callback_objects.end() )
      {
        if ( iter->cb_signal == cb_signal )
          iter = callback_objects.erase(iter);
        else
          ++iter;
      }
    }

    template<typename Object
           , typename std::enable_if< ! std::is_member_function_pointer<Object>::value
                                   && ! std::is_function<typename std::remove_pointer<Object>::type>::value
                                   && ! std::is_function<Object>::value
                                   && std::is_pointer<Object>::value
                                   && std::is_object<Object>::value
                                   && ! std::is_class<Object>::value
                                   , std::nullptr_t >::type = nullptr >
    void delCallback (const FString& cb_signal, Object&& cb_instance)
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

    template<typename FunctionPtr
           , typename std::enable_if< ! std::is_member_function_pointer<FunctionPtr>::value
                                   && std::is_function<typename std::remove_pointer<FunctionPtr>::type>::value
                                   && ! std::is_function<FunctionPtr>::value
                                   && std::is_pointer<FunctionPtr>::value
                                   && std::is_object<FunctionPtr>::value
                                   && ! std::is_class<FunctionPtr>::value
                                   , std::nullptr_t >::type = nullptr >
    void delCallback (FunctionPtr&& cb_func_ptr)
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

    template<typename Function
           , typename std::enable_if< ! std::is_member_function_pointer<Function>::value
                                   && std::is_function<typename std::remove_pointer<Function>::type>::value
                                   && std::is_function<Function>::value
                                   && ! std::is_pointer<Function>::value
                                   && ! std::is_object<Function>::value
                                   && ! std::is_class<Function>::value
                                   , std::nullptr_t >::type = nullptr >
    void delCallback (Function& cb_function)
    {
      // Deletes entries with the given function from the callback list

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

    void delCallback()
    {
      // Delete all callbacks from this widget

      callback_objects.clear();  // function pointer
    }


    void emitCallback (const FString& emit_signal) const
    {
      // Initiate callback for the given signal

      if ( callback_objects.empty() )
        return;

      for (auto&& cback : callback_objects)
      {
        if ( cback.cb_signal == emit_signal )
        {
          // Calling the stored function pointer
          cback.cb_function();
        }
      }
    }

  private:
    // Typedefs
    typedef std::vector<FCallbackData>  FCallbackObjects;

    // Data members
    FCallbackObjects  callback_objects{};
};

}  // namespace finalcut

#endif  // FCALLBACK_H
