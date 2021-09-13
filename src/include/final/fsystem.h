/***********************************************************************
* fsystem.h - Abstract class for system calls                          *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2019-2021 Markus Gans                                      *
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
#include <memory>
#include "final/ftypes.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FSystem
//----------------------------------------------------------------------

class FSystem
{
  public:
    // Using-declaration
    using fn_putc = int (*)(int);

    // Constructor
    FSystem() = default;

    // Destructor
    virtual ~FSystem() noexcept;

    // Accessor
    static auto   getInstance() -> std::unique_ptr<FSystem>&;

    // Methods
    virtual uChar inPortByte (uShort) = 0;
    virtual void  outPortByte (uChar, uShort) = 0;
    virtual int   isTTY (int) const = 0;
    virtual int   ioctl (int, uLong, ...) = 0;
    virtual int   open (const char*, int, ...) = 0;
    virtual int   close (int) = 0;
    virtual FILE* fopen (const char*, const char*) = 0;
    virtual int   fclose (FILE*) = 0;
    virtual int   putchar (int) = 0;
    virtual uid_t getuid() = 0;
    virtual uid_t geteuid() = 0;
    virtual int   getpwuid_r ( uid_t, struct passwd*, char*
                             , size_t, struct passwd**) = 0;
    virtual char* realpath (const char*, char*) = 0;
};

}  // namespace finalcut

#endif  // FSYSTEM_H
