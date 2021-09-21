/***********************************************************************
* fapplication.h - Manages the application events                      *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2013-2021 Markus Gans                                      *
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
 *     ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏1       1▕▔▔▔▔▔▔▏
 *     ▕ FApplication ▏-┬- - - -▕ FLog ▏
 *     ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏ :       ▕▁▁▁▁▁▁▏
 *                      :
 *                      :      *▕▔▔▔▔▔▔▔▔▏
 *                      :- - - -▕ FEvent ▏
 *                      :       ▕▁▁▁▁▁▁▁▁▏
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
#include <unordered_map>
#include <vector>

#include "final/ftypes.h"
#include "final/fwidget.h"

namespace finalcut
{

// class forward declaration
class FAccelEvent;
class FCloseEvent;
class FEvent;
class FFocusEvent;
class FKeyEvent;
class FLog;
class FMouseData;
class FMouseEvent;
class FStartOptions;
class FTimerEvent;
class FWheelEvent;
class FMouseControl;
class FPoint;
class FObject;

//----------------------------------------------------------------------
// class FApplication
//----------------------------------------------------------------------

class FApplication : public FWidget
{
  public:
    // Typedef
    using FLogPtr = std::shared_ptr<FLog>;
    using Args = std::vector<std::string>;

    // Constructor
    FApplication (const int&, char*[]);

    // Disable copy constructor
    FApplication (const FApplication&) = delete;

    // Destructor
    ~FApplication() override;

    // Disable copy assignment operator (=)
    FApplication& operator = (const FApplication&) = delete;

    // Accessors
    FString               getClassName() const override;
    Args                  getArgs() const;
    static FApplication*  getApplicationObject();
    static FWidget*       getKeyboardWidget();
    static FLogPtr&       getLog();

    // Mutator
    static void           setLog (const FLogPtr&);

    // Inquiry
    static bool           isQuit();

    // Methods
    int                   exec();  // run
    int                   enterLoop();
    void                  exitLoop() const;
    static void           exit (int = EXIT_SUCCESS);
    void                  quit() const;
    static bool           sendEvent (FObject*, FEvent*);
    void                  queueEvent (FObject*, FEvent*);
    void                  sendQueuedEvents();
    bool                  eventInQueue() const;
    bool                  removeQueuedEvent (const FObject*);
    void                  initTerminal() override;
    static void           setDefaultTheme();
    static void           setDarkTheme();
    static void           setLogFile (const FString&);
    static void           setKeyboardWidget (FWidget*);
    static void           closeConfirmationDialog (FWidget*, FCloseEvent*);

    // Callback method
    void                  cb_exitApp (FWidget*) const;

  protected:
    virtual void          processExternalUserEvent();

  private:
#if defined(__sun) && defined(__SVR4)
    struct CmdOption
    {
      const char* name;  // <- name is without 'const' in Solaris
      int         has_arg;
      int*        flag;
      int         val;
    };
#else
    using CmdOption = struct option;
#endif

    // Using-declaration
    using EventPair = std::pair<FObject*, FEvent*>;
    using FEventQueue = std::deque<EventPair>;
    using CmdMap = std::unordered_map<int, std::function<void(char*)>>;

    // Methods
    void                  init();
    static void           setTerminalEncoding (const FString&);
    static void           setLongOptions(std::vector<CmdOption>&);
    static void           setCmdOptionsMap (CmdMap&);
    static void           cmdOptions (const Args&);
    static FStartOptions& getStartOptions();
    static void           showParameterUsage();
    void                  destroyLog();
    void                  findKeyboardWidget() const;
    bool                  isKeyPressed() const;
    void                  keyPressed();
    void                  keyReleased() const;
    void                  escapeKeyPressed() const;
    void                  mouseTracking() const;
    void                  performKeyboardAction();
    void                  performMouseAction() const;
    void                  mouseEvent (const FMouseData&);
    void                  sendEscapeKeyPressEvent() const;
    bool                  sendKeyDownEvent (FWidget*) const;
    bool                  sendKeyPressEvent (FWidget*) const;
    bool                  sendKeyUpEvent (FWidget*) const;
    void                  sendKeyboardAccelerator();
    bool                  hasDataInQueue() const;
    void                  queuingKeyboardInput() const;
    void                  queuingMouseInput() const;
    void                  processKeyboardEvent() const;
    void                  processMouseEvent() const;
    bool                  processDialogSwitchAccelerator() const;
    bool                  processAccelerator (const FWidget&) const;
    void                  determineClickedWidget (const FMouseData&);
    void                  unsetMoveSizeMode() const;
    void                  closeDropDown (const FMouseData&) const;
    void                  unselectMenubarItems (const FMouseData&) const;
    void                  sendMouseEvent (const FMouseData&) const;
    void                  sendMouseMoveEvent ( const FMouseData&
                                             , const FPoint&
                                             , const FPoint&
                                             , MouseButton ) const;
    void                  sendMouseLeftClickEvent ( const FMouseData&
                                                  , const FPoint&
                                                  , const FPoint&
                                                  , MouseButton ) const;
    void                  sendMouseRightClickEvent ( const FMouseData&
                                                   , const FPoint&
                                                   , const FPoint&
                                                   , MouseButton ) const;
    void                  sendMouseMiddleClickEvent ( const FMouseData&
                                                    , const FPoint&
                                                    , const FPoint&
                                                    , MouseButton ) const;
    void                  sendWheelEvent ( const FMouseData&
                                         , const FPoint&
                                         , const FPoint& ) const;
    static FWidget*       processParameters (const Args&);
    void                  processResizeEvent() const;
    void                  processCloseWidget();
    void                  processLogger() const;
    bool                  processNextEvent();
    void                  performTimerAction (FObject*, FEvent*) override;
    static bool           isEventProcessable (FObject*, const FEvent*);
    static bool           isNextEventTimeout();

    // Data members
    Args                  app_args{};
    uInt64                key_timeout{100000};        // 100 ms
    uInt64                dblclick_interval{500000};  // 500 ms
    std::streambuf*       default_clog_rdbuf{std::clog.rdbuf()};
    FWidget*              clicked_widget{};
    FEventQueue           event_queue{};
    static uInt64         next_event_wait;
    static TimeValue      time_last_event;
    static int            loop_level;
    static int            quit_code;
    static bool           quit_now;
    static FWidget*       keyboard_widget;
};

// non-member function forward declarations
// implemented in fwidget_functions.cpp
//----------------------------------------------------------------------
FApplication* getFApplication();


// FApplication inline functions
//----------------------------------------------------------------------
inline FString FApplication::getClassName() const
{ return "FApplication"; }

//----------------------------------------------------------------------
inline FApplication::Args FApplication::getArgs() const
{ return app_args; }

//----------------------------------------------------------------------
inline void FApplication::cb_exitApp (FWidget* w) const
{ w->close(); }

}  // namespace finalcut

#endif  // FAPPLICATION_H
