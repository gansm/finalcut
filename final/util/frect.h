/***********************************************************************
* frect.h - Rectangle with position and size                           *
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
    // Constructors
    FRect () noexcept = default;
    FRect (int, int, std::size_t, std::size_t) noexcept;
    FRect (const FPoint&, const FSize&) noexcept;
    FRect (const FPoint&, const FPoint&) noexcept;

    // Accessors
    auto getClassName() const -> FString;
    auto getX1() const noexcept -> int;
    auto getY1() const noexcept -> int;
    auto getX2() const noexcept -> int;
    auto getY2() const noexcept -> int;
    auto getX() const noexcept -> int;
    auto getY() const noexcept -> int;
    auto getPos() const noexcept -> FPoint;
    auto getUpperLeftPos() const noexcept -> FPoint;
    auto getUpperRightPos() const noexcept -> FPoint;
    auto getLowerLeftPos() const noexcept -> FPoint;
    auto getLowerRightPos() const noexcept -> FPoint;
    auto getWidth() const noexcept -> std::size_t;
    auto getHeight() const noexcept -> std::size_t;
    auto getSize() const noexcept -> FSize;

    // Mutators
    void setX1 (int) noexcept;
    void setY1 (int) noexcept;
    void setX2 (int) noexcept;
    void setY2 (int) noexcept;
    void setX (int) noexcept;
    void setY (int) noexcept;
    void setPos (int, int) noexcept;
    void setPos (const FPoint&) noexcept;
    void setWidth (std::size_t) noexcept;
    void setHeight (std::size_t) noexcept;
    void setSize (std::size_t, std::size_t) noexcept;
    void setSize (const FSize&) noexcept;
    void setRect (const FRect&) noexcept;
    void setRect (const FPoint&, const FSize&) noexcept;
    void setRect (int, int, std::size_t, std::size_t) noexcept;
    void setCoordinates (const FPoint&, const FPoint&) noexcept;
    void setCoordinates (int, int, int, int) noexcept;

    // Inquiry
    auto isEmpty() const -> bool;

    // Coordinate references
    auto x1_ref() & noexcept -> int&;
    auto y1_ref() & noexcept -> int&;
    auto x2_ref() & noexcept -> int&;
    auto y2_ref() & noexcept -> int&;

    // Methods
    void move (int, int) noexcept;
    void move (const FPoint&) noexcept;
    void scaleBy (int, int) noexcept;
    void scaleBy (const FPoint&) noexcept;
    auto contains (int, int) const noexcept -> bool;
    auto contains (const FPoint&) const noexcept -> bool;
    auto contains (const FRect&) const noexcept -> bool;
    auto overlap  (const FRect&) const noexcept -> bool;
    auto intersect (const FRect&) const noexcept -> FRect;
    auto combined (const FRect&) const noexcept -> FRect;

  private:
    // Data members
    int X1{0};
    int Y1{0};
    int X2{-1};
    int Y2{-1};

    // Friend operator functions
    friend inline auto operator + (const FRect& r, const FSize& s) -> FRect
    {
      return { r.X1
             , r.Y1
             , std::size_t(r.X2 - r.X1) + 1 + s.getWidth()
             , std::size_t(r.Y2 - r.Y1) + 1 + s.getHeight() };
    }

    friend inline auto operator - (const FRect& r, const FSize& s) -> FRect
    {
      return { r.X1
             , r.Y1
             , std::size_t(r.X2 - r.X1) + 1 - s.getWidth()
             , std::size_t(r.Y2 - r.Y1) + 1 - s.getHeight() };
    }

    friend inline auto operator == (const FRect& r1, const FRect& r2) -> bool
    {
      return r1.X1 == r2.X1
          && r1.Y1 == r2.Y1
          && r1.X2 == r2.X2
          && r1.Y2 == r2.Y2;
    }

    friend inline auto operator != (const FRect& r1, const FRect& r2) -> bool
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
      int x1{};
      int y1{};
      int x2{};
      int y2{};
      instr >> x1;
      instr >> y1;
      instr >> x2;
      instr >> y2;
      r.setCoordinates (x1, y1, x2, y2);
      return instr;
    }
};

// FRect inline functions
//----------------------------------------------------------------------
inline FRect::FRect (int x, int y, std::size_t width, std::size_t height) noexcept
  : X1{x}
  , Y1{y}
  , X2{x + int(width) - 1}
  , Y2{y + int(height) - 1}
{ }

//----------------------------------------------------------------------
inline auto FRect::getClassName() const -> FString
{ return "FRect"; }

//----------------------------------------------------------------------
inline auto FRect::getX1() const noexcept -> int
{ return X1; }

//----------------------------------------------------------------------
inline auto FRect::getY1() const noexcept -> int
{ return Y1; }

//----------------------------------------------------------------------
inline auto FRect::getX2() const noexcept -> int
{ return X2; }

//----------------------------------------------------------------------
inline auto FRect::getY2() const noexcept -> int
{ return Y2; }

//----------------------------------------------------------------------
inline auto FRect::getX() const noexcept -> int
{ return X1; }

//----------------------------------------------------------------------
inline auto FRect::getY() const noexcept -> int
{ return Y1; }

//----------------------------------------------------------------------
inline auto FRect::getPos() const noexcept -> FPoint
{ return { X1, Y1 }; }

//----------------------------------------------------------------------
inline auto FRect::getUpperLeftPos() const noexcept -> FPoint
{ return { X1, Y1 }; }

//----------------------------------------------------------------------
inline auto FRect::getUpperRightPos() const noexcept -> FPoint
{ return { X2, Y1 }; }

//----------------------------------------------------------------------
inline auto FRect::getLowerLeftPos() const noexcept -> FPoint
{ return { X1, Y2 }; }

//----------------------------------------------------------------------
inline auto FRect::getLowerRightPos() const noexcept -> FPoint
{ return { X2, Y2 }; }

//----------------------------------------------------------------------
inline auto FRect::getWidth() const noexcept -> std::size_t
{
  return static_cast<std::size_t>(std::max(0, X2 - (X1 - 1)));  // overflow save
}

//----------------------------------------------------------------------
inline auto FRect::getHeight() const noexcept -> std::size_t
{
  return static_cast<std::size_t>(std::max(0, Y2 - (Y1 - 1)));  // overflow save
}

//----------------------------------------------------------------------
inline auto FRect::getSize() const noexcept -> FSize
{ return { FSize{getWidth(), getHeight()} }; }

//----------------------------------------------------------------------
inline void FRect::setX1 (int n) noexcept
{ X1 = n; }

//----------------------------------------------------------------------
inline void FRect::setY1 (int n) noexcept
{ Y1 = n; }

//----------------------------------------------------------------------
inline void FRect::setX2 (int n) noexcept
{ X2 = n; }

//----------------------------------------------------------------------
inline void FRect::setY2 (int n) noexcept
{ Y2 = n; }

//----------------------------------------------------------------------
inline void FRect::setX (int n) noexcept
{
  const int dX = X2 - X1;
  X1 = n;
  X2 = X1 + dX;
}

//----------------------------------------------------------------------
inline void FRect::setY (int n) noexcept
{
  const int dY = Y2 - Y1;
  Y1 = n;
  Y2 = Y1 + dY;
}

//----------------------------------------------------------------------
inline void FRect::setPos (int x, int y) noexcept
{
  const int dX = X2 - X1;
  const int dY = Y2 - Y1;
  X1 = x;
  Y1 = y;
  X2 = X1 + dX;
  Y2 = Y1 + dY;
}

//----------------------------------------------------------------------
inline void FRect::setPos (const FPoint& p) noexcept
{
  const int dX = X2 - X1;
  const int dY = Y2 - Y1;
  X1 = p.getX();
  Y1 = p.getY();
  X2 = X1 + dX;
  Y2 = Y1 + dY;
}

//----------------------------------------------------------------------
inline void FRect::setWidth (std::size_t w) noexcept
{ X2 = X1 + int(w) - 1; }

//----------------------------------------------------------------------
inline void FRect::setHeight (std::size_t h) noexcept
{ Y2 = Y1 + int(h) - 1; }

//----------------------------------------------------------------------
inline void FRect::setSize (std::size_t width, std::size_t height) noexcept
{
  X2 = X1 + int(width) - 1;
  Y2 = Y1 + int(height) - 1;
}

//----------------------------------------------------------------------
inline void FRect::setSize (const FSize& s) noexcept
{
  X2 = X1 + int(s.getWidth()) - 1;
  Y2 = Y1 + int(s.getHeight()) - 1;
}

//----------------------------------------------------------------------
inline void FRect::setRect (const FRect& r) noexcept
{
  X1 = r.X1;
  Y1 = r.Y1;
  X2 = r.X2;
  Y2 = r.Y2;
}

//----------------------------------------------------------------------
inline void FRect::setRect (const FPoint& p, const FSize& s) noexcept
{
  X1 = p.getX();
  Y1 = p.getY();
  X2 = p.getX() + int(s.getWidth()) - 1;
  Y2 = p.getY() + int(s.getHeight()) - 1;
}

//----------------------------------------------------------------------
inline void FRect::setRect (int x, int y, std::size_t width, std::size_t height) noexcept
{
  X1 = x;
  Y1 = y;
  X2 = x + int(width) - 1;
  Y2 = y + int(height) - 1;
}

//----------------------------------------------------------------------
inline void FRect::setCoordinates (const FPoint& p1, const FPoint& p2) noexcept
{
  setCoordinates (p1.getX(), p1.getY(), p2.getX(), p2.getY());
}

//----------------------------------------------------------------------
inline void FRect::setCoordinates (int x1, int y1, int x2, int y2) noexcept
{
  X1 = x1;
  Y1 = y1;
  X2 = x2;
  Y2 = y2;
}

//----------------------------------------------------------------------
inline auto FRect::isEmpty() const -> bool
{ return X2 == X1 - 1 && Y2 == Y1 - 1; }

//----------------------------------------------------------------------
inline auto FRect::x1_ref() & noexcept -> int&
{ return X1; }

//----------------------------------------------------------------------
inline auto FRect::y1_ref() & noexcept -> int&
{ return Y1; }

//----------------------------------------------------------------------
inline auto FRect::x2_ref() & noexcept -> int&
{ return X2; }

//----------------------------------------------------------------------
inline auto FRect::y2_ref() & noexcept -> int&
{ return Y2; }

//----------------------------------------------------------------------
inline void FRect::move (int dx, int dy) noexcept
{
  X1 += dx;
  Y1 += dy;
  X2 += dx;
  Y2 += dy;
}

//----------------------------------------------------------------------
inline void FRect::move (const FPoint& d) noexcept
{
  X1 += d.getX();
  Y1 += d.getY();
  X2 += d.getX();
  Y2 += d.getY();
}

//----------------------------------------------------------------------
inline void FRect::scaleBy (int dx, int dy) noexcept
{
  X2 += dx;
  Y2 += dy;
}

//----------------------------------------------------------------------
inline void FRect::scaleBy (const FPoint& d) noexcept
{
  X2 += d.getX();
  Y2 += d.getY();
}

//----------------------------------------------------------------------
inline auto FRect::contains (int x, int y) const noexcept -> bool
{
  return x >= X1 && x <= X2
      && y >= Y1 && y <= Y2;
}

//----------------------------------------------------------------------
inline auto FRect::contains (const FPoint& p) const noexcept -> bool
{
  return p.getX() >= X1 && p.getX() <= X2
      && p.getY() >= Y1 && p.getY() <= Y2;
}

//----------------------------------------------------------------------
inline auto FRect::contains (const FRect& r) const noexcept -> bool
{
  return r.X1 >= X1 && r.X2 <= X2
      && r.Y1 >= Y1 && r.Y2 <= Y2;
}

//----------------------------------------------------------------------
inline auto FRect::overlap (const FRect &r) const noexcept -> bool
{
  return ( std::max(X1, r.X1) <= std::min(X2, r.X2)
        && std::max(Y1, r.Y1) <= std::min(Y2, r.Y2) );
}

}  // namespace finalcut

#endif  // FRECT_H
