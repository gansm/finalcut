// File: fapplication.h
// Provides: class FApplication
//
//  Inheritance diagram
//  ═══════════════════
//
// ▕▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▏
// ▕ FObject ▏ ▕  FTerm  ▏
// ▕▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▏
//      ▲           ▲
//      │           │
//      └─────┬─────┘
//            │
//       ▕▔▔▔▔▔▔▔▔▔▏
//       ▕ FWidget ▏
//       ▕▁▁▁▁▁▁▁▁▁▏
//            ▲
//            │
//     ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏ 1      *▕▔▔▔▔▔▔▔▔▏
//     ▕ FApplication ▏-┬- - - -▕ FEvent ▏
//     ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏ :       ▕▁▁▁▁▁▁▁▁▏
//                      :
//                      :      *▕▔▔▔▔▔▔▔▔▏
//                      :- - - -▕ FPoint ▏
//                      :       ▕▁▁▁▁▁▁▁▁▏
//                      :
//                      :      *▕▔▔▔▔▔▔▔▔▔▏
//                      └- - - -▕ FWidget ▏
//                              ▕▁▁▁▁▁▁▁▁▁▏

#ifndef _FAPPLICATION_H
#define _FAPPLICATION_H

#include <linux/keyboard.h> // need for gpm keyboard modifiers
#include <sys/time.h>       // need for gettimeofday
#include <getopt.h>
#include <deque>

#include "fevent.h"
#include "fwidget.h"


//----------------------------------------------------------------------
// class FApplication
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FApplication : public FWidget
{
 public:
   typedef std::pair<FObject*,FEvent*> eventPair;
   static  std::deque<eventPair>* event_queue;

 private:
   int             app_argc;
   char**          app_argv;
   static int      quit_code;
   static bool     quit_now;
   static int      loop_level;
   static bool     process_timer_event;
   static FPoint*  zero_point;
   int             skipped_terminal_update;
   int             key;
   char            k_buf[1024];
   char            x11_mouse[4];
   char            sgr_mouse[13];
   char            urxvt_mouse[13];

#ifdef F_HAVE_LIBGPM
   Gpm_Event gpm_ev;
   bool gpmMouseEvent;
   enum gpmEventType
   {
     no_event       = 0,
     keyboard_event = 1,
     mouse_event    = 2
   };
#endif

   enum btn_state
   {
     Pressed     = 1,
     Released    = 2,
     DoubleClick = 3
   };

   struct button_state // bit field
   {
     uChar left_button    : 2;  // 0..3
     uChar right_button   : 2;  // 0..3
     uChar middle_button  : 2;  // 0..3
     uChar shift_button   : 1;  // 0..1
     uChar control_button : 1;  // 0..1
     uChar meta_button    : 1;  // 0..1
     uChar wheel_up       : 1;  // 0..1
     uChar wheel_down     : 1;  // 0..1
     uChar mouse_moved    : 1;  // 0..1
     uChar                : 4;  // padding bits
   } b_state;

   char            fifo_buf[512];
   int             fifo_offset;
   bool            fifo_in_use;
   int             fifo_buf_size;
   long            key_timeout;
   long            dblclick_interval;
   struct timeval  time_keypressed;
   struct timeval  time_mousepressed;
   FPoint          newMousePosition;
   static FWidget* main_widget;
   static FWidget* active_window;
   static FWidget* focus_widget;
   static FWidget* clicked_widget;
   static FWidget* open_menu;

 private:
   FApplication (const FApplication&);             // Disabled copy constructor
   FApplication& operator = (const FApplication&); // and operator '='

   void    init();
   void    cmd_options();
   bool    KeyPressed();
   ssize_t readKey();
   void    processKeyboardEvent();
   void    getX11ButtonState (int);
   bool    parseX11Mouse();
   bool    parseSGRMouse();
   bool    parseUrxvtMouse();

#ifdef F_HAVE_LIBGPM
   int     gpmEvent (bool = true);
   bool    processGpmEvent();
#endif

   void    processMouseEvent();
   void    processResizeEvent();
   int     processTimerEvent();
   void    processTerminalUpdate();
   void    processCloseWidget();
   bool    processNextEvent();
   friend  class FDialog;
   friend  class FWidget;
   friend  class FWindow;

 public:
   FApplication (int&, char**& );  // constructor
   virtual ~FApplication(); // destructor

   const char* getClassName()     const;
   int         argc()             const;
   char**      argv()             const;
   FWidget*    mainWidget()       const;
   FWidget*    focusWidget()      const;
   bool        unprocessedInput() const;
   static void print_cmd_Options();
   void        setMainWidget (FWidget*);
   int         exec(); // run
   int         enter_loop();
   void        exit_loop();
   static void exit (int = 0);
   void        quit();
   bool        isQuit();
   static bool sendEvent (FObject*, FEvent*);
   static void queueEvent (FObject*, FEvent*);
   static void sendQueuedEvents ();
   static bool eventInQueue();
   static bool removeQueuedEvent(FObject*);
};
#pragma pack(pop)


// FApplication inline functions
//----------------------------------------------------------------------
inline const char* FApplication::getClassName() const
{ return "FApplication"; }

//----------------------------------------------------------------------
inline int FApplication::argc() const
{ return app_argc; }

//----------------------------------------------------------------------
inline char** FApplication::argv() const
{ return app_argv; }

//----------------------------------------------------------------------
inline FWidget* FApplication::mainWidget() const
{ return main_widget; }

//----------------------------------------------------------------------
inline FWidget* FApplication::focusWidget() const
{ return focus_widget; }

//----------------------------------------------------------------------
inline bool FApplication::unprocessedInput() const
{ return input_data_pending; }

#endif  // _FAPPLICATION_H
