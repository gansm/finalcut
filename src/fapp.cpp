// File: fapplication.cpp
// Provides: class FApplication

#include "fapp.h"
#include "fmenu.h"
#include "fwindow.h"

// global application object
static FApplication* rootObj = 0;

// flag to exit local loop
static bool app_exit_loop = false;

// static attributes
int      FApplication::loop_level     = 0;  // event loop level
FWidget* FApplication::main_widget    = 0;  // main application widget
FWidget* FApplication::active_window  = 0;  // the active window
FWidget* FApplication::focus_widget   = 0;  // has keyboard input focus
FWidget* FApplication::clicked_widget = 0;  // is focused by click
FWidget* FApplication::open_menu      = 0;  // currently open menu
FPoint*  FApplication::zero_point     = 0;  // zero point (x=0, y=0)
int      FApplication::quit_code      = 0;
bool     FApplication::quit_now       = false;
std::deque<FApplication::eventPair>* FApplication::event_queue = 0;

//----------------------------------------------------------------------
// class FApplication
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FApplication::FApplication (int &_argc, char* _argv[])
  : app_argc(0)
  , app_argv(0)
  , skipped_terminal_update(0)
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
  , newMousePosition()
{
  assert ( ! rootObj
         && "FApplication: There should be only one application object" );

  rootObj = this;
  static char* empty = const_cast<char*>("");

  if ( _argc == 0 || _argv == 0 )
  {
    _argc = 0;
    _argv = &empty;
  }

  init(_argc, _argv);
}

//----------------------------------------------------------------------
FApplication::~FApplication() // destructor
{
  if ( zero_point )
    delete zero_point;
  if ( event_queue )
    delete event_queue;
  rootObj = 0;
}


// private methods of FApplication
//----------------------------------------------------------------------
void FApplication::init (int _argc, char* _argv[])
{
  app_argc = _argc;
  app_argv = _argv;

  // init keyboard values
  time_keypressed.tv_sec = 0;
  time_keypressed.tv_usec = 0;
  time_mousepressed.tv_sec = 0;
  time_mousepressed.tv_usec = 0;
  x11_button_state = 0x23;
#ifdef F_HAVE_LIBGPM
  gpm_ev.x = -1;
#endif
  zero_point = new FPoint(0,0);
  event_queue = new std::deque<eventPair>;
  // init arrays with '\0'
  std::fill_n (k_buf, sizeof(k_buf), '\0');
  std::fill_n (fifo_buf, fifo_buf_size, '\0');
  std::fill_n (x11_mouse, sizeof(x11_mouse), '\0');
  std::fill_n (sgr_mouse, sizeof(sgr_mouse), '\0');
  std::fill_n (urxvt_mouse, sizeof(urxvt_mouse), '\0');
  // init bit field with 0
  memset(&b_state, 0x00, sizeof(b_state));
  // interpret the command line options
  cmd_options();
}

//----------------------------------------------------------------------
void FApplication::cmd_options ()
{
  while ( true )
  {
    int c, idx = 0;
    static struct option long_options[] =
    {
      {"encoding", required_argument, 0,  0 },
      {"vgafont",  no_argument,       0,  0 },
      {"newfont",  no_argument,       0,  0 },
      {0,          0,                 0,  0 }
    };
    opterr = 0;
    c = getopt_long ( app_argc
                    , app_argv
                    , ""
                    , long_options
                    , &idx );
    if ( c == -1 )
      break;

    if ( c == 0 )
    {
      if ( strcmp(long_options[idx].name, "encoding") == 0 )
      {
        FString encoding(optarg);
        encoding = encoding.toUpper();
        if (  encoding.includes("UTF8")
           || encoding.includes("VT100")
           || encoding.includes("PC")
           || encoding.includes("ASCII") )
        {
          setEncoding(encoding.c_str());
        }
      }
      if ( strcmp(long_options[idx].name, "vgafont")  == 0 )
        setVGAFont();
      if ( strcmp(long_options[idx].name, "newfont")  == 0 )
        setNewFont();
    }
  }
}

