// foptimove.cpp
// class FOptiMove

#include "foptimove.h"

//----------------------------------------------------------------------
// class FOptiMove
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FOptiMove::FOptiMove (int baud)
{
  assert ( baud >= 0 );

  baudrate = baud;
  tabstop = 0;
  move_buf[0] = '\0';

  // init structs with 0
  memset(&F_cursor_home,       0x00, sizeof(capability));
  memset(&F_carriage_return,   0x00, sizeof(capability));
  memset(&F_cursor_to_ll,      0x00, sizeof(capability));
  memset(&F_tab,               0x00, sizeof(capability));
  memset(&F_back_tab,          0x00, sizeof(capability));
  memset(&F_cursor_up,         0x00, sizeof(capability));
  memset(&F_cursor_down,       0x00, sizeof(capability));
  memset(&F_cursor_left,       0x00, sizeof(capability));
  memset(&F_cursor_right,      0x00, sizeof(capability));
  memset(&F_cursor_address,    0x00, sizeof(capability));
  memset(&F_column_address,    0x00, sizeof(capability));
  memset(&F_row_address,       0x00, sizeof(capability));
  memset(&F_parm_up_cursor,    0x00, sizeof(capability));
  memset(&F_parm_down_cursor,  0x00, sizeof(capability));
  memset(&F_parm_left_cursor,  0x00, sizeof(capability));
  memset(&F_parm_right_cursor, 0x00, sizeof(capability));
  automatic_left_margin = false;
  eat_nl_glitch = false;

  calculateCharDuration();
}

//----------------------------------------------------------------------
FOptiMove::~FOptiMove() // destructor
{
}


// private methods of FApplication
//----------------------------------------------------------------------
void FOptiMove::calculateCharDuration()
{
  if ( baudrate != 0 )
  {
    const int baudbyte = 9; // = 7 bit + 1 parity + 1 stop
    char_duration = (baudbyte * 1000 * 10)
                    / (baudrate > 0 ? baudrate : 9600); // milliseconds
    if ( char_duration <= 0 )
      char_duration = 1;
  }
  else
    char_duration = 1;
}

//----------------------------------------------------------------------
int FOptiMove::cap_duration (char*& cap, int affcnt)
{
  // calculate the duration in milliseconds of a given operation
  // cap    - the term capability
  // affcnt - the number of lines affected

  if ( cap )
  {
    const char* p;
    float ms = 0;

    for (p=cap; *p; p++)
    {
      // check for delay with padding character
      if ( p[0] == '$' && p[1] == '<' && strchr(p, '>') )
      {
        float num=0;

        for (p += 2; *p != '>'; p++)
        {
          if ( isdigit(uChar(*p)) )
            num = num * 10 + float(*p - '0');
          else if ( *p == '*' )
            num *= float(affcnt);
          else if ( *p == '.' && *++p != '>' && isdigit(uChar(*p)) )
            num += float((*p - '0') / 10.0);
        }
        ms += num * 10;
      }
      else
        ms += char_duration;
    }
    return int(ms);
  }
  else
    return LONG_DURATION;
}


// public methods of FOptiMove
//----------------------------------------------------------------------
void FOptiMove::set_cursor_home (char*& cap)
{
  F_cursor_home.cap = cap;
  F_cursor_home.duration = cap_duration(cap, 0);
  F_cursor_home.length = ( cap != 0 ) ? int(strlen(cap)) : 0;
}

//----------------------------------------------------------------------
void FOptiMove::set_cursor_to_ll (char*& cap)
{
  F_cursor_to_ll.cap = cap;
  F_cursor_to_ll.duration = cap_duration(cap, 0);
  F_cursor_to_ll.length = ( cap != 0 ) ? int(strlen(cap)) : 0;
}

//----------------------------------------------------------------------
void FOptiMove::set_carriage_return (char*& cap)
{
  F_carriage_return.cap = cap;
  F_carriage_return.duration = cap_duration(cap, 0);
  F_carriage_return.length = ( cap != 0 ) ? int(strlen(cap)) : 0;
}

//----------------------------------------------------------------------
void FOptiMove::set_tabular (char*& cap)
{
  F_tab.cap = cap;
  F_tab.duration = cap_duration(cap, 0);
  F_tab.length = ( cap != 0 ) ? int(strlen(cap)) : 0;
}

//----------------------------------------------------------------------
void FOptiMove::set_back_tab (char*& cap)
{
  F_back_tab.cap = cap;
  F_back_tab.duration = cap_duration(cap, 0);
  F_back_tab.length = ( cap != 0 ) ? int(strlen(cap)) : 0;
}

//----------------------------------------------------------------------
void FOptiMove::set_cursor_up (char*& cap)
{
  F_cursor_up.cap = cap;
  F_cursor_up.duration = cap_duration(cap, 0);
  F_cursor_up.length = ( cap != 0 ) ? int(strlen(cap)) : 0;
}

