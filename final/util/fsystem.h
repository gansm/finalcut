/***********************************************************************
* fsystem.h - Abstract class for system calls                          *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2019-2023 Markus Gans                                      *
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

#ifdef __APPLE__
  using timer_t = void*;
#endif

#include <memory>
#include <pwd.h>

#include "final/ftypes.h"

// struct forward declaration
struct kevent;

namespace finalcut
{

//----------------------------------------------------------------------
// class FSystem
//----------------------------------------------------------------------

class FSystem
{
  public:
    // Constructor
    FSystem() = default;

    // Destructor
    virtual ~FSystem() noexcept;

    // Accessor
    static auto   getInstance() -> std::unique_ptr<FSystem>&;

    // Methods
    virtual auto inPortByte (uShort) -> uChar = 0;
    virtual void outPortByte (uChar, uShort) = 0;
    virtual auto isTTY (int) const -> int = 0;
    virtual auto ioctl (int, uLong, ...) -> int = 0;
    virtual auto pipe (PipeData&) -> int = 0;
    virtual auto open (const char*, int, ...) -> int = 0;
    virtual auto close (int) -> int = 0;
    virtual auto fopen (const char*, const char*) -> FILE* = 0;
    virtual auto fclose (FILE*) -> int = 0;
    virtual auto fputs (const char*, FILE*) -> int = 0;
    virtual auto putchar (int) -> int = 0;
    virtual auto sigaction ( int, const struct sigaction*
                           , struct sigaction* ) -> int = 0;
    virtual auto timer_create ( clockid_t, struct sigevent*
                              , timer_t* ) -> int = 0;
    virtual auto timer_settime ( timer_t, int,
                                 const struct itimerspec*,
                                 struct itimerspec* ) -> int = 0;
    virtual auto timer_delete (timer_t) -> int = 0;
    virtual auto kqueue() -> int = 0;
    virtual auto kevent ( int, const struct kevent*
                        , int, struct kevent*
                        , int, const struct timespec* ) -> int = 0;
    virtual auto getuid() -> uid_t = 0;
    virtual auto geteuid() -> uid_t = 0;
    virtual auto getpwuid_r ( uid_t, struct passwd*, char*
                            , size_t, struct passwd**) -> int = 0;
    virtual auto realpath (const char*, char*) -> char* = 0;
};

}  // namespace finalcut

#endif  // FSYSTEM_H
