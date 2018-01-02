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

#include <cstring>

#include "final/foptimove.h"


//----------------------------------------------------------------------
// class FOptiMove
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FOptiMove::FOptiMove (int baud)
  : F_cursor_home()
  , F_carriage_return()
  , F_cursor_to_ll()
  , F_tab()
  , F_back_tab()
  , F_cursor_up()
  , F_cursor_down()
  , F_cursor_left()
  , F_cursor_right()
  , F_cursor_address()
  , F_column_address()
  , F_row_address()
  , F_parm_up_cursor()
  , F_parm_down_cursor()
  , F_parm_left_cursor()
  , F_parm_right_cursor()
  , F_erase_chars()
  , F_repeat_char()
  , F_clr_bol()
  , F_clr_eol()
  , automatic_left_margin(false)
  , eat_nl_glitch(false)
  , char_duration(1)
  , baudrate(baud)
  , tabstop(0)
  , screen_width(80)
  , screen_height(24)
{
  assert ( baud >= 0 );
  move_buf[0] = '\0';
  calculateCharDuration();
}

//----------------------------------------------------------------------
FOptiMove::~FOptiMove()  // destructor
{ }


// public methods of FOptiMove
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
void FOptiMove::setTermSize (int w, int h)
{
  assert ( w > 0 );
  assert ( h > 0 );
  screen_width = w;
  screen_height = h;
}

//----------------------------------------------------------------------
int FOptiMove::set_cursor_home (char cap[])
{
  if ( cap )
  {
    F_cursor_home.cap = cap;
    F_cursor_home.duration = capDuration (cap, 0);
    F_cursor_home.length = capDurationToLength (F_cursor_home.duration);
  }
  else
  {
    F_cursor_home.cap = 0;
    F_cursor_home.duration = \
    F_cursor_home.length   = LONG_DURATION;
  }

  return F_cursor_home.length;
}

//----------------------------------------------------------------------
int FOptiMove::set_cursor_to_ll (char cap[])
{
  if ( cap )
  {
    F_cursor_to_ll.cap = cap;
    F_cursor_to_ll.duration = capDuration (cap, 0);
    F_cursor_to_ll.length = capDurationToLength (F_cursor_to_ll.duration);
  }
  else
  {
    F_cursor_to_ll.cap = 0;
    F_cursor_to_ll.duration = \
    F_cursor_to_ll.length   = LONG_DURATION;
  }

  return F_cursor_to_ll.length;
}

//----------------------------------------------------------------------
int FOptiMove::set_carriage_return (char cap[])
{
  if ( cap )
  {
    F_carriage_return.cap = cap;
    F_carriage_return.duration = capDuration (cap, 0);
    F_carriage_return.length = capDurationToLength (F_carriage_return.duration);
  }
  else
  {
    F_carriage_return.cap = 0;
    F_carriage_return.duration = \
    F_carriage_return.length   = LONG_DURATION;
  }

  return F_carriage_return.length;
}

//----------------------------------------------------------------------
int FOptiMove::set_tabular (char cap[])
{
  if ( cap )
  {
    F_tab.cap = cap;
    F_tab.duration = capDuration (cap, 0);
    F_tab.length = capDurationToLength (F_tab.duration);
  }
  else
  {
    F_tab.cap = 0;
    F_tab.duration = \
    F_tab.length   = LONG_DURATION;
  }

  return F_tab.length;
}

//----------------------------------------------------------------------
int FOptiMove::set_back_tab (char cap[])
{
  if ( cap )
  {
    F_back_tab.cap = cap;
    F_back_tab.duration = capDuration (cap, 0);
    F_back_tab.length = capDurationToLength (F_back_tab.duration);
  }
  else
  {
    F_back_tab.cap = 0;
    F_back_tab.duration = \
    F_back_tab.length   = LONG_DURATION;
  }

  return F_back_tab.length;
}

