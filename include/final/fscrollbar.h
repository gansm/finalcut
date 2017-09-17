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
//       ▕▔▔▔▔▔▔▔▔▏
//       ▕ FVTerm ▏
//       ▕▁▁▁▁▁▁▁▁▏
//            ▲
//            │
//       ▕▔▔▔▔▔▔▔▔▔▏
//       ▕ FWidget ▏
//       ▕▁▁▁▁▁▁▁▁▁▏
//            ▲
//            │
//      ▕▔▔▔▔▔▔▔▔▔▔▔▔▏
//      ▕ FScrollbar ▏
//      ▕▁▁▁▁▁▁▁▁▁▁▁▁▏

#ifndef FSCROLLBAR_H
#define FSCROLLBAR_H

#include "final/fwidget.h"


//----------------------------------------------------------------------
// class FScrollbar
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FScrollbar : public FWidget
{
  public:
    // Using-declarations
    using FWidget::setGeometry;

    // Enumeration
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

    // Constructors
    explicit FScrollbar(FWidget* = 0);
    FScrollbar (int = fc::vertical, FWidget* = 0);

    // Destructor
    virtual ~FScrollbar();

    // Accessors
    const char* getClassName() const;
    int   getValue() const;
    sType getScrollType() const;

    // Mutators
    void  setMinimum (int);
    void  setMaximum (int);
    void  setRange (int, int);
    void  setValue (int);
    void  setSteps (double);
    void  setPageSize (int, int);
    void  setOrientation (int);
    void  setGeometry (int, int, int, int, bool = true);

    // Methods
    void  resize();
    void  redraw();
    void  calculateSliderValues();
    void  drawButtons();
    void  drawBar();

    // Event handlers
    void  onMouseDown (FMouseEvent*);
    void  onMouseUp (FMouseEvent*);
    void  onMouseMove (FMouseEvent*);
    void  onWheel (FWheelEvent*);
    void  onTimer (FTimerEvent*);

  private:
    // Disable copy constructor
    FScrollbar (const FScrollbar&);

    // Disable assignment operator (=)
    FScrollbar& operator = (const FScrollbar&);

    // Methods
    void  init();
    void  draw();
    sType getClickedScrollType (int, int);
    void  processMiddleButton (int, int);
    void  processScroll();

    // Data Members
    sType  scroll_type;
    bool   threshold_reached;
    int    threshold_time;
    int    repeat_time;
    int    slider_click_pos;
    int    slider_click_stop_pos;
    int    current_slider_pos;
    int    slider_pos;
    int    slider_length;
    int    bar_length;
    int    val;
    int    min;
    int    max;
    double steps;
    int    pageSize;
    int    length;
    int    bar_orientation;
    int    max_color;
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

#endif  // FSCROLLBAR_H
