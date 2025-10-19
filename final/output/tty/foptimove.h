/***********************************************************************
* foptimove.h - Cursor movement optimization                           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2025 Markus Gans                                      *
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
    struct TermEnvCursor
    {
      const char* t_cursor_up;
      const char* t_cursor_down;
      const char* t_cursor_left;
      const char* t_cursor_right;
      const char* t_cursor_home;
      const char* t_cursor_to_ll;
      const char* t_carriage_return;
      const char* t_tab;
      const char* t_back_tab;
    };

    struct TermEnvParamCursor
    {
      const char* t_parm_up_cursor;
      const char* t_parm_down_cursor;
      const char* t_parm_left_cursor;
      const char* t_parm_right_cursor;
      const char* t_cursor_address;
      const char* t_column_address;
      const char* t_row_address;
    };

    struct TermEnvEdit
    {
      const char* t_erase_chars;
      const char* t_repeat_char;
      const char* t_repeat_last_char;
      const char* t_clr_bol;
      const char* t_clr_eol;
    };

    struct TermEnv
    {
      TermEnvCursor cursor;
      TermEnvParamCursor param_cursor;
      TermEnvEdit edit;
      int tabstop;
      bool automatic_left_margin;
      bool eat_nl_glitch;
    };

    // Constructor
    explicit FOptiMove (int = 0);

    // Accessors
    auto  getClassName() const -> FString;
    static auto  getInstance() -> FOptiMove&;
    auto  getCursorHomeLength() const noexcept -> uInt;
    auto  getCarriageReturnLength() const noexcept -> uInt;
    auto  getCursorToLLLength() const noexcept -> uInt;
    auto  getTabLength() const noexcept -> uInt;
    auto  getBackTabLength() const noexcept -> uInt;
    auto  getCursorUpLength() const noexcept -> uInt;
    auto  getCursorDownLength() const noexcept -> uInt;
    auto  getCursorLeftLength() const noexcept -> uInt;
    auto  getCursorRightLength() const noexcept -> uInt;
    auto  getCursorAddressLength() const noexcept -> uInt;
    auto  getColumnAddressLength() const noexcept -> uInt;
    auto  getRowAddressLength() const noexcept -> uInt;
    auto  getParmUpCursorLength() const noexcept -> uInt;
    auto  getParmDownCursorLength() const noexcept -> uInt;
    auto  getParmLeftCursorLength() const noexcept -> uInt;
    auto  getParmRightCursorLength() const noexcept -> uInt;
    auto  getEraseCharsLength() const noexcept -> uInt;
    auto  getRepeatCharLength() const noexcept -> uInt;
    auto  getClrBolLength() const noexcept -> uInt;
    auto  getClrEolLength() const noexcept -> uInt;

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
    void  set_repeat_last_char (const char[]);
    void  set_clr_bol (const char[]);
    void  set_clr_eol (const char[]);
    void  set_auto_left_margin (bool = true) noexcept;
    void  set_eat_newline_glitch (bool = true) noexcept;

    // Methods
    void  check_boundaries (int&, int&, int&, int&) const noexcept;
    auto  moveCursor (int, int, int, int) -> std::string;

  private:
    struct Capability
    {
      const char* cap;
      int duration;
      int length;
    };

    struct Cursor
    {
      Capability  up{};
      Capability  down{};
      Capability  left{};
      Capability  right{};
      Capability  home{};
      Capability  to_ll{};
      Capability  carriage_return{};
      Capability  tab{};
      Capability  back_tab{};
    };

    struct ParamCursor
    {
      Capability  up{};
      Capability  down{};
      Capability  left{};
      Capability  right{};
      Capability  column_address{};
      Capability  row_address{};
      Capability  address{};
    };

    struct Edit
    {
      Capability  erase_chars{};
      Capability  repeat_char{};
      Capability  repeat_last_char{};
      Capability  clr_bol{};
      Capability  clr_eol{};
    };

    struct Dimension
    {
      std::size_t width{};
      std::size_t height{};
    };

    // Constant
    static constexpr std::string::size_type BUF_SIZE{512u};

    // Constants
    static constexpr int LONG_DURATION{INT_MAX};
    // value for a long capability waiting time
    static constexpr int MOVE_LIMIT{7};
    // maximum character distance to avoid direct cursor addressing

    // Methods
    void  calculateCharDuration() noexcept;
    auto  capDuration (const char[], int) const noexcept -> int;
    auto  capDurationToLength (int) const noexcept -> int;
    auto  repeatedAppend (std::string&, const Capability&, int) const -> int;
    auto  relativeMove (std::string&, int, int, int, int) const -> int;
    auto  verticalMove (std::string&, int, int) const -> int;
    void  downMove (std::string&, int&, int, int) const;
    void  upMove (std::string&, int&, int, int) const;
    auto  horizontalMove (std::string&, int, int) const -> int;
    void  moveWithParmRightCursor (std::string&, int&, int) const;
    void  moveWithRightCursor (std::string&, int&, int, int, int) const;
    void  rightMove (std::string&, int&, int, int) const;
    void  moveWithParmLeftCursor (std::string&, int&, int) const;
    void  moveWithLeftCursor (std::string&, int&, int, int, int) const;
    void  leftMove (std::string&, int&, int, int) const;

    auto  isWideMove (int, int, int, int) const noexcept -> bool;
    auto  isMethod0Faster (int&, int, int) -> bool;
    auto  isMethod1Faster (int&, int, int, int, int) -> bool;
    auto  isMethod2Faster (int&, int, int, int) -> bool;
    auto  isMethod3Faster (int&, int, int) -> bool;
    auto  isMethod4Faster (int&, int, int) -> bool;
    auto  isMethod5Faster (int&, int, int, int) -> bool;
    void  moveByMethod (int, int, int, int, int);
    void  moveWithCarriageReturn (int, int, int);
    void  moveWithHome (int, int);
    void  moveWithToLL (int, int);
    void  moveWithCRAndWrapToLeft (int, int, int, int);

    // Data members
    Cursor      cursor{};
    ParamCursor parm_cursor{};
    Edit        edit{};
    Dimension   screen{80, 24};
    int         char_duration{1};
    int         baudrate{9600};
    int         tabstop{0};
    std::string move_buf{};
    std::string temp_result{};
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
inline auto FOptiMove::getCursorHomeLength() const noexcept -> uInt
{ return static_cast<uInt>(cursor.home.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getCarriageReturnLength() const noexcept -> uInt
{ return static_cast<uInt>(cursor.carriage_return.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getCursorToLLLength() const noexcept -> uInt
{ return static_cast<uInt>(cursor.to_ll.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getTabLength() const noexcept -> uInt
{ return static_cast<uInt>(cursor.tab.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getBackTabLength() const noexcept -> uInt
{ return static_cast<uInt>(cursor.back_tab.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getCursorUpLength() const noexcept -> uInt
{ return static_cast<uInt>(cursor.up.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getCursorDownLength() const noexcept -> uInt
{ return static_cast<uInt>(cursor.down.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getCursorLeftLength() const noexcept -> uInt
{ return static_cast<uInt>(cursor.left.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getCursorRightLength() const noexcept -> uInt
{ return static_cast<uInt>(cursor.right.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getCursorAddressLength() const noexcept -> uInt
{ return static_cast<uInt>(parm_cursor.address.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getColumnAddressLength() const noexcept -> uInt
{ return static_cast<uInt>(parm_cursor.column_address.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getRowAddressLength() const noexcept -> uInt
{ return static_cast<uInt>(parm_cursor.row_address.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getParmUpCursorLength() const noexcept -> uInt
{ return static_cast<uInt>(parm_cursor.up.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getParmDownCursorLength() const noexcept -> uInt
{ return static_cast<uInt>(parm_cursor.down.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getParmLeftCursorLength() const noexcept -> uInt
{ return static_cast<uInt>(parm_cursor.left.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getParmRightCursorLength() const noexcept -> uInt
{ return static_cast<uInt>(parm_cursor.right.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getEraseCharsLength() const noexcept -> uInt
{ return static_cast<uInt>(edit.erase_chars.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getRepeatCharLength() const noexcept -> uInt
{ return static_cast<uInt>(edit.repeat_char.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getClrBolLength() const noexcept -> uInt
{ return static_cast<uInt>(edit.clr_bol.length); }

//----------------------------------------------------------------------
inline auto FOptiMove::getClrEolLength() const noexcept -> uInt
{ return static_cast<uInt>(edit.clr_eol.length); }

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
