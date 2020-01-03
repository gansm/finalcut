/***********************************************************************
* fapplication.cpp - Manages the application events                    *
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

#include <memory>
#include <string>

#include "final/fapplication.h"
#include "final/fevent.h"
#include "final/fmenu.h"
#include "final/fmenubar.h"
#include "final/fmessagebox.h"
#include "final/fmouse.h"
#include "final/fstartoptions.h"
#include "final/fstatusbar.h"
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
int            FApplication::loop_level      {0};  // event loop level
int            FApplication::quit_code       {0};
bool           FApplication::quit_now        {false};

FApplication::eventQueue* FApplication::event_queue{nullptr};


//----------------------------------------------------------------------
// class FApplication
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FApplication::FApplication ( const int& _argc
                           , char* _argv[]
                           , bool disable_alt_screen )
  : FWidget(processParameters(_argc, _argv), disable_alt_screen)
  , app_argc{_argc}
  , app_argv{_argv}
{
  if ( app_object )
    throw std::runtime_error( "FApplication: There should be "
                              "only one application object" );

  app_object = this;

  if ( ! (_argc && _argv) )
  {
    static char* empty{C_STR("")};
    app_argc = 0;
    app_argv = static_cast<char**>(&empty);
  }

  init (key_timeout, dblclick_interval);
}

//----------------------------------------------------------------------
FApplication::~FApplication()  // destructor
{
  if ( event_queue )
    delete event_queue;

  app_object = nullptr;
}


// public methods of FApplication
//----------------------------------------------------------------------
FApplication* FApplication::getApplicationObject()
{
  return app_object;
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
    return EXIT_FAILURE;
  }

  quit_code = 0;
  enterLoop();
  return quit_code;
}

//----------------------------------------------------------------------
int FApplication::enterLoop()  // event loop
{
  loop_level++;
  quit_now = false;

  bool old_app_exit_loop = app_exit_loop;
  app_exit_loop = false;

  while ( ! (quit_now || app_exit_loop) )
    processNextEvent();

  app_exit_loop = old_app_exit_loop;
  loop_level--;
  return 0;
}

//----------------------------------------------------------------------
void FApplication::exitLoop()
{
  app_exit_loop = true;
}

//----------------------------------------------------------------------
void FApplication::exit (int retcode)
{
  if ( ! app_object )  // no global app object
    return;

  if ( quit_now )  // don't overwrite quit code
    return;

  quit_now  = true;
  quit_code = retcode;
}

//----------------------------------------------------------------------
void FApplication::quit()
{
  FApplication::exit(0);
}

//----------------------------------------------------------------------
bool FApplication::sendEvent ( const FObject* receiver
                             , const FEvent* event )
{
  if ( quit_now || app_exit_loop || ! receiver )
    return false;

  if ( receiver->isWidget() )
  {
    const auto r_widget = static_cast<const FWidget*>(receiver);
    auto widget = const_cast<FWidget*>(r_widget);

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
        switch ( uInt(event->type()) )
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
          case fc::Accelerator_Event:
            return false;

          default:
            break;
        }
      }
    }

    // Throw away mouse events for disabled widgets
    if ( event->type() >= fc::MouseDown_Event
      && event->type() <= fc::MouseMove_Event
      && ! widget->isEnabled() )
      return false;
  }

  // Sends event event directly to receiver
  auto r = const_cast<FObject*>(receiver);
  return r->event(const_cast<FEvent*>(event));
}

//----------------------------------------------------------------------
void FApplication::queueEvent ( const FObject* receiver
                              , const FEvent* event )
{
  if ( ! receiver )
    return;

  // queue this event
  eventPair send_event (receiver, std::make_shared<const FEvent>(*event));
  event_queue->push_back(send_event);
}

//----------------------------------------------------------------------
void FApplication::sendQueuedEvents()
{
  while ( eventInQueue() )
  {
    sendEvent( event_queue->front().first,
               event_queue->front().second.get() );
    event_queue->pop_front();
  }
}

//----------------------------------------------------------------------
bool FApplication::eventInQueue()
{
  if ( app_object )
    return ( ! event_queue->empty() );
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
  auto iter = event_queue->begin();

  while ( iter != event_queue->end() )
  {
    if ( iter->first == receiver )
    {
      iter = event_queue->erase(iter);
      retval = true;
    }
    else
      ++iter;
  }

  return retval;
}

//----------------------------------------------------------------------
FWidget* FApplication::processParameters (const int& argc, char* argv[])
{
  if ( argc > 0 && argv[1] && ( std::strcmp(argv[1], "--help") == 0
                             || std::strcmp(argv[1], "-h") == 0 ) )
  {
    showParameterUsage();
  }

  getStartOptions().setDefault();
  cmd_options (argc, argv);
  return 0;
}

//----------------------------------------------------------------------
void FApplication::showParameterUsage()
{
  std::cout \
    << "Generic options:\n"
    << "  -h, --help           "
    << "       Display this help and exit\n"
    << "\n"
    << "The Final Cut options:\n"
    << "  --encoding <name>    "
    << "       Sets the character encoding mode\n"
    << "                       "
    << "       {utf8, vt100, pc, ascii}\n"
    << "  --no-mouse             "
    << "     Disable mouse support\n"
    << "  --no-optimized-cursor  "
    << "     Disable cursor optimization\n"
    << "  --no-terminal-detection"
    << "     Disable terminal detection\n"
    << "  --no-color-change      "
    << "     Do not redefine the color palette\n"
    << "  --no-sgr-optimizer     "
    << "     Do not optimize SGR sequences\n"
    << "  --vgafont              "
    << "     Set the standard vga 8x16 font\n"
    << "  --newfont              "
    << "     Enables the graphical font\n"

#if defined(__FreeBSD__) || defined(__DragonFly__)
    << "\n"
    << "FreeBSD console options:\n"
    << "  --no-esc-for-alt-meta  "
    << "     Do not send a ESC prefix for the alt/meta key\n"
    << "  --no-cursorstyle-change"
    << "     Do not change the current cursor style\n"
#elif defined(__NetBSD__) || defined(__OpenBSD__)
    << "\n"
    << "NetBSD/OpenBSD console options:\n"
    << "  --no-esc-for-alt-meta  "
    << "     Do not send a ESC prefix for the alt/meta key\n"
#endif

    << std::endl;  // newline character + flushes the output stream
  std::exit(EXIT_SUCCESS);
}

//----------------------------------------------------------------------
void FApplication::closeConfirmationDialog (FWidget* w, FCloseEvent* ev)
{
  int ret = FMessageBox::info ( w, "Quit"
                              , "Do you really want\n"
                                "to quit the program ?"
                              , FMessageBox::Yes
                              , FMessageBox::No );
  if ( ret == FMessageBox::Yes )
    ev->accept();
  else
    ev->ignore();
}

// private methods of FApplication
//----------------------------------------------------------------------
void FApplication::init (uInt64 key_time, uInt64 dblclick_time)
{
  // Initialize keyboard
  keyboard = FVTerm::getFKeyboard();

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
    keyboard->setKeypressTimeout (key_time);
  }

  // Initialize mouse control
  mouse = FVTerm::getFMouseControl();

  // Set stdin number for a gpm-mouse
  if ( mouse )
    mouse->setStdinNo (FTermios::getStdIn());

  // Set the default double click interval
  if ( mouse )
    mouse->setDblclickInterval (dblclick_time);

  try
  {
    event_queue = new eventQueue;
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
    std::abort();
  }
}

//----------------------------------------------------------------------
void FApplication::cmd_options (const int& argc, char* argv[])
{
  // Interpret the command line options

  while ( true )
  {
    static struct option long_options[] =
    {
      {C_STR("encoding"),              required_argument, 0,  0 },
      {C_STR("no-mouse"),              no_argument,       0,  0 },
      {C_STR("no-optimized-cursor"),   no_argument,       0,  0 },
      {C_STR("no-terminal-detection"), no_argument,       0,  0 },
      {C_STR("no-color-change"),       no_argument,       0,  0 },
      {C_STR("no-sgr-optimizer"),      no_argument,       0,  0 },
      {C_STR("vgafont"),               no_argument,       0,  0 },
      {C_STR("newfont"),               no_argument,       0,  0 },

    #if defined(__FreeBSD__) || defined(__DragonFly__)
      {C_STR("no-esc-for-alt-meta"),   no_argument,       0,  0 },
      {C_STR("no-cursorstyle-change"), no_argument,       0,  0 },
    #elif defined(__NetBSD__) || defined(__OpenBSD__)
      {C_STR("no-esc-for-alt-meta"),   no_argument,       0,  0 },
    #endif

      {nullptr,                        0,           nullptr,  0 }
    };

    opterr = 0;
    int idx{0};
    int c = getopt_long (argc, argv, "", long_options, &idx);

    if ( c == -1 )
      break;

    if ( c == 0 )
    {
      if ( std::strcmp(long_options[idx].name, "encoding") == 0 )
      {
        FString encoding(optarg);
        encoding = encoding.toLower();

        if ( encoding.includes("utf8") )
          getStartOptions().encoding = fc::UTF8;
        else if ( encoding.includes("vt100") )
          getStartOptions().encoding = fc::VT100;
        else if ( encoding.includes("pc") )
          getStartOptions().encoding = fc::PC;
        else if ( encoding.includes("ascii") )
          getStartOptions().encoding = fc::ASCII;
        else if ( encoding.includes("help") )
          showParameterUsage();
        else
          exitWithMessage ( "Unknown encoding "
                          + std::string(encoding.c_str()) );
      }

      if ( std::strcmp(long_options[idx].name, "no-mouse")  == 0 )
        getStartOptions().mouse_support = false;

      if ( std::strcmp(long_options[idx].name, "no-optimized-cursor")  == 0 )
        getStartOptions().cursor_optimisation = false;

      if ( std::strcmp(long_options[idx].name, "no-terminal-detection")  == 0 )
        getStartOptions().terminal_detection = false;

      if ( std::strcmp(long_options[idx].name, "no-color-change")  == 0 )
        getStartOptions().color_change = false;

      if ( std::strcmp(long_options[idx].name, "no-sgr-optimizer")  == 0 )
        getStartOptions().sgr_optimizer = false;

      if ( std::strcmp(long_options[idx].name, "vgafont")  == 0 )
        getStartOptions().vgafont = true;

      if ( std::strcmp(long_options[idx].name, "newfont")  == 0 )
        getStartOptions().newfont = true;

    #if defined(__FreeBSD__) || defined(__DragonFly__)
      if ( std::strcmp(long_options[idx].name, "no-esc-for-alt-meta")  == 0 )
        getStartOptions().meta_sends_escape = false;

      if ( std::strcmp(long_options[idx].name, "no-cursorstyle-change")  == 0 )
        getStartOptions().change_cursorstyle = false;
    #elif defined(__NetBSD__) || defined(__OpenBSD__)
      if ( std::strcmp(long_options[idx].name, "no-esc-for-alt-meta")  == 0 )
        getStartOptions().meta_sends_escape = false;
    #endif
    }
  }
}

//----------------------------------------------------------------------
inline FStartOptions& FApplication::getStartOptions()
{
  return FStartOptions::getFStartOptions();
}

//----------------------------------------------------------------------
inline void FApplication::findKeyboardWidget()
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
inline bool FApplication::isKeyPressed()
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
void FApplication::keyReleased()
{
  sendKeyUpEvent (keyboard_widget);
}

//----------------------------------------------------------------------
void FApplication::escapeKeyPressed()
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
      bool acceptKeyDown = sendKeyDownEvent (keyboard_widget);
      bool acceptKeyPress = sendKeyPressEvent (keyboard_widget);

      if ( ! (acceptKeyDown || acceptKeyPress) )
        sendKeyboardAccelerator();

      break;
  }
}

//----------------------------------------------------------------------
inline void FApplication::sendEscapeKeyPressEvent()
{
  // Send an escape key press event
  FKeyEvent k_press_ev (fc::KeyPress_Event, fc::Fkey_escape);
  sendEvent (keyboard_widget, &k_press_ev);
}

//----------------------------------------------------------------------
inline bool FApplication::sendKeyDownEvent (FWidget* widget)
{
  // Send key down event
  FKeyEvent k_down_ev (fc::KeyDown_Event, keyboard->getKey());
  sendEvent (widget, &k_down_ev);
  return k_down_ev.isAccepted();
}

//----------------------------------------------------------------------
inline bool FApplication::sendKeyPressEvent (FWidget* widget)
{
  // Send key press event
  FKeyEvent k_press_ev (fc::KeyPress_Event, keyboard->getKey());
  sendEvent (widget, &k_press_ev);
  return k_press_ev.isAccepted();
}

//----------------------------------------------------------------------
inline bool FApplication::sendKeyUpEvent (FWidget* widget)
{
  // Send key up event
  FKeyEvent k_up_ev (fc::KeyUp_Event, keyboard->getKey());
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
void FApplication::processKeyboardEvent()
{
  if ( quit_now || app_exit_loop )
    return;

  findKeyboardWidget();
  flush();
  keyboard->clearKeyBufferOnTimeout();

  if ( isKeyPressed() )
    keyboard->fetchKeyCode();

  // special case: Esc key
  keyboard->escapeKeyHandling();
}

//----------------------------------------------------------------------
bool FApplication::processDialogSwitchAccelerator()
{
  if ( keyboard->getKey() >= fc::Fmkey_1
    && keyboard->getKey() <= fc::Fmkey_9 )
  {
    FKey key = keyboard->getKey();
    std::size_t n = key - fc::Fmkey_0;
    std::size_t s = getDialogList()->size();

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
bool FApplication::processAccelerator (const FWidget*& widget)
{
  bool accpt{false};

  if ( widget
    && ! widget->getAcceleratorList().empty() )
  {
    auto iter = widget->getAcceleratorList().begin();
    auto last = widget->getAcceleratorList().end();

    while ( iter != last )
    {
      if ( quit_now || app_exit_loop )
        break;

      if ( iter->key == keyboard->getKey() )
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
        sendEvent (iter->object, &a_ev);
        accpt = a_ev.isAccepted();
        break;
      };

      ++iter;
    }
  }

  return accpt;
}

//----------------------------------------------------------------------
bool FApplication::getMouseEvent()
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

  const auto& mouse_position = mouse->getPos();

  // Determine the window object on the current click position
  auto window = FWindow::getWindowWidgetAt (mouse_position);

  if ( window )
  {
    // Determine the widget at the current click position
    auto child = window->childWidgetAt (mouse_position);
    clicked = ( child != 0 ) ? child : window;
    setClickedWidget (clicked);
  }

  return clicked;
}

//----------------------------------------------------------------------
void FApplication::unsetMoveSizeMode()
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
void FApplication::closeDropDown()
{
  // Close the open menu

  if ( ! mouse || mouse->isMoved() )
    return;

  const auto& mouse_position = mouse->getPos();
  finalcut::closeDropDown (this, mouse_position);
}

//----------------------------------------------------------------------
void FApplication::unselectMenubarItems()
{
  // Unselect the menu bar items

  auto openmenu = FWidget::getOpenMenu();
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
void FApplication::sendMouseEvent()
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

  auto widgetMousePos = clicked->termToWidgetPos(mouse_position);

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
                                      , int key_state )
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
                                           , int key_state )
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
                                            , int key_state )
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
                                             , int key_state )
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
    if ( isGnomeTerminal() )
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
                                  , const FPoint& mouse_position )
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
void FApplication::processResizeEvent()
{
  if ( ! hasChangedTermSize() )
    return;

  FResizeEvent r_ev(fc::Resize_Event);
  sendEvent(app_object, &r_ev);

  if ( r_ev.isAccepted() )
    changeTermSizeFinished();
}

//----------------------------------------------------------------------
void FApplication::processCloseWidget()
{
  updateTerminal (FVTerm::stop_refresh);

  if ( getWidgetCloseList() && ! getWidgetCloseList()->empty() )
  {
    auto iter = getWidgetCloseList()->begin();

    while ( iter != getWidgetCloseList()->end() && *iter )
    {
      delete *iter;
      ++iter;
    }

    getWidgetCloseList()->clear();
  }

  updateTerminal (FVTerm::start_refresh);
}

//----------------------------------------------------------------------
bool FApplication::processNextEvent()
{
  uInt num_events{0};

  processKeyboardEvent();
  processMouseEvent();
  processResizeEvent();
  processTerminalUpdate();
  processCloseWidget();

  sendQueuedEvents();
  num_events += processTimerEvent();

  return ( num_events > 0 );
}

//----------------------------------------------------------------------
void FApplication::performTimerAction ( const FObject* receiver
                                      , const FEvent* event )
{
  sendEvent (receiver, event);
}

}  // namespace finalcut
