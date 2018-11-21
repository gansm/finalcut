/***********************************************************************
* frect.h - Rectangle with position and size                           *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2014-2018 Markus Gans                                      *
*                                                                      *
* The Final Cut is free software; you can redistribute it and/or       *
* modify it under the terms of the GNU Lesser General Public License   *
* as published by the Free Software Foundation; either version 3 of    *
* the License, or (at your option) any later version.                  *
*                                                                      *
* The Final Cut is distributed in the hope that it will be useful,     *
* but WITHOUT ANY WARRANTY; without even the implied warranty of       *
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
 * ▕ FRect ▏- - - -▕ FPoint ▏
 * ▕▁▁▁▁▁▁▁▏       ▕▁▁▁▁▁▁▁▁▏
 */

#ifndef FRECT_H
#define FRECT_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <algorithm>
#include "final/fpoint.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FRect
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FRect
{
  public:
    // Constructors
    FRect ();
    FRect (const FRect&);  // copy constructor
    FRect (int, int, std::size_t, std::size_t);
    FRect (const FPoint&, const FPoint&);

    // Destructor
    virtual ~FRect();

    // Overloaded operators
    FRect& operator = (const FRect&);

    friend FRect operator +  (const FRect&, const FPoint&);
    friend FRect operator -  (const FRect&, const FPoint&);
    friend bool  operator == (const FRect&, const FRect&);
    friend bool  operator != (const FRect&, const FRect&);
    friend std::ostream& operator << (std::ostream&, const FRect&);
    friend std::istream& operator >> (std::istream&, FRect&);

    // Accessors
    virtual const char* getClassName();
    int         getX1() const;
    int         getY1() const;
    int         getX2() const;
    int         getY2() const;
    int         getX() const;
    int         getY() const;
    FPoint      getPos() const;
    FPoint      getUpperLeftPos() const;
    FPoint      getUpperRightPos() const;
    FPoint      getLowerLeftPos() const;
    FPoint      getLowerRightPos() const;
    std::size_t getWidth() const;
    std::size_t getHeight() const;

    // Mutators
    void        setX1 (int);
    void        setY1 (int);
    void        setX2 (int);
    void        setY2 (int);
    void        setX (int);
    void        setY (int);
    void        setPos (int, int);
    void        setPos (const FPoint&);
    void        setWidth (std::size_t);
    void        setHeight (std::size_t);
    void        setSize (std::size_t, std::size_t);
    void        setRect (const FRect&);
    void        setRect (int, int, std::size_t, std::size_t);
    void        setCoordinates (const FPoint&, const FPoint&);
    void        setCoordinates (int, int, int, int);

    // Inquiry
    bool        isNull() const;

    // Coordinate references
    int&        x1_ref();
    int&        y1_ref();
    int&        x2_ref();
    int&        y2_ref();

    // Methods
    void        move (int, int);
    void        move (const FPoint&);
    bool        contains (int, int) const;
    bool        contains (const FPoint&) const;
    bool        contains (const FRect&) const;
    bool        overlap  (const FRect&) const;
    FRect       intersect (const FRect&) const;
    FRect       combined (const FRect&) const;

  private:
    // Data Members
    int X1;
    int Y1;
    int X2;
    int Y2;
};
#pragma pack(pop)


// FRect inline functions
//----------------------------------------------------------------------
inline FRect::FRect()
  : X1(0)
  , Y1(0)
  , X2(-1)
  , Y2(-1)
{ }

//----------------------------------------------------------------------
inline FRect::FRect (const FRect& r)  // copy constructor
  : X1(r.X1)
  , Y1(r.Y1)
  , X2(r.X2)
  , Y2(r.Y2)
{ }

//----------------------------------------------------------------------
inline FRect::FRect (int x, int y, std::size_t width, std::size_t height)
  : X1(x)
  , Y1(y)
  , X2(x + int(width) - 1)
  , Y2(y + int(height) - 1)
{ }

//----------------------------------------------------------------------
inline const char* FRect::getClassName()
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
inline FPoint FRect::getPos() const
{ return FPoint(X1, Y1); }

//----------------------------------------------------------------------
inline FPoint FRect::getUpperLeftPos() const
{ return FPoint(X1, Y1); }

//----------------------------------------------------------------------
inline FPoint FRect::getUpperRightPos() const
{ return FPoint(X2, Y1); }

//----------------------------------------------------------------------
inline FPoint FRect::getLowerLeftPos() const
{ return FPoint(X1, Y2); }

//----------------------------------------------------------------------
inline FPoint FRect::getLowerRightPos() const
{ return FPoint(X2, Y2); }

//----------------------------------------------------------------------
inline std::size_t FRect::getWidth() const
{
  int w = X2 - X1 + 1;
  return ( w < 0 ) ? 0 : std::size_t(w);
}

//----------------------------------------------------------------------
inline std::size_t FRect::getHeight() const
{
  int h = Y2 - Y1 + 1;
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
