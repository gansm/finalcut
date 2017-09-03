// File: frect.cpp
// Provides: class FRect

#include "frect.h"

//----------------------------------------------------------------------
// class FRect
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FRect::FRect (const FPoint& p1, const FPoint& p2)
  : X1(short(p1.getX()))
  , Y1(short(p1.getY()))
  , X2(short(p2.getX()))
  , Y2(short(p2.getY()))
{ }

//----------------------------------------------------------------------
FRect::~FRect()  // destructor
{ }

// public methods of FRect
//----------------------------------------------------------------------
bool FRect::isNull() const
{
  return X2 == X1 - 1 && Y2 == Y1 - 1;
}

//----------------------------------------------------------------------
void FRect::setX1 (int n)
{
  X1 = short(n);
}

//----------------------------------------------------------------------
void FRect::setY1 (int n)
{
  Y1 = short(n);
}

//----------------------------------------------------------------------
void FRect::setX2 (int n)
{
  X2 = short(n);
}

//----------------------------------------------------------------------
void FRect::setY2 (int n)
{
  Y2 = short(n);
}

//----------------------------------------------------------------------
void FRect::setX (int n)
{
  short dX = short(X2 - X1);
  X1 = short(n);
  X2 = short(X1 + dX);
}

//----------------------------------------------------------------------
void FRect::setY (int n)
{
  short dY = short(Y2 - Y1);
  Y1 = short(n);
  Y2 = short(Y1 + dY);
}

//----------------------------------------------------------------------
void FRect::setPos (int x, int y)
{
  short dX = short(X2 - X1);
  short dY = short(Y2 - Y1);
  X1 = short(x);
  Y1 = short(y);
  X2 = short(X1 + dX);
  Y2 = short(Y1 + dY);
}

//----------------------------------------------------------------------
void FRect::setPos (const FPoint& p)
{
  short dX = short(X2 - X1);
  short dY = short(Y2 - Y1);
  X1 = short(p.getX());
  Y1 = short(p.getY());
  X2 = short(X1 + dX);
  Y2 = short(Y1 + dY);
}

//----------------------------------------------------------------------
void FRect::setWidth (int w)
{
  X2 = short(X1 + w - 1);
}

//----------------------------------------------------------------------
void FRect::setHeight (int h)
{
  Y2 = short(Y1 + h - 1);
}

//----------------------------------------------------------------------
void FRect::setSize (int w, int h)
{
  X2 = short(X1 + w - 1);
  Y2 = short(Y1 + h - 1);
}

//----------------------------------------------------------------------
void FRect::setRect (const FRect& r)
{
  setRect (r.X1, r.Y1, r.X2 - r.X1 + 1, r.Y2 - r.Y1 + 1);
}

//----------------------------------------------------------------------
void FRect::setRect (int x, int y, int width, int height)
{
  X1 = short(x);
  Y1 = short(y);
  X2 = short(x + width - 1);
  Y2 = short(y + height - 1);
}

//----------------------------------------------------------------------
void FRect::setCoordinates (const FPoint& p1, const FPoint& p2)
{
  setCoordinates (p1.getX(), p1.getY(), p2.getX(), p2.getY());
}

//----------------------------------------------------------------------
void FRect::setCoordinates (int x1, int y1, int x2, int y2)
{
  X1 = short(x1);
  Y1 = short(y1);
  X2 = short(x2);
  Y2 = short(y2);
}

//----------------------------------------------------------------------
void FRect::move (int dx, int dy)
{
  X1 = short(X1 + dx);
  Y1 = short(Y1 + dy);
  X2 = short(X2 + dx);
  Y2 = short(Y2 + dy);
}

//----------------------------------------------------------------------
void FRect::move  (const FPoint& d)
{
  X1 = short(X1 + d.getX());
  Y1 = short(Y1 + d.getY());
  X2 = short(X2 + d.getX());
  Y2 = short(Y2 + d.getY());
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
  FRect new_rect;
  new_rect.X1 = std::max(X1, r.X1);
  new_rect.Y1 = std::max(Y1, r.Y1);
  new_rect.X2 = std::min(X2, r.X2);
  new_rect.Y2 = std::min(Y2, r.Y2);
  return new_rect;
}

//----------------------------------------------------------------------
FRect FRect::combined (const FRect& r) const
{
  // Union: this ∪ r
  FRect new_rect;
  new_rect.X1 = std::min(X1, r.X1);
  new_rect.Y1 = std::min(Y1, r.Y1);
  new_rect.X2 = std::max(X2, r.X2);
  new_rect.Y2 = std::max(Y2, r.Y2);
  return new_rect;
}

//----------------------------------------------------------------------
FRect& FRect::operator = (const FRect& r)
{
  X1 = short(r.getX1());
  Y1 = short(r.getY1());
  X2 = short(r.getX2());
  Y2 = short(r.getY2());
  return *this;
}

//----------------------------------------------------------------------
FRect operator + (const FRect& r, const FPoint& p)
{
  return FRect ( r.X1
               , r.Y1
               , r.X2 - r.X1 + 1 + p.getX()
               , r.Y2 - r.Y1 + 1 + p.getY() );
}

//----------------------------------------------------------------------
FRect operator - (const FRect& r, const FPoint& p)
{
  return FRect ( r.X1
               , r.Y1
               , r.X2 - r.X1 + 1 - p.getX()
               , r.Y2 - r.Y1 + 1 - p.getY() );
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
