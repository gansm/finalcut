// File: fevent.h
// Provides: class FEvent
//
//  Inheritance diagram
//  ═══════════════════
//
// ▕▔▔▔▔▔▔▔▔▔▏
// ▕ FEvent  ▏
// ▕▁▁▁▁▁▁▁▁▁▏
//       ▲
//       │
//       │    ▕▔▔▔▔▔▔▔▔▔▔▔▏
//       ├─────▏FKeyEvent ▏
//       │    ▕▁▁▁▁▁▁▁▁▁▁▁▏
//       │
//       │    ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
//       ├─────▏FMouseEvent ▏
//       │    ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
//       │
//       │    ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
//       ├─────▏FWheelEvent ▏
//       │    ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
//       │
//       │    ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
//       ├─────▏FFocusEvent ▏
//       │    ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
//       │
//       │    ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
//       ├─────▏FAccelEvent ▏
//       │    ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
//       │
//       │    ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
//       ├─────▏FResizeEvent ▏
//       │    ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
//       │
//       │    ▕▔▔▔▔▔▔▔▔▔▔▔▔▏
//       ├─────▏FShowEvent ▏
//       │    ▕▁▁▁▁▁▁▁▁▁▁▁▁▏
//       │
//       │    ▕▔▔▔▔▔▔▔▔▔▔▔▔▏
//       ├─────▏FHideEvent ▏
//       │    ▕▁▁▁▁▁▁▁▁▁▁▁▁▏
//       │
//       │    ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
//       ├─────▏FCloseEvent ▏
//       │    ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
//       │
//       │    ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
//       └─────▏FTimerEvent ▏
//            ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏


#ifndef _FEVENT_H
#define _FEVENT_H

#include "fpoint.h"


#define None_Event               0  // invalid event
#define KeyPress_Event           1  // key pressed
#define KeyUp_Event              2  // key released
#define KeyDown_Event            3  // key pressed
#define MouseDown_Event          4  // mouse button pressed
#define MouseUp_Event            5  // mouse button released
#define MouseDoubleClick_Event   6  // mouse button double click
#define MouseWheel_Event         7  // mouse wheel rolled
#define MouseMove_Event          8  // mouse move
#define FocusIn_Event            9  // focus in
#define FocusOut_Event          10  // focus out
#define WindowActive_Event      11  // activate window
#define WindowInactive_Event    12  // deactivate window
#define WindowRaised_Event      13  // raise window
#define WindowLowered_Event     14  // lower window
#define Accelerator_Event       15  // keyboard accelerator
#define Resize_Event            16  // terminal resize
#define Show_Event              17  // widget is shown
#define Hide_Event              18  // widget is hidden
#define Close_Event             19  // widget close
#define Timer_Event             20  // timer event occur


//----------------------------------------------------------------------
// class FEvent
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FEvent // event base class
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

class FKeyEvent : public FEvent        // keyboard event
{
 public:
   FKeyEvent (int, int);
  ~FKeyEvent();

   int  key() const;
   bool isAccepted() const;
   void accept();
   void ignore();

 protected:
   int  k;
   bool accpt;
};

#pragma pack(pop)


//----------------------------------------------------------------------
// class FMouseEvent
//----------------------------------------------------------------------

enum ButtonState                    // mouse/keyboard state values
{
  NoButton        = 0x00,
  LeftButton      = 0x01,
  RightButton     = 0x02,
  MiddleButton    = 0x04,
  MouseButtonMask = 0x07,
  ShiftButton     = 0x08,
  ControlButton   = 0x10,
  MetaButton      = 0x20,
  KeyButtonMask   = 0x38
};

#pragma pack(push)
#pragma pack(1)

class FMouseEvent : public FEvent   // mouse event
{
 public:
   FMouseEvent (int, const FPoint&, int);
   FMouseEvent (int, const FPoint&, const FPoint&, int);
  ~FMouseEvent();

   const FPoint& getPos() const;
   const FPoint& getGlobalPos() const;
   int getX() const;
   int getY() const;
   int getGlobalX() const;
   int getGlobalY() const;
   int getButton() const;

 protected:
   FPoint p;
   FPoint g;
   int b;
};

#pragma pack(pop)

	
//----------------------------------------------------------------------
// class FWheelEvent
//----------------------------------------------------------------------

enum WheelState                    // wheel state values
{
  NoWheel   = 0x00,
  WheelUp   = 0x01,
  WheelDown = 0x02,
  WheelMask = 0x03
};

#pragma pack(push)
#pragma pack(1)

class FWheelEvent : public FEvent   // wheel event
{
 public:
   FWheelEvent (int, const FPoint&, int);
   FWheelEvent (int, const FPoint&, const FPoint&, int);
  ~FWheelEvent();

   const FPoint& getPos() const;
   const FPoint& getGlobalPos() const;
   int getX() const;
   int getY() const;
   int getGlobalX() const;
   int getGlobalY() const;
   int getWheel() const;

 protected:
   FPoint p;
   FPoint g;
   int w;
};

#pragma pack(pop)


//----------------------------------------------------------------------
// class FFocusEvent
//----------------------------------------------------------------------

enum FocusTypes
{
  FocusNextWidget     = 0x00,
  FocusPreviousWidget = 0x01,
  FocusDefiniteWidget = 0x03
};

#pragma pack(push)
#pragma pack(1)

class FFocusEvent : public FEvent        // focus event
{
 public:
   explicit FFocusEvent (int);
  ~FFocusEvent();

   bool gotFocus()  const;
   bool lostFocus() const;
   FocusTypes getFocusType() const;
   void setFocusType(FocusTypes);
   bool isAccepted() const;
   void accept();
   void ignore();
 protected:
   bool accpt;
   FocusTypes focus_type;
};

#pragma pack(pop)


//----------------------------------------------------------------------
// class FAccelEvent
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FAccelEvent : public FEvent    // focus event
{
 private:
   FAccelEvent (const FAccelEvent&);             // Disabled copy constructor
   FAccelEvent& operator = (const FAccelEvent&); // and operator '='

 public:
   FAccelEvent (int, void*);
  ~FAccelEvent();

   void* focusedWidget() const;
   bool  isAccepted() const;
   void  accept();
   void  ignore();

 protected:
   bool  accpt;
   void* focus_widget;
};

#pragma pack(pop)


//----------------------------------------------------------------------
// class FResizeEvent
//----------------------------------------------------------------------

class FResizeEvent : public FEvent    // resize event
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

class FShowEvent : public FEvent    // show event
{
 public:
   explicit FShowEvent (int);
  ~FShowEvent();
};



//----------------------------------------------------------------------
// class FHideEvent
//----------------------------------------------------------------------

class FHideEvent : public FEvent    // hide event
{
 public:
   explicit FHideEvent (int);
  ~FHideEvent();
};



//----------------------------------------------------------------------
// class FCloseEvent
//----------------------------------------------------------------------

class FCloseEvent : public FEvent   // close event
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

class FTimerEvent : public FEvent   // timer event
{
 public:
   FTimerEvent(int, int);
  ~FTimerEvent();

   int timerId() const;

 protected:
   int id;
};

#pragma pack(pop)

#endif  // _FEVENT_H