//----------------------------------------------------------------------
#ifdef F_HAVE_LIBGPM
int FApplication::gpmEvent (bool clear)
{
  register int result;
  register int max = (gpm_fd > stdin_no) ? gpm_fd : stdin_no;
  fd_set ifds;
  struct timeval tv;

  FD_ZERO(&ifds);
  FD_SET(stdin_no, &ifds);
  FD_SET(gpm_fd, &ifds);
  tv.tv_sec  = 0;
  tv.tv_usec = 100000;  // 100 ms
  result = select (max+1, &ifds, 0, 0, &tv);
  if ( FD_ISSET(stdin_no, &ifds) )
  {
    if ( clear )
      FD_CLR (stdin_no, &ifds);
    return keyboard_event;
  }
  if ( clear && FD_ISSET(gpm_fd, &ifds) )
    FD_CLR (gpm_fd, &ifds);
  if (result > 0)
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
  result = select (stdin_no+1, &ifds, 0, 0, &tv);
  if ( FD_ISSET(stdin_no, &ifds) )
    FD_CLR (stdin_no, &ifds);
  return (result > 0);
}

//----------------------------------------------------------------------
inline ssize_t FApplication::readKey()
{
  register ssize_t bytes;
  setNonBlockingInput();
  bytes = read(stdin_no, &k_buf, sizeof(k_buf)-1);
  unsetNonBlockingInput();
  return bytes;
}

