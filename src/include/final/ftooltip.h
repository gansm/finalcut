/***********************************************************************
* ftooltip.h - Widget FToolTip                                         *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2016-2018 Markus Gans                                      *
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

#pragma pack(push)
#pragma pack(1)

class FToolTip : public FWindow
{
  public:
    // Constructor
    explicit FToolTip (FWidget* = 0);
    explicit FToolTip (const FString&, FWidget* = 0);

    // Destructor
    virtual ~FToolTip ();

    // Accessors
    const char*   getClassName() const;
    const FString getText() const;

    // Mutators
    void          setText (const FString&);

    // Methods
    virtual void  draw();
    virtual void  show();
    virtual void  hide();

    // Event handler
    virtual void  onMouseDown (FMouseEvent*);

  private:
    // Disable copy constructor
    FToolTip (const FToolTip&);

    // Disable assignment operator (=)
    FToolTip& operator = (const FToolTip&);

    // Methods
    void          init();
    void          calculateDimensions();
    virtual void  adjustSize();

    // Data Members
    FString       text;
    FString*      text_components;
    FStringList   text_split;
    std::size_t   max_line_width;
    std::size_t   text_num_lines;
};
#pragma pack(pop)


// FToolTip inline functions
//----------------------------------------------------------------------
inline const char* FToolTip::getClassName() const
{ return "FToolTip"; }

}  // namespace finalcut

#endif  // FTOOLTIP_H
