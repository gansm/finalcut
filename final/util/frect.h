/***********************************************************************
* frect.h - Rectangle with position and size                           *
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
 * ▕▔▔▔▔▔▔▔▏1     *▕▔▔▔▔▔▔▔▔▏
 * ▕ FRect ▏-┬- - -▕ FPoint ▏
 * ▕▁▁▁▁▁▁▁▏ :     ▕▁▁▁▁▁▁▁▁▏
 *           :
 *           :    1▕▔▔▔▔▔▔▔▏
 *           └- - -▕ FSize ▏
 *                 ▕▁▁▁▁▁▁▁▏
 */

#ifndef FRECT_H
#define FRECT_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <algorithm>
#include <utility>

#include "final/util/fpoint.h"
#include "final/util/fsize.h"
#include "final/util/fstring.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FRect
//----------------------------------------------------------------------

class FRect
{
  public:
    // Using-declarations
    using coordinate_type      = int;
    using coordinate_reference = coordinate_type&;
    using size_type            = std::size_t;

    // Constructors
    constexpr FRect () noexcept = default;

    constexpr FRect (coordinate_type, coordinate_type, size_type, size_type) noexcept;
    constexpr FRect (const FPoint&, const FSize&) noexcept;
    constexpr FRect (const FPoint&, const FPoint&) noexcept;

    // Copy constructor
    constexpr FRect(const FRect&) noexcept = default;

    // Move constructor
    constexpr FRect(FRect&&) noexcept = default;

    // Destructor
    ~FRect() noexcept = default;

     // Copy assignment operator (=)
    constexpr auto operator = (const FRect&) noexcept -> FRect& = default;

    // Move assignment operator (=)
    constexpr auto operator = (FRect&&) noexcept -> FRect& = default;

    // Accessors
    auto getClassName() const -> FString;
    constexpr auto getX1() const noexcept -> coordinate_type;
    constexpr auto getY1() const noexcept -> coordinate_type;
    constexpr auto getX2() const noexcept -> coordinate_type;
    constexpr auto getY2() const noexcept -> coordinate_type;
    constexpr auto getX() const noexcept -> coordinate_type;
    constexpr auto getY() const noexcept -> coordinate_type;
    constexpr auto getPos() const noexcept -> FPoint;
    constexpr auto getUpperLeftPos() const noexcept -> FPoint;
    constexpr auto getUpperRightPos() const noexcept -> FPoint;
    constexpr auto getLowerLeftPos() const noexcept -> FPoint;
    constexpr auto getLowerRightPos() const noexcept -> FPoint;
    constexpr auto getWidth() const noexcept -> size_type;
    constexpr auto getHeight() const noexcept -> size_type;
    constexpr auto getSize() const noexcept -> FSize;

    // Mutators
    constexpr void setX1 (coordinate_type) noexcept;
    constexpr void setY1 (coordinate_type) noexcept;
    constexpr void setX2 (coordinate_type) noexcept;
    constexpr void setY2 (coordinate_type) noexcept;
    constexpr void setX (coordinate_type) noexcept;
    constexpr void setY (coordinate_type) noexcept;
    constexpr void setPos (coordinate_type, coordinate_type) noexcept;
    constexpr void setPos (const FPoint&) noexcept;
    constexpr void setWidth (size_type) noexcept;
    constexpr void setHeight (size_type) noexcept;
    constexpr void setSize (size_type, size_type) noexcept;
    constexpr void setSize (const FSize&) noexcept;
    constexpr void setRect (const FRect&) noexcept;
    constexpr void setRect (const FPoint&, const FSize&) noexcept;
    constexpr void setRect (coordinate_type, coordinate_type, size_type, size_type) noexcept;
    constexpr void setCoordinates (const FPoint&, const FPoint&) noexcept;
    constexpr void setCoordinates ( coordinate_type, coordinate_type
                                  , coordinate_type, coordinate_type ) noexcept;

    // Inquiry
    constexpr auto isEmpty() const -> bool;

    // Coordinate references
    constexpr auto x1_ref() & noexcept -> coordinate_reference;
    constexpr auto y1_ref() & noexcept -> coordinate_reference;
    constexpr auto x2_ref() & noexcept -> coordinate_reference;
    constexpr auto y2_ref() & noexcept -> coordinate_reference;

