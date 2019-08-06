/***********************************************************************
* fscrollbar.h - Widget FScrollbar                                     *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2012-2019 Markus Gans                                      *
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

/*  Inheritance diagram
 *  ═══════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▏
 * ▕  FTerm  ▏
 * ▕▁▁▁▁▁▁▁▁▁▏
 *      ▲
 *      │
 * ▕▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▏
 * ▕ FVTerm  ▏ ▕ FObject ▏
 * ▕▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▏
 *      ▲           ▲
 *      │           │
 *      └─────┬─────┘
 *            │
 *       ▕▔▔▔▔▔▔▔▔▔▏
 *       ▕ FWidget ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏
 *            ▲
 *            │
 *      ▕▔▔▔▔▔▔▔▔▔▔▔▔▏
 *      ▕ FScrollbar ▏
 *      ▕▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FSCROLLBAR_H
#define FSCROLLBAR_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fwidget.h"

namespace finalcut
{

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
    explicit FScrollbar (FWidget* = nullptr);
    explicit FScrollbar (fc::orientation = fc::vertical, FWidget* = nullptr);

    // Disable copy constructor
    FScrollbar (const FScrollbar&) = delete;

    // Destructor
    virtual ~FScrollbar();

    // Disable assignment operator (=)
    FScrollbar& operator = (const FScrollbar&) = delete;

    // Accessors
    const char*         getClassName() const override;
    int                 getValue() const;
    sType               getScrollType() const;

    // Mutators
    void                setMinimum (int);
    void                setMaximum (int);
    void                setRange (int, int);
    void                setValue (int);
    void                setSteps (double);
    void                setPageSize (int, int);
    void                setOrientation (fc::orientation);
    void                setGeometry ( const FPoint&, const FSize&
                                    , bool = true ) override;

    // Methods
    void                resize() override;
    void                redraw() override;
    void                calculateSliderValues();
    void                drawVerticalBar();
    void                drawHorizontalBar();
    void                drawBar();

    // Event handlers
    void                onMouseDown (FMouseEvent*) override;
    void                onMouseUp (FMouseEvent*) override;
    void                onMouseMove (FMouseEvent*) override;
    void                onWheel (FWheelEvent*) override;
    void                onTimer (FTimerEvent*) override;

  private:
    // Methods
    void                init();
    void                draw() override;
    void                drawButtons();
    sType               getClickedScrollType (int, int);
    sType               getVerticalClickedScrollType (int);
    sType               getHorizontalClickedScrollType (int);
    int                 getSliderClickPos (int, int);
    void                jumpToClickPos (int, int);
    void                jumpToClickPos (int);
    void                avoidScrollOvershoot();
    void                processScroll();

    // Data Members
    sType           scroll_type{FScrollbar::noScroll};
    bool            threshold_reached{false};
    int             threshold_time{500};
    int             repeat_time{10};
    int             slider_click_pos{-1};
    int             slider_click_stop_pos{-1};
    int             current_slider_pos{-1};
    int             slider_pos{0};
    std::size_t     slider_length{18};  // = bar_length
    std::size_t     bar_length{18};     // = length - 2
    int             val{0};
    int             min{0};
    int             max{99};
    double          steps{1};
    int             pagesize{0};
    std::size_t     length{20};
    fc::orientation bar_orientation{fc::vertical};
    int             max_color{getMaxColor()};
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

}  // namespace finalcut

#endif  // FSCROLLBAR_H
