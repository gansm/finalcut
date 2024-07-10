/***********************************************************************
* fsize.h - Height and width of a two-dimensional surface              *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2024 Markus Gans                                      *
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
    // Constructors
    FSize () noexcept = default;
    template<typename WidthT, typename HeightT>
    FSize (WidthT, HeightT) noexcept;

    // Overloaded operators
    auto operator += (const FSize&) -> FSize&;
    auto operator -= (const FSize&) -> FSize&;

    // Accessors
    auto getClassName() const -> FString;
    auto getWidth() const noexcept -> std::size_t;
    auto getHeight() const noexcept -> std::size_t;
    auto getArea() const noexcept -> std::size_t;
    void setWidth (std::size_t) noexcept;
    void setHeight (std::size_t) noexcept;
    void setSize (const FSize&) noexcept;
    void setSize (std::size_t, std::size_t) noexcept;

    // Inquiry
    auto isEmpty() const noexcept -> bool;

    // Side references
    auto width_ref() & noexcept -> std::size_t&;
    auto height_ref() & noexcept -> std::size_t&;

    // Methods
    void scaleBy (int, int) noexcept;
    void scaleBy (const FPoint&) noexcept;

  private:
    // Data members
    std::size_t width{0};
    std::size_t height{0};

    // Friend operator functions
    friend inline auto operator < (const FSize& s1, const FSize& s2) -> bool
    {
      return s1.width < s2.width && s1.height < s2.height;
    }

    friend inline auto operator <= (const FSize& s1, const FSize& s2) -> bool
    {
      return s1.width <= s2.width && s1.height <= s2.height;
    }

    friend inline auto operator == (const FSize& s1, const FSize& s2) -> bool
    {
      return s1.width == s2.width && s1.height == s2.height;
    }

    friend inline auto operator != (const FSize& s1, const FSize& s2) -> bool
    {
      return s1.width != s2.width || s1.height != s2.height;
    }

    friend inline auto operator >= (const FSize& s1, const FSize& s2) -> bool
    {
      return s1.width >= s2.width && s1.height >= s2.height;
    }

    friend inline auto operator > (const FSize& s1, const FSize& s2) -> bool
    {
      return s1.width > s2.width && s1.height > s2.height;
    }

    friend inline auto operator + (const FSize& s1, const FSize& s2) -> FSize
    {
      constexpr std::size_t max = std::numeric_limits<std::size_t>::max();
      const std::size_t w = ( s1.width < max - s2.width) ? s1.width + s2.width : max;
      const std::size_t h = ( s1.height < max - s2.height) ? s1.height + s2.height : max;
      return { FSize{w, h} };
    }

    friend inline auto operator - (const FSize& s1, const FSize& s2) -> FSize
    {
      const std::size_t w = ( s1.width >= s2.width ) ? s1.width - s2.width : 0;
      const std::size_t h = ( s1.height >= s2.height ) ? s1.height - s2.height : 0;
      return { FSize{w, h} };
    }

    friend inline auto operator << (std::ostream& outstr, const FSize& s) -> std::ostream&
    {
      outstr << s.width << " " << s.height;
      return outstr;
    }

    friend inline auto operator >> (std::istream& instr, FSize& s) -> std::istream&
    {
      std::size_t w;
      std::size_t h;
      instr >> w;
      instr >> h;
      s.setSize (w, h);
      return instr;
    }
};

// FSize inline functions
//----------------------------------------------------------------------
template<typename WidthT, typename HeightT>
inline FSize::FSize (WidthT w, HeightT h) noexcept
  : width{static_cast<std::size_t>(w > 0 ? w : 0)}
  , height{static_cast<std::size_t>(h > 0 ? h : 0)}
{ }

//----------------------------------------------------------------------
inline auto FSize::getClassName() const -> FString
{ return "FSize"; }

//----------------------------------------------------------------------
inline auto FSize::getWidth() const noexcept -> std::size_t
{ return width; }

//----------------------------------------------------------------------
inline auto FSize::getHeight() const noexcept -> std::size_t
{ return height; }

//----------------------------------------------------------------------
inline auto FSize::getArea() const noexcept -> std::size_t
{ return width * height; }

//----------------------------------------------------------------------
inline void FSize::setWidth (std::size_t w) noexcept
{ width = w; }

//----------------------------------------------------------------------
inline void FSize::setHeight (std::size_t h) noexcept
{ height = h; }

//----------------------------------------------------------------------
inline void FSize::setSize (const FSize& s) noexcept
{
  width = s.width;
  height = s.height;
}

//----------------------------------------------------------------------
inline void FSize::setSize (std::size_t w, std::size_t h) noexcept
{
  width = w;
  height = h;
}

//----------------------------------------------------------------------
inline auto FSize::isEmpty() const noexcept -> bool
{ return width == 0 && height == 0; }

//----------------------------------------------------------------------
inline auto FSize::width_ref() & noexcept -> std::size_t&
{ return width; }

//----------------------------------------------------------------------
inline auto FSize::height_ref() & noexcept -> std::size_t&
{ return height; }

}  // namespace finalcut

#endif  // FSIZE_H
