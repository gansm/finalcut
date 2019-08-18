/***********************************************************************
* fsystem.h - Abstract class for system calls                          *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2019 Markus Gans                                           *
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

/*  Standalone class
 *  ════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▏
 * ▕ FSystem ▏
 * ▕▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FSYSTEM_H
#define FSYSTEM_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <pwd.h>
#include "final/ftypes.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FSystem
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FSystem
{
  public:
    // Constructor
    FSystem();

    // Destructor
    virtual ~FSystem();

    // Methods
    virtual uChar inPortByte (uShort) = 0;
    virtual void  outPortByte (uChar, uShort) = 0;
    virtual int   isTTY (int) = 0;
    virtual int   ioctl (int, uLong, ...) = 0;
    virtual int   open (const char*, int, ...) = 0;
    virtual int   close (int) = 0;
    virtual FILE* fopen (const char*, const char*) = 0;
    virtual int   fclose (FILE*) = 0;
    virtual int   putchar (int) = 0;
    virtual int   tputs (const char*, int, int (*)(int)) = 0;
    virtual uid_t getuid() = 0;
    virtual uid_t geteuid() = 0;
    virtual int   getpwuid_r ( uid_t, struct passwd*, char*
                             , size_t, struct passwd**) = 0;
    virtual char* realpath (const char*, char*) = 0;
};
#pragma pack(pop)

}  // namespace finalcut

#endif  // FSYSTEM_H


