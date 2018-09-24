/***********************************************************************
* ftermios.h - Provides access to POSIX tty I/O control                *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2018 Markus Gans                                           *
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
 * ▕▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FTermios ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FTERMIOS_H
#define FTERMIOS_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <termios.h>
#include <unistd.h>

#include "final/ftypes.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FTermios
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FTermios
{
  public:
    // Constructors
    FTermios();

    // Destructor
    virtual ~FTermios();

    // Accessors
    const char*    getClassName() const;
    static termios getTTY();
    static int     getStdIn();
    static int     getStdOut();
    static int     getStdErr();

    // Inquiries
    static bool    isRaw();

    // Methods
    static void    init();
    static void    setTTY (const termios&);
    static void    storeTTYsettings();
    static void    restoreTTYsettings();
    static void    setHardwareEcho();
    static void    unsetHardwareEcho();
    static void    setCaptureSendCharacters();
    static void    unsetCaptureSendCharacters();
    static bool    setRawMode (bool);
    static bool    setRawMode();
    static bool    unsetRawMode();
    static bool    setCookedMode();
    static uInt    getBaudRate();

  private:
    // Data Members
    static int    stdin_no;
    static int    stdout_no;
    static int    stderr_no;
    static bool   raw_mode;
    static struct termios term_init;
};
#pragma pack(pop)

// FTermios inline functions
//----------------------------------------------------------------------
inline const char* FTermios::getClassName() const
{ return "FTermios"; }

//----------------------------------------------------------------------
inline int FTermios::getStdIn()
{ return stdin_no; }

//----------------------------------------------------------------------
inline int FTermios::getStdOut()
{ return stdout_no; }

//----------------------------------------------------------------------
inline int FTermios::getStdErr()
{ return stderr_no; }

//----------------------------------------------------------------------
inline bool FTermios::isRaw()
{ return raw_mode; }

//----------------------------------------------------------------------
inline bool FTermios::setRawMode()
{ return setRawMode(true); }

//----------------------------------------------------------------------
inline bool FTermios::unsetRawMode()
{ return setRawMode(false); }

//----------------------------------------------------------------------
inline bool FTermios::setCookedMode()
{ return setRawMode(false); }

}  // namespace finalcut

#endif  // FTERMIOS_H
