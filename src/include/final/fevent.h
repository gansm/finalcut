/***********************************************************************
* fevent.h - Base event class of widgets                               *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2020 Markus Gans                                      *
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
 *      │    ▕▔▔▔▔▔▔▔▔▔▔▔▔▏1     1▕▔▔▔▔▔▔▔▏
 *      └─────▏FUserEvent ▏- - - -▕ FData ▏
 *           ▕▁▁▁▁▁▁▁▁▁▁▁▁▏       ▕▁▁▁▁▁▁▁▏
 */

#ifndef FEVENT_H
#define FEVENT_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <memory>

#include "final/fc.h"
#include "final/fdata.h"
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
    fc::events getType() const;
    bool isQueued() const;
    bool wasSent() const;

  private:
    // Data members
    fc::events t{fc::None_Event};
    bool queued{false};
    bool send{false};

    // Friend class
    friend class FApplication;
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

    const FPoint& getPos() const;
    const FPoint& getTermPos() const;
    int           getX() const;
    int           getY() const;
    int           getTermX() const;
    int           getTermY() const;
    int           getButton() const;

  private:
    FPoint  p{};
    FPoint  tp{};
    int     b{};
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

    const FPoint& getPos() const;
    const FPoint& getTermPos() const;
    int           getX() const;
    int           getY() const;
    int           getTermX() const;
    int           getTermY() const;
    int           getWheel() const;

  private:
    FPoint  p{};
    FPoint  tp{};
    int     w{};
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

    bool           gotFocus()  const;
    bool           lostFocus() const;
    fc::FocusTypes getFocusType() const;
    void           setFocusType(fc::FocusTypes);
    bool           isAccepted() const;
    void           accept();
    void           ignore();

  private:
    bool           accpt{true};
    fc::FocusTypes focus_type{fc::FocusDefiniteWidget};
};


//----------------------------------------------------------------------
// class FAccelEvent
//----------------------------------------------------------------------
class FWidget;  // class forward declaration

class FAccelEvent : public FEvent  // focus event
{
  public:
    FAccelEvent() = default;
    FAccelEvent (fc::events, FWidget*);
    FAccelEvent (const FAccelEvent&) = delete;
    ~FAccelEvent();
    FAccelEvent& operator = (const FAccelEvent&) = delete;

    FWidget* focusedWidget() const;
    bool     isAccepted() const;
    void     accept();
    void     ignore();

  private:
    bool     accpt{false};
    FWidget* focus_widget{};
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

class FUserEvent : public FEvent  // user event
{
  public:
    FUserEvent() = default;

    // Disable copy constructor
    FUserEvent (const FUserEvent&) = delete;
    FUserEvent (fc::events, int);

    ~FUserEvent();

    // Disable copy assignment operator (=)
    FUserEvent& operator = (const FUserEvent&) = delete;

    int               getUserId() const;
    template <typename T>
    FData<T>&&        getFDataObject() const;
    template <typename T>
    clean_fdata_t<T>& getData() const;
    template <typename T>
    void              setFDataObject (T&&);
    template <typename T>
    void              setData (T&&);

  private:
    // Using-declaration
    using FDataAccessPtr = std::shared_ptr<FDataAccess>;

    // Data members
    int               uid{0};
    FDataAccessPtr    data_pointer{nullptr};
};

//----------------------------------------------------------------------
template <typename T>
inline FData<T>&& FUserEvent::getFDataObject() const
{
  return static_cast<FData<T>&&>(*data_pointer);
}

//----------------------------------------------------------------------
template <typename T>
inline clean_fdata_t<T>& FUserEvent::getData() const
{
  return static_cast<FData<clean_fdata_t<T>>&>(*data_pointer).get();
}

//----------------------------------------------------------------------
template <typename T>
inline void FUserEvent::setFDataObject (T&& fdata)
{
  data_pointer.reset(&(std::forward<T>(fdata)));
}

//----------------------------------------------------------------------
template <typename T>
inline void FUserEvent::setData (T&& data)
{
  data_pointer.reset(makeFData(std::forward<T>(data)));
}

}  // namespace finalcut

#endif  // FEVENT_H