//----------------------------------------------------------------------
void FOptiMove::set_cursor_down (char*& cap)
{
  F_cursor_down.cap = cap;
  F_cursor_down.duration = cap_duration(cap, 0);
  F_cursor_down.length = ( cap != 0 ) ? int(strlen(cap)) : 0;
}

//----------------------------------------------------------------------
void FOptiMove::set_cursor_left (char*& cap)
{
  F_cursor_left.cap = cap;
  F_cursor_left.duration = cap_duration(cap, 0);
  F_cursor_left.length = ( cap != 0 ) ? int(strlen(cap)) : 0;
}

//----------------------------------------------------------------------
void FOptiMove::set_cursor_right (char*& cap)
{
  F_cursor_right.cap = cap;
  F_cursor_right.duration = cap_duration(cap, 0);
  F_cursor_right.length = ( cap != 0 ) ? int(strlen(cap)) : 0;
}

//----------------------------------------------------------------------
void FOptiMove::set_cursor_address (char*& cap)
{
  char* temp = tgoto(cap, 23, 23);
  F_cursor_address.cap = cap;
  F_cursor_address.duration = cap_duration(temp, 1);
  F_cursor_address.length = ( cap != 0 ) ? int(strlen(cap)) : 0;
}

//----------------------------------------------------------------------
void FOptiMove::set_column_address (char*& cap)
{
  char* temp = tparm(cap, 23);
  F_column_address.cap = cap;
  F_column_address.duration = cap_duration(temp, 1);
  F_column_address.length = ( cap != 0 ) ? int(strlen(cap)) : 0;
}

//----------------------------------------------------------------------
void FOptiMove::set_row_address (char*& cap)
{
  char* temp = tparm(cap, 23);
  F_row_address.cap = cap;
  F_row_address.duration = cap_duration(temp, 1);
  F_row_address.length = ( cap != 0 ) ? int(strlen(cap)) : 0;
}

//----------------------------------------------------------------------
void FOptiMove::set_parm_up_cursor (char*& cap)
{
  char* temp = tparm(cap, 23);
  F_parm_up_cursor.cap = cap;
  F_parm_up_cursor.duration = cap_duration(temp, 1);
  F_parm_up_cursor.length = ( cap != 0 ) ? int(strlen(cap)) : 0;
}

//----------------------------------------------------------------------
void FOptiMove::set_parm_down_cursor (char*& cap)
{
  char* temp = tparm(cap, 23);
  F_parm_down_cursor.cap = cap;
  F_parm_down_cursor.duration = cap_duration(temp, 1);
  F_parm_down_cursor.length = ( cap != 0 ) ? int(strlen(cap)) : 0;
}

//----------------------------------------------------------------------
void FOptiMove::set_parm_left_cursor (char*& cap)
{
  char* temp = tparm(cap, 23);
  F_parm_left_cursor.cap = cap;
  F_parm_left_cursor.duration = cap_duration(temp, 1);
  F_parm_left_cursor.length = ( cap != 0 ) ? int(strlen(cap)) : 0;
}

