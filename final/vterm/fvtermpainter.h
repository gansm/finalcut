/***********************************************************************
* fvtermpainter.h - Prints characters into regions                     *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2026 Markus Gans                                           *
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

/*  Helper class of FVTerm (internal)
 *  ═════════════════════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FVTermPainter ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *
 * FVTermPainter writes characters and character buffers into a region
 * and clears regions.  It holds no state of its own and works on the
 * FVTerm object it was created for.
 * This header is only meant to be included by fvterm.cpp.
 */

#ifndef FVTERMPAINTER_H
#define FVTERMPAINTER_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <vector>

#include "final/vterm/fvterm.h"
#include "final/vterm/fvtermbuffer.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FVTermPainter
//----------------------------------------------------------------------

class FVTermPainter
{
  public:
    // Using-declarations
    using FTermRegion = FVTerm::FTermRegion;

    // Constructor
    explicit FVTermPainter (const FVTerm& fvterm) noexcept
      : owner{fvterm}
    { }

    // Disable copy and move
    FVTermPainter (const FVTermPainter&) = delete;
    FVTermPainter (FVTermPainter&&) = delete;

    // Disable copy and move assignment operators (=)
    auto operator = (const FVTermPainter&) -> FVTermPainter& = delete;
    auto operator = (FVTermPainter&&) -> FVTermPainter& = delete;

    // Methods
    auto  print (FTermRegion*, const FVTermBuffer&) const noexcept -> int;
    auto  print (FTermRegion*, const FChar&) const noexcept -> int;
    auto  clearFullRegion (FTermRegion*, FChar&) const -> bool;

  private:
    // Using-declaration
    using FChar_iterator = std::vector<FChar>::iterator;

    // Methods
    auto  interpretControlCodes ( FTermRegion*
                                , FChar_iterator&
                                , const FChar&) const noexcept -> bool;
    auto  printComplexChar (FTermRegion*, FChar_iterator&, const FChar&) const noexcept -> int;
    void  printPaddingCharacter (FTermRegion*, const FChar&) const;

    // Data member
    const FVTerm&  owner;
};

}  // namespace finalcut

#endif  // FVTERMPAINTER_H