    // Methods
    constexpr void move (coordinate_type, coordinate_type) noexcept;
    constexpr void move (const FPoint&) noexcept;
    constexpr void scaleBy (coordinate_type, coordinate_type) noexcept;
    constexpr void scaleBy (const FPoint&) noexcept;
    constexpr auto contains (coordinate_type, coordinate_type) const noexcept -> bool;
    constexpr auto contains (const FPoint&) const noexcept -> bool;
    constexpr auto contains (const FRect&) const noexcept -> bool;
    constexpr auto overlap  (const FRect&) const noexcept -> bool;
    auto intersect (const FRect&) const noexcept -> FRect;
    auto combined (const FRect&) const noexcept -> FRect;
    constexpr void swap (FRect&) noexcept;

  private:
    // Data members
    coordinate_type X1{0};
    coordinate_type Y1{0};
    coordinate_type X2{-1};
    coordinate_type Y2{-1};

    // Friend operator functions
    friend constexpr auto operator + (const FRect& r, const FSize& s) -> FRect
    {
      return { r.X1
             , r.Y1
             , size_type(r.X2 - r.X1) + 1 + s.getWidth()
             , size_type(r.Y2 - r.Y1) + 1 + s.getHeight() };
    }

    friend constexpr auto operator - (const FRect& r, const FSize& s) -> FRect
    {
      return { r.X1
             , r.Y1
             , size_type(r.X2 - r.X1) + 1 - s.getWidth()
             , size_type(r.Y2 - r.Y1) + 1 - s.getHeight() };
    }

    friend constexpr auto operator == (const FRect& r1, const FRect& r2) -> bool
    {
      return r1.X1 == r2.X1
          && r1.Y1 == r2.Y1
          && r1.X2 == r2.X2
          && r1.Y2 == r2.Y2;
    }

    friend constexpr auto operator != (const FRect& r1, const FRect& r2) -> bool
    {
      return r1.X1 != r2.X1
          || r1.Y1 != r2.Y1
          || r1.X2 != r2.X2
          || r1.Y2 != r2.Y2;
    }

    friend inline auto operator << (std::ostream& outstr, const FRect& r) -> std::ostream&
    {
      outstr << r.X1 << " "
             << r.Y1 << " "
             << r.X2 << " "
             << r.Y2;
      return outstr;
    }

    friend inline auto operator >> (std::istream& instr, FRect& r) -> std::istream&
    {
      coordinate_type x1{};
      coordinate_type y1{};
      coordinate_type x2{};
      coordinate_type y2{};

      if ( instr >> x1 >> y1 >> x2 >> y2 )
        r.setCoordinates (x1, y1, x2, y2);

      return instr;
    }
};

// FRect inline functions
//----------------------------------------------------------------------
constexpr FRect::FRect ( coordinate_type x, coordinate_type y
                       , size_type width, size_type height ) noexcept
  : X1{x}
  , Y1{y}
  , X2{x + coordinate_type(width) - 1}
  , Y2{y + coordinate_type(height) - 1}
{ }

//----------------------------------------------------------------------
constexpr FRect::FRect (const FPoint& p, const FSize& s) noexcept
  : X1{p.getX()}
  , Y1{p.getY()}
  , X2{p.getX() + int(s.getWidth()) - 1}
  , Y2{p.getY() + int(s.getHeight()) - 1}
{ }

//----------------------------------------------------------------------
constexpr FRect::FRect (const FPoint& p1, const FPoint& p2) noexcept
  : X1{p1.getX()}
  , Y1{p1.getY()}
  , X2{p2.getX()}
  , Y2{p2.getY()}
{ }

//----------------------------------------------------------------------
inline auto FRect::getClassName() const -> FString
{ return "FRect"; }

//----------------------------------------------------------------------
constexpr auto FRect::getX1() const noexcept -> coordinate_type
{ return X1; }

//----------------------------------------------------------------------
constexpr auto FRect::getY1() const noexcept -> coordinate_type
{ return Y1; }

//----------------------------------------------------------------------
constexpr auto FRect::getX2() const noexcept -> coordinate_type
{ return X2; }

//----------------------------------------------------------------------
constexpr auto FRect::getY2() const noexcept -> coordinate_type
{ return Y2; }