//----------------------------------------------------------------------
int FOptiMove::set_cursor_up (char cap[])
{
  if ( cap )
  {
    F_cursor_up.cap = cap;
    F_cursor_up.duration = capDuration (cap, 0);
    F_cursor_up.length = capDurationToLength (F_cursor_up.duration);
  }
  else
  {
    F_cursor_up.cap = 0;
    F_cursor_up.duration = \
    F_cursor_up.length   = LONG_DURATION;
  }

  return F_cursor_up.length;
}

//----------------------------------------------------------------------
int FOptiMove::set_cursor_down (char cap[])
{
  if ( cap )
  {
    F_cursor_down.cap = cap;
    F_cursor_down.duration = capDuration (cap, 0);
    F_cursor_down.length = capDurationToLength (F_cursor_down.duration);
  }
  else
  {
    F_cursor_down.cap = 0;
    F_cursor_down.duration = \
    F_cursor_down.length   = LONG_DURATION;
  }

  return F_cursor_down.length;
}

//----------------------------------------------------------------------
int FOptiMove::set_cursor_left (char cap[])
{
  if ( cap )
  {
    F_cursor_left.cap = cap;
    F_cursor_left.duration = capDuration (cap, 0);
    F_cursor_left.length = capDurationToLength (F_cursor_left.duration);
  }
  else
  {
    F_cursor_left.cap = 0;
    F_cursor_left.duration = \
    F_cursor_left.length   = LONG_DURATION;
  }

  return F_cursor_left.length;
}

//----------------------------------------------------------------------
int FOptiMove::set_cursor_right (char cap[])
{
  if ( cap )
  {
    F_cursor_right.cap = cap;
    F_cursor_right.duration = capDuration (cap, 0);
    F_cursor_right.length = capDurationToLength (F_cursor_right.duration);
  }
  else
  {
    F_cursor_right.cap = 0;
    F_cursor_right.duration = \
    F_cursor_right.length   = LONG_DURATION;
  }

  return F_cursor_right.length;
}

//----------------------------------------------------------------------
int FOptiMove::set_cursor_address (char cap[])
{
  if ( cap )
  {
    char* temp = tgoto(cap, 23, 23);
    F_cursor_address.cap = cap;
    F_cursor_address.duration = capDuration (temp, 1);
    F_cursor_address.length = capDurationToLength (F_cursor_address.duration);
  }
  else
  {
    F_cursor_address.cap = 0;
    F_cursor_address.duration = \
    F_cursor_address.length   = LONG_DURATION;
  }

  return F_cursor_address.length;
}

//----------------------------------------------------------------------
int FOptiMove::set_column_address (char cap[])
{
  if ( cap )
  {
    char* temp = tparm(cap, 23, 0, 0, 0, 0, 0, 0, 0, 0);
    F_column_address.cap = cap;
    F_column_address.duration = capDuration (temp, 1);
    F_column_address.length = capDurationToLength (F_column_address.duration);
  }
  else
  {
    F_column_address.cap = 0;
    F_column_address.duration = \
    F_column_address.length   = LONG_DURATION;
  }

  return F_column_address.length;
}

//----------------------------------------------------------------------
int FOptiMove::set_row_address (char cap[])
{
  if ( cap )
  {
    char* temp = tparm(cap, 23, 0, 0, 0, 0, 0, 0, 0, 0);
    F_row_address.cap = cap;
    F_row_address.duration = capDuration (temp, 1);
    F_row_address.length = capDurationToLength (F_row_address.duration);
  }
  else
  {
    F_row_address.cap = 0;
    F_row_address.duration = \
    F_row_address.length   = LONG_DURATION;
  }

  return F_row_address.length;
}

//----------------------------------------------------------------------
int FOptiMove::set_parm_up_cursor (char cap[])
{
  if ( cap )
  {
    char* temp = tparm(cap, 23, 0, 0, 0, 0, 0, 0, 0, 0);
    F_parm_up_cursor.cap = cap;
    F_parm_up_cursor.duration = capDuration (temp, 1);
    F_parm_up_cursor.length = capDurationToLength (F_parm_up_cursor.duration);
  }
  else
  {
    F_parm_up_cursor.cap = 0;
    F_parm_up_cursor.duration = \
    F_parm_up_cursor.length   = LONG_DURATION;
  }

  return F_parm_up_cursor.length;
}

