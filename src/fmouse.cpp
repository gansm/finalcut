/***********************************************************************
* fmouse.cpp - Read mouse events                                       *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2018 Markus Gans                                           *
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

#include <cstring>
#include <iostream>
#include <new>
#include <stdio.h>
#include <unistd.h>

#include "final/fmouse.h"
#include "final/fterm.h"
#include "final/ftermxterminal.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FMouse
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FMouse::FMouse()
  : b_state()
  , mouse_event_occurred(false)
  , input_data_pending(false)
  , dblclick_interval(500000)  // 500 ms
  , max_width(80)
  , max_height(25)
  , time_mousepressed()
  , zero_point(0, 0)           // zero point (x=0, y=0)
  , mouse(0, 0)                // mouse click position
  , new_mouse_position()
{
  time_mousepressed.tv_sec = 0;
  time_mousepressed.tv_usec = 0;

  // init bit field with 0
  std::memset(&b_state, 0x00, sizeof(b_state));
}


// public methods of FMouse
//----------------------------------------------------------------------
const char* FMouse::getClassName() const
{
  return "FMouse";
}

//----------------------------------------------------------------------
inline FPoint& FMouse::getPos()
{
  return mouse;
}

//----------------------------------------------------------------------
inline void FMouse::clearEvent()
{
  mouse_event_occurred = false;
}

//----------------------------------------------------------------------
inline void FMouse::setMaxWidth (short x_max)
{
  max_width = x_max;
}

//----------------------------------------------------------------------
inline void FMouse::setMaxHeight (short y_max)
{
  max_height = y_max;
}

//----------------------------------------------------------------------
inline void FMouse::setDblclickInterval (const long timeout)
{
  dblclick_interval = timeout;
}

//----------------------------------------------------------------------
inline bool FMouse::hasEvent()
{
  return mouse_event_occurred;
}

//----------------------------------------------------------------------
inline bool FMouse::isLeftButtonPressed()
{
  return bool(b_state.left_button == Pressed);
}

//----------------------------------------------------------------------
inline bool FMouse::isLeftButtonReleased()
{
  return bool(b_state.left_button == Released);
}

//----------------------------------------------------------------------
inline bool FMouse::isLeftButtonDoubleClick()
{
  return bool(b_state.left_button == DoubleClick);
}

//----------------------------------------------------------------------
inline bool FMouse::isRightButtonPressed()
{
  return bool(b_state.right_button == Pressed);
}

//----------------------------------------------------------------------
inline bool FMouse::isRightButtonReleased()
{
  return bool(b_state.right_button == Released);
}
//----------------------------------------------------------------------
inline bool FMouse::isMiddleButtonPressed()
{
  return bool(b_state.middle_button == Pressed);
}

//----------------------------------------------------------------------
inline bool FMouse::isMiddleButtonReleased()
{
  return bool(b_state.middle_button == Released);
}

//----------------------------------------------------------------------
inline bool FMouse::isShiftKeyPressed()
{
  return bool(b_state.shift_button);
}

//----------------------------------------------------------------------
inline bool FMouse::isControlKeyPressed()
{
  return bool(b_state.control_button);
}

//----------------------------------------------------------------------
inline bool FMouse::isMetaKeyPressed()
{
  return bool(b_state.meta_button);
}

//----------------------------------------------------------------------
inline bool FMouse::isWheelUp()
{
  return bool(b_state.wheel_up);
}

//----------------------------------------------------------------------
inline bool FMouse::isWheelDown()
{
  return bool(b_state.wheel_down);
}

//----------------------------------------------------------------------
inline bool FMouse::isMoved()
{
  return bool(b_state.mouse_moved);
}

//----------------------------------------------------------------------
inline bool FMouse::isInputDataPending()
{
  return input_data_pending;
}

//----------------------------------------------------------------------
inline FMouse* FMouse::createMouseObject (mouse_type mt)
{
  switch ( mt )
  {
    case none:
      return 0;

    case gpm:
#ifdef F_HAVE_LIBGPM
      return new FMouseGPM;
#else
      break;
#endif

    case x11:
      return new FMouseX11;

    case sgr:
      return new FMouseSGR;

    case urxvt:
      return new FMouseUrxvt;
  }

  return new FMouseX11;
}


// protected methods of FMouse
//----------------------------------------------------------------------
void FMouse::setPos (const FPoint& m)
{
  mouse = m;
}

//----------------------------------------------------------------------
bool FMouse::isDblclickTimeout (timeval* time)
{
  return FObject::isTimeout (time, dblclick_interval);
}


#ifdef F_HAVE_LIBGPM
//----------------------------------------------------------------------
// class FMouseGPM
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FMouseGPM::FMouseGPM()
  : FMouse()
  , gpm_ev()
  , has_gpm_mouse_data(false)
  , gpm_mouse_enabled(false)
  , stdin_no(0)
{
  gpm_ev.x = -1;
}

//----------------------------------------------------------------------
FMouseGPM::~FMouseGPM()  // destructor
{ }


// public methods of FMouseX11
//----------------------------------------------------------------------
const char* FMouseGPM::getClassName() const
{
  return "FMouseGPM";
}

//----------------------------------------------------------------------
void FMouseGPM::setStdinNo (int file_descriptor)
{
  stdin_no = file_descriptor;
}

//----------------------------------------------------------------------
bool FMouseGPM::hasData()
{
  return has_gpm_mouse_data;
}

//----------------------------------------------------------------------
void FMouseGPM::setRawData (FKeyboard::keybuffer&)
{ }

//----------------------------------------------------------------------
void FMouseGPM::processEvent (struct timeval*)
{
  // Fill bit field with 0
  std::memset(&b_state, 0x00, sizeof(b_state));

  if ( Gpm_GetEvent(&gpm_ev) == 1 )
  {
    Gpm_FitEvent (&gpm_ev);

    if ( gpm_ev.type & GPM_DRAG && gpm_ev.wdx == 0 && gpm_ev.wdy == 0 )
      b_state.mouse_moved = true;

    if ( gpm_ev.wdy > 0 )
      b_state.wheel_up = true;
    else if ( gpm_ev.wdy < 0 )
      b_state.wheel_down = true;

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
          b_state.wheel_up = true;

        if ( gpm_ev.buttons & GPM_B_DOWN )
          b_state.wheel_down = true;

        // Keyboard modifiers
        if ( gpm_ev.modifiers & (1 << KG_SHIFT) )
          b_state.shift_button = true;

        if ( gpm_ev.modifiers & ((1 << KG_ALT) | (1 << KG_ALTGR)) )
          b_state.meta_button = true;

        if ( gpm_ev.modifiers & (1 << KG_CTRL) )
          b_state.control_button = true;

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

    setPos (FPoint(gpm_ev.x, gpm_ev.y));

    if ( gpmEvent(false) == mouse_event )
      input_data_pending = true;
    else
      input_data_pending = false;

    GPM_DRAWPOINTER(&gpm_ev);
    has_gpm_mouse_data = false;
    mouse_event_occurred = true;
    return;
  }

  has_gpm_mouse_data = false;
  mouse_event_occurred = false;
}

//----------------------------------------------------------------------
bool FMouseGPM::gpmMouse (bool on)
{
  // activate/deactivate the gpm mouse support

  if ( on )
  {
    Gpm_Connect conn;
    conn.eventMask   = uInt16(~GPM_MOVE);
    conn.defaultMask = GPM_MOVE;
    conn.maxMod      = uInt16(~0);
    conn.minMod      = 0;
    Gpm_Open(&conn, 0);

    switch ( gpm_fd )
    {
      case -1:  // error
        return false;

      case -2:  // xterm is in use
        return false;

      default:
        break;
    }
  }
  else
  {
    Gpm_Close();
  }

  gpm_mouse_enabled = on;
  return on;
}

//----------------------------------------------------------------------
bool FMouseGPM::getGpmKeyPressed (bool pending)
{
  input_data_pending = pending;
  has_gpm_mouse_data = false;
  int type = gpmEvent();

  switch ( type )
  {
    case mouse_event:
      has_gpm_mouse_data = true;
      break;

    case keyboard_event:
      return true;

    default:
      return false;
  }

  return false;
}

//----------------------------------------------------------------------
void FMouseGPM::drawGpmPointer()
{
  if ( isGpmMouseEnabled() && gpm_ev.x != -1 )
    GPM_DRAWPOINTER(&gpm_ev);
}

//----------------------------------------------------------------------
int FMouseGPM::gpmEvent (bool clear)
{
  int result;
  int max = ( gpm_fd > stdin_no ) ? gpm_fd : stdin_no;
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
// class FMouseX11
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FMouseX11::FMouseX11()
  : FMouse()
  , x11_mouse()
  , x11_button_state(all_buttons_released)
{
  x11_mouse[0] = '\0';
}

//----------------------------------------------------------------------
FMouseX11::~FMouseX11()  // destructor
{ }


// public methods of FMouseX11
//----------------------------------------------------------------------
const char* FMouseX11::getClassName() const
{
  return "FMouseX11";
}

//----------------------------------------------------------------------
bool FMouseX11::hasData()
{
  return bool(x11_mouse[0]);
}

//----------------------------------------------------------------------
void FMouseX11::setRawData (FKeyboard::keybuffer& fifo_buf)
{
  // Import the X11 xterm mouse protocol (SGR-Mode) raw mouse data

  static const std::size_t len = 6;
  std::size_t fifo_buf_size = sizeof(fifo_buf);
  std::size_t n;
  x11_mouse[0] = fifo_buf[3];
  x11_mouse[1] = fifo_buf[4];
  x11_mouse[2] = fifo_buf[5];
  x11_mouse[3] = '\0';

  // Remove founded entry
  for (n = len; n < fifo_buf_size; n++)
    fifo_buf[n - len] = fifo_buf[n];

  n = fifo_buf_size - len;

  // Fill rest with '\0'
  for (; n < fifo_buf_size; n++)
    fifo_buf[n] = '\0';

  input_data_pending = bool(fifo_buf[0] != '\0');
}

//----------------------------------------------------------------------
void FMouseX11::processEvent (struct timeval* time)
{
  // Parse and interpret the X11 xterm mouse string

  const FPoint& mouse_position = getPos();
  uChar x, y;
  int btn;

  x = uChar(x11_mouse[1] - 0x20);
  y = uChar(x11_mouse[2] - 0x20);
  btn = x11_mouse[0];
  new_mouse_position.setPoint (x, y);
  // Fill bit field with 0
  std::memset(&b_state, 0x00, sizeof(b_state));
  setKeyState (btn);
  setMoveState (mouse_position, btn);
  setButtonState (btn & button_mask, time);

  if ( new_mouse_position == mouse_position
    && ! isWheelUp()
    && ! isWheelDown()
    && uChar(btn) == x11_button_state )
  {
    mouse_event_occurred = false;
    x11_mouse[0] = '\0';  // Delete already interpreted data
    return;
  }

  mouse_event_occurred = true;
  setPos (FPoint(x, y));
  // Get the button state from string
  x11_button_state = uChar(btn);
  // Delete already interpreted data
  x11_mouse[0] = '\0';
}


// private methods of FMouseX11
//----------------------------------------------------------------------
void FMouseX11::setKeyState (int btn)
{
  if ( (btn & key_shift) == key_shift )
    b_state.shift_button = Pressed;

  if ( (btn & key_meta) == key_meta )
    b_state.meta_button = Pressed;

  if ( (btn & key_ctrl) == key_ctrl )
    b_state.control_button = Pressed;
}

//----------------------------------------------------------------------
void FMouseX11::setMoveState (const FPoint& mouse_position, int btn)
{
  if ( (btn & button_mask) >= button1_pressed_move
    && (btn & button_mask) <= button3_pressed_move
    && mouse_position != zero_point )
  {
    b_state.mouse_moved = true;
  }
}

//----------------------------------------------------------------------
void FMouseX11::setButtonState (int btn, struct timeval* time)
{
  // Get the x11 mouse button state

  const FPoint& mouse_position = getPos();

  switch ( btn )
  {
    case button1_pressed:
    case button1_pressed_move:
      if ( mouse_position == new_mouse_position
        && x11_button_state == all_buttons_released
        && ! isDblclickTimeout(&time_mousepressed) )
      {
        time_mousepressed.tv_sec = 0;
        time_mousepressed.tv_usec = 0;
        b_state.left_button = DoubleClick;
      }
      else
      {
        time_mousepressed = *time;  // save click time
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
// class FMouseSGR
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FMouseSGR::FMouseSGR()
  : FMouse()
  , sgr_mouse()
  , sgr_button_state(0x23)
{
  sgr_mouse[0] = '\0';
}

//----------------------------------------------------------------------
FMouseSGR::~FMouseSGR()  // destructor
{ }


// public methods of FMouseSGR
//----------------------------------------------------------------------
const char* FMouseSGR::getClassName() const
{
  return "FMouseSGR";
}

//----------------------------------------------------------------------
bool FMouseSGR::hasData()
{
  return bool(sgr_mouse[0]);
}

//----------------------------------------------------------------------
void FMouseSGR::setRawData (FKeyboard::keybuffer& fifo_buf)
{
  // Import the X11 xterm mouse protocol (SGR-Mode) raw mouse data

  std::size_t fifo_buf_size = sizeof(fifo_buf);
  std::size_t len = std::strlen(fifo_buf);
  std::size_t n = 3;

  while ( n < len && n <= MOUSE_BUF_SIZE + 1 )
  {
    sgr_mouse[n - 3] = fifo_buf[n];
    n++;

    if ( fifo_buf[n] == 'M' || fifo_buf[n] == 'm' )
      len = n + 1;
  }

  sgr_mouse[n - 3] = '\0';

  for (n = len; n < fifo_buf_size; n++)  // Remove founded entry
    fifo_buf[n - len] = fifo_buf[n];

  n = fifo_buf_size - len;

  for (; n < fifo_buf_size; n++)       // Fill rest with '\0'
    fifo_buf[n] = '\0';

  input_data_pending = bool(fifo_buf[0] != '\0');
}

//----------------------------------------------------------------------
void FMouseSGR::processEvent (struct timeval* time)
{
  const FPoint& mouse_position = getPos();
  char* p;
  int btn;
  short x, y;

  x = 0;
  y = 0;
  btn = 0;

  // parse the SGR mouse string
  p = sgr_mouse;

  while ( *p && *p != ';' )
  {
    if ( *p < '0' || *p > '9')
    {
      mouse_event_occurred = false;
      sgr_mouse[0] = '\0';  // Delete already interpreted data
      return;
    }

    btn = 10 * btn + (*p - '0');
    p++;
  }

  while ( *p++ && *p != ';' )
  {
    if ( *p < '0' || *p > '9')
    {
      mouse_event_occurred = false;
      sgr_mouse[0] = '\0';  // Delete already interpreted data
      return;
    }

    x = short(10 * x + (*p - '0'));
  }

  while ( *p++ && *p != 'M' && *p != 'm' )
  {
    if ( *p < '0' || *p > '9')
    {
      mouse_event_occurred = false;
      sgr_mouse[0] = '\0';  // Delete already interpreted data
      return;
    }

    y = short(10 * y + (*p - '0'));
  }

  new_mouse_position.setPoint (x, y);
  // Fill bit field with 0
  std::memset(&b_state, 0x00, sizeof(b_state));
  setKeyState (btn);
  setMoveState (mouse_position, btn);

  if ( *p == pressed )
    setPressedButtonState (btn & button_mask, time);
  else  // *p == released
    setReleasedButtonState (btn & button_mask);

  if ( mouse_position == new_mouse_position
    && ! isWheelUp()
    && ! isWheelDown()
    && sgr_button_state == uChar(((*p & 0x20) << 2) + btn) )
  {
    mouse_event_occurred = false;
    sgr_mouse[0] = '\0';  // Delete already interpreted data
    return;
  }

  mouse_event_occurred = true;
  setPos (FPoint(x, y));
  // Get the button state from string
  sgr_button_state = uChar(((*p & 0x20) << 2) + btn);
  // Delete already interpreted data
  sgr_mouse[0] = '\0';
}

// private methods of FMouseSGR
//----------------------------------------------------------------------
void FMouseSGR::setKeyState (int btn)
{
  if ( (btn & key_shift) == key_shift )
    b_state.shift_button = true;

  if ( (btn & key_meta) == key_meta )
    b_state.meta_button = true;

  if ( (btn & key_ctrl) == key_ctrl )
    b_state.control_button = true;
}

//----------------------------------------------------------------------
void FMouseSGR::setMoveState (const FPoint& mouse_position, int btn)
{
  if ( (btn & button_mask) >= button1_move
    && (btn & button_mask) <= button3_move
    && mouse_position != zero_point )
  {
    b_state.mouse_moved = true;
  }
}

//----------------------------------------------------------------------
void FMouseSGR::setPressedButtonState (int btn, struct timeval* time)
{
  // Gets the extended x11 mouse mode (SGR) status for pressed buttons

  const FPoint& mouse_position = getPos();

  switch ( btn )
  {
    case button1:
    case button1_move:
      if ( mouse_position == new_mouse_position
        && (((sgr_button_state & 0x80) >> 2) + 'M') == released
        && ! isDblclickTimeout(&time_mousepressed) )
      {
        time_mousepressed.tv_sec = 0;
        time_mousepressed.tv_usec = 0;
        b_state.left_button = DoubleClick;
      }
      else
      {
        time_mousepressed = *time;  // save click time
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
      b_state.wheel_up = true;
      break;

    case button_down:
      time_mousepressed.tv_sec = 0;
      time_mousepressed.tv_usec = 0;
      b_state.wheel_down = true;
      break;

    default:
      break;
  }
}

//----------------------------------------------------------------------
void FMouseSGR::setReleasedButtonState (int btn)
{
  // Gets the extended x11 mouse mode (SGR) status for released buttons

  switch ( btn )
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


//----------------------------------------------------------------------
// class FMouseUrxvt
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FMouseUrxvt::FMouseUrxvt()
  : FMouse()
  , urxvt_mouse()
  , urxvt_button_state(0x23)
{
  urxvt_mouse[0] = '\0';
}

//----------------------------------------------------------------------
FMouseUrxvt::~FMouseUrxvt()  // destructor
{ }


// public methods of FMouseUrxvt
//----------------------------------------------------------------------
const char* FMouseUrxvt::getClassName() const
{
  return "FMouseUrxvt";
}

//----------------------------------------------------------------------
bool FMouseUrxvt::hasData()
{
  return bool(urxvt_mouse[0]);
}

//----------------------------------------------------------------------
void FMouseUrxvt::setRawData (FKeyboard::keybuffer& fifo_buf)
{
  // Import the X11 xterm mouse protocol (Urxvt-Mode) raw mouse data

  std::size_t fifo_buf_size = sizeof(fifo_buf);
  std::size_t len = std::strlen(fifo_buf);
  std::size_t n = 2;

  while ( n < len && n <= MOUSE_BUF_SIZE )
  {
    urxvt_mouse[n - 2] = fifo_buf[n];
    n++;

    if ( fifo_buf[n] == 'M' || fifo_buf[n] == 'm' )
      len = n + 1;
  }

  urxvt_mouse[n - 2] = '\0';

  for (n = len; n < fifo_buf_size; n++)  // Remove founded entry
    fifo_buf[n - len] = fifo_buf[n];

  n = fifo_buf_size - len;

  for (; n < fifo_buf_size; n++)       // Fill rest with '\0'
    fifo_buf[n] = '\0';

  input_data_pending = bool(fifo_buf[0] != '\0');
}

//----------------------------------------------------------------------
void FMouseUrxvt::processEvent (struct timeval* time)
{
  // Parse and interpret the X11 xterm mouse string (Urxvt-Mode)

  const FPoint& mouse_position = getPos();
  char* p;
  bool x_neg;
  bool y_neg;
  int btn;
  short x, y;

  x = 0;
  y = 0;
  btn = 0;

  // Parse the Urxvt mouse string
  p = urxvt_mouse;
  x_neg = false;
  y_neg = false;

  while ( *p && *p != ';' )
  {
    if ( *p < '0' || *p > '9')
    {
      mouse_event_occurred = false;
      urxvt_mouse[0] = '\0';  // Delete already interpreted data
      return;
    }

    btn = 10 * btn + (*p - '0');
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
    {
      mouse_event_occurred = false;
      urxvt_mouse[0] = '\0';  // Delete already interpreted data
      return;
    }

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
    {
      mouse_event_occurred = false;
      urxvt_mouse[0] = '\0';  // Delete already interpreted data
      return;
    }

    y = short(10 * y + (*p - '0'));
    p++;
  }

  if ( x_neg || x == 0 )
    x = 1;

  if ( y_neg || y == 0 )
    y = 1;

  if ( x > max_width )
    x = max_width;

  if ( y > max_height )
    y = max_height;

  new_mouse_position.setPoint (x, y);
  // Fill bit field with 0
  std::memset(&b_state, 0x00, sizeof(b_state));
  setKeyState (btn);
  setMoveState (mouse_position, btn);
  setButtonState (btn & button_mask, time);

  if ( mouse_position == new_mouse_position
    && ! isWheelUp()
    && ! isWheelDown()
    && urxvt_button_state == uChar(btn) )
  {
    mouse_event_occurred = false;
    urxvt_mouse[0] = '\0';  // Delete already interpreted data
    return;
  }

  mouse_event_occurred = true;
  setPos (FPoint(x, y));
  urxvt_button_state = uChar(btn);
  // Delete already interpreted data
  urxvt_mouse[0] = '\0';
}


// private methods of FMouseUrxvt
//----------------------------------------------------------------------
void FMouseUrxvt::setKeyState (int btn)
{
  if ( (btn & key_shift) == key_shift )
    b_state.shift_button = Pressed;

  if ( (btn & key_meta) == key_meta )
    b_state.meta_button = Pressed;

  if ( (btn & key_ctrl) == key_ctrl )
    b_state.control_button = Pressed;
}

//----------------------------------------------------------------------
void FMouseUrxvt::setMoveState (const FPoint& mouse_position, int btn)
{
  if ( (btn & button_mask) >= button1_pressed_move
    && (btn & button_mask) <= button3_pressed_move
    && mouse_position != zero_point )
  {
    b_state.mouse_moved = true;
  }
}

//----------------------------------------------------------------------
void FMouseUrxvt::setButtonState (int btn, struct timeval* time)
{
  // Get the urxvt mouse button state

  const FPoint& mouse_position = getPos();

  switch ( btn )
  {
    case button1_pressed:
    case button1_pressed_move:
      if ( mouse_position == new_mouse_position
        && urxvt_button_state == all_buttons_released
        && ! isDblclickTimeout(&time_mousepressed) )
      {
        time_mousepressed.tv_sec = 0;
        time_mousepressed.tv_usec = 0;
        b_state.left_button = DoubleClick;
      }
      else
      {
        time_mousepressed = *time;  // save click time
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
      switch ( urxvt_button_state & button_mask )
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
// class FMouseControl
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FMouseControl::FMouseControl()
  : mouse_protocol()
  , iter()
  , zero_point(0, 0)
  , use_gpm_mouse(false)
  , use_xterm_mouse(false)
{
#ifdef F_HAVE_LIBGPM
  mouse_protocol[FMouse::gpm] = FMouse::createMouseObject(FMouse::gpm);
#endif

  mouse_protocol[FMouse::x11] = FMouse::createMouseObject(FMouse::x11);
  mouse_protocol[FMouse::sgr] = FMouse::createMouseObject(FMouse::sgr);
  mouse_protocol[FMouse::urxvt] = FMouse::createMouseObject(FMouse::urxvt);
}

//----------------------------------------------------------------------
FMouseControl::~FMouseControl()  // destructor
{
  iter = mouse_protocol.begin();

  while ( iter != mouse_protocol.end() )
  {
    if ( iter->second )
      delete iter->second;

    ++iter;
  }
}


// public methods of FMouseControl
//----------------------------------------------------------------------
FPoint& FMouseControl::getPos()
{
  FMouse* mouse_object = getMouseWithEvent();

  if ( mouse_object )
    return mouse_object->getPos();

  return zero_point;
}

//----------------------------------------------------------------------
void FMouseControl::clearEvent()
{
  FMouse* mouse_object;

  while ( (mouse_object = getMouseWithEvent()) != 0 )
    mouse_object->clearEvent();
}

//----------------------------------------------------------------------
#ifdef F_HAVE_LIBGPM
void FMouseControl::setStdinNo (int file_descriptor)
{
  FMouse* mouse = mouse_protocol[FMouse::gpm];
  FMouseGPM* gpm_mouse = static_cast<FMouseGPM*>(mouse);

  if ( gpm_mouse )
    gpm_mouse->setStdinNo(file_descriptor);
}
#else
void FMouseControl::setStdinNo (int)
{ }
#endif  // F_HAVE_LIBGPM

//----------------------------------------------------------------------
void FMouseControl::setMaxWidth (short x_max)
{
  mouse_protocol[FMouse::urxvt]->setMaxWidth(x_max);
}

//----------------------------------------------------------------------
void FMouseControl::setMaxHeight (short y_max)
{
  mouse_protocol[FMouse::urxvt]->setMaxHeight(y_max);
}

//----------------------------------------------------------------------
void FMouseControl::setDblclickInterval (const long timeout)
{
  iter = mouse_protocol.begin();

  while ( iter != mouse_protocol.end() )
  {
    if ( iter->second )
      iter->second->setDblclickInterval(timeout);

    ++iter;
  }
}

//----------------------------------------------------------------------
void FMouseControl::useGpmMouse (bool on)
{
  use_gpm_mouse = on;
}

//----------------------------------------------------------------------
void FMouseControl::useXtermMouse (bool on)
{
  use_xterm_mouse = on;
}

//----------------------------------------------------------------------
bool FMouseControl::hasData()
{
  FMouse* mouse_object = getMouseWithData();

  if ( mouse_object )  // with data
    return true;

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::hasEvent()
{
  FMouse* mouse_object = getMouseWithEvent();

  if ( mouse_object )  // with event
    return true;

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::isLeftButtonPressed()
{
  FMouse* mouse_object = getMouseWithEvent();

  if ( mouse_object )
    return mouse_object->isLeftButtonPressed();

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::isLeftButtonReleased()
{
  FMouse* mouse_object = getMouseWithEvent();

  if ( mouse_object )
    return mouse_object->isLeftButtonReleased();

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::isLeftButtonDoubleClick()
{
  FMouse* mouse_object = getMouseWithEvent();

  if ( mouse_object )
    return mouse_object->isLeftButtonDoubleClick();

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::isRightButtonPressed()
{
  FMouse* mouse_object = getMouseWithEvent();

  if ( mouse_object )
    return mouse_object->isRightButtonPressed();

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::isRightButtonReleased()
{
  FMouse* mouse_object = getMouseWithEvent();

  if ( mouse_object )
    return mouse_object->isRightButtonReleased();

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::isMiddleButtonPressed()
{
  FMouse* mouse_object = getMouseWithEvent();

  if ( mouse_object )
    return mouse_object->isMiddleButtonPressed();

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::isMiddleButtonReleased()
{
  FMouse* mouse_object = getMouseWithEvent();

  if ( mouse_object )
    return mouse_object->isMiddleButtonReleased();

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::isShiftKeyPressed()
{
  FMouse* mouse_object = getMouseWithEvent();

  if ( mouse_object )
    return mouse_object->isShiftKeyPressed();

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::isControlKeyPressed()
{
  FMouse* mouse_object = getMouseWithEvent();

  if ( mouse_object )
    return mouse_object->isControlKeyPressed();

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::isMetaKeyPressed()
{
  FMouse* mouse_object = getMouseWithEvent();

  if ( mouse_object )
    return mouse_object->isMetaKeyPressed();

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::isWheelUp()
{
  FMouse* mouse_object = getMouseWithEvent();

  if ( mouse_object )
    return mouse_object->isWheelUp();

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::isWheelDown()
{
  FMouse* mouse_object = getMouseWithEvent();

  if ( mouse_object )
    return mouse_object->isWheelDown();

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::isMoved()
{
  FMouse* mouse_object = getMouseWithEvent();

  if ( mouse_object )
    return mouse_object->isMoved();

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::isInputDataPending()
{
  iter = mouse_protocol.begin();

  while ( iter != mouse_protocol.end() )
  {
    if ( iter->second && iter->second->isInputDataPending() )
      return true;

    ++iter;
  }

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::isGpmMouseEnabled()
{
  if ( mouse_protocol.empty() )
    return false;

#ifdef F_HAVE_LIBGPM
  FMouse* mouse = mouse_protocol[FMouse::gpm];
  FMouseGPM* gpm_mouse = static_cast<FMouseGPM*>(mouse);

  if ( gpm_mouse )
    return gpm_mouse->isGpmMouseEnabled();
#endif  // F_HAVE_LIBGPM

  return false;
}

//----------------------------------------------------------------------
void FMouseControl::enable()
{
#ifdef F_HAVE_LIBGPM
  if ( use_gpm_mouse )
  {
    FMouse* mouse = mouse_protocol[FMouse::gpm];
    FMouseGPM* gpm_mouse = static_cast<FMouseGPM*>(mouse);

    if ( gpm_mouse )
      use_gpm_mouse = gpm_mouse->enableGpmMouse();
  }
#endif  // F_HAVE_LIBGPM

  if ( use_xterm_mouse )
    enableXTermMouse();
}

//----------------------------------------------------------------------
void FMouseControl::disable()
{
#ifdef F_HAVE_LIBGPM
  if ( use_gpm_mouse )
  {
    FMouse* mouse = mouse_protocol[FMouse::gpm];
    FMouseGPM* gpm_mouse = static_cast<FMouseGPM*>(mouse);

    if ( gpm_mouse )
      gpm_mouse->disableGpmMouse();
  }
#endif  // F_HAVE_LIBGPM

  if ( use_xterm_mouse )
    disableXTermMouse();
}

//----------------------------------------------------------------------
void FMouseControl::setRawData ( FMouse::mouse_type mt
                               , FKeyboard::keybuffer& fifo_buf)
{
  FMouse* mouse = mouse_protocol[mt];

  if ( mouse )
    mouse->setRawData (fifo_buf);
}

//----------------------------------------------------------------------
void FMouseControl::processEvent (struct timeval* time)
{
  FMouse* mouse_object = getMouseWithData();
  // Clear all old mouse events
  clearEvent();

  if ( mouse_object )
    mouse_object->processEvent(time);
}

//----------------------------------------------------------------------
#ifdef F_HAVE_LIBGPM

bool FMouseControl::getGpmKeyPressed (bool pending)
#else

bool FMouseControl::getGpmKeyPressed (bool)
#endif  // F_HAVE_LIBGPM
{
  if ( mouse_protocol.empty() )
    return false;

#ifdef F_HAVE_LIBGPM
  FMouse* mouse = mouse_protocol[FMouse::gpm];
  FMouseGPM* gpm_mouse = static_cast<FMouseGPM*>(mouse);

  if ( gpm_mouse )
    return gpm_mouse->getGpmKeyPressed(pending);
#endif  // F_HAVE_LIBGPM

  return false;
}

//----------------------------------------------------------------------
void FMouseControl::drawGpmPointer()
{
  if ( mouse_protocol.empty() )
    return;

#ifdef F_HAVE_LIBGPM

  FMouse* mouse = mouse_protocol[FMouse::gpm];
  FMouseGPM* gpm_mouse = static_cast<FMouseGPM*>(mouse);

  if ( gpm_mouse )
    gpm_mouse->drawGpmPointer();

#endif  // F_HAVE_LIBGPM
}


// private methods of FMouseControl
//----------------------------------------------------------------------
FMouse* FMouseControl::getMouseWithData()
{
  iter = mouse_protocol.begin();

  while ( iter != mouse_protocol.end() )
  {
    if ( iter->second && iter->second->hasData() )
      return iter->second;

    ++iter;
  }

  return 0;
}

//----------------------------------------------------------------------
FMouse* FMouseControl::getMouseWithEvent()
{
  iter = mouse_protocol.begin();

  while ( iter != mouse_protocol.end() )
  {
    if ( iter->second && iter->second->hasEvent() )
      return iter->second;

    ++iter;
  }

  return 0;
}

//----------------------------------------------------------------------
void FMouseControl::xtermMouse (bool on)
{
  // activate/deactivate the xterm mouse support

  if ( ! use_xterm_mouse )
    return;

  FTermXTerminal::setMouseSupport (on);
}

}  // namespace finalcut