//----------------------------------------------------------------------
void FApplication::processKeyboardEvent()
{
  bool isKeyPressed = false;
  FWidget* widget = 0;

  if ( focus_widget )
    widget = static_cast<FWidget*>(focus_widget);
  else
  {
    widget = static_cast<FWidget*>(main_widget);
    if ( widget->numOfChildren() >= 1 )
      widget->focusFirstChild();
  }

  // empty the buffer on timeout
  if ( fifo_in_use && widget->isKeyTimeout(&time_keypressed, key_timeout)  )
  {
    fifo_offset = 0;
    key = 0;
    std::fill_n (fifo_buf, fifo_buf_size, '\0');
    fifo_in_use = false;
  }

  flush_out();

#ifdef F_HAVE_LIBGPM
  if ( gpm_mouse_enabled )
  {
    gpmMouseEvent = false;
    int type = gpmEvent();
    switch ( type )
    {
      case mouse_event:
        gpmMouseEvent = true;
        break;

      case keyboard_event:
        isKeyPressed = true;

      default:
        break;
    }
  }
  else
    isKeyPressed = KeyPressed();
#else  // without gpm
  isKeyPressed = KeyPressed();
#endif  // F_HAVE_LIBGPM

  if ( isKeyPressed )
  {
    register ssize_t bytesread;
    widget->getCurrentTime(time_keypressed);
    x11_mouse[0] = sgr_mouse[0] = urxvt_mouse[0] = '\0';

    if ( quit_now || app_exit_loop )
      return;

    while ( (bytesread = readKey()) > 0 )
    {
      if ( bytesread + fifo_offset <= fifo_buf_size )
      {
        for (int i=0; i < bytesread; i++)
        {
          fifo_buf[fifo_offset] = k_buf[i];
          fifo_offset++;
        }
        fifo_in_use = true;
      }

      // read the rest from the fifo buffer
      while (  ! widget->isKeyTimeout(&time_keypressed, key_timeout)
            && fifo_offset > 0
            && key != NEED_MORE_DATA )
      {
        key = FTerm::parseKeyString(fifo_buf, fifo_buf_size, &time_keypressed);

        if ( key != NEED_MORE_DATA )
        {
          if ( key == 0x0c )  // Ctrl-L (redraw the screen)
            redraw();

          if ( key == fc::Fkey_mouse )
          {
            int n, len;
            x11_mouse[0] = fifo_buf[3];
            x11_mouse[1] = fifo_buf[4];
            x11_mouse[2] = fifo_buf[5];
            x11_mouse[3] = '\0';
            len = 6;
            n = len;

            for (; n < fifo_buf_size; n++)   // Remove founded entry
              fifo_buf[n-len] = fifo_buf[n];
            for (; n-len < len; n++)         // Fill rest with '\0'
              fifo_buf[n-len] = '\0';
            input_data_pending = bool(fifo_buf[0] != '\0');
            processMouseEvent();
          }
          else if ( key == fc::Fkey_extended_mouse )
          {
            int n = 3;
            int len = int(strlen(fifo_buf));

            while ( n < len && n < fifo_buf_size )
            {
              sgr_mouse[n-3] = fifo_buf[n];
              n++;
              if ( fifo_buf[n] == 'M' || fifo_buf[n] == 'm' )
                len = n + 1;
            }
            sgr_mouse[n-3] = '\0';
            for (n=len; n < fifo_buf_size; n++) // Remove founded entry
              fifo_buf[n-len] = fifo_buf[n];
            for (; n-len < len; n++)            // Fill rest with '\0'
              fifo_buf[n-len] = '\0';
            input_data_pending = bool(fifo_buf[0] != '\0');
            processMouseEvent();
          }
          else if ( key == fc::Fkey_urxvt_mouse )
          {
            int n = 2;
            int len = int(strlen(fifo_buf));

            while ( n < len && n < fifo_buf_size )
            {
              urxvt_mouse[n-2] = fifo_buf[n];
              n++;
              if ( fifo_buf[n] == 'M' || fifo_buf[n] == 'm' )
                len = n + 1;
            }
            urxvt_mouse[n-2] = '\0';
            for (n=len; n < fifo_buf_size; n++) // Remove founded entry
              fifo_buf[n-len] = fifo_buf[n];
            for (; n-len < len; n++)            // Fill rest with '\0'
              fifo_buf[n-len] = '\0';
            input_data_pending = bool(fifo_buf[0] != '\0');
            processMouseEvent();
          }
          else
          {
            // send key down event
            FKeyEvent k_down_ev (KeyDown_Event, key);
            sendEvent (widget, &k_down_ev);

            // send key press event
            FKeyEvent k_press_ev (KeyPress_Event, key);
            sendEvent (widget, &k_press_ev);

            if ( ! k_press_ev.isAccepted() && ! k_down_ev.isAccepted() )
            {
              // keyboard accelerator
              FWidget* window = static_cast<FWidget*>(active_window);
              if ( ! window )
                window = getRootWidget();
              if (  window
                 && window->accelerator_list
                 && ! window->accelerator_list->empty() )
              {
                FWidget::Accelerators::const_iterator iter, end;
                iter = window->accelerator_list->begin();
                end = window->accelerator_list->end();

                while ( iter != end )
                {
                  if ( quit_now || app_exit_loop )
                    break;
                  if ( iter->key == key )
                  {
                    FAccelEvent a_ev (Accelerator_Event, focus_widget);
                    sendEvent (iter->object, &a_ev);
                    break;
                  };
                  ++iter;
                }
              }
            }
          } // end of else
        }
        fifo_offset = int(strlen(fifo_buf));
      }
      // send key up event
      FKeyEvent k_up_ev (KeyUp_Event, key);
      sendEvent (widget, &k_up_ev);
      key = 0;
    }
    std::fill_n (k_buf, sizeof(k_buf), '\0');
  }
  // special case: Esc key
  if (  fifo_in_use
     && fifo_offset == 1
     && fifo_buf[0] == 0x1b
     && fifo_buf[1] == 0x00
     && isKeyTimeout(&time_keypressed, key_timeout) )
  {
    FKeyEvent k_press_ev (KeyPress_Event, fc::Fkey_escape);
    sendEvent (widget, &k_press_ev);
    input_data_pending = false;
  }
}

