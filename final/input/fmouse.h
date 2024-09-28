/***********************************************************************
* fmouse.h - Read mouse events                                         *
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

/*  Inheritance diagram
 *  ═══════════════════
 *
 *                     ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *                     ▕  FMouseData  ▏
 *                     ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *                            ▲
 *                            │
 *                       ▕▔▔▔▔▔▔▔▔▔▔▏
 *                       ▕  FMouse  ▏
 *                       ▕▁▁▁▁▁▁▁▁▁▁▏
 *                            ▲
 *                            │
 *       ┌─────────────┬──────┴───────┬─────────────┐
 *       │             │              │             │
 * ▕▔▔▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FMouseGPM ▏ ▕ FMouseX11 ▏ ▕ FMouseSGR ▏ ▕ FMouseUrxvt ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *
 *
 *
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏1       1▕▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FMouseControl ▏- -┬- - -▕ FMouseGPM ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏   :     ▕▁▁▁▁▁▁▁▁▁▁▁▏
 *                     :
 *                     :    1▕▔▔▔▔▔▔▔▔▔▔▔▏
 *                     :- - -▕ FMouseX11 ▏
 *                     :     ▕▁▁▁▁▁▁▁▁▁▁▁▏
 *                     :
 *                     :    1▕▔▔▔▔▔▔▔▔▔▔▔▏
 *                     └- - -▕ FMouseSGR ▏
 *                     :     ▕▁▁▁▁▁▁▁▁▁▁▁▏
 *                     :
 *                     :    1▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *                     └- - -▕ FMouseUrxvt ▏
 *                           ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FMOUSE_H
#define FMOUSE_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <cstddef>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include "final/input/fkeyboard.h"
#include "final/util/fpoint.h"

#if defined(__linux__)
  #include <linux/keyboard.h>  // need for gpm keyboard modifiers
#endif

#ifdef F_HAVE_LIBGPM
  #include <gpm.h>
#endif

namespace finalcut
{

//----------------------------------------------------------------------
// class FMouseData
//----------------------------------------------------------------------

class FMouseData
{
  public:
    // Constructor
    FMouseData() = default;

    // Copy constructor
    FMouseData (const FMouseData&) = default;

    // Move constructor
    FMouseData (FMouseData&&) noexcept = default;

    // Destructor
    virtual ~FMouseData() noexcept;

    // Copy assignment operator (=)
    auto operator = (const FMouseData&) -> FMouseData& = default;

    // Move assignment operator (=)
    auto operator = (FMouseData&&) noexcept -> FMouseData& = default;

    // Accessors
    virtual auto getClassName() const -> FString;
    auto getPos() const & noexcept -> const FPoint&;

    // Inquiries
    auto isLeftButtonPressed() const noexcept -> bool;
    auto isLeftButtonReleased() const noexcept -> bool;
    auto isLeftButtonDoubleClick() const noexcept -> bool;
    auto isRightButtonPressed() const noexcept -> bool;
    auto isRightButtonReleased() const noexcept -> bool;
    auto isMiddleButtonPressed() const noexcept -> bool;
    auto isMiddleButtonReleased() const noexcept -> bool;
    auto isShiftKeyPressed() const noexcept -> bool;
    auto isControlKeyPressed() const noexcept -> bool;
    auto isMetaKeyPressed() const noexcept -> bool;
    auto isWheelUp() const noexcept -> bool;
    auto isWheelDown() const noexcept -> bool;
    auto isWheelLeft() const noexcept -> bool;
    auto isWheelRight() const noexcept -> bool;
    auto isMoved() const noexcept -> bool;

    // Methods
    void clearButtonState() noexcept;

  protected:
    // Enumerations
    enum class State : uChar
    {
      Undefined   = 0,
      Pressed     = 1,
      Released    = 2,
      DoubleClick = 3
    };

    struct FMouseButton
    {
      State left_button{};
      State right_button{};
      State middle_button{};
      bool  shift_button{};
      bool  control_button{};
      bool  meta_button{};
      bool  wheel_up{};
      bool  wheel_down{};
      bool  wheel_left{};
      bool  wheel_right{};
      bool  mouse_moved{};
    };

    // Accessors
    auto getButtonState() & noexcept -> FMouseButton&;
    auto getButtonState() const & noexcept -> const FMouseButton&;

    // Mutator
    void setPos (const FPoint&) noexcept;

  private:
    // Data members
    FMouseButton b_state{};
    FPoint       mouse{0, 0};  // mouse click position
};


//----------------------------------------------------------------------
// class FMouse
//----------------------------------------------------------------------

class FMouse : public FMouseData
{
  public:
    // Enumeration
    enum class MouseType
    {
      None  = 0,
      Gpm   = 1,
      X11   = 2,
      Sgr   = 3,
      Urxvt = 4
    };

    // Constructor
    FMouse();

    // Accessors
    auto getClassName() const -> FString override;
    auto getMouseTypeID() const noexcept -> MouseType;
    void clearEvent() noexcept;

    // Mutators
    void setMaxWidth (uInt16) noexcept;
    void setMaxHeight (uInt16) noexcept;
    void setDblclickInterval (const uInt64) noexcept;

    // Inquiries
    virtual auto hasData() -> bool = 0;
    auto hasEvent() const noexcept -> bool;
    auto hasUnprocessedInput() const noexcept -> bool;

    // Methods
    template <typename ClassT>
    static auto  createMouseObject() -> std::unique_ptr<ClassT>;
    virtual void setRawData (FKeyboard::keybuffer&) = 0;
    virtual void processEvent (const TimeValue&) = 0;

  protected:
    // Accessors
    auto getNewPos() const & noexcept -> const FPoint&;
    auto getMaxWidth() const noexcept -> uInt16;
    auto getMaxHeight() const noexcept -> uInt16;
    auto getDblclickInterval() const noexcept -> uInt64;
    auto getMousePressedTime() const noexcept -> TimeValue;

    // Mutator
    void setMouseTypeID (MouseType) noexcept;
    void setNewPos (int, int) noexcept;
    void useNewPos() noexcept;
    void setPending (bool = true) noexcept;
    void setEvent() noexcept;
    void setMousePressedTime (const TimeValue&) noexcept;
    void resetMousePressedTime() noexcept;

    // Inquiry
    auto isDblclickTimeout (const TimeValue&) const -> bool;

  private:
    // Data members
    bool       mouse_event_occurred{false};
    bool       unprocessed_buffer_data{false};
    MouseType  MouseType_id{MouseType::None};
    uInt16     max_width{80};
    uInt16     max_height{25};
    uInt64     dblclick_interval{500000};  // 500 ms
    TimeValue  time_mousepressed{};
    FPoint     new_mouse_position{};
};

//----------------------------------------------------------------------
template <typename ClassT>
inline auto FMouse::createMouseObject() -> std::unique_ptr<ClassT>
{
  return std::make_unique<ClassT>();
}


#ifdef F_HAVE_LIBGPM
//----------------------------------------------------------------------
// class FMouseGPM
//----------------------------------------------------------------------

class FMouseGPM final : public FMouse
{
  public:
    // Constructor
    FMouseGPM();

    // Accessors
    auto getClassName() const -> FString override;

    // Mutators
    void setStdinNo(int) noexcept;

    // Inquiry
    auto hasData() noexcept -> bool override;
    auto isGpmMouseEnabled() const noexcept -> bool;

    // Methods
    void setRawData (FKeyboard::keybuffer&) noexcept override;
    void processEvent (const TimeValue&) override;
    auto gpmMouse (bool = true) -> bool;
    auto enableGpmMouse() -> bool;
    auto disableGpmMouse() -> bool;
    auto hasSignificantEvents() const noexcept -> bool;
    void interpretKeyDown() noexcept;
    void interpretKeyUp() noexcept;
    auto getGpmKeyPressed (bool = true) -> bool;
    void drawPointer() const;

  private:
    // Enumeration
    enum class gpmEventType
    {
      None     = 0,
      Keyboard = 1,
      Mouse    = 2
    };

    // Method
    void handleMouseEvent();
    void resetMouseState();
    void handleMouseMovement();
    void handleMouseWheel();
    void interpretMouseEvent();
    void updateMousePosition();
    auto gpmEvent (bool = true) const -> gpmEventType;

    // Data member
    Gpm_Event gpm_ev{};
    bool      has_gpm_mouse_data{false};
    bool      gpm_mouse_enabled{false};
    int       stdin_no{0};
};

//----------------------------------------------------------------------
inline auto FMouseGPM::enableGpmMouse() -> bool
{ return gpmMouse(true); }

//----------------------------------------------------------------------
inline auto FMouseGPM::disableGpmMouse() -> bool
{ return gpmMouse(false); }

//----------------------------------------------------------------------
inline auto FMouseGPM::isGpmMouseEnabled() const noexcept -> bool
{ return gpm_mouse_enabled; }
#endif  // F_HAVE_LIBGPM


//----------------------------------------------------------------------
// class FMouseX11
//----------------------------------------------------------------------

class FMouseX11 final : public FMouse
{
  public:
    // Constructor
    FMouseX11();

    // Accessors
    auto getClassName() const -> FString override;

    // Inquiry
    auto hasData() noexcept -> bool override;

    // Methods
    void setRawData (FKeyboard::keybuffer&) noexcept override;
    void processEvent (const TimeValue&) override;

  private:
    // Enumeration
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
      button_up            = 0x60,  // Mouse wheel scrolls up
      button_down          = 0x61,  // Mouse wheel scrolls down
      button_left          = 0x62,  // Mouse wheel left tilt
      button_right         = 0x63   // Mouse wheel right tilt
    };

    // Constant
    static constexpr std::size_t MOUSE_BUF_SIZE = 4;

    // Methods
    void setKeyState (int) noexcept;
    void setMoveState (const FPoint&, int) noexcept;
    auto isMouseClickButton (const int) const noexcept -> bool;
    auto isMouseWheelButton (const int) const noexcept -> bool;
    auto noChanges (const FPoint&, uChar) const noexcept -> bool;
    void handleMouseClickButton (int, const TimeValue&) noexcept;
    void handleMouseWheelButton (int) noexcept;
    void setButtonState (const int, const TimeValue&) noexcept;
    void handleButton1Pressed (const TimeValue& time) noexcept;
    void handleButtonRelease() noexcept;

    // Data member
    std::array<char, MOUSE_BUF_SIZE>x11_mouse{};
    uChar x11_button_state{all_buttons_released};
};


//----------------------------------------------------------------------
// class FMouseSGR
//----------------------------------------------------------------------

class FMouseSGR final : public FMouse
{
  public:
    // Constructor
    FMouseSGR();

    // Accessors
    auto getClassName() const -> FString override;

    // Inquiry
    auto hasData() noexcept -> bool override;

    // Methods
    void setRawData (FKeyboard::keybuffer&) noexcept override;
    void processEvent (const TimeValue&) override;

  private:
    struct Tokens
    {
      sInt16  x{0};
      sInt16  y{0};
      int     btn{0};
      const char* p{nullptr};  // Current read position
    };

    // Enumerations
    enum class ParseError { No, Yes };

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
      button_mask     = 0xe3,
      button_up       = 0x40,  // Mouse wheel scrolls up
      button_down     = 0x41,  // Mouse wheel scrolls down
      button_left     = 0x42,  // Mouse wheel left tilt
      button_right    = 0x43,  // Mouse wheel right tilt
      pressed         = 'M',
      released        = 'm'
    };

    // Constant
    static constexpr std::size_t MOUSE_BUF_SIZE = 13;

    // Methods
    void setKeyState (int) noexcept;
    void setMoveState (const FPoint&, int) noexcept;
    auto isMouseClickButton (const int) const noexcept -> bool;
    auto isMouseWheelButton (const int) const noexcept -> bool;
    auto parseSGRMouseString (Tokens&) const noexcept -> ParseError;
    auto noChanges (const FPoint&, uChar) const noexcept -> bool;
    void handleMouseClickButton (int, const TimeValue&) noexcept;
    void handleMouseWheelButton (int) noexcept;
    void setPressedButtonState (const int, const TimeValue&) noexcept;
    void handleButton1Pressed (const TimeValue& time) noexcept;
    void setReleasedButtonState (const int) noexcept;

    // Data members
    std::array<char, MOUSE_BUF_SIZE> sgr_mouse{};
    uChar sgr_button_state{0x23};
};


//----------------------------------------------------------------------
// class FMouseUrxvt
//----------------------------------------------------------------------

class FMouseUrxvt final : public FMouse
{
  public:
    // Constructor
    FMouseUrxvt();

    // Accessors
    auto getClassName() const -> FString override;

    // Inquiry
    auto hasData() noexcept -> bool override;

    // Methods
    void setRawData (FKeyboard::keybuffer&) noexcept override;
    void processEvent (const TimeValue&) override;

  private:
    struct Tokens
    {
      sInt16      x{0};
      sInt16      y{0};
      int         btn{0};
      bool        x_neg{false};
      bool        y_neg{false};
      const char* p{nullptr};  // Current read position
    };

    // Enumerations
    enum class ParseError { No, Yes };

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
      button_mask          = 0xe3,
      button_up            = 0x60,  // Mouse wheel scrolls up
      button_down          = 0x61,  // Mouse wheel scrolls down
      button_left          = 0x62,  // Mouse wheel left tilt
      button_right         = 0x63   // Mouse wheel right tilt
    };

    // Constant
    static constexpr std::size_t MOUSE_BUF_SIZE = 14;

    // Methods
    void setKeyState (int) noexcept;
    void setMoveState (const FPoint&, int) noexcept;
    auto isMouseClickButton (const int) const noexcept -> bool;
    auto isMouseWheelButton (const int) const noexcept -> bool;
    auto parseUrxvtMouseString (Tokens&) const noexcept -> ParseError;
    void adjustAndSetPosition (Tokens&);
    auto noChanges (const FPoint&, uChar) const noexcept -> bool;
    void handleMouseClickButton (int, const TimeValue&) noexcept;
    void handleButtonRelease() noexcept;
    void handleMouseWheelButton (int) noexcept;
    void setButtonState (const int, const TimeValue&) noexcept;

    // Data members
    std::array<char, MOUSE_BUF_SIZE>  urxvt_mouse{};
    uChar urxvt_button_state{all_buttons_released};
};


//----------------------------------------------------------------------
// class FMouseCommand
//----------------------------------------------------------------------

class FMouseCommand final
{
  public:
    // Constructors
    FMouseCommand() = default;

    explicit FMouseCommand (std::function<void()>&& fn)
      : handler1(std::move(fn))
    { }

    explicit FMouseCommand (std::function<void(const FMouseData&)>&& fn)
      : handler2(std::move(fn))
    { }

    // Method
    inline void execute() const
    {
      handler1();
    }

    template <typename T>
    inline void execute(T&& arg) const
    {
      handler2(std::forward<T>(arg));
    }

  private:
    // Data members
    std::function<void()> handler1{};
    std::function<void(const FMouseData&)> handler2{};
};


//----------------------------------------------------------------------
// class FMouseControl
//----------------------------------------------------------------------

class FMouseControl
{
  public:
    // Using-declaration
    using FMouseDataPtr = std::shared_ptr<FMouseData>;

    // Constructor
    FMouseControl();

    // Destructor
    virtual ~FMouseControl();

    // Accessors
    virtual auto getClassName() const -> FString;
    static auto  getInstance() -> FMouseControl&;
    static auto  getCurrentMouseEvent() -> FMouseDataPtr&;
    auto  getPos() & -> const FPoint&;
    void  clearEvent();

    // Mutators
    void  setStdinNo (int);
    void  setMaxWidth (uInt16);
    void  setMaxHeight (uInt16);
    void  setDblclickInterval (const uInt64) const;
    void  setEventCommand (const FMouseCommand&);
    void  setEnableXTermMouseCommand (const FMouseCommand&);
    void  setDisableXTermMouseCommand (const FMouseCommand&);
    void  useGpmMouse (bool = true);
    void  useXtermMouse (bool = true);

    // Inquiries
    auto  hasData() -> bool;
    auto  hasEvent() -> bool;
    auto  isLeftButtonPressed() -> bool;
    auto  isLeftButtonReleased() -> bool;
    auto  isLeftButtonDoubleClick() -> bool;
    auto  isRightButtonPressed() -> bool;
    auto  isRightButtonReleased() -> bool;
    auto  isMiddleButtonPressed() -> bool;
    auto  isMiddleButtonReleased() -> bool;
    auto  isShiftKeyPressed() -> bool;
    auto  isControlKeyPressed() -> bool;
    auto  isMetaKeyPressed() -> bool;
    auto  isWheelUp() -> bool;
    auto  isWheelDown() -> bool;
    auto  isWheelLeft() -> bool;
    auto  isWheelRight() -> bool;
    auto  isMoved() -> bool;
    auto  hasUnprocessedInput() const -> bool;
    auto  hasDataInQueue() const -> bool;
    auto  isGpmMouseEnabled() noexcept -> bool;

    // Methods
    void  enable();
    void  disable();
    virtual void setRawData ( const FMouse::MouseType&
                            , FKeyboard::keybuffer& );
    virtual void processEvent (const TimeValue&);
    void  processQueuedInput();
    auto  getGpmKeyPressed (bool = true) -> bool;
    void  drawPointer();

  private:
    // Constants
    static constexpr std::size_t MAX_QUEUE_SIZE = 64;

    // Using-declarations
    using FMousePtr = std::unique_ptr<FMouse>;
    using FMouseProtocol = std::vector<FMousePtr>;
    using MouseQueue = FRingBuffer<FMouseDataPtr, MAX_QUEUE_SIZE>;

    // Accessor
    auto  findMouseWithType (const FMouse::MouseType&) const -> FMouseProtocol::const_iterator;
    auto  findMouseWithData() const -> FMouseProtocol::const_iterator;
    auto  findMouseWithEvent() const -> FMouseProtocol::const_iterator;

    // Mutators
    void  xtermMouse (bool = true) const;
    void  enableXTermMouse() const;
    void  disableXTermMouse() const;
    static void  setCurrentMouseEvent (const FMouseDataPtr&);
    static void  resetCurrentMouseEvent();

    // Data member
    FMouseProtocol  mouse_protocol{};
    FMouseCommand   event_cmd{};
    FMouseCommand   enable_xterm_mouse_cmd{};
    FMouseCommand   disable_xterm_mouse_cmd{};
    MouseQueue      fmousedata_queue{};
    FPoint          zero_point{0, 0};
    bool            use_gpm_mouse{false};
    bool            use_xterm_mouse{false};
};

// FMouseControl inline functions
//----------------------------------------------------------------------
inline auto FMouseControl::getClassName() const -> FString
{ return "FMouseControl"; }

//----------------------------------------------------------------------
inline void FMouseControl::setEventCommand (const FMouseCommand& cmd)
{ event_cmd = cmd; }

//----------------------------------------------------------------------
inline void FMouseControl::setEnableXTermMouseCommand (const FMouseCommand& cmd)
{ enable_xterm_mouse_cmd = cmd; }

//----------------------------------------------------------------------
inline void FMouseControl::setDisableXTermMouseCommand (const FMouseCommand& cmd)
{ disable_xterm_mouse_cmd = cmd; }

//----------------------------------------------------------------------
inline auto FMouseControl::hasDataInQueue() const -> bool
{ return ! fmousedata_queue.isEmpty(); }

//----------------------------------------------------------------------
inline void FMouseControl::enableXTermMouse() const
{ xtermMouse(true); }

//----------------------------------------------------------------------
inline void FMouseControl::disableXTermMouse() const
{ xtermMouse(false); }

}  // namespace finalcut

#endif  // FMOUSE_H
