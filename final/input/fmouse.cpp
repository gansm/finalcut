/***********************************************************************
* fmouse.cpp - Read mouse events                                       *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2023 Markus Gans                                      *
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

#if defined(__CYGWIN__)
  #include "final/fconfig.h"  // includes _GNU_SOURCE for fd_set
#endif

#include <cstdio>
#include <cstring>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <new>

#include "final/fapplication.h"
#include "final/fconfig.h"
#include "final/fobject.h"
#include "final/ftypes.h"
#include "final/input/fkeyboard.h"
#include "final/input/fmouse.h"
#include "final/output/tty/fterm.h"
#include "final/output/tty/ftermlinux.h"
#include "final/output/tty/ftermxterminal.h"

namespace finalcut
{

// Function prototypes
template <typename NumT, typename UnaryPredicate>
auto parseNumberIf (const char*&, NumT&, UnaryPredicate) -> bool;

// non-member functions
//----------------------------------------------------------------------
template <typename NumT, typename UnaryPredicate>
inline auto parseNumberIf ( const char*& p
                          , NumT& number
                          , UnaryPredicate up ) -> bool
{
  while ( *p && up(*p) )
  {
    if ( ! std::isdigit(*p) )
      return false;

    number = 10 * number + (*p - '0');
    p++;
  }

  return true;
}

//----------------------------------------------------------------------
// class FMouseData
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FMouseData::~FMouseData() noexcept = default;  // destructor


// public methods of FMouseData
//----------------------------------------------------------------------
auto FMouseData::getClassName() const -> FString
{
  return "FMouseData";
}

//----------------------------------------------------------------------
auto FMouseControl::getCurrentMouseEvent() -> FMouseDataPtr&
{
  static const auto& current_mouse_event = std::make_unique<FMouseDataPtr>();
  return *current_mouse_event;
}

//----------------------------------------------------------------------
auto FMouseData::getPos() const & noexcept -> const FPoint&
{
  return mouse;
}

//----------------------------------------------------------------------
auto FMouseData::isLeftButtonPressed() const noexcept -> bool
{
  return getButtonState().left_button == State::Pressed;
}

//----------------------------------------------------------------------
auto FMouseData::isLeftButtonReleased() const noexcept -> bool
{
  return getButtonState().left_button == State::Released;
}

//----------------------------------------------------------------------
auto FMouseData::isLeftButtonDoubleClick() const noexcept -> bool
{
  return getButtonState().left_button == State::DoubleClick;
}

//----------------------------------------------------------------------
auto FMouseData::isRightButtonPressed() const noexcept -> bool
{
  return getButtonState().right_button == State::Pressed;
}

//----------------------------------------------------------------------
auto FMouseData::isRightButtonReleased() const noexcept -> bool
{
  return getButtonState().right_button == State::Released;
}
//----------------------------------------------------------------------
auto FMouseData::isMiddleButtonPressed() const noexcept -> bool
{
  return getButtonState().middle_button == State::Pressed;
}

//----------------------------------------------------------------------
auto FMouseData::isMiddleButtonReleased() const noexcept -> bool
{
  return getButtonState().middle_button == State::Released;
}

//----------------------------------------------------------------------
auto FMouseData::isShiftKeyPressed() const noexcept -> bool
{
  return getButtonState().shift_button;
}

//----------------------------------------------------------------------
auto FMouseData::isControlKeyPressed() const noexcept -> bool
{
  return getButtonState().control_button;
}

//----------------------------------------------------------------------
auto FMouseData::isMetaKeyPressed() const noexcept -> bool
{
  return getButtonState().meta_button;
}

//----------------------------------------------------------------------
auto FMouseData::isWheelUp() const noexcept -> bool
{
  return getButtonState().wheel_up;
}

//----------------------------------------------------------------------
auto FMouseData::isWheelDown() const noexcept -> bool
{
  return getButtonState().wheel_down;
}

//----------------------------------------------------------------------
auto FMouseData::isWheelLeft() const noexcept -> bool
{
  return getButtonState().wheel_left;
}

//----------------------------------------------------------------------
auto FMouseData::isWheelRight() const noexcept -> bool
{
  return getButtonState().wheel_right;
}

//----------------------------------------------------------------------
auto FMouseData::isMoved() const noexcept -> bool
{
  return getButtonState().mouse_moved;
}

//----------------------------------------------------------------------
void FMouseData::clearButtonState() noexcept
{
  b_state.left_button    = State::Undefined;
  b_state.right_button   = State::Undefined;
  b_state.middle_button  = State::Undefined;
  b_state.shift_button   = false;
  b_state.control_button = false;
  b_state.meta_button    = false;
  b_state.wheel_up       = false;
  b_state.wheel_down     = false;
  b_state.wheel_left     = false;
  b_state.wheel_right    = false;
  b_state.mouse_moved    = false;
}


// protected methods of FMouseData
//----------------------------------------------------------------------
inline auto FMouseData::getButtonState() & noexcept -> FMouseButton&
{
  return b_state;
}

//----------------------------------------------------------------------
inline auto FMouseData::getButtonState() const & noexcept -> const FMouseButton&
{
  return b_state;
}

//----------------------------------------------------------------------
void FMouseData::setPos (const FPoint& m) noexcept
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


// public methods of FMouse
//----------------------------------------------------------------------
auto FMouse::getClassName() const -> FString
{
  return "FMouse";
}

//----------------------------------------------------------------------
auto FMouse::getMouseTypeID() const noexcept -> MouseType
{
  return MouseType_id;
}

//----------------------------------------------------------------------
void FMouse::clearEvent() noexcept
{
  mouse_event_occurred = false;
}

//----------------------------------------------------------------------
void FMouse::setMaxWidth (uInt16 x_max) noexcept
{
  max_width = x_max;
}

//----------------------------------------------------------------------
void FMouse::setMaxHeight (uInt16 y_max) noexcept
{
  max_height = y_max;
}

//----------------------------------------------------------------------
void FMouse::setDblclickInterval (const uInt64 timeout) noexcept
{
  dblclick_interval = timeout;
}

//----------------------------------------------------------------------
auto FMouse::hasEvent() const noexcept -> bool
{
  return mouse_event_occurred;
}

//----------------------------------------------------------------------
auto FMouse::hasUnprocessedInput() const noexcept -> bool
{
  return unprocessed_buffer_data;
}


// protected methods of FMouse
//----------------------------------------------------------------------
auto FMouse::getNewPos() const & noexcept -> const FPoint&
{
  return new_mouse_position;
}

//----------------------------------------------------------------------
auto FMouse::getMaxWidth() const noexcept -> uInt16
{
  return max_width;
}

//----------------------------------------------------------------------
auto FMouse::getMaxHeight() const noexcept -> uInt16
{
  return max_height;
}

//----------------------------------------------------------------------
auto FMouse::getDblclickInterval() const noexcept -> uInt64
{
  return dblclick_interval;
}

//----------------------------------------------------------------------
auto FMouse::getMousePressedTime() const noexcept -> TimeValue
{
  return time_mousepressed;
}

//----------------------------------------------------------------------
void FMouse::setMouseTypeID (MouseType mt) noexcept
{
  MouseType_id = mt;
}

//----------------------------------------------------------------------
void FMouse::setNewPos (int x, int y) noexcept
{
  new_mouse_position.setPoint (x, y);
}

//----------------------------------------------------------------------
void FMouse::useNewPos() noexcept
{
  setPos(new_mouse_position);
}

//----------------------------------------------------------------------
void FMouse::setPending (bool is_pending) noexcept
{
  unprocessed_buffer_data = is_pending;
}

//----------------------------------------------------------------------
void FMouse::setMousePressedTime (const TimeValue& time) noexcept
{
  time_mousepressed = time;
}

//----------------------------------------------------------------------
void FMouse::resetMousePressedTime() noexcept
{
  time_mousepressed = TimeValue{};  // Set to epoch time
}

//----------------------------------------------------------------------
void FMouse::setEvent() noexcept
{
  mouse_event_occurred = true;
}

//----------------------------------------------------------------------
auto FMouse::isDblclickTimeout (const TimeValue& time) const -> bool
{
  return FObjectTimer::isTimeout (time, dblclick_interval);
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
  setMouseTypeID (FMouse::MouseType::Gpm);
}


// public methods of FMouseX11
//----------------------------------------------------------------------
auto FMouseGPM::getClassName() const -> FString
{
  return "FMouseGPM";
}

//----------------------------------------------------------------------
void FMouseGPM::setStdinNo (int file_descriptor) noexcept
{
  stdin_no = file_descriptor;
}

//----------------------------------------------------------------------
auto FMouseGPM::hasData() noexcept -> bool
{
  return has_gpm_mouse_data;
}

//----------------------------------------------------------------------
void FMouseGPM::setRawData (FKeyboard::keybuffer&) noexcept
{
  // This method need not be implemented for FMouseGPM
}

//----------------------------------------------------------------------
void FMouseGPM::processEvent (const TimeValue&)
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

    if ( gpm_ev.wdx > 0 )
      getButtonState().wheel_right = true;
    else if ( gpm_ev.wdx < 0 )
      getButtonState().wheel_left = true;

    switch ( gpm_ev.type & 0x0f )
    {
      case GPM_DOWN:
      case GPM_DRAG:
        interpretKeyDown();
        break;

      case GPM_UP:
        interpretKeyUp();
        break;

      default:
        break;
    }

    setPos (FPoint{ std::max(gpm_ev.x, sInt16(1))
                  , std::max(gpm_ev.y, sInt16(1)) });

    if ( gpmEvent(false) == gpmEventType::Mouse )
      setPending(true);
    else
      setPending(false);

    has_gpm_mouse_data = false;
    setEvent();
    return;
  }

  gpm_fd = -1;
  has_gpm_mouse_data = false;
  clearEvent();
}

//----------------------------------------------------------------------
auto FMouseGPM::gpmMouse (bool enable) -> bool
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
auto FMouseGPM::hasSignificantEvents() const noexcept -> bool
{
  return ! (gpm_ev.type & GPM_MOVE)
      || gpm_ev.wdy != 0
      || gpm_ev.buttons & GPM_B_UP
      || gpm_ev.buttons & GPM_B_DOWN;
}

//----------------------------------------------------------------------
void FMouseGPM::interpretKeyDown() noexcept
{
  if ( gpm_ev.buttons & GPM_B_LEFT )
  {
    if ( gpm_ev.type & GPM_DOUBLE )
      getButtonState().left_button = State::DoubleClick;
    else
      getButtonState().left_button = State::Pressed;
  }

  if ( gpm_ev.buttons & GPM_B_MIDDLE )
    getButtonState().middle_button = State::Pressed;

  if ( gpm_ev.buttons & GPM_B_RIGHT )
    getButtonState().right_button = State::Pressed;

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
void FMouseGPM::interpretKeyUp() noexcept
{
  if ( gpm_ev.buttons & GPM_B_LEFT )
    getButtonState().left_button = State::Released;

  if ( gpm_ev.buttons & GPM_B_MIDDLE )
    getButtonState().middle_button = State::Released;

  if ( gpm_ev.buttons & GPM_B_RIGHT )
    getButtonState().right_button = State::Released;
}

//----------------------------------------------------------------------
auto FMouseGPM::getGpmKeyPressed (bool is_pending) -> bool
{
  setPending(is_pending);
  has_gpm_mouse_data = false;
  const auto type = gpmEvent();

  if ( type == gpmEventType::Mouse )
    has_gpm_mouse_data = true;
  else if ( type == gpmEventType::Keyboard )
    return true;

  return false;
}

//----------------------------------------------------------------------
void FMouseGPM::drawPointer() const
{
  if ( isGpmMouseEnabled() && gpm_ev.x != -1 )
    GPM_DRAWPOINTER(&gpm_ev);
}

//----------------------------------------------------------------------
auto FMouseGPM::gpmEvent (bool clear) const -> gpmEventType
{
  const int max = std::max(gpm_fd, stdin_no);
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

    return gpmEventType::Keyboard;
  }

  if ( clear && result > 0 && FD_ISSET(gpm_fd, &ifds) )
    FD_CLR (gpm_fd, &ifds);

  if ( result > 0 )
    return gpmEventType::Mouse;

  return gpmEventType::None;
}
#endif  // F_HAVE_LIBGPM


//----------------------------------------------------------------------
// class FMouseX11
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FMouseX11::FMouseX11()
{
  setMouseTypeID (FMouse::MouseType::X11);
}

// public methods of FMouseX11
//----------------------------------------------------------------------
auto FMouseX11::getClassName() const -> FString
{
  return "FMouseX11";
}

//----------------------------------------------------------------------
auto FMouseX11::hasData() noexcept -> bool
{
  return ( x11_mouse[0] );
}

//----------------------------------------------------------------------
void FMouseX11::setRawData (FKeyboard::keybuffer& fifo_buf) noexcept
{
  // Import the X11 xterm mouse protocol (SGR-Mode) raw mouse data

  static constexpr std::size_t len = 6;
  x11_mouse[0] = fifo_buf[3];
  x11_mouse[1] = fifo_buf[4];
  x11_mouse[2] = fifo_buf[5];
  x11_mouse[3] = '\0';
  fifo_buf.pop(len);  // Remove founded entry
  setPending(fifo_buf.hasData());
}

//----------------------------------------------------------------------
void FMouseX11::processEvent (const TimeValue& time)
{
  // Parse and interpret the X11 xterm mouse string

  const auto& mouse_position = getPos();
  const auto x = uChar(x11_mouse[1] - 0x20);
  const auto y = uChar(x11_mouse[2] - 0x20);
  const int btn = uChar(x11_mouse[0]);
  setNewPos (x, y);
  clearButtonState();
  setKeyState (btn);
  setMoveState (mouse_position, btn);
  setButtonState (btn & button_mask, time);

  if ( mouse_position == getNewPos()
    && ! isWheelUp() && ! isWheelDown()
    && ! isWheelLeft() && ! isWheelRight()
    && x11_button_state == uChar(btn) )
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
void FMouseX11::setKeyState (int btn) noexcept
{
  if ( (btn & key_shift) == key_shift )
    getButtonState().shift_button = true;

  if ( (btn & key_meta) == key_meta )
    getButtonState().meta_button = true;

  if ( (btn & key_ctrl) == key_ctrl )
    getButtonState().control_button = true;
}

//----------------------------------------------------------------------
void FMouseX11::setMoveState (const FPoint& mouse_position, int btn) noexcept
{
  if ( (btn & button_mask) >= button1_pressed_move
    && (btn & button_mask) <= button3_pressed_move
    && ! mouse_position.isOrigin() )
  {
    getButtonState().mouse_moved = true;
  }
}

//----------------------------------------------------------------------
void FMouseX11::setButtonState (const int btn, const TimeValue& time) noexcept
{
  // Get the x11 mouse button state

  switch ( btn )
  {
    case button1_pressed:
    case button1_pressed_move:
      handleButton1Pressed(time);
      break;

    case button2_pressed:
    case button2_pressed_move:
      resetMousePressedTime();
      getButtonState().middle_button = State::Pressed;
      break;

    case button3_pressed:
    case button3_pressed_move:
      resetMousePressedTime();
      getButtonState().right_button = State::Pressed;
      break;

    case all_buttons_released:
      handleButtonRelease();
      break;

    case button_up:
      resetMousePressedTime();
      getButtonState().wheel_up = true;
      break;

    case button_down:
      resetMousePressedTime();
      getButtonState().wheel_down = true;
      break;

    case button_left:
      resetMousePressedTime();
      getButtonState().wheel_left = true;
      break;

    case button_right:
      resetMousePressedTime();
      getButtonState().wheel_right = true;
      break;

      default:
        break;
  }
}

//----------------------------------------------------------------------
void FMouseX11::handleButton1Pressed (const TimeValue& time) noexcept
{
  if ( getPos() == getNewPos()
    && x11_button_state == all_buttons_released
    && ! isDblclickTimeout(getMousePressedTime()) )
  {
    resetMousePressedTime();
    getButtonState().left_button = State::DoubleClick;
  }
  else
  {
    setMousePressedTime (time);  // save click time
    getButtonState().left_button = State::Pressed;
  }
}

//----------------------------------------------------------------------
void FMouseX11::handleButtonRelease() noexcept
{
  switch ( x11_button_state & button_mask )
  {
    case button1_pressed:
    case button1_pressed_move:
      getButtonState().left_button = State::Released;
      break;

    case button2_pressed:
    case button2_pressed_move:
      getButtonState().middle_button = State::Released;
      break;

    case button3_pressed:
    case button3_pressed_move:
      getButtonState().right_button = State::Released;
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
{
  setMouseTypeID (FMouse::MouseType::Sgr);
}

// public methods of FMouseSGR
//----------------------------------------------------------------------
auto FMouseSGR::getClassName() const -> FString
{
  return "FMouseSGR";
}

//----------------------------------------------------------------------
auto FMouseSGR::hasData() noexcept -> bool
{
  return ( sgr_mouse[0] );
}

//----------------------------------------------------------------------
void FMouseSGR::setRawData (FKeyboard::keybuffer& fifo_buf) noexcept
{
  // Import the X11 xterm mouse protocol (SGR-Mode) raw mouse data

  const auto max = fifo_buf.getSize();
  std::size_t len{0};
  std::size_t n{3};

  while ( n < max )
  {
    sgr_mouse[n - 3] = fifo_buf[n];

    if ( fifo_buf[n] == 'M' || fifo_buf[n] == 'm' )
    {
      n++;
      len = n;
      break;
    }

    n++;
  }

  sgr_mouse[n - 3] = '\0';
  fifo_buf.pop(len);  // Remove founded entry
  setPending(fifo_buf.hasData());
}

//----------------------------------------------------------------------
void FMouseSGR::processEvent (const TimeValue& time)
{
  const auto& mouse_position = getPos();
  sInt16 x{0};
  sInt16 y{0};
  int btn{0};

  // Parse the SGR mouse string
  const char* p = sgr_mouse.data();

  // Parse button
  if ( ! parseNumberIf (p, btn, [] (char ch) { return ch != ';'; }) )
  {
    clearEvent();
    sgr_mouse[0] = '\0';  // Delete already interpreted data
    return;
  }

  if ( *p ) p++;  // ship one character after the number

  // Parse x-value
  if ( ! parseNumberIf (p, x, [] (char ch) { return ch != ';'; }) )
  {
    clearEvent();
    sgr_mouse[0] = '\0';  // Delete already interpreted data
    return;
  }

  if ( *p ) p++;  // ship one character after the number

  // Parse y-value
  if ( ! parseNumberIf (p, y, [] (char ch) { return ch != 'M' && ch != 'm'; }) )
  {
    clearEvent();
    sgr_mouse[0] = '\0';  // Delete already interpreted data
    return;
  }

  setNewPos (x, y);  // Update the mouse state
  clearButtonState();
  setKeyState (btn);
  setMoveState (mouse_position, btn);

  if ( *p == pressed )
    setPressedButtonState (btn & button_mask, time);
  else  // *p == released
    setReleasedButtonState (btn & button_mask);

  if ( mouse_position == getNewPos()
    && ! isWheelUp() && ! isWheelDown()
    && ! isWheelLeft() && ! isWheelRight()
    && sgr_button_state == uChar(((*p & 0x20) << 2) + btn) )
  {
    clearEvent();
    sgr_mouse[0] = '\0';  // Delete already interpreted data
    return;
  }

  setEvent();
  useNewPos();
  // Get the button state from string
  sgr_button_state = uChar(((*p & 0x20) << 2) + btn);
  // Delete already interpreted data
  sgr_mouse[0] = '\0';
}

// private methods of FMouseSGR
//----------------------------------------------------------------------
void FMouseSGR::setKeyState (int btn) noexcept
{
  if ( (btn & key_shift) == key_shift )
    getButtonState().shift_button = true;

  if ( (btn & key_meta) == key_meta )
    getButtonState().meta_button = true;

  if ( (btn & key_ctrl) == key_ctrl )
    getButtonState().control_button = true;
}

//----------------------------------------------------------------------
void FMouseSGR::setMoveState (const FPoint& mouse_position, int btn) noexcept
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
                                      , const TimeValue& time ) noexcept
{
  // Gets the extended x11 mouse mode (SGR) status for pressed buttons

  switch ( btn )
  {
    case button1:
    case button1_move:
      handleButton1Pressed(time);
      break;

    case button2:
    case button2_move:
      resetMousePressedTime();
      getButtonState().middle_button = State::Pressed;
      break;

    case button3:
    case button3_move:
      resetMousePressedTime();
      getButtonState().right_button = State::Pressed;
      break;

    case button_up:
      resetMousePressedTime();
      getButtonState().wheel_up = true;
      break;

    case button_down:
      resetMousePressedTime();
      getButtonState().wheel_down = true;
      break;

    case button_left:
      resetMousePressedTime();
      getButtonState().wheel_left = true;
      break;

    case button_right:
      resetMousePressedTime();
      getButtonState().wheel_right = true;
      break;

    default:
      break;
  }
}

//----------------------------------------------------------------------
void FMouseSGR::handleButton1Pressed (const TimeValue& time) noexcept
{
  if ( getPos() == getNewPos()
    && (((sgr_button_state & 0x80) >> 2) + 'M') == released
    && ! isDblclickTimeout(getMousePressedTime()) )
  {
    resetMousePressedTime();
    getButtonState().left_button = State::DoubleClick;
  }
  else
  {
    setMousePressedTime (time);  // save click time
    getButtonState().left_button = State::Pressed;
  }
}

//----------------------------------------------------------------------
void FMouseSGR::setReleasedButtonState (const int btn) noexcept
{
  // Gets the extended x11 mouse mode (SGR) status for released buttons

  switch ( btn )
  {
    case button1:
    case button1_move:
      getButtonState().left_button = State::Released;
      break;

    case button2:
    case button2_move:
      getButtonState().middle_button = State::Released;
      break;

    case button3:
    case button3_move:
      getButtonState().right_button = State::Released;
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
{
  setMouseTypeID (FMouse::MouseType::Urxvt);
}

// public methods of FMouseUrxvt
//----------------------------------------------------------------------
auto FMouseUrxvt::getClassName() const -> FString
{
  return "FMouseUrxvt";
}

//----------------------------------------------------------------------
auto FMouseUrxvt::hasData() noexcept -> bool
{
  return ( urxvt_mouse[0] );
}

//----------------------------------------------------------------------
void FMouseUrxvt::setRawData (FKeyboard::keybuffer& fifo_buf) noexcept
{
  // Import the X11 xterm mouse protocol (Urxvt-Mode) raw mouse data

  const auto max = fifo_buf.getSize();
  std::size_t len{0};
  std::size_t n{2};

  while ( n < max )
  {
    urxvt_mouse[n - 2] = fifo_buf[n];

    if ( fifo_buf[n] == 'M' || fifo_buf[n] == 'm' )
    {
      n++;
      len = n;
      break;
    }

    n++;
  }

  urxvt_mouse[n - 2] = '\0';
  fifo_buf.pop(len);  // Remove founded entry
  setPending(fifo_buf.hasData());
}

//----------------------------------------------------------------------
void FMouseUrxvt::processEvent (const TimeValue& time)
{
  // Parse and interpret the X11 xterm mouse string (Urxvt-Mode)

  const auto& mouse_position = getPos();
  sInt16 x{0};
  sInt16 y{0};
  int btn{0};

  // Parse the Urxvt mouse string
  const char* p = urxvt_mouse.data();
  bool x_neg{false};
  bool y_neg{false};

  // Parse button
  if ( ! parseNumberIf (p, btn, [] (char ch) { return ch != ';'; }) )
  {
    clearEvent();
    urxvt_mouse[0] = '\0';  // Delete already interpreted data
    return;
  }

  if ( *++p == '-' )
  {
    p++;
    x_neg = true;
  }

  // Parse x-value
  if ( ! parseNumberIf (p, x, [] (char ch) { return ch != ';'; }) )
  {
    clearEvent();
    urxvt_mouse[0] = '\0';  // Delete already interpreted data
    return;
  }

  if ( *++p == '-' )
  {
    p++;
    y_neg = true;
  }

  // Parse y-value
  if ( ! parseNumberIf (p, y, [] (char ch) { return ch != 'M'; }) )
  {
    clearEvent();
    urxvt_mouse[0] = '\0';  // Delete already interpreted data
    return;
  }

  if ( x_neg || x == 0 )
    x = 1;

  if ( y_neg || y == 0 )
    y = 1;

  x = std::min(x, sInt16(getMaxWidth()));
  y = std::min(y, sInt16(getMaxHeight()));
  setNewPos (x, y);
  clearButtonState();
  setKeyState (btn);
  setMoveState (mouse_position, btn);
  setButtonState (btn & button_mask, time);

  if ( mouse_position == getNewPos()
    && ! isWheelUp() && ! isWheelDown()
    && ! isWheelLeft() && ! isWheelRight()
    && urxvt_button_state == uChar(btn) )
  {
    clearEvent();
    urxvt_mouse[0] = '\0';  // Delete already interpreted data
    return;
  }

  setEvent();
  useNewPos();
  urxvt_button_state = uChar(btn);
  // Delete already interpreted data
  urxvt_mouse[0] = '\0';
}


// private methods of FMouseUrxvt
//----------------------------------------------------------------------
void FMouseUrxvt::setKeyState (int btn) noexcept
{
  if ( (btn & key_shift) == key_shift )
    getButtonState().shift_button = true;

  if ( (btn & key_meta) == key_meta )
    getButtonState().meta_button = true;

  if ( (btn & key_ctrl) == key_ctrl )
    getButtonState().control_button = true;
}

//----------------------------------------------------------------------
void FMouseUrxvt::setMoveState (const FPoint& mouse_position, int btn) noexcept
{
  if ( (btn & button_mask) >= button1_pressed_move
    && (btn & button_mask) <= button3_pressed_move
    && ! mouse_position.isOrigin() )
  {
    getButtonState().mouse_moved = true;
  }
}

//----------------------------------------------------------------------
void FMouseUrxvt::setButtonState (const int btn, const TimeValue& time) noexcept
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
        getButtonState().left_button = State::DoubleClick;
      }
      else
      {
        setMousePressedTime (time);  // save click time
        getButtonState().left_button = State::Pressed;
      }
      break;

    case button2_pressed:
    case button2_pressed_move:
      resetMousePressedTime();
      getButtonState().middle_button = State::Pressed;
      break;

    case button3_pressed:
    case button3_pressed_move:
      resetMousePressedTime();
      getButtonState().right_button = State::Pressed;
      break;

    case all_buttons_released:
      handleButtonRelease();
      break;

    case button_up:
      resetMousePressedTime();
      getButtonState().wheel_up = true;
      break;

    case button_down:
      resetMousePressedTime();
      getButtonState().wheel_down = true;
      break;

    case button_left:
      resetMousePressedTime();
      getButtonState().wheel_left = true;
      break;

    case button_right:
      resetMousePressedTime();
      getButtonState().wheel_right = true;
      break;

      default:
        break;
  }
}

//----------------------------------------------------------------------
void FMouseUrxvt::handleButtonRelease() noexcept
{
  switch ( urxvt_button_state & button_mask )
  {
    case button1_pressed:
    case button1_pressed_move:
      getButtonState().left_button = State::Released;
      break;

    case button2_pressed:
    case button2_pressed_move:
      getButtonState().middle_button = State::Released;
      break;

    case button3_pressed:
    case button3_pressed_move:
      getButtonState().right_button = State::Released;
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
  if ( FTermLinux::isLinuxConsole() )
    mouse_protocol.push_back(FMouse::createMouseObject<FMouseGPM>());
#endif

  mouse_protocol.push_back(FMouse::createMouseObject<FMouseX11>());
  mouse_protocol.push_back(FMouse::createMouseObject<FMouseSGR>());
  mouse_protocol.push_back(FMouse::createMouseObject<FMouseUrxvt>());
}

//----------------------------------------------------------------------
FMouseControl::~FMouseControl() = default;  // destructor


// public methods of FMouseControl
//----------------------------------------------------------------------
auto FMouseControl::getInstance() -> FMouseControl&
{
  static const auto& mouse = std::make_unique<FMouseControl>();
  return *mouse;
}

//----------------------------------------------------------------------
auto FMouseControl::getPos() & -> const FPoint&
{
  const auto& iter = findMouseWithEvent();

  if ( iter != mouse_protocol.end() )
    return (*iter)->getPos();

  return zero_point;
}

//----------------------------------------------------------------------
void FMouseControl::clearEvent()
{
  FMouseProtocol::const_iterator iter;

  do
  {
    iter = findMouseWithEvent();

    if ( iter != mouse_protocol.end() )
      (*iter)->clearEvent();
  }
  while ( iter != mouse_protocol.end() );
}

//----------------------------------------------------------------------
#ifdef F_HAVE_LIBGPM
void FMouseControl::setStdinNo (int file_descriptor)
{
  const auto iter = findMouseWithType (FMouse::MouseType::Gpm);

  if ( iter != mouse_protocol.end() )
    static_cast<FMouseGPM*>(iter->get())->setStdinNo(file_descriptor);
}
#else
void FMouseControl::setStdinNo (int)
{ }
#endif  // F_HAVE_LIBGPM

//----------------------------------------------------------------------
void FMouseControl::setMaxWidth (uInt16 x_max)
{
  const auto iter = findMouseWithType (FMouse::MouseType::Urxvt);

  if ( iter != mouse_protocol.end() )
    (*iter)->setMaxWidth(x_max);
}

//----------------------------------------------------------------------
void FMouseControl::setMaxHeight (uInt16 y_max)
{
  const auto iter = findMouseWithType (FMouse::MouseType::Urxvt);

  if ( iter != mouse_protocol.end() )
    (*iter)->setMaxHeight(y_max);
}

//----------------------------------------------------------------------
void FMouseControl::setDblclickInterval (const uInt64 timeout) const
{
  for (auto&& mouse : mouse_protocol)
  {
    if ( mouse )
      mouse->setDblclickInterval(timeout);
  }
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
auto FMouseControl::hasData() -> bool
{
  return findMouseWithData() != mouse_protocol.end();  // with data
}

//----------------------------------------------------------------------
auto FMouseControl::hasEvent() -> bool
{
  return findMouseWithEvent() != mouse_protocol.end();  // with event
}

//----------------------------------------------------------------------
auto FMouseControl::isLeftButtonPressed() -> bool
{
  auto iter = findMouseWithEvent();
  const bool found = (iter != mouse_protocol.end());
  return found ? (*iter)->isLeftButtonPressed() : false;
}

//----------------------------------------------------------------------
auto FMouseControl::isLeftButtonReleased() -> bool
{
  auto iter = findMouseWithEvent();
  const bool found = (iter != mouse_protocol.end());
  return found ? (*iter)->isLeftButtonReleased() : false;
}

//----------------------------------------------------------------------
auto FMouseControl::isLeftButtonDoubleClick() -> bool
{
  auto iter = findMouseWithEvent();
  const bool found = (iter != mouse_protocol.end());
  return found ? (*iter)->isLeftButtonDoubleClick() : false;
}

//----------------------------------------------------------------------
auto FMouseControl::isRightButtonPressed() -> bool
{
  auto iter = findMouseWithEvent();
  const bool found = (iter != mouse_protocol.end());
  return found ? (*iter)->isRightButtonPressed() : false;
}

//----------------------------------------------------------------------
auto FMouseControl::isRightButtonReleased() -> bool
{
  auto iter = findMouseWithEvent();
  const bool found = (iter != mouse_protocol.end());
  return found ? (*iter)->isRightButtonReleased() : false;
}

//----------------------------------------------------------------------
auto FMouseControl::isMiddleButtonPressed() -> bool
{
  auto iter = findMouseWithEvent();
  const bool found = (iter != mouse_protocol.end());
  return found ? (*iter)->isMiddleButtonPressed() : false;
}

//----------------------------------------------------------------------
auto FMouseControl::isMiddleButtonReleased() -> bool
{
  auto iter = findMouseWithEvent();
  const bool found = (iter != mouse_protocol.end());
  return found ? (*iter)->isMiddleButtonReleased() : false;
}

//----------------------------------------------------------------------
auto FMouseControl::isShiftKeyPressed() -> bool
{
  auto iter = findMouseWithEvent();
  const bool found = (iter != mouse_protocol.end());
  return found ? (*iter)->isShiftKeyPressed() : false;
}

//----------------------------------------------------------------------
auto FMouseControl::isControlKeyPressed() -> bool
{
  auto iter = findMouseWithEvent();
  const bool found = (iter != mouse_protocol.end());
  return found ? (*iter)->isControlKeyPressed() : false;
}

//----------------------------------------------------------------------
auto FMouseControl::isMetaKeyPressed() -> bool
{
  auto iter = findMouseWithEvent();
  const bool found = (iter != mouse_protocol.end());
  return found ? (*iter)->isMetaKeyPressed() : false;
}

//----------------------------------------------------------------------
auto FMouseControl::isWheelUp() -> bool
{
  auto iter = findMouseWithEvent();
  const bool found = (iter != mouse_protocol.end());
  return found ? (*iter)->isWheelUp() : false;
}

//----------------------------------------------------------------------
auto FMouseControl::isWheelDown() -> bool
{
  auto iter = findMouseWithEvent();
  const bool found = (iter != mouse_protocol.end());
  return found ? (*iter)->isWheelDown() : false;
}

//----------------------------------------------------------------------
auto FMouseControl::isWheelLeft() -> bool
{
  auto iter = findMouseWithEvent();
  const bool found = (iter != mouse_protocol.end());
  return found ? (*iter)->isWheelLeft() : false;
}

//----------------------------------------------------------------------
auto FMouseControl::isWheelRight() -> bool
{
  auto iter = findMouseWithEvent();
  const bool found = (iter != mouse_protocol.end());
  return found ? (*iter)->isWheelRight() : false;
}

//----------------------------------------------------------------------
auto FMouseControl::isMoved() -> bool
{
  auto iter = findMouseWithEvent();
  const bool found = (iter != mouse_protocol.end());
  return found ? (*iter)->isMoved() : false;
}

//----------------------------------------------------------------------
auto FMouseControl::hasUnprocessedInput() const -> bool
{
  return std::any_of ( std::cbegin(mouse_protocol)
                     , std::cend(mouse_protocol)
                     , [] (const auto& mouse)
                       {
                         return mouse->hasUnprocessedInput();
                       }
                     );
}

//----------------------------------------------------------------------
#ifdef F_HAVE_LIBGPM
auto FMouseControl::isGpmMouseEnabled() noexcept -> bool
{
  if ( ! use_gpm_mouse || mouse_protocol.empty() )
    return false;

  const auto iter = findMouseWithType (FMouse::MouseType::Gpm);

  return ( iter != mouse_protocol.end() )
       ? static_cast<FMouseGPM*>(iter->get())->isGpmMouseEnabled()
       : false;
}
#else  // F_HAVE_LIBGPM
bool FMouseControl::isGpmMouseEnabled() noexcept
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
    const auto iter = findMouseWithType (FMouse::MouseType::Gpm);

    if ( iter != mouse_protocol.end() )
      use_gpm_mouse = static_cast<FMouseGPM*>(iter->get())->enableGpmMouse();
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
    const auto iter = findMouseWithType (FMouse::MouseType::Gpm);

    if ( iter != mouse_protocol.end() )
      use_gpm_mouse = static_cast<FMouseGPM*>(iter->get())->disableGpmMouse();
  }
#endif  // F_HAVE_LIBGPM

  if ( use_xterm_mouse )
    disableXTermMouse();
}

//----------------------------------------------------------------------
void FMouseControl::setRawData ( const FMouse::MouseType& mt
                               , FKeyboard::keybuffer& fifo_buf )
{
  const auto iter = findMouseWithType(mt);

  if ( iter != mouse_protocol.end() )
    (*iter)->setRawData (fifo_buf);
}

//----------------------------------------------------------------------
void FMouseControl::processQueuedInput()
{
  while ( ! fmousedata_queue.isEmpty() )
  {
    if ( FApplication::isQuit() )
      return;

    FMouseDataPtr md(std::move(fmousedata_queue.front()));
    fmousedata_queue.pop();

    if ( md )
    {
      setCurrentMouseEvent (md);
      event_cmd.execute(*md);
      resetCurrentMouseEvent();
    }

    if ( FApplication::isQuit() )
      return;
  }
}

//----------------------------------------------------------------------
void FMouseControl::processEvent (const TimeValue& time)
{
  const auto iter = findMouseWithData();

  // Clear all old mouse events
  clearEvent();

  if ( iter != mouse_protocol.end() )
  {
    (*iter)->processEvent(time);
    auto& md = static_cast<FMouseData&>(**iter);
    fmousedata_queue.emplace(std::make_unique<FMouseData>(std::move(md)));
  }
}

//----------------------------------------------------------------------
#ifdef F_HAVE_LIBGPM
auto FMouseControl::getGpmKeyPressed (bool pending) -> bool
{
  if ( mouse_protocol.empty() )
    return false;

  const auto iter = findMouseWithType (FMouse::MouseType::Gpm);

  return ( iter != mouse_protocol.end() )
       && static_cast<FMouseGPM*>(iter->get())->getGpmKeyPressed(pending);
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

  const auto iter = findMouseWithType (FMouse::MouseType::Gpm);

  if ( iter != mouse_protocol.end() )
    static_cast<FMouseGPM*>(iter->get())->drawPointer();
}
#else  // F_HAVE_LIBGPM
void FMouseControl::drawPointer()
{ }
#endif  // F_HAVE_LIBGPM


// private methods of FMouseControl
//----------------------------------------------------------------------
auto FMouseControl::findMouseWithType (const FMouse::MouseType& mt) const -> FMouseProtocol::const_iterator
{
  return std::find_if ( std::cbegin(mouse_protocol)
                      , std::cend(mouse_protocol)
                      , [&mt] (const auto& mouse)
                        {
                          return mouse->getMouseTypeID() == mt;
                        }
                      );
}

//----------------------------------------------------------------------
auto FMouseControl::findMouseWithData() const -> FMouseProtocol::const_iterator
{
  return std::find_if ( std::cbegin(mouse_protocol)
                      , std::cend(mouse_protocol)
                      , [] (const auto& mouse)
                        {
                          return mouse->hasData();
                        }
                      );
}

//----------------------------------------------------------------------
auto FMouseControl::findMouseWithEvent() const -> FMouseProtocol::const_iterator
{
  return std::find_if ( std::cbegin(mouse_protocol)
                      , std::cend(mouse_protocol)
                      , [] (const auto& mouse)
                        {
                          return mouse->hasEvent();
                        }
                      );
}

//----------------------------------------------------------------------
void FMouseControl::xtermMouse (bool enable) const
{
  // activate/deactivate the xterm mouse support

  if ( ! use_xterm_mouse )
    return;

  FTermXTerminal::setMouseSupport (enable);
}

//----------------------------------------------------------------------
void FMouseControl::setCurrentMouseEvent (const FMouseDataPtr& ptr)
{
  getCurrentMouseEvent() = ptr;
}

//----------------------------------------------------------------------
void FMouseControl::resetCurrentMouseEvent()
{
  getCurrentMouseEvent() = nullptr;
}

}  // namespace finalcut
