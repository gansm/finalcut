/***********************************************************************
* fpoint.h - Point with an x and y coordinate                          *
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
 * ▕▔▔▔▔▔▔▔▔▏
 * ▕ FPoint ▏
 * ▕▁▁▁▁▁▁▁▁▏
 */

#ifndef FPOINT_H
#define FPOINT_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <iostream>
#include <utility>

#include "final/util/fstring.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FPoint
//----------------------------------------------------------------------

class FPoint
{
  public:
    // Using-declarations
    using coordinate_type      = int;
    using coordinate_reference = coordinate_type&;

    // Constructors
    constexpr FPoint () noexcept = default;
    constexpr FPoint (coordinate_type, coordinate_type) noexcept;

    // Copy constructor
    constexpr FPoint (const FPoint&) noexcept = default;

    // Move constructor
    constexpr FPoint (FPoint&&) noexcept = default;

    // Destructor
    ~FPoint() noexcept = default;

    // Copy assignment operator (=)
    constexpr auto operator = (const FPoint&) noexcept -> FPoint& = default;

    // Move assignment operator (=)
    constexpr auto operator = (FPoint&&) noexcept -> FPoint& = default;

    // Overloaded operators
    auto operator += (const FPoint&) -> FPoint&;
    auto operator -= (const FPoint&) -> FPoint&;

    // Accessors
    auto getClassName() const -> FString;
    constexpr auto getX() const noexcept -> coordinate_type;
    constexpr auto getY() const noexcept -> coordinate_type;

    // Mutators
    constexpr void setX (coordinate_type) noexcept;
    constexpr void setY (coordinate_type) noexcept;
    constexpr void setPoint (const FPoint&) noexcept;
    constexpr void setPoint (coordinate_type, coordinate_type) noexcept;

    // Inquiry
    constexpr auto isOrigin() const noexcept -> bool;

    // Point references
    constexpr auto x_ref() & noexcept -> coordinate_type&;
    constexpr auto y_ref() & noexcept -> coordinate_type&;

    // Methods
    constexpr void move (coordinate_type, coordinate_type) noexcept;
    constexpr void move (const FPoint&) noexcept;
    constexpr void swap (FPoint&) noexcept;

  private:
    // Data members
    coordinate_type xpos{0};
    coordinate_type ypos{0};

    // Friend operator functions
    friend constexpr auto operator == (const FPoint& p1, const FPoint& p2) -> bool
    {
      return p1.xpos == p2.xpos && p1.ypos == p2.ypos;
    }

    friend constexpr auto operator != (const FPoint& p1, const FPoint& p2) -> bool
    {
      return ! ( p1 == p2 );
    }

    friend constexpr auto operator + (const FPoint& p1, const FPoint& p2) -> FPoint
    {
      return {p1.xpos + p2.xpos, p1.ypos + p2.ypos};
    }

    friend constexpr auto operator - (const FPoint& p1, const FPoint& p2) -> FPoint
    {
      return {p1.xpos - p2.xpos, p1.ypos - p2.ypos};
    }

    friend constexpr auto operator - (const FPoint& p) -> FPoint
    {
      return {-p.xpos, -p.ypos};
    }

    friend inline auto operator << (std::ostream& outstr, const FPoint& p) -> std::ostream&
    {
      outstr << p.xpos << " " << p.ypos;
      return outstr;
    }

    friend inline auto operator >> (std::istream& instr, FPoint& p) -> std::istream&
    {
      coordinate_type x{};
      coordinate_type y{};

      if ( instr >> x >> y )
        p.setPoint (x, y);

      return instr;
    }
};


// FPoint inline functions
//----------------------------------------------------------------------
constexpr FPoint::FPoint (coordinate_type x, coordinate_type y) noexcept
  : xpos{x}
  , ypos{y}
{ }

//----------------------------------------------------------------------
inline auto FPoint::getClassName() const -> FString
{ return "FPoint"; }

//----------------------------------------------------------------------
constexpr auto FPoint::getX() const noexcept -> coordinate_type
{ return xpos; }

//----------------------------------------------------------------------
constexpr auto FPoint::getY() const noexcept -> coordinate_type
{ return ypos; }

//----------------------------------------------------------------------
constexpr void FPoint::setX (coordinate_type x) noexcept
{ xpos = x; }

//----------------------------------------------------------------------
constexpr void FPoint::setY (coordinate_type y) noexcept
{ ypos = y; }

//----------------------------------------------------------------------
constexpr void FPoint::setPoint (const FPoint& p) noexcept
{ setPoint(p.xpos, p.ypos); }

//----------------------------------------------------------------------
constexpr void FPoint::setPoint (coordinate_type x, coordinate_type y) noexcept
{
  xpos = x;
  ypos = y;
}

//----------------------------------------------------------------------
constexpr auto FPoint::isOrigin() const noexcept -> bool
{ return xpos == 0 && ypos == 0; }

//----------------------------------------------------------------------
constexpr auto FPoint::x_ref() & noexcept -> coordinate_reference
{ return xpos; }

//----------------------------------------------------------------------
constexpr auto FPoint::y_ref() & noexcept -> coordinate_reference
{ return ypos; }

//----------------------------------------------------------------------
constexpr void FPoint::move (coordinate_type dx, coordinate_type dy) noexcept
{
  xpos += dx;
  ypos += dy;
}

//----------------------------------------------------------------------
constexpr void FPoint::move (const FPoint& d) noexcept
{
  xpos += d.getX();
  ypos += d.getY();
}

//----------------------------------------------------------------------
constexpr void FPoint::swap (FPoint& p) noexcept
{
  coordinate_type tmp_x{xpos};
  coordinate_type tmp_y{ypos};
  xpos = p.xpos;
  ypos = p.ypos;
  p.xpos = tmp_x;
  p.ypos = tmp_y;
}

}  // namespace finalcut

#endif  // FPOINT_H
