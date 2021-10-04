/***********************************************************************
* foptimove.cpp - Cursor movement optimization                         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2021 Markus Gans                                      *
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
#include <string>

#include "final/util/fstring.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FOptiMove
//----------------------------------------------------------------------

class FOptiMove final
{
  public:
    struct TermEnv
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
    };

    // Constructor
    explicit FOptiMove (int = 0);

    // Accessors
    FString       getClassName() const;
    static auto   getInstance() -> FOptiMove&;
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
    void          setTermEnvironment (const TermEnv&);
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
    void          set_auto_left_margin (bool = true);
    void          set_eat_newline_glitch (bool = true);

    // Methods
    void          check_boundaries (int&, int&, int&, int&) const;
    std::string   moveCursor (int, int, int, int);

  private:
    struct Capability
    {
      const char* cap;
      int duration;
      int length;
    };

    // Constant
    static constexpr std::string::size_type BUF_SIZE{512u};

    // Constants
    static constexpr int LONG_DURATION{INT_MAX};
    // value for a long capability waiting time
    static constexpr int MOVE_LIMIT{7};
    // maximum character distance to avoid direct cursor addressing

    // Methods
    void          calculateCharDuration();
    int           capDuration (const char[], int) const;
    int           capDurationToLength (int) const;
    int           repeatedAppend (std::string&, const Capability&, int) const;
    int           relativeMove (std::string&, int, int, int, int) const;
    int           verticalMove (std::string&, int, int) const;
    void          downMove (std::string&, int&, int, int) const;
    void          upMove (std::string&, int&, int, int) const;
    int           horizontalMove (std::string&, int, int) const;
    void          rightMove (std::string&, int&, int, int) const;
    void          leftMove (std::string&, int&, int, int) const;

    bool          isWideMove (int, int, int, int) const;
    bool          isMethod0Faster (int&, int, int);
    bool          isMethod1Faster (int&, int, int, int, int) const;
    bool          isMethod2Faster (int&, int, int, int) const;
    bool          isMethod3Faster (int&, int, int) const;
    bool          isMethod4Faster (int&, int, int) const;
    bool          isMethod5Faster (int&, int, int, int) const;
    void          moveByMethod (int, int, int, int, int);

    // Data members
    Capability    F_cursor_home{};
    Capability    F_carriage_return{};
    Capability    F_cursor_to_ll{};
    Capability    F_tab{};
    Capability    F_back_tab{};
    Capability    F_cursor_up{};
    Capability    F_cursor_down{};
    Capability    F_cursor_left{};
    Capability    F_cursor_right{};
    Capability    F_cursor_address{};
    Capability    F_column_address{};
    Capability    F_row_address{};
    Capability    F_parm_up_cursor{};
    Capability    F_parm_down_cursor{};
    Capability    F_parm_left_cursor{};
    Capability    F_parm_right_cursor{};
    Capability    F_erase_chars{};
    Capability    F_repeat_char{};
    Capability    F_clr_bol{};
    Capability    F_clr_eol{};

    std::size_t   screen_width{80};
    std::size_t   screen_height{24};
    int           char_duration{1};
    int           baudrate{9600};
    int           tabstop{0};
    std::string   move_buf{};
    bool          automatic_left_margin{false};
    bool          eat_nl_glitch{false};

    // Friend function
    friend void   printDurations (const FOptiMove&);
};


// FOptiMove inline functions
//----------------------------------------------------------------------
inline FString FOptiMove::getClassName() const
{ return "FOptiMove"; }

//----------------------------------------------------------------------
inline uInt FOptiMove::getCursorHomeLength() const
{ return static_cast<uInt>(F_cursor_home.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getCarriageReturnLength() const
{ return static_cast<uInt>(F_carriage_return.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getCursorToLLLength() const
{ return static_cast<uInt>(F_cursor_to_ll.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getTabLength() const
{ return static_cast<uInt>(F_tab.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getBackTabLength() const
{ return static_cast<uInt>(F_back_tab.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getCursorUpLength() const
{ return static_cast<uInt>(F_cursor_up.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getCursorDownLength() const
{ return static_cast<uInt>(F_cursor_down.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getCursorLeftLength() const
{ return static_cast<uInt>(F_cursor_left.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getCursorRightLength() const
{ return static_cast<uInt>(F_cursor_right.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getCursorAddressLength() const
{ return static_cast<uInt>(F_cursor_address.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getColumnAddressLength() const
{ return static_cast<uInt>(F_column_address.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getRowAddressLength() const
{ return static_cast<uInt>(F_row_address.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getParmUpCursorLength() const
{ return static_cast<uInt>(F_parm_up_cursor.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getParmDownCursorLength() const
{ return static_cast<uInt>(F_parm_down_cursor.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getParmLeftCursorLength() const
{ return static_cast<uInt>(F_parm_left_cursor.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getParmRightCursorLength() const
{ return static_cast<uInt>(F_parm_right_cursor.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getEraseCharsLength() const
{ return static_cast<uInt>(F_erase_chars.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getRepeatCharLength() const
{ return static_cast<uInt>(F_repeat_char.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getClrBolLength() const
{ return static_cast<uInt>(F_clr_bol.length); }

//----------------------------------------------------------------------
inline uInt FOptiMove::getClrEolLength() const
{ return static_cast<uInt>(F_clr_eol.length); }

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
