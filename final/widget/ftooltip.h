/***********************************************************************
* ftooltip.h - Widget FToolTip                                         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2016-2024 Markus Gans                                      *
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
 *       ▕▔▔▔▔▔▔▔▔▔▏
 *       ▕ FWindow ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏
 *            ▲
 *            │
 *       ▕▔▔▔▔▔▔▔▔▔▔▏
 *       ▕ FToolTip ▏
 *       ▕▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FTOOLTIP_H
#define FTOOLTIP_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <vector>

#include "final/widget/fwindow.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FToolTip
//----------------------------------------------------------------------

class FToolTip : public FWindow
{
  public:
    // Constructors
    explicit FToolTip (FWidget* = nullptr);
    explicit FToolTip (const FString&, FWidget* = nullptr);

    // Destructor
    ~FToolTip () override;

    // Accessors
    auto getClassName() const -> FString override;
    auto getText() const -> FString;

    // Mutators
    void setText (const FString&);
    void enableAutoTrim();
    void disableAutoTrim();
    void resetColors() override;
    void setBorder (bool = true);
    void unsetBorder();

    // Inquiries
    auto hasBorder() const -> bool;

    // Methods
    void show() override;

    // Event handler
    void onMouseDown (FMouseEvent*) override;

  private:
    // Methods
    void init();
    void draw() override;
    void calculateDimensions();
    auto getCalculatedSize() const -> FSize;
    auto getCalculatedPosition (FSize) const -> FPoint;
    void initLayout() override;
    void adjustSize() override;

    // Data members
    FString       text{};
    FStringList   text_components{};
    bool          text_auto_trim{true};
    std::size_t   max_line_width{0};
    std::size_t   text_num_lines{0};
};


// FToolTip inline functions
//----------------------------------------------------------------------
inline auto FToolTip::getClassName() const -> FString
{ return "FToolTip"; }

//----------------------------------------------------------------------
inline auto FToolTip::getText() const -> FString
{ return text; }

//----------------------------------------------------------------------
inline void FToolTip::enableAutoTrim()
{ text_auto_trim = true; }

//----------------------------------------------------------------------
inline void FToolTip::disableAutoTrim()
{ text_auto_trim = false; }

//----------------------------------------------------------------------
inline void FToolTip::unsetBorder()
{ setBorder(false); }

//----------------------------------------------------------------------
inline auto FToolTip::hasBorder() const -> bool
{ return ! getFlags().feature.no_border; }

}  // namespace finalcut

#endif  // FTOOLTIP_H