//----------------------------------------------------------------------
int FOptiMove::set_parm_down_cursor (char cap[])
{
  if ( cap )
  {
    char* temp = tparm(cap, 23, 0, 0, 0, 0, 0, 0, 0, 0);
    F_parm_down_cursor.cap = cap;
    F_parm_down_cursor.duration = capDuration (temp, 1);
    F_parm_down_cursor.length = capDurationToLength (F_parm_down_cursor.duration);
  }
  else
  {
    F_parm_down_cursor.cap = 0;
    F_parm_down_cursor.duration = \
    F_parm_down_cursor.length   = LONG_DURATION;
  }

  return F_parm_down_cursor.length;
}

//----------------------------------------------------------------------
int FOptiMove::set_parm_left_cursor (char cap[])
{
  if ( cap )
  {
    char* temp = tparm(cap, 23, 0, 0, 0, 0, 0, 0, 0, 0);
    F_parm_left_cursor.cap = cap;
    F_parm_left_cursor.duration = capDuration (temp, 1);
    F_parm_left_cursor.length = capDurationToLength (F_parm_left_cursor.duration);
  }
  else
  {
    F_parm_left_cursor.cap = 0;
    F_parm_left_cursor.duration = \
    F_parm_left_cursor.length   = LONG_DURATION;
  }

  return F_parm_left_cursor.length;
}

//----------------------------------------------------------------------
int FOptiMove::set_parm_right_cursor (char cap[])
{
  if ( cap )
  {
    char* temp = tparm(cap, 23, 0, 0, 0, 0, 0, 0, 0, 0);
    F_parm_right_cursor.cap = cap;
    F_parm_right_cursor.duration = capDuration (temp, 1);
    F_parm_right_cursor.length = capDurationToLength (F_parm_right_cursor.duration);
  }
  else
  {
    F_parm_right_cursor.cap = 0;
    F_parm_right_cursor.duration = \
    F_parm_right_cursor.length   = LONG_DURATION;
  }

  return F_parm_right_cursor.length;
}

//----------------------------------------------------------------------
int FOptiMove::set_erase_chars (char cap[])
{
  if ( cap )
  {
    char* temp = tparm(cap, 23, 0, 0, 0, 0, 0, 0, 0, 0);
    F_erase_chars.cap = cap;
    F_erase_chars.duration = capDuration (temp, 1);
    F_erase_chars.length = capDurationToLength (F_erase_chars.duration);
  }
  else
  {
    F_erase_chars.cap = 0;
    F_erase_chars.duration = \
    F_erase_chars.length   = LONG_DURATION;
  }

  return F_erase_chars.length;
}

//----------------------------------------------------------------------
int FOptiMove::set_repeat_char (char cap[])
{
  if ( cap )
  {
    char* temp = tparm(cap, ' ', 23, 0, 0, 0, 0, 0, 0, 0);
    F_repeat_char.cap = cap;
    F_repeat_char.duration = capDuration (temp, 1);
    F_repeat_char.length = capDurationToLength (F_repeat_char.duration);
  }
  else
  {
    F_repeat_char.cap = 0;
    F_repeat_char.duration = \
    F_repeat_char.length   = LONG_DURATION;
  }

  return F_repeat_char.length;
}

//----------------------------------------------------------------------
int FOptiMove::set_clr_bol (char cap[])
{
  if ( cap )
  {
    F_clr_bol.cap = cap;
    F_clr_bol.duration = capDuration (cap, 0);
    F_clr_bol.length = capDurationToLength (F_clr_bol.duration);
  }
  else
  {
    F_clr_bol.cap = 0;
    F_clr_bol.duration = \
    F_clr_bol.length   = LONG_DURATION;
  }

  return F_clr_bol.length;
}

