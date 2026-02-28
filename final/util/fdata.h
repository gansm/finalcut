/***********************************************************************
* fdata.h - A general-purpose data wrapper                             *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2020-2026 Markus Gans                                      *
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

#include <functional>
#include <memory>
#include <stdexcept>
#include <type_traits>
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

// Define the clean condition
template <typename T>
using cleanCondition_t = \
    std::conditional_t<std::is_array<T>::value
                     , std::remove_extent_t<T>*
                     , std::conditional_t<std::is_function<T>::value
                                        , std::add_pointer_t<T>
                                        , T>>;

// Type trait helpers for better error messages
//----------------------------------------------------------------------
template <typename T>
struct is_reference_wrapper_helper : std::false_type
{ };

template <typename T>
struct is_reference_wrapper_helper<std::reference_wrapper<T>> : std::true_type
{ };

template <typename T>
struct is_reference_wrapper : is_reference_wrapper_helper<std::decay_t<T>>
{ };

template<typename T>
constexpr bool is_reference_wrapper_v = is_reference_wrapper<T>::value;

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

    // Static assertions for better error messages
    static_assert ( ! std::is_void<remove_ref>::value
                  , "FData cannot store void types" );
    static_assert ( ! internal::is_reference_wrapper_v<remove_ref>
                  , "FData cannot store reference_wrapper types directly" );
};

//----------------------------------------------------------------------
template <typename T>
using clean_fdata_t = typename cleanFData<T>::type;

//----------------------------------------------------------------------
template <typename T>
inline auto makeFData (T&& data) -> std::unique_ptr<FData<clean_fdata_t<T>>>
{
  using CleanT = clean_fdata_t<T>;
  static_assert ( std::is_constructible<CleanT, T&&>::value
                , "Cannot construct FData with the provided type" );

  return std::make_unique<FData<clean_fdata_t<T>>>(std::forward<T>(data));
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
    constexpr auto get() noexcept -> clean_fdata_t<T>&
    {
      static_assert ( ! std::is_void<T>::value
                    , "Cannot get void type from FDataAccess" );

      return static_cast<FData<clean_fdata_t<T>>*>(this)->get();
    }

    template<typename T>
    constexpr auto get() const noexcept -> const clean_fdata_t<T>&
    {
      static_assert ( ! std::is_void<T>::value
                    , "Cannot get void type from FDataAccess" );

      return static_cast<const FData<clean_fdata_t<T>>*>(this)->get();
    }

    // Mutator
    template <typename T
            , typename V>
    constexpr void set (V&& data) noexcept(noexcept(std::declval<FData<T>>().set(std::forward<V>(data))))
    {
      static_assert ( std::is_convertible<V&&, T>::value
                    , "Type V must be convertible to type T" );

      static_cast<FData<T>*>(this)->set(std::forward<V>(data));
    }
};


//----------------------------------------------------------------------
// class FData
//----------------------------------------------------------------------

template <typename T>
class FData final : public FDataAccess
{
  public:
    // Using-declarations
    using value_type      = T;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using T_nocv          = std::remove_cv_t<value_type>;

    // Noexcept trait shortcuts
    static constexpr bool nothrow_copy_ctor_v = \
        std::is_nothrow_copy_constructible<value_type>::value;

    static constexpr bool nothrow_move_ctor_v = \
        std::is_nothrow_move_constructible<value_type>::value;

    static constexpr bool nothrow_copy_asgn_v = \
        std::is_nothrow_copy_assignable<value_type>::value;

    static constexpr bool nothrow_move_asgn_v = \
        std::is_nothrow_move_assignable<value_type>::value;

    // Static assertions for better error messages
    static_assert ( ! std::is_void<value_type>::value
                  , "FData cannot store void types") ;
    static_assert ( ! std::is_reference<value_type>::value
                  , "FData cannot store reference types directly" );

    // Constructors
    explicit FData (value_type& v) noexcept  // constructor
      : value_ref{v}
    { }

    explicit FData (value_type&& v) noexcept(nothrow_move_ctor_v)  // constructor
      : value{std::move(v)}
      , value_ref{value}
    { }

    ~FData() noexcept final = default;  // Destructor

    FData (const FData& d) noexcept(nothrow_copy_ctor_v)  // Copy constructor
      : FDataAccess{d}
      , value{d.value}
      , value_ref{d.isInitializedCopy() ? std::ref(value) : d.value_ref}
    { }

    FData (FData&& d) noexcept  // Move constructor
      : FDataAccess{std::move(d)}
      , value{std::move(d.value)}
      , value_ref{d.isInitializedCopy() ? std::ref(value) : d.value_ref}
    { }

    // Copy assignment operator (=)
    auto operator = (const FData& d) noexcept(nothrow_copy_asgn_v) -> FData&
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

    // Move assignment operator (=)
    auto operator = (FData&& d) noexcept -> FData&
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

    // Function call operator
    constexpr auto operator () () const noexcept(nothrow_copy_ctor_v) -> value_type
    {
      return value_ref;
    }

    constexpr explicit operator value_type () const noexcept(nothrow_copy_ctor_v)
    {
      return value_ref;
    }

    constexpr auto operator << (const_reference v) noexcept(nothrow_copy_asgn_v) -> FData&
    {
      value_ref.get() = v;
      return *this;
    }

    // Accessors
    auto getClassName() const -> FString final
    {
      return "FData";
    }

    constexpr auto get() const noexcept -> const_reference
    {
      return value_ref.get();
    }

    constexpr auto get() noexcept -> reference
    {
      return value_ref.get();
    }

    // Mutator
    constexpr void set (const value_type& v) noexcept(nothrow_copy_asgn_v)
    {
      value_ref.get() = v;
    }

    constexpr void set (value_type&& v) noexcept(nothrow_move_asgn_v)
    {
      value_ref.get() = std::move(v);
    }

    // Inquiries
    constexpr auto isInitializedCopy() const noexcept -> bool
    {
      return std::addressof(value) == std::addressof(value_ref.get());
    }

    constexpr auto isInitializedReference() const noexcept -> bool
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
    value_type value{};
    std::reference_wrapper<value_type> value_ref;
};

}  // namespace finalcut

#endif  // FDATA_H
