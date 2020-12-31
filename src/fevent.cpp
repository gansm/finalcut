/***********************************************************************
* fevent.cpp - Base event class of widgets                             *
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
Event FEvent::getType() const
{ return t; }

//----------------------------------------------------------------------
bool FEvent::isQueued() const
{ return queued; }

//----------------------------------------------------------------------
bool FEvent::wasSent() const
{ return send; }


//----------------------------------------------------------------------
// class FKeyEvent
//----------------------------------------------------------------------

FKeyEvent::FKeyEvent (Event ev_type, FKey key_num)  // constructor
  : FEvent{ev_type}
  , k{key_num}
{ }

//----------------------------------------------------------------------
FKey FKeyEvent::key() const
{ return k; }

//----------------------------------------------------------------------
bool FKeyEvent::isAccepted() const
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
const FPoint& FMouseEvent::getPos() const
{ return p; }

//----------------------------------------------------------------------
const FPoint& FMouseEvent::getTermPos() const
{ return tp; }

//----------------------------------------------------------------------
int FMouseEvent::getX() const
{ return p.getX(); }

//----------------------------------------------------------------------
int FMouseEvent::getY() const
{ return p.getY(); }

//----------------------------------------------------------------------
int FMouseEvent::getTermX() const
{ return tp.getX(); }

//----------------------------------------------------------------------
int FMouseEvent::getTermY() const
{ return tp.getY(); }

//----------------------------------------------------------------------
MouseButton FMouseEvent::getButton() const
{ return b; }


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
const FPoint& FWheelEvent::getPos() const
{ return p; }

//----------------------------------------------------------------------
const FPoint& FWheelEvent::getTermPos() const
{ return tp; }

//----------------------------------------------------------------------
int FWheelEvent::getX() const
{ return p.getX(); }

//----------------------------------------------------------------------
int FWheelEvent::getY() const
{ return p.getY(); }

//----------------------------------------------------------------------
int FWheelEvent::getTermX() const
{ return tp.getX(); }

//----------------------------------------------------------------------
int FWheelEvent::getTermY() const
{ return tp.getY(); }

//----------------------------------------------------------------------
MouseWheel FWheelEvent::getWheel() const
{ return w; }


//----------------------------------------------------------------------
// class FFocusEvent
//----------------------------------------------------------------------

FFocusEvent::FFocusEvent (Event ev_type)  // constructor
  : FEvent{ev_type}
{ }

//----------------------------------------------------------------------
bool FFocusEvent::gotFocus() const
{
  return ( getType() == Event::FocusIn );
}

//----------------------------------------------------------------------
bool FFocusEvent::lostFocus() const
{
  return ( getType() == Event::FocusOut );
}

//----------------------------------------------------------------------
FocusTypes FFocusEvent::getFocusType() const
{ return focus_type; }

//----------------------------------------------------------------------
void FFocusEvent::setFocusType (FocusTypes ft)
{ focus_type = ft; }

//----------------------------------------------------------------------
bool FFocusEvent::isAccepted() const
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
FWidget* FAccelEvent::focusedWidget() const
{ return focus_widget; }

//----------------------------------------------------------------------
bool FAccelEvent::isAccepted() const
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
bool FResizeEvent::isAccepted() const
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
bool FCloseEvent::isAccepted() const
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
int FTimerEvent::getTimerId() const
{ return id; }


//----------------------------------------------------------------------
// class FUserEvent
//----------------------------------------------------------------------

FUserEvent::FUserEvent (Event ev_type, int user_event_id)  // constructor
  : FEvent{ev_type}
  , uid{user_event_id}
{ }

//----------------------------------------------------------------------
int FUserEvent::getUserId() const
{ return uid; }

}  // namespace finalcut
