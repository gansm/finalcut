/***********************************************************************
* fpoint.cpp - Point with an x and y coordinate                        *
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

#include <utility>

#include "final/util/fpoint.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FPoint
//----------------------------------------------------------------------

// public methods of FPoint
//----------------------------------------------------------------------
auto FPoint::operator += (const FPoint& p) -> FPoint&
{
  xpos += p.xpos;
  ypos += p.ypos;
  return *this;
}

//----------------------------------------------------------------------
auto FPoint::operator -= (const FPoint& p) -> FPoint&
{
  xpos -= p.xpos;
  ypos -= p.ypos;
  return *this;
}

//----------------------------------------------------------------------
void FPoint::setX (int x) noexcept
{
  xpos = x;
}

//----------------------------------------------------------------------
void FPoint::setY (int y) noexcept
{
  ypos = y;
}

//----------------------------------------------------------------------
void FPoint::setPoint (int x, int y) noexcept
{
  xpos = x;
  ypos = y;
}

//----------------------------------------------------------------------
auto FPoint::isOrigin() const noexcept -> bool
{
  return xpos == 0 && ypos == 0;
}

//----------------------------------------------------------------------
void FPoint::move (int dx, int dy) noexcept
{
  xpos += dx;
  ypos += dy;
}

//----------------------------------------------------------------------
void FPoint::move (const FPoint& d)
{
  xpos += d.getX();
  ypos += d.getY();
}

//----------------------------------------------------------------------
auto operator << (std::ostream& outstr, const FPoint& p) -> std::ostream&
{
  outstr << p.xpos << " " << p.ypos;
  return outstr;
}

//----------------------------------------------------------------------
auto operator >> (std::istream& instr, FPoint& p) -> std::istream&
{
  int x{};
  int y{};
  instr >> x;
  instr >> y;
  p.setPoint (x, y);
  return instr;
}

}  // namespace finalcut
