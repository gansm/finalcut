/***********************************************************************
* fkey_map.h - Key name mapping                                        *
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

#ifndef FKEYMAP_H
#define FKEYMAP_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <array>

#include "final/ftypes.h"

namespace finalcut
{

enum class FKey : uInt32;   // forward declaration

namespace fc
{

struct FKeyCapMap
{
  FKey  num;
  const char* string;
  char  tname[4];
};

extern std::array<FKeyCapMap, 188> fkey_cap_table;

struct FKeyMap
{
  FKey num;
  char string[8];
};

extern const std::array<FKeyMap, 232> fkey_table;

struct FKeyName
{
  FKey num;
  char string[26];
};

extern const std::array<FKeyName, 388> fkeyname;

}  // namespace fc

}  // namespace finalcut

#endif  // FKEYMAP_H
