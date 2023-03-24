/***********************************************************************
* fapplication.cpp - Manages the application events                    *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2013-2023 Markus Gans                                      *
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

#include "final/dialog/fmessagebox.h"
#include "final/fapplication.h"
#include "final/fevent.h"
#include "final/fstartoptions.h"
#include "final/fwidgetcolors.h"
#include "final/input/fmouse.h"
#include "final/menu/fmenubar.h"
#include "final/menu/fmenu.h"
#include "final/output/tty/ftermdata.h"
#include "final/output/tty/ftermios.h"
#include "final/util/flogger.h"
#include "final/util/flog.h"
#include "final/widget/fstatusbar.h"
#include "final/widget/fwindow.h"

namespace finalcut
{

namespace internal
{

struct var
{
  static FApplication* app_object;  // Global application object
  static bool          exit_loop;   // Flag to exit the local event loop
};

FApplication*  var::app_object {nullptr};
bool           var::exit_loop  {false};

}  // namespace internal

// Static attributes
FWidget*  FWidget::main_widget          {nullptr};  // main application widget
FWidget*  FWidget::active_window        {nullptr};  // the active window
FWidget*  FWidget::focus_widget         {nullptr};  // has keyboard input focus
FWidget*  FWidget::clicked_widget       {nullptr};  // is focused by click
FWidget*  FWidget::open_menu            {nullptr};  // currently open menu
FWidget*  FWidget::move_resize_widget   {nullptr};  // move or resize by keyboard
FWidget*  FApplication::keyboard_widget {nullptr};  // has the keyboard focus
int       FApplication::loop_level      {0};        // event loop level
int       FApplication::quit_code       {EXIT_SUCCESS};
bool      FApplication::quit_now        {false};
uInt64    FApplication::next_event_wait {5000};     // 5 ms (200 Hz)
TimeValue FApplication::time_last_event {};

// FEvent friend function forward declaration
void setSend (FEvent&, bool = true);
void setQueued (FEvent&, bool = true);


//----------------------------------------------------------------------
// class FApplication
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FApplication::FApplication (const int& arg_c, char* arg_v[])
  : FWidget{processParameters(Args(arg_v, arg_v + arg_c))}
  , app_args{arg_v, arg_v + arg_c}
{
  if ( quit_now )
    return;

  if ( internal::var::app_object )
  {
    setExitMessage("FApplication: There should be "
                   "only one application object");
    FApplication::exit(EXIT_FAILURE);
    return;
  }

  // First define the application object
  internal::var::app_object = this;

  init();
}

//----------------------------------------------------------------------
FApplication::~FApplication()  // destructor
{
  internal::var::app_object = nullptr;

  if ( eventInQueue() )
    event_queue.clear();

  destroyLog();
}


// public methods of FApplication
//----------------------------------------------------------------------
auto FApplication::getApplicationObject() -> FApplication*
{
  return internal::var::app_object;
}

//----------------------------------------------------------------------
auto FApplication::getKeyboardWidget() -> FWidget*
{
  return keyboard_widget;
}

//----------------------------------------------------------------------
auto FApplication::getLog() -> FLogPtr&
{
  // Global logger object
  static auto logger_ptr = new FLogPtr();

  if ( logger_ptr && *logger_ptr == nullptr )
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
auto FApplication::isQuit() -> bool
{
  return internal::var::app_object ? quit_now : true;
}

//----------------------------------------------------------------------
#if defined(UNIT_TEST)
void FApplication::start()  // Simulates application start
{
  quit_now = false;
}
#endif  // defined(UNIT_TEST)

//----------------------------------------------------------------------
auto FApplication::exec() -> int  // run
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
auto FApplication::enterLoop() -> int  // event loop
{
  loop_level++;
  quit_now = false;

  const bool old_app_exit_loop = internal::var::exit_loop;
  internal::var::exit_loop = false;

  while ( ! (quit_now || internal::var::exit_loop) )
    processNextEvent();

  internal::var::exit_loop = old_app_exit_loop;
  loop_level--;
  return 0;
}

//----------------------------------------------------------------------
void FApplication::exitLoop() const
{
  internal::var::exit_loop = true;
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
auto FApplication::sendEvent (FObject* receiver, FEvent* event ) -> bool
{
  if ( quit_now
    || internal::var::exit_loop
    || ! (bool(receiver) && bool(event))
    || ! isEventProcessable (receiver, event) )
    return false;

  // Sends the event event directly to receiver
  const auto& ret = receiver->event(event);
  setSend(*event);
  return ret;
}

//----------------------------------------------------------------------
void FApplication::queueEvent (FObject* receiver, FEvent* event)
{
  if ( ! (bool(receiver) && bool(event)) )
    return;

  // queue this event
  setQueued(*event);
  event_queue.emplace_back (receiver, event);
}

//----------------------------------------------------------------------
void FApplication::sendQueuedEvents()
{
  while ( eventInQueue() )
  {
    const auto& event_pair = event_queue.front();
    setQueued(*event_pair.second, false);
    sendEvent(event_pair.first, event_pair.second);
    event_queue.pop_front();
  }
}

//----------------------------------------------------------------------
auto FApplication::eventInQueue() const -> bool
{
  if ( internal::var::app_object )
    return ( ! event_queue.empty() );

  return false;
}

//----------------------------------------------------------------------
auto FApplication::removeQueuedEvent (const FObject* receiver) -> bool
{
  if ( ! eventInQueue() )
    return false;

  if ( ! receiver )
    return false;

  bool retval{false};
  auto iter = event_queue.cbegin();

  while ( iter != event_queue.cend() )
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
  auto foutput = FVTerm::getFOutput();

  if ( foutput->getMaxColor() < 16 )  // for 8 color mode
  {
    if ( getStartOptions().color_change )
      foutput->setColorPaletteTheme<default8ColorPalette>();

    setColorTheme<default8ColorTheme>();
  }
  else
  {
    if ( getStartOptions().color_change )
      foutput->setColorPaletteTheme<default16ColorPalette>();

    setColorTheme<default16ColorTheme>();
  }
}

//----------------------------------------------------------------------
void FApplication::setDarkTheme()
{
  auto foutput = FVTerm::getFOutput();

  if ( getStartOptions().color_change )
    foutput->setColorPaletteTheme<default16DarkColorPalette>();

  if ( foutput->getMaxColor() < 16 )  // for 8 color mode
    setColorTheme<default8ColorDarkTheme>();
  else
    setColorTheme<default16ColorDarkTheme>();
}

//----------------------------------------------------------------------
void FApplication::setLogFile (const FString& filename)
{
  auto& log_stream = getStartOptions().logfile_stream;
  log_stream.open(filename.toString(), std::ofstream::out);

  if ( log_stream.is_open() )
  {
    // Get the global logger object
    auto& log = *FApplication::getLog();
    log.setOutputStream(log_stream);
    log.enableTimestamp();
    log.setLineEnding (FLog::LineEnding::LF);
  }
  else
  {
    setExitMessage ("Could not open log file \"" + filename + "\"");
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
  internal::var::app_object->unsetMoveResizeMode();
  const auto& ret = \
      FMessageBox::info ( w, "Quit"
                        , "Do you really want\n"
                          "to quit the program ?"
                        , FMessageBox::ButtonType::Yes
                        , FMessageBox::ButtonType::No );

  if ( ret == FMessageBox::ButtonType::Yes )
    ev->accept();
  else
  {
    ev->ignore();

    // Status bar restore after closing the FMessageBox
    drawStatusBarMessage();
  }
}


// protected methods of FApplication
//----------------------------------------------------------------------
void FApplication::processExternalUserEvent()
{
  // This method can be overloaded and replaced by own code
}


// private methods of FApplication
//----------------------------------------------------------------------
void FApplication::init()
{
  // FApplication cannot have a second child widget
  setMaxChildren(1);

  // Initialize the last event time
  time_last_event = TimeValue{};

  // Initialize keyboard
  static auto& keyboard = FKeyboard::getInstance();
  auto cmd1 = [this] () { this->keyPressed(); };
  auto cmd2 = [this] () { this->keyReleased(); };
  auto cmd3 = [this] () { this->escapeKeyPressed(); };
  auto cmd4 = [this] () { this->mouseTracking(); };
  FKeyboardCommand key_cmd1 (cmd1);
  FKeyboardCommand key_cmd2 (cmd2);
  FKeyboardCommand key_cmd3 (cmd3);
  FKeyboardCommand key_cmd4 (cmd4);
  keyboard.setPressCommand (key_cmd1);
  keyboard.setReleaseCommand (key_cmd2);
  keyboard.setEscPressedCommand (key_cmd3);
  keyboard.setMouseTrackingCommand (key_cmd4);
  // Set the keyboard keypress timeout
  keyboard.setKeypressTimeout (key_timeout);

  // Initialize mouse control
  static auto& mouse = FMouseControl::getInstance();
  auto cmd = [this] (const auto& md) { this->mouseEvent(md); };
  FMouseCommand mouse_cmd (cmd);
  mouse.setEventCommand (mouse_cmd);
  // Set stdin number for a gpm-mouse
  mouse.setStdinNo (FTermios::getStdIn());
  // Set the default double click interval
  mouse.setDblclickInterval (dblclick_interval);

  // Initialize logging
  if ( ! getStartOptions().logfile_stream.is_open() )
    getLog()->setLineEnding(FLog::LineEnding::CRLF);
}

//----------------------------------------------------------------------
void FApplication::setTerminalEncoding (const FString& enc_str)
{
  const auto& enc = enc_str.toLower();

  if ( enc.includes("utf8") )
    getStartOptions().encoding = Encoding::UTF8;
  else if ( enc.includes("vt100") )
    getStartOptions().encoding = Encoding::VT100;
  else if ( enc.includes("pc") )
    getStartOptions().encoding = Encoding::PC;
  else if ( enc.includes("ascii") )
    getStartOptions().encoding = Encoding::ASCII;
  else if ( enc.includes("help") )
    showParameterUsage();
  else
  {
    setExitMessage ( "Unknown encoding \"" + enc_str
                   + "\"\n(Valid encodings are utf8, "
                   + "vt100, pc and ascii)" );
    exit(EXIT_FAILURE);
  }
}

//----------------------------------------------------------------------
inline auto FApplication::getLongOptions() -> const std::vector<CmdOption>&
{
  static const std::vector<CmdOption>& long_options =
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
  return long_options;
}

//----------------------------------------------------------------------
inline void FApplication::setCmdOptionsMap (CmdMap& cmd_map)
{
  auto enc = [] (const auto& s) { FApplication::setTerminalEncoding(s); };
  auto log = [] (const auto& s) { FApplication::setLogFile(s); };
  auto opt = &FApplication::getStartOptions;

  // --encoding
  cmd_map['e'] = [enc] (const auto& arg) { enc(FString(arg)); };
  // --log-file
  cmd_map['l'] = [log] (const auto& arg) { log(FString(arg)); };
  // --no-mouse
  cmd_map['m'] = [opt] (const auto&) { opt().mouse_support = false; };
  // --no-optimized-cursor
  cmd_map['o'] = [opt] (const auto&) { opt().cursor_optimisation = false; };
  // --no-terminal-detection
  cmd_map['d'] = [opt] (const auto&) { opt().terminal_detection = false; };
  // --no-terminal-data-request
  cmd_map['r'] = [opt] (const auto&) { opt().terminal_data_request = false; };
  // --no-color-change
  cmd_map['c'] = [opt] (const auto&) { opt().color_change = false; };
  // --no-sgr-optimizer
  cmd_map['s'] = [opt] (const auto&) { opt().sgr_optimizer = false; };
  // --vgafont
  cmd_map['v'] = [opt] (const auto&) { opt().vgafont = true; };
  // --newfont
  cmd_map['n'] = [opt] (const auto&) { opt().newfont = true; };
  // --dark-theme
  cmd_map['t'] = [opt] (const auto&) { opt().dark_theme = true; };
#if defined(__FreeBSD__) || defined(__DragonFly__)
  // --no-esc-for-alt-meta
  cmd_map['E'] = [opt] (const auto&) { opt().meta_sends_escape = false; };
  // --no-cursorstyle-change
  cmd_map['C'] = [opt] (const auto&) { opt().change_cursorstyle = false; };
#elif defined(__NetBSD__) || defined(__OpenBSD__)
  // --no-esc-for-alt-meta
  cmd_map['E'] = [opt] (const auto&) { opt().meta_sends_escape = false; };
#endif
}

//----------------------------------------------------------------------
void FApplication::cmdOptions (const Args& args)
{
  // Interpret the command line options

  CmdMap cmd_map{};
  setCmdOptionsMap(cmd_map);
  auto argc = args.size();
  std::vector<const char*> argv(argc);
  std::transform ( args.cbegin()
                 , args.cend()
                 , argv.begin()
                 , [] (const auto& str)
                   {
                     return str.data();
                   }
                 );

  while ( true )
  {
    opterr = 0;
    int idx{0};
    const auto& long_options = getLongOptions();
    const struct option* p = long_options.data();
    auto argv_data = const_cast<char* const*>(argv.data());
    const int opt = getopt_long (int(argc), argv_data, "", p, &idx);

    if ( opt == -1 )
      break;

    const auto& iter = cmd_map.find(opt);

    if ( iter != cmd_map.end() )
      iter->second(optarg);
  }

  cmd_map.clear();
}

//----------------------------------------------------------------------
inline auto FApplication::getStartOptions() -> FStartOptions&
{
  static auto& start_options = FStartOptions::getInstance();
  return start_options;
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
  std::clog << std::flush;
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
  auto move_size = getMoveResizeWidget();

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

    if ( widget && widget->numOfFocusableChildren() >= 1 )
      widget->focusFirstChild();
  }

  keyboard_widget = widget;
}

//----------------------------------------------------------------------
inline auto FApplication::isKeyPressed(uInt64 ms) const -> bool
{
  static auto& mouse = FMouseControl::getInstance();
  static auto& keyboard = FKeyboard::getInstance();
  const auto blocking_time = (ms != 0U) ? ms : keyboard.getReadBlockingTime();

  if ( mouse.isGpmMouseEnabled() )
    return mouse.getGpmKeyPressed(keyboard.hasUnprocessedInput());

  return (keyboard.isKeyPressed(blocking_time) || keyboard.hasPendingInput());
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
void FApplication::mouseTracking() const
{
  performMouseAction();
}

//----------------------------------------------------------------------
inline void FApplication::performKeyboardAction()
{
  static const auto& keyboard = FKeyboard::getInstance();

  if ( keyboard.getKey() == FKey::Ctrl_l )  // Ctrl-L (redraw the screen)
  {
    redraw();
  }
  else if ( keyboard.getKey() == FKey::Term_Focus_In
         || keyboard.getKey() == FKey::Term_Focus_Out )
  {
    processTerminalFocus (keyboard.getKey());  // Term focus-in/focus-out
  }
  else
  {
    const bool acceptKeyDown = sendKeyDownEvent (keyboard_widget);
    const bool acceptKeyPress = sendKeyPressEvent (keyboard_widget);

    if ( ! (acceptKeyDown || acceptKeyPress) )
      sendKeyboardAccelerator();
  }
}

//----------------------------------------------------------------------
inline void FApplication::performMouseAction() const
{
  static auto& mouse = FMouseControl::getInstance();
  static auto& keyboard = FKeyboard::getInstance();
  const auto key = keyboard.getKey();
  auto& buffer = keyboard.getKeyBuffer();

  if ( key == FKey::X11mouse )
  {
    mouse.setRawData (FMouse::MouseType::X11, buffer);
  }
  else if ( key == FKey::Extended_mouse )
  {
    mouse.setRawData (FMouse::MouseType::Sgr, buffer);
  }
  else if ( key == FKey::Urxvt_mouse )
  {
    mouse.setRawData (FMouse::MouseType::Urxvt, buffer);
  }

  queuingMouseInput();
}

//----------------------------------------------------------------------
void FApplication::mouseEvent (const FMouseData& md)
{
  determineClickedWidget (md);
  unsetMoveResizeMode();
  closeDropDown (md);
  unselectMenubarItems (md);

  if ( FWidget::getClickedWidget() )  // A widget was clicked
    sendMouseEvent (md);
}

//----------------------------------------------------------------------
inline void FApplication::sendEscapeKeyPressEvent() const
{
  // Send an escape key press event
  FKeyEvent k_press_ev (Event::KeyPress, FKey::Escape);
  sendEvent (keyboard_widget, &k_press_ev);
}

//----------------------------------------------------------------------
inline auto FApplication::sendKeyDownEvent (FWidget* widget) const -> bool
{
  // Send key down event
  static const auto& keyboard = FKeyboard::getInstance();
  FKeyEvent k_down_ev (Event::KeyDown, keyboard.getKey());
  sendEvent (widget, &k_down_ev);
  return k_down_ev.isAccepted();
}

//----------------------------------------------------------------------
inline auto FApplication::sendKeyPressEvent (FWidget* widget) const -> bool
{
  // Send key press event
  static const auto& keyboard = FKeyboard::getInstance();
  FKeyEvent k_press_ev (Event::KeyPress, keyboard.getKey());
  sendEvent (widget, &k_press_ev);
  return k_press_ev.isAccepted();
}

//----------------------------------------------------------------------
inline auto FApplication::sendKeyUpEvent (FWidget* widget) const -> bool
{
  // Send key up event
  static const auto& keyboard = FKeyboard::getInstance();
  FKeyEvent k_up_ev (Event::KeyUp, keyboard.getKey());
  sendEvent (widget, &k_up_ev);
  return k_up_ev.isAccepted();
}

//----------------------------------------------------------------------
inline void FApplication::sendKeyboardAccelerator()
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
      accpt = processAccelerator(*window);
  }

  // Global keyboard accelerator
  if ( ! accpt )
  {
    auto root_widget = getRootWidget();

    if ( root_widget )
      processAccelerator(*root_widget);
  }
}

//----------------------------------------------------------------------
inline auto FApplication::hasDataInQueue() const -> bool
{
  static const auto& keyboard = FKeyboard::getInstance();
  static const auto& mouse = FMouseControl::getInstance();
  return ( keyboard.hasDataInQueue() || mouse.hasDataInQueue() );
}

//----------------------------------------------------------------------
void FApplication::queuingKeyboardInput() const
{
  findKeyboardWidget();
  static auto& keyboard = FKeyboard::getInstance();
  keyboard.escapeKeyHandling();  // special case: Esc key
  keyboard.clearKeyBufferOnTimeout();

  if ( isKeyPressed() )
    keyboard.fetchKeyCode();
}

//----------------------------------------------------------------------
void FApplication::queuingMouseInput() const
{
  static auto& mouse = FMouseControl::getInstance();

  if ( ! mouse.hasData() )
    return;

  static const auto& keyboard = FKeyboard::getInstance();
  auto time_keypressed = keyboard.getKeyPressedTime();
  mouse.processEvent (time_keypressed);
  mouse.clearEvent();
}

//----------------------------------------------------------------------
void FApplication::processKeyboardEvent() const
{
  static auto& keyboard = FKeyboard::getInstance();
  keyboard.processQueuedInput();
}

//----------------------------------------------------------------------
void FApplication::processMouseEvent() const
{
  static auto& mouse = FMouseControl::getInstance();
  mouse.processQueuedInput();
}

//----------------------------------------------------------------------
inline void FApplication::processInput() const
{
  if ( quit_now || internal::var::exit_loop || has_terminal_resized )
    return;

  queuingKeyboardInput();
  queuingMouseInput();

  do
  {
    processKeyboardEvent();
    processMouseEvent();
  }
  while ( ! quit_now && hasDataInQueue() );
  // quit_now is checked again to avoid an infinite loop on exit
}

//----------------------------------------------------------------------
auto FApplication::processDialogSwitchAccelerator() const -> bool
{
  static const auto& keyboard = FKeyboard::getInstance();

  if ( keyboard.getKey() >= FKey::Meta_1
    && keyboard.getKey() <= FKey::Meta_9 )
  {
    const FKey key = keyboard.getKey();
    const auto n = std::size_t(key - FKey::Meta_0);
    const std::size_t s = getDialogList()->size();

    if ( s > 0 && s >= n )
    {
      // unset the move/resize mode
      auto move_size = getMoveResizeWidget();

      if ( move_size )
      {
        auto w = move_size;
        setMoveSizeWidget(nullptr);
        w->redraw();
      }

      FAccelEvent a_ev (Event::Accelerator, getFocusWidget());
      sendEvent (getDialogList()->at(n - 1), &a_ev);
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------
auto FApplication::processAccelerator (const FWidget& widget) const -> bool
{
  if ( widget.getAcceleratorList().empty() )
    return false;

  for (auto&& item : widget.getAcceleratorList())
  {
    static const auto& keyboard = FKeyboard::getInstance();

    if ( item.key == keyboard.getKey() )
    {
      // unset the move/size mode
      auto move_size = getMoveResizeWidget();

      if ( move_size )
      {
        setMoveSizeWidget(nullptr);
        move_size->redraw();
      }

      FAccelEvent a_ev (Event::Accelerator, getFocusWidget());
      sendEvent (item.object, &a_ev);
      return a_ev.isAccepted();
    }

    if ( quit_now || internal::var::exit_loop )
      break;
  }

  return false;
}

//----------------------------------------------------------------------
void FApplication::processTerminalFocus (const FKey& key)
{
  auto root_widget = getRootWidget();

  if ( ! root_widget
    || ( key != FKey::Term_Focus_In && key != FKey::Term_Focus_Out ) )
    return;

  // unset the move/size mode
  auto move_size = getMoveResizeWidget();

  if ( move_size )
  {
    setMoveSizeWidget(nullptr);
    move_size->redraw();
  }

  auto event = ( key == FKey::Term_Focus_In )
              ? Event::TerminalFocusIn
              : Event::TerminalFocusOut;
  FFocusEvent tf_ev (event);
  sendEvent (root_widget, &tf_ev);
}

//----------------------------------------------------------------------
void FApplication::determineClickedWidget (const FMouseData& md)
{
  clicked_widget = FWidget::getClickedWidget();

  if ( clicked_widget )
    return;  // The clicked widget was already found

  if ( ! md.isLeftButtonPressed()
    && ! md.isLeftButtonDoubleClick()
    && ! md.isRightButtonPressed()
    && ! md.isMiddleButtonPressed()
    && ! md.isWheelUp()
    && ! md.isWheelDown() )
    return;

  const auto& mouse_position = md.getPos();

  // Determine the window object on the current click position
  auto window = FWindow::getWindowWidgetAt (mouse_position);

  if ( window )
  {
    // Determine the widget at the current click position
    auto child = window->childWidgetAt(mouse_position);
    clicked_widget = ( child != nullptr ) ? child : window;
    setClickedWidget (clicked_widget);
  }
}

//----------------------------------------------------------------------
void FApplication::unsetMoveResizeMode() const
{
  // Unset the move or resize mode

  auto& move_size = getMoveResizeWidget();

  if ( move_size )
  {
    FWidget* w{nullptr};
    std::swap(w, move_size);  // Clear move_resize_widget
    w->redraw();
  }
}

//----------------------------------------------------------------------
void FApplication::closeDropDown (const FMouseData& md) const
{
  // Close the open menu

  if ( md.isMoved() )
    return;

  const auto& mouse_position = md.getPos();
  finalcut::closeDropDown (this, mouse_position);  // in fwindow.cpp
}

//----------------------------------------------------------------------
void FApplication::unselectMenubarItems (const FMouseData& md) const
{
  // Unselect the menu bar items

  const auto& openmenu = FWidget::getOpenMenu();
  auto menu_bar = FWidget::getMenuBar();

  if ( openmenu || md.isMoved() )
    return;

  if ( ! (menu_bar && menu_bar->hasSelectedItem()) )
    return;

  const auto& mouse_position = md.getPos();

  if ( ! menu_bar->getTermGeometry().contains(mouse_position) )
  {
    if ( FWidget::getStatusBar() )
      FWidget::getStatusBar()->clearMessage();

    menu_bar->resetMenu();
    menu_bar->redraw();

    // No widget was been clicked
    if ( ! FWidget::getClickedWidget() )
      FWindow::switchToPrevWindow(this);

    drawStatusBarMessage();
  }
}

//----------------------------------------------------------------------
void FApplication::sendMouseEvent (const FMouseData& md) const
{
  const auto& mouse_position = md.getPos();
  MouseButton key_state{MouseButton::None};

  if ( md.isShiftKeyPressed() )
    key_state |= MouseButton::Shift;

  if ( md.isControlKeyPressed() )
    key_state |= MouseButton::Control;

  if ( md.isMetaKeyPressed() )
    key_state |= MouseButton::Meta;

  const auto& widgetMousePos = clicked_widget->termToWidgetPos(mouse_position);

  if ( md.isMoved() )
  {
    sendMouseMoveEvent (md, widgetMousePos, mouse_position, key_state);
  }
  else
  {
    sendMouseLeftClickEvent (md, widgetMousePos, mouse_position, key_state);
    sendMouseRightClickEvent (md, widgetMousePos, mouse_position, key_state);
    sendMouseMiddleClickEvent (md, widgetMousePos, mouse_position, key_state);
  }

  sendWheelEvent (md, widgetMousePos, mouse_position);
}

//----------------------------------------------------------------------
void FApplication::sendMouseMoveEvent ( const FMouseData& md
                                      , const FPoint& widgetMousePos
                                      , const FPoint& mouse_position
                                      , MouseButton key_state ) const
{
  if ( md.isLeftButtonPressed() )
  {
    FMouseEvent m_down_ev ( Event::MouseMove
                          , widgetMousePos
                          , mouse_position
                          , MouseButton::Left | key_state );
    sendEvent (clicked_widget, &m_down_ev);
  }

  if ( md.isRightButtonPressed() )
  {
    FMouseEvent m_down_ev ( Event::MouseMove
                          , widgetMousePos
                          , mouse_position
                          , MouseButton::Right | key_state );
    sendEvent (clicked_widget, &m_down_ev);
  }

  if ( md.isMiddleButtonPressed() )
  {
    FMouseEvent m_down_ev ( Event::MouseMove
                          , widgetMousePos
                          , mouse_position
                          , MouseButton::Middle | key_state );
    sendEvent (clicked_widget, &m_down_ev);
  }
}

//----------------------------------------------------------------------
void FApplication::sendMouseLeftClickEvent ( const FMouseData& md
                                           , const FPoint& widgetMousePos
                                           , const FPoint& mouse_position
                                           , MouseButton key_state ) const
{
  if ( md.isLeftButtonDoubleClick() )
  {
    FMouseEvent m_dblclick_ev ( Event::MouseDoubleClick
                              , widgetMousePos
                              , mouse_position
                              , MouseButton::Left | key_state );
    sendEvent (clicked_widget, &m_dblclick_ev);
  }
  else if ( md.isLeftButtonPressed() )
  {
    FMouseEvent m_down_ev ( Event::MouseDown
                          , widgetMousePos
                          , mouse_position
                          , MouseButton::Left | key_state );
    sendEvent (clicked_widget, &m_down_ev);
  }
  else if ( md.isLeftButtonReleased() )
  {
    FMouseEvent m_up_ev ( Event::MouseUp
                        , widgetMousePos
                        , mouse_position
                        , MouseButton::Left | key_state );
    auto released_widget = clicked_widget;

    if ( ! md.isRightButtonPressed()
      && ! md.isMiddleButtonPressed() )
      setClickedWidget(nullptr);

    sendEvent (released_widget, &m_up_ev);
  }
}

//----------------------------------------------------------------------
void FApplication::sendMouseRightClickEvent ( const FMouseData& md
                                            , const FPoint& widgetMousePos
                                            , const FPoint& mouse_position
                                            , MouseButton key_state ) const
{
  if ( md.isRightButtonPressed() )
  {
    FMouseEvent m_down_ev ( Event::MouseDown
                          , widgetMousePos
                          , mouse_position
                          , MouseButton::Right | key_state );
    sendEvent (clicked_widget, &m_down_ev);
  }
  else if ( md.isRightButtonReleased() )
  {
    FMouseEvent m_up_ev ( Event::MouseUp
                        , widgetMousePos
                        , mouse_position
                        , MouseButton::Right | key_state );
    auto released_widget = clicked_widget;

    if ( ! md.isLeftButtonPressed()
      && ! md.isMiddleButtonPressed() )
      setClickedWidget(nullptr);

    sendEvent (released_widget, &m_up_ev);
  }
}

//----------------------------------------------------------------------
void FApplication::sendMouseMiddleClickEvent ( const FMouseData& md
                                             , const FPoint& widgetMousePos
                                             , const FPoint& mouse_position
                                             , MouseButton key_state ) const
{
  if ( md.isMiddleButtonPressed() )
  {
    FMouseEvent m_down_ev ( Event::MouseDown
                          , widgetMousePos
                          , mouse_position
                          , MouseButton::Middle | key_state );
    sendEvent (clicked_widget, &m_down_ev);
  }
  else if ( md.isMiddleButtonReleased() )
  {
    FMouseEvent m_up_ev ( Event::MouseUp
                        , widgetMousePos
                        , mouse_position
                        , MouseButton::Middle | key_state );
    auto released_widget = clicked_widget;

    if ( ! md.isLeftButtonPressed()
      && ! md.isRightButtonPressed() )
    {
      setClickedWidget(nullptr);
    }

    sendEvent (released_widget, &m_up_ev);
  }
}

//----------------------------------------------------------------------
void FApplication::sendWheelEvent ( const FMouseData& md
                                  , const FPoint& widgetMousePos
                                  , const FPoint& mouse_position ) const
{
  if ( md.isWheelUp() )
  {
    FWheelEvent wheel_ev ( Event::MouseWheel
                         , widgetMousePos
                         , mouse_position
                         , MouseWheel::Up );
    auto scroll_over_widget = clicked_widget;
    setClickedWidget(nullptr);
    sendEvent(scroll_over_widget, &wheel_ev);
  }

  if ( md.isWheelDown() )
  {
    FWheelEvent wheel_ev ( Event::MouseWheel
                         , widgetMousePos
                         , mouse_position
                         , MouseWheel::Down );
    auto scroll_over_widget = clicked_widget;
    setClickedWidget(nullptr);
    sendEvent (scroll_over_widget, &wheel_ev);
  }
}

//----------------------------------------------------------------------
auto FApplication::processParameters (const Args& args) -> FWidget*
{
  if ( args.size() > 1 && (args[1] == "--help" || args[1] == "-h") )
  {
    showParameterUsage();
    FApplication::exit(EXIT_SUCCESS);
  }

  cmdOptions (args);
  return nullptr;
}

//----------------------------------------------------------------------
void FApplication::processResizeEvent()
{
  if ( ! has_terminal_resized )  // A SIGWINCH signal was received
    return;

  auto foutput_ptr = FVTerm::getFOutput();
  foutput_ptr->detectTerminalSize();  // Detect and save the current terminal size
  static auto& mouse = FMouseControl::getInstance();
  mouse.setMaxWidth (uInt16(getDesktopWidth()));
  mouse.setMaxHeight (uInt16(getDesktopHeight()));
  FResizeEvent r_ev(Event::Resize);
  sendEvent(internal::var::app_object, &r_ev);
  has_terminal_resized = false;

  if ( r_ev.isAccepted() )
    foutput_ptr->commitTerminalResize();
}

//----------------------------------------------------------------------
void FApplication::processDialogResizeMove() const
{
  for (auto&& dialog : *FWidget::getDialogList())
    dialog->flushChanges();
}

//----------------------------------------------------------------------
void FApplication::processCloseWidget()
{
  if ( ! getWidgetCloseList() || getWidgetCloseList()->empty() )
    return;

  setTerminalUpdates (FVTerm::TerminalUpdate::Stop);
  auto iter = getWidgetCloseList()->cbegin();

  while ( iter != getWidgetCloseList()->cend() && *iter )
  {
    delete *iter;
    ++iter;
  }

  getWidgetCloseList()->clear();
  setTerminalUpdates (FVTerm::TerminalUpdate::Start);
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
auto FApplication::processNextEvent() -> bool
{
  uInt num_events{0};

  if ( hasDataInQueue() || hasTerminalResized() || isNextEventTimeout() )
  {
    time_last_event = FObjectTimer::getCurrentTime();
    num_events += processTimerEvent();
    processInput();
    processResizeEvent();
    processCloseWidget();
    sendQueuedEvents();
    processDialogResizeMove();
    processTerminalUpdate();  // after terminal changes
    flush();
    processLogger();
  }
  else
  {
    if ( isKeyPressed(next_event_wait) )
      time_last_event = TimeValue{};
  }

  processExternalUserEvent();
  return ( num_events > 0 );
}

//----------------------------------------------------------------------
void FApplication::performTimerAction (FObject* receiver, FEvent* event)
{
  sendEvent (receiver, event);
}

//----------------------------------------------------------------------
inline auto FApplication::hasTerminalResized() -> bool
{
  auto foutput_ptr = FVTerm::getFOutput();
  has_terminal_resized = foutput_ptr->hasTerminalResized();
  return has_terminal_resized;
}

//----------------------------------------------------------------------
auto FApplication::isEventProcessable ( FObject* receiver
                                      , const FEvent* event ) -> bool
{
  if ( ! receiver->isWidget() )  // No restrictions for non-widgets
    return true;

  auto widget = static_cast<FWidget*>(receiver);

  if ( getModalDialogCounter() > 0 )
  {
    const FWidget* window;

    if ( widget->isWindowWidget() )
      window = widget;
    else
      window = FWindow::getWindowWidget(widget);

    // block events for widgets in non modal windows
    if ( window
      && ! window->getFlags().visibility.modal
      && ! window->isMenuWidget() )
    {
      constexpr std::array<const Event, 13> blocked_events
      {{
        Event::KeyPress,
        Event::KeyUp,
        Event::KeyDown,
        Event::MouseDown,
        Event::MouseUp,
        Event::MouseDoubleClick,
        Event::MouseWheel,
        Event::MouseMove,
        Event::FocusIn,
        Event::FocusOut,
        Event::ChildFocusIn,
        Event::ChildFocusOut,
        Event::Accelerator
      }};

      if ( std::any_of( blocked_events.cbegin()
                      , blocked_events.cend()
                      , [&event](const auto& ev)
                        {
                          return ev == event->getType();
                        } ) )
      {
        return false;
      }
    }
  }

  // Throw away mouse events for disabled widgets
  return ( event->getType() < Event::MouseDown
        || event->getType() > Event::MouseMove
        || widget->isEnabled() );
}

//----------------------------------------------------------------------
auto FApplication::isNextEventTimeout() -> bool
{
  return FObjectTimer::isTimeout(time_last_event, next_event_wait);
}


// Friend functions definition of FEvent
//----------------------------------------------------------------------
void setSend (FEvent& event, bool state)
{
  event.send = state;
}

//----------------------------------------------------------------------
void setQueued (FEvent& event, bool state)
{
  event.queued = state;
}


// FLog non-member operators
//----------------------------------------------------------------------
auto operator << (std::ostream& outstr, FLog::LogLevel l) -> std::ostream&
{
  try
  {
    *FApplication::getLog() << l;
  }
  catch (const std::invalid_argument&)
  {
    try
    {
      *FApplication::getLog() << FLog::LogLevel::Info;
    }
    catch (const std::invalid_argument&)  // Avoid being thrown again
    {
      *FApplication::getLog();
    }
  }

  return outstr;
}

}  // namespace finalcut
