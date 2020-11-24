/***********************************************************************
* fpoint.h - Point with an x and y coordinate                          *
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

#include "final/fstring.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FPoint
//----------------------------------------------------------------------

class FPoint
{
  public:
    // Constructors
    FPoint () = default;
    FPoint (int, int);

    // Overloaded operators
    FPoint& operator += (const FPoint&);
    FPoint& operator -= (const FPoint&);

    // Accessors
    FString               getClassName();
    int                   getX() const;
    int                   getY() const;
    void                  setX (int);
    void                  setY (int);
    void                  setPoint (const FPoint&);
    void                  setPoint (int, int);

    // Inquiry
    bool                  isOrigin() const;

    // Point references
    int&                  x_ref();
    int&                  y_ref();

    // Methods
    void                  move (int, int);
    void                  move (const FPoint&);

  private:
    // Data members
    int xpos{0};
    int ypos{0};

    // Friend operator functions
    friend bool operator == (const FPoint&, const FPoint&);
    friend bool operator != (const FPoint&, const FPoint&);
    friend FPoint operator +  (const FPoint&, const FPoint&);
    friend FPoint operator -  (const FPoint&, const FPoint&);
    friend FPoint operator -  (const FPoint&);
    friend std::ostream& operator << (std::ostream&, const FPoint&);
    friend std::istream& operator >> (std::istream&, FPoint&);
};


// FPoint inline functions
//----------------------------------------------------------------------
inline FPoint::FPoint (int x, int y)
  : xpos{x}
  , ypos{y}
{ }

//----------------------------------------------------------------------
inline FString FPoint::getClassName()
{ return "FPoint"; }

//----------------------------------------------------------------------
inline int FPoint::getX() const
{ return xpos; }

//----------------------------------------------------------------------
inline int FPoint::getY() const
{ return ypos; }

//----------------------------------------------------------------------
inline void FPoint::setPoint (const FPoint& p)
{ setPoint(p.xpos, p.ypos); }

//----------------------------------------------------------------------
inline int& FPoint::x_ref()
{ return xpos; }

//----------------------------------------------------------------------
inline int& FPoint::y_ref()
{ return ypos; }


// FPoint non-member operators
//----------------------------------------------------------------------
inline bool operator == (const FPoint& p1, const FPoint& p2)
{ return p1.xpos == p2.xpos && p1.ypos == p2.ypos; }

//----------------------------------------------------------------------
inline bool operator != (const FPoint& p1, const FPoint& p2)
{ return p1.xpos != p2.xpos || p1.ypos != p2.ypos; }

//----------------------------------------------------------------------
inline FPoint operator + (const FPoint& p1, const FPoint& p2)
{ return {p1.xpos + p2.xpos, p1.ypos + p2.ypos}; }

//----------------------------------------------------------------------
inline FPoint operator - (const FPoint& p1, const FPoint& p2)
{ return {p1.xpos - p2.xpos, p1.ypos - p2.ypos}; }

//----------------------------------------------------------------------
inline FPoint operator - (const FPoint& p)
{ return {-p.xpos, -p.ypos}; }

}  // namespace finalcut

#endif  // FPOINT_H
