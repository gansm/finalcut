/***********************************************************************
* fvtermattribute.cpp - Manipulation of FChar colors and attributes    *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2021-2022 Markus Gans                                      *
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

#include <unordered_map>

#include "final/vterm/fcolorpair.h"
#include "final/vterm/fstyle.h"
#include "final/vterm/fvtermattribute.h"

namespace finalcut
{

// static class attributes
FChar FVTermAttribute::next_attribute{};

//----------------------------------------------------------------------
// class FVTermAttribute
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
// constructor
FVTermAttribute::FVTermAttribute() = default;

//----------------------------------------------------------------------
// destructor
FVTermAttribute::~FVTermAttribute() noexcept = default;


// public methods of FVTermAttribute
//----------------------------------------------------------------------
void FVTermAttribute::initAttribute()
{
  // next_attribute contains the state of the next printed character
  next_attribute.ch           = {{ L'\0' }};
  next_attribute.fg_color     = FColor::Default;
  next_attribute.bg_color     = FColor::Default;
  next_attribute.attr.byte[0] = 0;
  next_attribute.attr.byte[1] = 0;
  next_attribute.attr.byte[2] = 0;
  next_attribute.attr.byte[3] = 0;
}

//----------------------------------------------------------------------
void FVTermAttribute::print (const FStyle& style)
{
  const std::unordered_map<Style, std::function<void(bool)>> attributeLookup
  {
    { Style::Bold, &FVTermAttribute::setBold },
    { Style::Dim, &FVTermAttribute::setDim },
    { Style::Italic, &FVTermAttribute::setItalic },
    { Style::Underline, &FVTermAttribute::setUnderline },
    { Style::Blink, &FVTermAttribute::setBlink },
    { Style::Reverse, &FVTermAttribute::setReverse },
    { Style::Standout, &FVTermAttribute::setStandout },
    { Style::Invisible, &FVTermAttribute::setInvisible },
    { Style::Protected, &FVTermAttribute::setProtected },
    { Style::CrossedOut, &FVTermAttribute::setCrossedOut },
    { Style::DoubleUnderline, &FVTermAttribute::setDoubleUnderline },
    { Style::Transparent, &FVTermAttribute::setTransparent },
    { Style::ColorOverlay, &FVTermAttribute::setColorOverlay },
    { Style::InheritBackground, &FVTermAttribute::setInheritBackground }
  };

  Style attr = style.getStyle();

  if ( attr == Style::None )
    setNormal();
  else
  {
    for (const auto& p : attributeLookup)
    {
      const auto& style_name = p.first;
      const auto& set_function = p.second;

      if ( (attr & style_name) != Style::None )
        set_function(true);
    }
  }
}

//----------------------------------------------------------------------
void FVTermAttribute::print (const FColorPair& pair)
{
  setColor (pair.getForegroundColor(), pair.getBackgroundColor());
}

}  // namespace finalcut
