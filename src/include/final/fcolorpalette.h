/***********************************************************************
* fcolorpalette.h - Define RGB color value for a palette entry         *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2018-2020 Markus Gans                                      *
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

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <functional>

#include "final/fstring.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FColorPalette
//----------------------------------------------------------------------

class FColorPalette final
{
  public:
    // Constructor
    FColorPalette() = default;

    // Destructor
    ~FColorPalette();

    // Accessor
    const FString getClassName() const;

    // Methods
    template<typename funcT>
    static void set8ColorPalette (funcT);
    template<typename funcT>
    static void set16ColorPalette (funcT);
    template<typename funcT>
    static void reset8ColorPalette (funcT);
    template<typename funcT>
    static void reset16ColorPalette (funcT);
};

// FColorPalette inline functions
//----------------------------------------------------------------------
inline const FString FColorPalette::getClassName() const
{ return "FColorPalette"; }

// constructors and destructor
//----------------------------------------------------------------------
inline FColorPalette::~FColorPalette()  // destructor
{ }

// public methods of FColorPalette
//----------------------------------------------------------------------
template<typename funcT>
void FColorPalette::set8ColorPalette (funcT set_palette)
{
  set_palette (fc::Black, 0x00, 0x00, 0x00);
  set_palette (fc::Blue, 0x10, 0x3b, 0x9e);
  set_palette (fc::Green, 0x18, 0x78, 0x18);
  set_palette (fc::Cyan, 0xa0, 0xb2, 0xb2);
  set_palette (fc::Red, 0xb2, 0x18, 0x18);
  set_palette (fc::Magenta, 0xb2, 0x18, 0xb2);
  set_palette (fc::Brown, 0xe8, 0x87, 0x1f);
  set_palette (fc::LightGray, 0xe0, 0xe0, 0xe0);
  // The same colors again...
  set_palette (fc::DarkGray, 0x00, 0x00, 0x00);
  set_palette (fc::LightBlue, 0x10, 0x3b, 0x9e);
  set_palette (fc::LightGreen, 0x18, 0x78, 0x18);
  set_palette (fc::Cyan, 0xa0, 0xb2, 0xb2);
  set_palette (fc::LightRed, 0xb2, 0x18, 0x18);
  set_palette (fc::LightMagenta, 0xb2, 0x18, 0xb2);
  set_palette (fc::Yellow, 0xe8, 0x87, 0x1f);
  set_palette (fc::White, 0xe0, 0xe0, 0xe0);
}

//----------------------------------------------------------------------
template<typename funcT>
void FColorPalette::set16ColorPalette (funcT set_palette)
{
  set_palette (fc::Black, 0x00, 0x00, 0x00);
  set_palette (fc::Blue, 0x10, 0x3b, 0x9e);
  set_palette (fc::Green, 0x18, 0x78, 0x18);
  set_palette (fc::Cyan, 0x55, 0x6a, 0xcf);
  set_palette (fc::Red, 0xba, 0x1a, 0x1a);
  set_palette (fc::Magenta, 0xb2, 0x18, 0xb2);
  set_palette (fc::Brown, 0xe8, 0x87, 0x1f);
  set_palette (fc::LightGray, 0xbc, 0xbc, 0xbc);
  set_palette (fc::DarkGray, 0x50, 0x50, 0x50);
  set_palette (fc::LightBlue, 0x80, 0xa4, 0xec);
  set_palette (fc::LightGreen, 0x5e, 0xeb, 0x5c);
  set_palette (fc::LightCyan, 0x62, 0xbf, 0xf8);
  set_palette (fc::LightRed, 0xee, 0x44, 0x44);
  set_palette (fc::LightMagenta, 0xe9, 0xad, 0xff);
  set_palette (fc::Yellow, 0xfb, 0xe8, 0x67);
  set_palette (fc::White, 0xff, 0xff, 0xff);
}

//----------------------------------------------------------------------
template<typename funcT>
void FColorPalette::reset8ColorPalette (funcT set_palette)
{
  reset16ColorPalette(set_palette);
}

//----------------------------------------------------------------------
template<typename funcT>
void FColorPalette::reset16ColorPalette (funcT set_palette)
{
  set_palette (fc::Black, 0x00, 0x00, 0x00);
  set_palette (fc::Blue, 0x00, 0x00, 0xaa);
  set_palette (fc::Green, 0x00, 0xaa, 0x00);
  set_palette (fc::Cyan, 0x00, 0x55, 0xaa);
  set_palette (fc::Red, 0xaa, 0x00, 0x00);
  set_palette (fc::Magenta, 0xaa, 0x00, 0xaa);
  set_palette (fc::Brown, 0xaa, 0xaa, 0x00);
  set_palette (fc::LightGray, 0xaa, 0xaa, 0xaa);
  set_palette (fc::DarkGray, 0x55, 0x55, 0x55);
  set_palette (fc::LightBlue, 0x55, 0x55, 0xff);
  set_palette (fc::LightGreen, 0x55, 0xff, 0x55);
  set_palette (fc::LightCyan, 0x55, 0xff, 0xff);
  set_palette (fc::LightRed, 0xff, 0x55, 0x55);
  set_palette (fc::LightMagenta, 0xff, 0x55, 0xff);
  set_palette (fc::Yellow, 0xff, 0xff, 0x55);
  set_palette (fc::White, 0xff, 0xff, 0xff);
}

}  // namespace finalcut

#endif  // FCOLORPALETTE_H
