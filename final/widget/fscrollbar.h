/***********************************************************************
* fscrollbar.h - Widget FScrollbar                                     *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2012-2024 Markus Gans                                      *
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

/*  Inheritance diagram
 *  ═══════════════════
 *
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

#include <functional>
#include <memory>

#include "final/fapplication.h"
#include "final/fwidget.h"
#include "final/util/flog.h"

namespace finalcut
{

// class forward declaration
class FScrollbar;

// Global using-declaration
using FScrollbarPtr = std::shared_ptr<FScrollbar>;

//----------------------------------------------------------------------
// class FScrollbar
//----------------------------------------------------------------------

class FScrollbar : public FWidget
{
  public:
    // Using-declaration
    using FWidget::setGeometry;

    // Enumeration
    enum class ScrollType
    {
      None         = 0,
      Jump         = 1,
      StepBackward = 2,
      StepForward  = 3,
      PageBackward = 4,
      PageForward  = 5,
      WheelUp      = 6,
      WheelDown    = 7,
      WheelLeft    = 8,
      WheelRight   = 9
    };

    // Constructors
    explicit FScrollbar (FWidget* = nullptr);
    explicit FScrollbar (Orientation = Orientation::Vertical, FWidget* = nullptr);

    // Destructor
    ~FScrollbar() override;

    // Accessors
    auto getClassName() const -> FString override;
    auto getValue() const noexcept -> int;
    auto getScrollType() const -> ScrollType;

    // Mutators
    void setMinimum (int);
    void setMaximum (int);
    void setRange (int, int);
    void setValue (int);
    void setSteps (double);
    void setPageSize (int, int);
    void setOrientation (Orientation);
    void setSize (const FSize&, bool = true) override;
    void setGeometry (const FPoint&, const FSize&, bool = true) override;

    // Methods
    void resize() override;
    void redraw() override;
    void calculateSliderValues();
    void drawBar();

    // Event handlers
    void onMouseDown (FMouseEvent*) override;
    void onMouseUp (FMouseEvent*) override;
    void onMouseMove (FMouseEvent*) override;
    void onWheel (FWheelEvent*) override;
    void onTimer (FTimerEvent*) override;

  private:
    // Methods
    void init();
    void draw() override;
    void drawVerticalBar();
    void drawVerticalBackgroundLine();
    void drawHorizontalBar();
    void drawHorizontalBackgroundColumn();
    void drawButtons();
    auto getClickedScrollType (int, int) const -> ScrollType;
    auto getVerticalClickedScrollType (int) const -> ScrollType;
    auto getHorizontalClickedScrollType (int) const -> ScrollType;
    auto getNewFontHorizontalScrollType (int) const -> ScrollType;
    auto getHorizontalScrollType (int) const -> ScrollType;
    auto getSliderClickPos (int, int) const -> int;
    auto isMouseOutsideScrollbar (int, int) const -> bool;
    auto shouldStopTimer() const -> bool;
    void jumpToClickPos (int, int);
    void jumpToClickPos (int);
    void handleJumpScroll (int, int);
    void stopTimer();
    void avoidScrollOvershoot();
    void processScroll();
    void changeOnResize();
    void handleWidgetFocus() const;
    void handleSliderClick (int, int);
    void handleTrackClick (int, int);

    // Data members
    ScrollType   scroll_type{ScrollType::None};
    bool         threshold_reached{false};
    int          threshold_time{500};
    int          repeat_time{80};
    int          slider_click_pos{-1};
    int          slider_click_stop_pos{-1};
    int          current_slider_pos{-1};
    int          slider_pos{0};
    std::size_t  slider_length{18};  // = bar_length
    std::size_t  bar_length{18};     // = length - 2
    int          val{0};
    int          min{0};
    int          max{99};
    int          pagesize{0};
    double       steps{1};
    std::size_t  length{20};
    Orientation  bar_orientation{Orientation::Vertical};
    int          max_color{FVTerm::getFOutput()->getMaxColor()};
};

// non-member functions
//----------------------------------------------------------------------
template <typename Instance
        , typename Callback>
void initScrollbar ( FScrollbarPtr& bar
                   , Orientation o
                   , Instance cb_instance
                   , const Callback& cb_handler )
{
  bar = std::make_shared<FScrollbar>(o, cb_instance);
  bar->setMinimum(0);
  bar->setValue(0);
  bar->hide();
  bar->addCallback
  (
    "change-value",
    std::bind(cb_handler, cb_instance, bar.get())
  );
}


// FScrollbar inline functions
//----------------------------------------------------------------------
inline auto FScrollbar::getClassName() const -> FString
{ return "FScrollbar"; }

//----------------------------------------------------------------------
inline auto FScrollbar::getValue() const noexcept -> int
{ return val; }

//----------------------------------------------------------------------
inline auto FScrollbar::getScrollType() const -> ScrollType
{ return scroll_type; }

}  // namespace finalcut

#endif  // FSCROLLBAR_H
