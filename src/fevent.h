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

#include "fenum.h"
#include "fpoint.h"


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

#pragma pack(push)
#pragma pack(1)

class FMouseEvent : public FEvent   // mouse event
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

class FWheelEvent : public FEvent   // wheel event
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

class FFocusEvent : public FEvent        // focus event
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
