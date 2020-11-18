/***********************************************************************
* fmouse.cpp - Read mouse events                                       *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2020 Markus Gans                                      *
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

#include <stdio.h>
#include <unistd.h>

#include <cstring>
#include <algorithm>
#include <iostream>
#include <new>

#include "final/fapplication.h"
#include "final/fconfig.h"
#include "final/fkeyboard.h"
#include "final/fmouse.h"
#include "final/fobject.h"
#include "final/fterm.h"
#include "final/ftermxterminal.h"
#include "final/ftypes.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FMouseData
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FMouseData::FMouseData()
{ }

//----------------------------------------------------------------------
FMouseData::~FMouseData()
{ }

// public methods of FMouseData
//----------------------------------------------------------------------
FString FMouseData::getClassName() const
{
  return "FMouseData";
}

//----------------------------------------------------------------------
const FPoint& FMouseData::getPos() const
{
  return mouse;
}
//----------------------------------------------------------------------
bool FMouseData::isLeftButtonPressed() const
{
  return bool(getButtonState().left_button == Pressed);
}

//----------------------------------------------------------------------
bool FMouseData::isLeftButtonReleased() const
{
  return bool(getButtonState().left_button == Released);
}

//----------------------------------------------------------------------
bool FMouseData::isLeftButtonDoubleClick() const
{
  return bool(getButtonState().left_button == DoubleClick);
}

//----------------------------------------------------------------------
bool FMouseData::isRightButtonPressed() const
{
  return bool(getButtonState().right_button == Pressed);
}

//----------------------------------------------------------------------
bool FMouseData::isRightButtonReleased() const
{
  return bool(getButtonState().right_button == Released);
}
//----------------------------------------------------------------------
bool FMouseData::isMiddleButtonPressed() const
{
  return bool(getButtonState().middle_button == Pressed);
}

//----------------------------------------------------------------------
bool FMouseData::isMiddleButtonReleased() const
{
  return bool(getButtonState().middle_button == Released);
}

//----------------------------------------------------------------------
bool FMouseData::isShiftKeyPressed() const
{
  return bool(getButtonState().shift_button);
}

//----------------------------------------------------------------------
bool FMouseData::isControlKeyPressed() const
{
  return bool(getButtonState().control_button);
}

//----------------------------------------------------------------------
bool FMouseData::isMetaKeyPressed() const
{
  return bool(getButtonState().meta_button);
}

//----------------------------------------------------------------------
bool FMouseData::isWheelUp() const
{
  return bool(getButtonState().wheel_up);
}

//----------------------------------------------------------------------
bool FMouseData::isWheelDown() const
{
  return bool(getButtonState().wheel_down);
}

//----------------------------------------------------------------------
bool FMouseData::isMoved() const
{
  return bool(getButtonState().mouse_moved);
}

//----------------------------------------------------------------------
void FMouseData::clearButtonState()
{
  // Fill bit field with 0
  std::memset(&b_state, 0x00, sizeof(b_state));
}


// protected methods of FMouseData
//----------------------------------------------------------------------
inline FMouseData::FMouseButton& FMouseData::getButtonState()
{
  return b_state;
}

//----------------------------------------------------------------------
inline const FMouseData::FMouseButton& FMouseData::getButtonState() const
{
  return b_state;
}

//----------------------------------------------------------------------
void FMouseData::setPos (const FPoint& m)
{
  mouse = m;
}


//----------------------------------------------------------------------
// class FMouse
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FMouse::FMouse()
{
  resetMousePressedTime();
  clearButtonState();
}

//----------------------------------------------------------------------
FMouse::~FMouse()  // destructor
{ }


// public methods of FMouse
//----------------------------------------------------------------------
FString FMouse::getClassName() const
{
  return "FMouse";
}

//----------------------------------------------------------------------
inline void FMouse::clearEvent()
{
  mouse_event_occurred = false;
}

//----------------------------------------------------------------------
inline void FMouse::setMaxWidth (uInt16 x_max)
{
  max_width = x_max;
}

//----------------------------------------------------------------------
inline void FMouse::setMaxHeight (uInt16 y_max)
{
  max_height = y_max;
}

//----------------------------------------------------------------------
inline void FMouse::setDblclickInterval (const uInt64 timeout)
{
  dblclick_interval = timeout;
}

//----------------------------------------------------------------------
inline bool FMouse::hasEvent() const
{
  return mouse_event_occurred;
}

//----------------------------------------------------------------------
inline bool FMouse::hasUnprocessedInput() const
{
  return unprocessed_buffer_data;
}


// protected methods of FMouse
//----------------------------------------------------------------------
inline const FPoint& FMouse::getNewPos() const
{
  return new_mouse_position;
}

//----------------------------------------------------------------------
uInt16 FMouse::getMaxWidth() const
{
  return max_width;
}

//----------------------------------------------------------------------
uInt16 FMouse::getMaxHeight() const
{
  return max_height;
}

//----------------------------------------------------------------------
uInt64 FMouse::getDblclickInterval() const
{
  return dblclick_interval;
}

//----------------------------------------------------------------------
timeval* FMouse::getMousePressedTime()
{
  return &time_mousepressed;
}

//----------------------------------------------------------------------
void FMouse::setNewPos (int x, int y)
{
  new_mouse_position.setPoint (x, y);
}

//----------------------------------------------------------------------
void FMouse::setPending (bool is_pending)
{
  unprocessed_buffer_data = is_pending;
}

//----------------------------------------------------------------------
void FMouse::setMousePressedTime (const timeval* time)
{
  time_mousepressed = *time;
}

//----------------------------------------------------------------------
void FMouse::resetMousePressedTime()
{
  time_mousepressed.tv_sec = 0;
  time_mousepressed.tv_usec = 0;
}

//----------------------------------------------------------------------
void FMouse::setEvent()
{
  mouse_event_occurred = true;
}

//----------------------------------------------------------------------
bool FMouse::isDblclickTimeout (const timeval* time) const
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
  : FMouse{}
{
  gpm_ev.x = -1;
}

//----------------------------------------------------------------------
FMouseGPM::~FMouseGPM()  // destructor
{ }


// public methods of FMouseX11
//----------------------------------------------------------------------
FString FMouseGPM::getClassName() const
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
{
  // This method need not be implemented for FMouseGPM
}

//----------------------------------------------------------------------
void FMouseGPM::processEvent (struct timeval*)
{
  clearButtonState();

  if ( Gpm_GetEvent(&gpm_ev) == 1 )
  {
    Gpm_FitEvent (&gpm_ev);
    GPM_DRAWPOINTER(&gpm_ev);

    if ( ! hasSignificantEvents() )
    {
      has_gpm_mouse_data = false;
      clearEvent();
      return;
    }

    if ( gpm_ev.type & GPM_DRAG && gpm_ev.wdx == 0 && gpm_ev.wdy == 0 )
      getButtonState().mouse_moved = true;

    if ( gpm_ev.wdy > 0 )
      getButtonState().wheel_up = true;
    else if ( gpm_ev.wdy < 0 )
      getButtonState().wheel_down = true;

    switch ( gpm_ev.type & 0x0f )
    {
      case GPM_DOWN:
      case GPM_DRAG:
        interpretKeyDown();
        break;

      case GPM_UP:
        interpretKeyUp();

      default:
        break;
    }

    setPos (FPoint{ std::max(gpm_ev.x, sInt16(1))
                  , std::max(gpm_ev.y, sInt16(1)) });

    if ( gpmEvent(false) == mouse_event )
      setPending(true);
    else
      setPending(false);

    has_gpm_mouse_data = false;
    setEvent();
    return;
  }
  else
    gpm_fd = -1;

  has_gpm_mouse_data = false;
  clearEvent();
}

//----------------------------------------------------------------------
bool FMouseGPM::gpmMouse (bool enable)
{
  // activate/deactivate the gpm mouse support

  static constexpr int gpm_error = -1;
  static constexpr int gpm_xterm_is_in_use = -2;

  if ( enable )
  {
    Gpm_Connect conn;
    conn.eventMask   = GPM_MOVE | GPM_DRAG | GPM_DOWN | GPM_UP;
    conn.defaultMask = 0;
    conn.maxMod      = 0;
    conn.minMod      = 0;
    Gpm_Open(&conn, 0);

    if ( gpm_fd == gpm_error || gpm_fd == gpm_xterm_is_in_use )
      return false;
  }
  else
  {
    Gpm_Close();
  }

  gpm_mouse_enabled = enable;
  return enable;
}

//----------------------------------------------------------------------
bool FMouseGPM::hasSignificantEvents() const
{
  return ! (gpm_ev.type & GPM_MOVE)
      || gpm_ev.wdy != 0
      || gpm_ev.buttons & GPM_B_UP
      || gpm_ev.buttons & GPM_B_DOWN;
}

//----------------------------------------------------------------------
void FMouseGPM::interpretKeyDown()
{
  if ( gpm_ev.buttons & GPM_B_LEFT )
  {
    if ( gpm_ev.type & GPM_DOUBLE )
      getButtonState().left_button = DoubleClick;
    else
      getButtonState().left_button = Pressed;
  }

  if ( gpm_ev.buttons & GPM_B_MIDDLE )
    getButtonState().middle_button = Pressed;

  if ( gpm_ev.buttons & GPM_B_RIGHT )
    getButtonState().right_button = Pressed;

  if ( gpm_ev.buttons & GPM_B_UP )
    getButtonState().wheel_up = true;

  if ( gpm_ev.buttons & GPM_B_DOWN )
    getButtonState().wheel_down = true;

  // Keyboard modifiers
  if ( gpm_ev.modifiers & (1 << KG_SHIFT) )
    getButtonState().shift_button = true;

  if ( gpm_ev.modifiers & ((1 << KG_ALT) | (1 << KG_ALTGR)) )
    getButtonState().meta_button = true;

  if ( gpm_ev.modifiers & (1 << KG_CTRL) )
    getButtonState().control_button = true;
}

//----------------------------------------------------------------------
void FMouseGPM::interpretKeyUp()
{
  if ( gpm_ev.buttons & GPM_B_LEFT )
    getButtonState().left_button = Released;

  if ( gpm_ev.buttons & GPM_B_MIDDLE )
    getButtonState().middle_button = Released;

  if ( gpm_ev.buttons & GPM_B_RIGHT )
    getButtonState().right_button = Released;
}

//----------------------------------------------------------------------
bool FMouseGPM::getGpmKeyPressed (bool is_pending)
{
  setPending(is_pending);
  has_gpm_mouse_data = false;
  const int type = gpmEvent();

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
void FMouseGPM::drawPointer() const
{
  if ( isGpmMouseEnabled() && gpm_ev.x != -1 )
    GPM_DRAWPOINTER(&gpm_ev);
}

//----------------------------------------------------------------------
int FMouseGPM::gpmEvent (bool clear) const
{
  const int max = ( gpm_fd > stdin_no ) ? gpm_fd : stdin_no;
  fd_set ifds{};
  struct timeval tv{};

  FD_ZERO(&ifds);
  FD_SET(stdin_no, &ifds);
  FD_SET(gpm_fd, &ifds);
  tv.tv_sec  = 0;
  tv.tv_usec = suseconds_t(FKeyboard::getReadBlockingTime());  // preset to 100 ms
  const int result = select (max + 1, &ifds, nullptr, nullptr, &tv);

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

// public methods of FMouseX11
//----------------------------------------------------------------------
FString FMouseX11::getClassName() const
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

  static constexpr std::size_t len = 6;
  const std::size_t fifo_buf_size{sizeof(fifo_buf)};
  std::size_t n{};
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

  setPending(bool(fifo_buf[0] != '\0'));
}

//----------------------------------------------------------------------
void FMouseX11::processEvent (struct timeval* time)
{
  // Parse and interpret the X11 xterm mouse string

  const auto& mouse_position = getPos();
  const auto x = uChar(x11_mouse[1] - 0x20);
  const auto y = uChar(x11_mouse[2] - 0x20);
  const int btn = x11_mouse[0];
  setNewPos (x, y);
  clearButtonState();
  setKeyState (btn);
  setMoveState (mouse_position, btn);
  setButtonState (btn & button_mask, time);

  if ( mouse_position == getNewPos()
    && ! isWheelUp()
    && ! isWheelDown()
    && uChar(btn) == x11_button_state )
  {
    clearEvent();
    x11_mouse[0] = '\0';  // Delete already interpreted data
    return;
  }

  setEvent();
  setPos (FPoint{x, y});
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
    getButtonState().shift_button = Pressed;

  if ( (btn & key_meta) == key_meta )
    getButtonState().meta_button = Pressed;

  if ( (btn & key_ctrl) == key_ctrl )
    getButtonState().control_button = Pressed;
}

//----------------------------------------------------------------------
void FMouseX11::setMoveState (const FPoint& mouse_position, int btn)
{
  if ( (btn & button_mask) >= button1_pressed_move
    && (btn & button_mask) <= button3_pressed_move
    && ! mouse_position.isOrigin() )
  {
    getButtonState().mouse_moved = true;
  }
}

//----------------------------------------------------------------------
void FMouseX11::setButtonState (const int btn, const struct timeval* time)
{
  // Get the x11 mouse button state

  const auto& mouse_position = getPos();

  switch ( btn )
  {
    case button1_pressed:
    case button1_pressed_move:
      if ( mouse_position == getNewPos()
        && x11_button_state == all_buttons_released
        && ! isDblclickTimeout(getMousePressedTime()) )
      {
        resetMousePressedTime();
        getButtonState().left_button = DoubleClick;
      }
      else
      {
        setMousePressedTime (time);  // save click time
        getButtonState().left_button = Pressed;
      }
      break;

    case button2_pressed:
    case button2_pressed_move:
      resetMousePressedTime();
      getButtonState().middle_button = Pressed;
      break;

    case button3_pressed:
    case button3_pressed_move:
      resetMousePressedTime();
      getButtonState().right_button = Pressed;
      break;

    case all_buttons_released:
      switch ( x11_button_state & button_mask )
      {
        case button1_pressed:
        case button1_pressed_move:
          getButtonState().left_button = Released;
          break;

        case button2_pressed:
        case button2_pressed_move:
          getButtonState().middle_button = Released;
          break;

        case button3_pressed:
        case button3_pressed_move:
          getButtonState().right_button = Released;
          break;

        default:
          break;
      }
      break;

    case button_up:
      resetMousePressedTime();
      getButtonState().wheel_up = Pressed;
      break;

    case button_down:
      resetMousePressedTime();
      getButtonState().wheel_down = Pressed;
      break;

      default:
        break;
  }
}


//----------------------------------------------------------------------
// class FMouseSGR
//----------------------------------------------------------------------

// public methods of FMouseSGR
//----------------------------------------------------------------------
FString FMouseSGR::getClassName() const
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

  const std::size_t fifo_buf_size = sizeof(fifo_buf);
  std::size_t len = std::strlen(fifo_buf);
  std::size_t n{3};

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

  setPending(bool(fifo_buf[0] != '\0'));
}

//----------------------------------------------------------------------
void FMouseSGR::processEvent (struct timeval* time)
{
  const auto& mouse_position = getPos();
  uInt16 x{0};
  uInt16 y{0};
  int btn{0};

  // parse the SGR mouse string
  const char* p = sgr_mouse;

  while ( *p && *p != ';' )
  {
    if ( *p < '0' || *p > '9')
    {
      clearEvent();
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
      clearEvent();
      sgr_mouse[0] = '\0';  // Delete already interpreted data
      return;
    }

    x = uInt16(10 * x + (*p - '0'));
  }

  while ( *p++ && *p != 'M' && *p != 'm' )
  {
    if ( *p < '0' || *p > '9')
    {
      clearEvent();
      sgr_mouse[0] = '\0';  // Delete already interpreted data
      return;
    }

    y = uInt16(10 * y + (*p - '0'));
  }

  setNewPos (x, y);
  clearButtonState();
  setKeyState (btn);
  setMoveState (mouse_position, btn);

  if ( *p == pressed )
    setPressedButtonState (btn & button_mask, time);
  else  // *p == released
    setReleasedButtonState (btn & button_mask);

  if ( mouse_position == getNewPos()
    && ! isWheelUp()
    && ! isWheelDown()
    && sgr_button_state == uChar(((*p & 0x20) << 2) + btn) )
  {
    clearEvent();
    sgr_mouse[0] = '\0';  // Delete already interpreted data
    return;
  }

  setEvent();
  setPos (FPoint{x, y});
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
    getButtonState().shift_button = true;

  if ( (btn & key_meta) == key_meta )
    getButtonState().meta_button = true;

  if ( (btn & key_ctrl) == key_ctrl )
    getButtonState().control_button = true;
}

//----------------------------------------------------------------------
void FMouseSGR::setMoveState (const FPoint& mouse_position, int btn)
{
  if ( (btn & button_mask) >= button1_move
    && (btn & button_mask) <= button3_move
    && ! mouse_position.isOrigin() )
  {
    getButtonState().mouse_moved = true;
  }
}

//----------------------------------------------------------------------
void FMouseSGR::setPressedButtonState ( const int btn
                                      , const struct timeval* time )
{
  // Gets the extended x11 mouse mode (SGR) status for pressed buttons

  const auto& mouse_position = getPos();

  switch ( btn )
  {
    case button1:
    case button1_move:
      if ( mouse_position == getNewPos()
        && (((sgr_button_state & 0x80) >> 2) + 'M') == released
        && ! isDblclickTimeout(getMousePressedTime()) )
      {
        resetMousePressedTime();
        getButtonState().left_button = DoubleClick;
      }
      else
      {
        setMousePressedTime (time);  // save click time
        getButtonState().left_button = Pressed;
      }
      break;

    case button2:
    case button2_move:
      resetMousePressedTime();
      getButtonState().middle_button = Pressed;
      break;

    case button3:
    case button3_move:
      resetMousePressedTime();
      getButtonState().right_button = Pressed;
      break;

    case button_up:
      resetMousePressedTime();
      getButtonState().wheel_up = true;
      break;

    case button_down:
      resetMousePressedTime();
      getButtonState().wheel_down = true;
      break;

    default:
      break;
  }
}

//----------------------------------------------------------------------
void FMouseSGR::setReleasedButtonState (const int btn)
{
  // Gets the extended x11 mouse mode (SGR) status for released buttons

  switch ( btn )
  {
    case button1:
    case button1_move:
      getButtonState().left_button = Released;
      break;

    case button2:
    case button2_move:
      getButtonState().middle_button = Released;
      break;

    case button3:
    case button3_move:
      getButtonState().right_button = Released;
      break;

    default:
      break;
  }
}


//----------------------------------------------------------------------
// class FMouseUrxvt
//----------------------------------------------------------------------

// public methods of FMouseUrxvt
//----------------------------------------------------------------------
FString FMouseUrxvt::getClassName() const
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

  const std::size_t fifo_buf_size = sizeof(fifo_buf);
  std::size_t len = std::strlen(fifo_buf);
  std::size_t n{2};

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

  setPending(bool(fifo_buf[0] != '\0'));
}

//----------------------------------------------------------------------
void FMouseUrxvt::processEvent (struct timeval* time)
{
  // Parse and interpret the X11 xterm mouse string (Urxvt-Mode)

  const auto& mouse_position = getPos();
  uInt16 x{0};
  uInt16 y{0};
  int btn{0};

  // Parse the Urxvt mouse string
  const char* p = urxvt_mouse;
  bool x_neg{false};
  bool y_neg{false};

  while ( *p && *p != ';' )
  {
    if ( *p < '0' || *p > '9')
    {
      clearEvent();
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
      clearEvent();
      urxvt_mouse[0] = '\0';  // Delete already interpreted data
      return;
    }

    x = uInt16(10 * x + (*p - '0'));
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
      clearEvent();
      urxvt_mouse[0] = '\0';  // Delete already interpreted data
      return;
    }

    y = uInt16(10 * y + (*p - '0'));
    p++;
  }

  if ( x_neg || x == 0 )
    x = 1;

  if ( y_neg || y == 0 )
    y = 1;

  if ( x > getMaxWidth() )
    x = getMaxWidth();

  if ( y > getMaxHeight() )
    y = getMaxHeight();

  setNewPos (x, y);
  clearButtonState();
  setKeyState (btn);
  setMoveState (mouse_position, btn);
  setButtonState (btn & button_mask, time);

  if ( mouse_position == getNewPos()
    && ! isWheelUp()
    && ! isWheelDown()
    && urxvt_button_state == uChar(btn) )
  {
    clearEvent();
    urxvt_mouse[0] = '\0';  // Delete already interpreted data
    return;
  }

  setEvent();
  setPos (FPoint{x, y});
  urxvt_button_state = uChar(btn);
  // Delete already interpreted data
  urxvt_mouse[0] = '\0';
}


// private methods of FMouseUrxvt
//----------------------------------------------------------------------
void FMouseUrxvt::setKeyState (int btn)
{
  if ( (btn & key_shift) == key_shift )
    getButtonState().shift_button = Pressed;

  if ( (btn & key_meta) == key_meta )
    getButtonState().meta_button = Pressed;

  if ( (btn & key_ctrl) == key_ctrl )
    getButtonState().control_button = Pressed;
}

//----------------------------------------------------------------------
void FMouseUrxvt::setMoveState (const FPoint& mouse_position, int btn)
{
  if ( (btn & button_mask) >= button1_pressed_move
    && (btn & button_mask) <= button3_pressed_move
    && ! mouse_position.isOrigin() )
  {
    getButtonState().mouse_moved = true;
  }
}

//----------------------------------------------------------------------
void FMouseUrxvt::setButtonState (const int btn, const struct timeval* time)
{
  // Get the urxvt mouse button state

  const auto& mouse_position = getPos();

  switch ( btn )
  {
    case button1_pressed:
    case button1_pressed_move:
      if ( mouse_position == getNewPos()
        && urxvt_button_state == all_buttons_released
        && ! isDblclickTimeout(getMousePressedTime()) )
      {
        resetMousePressedTime();
        getButtonState().left_button = DoubleClick;
      }
      else
      {
        setMousePressedTime (time);  // save click time
        getButtonState().left_button = Pressed;
      }
      break;

    case button2_pressed:
    case button2_pressed_move:
      resetMousePressedTime();
      getButtonState().middle_button = Pressed;
      break;

    case button3_pressed:
    case button3_pressed_move:
      resetMousePressedTime();
      getButtonState().right_button = Pressed;
      break;

    case all_buttons_released:
      switch ( urxvt_button_state & button_mask )
      {
        case button1_pressed:
        case button1_pressed_move:
          getButtonState().left_button = Released;
          break;

        case button2_pressed:
        case button2_pressed_move:
          getButtonState().middle_button = Released;
          break;

        case button3_pressed:
        case button3_pressed_move:
          getButtonState().right_button = Released;
          break;

        default:
          break;
      }
      break;

    case button_up:
      resetMousePressedTime();
      getButtonState().wheel_up = Pressed;
      break;

    case button_down:
      resetMousePressedTime();
      getButtonState().wheel_down = Pressed;
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
{
#ifdef F_HAVE_LIBGPM
  mouse_protocol[FMouse::gpm] = FMouse::createMouseObject<FMouseGPM>();
#endif

  mouse_protocol[FMouse::x11] = FMouse::createMouseObject<FMouseX11>();
  mouse_protocol[FMouse::sgr] = FMouse::createMouseObject<FMouseSGR>();
  mouse_protocol[FMouse::urxvt] = FMouse::createMouseObject<FMouseUrxvt>();
}

//----------------------------------------------------------------------
FMouseControl::~FMouseControl()  // destructor
{
  for (auto&& m : mouse_protocol)
    delete m.second;
}


// public methods of FMouseControl
//----------------------------------------------------------------------
const FPoint& FMouseControl::getPos()
{
  const auto& mouse_object = getMouseWithEvent();

  if ( mouse_object )
    return mouse_object->getPos();

  return zero_point;
}

//----------------------------------------------------------------------
void FMouseControl::clearEvent()
{
  FMouse* mouse_object;

  while ( (mouse_object = getMouseWithEvent()) != nullptr )
    mouse_object->clearEvent();
}

//----------------------------------------------------------------------
#ifdef F_HAVE_LIBGPM
void FMouseControl::setStdinNo (int file_descriptor)
{
  auto mouse = mouse_protocol[FMouse::gpm];
  auto gpm_mouse = static_cast<FMouseGPM*>(mouse);

  if ( gpm_mouse )
    gpm_mouse->setStdinNo(file_descriptor);
}
#else
void FMouseControl::setStdinNo (int)
{ }
#endif  // F_HAVE_LIBGPM

//----------------------------------------------------------------------
void FMouseControl::setMaxWidth (uInt16 x_max)
{
  mouse_protocol[FMouse::urxvt]->setMaxWidth(x_max);
}

//----------------------------------------------------------------------
void FMouseControl::setMaxHeight (uInt16 y_max)
{
  mouse_protocol[FMouse::urxvt]->setMaxHeight(y_max);
}

//----------------------------------------------------------------------
void FMouseControl::setDblclickInterval (const uInt64 timeout)
{
  for (auto&& m : mouse_protocol)
    if ( m.second )
      m.second->setDblclickInterval(timeout);
}

//----------------------------------------------------------------------
void FMouseControl::useGpmMouse (bool enable)
{
  use_gpm_mouse = enable;
}

//----------------------------------------------------------------------
void FMouseControl::useXtermMouse (bool enable)
{
  use_xterm_mouse = enable;
}

//----------------------------------------------------------------------
bool FMouseControl::hasData()
{
  const auto& mouse_object = getMouseWithData();

  if ( mouse_object )  // with data
    return true;

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::hasEvent()
{
  const auto& mouse_object = getMouseWithEvent();

  if ( mouse_object )  // with event
    return true;

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::isLeftButtonPressed()
{
  const auto& mouse_object = getMouseWithEvent();

  if ( mouse_object )
    return mouse_object->isLeftButtonPressed();

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::isLeftButtonReleased()
{
  const auto& mouse_object = getMouseWithEvent();

  if ( mouse_object )
    return mouse_object->isLeftButtonReleased();

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::isLeftButtonDoubleClick()
{
  const auto& mouse_object = getMouseWithEvent();

  if ( mouse_object )
    return mouse_object->isLeftButtonDoubleClick();

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::isRightButtonPressed()
{
  const auto& mouse_object = getMouseWithEvent();

  if ( mouse_object )
    return mouse_object->isRightButtonPressed();

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::isRightButtonReleased()
{
  const auto& mouse_object = getMouseWithEvent();

  if ( mouse_object )
    return mouse_object->isRightButtonReleased();

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::isMiddleButtonPressed()
{
  const auto& mouse_object = getMouseWithEvent();

  if ( mouse_object )
    return mouse_object->isMiddleButtonPressed();

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::isMiddleButtonReleased()
{
  const auto& mouse_object = getMouseWithEvent();

  if ( mouse_object )
    return mouse_object->isMiddleButtonReleased();

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::isShiftKeyPressed()
{
  const auto& mouse_object = getMouseWithEvent();

  if ( mouse_object )
    return mouse_object->isShiftKeyPressed();

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::isControlKeyPressed()
{
  const auto& mouse_object = getMouseWithEvent();

  if ( mouse_object )
    return mouse_object->isControlKeyPressed();

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::isMetaKeyPressed()
{
  const auto& mouse_object = getMouseWithEvent();

  if ( mouse_object )
    return mouse_object->isMetaKeyPressed();

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::isWheelUp()
{
  const auto& mouse_object = getMouseWithEvent();

  if ( mouse_object )
    return mouse_object->isWheelUp();

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::isWheelDown()
{
  const auto& mouse_object = getMouseWithEvent();

  if ( mouse_object )
    return mouse_object->isWheelDown();

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::isMoved()
{
  const auto& mouse_object = getMouseWithEvent();

  if ( mouse_object )
    return mouse_object->isMoved();

  return false;
}

//----------------------------------------------------------------------
bool FMouseControl::hasUnprocessedInput()
{
  return std::any_of ( std::begin(mouse_protocol)
                     , std::end(mouse_protocol)
                     , [] (FMouseProtocol::const_reference m)
                       {
                         return m.second
                             && m.second->hasUnprocessedInput();
                       }
                     );
}

//----------------------------------------------------------------------
#ifdef F_HAVE_LIBGPM
bool FMouseControl::isGpmMouseEnabled()
{
  if ( mouse_protocol.empty() )
    return false;

  const auto& mouse = mouse_protocol[FMouse::gpm];
  const auto& gpm_mouse = static_cast<FMouseGPM*>(mouse);

  if ( gpm_mouse )
    return gpm_mouse->isGpmMouseEnabled();

  return false;
}
#else  // F_HAVE_LIBGPM
bool FMouseControl::isGpmMouseEnabled()
{
  return false;
}
#endif  // F_HAVE_LIBGPM

//----------------------------------------------------------------------
void FMouseControl::enable()
{
#ifdef F_HAVE_LIBGPM
  if ( use_gpm_mouse )
  {
    auto mouse = mouse_protocol[FMouse::gpm];
    auto gpm_mouse = static_cast<FMouseGPM*>(mouse);

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
    auto mouse = mouse_protocol[FMouse::gpm];
    auto gpm_mouse = static_cast<FMouseGPM*>(mouse);

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
  auto mouse = mouse_protocol[mt];

  if ( mouse )
    mouse->setRawData (fifo_buf);
}

//----------------------------------------------------------------------
void FMouseControl::processQueuedInput()
{
  while ( ! fmousedata_queue.empty() )
  {
    if ( FApplication::isQuit() )
      return;

    FMouseDataPtr md(std::move(fmousedata_queue.front()));
    fmousedata_queue.pop();

    if ( md.get() )
      event_cmd.execute(*md);

    if ( FApplication::isQuit() )
      return;
  }
}

//----------------------------------------------------------------------
void FMouseControl::processEvent (struct timeval* time)
{
  auto mouse_object = getMouseWithData();
  // Clear all old mouse events
  clearEvent();

  if ( mouse_object )
  {
    mouse_object->processEvent(time);
    auto& md = static_cast<FMouseData&>(*mouse_object);
    fmousedata_queue.emplace(new FMouseData(std::move(md)));
  }
}

//----------------------------------------------------------------------
#ifdef F_HAVE_LIBGPM
bool FMouseControl::getGpmKeyPressed (bool pending)
{
  if ( mouse_protocol.empty() )
    return false;

  auto mouse = mouse_protocol[FMouse::gpm];
  auto gpm_mouse = static_cast<FMouseGPM*>(mouse);

  if ( gpm_mouse )
    return gpm_mouse->getGpmKeyPressed(pending);

  return false;
}
#else  // F_HAVE_LIBGPM
bool FMouseControl::getGpmKeyPressed (bool)
{
  return false;
}
#endif  // F_HAVE_LIBGPM

//----------------------------------------------------------------------
#ifdef F_HAVE_LIBGPM
void FMouseControl::drawPointer()
{
  if ( mouse_protocol.empty() )
    return;

  auto mouse = mouse_protocol[FMouse::gpm];
  auto gpm_mouse = static_cast<FMouseGPM*>(mouse);

  if ( gpm_mouse )
    gpm_mouse->drawPointer();
}
#else  // F_HAVE_LIBGPM
void FMouseControl::drawPointer()
{ }
#endif  // F_HAVE_LIBGPM


// private methods of FMouseControl
//----------------------------------------------------------------------
FMouse* FMouseControl::getMouseWithData()
{
  const auto& iter = \
      std::find_if ( std::begin(mouse_protocol)
                   , std::end(mouse_protocol)
                   , [] (FMouseProtocol::const_reference m)
                     {
                       return m.second
                           && m.second->hasData();
                     }
                   );

  return ( iter != mouse_protocol.end() ) ? iter->second : nullptr;
}

//----------------------------------------------------------------------
FMouse* FMouseControl::getMouseWithEvent()
{
  const auto& iter = \
      std::find_if ( std::begin(mouse_protocol)
                   , std::end(mouse_protocol)
                   , [] (FMouseProtocol::const_reference m)
                     {
                       return m.second
                           && m.second->hasEvent();
                     }
                   );

  return ( iter != mouse_protocol.end() ) ? iter->second : nullptr;
}

//----------------------------------------------------------------------
void FMouseControl::xtermMouse (bool enable) const
{
  // activate/deactivate the xterm mouse support

  if ( ! use_xterm_mouse )
    return;

  FTermXTerminal::setMouseSupport (enable);
}

}  // namespace finalcut