//----------------------------------------------------------------------
constexpr auto FRect::getX() const noexcept -> coordinate_type
{ return X1; }

//----------------------------------------------------------------------
constexpr auto FRect::getY() const noexcept -> coordinate_type
{ return Y1; }

//----------------------------------------------------------------------
constexpr auto FRect::getPos() const noexcept -> FPoint
{ return { X1, Y1 }; }

//----------------------------------------------------------------------
constexpr auto FRect::getUpperLeftPos() const noexcept -> FPoint
{ return { X1, Y1 }; }

//----------------------------------------------------------------------
constexpr auto FRect::getUpperRightPos() const noexcept -> FPoint
{ return { X2, Y1 }; }

//----------------------------------------------------------------------
constexpr auto FRect::getLowerLeftPos() const noexcept -> FPoint
{ return { X1, Y2 }; }

//----------------------------------------------------------------------
constexpr auto FRect::getLowerRightPos() const noexcept -> FPoint
{ return { X2, Y2 }; }

//----------------------------------------------------------------------
constexpr auto FRect::getWidth() const noexcept -> size_type
{
  return static_cast<size_type>(std::max(0, X2 - (X1 - 1)));  // overflow save
}

//----------------------------------------------------------------------
constexpr auto FRect::getHeight() const noexcept -> size_type
{
  return static_cast<size_type>(std::max(0, Y2 - (Y1 - 1)));  // overflow save
}

//----------------------------------------------------------------------
constexpr auto FRect::getSize() const noexcept -> FSize
{ return { FSize{getWidth(), getHeight()} }; }

//----------------------------------------------------------------------
constexpr void FRect::setX1 (coordinate_type n) noexcept
{ X1 = n; }

//----------------------------------------------------------------------
constexpr void FRect::setY1 (coordinate_type n) noexcept
{ Y1 = n; }

//----------------------------------------------------------------------
constexpr void FRect::setX2 (coordinate_type n) noexcept
{ X2 = n; }

//----------------------------------------------------------------------
constexpr void FRect::setY2 (coordinate_type n) noexcept
{ Y2 = n; }

//----------------------------------------------------------------------
constexpr void FRect::setX (coordinate_type n) noexcept
{
  const coordinate_type dX = X2 - X1;
  X1 = n;
  X2 = X1 + dX;
}

//----------------------------------------------------------------------
constexpr void FRect::setY (coordinate_type n) noexcept
{
  const coordinate_type dY = Y2 - Y1;
  Y1 = n;
  Y2 = Y1 + dY;
}

//----------------------------------------------------------------------
constexpr void FRect::setPos (coordinate_type x, coordinate_type y) noexcept
{
  const coordinate_type dX = X2 - X1;
  const coordinate_type dY = Y2 - Y1;
  X1 = x;
  Y1 = y;
  X2 = X1 + dX;
  Y2 = Y1 + dY;
}

//----------------------------------------------------------------------
constexpr void FRect::setPos (const FPoint& p) noexcept
{
  const coordinate_type dX = X2 - X1;
  const coordinate_type dY = Y2 - Y1;
  X1 = p.getX();
  Y1 = p.getY();
  X2 = X1 + dX;
  Y2 = Y1 + dY;
}

//----------------------------------------------------------------------
constexpr void FRect::setWidth (size_type w) noexcept
{ X2 = X1 + coordinate_type(w) - 1; }

//----------------------------------------------------------------------
constexpr void FRect::setHeight (size_type h) noexcept
{ Y2 = Y1 + coordinate_type(h) - 1; }

//----------------------------------------------------------------------
constexpr void FRect::setSize (size_type width, size_type height) noexcept
{
  X2 = X1 + coordinate_type(width) - 1;
  Y2 = Y1 + coordinate_type(height) - 1;
}

//----------------------------------------------------------------------
constexpr void FRect::setSize (const FSize& s) noexcept
{
  X2 = X1 + coordinate_type(s.getWidth()) - 1;
  Y2 = Y1 + coordinate_type(s.getHeight()) - 1;
}

//----------------------------------------------------------------------
constexpr void FRect::setRect (const FRect& r) noexcept
{
  X1 = r.X1;
  Y1 = r.Y1;
  X2 = r.X2;
  Y2 = r.Y2;
}

