/***********************************************************************
* fevent.h - Base event class of widgets                               *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2021 Markus Gans                                      *
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
    Event getType() const;
    bool isQueued() const;
    bool wasSent() const;

  private:
    // Data members
    Event t{Event::None};
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
    FKeyEvent (Event, FKey);

    FKey     key() const;
    bool     isAccepted() const;
    void     accept();
    void     ignore();

  private:
    FKey     k{FKey::None};
    bool     accpt{false};
};


//----------------------------------------------------------------------
// class FMouseEvent
//----------------------------------------------------------------------

class FMouseEvent : public FEvent  // mouse event
{
  public:
    FMouseEvent (Event, const FPoint&, const FPoint&, MouseButton);
    FMouseEvent (Event, const FPoint&, MouseButton);

    const FPoint& getPos() const &;
    const FPoint& getTermPos() const &;
    int           getX() const;
    int           getY() const;
    int           getTermX() const;
    int           getTermY() const;
    MouseButton   getButton() const;
    void          setPos (const FPoint&);
    void          setTermPos (const FPoint&);

  private:
    FPoint        p{};
    FPoint        tp{};
    MouseButton   b{};
};


//----------------------------------------------------------------------
// class FWheelEvent
//----------------------------------------------------------------------

class FWheelEvent : public FEvent  // wheel event
{
  public:
    FWheelEvent (Event, const FPoint&, MouseWheel);
    FWheelEvent (Event, const FPoint&, const FPoint&, MouseWheel);

    const FPoint& getPos() const &;
    const FPoint& getTermPos() const &;
    int           getX() const;
    int           getY() const;
    int           getTermX() const;
    int           getTermY() const;
    MouseWheel    getWheel() const;

  private:
    FPoint        p{};
    FPoint        tp{};
    MouseWheel    w{MouseWheel::None};
};


//----------------------------------------------------------------------
// class FFocusEvent
//----------------------------------------------------------------------

class FFocusEvent : public FEvent  // focus event
{
  public:
    explicit FFocusEvent (Event);

    bool           gotFocus()  const;
    bool           lostFocus() const;
    FocusTypes     getFocusType() const;
    void           setFocusType (FocusTypes);
    bool           isAccepted() const;
    void           accept();
    void           ignore();

  private:
    bool           accpt{true};
    FocusTypes     focus_type{FocusTypes::DefiniteWidget};
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
    explicit FResizeEvent (Event);

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
    FTimerEvent (Event, int);

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
    FUserEvent (Event, int);

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
