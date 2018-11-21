/***********************************************************************
* fevent.cpp - Base event class of widgets                             *
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

#include <cstdio>

#include "final/fevent.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FEvent
//----------------------------------------------------------------------

FEvent::FEvent(int ev_type)  // constructor
  : t(ev_type)
{ }

//----------------------------------------------------------------------
FEvent::~FEvent()  // destructor
{ }

//----------------------------------------------------------------------
int FEvent::type() const
{ return t; }


//----------------------------------------------------------------------
// class FKeyEvent
//----------------------------------------------------------------------

FKeyEvent::FKeyEvent (int ev_type, FKey key_num)  // constructor
  : FEvent(ev_type)
  , k(key_num)
  , accpt(false)
{ }

//----------------------------------------------------------------------
FKeyEvent::~FKeyEvent()  // destructor
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

FMouseEvent::FMouseEvent ( int ev_type         // constructor
                         , const FPoint& pos
                         , int button )
  : FEvent(ev_type)
  , p(pos)
  , tp()
  , b(button)
{ }

//----------------------------------------------------------------------
FMouseEvent::FMouseEvent ( int ev_type         // constructor
                         , const FPoint& pos
                         , const FPoint& termPos
                         , int button )
  : FEvent(ev_type)
  , p(pos)
  , tp(termPos)
  , b(button)
{ }

//----------------------------------------------------------------------
FMouseEvent::~FMouseEvent()  // destructor
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
int FMouseEvent::getButton() const
{ return b; }


//----------------------------------------------------------------------
// class FWheelEvent
//----------------------------------------------------------------------

FWheelEvent::FWheelEvent ( int ev_type         // constructor
                         , const FPoint& pos
                         , int wheel )
  : FEvent(ev_type)
  , p(pos)
  , tp()
  , w(wheel)
{ }

//----------------------------------------------------------------------
FWheelEvent::FWheelEvent ( int ev_type         // constructor
                         , const FPoint& pos
                         , const FPoint& termPos
                         , int wheel )
  : FEvent(ev_type)
  , p(pos)
  , tp(termPos)
  , w(wheel)
{ }

//----------------------------------------------------------------------
FWheelEvent::~FWheelEvent()  // destructor
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
int FWheelEvent::getWheel() const
{ return w; }


//----------------------------------------------------------------------
// class FFocusEvent
//----------------------------------------------------------------------

FFocusEvent::FFocusEvent (int ev_type)  // constructor
  : FEvent(ev_type)
  , accpt(true)
  , focus_type(fc::FocusDefiniteWidget)
{ }

//----------------------------------------------------------------------
FFocusEvent::~FFocusEvent()  // destructor
{ }

//----------------------------------------------------------------------
bool FFocusEvent::gotFocus() const
{
  return ( type() == fc::FocusIn_Event );
}

//----------------------------------------------------------------------
bool FFocusEvent::lostFocus() const
{
  return ( type() == fc::FocusOut_Event );
}

//----------------------------------------------------------------------
fc::FocusTypes FFocusEvent::getFocusType() const
{ return focus_type; }

//----------------------------------------------------------------------
void FFocusEvent::setFocusType(fc::FocusTypes ft)
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

FAccelEvent::FAccelEvent(int ev_type, void* focused)  // constructor
  : FEvent(ev_type)
  , accpt(false)
  , focus_widget(focused)
{ }

//----------------------------------------------------------------------
FAccelEvent::~FAccelEvent()  // destructor
{ }

//----------------------------------------------------------------------
void* FAccelEvent::focusedWidget() const
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

FResizeEvent::FResizeEvent(int ev_type)  // constructor
  : FEvent(ev_type)
  , accpt(false)
{ }

//----------------------------------------------------------------------
FResizeEvent::~FResizeEvent()  // destructor
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

FShowEvent::FShowEvent(int ev_type)  // constructor
  : FEvent(ev_type)
{ }

//----------------------------------------------------------------------
FShowEvent::~FShowEvent()  // destructor
{ }

//----------------------------------------------------------------------
// class FHideEvent
//----------------------------------------------------------------------

FHideEvent::FHideEvent(int ev_type)  // constructor
  : FEvent(ev_type)
{ }

//----------------------------------------------------------------------
FHideEvent::~FHideEvent()  // destructor
{ }

//----------------------------------------------------------------------
// class FCloseEvent
//----------------------------------------------------------------------

FCloseEvent::FCloseEvent(int ev_type)  // constructor
  : FEvent(ev_type)
  , accpt(false)
{ }

//----------------------------------------------------------------------
FCloseEvent::~FCloseEvent()  // destructor
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

FTimerEvent::FTimerEvent(int ev_type, int timer_id)  // constructor
  : FEvent(ev_type)
  , id(timer_id)
{ }

//----------------------------------------------------------------------
FTimerEvent::~FTimerEvent()  // destructor
{ }

//----------------------------------------------------------------------
int FTimerEvent::timerId() const
{ return id; }

}  // namespace finalcut
