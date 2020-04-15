/***********************************************************************
* fapplication.h - Manages the application events                      *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2013-2020 Markus Gans                                      *
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
#include <memory>
#include <string>
#include <utility>

#include "final/ftypes.h"
#include "final/fwidget.h"

namespace finalcut
{

// class forward declaration
class FEvent;
class FAccelEvent;
class FCloseEvent;
class FFocusEvent;
class FKeyEvent;
class FMouseEvent;
class FStartOptions;
class FTimerEvent;
class FWheelEvent;
class FMouseControl;
class FKeyboard;
class FPoint;
class FObject;

//----------------------------------------------------------------------
// class FApplication
//----------------------------------------------------------------------

class FApplication : public FWidget
{
  public:
    // Constructor
    FApplication (const int&, char*[], bool = false);

    // Disable copy constructor
    FApplication (const FApplication&) = delete;

    // Destructor
    ~FApplication() override;

    // Disable copy assignment operator (=)
    FApplication& operator = (const FApplication&) = delete;

    // Accessors
    const FString         getClassName() const override;
    int                   getArgc() const;
    char**                getArgv() const;
    static FApplication*  getApplicationObject();

    // Inquiry
    static bool           isQuit();

    // Methods
    int                   exec();  // run
    int                   enterLoop();
    void                  exitLoop();
    static void           exit (int = 0);
    void                  quit();
    static bool           sendEvent (FObject*, FEvent*);
    void                  queueEvent (FObject*, FEvent*);
    void                  sendQueuedEvents ();
    bool                  eventInQueue();
    bool                  removeQueuedEvent (const FObject*);
    static FWidget*       processParameters (const int&, char*[]);
    static void           showParameterUsage ()
    #if defined(__clang__) || defined(__GNUC__)
      __attribute__((noreturn))
    #endif
                          ;
    static void           closeConfirmationDialog (FWidget*, FCloseEvent*);

    // Callback method
    void cb_exitApp (const FWidget*, const FDataPtr);

  private:
    // Typedefs
    typedef std::pair<FObject*, std::shared_ptr<FEvent> > eventPair;
    typedef std::deque<eventPair> FEventQueue;

    // Methods
    void                  init (uInt64, uInt64);
    static void           cmd_options (const int&, char*[]);
    static FStartOptions& getStartOptions();
    void                  findKeyboardWidget();
    bool                  isKeyPressed();
    void                  keyPressed();
    void                  keyReleased();
    void                  escapeKeyPressed();
    void                  performKeyboardAction();
    void                  sendEscapeKeyPressEvent();
    bool                  sendKeyDownEvent (FWidget*);
    bool                  sendKeyPressEvent (FWidget*);
    bool                  sendKeyUpEvent (FWidget*);
    void                  sendKeyboardAccelerator();
    void                  processKeyboardEvent();
    bool                  processDialogSwitchAccelerator();
    bool                  processAccelerator (const FWidget* const&);
    bool                  getMouseEvent();
    FWidget*&             determineClickedWidget();
    void                  unsetMoveSizeMode();
    void                  closeDropDown();
    void                  unselectMenubarItems();
    void                  sendMouseEvent();
    void                  sendMouseMoveEvent ( const FPoint&
                                             , const FPoint&
                                             , int );
    void                  sendMouseLeftClickEvent ( const FPoint&
                                                  , const FPoint&
                                                  , int );
    void                  sendMouseRightClickEvent ( const FPoint&
                                                   , const FPoint&
                                                   , int );
    void                  sendMouseMiddleClickEvent ( const FPoint&
                                                    , const FPoint&
                                                    , int );
    void                  sendWheelEvent (const FPoint&, const FPoint&);
    void                  processMouseEvent();
    void                  processResizeEvent();
    void                  processCloseWidget();
    bool                  processNextEvent();
    void                  performTimerAction (FObject*, FEvent*) override;
    static bool           isEventProcessable (FObject*, FEvent*);

    // Data members
    int                   app_argc{};
    char**                app_argv{};
    uInt64                key_timeout{100000};        // 100 ms
    uInt64                dblclick_interval{500000};  // 500 ms
    FEventQueue           event_queue{};
    static int            quit_code;
    static bool           quit_now;
    static int            loop_level;
    static bool           process_timer_event;
    static FMouseControl* mouse;
    static FKeyboard*     keyboard;
    static FWidget*       keyboard_widget;
};

// FApplication inline functions
//----------------------------------------------------------------------
inline const FString FApplication::getClassName() const
{ return "FApplication"; }

//----------------------------------------------------------------------
inline int FApplication::getArgc() const
{ return app_argc; }

//----------------------------------------------------------------------
inline char** FApplication::getArgv() const
{ return app_argv; }

//----------------------------------------------------------------------
inline void FApplication::cb_exitApp (const FWidget*, const FDataPtr)
{ close(); }

}  // namespace finalcut

#endif  // FAPPLICATION_H
