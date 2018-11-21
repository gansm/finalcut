/***********************************************************************
* fapplication.h - Manages the application events                      *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2013-2018 Markus Gans                                      *
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

/*  Inheritance diagram
 *  ═══════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▏
 * ▕  FTerm  ▏
 * ▕▁▁▁▁▁▁▁▁▁▏
 *      ▲
 *      │
 * ▕▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▏
 * ▕ FVTerm  ▏ ▕ FObject ▏
 * ▕▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▏
 *      ▲           ▲
 *      │           │
 *      └─────┬─────┘
 *            │
 *       ▕▔▔▔▔▔▔▔▔▔▏
 *       ▕ FWidget ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏
 *            ▲
 *            │
 *     ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏1       *▕▔▔▔▔▔▔▔▔▏
 *     ▕ FApplication ▏-┬- - - -▕ FEvent ▏
 *     ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏ :       ▕▁▁▁▁▁▁▁▁▏
 *                      :
 *                      :      *▕▔▔▔▔▔▔▔▔▏
 *                      :- - - -▕ FPoint ▏
 *                      :       ▕▁▁▁▁▁▁▁▁▏
 *                      :
 *                      :      *▕▔▔▔▔▔▔▔▔▔▏
 *                      └- - - -▕ FWidget ▏
 *                              ▕▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FAPPLICATION_H
#define FAPPLICATION_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <getopt.h>
#include <deque>
#include <string>
#include <utility>

#include "final/fevent.h"
#include "final/fwidget.h"
#include "final/fwindow.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FApplication
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FApplication : public FWidget
{
  public:
    // Constructor
    FApplication (const int&, char*[], bool = false);

    // Destructor
    virtual ~FApplication();

    // Accessors
    const char*        getClassName()   const;
    int                getArgc()        const;
    char**             getArgv()        const;
    static FWidget*    getApplicationObject();

    // Inquiry
    static bool        isQuit();

    // Methods
    int                exec();  // run
    int                enter_loop();
    void               exit_loop();
    static void        exit (int = 0);
    void               quit();
    static bool        sendEvent (const FObject*, const FEvent*);
    static void        queueEvent (const FObject*, const FEvent*);
    static void        sendQueuedEvents ();
    static bool        eventInQueue();
    static bool        removeQueuedEvent (const FObject*);
    FWidget*           processParameters (const int&, char*[]);
    static void        showParameterUsage ()
    #if defined(__clang__) || defined(__GNUC__)
      __attribute__((noreturn))
    #endif
                       ;
    static void        closeConfirmationDialog (FWidget*, FCloseEvent*);

    // Callback method
    void cb_exitApp (FWidget*, data_ptr);

  private:
    // Typedefs and Enumerations
    typedef std::pair<const FObject*, const FEvent*> eventPair;
    typedef std::deque<eventPair> eventQueue;

    // Constants
    static const int NEED_MORE_DATA = -1;  // parseKeyString return value

    // Disable copy constructor
    FApplication (const FApplication&);

    // Disable assignment operator (=)
    FApplication& operator = (const FApplication&);

    // Methods
    void               init (long, long);
    void               cmd_options (const int&, char*[]);
    void               findKeyboardWidget();
    bool               isKeyPressed();
    void               keyPressed();
    void               keyReleased();
    void               escapeKeyPressed();
    void               performKeyboardAction();
    void               sendEscapeKeyPressEvent();
    bool               sendKeyDownEvent (FWidget*);
    bool               sendKeyPressEvent (FWidget*);
    bool               sendKeyUpEvent (FWidget*);
    void               sendKeyboardAccelerator();
    void               processKeyboardEvent();
    bool               processDialogSwitchAccelerator();
    bool               processAccelerator (const FWidget*&);
    bool               getMouseEvent();
    FWidget*&          determineClickedWidget();
    void               unsetMoveSizeMode();
    void               closeOpenMenu();
    void               unselectMenubarItems();
    void               sendMouseEvent();
    void               sendMouseMoveEvent ( const FPoint&
                                          , const FPoint&
                                          , int );
    void               sendMouseLeftClickEvent ( const FPoint&
                                               , const FPoint&
                                               , int );
    void               sendMouseRightClickEvent ( const FPoint&
                                                , const FPoint&
                                                , int );
    void               sendMouseMiddleClickEvent ( const FPoint&
                                                 , const FPoint&
                                                 , int );
    void               sendWheelEvent (const FPoint&, const FPoint&);
    void               processMouseEvent();
    void               processResizeEvent();
    void               processCloseWidget();
    bool               processNextEvent();
    virtual void       performTimerAction ( const FObject*
                                          , const FEvent* );

    // Data Members
    int                app_argc;
    char**             app_argv;
    long               key_timeout;
    long               dblclick_interval;
    static FMouseControl* mouse;
    static eventQueue* event_queue;
    static int         quit_code;
    static bool        quit_now;
    static int         loop_level;
    static bool        process_timer_event;
    static FKeyboard*  keyboard;
    static FWidget*    keyboard_widget;
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
inline void FApplication::cb_exitApp (FWidget*, data_ptr)
{ close(); }

}  // namespace finalcut

#endif  // FAPPLICATION_H
