/***********************************************************************
* fsize.h - Height and width of a two-dimensional surface              *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2025 Markus Gans                                      *
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
 * ▕▔▔▔▔▔▔▔▏
 * ▕ FSize ▏
 * ▕▁▁▁▁▁▁▁▏
 */

#ifndef FSIZE_H
#define FSIZE_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <iostream>
#include <limits>
#include <utility>

#include "final/ftypes.h"
#include "final/util/fstring.h"

namespace finalcut
{

// class forward declaration
class FPoint;

//----------------------------------------------------------------------
// class FSize
//----------------------------------------------------------------------

class FSize
{
  public:
    // Using-declarations
    using size_type       = std::size_t;
    using size_reference  = size_type&;
    using area_type       = std::size_t;

    // Constructors
    constexpr FSize () noexcept = default;

    template<typename WidthT, typename HeightT
           , typename = std::enable_if_t< std::is_arithmetic<WidthT>::value
                                       && std::is_arithmetic<HeightT>::value >>
    constexpr FSize (WidthT, HeightT) noexcept;

    // Copy constructor
    constexpr FSize (const FSize&) noexcept = default;

    // Move constructor
    constexpr FSize (FSize&&) noexcept = default;

    // Destructor
    ~FSize() noexcept = default;

    // Copy assignment operator (=)
    constexpr auto operator = (const FSize&) noexcept -> FSize& = default;

    // Move assignment operator (=)
    constexpr auto operator = (FSize&&) noexcept -> FSize& = default;

    // Overloaded operators
    auto operator += (const FSize&) noexcept -> FSize&;
    auto operator -= (const FSize&) noexcept -> FSize&;

    // Accessors
    auto getClassName() const -> FString;
    constexpr auto getWidth() const noexcept -> size_type;
    constexpr auto getHeight() const noexcept -> size_type;
    constexpr auto getArea() const noexcept -> area_type;

    // Mutators
    constexpr void setWidth (size_type) noexcept;
    constexpr void setHeight (size_type) noexcept;
    constexpr void setSize (const FSize&) noexcept;
    constexpr void setSize (size_type, size_type) noexcept;

    // Inquiry
    constexpr auto isEmpty() const noexcept -> bool;

    // Side references
    constexpr auto width_ref() & noexcept -> size_reference;
    constexpr auto height_ref() & noexcept -> size_reference;

    // Methods
    void scaleBy (int, int) noexcept;
    void scaleBy (const FPoint&) noexcept;
    constexpr void swap (FSize&) noexcept;

  private:
    // Data members
    size_type width{0};
    size_type height{0};

    // Friend operator functions
    friend constexpr auto operator < (const FSize& s1, const FSize& s2) -> bool
    {
      return s1.width < s2.width && s1.height < s2.height;
    }

    friend constexpr auto operator <= (const FSize& s1, const FSize& s2) -> bool
    {
      return s1.width <= s2.width && s1.height <= s2.height;
    }

    friend constexpr auto operator == (const FSize& s1, const FSize& s2) -> bool
    {
      return s1.width == s2.width && s1.height == s2.height;
    }

    friend constexpr auto operator != (const FSize& s1, const FSize& s2) -> bool
    {
      return s1.width != s2.width || s1.height != s2.height;
    }

    friend constexpr auto operator >= (const FSize& s1, const FSize& s2) -> bool
    {
      return s1.width >= s2.width && s1.height >= s2.height;
    }

    friend constexpr auto operator > (const FSize& s1, const FSize& s2) -> bool
    {
      return s1.width > s2.width && s1.height > s2.height;
    }

    friend constexpr auto operator + (const FSize& s1, const FSize& s2) -> FSize
    {
      constexpr size_type max = std::numeric_limits<size_type>::max();
      const size_type w = ( s1.width < max - s2.width) ? s1.width + s2.width : max;
      const size_type h = ( s1.height < max - s2.height) ? s1.height + s2.height : max;
      return { FSize{w, h} };
    }

    friend constexpr auto operator - (const FSize& s1, const FSize& s2) -> FSize
    {
      const size_type w = ( s1.width >= s2.width ) ? s1.width - s2.width : 0;
      const size_type h = ( s1.height >= s2.height ) ? s1.height - s2.height : 0;
      return { FSize{w, h} };
    }

    friend inline auto operator << (std::ostream& outstr, const FSize& s) -> std::ostream&
    {
      outstr << s.width << " " << s.height;
      return outstr;
    }

    friend inline auto operator >> (std::istream& instr, FSize& s) -> std::istream&
    {
      size_type w{};
      size_type h{};

      if ( instr >> w >> h )
        s.setSize (w, h);

      return instr;
    }
};

// FSize inline functions
//----------------------------------------------------------------------
template<typename WidthT, typename HeightT, typename>
constexpr FSize::FSize (WidthT w, HeightT h) noexcept
  : width{static_cast<size_type>(w > 0 ? w : 0)}
  , height{static_cast<size_type>(h > 0 ? h : 0)}
{ }

//----------------------------------------------------------------------
inline auto FSize::getClassName() const -> FString
{ return "FSize"; }

//----------------------------------------------------------------------
constexpr auto FSize::getWidth() const noexcept -> size_type
{ return width; }

//----------------------------------------------------------------------
constexpr auto FSize::getHeight() const noexcept -> size_type
{ return height; }

//----------------------------------------------------------------------
constexpr auto FSize::getArea() const noexcept -> area_type
{ return width * height; }

//----------------------------------------------------------------------
constexpr void FSize::setWidth (size_type w) noexcept
{ width = w; }

//----------------------------------------------------------------------
constexpr void FSize::setHeight (size_type h) noexcept
{ height = h; }

//----------------------------------------------------------------------
constexpr void FSize::setSize (const FSize& s) noexcept
{
  width = s.width;
  height = s.height;
}

//----------------------------------------------------------------------
constexpr void FSize::setSize (size_type w, size_type h) noexcept
{
  width = w;
  height = h;
}

//----------------------------------------------------------------------
constexpr auto FSize::isEmpty() const noexcept -> bool
{ return width == 0 && height == 0; }

//----------------------------------------------------------------------
constexpr auto FSize::width_ref() & noexcept -> size_reference
{ return width; }

//----------------------------------------------------------------------
constexpr auto FSize::height_ref() & noexcept -> size_reference
{ return height; }

//----------------------------------------------------------------------
constexpr void FSize::swap (FSize& s) noexcept
{
  size_type tmp_width{width};
  size_type tmp_height{height};
  width = s.width;
  height = s.height;
  s.width = tmp_width;
  s.height = tmp_height;
}

}  // namespace finalcut

#endif  // FSIZE_H
