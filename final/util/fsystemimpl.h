/***********************************************************************
* fsystemimpl.h - FSystem implementation                               *
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

#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>

#include "final/fc.h"
#include "final/fsystem.h"

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
    uChar inPortByte (uShort port) override
    {
      return ::inb (port);
    }
#else
    uChar inPortByte (uShort) override
    {
      return 0;
    }
#endif


#if defined(ISA_SYSCTL_SUPPORT)
    void outPortByte (uChar value, uShort port) override
    {
      ::outb (value, port);
    }
#else
    void outPortByte (uChar, uShort) override
    { }
#endif

    int isTTY (int fd) const override
    {
      return ::isatty(fd);
    }

    int ioctl (int fd, uLong request, ...) override
    {
      va_list args{};
      va_start (args, request);
      void* argp = va_arg (args, void*);
      int ret = ::ioctl (fd, request, argp);
      va_end (args);
      return ret;
    }

    int open (const char* pathname, int flags, ...) override
    {
      va_list args{};
      va_start (args, flags);
      auto mode = static_cast<mode_t>(va_arg (args, int));
      int ret = ::open (pathname, flags, mode);
      va_end (args);
      return ret;
    }

    int close (int fildes) override
    {
      return ::close(fildes);
    }

    FILE* fopen (const char* path, const char* mode) override
    {
      return std::fopen (path, mode);
    }

    int fclose (FILE* fp) override
    {
      return std::fclose (fp);
    }

    int fputs (const char* str, FILE* stream) override
    {
      return std::fputs (str, stream);
    }

    int putchar (int c) override
    {
#if defined(__sun) && defined(__SVR4)
      return std::putchar(char(c));
#else
      return std::putchar(c);
#endif
    }

    uid_t getuid() override
    {
      return ::getuid();
    }

    uid_t geteuid() override
    {
      return ::geteuid();
    }

    int getpwuid_r ( uid_t, struct passwd*, char*, size_t
                   , struct passwd** ) override;

    char* realpath (const char*, char*) override;
};

}  // namespace finalcut

#endif  // FSYSTEMIMPL_H


