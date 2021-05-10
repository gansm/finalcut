/***********************************************************************
* fcharmap.h - Character mapping and encoding                          *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2020 Markus Gans                                      *
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

namespace finalcut
{

namespace fc
{

// Unicode fallback table for VT100, PC, and ASCII
struct CharEncodeMap
{
  wchar_t unicode;
  wchar_t vt100;
  wchar_t pc;
  wchar_t ascii;
};

extern std::array<CharEncodeMap, 115> character;

inline wchar_t& getCharacter (CharEncodeMap& char_enc, const Encoding& enc)
{
  const auto array = reinterpret_cast<wchar_t*>(&char_enc);
  return array[std::size_t(enc)];
}

// vt100 <-> utf-8
struct DECSpecialGraphics
{
  VT100Key key;
  UniChar  unicode;
};

extern const std::array<DECSpecialGraphics, 39> dec_special_graphics;
extern const std::array<std::array<wchar_t, 2>, 256> cp437_ucs;
extern const std::array<std::array<wchar_t, 2>, 227> halfwidth_fullwidth;

}  // namespace fc

}  // namespace finalcut

#endif  // FCHARMAP_H
