// fscrollbar.h
// class FScrollbar

#ifndef _FSCROLLBAR_H
#define _FSCROLLBAR_H

#include "fwidget.h"

//----------------------------------------------------------------------
// class FScrollbar
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FScrollbar : public FWidget
{
 private:
   int   scrollType;
   bool  thresholdReached;
   int   thresholdTime;
   int   repeatTime;
   int   SliderClickPos;
   int   SliderClickStopPos;
   int   currentSliderPos;
   int   SliderPos;
   int   SliderLength;
   int   BarLength;
   int   val;
   int   min;
   int   max;
   float steps;
   int   pageSize;
   int   length;
   int   bar_orientation;
   int   max_color;

 public:
   enum scroll_type
   {
     noScroll           = 0,
     scrollJump         = 1,
     scrollStepBackward = 2,
     scrollStepForward  = 3,
     scrollPageBackward = 4,
     scrollPageForward  = 5,
     scrollWheelUp      = 6,
     scrollWheelDown    = 7
   };

 private:
   FScrollbar (const FScrollbar&);
   FScrollbar& operator = (const FScrollbar&);

   void init();
   void draw();
   int  getClickedScrollType (int, int);
   void processMiddleButton (int, int);
   void processScroll();

 public:
   FScrollbar(FWidget* parent=0);  // constructor
   FScrollbar(int o=fc::vertical, FWidget* parent=0);  // constructor
   virtual ~FScrollbar();
   const char* getClassName() const;

   void onMouseDown (FMouseEvent*);
   void onMouseUp (FMouseEvent*);
   void onMouseMove (FMouseEvent*);
   void onWheel (FWheelEvent*);
   void onTimer (FTimerEvent*);

   void resize();
   void redraw();
   void setMinimum (int);
   void setMaximum (int);
   void setRange (int, int);
   void setValue (int);
   int  getValue() const;
   void setSteps (float);
   void setPageSize (int, int);
   void calculateSliderValues();
   void setOrientation (int);
   void setGeometry (int, int, int, int, bool adjust=true);
   void drawButtons();
   void drawBar();
   int  getScrollType() const;
};
#pragma pack(pop)


// FScrollbar inline functions
//----------------------------------------------------------------------
inline const char* FScrollbar::getClassName() const
{ return "FScrollbar"; }

//----------------------------------------------------------------------
inline int FScrollbar::getValue() const
{ return val; }

//----------------------------------------------------------------------
inline int FScrollbar::getScrollType() const
{ return scrollType; }

#endif  // _FSCROLLBAR_H
