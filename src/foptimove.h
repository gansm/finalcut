// File: foptimove.h
// Provides: class FOptiMove
//
//  Standalone class
//  ════════════════
//
// ▕▔▔▔▔▔▔▔▔▔▔▔▏
// ▕ FOptiMove ▏
// ▕▁▁▁▁▁▁▁▁▁▁▁▏

// The cursor optimization based on ncurses lib_mvcur.c
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef _FOPTIMOVE_H
#define _FOPTIMOVE_H

#include <assert.h>
#include <term.h>  // need for tparm

#include <cctype>
#include <cstdio>  // need for printf
#include <cstdlib>
#include <cstring>


// value for a long capability waiting time
#define LONG_DURATION 9999999

// maximum character distance to avoid direct cursor addressing
#define MOVE_LIMIT 7

//----------------------------------------------------------------------
// class FOptiMove
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FOptiMove
{
 private:
   typedef unsigned char uChar;
   typedef unsigned int  uInt;

   typedef struct
   {
     char* cap;
     int duration;
     int length;
   } capability;

   capability F_cursor_home;
   capability F_carriage_return;
   capability F_cursor_to_ll;
   capability F_tab;
   capability F_back_tab;
   capability F_cursor_up;
   capability F_cursor_down;
   capability F_cursor_left;
   capability F_cursor_right;
   capability F_cursor_address;
   capability F_column_address;
   capability F_row_address;
   capability F_parm_up_cursor;
   capability F_parm_down_cursor;
   capability F_parm_left_cursor;
   capability F_parm_right_cursor;

   bool automatic_left_margin;
   bool eat_nl_glitch;

   char move_buf[512];
   int  char_duration;
   int  baudrate;
   int  tabstop;
   int  screen_width;
   int  screen_height;

 private:
   void calculateCharDuration();
   int  cap_duration (char*&, int);
   int  repeated_append (capability&, int, char*);
   int  relative_move (char*&, int, int, int, int);
   bool isTwoDirectionMove (int, int, int, int);
   bool isWideMove (int, int, int, int);

 public:
   explicit FOptiMove (int = 0);  // constructor
  ~FOptiMove();                   // destructor

   void setBaudRate (int);
   void setTabStop (int);
   void setTermSize (int, int);
   void set_cursor_home (char*&);
   void set_cursor_to_ll (char*&);
   void set_carriage_return (char*&);
   void set_tabular (char*&);
   void set_back_tab (char*&);
   void set_cursor_up (char*&);
   void set_cursor_down (char*&);
   void set_cursor_left (char*&);
   void set_cursor_right (char*&);
   void set_cursor_address (char*&);
   void set_column_address (char*&);
   void set_row_address (char*&);
   void set_parm_up_cursor (char*&);
   void set_parm_down_cursor (char*&);
   void set_parm_left_cursor (char*&);
   void set_parm_right_cursor (char*&);
   void set_auto_left_margin (bool&);
   void set_eat_newline_glitch (bool&);

   char* cursor_move (int, int, int, int);
   void printDurations();
};
#pragma pack(pop)


// FOptiMove inline functions
//----------------------------------------------------------------------
inline void FOptiMove::set_auto_left_margin (bool& bcap)
{ automatic_left_margin = bcap; }

//----------------------------------------------------------------------
inline void FOptiMove::set_eat_newline_glitch (bool& bcap)
{ eat_nl_glitch = bcap; }

#endif  // _FOPTIMOVE_H
