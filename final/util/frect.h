/***********************************************************************
* frect.h - Rectangle with position and size                           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2022 Markus Gans                                      *
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

#include "final/util/fstring.h"

namespace finalcut
{

// class forward declaration
class FPoint;
class FSize;

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
    auto getPos() const -> FPoint;
    auto getUpperLeftPos() const -> FPoint;
    auto getUpperRightPos() const -> FPoint;
    auto getLowerLeftPos() const -> FPoint;
    auto getLowerRightPos() const -> FPoint;
    auto getWidth() const noexcept -> std::size_t;
    auto getHeight() const noexcept -> std::size_t;
    auto getSize() const -> FSize;

    // Mutators
    void setX1 (int) noexcept;
    void setY1 (int) noexcept;
    void setX2 (int) noexcept;
    void setY2 (int) noexcept;
    void setX (int) noexcept;
    void setY (int) noexcept;
    void setPos (int, int) noexcept;
    void setPos (const FPoint&);
    void setWidth (std::size_t) noexcept;
    void setHeight (std::size_t) noexcept;
    void setSize (std::size_t, std::size_t) noexcept;
    void setSize (const FSize&);
    void setRect (const FRect&);
    void setRect (const FPoint&, const FSize&);
    void setRect (int, int, std::size_t, std::size_t) noexcept;
    void setCoordinates (const FPoint&, const FPoint&);
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
    void move (const FPoint&);
    void scaleBy (int, int) noexcept;
    void scaleBy (const FPoint&);
    auto contains (int, int) const noexcept -> bool;
    auto contains (const FPoint&) const -> bool;
    auto contains (const FRect&) const -> bool;
    auto overlap  (const FRect&) const -> bool;
    auto intersect (const FRect&) const -> FRect;
    auto combined (const FRect&) const -> FRect;

  private:
    // Data members
    int X1{0};
    int Y1{0};
    int X2{-1};
    int Y2{-1};

    // Friend operator functions
    friend auto operator + (const FRect&, const FSize&) -> FRect;
    friend auto operator - (const FRect&, const FSize&) -> FRect;
    friend auto operator == (const FRect&, const FRect&) -> bool;
    friend auto operator != (const FRect&, const FRect&) -> bool;
    friend auto operator << (std::ostream&, const FRect&) -> std::ostream&;
    friend auto operator >> (std::istream&, FRect&) -> std::istream&;
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
inline auto FRect::getWidth() const noexcept -> std::size_t
{
  const int w = X2 - (X1 - 1);  // overflow save
  return ( w < 0 ) ? 0 : std::size_t(w);
}

//----------------------------------------------------------------------
inline auto FRect::getHeight() const noexcept -> std::size_t
{
  const int h = Y2 - (Y1 - 1);  // overflow save
  return ( h < 0 ) ? 0 : std::size_t(h);
}

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

}  // namespace finalcut

#endif  // FRECT_H
