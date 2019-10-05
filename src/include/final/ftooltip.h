/***********************************************************************
* ftooltip.h - Widget FToolTip                                         *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2016-2019 Markus Gans                                      *
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

/*  Inheritance diagram
 *  ═══════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▏
 * ▕  FTerm  ▏
 * ▕▁▁▁▁▁▁▁▁▁▏
 *      ▲
 *      │
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

#include "final/fwindow.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FToolTip
//----------------------------------------------------------------------

class FToolTip : public FWindow
{
  public:
    // Constructor
    explicit FToolTip (FWidget* = nullptr);
    explicit FToolTip (const FString&, FWidget* = nullptr);

    // Disable copy constructor
    FToolTip (const FToolTip&) = delete;

    // Destructor
    virtual ~FToolTip ();

    // Disable assignment operator (=)
    FToolTip& operator = (const FToolTip&) = delete;

    // Accessors
    const FString       getClassName() const override;
    const FString       getText() const;

    // Mutators
    void                setText (const FString&);

    // Methods
    void                draw() override;
    void                show() override;
    void                hide() override;

    // Event handler
    void                onMouseDown (FMouseEvent*) override;

  private:
    // Methods
    void                init();
    void                calculateDimensions();
    void                adjustSize() override;

    // Data members
    FString       text{};
    FStringList   text_components{};
    std::size_t   max_line_width{0};
    std::size_t   text_num_lines{0};
};


// FToolTip inline functions
//----------------------------------------------------------------------
inline const FString FToolTip::getClassName() const
{ return "FToolTip"; }

}  // namespace finalcut

#endif  // FTOOLTIP_H
