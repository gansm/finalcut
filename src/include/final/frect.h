/***********************************************************************
* frect.h - Rectangle with position and size                           *
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

#include "final/fstring.h"

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
    FRect () = default;
    FRect (const FRect&);      // copy constructor
    FRect (FRect&&) noexcept;  // move constructor
    FRect (int, int, std::size_t, std::size_t);
    FRect (const FPoint&, const FSize&);
    FRect (const FPoint&, const FPoint&);

    // Destructor
    virtual ~FRect();

    // Overloaded operators
    FRect& operator = (const FRect&);
    FRect& operator = (FRect&&) noexcept;

    // Accessors
    virtual FString       getClassName();
    int                   getX1() const;
    int                   getY1() const;
    int                   getX2() const;
    int                   getY2() const;
    int                   getX() const;
    int                   getY() const;
    FPoint                getPos() const;
    FPoint                getUpperLeftPos() const;
    FPoint                getUpperRightPos() const;
    FPoint                getLowerLeftPos() const;
    FPoint                getLowerRightPos() const;
    std::size_t           getWidth() const;
    std::size_t           getHeight() const;
    FSize                 getSize() const;

    // Mutators
    void                  setX1 (int);
    void                  setY1 (int);
    void                  setX2 (int);
    void                  setY2 (int);
    void                  setX (int);
    void                  setY (int);
    void                  setPos (int, int);
    void                  setPos (const FPoint&);
    void                  setWidth (std::size_t);
    void                  setHeight (std::size_t);
    void                  setSize (std::size_t, std::size_t);
    void                  setSize (const FSize&);
    void                  setRect (const FRect&);
    void                  setRect (const FPoint&, const FSize&);
    void                  setRect (int, int, std::size_t, std::size_t);
    void                  setCoordinates (const FPoint&, const FPoint&);
    void                  setCoordinates (int, int, int, int);

    // Inquiry
    bool                  isEmpty() const;

    // Coordinate references
    int&                  x1_ref();
    int&                  y1_ref();
    int&                  x2_ref();
    int&                  y2_ref();

    // Methods
    void                  move (int, int);
    void                  move (const FPoint&);
    void                  scaleBy (int, int);
    void                  scaleBy (const FPoint&);
    bool                  contains (int, int) const;
    bool                  contains (const FPoint&) const;
    bool                  contains (const FRect&) const;
    bool                  overlap  (const FRect&) const;
    FRect                 intersect (const FRect&) const;
    FRect                 combined (const FRect&) const;

  private:
    // Data members
    int X1{0};
    int Y1{0};
    int X2{-1};
    int Y2{-1};

    // Friend operator functions
    friend FRect operator + (const FRect&, const FSize&);
    friend FRect operator - (const FRect&, const FSize&);
    friend bool  operator == (const FRect&, const FRect&);
    friend bool  operator != (const FRect&, const FRect&);
    friend std::ostream& operator << (std::ostream&, const FRect&);
    friend std::istream& operator >> (std::istream&, FRect&);
};

// FRect inline functions
//----------------------------------------------------------------------
inline FRect::FRect (const FRect& r)  // copy constructor
  : X1{r.X1}
  , Y1{r.Y1}
  , X2{r.X2}
  , Y2{r.Y2}
{ }

//----------------------------------------------------------------------
inline FRect::FRect (FRect&& r) noexcept  // move constructor
  : X1{std::move(r.X1)}
  , Y1{std::move(r.Y1)}
  , X2{std::move(r.X2)}
  , Y2{std::move(r.Y2)}
{ }

//----------------------------------------------------------------------
inline FRect::FRect (int x, int y, std::size_t width, std::size_t height)
  : X1{x}
  , Y1{y}
  , X2{x + int(width) - 1}
  , Y2{y + int(height) - 1}
{ }

//----------------------------------------------------------------------
inline FString FRect::getClassName()
{ return "FRect"; }

//----------------------------------------------------------------------
inline int FRect::getX1() const
{ return X1; }

//----------------------------------------------------------------------
inline int FRect::getY1() const
{ return Y1; }

//----------------------------------------------------------------------
inline int FRect::getX2() const
{ return X2; }

//----------------------------------------------------------------------
inline int FRect::getY2() const
{ return Y2; }

//----------------------------------------------------------------------
inline int FRect::getX() const
{ return X1; }

//----------------------------------------------------------------------
inline int FRect::getY() const
{ return Y1; }

//----------------------------------------------------------------------
inline std::size_t FRect::getWidth() const
{
  const int w = X2 - X1 + 1;
  return ( w < 0 ) ? 0 : std::size_t(w);
}

//----------------------------------------------------------------------
inline std::size_t FRect::getHeight() const
{
  const int h = Y2 - Y1 + 1;
  return ( h < 0 ) ? 0 : std::size_t(h);
}

//----------------------------------------------------------------------
inline int& FRect::x1_ref()
{ return X1; }

//----------------------------------------------------------------------
inline int& FRect::y1_ref()
{ return Y1; }

//----------------------------------------------------------------------
inline int& FRect::x2_ref()
{ return X2; }

//----------------------------------------------------------------------
inline int& FRect::y2_ref()
{ return Y2; }

}  // namespace finalcut

#endif  // FRECT_H
