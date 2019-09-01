/***********************************************************************
* fmouse.h - Read mouse events                                         *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2018-2019 Markus Gans                                      *
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

/*  Inheritance diagram
 *  ═══════════════════
 *
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
#include <map>

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
// class FMouse
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FMouse
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
    virtual ~FMouse()
    { }

    // Accessors
    virtual const char* getClassName() const;
    FPoint&             getPos();
    void                clearEvent();

    // Mutators
    void                setMaxWidth (uInt16);
    void                setMaxHeight (uInt16);
    void                setDblclickInterval (const uInt64);

    // Inquiries
    virtual bool        hasData() = 0;
    bool                hasEvent();
    bool                isLeftButtonPressed();
    bool                isLeftButtonReleased();
    bool                isLeftButtonDoubleClick();
    bool                isRightButtonPressed();
    bool                isRightButtonReleased();
    bool                isMiddleButtonPressed();
    bool                isMiddleButtonReleased();
    bool                isShiftKeyPressed();
    bool                isControlKeyPressed();
    bool                isMetaKeyPressed();
    bool                isWheelUp();
    bool                isWheelDown();
    bool                isMoved();
    bool                isInputDataPending();

    // Methods
    static FMouse*      createMouseObject (mouse_type);
    void                clearButtonState();
    virtual void        setRawData (FKeyboard::keybuffer&) = 0;
    virtual void        processEvent (struct timeval*) = 0;

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
    } button;  // bit field

    enum states
    {
      Undefined   = 0,
      Pressed     = 1,
      Released    = 2,
      DoubleClick = 3
    };

    // Accessors
    button&             getButtonState();
    FPoint&             getNewPos();
    uInt16              getMaxWidth();
    uInt16              getMaxHeight();
    uInt64              getDblclickInterval();
    timeval*            getMousePressedTime();

    // Mutator
    void                setPos (const FPoint&);
    void                setNewPos (int, int);
    void                setPending (bool);
    void                setEvent();
    void                setMousePressedTime (timeval*);
    void                resetMousePressedTime();

    // Inquiry
    bool                isDblclickTimeout (timeval*);

  private:
    // Data Members
    button              b_state{};
    bool                mouse_event_occurred{false};
    bool                input_data_pending{false};
    uInt16              max_width{80};
    uInt16              max_height{25};
    uInt64              dblclick_interval{500000};  // 500 ms
    struct timeval      time_mousepressed{};
    FPoint              mouse{0, 0};       // mouse click position
    FPoint              new_mouse_position{};
};
#pragma pack(pop)


#ifdef F_HAVE_LIBGPM
//----------------------------------------------------------------------
// class FMouseGPM
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FMouseGPM final : public FMouse
{
  public:
    // Constructor
    FMouseGPM();

    // Destructor
    virtual ~FMouseGPM();

    // Accessors
    const char*          getClassName() const override;

    // Mutators
    void                 setStdinNo(int);

    // Inquiry
    bool                 hasData() override;
    bool                 isGpmMouseEnabled();

    // Methods
    void                 setRawData (FKeyboard::keybuffer&) override;
    void                 processEvent (struct timeval*) override;
    bool                 gpmMouse (bool);
    bool                 enableGpmMouse();
    bool                 disableGpmMouse();
    bool                 hasSignificantEvents();
    void                 interpretKeyDown();
    void                 interpretKeyUp();
    bool                 getGpmKeyPressed(bool);
    void                 drawGpmPointer();

  private:
    // Enumeration
    enum gpmEventType
    {
      no_event       = 0,
      keyboard_event = 1,
      mouse_event    = 2
    };

    // Method
    int                gpmEvent (bool = true);

    // Data Member
    Gpm_Event          gpm_ev{};
    bool               has_gpm_mouse_data{false};
    bool               gpm_mouse_enabled{false};
    int                stdin_no{0};
};
#pragma pack(pop)

//----------------------------------------------------------------------
inline bool FMouseGPM::enableGpmMouse()
{ return gpmMouse(true); }

//----------------------------------------------------------------------
inline bool FMouseGPM::disableGpmMouse()
{ return gpmMouse(false); }

//----------------------------------------------------------------------
inline bool FMouseGPM::isGpmMouseEnabled()
{ return gpm_mouse_enabled; }
#endif  // F_HAVE_LIBGPM


//----------------------------------------------------------------------
// class FMouseX11
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FMouseX11 final : public FMouse
{
  public:
    // Constructor
    FMouseX11() = default;

    // Destructor
    virtual ~FMouseX11() = default;

    // Accessors
    const char*          getClassName() const override;

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

    // Method
    void         setKeyState (int);
    void         setMoveState (const FPoint&, int);
    void         setButtonState (int, struct timeval*);

    // Data Member
    char  x11_mouse[MOUSE_BUF_SIZE]{'\0'};
    uChar x11_button_state{all_buttons_released};
};
#pragma pack(pop)


//----------------------------------------------------------------------
// class FMouseSGR
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FMouseSGR final : public FMouse
{
  public:
    // Constructor
    FMouseSGR() = default;

    // Destructor
    virtual ~FMouseSGR() = default;

    // Accessors
    const char*  getClassName() const override;

    // Inquiry
    bool         hasData() override;

    // Methods
    void         setRawData (FKeyboard::keybuffer&) override;
    void         processEvent (struct timeval*) override;

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
    void         setKeyState (int);
    void         setMoveState (const FPoint&, int);
    void         setPressedButtonState (int, struct timeval*);
    void         setReleasedButtonState (int);

    // Data Members
    char  sgr_mouse[MOUSE_BUF_SIZE]{'\0'};
    uChar sgr_button_state{0x23};
};
#pragma pack(pop)


//----------------------------------------------------------------------
// class FMouseUrxvt
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FMouseUrxvt final : public FMouse
{
  public:
    // Constructor
    FMouseUrxvt() = default;

    // Destructor
    virtual ~FMouseUrxvt() = default;

    // Accessors
    const char*  getClassName() const override;

    // Inquiry
    bool         hasData() override;

    // Methods
    void         setRawData (FKeyboard::keybuffer&) override;
    void         processEvent (struct timeval*) override;

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
    void         setKeyState (int);
    void         setMoveState (const FPoint&, int);
    void         setButtonState (int, struct timeval*);

    // Data Members
    char  urxvt_mouse[MOUSE_BUF_SIZE]{'\0'};
    uChar urxvt_button_state{all_buttons_released};
};
#pragma pack(pop)


//----------------------------------------------------------------------
// class FMouseControl
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)
class FMouseControl
{
  public:
    // Constructor
    FMouseControl();

    // Destructor
    virtual ~FMouseControl();

    // Accessors
    virtual const char* getClassName() const;
    FPoint&             getPos();
    void                clearEvent();

    // Mutators
    void                setStdinNo (int);
    void                setMaxWidth (uInt16);
    void                setMaxHeight (uInt16);
    void                setDblclickInterval (const uInt64);
    void                useGpmMouse (bool = true);
    void                useXtermMouse (bool = true);

    // Inquiries
    bool                hasData();
    bool                hasEvent();
    bool                isLeftButtonPressed();
    bool                isLeftButtonReleased();
    bool                isLeftButtonDoubleClick();
    bool                isRightButtonPressed();
    bool                isRightButtonReleased();
    bool                isMiddleButtonPressed();
    bool                isMiddleButtonReleased();
    bool                isShiftKeyPressed();
    bool                isControlKeyPressed();
    bool                isMetaKeyPressed();
    bool                isWheelUp();
    bool                isWheelDown();
    bool                isMoved();
    bool                isInputDataPending();
    bool                isGpmMouseEnabled();

    // Methods
    void                enable();
    void                disable();
    virtual void        setRawData ( FMouse::mouse_type
                                   , FKeyboard::keybuffer& );
    virtual void        processEvent (struct timeval* time);
    bool                getGpmKeyPressed (bool);
    void                drawGpmPointer();

  private:
    // Typedef
    typedef std::map<FMouse::mouse_type, FMouse*> FMouseProtocol;

    // Accessor
    FMouse*             getMouseWithData();
    FMouse*             getMouseWithEvent();
    void                xtermMouse (bool);
    void                enableXTermMouse();
    void                disableXTermMouse();

    // Data Member
    FMouseProtocol mouse_protocol{};
    FPoint         zero_point{0, 0};
    bool           use_gpm_mouse{false};
    bool           use_xterm_mouse{false};
};
#pragma pack(pop)

// FMouseControl inline functions
//----------------------------------------------------------------------
inline const char* FMouseControl::getClassName() const
{ return "FMouseControl"; }

//----------------------------------------------------------------------
inline void FMouseControl::enableXTermMouse()
{ xtermMouse(true); }

//----------------------------------------------------------------------
inline void FMouseControl::disableXTermMouse()
{ xtermMouse(false); }

}  // namespace finalcut

#endif  // FMOUSE_H
