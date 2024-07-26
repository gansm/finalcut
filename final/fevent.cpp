/***********************************************************************
* fevent.cpp - Base event class of widgets                             *
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

#include <cstdio>
#include "final/fevent.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FEvent
//----------------------------------------------------------------------

FEvent::FEvent (Event ev_type)  // constructor
  : t{ev_type}
{ }

//----------------------------------------------------------------------
auto FEvent::getType() const -> Event
{ return t; }

//----------------------------------------------------------------------
auto FEvent::isQueued() const -> bool
{ return queued; }

//----------------------------------------------------------------------
auto FEvent::wasSent() const -> bool
{ return send; }


//----------------------------------------------------------------------
// class FKeyEvent
//----------------------------------------------------------------------

FKeyEvent::FKeyEvent (Event ev_type, FKey key_num)  // constructor
  : FEvent{ev_type}
  , k{key_num}
{ }

//----------------------------------------------------------------------
auto FKeyEvent::key() const -> FKey
{ return k; }

//----------------------------------------------------------------------
auto FKeyEvent::isAccepted() const -> bool
{ return accpt; }

//----------------------------------------------------------------------
void FKeyEvent::accept()
{ accpt = true; }

//----------------------------------------------------------------------
void FKeyEvent::ignore()
{ accpt = false; }


//----------------------------------------------------------------------
// class FMouseEvent
//----------------------------------------------------------------------

FMouseEvent::FMouseEvent ( Event ev_type  // constructor
                         , const FPoint& pos
                         , const FPoint& termPos
                         , MouseButton button )
  : FEvent{ev_type}
  , p{pos}
  , tp{termPos}
  , b{button}
{ }

//----------------------------------------------------------------------
FMouseEvent::FMouseEvent ( Event ev_type  // constructor
                         , const FPoint& pos
                         , MouseButton button )
  : FMouseEvent{ev_type, pos, FPoint{}, button}
{ }

//----------------------------------------------------------------------
auto FMouseEvent::getPos() const & -> const FPoint&
{ return p; }

//----------------------------------------------------------------------
auto FMouseEvent::getTermPos() const & -> const FPoint&
{ return tp; }

//----------------------------------------------------------------------
auto FMouseEvent::getX() const -> int
{ return p.getX(); }

//----------------------------------------------------------------------
auto FMouseEvent::getY() const -> int
{ return p.getY(); }

//----------------------------------------------------------------------
auto FMouseEvent::getTermX() const -> int
{ return tp.getX(); }

//----------------------------------------------------------------------
auto FMouseEvent::getTermY() const -> int
{ return tp.getY(); }

//----------------------------------------------------------------------
auto FMouseEvent::getButton() const -> MouseButton
{ return b; }

//----------------------------------------------------------------------
void FMouseEvent::setPos (const FPoint& pos)
{ p = pos; }

//----------------------------------------------------------------------
void FMouseEvent::setTermPos (const FPoint& termPos)
{ tp = termPos; }


//----------------------------------------------------------------------
// class FWheelEvent
//----------------------------------------------------------------------

FWheelEvent::FWheelEvent ( Event ev_type  // constructor
                         , const FPoint& pos
                         , const FPoint& termPos
                         , MouseWheel wheel )
  : FEvent{ev_type}
  , p{pos}
  , tp{termPos}
  , w{wheel}
{ }

//----------------------------------------------------------------------
FWheelEvent::FWheelEvent ( Event ev_type  // constructor
                         , const FPoint& pos
                         , MouseWheel wheel )
  : FWheelEvent{ev_type, pos, FPoint{}, wheel}
{ }

//----------------------------------------------------------------------
auto FWheelEvent::getPos() const & -> const FPoint&
{ return p; }

//----------------------------------------------------------------------
auto FWheelEvent::getTermPos() const & -> const FPoint&
{ return tp; }

//----------------------------------------------------------------------
auto FWheelEvent::getX() const -> int
{ return p.getX(); }

//----------------------------------------------------------------------
auto FWheelEvent::getY() const -> int
{ return p.getY(); }

//----------------------------------------------------------------------
auto FWheelEvent::getTermX() const -> int
{ return tp.getX(); }

//----------------------------------------------------------------------
auto FWheelEvent::getTermY() const -> int
{ return tp.getY(); }

//----------------------------------------------------------------------
auto FWheelEvent::getWheel() const -> MouseWheel
{ return w; }


//----------------------------------------------------------------------
// class FFocusEvent
//----------------------------------------------------------------------

FFocusEvent::FFocusEvent (Event ev_type)  // constructor
  : FEvent{ev_type}
{ }

//----------------------------------------------------------------------
auto FFocusEvent::gotFocus() const -> bool
{
  return getType() == Event::FocusIn;
}

//----------------------------------------------------------------------
auto FFocusEvent::lostFocus() const -> bool
{
  return getType() == Event::FocusOut;
}

//----------------------------------------------------------------------
auto FFocusEvent::getFocusType() const -> FocusTypes
{ return focus_type; }

//----------------------------------------------------------------------
void FFocusEvent::setFocusType (FocusTypes ft)
{ focus_type = ft; }

//----------------------------------------------------------------------
auto FFocusEvent::isAccepted() const -> bool
{ return accpt; }

//----------------------------------------------------------------------
void FFocusEvent::accept()
{ accpt = true; }

//----------------------------------------------------------------------
void FFocusEvent::ignore()
{ accpt = false; }


//----------------------------------------------------------------------
// class FAccelEvent
//----------------------------------------------------------------------

FAccelEvent::FAccelEvent (Event ev_type, FWidget* focused)  // constructor
  : FEvent{ev_type}
  , focus_widget{focused}
{ }

//----------------------------------------------------------------------
auto FAccelEvent::focusedWidget() const -> FWidget*
{ return focus_widget; }

//----------------------------------------------------------------------
auto FAccelEvent::isAccepted() const -> bool
{ return accpt; }

//----------------------------------------------------------------------
void FAccelEvent::accept()
{ accpt = true; }

//----------------------------------------------------------------------
void FAccelEvent::ignore()
{ accpt = false; }


//----------------------------------------------------------------------
// class FResizeEvent
//----------------------------------------------------------------------

FResizeEvent::FResizeEvent (Event ev_type)  // constructor
  : FEvent{ev_type}
{ }

//----------------------------------------------------------------------
auto FResizeEvent::isAccepted() const -> bool
{ return accpt; }

//----------------------------------------------------------------------
void FResizeEvent::accept()
{ accpt = true; }

//----------------------------------------------------------------------
void FResizeEvent::ignore()
{ accpt = false; }


//----------------------------------------------------------------------
// class FShowEvent
//----------------------------------------------------------------------

FShowEvent::FShowEvent (Event ev_type)  // constructor
  : FEvent{ev_type}
{ }


//----------------------------------------------------------------------
// class FHideEvent
//----------------------------------------------------------------------

FHideEvent::FHideEvent (Event ev_type)  // constructor
  : FEvent{ev_type}
{ }


//----------------------------------------------------------------------
// class FCloseEvent
//----------------------------------------------------------------------

FCloseEvent::FCloseEvent (Event ev_type)  // constructor
  : FEvent{ev_type}
{ }

//----------------------------------------------------------------------
auto FCloseEvent::isAccepted() const -> bool
{ return accpt; }

//----------------------------------------------------------------------
void FCloseEvent::accept()
{ accpt = true; }

//----------------------------------------------------------------------
void FCloseEvent::ignore()
{ accpt = false; }


//----------------------------------------------------------------------
// class FTimerEvent
//----------------------------------------------------------------------

FTimerEvent::FTimerEvent (Event ev_type, int timer_id)  // constructor
  : FEvent{ev_type}
  , id{timer_id}
{ }

//----------------------------------------------------------------------
auto FTimerEvent::getTimerId() const -> int
{ return id; }


//----------------------------------------------------------------------
// class FUserEvent
//----------------------------------------------------------------------

FUserEvent::FUserEvent (Event ev_type, int user_event_id)  // constructor
  : FEvent{ev_type}
  , uid{user_event_id}
{ }

//----------------------------------------------------------------------
auto FUserEvent::getUserId() const -> int
{ return uid; }

}  // namespace finalcut
