/************************************************************************
* ftypes.h - Implements global data types                               *
*                                                                       *
* This file is part of the Final Cut widget toolkit                     *
*                                                                       *
* Copyright 2017 Markus Gans                                            *
*                                                                       *
* The Final Cut is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by  *
* the Free Software Foundation; either version 3 of the License, or     *
* (at your option) any later version.                                   *
*                                                                       *
* The Final Cut is distributed in the hope that it will be useful,      *
* but WITHOUT ANY WARRANTY; without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
* GNU General Public License for more details.                          *
*                                                                       *
* You should have received a copy of the GNU General Public License     *
* along with this program.  If not, see <http://www.gnu.org/licenses/>. *
************************************************************************/

#ifndef FTYPES_H
#define FTYPES_H

#include <sys/types.h>

#define null NULL

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

#endif  // FTYPES_H

