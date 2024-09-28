/***********************************************************************
* fpoint.h - Point with an x and y coordinate                          *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2024 Markus Gans                                      *
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

#include "final/util/fstring.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FPoint
//----------------------------------------------------------------------

class FPoint
{
  public:
    // Constructors
    FPoint () noexcept = default;
    FPoint (int, int) noexcept;

    // Overloaded operators
    auto operator += (const FPoint&) -> FPoint&;
    auto operator -= (const FPoint&) -> FPoint&;

    // Accessors
    auto getClassName() const -> FString;
    auto getX() const noexcept -> int;
    auto getY() const noexcept -> int;
    void setX (int) noexcept;
    void setY (int) noexcept;
    void setPoint (const FPoint&) noexcept;
    void setPoint (int, int) noexcept;

    // Inquiry
    auto isOrigin() const noexcept -> bool;

    // Point references
    auto x_ref() & noexcept -> int&;
    auto y_ref() & noexcept -> int&;

    // Methods
    void move (int, int) noexcept;
    void move (const FPoint&) noexcept;

  private:
    // Data members
    int xpos{0};
    int ypos{0};

    // Friend operator functions
    friend inline auto operator == (const FPoint& p1, const FPoint& p2) -> bool
    {
      return p1.xpos == p2.xpos && p1.ypos == p2.ypos;
    }

    friend inline auto operator != (const FPoint& p1, const FPoint& p2) -> bool
    {
      return p1.xpos != p2.xpos || p1.ypos != p2.ypos;
    }

    friend inline auto operator + (const FPoint& p1, const FPoint& p2) -> FPoint
    {
      return {p1.xpos + p2.xpos, p1.ypos + p2.ypos};
    }

    friend inline auto operator - (const FPoint& p1, const FPoint& p2) -> FPoint
    {
      return {p1.xpos - p2.xpos, p1.ypos - p2.ypos};
    }

    friend inline auto operator - (const FPoint& p) -> FPoint
    {
      return {-p.xpos, -p.ypos};
    }

    friend inline auto operator << (std::ostream& outstr, const FPoint& p) -> std::ostream&
    {
      outstr << p.xpos << " " << p.ypos;
      return outstr;
    }

    friend inline auto operator >> (std::istream& instr, FPoint& p) -> std::istream&
    {
      int x{};
      int y{};
      instr >> x;
      instr >> y;
      p.setPoint (x, y);
      return instr;
    }
};


// FPoint inline functions
//----------------------------------------------------------------------
inline FPoint::FPoint (int x, int y) noexcept
  : xpos{x}
  , ypos{y}
{ }

//----------------------------------------------------------------------
inline auto FPoint::getClassName() const -> FString
{ return "FPoint"; }

//----------------------------------------------------------------------
inline auto FPoint::getX() const noexcept -> int
{ return xpos; }

//----------------------------------------------------------------------
inline auto FPoint::getY() const noexcept -> int
{ return ypos; }

//----------------------------------------------------------------------
inline void FPoint::setX (int x) noexcept
{ xpos = x; }

//----------------------------------------------------------------------
inline void FPoint::setY (int y) noexcept
{ ypos = y; }

//----------------------------------------------------------------------
inline void FPoint::setPoint (const FPoint& p) noexcept
{ setPoint(p.xpos, p.ypos); }

//----------------------------------------------------------------------
inline void FPoint::setPoint (int x, int y) noexcept
{
  xpos = x;
  ypos = y;
}

//----------------------------------------------------------------------
inline auto FPoint::isOrigin() const noexcept -> bool
{ return xpos == 0 && ypos == 0; }

//----------------------------------------------------------------------
inline auto FPoint::x_ref() & noexcept -> int&
{ return xpos; }

//----------------------------------------------------------------------
inline auto FPoint::y_ref() & noexcept -> int&
{ return ypos; }

//----------------------------------------------------------------------
inline void FPoint::move (int dx, int dy) noexcept
{
  xpos += dx;
  ypos += dy;
}

//----------------------------------------------------------------------
inline void FPoint::move (const FPoint& d) noexcept
{
  xpos += d.getX();
  ypos += d.getY();
}

}  // namespace finalcut

#endif  // FPOINT_H
