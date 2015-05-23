// fevent.cpp
// class FEvent

#include <cstdio>

#include "fevent.h"


//----------------------------------------------------------------------
// class FKeyEvent
//----------------------------------------------------------------------

FKeyEvent::FKeyEvent(int type, int key) : FEvent(type)  // constructor
{
  k = key;
  accpt = false;
}

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

FMouseEvent::FMouseEvent (int type,           // constructor
                          const FPoint& pos,
                          int button) : FEvent(type),
                                        p(pos),
                                        b(button)
{
}

//----------------------------------------------------------------------
FMouseEvent::FMouseEvent (int type,           // constructor
                          const FPoint& pos,
                          const FPoint& globalPos,
                          int button) : FEvent(type),
                                        p(pos),
                                        g(globalPos),
                                        b(button)
{
}

//----------------------------------------------------------------------
const FPoint& FMouseEvent::getPos() const
{ return p; }

//----------------------------------------------------------------------
const FPoint& FMouseEvent::getGlobalPos() const
{ return g; }

//----------------------------------------------------------------------
int FMouseEvent::getX() const
{ return p.getX(); }

//----------------------------------------------------------------------
int FMouseEvent::getY() const
{ return p.getY(); }

//----------------------------------------------------------------------
int FMouseEvent::getGlobalX() const
{ return g.getX(); }

//----------------------------------------------------------------------
int FMouseEvent::getGlobalY() const
{ return g.getY(); }

//----------------------------------------------------------------------
int FMouseEvent::getButton() const
{ return b; }


//----------------------------------------------------------------------
// class FWheelEvent
//----------------------------------------------------------------------

FWheelEvent::FWheelEvent (int type,           // constructor
                          const FPoint& pos,
                          int wheel) : FEvent(type),
                                        p(pos),
                                        w(wheel)
{
}

//----------------------------------------------------------------------
FWheelEvent::FWheelEvent (int type,           // constructor
                          const FPoint& pos,
                          const FPoint& globalPos,
                          int wheel) : FEvent(type),
                                        p(pos),
                                        g(globalPos),
                                        w(wheel)
{
}

//----------------------------------------------------------------------
const FPoint& FWheelEvent::getPos() const
{ return p; }

//----------------------------------------------------------------------
const FPoint& FWheelEvent::getGlobalPos() const
{ return g; }

//----------------------------------------------------------------------
int FWheelEvent::getX() const
{ return p.getX(); }

//----------------------------------------------------------------------
int FWheelEvent::getY() const
{ return p.getY(); }

//----------------------------------------------------------------------
int FWheelEvent::getGlobalX() const
{ return g.getX(); }

//----------------------------------------------------------------------
int FWheelEvent::getGlobalY() const
{ return g.getY(); }

//----------------------------------------------------------------------
int FWheelEvent::getWheel() const
{ return w; }


//----------------------------------------------------------------------
// class FFocusEvent
//----------------------------------------------------------------------

FFocusEvent::FFocusEvent (int type) : FEvent(type)  // constructor
{
  accpt = true;
  focus_type = FocusDefiniteWidget;
}

//----------------------------------------------------------------------
bool FFocusEvent::gotFocus() const
{
  return (type() == FocusIn_Event);
}

//----------------------------------------------------------------------
bool FFocusEvent::lostFocus() const
{
  return (type() == FocusOut_Event);
}

//----------------------------------------------------------------------
FocusTypes FFocusEvent::getFocusType() const
{ return focus_type; }

//----------------------------------------------------------------------
void FFocusEvent::setFocusType(FocusTypes ft)
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

FAccelEvent::FAccelEvent(int type, void* focused) : FEvent(type)  // constructor
{
  focus_widget = focused;
  accpt = false;
}

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

FResizeEvent::FResizeEvent(int type) : FEvent(type)  // constructor
{ accpt = false; }

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

FShowEvent::FShowEvent(int type) : FEvent(type)  // constructor
{
}

//----------------------------------------------------------------------
// class FHideEvent
//----------------------------------------------------------------------

FHideEvent::FHideEvent(int type) : FEvent(type)  // constructor
{
}

//----------------------------------------------------------------------
// class FCloseEvent
//----------------------------------------------------------------------

FCloseEvent::FCloseEvent(int type) : FEvent(type)  // constructor
{ accpt = false; }

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

FTimerEvent::FTimerEvent(int type, int timer_id) : FEvent(type)  // constructor
{ id = timer_id; }

//----------------------------------------------------------------------
int FTimerEvent::timerId() const
{ return id; }
