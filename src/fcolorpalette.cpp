/***********************************************************************
* fcolorpalette.cpp - Define RGB color value for a palette entry       *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2020 Markus Gans                                           *
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
FColorPalette::FColorPalette (const FSetPalette& f)
  : set_palette{f}
{ }

//----------------------------------------------------------------------
FColorPalette::~FColorPalette()  // destructor
{ }


// protected methods of FColorPalette
//----------------------------------------------------------------------
void FColorPalette::setPalette (FColor index, int r, int g, int b)
{
  set_palette (index, r, g, b);
}

//----------------------------------------------------------------------
void FColorPalette::setVGAdefaultPalette()
{
  setPalette (fc::Black, 0x00, 0x00, 0x00);
  setPalette (fc::Blue, 0x00, 0x00, 0xaa);
  setPalette (fc::Green, 0x00, 0xaa, 0x00);
  setPalette (fc::Cyan, 0x00, 0xaa, 0xaa);
  setPalette (fc::Red, 0xaa, 0x00, 0x00);
  setPalette (fc::Magenta, 0xaa, 0x00, 0xaa);
  setPalette (fc::Brown, 0xaa, 0x55, 0x00);
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
// class default8ColorPalette
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
default8ColorPalette::default8ColorPalette (const FSetPalette& f)
  : FColorPalette(f)
{ }

//----------------------------------------------------------------------
default8ColorPalette::~default8ColorPalette()
{ }

// public methods of default8ColorPalette
//----------------------------------------------------------------------
void default8ColorPalette::setColorPalette()
{
  setPalette (fc::Black, 0x00, 0x00, 0x00);
  setPalette (fc::Blue, 0x10, 0x3b, 0x9e);
  setPalette (fc::Green, 0x18, 0x78, 0x18);
  setPalette (fc::Cyan, 0xa0, 0xb2, 0xb2);
  setPalette (fc::Red, 0xb2, 0x18, 0x18);
  setPalette (fc::Magenta, 0xb2, 0x18, 0xb2);
  setPalette (fc::Brown, 0xe8, 0x87, 0x1f);
  setPalette (fc::LightGray, 0xe0, 0xe0, 0xe0);
  // The same colors again...
  setPalette (fc::DarkGray, 0x00, 0x00, 0x00);
  setPalette (fc::LightBlue, 0x10, 0x3b, 0x9e);
  setPalette (fc::LightGreen, 0x18, 0x78, 0x18);
  setPalette (fc::Cyan, 0xa0, 0xb2, 0xb2);
  setPalette (fc::LightRed, 0xb2, 0x18, 0x18);
  setPalette (fc::LightMagenta, 0xb2, 0x18, 0xb2);
  setPalette (fc::Yellow, 0xe8, 0x87, 0x1f);
  setPalette (fc::White, 0xe0, 0xe0, 0xe0);
}

//----------------------------------------------------------------------
void default8ColorPalette::resetColorPalette()
{
  setVGAdefaultPalette();
}


//----------------------------------------------------------------------
// class default16ColorPalette
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
default16ColorPalette::default16ColorPalette (const FSetPalette& f)
  : FColorPalette(f)
{ }

//----------------------------------------------------------------------
default16ColorPalette::~default16ColorPalette()
{ }

// public methods of default16ColorPalette
//----------------------------------------------------------------------
void default16ColorPalette::setColorPalette()
{
  setPalette (fc::Black, 0x00, 0x00, 0x00);
  setPalette (fc::Blue, 0x10, 0x3b, 0x9e);
  setPalette (fc::Green, 0x18, 0x78, 0x18);
  setPalette (fc::Cyan, 0x55, 0x6a, 0xcf);
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
void default16ColorPalette::resetColorPalette()
{
  setVGAdefaultPalette();
}


//----------------------------------------------------------------------
// class default16DarkColorPalette
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
default16DarkColorPalette::default16DarkColorPalette (const FSetPalette& f)
  : FColorPalette(f)
{ }

//----------------------------------------------------------------------
default16DarkColorPalette::~default16DarkColorPalette()
{ }

// public methods of default16DarkColorPalette
//----------------------------------------------------------------------
void default16DarkColorPalette::setColorPalette()
{
  setPalette (fc::Black, 0x00, 0x00, 0x00);
  setPalette (fc::Blue, 0x41, 0x58, 0xb3);
  setPalette (fc::Green, 0x18, 0x78, 0x18);
  setPalette (fc::Cyan, 0x4e, 0x66, 0x72);
  setPalette (fc::Red, 0xa5, 0x40, 0x40);
  setPalette (fc::Magenta, 0xb2, 0x18, 0xb2);
  setPalette (fc::Brown, 0xe8, 0x87, 0x1f);
  setPalette (fc::LightGray, 0xdc, 0xdc, 0xdc);
  setPalette (fc::DarkGray, 0x27, 0x33, 0x39);
  setPalette (fc::LightBlue, 0xb0, 0xb0, 0xb8);
  setPalette (fc::LightGreen, 0x5e, 0xeb, 0x5c);
  setPalette (fc::LightCyan, 0x62, 0xbf, 0xf8);
  setPalette (fc::LightRed, 0xdd, 0x51, 0x45);
  setPalette (fc::LightMagenta, 0xe9, 0xad, 0xff);
  setPalette (fc::Yellow, 0xfb, 0xe8, 0x67);
  setPalette (fc::White, 0xff, 0xff, 0xff);
}

//----------------------------------------------------------------------
void default16DarkColorPalette::resetColorPalette()
{
  setVGAdefaultPalette();
}

}  // namespace finalcut

