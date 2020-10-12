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

extern std::array<std::array<uInt, fc::NUM_OF_ENCODINGS>, 115> character;
extern const std::array<std::array<int, 2>, 39> vt100_key_to_utf8;
extern const std::array<std::array<wchar_t, 2>, 256> cp437_ucs;
extern const std::array<std::array<wchar_t, 2>, 227> halfwidth_fullwidth;

}  // namespace fc

}  // namespace finalcut

#endif  // FCHARMAP_H
