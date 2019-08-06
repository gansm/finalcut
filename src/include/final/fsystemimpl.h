/***********************************************************************
* fsystemimpl.h - FSystem implementation                               *
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
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FSystemImpl ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FSYSTEMIMPL_H
#define FSYSTEMIMPL_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#if defined(__linux__)
  #if defined(__x86_64__) || defined(__i386) || defined(__arm__)
    #include <sys/io.h>        // <asm/io.h> is deprecated
  #endif  // defined(__x86_64__) || defined(__i386) || defined(__arm__)
#endif  // defined(__linux__)

#if defined(__sun) && defined(__SVR4)
  #include <termio.h>
  typedef struct termio SGTTY;
  typedef struct termios SGTTYS;

  #ifdef _LP64
    typedef unsigned int chtype;
  #else
    typedef unsigned long chtype;
  #endif  // _LP64

  #include <term.h>  // termcap
#else
  #include <term.h>  // termcap
#endif  // defined(__sun) && defined(__SVR4)

#ifdef F_HAVE_LIBGPM
  #undef buttons  // from term.h
#endif

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

#pragma pack(push)
#pragma pack(1)

class FSystemImpl : public FSystem
{
  public:
    // Constructor
    FSystemImpl();

    // Destructor
    virtual ~FSystemImpl();

    // Methods
#if defined(__linux__)
#if defined(__x86_64__) || defined(__i386) || defined(__arm__)
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
#else
    uChar inPortByte (uShort) override
    {
      return 0;
    }
#endif


#if defined(__linux__)
#if defined(__x86_64__) || defined(__i386) || defined(__arm__)
    void outPortByte (uChar value, uShort port) override
    {
      ::outb (value, port);
    }
#else
    void outPortByte (uChar, uShort) override
    { }
#endif
#else
    void outPortByte (uChar, uShort) override
    { }
#endif


    int isTTY (int fd) override
    {
      return ::isatty(fd);
    }

    int ioctl (int fd, uLong request, ...) override
    {
      va_list args;
      va_start (args, request);
      void* argp = va_arg (args, void*);
      int ret = ::ioctl (fd, request, argp);
      va_end (args);
      return ret;
    }

    int open (const char* pathname, int flags, ...) override
    {
      va_list args;
      va_start (args, flags);
      mode_t mode = static_cast<mode_t>(va_arg (args, int));
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

    int putchar (int c) override
    {
#if defined(__sun) && defined(__SVR4)
      return std::putchar(char(c));
#else
      return std::putchar(c);
#endif
    }

    int tputs (const char* str, int affcnt, int (*putc)(int)) override
    {
#if defined(__sun) && defined(__SVR4)
      return ::tputs (C_STR(str), affcnt, reinterpret_cast<int (*)(char)>(putc));
#else
      return ::tputs (str, affcnt, putc);
#endif
    }

    uid_t getuid() override
    {
      return ::getuid();
    }
};
#pragma pack(pop)

}  // namespace finalcut

#endif  // FSYSTEMIMPL_H


