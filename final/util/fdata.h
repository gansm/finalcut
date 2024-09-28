/***********************************************************************
* fdata.h - A general-purpose data wrapper                             *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2020-2023 Markus Gans                                      *
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

#include "final/util/fstring.h"

namespace finalcut
{

template <typename T>
class FData;  // Class forward declaration

// non-member functions
//----------------------------------------------------------------------
namespace internal
{

// Enumerations
enum class IsArray { No, Yes };
enum class IsFunction { No, Yes };

// Define the clean condition
template <typename T
        , IsArray is_array = std::is_array<T>::value
                           ? IsArray::Yes
                           : IsArray::No
        , IsFunction is_function = std::is_function<T>::value
                                 ? IsFunction::Yes
                                 : IsFunction::No>
struct cleanCondition;

//----------------------------------------------------------------------
template <typename T>
struct cleanCondition<T, IsArray::No, IsFunction::No>
{
  // Leave the type untouched
  using type = T;
};

//----------------------------------------------------------------------
template <typename T>
struct cleanCondition<T, IsArray::Yes, IsFunction::No>
{
  // Array to pointer
  using type = std::remove_extent_t<T>*;
};

//----------------------------------------------------------------------
template <typename T>
struct cleanCondition<T, IsArray::No, IsFunction::Yes>
{
  // Add pointer to function
  using type = std::add_pointer_t<T>;
};

//----------------------------------------------------------------------
template<typename T>
using cleanCondition_t = typename cleanCondition<T>::type;

}  // namespace internal

//----------------------------------------------------------------------
template <typename T>
class cleanFData
{
  private:
    using remove_ref = std::remove_reference_t<T>;

  public:
    // Similar to std::decay, but keeps const and volatile
    using type = internal::cleanCondition_t<remove_ref>;
};

//----------------------------------------------------------------------
template <typename T>
using clean_fdata_t = typename cleanFData<T>::type;

//----------------------------------------------------------------------
template <typename T>
constexpr auto makeFData (T&& data) -> FData<clean_fdata_t<T>>*
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
    virtual auto getClassName() const -> FString
    {
      return "FDataAccess";
    }

    template<typename T>
    constexpr auto get() -> clean_fdata_t<T>&
    {
      return static_cast<FData<clean_fdata_t<T>>&>(*this).get();
    }

    // Mutator
    template <typename T
            , typename V>
    constexpr void set (V&& data)
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
    using T_nocv = std::remove_cv_t<T>;

    // Constructors
    explicit FData (T& v)  // constructor
      : value_ref{v}
    { }

    explicit FData (T&& v)  // constructor
      : value{std::move(v)}
      , value_ref{value}
    { }

    ~FData() noexcept override = default;  // Destructor

    FData (const FData& d)  // Copy constructor
      : value{d.value}
      , value_ref{d.isInitializedCopy() ? std::ref(value) : d.value_ref}
    { }

    FData (FData&& d) noexcept  // Move constructor
      : value{std::move(d.value)}
      , value_ref{d.isInitializedCopy() ? std::ref(value) : std::move(d.value_ref)}
    { }

    // Overloaded operators
    auto operator = (const FData& d) -> FData&  // Copy assignment operator (=)
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

    auto operator = (FData&& d) noexcept -> FData&  // Move assignment operator (=)
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

    constexpr auto operator () () const -> T
    {
      return value_ref;
    }

    constexpr explicit operator T () const
    {
      return value_ref;
    }

    constexpr auto operator << (const T& v) -> FData&
    {
      value_ref.get() = v;
      return *this;
    }

    // Accessors
    auto getClassName() const -> FString override
    {
      return "FData";
    }

    constexpr auto get() const -> T&
    {
      return value_ref;
    }

    // Mutator
    constexpr void set (const T& v)
    {
      value_ref.get() = v;
    }

    // Inquiries
    constexpr auto isInitializedCopy() const -> bool
    {
      const auto* v = reinterpret_cast<void*>(const_cast<T_nocv*>(&value));
      const auto* r = reinterpret_cast<void*>(const_cast<T_nocv*>(&value_ref.get()));
      return v == r;
    }

    constexpr auto isInitializedReference() const -> bool
    {
      return ! isInitializedCopy();
    }

    // Friend Non-member operator functions
    constexpr friend auto operator << (std::ostream &os, const FData& data) -> std::ostream&
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
