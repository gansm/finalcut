/***********************************************************************
* fpoint.h - Point with an x and y coordinate                          *
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
#include "final/ftypes.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FPoint
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FPoint
{
  public:
    // Constructors
    FPoint ();
    FPoint (const FPoint&);  // copy constructor
    FPoint (int, int);

    // Destructor
    virtual ~FPoint();

    // Overloaded operators
    FPoint& operator =  (const FPoint&);
    FPoint& operator += (const FPoint&);
    FPoint& operator -= (const FPoint&);

    friend bool   operator == (const FPoint&, const FPoint&);
    friend bool   operator != (const FPoint&, const FPoint&);
    friend FPoint operator +  (const FPoint&, const FPoint&);
    friend FPoint operator -  (const FPoint&, const FPoint&);
    friend FPoint operator -  (const FPoint&);
    friend std::ostream& operator << (std::ostream&, const FPoint&);
    friend std::istream& operator >> (std::istream&, FPoint&);

    // Accessors
    virtual const char* getClassName();
    int     getX() const;
    int     getY() const;
    void    setX (int);
    void    setY (int);
    void    setPoint (int, int);

    // Inquiry
    bool    isNull() const;

    // Point references
    int& x_ref();
    int& y_ref();

  private:
    // Data Members
    int xpos;
    int ypos;
};
#pragma pack(pop)


// FPoint inline functions
//----------------------------------------------------------------------
inline FPoint::FPoint()
  : xpos(0)
  , ypos(0)
{ }

//----------------------------------------------------------------------
inline FPoint::FPoint (const FPoint& p)  // copy constructor
  : xpos(p.xpos)
  , ypos(p.ypos)
{ }

//----------------------------------------------------------------------
inline FPoint::FPoint (int x, int y)
  : xpos(x)
  , ypos(y)
{ }

//----------------------------------------------------------------------
inline bool operator == (const FPoint& p1, const FPoint& p2)
{ return p1.xpos == p2.xpos && p1.ypos == p2.ypos; }

//----------------------------------------------------------------------
inline bool operator != (const FPoint& p1, const FPoint& p2)
{ return p1.xpos != p2.xpos || p1.ypos != p2.ypos; }

//----------------------------------------------------------------------
inline FPoint operator + (const FPoint& p1, const FPoint& p2)
{ return FPoint(p1.xpos + p2.xpos, p1.ypos + p2.ypos); }

//----------------------------------------------------------------------
inline FPoint operator - (const FPoint& p1, const FPoint& p2)
{ return FPoint(p1.xpos - p2.xpos, p1.ypos - p2.ypos); }

//----------------------------------------------------------------------
inline FPoint operator - (const FPoint& p)
{ return FPoint(-p.xpos, -p.ypos); }

//----------------------------------------------------------------------
inline const char* FPoint::getClassName()
{ return "FPoint"; }

//----------------------------------------------------------------------
inline int FPoint::getX() const
{ return xpos; }

//----------------------------------------------------------------------
inline int FPoint::getY() const
{ return ypos; }

//----------------------------------------------------------------------
inline int& FPoint::x_ref()
{ return xpos; }

//----------------------------------------------------------------------
inline int& FPoint::y_ref()
{ return ypos; }

}  // namespace finalcut

#endif  // FPOINT_H
