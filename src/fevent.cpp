// File: fevent.cpp
// Provides: class FEvent

#include <cstdio>

#include "final/fevent.h"


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

FKeyEvent::FKeyEvent (int ev_type, int key_num)  // constructor
  : FEvent(ev_type)
  , k(key_num)
  , accpt(false)
{ }

//----------------------------------------------------------------------
FKeyEvent::~FKeyEvent()  // destructor
{ }

//----------------------------------------------------------------------
int FKeyEvent::key() const
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
