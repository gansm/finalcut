/***********************************************************************
* fpoint.cpp - Point with an x and y coordinate                        *
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

#include "final/fpoint.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FPoint
//----------------------------------------------------------------------

FPoint::~FPoint()  // destructor
{ }

// public methods of FPoint
//----------------------------------------------------------------------
FPoint& FPoint::operator = (const FPoint& p)
{
  xpos = p.xpos;
  ypos = p.ypos;
  return *this;
}

//----------------------------------------------------------------------
FPoint& FPoint::operator += (const FPoint& p)
{
  xpos = xpos + p.xpos;
  ypos = ypos + p.ypos;
  return *this;
}

//----------------------------------------------------------------------
FPoint& FPoint::operator -= (const FPoint& p)
{
  xpos = xpos - p.xpos;
  ypos = ypos - p.ypos;
  return *this;
}

//----------------------------------------------------------------------
void FPoint::setX (int x)
{
  xpos = x;
}

//----------------------------------------------------------------------
void FPoint::setY (int y)
{
  ypos = y;
}

//----------------------------------------------------------------------
void FPoint::setPoint (int x, int y)
{
  xpos = x;
  ypos = y;
}

//----------------------------------------------------------------------
bool FPoint::isNull() const
{
  return xpos == 0 && ypos == 0;
}

//----------------------------------------------------------------------
std::ostream& operator << (std::ostream& outstr, const FPoint& p)
{
  outstr << p.getX() << " " << p.getY();
  return outstr;
}

//----------------------------------------------------------------------
std::istream& operator >> (std::istream& instr, FPoint& p)
{
  int x, y;
  instr >> x;
  instr >> y;
  p.setPoint (x, y);
  return instr;
}

}  // namespace finalcut
