/***********************************************************************
* fprogressbar.h - Widget FProgressbar                                 *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2014-2018 Markus Gans                                      *
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

#pragma pack(push)
#pragma pack(1)

class FProgressbar : public FWidget
{
  public:
    // Using-declarations
    using FWidget::setGeometry;

    // Constructor
    explicit FProgressbar(FWidget* = 0);

    // Destructor
    virtual ~FProgressbar();

    // Accessors
    const char*  getClassName() const;
    std::size_t  getPercentage();

    // Mutators
    void         setPercentage (std::size_t);
    virtual void setGeometry (int, int, std::size_t, std::size_t, bool = true);
    bool         setShadow (bool);
    bool         setShadow();
    bool         unsetShadow();

    // Inquiries
    bool         hasShadow();

    // Methods
    virtual void hide();
    void         reset();

  private:
    // Constants
    static const std::size_t NOT_SET = static_cast<std::size_t>(-1);

    // Methods
    virtual void draw();
    void         drawPercentage();
    void         drawBar();

    // Data Members
    std::size_t  percentage;
    std::size_t  bar_length;
};
#pragma pack(pop)


// FProgressbar inline functions
//----------------------------------------------------------------------
inline const char* FProgressbar::getClassName() const
{ return "FProgressbar"; }

//----------------------------------------------------------------------
inline std::size_t FProgressbar::getPercentage()
{ return percentage; }

//----------------------------------------------------------------------
inline bool FProgressbar::setShadow()
{ return setShadow(true); }

//----------------------------------------------------------------------
inline bool FProgressbar::unsetShadow()
{ return setShadow(false); }

//----------------------------------------------------------------------
inline bool FProgressbar::hasShadow()
{ return flags.shadow; }

}  // namespace finalcut

#endif  // FPROGRESSBAR_H
