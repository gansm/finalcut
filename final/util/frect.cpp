/***********************************************************************
* frect.cpp - Rectangle with position and size                         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2023 Markus Gans                                      *
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

#include "final/util/fpoint.h"
#include "final/util/frect.h"
#include "final/util/fsize.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FRect
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FRect::FRect (const FPoint& p, const FSize& s) noexcept
  : X1{p.getX()}
  , Y1{p.getY()}
  , X2{p.getX() + int(s.getWidth()) - 1}
  , Y2{p.getY() + int(s.getHeight()) - 1}
{ }

//----------------------------------------------------------------------
FRect::FRect (const FPoint& p1, const FPoint& p2) noexcept
  : X1{p1.getX()}
  , Y1{p1.getY()}
  , X2{p2.getX()}
  , Y2{p2.getY()}
{ }


// public methods of FRect
//----------------------------------------------------------------------
auto FRect::intersect (const FRect& r) const noexcept -> FRect
{
  // intersection: this ∩ r
  int p1_x = std::max(X1, r.X1);
  int p1_y = std::max(Y1, r.Y1);
  int p2_x = std::min(X2, r.X2);
  int p2_y = std::min(Y2, r.Y2);
  const FPoint p1{ p1_x, p1_y };
  const FPoint p2{ p2_x, p2_y };
  return { p1, p2 };
}

//----------------------------------------------------------------------
auto FRect::combined (const FRect& r) const noexcept -> FRect
{
  // Union: this ∪ r
  int p1_x = std::min(X1, r.X1);
  int p1_y = std::min(Y1, r.Y1);
  int p2_x = std::max(X2, r.X2);
  int p2_y = std::max(Y2, r.Y2);
  const FPoint p1{ p1_x, p1_y };
  const FPoint p2{ p2_x, p2_y };
  return { p1, p2 };
}

}  // namespace finalcut
