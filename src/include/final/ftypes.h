/***********************************************************************
* ftypes.h - Implements global data types                              *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2017-2018 Markus Gans                                      *
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

#ifndef FTYPES_H
#define FTYPES_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <stdint.h>
#include <sys/types.h>

#include <limits>
#include <string>

#define null NULL

namespace
{

typedef unsigned char  uChar;
typedef unsigned int   uInt;
typedef unsigned long  uLong;
typedef uint8_t        uInt8;
typedef uint16_t       uInt16;
typedef uint32_t       uInt32;
typedef uint64_t       uInt64;

typedef signed int     sInt;
typedef signed long    sLong;
typedef int8_t         sInt8;
typedef int16_t        sInt16;
typedef int32_t        sInt32;
typedef int64_t        sInt64;

typedef long double    lDouble;

typedef uInt16         FColor;
typedef uInt32         FKey;

}  // namespace


namespace finalcut
{

const char* const bad_alloc_str = \
    "not enough memory to alloc ";

template <typename T, bool is_signed>
struct is_negative
{
  inline bool operator () (const T& x)
  {
    return x < 0;
  }
};

template <typename T>
struct is_negative<T,false>
{
  inline bool operator () (const T&)
  {
    return false;
  }
};

template <typename T>
inline bool isNegative (const T& x)
{
  return is_negative<T, std::numeric_limits<T>::is_signed>()(x);
}

namespace fc
{
#pragma pack(push)
#pragma pack(1)
typedef struct
{
  FKey  num;
  char* string;
  char  tname[4];
}
fkeymap;

typedef struct
{
  FKey num;
  char string[8];
}
fmetakeymap;

typedef struct
{
  FKey num;
  char string[25];
}
keyname;
#pragma pack(pop)

}  // namespace fc

}  // namespace finalcut

#endif  // FTYPES_H