//----------------------------------------------------------------------
void FApplication::getX11ButtonState (int button)
{
  // get the x11 and urxvt mouse button state

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
      if (  *mouse == newMousePosition
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
  newMousePosition.setPoint(x,y);
  memset(&b_state, 0x00, sizeof(b_state));

  if ( (x11_mouse[0] & key_shift) == key_shift )
    b_state.shift_button = Pressed;
  if ( (x11_mouse[0] & key_meta) == key_meta )
    b_state.meta_button = Pressed;
  if ( (x11_mouse[0] & key_ctrl) == key_ctrl )
    b_state.control_button = Pressed;

  if (  (x11_mouse[0] & button_mask) >= button1_pressed_move
     && (x11_mouse[0] & button_mask) <= button3_pressed_move
     && *mouse != *zero_point )
  {
    b_state.mouse_moved = true;
  }

  getX11ButtonState (x11_mouse[0] & button_mask);

  if (  uChar(x11_mouse[1]) == mouse->getX() + 0x20
     && uChar(x11_mouse[2]) == mouse->getY() + 0x20
     && b_state.wheel_up != Pressed
     && b_state.wheel_down != Pressed
     && uChar(x11_mouse[0]) == x11_button_state )
  {
    return false;
  }
  mouse->setPoint(x,y);
  x11_button_state = uChar(x11_mouse[0]);
  x11_mouse[0] = '\0';
  return true;
}

//----------------------------------------------------------------------
bool FApplication::parseSGRMouse()
{
  register char* p;
  int button;
  uChar x, y;

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
    x = uChar(10 * x + (*p - '0'));
  }

  while ( *p++ && *p != 'M' && *p != 'm' )
  {
    if ( *p < '0' || *p > '9')
      return false;
    y = uChar(10 * y + (*p - '0'));
  }

  newMousePosition.setPoint(x,y);
  memset(&b_state, 0x00, sizeof(b_state));

  if ( (button & key_shift) == key_shift )
    b_state.shift_button = Pressed;
  if ( (button & key_meta) == key_meta )
    b_state.meta_button = Pressed;
  if ( (button & key_ctrl) == key_ctrl )
    b_state.control_button = Pressed;

  if (  (button & button_mask) >= button1_move
     && (button & button_mask) <= button3_move
     && *mouse != *zero_point )
  {
    b_state.mouse_moved = true;
  }

  if ( *p == pressed )
  {
    switch ( button & button_mask )
    {
      case button1:
      case button1_move:
        if (  *mouse == newMousePosition
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
  if (  *mouse == newMousePosition
      && b_state.wheel_up != Pressed
      && b_state.wheel_down != Pressed
      && x11_button_state == uChar(((*p & 0x20) << 2) + button) )
  {
    return false;
  }
  mouse->setPoint(x,y);
  x11_button_state = uChar(((*p & 0x20) << 2) + button);
  sgr_mouse[0] = '\0';
  return true;
}

//----------------------------------------------------------------------
bool FApplication::parseUrxvtMouse()
{
  register char* p;
  register bool x_neg;
  register bool y_neg;
  int button;
  uChar x, y;

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
    x = uChar(10 * x + (*p - '0'));
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
    y = uChar(10 * y + (*p - '0'));
    p++;
  }

  if ( x_neg || x == 0 )
    x = 1;
  if ( y_neg || y == 0 )
    y = 1;
  if ( x > term->getWidth() )
    x = uChar(term->getWidth());
  if ( y > term->getHeight() )
    y = uChar(term->getHeight());

  newMousePosition.setPoint(x,y);
  memset(&b_state, 0x00, sizeof(b_state));

  if ( (button & key_shift) == key_shift )
    b_state.shift_button = Pressed;
  if ( (button & key_meta) == key_meta )
    b_state.meta_button = Pressed;
  if ( (button & key_ctrl) == key_ctrl )
    b_state.control_button = Pressed;

  if (  (button & button_mask) >= button1_pressed_move
     && (button & button_mask) <= button3_pressed_move
     && *mouse != *zero_point )
  {
    b_state.mouse_moved = true;
  }

  getX11ButtonState (button & button_mask);

  if (  *mouse == newMousePosition
      && b_state.wheel_up != Pressed
      && b_state.wheel_down != Pressed
      && x11_button_state == uChar(button) )
  {
    return false;
  }
  mouse->setPoint(x,y);
  x11_button_state = uChar(button);
  urxvt_mouse[0] = '\0';
  return true;
}

//----------------------------------------------------------------------
#ifdef F_HAVE_LIBGPM
bool FApplication::processGpmEvent()
{
  memset(&b_state, 0x00, sizeof(b_state));

  if ( Gpm_GetEvent(&gpm_ev) == 1 )
  {
    Gpm_FitEvent (&gpm_ev);
    if ( gpm_ev.type & GPM_DRAG && gpm_ev.wdx == 0 && gpm_ev.wdy == 0 )
      b_state.mouse_moved = true;

    if ( gpm_ev.wdy > 0)
      b_state.wheel_up = Pressed;
    else if ( gpm_ev.wdy < 0)
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
    mouse->setPoint(gpm_ev.x, gpm_ev.y);

    if ( gpmEvent(false) == mouse_event )
      input_data_pending = true;
    else
      input_data_pending = false;

    GPM_DRAWPOINTER(&gpm_ev);
    gpmMouseEvent = false;

    return true;
  }
  gpmMouseEvent = false;
  return false;
}
#endif  // F_HAVE_LIBGPM

//----------------------------------------------------------------------
void FApplication::processMouseEvent()
{
  bool Event = false;

#ifdef F_HAVE_LIBGPM
  if (  ! gpmMouseEvent
     && x11_mouse[0] == '\0'
     && sgr_mouse[0] == '\0'
     && urxvt_mouse[0] == '\0' )
  {
    return;
  }
#else
  if (  x11_mouse[0] == '\0'
     && sgr_mouse[0] == '\0'
     && urxvt_mouse[0] == '\0' )
  {
    return;
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
    return;

  if (  ! clicked_widget
     && (  b_state.left_button == Pressed
        || b_state.left_button == DoubleClick
        || b_state.right_button == Pressed
        || b_state.middle_button == Pressed
        || b_state.wheel_up == Pressed
        || b_state.wheel_down == Pressed ) )
  {
    FWidget* window = FWindow::getWindowWidgetAt (*mouse);
    if ( window )
    {
      FWidget* child = childWidgetAt (window, *mouse);
      clicked_widget = (child != 0) ? child : window;
    }
  }

  if ( open_menu && ! b_state.mouse_moved )
  {
    FMenu* menu = static_cast<FMenu*>(open_menu);

    if ( ! menu->containsMenuStructure(*mouse) )
    {
      menu->unselectItemInList();
      menu->hide();
      menu->hideSubMenus();
      menu->hideSuperMenus();
    }
  }

  if ( clicked_widget )
  {
    FPoint localMousePos;
    int key_state = 0;

    if ( b_state.shift_button == Pressed )
      key_state |= ShiftButton;
    if ( b_state.meta_button == Pressed )
      key_state |= MetaButton;
    if ( b_state.control_button == Pressed )
      key_state |= ControlButton;

    localMousePos = clicked_widget->globalToLocalPos(*mouse);

    if ( b_state.mouse_moved )
    {
      if ( b_state.left_button == Pressed )
      {
        FMouseEvent m_down_ev ( MouseMove_Event
                              , localMousePos
                              , *mouse
                              , LeftButton | key_state );
        sendEvent (clicked_widget, &m_down_ev);
      }
      if ( b_state.right_button == Pressed )
      {
        FMouseEvent m_down_ev ( MouseMove_Event
                              , localMousePos
                              , *mouse
                              , RightButton | key_state );
        sendEvent (clicked_widget, &m_down_ev);
      }
      if ( b_state.middle_button == Pressed )
      {
        FMouseEvent m_down_ev ( MouseMove_Event
                              , localMousePos
                              , *mouse
                              , MiddleButton | key_state );
        sendEvent (clicked_widget, &m_down_ev);
      }
    }
    else
    {
      if ( b_state.left_button == DoubleClick )
      {
        FMouseEvent m_dblclick_ev ( MouseDoubleClick_Event
                                  , localMousePos
                                  , *mouse
                                  , LeftButton | key_state );
        sendEvent (clicked_widget, &m_dblclick_ev);
      }
      else if ( b_state.left_button == Pressed )
      {
        FMouseEvent m_down_ev ( MouseDown_Event
                              , localMousePos
                              , *mouse
                              , LeftButton | key_state );
        sendEvent (clicked_widget, &m_down_ev);
      }
      else if ( b_state.left_button == Released )
      {
        FMouseEvent m_up_ev ( MouseUp_Event
                            , localMousePos
                            , *mouse
                            , LeftButton | key_state );
        FWidget* released_widget = clicked_widget;
        if (  b_state.right_button != Pressed
           && b_state.middle_button != Pressed )
          clicked_widget = 0;
        sendEvent (released_widget, &m_up_ev);
      }

      if ( b_state.right_button == Pressed )
      {
        FMouseEvent m_down_ev ( MouseDown_Event
                              , localMousePos
                              , *mouse
                              , RightButton | key_state );
        sendEvent (clicked_widget, &m_down_ev);
      }
      else if ( b_state.right_button == Released )
      {
        FMouseEvent m_up_ev ( MouseUp_Event
                            , localMousePos
                            , *mouse
                            , RightButton | key_state );
        FWidget* released_widget = clicked_widget;
        if (  b_state.left_button != Pressed
           && b_state.middle_button != Pressed )
          clicked_widget = 0;
        sendEvent (released_widget, &m_up_ev);
      }

      if ( b_state.middle_button == Pressed )
      {
        FMouseEvent m_down_ev ( MouseDown_Event
                              , localMousePos
                              , *mouse
                              , MiddleButton | key_state );
        sendEvent (clicked_widget, &m_down_ev);
        // gnome-terminal sends no released on middle click
        if ( gnome_terminal )
          clicked_widget = 0;
      }
      else if ( b_state.middle_button == Released )
      {
        FMouseEvent m_up_ev ( MouseUp_Event
                            , localMousePos
                            , *mouse
                            , MiddleButton | key_state );
        FWidget* released_widget = clicked_widget;
        if (  b_state.right_button != Pressed
           && b_state.left_button != Pressed )
        clicked_widget = 0;
        sendEvent (released_widget, &m_up_ev);
      }
    }
    if ( b_state.wheel_up == Pressed )
    {
      FWheelEvent wheel_ev ( MouseWheel_Event
                           , localMousePos
                           , *mouse
                           , WheelUp );
      FWidget* scroll_over_widget = clicked_widget;
      clicked_widget = 0;
      sendEvent(scroll_over_widget, &wheel_ev);
    }
    if ( b_state.wheel_down == Pressed )
    {
      FWheelEvent wheel_ev ( MouseWheel_Event
                           , localMousePos
                           , *mouse
                           , WheelDown );
      FWidget* scroll_over_widget = clicked_widget;
      clicked_widget = 0;
      sendEvent (scroll_over_widget, &wheel_ev);
    }
  }

#ifdef F_HAVE_LIBGPM
  if ( gpm_mouse_enabled && gpm_ev.x != -1 )
    GPM_DRAWPOINTER(&gpm_ev);
#endif
}

//----------------------------------------------------------------------
void FApplication::processResizeEvent()
{
  if ( resize_term )
  {
    FResizeEvent r_ev(Resize_Event);
    sendEvent(rootObj, &r_ev);

    if ( r_ev.isAccepted() )
      resize_term = false;
  }
}

//----------------------------------------------------------------------
int FApplication::processTimerEvent()
{
  FObject::TimerList::iterator iter, end;
  timeval currentTime;
  int activated = 0;

  getCurrentTime (currentTime);

  if ( modify_timer )
    return 0;

  if ( ! timer_list )
    return 0;
  if ( timer_list->empty() )
    return 0;

  iter = timer_list->begin();
  end  = timer_list->end();

  while ( iter != end )
  {
    if (  (*iter).id == 0
       || (*iter).object == 0
       || currentTime < (*iter).timeout )  // no timer expired
      break;

    (*iter).timeout += (*iter).interval;

    if ( (*iter).timeout < currentTime )
      (*iter).timeout = currentTime + (*iter).interval;

    if ( (*iter).interval.tv_usec > 0 || (*iter).interval.tv_sec > 0 )
      activated++;

    FTimerEvent t_ev(Timer_Event, (*iter).id);
    sendEvent((*iter).object, &t_ev);

    ++iter;
  }

  return activated;
}

//----------------------------------------------------------------------
bool FApplication::processNextEvent()
{
  int num_events = 0;

  processKeyboardEvent();
  processMouseEvent();
  processResizeEvent();

  if ( terminal_update_pending )
  {
    if ( ! input_data_pending )
    {
      updateTerminal();
      terminal_update_pending = false;
      skipped_terminal_update = 0;
    }
    else if ( skipped_terminal_update > 8 )
    {
      force_terminal_update = true;
      updateTerminal();
      force_terminal_update = false;
      terminal_update_pending = false;
      skipped_terminal_update = 0;
    }
    else
      skipped_terminal_update++;
  }

  if ( close_widget && ! close_widget->empty() )
  {
    widgetList::iterator iter;
    iter = close_widget->begin();

    while ( iter != close_widget->end() )
    {
      delete *iter;
      ++iter;
    }
    close_widget->clear();
  }

  sendQueuedEvents();
  num_events += processTimerEvent();

  return (num_events > 0);
}


// public methods of FApplication
//----------------------------------------------------------------------
void FApplication::print_cmd_Options ()
{
  ::printf("\nFinalCut Options:\n"
    "  --encoding <name>           Sets the character encoding mode\n"
    "                              {UTF8, VT100, PC, ASCII}\n"
    "  --vgafont                   Set the standard vga 8x16 font\n"
    "  --newfont                   Enables the graphical font\n");
}

//----------------------------------------------------------------------
void FApplication::setMainWidget (FWidget* widget)
{
  main_widget = widget;

  if ( widget && ! getFocusWidget() )
    rootObj->focusFirstChild();
}

//----------------------------------------------------------------------
int FApplication::exec() // run
{
  quit_now = false;
  quit_code = 0;

  // set the cursor to the focus widget
  if (  getFocusWidget()
     && getFocusWidget()->isVisible()
     && getFocusWidget()->hasVisibleCursor() )
  {
    getFocusWidget()->setCursor();
    showCursor();
    flush_out();
  }
  enter_loop();
  return quit_code;
}

//----------------------------------------------------------------------
int FApplication::enter_loop() // event loop
{
  bool old_app_exit_loop;
  loop_level++;
  quit_now = false;

  old_app_exit_loop = app_exit_loop;
  app_exit_loop = false;
  while ( ! quit_now && ! app_exit_loop )
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
  if ( quit_now ) // don't overwrite quit code
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
bool FApplication::isQuit()
{
  if ( rootObj )
    return quit_now;
  else
    return true;
}

//----------------------------------------------------------------------
bool FApplication::sendEvent(FObject* receiver, FEvent* event)
{
  FWidget* widget;

  if ( quit_now || app_exit_loop )
    return false;

  if ( receiver == 0 )
    return false;

  widget = static_cast<FWidget*>(receiver);

  if ( modal_dialogs > 0 )
  {
    FWidget* window;
    if ( widget->isWindow() )
      window = widget;
    else
      window = FWindow::getWindowWidget(widget);

    // block events for widgets in non modal windows
    if ( window && (window->getFlags() & MODAL) == 0 )
    {
      switch ( event->type() )
      {
        case KeyPress_Event:
        case KeyUp_Event:
        case KeyDown_Event:
        case MouseDown_Event:
        case MouseUp_Event:
        case MouseDoubleClick_Event:
        case MouseWheel_Event:
        case MouseMove_Event:
        case FocusIn_Event:
        case FocusOut_Event:
        case Accelerator_Event:
          return false;

        default:
          break;
      }
    }
  }

  // throw away mouse events to disabled widgets
  if (  event->type() >= MouseDown_Event
     && event->type() <= MouseMove_Event
     && ! widget->isEnabled() )
    return false;

  // sends event event directly to receiver
  return widget->event(event);
}

//----------------------------------------------------------------------
void FApplication::queueEvent (FObject* receiver, FEvent* event)
{
  if ( receiver == 0 )
    return;

  // queue this event
  eventPair Event (receiver, event);
  event_queue->push_back(Event);
}

//----------------------------------------------------------------------
void FApplication::sendQueuedEvents()
{
  std::deque<eventPair>* events;

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
bool FApplication::removeQueuedEvent(FObject* receiver)
{
  bool retval;
  std::deque<eventPair>::iterator iter;

  if ( ! eventInQueue() )
    return false;
  if ( receiver == 0 )
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
