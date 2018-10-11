/***********************************************************************
* fmouse.h - Read mouse events                                         *
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

#include "final/fconfig.h"
#include "final/fkeyboard.h"
#include "final/fpoint.h"
#include "final/ftypes.h"

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
    void                setMaxWidth (short);
    void                setMaxHeight (short);
    void                setDblclickInterval (const long);

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

    // Mutator
    void                setPos (const FPoint&);

    // Method
    bool                isDblclickTimeout (timeval*);

    // Data Members
    button              b_state;
    bool                mouse_event_occurred;
    bool                input_data_pending;
    long                dblclick_interval;
    short               max_width;
    short               max_height;
    struct timeval      time_mousepressed;
    FPoint              zero_point;
    FPoint              mouse;    // mouse click position
    FPoint              new_mouse_position;
};
#pragma pack(pop)


#ifdef F_HAVE_LIBGPM
//----------------------------------------------------------------------
// class FMouseGPM
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FMouseGPM : public FMouse
{
  public:
    // Constructor
    FMouseGPM();

    // Destructor
    virtual ~FMouseGPM();

    // Accessors
    virtual const char*  getClassName() const;

    // Mutators
    void                 setStdinNo(int);

    // Inquiry
    virtual bool         hasData();
    bool                 isGpmMouseEnabled();

    // Methods
    virtual void         setRawData (FKeyboard::keybuffer&);
    virtual void         processEvent (struct timeval*);
    bool                 gpmMouse (bool);
    bool                 enableGpmMouse();
    bool                 disableGpmMouse();
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
    Gpm_Event          gpm_ev;
    bool               has_gpm_mouse_data;
    bool               gpm_mouse_enabled;
    int                stdin_no;
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

class FMouseX11 : public FMouse
{
  public:
    // Constructor
    FMouseX11();

    // Destructor
    virtual ~FMouseX11();

    // Accessors
    virtual const char*  getClassName() const;

    // Inquiry
    virtual bool         hasData();

    // Methods
    virtual void         setRawData (FKeyboard::keybuffer&);
    virtual void         processEvent (struct timeval*);

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
    static const std::size_t MOUSE_BUF_SIZE = 4;

    // Method
    void         setKeyState (int);
    void         setMoveState (const FPoint&, int);
    void         setButtonState (int, struct timeval*);

    // Data Member
    char  x11_mouse[MOUSE_BUF_SIZE];
    uChar x11_button_state;
};
#pragma pack(pop)


//----------------------------------------------------------------------
// class FMouseSGR
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FMouseSGR : public FMouse
{
  public:
    // Constructor
    FMouseSGR();

    // Destructor
    virtual ~FMouseSGR();

    // Accessors
    virtual const char* getClassName() const;

    // Inquiry
    virtual bool hasData();

    // Methods
    virtual void setRawData (FKeyboard::keybuffer&);
    virtual void processEvent (struct timeval*);

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
    static const std::size_t MOUSE_BUF_SIZE = 13;

    // Methods
    void         setKeyState (int);
    void         setMoveState (const FPoint&, int);
    void         setPressedButtonState (int, struct timeval*);
    void         setReleasedButtonState (int);

    // Data Members
    char  sgr_mouse[MOUSE_BUF_SIZE];
    uChar sgr_button_state;
};
#pragma pack(pop)


//----------------------------------------------------------------------
// class FMouseUrxvt
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FMouseUrxvt : public FMouse
{
  public:
    // Constructor
    FMouseUrxvt();

    // Destructor
    virtual ~FMouseUrxvt();

    // Accessors
    virtual const char* getClassName() const;

    // Inquiry
    virtual bool hasData();

    // Methods
    virtual void setRawData (FKeyboard::keybuffer&);
    virtual void processEvent (struct timeval*);

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
    static const std::size_t MOUSE_BUF_SIZE = 13;

    // Methods
    void         setKeyState (int);
    void         setMoveState (const FPoint&, int);
    void         setButtonState (int, struct timeval*);

    // Data Members
    char  urxvt_mouse[MOUSE_BUF_SIZE];
    uChar urxvt_button_state;
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
    void                setMaxWidth (short);
    void                setMaxHeight (short);
    void                setDblclickInterval (const long);
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
    // Accessor
    FMouse*             getMouseWithData();
    FMouse*             getMouseWithEvent();
    void                xtermMouse (bool);
    void                enableXTermMouse();
    void                disableXTermMouse();

    // Data Member
    std::map<FMouse::mouse_type, FMouse*> mouse_protocol;
    std::map<FMouse::mouse_type, FMouse*>::iterator iter;
    FPoint zero_point;
    bool   use_gpm_mouse;
    bool   use_xterm_mouse;
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
