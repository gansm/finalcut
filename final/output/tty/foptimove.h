/***********************************************************************
* foptimove.cpp - Cursor movement optimization                         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2022 Markus Gans                                      *
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

#include <cassert>
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
    auto  getClassName() const -> FString;
    static auto  getInstance() -> FOptiMove&;
    auto  getCursorHomeLength() const -> uInt;
    auto  getCarriageReturnLength() const -> uInt;
    auto  getCursorToLLLength() const -> uInt;
    auto  getTabLength() const -> uInt;
    auto  getBackTabLength() const -> uInt;
    auto  getCursorUpLength() const -> uInt;
    auto  getCursorDownLength() const -> uInt;
    auto  getCursorLeftLength() const -> uInt;
    auto  getCursorRightLength() const -> uInt;
    auto  getCursorAddressLength() const -> uInt;
    auto  getColumnAddressLength() const -> uInt;
    auto  getRowAddressLength() const -> uInt;
    auto  getParmUpCursorLength() const -> uInt;
    auto  getParmDownCursorLength() const -> uInt;
    auto  getParmLeftCursorLength() const -> uInt;
    auto  getParmRightCursorLength() const -> uInt;
    auto  getEraseCharsLength() const -> uInt;
    auto  getRepeatCharLength() const -> uInt;
    auto  getClrBolLength() const -> uInt;
    auto  getClrEolLength() const -> uInt;

    // Mutators
    void  setBaudRate (int);
    void  setTabStop (int);
    void  setTermSize (std::size_t, std::size_t);
    void  setTermEnvironment (const TermEnv&);
    void  set_cursor_home (const char[]);
    void  set_cursor_to_ll (const char[]);
    void  set_carriage_return (const char[]);
    void  set_tabular (const char[]);
    void  set_back_tab (const char[]);
    void  set_cursor_up (const char[]);
    void  set_cursor_down (const char[]);
    void  set_cursor_left (const char[]);
    void  set_cursor_right (const char[]);
    void  set_cursor_address (const char[]);
    void  set_column_address (const char[]);
    void  set_row_address (const char[]);
    void  set_parm_up_cursor (const char[]);
    void  set_parm_down_cursor (const char[]);
    void  set_parm_left_cursor (const char[]);
    void  set_parm_right_cursor (const char[]);
    void  set_erase_chars (const char[]);
    void  set_repeat_char (const char[]);
    void  set_clr_bol (const char[]);
    void  set_clr_eol (const char[]);
    void  set_auto_left_margin (bool = true) noexcept;
    void  set_eat_newline_glitch (bool = true) noexcept;

    // Methods
    void  check_boundaries (int&, int&, int&, int&) const;
    auto  moveCursor (int, int, int, int) -> std::string;

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
    void  calculateCharDuration();
    auto  capDuration (const char[], int) const -> int;
    auto  capDurationToLength (int) const -> int;
    auto  repeatedAppend (std::string&, const Capability&, int) const -> int;
    auto  relativeMove (std::string&, int, int, int, int) const -> int;
    auto  verticalMove (std::string&, int, int) const -> int;
    void  downMove (std::string&, int&, int, int) const;
    void  upMove (std::string&, int&, int, int) const;
    auto  horizontalMove (std::string&, int, int) const -> int;
    void  rightMove (std::string&, int&, int, int) const;
    void  leftMove (std::string&, int&, int, int) const;

    auto  isWideMove (int, int, int, int) const -> bool;
    auto  isMethod0Faster (int&, int, int) -> bool;
    auto  isMethod1Faster (int&, int, int, int, int) const -> bool;
    auto  isMethod2Faster (int&, int, int, int) const -> bool;
    auto  isMethod3Faster (int&, int, int) const -> bool;
    auto  isMethod4Faster (int&, int, int) const -> bool;
    auto  isMethod5Faster (int&, int, int, int) const -> bool;
    void  moveByMethod (int, int, int, int, int);

    // Data members
    Capability  F_cursor_home{};
    Capability  F_carriage_return{};
    Capability  F_cursor_to_ll{};
    Capability  F_tab{};
    Capability  F_back_tab{};
    Capability  F_cursor_up{};
    Capability  F_cursor_down{};
    Capability  F_cursor_left{};
    Capability  F_cursor_right{};
    Capability  F_cursor_address{};
    Capability  F_column_address{};
    Capability  F_row_address{};
    Capability  F_parm_up_cursor{};
    Capability  F_parm_down_cursor{};
    Capability  F_parm_left_cursor{};
    Capability  F_parm_right_cursor{};
    Capability  F_erase_chars{};
    Capability  F_repeat_char{};
    Capability  F_clr_bol{};
    Capability  F_clr_eol{};

    std::size_t screen_width{80};
    std::size_t screen_height{24};
    int         char_duration{1};
    int         baudrate{9600};
    int         tabstop{0};
    std::string move_buf{};
    bool        automatic_left_margin{false};
    bool        eat_nl_glitch{false};

    // Friend function
    friend void printDurations (const FOptiMove&);
};


// FOptiMove inline functions
//----------------------------------------------------------------------
inline auto FOptiMove::getClassName() const -> FString
{ return "FOptiMove"; }

//----------------------------------------------------------------------
inline auto FOptiMove::getCursorHomeLength() const -> uInt
{ return static_cast<uInt>(F_cursor_home.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getCarriageReturnLength() const -> uInt
{ return static_cast<uInt>(F_carriage_return.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getCursorToLLLength() const -> uInt
{ return static_cast<uInt>(F_cursor_to_ll.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getTabLength() const -> uInt
{ return static_cast<uInt>(F_tab.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getBackTabLength() const -> uInt
{ return static_cast<uInt>(F_back_tab.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getCursorUpLength() const -> uInt
{ return static_cast<uInt>(F_cursor_up.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getCursorDownLength() const -> uInt
{ return static_cast<uInt>(F_cursor_down.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getCursorLeftLength() const -> uInt
{ return static_cast<uInt>(F_cursor_left.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getCursorRightLength() const -> uInt
{ return static_cast<uInt>(F_cursor_right.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getCursorAddressLength() const -> uInt
{ return static_cast<uInt>(F_cursor_address.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getColumnAddressLength() const -> uInt
{ return static_cast<uInt>(F_column_address.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getRowAddressLength() const -> uInt
{ return static_cast<uInt>(F_row_address.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getParmUpCursorLength() const -> uInt
{ return static_cast<uInt>(F_parm_up_cursor.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getParmDownCursorLength() const -> uInt
{ return static_cast<uInt>(F_parm_down_cursor.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getParmLeftCursorLength() const -> uInt
{ return static_cast<uInt>(F_parm_left_cursor.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getParmRightCursorLength() const -> uInt
{ return static_cast<uInt>(F_parm_right_cursor.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getEraseCharsLength() const -> uInt
{ return static_cast<uInt>(F_erase_chars.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getRepeatCharLength() const -> uInt
{ return static_cast<uInt>(F_repeat_char.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getClrBolLength() const -> uInt
{ return static_cast<uInt>(F_clr_bol.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getClrEolLength() const -> uInt
{ return static_cast<uInt>(F_clr_eol.length); }

//----------------------------------------------------------------------
inline void FOptiMove::set_auto_left_margin (bool bcap) noexcept
{ automatic_left_margin = bcap; }

//----------------------------------------------------------------------
inline void FOptiMove::set_eat_newline_glitch (bool bcap) noexcept
{ eat_nl_glitch = bcap; }


// FOptiMove non-member function forward declaration
//----------------------------------------------------------------------
void printDurations (const FOptiMove&);

}  // namespace finalcut

#endif  // FOPTIMOVE_H
