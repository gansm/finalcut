/***********************************************************************
* fcolorpalette.cpp - Define RGB color value for a palette entry       *
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

#include "final/fcolorpalette.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FColorPalette
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FColorPalette::FColorPalette()
{ }

//----------------------------------------------------------------------
FColorPalette::~FColorPalette()  // destructor
{ }


// public methods of FColorPalette
//----------------------------------------------------------------------
void FColorPalette::set8ColorPalette (funcp setPalette)
{
  setPalette (fc::Black, 0x00, 0x00, 0x00);
  setPalette (fc::Blue, 0x22, 0x22, 0xb2);
  setPalette (fc::Green, 0x18, 0x78, 0x18);
  setPalette (fc::Cyan, 0x66, 0x66, 0xff);
  setPalette (fc::Red, 0xb2, 0x18, 0x18);
  setPalette (fc::Magenta, 0xb2, 0x18, 0xb2);
  setPalette (fc::Brown, 0xe8, 0x87, 0x1f);
  setPalette (fc::LightGray, 0xe0, 0xe0, 0xe0);
  // The same colors again...
  setPalette (fc::DarkGray, 0x00, 0x00, 0x00);
  setPalette (fc::LightBlue, 0x22, 0x22, 0xb2);
  setPalette (fc::LightGreen, 0x18, 0x78, 0x18);
  setPalette (fc::LightCyan, 0x66, 0x66, 0xff);
  setPalette (fc::LightRed, 0xb2, 0x18, 0x18);
  setPalette (fc::LightMagenta, 0xb2, 0x18, 0xb2);
  setPalette (fc::Yellow, 0xe8, 0x87, 0x1f);
  setPalette (fc::White, 0xe0, 0xe0, 0xe0);
}

//----------------------------------------------------------------------
void FColorPalette::set16ColorPalette (funcp setPalette)
{
  setPalette (fc::Black, 0x00, 0x00, 0x00);
  setPalette (fc::Blue, 0x22, 0x22, 0xb2);
  setPalette (fc::Green, 0x18, 0x78, 0x18);
  setPalette (fc::Cyan, 0x4a, 0x4a, 0xe4);
  setPalette (fc::Red, 0xba, 0x1a, 0x1a);
  setPalette (fc::Magenta, 0xb2, 0x18, 0xb2);
  setPalette (fc::Brown, 0xe8, 0x87, 0x1f);
  setPalette (fc::LightGray, 0xbc, 0xbc, 0xbc);
  setPalette (fc::DarkGray, 0x50, 0x50, 0x50);
  setPalette (fc::LightBlue, 0x80, 0xa4, 0xec);
  setPalette (fc::LightGreen, 0x5e, 0xeb, 0x5c);
  setPalette (fc::LightCyan, 0x62, 0xbf, 0xf8);
  setPalette (fc::LightRed, 0xee, 0x44, 0x44);
  setPalette (fc::LightMagenta, 0xe9, 0xad, 0xff);
  setPalette (fc::Yellow, 0xfb, 0xe8, 0x67);
  setPalette (fc::White, 0xff, 0xff, 0xff);
}

//----------------------------------------------------------------------
void FColorPalette::reset8ColorPalette (funcp setPalette)
{
  setPalette (fc::Black, 0x00, 0x00, 0x00);
  setPalette (fc::Blue, 0x00, 0x00, 0xaa);
  setPalette (fc::Green, 0x00, 0xaa, 0x00);
  setPalette (fc::Cyan, 0x00, 0x55, 0xaa);
  setPalette (fc::Red, 0xaa, 0x00, 0x00);
  setPalette (fc::Magenta, 0xaa, 0x00, 0xaa);
  setPalette (fc::Brown, 0xaa, 0xaa, 0x00);
  setPalette (fc::LightGray, 0xaa, 0xaa, 0xaa);
  setPalette (fc::DarkGray, 0x55, 0x55, 0x55);
  setPalette (fc::LightBlue, 0x55, 0x55, 0xff);
  setPalette (fc::LightGreen, 0x55, 0xff, 0x55);
  setPalette (fc::LightCyan, 0x55, 0xff, 0xff);
  setPalette (fc::LightRed, 0xff, 0x55, 0x55);
  setPalette (fc::LightMagenta, 0xff, 0x55, 0xff);
  setPalette (fc::Yellow, 0xff, 0xff, 0x55);
  setPalette (fc::White, 0xff, 0xff, 0xff);
}

//----------------------------------------------------------------------
void FColorPalette::reset16ColorPalette (funcp setPalette)
{
  setPalette (fc::Black, 0x00, 0x00, 0x00);
  setPalette (fc::Blue, 0x00, 0x00, 0xaa);
  setPalette (fc::Green, 0x00, 0xaa, 0x00);
  setPalette (fc::Cyan, 0x00, 0x55, 0xaa);
  setPalette (fc::Red, 0xaa, 0x00, 0x00);
  setPalette (fc::Magenta, 0xaa, 0x00, 0xaa);
  setPalette (fc::Brown, 0xaa, 0xaa, 0x00);
  setPalette (fc::LightGray, 0xaa, 0xaa, 0xaa);
  setPalette (fc::DarkGray, 0x55, 0x55, 0x55);
  setPalette (fc::LightBlue, 0x55, 0x55, 0xff);
  setPalette (fc::LightGreen, 0x55, 0xff, 0x55);
  setPalette (fc::LightCyan, 0x55, 0xff, 0xff);
  setPalette (fc::LightRed, 0xff, 0x55, 0x55);
  setPalette (fc::LightMagenta, 0xff, 0x55, 0xff);
  setPalette (fc::Yellow, 0xff, 0xff, 0x55);
  setPalette (fc::White, 0xff, 0xff, 0xff);
}

}  // namespace finalcut
