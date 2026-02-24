/***********************************************************************
* fvtermattribute.cpp - Manipulation of FChar colors and attributes    *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2021-2026 Markus Gans                                      *
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

#include <functional>
#include <unordered_map>

#include "final/vterm/fstyle.h"
#include "final/vterm/fvtermattribute.h"

namespace finalcut
{

namespace internal
{
constexpr FCellColor color_var::default_color_pair;
constexpr uInt32 attr_var::reset_mask;
}

// static class attributes
FChar FVTermAttribute::next_attribute{};

// Using-declaration
using map_type = std::pair<const Style, std::function<void()>>;
using AttributeLookupMap = std::unordered_map<const Style, std::function<void()>>;

//----------------------------------------------------------------------
static auto getAttributeLookupMap() -> AttributeLookupMap&
{
  // Encapsulate global unordered_map object
  static const auto& attribute_lookup = std::make_unique<AttributeLookupMap>
  (
    std::initializer_list<map_type>
    ({
      { Style::Bold,              [] () { FVTermAttribute::setBold(); } },
      { Style::Dim,               [] () { FVTermAttribute::setDim(); } },
      { Style::Italic,            [] () { FVTermAttribute::setItalic(); } },
      { Style::Underline,         [] () { FVTermAttribute::setUnderline(); } },
      { Style::Blink,             [] () { FVTermAttribute::setBlink(); } },
      { Style::Reverse,           [] () { FVTermAttribute::setReverse(); } },
      { Style::Standout,          [] () { FVTermAttribute::setStandout(); } },
      { Style::Invisible,         [] () { FVTermAttribute::setInvisible(); } },
      { Style::Protected,         [] () { FVTermAttribute::setProtected(); } },
      { Style::CrossedOut,        [] () { FVTermAttribute::setCrossedOut(); } },
      { Style::DoubleUnderline,   [] () { FVTermAttribute::setDoubleUnderline(); } },
      { Style::Transparent,       [] () { FVTermAttribute::setTransparent(); } },
      { Style::ColorOverlay,      [] () { FVTermAttribute::setColorOverlay(); } },
      { Style::InheritBackground, [] () { FVTermAttribute::setInheritBackground(); } }
    })
  );

  return *attribute_lookup;
}


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
void FVTermAttribute::initAttribute() noexcept
{
  // next_attribute contains the state of the next printed character
  next_attribute.ch = {L'\0', L'\0', L'\0', L'\0', L'\0'};
  next_attribute.color = internal::color_var::default_color_pair;
  next_attribute.attr.data = 0;
}

//----------------------------------------------------------------------
void FVTermAttribute::print (const FStyle& style)
{
  auto attr = style.getStyle();
  static const auto& attribute_lookup = getAttributeLookupMap();

  if ( attr == Style::None )
  {
    setNormal();
    return;
  }

  while ( attr != Style::None )
  {
    const auto style_name = Style(attr & -attr);  // Find rightmost set bit
    const auto iter = attribute_lookup.find(style_name);

    if ( iter != attribute_lookup.end() )
      iter->second();  // Sets the found style

    attr ^= style_name;  // Clear the rightmost set bit
  }
}

//----------------------------------------------------------------------
void FVTermAttribute::print (const FColorPair& pair) noexcept
{
  setColor (pair);
}

}  // namespace finalcut
