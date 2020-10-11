/***********************************************************************
* fapplication.cpp - Manages the application events                    *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2013-2020 Markus Gans                                      *
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

#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <thread>

#include "final/fapplication.h"
#include "final/fevent.h"
#include "final/flog.h"
#include "final/flogger.h"
#include "final/fmenu.h"
#include "final/fmenubar.h"
#include "final/fmessagebox.h"
#include "final/fmouse.h"
#include "final/fstartoptions.h"
#include "final/fstatusbar.h"
#include "final/ftermdata.h"
#include "final/ftermios.h"
#include "final/fwidgetcolors.h"
#include "final/fwindow.h"

namespace finalcut
{

// Global application object
static FApplication* app_object{nullptr};

// Flag to exit the local event loop
static bool app_exit_loop{false};

// Static attributes
FWidget*       FWidget::main_widget          {nullptr};  // main application widget
FWidget*       FWidget::active_window        {nullptr};  // the active window
FWidget*       FWidget::focus_widget         {nullptr};  // has keyboard input focus
FWidget*       FWidget::clicked_widget       {nullptr};  // is focused by click
FWidget*       FWidget::open_menu            {nullptr};  // currently open menu
FWidget*       FWidget::move_size_widget     {nullptr};  // move/size by keyboard
FWidget*       FApplication::keyboard_widget {nullptr};  // has the keyboard focus
FKeyboard*     FApplication::keyboard        {nullptr};  // keyboard access
FMouseControl* FApplication::mouse           {nullptr};  // mouse control
int            FApplication::loop_level      {0};        // event loop level
int            FApplication::quit_code       {EXIT_SUCCESS};
bool           FApplication::quit_now        {false};
uInt64         FApplication::next_event_wait {5000};     // preset to 5 ms (200 Hz)
struct timeval FApplication::time_last_event{};


const std::vector<FApplication::CmdOption> FApplication::long_options =
{
  {"encoding",                 required_argument, nullptr,  'e' },
  {"log-file",                 required_argument, nullptr,  'l' },
  {"no-mouse",                 no_argument,       nullptr,  'm' },
  {"no-optimized-cursor",      no_argument,       nullptr,  'o' },
  {"no-terminal-detection",    no_argument,       nullptr,  'd' },
  {"no-terminal-data-request", no_argument,       nullptr,  'r' },
  {"no-color-change",          no_argument,       nullptr,  'c' },
  {"no-sgr-optimizer",         no_argument,       nullptr,  's' },
  {"vgafont",                  no_argument,       nullptr,  'v' },
  {"newfont",                  no_argument,       nullptr,  'n' },
  {"dark-theme",               no_argument,       nullptr,  't' },

#if defined(__FreeBSD__) || defined(__DragonFly__)
  {"no-esc-for-alt-meta",      no_argument,       nullptr,  'E' },
  {"no-cursorstyle-change",    no_argument,       nullptr,  'C' },
#elif defined(__NetBSD__) || defined(__OpenBSD__)
  {"no-esc-for-alt-meta",      no_argument,       nullptr,  'E' },
#endif

  {nullptr,                    0,                 nullptr,  0   }
};


//----------------------------------------------------------------------
// class FApplication
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FApplication::FApplication (const int& _argc, char* _argv[])
  : FWidget{processParameters(_argc, _argv)}
  , app_argc{_argc}
  , app_argv{_argv}
{
  if ( quit_now )
    return;

  if ( app_object )
  {
    auto ftermdata = FTerm::getFTermData();
    ftermdata->setExitMessage("FApplication: There should be "
                              "only one application object");
    FApplication::exit(EXIT_FAILURE);
    return;
  }

  // First define the application object
  app_object = this;

  if ( ! (_argc && _argv) )
  {
    typedef char* CString;
    static std::array<CString, 1> empty{{CString("")}};
    app_argc = 0;
    app_argv = empty.data();
  }

  init();
}

//----------------------------------------------------------------------
FApplication::~FApplication()  // destructor
{
  app_object = nullptr;

  if ( eventInQueue() )
    event_queue.clear();

  destroyLog();
}


// public methods of FApplication
//----------------------------------------------------------------------
FApplication* FApplication::getApplicationObject()
{
  return app_object;
}

//----------------------------------------------------------------------
FWidget* FApplication::getKeyboardWidget()
{
  return keyboard_widget;
}

//----------------------------------------------------------------------
FApplication::FLogPtr& FApplication::getLog()
{
  // Global logger object
  static auto logger_ptr = new FLogPtr();

  if ( logger_ptr && logger_ptr->get() == nullptr )
  {
    *logger_ptr = std::make_shared<FLogger>();

    // Set the logger as rdbuf of clog
    std::clog.rdbuf(logger_ptr->get());
  }

  return *logger_ptr;
}

//----------------------------------------------------------------------
void FApplication::setLog (const FLogPtr& log)
{
  FLogPtr& logger = getLog();
  logger.reset();
  logger = log;

  // Set the logger as rdbuf of clog
  std::clog.rdbuf(logger.get());
}

//----------------------------------------------------------------------
bool FApplication::isQuit()
{
  return ( app_object ) ? quit_now : true;
}

//----------------------------------------------------------------------
int FApplication::exec()  // run
{
  if ( quit_now )
  {
    quit_now = false;
    return quit_code;
  }

  quit_code = EXIT_SUCCESS;
  enterLoop();
  return quit_code;
}

//----------------------------------------------------------------------
int FApplication::enterLoop()  // event loop
{
  loop_level++;
  quit_now = false;

  const bool old_app_exit_loop = app_exit_loop;
  app_exit_loop = false;

  while ( ! (quit_now || app_exit_loop) )
    processNextEvent();

  app_exit_loop = old_app_exit_loop;
  loop_level--;
  return 0;
}

//----------------------------------------------------------------------
void FApplication::exitLoop() const
{
  app_exit_loop = true;
}

//----------------------------------------------------------------------
void FApplication::exit (int retcode)
{
  quit_now  = true;
  quit_code = retcode;
}

//----------------------------------------------------------------------
void FApplication::quit() const
{
  FApplication::exit(0);
}

//----------------------------------------------------------------------
bool FApplication::sendEvent (FObject* receiver, FEvent* event )
{
  if ( quit_now || app_exit_loop || ! (bool(receiver) && bool(event)) )
    return false;

  if ( ! isEventProcessable (receiver, event) )
    return false;

  // Sends the event event directly to receiver
  bool ret = receiver->event(event);
  event->send = true;
  return ret;
}

//----------------------------------------------------------------------
void FApplication::queueEvent (FObject* receiver, FEvent* event)
{
  if ( ! (bool(receiver) && bool(event)) )
    return;

  // queue this event
  event->queued = true;
  event_queue.emplace_back (receiver, event);
}

//----------------------------------------------------------------------
void FApplication::sendQueuedEvents()
{
  while ( eventInQueue() )
  {
    const EventPair& event_pair = event_queue.front();
    event_pair.second->queued = false;
    sendEvent(event_pair.first, event_pair.second);
    event_queue.pop_front();
  }
}

//----------------------------------------------------------------------
bool FApplication::eventInQueue() const
{
  if ( app_object )
    return ( ! event_queue.empty() );
  else
    return false;
}

//----------------------------------------------------------------------
bool FApplication::removeQueuedEvent (const FObject* receiver)
{
  if ( ! eventInQueue() )
    return false;

  if ( ! receiver )
    return false;

  bool retval{false};
  auto iter = event_queue.begin();

  while ( iter != event_queue.end() )
  {
    if ( iter->first == receiver )
    {
      iter = event_queue.erase(iter);
      retval = true;
    }
    else
      ++iter;
  }

  return retval;
}

//----------------------------------------------------------------------
void FApplication::initTerminal()
{
  if ( ! isQuit() )
    FWidget::initTerminal();
}

//----------------------------------------------------------------------
void FApplication::setDefaultTheme()
{
  if ( FTerm::getMaxColor() < 16 )  // for 8 color mode
  {
    if ( getStartOptions().color_change )
      FTerm::setColorPaletteTheme<default8ColorPalette>();

    setColorTheme<default8ColorTheme>();
  }
  else
  {
    if ( getStartOptions().color_change )
      FTerm::setColorPaletteTheme<default16ColorPalette>();

    setColorTheme<default16ColorTheme>();
  }
}

//----------------------------------------------------------------------
void FApplication::setDarkTheme()
{
  if ( getStartOptions().color_change )
    FTerm::setColorPaletteTheme<default16DarkColorPalette>();

  if ( FTerm::getMaxColor() < 16 )  // for 8 color mode
    setColorTheme<default8ColorDarkTheme>();
  else
    setColorTheme<default16ColorDarkTheme>();
}

//----------------------------------------------------------------------
void FApplication::setLogFile (const FString& filename)
{
  auto& log_stream = getStartOptions().logfile_stream;
  log_stream.open(filename, std::ofstream::out);

  if ( log_stream.is_open() )
  {
    // Get the global logger object
    FLog& log = *FApplication::getLog();
    log.setOutputStream(log_stream);
    log.enableTimestamp();
    log.setLineEnding (finalcut::FLog::LF);
  }
  else
  {
    auto ftermdata = FTerm::getFTermData();
    ftermdata->setExitMessage ( "Could not open log file \""
                              + filename + "\"" );
    exit(EXIT_FAILURE);
  }
}

//----------------------------------------------------------------------
void FApplication::setKeyboardWidget (FWidget* widget)
{
  keyboard_widget = widget;
}

//----------------------------------------------------------------------
void FApplication::closeConfirmationDialog (FWidget* w, FCloseEvent* ev)
{
  app_object->unsetMoveSizeMode();
  const int ret = FMessageBox::info ( w, "Quit"
                                    , "Do you really want\n"
                                      "to quit the program ?"
                                    , FMessageBox::Yes
                                    , FMessageBox::No );
  if ( ret == FMessageBox::Yes )
    ev->accept();
  else
  {
    ev->ignore();

    // Status bar restore after closing the FMessageBox
    if ( getStatusBar() )
      getStatusBar()->drawMessage();
  }
}


// protected methods of FApplication
//----------------------------------------------------------------------
void FApplication::processExternalUserEvent()
{
  // This method can be overloaded and replaced by own code

  if ( FKeyboard::getReadBlockingTime() < 10000 )
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
}


// private methods of FApplication
//----------------------------------------------------------------------
void FApplication::init()
{
  // FApplication cannot have a second child widget
  setMaxChildren(1);

  // Initialize the last event time
  time_last_event.tv_sec = 0;
  time_last_event.tv_usec = 0;

  // Initialize keyboard
  keyboard = FTerm::getFKeyboard();

  // Set the keyboard keypress timeout
  if ( keyboard )
  {
    auto cmd1 = std::bind(&FApplication::keyPressed, this);
    auto cmd2 = std::bind(&FApplication::keyReleased, this);
    auto cmd3 = std::bind(&FApplication::escapeKeyPressed, this);
    FKeyboardCommand key_cmd1 (cmd1);
    FKeyboardCommand key_cmd2 (cmd2);
    FKeyboardCommand key_cmd3 (cmd3);
    keyboard->setPressCommand (key_cmd1);
    keyboard->setReleaseCommand (key_cmd2);
    keyboard->setEscPressedCommand (key_cmd3);
    keyboard->setKeypressTimeout (key_timeout);
  }

  // Initialize mouse control
  mouse = FTerm::getFMouseControl();

  // Set stdin number for a gpm-mouse
  if ( mouse )
    mouse->setStdinNo (FTermios::getStdIn());

  // Set the default double click interval
  if ( mouse )
    mouse->setDblclickInterval (dblclick_interval);

  // Initialize logging
  if ( ! getStartOptions().logfile_stream.is_open() )
    getLog()->setLineEnding(FLog::CRLF);
}

//----------------------------------------------------------------------
void FApplication::setTerminalEncoding (const FString& enc_str)
{
  const FString& enc = enc_str.toLower();

  if ( enc.includes("utf8") )
    getStartOptions().encoding = fc::UTF8;
  else if ( enc.includes("vt100") )
    getStartOptions().encoding = fc::VT100;
  else if ( enc.includes("pc") )
    getStartOptions().encoding = fc::PC;
  else if ( enc.includes("ascii") )
    getStartOptions().encoding = fc::ASCII;
  else if ( enc.includes("help") )
    showParameterUsage();
  else
  {
    auto ftermdata = FTerm::getFTermData();
    ftermdata->setExitMessage ( "Unknown encoding \"" + enc_str
                              + "\"\n(Valid encodings are utf8, "
                              + "vt100, pc and ascii)" );
    exit(EXIT_FAILURE);
  }
}

//----------------------------------------------------------------------
inline FApplication::CmdMap& FApplication::mapCmdOptions()
{
  using std::placeholders::_1;
  auto enc = std::bind(&FApplication::setTerminalEncoding, _1);
  auto log = std::bind(&FApplication::setLogFile, _1);
  auto opt = &FApplication::getStartOptions;
  static CmdMap cmd_map{};

  // --encoding
  cmd_map['e'] = [enc] (char* arg) { enc(FString(arg)); };
  // --log-file
  cmd_map['l'] = [log] (char* arg) { log(FString(arg)); };
  // --no-mouse
  cmd_map['m'] = [opt] (char*) { opt().mouse_support = false; };
  // --no-optimized-cursor
  cmd_map['o'] = [opt] (char*) { opt().cursor_optimisation = false; };
  // --no-terminal-detection
  cmd_map['d'] = [opt] (char*) { opt().terminal_detection = false; };
  // --no-terminal-data-request
  cmd_map['r'] = [opt] (char*) { opt().terminal_data_request = false; };
  // --no-color-change
  cmd_map['c'] = [opt] (char*) { opt().color_change = false; };
  // --no-sgr-optimizer
  cmd_map['s'] = [opt] (char*) { opt().sgr_optimizer = false; };
  // --vgafont
  cmd_map['v'] = [opt] (char*) { opt().vgafont = true; };
  // --newfont
  cmd_map['n'] = [opt] (char*) { opt().newfont = true; };
  // --dark-theme
  cmd_map['t'] = [opt] (char*) { opt().dark_theme = true; };
#if defined(__FreeBSD__) || defined(__DragonFly__)
  // --no-esc-for-alt-meta
  cmd_map['E'] = [opt] (char*) { opt().meta_sends_escape = false; };
  // --no-cursorstyle-change
  cmd_map['C'] = [opt] (char*) { opt().change_cursorstyle = false; };
#elif defined(__NetBSD__) || defined(__OpenBSD__)
  // --no-esc-for-alt-meta
  cmd_map['E'] = [opt] (char*) { opt().meta_sends_escape = false; };
#endif
  return cmd_map;
}

//----------------------------------------------------------------------
void FApplication::cmdOptions (const int& argc, char* argv[])
{
  // Interpret the command line options

  auto& cmd_map = mapCmdOptions();

  while ( true )
  {
    opterr = 0;
    int idx{0};
    auto p = reinterpret_cast<const struct option*>(long_options.data());
    const int opt = getopt_long (argc, argv, "", p, &idx);

    if ( opt == -1 )
      break;

    if ( cmd_map.find(opt) != cmd_map.end() )
      cmd_map[opt](optarg);
  }

  cmd_map.clear();
}

//----------------------------------------------------------------------
inline FStartOptions& FApplication::getStartOptions()
{
  return FStartOptions::getFStartOptions();
}

//----------------------------------------------------------------------
void FApplication::showParameterUsage()
{
  std::cout \
    << "Generic options:\n"
    << "  -h, --help                "
    << "    Display this help and exit\n"
    << "\n"
    << "FINAL CUT options:\n"
    << "  --encoding=<MODE>         "
    << "    Sets the character encoding mode\n"
    << "                            "
    << "    {utf8, vt100, pc, ascii}\n"
    << "  --log-file=<FILE>         "
    << "    Writes log output to FILE\n"
    << "  --no-mouse                "
    << "    Disable mouse support\n"
    << "  --no-optimized-cursor     "
    << "    Disable cursor optimization\n"
    << "  --no-terminal-detection   "
    << "    Disable terminal detection\n"
    << "  --no-terminal-data-request"
    << "    Do not determine terminal font and title\n"
    << "  --no-color-change         "
    << "    Do not redefine the color palette\n"
    << "  --no-sgr-optimizer        "
    << "    Do not optimize SGR sequences\n"
    << "  --vgafont                 "
    << "    Set the standard vga 8x16 font\n"
    << "  --newfont                 "
    << "    Enables the graphical font\n"
    << "  --dark-theme              "
    << "    Enables the dark theme\n"

#if defined(__FreeBSD__) || defined(__DragonFly__)
    << "\n"
    << "FreeBSD console options:\n"
    << "  --no-esc-for-alt-meta     "
    << "    Do not send a ESC prefix for the alt/meta key\n"
    << "  --no-cursorstyle-change   "
    << "    Do not change the current cursor style\n"
#elif defined(__NetBSD__) || defined(__OpenBSD__)
    << "\n"
    << "NetBSD/OpenBSD console options:\n"
    << "  --no-esc-for-alt-meta     "
    << "    Do not send a ESC prefix for the alt/meta key\n"
#endif

    << std::endl;  // newline character + flushes the output stream
}

//----------------------------------------------------------------------
inline void FApplication::destroyLog()
{
  // Reset the rdbuf of clog
  std::clog.rdbuf(default_clog_rdbuf);

  // Delete the logger
  const FLogPtr* logger = &(getLog());
  delete logger;
}

//----------------------------------------------------------------------
inline void FApplication::findKeyboardWidget() const
{
  // Find the widget that has the keyboard focus

  FWidget* widget{nullptr};
  auto focus = getFocusWidget();
  auto move_size = getMoveSizeWidget();

  if ( focus )
  {
    if ( move_size )
      widget = move_size;
    else
      widget = focus;
  }
  else
  {
    widget = getMainWidget();

    if ( widget && widget->numOfChildren() >= 1 )
      widget->focusFirstChild();
  }

  keyboard_widget = widget;
}

//----------------------------------------------------------------------
inline bool FApplication::isKeyPressed() const
{
  if ( mouse && mouse->isGpmMouseEnabled() )
    return mouse->getGpmKeyPressed(keyboard->unprocessedInput());

  return keyboard->isKeyPressed();
}

//----------------------------------------------------------------------
void FApplication::keyPressed()
{
  performKeyboardAction();
}

//----------------------------------------------------------------------
void FApplication::keyReleased() const
{
  sendKeyUpEvent (keyboard_widget);
}

//----------------------------------------------------------------------
void FApplication::escapeKeyPressed() const
{
  sendEscapeKeyPressEvent();
}

//----------------------------------------------------------------------
inline void FApplication::performKeyboardAction()
{
  switch ( keyboard->getKey() )
  {
    case fc::Fckey_l:  // Ctrl-L (redraw the screen)
      redraw();
      break;

    case fc::Fkey_mouse:
      if ( mouse )
      {
        FKeyboard::keybuffer& buffer = keyboard->getKeyBuffer();
        mouse->setRawData (FMouse::x11, buffer);
        keyboard->unprocessedInput() = mouse->isInputDataPending();
        processMouseEvent();
      }
      break;

    case fc::Fkey_extended_mouse:
      if ( mouse )
      {
        FKeyboard::keybuffer& buffer = keyboard->getKeyBuffer();
        mouse->setRawData (FMouse::sgr, buffer);
        keyboard->unprocessedInput() = mouse->isInputDataPending();
        processMouseEvent();
      }
      break;

    case fc::Fkey_urxvt_mouse:
      if ( mouse )
      {
        FKeyboard::keybuffer& buffer = keyboard->getKeyBuffer();
        mouse->setRawData (FMouse::urxvt, buffer);
        keyboard->unprocessedInput() = mouse->isInputDataPending();
        processMouseEvent();
      }
      break;

    default:
      const bool acceptKeyDown = sendKeyDownEvent (keyboard_widget);
      const bool acceptKeyPress = sendKeyPressEvent (keyboard_widget);

      if ( ! (acceptKeyDown || acceptKeyPress) )
        sendKeyboardAccelerator();

      break;
  }
}

//----------------------------------------------------------------------
inline void FApplication::sendEscapeKeyPressEvent() const
{
  // Send an escape key press event
  FKeyEvent k_press_ev (fc::KeyPress_Event, fc::Fkey_escape);
  sendEvent (keyboard_widget, &k_press_ev);
}

//----------------------------------------------------------------------
inline bool FApplication::sendKeyDownEvent (FWidget* widget) const
{
  // Send key down event
  FKeyEvent k_down_ev (fc::KeyDown_Event, keyboard->getKey());
  sendEvent (widget, &k_down_ev);
  return k_down_ev.isAccepted();
}

//----------------------------------------------------------------------
inline bool FApplication::sendKeyPressEvent (FWidget* widget) const
{
  // Send key press event
  FKeyEvent k_press_ev (fc::KeyPress_Event, keyboard->getKey());
  sendEvent (widget, &k_press_ev);
  return k_press_ev.isAccepted();
}

//----------------------------------------------------------------------
inline bool FApplication::sendKeyUpEvent (FWidget* widget) const
{
  // Send key up event
  FKeyEvent k_up_ev (fc::KeyUp_Event, keyboard->getKey());
  sendEvent (widget, &k_up_ev);
  return k_up_ev.isAccepted();
}

//----------------------------------------------------------------------
inline void FApplication::sendKeyboardAccelerator() const
{
  if ( FWidget::getOpenMenu() )
    return;

  // Switch to a specific dialog with Meta + 1..9
  bool accpt = processDialogSwitchAccelerator();

  // Windows keyboard accelerator
  if ( ! accpt )
  {
    auto window = static_cast<const FWidget*>(getActiveWindow());

    if ( window )
      accpt = processAccelerator (window);
  }

  // Global keyboard accelerator
  if ( ! accpt )
  {
    auto root_widget = static_cast<const FWidget*>(getRootWidget());

    if ( root_widget )
      processAccelerator (root_widget);
  }
}

//----------------------------------------------------------------------
void FApplication::processKeyboardEvent() const
{
  if ( quit_now || app_exit_loop )
    return;

  findKeyboardWidget();
  flush();
  keyboard->escapeKeyHandling();  // special case: Esc key
  keyboard->clearKeyBufferOnTimeout();

  if ( isKeyPressed() )
    keyboard->fetchKeyCode();
}

//----------------------------------------------------------------------
bool FApplication::processDialogSwitchAccelerator() const
{
  if ( keyboard->getKey() >= fc::Fmkey_1
    && keyboard->getKey() <= fc::Fmkey_9 )
  {
    const FKey key = keyboard->getKey();
    const std::size_t n = key - fc::Fmkey_0;
    const std::size_t s = getDialogList()->size();

    if ( s > 0 && s >= n )
    {
      // unset the move/size mode
      auto move_size = getMoveSizeWidget();

      if ( move_size )
      {
        auto w = move_size;
        setMoveSizeWidget(nullptr);
        w->redraw();
      }

      FAccelEvent a_ev (fc::Accelerator_Event, getFocusWidget());
      sendEvent (getDialogList()->at(n - 1), &a_ev);
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------
bool FApplication::processAccelerator (const FWidget* const& widget) const
{
  bool accpt{false};

  if ( widget && widget->getAcceleratorList().empty() )
    return accpt;

  for (auto&& item : widget->getAcceleratorList())
  {
    if ( item.key == keyboard->getKey() )
    {
      // unset the move/size mode
      auto move_size = getMoveSizeWidget();

      if ( move_size )
      {
        setMoveSizeWidget(nullptr);
        move_size->redraw();
      }

      FAccelEvent a_ev (fc::Accelerator_Event, getFocusWidget());
      sendEvent (item.object, &a_ev);
      accpt = a_ev.isAccepted();
      break;
    }

    if ( quit_now || app_exit_loop )
      break;
  }

  return accpt;
}

//----------------------------------------------------------------------
bool FApplication::getMouseEvent() const
{
  bool mouse_event_occurred{false};

  if ( mouse && mouse->hasData() )
  {
    struct timeval* time_keypressed = keyboard->getKeyPressedTime();
    mouse->processEvent (time_keypressed);
    keyboard->unprocessedInput() = mouse->isInputDataPending();
    mouse_event_occurred = mouse->hasEvent();
  }

  return mouse_event_occurred;
}

//----------------------------------------------------------------------
FWidget*& FApplication::determineClickedWidget()
{
  FWidget*& clicked = FWidget::getClickedWidget();

  if ( clicked || ! mouse )
    return clicked;

  if ( ! mouse->isLeftButtonPressed()
    && ! mouse->isLeftButtonDoubleClick()
    && ! mouse->isRightButtonPressed()
    && ! mouse->isMiddleButtonPressed()
    && ! mouse->isWheelUp()
    && ! mouse->isWheelDown() )
    return clicked;

  auto mouse_position = mouse->getPos();

  // Determine the window object on the current click position
  auto window = FWindow::getWindowWidgetAt (mouse_position);

  if ( window )
  {
    // Determine the widget at the current click position
    auto child = window->childWidgetAt (mouse_position);
    clicked = ( child != nullptr ) ? child : window;
    setClickedWidget (clicked);
  }

  return clicked;
}

//----------------------------------------------------------------------
void FApplication::unsetMoveSizeMode() const
{
  // Unset the move/size mode

  auto move_size = getMoveSizeWidget();

  if ( move_size )
  {
    auto w = move_size;
    setMoveSizeWidget(nullptr);
    w->redraw();
  }
}

//----------------------------------------------------------------------
void FApplication::closeDropDown() const
{
  // Close the open menu

  if ( ! mouse || mouse->isMoved() )
    return;

  auto mouse_position = mouse->getPos();
  finalcut::closeDropDown (this, mouse_position);
}

//----------------------------------------------------------------------
void FApplication::unselectMenubarItems() const
{
  // Unselect the menu bar items

  const auto& openmenu = FWidget::getOpenMenu();
  auto menu_bar = FWidget::getMenuBar();

  if ( openmenu || (mouse && mouse->isMoved()) )
    return;

  if ( ! (menu_bar && menu_bar->hasSelectedItem() && mouse) )
    return;

  const auto& mouse_position = mouse->getPos();

  if ( ! menu_bar->getTermGeometry().contains(mouse_position) )
  {
    if ( FWidget::getStatusBar() )
      FWidget::getStatusBar()->clearMessage();

    menu_bar->resetMenu();
    menu_bar->redraw();

    // No widget was been clicked
    if ( ! FWidget::getClickedWidget() )
      FWindow::switchToPrevWindow(this);

    if ( FWidget::getStatusBar() )
      FWidget::getStatusBar()->drawMessage();

    updateTerminal();
    flush();
  }
}

//----------------------------------------------------------------------
void FApplication::sendMouseEvent() const
{
  auto clicked = FWidget::getClickedWidget();

  if ( ! (clicked && mouse) )
    return;

  const auto& mouse_position = mouse->getPos();
  int key_state{0};

  if ( mouse->isShiftKeyPressed() )
    key_state |= fc::ShiftButton;

  if ( mouse->isControlKeyPressed() )
    key_state |= fc::ControlButton;

  if ( mouse->isMetaKeyPressed() )
    key_state |= fc::MetaButton;

  const auto& widgetMousePos = clicked->termToWidgetPos(mouse_position);

  if ( mouse->isMoved() )
  {
    sendMouseMoveEvent (widgetMousePos, mouse_position, key_state);
  }
  else
  {
    sendMouseLeftClickEvent (widgetMousePos, mouse_position, key_state);
    sendMouseRightClickEvent (widgetMousePos, mouse_position, key_state);
    sendMouseMiddleClickEvent (widgetMousePos, mouse_position, key_state);
  }

  sendWheelEvent (widgetMousePos, mouse_position);
  mouse->clearEvent();
}

//----------------------------------------------------------------------
void FApplication::sendMouseMoveEvent ( const FPoint& widgetMousePos
                                      , const FPoint& mouse_position
                                      , int key_state ) const
{
  if ( ! mouse )
    return;

  auto clicked = FWidget::getClickedWidget();

  if ( mouse->isLeftButtonPressed() )
  {
    FMouseEvent m_down_ev ( fc::MouseMove_Event
                          , widgetMousePos
                          , mouse_position
                          , fc::LeftButton | key_state );
    sendEvent (clicked, &m_down_ev);
  }

  if ( mouse->isRightButtonPressed() )
  {
    FMouseEvent m_down_ev ( fc::MouseMove_Event
                          , widgetMousePos
                          , mouse_position
                          , fc::RightButton | key_state );
    sendEvent (clicked, &m_down_ev);
  }

  if ( mouse->isMiddleButtonPressed() )
  {
    FMouseEvent m_down_ev ( fc::MouseMove_Event
                          , widgetMousePos
                          , mouse_position
                          , fc::MiddleButton | key_state );
    sendEvent (clicked, &m_down_ev);
  }
}

//----------------------------------------------------------------------
void FApplication::sendMouseLeftClickEvent ( const FPoint& widgetMousePos
                                           , const FPoint& mouse_position
                                           , int key_state ) const
{
  if ( ! mouse )
    return;

  auto clicked = FWidget::getClickedWidget();

  if ( mouse->isLeftButtonDoubleClick() )
  {
    FMouseEvent m_dblclick_ev ( fc::MouseDoubleClick_Event
                              , widgetMousePos
                              , mouse_position
                              , fc::LeftButton | key_state );
    sendEvent (clicked, &m_dblclick_ev);
  }
  else if ( mouse->isLeftButtonPressed() )
  {
    FMouseEvent m_down_ev ( fc::MouseDown_Event
                          , widgetMousePos
                          , mouse_position
                          , fc::LeftButton | key_state );
    sendEvent (clicked, &m_down_ev);
  }
  else if ( mouse->isLeftButtonReleased() )
  {
    FMouseEvent m_up_ev ( fc::MouseUp_Event
                        , widgetMousePos
                        , mouse_position
                        , fc::LeftButton | key_state );
    auto released_widget = clicked;

    if ( ! mouse->isRightButtonPressed()
      && ! mouse->isMiddleButtonPressed() )
      setClickedWidget(nullptr);

    sendEvent (released_widget, &m_up_ev);
  }
}

//----------------------------------------------------------------------
void FApplication::sendMouseRightClickEvent ( const FPoint& widgetMousePos
                                            , const FPoint& mouse_position
                                            , int key_state ) const
{
  if ( ! mouse )
    return;

  auto clicked = FWidget::getClickedWidget();

  if ( mouse->isRightButtonPressed() )
  {
    FMouseEvent m_down_ev ( fc::MouseDown_Event
                          , widgetMousePos
                          , mouse_position
                          , fc::RightButton | key_state );
    sendEvent (clicked, &m_down_ev);
  }
  else if ( mouse->isRightButtonReleased() )
  {
    FMouseEvent m_up_ev ( fc::MouseUp_Event
                        , widgetMousePos
                        , mouse_position
                        , fc::RightButton | key_state );
    auto released_widget = clicked;

    if ( ! mouse->isLeftButtonPressed()
      && ! mouse->isMiddleButtonPressed() )
      setClickedWidget(nullptr);

    sendEvent (released_widget, &m_up_ev);
  }
}

//----------------------------------------------------------------------
void FApplication::sendMouseMiddleClickEvent ( const FPoint& widgetMousePos
                                             , const FPoint& mouse_position
                                             , int key_state ) const
{
  if ( ! mouse )
    return;

  auto clicked = FWidget::getClickedWidget();

  if ( mouse->isMiddleButtonPressed() )
  {
    FMouseEvent m_down_ev ( fc::MouseDown_Event
                          , widgetMousePos
                          , mouse_position
                          , fc::MiddleButton | key_state );
    sendEvent (clicked, &m_down_ev);

    // gnome-terminal sends no released on middle click
    if ( FTerm::isGnomeTerminal() )
      setClickedWidget(nullptr);
  }
  else if ( mouse->isMiddleButtonReleased() )
  {
    FMouseEvent m_up_ev ( fc::MouseUp_Event
                        , widgetMousePos
                        , mouse_position
                        , fc::MiddleButton | key_state );
    auto released_widget = clicked;

    if ( ! mouse->isLeftButtonPressed()
      && ! mouse->isRightButtonPressed() )
    {
      setClickedWidget(nullptr);
    }

    sendEvent (released_widget, &m_up_ev);
  }
}

//----------------------------------------------------------------------
void FApplication::sendWheelEvent ( const FPoint& widgetMousePos
                                  , const FPoint& mouse_position ) const
{
  if ( ! mouse )
    return;

  auto clicked = FWidget::getClickedWidget();

  if ( mouse->isWheelUp() )
  {
    FWheelEvent wheel_ev ( fc::MouseWheel_Event
                         , widgetMousePos
                         , mouse_position
                         , fc::WheelUp );
    auto scroll_over_widget = clicked;
    setClickedWidget(nullptr);
    sendEvent(scroll_over_widget, &wheel_ev);
  }

  if ( mouse->isWheelDown() )
  {
    FWheelEvent wheel_ev ( fc::MouseWheel_Event
                         , widgetMousePos
                         , mouse_position
                         , fc::WheelDown );
    auto scroll_over_widget = clicked;
    setClickedWidget(nullptr);
    sendEvent (scroll_over_widget, &wheel_ev);
  }
}

//----------------------------------------------------------------------
FWidget* FApplication::processParameters (const int& argc, char* argv[])
{
  if ( argc > 0 && argv[1] && ( std::strcmp(argv[1], "--help") == 0
                             || std::strcmp(argv[1], "-h") == 0 ) )
  {
    showParameterUsage();
    FApplication::exit(EXIT_SUCCESS);
  }

  cmdOptions (argc, argv);
  return nullptr;
}

//----------------------------------------------------------------------
void FApplication::processMouseEvent()
{
  if ( ! getMouseEvent() )
    return;

  determineClickedWidget();
  unsetMoveSizeMode();
  closeDropDown();
  unselectMenubarItems();
  sendMouseEvent();

  if ( mouse )
    mouse->drawGpmPointer();
}

//----------------------------------------------------------------------
void FApplication::processResizeEvent() const
{
  if ( ! FTerm::hasChangedTermSize() )
    return;

  if ( mouse )
  {
    mouse->setMaxWidth (uInt16(getDesktopWidth()));
    mouse->setMaxHeight (uInt16(getDesktopHeight()));
  }

  FResizeEvent r_ev(fc::Resize_Event);
  sendEvent(app_object, &r_ev);

  if ( r_ev.isAccepted() )
    FTerm::changeTermSizeFinished();
}

//----------------------------------------------------------------------
void FApplication::processCloseWidget()
{
  if ( ! getWidgetCloseList() || getWidgetCloseList()->empty() )
    return;

  setTerminalUpdates (FVTerm::stop_terminal_updates);
  auto iter = getWidgetCloseList()->begin();

  while ( iter != getWidgetCloseList()->end() && *iter )
  {
    delete *iter;
    ++iter;
  }

  getWidgetCloseList()->clear();
  setTerminalUpdates (FVTerm::start_terminal_updates);
}

//----------------------------------------------------------------------
void FApplication::processLogger() const
{
  // Synchronizing the stream buffer with the logging output

  auto logger = getLog();

  if ( ! logger->str().empty() )
    logger->pubsync();

  logger->flush();
}

//----------------------------------------------------------------------
bool FApplication::processNextEvent()
{
  uInt num_events{0};
  bool is_timeout = isNextEventTimeout();

  if ( is_timeout )
  {
    FObject::getCurrentTime (&time_last_event);
    processKeyboardEvent();
    processMouseEvent();
    processResizeEvent();
    processTerminalUpdate();
    processCloseWidget();
    processLogger();
    updateTerminal();
  }

  processExternalUserEvent();

  if ( is_timeout )
  {
    sendQueuedEvents();
    num_events += processTimerEvent();
    uInt64 wait{next_event_wait / 2};
    std::this_thread::sleep_for(std::chrono::microseconds(wait));
  }

  return ( num_events > 0 );
}

//----------------------------------------------------------------------
void FApplication::performTimerAction (FObject* receiver, FEvent* event)
{
  sendEvent (receiver, event);
}

//----------------------------------------------------------------------
bool FApplication::isEventProcessable ( const FObject* receiver
                                      , const FEvent* event )
{
  if ( ! receiver->isWidget() )  // No restrictions for non-widgets
    return true;

  const auto widget = static_cast<const FWidget*>(receiver);

  if ( getModalDialogCounter() > 0 )
  {
    const FWidget* window;

    if ( widget->isWindowWidget() )
      window = widget;
    else
      window = FWindow::getWindowWidget(widget);

    // block events for widgets in non modal windows
    if ( window
      && ! window->getFlags().modal
      && ! window->isMenuWidget() )
    {
      switch ( uInt(event->getType()) )
      {
        case fc::KeyPress_Event:
        case fc::KeyUp_Event:
        case fc::KeyDown_Event:
        case fc::MouseDown_Event:
        case fc::MouseUp_Event:
        case fc::MouseDoubleClick_Event:
        case fc::MouseWheel_Event:
        case fc::MouseMove_Event:
        case fc::FocusIn_Event:
        case fc::FocusOut_Event:
        case fc::ChildFocusIn_Event:
        case fc::ChildFocusOut_Event:
        case fc::Accelerator_Event:
          return false;

        default:
          break;
      }
    }
  }

  // Throw away mouse events for disabled widgets
  if ( event->getType() >= fc::MouseDown_Event
    && event->getType() <= fc::MouseMove_Event
    && ! widget->isEnabled() )
    return false;

  return true;
}

//----------------------------------------------------------------------
bool FApplication::isNextEventTimeout()
{
  return FObject::isTimeout (&time_last_event, next_event_wait);
}


// FLog non-member operators
//----------------------------------------------------------------------
std::ostream& operator << (std::ostream& outstr, FLog::LogLevel l)
{
  *FApplication::getLog() << l;
  return outstr;
}

}  // namespace finalcut
