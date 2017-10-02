/************************************************************************
* fpoint.h - Point with an x and y coordinate                           *
*                                                                       *
* This file is part of the Final Cut widget toolkit                     *
*                                                                       *
* Copyright 2014-2017 Markus Gans                                       *
*                                                                       *
* The Final Cut is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by  *
* the Free Software Foundation; either version 3 of the License, or     *
* (at your option) any later version.                                   *
*                                                                       *
* The Final Cut is distributed in the hope that it will be useful,      *
* but WITHOUT ANY WARRANTY; without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
* GNU General Public License for more details.                          *
*                                                                       *
* You should have received a copy of the GNU General Public License     *
* along with this program.  If not, see <http://www.gnu.org/licenses/>. *
************************************************************************/

/*  Standalone class
 *  ════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▏
 * ▕ FPoint ▏
 * ▕▁▁▁▁▁▁▁▁▏
 */

#ifndef FPOINT_H
#define FPOINT_H

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

    friend inline bool   operator == (const FPoint&, const FPoint&);
    friend inline bool   operator != (const FPoint&, const FPoint&);
    friend inline FPoint operator +  (const FPoint&, const FPoint&);
    friend inline FPoint operator -  (const FPoint&, const FPoint&);
    friend inline FPoint operator -  (const FPoint&);

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
    short&  x_ref();
    short&  y_ref();

  private:
    // Data Members
    short xpos;
    short ypos;
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
  : xpos(short(x))
  , ypos(short(y))
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
inline short& FPoint::x_ref()
{ return xpos; }

//----------------------------------------------------------------------
inline short& FPoint::y_ref()
{ return ypos; }

#endif  // FPOINT_H
