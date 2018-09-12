/***********************************************************************
* fcolorpalette.h - Define RGB color value for a palette entry         *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2018 Markus Gans                                           *
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

/*  Standalone class
 *  ════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FColorPalette ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FCOLORPALETTE_H
#define FCOLORPALETTE_H

#include "final/fc.h"


//----------------------------------------------------------------------
// class FColorPalette
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FColorPalette
{
  public:
    // Constructor
    explicit FColorPalette();

    // Destructor
    virtual ~FColorPalette();

    // Typedefs
    typedef void (*funcp)(short, int, int, int);

    // Accessor
    virtual const char* getClassName() const;

    // Methods
    static void set8ColorPalette (funcp);
    static void set16ColorPalette (funcp);
    static void reset8ColorPalette (funcp);
    static void reset16ColorPalette (funcp);
};
#pragma pack(pop)

// FColorPalette inline functions
//----------------------------------------------------------------------
inline const char* FColorPalette::getClassName() const
{ return "FColorPalette"; }

#endif  // FCOLORPALETTE_H