//----------------------------------------------------------------------
void FOptiMove::set_parm_right_cursor (char*& cap)
{
  char* temp = tparm(cap, 23);
  F_parm_right_cursor.cap = cap;
  F_parm_right_cursor.duration = cap_duration(temp, 1);
  F_parm_right_cursor.length = ( cap != 0 ) ? int(strlen(cap)) : 0;
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
void FOptiMove::setTermSize (int w, int h)
{
  assert ( w > 0 );
  assert ( h > 0 );
  screen_width = w;
  screen_height = h;
}

//----------------------------------------------------------------------
int FOptiMove::repeated_append (capability& o, int count, char* dst)
{
  register size_t src_len;
  register size_t dst_len;
  register int total;

  src_len = strlen(o.cap);
  dst_len = ( dst != 0 ) ? strlen(dst) : 0;
  total = 0;

  if ( (dst_len + uInt(count) * src_len) < sizeof(move_buf)-1 )
  {
    total += count * o.duration;
    if ( dst )
    {
      dst += dst_len;
      while ( count-- > 0 )
      {
        strcpy (dst, o.cap);
        dst += src_len;
      }
    }
  }
  else
    total = LONG_DURATION;

  return total;
}

//----------------------------------------------------------------------
int FOptiMove::relative_move ( char*& move,
                               int from_x, int from_y,
                               int to_x, int to_y )
{
  int num;
  int vtime = 0;
  int htime = 0;

  if ( move )
    move[0] = '\0';

  if ( to_y != from_y )  // vertical move
  {
    vtime = LONG_DURATION;

    if ( F_row_address.cap )
    {
      if ( move )
        strcpy (move, tparm(F_row_address.cap, to_y));
      vtime = F_row_address.duration;
    }

    if ( to_y > from_y )
    {
      num = to_y - from_y;

      if ( F_parm_down_cursor.cap && F_parm_down_cursor.duration < vtime )
      {
        if ( move )
          strcpy (move, tparm(F_parm_down_cursor.cap, num));
        vtime = F_parm_down_cursor.duration;
      }

      if ( F_cursor_down.cap && (num * F_cursor_down.duration < vtime) )
      {
        if ( move )
          move[0] = '\0';
        vtime = repeated_append (F_cursor_down, num, move);
      }
    }
    else // to_y < from_y
    {
      num = from_y - to_y;

      if ( F_parm_up_cursor.cap && F_parm_up_cursor.duration < vtime )
      {
        if ( move )
          strcpy (move, tparm(F_parm_up_cursor.cap, num));
        vtime = F_parm_up_cursor.duration;
      }

      if ( F_cursor_up.cap && (num * F_cursor_up.duration < vtime) )
      {
        if ( move )
          move[0] = '\0';
        vtime = repeated_append (F_cursor_up, num, move);
      }
    }

    if ( vtime >= LONG_DURATION )
      return LONG_DURATION;
  }

  if ( to_x != from_x )  // horizontal move
  {
    char str[sizeof(move_buf)];
    char hmove[sizeof(move_buf)];
    hmove[0] = '\0';
    htime = LONG_DURATION;

    if ( F_column_address.cap )
    {
      strcat (hmove, tparm(F_column_address.cap, to_x));
      htime = F_column_address.duration;
    }

    if ( to_x > from_x )
    {
      num = to_x - from_x;

      if ( F_parm_right_cursor.cap && F_parm_right_cursor.duration < htime )
      {
        strcat (hmove, tparm(F_parm_right_cursor.cap, num));
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
            htime_r += repeated_append (F_tab, 1, str);
            if ( htime_r >= LONG_DURATION )
              break;
            pos = tab_pos;
          }
          num = to_x - pos;
          from_x = pos;
        }

        htime_r += repeated_append (F_cursor_right, num, str);

        if ( htime_r < htime )
        {
          strcpy (hmove, str);
          htime = htime_r;
        }
      }
    }
    else // to_x < from_x
    {
      num = from_x - to_x;

      if ( F_parm_left_cursor.cap && F_parm_left_cursor.duration < htime )
      {
        strcat (hmove, tparm(F_parm_left_cursor.cap, num));
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
            int tab_pos = ( pos > 0 ) ? ((pos-1)/tabstop)*tabstop : -1;

            if ( tab_pos < to_x )
              break;
            htime_l += repeated_append (F_back_tab, 1, str);
            if ( htime_l >= LONG_DURATION )
              break;
            pos = tab_pos;
          }
          num = pos - to_x;
        }

        htime_l += repeated_append (F_cursor_left, num, str);

        if ( htime_l < htime )
        {
          strcpy (hmove, str);
          htime = htime_l;
        }
      }
    }

    if ( htime >= LONG_DURATION )
      return LONG_DURATION;

    strcat (move, hmove);
  }

  return (vtime + htime);
}

//----------------------------------------------------------------------
inline bool FOptiMove::isTwoDirectionMove ( int xold, int yold,
                                            int xnew, int ynew )
{
  return bool (  (xold != xnew || ! F_row_address.cap)
              && (yold != ynew || ! F_column_address.cap) );
}

//----------------------------------------------------------------------
inline bool FOptiMove::isWideMove ( int xold, int yold,
                                    int xnew, int ynew )
{
  return bool (  (xnew > MOVE_LIMIT)
              && (xnew < screen_width - 1 - MOVE_LIMIT)
              && (abs(xnew-xold) + abs(ynew-yold) > MOVE_LIMIT) );
}

