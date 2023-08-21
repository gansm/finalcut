/***********************************************************************
* ftermios.cpp - Provides access to POSIX tty I/O control              *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2022 Markus Gans                                      *
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

#if defined(__CYGWIN__)
  #define _POSIX_C_SOURCE 200809L
  #define _BSD_SOURCE
  #ifndef __STRICT_ANSI__
    #define __STRICT_ANSI__  // need for bits/c++config.h
  #endif
#endif

#include <termios.h>

#include <system_error>
#include <unordered_map>

#include "final/output/tty/fterm.h"
#include "final/output/tty/ftermios.h"

namespace finalcut
{

// static class attributes
int     FTermios::stdin_no;
int     FTermios::stdout_no;
int     FTermios::stderr_no;
bool    FTermios::raw_mode;
termios FTermios::term_init;


//----------------------------------------------------------------------
// class FTermios
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FTermios::FTermios()
{
  init();
}

// public methods of FTermios
//----------------------------------------------------------------------
void FTermios::init()
{
  // Assertion: programm start in cooked mode
  raw_mode = false;

  // Get file descriptor for standard input and standard output
  stdin_no  = fileno(stdin);
  stdout_no = fileno(stdout);
  stderr_no = fileno(stderr);
}

//----------------------------------------------------------------------
auto FTermios::getTTY() -> termios
{
  struct termios t{};

  if ( tcgetattr(stdin_no, &t) == -1 )
    throw std::system_error(errno, std::generic_category());

  return t;
}

//----------------------------------------------------------------------
void FTermios::setTTY (const termios& t)
{
  tcsetattr (stdin_no, TCSADRAIN, &t);
}

//----------------------------------------------------------------------
void FTermios::storeTTYsettings()
{
  // Store termios settings
  try
  {
    term_init = getTTY();
  }
  catch (const std::system_error&)
  {
    throw std::current_exception();
  }
}

//----------------------------------------------------------------------
void FTermios::restoreTTYsettings()
{
  // Restore termios settings
  setTTY (term_init);
}

//----------------------------------------------------------------------
void FTermios::setHardwareEcho()
{
  // Info under: man 3 termios
  struct termios t{};
  tcgetattr (stdin_no, &t);

  // local mode
  t.c_lflag &= uInt(ECHO | ECHONL);

  // input mode
  t.c_iflag &= uInt(ICRNL | INLCR | IGNCR);

  // output mode
  t.c_oflag &= uInt(ONLCR);

  // set the new termios settings
  setTTY (t);
}

//----------------------------------------------------------------------
void FTermios::unsetHardwareEcho()
{
  // Info under: man 3 termios
  struct termios t{};
  tcgetattr (stdin_no, &t);

  // local mode
  t.c_lflag &= uInt(~(ECHO | ECHONL));

  // input mode
  t.c_iflag &= uInt(~(ICRNL | INLCR | IGNCR));

  // output mode
  t.c_oflag &= uInt(~ONLCR);

  // set the new termios settings
  setTTY (t);
}

//----------------------------------------------------------------------
void FTermios::setCaptureSendCharacters()
{
  struct termios t{};
  tcgetattr (stdin_no, &t);
  t.c_lflag &= uInt(~(ICANON | ECHO));
  t.c_cc[VTIME] = 10;  // Timeout in deciseconds
  t.c_cc[VMIN]  = 0;  // Minimum number of characters
  tcsetattr (stdin_no, TCSANOW, &t);
}

//----------------------------------------------------------------------
void FTermios::unsetCaptureSendCharacters()
{
  struct termios t{};
  tcgetattr (stdin_no, &t);
  t.c_lflag |= uInt(ICANON | ECHO);
  t.c_cc[VTIME] = 0;  // Timeout in deciseconds
  t.c_cc[VMIN]  = 1;  // Minimum number of characters
  setTTY (t);
}

//----------------------------------------------------------------------
auto FTermios::setRawMode (bool enable) -> bool
{
  // set + unset flags for raw mode
  if ( raw_mode == enable )
    return raw_mode;

  // Info under: man 3 termios
  struct termios t{};
  tcgetattr (stdin_no, &t);

  if ( enable )
  {
    // local mode
#if DEBUG
    // Exit with ctrl-c only if compiled with "DEBUG" option
    t.c_lflag &= uInt(~(ICANON | IEXTEN));
#else
    t.c_lflag &= uInt(~(ICANON | ISIG | IEXTEN));
#endif

    // input mode
    t.c_iflag &= uInt(~(IXON | BRKINT | PARMRK));

    // defines the terminal special characters for noncanonical read
    t.c_cc[VTIME] = 0;  // Timeout in deciseconds
    t.c_cc[VMIN]  = 1;  // Minimum number of characters

    // set the new termios settings
    setTTY (t);
    raw_mode = true;
  }
  else
  {
    // local mode
    t.c_lflag |= uInt(ISIG | ICANON | (term_init.c_lflag & IEXTEN));

    // input mode
    t.c_iflag |= uInt(IXON | BRKINT | PARMRK);

    // set the new termios settings
    setTTY (t);
    raw_mode = false;
  }

  return raw_mode;
}

//----------------------------------------------------------------------
auto FTermios::getBaudRate() -> uInt
{
  std::unordered_map<speed_t, uInt> outspeed;
  outspeed[B0]      = 0;       // hang up
  outspeed[B50]     = 50;      //      50 baud
  outspeed[B75]     = 75;      //      75 baud
  outspeed[B110]    = 110;     //     110 baud
  outspeed[B134]    = 134;     //     134.5 baud
  outspeed[B150]    = 150;     //     150 baud
  outspeed[B200]    = 200;     //     200 baud
  outspeed[B300]    = 300;     //     300 baud
  outspeed[B600]    = 600;     //     600 baud
  outspeed[B1200]   = 1200;    //   1,200 baud
  outspeed[B1800]   = 1800;    //   1,800 baud
  outspeed[B2400]   = 2400;    //   2,400 baud
  outspeed[B4800]   = 4800;    //   4,800 baud
  outspeed[B9600]   = 9600;    //   9,600 baud
  outspeed[B19200]  = 19200;   //  19,200 baud
  outspeed[B38400]  = 38400;   //  38,400 baud
  outspeed[B57600]  = 57600;   //  57,600 baud
  outspeed[B115200] = 115200;  // 115,200 baud
  outspeed[B230400] = 230400;  // 230,400 baud

  if ( outspeed.find(cfgetospeed(&term_init)) != outspeed.end() )
    return outspeed[cfgetospeed(&term_init)];

  return 0;
}

}  // namespace finalcut
