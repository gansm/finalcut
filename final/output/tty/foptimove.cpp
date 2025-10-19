/***********************************************************************
* foptimove.cpp - Cursor movement optimization                         *
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

#include <algorithm>
#include <array>
#include <cstring>
#include <memory>

#include "final/fapplication.h"
#include "final/fc.h"
#include "final/output/tty/foptimove.h"
#include "final/output/tty/ftermcap.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FOptiMove
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FOptiMove::FOptiMove (int baud)
  : baudrate{baud}
{
  assert ( baud >= 0 );
  move_buf.reserve(BUF_SIZE);
  calculateCharDuration();

  // ANSI set cursor address preset for undefined terminals
  set_cursor_address (CSI "%i%p1%d;%p2%dH");
  // Set carriage return preset
  set_carriage_return ("\r");
  // Set cursor down preset
  set_cursor_down ("\n");
}


// public methods of FOptiMove
//----------------------------------------------------------------------
auto FOptiMove::getInstance() -> FOptiMove&
{
  static const auto& opti_move = std::make_unique<FOptiMove>();
  return *opti_move;
}

//----------------------------------------------------------------------
void FOptiMove::setBaudRate (int baud)
{
  assert ( baud >= 0 );
  baudrate = baud;
  calculateCharDuration();
}

//----------------------------------------------------------------------
void FOptiMove::setTabStop (int t)
{
  assert ( t > 0 );
  tabstop = t;
}

//----------------------------------------------------------------------
void FOptiMove::setTermSize (std::size_t w, std::size_t h)
{
  assert ( w > 0 );
  assert ( h > 0 );
  screen.width = w;
  screen.height = h;
}

//----------------------------------------------------------------------
void FOptiMove::setTermEnvironment (const TermEnv& term_env)
{
  // Set all required termcap values at once

  set_cursor_home (term_env.cursor.t_cursor_home);
  set_cursor_to_ll (term_env.cursor.t_cursor_to_ll);
  set_carriage_return (term_env.cursor.t_carriage_return);
  set_tabular (term_env.cursor.t_tab);
  set_back_tab (term_env.cursor.t_back_tab);
  set_cursor_up (term_env.cursor.t_cursor_up);
  set_cursor_down (term_env.cursor.t_cursor_down);
  set_cursor_left (term_env.cursor.t_cursor_left);
  set_cursor_right (term_env.cursor.t_cursor_right);
  set_cursor_address (term_env.param_cursor.t_cursor_address);
  set_column_address (term_env.param_cursor.t_column_address);
  set_row_address (term_env.param_cursor.t_row_address);
  set_parm_up_cursor (term_env.param_cursor.t_parm_up_cursor);
  set_parm_down_cursor (term_env.param_cursor.t_parm_down_cursor);
  set_parm_left_cursor (term_env.param_cursor.t_parm_left_cursor);
  set_parm_right_cursor (term_env.param_cursor.t_parm_right_cursor);
  set_erase_chars (term_env.edit.t_erase_chars);
  set_repeat_char (term_env.edit.t_repeat_char);
  set_repeat_last_char (term_env.edit.t_repeat_last_char);
  set_clr_bol (term_env.edit.t_clr_bol);
  set_clr_eol (term_env.edit.t_clr_eol);
  setTabStop (term_env.tabstop);
  set_auto_left_margin (term_env.automatic_left_margin);
  set_eat_newline_glitch (term_env.eat_nl_glitch);
}

//----------------------------------------------------------------------
void FOptiMove::set_cursor_home (const char cap[])
{
  if ( cap )
  {
    cursor.home.cap = cap;
    cursor.home.duration = capDuration (cap, 0);
    cursor.home.length = capDurationToLength (cursor.home.duration);
  }
  else
  {
    cursor.home.cap = nullptr;
    cursor.home.duration = \
    cursor.home.length   = LONG_DURATION;
  }
}

//----------------------------------------------------------------------
void FOptiMove::set_cursor_to_ll (const char cap[])
{
  if ( cap )
  {
    cursor.to_ll.cap = cap;
    cursor.to_ll.duration = capDuration (cap, 0);
    cursor.to_ll.length = capDurationToLength (cursor.to_ll.duration);
  }
  else
  {
    cursor.to_ll.cap = nullptr;
    cursor.to_ll.duration = \
    cursor.to_ll.length   = LONG_DURATION;
  }
}

//----------------------------------------------------------------------
void FOptiMove::set_carriage_return (const char cap[])
{
  if ( cap )
  {
    cursor.carriage_return.cap = cap;
    cursor.carriage_return.duration = capDuration (cap, 0);
    cursor.carriage_return.length = capDurationToLength (cursor.carriage_return.duration);
  }
  else
  {
    cursor.carriage_return.cap = nullptr;
    cursor.carriage_return.duration = \
    cursor.carriage_return.length   = LONG_DURATION;
  }
}

//----------------------------------------------------------------------
void FOptiMove::set_tabular (const char cap[])
{
  if ( cap )
  {
    cursor.tab.cap = cap;
    cursor.tab.duration = capDuration (cap, 0);
    cursor.tab.length = capDurationToLength (cursor.tab.duration);
  }
  else
  {
    cursor.tab.cap = nullptr;
    cursor.tab.duration = \
    cursor.tab.length   = LONG_DURATION;
  }
}

//----------------------------------------------------------------------
void FOptiMove::set_back_tab (const char cap[])
{
  if ( cap )
  {
    cursor.back_tab.cap = cap;
    cursor.back_tab.duration = capDuration (cap, 0);
    cursor.back_tab.length = capDurationToLength (cursor.back_tab.duration);
  }
  else
  {
    cursor.back_tab.cap = nullptr;
    cursor.back_tab.duration = \
    cursor.back_tab.length   = LONG_DURATION;
  }
}

//----------------------------------------------------------------------
void FOptiMove::set_cursor_up (const char cap[])
{
  if ( cap )
  {
    cursor.up.cap = cap;
    cursor.up.duration = capDuration (cap, 0);
    cursor.up.length = capDurationToLength (cursor.up.duration);
  }
  else
  {
    cursor.up.cap = nullptr;
    cursor.up.duration = \
    cursor.up.length   = LONG_DURATION;
  }
}

//----------------------------------------------------------------------
void FOptiMove::set_cursor_down (const char cap[])
{
  if ( cap )
  {
    cursor.down.cap = cap;
    cursor.down.duration = capDuration (cap, 0);
    cursor.down.length = capDurationToLength (cursor.down.duration);
  }
  else
  {
    cursor.down.cap = nullptr;
    cursor.down.duration = \
    cursor.down.length   = LONG_DURATION;
  }
}

//----------------------------------------------------------------------
void FOptiMove::set_cursor_left (const char cap[])
{
  if ( cap )
  {
    cursor.left.cap = cap;
    cursor.left.duration = capDuration (cap, 0);
    cursor.left.length = capDurationToLength (cursor.left.duration);
  }
  else
  {
    cursor.left.cap = nullptr;
    cursor.left.duration = \
    cursor.left.length   = LONG_DURATION;
  }
}

//----------------------------------------------------------------------
void FOptiMove::set_cursor_right (const char cap[])
{
  if ( cap )
  {
    cursor.right.cap = cap;
    cursor.right.duration = capDuration (cap, 0);
    cursor.right.length = capDurationToLength (cursor.right.duration);
  }
  else
  {
    cursor.right.cap = nullptr;
    cursor.right.duration = \
    cursor.right.length   = LONG_DURATION;
  }
}

//----------------------------------------------------------------------
void FOptiMove::set_cursor_address (const char cap[])
{
  if ( cap && FTermcap::isInitialized() )
  {
    const auto temp = FTermcap::encodeMotionParameter(cap, 23, 23);
    parm_cursor.address.cap = cap;
    parm_cursor.address.duration = capDuration (temp.data(), 1);
    parm_cursor.address.length = capDurationToLength (parm_cursor.address.duration);
  }
  else
  {
    parm_cursor.address.cap = nullptr;
    parm_cursor.address.duration = \
    parm_cursor.address.length   = LONG_DURATION;
  }
}

//----------------------------------------------------------------------
void FOptiMove::set_column_address (const char cap[])
{
  if ( cap && FTermcap::isInitialized() )
  {
    const auto temp = FTermcap::encodeParameter(cap, 23);
    parm_cursor.column_address.cap = cap;
    parm_cursor.column_address.duration = capDuration (temp.data(), 1);
    parm_cursor.column_address.length = capDurationToLength (parm_cursor.column_address.duration);
  }
  else
  {
    parm_cursor.column_address.cap = nullptr;
    parm_cursor.column_address.duration = \
    parm_cursor.column_address.length   = LONG_DURATION;
  }
}

//----------------------------------------------------------------------
void FOptiMove::set_row_address (const char cap[])
{
  if ( cap && FTermcap::isInitialized() )
  {
    const auto temp = FTermcap::encodeParameter(cap, 23);
    parm_cursor.row_address.cap = cap;
    parm_cursor.row_address.duration = capDuration (temp.data(), 1);
    parm_cursor.row_address.length = capDurationToLength (parm_cursor.row_address.duration);
  }
  else
  {
    parm_cursor.row_address.cap = nullptr;
    parm_cursor.row_address.duration = \
    parm_cursor.row_address.length   = LONG_DURATION;
  }
}

//----------------------------------------------------------------------
void FOptiMove::set_parm_up_cursor (const char cap[])
{
  if ( cap && FTermcap::isInitialized() )
  {
    const auto temp = FTermcap::encodeParameter(cap, 23);
    parm_cursor.up.cap = cap;
    parm_cursor.up.duration = capDuration (temp.data(), 1);
    parm_cursor.up.length = capDurationToLength (parm_cursor.up.duration);
  }
  else
  {
    parm_cursor.up.cap = nullptr;
    parm_cursor.up.duration = \
    parm_cursor.up.length   = LONG_DURATION;
  }
}

//----------------------------------------------------------------------
void FOptiMove::set_parm_down_cursor (const char cap[])
{
  if ( cap && FTermcap::isInitialized() )
  {
    const auto temp = FTermcap::encodeParameter(cap, 23);
    parm_cursor.down.cap = cap;
    parm_cursor.down.duration = capDuration (temp.data(), 1);
    parm_cursor.down.length = capDurationToLength (parm_cursor.down.duration);
  }
  else
  {
    parm_cursor.down.cap = nullptr;
    parm_cursor.down.duration = \
    parm_cursor.down.length   = LONG_DURATION;
  }
}

//----------------------------------------------------------------------
void FOptiMove::set_parm_left_cursor (const char cap[])
{
  if ( cap && FTermcap::isInitialized() )
  {
    const auto temp = FTermcap::encodeParameter(cap, 23);
    parm_cursor.left.cap = cap;
    parm_cursor.left.duration = capDuration (temp.data(), 1);
    parm_cursor.left.length = capDurationToLength (parm_cursor.left.duration);
  }
  else
  {
    parm_cursor.left.cap = nullptr;
    parm_cursor.left.duration = \
    parm_cursor.left.length   = LONG_DURATION;
  }
}

//----------------------------------------------------------------------
void FOptiMove::set_parm_right_cursor (const char cap[])
{
  if ( cap && FTermcap::isInitialized() )
  {
    const auto temp = FTermcap::encodeParameter(cap, 23);
    parm_cursor.right.cap = cap;
    parm_cursor.right.duration = capDuration (temp.data(), 1);
    parm_cursor.right.length = capDurationToLength (parm_cursor.right.duration);
  }
  else
  {
    parm_cursor.right.cap = nullptr;
    parm_cursor.right.duration = \
    parm_cursor.right.length   = LONG_DURATION;
  }
}

//----------------------------------------------------------------------
void FOptiMove::set_erase_chars (const char cap[])
{
  if ( cap && FTermcap::isInitialized() )
  {
    const auto temp = FTermcap::encodeParameter(cap, 23);
    edit.erase_chars.cap = cap;
    edit.erase_chars.duration = capDuration (temp.data(), 1);
    edit.erase_chars.length = capDurationToLength (edit.erase_chars.duration);
  }
  else
  {
    edit.erase_chars.cap = nullptr;
    edit.erase_chars.duration = \
    edit.erase_chars.length   = LONG_DURATION;
  }
}

//----------------------------------------------------------------------
void FOptiMove::set_repeat_char (const char cap[])
{
  if ( cap && FTermcap::isInitialized() )
  {
    const auto temp = FTermcap::encodeParameter(cap, ' ', 23);
    edit.repeat_char.cap = cap;
    edit.repeat_char.duration = capDuration (temp.data(), 1);
    edit.repeat_char.length = capDurationToLength (edit.repeat_char.duration);
  }
  else
  {
    edit.repeat_char.cap = nullptr;
    edit.repeat_char.duration = \
    edit.repeat_char.length   = LONG_DURATION;
  }
}

//----------------------------------------------------------------------
void FOptiMove::set_repeat_last_char (const char cap[])
{
  if ( cap && FTermcap::isInitialized() )
  {
    const auto temp = FTermcap::encodeParameter(cap, 23);
    edit.repeat_last_char.cap = cap;
    edit.repeat_last_char.duration = capDuration (temp.data(), 1);
    edit.repeat_last_char.length = capDurationToLength (edit.repeat_last_char.duration);
  }
  else
  {
    edit.repeat_last_char.cap = nullptr;
    edit.repeat_last_char.duration = \
    edit.repeat_last_char.length   = LONG_DURATION;
  }
}

//----------------------------------------------------------------------
void FOptiMove::set_clr_bol (const char cap[])
{
  if ( cap )
  {
    edit.clr_bol.cap = cap;
    edit.clr_bol.duration = capDuration (cap, 0);
    edit.clr_bol.length = capDurationToLength (edit.clr_bol.duration);
  }
  else
  {
    edit.clr_bol.cap = nullptr;
    edit.clr_bol.duration = \
    edit.clr_bol.length   = LONG_DURATION;
  }
}

//----------------------------------------------------------------------
void FOptiMove::set_clr_eol (const char cap[])
{
  if ( cap )
  {
    edit.clr_eol.cap = cap;
    edit.clr_eol.duration = capDuration (cap, 0);
    edit.clr_eol.length = capDurationToLength (edit.clr_eol.duration);
  }
  else
  {
    edit.clr_eol.cap = nullptr;
    edit.clr_eol.duration = \
    edit.clr_eol.length   = LONG_DURATION;
  }
}

//----------------------------------------------------------------------
void FOptiMove::check_boundaries ( int& xold, int& yold
                                 , int& xnew, int& ynew ) const noexcept
{
  if ( xold < 0 || xold >= int(screen.width) )
    xold = -1;

  if ( yold < 0 || yold >= int(screen.height) )
    yold = -1;

  xnew = std::max(0, xnew);
  ynew = std::max(0, ynew);
  xnew = std::min(int(screen.width) - 1, xnew);
  ynew = std::min(int(screen.height) - 1, ynew);
}

//----------------------------------------------------------------------
auto FOptiMove::moveCursor (int xold, int yold, int xnew, int ynew) -> std::string
{
  int method{0};
  int move_time{LONG_DURATION};

  check_boundaries (xold, yold, xnew, ynew);

  // Method 0: direct cursor addressing
  if ( isMethod0Faster(move_time, xnew, ynew)
    && ( xold < 0
      || yold < 0
      || isWideMove (xold, yold, xnew, ynew) ) )
  {
    return ( move_time < LONG_DURATION ) ? move_buf : std::string{};
  }

  // Method 1: local movement
  if ( isMethod1Faster(move_time, xold, yold, xnew, ynew) )
    method = 1;

  // Method 2: carriage-return + local movement
  if ( isMethod2Faster(move_time, yold, xnew, ynew) )
    method = 2;

  // Method 3: home-cursor + local movement
  if ( isMethod3Faster(move_time, xnew, ynew) )
    method = 3;

  // Method 4: home-down + local movement
  if ( isMethod4Faster(move_time, xnew, ynew) )
    method = 4;

  // Method 5: left margin for wrap to right-hand side
  if ( isMethod5Faster(move_time, yold, xnew, ynew) )
    method = 5;

  // Copy the escape sequence for the chosen method in move_buf
  moveByMethod (method, xold, yold, xnew, ynew);

  if ( move_time < LONG_DURATION )
    return move_buf;

  return {};
}


// private methods of FApplication
//----------------------------------------------------------------------
void FOptiMove::calculateCharDuration() noexcept
{
  if ( baudrate != 0 )
  {
    static constexpr int baudbyte = 9;  // = 7 bit + 1 parity + 1 stop
    char_duration = (baudbyte * 1000 * 10)
                  / (baudrate > 0 ? baudrate : 9600);  // milliseconds

    if ( char_duration <= 0 )
      char_duration = 1;
  }
  else
    char_duration = 1;
}

//----------------------------------------------------------------------
auto FOptiMove::capDuration (const char cap[], int affcnt) const noexcept -> int
{
  // calculate the duration in milliseconds of a given operation
  // cap    - the term capability
  // affcnt - the number of lines affected

  if ( ! cap )
    return LONG_DURATION;

  float ms{0};
  const char* p = cap;

  const auto parse_digit = \
      [&p, affcnt] (float& num) noexcept
      {
        if ( *p >= '0' && *p <= '9' )
          num = num * 10 + float(*p - '0');
        else if ( *p == '*' )
          num *= float(affcnt);
        else if ( *p == '.' )
        {
          ++p;

          if ( *p != '>' && *p >= '0' && *p <= '9' )
            num += float((*p - '0') / 10.0);
        }
      };

  while ( *p )
  {
    // check for delay with padding character
    if ( p[0] == '$' && p[1] == '<' && std::strchr(p, '>') )
    {
      float num{0};
      p += 2;

      while ( *p != '>' )
      {
        parse_digit(num);
        ++p;
      }

      ms += num * 10;
    }
    else
      ms += float(char_duration);

    ++p;
  }

  return int(ms);
}

//----------------------------------------------------------------------
auto FOptiMove::capDurationToLength (int duration) const noexcept -> int
{
  if ( duration != LONG_DURATION )
    return (duration + char_duration - 1) / char_duration;

  return LONG_DURATION;
}

//----------------------------------------------------------------------
auto FOptiMove::repeatedAppend ( std::string& dst
                               , const Capability& o
                               , int count ) const -> int
{
  const auto src_len = stringLength(o.cap);
  const auto dst_len = dst.length();
  int total{0};

  if ( (dst_len + uInt(count) * src_len) < BUF_SIZE - 1 )
  {
    total += count * o.duration;

    while ( count > 0 )
    {
      --count;
      dst.append(o.cap);
    }
  }
  else
    total = LONG_DURATION;

  return total;
}

//----------------------------------------------------------------------
auto FOptiMove::relativeMove ( std::string& move
                             , int from_x, int from_y
                             , int to_x, int to_y ) const -> int
{
  int vtime{0};
  int htime{0};

  if ( ! move.empty() )
    move.clear();

  if ( to_y != from_y )  // vertical move
  {
    vtime = verticalMove (move, from_y, to_y);

    if ( vtime >= LONG_DURATION )
      return LONG_DURATION;
  }

  if ( to_x != from_x )  // horizontal move
  {
    std::string hmove{};
    htime = horizontalMove (hmove, from_x, to_x);

    if ( htime >= LONG_DURATION )
      return LONG_DURATION;

    if ( ! move.empty() )
      move.append(hmove);
    else
      move = std::move(hmove);
  }

  return vtime + htime;
}

//----------------------------------------------------------------------
inline auto FOptiMove::verticalMove (std::string& move, int from_y, int to_y) const -> int
{
  int vtime{LONG_DURATION};

  if ( parm_cursor.row_address.cap )
  {
    // Move to fixed row position
    move = FTermcap::encodeParameter(parm_cursor.row_address.cap, to_y);
    vtime = parm_cursor.row_address.duration;
  }

  if ( to_y > from_y )
    downMove (move, vtime, from_y, to_y);
  else  // to_y < from_y
    upMove (move, vtime, from_y, to_y);

  return vtime;
}

//----------------------------------------------------------------------
inline void FOptiMove::downMove ( std::string& move, int& vtime
                                , int from_y, int to_y ) const
{
  const int num = to_y - from_y;

  if ( parm_cursor.down.cap && parm_cursor.down.duration < vtime )
  {
    move = FTermcap::encodeParameter(parm_cursor.down.cap, num);
    vtime = parm_cursor.down.duration;
  }

  if ( cursor.down.cap && (num * cursor.down.duration < vtime) )
  {
    if ( ! move.empty() )
      move.clear();

    vtime = repeatedAppend (move, cursor.down, num);
  }
}

//----------------------------------------------------------------------
inline void FOptiMove::upMove ( std::string& move, int& vtime
                              , int from_y, int to_y ) const
{
  const int num = from_y - to_y;

  if ( parm_cursor.up.cap && parm_cursor.up.duration < vtime )
  {
    move = FTermcap::encodeParameter(parm_cursor.up.cap, num);
    vtime = parm_cursor.up.duration;
  }

  if ( cursor.up.cap && (num * cursor.up.duration < vtime) )
  {
    if ( ! move.empty() )
      move.clear();

    vtime = repeatedAppend (move, cursor.up, num);
  }
}

//----------------------------------------------------------------------
inline auto FOptiMove::horizontalMove (std::string& hmove, int from_x, int to_x) const -> int
{
  int htime{LONG_DURATION};

  if ( parm_cursor.column_address.cap )
  {
    // Move to fixed column position
    hmove = FTermcap::encodeParameter(parm_cursor.column_address.cap, to_x);
    htime = parm_cursor.column_address.duration;
  }

  if ( to_x > from_x )
    rightMove (hmove, htime, from_x, to_x);
  else  // to_x < from_x
    leftMove (hmove, htime, from_x, to_x);

  return htime;
}

//----------------------------------------------------------------------
inline void FOptiMove::moveWithParmRightCursor ( std::string& hmove
                                               , int& htime, int num ) const
{
  // Use parameterized cursor right capability
  hmove = FTermcap::encodeParameter(parm_cursor.right.cap, num);
  htime = parm_cursor.right.duration;
}

//----------------------------------------------------------------------
inline void FOptiMove::moveWithRightCursor ( std::string& hmove, int& htime
                                           , int num, int from_x, int to_x ) const
{
  std::string str{};
  int htime_r{0};

  // try to use tab
  if ( tabstop > 0 && cursor.tab.cap )
  {
    int pos = from_x;

    for ( int tab_pos = pos + tabstop - (pos % tabstop)
        ; tab_pos <= to_x
        ; tab_pos += tabstop )
    {
      htime_r += repeatedAppend (str, cursor.tab, 1);

      if ( htime_r >= LONG_DURATION )
        break;

      pos = tab_pos;
    }

    num = to_x - pos;
  }

  // Use the cursor right capability
  htime_r += repeatedAppend (str, cursor.right, num);

  if ( htime_r < htime )
  {
    hmove = std::move(str);
    htime = htime_r;
  }
}

//----------------------------------------------------------------------
inline void FOptiMove::rightMove ( std::string& hmove, int& htime
                                 , int from_x, int to_x ) const
{
  const int num = to_x - from_x;

  if ( num == 0 )
    return;

  if ( parm_cursor.right.cap && parm_cursor.right.duration < htime )
    moveWithParmRightCursor (hmove, htime, num);

  if ( cursor.right.cap )
    moveWithRightCursor (hmove, htime, num, from_x, to_x);
}

//----------------------------------------------------------------------
inline void FOptiMove::moveWithParmLeftCursor ( std::string& hmove
                                              , int& htime, int num ) const
{
  // Use parameterized cursor right capability
  hmove = FTermcap::encodeParameter(parm_cursor.left.cap, num);
  htime = parm_cursor.left.duration;
}

//----------------------------------------------------------------------
inline void FOptiMove::moveWithLeftCursor ( std::string& hmove, int& htime
                                          , int num, int from_x, int to_x ) const
{
  std::string str{};
  int htime_l{0};

  // try to use backward tab
  if ( tabstop > 0 && cursor.back_tab.cap )
  {
    int pos = from_x;

    for ( int tab_pos = ( pos > 0 ) ? ((pos - 1) / tabstop) * tabstop : -1
        ; tab_pos >= to_x
        ; tab_pos = ( pos > 0 ) ? ((pos - 1) / tabstop) * tabstop : -1)
    {
      htime_l += repeatedAppend (str, cursor.back_tab, 1);

      if ( htime_l >= LONG_DURATION )
        break;

      pos = tab_pos;
    }

    num = pos - to_x;
  }

  // Use the cursor left capability
  htime_l += repeatedAppend (str, cursor.left, num);

  if ( htime_l < htime )
  {
    hmove = std::move(str);
    htime = htime_l;
  }
}

//----------------------------------------------------------------------
inline void FOptiMove::leftMove ( std::string& hmove, int& htime
                                , int from_x, int to_x ) const
{
  const int num = from_x - to_x;

  if ( num == 0 )
    return;

  if ( parm_cursor.left.cap && parm_cursor.left.duration < htime )
    moveWithParmLeftCursor (hmove, htime, num);

  if ( cursor.left.cap )
    moveWithLeftCursor (hmove, htime, num, from_x, to_x);
}

//----------------------------------------------------------------------
inline auto FOptiMove::isWideMove ( int xold, int yold
                                  , int xnew, int ynew ) const noexcept -> bool
{
  return xnew > MOVE_LIMIT
      && xnew < int(screen.width) - 1 - MOVE_LIMIT
      && std::abs(xnew - xold) + std::abs(ynew - yold) > MOVE_LIMIT;
}

//----------------------------------------------------------------------
inline auto FOptiMove::isMethod0Faster ( int& move_time
                                       , int xnew, int ynew ) -> bool
{
  // Test method 0: direct cursor addressing

  if ( ! parm_cursor.address.cap )
    return false;

  const auto move_xy = \
    FTermcap::encodeMotionParameter(parm_cursor.address.cap, xnew, ynew);

  if ( ! move_xy.empty() )
  {
    move_buf = move_xy;
    move_time = parm_cursor.address.duration;
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
inline auto FOptiMove::isMethod1Faster ( int& move_time
                                       , int xold, int yold
                                       , int xnew, int ynew ) -> bool
{
  // Test method 1: local movement

  if ( xold >= 0 && yold >= 0 )
  {
    const int new_time = relativeMove (temp_result, xold, yold, xnew, ynew);

    if ( new_time < LONG_DURATION && new_time < move_time )
    {
      move_time = new_time;
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------
inline auto FOptiMove::isMethod2Faster ( int& move_time
                                       , int yold
                                       , int xnew, int ynew ) -> bool
{
  // Test method 2: carriage-return + local movement

  if ( yold >= 0 && cursor.carriage_return.cap )
  {
    const int new_time = relativeMove (temp_result, 0, yold, xnew, ynew);

    if ( new_time < LONG_DURATION
      && cursor.carriage_return.duration + new_time < move_time )
    {
      move_time = cursor.carriage_return.duration + new_time;
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------
inline auto FOptiMove::isMethod3Faster ( int& move_time
                                       , int xnew, int ynew ) -> bool
{
  // Test method 3: home-cursor + local movement

  if ( cursor.home.cap )
  {
    const int new_time = relativeMove (temp_result, 0, 0, xnew, ynew);

    if ( new_time < LONG_DURATION
      && cursor.home.duration + new_time < move_time )
    {
      move_time = cursor.home.duration + new_time;
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------
inline auto FOptiMove::isMethod4Faster ( int& move_time
                                       , int xnew, int ynew ) -> bool
{
  // Test method 4: home-down + local movement
  if ( cursor.to_ll.cap )
  {
    const int down = int(screen.height) - 1;
    const int new_time = relativeMove (temp_result, 0, down, xnew, ynew);

    if ( new_time < LONG_DURATION
      && cursor.to_ll.duration + new_time < move_time )
    {
      move_time = cursor.to_ll.duration + new_time;
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------
inline auto FOptiMove::isMethod5Faster ( int& move_time
                                       , int yold
                                       , int xnew, int ynew ) -> bool
{
  // Test method 5: left margin for wrap to right-hand side
  if ( automatic_left_margin
    && ! eat_nl_glitch
    && yold > 0
    && cursor.left.cap )
  {
    const int x = int(screen.width) - 1;
    const int y = yold - 1;
    const int new_time = relativeMove (temp_result, x, y, xnew, ynew);

    if ( new_time < LONG_DURATION
      && cursor.carriage_return.cap
      && cursor.carriage_return.duration
       + cursor.left.duration + new_time < move_time )
    {
      move_time = cursor.carriage_return.duration
                + cursor.left.duration + new_time;
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------
void FOptiMove::moveByMethod ( int method
                             , int xold, int yold
                             , int xnew, int ynew )
{
  switch ( method )
  {
    case 0:  // direct cursor addressing
      return;

    case 1:
      relativeMove (move_buf, xold, yold, xnew, ynew);
      break;

    case 2:
      moveWithCarriageReturn (yold, xnew, ynew);
      break;

    case 3:
      moveWithHome (xnew, ynew);
      break;

    case 4:
      moveWithToLL (xnew, ynew);
      break;

    case 5:
      moveWithCRAndWrapToLeft (xold, yold, xnew, ynew);
      break;

    default:
      throw std::invalid_argument{"Invalid cursor movement method"};
  }
}

//----------------------------------------------------------------------
inline void FOptiMove::moveWithCarriageReturn (int yold, int xnew, int ynew)
{
  if ( ! cursor.carriage_return.cap )
    return;

  move_buf = cursor.carriage_return.cap;
  relativeMove (temp_result, 0, yold, xnew, ynew);
  move_buf.append(temp_result);
}

//----------------------------------------------------------------------
inline void FOptiMove::moveWithHome (int xnew, int ynew)
{
  move_buf = cursor.home.cap;
  relativeMove (temp_result, 0, 0, xnew, ynew);
  move_buf.append(temp_result);
}

//----------------------------------------------------------------------
inline void FOptiMove::moveWithToLL (int xnew, int ynew)
{
  move_buf = cursor.to_ll.cap;
  const int down = int(screen.height) - 1;
  relativeMove (temp_result, 0, down, xnew, ynew);
  move_buf.append(temp_result);
}

//----------------------------------------------------------------------
inline void FOptiMove::moveWithCRAndWrapToLeft ( int xold, int yold
                                               , int xnew, int ynew )
{
  if ( xold >= 0 )
    move_buf = cursor.carriage_return.cap;
  else
    move_buf.clear();

  move_buf.append(cursor.left.cap);
  const int x = int(screen.width) - 1;
  const int y = yold - 1;
  relativeMove (temp_result, x, y, xnew, ynew);
  move_buf.append(temp_result);
}


// FOptiMove non-member function
//----------------------------------------------------------------------
void printDurations (const FOptiMove& om)
{
  std::clog << "            speed: "
            << om.baudrate << " baud" << std::flush;
  std::clog << "    char_duration: "
            << om.char_duration << " ms" << std::flush;
  std::clog << "      cursor_home: "
            << om.cursor.home.duration << " ms" << std::flush;
  std::clog << "     cursor_to_ll: "
            << om.cursor.to_ll.duration << " ms" << std::flush;
  std::clog << "  carriage_return: "
            << om.cursor.carriage_return.duration << " ms" << std::flush;
  std::clog << "              tab: "
            << om.cursor.tab.duration << " ms" << std::flush;
  std::clog << "         back_tab: "
            << om.cursor.back_tab.duration << " ms" << std::flush;
  std::clog << "        cursor_up: "
            << om.cursor.up.duration << " ms" << std::flush;
  std::clog << "      cursor_down: "
            << om.cursor.down.duration << " ms" << std::flush;
  std::clog << "      cursor_left: "
            << om.cursor.left.duration << " ms" << std::flush;
  std::clog << "     cursor_right: "
            << om.cursor.right.duration << " ms" << std::flush;
  std::clog << "   cursor_address: "
            << om.parm_cursor.address.duration << " ms" << std::flush;
  std::clog << "   column_address: "
            << om.parm_cursor.column_address.duration << " ms" << std::flush;
  std::clog << "      row_address: "
            << om.parm_cursor.row_address.duration << " ms" << std::flush;
  std::clog << "   parm_up_cursor: "
            << om.parm_cursor.up.duration << " ms" << std::flush;
  std::clog << " parm_down_cursor: "
            << om.parm_cursor.down.duration << " ms" << std::flush;
  std::clog << " parm_left_cursor: "
            << om.parm_cursor.left.duration << " ms" << std::flush;
  std::clog << "parm_right_cursor: "
            << om.parm_cursor.right.duration << " ms" << std::flush;
}

}  // namespace finalcut
