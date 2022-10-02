/***********************************************************************
* fevent.h - Base event class of widgets                               *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2022 Markus Gans                                      *
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
#include <utility>

#include "final/fc.h"
#include "final/ftypes.h"
#include "final/util/fdata.h"
#include "final/util/fpoint.h"

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
    explicit FEvent(Event);
    auto getType() const -> Event;
    auto isQueued() const -> bool;
    auto wasSent() const -> bool;

  private:
    friend void setSend (FEvent&, bool);
    friend void setQueued (FEvent&, bool);

    // Data members
    Event t{Event::None};
    bool  queued{false};
    bool  send{false};
};


//----------------------------------------------------------------------
// class FKeyEvent
//----------------------------------------------------------------------

class FKeyEvent : public FEvent  // keyboard event
{
  public:
    FKeyEvent (Event, FKey);

    auto key() const -> FKey;
    auto isAccepted() const -> bool;
    void accept();
    void ignore();

  private:
    FKey k{};
    bool accpt{false};  // reject by default
};


//----------------------------------------------------------------------
// class FMouseEvent
//----------------------------------------------------------------------

class FMouseEvent : public FEvent  // mouse event
{
  public:
    FMouseEvent (Event, const FPoint&, const FPoint&, MouseButton);
    FMouseEvent (Event, const FPoint&, MouseButton);

    auto getPos() const & -> const FPoint&;
    auto getTermPos() const & -> const FPoint&;
    auto getX() const -> int;
    auto getY() const -> int;
    auto getTermX() const -> int;
    auto getTermY() const -> int;
    auto getButton() const -> MouseButton;
    void setPos (const FPoint&);
    void setTermPos (const FPoint&);

  private:
    FPoint      p{};
    FPoint      tp{};
    MouseButton b{};
};


//----------------------------------------------------------------------
// class FWheelEvent
//----------------------------------------------------------------------

class FWheelEvent : public FEvent  // wheel event
{
  public:
    FWheelEvent (Event, const FPoint&, MouseWheel);
    FWheelEvent (Event, const FPoint&, const FPoint&, MouseWheel);

    auto getPos() const & -> const FPoint&;
    auto getTermPos() const & -> const FPoint&;
    auto getX() const -> int;
    auto getY() const -> int;
    auto getTermX() const -> int;
    auto getTermY() const -> int;
    auto getWheel() const -> MouseWheel;

  private:
    FPoint     p{};
    FPoint     tp{};
    MouseWheel w{MouseWheel::None};
};


//----------------------------------------------------------------------
// class FFocusEvent
//----------------------------------------------------------------------

class FFocusEvent : public FEvent  // focus event
{
  public:
    explicit FFocusEvent (Event);

    auto gotFocus()  const -> bool;
    auto lostFocus() const -> bool;
    auto getFocusType() const -> FocusTypes;
    void setFocusType (FocusTypes);
    auto isAccepted() const -> bool;
    void accept();
    void ignore();

  private:
    bool       accpt{true};  // accept by default
    FocusTypes focus_type{FocusTypes::DefiniteWidget};
};


//----------------------------------------------------------------------
// class FAccelEvent
//----------------------------------------------------------------------
class FWidget;  // class forward declaration

class FAccelEvent : public FEvent  // focus event
{
  public:
    FAccelEvent (Event, FWidget*);
    FAccelEvent (const FAccelEvent&) = delete;
    auto operator = (const FAccelEvent&) -> FAccelEvent& = delete;

    auto focusedWidget() const -> FWidget*;
    auto isAccepted() const -> bool;
    void accept();
    void ignore();

  private:
    bool     accpt{false};  // reject by default
    FWidget* focus_widget{};
};


//----------------------------------------------------------------------
// class FResizeEvent
//----------------------------------------------------------------------

class FResizeEvent : public FEvent  // resize event
{
  public:
    explicit FResizeEvent (Event);

    auto isAccepted() const -> bool;
    void accept();
    void ignore();

  private:
    bool accpt{false};  // reject by default
};


//----------------------------------------------------------------------
// class FShowEvent
//----------------------------------------------------------------------

class FShowEvent : public FEvent  // show event
{
  public:
    explicit FShowEvent (Event);
};


//----------------------------------------------------------------------
// class FHideEvent
//----------------------------------------------------------------------

class FHideEvent : public FEvent  // hide event
{
  public:
    explicit FHideEvent (Event);
};


//----------------------------------------------------------------------
// class FCloseEvent
//----------------------------------------------------------------------

class FCloseEvent : public FEvent  // close event
{
  public:
    explicit FCloseEvent(Event);

    auto isAccepted() const -> bool;
    void accept();
    void ignore();

  private:
    bool accpt{false};  // reject by default
};


//----------------------------------------------------------------------
// class FTimerEvent
//----------------------------------------------------------------------

class FTimerEvent : public FEvent  // timer event
{
  public:
    FTimerEvent (Event, int);

    auto getTimerId() const -> int;

  private:
    int id{0};
};


//----------------------------------------------------------------------
// class FUserEvent
//----------------------------------------------------------------------

class FUserEvent : public FEvent  // user event
{
  public:
    FUserEvent (Event, int);

    auto getUserId() const -> int;
    template <typename T>
    auto getFDataObject() const -> FData<T>&&;
    template <typename T>
    auto getData() const -> clean_fdata_t<T>&;
    template <typename T>
    void setFDataObject (T&&);
    template <typename T>
    void setData (T&&);

  private:
    // Using-declaration
    using FDataAccessPtr = std::shared_ptr<FDataAccess>;

    // Data members
    int            uid{0};
    FDataAccessPtr data_pointer{nullptr};
};

//----------------------------------------------------------------------
template <typename T>
inline auto FUserEvent::getFDataObject() const -> FData<T>&&
{
  return static_cast<FData<T>&&>(*data_pointer);
}

//----------------------------------------------------------------------
template <typename T>
inline auto FUserEvent::getData() const -> clean_fdata_t<T>&
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
