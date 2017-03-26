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
//       ▕▔▔▔▔▔▔▔▔▏
//       ▕ FVTerm ▏
//       ▕▁▁▁▁▁▁▁▁▏
//            ▲
//            │
//       ▕▔▔▔▔▔▔▔▔▔▏
//       ▕ FWidget ▏
//       ▕▁▁▁▁▁▁▁▁▁▏
//            ▲
//            │
//     ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏1       *▕▔▔▔▔▔▔▔▔▏
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

#include <getopt.h>
#include <deque>

#include "fevent.h"
#include "fwidget.h"
#include "fwindow.h"


//----------------------------------------------------------------------
// class FApplication
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FApplication : public FWidget
{
 public:
   // Constructor
   FApplication (int&, char*[], bool = false);

   // Destructor
   virtual ~FApplication();

   // Accessors
   const char*        getClassName()     const;
   int                getArgc()          const;
   char**             getArgv()          const;
   FWidget*           getMainWidget()    const;
   FWidget*           getFocusWidget()   const;

   // Mutator
   void               setMainWidget (FWidget*);

   // Inquiry
   bool               isQuit();

   // Methods
   int                exec(); // run
   int                enter_loop();
   void               exit_loop();
   static void        exit (int = 0);
   void               quit();
   static bool        sendEvent (FObject*, FEvent*);
   static void        queueEvent (FObject*, FEvent*);
   static void        sendQueuedEvents ();
   static bool        eventInQueue();
   static bool        removeQueuedEvent(FObject*);
   static void        print_cmd_Options();

 private:
   // Typedefs and Enumerations
   typedef std::pair<FObject*,FEvent*> eventPair;
   typedef std::deque<eventPair> eventQueue;

   enum btn_state
   {
     Pressed     = 1,
     Released    = 2,
     DoubleClick = 3
   };

#ifdef F_HAVE_LIBGPM
   enum gpmEventType
   {
     no_event       = 0,
     keyboard_event = 1,
     mouse_event    = 2
   };
#endif

   // Constants
   static const int NEED_MORE_DATA = -1;  // parseKeyString return value

   // Disable copy constructor
   FApplication (const FApplication&);

   // Disable assignment operator (=)
   FApplication& operator = (const FApplication&);

   // Methods
   void               init();
   void               setExitMessage (std::string);
   void               cmd_options();

#ifdef F_HAVE_LIBGPM
   int                gpmEvent (bool = true);
   bool               processGpmEvent();
#endif

   bool               KeyPressed();
   ssize_t            readKey();
   void               processKeyboardEvent();
#if defined(__linux__)
   int                modifierKeyCorrection (int& key);
#endif
   bool               processDialogSwitchAccelerator();
   bool               processAccelerator (FWidget*&);
   void               getX11ButtonState (int);
   bool               parseX11Mouse();
   bool               parseSGRMouse();
   bool               parseUrxvtMouse();
   void               processMouseEvent();
   void               processResizeEvent();
   int                processTimerEvent();
   void               processCloseWidget();
   bool               processNextEvent();

   // Data Members
   int                app_argc;
   char**             app_argv;
   int                key;

#ifdef F_HAVE_LIBGPM
   Gpm_Event          gpm_ev;
   bool               gpmMouseEvent;
#endif

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

   char               k_buf[1024];
   char               x11_mouse[4];
   char               sgr_mouse[13];
   char               urxvt_mouse[13];
   char               fifo_buf[512];
   int                fifo_offset;
   bool               fifo_in_use;
   int                fifo_buf_size;
   long               key_timeout;
   long               dblclick_interval;
   struct timeval     time_keypressed;
   struct timeval     time_mousepressed;
   FPoint             new_mouse_position;
   static eventQueue* event_queue;
   static int         quit_code;
   static bool        quit_now;
   static int         loop_level;
   static bool        process_timer_event;
   static FPoint*     zero_point;
   static uChar       x11_button_state;
   static FWidget*    move_size_widget;
   static FWidget*    main_widget;
   static FWidget*    active_window;
   static FWidget*    focus_widget;
   static FWidget*    clicked_widget;
   static FWidget*    open_menu;

   // Friend functions from FWidget
   friend  FWidget* FWidget::getMainWidget();
   friend  FWidget* FWidget::getFocusWidget() const;
   friend  void     FWidget::setFocusWidget (FWidget*);
   friend  FWidget* FWidget::getClickedWidget();
   friend  void     FWidget::setClickedWidget (FWidget*);
   friend  FWidget* FWidget::getMoveSizeWidget();
   friend  void     FWidget::setMoveSizeWidget (FWidget*);
   friend  FWidget* FWidget::getOpenMenu();
   friend  void     FWidget::setOpenMenu (FWidget*);

   // Friend functions from FWindow
   friend  bool     FWindow::activateWindow (bool);
   friend  FWindow* FWindow::getActiveWindow();
   friend  void     FWindow::unsetActiveWindow();
};
#pragma pack(pop)


// FApplication inline functions
//----------------------------------------------------------------------
inline const char* FApplication::getClassName() const
{ return "FApplication"; }

//----------------------------------------------------------------------
inline int FApplication::getArgc() const
{ return app_argc; }

//----------------------------------------------------------------------
inline char** FApplication::getArgv() const
{ return app_argv; }

//----------------------------------------------------------------------
inline FWidget* FApplication::getMainWidget() const
{ return main_widget; }

//----------------------------------------------------------------------
inline FWidget* FApplication::getFocusWidget() const
{ return focus_widget; }


#endif  // _FAPPLICATION_H