//----------------------------------------------------------------------
char* FOptiMove::cursor_move (int xold, int yold, int xnew, int ynew)
{
  char  null_result[sizeof(move_buf)];
  char* null_ptr = null_result;
  char* move_ptr = move_buf;
  char* move_xy;
  int   method = 0;
  int   new_time;
  int   move_time = LONG_DURATION;

  // Method 0: direct cursor addressing
  move_xy = tgoto(F_cursor_address.cap, xnew, ynew);
  if ( move_xy )
  {
    method = 0;
    strcpy (move_ptr, move_xy);
    move_time = F_cursor_address.duration;

    if (  xold < 0
       || yold < 0
       || isTwoDirectionMove (xold, yold, xnew, ynew)
       || isWideMove (xold, yold, xnew, ynew) )
    {
      return ( move_time < LONG_DURATION ) ? move_buf : 0;
    }
  }

  // Method 1: local movement
  if ( xold >= 0 && yold >= 0 )
  {
    new_time = relative_move (null_ptr, xold, yold, xnew, ynew);
    if ( new_time < LONG_DURATION && new_time < move_time )
    {
      method = 1;
      move_time = new_time;
    }
  }

  // Method 2: carriage-return + local movement
  if ( yold >= 0 && F_carriage_return.cap )
  {
    new_time = relative_move (null_ptr, 0, yold, xnew, ynew);
    if (  new_time < LONG_DURATION
       && F_carriage_return.duration + new_time < move_time )
    {
      method = 2;
      move_time = F_carriage_return.duration + new_time;
    }
  }

  // Method 3: home-cursor + local movement
  if ( F_cursor_home.cap )
  {
    new_time = relative_move (null_ptr, 0, 0, xnew, ynew);
    if (  new_time < LONG_DURATION
       && F_cursor_home.duration + new_time < move_time )
    {
      method = 3;
      move_time = F_cursor_home.duration + new_time;
    }
  }

  // Method 4: home-down + local movement
  if ( F_cursor_to_ll.cap )
  {
    new_time = relative_move (null_ptr, 0, screen_height-1, xnew, ynew);
    if (  new_time < LONG_DURATION
       && F_cursor_to_ll.duration + new_time < move_time )
    {
      method = 4;
      move_time = F_cursor_to_ll.duration + new_time;
    }
  }

  // Method 5: left margin for wrap to right-hand side
  if (  automatic_left_margin
     && ! eat_nl_glitch
     && yold > 0
     && F_cursor_left.cap )
  {
    new_time = relative_move (null_ptr, screen_width-1, yold-1, xnew, ynew);
    if (  new_time < LONG_DURATION
       && F_carriage_return.duration
        + F_cursor_left.duration + new_time < move_time )
    {
      method = 5;
      move_time = F_carriage_return.duration
                + F_cursor_left.duration + new_time;
    }
  }

  if ( method )
  {
    switch ( method )
    {
      case 1:
        relative_move (move_ptr, xold, yold, xnew, ynew);
        break;

      case 2:
        strcpy (move_ptr, F_carriage_return.cap);
        move_ptr += F_carriage_return.length;
        relative_move (move_ptr, 0, yold, xnew, ynew);
        break;

      case 3:
        strcpy (move_ptr, F_cursor_home.cap);
        move_ptr += F_cursor_home.length;
        relative_move (move_ptr, 0, 0, xnew, ynew);
        break;

      case 4:
        strcpy (move_ptr, F_cursor_to_ll.cap);
        move_ptr += F_cursor_to_ll.length;
        relative_move (move_ptr, 0, screen_height-1, xnew, ynew);
        break;

      case 5:
        move_buf[0] = '\0';
        if ( xold >= 0 )
          strcat (move_ptr, F_carriage_return.cap);
        strcat (move_ptr, F_cursor_left.cap);
        move_ptr += strlen(move_buf);
        relative_move (move_ptr, screen_width-1, yold-1, xnew, ynew);
        break;
    }
  }
  if ( move_time < LONG_DURATION )
    return move_buf;
  else
    return 0;
}

//----------------------------------------------------------------------
void FOptiMove::printDurations()
{
  ::printf ("            speed: %d baud\n", baudrate);
  ::printf ("    char_duration: %d ms\n", char_duration);
  ::printf ("      cursor_home: %d ms\n", F_cursor_home.duration);
  ::printf ("     cursor_to_ll: %d ms\n", F_cursor_to_ll.duration);
  ::printf ("  carriage_return: %d ms\n", F_carriage_return.duration);
  ::printf ("              tab: %d ms\n", F_tab.duration);
  ::printf ("         back_tab: %d ms\n", F_back_tab.duration);
  ::printf ("        cursor_up: %d ms\n", F_cursor_up.duration);
  ::printf ("      cursor_down: %d ms\n", F_cursor_down.duration);
  ::printf ("      cursor_left: %d ms\n", F_cursor_left.duration);
  ::printf ("     cursor_right: %d ms\n", F_cursor_right.duration);
  ::printf ("   cursor_address: %d ms\n", F_cursor_address.duration);
  ::printf ("   column_address: %d ms\n", F_column_address.duration);
  ::printf ("      row_address: %d ms\n", F_row_address.duration);
  ::printf ("   parm_up_cursor: %d ms\n", F_parm_up_cursor.duration);
  ::printf (" parm_down_cursor: %d ms\n", F_parm_down_cursor.duration);
  ::printf (" parm_left_cursor: %d ms\n", F_parm_left_cursor.duration);
  ::printf ("parm_right_cursor: %d ms\n", F_parm_right_cursor.duration);
}