//----------------------------------------------------------------------
constexpr void FRect::setRect (const FPoint& p, const FSize& s) noexcept
{
  X1 = p.getX();
  Y1 = p.getY();
  X2 = p.getX() + coordinate_type(s.getWidth()) - 1;
  Y2 = p.getY() + coordinate_type(s.getHeight()) - 1;
}

//----------------------------------------------------------------------
constexpr void FRect::setRect ( coordinate_type x, coordinate_type y
                              , size_type width, size_type height ) noexcept
{
  X1 = x;
  Y1 = y;
  X2 = x + coordinate_type(width) - 1;
  Y2 = y + coordinate_type(height) - 1;
}

//----------------------------------------------------------------------
constexpr void FRect::setCoordinates (const FPoint& p1, const FPoint& p2) noexcept
{
  setCoordinates (p1.getX(), p1.getY(), p2.getX(), p2.getY());
}

//----------------------------------------------------------------------
constexpr void FRect::setCoordinates ( coordinate_type x1, coordinate_type y1
                                     , coordinate_type x2, coordinate_type y2 ) noexcept
{
  X1 = x1;
  Y1 = y1;
  X2 = x2;
  Y2 = y2;
}

//----------------------------------------------------------------------
constexpr auto FRect::isEmpty() const -> bool
{ return X2 == X1 - 1 && Y2 == Y1 - 1; }

//----------------------------------------------------------------------
constexpr auto FRect::x1_ref() & noexcept -> coordinate_reference
{ return X1; }

//----------------------------------------------------------------------
constexpr auto FRect::y1_ref() & noexcept -> coordinate_reference
{ return Y1; }

//----------------------------------------------------------------------
constexpr auto FRect::x2_ref() & noexcept -> coordinate_reference
{ return X2; }

//----------------------------------------------------------------------
constexpr auto FRect::y2_ref() & noexcept -> coordinate_reference
{ return Y2; }

//----------------------------------------------------------------------
constexpr void FRect::move (coordinate_type dx, coordinate_type dy) noexcept
{
  X1 += dx;
  Y1 += dy;
  X2 += dx;
  Y2 += dy;
}

//----------------------------------------------------------------------
constexpr void FRect::move (const FPoint& d) noexcept
{
  X1 += d.getX();
  Y1 += d.getY();
  X2 += d.getX();
  Y2 += d.getY();
}

//----------------------------------------------------------------------
constexpr void FRect::scaleBy (int dx, int dy) noexcept
{
  X2 += dx;
  Y2 += dy;
}

//----------------------------------------------------------------------
constexpr void FRect::scaleBy (const FPoint& d) noexcept
{
  X2 += d.getX();
  Y2 += d.getY();
}

//----------------------------------------------------------------------
constexpr auto FRect::contains (coordinate_type x, coordinate_type y) const noexcept -> bool
{
  return x >= X1 && x <= X2
      && y >= Y1 && y <= Y2;
}

//----------------------------------------------------------------------
constexpr auto FRect::contains (const FPoint& p) const noexcept -> bool
{
  return p.getX() >= X1 && p.getX() <= X2
      && p.getY() >= Y1 && p.getY() <= Y2;
}

//----------------------------------------------------------------------
constexpr auto FRect::contains (const FRect& r) const noexcept -> bool
{
  return r.X1 >= X1 && r.X2 <= X2
      && r.Y1 >= Y1 && r.Y2 <= Y2;
}

//----------------------------------------------------------------------
constexpr auto FRect::overlap (const FRect& r) const noexcept -> bool
{
  return ( std::max(X1, r.X1) <= std::min(X2, r.X2)
        && std::max(Y1, r.Y1) <= std::min(Y2, r.Y2) );
}

//----------------------------------------------------------------------
constexpr void FRect::swap (FRect& r) noexcept
{
  coordinate_type tmp_X1{X1};
  coordinate_type tmp_Y1{Y1};
  coordinate_type tmp_X2{X2};
  coordinate_type tmp_Y2{Y2};
  X1 = r.X1;
  Y1 = r.Y1;
  X2 = r.X2;
  Y2 = r.Y2;
  r.X1 = tmp_X1;
  r.Y1 = tmp_Y1;
  r.X2 = tmp_X2;
  r.Y2 = tmp_Y2;
}

}  // namespace finalcut

#endif  // FRECT_H
