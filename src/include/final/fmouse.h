/***********************************************************************
* fmouse.h - Read mouse events                                         *
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
#include <map>
#include <queue>

#include "final/fkeyboard.h"
#include "final/fpoint.h"

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
    // Accessors
    virtual FString       getClassName() const;
    const FPoint&         getPos() const;

    // Constructor
    FMouseData();

    // Default copy constructor
    FMouseData (const FMouseData&) = default;
    // Default move constructor
    FMouseData (FMouseData&&) = default;
    // Default copy assignment operator (=)
    FMouseData& operator = (const FMouseData&) = default;
    // Default move assignment operator (=)
    FMouseData& operator = (FMouseData&&) = default;

    // Destructor
    virtual ~FMouseData();

    // Inquiries
    bool                  isLeftButtonPressed() const;
    bool                  isLeftButtonReleased() const;
    bool                  isLeftButtonDoubleClick() const;
    bool                  isRightButtonPressed() const;
    bool                  isRightButtonReleased() const;
    bool                  isMiddleButtonPressed() const;
    bool                  isMiddleButtonReleased() const;
    bool                  isShiftKeyPressed() const;
    bool                  isControlKeyPressed() const;
    bool                  isMetaKeyPressed() const;
    bool                  isWheelUp() const;
    bool                  isWheelDown() const;
    bool                  isMoved() const;

    // Methods
    void                  clearButtonState();

  protected:
    // Typedef and Enumerations
    typedef struct
    {
      uChar left_button    : 2;  // 0..3
      uChar right_button   : 2;  // 0..3
      uChar middle_button  : 2;  // 0..3
      uChar shift_button   : 1;  // 0..1
      uChar control_button : 1;  // 0..1
      uChar meta_button    : 1;  // 0..1
      uChar wheel_up       : 1;  // 0..1
      uChar wheel_down     : 1;  // 0..1
      uChar mouse_moved    : 1;  // 0..1
      uChar                : 4;  // padding bits
    } FMouseButton;  // bit field

    enum states
    {
      Undefined   = 0,
      Pressed     = 1,
      Released    = 2,
      DoubleClick = 3
    };

    // Accessors
    FMouseButton&       getButtonState();
    const FMouseButton& getButtonState() const;

    // Mutator
    void                setPos (const FPoint&);

  private:
    // Data members
    FMouseButton        b_state{};
    FPoint              mouse{0, 0};  // mouse click position
};


//----------------------------------------------------------------------
// class FMouse
//----------------------------------------------------------------------

class FMouse : public FMouseData
{
  public:
    // Enumeration
    enum mouse_type
    {
      none  = 0,
      gpm   = 1,
      x11   = 2,
      sgr   = 3,
      urxvt = 4
    };

    // Constructor
    FMouse();

    // Destructor
    ~FMouse() override;

    // Accessors
    FString               getClassName() const override;
    void                  clearEvent();

    // Mutators
    void                  setMaxWidth (uInt16);
    void                  setMaxHeight (uInt16);
    void                  setDblclickInterval (const uInt64);

    // Inquiries
    virtual bool          hasData() = 0;
    bool                  hasEvent() const;
    bool                  hasUnprocessedInput() const;

    // Methods
    template <typename ClassT>
    static FMouse*        createMouseObject ();
    virtual void          setRawData (FKeyboard::keybuffer&) = 0;
    virtual void          processEvent (struct timeval*) = 0;

  protected:
    // Accessors
    const FPoint&         getNewPos() const;
    uInt16                getMaxWidth() const;
    uInt16                getMaxHeight() const;
    uInt64                getDblclickInterval() const;
    timeval*              getMousePressedTime();

    // Mutator
    void                   setNewPos (int, int);
    void                   setPending (bool);
    void                   setEvent();
    void                   setMousePressedTime (const timeval*);
    void                   resetMousePressedTime();

    // Inquiry
    bool                   isDblclickTimeout (const timeval*) const;

  private:
    // Data members
    bool                   mouse_event_occurred{false};
    bool                   unprocessed_buffer_data{false};
    uInt16                 max_width{80};
    uInt16                 max_height{25};
    uInt64                 dblclick_interval{500000};  // 500 ms
    struct timeval         time_mousepressed{};
    FPoint                 new_mouse_position{};
};

//----------------------------------------------------------------------
template <typename ClassT>
inline FMouse* FMouse::createMouseObject()
{
  return new ClassT;
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

    // Destructor
    ~FMouseGPM() override;

    // Accessors
    FString              getClassName() const override;

    // Mutators
    void                 setStdinNo(int);

    // Inquiry
    bool                 hasData() override;
    bool                 isGpmMouseEnabled() const;

    // Methods
    void                 setRawData (FKeyboard::keybuffer&) override;
    void                 processEvent (struct timeval*) override;
    bool                 gpmMouse (bool);
    bool                 enableGpmMouse();
    bool                 disableGpmMouse();
    bool                 hasSignificantEvents() const;
    void                 interpretKeyDown();
    void                 interpretKeyUp();
    bool                 getGpmKeyPressed(bool);
    void                 drawGpmPointer() const;

  private:
    // Enumeration
    enum gpmEventType
    {
      no_event       = 0,
      keyboard_event = 1,
      mouse_event    = 2
    };

    // Method
    int                gpmEvent (bool = true) const;

    // Data member
    Gpm_Event          gpm_ev{};
    bool               has_gpm_mouse_data{false};
    bool               gpm_mouse_enabled{false};
    int                stdin_no{0};
};

//----------------------------------------------------------------------
inline bool FMouseGPM::enableGpmMouse()
{ return gpmMouse(true); }

//----------------------------------------------------------------------
inline bool FMouseGPM::disableGpmMouse()
{ return gpmMouse(false); }

//----------------------------------------------------------------------
inline bool FMouseGPM::isGpmMouseEnabled() const
{ return gpm_mouse_enabled; }
#endif  // F_HAVE_LIBGPM


//----------------------------------------------------------------------
// class FMouseX11
//----------------------------------------------------------------------

class FMouseX11 final : public FMouse
{
  public:
    // Constructor
    FMouseX11() = default;

    // Destructor
    ~FMouseX11() override = default;

    // Accessors
    FString              getClassName() const override;

    // Inquiry
    bool                 hasData() override;

    // Methods
    void                 setRawData (FKeyboard::keybuffer&) override;
    void                 processEvent (struct timeval*) override;

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
      button_up            = 0x60,
      button_down          = 0x61,
      button_up_move       = 0x60,
      button_down_move     = 0x61
    };

    // Constant
    static constexpr std::size_t MOUSE_BUF_SIZE = 4;

    // Methods
    void         setKeyState (int);
    void         setMoveState (const FPoint&, int);
    void         setButtonState (const int, const struct timeval*);

    // Data member
    char  x11_mouse[MOUSE_BUF_SIZE]{'\0'};
    uChar x11_button_state{all_buttons_released};
};


//----------------------------------------------------------------------
// class FMouseSGR
//----------------------------------------------------------------------

class FMouseSGR final : public FMouse
{
  public:
    // Constructor
    FMouseSGR() = default;

    // Destructor
    ~FMouseSGR() override = default;

    // Accessors
    FString       getClassName() const override;

    // Inquiry
    bool          hasData() override;

    // Methods
    void          setRawData (FKeyboard::keybuffer&) override;
    void          processEvent (struct timeval*) override;

  private:
    // Enumeration
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

    // Constant
    static constexpr std::size_t MOUSE_BUF_SIZE = 13;

    // Methods
    void          setKeyState (int);
    void          setMoveState (const FPoint&, int);
    void          setPressedButtonState (const int, const struct timeval*);
    void          setReleasedButtonState (const int);

    // Data members
    char  sgr_mouse[MOUSE_BUF_SIZE]{'\0'};
    uChar sgr_button_state{0x23};
};


//----------------------------------------------------------------------
// class FMouseUrxvt
//----------------------------------------------------------------------

class FMouseUrxvt final : public FMouse
{
  public:
    // Constructor
    FMouseUrxvt() = default;

    // Destructor
    ~FMouseUrxvt() override = default;

    // Accessors
    FString       getClassName() const override;

    // Inquiry
    bool          hasData() override;

    // Methods
    void          setRawData (FKeyboard::keybuffer&) override;
    void          processEvent (struct timeval*) override;

  private:
    // Enumeration
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

    // Constant
    static constexpr std::size_t MOUSE_BUF_SIZE = 13;

    // Methods
    void          setKeyState (int);
    void          setMoveState (const FPoint&, int);
    void          setButtonState (const int, const struct timeval*);

    // Data members
    char  urxvt_mouse[MOUSE_BUF_SIZE]{'\0'};
    uChar urxvt_button_state{all_buttons_released};
};


//----------------------------------------------------------------------
// class FMouseCommand
//----------------------------------------------------------------------

class FMouseCommand final
{
  public:
    // Constructors
    FMouseCommand () = default;
    explicit FMouseCommand (const std::function<void(const FMouseData&)>& fn)
      : handler(fn)
    { }

    // Method
    template <typename T>
    void execute(T&& arg) const
    {
      handler(std::forward<T>(arg));
    }

  private:
    // Data members
    std::function<void(const FMouseData&)> handler{};
};


//----------------------------------------------------------------------
// class FMouseControl
//----------------------------------------------------------------------

class FMouseControl
{
  public:
    // Constructor
    FMouseControl();

    // Disable copy constructor
    FMouseControl (const FMouseControl&) = delete;

    // Destructor
    virtual ~FMouseControl();

    // Disable copy assignment operator (=)
    FMouseControl& operator = (const FMouseControl&) = delete;

    // Accessors
    virtual FString           getClassName() const;
    const FPoint&             getPos();
    void                      clearEvent();

    // Mutators
    void                      setStdinNo (int);
    void                      setMaxWidth (uInt16);
    void                      setMaxHeight (uInt16);
    void                      setDblclickInterval (const uInt64);
    void                      setEventCommand (const FMouseCommand&);
    void                      useGpmMouse (bool = true);
    void                      useXtermMouse (bool = true);

    // Inquiries
    bool                      hasData();
    bool                      hasEvent();
    bool                      isLeftButtonPressed();
    bool                      isLeftButtonReleased();
    bool                      isLeftButtonDoubleClick();
    bool                      isRightButtonPressed();
    bool                      isRightButtonReleased();
    bool                      isMiddleButtonPressed();
    bool                      isMiddleButtonReleased();
    bool                      isShiftKeyPressed();
    bool                      isControlKeyPressed();
    bool                      isMetaKeyPressed();
    bool                      isWheelUp();
    bool                      isWheelDown();
    bool                      isMoved();
    bool                      hasUnprocessedInput();
    bool                      hasDataInQueue() const;
    bool                      isGpmMouseEnabled();

    // Methods
    void                      enable();
    void                      disable();
    virtual void              setRawData ( FMouse::mouse_type
                                         , FKeyboard::keybuffer& );
    virtual void              processEvent (struct timeval* time);
    void                      processQueuedInput();
    bool                      getGpmKeyPressed (bool);
    void                      drawGpmPointer();

  private:
    // Typedef
    typedef std::map<FMouse::mouse_type, FMouse*> FMouseProtocol;
    typedef std::unique_ptr<FMouseData> FMouseDataPtr;

    // Accessor
    FMouse*                   getMouseWithData();
    FMouse*                   getMouseWithEvent();
    void                      xtermMouse (bool) const;
    void                      enableXTermMouse() const;
    void                      disableXTermMouse() const;

    // Data member
    FMouseProtocol            mouse_protocol{};
    FMouseCommand             event_cmd{};
    std::queue<FMouseDataPtr> fmousedata_queue{};
    FPoint                    zero_point{0, 0};
    bool                      use_gpm_mouse{false};
    bool                      use_xterm_mouse{false};
};

// FMouseControl inline functions
//----------------------------------------------------------------------
inline FString FMouseControl::getClassName() const
{ return "FMouseControl"; }

//----------------------------------------------------------------------
inline void FMouseControl::setEventCommand (const FMouseCommand& cmd)
{ event_cmd = cmd; }

//----------------------------------------------------------------------
inline bool FMouseControl::hasDataInQueue() const
{ return ! fmousedata_queue.empty(); }

//----------------------------------------------------------------------
inline void FMouseControl::enableXTermMouse() const
{ xtermMouse(true); }

//----------------------------------------------------------------------
inline void FMouseControl::disableXTermMouse() const
{ xtermMouse(false); }

}  // namespace finalcut

#endif  // FMOUSE_H
