/***********************************************************************
* fcharmap.h - Character mapping and encoding                          *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2021 Markus Gans                                      *
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

#ifndef FCHARMAP_H
#define FCHARMAP_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <array>

#include "final/fc.h"
#include "final/ftypes.h"
#include "final/util/fstring.h"

namespace finalcut
{

class FCharMap final
{
  public:
    // Unicode fallback table for VT100, PC, and ASCII
    struct CharEncodeMap
    {
      wchar_t unicode;
      wchar_t vt100;
      wchar_t pc;
      wchar_t ascii;
    };

    // vt100 <-> utf-8
    struct DECSpecialGraphics
    {
      VT100Key key;
      UniChar  unicode;
    };

    // Using-declaration
    using CharEncodeType = std::array<CharEncodeMap, 115>;
    using DECGraphicsType = std::array<DECSpecialGraphics, 39>;
    using Cp437UcsType = std::array<std::array<wchar_t, 2>, 256>;
    using HalfFullWidthType = std::array<std::array<wchar_t, 2>, 227>;

    // Constructors
    FCharMap() = default;

    // Accessors
    FString                         getClassName() const;
    static auto                     getInstance() -> FCharMap&;
    static const wchar_t&           getCharacter ( const CharEncodeMap& char_enc
                                                 , const Encoding& enc );
    static CharEncodeType&          getCharEncodeMap();
    static const DECGraphicsType&   getDECSpecialGraphics();
    static const Cp437UcsType&      getCP437UCSMap();
    static const HalfFullWidthType& getHalfFullWidthMap();

    // Mutators
    static wchar_t&                 setCharacter ( CharEncodeMap& char_enc
                                                 , const Encoding& enc );

  private:
    // Data members
    static CharEncodeType          character;
    static const DECGraphicsType   dec_special_graphics;
    static const Cp437UcsType      cp437_ucs;
    static const HalfFullWidthType halfwidth_fullwidth;
};

// FCharMap inline functions
//----------------------------------------------------------------------
inline FString FCharMap::getClassName() const
{ return "FCharMap"; }

}  // namespace finalcut

#endif  // FCHARMAP_H
