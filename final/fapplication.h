/***********************************************************************
* fapplication.h - Manages the application events                      *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2013-2024 Markus Gans                                      *
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
#include <unordered_map>
#include <utility>
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
    using FMouseHandler = std::function<void(FMouseData)>;

    // Constructor
    FApplication (const int&, char*[]);

    // Disable copy constructor
    FApplication (const FApplication&) = delete;

    // Disable move constructor
    FApplication (FApplication&&) noexcept = delete;

    // Destructor
    ~FApplication() override;

    // Disable copy assignment operator (=)
    auto operator = (const FApplication&) -> FApplication& = delete;

    // Disable move assignment operator (=)
    auto operator = (FApplication&&) noexcept -> FApplication& = delete;

    // Accessors
    auto         getClassName() const -> FString override;
    auto         getArgs() const -> Args;
    static auto  getApplicationObject() -> FApplication*;
    static auto  getKeyboardWidget() -> FWidget*;
    static auto  getLog() -> FLogPtr&;

    // Mutator
    static void  setLog (const FLogPtr&);

    // Inquiry
    static auto  isQuit() -> bool;

    // Methods
#if defined(UNIT_TEST)
    static void  start();  // Simulates application start
#endif
    auto         exec() -> int;  // run
    auto         enterLoop() -> int;
    void         exitLoop() const;
    static void  exit (int = EXIT_SUCCESS);
    void         quit() const;
    static auto  sendEvent (FObject*, FEvent*) -> bool;
    void         queueEvent (FObject*, FEvent*);
    void         sendQueuedEvents();
    auto         eventInQueue() const -> bool;
    auto         removeQueuedEvent (const FObject*) -> bool;
    void         registerMouseHandler (const FMouseHandler&);
    void         initTerminal() override;
    static void  setDefaultTheme();
    static void  setDarkTheme();
    static void  setLogFile (const FString&);
    static void  setKeyboardWidget (FWidget*);
    static void  closeConfirmationDialog (FWidget*, FCloseEvent*);

    // Callback method
    void         cb_exitApp (FWidget*) const;

  protected:
    virtual void processExternalUserEvent();

  private:
    // Using-declaration
    using CmdOption = struct option;
    using EventPair = std::pair<FObject*, FEvent*>;
    using FEventQueue = std::deque<EventPair>;
    using FMouseHandlerList = std::vector<FMouseHandler>;
    using CmdMap = std::unordered_map<int, std::function<void(char*)>>;

    // Methods
    void         init();
    static void  setTerminalEncoding (const FString&);
    static auto  getLongOptions() -> const std::vector<struct option>&;
    static void  setCmdOptionsMap (CmdMap&);
    static void  cmdOptions (const Args&);
    static auto  getStartOptions() -> FStartOptions&;
    static void  showParameterUsage();
    void         destroyLog();
    void         findKeyboardWidget() const;
    auto         isKeyPressed (uInt64 = 0U) const -> bool;
    void         keyPressed();
    void         keyReleased() const;
    void         escapeKeyPressed() const;
    void         mouseTracking() const;
    void         performKeyboardAction();
    void         performMouseAction() const;
    void         mouseEvent (const FMouseData&) const;
    void         sendEscapeKeyPressEvent() const;
    auto         sendKeyDownEvent (FWidget*) const -> bool;
    auto         sendKeyPressEvent (FWidget*) const -> bool;
    auto         sendKeyUpEvent (FWidget*) const -> bool;
    void         sendKeyboardAccelerator() const;
    auto         hasDataInQueue() const -> bool;
    void         queuingKeyboardInput() const;
    void         queuingMouseInput() const;
    void         processKeyboardEvent() const;
    void         processMouseEvent() const;
    void         processInput() const;
    auto         processDialogSwitchAccelerator() const -> bool;
    auto         processAccelerator (const FWidget&) const -> bool;
    void         processTerminalFocus (const FKey&) const;
    static void  determineClickedWidget (const FMouseData&);
    static void  determineWheelWidget (const FMouseData&);
    static auto  isNonActivatingMouseEvent (const FMouseData&) -> bool;
    static auto  isWheelEvent (const FMouseData&) -> bool;
    static void  unsetMoveResizeMode (const FMouseData&);
    void         sendMouseEvent (const FMouseData&) const;
    void         sendMouseWheelEvent (const FMouseData&) const;
    void         sendMouseMoveEvent ( const FMouseData&
                                    , const FPoint&
                                    , const FPoint&
                                    , MouseButton ) const;
    void         sendMouseLeftClickEvent ( const FMouseData&
                                         , const FPoint&
                                         , const FPoint&
                                         , MouseButton ) const;
    void         sendMouseRightClickEvent ( const FMouseData&
                                          , const FPoint&
                                          , const FPoint&
                                          , MouseButton ) const;
    void         sendMouseMiddleClickEvent ( const FMouseData&
                                           , const FPoint&
                                           , const FPoint&
                                           , MouseButton ) const;
    void         sendWheelEvent ( const FMouseData&
                                , const FPoint&
                                , const FPoint& ) const;
    static auto  processParameters (const Args&) -> FWidget*;
    void         processResizeEvent();
    void         processCloseWidget();
    void         processDialogResizeMove() const;
    void         processLogger() const;
    auto         processNextEvent() -> bool;
    void         performTimerAction (FObject*, FEvent*) override;
    auto         hasTerminalResized() -> bool;
    static auto  isEventProcessable (FObject*, const FEvent*) -> bool;
    static auto  isNextEventTimeout() -> bool;

    // Data members
    Args              app_args{};
    uInt64            key_timeout{100'000};        // 100 ms
    uInt64            dblclick_interval{500'000};  // 500 ms
    std::streambuf*   default_clog_rdbuf{std::clog.rdbuf()};
    FEventQueue       event_queue{};
    FMouseHandlerList mouse_handler_list{};
    bool              has_terminal_resized{false};
    static uInt64     next_event_wait;
    static TimeValue  time_last_event;
    static int        loop_level;
    static int        quit_code;
    static bool       quit_now;
    static FWidget*   clicked_widget;
    static FWidget*   wheel_widget;
    static FWidget*   keyboard_widget;
};

// non-member function forward declarations
// implemented in fwidget_functions.cpp
//----------------------------------------------------------------------
auto getFApplication() -> FApplication*;


// FApplication inline functions
//----------------------------------------------------------------------
inline auto FApplication::getClassName() const -> FString
{ return "FApplication"; }

//----------------------------------------------------------------------
inline auto FApplication::getArgs() const -> Args
{ return app_args; }

//----------------------------------------------------------------------
inline void FApplication::cb_exitApp (FWidget* w) const
{ w->close(); }

}  // namespace finalcut

#endif  // FAPPLICATION_H
