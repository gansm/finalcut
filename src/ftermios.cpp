/***********************************************************************
* ftermios.cpp - Provides access to POSIX tty I/O control              *
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

#include <map>

#include "final/ftermios.h"
#include "final/fterm.h"

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

//----------------------------------------------------------------------
FTermios::~FTermios()  // destructor
{ }


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
termios FTermios::getTTY()
{
  struct termios t;
  tcgetattr (stdin_no, &t);
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
  term_init = getTTY();
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
  struct termios t;
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
  struct termios t;
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
  struct termios t;
  tcgetattr (stdin_no, &t);
  t.c_lflag &= uInt(~(ICANON | ECHO));
  t.c_cc[VTIME] = 1;  // Timeout in deciseconds
  t.c_cc[VMIN]  = 0;  // Minimum number of characters
  tcsetattr (stdin_no, TCSANOW, &t);
}

//----------------------------------------------------------------------
void FTermios::unsetCaptureSendCharacters()
{
  struct termios t;
  tcgetattr (stdin_no, &t);
  t.c_lflag |= uInt(ICANON | ECHO);
  t.c_cc[VTIME] = 0;  // Timeout in deciseconds
  t.c_cc[VMIN]  = 1;  // Minimum number of characters
  setTTY (t);
}

//----------------------------------------------------------------------
bool FTermios::setRawMode (bool on)
{
  // set + unset flags for raw mode
  if ( on == raw_mode )
    return raw_mode;

  // Info under: man 3 termios
  struct termios t;
  tcgetattr (stdin_no, &t);

  if ( on )
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
uInt FTermios::getBaudRate()
{
  std::map<speed_t,uInt> outspeed;
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

  return outspeed[cfgetospeed(&term_init)];
}

}  // namespace finalcut
