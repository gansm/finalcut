/***********************************************************************
* fevent.h - Base event class of widgets                               *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2014-2018 Markus Gans                                      *
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
 *      └─────▏FTimerEvent ▏
 *           ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FEVENT_H
#define FEVENT_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fc.h"
#include "final/fpoint.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FEvent
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FEvent  // event base class
{
  public:
    explicit FEvent(int);
    virtual ~FEvent();
    int type() const;

  protected:
    int t;
};

#pragma pack(pop)


//----------------------------------------------------------------------
// class FKeyEvent
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FKeyEvent : public FEvent  // keyboard event
{
  public:
    FKeyEvent (int, FKey);
    ~FKeyEvent();

    FKey key() const;
    bool isAccepted() const;
    void accept();
    void ignore();

  protected:
    FKey  k;
    bool accpt;
};

#pragma pack(pop)


//----------------------------------------------------------------------
// class FMouseEvent
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FMouseEvent : public FEvent  // mouse event
{
  public:
    FMouseEvent (int, const FPoint&, int);
    FMouseEvent (int, const FPoint&, const FPoint&, int);
    ~FMouseEvent();

    const FPoint& getPos() const;
    const FPoint& getTermPos() const;
    int getX() const;
    int getY() const;
    int getTermX() const;
    int getTermY() const;
    int getButton() const;

  protected:
    FPoint p;
    FPoint tp;
    int b;
};

#pragma pack(pop)


//----------------------------------------------------------------------
// class FWheelEvent
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FWheelEvent : public FEvent  // wheel event
{
  public:
    FWheelEvent (int, const FPoint&, int);
    FWheelEvent (int, const FPoint&, const FPoint&, int);
    ~FWheelEvent();

    const FPoint& getPos() const;
    const FPoint& getTermPos() const;
    int getX() const;
    int getY() const;
    int getTermX() const;
    int getTermY() const;
    int getWheel() const;

  protected:
    FPoint p;
    FPoint tp;
    int w;
};

#pragma pack(pop)


//----------------------------------------------------------------------
// class FFocusEvent
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FFocusEvent : public FEvent  // focus event
{
  public:
    explicit FFocusEvent (int);
    ~FFocusEvent();

    bool gotFocus()  const;
    bool lostFocus() const;
    fc::FocusTypes getFocusType() const;
    void setFocusType(fc::FocusTypes);
    bool isAccepted() const;
    void accept();
    void ignore();
  protected:
    bool accpt;
    fc::FocusTypes focus_type;
};

#pragma pack(pop)


//----------------------------------------------------------------------
// class FAccelEvent
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FAccelEvent : public FEvent  // focus event
{
  private:
    // Disable copy constructor
    FAccelEvent (const FAccelEvent&);
    // Disable assignment operator (=)
    FAccelEvent& operator = (const FAccelEvent&);

  public:
    FAccelEvent (int, void*);
    ~FAccelEvent();

    void*    focusedWidget() const;
    bool     isAccepted() const;
    void     accept();
    void     ignore();

  protected:
    bool     accpt;
    void*    focus_widget;
};

#pragma pack(pop)


//----------------------------------------------------------------------
// class FResizeEvent
//----------------------------------------------------------------------

class FResizeEvent : public FEvent  // resize event
{
  public:
    explicit FResizeEvent (int);
    ~FResizeEvent();

    bool isAccepted() const;
    void accept();
    void ignore();

  protected:
    bool accpt;
};



//----------------------------------------------------------------------
// class FShowEvent
//----------------------------------------------------------------------

class FShowEvent : public FEvent  // show event
{
  public:
    explicit FShowEvent (int);
    ~FShowEvent();
};



//----------------------------------------------------------------------
// class FHideEvent
//----------------------------------------------------------------------

class FHideEvent : public FEvent  // hide event
{
  public:
    explicit FHideEvent (int);
    ~FHideEvent();
};



//----------------------------------------------------------------------
// class FCloseEvent
//----------------------------------------------------------------------

class FCloseEvent : public FEvent  // close event
{
  public:
    explicit FCloseEvent(int);
    ~FCloseEvent();

    bool isAccepted() const;
    void accept();
    void ignore();

  protected:
    bool accpt;
};



//----------------------------------------------------------------------
// class FTimerEvent
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FTimerEvent : public FEvent  // timer event
{
  public:
    FTimerEvent(int, int);
    ~FTimerEvent();

    int timerId() const;

  protected:
    int id;
};

#pragma pack(pop)

}  // namespace finalcut

#endif  // FEVENT_H
