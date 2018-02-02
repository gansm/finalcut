/***********************************************************************
* fapplication.cpp - Manages the application events                    *
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

#include <string>

#include "final/fapplication.h"
#include "final/fmenu.h"
#include "final/fmessagebox.h"
#include "final/fstatusbar.h"
#include "final/fwindow.h"

// global application object
static FApplication* rootObj = 0;

// flag to exit local loop
static bool app_exit_loop = false;

// static attributes
int            FApplication::loop_level       = 0;  // event loop level
FWidget*       FApplication::main_widget      = 0;  // main application widget
FWidget*       FApplication::active_window    = 0;  // the active window
FWidget*       FApplication::focus_widget     = 0;  // has keyboard input focus
FWidget*       FApplication::clicked_widget   = 0;  // is focused by click
FWidget*       FApplication::open_menu        = 0;  // currently open menu
FWidget*       FApplication::move_size_widget = 0;  // move/size by keyboard
FMouseControl* FApplication::mouse            = 0;  // mouse control
int            FApplication::quit_code        = 0;
bool           FApplication::quit_now         = false;

FApplication::eventQueue*    FApplication::event_queue = 0;


//----------------------------------------------------------------------
// class FApplication
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FApplication::FApplication ( const int& _argc
                           , char* _argv[]
                           , bool disable_alt_screen )
  : FWidget(processParameters(_argc,_argv), disable_alt_screen)
  , app_argc(_argc)
  , app_argv(_argv)
  , key(0)
  , fifo_offset(0)
  , fifo_in_use(false)
  , fifo_buf_size(sizeof(fifo_buf))
  , key_timeout(100000)        // 100 ms
  , dblclick_interval(500000)  // 500 ms
  , time_keypressed()
{
  assert ( ! rootObj
        && "FApplication: There should be only one application object" );
  rootObj = this;

  if ( ! (_argc && _argv) )
  {
    static char* empty = C_STR("");
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

  rootObj = 0;
}


// public methods of FApplication
//----------------------------------------------------------------------
void FApplication::setMainWidget (FWidget* widget)
{
  main_widget = widget;

  if ( widget && ! getFocusWidget() )
    rootObj->focusFirstChild();
}

//----------------------------------------------------------------------
bool FApplication::isQuit()
{
  if ( rootObj )
    return quit_now;
  else
    return true;
}

//----------------------------------------------------------------------
int FApplication::exec()  // run
{
  if ( quit_now )
    return EXIT_FAILURE;

  quit_now = false;
  quit_code = 0;

  enter_loop();
  return quit_code;
}

//----------------------------------------------------------------------
int FApplication::enter_loop()  // event loop
{
  bool old_app_exit_loop;
  loop_level++;
  quit_now = false;

  old_app_exit_loop = app_exit_loop;
  app_exit_loop = false;

  while ( ! (quit_now || app_exit_loop) )
    processNextEvent();

  app_exit_loop = old_app_exit_loop;
  loop_level--;
  return 0;
}

//----------------------------------------------------------------------
void FApplication::exit_loop()
{
  app_exit_loop = true;
}

//----------------------------------------------------------------------
void FApplication::exit (int retcode)
{
  if ( ! rootObj )  // no global app object
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
  if ( quit_now || app_exit_loop )
    return false;

  if ( ! receiver )
    return false;

  if ( ! receiver->isWidget() )
    return false;

  const FWidget* r_widget = static_cast<const FWidget*>(receiver);
  FWidget* widget = const_cast<FWidget*>(r_widget);

  if ( modal_dialogs > 0 )
  {
    const FWidget* window;
    if ( widget->isWindowWidget() )
      window = widget;
    else
      window = FWindow::getWindowWidget(widget);

    // block events for widgets in non modal windows
    if ( window
      && (window->getFlags() & fc::modal) == 0
      && ! window->isMenuWidget() )
    {
      switch ( event->type() )
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

  // For access to a protected base class member
  const FApplication* const_w = static_cast<const FApplication*>(widget);
  FApplication* w = const_cast<FApplication*>(const_w);

  // Sends event event directly to receiver
  return w->event(const_cast<FEvent*>(event));
}

//----------------------------------------------------------------------
void FApplication::queueEvent ( const FObject* receiver
                              , const FEvent* event )
{
  if ( ! receiver )
    return;

  // queue this event
  eventPair Event (receiver, event);
  event_queue->push_back(Event);
}

//----------------------------------------------------------------------
void FApplication::sendQueuedEvents()
{
  eventQueue* events;

  if ( ! eventInQueue() )
    return;

  events = event_queue;

  while ( ! eventInQueue() )
  {
    sendEvent(events->front().first, events->front().second);
    events->pop_front();
  }
}

//----------------------------------------------------------------------
bool FApplication::eventInQueue()
{
  if ( rootObj )
    return ( ! event_queue->empty() );
  else
    return false;
}

//----------------------------------------------------------------------
bool FApplication::removeQueuedEvent (const FObject* receiver)
{
  bool retval;
  eventQueue::iterator iter;

  if ( ! eventInQueue() )
    return false;

  if ( ! receiver )
    return false;

  retval = false;
  iter = event_queue->begin();

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

  init_values.setDefault();
  cmd_options (argc, argv);
  return 0;
}

//----------------------------------------------------------------------
void FApplication::showParameterUsage()
{
  std::cout \
    << "Generic options:" << std::endl
    << "  -h, --help           "
    << "       Display this help and exit" << std::endl
    << std::endl
    << "FinalCut Options:" << std::endl
    << "  --encoding <name>    "
    << "       Sets the character encoding mode" << std::endl
    << "                       "
    << "       {utf8, vt100, pc, ascii}" << std::endl
    << "  --no-optimized-cursor  "
    << "     Disable cursor optimization" << std::endl
    << "  --no-terminal-detection"
    << "     Disable terminal detection" << std::endl
    << "  --no-color-change      "
    << "     Do not redefine the color palette" << std::endl
    << "  --vgafont              "
    << "     Set the standard vga 8x16 font" << std::endl
    << "  --newfont              "
    << "     Enables the graphical font" << std::endl;
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
void FApplication::init (long key_timeout, long dblclick_interval)
{
  // Initialize keyboard values
  time_keypressed.tv_sec = 0;
  time_keypressed.tv_usec = 0;

  // Set the keyboard keypress timeout
  setKeypressTimeout (key_timeout);

  // Initialize mouse control
  mouse = getMouseControl();

  // Set stdin number for a gpm-mouse
  if ( mouse )
    mouse->setStdinNo (stdin_no);

  // Set the default double click interval
  if ( mouse )
    mouse->setDblclickInterval (dblclick_interval);

  try
  {
    event_queue = new eventQueue;
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
    std::abort();
  }

  // Initialize arrays with '\0'
  std::fill_n (k_buf, sizeof(k_buf), '\0');
  std::fill_n (fifo_buf, fifo_buf_size, '\0');
}

//----------------------------------------------------------------------
void FApplication::cmd_options (const int& argc, char* argv[])
{
  // Interpret the command line options

  while ( true )
  {
    int c, idx = 0;

    static struct option long_options[] =
    {
      {C_STR("encoding"),              required_argument, 0,  0 },
      {C_STR("no-optimized-cursor"),   no_argument,       0,  0 },
      {C_STR("no-terminal-detection"), no_argument,       0,  0 },
      {C_STR("no-color-change"),       no_argument,       0,  0 },
      {C_STR("vgafont"),               no_argument,       0,  0 },
      {C_STR("newfont"),               no_argument,       0,  0 },
      {0,                              0,                 0,  0 }
    };

    opterr = 0;
    c = getopt_long ( argc
                    , argv
                    , ""
                    , long_options
                    , &idx );
    if ( c == -1 )
      break;

    if ( c == 0 )
    {
      if ( std::strcmp(long_options[idx].name, "encoding") == 0 )
      {
        FString encoding(optarg);
        encoding = encoding.toLower();

        if ( encoding.includes("utf8") )
          init_values.encoding = fc::UTF8;
        else if ( encoding.includes("vt100") )
          init_values.encoding = fc::VT100;
        else if ( encoding.includes("pc") )
          init_values.encoding = fc::PC;
        else if ( encoding.includes("ascii") )
          init_values.encoding = fc::ASCII;
        else if ( encoding.includes("help") )
          showParameterUsage();
        else
          exitWithMessage ( "Unknown encoding "
                          + std::string(encoding.c_str()) );
      }

      if ( std::strcmp(long_options[idx].name, "no-optimized-cursor")  == 0 )
        init_values.cursor_optimisation = false;

      if ( std::strcmp(long_options[idx].name, "no-terminal-detection")  == 0 )
        init_values.terminal_detection = false;

      if ( std::strcmp(long_options[idx].name, "no-color-change")  == 0 )
        init_values.color_change = false;

      if ( std::strcmp(long_options[idx].name, "vgafont")  == 0 )
        init_values.vgafont = true;

      if ( std::strcmp(long_options[idx].name, "newfont")  == 0 )
        init_values.newfont = true;
    }
  }
}

//----------------------------------------------------------------------
inline bool FApplication::KeyPressed()
{
  register int result;
  fd_set ifds;
  struct timeval tv;

  FD_ZERO(&ifds);
  FD_SET(stdin_no, &ifds);
  tv.tv_sec  = 0;
  tv.tv_usec = 100000;  // 100 ms
  result = select (stdin_no + 1, &ifds, 0, 0, &tv);

  if ( result > 0 && FD_ISSET(stdin_no, &ifds) )
    FD_CLR (stdin_no, &ifds);

  return ( result > 0 );
}

//----------------------------------------------------------------------
inline ssize_t FApplication::readKey()
{
  register ssize_t bytes;
  setNonBlockingInput();
  bytes = read(stdin_no, &k_buf, sizeof(k_buf) - 1);
  unsetNonBlockingInput();
  return bytes;
}

//----------------------------------------------------------------------
inline FWidget* FApplication::findKeyboardWidget()
{
  FWidget* widget = 0;

  if ( focus_widget )
  {
    if ( move_size_widget )
      widget = move_size_widget;
    else
      widget = focus_widget;
  }
  else
  {
    widget = main_widget;

    if ( widget && widget->numOfChildren() >= 1 )
      widget->focusFirstChild();
  }

  return widget;
}

//----------------------------------------------------------------------
inline bool FApplication::getKeyPressedState()
{
  if ( mouse && mouse->isGpmMouseEnabled() )
    return mouse->getGpmKeyPressed(unprocessedInput());

  return KeyPressed();
}

//----------------------------------------------------------------------
inline void FApplication::keyboardBufferTimeout (FWidget*)
{
  // Empty the buffer on timeout
  if ( fifo_in_use && isKeypressTimeout(&time_keypressed)  )
  {
    fifo_offset = 0;
    key = 0;
    std::fill_n (fifo_buf, fifo_buf_size, '\0');
    fifo_in_use = false;
  }
}

//----------------------------------------------------------------------
inline void FApplication::parseKeyPuffer (FWidget* widget)
{
  register ssize_t bytesread;
  getCurrentTime (&time_keypressed);

  if ( quit_now || app_exit_loop )
    return;

  while ( (bytesread = readKey()) > 0 )
  {
    if ( bytesread + fifo_offset <= fifo_buf_size )
    {
      for (int i = 0; i < bytesread; i++)
      {
        fifo_buf[fifo_offset] = k_buf[i];
        fifo_offset++;
      }

      fifo_in_use = true;
    }

    // Read the rest from the fifo buffer
    while ( ! widget->isKeypressTimeout(&time_keypressed)
         && fifo_offset > 0
         && key != NEED_MORE_DATA )
    {
      key = FTerm::parseKeyString(fifo_buf, fifo_buf_size, &time_keypressed);

      if ( key != NEED_MORE_DATA )
        performKeyboardAction (widget);

      fifo_offset = int(std::strlen(fifo_buf));
    }

    // Send key up event
    sendKeyUpEvent (widget);
    key = 0;
  }

  std::fill_n (k_buf, sizeof(k_buf), '\0');
}

//----------------------------------------------------------------------
inline void FApplication::performKeyboardAction (FWidget* widget)
{
#if defined(__linux__)
  key = linuxModifierKeyCorrection (key);
#endif

  switch ( key )
  {
    case fc::Fckey_l:  // Ctrl-L (redraw the screen)
      redraw();
      break;

    case fc::Fkey_mouse:
      if ( mouse )
      {
        mouse->setRawData (FMouse::x11, fifo_buf, sizeof(fifo_buf));
        unprocessedInput() = mouse->isInputDataPending();
        processMouseEvent();
      }
      break;

    case fc::Fkey_extended_mouse:
      if ( mouse )
      {
        mouse->setRawData (FMouse::sgr, fifo_buf, sizeof(fifo_buf));
        unprocessedInput() = mouse->isInputDataPending();
        processMouseEvent();
      }
      break;

    case fc::Fkey_urxvt_mouse:
      if ( mouse )
      {
        mouse->setRawData (FMouse::urxvt, fifo_buf, sizeof(fifo_buf));
        unprocessedInput() = mouse->isInputDataPending();
        processMouseEvent();
      }
      break;

    default:
      bool acceptKeyDown = sendKeyDownEvent (widget);
      bool acceptKeyPress = sendKeyPressEvent (widget);

      if ( ! (acceptKeyDown || acceptKeyPress) )
        sendKeyboardAccelerator();

      break;
  }
}

//----------------------------------------------------------------------
inline void FApplication::sendEscapeKeyPressEvent (FWidget* widget)
{
  // Send an escape key press event if there is only one 0x1b
  // in the buffer and the timeout is reached

  if ( fifo_in_use
    && fifo_offset == 1
    && fifo_buf[0] == 0x1b
    && fifo_buf[1] == 0x00
    && isKeypressTimeout(&time_keypressed) )
  {
    FKeyEvent k_press_ev (fc::KeyPress_Event, fc::Fkey_escape);
    sendEvent (widget, &k_press_ev);
    unprocessedInput() = false;
  }
}

//----------------------------------------------------------------------
inline bool FApplication::sendKeyDownEvent (FWidget* widget)
{
  // Send key down event
  FKeyEvent k_down_ev (fc::KeyDown_Event, key);
  sendEvent (widget, &k_down_ev);
  return k_down_ev.isAccepted();
}

//----------------------------------------------------------------------
inline bool FApplication::sendKeyPressEvent (FWidget* widget)
{
  // Send key press event
  FKeyEvent k_press_ev (fc::KeyPress_Event, key);
  sendEvent (widget, &k_press_ev);
  return k_press_ev.isAccepted();
}

//----------------------------------------------------------------------
inline bool FApplication::sendKeyUpEvent (FWidget* widget)
{
  // Send key up event
  FKeyEvent k_up_ev (fc::KeyUp_Event, key);
  sendEvent (widget, &k_up_ev);
  return k_up_ev.isAccepted();
}

//----------------------------------------------------------------------
inline void FApplication::sendKeyboardAccelerator()
{
  if ( open_menu )
    return;

  // Switch to a specific dialog with Meta + 1..9
  bool accpt = processDialogSwitchAccelerator();

  // Windows keyboard accelerator
  if ( ! accpt )
  {
    const FWidget* window = active_window;

    if ( window )
      accpt = processAccelerator (window);
  }

  // Global keyboard accelerator
  if ( ! accpt )
  {
    const FWidget* root_widget = getRootWidget();

    if ( root_widget )
      processAccelerator (root_widget);
  }
}

//----------------------------------------------------------------------
void FApplication::processKeyboardEvent()
{
  FWidget* widget = findKeyboardWidget();
  keyboardBufferTimeout(widget);
  flush_out();
  bool isKeyPressed = getKeyPressedState();

  if ( isKeyPressed )
    parseKeyPuffer (widget);

  // special case: Esc key
  sendEscapeKeyPressEvent (widget);
}

#if defined(__linux__)
//----------------------------------------------------------------------
int FApplication::linuxShiftKeyCorrection (const int& key_id)
{
  switch ( key_id )
  {
    case fc::Fkey_up:
      return fc::Fkey_sr;         // Shift+Up

    case fc::Fkey_down:
      return fc::Fkey_sf;         // Shift+Down

    case fc::Fkey_left:
      return fc::Fkey_sleft;      // Shift+Left

    case fc::Fkey_right:
      return fc::Fkey_sright;     // Shift+Right

    case fc::Fkey_ic:
      return fc::Fkey_sic;        // Shift+Ins

    case fc::Fkey_dc:
      return fc::Fkey_sdc;        // Shift+Del

    case fc::Fkey_home:
      return fc::Fkey_shome;      // Shift+Home

    case fc::Fkey_end:
      return fc::Fkey_send;       // Shift+End

    case fc::Fkey_ppage:
      return fc::Fkey_sprevious;  // Shift+PgUp

    case fc::Fkey_npage:
      return fc::Fkey_snext;      // Shift+PgDn

    default:
      return key_id;
  }
}

//----------------------------------------------------------------------
int FApplication::linuxCtrlKeyCorrection (const int& key_id)
{
  switch ( key_id )
  {
    case fc::Fkey_up:
      return fc::Fckey_up;     // Ctrl+Up

    case fc::Fkey_down:
      return fc::Fckey_down;   // Ctrl+Down

    case fc::Fkey_left:
      return fc::Fckey_left;   // Ctrl+Left

    case fc::Fkey_right:
      return fc::Fckey_right;  // Ctrl+Right

    case fc::Fkey_ic:
      return fc::Fckey_ic;     // Ctrl+Ins

    case fc::Fkey_dc:
      return fc::Fckey_dc;     // Ctrl+Del

    case fc::Fkey_home:
      return fc::Fckey_home;   // Ctrl+Home

    case fc::Fkey_end:
      return fc::Fckey_end;    // Ctrl+End

    case fc::Fkey_ppage:
      return fc::Fckey_ppage;  // Ctrl+PgUp

    case fc::Fkey_npage:
      return fc::Fckey_npage;  // Ctrl+PgDn

    default:
      return key_id;
  }
}

//----------------------------------------------------------------------
int FApplication::linuxAltKeyCorrection (const int& key_id)
{
  switch ( key_id )
  {
    case fc::Fkey_up:
      return fc::Fmkey_up;     // Meta+Up

    case fc::Fkey_down:
      return fc::Fmkey_down;   // Meta+Down

    case fc::Fkey_left:
      return fc::Fmkey_left;   // Meta+Left

    case fc::Fkey_right:
      return fc::Fmkey_right;  // Meta+Right

    case fc::Fkey_ic:
      return fc::Fmkey_ic;     // Meta+Ins

    case fc::Fkey_dc:
      return fc::Fmkey_dc;     // Meta+Del

    case fc::Fkey_home:
      return fc::Fmkey_home;   // Meta+Home

    case fc::Fkey_end:
      return fc::Fmkey_end;    // Meta+End

    case fc::Fkey_ppage:
      return fc::Fmkey_ppage;  // Meta+PgUp

    case fc::Fkey_npage:
      return fc::Fmkey_npage;  // Meta+PgDn

    default:
      return key_id;
  }
}

//----------------------------------------------------------------------
int FApplication::linuxShiftCtrlKeyCorrection (const int& key_id)
{
  switch ( key_id )
  {
    case fc::Fkey_up:
      return fc::Fckey_sup;     // Shift+Ctrl+Up

    case fc::Fkey_down:
      return fc::Fckey_sdown;   // Shift+Ctrl+Down

    case fc::Fkey_left:
      return fc::Fckey_sleft;   // Shift+Ctrl+Left

    case fc::Fkey_right:
      return fc::Fckey_sright;  // Shift+Ctrl+Right

    case fc::Fkey_ic:
      return fc::Fckey_sic;     // Shift+Ctrl+Ins

    case fc::Fkey_dc:
      return fc::Fckey_sdc;     // Shift+Ctrl+Del

    case fc::Fkey_home:
      return fc::Fckey_shome;   // Shift+Ctrl+Home

    case fc::Fkey_end:
      return fc::Fckey_send;    // Shift+Ctrl+End

    case fc::Fkey_ppage:
      return fc::Fckey_sppage;  // Shift+Ctrl+PgUp

    case fc::Fkey_npage:
      return fc::Fckey_snpage;  // Shift+Ctrl+PgDn

    default:
      return key_id;
  }
}

//----------------------------------------------------------------------
int FApplication::linuxShiftAltKeyCorrection (const int& key_id)
{
  switch ( key_id )
  {
    case fc::Fkey_up:
      return fc::Fmkey_sup;     // Shift+Meta+Up

    case fc::Fkey_down:
      return fc::Fmkey_sdown;   // Shift+Meta+Down

    case fc::Fkey_left:
      return fc::Fmkey_sright;  // Shift+Meta+Left

    case fc::Fkey_right:
      return fc::Fmkey_sleft;   // Shift+Meta+Right

    case fc::Fkey_ic:
      return fc::Fmkey_sic;     // Shift+Meta+Ins

    case fc::Fkey_dc:
      return fc::Fmkey_sdc;     // Shift+Meta+Del

    case fc::Fkey_home:
      return fc::Fmkey_shome;   // Shift+Meta+Home

    case fc::Fkey_end:
      return fc::Fmkey_send;    // Shift+Meta+End

    case fc::Fkey_ppage:
      return fc::Fmkey_sppage;  // Shift+Meta+PgUp

    case fc::Fkey_npage:
      return fc::Fmkey_snpage;  // Shift+Meta+PgDn

    default:
      return key_id;
  }
}

//----------------------------------------------------------------------
int FApplication::linuxCtrlAltKeyCorrection (const int& key_id)
{
  switch ( key_id )
  {
    case fc::Fkey_up:
      return fc::Fcmkey_up;     // Ctrl+Meta+Up

    case fc::Fkey_down:
      return fc::Fcmkey_down;   // Ctrl+Meta+Down

    case fc::Fkey_left:
      return fc::Fcmkey_left;   // Ctrl+Meta+Left

    case fc::Fkey_right:
      return fc::Fcmkey_right;  // Ctrl+Meta+Right

    case fc::Fkey_ic:
      return fc::Fcmkey_ic;     // Ctrl+Meta+Ins

    case fc::Fkey_dc:
      return fc::Fcmkey_dc;     // Ctrl+Meta+Del

    case fc::Fkey_home:
      return fc::Fcmkey_home;   // Ctrl+Meta+Home

    case fc::Fkey_end:
      return fc::Fcmkey_end;    // Ctrl+Meta+End

    case fc::Fkey_ppage:
      return fc::Fcmkey_ppage;  // Ctrl+Meta+PgUp

    case fc::Fkey_npage:
      return fc::Fcmkey_npage;  // Ctrl+Meta+PgDn

    default:
      return key_id;
  }
}

//----------------------------------------------------------------------
int FApplication::linuxShiftCtrlAltKeyCorrection (const int& key_id)
{
  switch ( key_id )
  {
    case fc::Fkey_up:
      return fc::Fcmkey_sup;     // Shift+Ctrl+Meta+Up

    case fc::Fkey_down:
      return fc::Fcmkey_sdown;   // Shift+Ctrl+Meta+Down

    case fc::Fkey_left:
      return fc::Fcmkey_sleft;   // Shift+Ctrl+Meta+Left

    case fc::Fkey_right:
      return fc::Fcmkey_sright;  // Shift+Ctrl+Meta+Right

    case fc::Fkey_ic:
      return fc::Fcmkey_sic;     // Shift+Ctrl+Meta+Ins

    case fc::Fkey_dc:
      return fc::Fcmkey_sdc;     // Shift+Ctrl+Meta+Del

    case fc::Fkey_home:
      return fc::Fcmkey_shome;   // Shift+Ctrl+Meta+Home

    case fc::Fkey_end:
      return fc::Fcmkey_send;    // Shift+Ctrl+Meta+End

    case fc::Fkey_ppage:
      return fc::Fcmkey_sppage;  // Shift+Ctrl+Meta+PgUp

    case fc::Fkey_npage:
      return fc::Fcmkey_snpage;  // Shift+Ctrl+Meta+PgDn

    default:
      return key_id;
  }
}

//----------------------------------------------------------------------
int FApplication::linuxModifierKeyCorrection (const int& key_id)
{
  // Get the current modifier key state

  FTerm::modifier_key& m = getLinuxModifierKey();

  if ( ! (m.shift || m.ctrl || m.alt) )
  {
    return key_id;
  }
  else if ( m.shift && ! m.ctrl && ! m.alt )
  {
    return linuxShiftKeyCorrection(key_id);
  }
  else if ( ! m.shift && m.ctrl && ! m.alt )
  {
    return linuxCtrlKeyCorrection(key_id);
  }
  else if ( ! m.shift && ! m.ctrl && m.alt )
  {
    return linuxAltKeyCorrection(key_id);
  }
  else if ( m.shift && m.ctrl && ! m.alt )
  {
    return linuxShiftCtrlKeyCorrection(key_id);
  }
  else if ( m.shift && ! m.ctrl && m.alt )
  {
    return linuxShiftAltKeyCorrection(key_id);
  }
  else if ( ! m.shift && m.ctrl && m.alt )
  {
    return linuxCtrlAltKeyCorrection(key_id);
  }
  else if ( m.shift &&  m.ctrl && m.alt )
  {
    return linuxShiftCtrlAltKeyCorrection(key_id);
  }

  return key_id;
}
#endif

//----------------------------------------------------------------------
bool FApplication::processDialogSwitchAccelerator()
{
  if ( key >= fc::Fmkey_1 && key <= fc::Fmkey_9 )
  {
    uLong n = uLong(key - fc::Fmkey_0);
    uLong s = dialog_list->size();

    if ( s > 0 && s >= n )
    {
      // unset the move/size mode
      if ( move_size_widget )
      {
        FWidget* w = move_size_widget;
        move_size_widget = 0;
        w->redraw();
      }

      FAccelEvent a_ev (fc::Accelerator_Event, focus_widget);
      sendEvent (dialog_list->at(n - 1), &a_ev);
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------
bool FApplication::processAccelerator (const FWidget*& widget)
{
  bool accpt = false;

  if ( widget
    && widget->accelerator_list
    && ! widget->accelerator_list->empty() )
  {
    FWidget::Accelerators::const_iterator iter, last;
    iter = widget->accelerator_list->begin();
    last = widget->accelerator_list->end();

    while ( iter != last )
    {
      if ( quit_now || app_exit_loop )
        break;

      if ( iter->key == key )
      {
        // unset the move/size mode
        if ( move_size_widget )
        {
          FWidget* w = move_size_widget;
          move_size_widget = 0;
          w->redraw();
        }

        FAccelEvent a_ev (fc::Accelerator_Event, focus_widget);
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
  bool mouse_event_occurred = false;

  if ( mouse && mouse->hasData() )
  {
    mouse->processEvent (&time_keypressed);
    unprocessedInput() = mouse->isInputDataPending();
    mouse_event_occurred = mouse->hasEvent();
  }

  return mouse_event_occurred;
}

//----------------------------------------------------------------------
FWidget*& FApplication::determineClickedWidget()
{
  if ( clicked_widget )
    return clicked_widget;

  if ( ! mouse )
    return clicked_widget;

  if ( ! mouse->isLeftButtonPressed()
    && ! mouse->isLeftButtonDoubleClick()
    && ! mouse->isRightButtonPressed()
    && ! mouse->isMiddleButtonPressed()
    && ! mouse->isWheelUp()
    && ! mouse->isWheelDown() )
    return clicked_widget;

  const FPoint& mouse_position = mouse->getPos();

  // Determine the window object on the current click position
  FWidget* window = FWindow::getWindowWidgetAt (mouse_position);

  if ( window )
  {
    // Determine the widget at the current click position
    FWidget* child = childWidgetAt (window, mouse_position);
    clicked_widget = ( child != 0 ) ? child : window;
  }

  return clicked_widget;
}

//----------------------------------------------------------------------
void FApplication::unsetMoveSizeMode()
{
  // Unset the move/size mode
  if ( move_size_widget )
  {
    FWidget* w = move_size_widget;
    move_size_widget = 0;
    w->redraw();
  }
}

//----------------------------------------------------------------------
void FApplication::closeOpenMenu()
{
  // Close the open menu

  if ( ! open_menu || ( mouse && mouse->isMoved()) )
    return;

  FMenu* menu = static_cast<FMenu*>(open_menu);

  if ( mouse )
  {
    const FPoint& mouse_position = mouse->getPos();

    if ( menu->containsMenuStructure(mouse_position) )
      return;
  }

  bool is_window_menu;
  FWidget* super = menu->getSuperMenu();

  if ( super && menu->isWindowsMenu(super) )
    is_window_menu = true;
  else
    is_window_menu = false;

  menu->unselectItem();
  menu->hide();
  menu->hideSubMenus();
  menu->hideSuperMenus();

  // No widget was been clicked and the menu is no dialog menu
  if ( ! (clicked_widget || is_window_menu) )
    FWindow::switchToPrevWindow();

  if ( getStatusBar() )
    getStatusBar()->drawMessage();

  updateTerminal();
  flush_out();
}

//----------------------------------------------------------------------
void FApplication::unselectMenubarItems()
{
  // Unselect the menu bar items

  if ( open_menu || (mouse && mouse->isMoved()) )
    return;

  FMenuBar* menubar = getMenuBar();

  if ( ! menubar )
    return;

  if ( ! menubar->hasSelectedItem() )
    return;

  if ( ! mouse )
    return;

  const FPoint& mouse_position = mouse->getPos();

  if ( ! getMenuBar()->getTermGeometry().contains(mouse_position) )
  {
    if ( getStatusBar() )
      getStatusBar()->clearMessage();

    getMenuBar()->resetMenu();
    getMenuBar()->redraw();

    // No widget was been clicked
    if ( ! clicked_widget )
      FWindow::switchToPrevWindow();

    if ( getStatusBar() )
      getStatusBar()->drawMessage();

    updateTerminal();
    flush_out();
  }
}

//----------------------------------------------------------------------
void FApplication::sendMouseEvent()
{
  if ( ! clicked_widget )
    return;

  if ( ! mouse )
    return;

  FPoint widgetMousePos;
  const FPoint& mouse_position = mouse->getPos();
  int key_state = 0;

  if ( mouse->isShiftKeyPressed() )
    key_state |= fc::ShiftButton;

  if ( mouse->isControlKeyPressed() )
    key_state |= fc::ControlButton;

  if ( mouse->isMetaKeyPressed() )
    key_state |= fc::MetaButton;

  widgetMousePos = clicked_widget->termToWidgetPos(mouse_position);

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

  if ( mouse->isLeftButtonPressed() )
  {
    FMouseEvent m_down_ev ( fc::MouseMove_Event
                          , widgetMousePos
                          , mouse_position
                          , fc::LeftButton | key_state );
    sendEvent (clicked_widget, &m_down_ev);
  }

  if ( mouse->isRightButtonPressed() )
  {
    FMouseEvent m_down_ev ( fc::MouseMove_Event
                          , widgetMousePos
                          , mouse_position
                          , fc::RightButton | key_state );
    sendEvent (clicked_widget, &m_down_ev);
  }

  if ( mouse->isMiddleButtonPressed() )
  {
    FMouseEvent m_down_ev ( fc::MouseMove_Event
                          , widgetMousePos
                          , mouse_position
                          , fc::MiddleButton | key_state );
    sendEvent (clicked_widget, &m_down_ev);
  }
}

//----------------------------------------------------------------------
void FApplication::sendMouseLeftClickEvent ( const FPoint& widgetMousePos
                                           , const FPoint& mouse_position
                                           , int key_state )
{
  if ( ! mouse )
    return;

  if ( mouse->isLeftButtonDoubleClick() )
  {
    FMouseEvent m_dblclick_ev ( fc::MouseDoubleClick_Event
                              , widgetMousePos
                              , mouse_position
                              , fc::LeftButton | key_state );
    sendEvent (clicked_widget, &m_dblclick_ev);
  }
  else if ( mouse->isLeftButtonPressed() )
  {
    FMouseEvent m_down_ev ( fc::MouseDown_Event
                          , widgetMousePos
                          , mouse_position
                          , fc::LeftButton | key_state );
    sendEvent (clicked_widget, &m_down_ev);
  }
  else if ( mouse->isLeftButtonReleased() )
  {
    FMouseEvent m_up_ev ( fc::MouseUp_Event
                        , widgetMousePos
                        , mouse_position
                        , fc::LeftButton | key_state );
    FWidget* released_widget = clicked_widget;

    if ( ! mouse->isRightButtonPressed()
      && ! mouse->isMiddleButtonPressed() )
      clicked_widget = 0;

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

  if ( mouse->isRightButtonPressed() )
  {
    FMouseEvent m_down_ev ( fc::MouseDown_Event
                          , widgetMousePos
                          , mouse_position
                          , fc::RightButton | key_state );
    sendEvent (clicked_widget, &m_down_ev);
  }
  else if ( mouse->isRightButtonReleased() )
  {
    FMouseEvent m_up_ev ( fc::MouseUp_Event
                        , widgetMousePos
                        , mouse_position
                        , fc::RightButton | key_state );
    FWidget* released_widget = clicked_widget;

    if ( ! mouse->isLeftButtonPressed()
      && ! mouse->isMiddleButtonPressed() )
      clicked_widget = 0;

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

  if ( mouse->isMiddleButtonPressed() )
  {
    FMouseEvent m_down_ev ( fc::MouseDown_Event
                          , widgetMousePos
                          , mouse_position
                          , fc::MiddleButton | key_state );
    sendEvent (clicked_widget, &m_down_ev);

    // gnome-terminal sends no released on middle click
    if ( isGnomeTerminal() )
      clicked_widget = 0;
  }
  else if ( mouse->isMiddleButtonReleased() )
  {
    FMouseEvent m_up_ev ( fc::MouseUp_Event
                        , widgetMousePos
                        , mouse_position
                        , fc::MiddleButton | key_state );
    FWidget* released_widget = clicked_widget;

    if ( ! mouse->isLeftButtonPressed()
      && ! mouse->isRightButtonPressed() )
    {
      clicked_widget = 0;
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

  if ( mouse->isWheelUp() )
  {
    FWheelEvent wheel_ev ( fc::MouseWheel_Event
                         , widgetMousePos
                         , mouse_position
                         , fc::WheelUp );
    FWidget* scroll_over_widget = clicked_widget;
    clicked_widget = 0;
    sendEvent(scroll_over_widget, &wheel_ev);
  }

  if ( mouse->isWheelDown() )
  {
    FWheelEvent wheel_ev ( fc::MouseWheel_Event
                         , widgetMousePos
                         , mouse_position
                         , fc::WheelDown );
    FWidget* scroll_over_widget = clicked_widget;
    clicked_widget = 0;
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
  closeOpenMenu();
  unselectMenubarItems();
  sendMouseEvent();

  if ( mouse )
    mouse->drawGpmPointer();
}

//----------------------------------------------------------------------
void FApplication::processResizeEvent()
{
  if ( hasChangedTermSize() )
  {
    FResizeEvent r_ev(fc::Resize_Event);
    sendEvent(rootObj, &r_ev);

    if ( r_ev.isAccepted() )
      changeTermSizeFinished();
  }
}

//----------------------------------------------------------------------
int FApplication::processTimerEvent()
{
  FObject::TimerList::iterator iter, last;
  timeval currentTime;
  int activated = 0;

  getCurrentTime (&currentTime);

  if ( isTimerInUpdating() )
    return 0;

  if ( ! timer_list )
    return 0;

  if ( timer_list->empty() )
    return 0;

  iter = timer_list->begin();
  last  = timer_list->end();

  while ( iter != last )
  {
    if ( ! (*iter).id
      || ! (*iter).object
      || currentTime < (*iter).timeout )  // no timer expired
      break;

    (*iter).timeout += (*iter).interval;

    if ( (*iter).timeout < currentTime )
      (*iter).timeout = currentTime + (*iter).interval;

    if ( (*iter).interval.tv_usec > 0 || (*iter).interval.tv_sec > 0 )
      activated++;

    FTimerEvent t_ev(fc::Timer_Event, (*iter).id);
    sendEvent((*iter).object, &t_ev);

    ++iter;
  }

  return activated;
}

//----------------------------------------------------------------------
void FApplication::processCloseWidget()
{
  updateTerminal (FVTerm::stop_refresh);

  if ( close_widget && ! close_widget->empty() )
  {
    widgetList::iterator iter;
    iter = close_widget->begin();

    while ( iter != close_widget->end() && *iter )
    {
      delete *iter;
      ++iter;
    }

    close_widget->clear();
  }

  updateTerminal (FVTerm::start_refresh);
}

//----------------------------------------------------------------------
bool FApplication::processNextEvent()
{
  int num_events = 0;

  processKeyboardEvent();
  processMouseEvent();
  processResizeEvent();
  processTerminalUpdate();
  processCloseWidget();

  sendQueuedEvents();
  num_events += processTimerEvent();

  return ( num_events > 0 );
}
