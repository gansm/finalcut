/***********************************************************************
* fvtermcompositor.h - Composes regions into the virtual terminal      *
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
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FVTermCompositor ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *
 * FVTermCompositor merges the changed regions of all windows (layers)
 * into the virtual terminal.  It holds no state of its own: the buffers
 * and mutexes remain data members of the FVTerm object it works on.
 * This header is only meant to be included by fvterm.cpp.
 */

#ifndef FVTERMCOMPOSITOR_H
#define FVTERMCOMPOSITOR_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <vector>

#include "final/util/fpoint.h"
#include "final/vterm/fvterm.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FVTermCompositor
//----------------------------------------------------------------------

class FVTermCompositor
{
  public:
    // Using-declarations
    using FTermRegion = FVTerm::FTermRegion;

    // Constructor
    explicit FVTermCompositor (const FVTerm& fvterm) noexcept
      : owner{fvterm}
    { }

    // Disable copy and move
    FVTermCompositor (const FVTermCompositor&) = delete;
    FVTermCompositor (FVTermCompositor&&) = delete;

    // Disable copy and move assignment operators (=)
    auto operator = (const FVTermCompositor&) -> FVTermCompositor& = delete;
    auto operator = (FVTermCompositor&&) -> FVTermCompositor& = delete;

    // Methods
    void  addLayer (FTermRegion*) const noexcept;
    void  copyRegion (FTermRegion*, const FPoint&, FTermRegion*) const noexcept;
    auto  isCovered (const FPoint&, const FTermRegion*) const noexcept -> FVTerm::CoveredState;
    void  updateVTerm() const;

  private:
    // Using-declarations
    using FChar_iterator = std::vector<FChar>::iterator;
    using FChar_const_iterator = std::vector<FChar>::const_iterator;
    using FTermRegionList = FVTerm::FTermRegionList;
    using LayerGeometry = FVTerm::LayerGeometry;
    using RegionRenderContext = FVTerm::RegionRenderContext;
    using NoTrans = FVTerm::NoTrans;
    using CoveredState = FVTerm::CoveredState;
    using SearchState = FVTerm::SearchState;
    using FLineChangesBatch = FVTerm::FLineChangesBatch;
    using FVTermList = FVTerm::FVTermList;

    // Methods
    void  passChangesToOverlap (const FTermRegion*) const;
    void  processOverlappingWindows (const FTermRegion*, const FVTermList&) const;
    void  passChangesToOverlappingWindow (FTermRegion*, const FTermRegion*) const noexcept;
    void  passChangesToOverlappingWindowLine (FTermRegion*, int, const FTermRegion*) const noexcept;
    void  updateVTermDesktop() const;
    void  updateVTermWindow (FTermRegion*) const;
    auto  computeLayerGeometry (const FTermRegion*) const noexcept -> LayerGeometry;
    auto  isLayerOutsideVTerm (const LayerGeometry&) const noexcept -> bool;
    void  buildLineChangeBatch ( const FTermRegion*
                               , const LayerGeometry&
                               , FLineChangesBatch&) const noexcept;
    void  applyLineBatch ( FTermRegion*
                         , const LayerGeometry&
                         , const FLineChangesBatch&) const noexcept;
    void  updateVTermChangesFromBatch ( const LayerGeometry&
                                      , FLineChangesBatch& ) const noexcept;
    void  callPreprocessingHandler (const FTermRegion*) const;
    auto  hasChildRegionChanges (const FTermRegion*) const -> bool;
    void  clearChildRegionChanges (const FTermRegion*) const;
    void  putMultiLayerRegionLine (FChar_iterator, const int, const FPoint&) const noexcept;
    void  processCharState (SearchState&, const FChar&, FChar&) const noexcept;
    void  putRegionLineWithTransparency (RegionRenderContext&, const int) const;
    void  processAndFlushRegion (RegionRenderContext&, bool, FChar_const_iterator) const;
    void  addVDesktopToListIfExists (FTermRegionList&) const;
    void  determineCoveredRegions (FTermRegion*) const;

    // Data member
    const FVTerm&  owner;
};

}  // namespace finalcut

#endif  // FVTERMCOMPOSITOR_H
