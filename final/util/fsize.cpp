/***********************************************************************
* fsize.cpp - Height and width of a two-dimensional surface            *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2025 Markus Gans                                      *
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

#include <limits>
#include <utility>

#include "final/util/fpoint.h"
#include "final/util/fsize.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FSize
//----------------------------------------------------------------------

// public methods of FSize
//----------------------------------------------------------------------
auto FSize::operator += (const FSize& s) noexcept -> FSize&
{
  static constexpr size_type max = std::numeric_limits<size_type>::max();
  width = ( width < max - s.width) ? width + s.width : max;
  height = ( height < max - s.height) ? height + s.height : max;
  return *this;
}

//----------------------------------------------------------------------
auto FSize::operator -= (const FSize& s) noexcept -> FSize&
{
  width = ( width >= s.width ) ? width - s.width : 0;
  height = ( height >= s.height ) ? height - s.height : 0;
  return *this;
}

//----------------------------------------------------------------------
void FSize::scaleBy (int dx, int dy) noexcept
{
  if ( dx < 0 )
  {
    const auto minus_dx = size_type(-dx);
    width = ( minus_dx < width )
            ? width - minus_dx
            : minus_dx - width;
  }
  else
    width += size_type(dx);

  if ( dy < 0 )
  {
    const auto minus_dy = size_type(-dy);
    height = ( minus_dy < height )
             ? height - minus_dy
             : minus_dy - height;
  }
  else
    height += size_type(dy);
}

//----------------------------------------------------------------------
void FSize::scaleBy (const FPoint& d) noexcept
{
  scaleBy (d.getX(), d.getY());
}

}  // namespace finalcut
