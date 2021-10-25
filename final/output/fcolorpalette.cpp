/***********************************************************************
* fcolorpalette.cpp - Define RGB color value for a palette entry       *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2020-2021 Markus Gans                                      *
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

#include "final/output/fcolorpalette.h"

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
FColorPalette::~FColorPalette() noexcept = default;  // destructor


// public methods of FColorPalette
//----------------------------------------------------------------------
auto FColorPalette::getInstance() -> std::shared_ptr<FColorPalette>&
{
  static const auto& color_theme = make_unique<std::shared_ptr<FColorPalette>>();
  return *color_theme;
}


// protected methods of FColorPalette
//----------------------------------------------------------------------
void FColorPalette::setPalette (FColor index, int r, int g, int b) const
{
  set_palette (index, r, g, b);
}

//----------------------------------------------------------------------
void FColorPalette::setVGAdefaultPalette() const
{
  setPalette (FColor::Black, 0x00, 0x00, 0x00);
  setPalette (FColor::Blue, 0x00, 0x00, 0xaa);
  setPalette (FColor::Green, 0x00, 0xaa, 0x00);
  setPalette (FColor::Cyan, 0x00, 0xaa, 0xaa);
  setPalette (FColor::Red, 0xaa, 0x00, 0x00);
  setPalette (FColor::Magenta, 0xaa, 0x00, 0xaa);
  setPalette (FColor::Brown, 0xaa, 0x55, 0x00);
  setPalette (FColor::LightGray, 0xaa, 0xaa, 0xaa);
  setPalette (FColor::DarkGray, 0x55, 0x55, 0x55);
  setPalette (FColor::LightBlue, 0x55, 0x55, 0xff);
  setPalette (FColor::LightGreen, 0x55, 0xff, 0x55);
  setPalette (FColor::LightCyan, 0x55, 0xff, 0xff);
  setPalette (FColor::LightRed, 0xff, 0x55, 0x55);
  setPalette (FColor::LightMagenta, 0xff, 0x55, 0xff);
  setPalette (FColor::Yellow, 0xff, 0xff, 0x55);
  setPalette (FColor::White, 0xff, 0xff, 0xff);
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
default8ColorPalette::~default8ColorPalette() noexcept = default;  // destructor


// public methods of default8ColorPalette
//----------------------------------------------------------------------
void default8ColorPalette::setColorPalette()
{
  setPalette (FColor::Black, 0x00, 0x00, 0x00);
  setPalette (FColor::Blue, 0x10, 0x3b, 0x9e);
  setPalette (FColor::Green, 0x18, 0x78, 0x18);
  setPalette (FColor::Cyan, 0xa0, 0xb2, 0xb2);
  setPalette (FColor::Red, 0xb2, 0x18, 0x18);
  setPalette (FColor::Magenta, 0xb2, 0x18, 0xb2);
  setPalette (FColor::Brown, 0xe8, 0x87, 0x1f);
  setPalette (FColor::LightGray, 0xe0, 0xe0, 0xe0);
  // The same colors again...
  setPalette (FColor::DarkGray, 0x00, 0x00, 0x00);
  setPalette (FColor::LightBlue, 0x10, 0x3b, 0x9e);
  setPalette (FColor::LightGreen, 0x18, 0x78, 0x18);
  setPalette (FColor::Cyan, 0xa0, 0xb2, 0xb2);
  setPalette (FColor::LightRed, 0xb2, 0x18, 0x18);
  setPalette (FColor::LightMagenta, 0xb2, 0x18, 0xb2);
  setPalette (FColor::Yellow, 0xe8, 0x87, 0x1f);
  setPalette (FColor::White, 0xe0, 0xe0, 0xe0);
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
default16ColorPalette::~default16ColorPalette() noexcept = default;  // destructor


// public methods of default16ColorPalette
//----------------------------------------------------------------------
void default16ColorPalette::setColorPalette()
{
  setPalette (FColor::Black, 0x00, 0x00, 0x00);
  setPalette (FColor::Blue, 0x10, 0x3b, 0x9e);
  setPalette (FColor::Green, 0x18, 0x78, 0x18);
  setPalette (FColor::Cyan, 0x55, 0x6a, 0xcf);
  setPalette (FColor::Red, 0xba, 0x1a, 0x1a);
  setPalette (FColor::Magenta, 0xb2, 0x18, 0xb2);
  setPalette (FColor::Brown, 0xe8, 0x87, 0x1f);
  setPalette (FColor::LightGray, 0xbc, 0xbc, 0xbc);
  setPalette (FColor::DarkGray, 0x50, 0x50, 0x50);
  setPalette (FColor::LightBlue, 0x80, 0xa4, 0xec);
  setPalette (FColor::LightGreen, 0x5e, 0xeb, 0x5c);
  setPalette (FColor::LightCyan, 0x62, 0xbf, 0xf8);
  setPalette (FColor::LightRed, 0xee, 0x44, 0x44);
  setPalette (FColor::LightMagenta, 0xe9, 0xad, 0xff);
  setPalette (FColor::Yellow, 0xfb, 0xe8, 0x67);
  setPalette (FColor::White, 0xff, 0xff, 0xff);
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
default16DarkColorPalette::~default16DarkColorPalette() noexcept = default;  // destructor


// public methods of default16DarkColorPalette
//----------------------------------------------------------------------
void default16DarkColorPalette::setColorPalette()
{
  setPalette (FColor::Black, 0x00, 0x00, 0x00);
  setPalette (FColor::Blue, 0x41, 0x58, 0xb3);
  setPalette (FColor::Green, 0x18, 0x78, 0x18);
  setPalette (FColor::Cyan, 0x4e, 0x66, 0x72);
  setPalette (FColor::Red, 0xa5, 0x40, 0x40);
  setPalette (FColor::Magenta, 0xb2, 0x18, 0xb2);
  setPalette (FColor::Brown, 0xe8, 0x87, 0x1f);
  setPalette (FColor::LightGray, 0xdc, 0xdc, 0xdc);
  setPalette (FColor::DarkGray, 0x27, 0x33, 0x39);
  setPalette (FColor::LightBlue, 0xb0, 0xb0, 0xb8);
  setPalette (FColor::LightGreen, 0x5e, 0xeb, 0x5c);
  setPalette (FColor::LightCyan, 0x62, 0xbf, 0xf8);
  setPalette (FColor::LightRed, 0xdd, 0x51, 0x45);
  setPalette (FColor::LightMagenta, 0xe9, 0xad, 0xff);
  setPalette (FColor::Yellow, 0xfb, 0xe8, 0x67);
  setPalette (FColor::White, 0xff, 0xff, 0xff);
}

//----------------------------------------------------------------------
void default16DarkColorPalette::resetColorPalette()
{
  setVGAdefaultPalette();
}

}  // namespace finalcut

