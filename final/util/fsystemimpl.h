/***********************************************************************
* fsystemimpl.h - FSystem implementation                               *
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

/*  Inheritance diagram
 *  ═══════════════════
 *
 *    ▕▔▔▔▔▔▔▔▔▔▏
 *    ▕ FSystem ▏
 *    ▕▁▁▁▁▁▁▁▁▁▏
 *         ▲
 *         │
 *  ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *  ▕ FSystemImpl ▏
 *  ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FSYSTEMIMPL_H
#define FSYSTEMIMPL_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#if defined(__linux__)

  #if defined(__arm__) && defined(__GLIBC__) && defined(__GLIBC_PREREQ)
    // ISA sysctl support on arm processors only up to glibc-2.29
    #if !__GLIBC_PREREQ(2,30)
      #define ARM_ISA_SYSCTL
    #endif
  #endif

  #if defined(__x86_64__) || defined(__i386) || defined(ARM_ISA_SYSCTL)
    #define ISA_SYSCTL_SUPPORT
    #include <sys/io.h>
  #endif  // defined(__x86_64__) || defined(__i386) || defined(ARM_ISA_SYSCTL)

#endif  // defined(__linux__)


#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <cstdarg>
#include <fcntl.h>
#include <unistd.h>

#include "final/fc.h"
#include "final/util/fsystem.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FSystemImpl
//----------------------------------------------------------------------

class FSystemImpl : public FSystem
{
  public:
    // Constructor
    FSystemImpl() = default;

    // Destructor
    ~FSystemImpl() noexcept override;

    // Methods
#if defined(ISA_SYSCTL_SUPPORT)
    inline auto inPortByte (uShort port) -> uChar override
    {
      return ::inb (port);
    }
#else
    inline uChar inPortByte (uShort) override
    {
      return 0;
    }
#endif


#if defined(ISA_SYSCTL_SUPPORT)
    inline void outPortByte (uChar value, uShort port) override
    {
      ::outb (value, port);
    }
#else
    inline void outPortByte (uChar, uShort) override
    { }
#endif

    inline auto isTTY (int file_descriptor) const -> int override
    {
      return ::isatty(file_descriptor);
    }

    inline auto ioctl (int file_descriptor, uLong request, ...) -> int override
    {
      va_list args{};
      va_start (args, request);
      void* argp = va_arg (args, void*);
      int ret = ::ioctl (file_descriptor, request, argp);
      va_end (args);
      return ret;
    }

    inline auto pipe (PipeData& pipe) -> int override
    {
      return ::pipe(pipe.getArrayData());
    }

    inline auto open (const char* pathname, int flags, ...) -> int override
    {
      va_list args{};
      va_start (args, flags);
      auto mode = static_cast<mode_t>(va_arg (args, int));
      int ret = ::open (pathname, flags, mode);
      va_end (args);
      return ret;
    }

    inline auto close (int file_descriptor) -> int override
    {
      return ::close(file_descriptor);
    }

    inline auto fopen (const char* path, const char* mode) -> FILE* override
    {
      return std::fopen (path, mode);
    }

    inline auto fclose (FILE* file_ptr) -> int override
    {
      return std::fclose (file_ptr);
    }

    inline auto fputs (const char* str, FILE* stream) -> int override
    {
      return std::fputs (str, stream);
    }

    inline auto putchar (int c) -> int override
    {
#if defined(__sun) && defined(__SVR4)
      return std::putchar(char(c));
#else
      return std::putchar(c);
#endif
    }

    auto sigaction ( int, const struct sigaction*
                   , struct sigaction* ) -> int override;
    auto timer_create ( clockid_t, struct sigevent*
                      , timer_t* ) -> int override;
    auto timer_settime ( timer_t, int
                       , const struct itimerspec*
                       , struct itimerspec* ) -> int override;
    auto timer_delete (timer_t) -> int override;
    auto kqueue() -> int override;
    auto kevent ( int, const struct ::kevent*
                , int, struct ::kevent*
                , int, const struct timespec* ) -> int override;

    inline auto getuid() -> uid_t override
    {
      return ::getuid();
    }

    inline auto geteuid() -> uid_t override
    {
      return ::geteuid();
    }

    auto getpwuid_r ( uid_t, struct passwd*, char*, size_t
                    , struct passwd** ) -> int override;

    auto realpath (const char*, char*) -> char* override;
};

}  // namespace finalcut

#endif  // FSYSTEMIMPL_H


