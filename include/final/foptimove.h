/***********************************************************************
* foptimove.cpp - Cursor movement optimization                         *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2015-2018 Markus Gans                                      *
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
 * ▕▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FOptiMove ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▏
 */

// The cursor optimization based on ncurses lib_mvcur.c
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef FOPTIMOVE_H
#define FOPTIMOVE_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <assert.h>

#if defined(__sun) && defined(__SVR4)
  #include <termio.h>
  typedef struct termio SGTTY;
  typedef struct termios SGTTYS;

  #ifdef _LP64
    typedef unsigned int chtype;
  #else
    typedef unsigned long chtype;
  #endif

  #include <term.h>  // need for tparm
#else
  #include <term.h>  // need for tparm
#endif

#include <cctype>
#include <climits>
#include <cstdio>  // need for printf
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "final/ftypes.h"

//----------------------------------------------------------------------
// class FOptiMove
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FOptiMove
{
  public:
    // Constructor
    explicit FOptiMove (int = 0);

    // Destructor
    ~FOptiMove();

    // Accessors
    const char* getClassName() const;

    // Mutators
    void  setBaudRate (int);
    void  setTabStop (int);
    void  setTermSize (int, int);
    uInt  set_cursor_home (char[]);
    uInt  set_cursor_to_ll (char[]);
    uInt  set_carriage_return (char[]);
    uInt  set_tabular (char[]);
    uInt  set_back_tab (char[]);
    uInt  set_cursor_up (char[]);
    uInt  set_cursor_down (char[]);
    uInt  set_cursor_left (char[]);
    uInt  set_cursor_right (char[]);
    uInt  set_cursor_address (char[]);
    uInt  set_column_address (char[]);
    uInt  set_row_address (char[]);
    uInt  set_parm_up_cursor (char[]);
    uInt  set_parm_down_cursor (char[]);
    uInt  set_parm_left_cursor (char[]);
    uInt  set_parm_right_cursor (char[]);
    uInt  set_erase_chars (char[]);
    uInt  set_repeat_char (char[]);
    uInt  set_clr_bol (char[]);
    uInt  set_clr_eol (char[]);
    void  set_auto_left_margin (const bool&);
    void  set_eat_newline_glitch (const bool&);

    // Methods
    void  check_boundaries (int&, int&, int&, int&);
    char* moveCursor (int, int, int, int);
    void  printDurations();

  private:
    // Constant
    static const std::size_t BUF_SIZE = 512;

    // Typedef
    typedef struct
    {
      char* cap;
      int duration;
      int length;
    } capability;

    // Constants
    static const int LONG_DURATION = INT_MAX;
    // value for a long capability waiting time
    static const int MOVE_LIMIT = 7;
    // maximum character distance to avoid direct cursor addressing

    // Methods
    void calculateCharDuration();
    int  capDuration (char[], int);
    int  capDurationToLength (int);
    int  repeatedAppend (const capability&, volatile int, char*);
    int  relativeMove (char[], int, int, int, int);
    int  verticalMove (char[], int, int);
    void downMove (char[], int&, int, int);
    void upMove (char[], int&, int, int);
    int  horizontalMove (char[], int, int);
    void rightMove (char[], int&, int, int);
    void leftMove (char[], int&, int, int);

    bool isWideMove (int, int, int, int);
    bool isMethod0Faster (int&, int, int);
    bool isMethod1Faster (int&, int, int, int, int);
    bool isMethod2Faster (int&, int, int, int);
    bool isMethod3Faster (int&, int, int);
    bool isMethod4Faster (int&, int, int);
    bool isMethod5Faster (int&, int, int, int);
    void moveByMethod (int, int, int, int, int);

    // Data Members
    capability  F_cursor_home;
    capability  F_carriage_return;
    capability  F_cursor_to_ll;
    capability  F_tab;
    capability  F_back_tab;
    capability  F_cursor_up;
    capability  F_cursor_down;
    capability  F_cursor_left;
    capability  F_cursor_right;
    capability  F_cursor_address;
    capability  F_column_address;
    capability  F_row_address;
    capability  F_parm_up_cursor;
    capability  F_parm_down_cursor;
    capability  F_parm_left_cursor;
    capability  F_parm_right_cursor;
    capability  F_erase_chars;
    capability  F_repeat_char;
    capability  F_clr_bol;
    capability  F_clr_eol;

    bool        automatic_left_margin;
    bool        eat_nl_glitch;

    char        move_buf[BUF_SIZE];
    int         char_duration;
    int         baudrate;
    int         tabstop;
    int         screen_width;
    int         screen_height;
};
#pragma pack(pop)


// FOptiMove inline functions
//----------------------------------------------------------------------
inline const char* FOptiMove::getClassName() const
{ return "FOptiMove"; }

//----------------------------------------------------------------------
inline void FOptiMove::set_auto_left_margin (const bool& bcap)
{ automatic_left_margin = bcap; }

//----------------------------------------------------------------------
inline void FOptiMove::set_eat_newline_glitch (const bool& bcap)
{ eat_nl_glitch = bcap; }

#endif  // FOPTIMOVE_H
