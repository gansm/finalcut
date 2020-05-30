/***********************************************************************
* fscrollbar.cpp - Widget FScrollbar                                   *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2012-2020 Markus Gans                                      *
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

#include <algorithm>

#include "final/fevent.h"
#include "final/fscrollbar.h"
#include "final/fsize.h"
#include "final/fwidgetcolors.h"

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
  setGeometry(FPoint{1, 1}, FSize{1, length}, false);
  init();
}

//----------------------------------------------------------------------
FScrollbar::FScrollbar(fc::orientation o, FWidget* parent)
  : FWidget{parent}
{
  setOrientation (o);
  init();
}

//----------------------------------------------------------------------
FScrollbar::~FScrollbar()  // destructor
{
  delOwnTimer();
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
  if ( value < min )
    val = min;
  else if ( value > max )
    val = max;
  else
    val = value;

  calculateSliderValues();
}

//----------------------------------------------------------------------
void FScrollbar::setSteps (double st)
{
  if ( st <= 0.0 )
    steps = 1.0;
  else
    steps = st;

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
void FScrollbar::setOrientation (fc::orientation o)
{
  length = ( o == fc::vertical ) ? getHeight() : getWidth();

  if ( o == fc::vertical && bar_orientation == fc::horizontal )
  {
    setWidth(1);
    setHeight(length);
  }
  else if ( o == fc::horizontal && bar_orientation == fc::vertical )
  {
    setWidth(length);
    setHeight(1);
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
  if ( FTerm::isNewFont() && bar_orientation == fc::horizontal )
    bar_length = ( length > 2 ) ? length - 4 : 1;
  else
    bar_length = ( length > 2 ) ? length - 2 : 1;

  slider_length = std::size_t(double(bar_length) / steps);

  if ( slider_length < 1 )
    slider_length = 1;
  else if ( slider_length > bar_length )
    slider_length = bar_length;

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

  if ( slider_pos > int(bar_length - slider_length) )
    slider_pos = int(bar_length - slider_length);
}



//----------------------------------------------------------------------
void FScrollbar::drawBar()
{
  if ( ! isShown() )
    return;

  if ( slider_pos == current_slider_pos || length < 3 )
    return;

  if ( bar_orientation == fc::vertical )
    drawVerticalBar();
  else  // horizontal
    drawHorizontalBar();

  current_slider_pos = slider_pos;
}

//----------------------------------------------------------------------
void FScrollbar::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton
    && ev->getButton() != fc::MiddleButton )
    return;

  if ( min == max )
    return;

  const int mouse_x = ev->getX();
  const int mouse_y = ev->getY();

  if ( ev->getButton() == fc::MiddleButton )
  {
    jumpToClickPos (mouse_x, mouse_y);
    return;
  }

  // Process left mouse button
  scroll_type = getClickedScrollType(mouse_x, mouse_y);

  if ( scroll_type == FScrollbar::noScroll )
  {
    slider_click_pos = getSliderClickPos (mouse_x, mouse_y);

    if ( slider_click_pos > 0 )
      scroll_type = FScrollbar::scrollJump;
  }

  if ( scroll_type == FScrollbar::scrollPageBackward
    || scroll_type == FScrollbar::scrollPageForward )
  {
    if ( bar_orientation == fc::vertical )
      slider_click_stop_pos = mouse_y - 2;
    else
    {
      if ( FTerm::isNewFont() )
        slider_click_stop_pos = mouse_x - 3;
      else
        slider_click_stop_pos = mouse_x - 2;
    }
  }
  else
    slider_click_stop_pos = -1;

  if ( scroll_type >= FScrollbar::scrollStepBackward
    && scroll_type <= FScrollbar::scrollPageForward )
  {
    processScroll();
    threshold_reached = false;
    addTimer(threshold_time);
  }
}

//----------------------------------------------------------------------
void FScrollbar::onMouseUp (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton
    && ev->getButton() != fc::MiddleButton )
    return;

  slider_click_pos = -1;

  if ( scroll_type != FScrollbar::noScroll )
  {
    delOwnTimer();
    scroll_type = FScrollbar::noScroll;
  }
}

//----------------------------------------------------------------------
void FScrollbar::onMouseMove (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton
    && ev->getButton() != fc::MiddleButton )
    return;

  const int mouse_x = ev->getX();
  const int mouse_y = ev->getY();

  if ( ev->getButton() == fc::MiddleButton )
  {
    jumpToClickPos (mouse_x, mouse_y);
    return;
  }

  // Process left mouse button
  const int new_scroll_type = getClickedScrollType(mouse_x, mouse_y);

  if ( scroll_type == FScrollbar::scrollJump )
  {
    int new_val{};

    if ( bar_orientation == fc::vertical )
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

    if ( new_val != val )
    {
      setValue(new_val);
      drawBar();
      updateTerminal();
      processScroll();
    }
  }

  if ( mouse_x < 1 || mouse_x > int(getWidth())
    || mouse_y < 1 || mouse_y > int(getHeight()) )
  {
    delOwnTimer();
  }
  else if ( scroll_type != FScrollbar::scrollJump )
  {
    addTimer(repeat_time);
  }

  if ( scroll_type != new_scroll_type )
  {
    delOwnTimer();
  }
}

//----------------------------------------------------------------------
void FScrollbar::onWheel (FWheelEvent* ev)
{
  const int wheel = ev->getWheel();

  if ( scroll_type != FScrollbar::noScroll )
  {
    delOwnTimer();
    scroll_type = FScrollbar::noScroll;
  }

  if ( wheel == fc::WheelUp )
    scroll_type = FScrollbar::scrollWheelUp;
  else if ( wheel == fc::WheelDown )
    scroll_type = FScrollbar::scrollWheelDown;

  processScroll();
}

//----------------------------------------------------------------------
void FScrollbar::onTimer (FTimerEvent*)
{
  if ( scroll_type == FScrollbar::noScroll )
    return;

  if ( ! threshold_reached )
  {
    threshold_reached = true;
    delOwnTimer();
    addTimer(repeat_time);
  }

  // Timer stop condition
  if ( ( scroll_type == FScrollbar::scrollPageBackward
      && slider_pos == slider_click_stop_pos )
    || ( scroll_type == FScrollbar::scrollPageForward
      && slider_pos == slider_click_stop_pos ) )
  {
    const int max_slider_pos = int(bar_length - slider_length);

    if ( scroll_type == FScrollbar::scrollPageBackward
      && slider_pos == 0 )
    {
      jumpToClickPos(0);  // Scroll to the start
      processScroll();
    }
    else if ( scroll_type == FScrollbar::scrollPageForward
           && slider_pos == max_slider_pos )
    {
      jumpToClickPos (max_slider_pos);  // Scroll to the end
      processScroll();
    }

    delOwnTimer();
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
  setGeometry(FPoint{1, 1}, FSize{getWidth(), getHeight()});
}

//----------------------------------------------------------------------
void FScrollbar::draw()
{
  if ( length < 2 )
    return;

  if ( isShown() )
    drawButtons();

  current_slider_pos = -1;
  drawBar();
}

//----------------------------------------------------------------------
void FScrollbar::drawVerticalBar()
{
  const auto& wc = getColorTheme();
  setColor (wc->scrollbar_fg, wc->scrollbar_bg);

  for (int z{1}; z <= slider_pos; z++)
  {
    print() << FPoint{1, 1 + z};
    drawVerticalBackgroundLine();
  }

  setColor (wc->scrollbar_bg, wc->scrollbar_fg);

  if ( FTerm::isMonochron() )
    setReverse(false);

  for (int z{1}; z <= int(slider_length); z++)  // Draw slider
  {
    print() << FPoint{1, 1 + slider_pos + z};

    if ( FTerm::isNewFont() )
      print (' ');

    print (' ');
  }

  if ( FTerm::isMonochron() )
    setReverse(true);

  setColor (wc->scrollbar_fg, wc->scrollbar_bg);

  for (int z = slider_pos + int(slider_length) + 1; z <= int(bar_length); z++)
  {
    print() << FPoint{1, 1 + z};
    drawVerticalBackgroundLine();
  }

  if ( FTerm::isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
inline void FScrollbar::drawVerticalBackgroundLine()
{
  if ( FTerm::isNewFont() )
  {
    if ( FTerm::isMonochron() || max_color < 16 )
      print (fc::MediumShade);  // ▒
    else
      print (fc::NF_border_line_left);  // ⎸
  }

  if ( FTerm::isMonochron() || max_color < 16 )
    print (fc::MediumShade);  // ▒
  else if ( FTerm::isNewFont() )
    print (fc::NF_rev_border_line_right);  //⎹
      else
    print (' ');
}

//----------------------------------------------------------------------
void FScrollbar::drawHorizontalBar()
{
  const auto& wc = getColorTheme();
  setColor (wc->scrollbar_fg, wc->scrollbar_bg);

  if ( FTerm::isNewFont() )
    print() << FPoint{3, 1};
  else
    print() << FPoint{2, 1};

  for (int z{0}; z < slider_pos; z++)
    drawHorizontalBackgroundColumn();

  setColor (wc->scrollbar_bg, wc->scrollbar_fg);

  if ( FTerm::isMonochron() )
    setReverse(false);

  for (int z{0}; z < int(slider_length); z++)  // Draw slider
    print (' ');

  if ( FTerm::isMonochron() )
    setReverse(true);

  setColor (wc->scrollbar_fg, wc->scrollbar_bg);
  int z = slider_pos + int(slider_length) + 1;

  for (; z <= int(bar_length); z++)
    drawHorizontalBackgroundColumn();

  if ( FTerm::isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
inline void FScrollbar::drawHorizontalBackgroundColumn()
{
  if ( FTerm::isNewFont() && max_color > 8 )
    print (fc::NF_border_line_up_and_down);  // ニ
  else if ( FTerm::isMonochron() || max_color < 16 )
    print (fc::MediumShade);  // ▒
  else
    print (' ');
}

//----------------------------------------------------------------------
void FScrollbar::drawButtons()
{
  const auto& wc = getColorTheme();
  setColor (wc->scrollbar_button_fg, wc->scrollbar_button_bg);

  if ( FTerm::isNewFont() )
  {
    print() << FPoint{1, 1};

    if ( bar_orientation == fc::vertical )
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

    if ( FTerm::isMonochron() )
      setReverse(true);

    if ( bar_orientation == fc::vertical )
    {
      print() << fc::BlackUpPointingTriangle     // ▲
              << FPoint{1, int(length)}
              << fc::BlackDownPointingTriangle;  // ▼
    }
    else  // horizontal
    {
      print() << fc::BlackLeftPointingPointer    // ◄
              << FPoint{int(length), 1}
              << fc::BlackRightPointingPointer;  // ►
    }

    if ( FTerm::isMonochron() )
      setReverse(false);
  }
}

//----------------------------------------------------------------------
FScrollbar::sType FScrollbar::getClickedScrollType (int x, int y)
{
  if ( bar_orientation == fc::vertical )
  {
    return getVerticalClickedScrollType(y);
  }
  else  // horizontal
  {
    return getHorizontalClickedScrollType(x);
  }
}

//----------------------------------------------------------------------
FScrollbar::sType FScrollbar::getVerticalClickedScrollType (int y)
{
  if ( y == 1 )
  {
    return FScrollbar::scrollStepBackward;  // decrement button
  }
  else if ( y > 1 && y <= slider_pos + 1 )
  {
    return FScrollbar::scrollPageBackward;  // before slider
  }
  else if ( y > slider_pos + int(slider_length) + 1 && y < int(getHeight()) )
  {
    return FScrollbar::scrollPageForward;  // after slider
  }
  else if ( y == int(getHeight()) )
  {
    return FScrollbar::scrollStepForward;  // increment button
  }

  return FScrollbar::noScroll;
}

//----------------------------------------------------------------------
FScrollbar::sType FScrollbar::getHorizontalClickedScrollType (int x)
{
  if ( FTerm::isNewFont() )
  {
    if ( x == 1 || x == 2 )
    {
      return FScrollbar::scrollStepBackward;  // decrement button
    }
    else if ( x > 2 && x <= slider_pos + 2 )
    {
      return FScrollbar::scrollPageBackward;  // before slider
    }
    else if ( x > slider_pos + int(slider_length) + 2 && x < int(getWidth()) - 1 )
    {
      return FScrollbar::scrollPageForward;  // after slider
    }
    else if ( x == int(getWidth()) - 1 || x == int(getWidth()) )
    {
      return FScrollbar::scrollStepForward;  // increment button
    }

    return FScrollbar::noScroll;
  }
  else
  {
    if ( x == 1 )
    {
      return FScrollbar::scrollStepBackward;  // decrement button
    }
    else if ( x > 1 && x <= slider_pos + 1 )
    {
      return FScrollbar::scrollPageBackward;  // before slider
    }
    else if ( x > slider_pos + int(slider_length) + 1 && x < int(getWidth()) )
    {
      return FScrollbar::scrollPageForward;  // after slider
    }
    else if ( x == int(getWidth()) )
    {
      return FScrollbar::scrollStepForward;  // increment button
    }

    return FScrollbar::noScroll;
  }
}

//----------------------------------------------------------------------
int FScrollbar::getSliderClickPos (int mouse_x, int mouse_y)
{
  // Get the clicked position on the slider

  if ( bar_orientation == fc::vertical )
  {
    if ( mouse_y > slider_pos + 1
      && mouse_y <= slider_pos + int(slider_length) + 1 )
      return mouse_y;  // on slider
  }
  else  // horizontal bar orientation
  {
    if ( FTerm::isNewFont() )
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
void FScrollbar::jumpToClickPos (int x, int y)
{
  int new_val{};

  if ( bar_orientation == fc::vertical )
  {
    if ( y >1 && y < int(getHeight()) )
    {
      new_val = int( round ( double(max - min) * (y - 2.0 - (slider_length/2))
                           / double(bar_length - slider_length) ) );
    }
    else
      return;
  }
  else  // horizontal
  {
    const int nf = FTerm::isNewFont() ? 1 : 0;

    if ( x > 1 + nf && x < int(getWidth()) - nf )
    {
      new_val = int( round ( double(max - min) * (x - 2.0 - nf - (slider_length/2))
                           / double(bar_length - slider_length) ) );
    }
    else
      return;
  }

  if ( new_val != val )
  {
    setValue(new_val);
    drawBar();
    updateTerminal();
    scroll_type = FScrollbar::scrollJump;
    processScroll();
  }
}

//----------------------------------------------------------------------
void FScrollbar::jumpToClickPos (int pos)
{
  if ( bar_orientation == fc::vertical )
    jumpToClickPos (0, pos + 2);
  else
  {
    if ( FTerm::isNewFont() )
      jumpToClickPos (pos + 3, 0);
    else
      jumpToClickPos (pos + 2, 0);
  }
}

//----------------------------------------------------------------------
void FScrollbar::avoidScrollOvershoot()
{
  // Avoid overshoot
  if ( ( scroll_type == FScrollbar::scrollPageBackward
      && slider_pos < slider_click_stop_pos )
    || ( scroll_type == FScrollbar::scrollPageForward
      && slider_pos > slider_click_stop_pos ) )
  {
    jumpToClickPos (slider_click_stop_pos);
    delOwnTimer();
  }
}

//----------------------------------------------------------------------
void FScrollbar::processScroll()
{
  emitCallback("change-value");
  avoidScrollOvershoot();
}

//----------------------------------------------------------------------
void FScrollbar::changeOnResize()
{
  const FSize& size = getSize();
  const std::size_t w = size.getWidth();
  const std::size_t h = size.getHeight();
  length = ( bar_orientation == fc::vertical ) ? h : w;

  if ( bar_orientation == fc::vertical )
  {
    setWidth(FTerm::isNewFont() ? 2 : 1);
    setHeight(length);
  }
  else  // horizontal
  {
    setWidth(length);
    setHeight(1);
  }

  calculateSliderValues();
}

}  // namespace finalcut
