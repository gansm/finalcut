/***********************************************************************
* fprogressbar.h - Widget FProgressbar                                 *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2020 Markus Gans                                      *
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
 *     ▕ FProgressbar ▏
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

//----------------------------------------------------------------------
// class FProgressbar
//----------------------------------------------------------------------

class FProgressbar : public FWidget
{
  public:
    // Using-declarations
    using FWidget::setGeometry;

    // Constructor
    explicit FProgressbar(FWidget* = nullptr);

    // Destructor
    ~FProgressbar() override;

    // Accessors
    const FString       getClassName() const override;
    std::size_t         getPercentage() const;

    // Mutators
    void                setPercentage (std::size_t);
    void                setSize (const FSize&, bool = true) override;
    void                setGeometry ( const FPoint&, const FSize&
                                    , bool = true ) override;
    bool                setShadow (bool);
    bool                setShadow();
    bool                unsetShadow();

    // Inquiries
    bool                hasShadow() const;

    // Methods
    void                hide() override;
    void                reset();

  private:
    // Constants
    static constexpr std::size_t NOT_SET = static_cast<std::size_t>(-1);

    // Methods
    void                init();
    void                draw() override;
    void                drawProgressLabel();
    void                drawProgressBar();
    std::size_t         drawProgressIndicator();
    void                drawProgressBackground (std::size_t);

    // Data members
    std::size_t  percentage{NOT_SET};
    std::size_t  bar_length{getWidth()};
};


// FProgressbar inline functions
//----------------------------------------------------------------------
inline const FString FProgressbar::getClassName() const
{ return "FProgressbar"; }

//----------------------------------------------------------------------
inline std::size_t FProgressbar::getPercentage() const
{ return percentage; }

//----------------------------------------------------------------------
inline bool FProgressbar::setShadow()
{ return setShadow(true); }

//----------------------------------------------------------------------
inline bool FProgressbar::unsetShadow()
{ return setShadow(false); }

//----------------------------------------------------------------------
inline bool FProgressbar::hasShadow() const
{ return getFlags().shadow; }

}  // namespace finalcut

#endif  // FPROGRESSBAR_H
