/***********************************************************************
* frect.cpp - Rectangle with position and size                         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2020 Markus Gans                                      *
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

#include <algorithm>
#include <utility>

#include "final/fpoint.h"
#include "final/frect.h"
#include "final/fsize.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FRect
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FRect::FRect (const FPoint& p, const FSize& s)
  : X1{p.getX()}
  , Y1{p.getY()}
  , X2{p.getX() + int(s.getWidth()) - 1}
  , Y2{p.getY() + int(s.getHeight()) - 1}
{ }

//----------------------------------------------------------------------
FRect::FRect (const FPoint& p1, const FPoint& p2)
  : X1{p1.getX()}
  , Y1{p1.getY()}
  , X2{p2.getX()}
  , Y2{p2.getY()}
{ }

//----------------------------------------------------------------------
FRect::~FRect()  // destructor
{ }


// public methods of FRect
//----------------------------------------------------------------------
FRect& FRect::operator = (const FRect& r)
{
  X1 = r.X1;
  Y1 = r.Y1;
  X2 = r.X2;
  Y2 = r.Y2;
  return *this;
}

//----------------------------------------------------------------------
FRect& FRect::operator = (FRect&& r) noexcept
{
  X1 = std::move(r.X1);
  Y1 = std::move(r.Y1);
  X2 = std::move(r.X2);
  Y2 = std::move(r.Y2);
  return *this;
}

//----------------------------------------------------------------------
bool FRect::isEmpty() const
{
  return X2 == X1 - 1 && Y2 == Y1 - 1;
}

//----------------------------------------------------------------------
FPoint FRect::getPos() const
{
  return { X1, Y1 };
}

//----------------------------------------------------------------------
FPoint FRect::getUpperLeftPos() const
{
  return { X1, Y1 };
}

//----------------------------------------------------------------------
FPoint FRect::getUpperRightPos() const
{
  return { X2, Y1 };
}

//----------------------------------------------------------------------
FPoint FRect::getLowerLeftPos() const
{
  return { X1, Y2 };
}

//----------------------------------------------------------------------
FPoint FRect::getLowerRightPos() const
{
  return { X2, Y2 };
}

//----------------------------------------------------------------------
FSize FRect::getSize() const
{
  return { getWidth(), getHeight() };
}

//----------------------------------------------------------------------
void FRect::setX1 (int n)
{
  X1 = n;
}

//----------------------------------------------------------------------
void FRect::setY1 (int n)
{
  Y1 = n;
}

//----------------------------------------------------------------------
void FRect::setX2 (int n)
{
  X2 = n;
}

//----------------------------------------------------------------------
void FRect::setY2 (int n)
{
  Y2 = n;
}

//----------------------------------------------------------------------
void FRect::setX (int n)
{
  const int dX = X2 - X1;
  X1 = n;
  X2 = X1 + dX;
}

//----------------------------------------------------------------------
void FRect::setY (int n)
{
  const int dY = Y2 - Y1;
  Y1 = n;
  Y2 = Y1 + dY;
}

//----------------------------------------------------------------------
void FRect::setPos (int x, int y)
{
  const int dX = X2 - X1;
  const int dY = Y2 - Y1;
  X1 = x;
  Y1 = y;
  X2 = X1 + dX;
  Y2 = Y1 + dY;
}

//----------------------------------------------------------------------
void FRect::setPos (const FPoint& p)
{
  const int dX = X2 - X1;
  const int dY = Y2 - Y1;
  X1 = p.getX();
  Y1 = p.getY();
  X2 = X1 + dX;
  Y2 = Y1 + dY;
}

//----------------------------------------------------------------------
void FRect::setWidth (std::size_t w)
{
  X2 = X1 + int(w) - 1;
}

//----------------------------------------------------------------------
void FRect::setHeight (std::size_t h)
{
  Y2 = Y1 + int(h) - 1;
}

//----------------------------------------------------------------------
void FRect::setSize (std::size_t width, std::size_t height)
{
  X2 = X1 + int(width) - 1;
  Y2 = Y1 + int(height) - 1;
}

//----------------------------------------------------------------------
void FRect::setSize (const FSize& s)
{
  X2 = X1 + int(s.getWidth()) - 1;
  Y2 = Y1 + int(s.getHeight()) - 1;
}

//----------------------------------------------------------------------
void FRect::setRect (const FRect& r)
{
  X1 = r.X1;
  Y1 = r.Y1;
  X2 = r.X2;
  Y2 = r.Y2;
}

//----------------------------------------------------------------------
void FRect::setRect (const FPoint& p, const FSize& s)
{
  X1 = p.getX();
  Y1 = p.getY();
  X2 = p.getX() + int(s.getWidth()) - 1;
  Y2 = p.getY() + int(s.getHeight()) - 1;
}

//----------------------------------------------------------------------
void FRect::setRect (int x, int y, std::size_t width, std::size_t height)
{
  X1 = x;
  Y1 = y;
  X2 = x + int(width) - 1;
  Y2 = y + int(height) - 1;
}

//----------------------------------------------------------------------
void FRect::setCoordinates (const FPoint& p1, const FPoint& p2)
{
  setCoordinates (p1.getX(), p1.getY(), p2.getX(), p2.getY());
}

//----------------------------------------------------------------------
void FRect::setCoordinates (int x1, int y1, int x2, int y2)
{
  X1 = x1;
  Y1 = y1;
  X2 = x2;
  Y2 = y2;
}

//----------------------------------------------------------------------
void FRect::move (int dx, int dy)
{
  X1 += dx;
  Y1 += dy;
  X2 += dx;
  Y2 += dy;
}

//----------------------------------------------------------------------
void FRect::move (const FPoint& d)
{
  X1 += d.getX();
  Y1 += d.getY();
  X2 += d.getX();
  Y2 += d.getY();
}

//----------------------------------------------------------------------
void FRect::scaleBy (int dx, int dy)
{
  X2 += dx;
  Y2 += dy;
}

//----------------------------------------------------------------------
void FRect::scaleBy (const FPoint& d)
{
  X2 += d.getX();
  Y2 += d.getY();
}

//----------------------------------------------------------------------
bool FRect::contains (int x, int y) const
{
  return x >= X1 && x <= X2
      && y >= Y1 && y <= Y2;
}

//----------------------------------------------------------------------
bool FRect::contains (const FPoint& p) const
{
  return p.getX() >= X1 && p.getX() <= X2
      && p.getY() >= Y1 && p.getY() <= Y2;
}

//----------------------------------------------------------------------
bool FRect::contains (const FRect& r) const
{
  return r.X1 >= X1 && r.X2 <= X2
      && r.Y1 >= Y1 && r.Y2 <= Y2;
}

//----------------------------------------------------------------------
bool FRect::overlap (const FRect &r) const
{
  return ( std::max(X1, r.X1) <= std::min(X2, r.X2)
        && std::max(Y1, r.Y1) <= std::min(Y2, r.Y2) );
}

//----------------------------------------------------------------------
FRect FRect::intersect (const FRect& r) const
{
  // intersection: this ∩ r
  int _X1 = std::max(X1, r.X1);
  int _Y1 = std::max(Y1, r.Y1);
  int _X2 = std::min(X2, r.X2);
  int _Y2 = std::min(Y2, r.Y2);
  const FPoint p1{ _X1, _Y1 };
  const FPoint p2{ _X2, _Y2 };
  return { p1, p2 };
}

//----------------------------------------------------------------------
FRect FRect::combined (const FRect& r) const
{
  // Union: this ∪ r
  int _X1 = std::min(X1, r.X1);
  int _Y1 = std::min(Y1, r.Y1);
  int _X2 = std::max(X2, r.X2);
  int _Y2 = std::max(Y2, r.Y2);
  const FPoint p1{ _X1, _Y1 };
  const FPoint p2{ _X2, _Y2 };
  return { p1, p2 };
}


// FRect non-member operators
//----------------------------------------------------------------------
FRect operator + (const FRect& r, const FSize& s)
{
  return { r.X1
         , r.Y1
         , std::size_t(r.X2 - r.X1) + 1 + s.getWidth()
         , std::size_t(r.Y2 - r.Y1) + 1 + s.getHeight() };
}

//----------------------------------------------------------------------
FRect operator - (const FRect& r, const FSize& s)
{
  return { r.X1
         , r.Y1
         , std::size_t(r.X2 - r.X1) + 1 - s.getWidth()
         , std::size_t(r.Y2 - r.Y1) + 1 - s.getHeight() };
}

//----------------------------------------------------------------------
bool operator == (const FRect& r1, const FRect& r2)
{
  return r1.X1 == r2.X1
      && r1.Y1 == r2.Y1
      && r1.X2 == r2.X2
      && r1.Y2 == r2.Y2;
}

//----------------------------------------------------------------------
bool operator != (const FRect& r1, const FRect& r2)
{
  return r1.X1 != r2.X1
      || r1.Y1 != r2.Y1
      || r1.X2 != r2.X2
      || r1.Y2 != r2.Y2;
}

//----------------------------------------------------------------------
std::ostream& operator << (std::ostream& outstr, const FRect& r)
{
  outstr << r.X1 << " "
         << r.Y1 << " "
         << r.X2 << " "
         << r.Y2;
  return outstr;
}

//----------------------------------------------------------------------
std::istream& operator >> (std::istream& instr, FRect& r)
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

}  // namespace finalcut
