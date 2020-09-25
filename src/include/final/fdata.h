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

//----------------------------------------------------------------------
// class FDataAccess
//----------------------------------------------------------------------

template <typename T>
class FData;  // Class forward declaration

class FDataAccess
{
  public:
    // Constructor
    FDataAccess();

    // Destructor
    virtual ~FDataAccess();

    // Accessors
    virtual const FString getClassName() const
    {
      return "FDataAccess";
    }

    template <typename T>
    const T& get() const
    {
      return static_cast<const FData<T>&>(*this).get();
    }

    // Mutator
    template <typename T
            , typename V>
    void set (const V& v)
    {
      static_cast<FData<T>&>(*this).set(v);
    }
};


//----------------------------------------------------------------------
// class FData
//----------------------------------------------------------------------

template <typename T>
class FData : public FDataAccess
{
  public:
    // Constructors
    explicit FData (T& v)  // constructor
      : value_ref{v}
    { }

    explicit FData (T&& v)  // constructor
      : value{std::move(v)}
      , value_ref{value}
    { }

    // Destructor
    ~FData() override
    { }

    FData (const FData& d)  // Copy constructor
      : value{d.value}
      , value_ref{value}
    { }

    FData (FData&& d) noexcept  // Move constructor
      : value{std::move(d.value)}
      , value_ref{value}
    { }

    // Overloaded operators
    FData& operator = (const FData& d)  // Copy assignment operator (=)
    {
      value = d.value;
      value_ref = value;
      return *this;
    }

    FData& operator = (FData&& d) noexcept  // Move assignment operator (=)
    {
      value = std::move(d.value);
      value_ref = value;
      return *this;
    }

    T operator () () const
    {
      return value_ref;
    }

    template <typename... Args>
    T operator () (Args... args) const
    {
      return value_ref(args...);
    }

    explicit operator T () const
    {
      return value_ref;
    }

    FData& operator << (const T& v)
    {
      value_ref = v;
      return *this;
    }

    // Accessors
    const FString getClassName() const override
    {
      return "FData";
    }

    T& get()
    {
      return value_ref;
    }

    // Mutator
    void set (const T& v)
    {
      value_ref = v;
    }

    // Inquiries
    bool isInitializedCopy()
    {
      return bool( &value == &value_ref );
    }

    bool isInitializedReference()
    {
      return ! isInitializedCopy();
    }

    // Friend Non-member operator functions
    friend std::ostream& operator << (std::ostream &os, const FData& data)
    {
      os << data.value_ref;
      return os;
    }

  private:
    // Data members
    T value{};
    T& value_ref;
};

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
  typedef T type;
};

//----------------------------------------------------------------------
template <typename T>
struct cleanCondition<T, true, false>
{
  // Array to pointer
  typedef typename std::remove_extent<T>::type* type;
};

//----------------------------------------------------------------------
template <typename T>
struct cleanCondition<T, false, true>
{
  // Add pointer to function
  typedef typename std::add_pointer<T>::type type;
};

}  // namespace internal

//----------------------------------------------------------------------
template <typename T>
class cleanFData
{
  private:
    typedef typename std::remove_reference<T>::type remove_ref;

  public:
    // Similar to std::decay, but keeps const and volatile
    typedef typename internal::cleanCondition<remove_ref>::type type;
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

}  // namespace finalcut

#endif  // FDATA_H
