/***********************************************************************
* fdata.h - A general-purpose data wrapper                             *
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

/*  Inheritance diagram
 *  ═══════════════════
 *
 *      ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *      ▕ FDataAccess ▏
 *      ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *             ▲
 *             │
 *         ▕▔▔▔▔▔▔▔▏
 *         ▕ FData ▏
 *         ▕▁▁▁▁▁▁▁▏
 */

#ifndef FDATA_H
#define FDATA_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <utility>

#include "final/fstring.h"

namespace finalcut
{

template <typename T>
class FData;  // Class forward declaration

// non-member functions
//----------------------------------------------------------------------
namespace internal
{

template <typename T
        , bool isArray = std::is_array<T>::value
        , bool isFunction = std::is_function<T>::value>
struct cleanCondition;

//----------------------------------------------------------------------
template <typename T>
struct cleanCondition<T, false, false>
{
  // Leave the type untouched
  using type = T;
};

//----------------------------------------------------------------------
template <typename T>
struct cleanCondition<T, true, false>
{
  // Array to pointer
  using type = typename std::remove_extent<T>::type*;
};

//----------------------------------------------------------------------
template <typename T>
struct cleanCondition<T, false, true>
{
  // Add pointer to function
  using type = typename std::add_pointer<T>::type;
};

}  // namespace internal

//----------------------------------------------------------------------
template <typename T>
class cleanFData
{
  private:
    using remove_ref = typename std::remove_reference<T>::type;

  public:
    // Similar to std::decay, but keeps const and volatile
    using type = typename internal::cleanCondition<remove_ref>::type;
};

//----------------------------------------------------------------------
template <typename T>
using clean_fdata_t = typename cleanFData<T>::type;

//----------------------------------------------------------------------
template <typename T>
constexpr FData<clean_fdata_t<T>>* makeFData (T&& data)
{
  return new FData<clean_fdata_t<T>>(std::forward<T>(data));
}


//----------------------------------------------------------------------
// class FDataAccess
//----------------------------------------------------------------------

class FDataAccess
{
  public:
    // Constructor
    FDataAccess() = default;

    // Destructor
    virtual ~FDataAccess() noexcept;

    // Accessors
    virtual FString getClassName() const
    {
      return "FDataAccess";
    }

    template<typename T>
    clean_fdata_t<T>& get()
    {
      return static_cast<FData<clean_fdata_t<T>>&>(*this).get();
    }

    // Mutator
    template <typename T
            , typename V>
    void set (V&& data)
    {
      static_cast<FData<T>&>(*this).set(std::forward<V>(data));
    }
};


//----------------------------------------------------------------------
// class FData
//----------------------------------------------------------------------

template <typename T>
class FData : public FDataAccess
{
  public:
    using T_nocv = typename std::remove_cv<T>::type;

    // Constructors
    explicit FData (T& v)  // constructor
      : value_ref{v}
    { }

    explicit FData (T&& v)  // constructor
      : value{std::move(v)}
      , value_ref{value}
    { }

    // Destructor
    ~FData() noexcept override = default;

    FData (const FData& d)  // Copy constructor
      : value{d.value}
      , value_ref{d.isInitializedCopy() ? std::ref(value) : d.value_ref}
    { }

    FData (FData&& d) noexcept  // Move constructor
      : value{std::move(d.value)}
      , value_ref{d.isInitializedCopy() ? std::ref(value) : std::move(d.value_ref)}
    { }

    // Overloaded operators
    FData& operator = (const FData& d)  // Copy assignment operator (=)
    {
      if ( &d != this )
      {
        value = d.value;

        if ( d.isInitializedCopy() )
          value_ref = value;
        else
          value_ref = d.value_ref;
      }

      return *this;
    }

    FData& operator = (FData&& d) noexcept  // Move assignment operator (=)
    {
      if ( &d != this )
      {
        value = std::move(d.value);

        if ( d.isInitializedCopy() )
          value_ref = value;
        else
          value_ref = std::move(d.value_ref);
      }

      return *this;
    }

    T operator () () const
    {
      return value_ref;
    }

    explicit operator T () const
    {
      return value_ref;
    }

    FData& operator << (const T& v)
    {
      value_ref.get() = v;
      return *this;
    }

    // Accessors
    FString getClassName() const override
    {
      return "FData";
    }

    T& get() const
    {
      return value_ref;
    }

    // Mutator
    void set (const T& v)
    {
      value_ref.get() = v;
    }

    // Inquiries
    bool isInitializedCopy() const
    {
      const auto& v = reinterpret_cast<void*>(const_cast<T_nocv*>(&value));
      const auto& r = reinterpret_cast<void*>(const_cast<T_nocv*>(&value_ref.get()));
      return bool( v == r );
    }

    bool isInitializedReference() const
    {
      return ! isInitializedCopy();
    }

    // Friend Non-member operator functions
    friend std::ostream& operator << (std::ostream &os, const FData& data)
    {
      os << data.value_ref.get();
      return os;
    }

  private:
    // Data members
    T value{};
    std::reference_wrapper<T> value_ref;
};

}  // namespace finalcut

#endif  // FDATA_H
