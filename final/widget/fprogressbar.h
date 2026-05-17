/***********************************************************************
* fprogressbar.h - Widget FProgressBar                                 *
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

/*  Inheritance diagram
 *  ═══════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▏
 * ▕ FVTerm  ▏ ▕ FObject ▏
 * ▕▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▏
 *      ▲           ▲
 *      │           │
 *      └─────┬─────┘
 *            │
 *       ▕▔▔▔▔▔▔▔▔▔▏
 *       ▕ FWidget ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏
 *            ▲
 *            │
 *     ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *     ▕ FProgressBar ▏
 *     ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FPROGRESSBAR_H
#define FPROGRESSBAR_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fwidget.h"

namespace finalcut
{

// class forward declaration
class FProgressBar;

// Legacy code compatibility
using FProgressbar = FProgressBar;

//----------------------------------------------------------------------
// class FProgressBar
//----------------------------------------------------------------------

class FProgressBar : public FWidget
{
  public:
    // Using-declarations
    using FWidget::setGeometry;

    // Constructor
    explicit FProgressBar(FWidget* = nullptr);

    // Destructor
    ~FProgressBar() noexcept override;

    // Accessors
    auto getClassName() const -> FString override;
    auto getPercentage() const noexcept -> std::size_t;

    // Mutators
    void setPercentage (std::size_t);
    void setSize (const FSize&, bool = true) override;
    void setGeometry (const FPoint&, const FSize&, bool = true) override;
    void setShadow (bool = true) ;
    void unsetShadow();

    // Predicate
    auto hasShadow() const -> bool;

    // Methods
    void hide() override;
    void reset();

  private:
    // Constants
    static constexpr auto NOT_SET = static_cast<std::size_t>(-1);

    // Methods
    void init();
    void draw() override;
    void drawProgressLabel();
    void drawProgressBar();
    auto drawProgressIndicator() -> std::size_t;
    void drawProgressBackground (std::size_t);

    // Data members
    std::size_t  percentage{NOT_SET};
    std::size_t  bar_length{getWidth()};
};


// FProgressBar inline functions
//----------------------------------------------------------------------
inline auto FProgressBar::getClassName() const -> FString
{ return "FProgressBar"; }

//----------------------------------------------------------------------
inline auto FProgressBar::getPercentage() const noexcept -> std::size_t
{ return percentage; }

//----------------------------------------------------------------------
inline void FProgressBar::unsetShadow()
{ setShadow(false); }

//----------------------------------------------------------------------
inline auto FProgressBar::hasShadow() const -> bool
{ return getFlags().shadow.shadow; }

}  // namespace finalcut

#endif  // FPROGRESSBAR_H
