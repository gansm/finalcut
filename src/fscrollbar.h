// File: fscrollbar.h
// Provides: class FScrollbar
//
//  Inheritance diagram
//  ═══════════════════
//
// ▕▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▏
// ▕ FObject ▏ ▕  FTerm  ▏
// ▕▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▏
//      ▲           ▲
//      │           │
//      └─────┬─────┘
//            │
//       ▕▔▔▔▔▔▔▔▔▔▏
//       ▕ FWidget ▏
//       ▕▁▁▁▁▁▁▁▁▁▏
//            ▲
//            │
//      ▕▔▔▔▔▔▔▔▔▔▔▔▔▏
//      ▕ FScrollbar ▏
//      ▕▁▁▁▁▁▁▁▁▁▁▁▁▏

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
 public:
   enum sType
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
   sType scroll_type;
   bool  threshold_reached;
   int   threshold_time;
   int   repeat_time;
   int   slider_click_pos;
   int   slider_click_stop_pos;
   int   current_slider_pos;
   int   slider_pos;
   int   slider_length;
   int   bar_length;
   int   val;
   int   min;
   int   max;
   float steps;
   int   pageSize;
   int   length;
   int   bar_orientation;
   int   max_color;

 private:
   // Disable copy constructor
   FScrollbar (const FScrollbar&);
   // Disable assignment operator (=)
   FScrollbar& operator = (const FScrollbar&);

   void  init();
   void  draw();
   sType getClickedScrollType (int, int);
   void  processMiddleButton (int, int);
   void  processScroll();

 public:
   // Constructors
   explicit FScrollbar(FWidget* = 0);
   FScrollbar (int = fc::vertical, FWidget* = 0);
   // Destructor
   virtual ~FScrollbar();

   const char* getClassName() const;

   // Event handlers
   void  onMouseDown (FMouseEvent*);
   void  onMouseUp (FMouseEvent*);
   void  onMouseMove (FMouseEvent*);
   void  onWheel (FWheelEvent*);
   void  onTimer (FTimerEvent*);

   void  resize();
   void  redraw();
   void  setMinimum (int);
   void  setMaximum (int);
   void  setRange (int, int);
   void  setValue (int);
   int   getValue() const;
   void  setSteps (float);
   void  setPageSize (int, int);
   void  calculateSliderValues();
   void  setOrientation (int);
   // make every setGeometry from FWidget available
   using FWidget::setGeometry;
   void  setGeometry (int, int, int, int, bool = true);
   void  drawButtons();
   void  drawBar();
   sType getScrollType() const;
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
inline FScrollbar::sType FScrollbar::getScrollType() const
{ return scroll_type; }

#endif  // _FSCROLLBAR_H