//----------------------------------------------------------------------
int FOptiMove::set_clr_eol (char cap[])
{
  if ( cap )
  {
    F_clr_eol.cap = cap;
    F_clr_eol.duration = capDuration (cap, 0);
    F_clr_eol.length = capDurationToLength (F_clr_eol.duration);
  }
  else
  {
    F_clr_eol.cap = 0;
    F_clr_eol.duration = \
    F_clr_eol.length   = LONG_DURATION;
  }

  return F_clr_eol.length;
}

//----------------------------------------------------------------------
char* FOptiMove::moveCursor (int xold, int yold, int xnew, int ynew)
{
  int   method = 0;
  int   move_time = LONG_DURATION;

  // Method 0: direct cursor addressing
  if ( isMethod0Faster(move_time, xnew, ynew) )
  {
    if ( xold < 0
      || yold < 0
      || isWideMove (xold, yold, xnew, ynew) )
    {
      return ( move_time < LONG_DURATION ) ? move_buf : 0;
    }
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
  else
    return 0;
}

//----------------------------------------------------------------------
void FOptiMove::printDurations()
{
  std::cout << "            speed: "
            << baudrate << " baud\r\n";
  std::cout << "    char_duration: "
            << char_duration << " ms\r\n";
  std::cout << "      cursor_home: "
            << F_cursor_home.duration << " ms\r\n";
  std::cout << "     cursor_to_ll: "
            << F_cursor_to_ll.duration << " ms\r\n";
  std::cout << "  carriage_return: "
            << F_carriage_return.duration << " ms\r\n";
  std::cout << "              tab: "
            << F_tab.duration << " ms\r\n";
  std::cout << "         back_tab: "
            << F_back_tab.duration << " ms\r\n";
  std::cout << "        cursor_up: "
            << F_cursor_up.duration << " ms\r\n";
  std::cout << "      cursor_down: "
            << F_cursor_down.duration << " ms\r\n";
  std::cout << "      cursor_left: "
            << F_cursor_left.duration << " ms\r\n";
  std::cout << "     cursor_right: "
            << F_cursor_right.duration << " ms\r\n";
  std::cout << "   cursor_address: "
            << F_cursor_address.duration << " ms\r\n";
  std::cout << "   column_address: "
            << F_column_address.duration << " ms\r\n";
  std::cout << "      row_address: "
            << F_row_address.duration << " ms\r\n";
  std::cout << "   parm_up_cursor: "
            << F_parm_up_cursor.duration << " ms\r\n";
  std::cout << " parm_down_cursor: "
            << F_parm_down_cursor.duration << " ms\r\n";
  std::cout << " parm_left_cursor: "
            << F_parm_left_cursor.duration << " ms\r\n";
  std::cout << "parm_right_cursor: "
            << F_parm_right_cursor.duration << " ms\r\n";
}


// private methods of FApplication
//----------------------------------------------------------------------
void FOptiMove::calculateCharDuration()
{
  if ( baudrate != 0 )
  {
    static const int baudbyte = 9;  // = 7 bit + 1 parity + 1 stop
    char_duration = (baudbyte * 1000 * 10)
                  / (baudrate > 0 ? baudrate : 9600);  // milliseconds

    if ( char_duration <= 0 )
      char_duration = 1;
  }
  else
    char_duration = 1;
}

//----------------------------------------------------------------------
int FOptiMove::capDuration (char cap[], int affcnt)
{
  // calculate the duration in milliseconds of a given operation
  // cap    - the term capability
  // affcnt - the number of lines affected

  if ( ! cap )
    return LONG_DURATION;

  const char* p;
  float ms = 0;

  for (p = cap; *p; p++)
  {
    // check for delay with padding character
    if ( p[0] == '$' && p[1] == '<' && std::strchr(p, '>') )
    {
      float num = 0;

      for (p += 2; *p != '>'; p++)
      {
        if ( std::isdigit(uChar(*p)) )
          num = num * 10 + float(*p - '0');
        else if ( *p == '*' )
          num *= float(affcnt);
        else if ( *p == '.' && *++p != '>' && std::isdigit(uChar(*p)) )
          num += float((*p - '0') / 10.0);
      }

      ms += num * 10;
    }
    else
      ms += float(char_duration);
  }

  return int(ms);
}

//----------------------------------------------------------------------
int FOptiMove::capDurationToLength (int duration)
{
  if ( duration != LONG_DURATION )
    return (duration + char_duration - 1) / char_duration;
  else
    return LONG_DURATION;
}

//----------------------------------------------------------------------
int FOptiMove::repeatedAppend ( const capability& o
                              , volatile int count
                              , char* dst )
{
  register std::size_t src_len;
  register std::size_t dst_len;
  register int total;

  src_len = std::strlen(o.cap);
  dst_len = ( dst != 0 ) ? std::strlen(dst) : 0;
  total = 0;

  if ( (dst_len + uInt(count) * src_len) < sizeof(move_buf) - 1 )
  {
    total += count * o.duration;

    if ( dst )
    {
      dst += dst_len;

      while ( count-- > 0 )
      {
        std::strncpy (dst, o.cap, src_len + 1);
        dst += src_len;
      }
    }
  }
  else
    total = LONG_DURATION;

  return total;
}

//----------------------------------------------------------------------
int FOptiMove::relativeMove ( char move[]
                            , int from_x, int from_y
                            , int to_x, int to_y )
{
  int num
    , vtime = 0
    , htime = 0;

  if ( move )
    move[0] = '\0';

  if ( to_y != from_y )  // vertical move
  {
    vtime = LONG_DURATION;

    if ( F_row_address.cap )
    {
      if ( move )
        std::strncpy ( move
                     , tparm(F_row_address.cap, to_y, 0, 0, 0, 0, 0, 0, 0, 0)
                     , sizeof(move_buf) - 1 );

      vtime = F_row_address.duration;
    }

    if ( to_y > from_y )
    {
      num = to_y - from_y;

      if ( F_parm_down_cursor.cap && F_parm_down_cursor.duration < vtime )
      {
        if ( move )
          std::strncpy ( move
                       , tparm(F_parm_down_cursor.cap, num, 0, 0, 0, 0, 0, 0, 0, 0)
                       , sizeof(move_buf) - 1 );

        vtime = F_parm_down_cursor.duration;
      }

      if ( F_cursor_down.cap && (num * F_cursor_down.duration < vtime) )
      {
        if ( move )
          move[0] = '\0';

        vtime = repeatedAppend (F_cursor_down, num, move);
      }
    }
    else  // to_y < from_y
    {
      num = from_y - to_y;

      if ( F_parm_up_cursor.cap && F_parm_up_cursor.duration < vtime )
      {
        if ( move )
          std::strncpy ( move
                       , tparm(F_parm_up_cursor.cap, num, 0, 0, 0, 0, 0, 0, 0, 0)
                       , sizeof(move_buf) - 1 );

        vtime = F_parm_up_cursor.duration;
      }

      if ( F_cursor_up.cap && (num * F_cursor_up.duration < vtime) )
      {
        if ( move )
          move[0] = '\0';

        vtime = repeatedAppend (F_cursor_up, num, move);
      }
    }

    if ( vtime >= LONG_DURATION )
      return LONG_DURATION;
  }

  if ( to_x != from_x )  // horizontal move
  {
    char str[sizeof(move_buf)] = {};
    char hmove[sizeof(move_buf)] = {};
    htime = LONG_DURATION;

    if ( F_column_address.cap )
    {
      std::strncat ( hmove
                   , tparm(F_column_address.cap, to_x, 0, 0, 0, 0, 0, 0, 0, 0)
                   , sizeof(hmove) - std::strlen(hmove) - 1 );
      htime = F_column_address.duration;
    }

    if ( to_x > from_x )
    {
      num = to_x - from_x;

      if ( F_parm_right_cursor.cap && F_parm_right_cursor.duration < htime )
      {
        std::strncat ( hmove
                     , tparm(F_parm_right_cursor.cap, num, 0, 0, 0, 0, 0, 0, 0, 0)
                     , sizeof(hmove) - std::strlen(hmove) - 1 );
        htime = F_parm_right_cursor.duration;
      }

      if ( F_cursor_right.cap )
      {
        int htime_r = 0;
        str[0] = '\0';

        // try to use tab
        if ( tabstop > 0 && F_tab.cap )
        {
          int pos = from_x;

          while ( true )
          {
            int tab_pos = pos + tabstop - (pos % tabstop);

            if ( tab_pos > to_x )
              break;

            htime_r += repeatedAppend (F_tab, 1, str);

            if ( htime_r >= LONG_DURATION )
              break;

            pos = tab_pos;
          }

          num = to_x - pos;
        }

        htime_r += repeatedAppend (F_cursor_right, num, str);

        if ( htime_r < htime )
        {
          std::strncpy (hmove, str, sizeof(move_buf) - 1);
          htime = htime_r;
        }
      }
    }
    else  // to_x < from_x
    {
      num = from_x - to_x;

      if ( F_parm_left_cursor.cap && F_parm_left_cursor.duration < htime )
      {
        std::strncat ( hmove
                     , tparm(F_parm_left_cursor.cap, num, 0, 0, 0, 0, 0, 0, 0, 0)
                     , sizeof(hmove) - std::strlen(hmove) - 1 );
        htime = F_parm_left_cursor.duration;
      }

      if ( F_cursor_left.cap )
      {
        int htime_l = 0;
        str[0] = '\0';

        // try to use backward tab
        if ( tabstop > 0 && F_back_tab.cap )
        {
          int pos = from_x;

          while ( true )
          {
            int tab_pos = ( pos > 0 ) ? ((pos - 1) / tabstop) * tabstop : -1;

            if ( tab_pos < to_x )
              break;

            htime_l += repeatedAppend (F_back_tab, 1, str);

            if ( htime_l >= LONG_DURATION )
              break;

            pos = tab_pos;
          }

          num = pos - to_x;
        }

        htime_l += repeatedAppend (F_cursor_left, num, str);

        if ( htime_l < htime )
        {
          std::strncpy (hmove, str, sizeof(move_buf) - 1);
          htime = htime_l;
        }
      }
    }

    if ( htime >= LONG_DURATION )
      return LONG_DURATION;

    if ( move )
    {
      if ( *move )
        std::strncat (move, hmove, sizeof(move_buf) - std::strlen(move) - 1);
      else
        std::strncpy (move, hmove, sizeof(move_buf) - 1);
    }
  }

  return vtime + htime;
}

//----------------------------------------------------------------------
inline bool FOptiMove::isWideMove ( int xold, int yold
                                  , int xnew, int ynew )
{
  return bool ( xnew > MOVE_LIMIT
             && xnew < screen_width - 1 - MOVE_LIMIT
             && std::abs(xnew - xold) + std::abs(ynew - yold)
              > MOVE_LIMIT );
}

//----------------------------------------------------------------------
inline bool FOptiMove::isMethod0Faster ( int& move_time
                                       , int xnew, int ynew )
{
  // Test method 0: direct cursor addressing
  char* move_xy = tgoto(F_cursor_address.cap, xnew, ynew);

  if ( move_xy )
  {
    char* move_ptr = move_buf;
    std::strncpy (move_ptr, move_xy, sizeof(move_buf) - 1);
    move_time = F_cursor_address.duration;
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
inline bool FOptiMove::isMethod1Faster ( int& move_time
                                       , int xold, int yold
                                       , int xnew, int ynew )
{
  // Test method 1: local movement

  if ( xold >= 0 && yold >= 0 )
  {
    char  null_result[sizeof(move_buf)];
    char* null_ptr = null_result;
    int   new_time = relativeMove (null_ptr, xold, yold, xnew, ynew);

    if ( new_time < LONG_DURATION && new_time < move_time )
    {
      move_time = new_time;
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------
inline bool FOptiMove::isMethod2Faster ( int& move_time
                                       , int yold
                                       , int xnew, int ynew )
{
  // Test method 2: carriage-return + local movement

  if ( yold >= 0 && F_carriage_return.cap )
  {
    char  null_result[sizeof(move_buf)];
    char* null_ptr = null_result;
    int   new_time = relativeMove (null_ptr, 0, yold, xnew, ynew);

    if ( new_time < LONG_DURATION
      && F_carriage_return.duration + new_time < move_time )
    {
      move_time = F_carriage_return.duration + new_time;
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------
inline bool FOptiMove::isMethod3Faster ( int& move_time
                                       , int xnew, int ynew )
{
  // Test method 3: home-cursor + local movement

  if ( F_cursor_home.cap )
  {
    char  null_result[sizeof(move_buf)];
    char* null_ptr = null_result;
    int   new_time = relativeMove (null_ptr, 0, 0, xnew, ynew);

    if ( new_time < LONG_DURATION
      && F_cursor_home.duration + new_time < move_time )
    {
      move_time = F_cursor_home.duration + new_time;
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------
inline bool FOptiMove::isMethod4Faster ( int& move_time
                                       , int xnew, int ynew )
{
  // Test method 4: home-down + local movement
  if ( F_cursor_to_ll.cap )
  {
    char  null_result[sizeof(move_buf)];
    char* null_ptr = null_result;
    int   new_time = relativeMove ( null_ptr
                                  , 0, screen_height - 1
                                  , xnew, ynew );

    if ( new_time < LONG_DURATION
      && F_cursor_to_ll.duration + new_time < move_time )
    {
      move_time = F_cursor_to_ll.duration + new_time;
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------
inline bool FOptiMove::isMethod5Faster ( int& move_time
                                       , int yold
                                       , int xnew, int ynew )
{
  // Test method 5: left margin for wrap to right-hand side
  if ( automatic_left_margin
    && ! eat_nl_glitch
    && yold > 0
    && F_cursor_left.cap )
  {
    char  null_result[sizeof(move_buf)];
    char* null_ptr = null_result;
    int   new_time = relativeMove ( null_ptr
                                  , screen_width - 1, yold - 1
                                  , xnew, ynew );

    if ( new_time < LONG_DURATION
      && F_carriage_return.cap
      && F_carriage_return.duration
       + F_cursor_left.duration + new_time < move_time )
    {
      move_time = F_carriage_return.duration
                + F_cursor_left.duration + new_time;
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
  char* move_ptr = move_buf;

  switch ( method )
  {
    case 1:
      relativeMove (move_ptr, xold, yold, xnew, ynew);
      break;

    case 2:
      if ( F_carriage_return.cap )
      {
        std::strncpy (move_ptr, F_carriage_return.cap, sizeof(move_buf) - 1);
        move_ptr += F_carriage_return.length;
        relativeMove (move_ptr, 0, yold, xnew, ynew);
      }
      break;

    case 3:
      std::strncpy (move_ptr, F_cursor_home.cap, sizeof(move_buf) - 1);
      move_ptr += F_cursor_home.length;
      relativeMove (move_ptr, 0, 0, xnew, ynew);
      break;

    case 4:
      std::strncpy (move_ptr, F_cursor_to_ll.cap, sizeof(move_buf) - 1);
      move_ptr += F_cursor_to_ll.length;
      relativeMove (move_ptr, 0, screen_height - 1, xnew, ynew);
      break;

    case 5:
      move_buf[0] = '\0';

      if ( xold >= 0 )
        std::strncat ( move_ptr
                     , F_carriage_return.cap
                     , sizeof(move_buf) - std::strlen(move_ptr) - 1 );

      std::strncat ( move_ptr
                   , F_cursor_left.cap
                   , sizeof(move_buf) - std::strlen(move_ptr) - 1 );
      move_ptr += std::strlen(move_buf);
      relativeMove (move_ptr, screen_width - 1, yold - 1, xnew, ynew);
      break;

    default:
      break;
  }
}
