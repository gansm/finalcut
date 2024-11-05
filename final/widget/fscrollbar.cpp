/***********************************************************************
* fscrollbar.cpp - Widget FScrollbar                                   *
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

#include <algorithm>

#include "final/fevent.h"
#include "final/fwidgetcolors.h"
#include "final/util/fsize.h"
#include "final/widget/fscrollbar.h"
#include "final/widget/fstatusbar.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FScrollbar
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FScrollbar::FScrollbar(FWidget* parent)
  : FWidget{parent}
{
  // The default scrollbar orientation is vertical
  FScrollbar::setGeometry(FPoint{1, 1}, FSize{1, length}, false);
  init();
}

//----------------------------------------------------------------------
FScrollbar::FScrollbar(Orientation o, FWidget* parent)
  : FWidget{parent}
{
  setOrientation (o);
  init();
}

//----------------------------------------------------------------------
FScrollbar::~FScrollbar()  // destructor
{
  delOwnTimers();
}


// public methods of FScrollbar
//----------------------------------------------------------------------
void FScrollbar::setMinimum (int minimum)
{
  min = minimum;
  calculateSliderValues();
}

//----------------------------------------------------------------------
void FScrollbar::setMaximum (int maximum)
{
  max = maximum;
  calculateSliderValues();
}

//----------------------------------------------------------------------
void FScrollbar::setRange (int minimum, int maximum)
{
  min = minimum;
  max = maximum;
  calculateSliderValues();
}

//----------------------------------------------------------------------
void FScrollbar::setValue (int value)
{
  val = std::max(min, std::min(value, max));
  calculateSliderValues();
}

//----------------------------------------------------------------------
void FScrollbar::setSteps (double st)
{
  steps = (st <= 0.0) ? 1.0 : st;

  if ( pagesize == 0 )
    pagesize = int(double(max)/steps);
}

//----------------------------------------------------------------------
void FScrollbar::setPageSize (int document_size, int page_size)
{
  if ( page_size == 0 )
  {
    pagesize = document_size;
    steps = 1.0;
  }
  else
  {
    pagesize = page_size;

    if ( document_size <= 0 || page_size <= 0 )
      steps = 1.0;
    else
      steps = double(double(document_size) / double(page_size));
  }
}

//----------------------------------------------------------------------
void FScrollbar::setOrientation (Orientation o)
{
  length = ( o == Orientation::Vertical ) ? getHeight() : getWidth();

  if ( o == Orientation::Vertical && bar_orientation == Orientation::Horizontal )
  {
    FWidget::setWidth(1);
    FWidget::setHeight(length);
  }
  else if ( o == Orientation::Horizontal && bar_orientation == Orientation::Vertical )
  {
    FWidget::setWidth(length);
    FWidget::setHeight(1);
  }

  calculateSliderValues();
  bar_orientation = o;
}

//----------------------------------------------------------------------
void FScrollbar::setSize (const FSize& size, bool adjust)
{
  // Set the scrollbar size

  FWidget::setSize (size, adjust);
  changeOnResize();
}

//----------------------------------------------------------------------
void FScrollbar::setGeometry ( const FPoint& pos, const FSize& size
                             , bool adjust )
{
  // Set the scrollbar geometry

  FWidget::setGeometry (pos, size, adjust);
  changeOnResize();
}

//----------------------------------------------------------------------
void FScrollbar::resize()
{
  FWidget::resize();
  setOrientation (bar_orientation);
  setValue (val);
  calculateSliderValues();
}

//----------------------------------------------------------------------
void FScrollbar::redraw()
{
  if ( isShown() )
    draw();
}

//----------------------------------------------------------------------
void FScrollbar::calculateSliderValues()
{
  if ( FVTerm::getFOutput()->isNewFont()
    && bar_orientation == Orientation::Horizontal )
    bar_length = ( length > 2 ) ? length - 4 : 1;
  else
    bar_length = ( length > 2 ) ? length - 2 : 1;

  slider_length = std::size_t(double(bar_length) / steps);
  slider_length = std::max(std::size_t(1), std::min(slider_length, bar_length));

  if ( val == min )
  {
    slider_pos = 0;
    return;
  }

  if ( val == max )
  {
    slider_pos = int(bar_length - slider_length);
    return;
  }

  const std::size_t v = ( min < 0 ) ? std::size_t(val - min) : std::size_t(val);

  if ( slider_length >= bar_length )
    slider_pos = 0;
  else
    slider_pos = int( round ( double((bar_length - slider_length) * v)
                            / double(max - min) ) );

  slider_pos = std::min(slider_pos, int(bar_length - slider_length));
}



//----------------------------------------------------------------------
void FScrollbar::drawBar()
{
  if ( ! isShown() )
    return;

  if ( slider_pos == current_slider_pos || length < 3 )
    return;

  if ( bar_orientation == Orientation::Vertical )
    drawVerticalBar();
  else  // horizontal
    drawHorizontalBar();

  current_slider_pos = slider_pos;
}

//----------------------------------------------------------------------
void FScrollbar::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != MouseButton::Left
    && ev->getButton() != MouseButton::Middle )
    return;

  handleWidgetFocus();

  if ( min == max )
    return;

  const int mouse_x = ev->getX();
  const int mouse_y = ev->getY();

  if ( ev->getButton() == MouseButton::Middle )
  {
    jumpToClickPos (mouse_x, mouse_y);
    return;
  }

  // Process left mouse button
  scroll_type = getClickedScrollType (mouse_x, mouse_y);
  handleSliderClick (mouse_x, mouse_y);
  handleTrackClick (mouse_x, mouse_y);
}

//----------------------------------------------------------------------
void FScrollbar::onMouseUp (FMouseEvent* ev)
{
  if ( ev->getButton() != MouseButton::Left
    && ev->getButton() != MouseButton::Middle )
    return;

  slider_click_pos = -1;

  if ( scroll_type != ScrollType::None )
  {
    delOwnTimers();
    scroll_type = ScrollType::None;
  }
}

//----------------------------------------------------------------------
void FScrollbar::onMouseMove (FMouseEvent* ev)
{
  if ( ev->getButton() != MouseButton::Left
    && ev->getButton() != MouseButton::Middle )
    return;

  const int mouse_x = ev->getX();
  const int mouse_y = ev->getY();

  if ( ev->getButton() == MouseButton::Middle )
  {
    jumpToClickPos (mouse_x, mouse_y);
    return;
  }

  // Process left mouse button
  const auto scroll_type_changed = \
      bool( scroll_type != getClickedScrollType(mouse_x, mouse_y) );

  if ( scroll_type == ScrollType::Jump )
    handleJumpScroll(mouse_x, mouse_y);

  if ( isMouseOutsideScrollbar(mouse_x, mouse_y) )
    delOwnTimers();
  else if ( scroll_type != ScrollType::Jump )
    addTimer(repeat_time);

  if ( scroll_type_changed )
    delOwnTimers();
}

//----------------------------------------------------------------------
void FScrollbar::onWheel (FWheelEvent* ev)
{
  const MouseWheel wheel = ev->getWheel();

  if ( scroll_type != ScrollType::None )
  {
    delOwnTimers();
    scroll_type = ScrollType::None;
  }

  if ( wheel == MouseWheel::Up )
    scroll_type = ScrollType::WheelUp;
  else if ( wheel == MouseWheel::Down )
    scroll_type = ScrollType::WheelDown;
  else if ( wheel == MouseWheel::Left )
    scroll_type = ScrollType::WheelLeft;
  else if ( wheel == MouseWheel::Right )
    scroll_type = ScrollType::WheelRight;

  processScroll();
}

//----------------------------------------------------------------------
void FScrollbar::onTimer (FTimerEvent*)
{
  if ( scroll_type == ScrollType::None )
    return;

  if ( ! threshold_reached )
  {
    threshold_reached = true;
    delOwnTimers();
    addTimer(repeat_time);
  }

  if ( shouldStopTimer() )  // Timer stop condition
  {
    stopTimer();
    return;
  }

  processScroll();
}


// private methods of FScrollbar
//----------------------------------------------------------------------
void FScrollbar::init()
{
  unsetFocusable();
  ignorePadding();
  FScrollbar::setGeometry(FPoint{1, 1}, FSize{getWidth(), getHeight()});
}

//----------------------------------------------------------------------
void FScrollbar::draw()
{
  if ( length < 2 )
    return;

  if ( isShown() )
    drawButtons();

  current_slider_pos = -1;
  max_color = FVTerm::getFOutput()->getMaxColor();
  drawBar();
}

//----------------------------------------------------------------------
void FScrollbar::drawVerticalBar()
{
  const auto& wc_scrollbar = getColorTheme()->scrollbar;
  setColor (wc_scrollbar.fg, wc_scrollbar.bg);

  for (auto z{1}; z <= slider_pos; z++)
  {
    print() << FPoint{1, 1 + z};
    drawVerticalBackgroundLine();
  }

  setColor (wc_scrollbar.bg, wc_scrollbar.fg);

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);

  for (auto z{1}; z <= int(slider_length); z++)  // Draw slider
  {
    print() << FPoint{1, 1 + slider_pos + z};

    if ( FVTerm::getFOutput()->isNewFont() )
      print (' ');

    print (' ');
  }

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(true);

  setColor (wc_scrollbar.fg, wc_scrollbar.bg);

  for (auto z = slider_pos + int(slider_length) + 1; z <= int(bar_length); z++)
  {
    print() << FPoint{1, 1 + z};
    drawVerticalBackgroundLine();
  }

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
inline void FScrollbar::drawVerticalBackgroundLine()
{
  if ( FVTerm::getFOutput()->isNewFont() )
  {
    if ( FVTerm::getFOutput()->isMonochron() || max_color < 16 )
      print (UniChar::MediumShade);  // ▒
    else
      print (UniChar::NF_border_line_left);  // ⎸
  }

  if ( FVTerm::getFOutput()->isMonochron() || max_color < 16 )
    print (UniChar::MediumShade);  // ▒
  else if ( FVTerm::getFOutput()->isNewFont() )
    print (UniChar::NF_rev_border_line_right);  //⎹
  else
    print (' ');
}

//----------------------------------------------------------------------
void FScrollbar::drawHorizontalBar()
{
  const auto& wc_scrollbar = getColorTheme()->scrollbar;
  setColor (wc_scrollbar.fg, wc_scrollbar.bg);

  if ( FVTerm::getFOutput()->isNewFont() )
    print() << FPoint{3, 1};
  else
    print() << FPoint{2, 1};

  for (auto z{0}; z < slider_pos; z++)
    drawHorizontalBackgroundColumn();

  setColor (wc_scrollbar.bg, wc_scrollbar.fg);

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);

  for (auto z{0}; z < int(slider_length); z++)  // Draw slider
    print (' ');

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(true);

  setColor (wc_scrollbar.fg, wc_scrollbar.bg);
  int z = slider_pos + int(slider_length) + 1;

  for (; z <= int(bar_length); z++)
    drawHorizontalBackgroundColumn();

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
inline void FScrollbar::drawHorizontalBackgroundColumn()
{
  if ( FVTerm::getFOutput()->isNewFont() && max_color > 8 )
    print (UniChar::NF_border_line_up_and_down);  // ニ
  else if ( FVTerm::getFOutput()->isMonochron() || max_color < 16 )
    print (UniChar::MediumShade);  // ▒
  else
    print (' ');
}

//----------------------------------------------------------------------
void FScrollbar::drawButtons()
{
  const auto& wc_scrollbar = getColorTheme()->scrollbar;
  setColor (wc_scrollbar.button_fg, wc_scrollbar.button_bg);

  if ( FVTerm::getFOutput()->isNewFont() )
  {
    print() << FPoint{1, 1};

    if ( bar_orientation == Orientation::Vertical )
    {
      print() << NF_button_arrow_up
              << FPoint{1, int(length)}
              << NF_button_arrow_down;
    }
    else  // horizontal
    {
      print() << NF_button_arrow_left
              << FPoint{int(length) - 1, 1}
              << NF_button_arrow_right;
    }
  }
  else
  {
    print() << FPoint{1, 1};

    if ( FVTerm::getFOutput()->isMonochron() )
      setReverse(true);

    if ( bar_orientation == Orientation::Vertical )
    {
      print() << UniChar::BlackUpPointingTriangle     // ▲
              << FPoint{1, int(length)}
              << UniChar::BlackDownPointingTriangle;  // ▼
    }
    else  // horizontal
    {
      print() << UniChar::BlackLeftPointingPointer    // ◄
              << FPoint{int(length), 1}
              << UniChar::BlackRightPointingPointer;  // ►
    }

    if ( FVTerm::getFOutput()->isMonochron() )
      setReverse(false);
  }
}

//----------------------------------------------------------------------
auto FScrollbar::getClickedScrollType (int x, int y) const -> ScrollType
{
  if ( bar_orientation == Orientation::Vertical )
    return getVerticalClickedScrollType(y);

  return getHorizontalClickedScrollType(x);  // horizontal
}

//----------------------------------------------------------------------
auto FScrollbar::getVerticalClickedScrollType (int y) const -> ScrollType
{
  if ( y == 1 )
    return ScrollType::StepBackward;  // decrement button

  if ( y > 1 && y <= slider_pos + 1 )
    return ScrollType::PageBackward;  // before slider

  if ( y > slider_pos + int(slider_length) + 1 && y < int(getHeight()) )
    return ScrollType::PageForward;  // after slider

  if ( y == int(getHeight()) )
    return ScrollType::StepForward;  // increment button

  return ScrollType::None;
}

//----------------------------------------------------------------------
auto FScrollbar::getHorizontalClickedScrollType (int x) const -> ScrollType
{
  return FVTerm::getFOutput()->isNewFont()
       ? getNewFontHorizontalScrollType (x)
       : getHorizontalScrollType (x);
}

//----------------------------------------------------------------------
auto FScrollbar::getNewFontHorizontalScrollType (int x) const -> ScrollType
{
  if ( x == 1 || x == 2 )
    return ScrollType::StepBackward;  // decrement button

  if ( x > 2 && x <= slider_pos + 2 )
    return ScrollType::PageBackward;  // before slider

  if ( x > slider_pos + int(slider_length) + 2 && x < int(getWidth()) - 1 )
    return ScrollType::PageForward;  // after slider

  if ( x == int(getWidth()) - 1 || x == int(getWidth()) )
    return ScrollType::StepForward;  // increment button

  return ScrollType::None;
}

//----------------------------------------------------------------------
auto FScrollbar::getHorizontalScrollType (int x) const -> ScrollType
{
  if ( x == 1 )
    return ScrollType::StepBackward;  // decrement button

  if ( x > 1 && x <= slider_pos + 1 )
    return ScrollType::PageBackward;  // before slider

  if ( x > slider_pos + int(slider_length) + 1 && x < int(getWidth()) )
    return ScrollType::PageForward;  // after slider

  if ( x == int(getWidth()) )
    return ScrollType::StepForward;  // increment button

  return ScrollType::None;
}

//----------------------------------------------------------------------
auto FScrollbar::getSliderClickPos (int mouse_x, int mouse_y) const -> int
{
  // Get the clicked position on the slider

  if ( bar_orientation == Orientation::Vertical )
  {
    if ( mouse_y > slider_pos + 1
      && mouse_y <= slider_pos + int(slider_length) + 1 )
      return mouse_y;  // on slider
  }
  else  // horizontal bar orientation
  {
    if ( FVTerm::getFOutput()->isNewFont() )
    {
      if ( mouse_x > slider_pos + 2
        && mouse_x <= slider_pos + int(slider_length) + 2 )
        return mouse_x;  // on slider
    }
    else
    {
      if ( mouse_x > slider_pos + 1
        && mouse_x <= slider_pos + int(slider_length) + 1 )
        return mouse_x;  // on slider
    }
  }

  return -1;
}

//----------------------------------------------------------------------
inline auto FScrollbar::isMouseOutsideScrollbar ( int mouse_x
                                                , int mouse_y ) const -> bool
{
  return mouse_x < 1
      || mouse_x > int(getWidth())
      || mouse_y < 1
      || mouse_y > int(getHeight());
}

//----------------------------------------------------------------------
inline auto FScrollbar::shouldStopTimer() const -> bool
{
  return ( scroll_type == ScrollType::PageBackward
        && slider_pos == slider_click_stop_pos )
      || ( scroll_type == ScrollType::PageForward
        && slider_pos == slider_click_stop_pos );
}

//----------------------------------------------------------------------
void FScrollbar::jumpToClickPos (int x, int y)
{
  int new_val{};

  if ( bar_orientation == Orientation::Vertical )
  {
    if ( y > 1 && y < int(getHeight()) )
    {
      new_val = int( round ( double(max - min) * (y - 2.0 - (double(slider_length) / 2))
                           / double(bar_length - slider_length) ) );
    }
    else
      return;
  }
  else  // horizontal
  {
    const int nf = FVTerm::getFOutput()->isNewFont() ? 1 : 0;

    if ( x > 1 + nf && x < int(getWidth()) - nf )
    {
      new_val = int( round ( double(max - min) * (x - 2.0 - nf - (double(slider_length) / 2))
                           / double(bar_length - slider_length) ) );
    }
    else
      return;
  }

  if ( new_val != val )
  {
    setValue(new_val);
    drawBar();
    scroll_type = ScrollType::Jump;
    processScroll();
  }
}

//----------------------------------------------------------------------
void FScrollbar::jumpToClickPos (int pos)
{
  if ( bar_orientation == Orientation::Vertical )
    jumpToClickPos (0, pos + 2);
  else
  {
    if ( FVTerm::getFOutput()->isNewFont() )
      jumpToClickPos (pos + 3, 0);
    else
      jumpToClickPos (pos + 2, 0);
  }
}

//----------------------------------------------------------------------
inline void FScrollbar::handleJumpScroll (int mouse_x, int mouse_y)
{
  int new_val{};

  if ( bar_orientation == Orientation::Vertical )
  {
    const int dy = mouse_y - slider_click_pos;
    slider_click_pos = mouse_y;
    new_val = int( round ( double((max - min) * (slider_pos + dy))
                         / double(bar_length - slider_length) ) );
  }
  else  // horizontal
  {
    const int dx = mouse_x - slider_click_pos;
    slider_click_pos = mouse_x;
    new_val = int( round ( double((max - min) * (slider_pos + dx))
                         / double(bar_length - slider_length) ) );
  }

  if ( new_val == val )
    return;

  setValue(new_val);
  drawBar();
  processScroll();
}

//----------------------------------------------------------------------
void FScrollbar::stopTimer()
{
  const auto max_slider_pos = int(bar_length - slider_length);

  if ( scroll_type == ScrollType::PageBackward
    && slider_pos == 0 )
  {
    jumpToClickPos(0);  // Scroll to the start
    processScroll();
  }
  else if ( scroll_type == ScrollType::PageForward
         && slider_pos == max_slider_pos )
  {
    jumpToClickPos (max_slider_pos);  // Scroll to the end
    processScroll();
  }

  delOwnTimers();
}

//----------------------------------------------------------------------
void FScrollbar::avoidScrollOvershoot()
{
  // Avoid overshoot
  if ( ( scroll_type == ScrollType::PageBackward
      && slider_pos < slider_click_stop_pos )
    || ( scroll_type == ScrollType::PageForward
      && slider_pos > slider_click_stop_pos ) )
  {
    jumpToClickPos (slider_click_stop_pos);
    delOwnTimers();
  }
}

//----------------------------------------------------------------------
void FScrollbar::processScroll()
{
  startDrawing();  // Avoid printing an overshoot on the emit callback
  emitCallback("change-value");
  avoidScrollOvershoot();
  finishDrawing();
}

//----------------------------------------------------------------------
void FScrollbar::changeOnResize()
{
  const FSize& size = getSize();
  const std::size_t w = size.getWidth();
  const std::size_t h = size.getHeight();
  length = ( bar_orientation == Orientation::Vertical ) ? h : w;

  if ( bar_orientation == Orientation::Vertical )
  {
    FWidget::setWidth(FVTerm::getFOutput()->isNewFont() ? 2 : 1);
    FWidget::setHeight(length);
  }
  else  // horizontal
  {
    FWidget::setWidth(length);
    FWidget::setHeight(1);
  }

  calculateSliderValues();
}

//----------------------------------------------------------------------
inline void FScrollbar::handleWidgetFocus() const
{
  const auto& parent_widget = getParentWidget();

  if ( parent_widget && ! parent_widget->isInstanceOf("FScrollView") )
    setWidgetFocus(parent_widget);
}

//----------------------------------------------------------------------
inline void FScrollbar::handleSliderClick (int mouse_x, int mouse_y)
{
  if ( scroll_type != ScrollType::None )
    return;

  slider_click_pos = getSliderClickPos (mouse_x, mouse_y);

  if ( slider_click_pos > 0 )
    scroll_type = ScrollType::Jump;
}

//----------------------------------------------------------------------
inline void FScrollbar::handleTrackClick (int mouse_x, int mouse_y)
{
  if ( scroll_type == ScrollType::PageBackward
    || scroll_type == ScrollType::PageForward )
  {
    if ( bar_orientation == Orientation::Vertical )
      slider_click_stop_pos = mouse_y - 2;
    else
    {
      if ( FVTerm::getFOutput()->isNewFont() )
        slider_click_stop_pos = mouse_x - 3;
      else
        slider_click_stop_pos = mouse_x - 2;
    }
  }
  else
    slider_click_stop_pos = -1;

  if ( scroll_type >= ScrollType::StepBackward
    && scroll_type <= ScrollType::PageForward )
  {
    processScroll();
    threshold_reached = false;
    addTimer(threshold_time);
  }
}

}  // namespace finalcut
