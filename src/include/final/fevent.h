/***********************************************************************
* fevent.h - Base event class of widgets                               *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2014-2019 Markus Gans                                      *
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
 * ▕▔▔▔▔▔▔▔▔▔▏
 * ▕ FEvent  ▏
 * ▕▁▁▁▁▁▁▁▁▁▏
 *      ▲
 *      │
 *      │    ▕▔▔▔▔▔▔▔▔▔▔▔▏
 *      ├─────▏FKeyEvent ▏
 *      │    ▕▁▁▁▁▁▁▁▁▁▁▁▏
 *      │
 *      │    ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *      ├─────▏FMouseEvent ▏
 *      │    ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *      │
 *      │    ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *      ├─────▏FWheelEvent ▏
 *      │    ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *      │
 *      │    ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *      ├─────▏FFocusEvent ▏
 *      │    ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *      │
 *      │    ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *      ├─────▏FAccelEvent ▏
 *      │    ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *      │
 *      │    ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *      ├─────▏FResizeEvent ▏
 *      │    ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *      │
 *      │    ▕▔▔▔▔▔▔▔▔▔▔▔▔▏
 *      ├─────▏FShowEvent ▏
 *      │    ▕▁▁▁▁▁▁▁▁▁▁▁▁▏
 *      │
 *      │    ▕▔▔▔▔▔▔▔▔▔▔▔▔▏
 *      ├─────▏FHideEvent ▏
 *      │    ▕▁▁▁▁▁▁▁▁▁▁▁▁▏
 *      │
 *      │    ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *      ├─────▏FCloseEvent ▏
 *      │    ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *      │
 *      │    ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *      ├─────▏FTimerEvent ▏
 *      │    ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *      │
 *      │    ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *      └─────▏FUserEvent  ▏
 *           ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FEVENT_H
#define FEVENT_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fc.h"
#include "final/fpoint.h"
#include "final/ftypes.h"

namespace finalcut
{

// class forward declaration
class FPoint;

//----------------------------------------------------------------------
// class FEvent
//----------------------------------------------------------------------

class FEvent  // event base class
{
  public:
    FEvent() = default;
    explicit FEvent(fc::events);
    fc::events type() const;

  private:
    fc::events t{fc::None_Event};
};


//----------------------------------------------------------------------
// class FKeyEvent
//----------------------------------------------------------------------

class FKeyEvent : public FEvent  // keyboard event
{
  public:
    FKeyEvent() = default;
    FKeyEvent (fc::events, FKey);
    ~FKeyEvent();

    FKey     key() const;
    bool     isAccepted() const;
    void     accept();
    void     ignore();

  private:
    FKey     k{0};
    bool     accpt{false};
};


//----------------------------------------------------------------------
// class FMouseEvent
//----------------------------------------------------------------------

class FMouseEvent : public FEvent  // mouse event
{
  public:
    FMouseEvent() = default;
    FMouseEvent (fc::events, const FPoint&, const FPoint&, int);
    FMouseEvent (fc::events, const FPoint&, int);
    ~FMouseEvent();

    const    FPoint& getPos() const;
    const    FPoint& getTermPos() const;
    int      getX() const;
    int      getY() const;
    int      getTermX() const;
    int      getTermY() const;
    int      getButton() const;

  private:
    FPoint   p{};
    FPoint   tp{};
    int      b{};
};


//----------------------------------------------------------------------
// class FWheelEvent
//----------------------------------------------------------------------

class FWheelEvent : public FEvent  // wheel event
{
  public:
    FWheelEvent() = default;
    FWheelEvent (fc::events, const FPoint&, int);
    FWheelEvent (fc::events, const FPoint&, const FPoint&, int);
    ~FWheelEvent();

    const    FPoint& getPos() const;
    const    FPoint& getTermPos() const;
    int      getX() const;
    int      getY() const;
    int      getTermX() const;
    int      getTermY() const;
    int      getWheel() const;

  private:
    FPoint   p;
    FPoint   tp;
    int      w;
};


//----------------------------------------------------------------------
// class FFocusEvent
//----------------------------------------------------------------------

class FFocusEvent : public FEvent  // focus event
{
  public:
    FFocusEvent() = default;
    explicit FFocusEvent (fc::events);
    ~FFocusEvent();

    bool     gotFocus()  const;
    bool     lostFocus() const;
    fc::FocusTypes getFocusType() const;
    void     setFocusType(fc::FocusTypes);
    bool     isAccepted() const;
    void     accept();
    void     ignore();

  private:
    bool     accpt{true};
    fc::FocusTypes focus_type{fc::FocusDefiniteWidget};
};


//----------------------------------------------------------------------
// class FAccelEvent
//----------------------------------------------------------------------

class FAccelEvent : public FEvent  // focus event
{
  public:
    FAccelEvent() = default;
    FAccelEvent (fc::events, void*);
    FAccelEvent (const FAccelEvent&) = delete;
    ~FAccelEvent();
    FAccelEvent& operator = (const FAccelEvent&) = delete;

    void*    focusedWidget() const;
    bool     isAccepted() const;
    void     accept();
    void     ignore();

  private:
    bool     accpt{false};
    void*    focus_widget{};
};


//----------------------------------------------------------------------
// class FResizeEvent
//----------------------------------------------------------------------

class FResizeEvent : public FEvent  // resize event
{
  public:
    FResizeEvent() = default;
    explicit FResizeEvent (fc::events);
    ~FResizeEvent();

    bool     isAccepted() const;
    void     accept();
    void     ignore();

  private:
    bool     accpt{false};
};


//----------------------------------------------------------------------
// class FShowEvent
//----------------------------------------------------------------------

class FShowEvent : public FEvent  // show event
{
  public:
    FShowEvent() = default;
    explicit FShowEvent (fc::events);
    ~FShowEvent();
};


//----------------------------------------------------------------------
// class FHideEvent
//----------------------------------------------------------------------

class FHideEvent : public FEvent  // hide event
{
  public:
    FHideEvent() = default;
    explicit FHideEvent (fc::events);
    ~FHideEvent();
};


//----------------------------------------------------------------------
// class FCloseEvent
//----------------------------------------------------------------------

class FCloseEvent : public FEvent  // close event
{
  public:
    FCloseEvent() = default;
    explicit FCloseEvent(fc::events);
    ~FCloseEvent();

    bool     isAccepted() const;
    void     accept();
    void     ignore();

  private:
    bool     accpt{false};
};


//----------------------------------------------------------------------
// class FTimerEvent
//----------------------------------------------------------------------

class FTimerEvent : public FEvent  // timer event
{
  public:
    FTimerEvent() = default;
    FTimerEvent (fc::events, int);
    ~FTimerEvent();

    int      getTimerId() const;

  private:
    int      id{0};
};


//----------------------------------------------------------------------
// class FUserEvent
//----------------------------------------------------------------------

class FUserEvent : public FEvent  // timer event
{
  public:
    FUserEvent() = default;

    // Disable copy constructor
    FUserEvent (const FUserEvent&) = delete;
    FUserEvent (fc::events, int);

    ~FUserEvent();

    // Disable assignment operator (=)
    FUserEvent& operator = (const FUserEvent&) = delete;

    int      getUserId() const;
    FDataPtr getData() const;
    void     setData (FDataPtr);

  private:
    int      uid{0};
    FDataPtr data_pointer{nullptr};
};

}  // namespace finalcut

#endif  // FEVENT_H
