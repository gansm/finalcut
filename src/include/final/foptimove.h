/***********************************************************************
* foptimove.cpp - Cursor movement optimization                         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2020 Markus Gans                                      *
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
#include <cctype>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "final/fstring.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FOptiMove
//----------------------------------------------------------------------

class FOptiMove final
{
  public:
    // Typedef
    typedef struct
    {
      const char* t_cursor_home;
      const char* t_carriage_return;
      const char* t_cursor_to_ll;
      const char* t_tab;
      const char* t_back_tab;
      const char* t_cursor_up;
      const char* t_cursor_down;
      const char* t_cursor_left;
      const char* t_cursor_right;
      const char* t_cursor_address;
      const char* t_column_address;
      const char* t_row_address;
      const char* t_parm_up_cursor;
      const char* t_parm_down_cursor;
      const char* t_parm_left_cursor;
      const char* t_parm_right_cursor;
      const char* t_erase_chars;
      const char* t_repeat_char;
      const char* t_clr_bol;
      const char* t_clr_eol;
      int   tabstop;
      bool  automatic_left_margin;
      bool  eat_nl_glitch;
    } termEnv;

    // Constructor
    explicit FOptiMove (int = 0);

    // Destructor
    ~FOptiMove();

    // Accessors
    const FString getClassName() const;
    uInt          getCursorHomeLength() const;
    uInt          getCarriageReturnLength() const;
    uInt          getCursorToLLLength() const;
    uInt          getTabLength() const;
    uInt          getBackTabLength() const;
    uInt          getCursorUpLength() const;
    uInt          getCursorDownLength() const;
    uInt          getCursorLeftLength() const;
    uInt          getCursorRightLength() const;
    uInt          getCursorAddressLength() const;
    uInt          getColumnAddressLength() const;
    uInt          getRowAddressLength() const;
    uInt          getParmUpCursorLength() const;
    uInt          getParmDownCursorLength() const;
    uInt          getParmLeftCursorLength() const;
    uInt          getParmRightCursorLength() const;
    uInt          getEraseCharsLength() const;
    uInt          getRepeatCharLength() const;
    uInt          getClrBolLength() const;
    uInt          getClrEolLength() const;

    // Mutators
    void          setBaudRate (int);
    void          setTabStop (int);
    void          setTermSize (std::size_t, std::size_t);
    void          setTermEnvironment (const termEnv&);
    void          set_cursor_home (const char[]);
    void          set_cursor_to_ll (const char[]);
    void          set_carriage_return (const char[]);
    void          set_tabular (const char[]);
    void          set_back_tab (const char[]);
    void          set_cursor_up (const char[]);
    void          set_cursor_down (const char[]);
    void          set_cursor_left (const char[]);
    void          set_cursor_right (const char[]);
    void          set_cursor_address (const char[]);
    void          set_column_address (const char[]);
    void          set_row_address (const char[]);
    void          set_parm_up_cursor (const char[]);
    void          set_parm_down_cursor (const char[]);
    void          set_parm_left_cursor (const char[]);
    void          set_parm_right_cursor (const char[]);
    void          set_erase_chars (const char[]);
    void          set_repeat_char (const char[]);
    void          set_clr_bol (const char[]);
    void          set_clr_eol (const char[]);
    void          set_auto_left_margin (bool);
    void          set_eat_newline_glitch (bool);

    // Methods
    void          check_boundaries (int&, int&, int&, int&) const;
    const char*   moveCursor (int, int, int, int);

  private:
    // Constant
    static constexpr std::size_t BUF_SIZE{512};

    // Typedef
    typedef struct
    {
      const char* cap;
      int duration;
      int length;
    } capability;

    // Constants
    static constexpr int LONG_DURATION{INT_MAX};
    // value for a long capability waiting time
    static constexpr int MOVE_LIMIT{7};
    // maximum character distance to avoid direct cursor addressing

    // Methods
    void          calculateCharDuration();
    int           capDuration (const char[], int) const;
    int           capDurationToLength (int) const;
    int           repeatedAppend (const capability&, volatile int, char*) const;
    int           relativeMove (char[], int, int, int, int);
    int           verticalMove (char[], int, int) const;
    void          downMove (char[], int&, int, int) const;
    void          upMove (char[], int&, int, int) const;
    int           horizontalMove (char[], int, int) const;
    void          rightMove (char[], int&, int, int) const;
    void          leftMove (char[], int&, int, int) const;

    bool          isWideMove (int, int, int, int) const;
    bool          isMethod0Faster (int&, int, int);
    bool          isMethod1Faster (int&, int, int, int, int);
    bool          isMethod2Faster (int&, int, int, int);
    bool          isMethod3Faster (int&, int, int);
    bool          isMethod4Faster (int&, int, int);
    bool          isMethod5Faster (int&, int, int, int);
    void          moveByMethod (int, int, int, int, int);

    // Data members
    capability    F_cursor_home{};
    capability    F_carriage_return{};
    capability    F_cursor_to_ll{};
    capability    F_tab{};
    capability    F_back_tab{};
    capability    F_cursor_up{};
    capability    F_cursor_down{};
    capability    F_cursor_left{};
    capability    F_cursor_right{};
    capability    F_cursor_address{};
    capability    F_column_address{};
    capability    F_row_address{};
    capability    F_parm_up_cursor{};
    capability    F_parm_down_cursor{};
    capability    F_parm_left_cursor{};
    capability    F_parm_right_cursor{};
    capability    F_erase_chars{};
    capability    F_repeat_char{};
    capability    F_clr_bol{};
    capability    F_clr_eol{};

    std::size_t   screen_width{80};
    std::size_t   screen_height{24};
    int           char_duration{1};
    int           baudrate{9600};
    int           tabstop{0};
    char          move_buf[BUF_SIZE]{'\0'};
    bool          automatic_left_margin{false};
    bool          eat_nl_glitch{false};

    // Friend function
    friend void   printDurations (const FOptiMove&);
};


// FOptiMove inline functions
//----------------------------------------------------------------------
inline const FString FOptiMove::getClassName() const
{ return "FOptiMove"; }

//----------------------------------------------------------------------
inline uInt FOptiMove::getCursorHomeLength() const
{ return uInt(F_cursor_home.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getCarriageReturnLength() const
{ return uInt(F_carriage_return.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getCursorToLLLength() const
{ return uInt(F_cursor_to_ll.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getTabLength() const
{ return uInt(F_tab.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getBackTabLength() const
{ return uInt(F_back_tab.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getCursorUpLength() const
{ return uInt(F_cursor_up.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getCursorDownLength() const
{ return uInt(F_cursor_down.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getCursorLeftLength() const
{ return uInt(F_cursor_left.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getCursorRightLength() const
{ return uInt(F_cursor_right.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getCursorAddressLength() const
{ return uInt(F_cursor_address.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getColumnAddressLength() const
{ return uInt(F_column_address.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getRowAddressLength() const
{ return uInt(F_row_address.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getParmUpCursorLength() const
{ return uInt(F_parm_up_cursor.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getParmDownCursorLength() const
{ return uInt(F_parm_down_cursor.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getParmLeftCursorLength() const
{ return uInt(F_parm_left_cursor.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getParmRightCursorLength() const
{ return uInt(F_parm_right_cursor.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getEraseCharsLength() const
{ return uInt(F_erase_chars.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getRepeatCharLength() const
{ return uInt(F_repeat_char.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getClrBolLength() const
{ return uInt(F_clr_bol.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getClrEolLength() const
{ return uInt(F_clr_eol.length); }

//----------------------------------------------------------------------
inline void FOptiMove::set_auto_left_margin (bool bcap)
{ automatic_left_margin = bcap; }

//----------------------------------------------------------------------
inline void FOptiMove::set_eat_newline_glitch (bool bcap)
{ eat_nl_glitch = bcap; }


// FOptiMove non-member function forward declaration
//----------------------------------------------------------------------
void printDurations (const FOptiMove&);

}  // namespace finalcut

#endif  // FOPTIMOVE_H
