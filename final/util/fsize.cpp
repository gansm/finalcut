/***********************************************************************
* fsize.cpp - Height and width of a two-dimensional surface            *
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
auto FSize::operator += (const FSize& s) -> FSize&
{
  constexpr std::size_t max = std::numeric_limits<std::size_t>::max();
  width = ( width < max - s.width) ? width + s.width : max;
  height = ( height < max - s.height) ? height + s.height : max;
  return *this;
}

//----------------------------------------------------------------------
auto FSize::operator -= (const FSize& s) -> FSize&
{
  width = ( width >= s.width ) ? width - s.width : 0;
  height = ( height >= s.height ) ? height - s.height : 0;
  return *this;
}

//----------------------------------------------------------------------
void FSize::setWidth (std::size_t w) noexcept
{
  width = w;
}

//----------------------------------------------------------------------
void FSize::setHeight (std::size_t h) noexcept
{
  height = h;
}

//----------------------------------------------------------------------
void FSize::setSize (const FSize& s)
{
  width = s.width;
  height = s.height;
}

//----------------------------------------------------------------------
void FSize::setSize (std::size_t w, std::size_t h) noexcept
{
  width = w;
  height = h;
}

//----------------------------------------------------------------------
auto FSize::isEmpty() const noexcept -> bool
{
  return width == 0 && height == 0;
}

//----------------------------------------------------------------------
void FSize::scaleBy (int dx, int dy) noexcept
{
  if ( dx < 0 )
  {
    if ( std::size_t(-dx) < width )
      width -= std::size_t(-dx);
    else
      width = std::size_t(-dx) - width;
  }
  else
    width += std::size_t(dx);

  if ( dy < 0 )
  {
    if ( std::size_t(-dy) < height )
      height -= std::size_t(-dy);
    else
      height = std::size_t(-dy) - height;
  }
  else
    height += std::size_t(dy);
}

//----------------------------------------------------------------------
void FSize::scaleBy (const FPoint& d)
{
  scaleBy (d.getX(), d.getY());
}

// FSize non-member operators
//----------------------------------------------------------------------
auto operator << (std::ostream& outstr, const FSize& s) -> std::ostream&
{
  outstr << s.width << " " << s.height;
  return outstr;
}

//----------------------------------------------------------------------
auto operator >> (std::istream& instr, FSize& s) -> std::istream&
{
  std::size_t w;
  std::size_t h;
  instr >> w;
  instr >> h;
  s.setSize (w, h);
  return instr;
}

}  // namespace finalcut
