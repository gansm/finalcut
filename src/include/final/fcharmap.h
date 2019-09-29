/***********************************************************************
* fcharmap.h - Character mapping and encoding                          *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2015-2019 Markus Gans                                      *
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

#ifndef FCHARMAP_H
#define FCHARMAP_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fc.h"
#include "final/ftypes.h"

namespace finalcut
{

namespace fc
{

extern uInt character[][fc::NUM_OF_ENCODINGS];
extern const std::size_t lastCharItem;

extern int vt100_key_to_utf8[][2];
extern const std::size_t lastKeyItem;

extern wchar_t cp437_ucs[][2];
extern const std::size_t lastCP437Item;

extern wchar_t halfWidth_fullWidth[][2];
extern const std::size_t lastHalfWidthItem;

}  // namespace fc

}  // namespace finalcut

#endif  // FCHARMAP_H
