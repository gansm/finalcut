/***********************************************************************
* fapplication.cpp - Manages the application events                    *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2013-2017 Markus Gans                                      *
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
int      FApplication::loop_level       = 0;  // event loop level
FWidget* FApplication::main_widget      = 0;  // main application widget
FWidget* FApplication::active_window    = 0;  // the active window
FWidget* FApplication::focus_widget     = 0;  // has keyboard input focus
FWidget* FApplication::clicked_widget   = 0;  // is focused by click
FWidget* FApplication::open_menu        = 0;  // currently open menu
FWidget* FApplication::move_size_widget = 0;  // move/size by keyboard
FPoint*  FApplication::zero_point       = 0;  // zero point (x=0, y=0)
uChar    FApplication::x11_button_state = 0x03;
int      FApplication::quit_code        = 0;
bool     FApplication::quit_now         = false;

FApplication::eventQueue* FApplication::event_queue = 0;

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
#ifdef F_HAVE_LIBGPM
  , gpm_ev()
  , gpmMouseEvent(false)
#endif
  , b_state()
  , fifo_offset(0)
  , fifo_in_use(false)
  , fifo_buf_size(sizeof(fifo_buf))
  , key_timeout(100000)        // 100 ms
  , dblclick_interval(500000)  // 500 ms
  , time_keypressed()
  , time_mousepressed()
  , new_mouse_position()
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

  init();
}

//----------------------------------------------------------------------
FApplication::~FApplication()  // destructor
{
  if ( zero_point )
    delete zero_point;

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
    << "       {UTF8, VT100, PC, ASCII}" << std::endl
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
void FApplication::init()
{
  // init keyboard values
  time_keypressed.tv_sec = 0;
  time_keypressed.tv_usec = 0;
  time_mousepressed.tv_sec = 0;
  time_mousepressed.tv_usec = 0;
  x11_button_state = 0x23;

#ifdef F_HAVE_LIBGPM
  gpm_ev.x = -1;
#endif

  try
  {
    zero_point = new FPoint (0,0);
    event_queue = new eventQueue;
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
    std::abort();
  }

  // init arrays with '\0'
  std::fill_n (k_buf, sizeof(k_buf), '\0');
  std::fill_n (fifo_buf, fifo_buf_size, '\0');
  std::fill_n (x11_mouse, sizeof(x11_mouse), '\0');
  std::fill_n (sgr_mouse, sizeof(sgr_mouse), '\0');
  std::fill_n (urxvt_mouse, sizeof(urxvt_mouse), '\0');
  // init bit field with 0
  std::memset(&b_state, 0x00, sizeof(b_state));
}

//----------------------------------------------------------------------
void FApplication::cmd_options (const int& argc, char* argv[])
{
  // interpret the command line options

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
        encoding = encoding.toUpper();

        if ( encoding.includes("UTF8") )
          init_values.encoding = fc::UTF8;
        else if ( encoding.includes("VT100") )
          init_values.encoding = fc::VT100;
        else if ( encoding.includes("PC") )
          init_values.encoding = fc::PC;
        else if ( encoding.includes("ASCII") )
          init_values.encoding = fc::ASCII;
        else if ( encoding.includes("HELP") )
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
#ifdef F_HAVE_LIBGPM
int FApplication::gpmEvent (bool clear)
{
  register int result;
  register int max = ( gpm_fd > stdin_no ) ? gpm_fd : stdin_no;
  fd_set ifds;
  struct timeval tv;

  FD_ZERO(&ifds);
  FD_SET(stdin_no, &ifds);
  FD_SET(gpm_fd, &ifds);
  tv.tv_sec  = 0;
  tv.tv_usec = 100000;  // 100 ms
  result = select (max + 1, &ifds, 0, 0, &tv);

  if ( result > 0 && FD_ISSET(stdin_no, &ifds) )
  {
    if ( clear )
      FD_CLR (stdin_no, &ifds);

    return keyboard_event;
  }

  if ( clear && result > 0 && FD_ISSET(gpm_fd, &ifds) )
    FD_CLR (gpm_fd, &ifds);

  if ( result > 0 )
    return mouse_event;
  else
    return no_event;
}
#endif  // F_HAVE_LIBGPM

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
inline void FApplication::keyboardBufferTimeout (FWidget*)
{
  // Empty the buffer on timeout
  if ( fifo_in_use && isKeyTimeout(&time_keypressed, key_timeout)  )
  {
    fifo_offset = 0;
    key = 0;
    std::fill_n (fifo_buf, fifo_buf_size, '\0');
    fifo_in_use = false;
  }
}

//----------------------------------------------------------------------
inline bool FApplication::getKeyPressedState()
{
#ifdef F_HAVE_LIBGPM

  if ( isGpmMouseEnabled() )
  {
    gpmMouseEvent = false;
    int type = gpmEvent();

    switch ( type )
    {
      case mouse_event:
        gpmMouseEvent = true;
        break;

      case keyboard_event:
        return true;

      default:
        return false;
    }
  }

#endif  // F_HAVE_LIBGPM

  return KeyPressed();
}

//----------------------------------------------------------------------
inline void FApplication::readRawX11MouseData()
{
  static const int len = 6;
  int n;
  x11_mouse[0] = fifo_buf[3];
  x11_mouse[1] = fifo_buf[4];
  x11_mouse[2] = fifo_buf[5];
  x11_mouse[3] = '\0';

  // Remove founded entry
  for (n = len; n < fifo_buf_size; n++)
    fifo_buf[n - len] = fifo_buf[n];

  n = fifo_buf_size - len - 1;

  // Fill rest with '\0'
  for (; n < fifo_buf_size; n++)
    fifo_buf[n - len] = '\0';

  unprocessedInput() = bool(fifo_buf[0] != '\0');
  processMouseEvent();
}

//----------------------------------------------------------------------
inline void FApplication::readRawExtendedMouseData()
{
  int len = int(std::strlen(fifo_buf));
  int n = 3;

  while ( n < len && n < fifo_buf_size )
  {
    sgr_mouse[n - 3] = fifo_buf[n];
    n++;

    if ( fifo_buf[n] == 'M' || fifo_buf[n] == 'm' )
      len = n + 1;
  }

  sgr_mouse[n - 3] = '\0';

  for (n = len; n < fifo_buf_size; n++)  // Remove founded entry
    fifo_buf[n - len] = fifo_buf[n];

  n = fifo_buf_size - len - 1;

  for (; n < fifo_buf_size; n++)       // Fill rest with '\0'
    fifo_buf[n - len] = '\0';

  unprocessedInput() = bool(fifo_buf[0] != '\0');
  processMouseEvent();
}

//----------------------------------------------------------------------
inline void FApplication::readRawUrxvtMouseData()
{
  int len = int(std::strlen(fifo_buf));
  int n = 2;

  while ( n < len && n < fifo_buf_size )
  {
    urxvt_mouse[n - 2] = fifo_buf[n];
    n++;

    if ( fifo_buf[n] == 'M' || fifo_buf[n] == 'm' )
      len = n + 1;
  }

  urxvt_mouse[n - 2] = '\0';

  for (n = len; n < fifo_buf_size; n++)  // Remove founded entry
    fifo_buf[n - len] = fifo_buf[n];

  n = fifo_buf_size - len - 1;

  for (; n < fifo_buf_size; n++)       // Fill rest with '\0'
    fifo_buf[n - len] = '\0';

  unprocessedInput() = bool(fifo_buf[0] != '\0');
  processMouseEvent();
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
    && isKeyTimeout(&time_keypressed, key_timeout) )
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
  bool isKeyPressed;
  FWidget* widget = findKeyboardWidget();
  keyboardBufferTimeout(widget);
  flush_out();
  isKeyPressed = getKeyPressedState();

  if ( isKeyPressed )
  {
    register ssize_t bytesread;
    widget->getCurrentTime (&time_keypressed);
    x11_mouse[0] = sgr_mouse[0] = urxvt_mouse[0] = '\0';

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

      // read the rest from the fifo buffer
      while ( ! widget->isKeyTimeout(&time_keypressed, key_timeout)
           && fifo_offset > 0
           && key != NEED_MORE_DATA )
      {
        key = FTerm::parseKeyString(fifo_buf, fifo_buf_size, &time_keypressed);

        if ( key != NEED_MORE_DATA )
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
              readRawX11MouseData();
              break;

            case fc::Fkey_extended_mouse:
              readRawExtendedMouseData();
              break;

            case fc::Fkey_urxvt_mouse:
              readRawUrxvtMouseData();
              break;

            default:
              bool acceptKeyDown = sendKeyDownEvent (widget);
              bool acceptKeyPress = sendKeyPressEvent (widget);

              if ( ! (acceptKeyDown || acceptKeyPress) )
                sendKeyboardAccelerator();

              break;
          }  // end of switch
        }

        fifo_offset = int(std::strlen(fifo_buf));
      }

      // Send key up event
      sendKeyUpEvent (widget);
      key = 0;
    }

    std::fill_n (k_buf, sizeof(k_buf), '\0');
  }

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
void FApplication::getX11ButtonState (int button)
{
  // get the x11 and urxvt mouse button state
  const FPoint& mouse_position = getMousePos();

  enum btn_states
  {
    key_shift            = 0x04,
    key_meta             = 0x08,
    key_ctrl             = 0x10,
    key_button_mask      = 0x1c,
    button1_pressed      = 0x20,
    button2_pressed      = 0x21,
    button3_pressed      = 0x22,
    all_buttons_released = 0x23,
    button1_pressed_move = 0x40,
    button2_pressed_move = 0x41,
    button3_pressed_move = 0x42,
    button_mask          = 0x63,
    button_up            = 0x60,
    button_down          = 0x61,
    button_up_move       = 0x60,
    button_down_move     = 0x61
  };

  switch ( button )
  {
    case button1_pressed:
    case button1_pressed_move:
      if ( mouse_position == new_mouse_position
        && x11_button_state == all_buttons_released
        && ! isKeyTimeout(&time_mousepressed, dblclick_interval) )
      {
        time_mousepressed.tv_sec = 0;
        time_mousepressed.tv_usec = 0;
        b_state.left_button = DoubleClick;
      }
      else
      {
        time_mousepressed = time_keypressed;  // save click time
        b_state.left_button = Pressed;
      }
      break;

    case button2_pressed:
    case button2_pressed_move:
      time_mousepressed.tv_sec = 0;
      time_mousepressed.tv_usec = 0;
      b_state.middle_button = Pressed;
      break;

    case button3_pressed:
    case button3_pressed_move:
      time_mousepressed.tv_sec = 0;
      time_mousepressed.tv_usec = 0;
      b_state.right_button = Pressed;
      break;

    case all_buttons_released:
      switch ( x11_button_state & button_mask )
      {
        case button1_pressed:
        case button1_pressed_move:
          b_state.left_button = Released;
          break;

        case button2_pressed:
        case button2_pressed_move:
          b_state.middle_button = Released;
          break;

        case button3_pressed:
        case button3_pressed_move:
          b_state.right_button = Released;
          break;

        default:
          break;
      }
      break;

    case button_up:
      time_mousepressed.tv_sec = 0;
      time_mousepressed.tv_usec = 0;
      b_state.wheel_up = Pressed;
      break;

    case button_down:
      time_mousepressed.tv_sec = 0;
      time_mousepressed.tv_usec = 0;
      b_state.wheel_down = Pressed;
      break;

      default:
        break;
  }
}

//----------------------------------------------------------------------
bool FApplication::parseX11Mouse()
{
  const FPoint& mouse_position = getMousePos();
  uChar x, y;

  enum x11_btn_states
  {
    key_shift            = 0x04,
    key_meta             = 0x08,
    key_ctrl             = 0x10,
    key_button_mask      = 0x1c,
    button1_pressed      = 0x20,
    button2_pressed      = 0x21,
    button3_pressed      = 0x22,
    all_buttons_released = 0x23,
    button1_pressed_move = 0x40,
    button2_pressed_move = 0x41,
    button3_pressed_move = 0x42,
    button_mask          = 0x63,
    button_up            = 0x60,
    button_down          = 0x61
  };

  x = uChar(x11_mouse[1] - 0x20);
  y = uChar(x11_mouse[2] - 0x20);
  new_mouse_position.setPoint (x, y);
  // fill bit field with 0
  std::memset(&b_state, 0x00, sizeof(b_state));

  if ( (x11_mouse[0] & key_shift) == key_shift )
    b_state.shift_button = Pressed;

  if ( (x11_mouse[0] & key_meta) == key_meta )
    b_state.meta_button = Pressed;

  if ( (x11_mouse[0] & key_ctrl) == key_ctrl )
    b_state.control_button = Pressed;

  if ( (x11_mouse[0] & button_mask) >= button1_pressed_move
    && (x11_mouse[0] & button_mask) <= button3_pressed_move
    && mouse_position != *zero_point )
  {
    b_state.mouse_moved = true;
  }

  getX11ButtonState (x11_mouse[0] & button_mask);

  if ( uChar(x11_mouse[1]) == mouse_position.getX() + 0x20
    && uChar(x11_mouse[2]) == mouse_position.getY() + 0x20
    && b_state.wheel_up != Pressed
    && b_state.wheel_down != Pressed
    && uChar(x11_mouse[0]) == x11_button_state )
  {
    return false;
  }

  setMousePos (x, y);
  x11_button_state = uChar(x11_mouse[0]);
  x11_mouse[0] = '\0';
  return true;
}

//----------------------------------------------------------------------
bool FApplication::parseSGRMouse()
{
  const FPoint& mouse_position = getMousePos();
  register char* p;
  int button;
  short x, y;

  enum x11_ext_btn_states
  {
    key_shift       = 0x04,
    key_meta        = 0x08,
    key_ctrl        = 0x10,
    key_button_mask = 0x1c,
    button1         = 0x00,
    button2         = 0x01,
    button3         = 0x02,
    button1_move    = 0x20,
    button2_move    = 0x21,
    button3_move    = 0x22,
    button_mask     = 0x63,
    button_up       = 0x40,
    button_down     = 0x41,
    pressed         = 'M',
    released        = 'm'
  };

  x = 0;
  y = 0;
  button = 0;

  // parse the SGR mouse string
  p = sgr_mouse;

  while ( *p && *p != ';' )
  {
    if ( *p < '0' || *p > '9')
      return false;

    button = 10 * button + (*p - '0');
    p++;
  }

  while ( *p++ && *p != ';' )
  {
    if ( *p < '0' || *p > '9')
      return false;
    x = short(10 * x + (*p - '0'));
  }

  while ( *p++ && *p != 'M' && *p != 'm' )
  {
    if ( *p < '0' || *p > '9')
      return false;

    y = short(10 * y + (*p - '0'));
  }

  new_mouse_position.setPoint (x, y);
  // fill bit field with 0
  std::memset(&b_state, 0x00, sizeof(b_state));

  if ( (button & key_shift) == key_shift )
    b_state.shift_button = Pressed;

  if ( (button & key_meta) == key_meta )
    b_state.meta_button = Pressed;

  if ( (button & key_ctrl) == key_ctrl )
    b_state.control_button = Pressed;

  if ( (button & button_mask) >= button1_move
    && (button & button_mask) <= button3_move
    && mouse_position != *zero_point )
  {
    b_state.mouse_moved = true;
  }

  if ( *p == pressed )
  {
    switch ( button & button_mask )
    {
      case button1:
      case button1_move:
        if ( mouse_position == new_mouse_position
          && (((x11_button_state & 0x80) >> 2) + 'M') == released
          && ! isKeyTimeout(&time_mousepressed, dblclick_interval) )
        {
          time_mousepressed.tv_sec = 0;
          time_mousepressed.tv_usec = 0;
          b_state.left_button = DoubleClick;
        }
        else
        {
          time_mousepressed = time_keypressed;  // save click time
          b_state.left_button = Pressed;
        }
        break;

      case button2:
      case button2_move:
        time_mousepressed.tv_sec = 0;
        time_mousepressed.tv_usec = 0;
        b_state.middle_button = Pressed;
        break;

      case button3:
      case button3_move:
        time_mousepressed.tv_sec = 0;
        time_mousepressed.tv_usec = 0;
        b_state.right_button = Pressed;
        break;

      case button_up:
        time_mousepressed.tv_sec = 0;
        time_mousepressed.tv_usec = 0;
        b_state.wheel_up = Pressed;
        break;

      case button_down:
        time_mousepressed.tv_sec = 0;
        time_mousepressed.tv_usec = 0;
        b_state.wheel_down = Pressed;
        break;

      default:
        break;
    }
  }
  else  // *p == released
  {
    switch ( button & button_mask )
    {
      case button1:
      case button1_move:
        b_state.left_button = Released;
        break;

      case button2:
      case button2_move:
        b_state.middle_button = Released;
        break;

      case button3:
      case button3_move:
        b_state.right_button = Released;
        break;

      default:
        break;
    }
  }

  if ( mouse_position == new_mouse_position
    && b_state.wheel_up != Pressed
    && b_state.wheel_down != Pressed
    && x11_button_state == uChar(((*p & 0x20) << 2) + button) )
  {
    return false;
  }

  setMousePos (x, y);
  x11_button_state = uChar(((*p & 0x20) << 2) + button);
  sgr_mouse[0] = '\0';
  return true;
}

//----------------------------------------------------------------------
bool FApplication::parseUrxvtMouse()
{
  const FPoint& mouse_position = getMousePos();
  register char* p;
  register bool x_neg;
  register bool y_neg;
  int button;
  short x, y;

  enum urxvt_btn_states
  {
    key_shift            = 0x04,
    key_meta             = 0x08,
    key_ctrl             = 0x10,
    key_button_mask      = 0x1c,
    button1_pressed      = 0x20,
    button2_pressed      = 0x21,
    button3_pressed      = 0x22,
    all_buttons_released = 0x23,
    button1_pressed_move = 0x40,
    button2_pressed_move = 0x41,
    button3_pressed_move = 0x42,
    button_mask          = 0x63,
    button_up            = 0x60,
    button_down          = 0x61,
    button_up_move       = 0x60,
    button_down_move     = 0x61
  };
  x = 0;
  y = 0;
  button = 0;

  // parse the SGR mouse string
  p = urxvt_mouse;
  x_neg = false;
  y_neg = false;

  while ( *p && *p != ';' )
  {
    if ( *p < '0' || *p > '9')
      return false;

    button = 10 * button + (*p - '0');
    p++;
  }

  if ( *++p == '-' )
  {
    p++;
    x_neg = true;
  }

  while ( *p && *p != ';' )
  {
    if ( *p < '0' || *p > '9')
      return false;

    x = short(10 * x + (*p - '0'));
    p++;
  }

  if ( *++p == '-' )
  {
    p++;
    y_neg = true;
  }

  while ( *p && *p != 'M' )
  {
    if ( *p < '0' || *p > '9')
      return false;

    y = short(10 * y + (*p - '0'));
    p++;
  }

  if ( x_neg || x == 0 )
    x = 1;

  if ( y_neg || y == 0 )
    y = 1;

  if ( x > getColumnNumber() )
    x = short(getColumnNumber());

  if ( y > getLineNumber() )
    y = short(getLineNumber());

  new_mouse_position.setPoint (x, y);
  // fill bit field with 0
  std::memset(&b_state, 0x00, sizeof(b_state));

  if ( (button & key_shift) == key_shift )
    b_state.shift_button = Pressed;

  if ( (button & key_meta) == key_meta )
    b_state.meta_button = Pressed;

  if ( (button & key_ctrl) == key_ctrl )
    b_state.control_button = Pressed;

  if ( (button & button_mask) >= button1_pressed_move
    && (button & button_mask) <= button3_pressed_move
    && mouse_position != *zero_point )
  {
    b_state.mouse_moved = true;
  }

  getX11ButtonState (button & button_mask);

  if ( mouse_position == new_mouse_position
    && b_state.wheel_up != Pressed
    && b_state.wheel_down != Pressed
    && x11_button_state == uChar(button) )
  {
    return false;
  }

  setMousePos (x, y);
  x11_button_state = uChar(button);
  urxvt_mouse[0] = '\0';
  return true;
}

//----------------------------------------------------------------------
#ifdef F_HAVE_LIBGPM
bool FApplication::processGpmEvent()
{
  // fill bit field with 0
  std::memset(&b_state, 0x00, sizeof(b_state));

  if ( Gpm_GetEvent(&gpm_ev) == 1 )
  {
    Gpm_FitEvent (&gpm_ev);

    if ( gpm_ev.type & GPM_DRAG && gpm_ev.wdx == 0 && gpm_ev.wdy == 0 )
      b_state.mouse_moved = true;

    if ( gpm_ev.wdy > 0 )
      b_state.wheel_up = Pressed;
    else if ( gpm_ev.wdy < 0 )
      b_state.wheel_down = Pressed;

    switch ( gpm_ev.type & 0x0f )
    {
      case GPM_DOWN:
      case GPM_DRAG:
        if ( gpm_ev.buttons & GPM_B_LEFT )
        {
          if ( gpm_ev.type & GPM_DOUBLE )
            b_state.left_button = DoubleClick;
          else
            b_state.left_button = Pressed;
        }

        if ( gpm_ev.buttons & GPM_B_MIDDLE )
          b_state.middle_button = Pressed;

        if ( gpm_ev.buttons & GPM_B_RIGHT )
          b_state.right_button = Pressed;

        if ( gpm_ev.buttons & GPM_B_UP )
          b_state.wheel_up = Pressed;

        if ( gpm_ev.buttons & GPM_B_DOWN )
          b_state.wheel_down = Pressed;

        // keyboard modifiers
        if ( gpm_ev.modifiers & (1 << KG_SHIFT) )
          b_state.shift_button = Pressed;

        if ( gpm_ev.modifiers & ((1 << KG_ALT) | (1 << KG_ALTGR)) )
          b_state.meta_button = Pressed;

        if ( gpm_ev.modifiers & (1 << KG_CTRL) )
          b_state.control_button = Pressed;

        break;

      case GPM_UP:
        if ( gpm_ev.buttons & GPM_B_LEFT )
          b_state.left_button = Released;

        if ( gpm_ev.buttons & GPM_B_MIDDLE )
          b_state.middle_button = Released;

        if ( gpm_ev.buttons & GPM_B_RIGHT )
          b_state.right_button = Released;

      default:
        break;
    }

    setMousePos (gpm_ev.x, gpm_ev.y);

    if ( gpmEvent(false) == mouse_event )
      unprocessedInput() = true;
    else
      unprocessedInput() = false;

    GPM_DRAWPOINTER(&gpm_ev);
    gpmMouseEvent = false;

    return true;
  }

  gpmMouseEvent = false;
  return false;
}
#endif  // F_HAVE_LIBGPM

//----------------------------------------------------------------------
bool FApplication::getMouseEvent()
{
  bool Event = false;

#ifdef F_HAVE_LIBGPM
  if ( ! gpmMouseEvent
    && x11_mouse[0] == '\0'
    && sgr_mouse[0] == '\0'
    && urxvt_mouse[0] == '\0' )
  {
    return false;
  }
#else
  if ( x11_mouse[0] == '\0'
    && sgr_mouse[0] == '\0'
    && urxvt_mouse[0] == '\0' )
  {
    return false;
  }
#endif

#ifdef F_HAVE_LIBGPM
  if ( gpmMouseEvent )
    Event = processGpmEvent();
#endif

  if ( x11_mouse[0] )
    Event = parseX11Mouse();

  if ( sgr_mouse[0] )
    Event = parseSGRMouse();

  if ( urxvt_mouse[0] )
    Event = parseUrxvtMouse();

  if ( ! Event )
    return false;

  return true;
}

//----------------------------------------------------------------------
FWidget*& FApplication::determineClickedWidget()
{
  if ( clicked_widget )
    return clicked_widget;

  if ( b_state.left_button != Pressed
    && b_state.left_button != DoubleClick
    && b_state.right_button != Pressed
    && b_state.middle_button != Pressed
    && b_state.wheel_up != Pressed
    && b_state.wheel_down != Pressed )
    return clicked_widget;

  const FPoint& mouse_position = getMousePos();

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

  if ( ! open_menu || b_state.mouse_moved )
    return;

  FMenu* menu = static_cast<FMenu*>(open_menu);
  const FPoint& mouse_position = getMousePos();

  if ( menu && menu->containsMenuStructure(mouse_position) )
    return;

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

  if ( open_menu || b_state.mouse_moved )
    return;

  FMenuBar* menubar = getMenuBar();

  if ( ! menubar )
    return;

  if ( ! menubar->hasSelectedItem() )
    return;

  const FPoint& mouse_position = getMousePos();

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

  FPoint widgetMousePos;
  const FPoint& mouse_position = getMousePos();
  int key_state = 0;

  if ( b_state.shift_button == Pressed )
    key_state |= fc::ShiftButton;

  if ( b_state.meta_button == Pressed )
    key_state |= fc::MetaButton;

  if ( b_state.control_button == Pressed )
    key_state |= fc::ControlButton;

  widgetMousePos = clicked_widget->termToWidgetPos(mouse_position);

  if ( b_state.mouse_moved )
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
}

//----------------------------------------------------------------------
void FApplication::sendMouseMoveEvent ( const FPoint& widgetMousePos
                                      , const FPoint& mouse_position
                                      , int key_state )
{
  if ( b_state.left_button == Pressed )
  {
    FMouseEvent m_down_ev ( fc::MouseMove_Event
                          , widgetMousePos
                          , mouse_position
                          , fc::LeftButton | key_state );
    sendEvent (clicked_widget, &m_down_ev);
  }

  if ( b_state.right_button == Pressed )
  {
    FMouseEvent m_down_ev ( fc::MouseMove_Event
                          , widgetMousePos
                          , mouse_position
                          , fc::RightButton | key_state );
    sendEvent (clicked_widget, &m_down_ev);
  }

  if ( b_state.middle_button == Pressed )
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
  if ( b_state.left_button == DoubleClick )
  {
    FMouseEvent m_dblclick_ev ( fc::MouseDoubleClick_Event
                              , widgetMousePos
                              , mouse_position
                              , fc::LeftButton | key_state );
    sendEvent (clicked_widget, &m_dblclick_ev);
  }
  else if ( b_state.left_button == Pressed )
  {
    FMouseEvent m_down_ev ( fc::MouseDown_Event
                          , widgetMousePos
                          , mouse_position
                          , fc::LeftButton | key_state );
    sendEvent (clicked_widget, &m_down_ev);
  }
  else if ( b_state.left_button == Released )
  {
    FMouseEvent m_up_ev ( fc::MouseUp_Event
                        , widgetMousePos
                        , mouse_position
                        , fc::LeftButton | key_state );
    FWidget* released_widget = clicked_widget;

    if ( b_state.right_button != Pressed
      && b_state.middle_button != Pressed )
      clicked_widget = 0;

    sendEvent (released_widget, &m_up_ev);
  }
}

//----------------------------------------------------------------------
void FApplication::sendMouseRightClickEvent ( const FPoint& widgetMousePos
                                            , const FPoint& mouse_position
                                            , int key_state )
{
  if ( b_state.right_button == Pressed )
  {
    FMouseEvent m_down_ev ( fc::MouseDown_Event
                          , widgetMousePos
                          , mouse_position
                          , fc::RightButton | key_state );
    sendEvent (clicked_widget, &m_down_ev);
  }
  else if ( b_state.right_button == Released )
  {
    FMouseEvent m_up_ev ( fc::MouseUp_Event
                        , widgetMousePos
                        , mouse_position
                        , fc::RightButton | key_state );
    FWidget* released_widget = clicked_widget;

    if ( b_state.left_button != Pressed
      && b_state.middle_button != Pressed )
      clicked_widget = 0;

    sendEvent (released_widget, &m_up_ev);
  }
}

//----------------------------------------------------------------------
void FApplication::sendMouseMiddleClickEvent ( const FPoint& widgetMousePos
                                             , const FPoint& mouse_position
                                             , int key_state )
{
  if ( b_state.middle_button == Pressed )
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
  else if ( b_state.middle_button == Released )
  {
    FMouseEvent m_up_ev ( fc::MouseUp_Event
                        , widgetMousePos
                        , mouse_position
                        , fc::MiddleButton | key_state );
    FWidget* released_widget = clicked_widget;

    if ( b_state.right_button != Pressed
      && b_state.left_button != Pressed )
    {
      clicked_widget = 0;
    }

    sendEvent (released_widget, &m_up_ev);
  }

  if ( b_state.middle_button == Pressed )
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
  else if ( b_state.middle_button == Released )
  {
    FMouseEvent m_up_ev ( fc::MouseUp_Event
                        , widgetMousePos
                        , mouse_position
                        , fc::MiddleButton | key_state );
    FWidget* released_widget = clicked_widget;

    if ( b_state.right_button != Pressed
      && b_state.left_button != Pressed )
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
  if ( b_state.wheel_up == Pressed )
  {
    FWheelEvent wheel_ev ( fc::MouseWheel_Event
                         , widgetMousePos
                         , mouse_position
                         , fc::WheelUp );
    FWidget* scroll_over_widget = clicked_widget;
    clicked_widget = 0;
    sendEvent(scroll_over_widget, &wheel_ev);
  }

  if ( b_state.wheel_down == Pressed )
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

#ifdef F_HAVE_LIBGPM
  if ( isGpmMouseEnabled() && gpm_ev.x != -1 )
    GPM_DRAWPOINTER(&gpm_ev);
#endif
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
  updateTerminal(false);

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

  updateTerminal(true);
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
