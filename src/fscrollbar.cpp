/***********************************************************************
* fscrollbar.cpp - Widget FScrollbar                                   *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2012-2018 Markus Gans                                      *
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

#include "final/fscrollbar.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FScrollbar
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FScrollbar::FScrollbar(FWidget* parent)
  : FWidget(parent)
  , scroll_type(FScrollbar::noScroll)
  , threshold_reached(false)
  , threshold_time(500)
  , repeat_time(10)
  , slider_click_pos(-1)
  , slider_click_stop_pos(-1)
  , current_slider_pos(-1)
  , slider_pos(0)
  , slider_length(18)  // = bar_length
  , bar_length(18)     // = length - 2
  , val(0)
  , min(0)
  , max(99)
  , steps(1)
  , pagesize(0)
  , length(20)
  , bar_orientation(fc::vertical)
  , max_color(getMaxColor())
{
  // The default scrollbar orientation is vertical
  setGeometry(1, 1, 1, length, false);
  init();
}

//----------------------------------------------------------------------
FScrollbar::FScrollbar(int o, FWidget* parent)
  : FWidget(parent)
  , scroll_type(FScrollbar::noScroll)
  , threshold_reached(false)
  , threshold_time(500)
  , repeat_time(10)
  , slider_click_pos(-1)
  , slider_click_stop_pos(-1)
  , current_slider_pos(-1)
  , slider_pos(0)
  , slider_length(18)  // = bar_length
  , bar_length(18)     // = length - 2
  , val(0)
  , min(0)
  , max(99)
  , steps(1)
  , pagesize(0)
  , length(20)
  , bar_orientation(fc::vertical)
  , max_color(getMaxColor())
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
  val = value;
  calculateSliderValues();
}

//----------------------------------------------------------------------
void FScrollbar::setSteps (double st)
{
  if ( st <= 0 )
    steps = 1;
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
    steps = double(double(document_size) / double(page_size));
  }
}

//----------------------------------------------------------------------
void FScrollbar::setOrientation (int o)
{
  std::size_t nf = 0;
  length = ( getHeight() > getWidth() ) ? getHeight() : getWidth();

  if ( o == fc::vertical && bar_orientation == fc::horizontal )
  {
    setWidth(1);
    setHeight(length);
  }
  else if ( o == fc::horizontal && bar_orientation == fc::vertical )
  {
    setWidth(length);
    setHeight(1);

    if ( isNewFont() )
      nf = 2;
  }

  slider_length = bar_length = length - nf - 2;
  bar_orientation = o;
}

//----------------------------------------------------------------------
void FScrollbar::setGeometry ( int x, int y
                             , std::size_t w, std::size_t h
                             , bool adjust )
{
  // Set the scrollbar geometry

  FWidget::setGeometry (x, y, w, h, adjust);

  std::size_t nf = 0;
  length = ( h > w ) ? h : w;

  if ( bar_orientation == fc::vertical )
  {
    setWidth(isNewFont() ? 2 : 1);
    setHeight(length);
  }
  else  // horizontal
  {
    setWidth(length);
    setHeight(1);

    if ( isNewFont() )
      nf = 2;
  }

  slider_length = bar_length = length - nf - 2;
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
  draw();
}

//----------------------------------------------------------------------
void FScrollbar::calculateSliderValues()
{
  if ( isNewFont() && bar_orientation == fc::horizontal )
    bar_length = length - 4;
  else
    bar_length = length - 2;

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

  std::size_t v = std::size_t(val);
  slider_pos = int( round ( double((bar_length - slider_length) * v)
                          / double(max - min) ) );

  if ( slider_pos < 0 )
    slider_pos = 0;
  else if ( slider_pos > int(bar_length - slider_length) )
    slider_pos = int(bar_length - slider_length);
}

//----------------------------------------------------------------------
void FScrollbar::drawVerticalBar()
{
  int z;
  setColor (wc.scrollbar_fg, wc.scrollbar_bg);

  for (z = 1; z <= slider_pos; z++)
  {
    setPrintPos (1, 1 + z);

    if ( isNewFont() )
    {
      if ( isMonochron() || max_color < 16 )
        print (fc::MediumShade);  // ▒
      else
        print (fc::NF_border_line_left);  // ⎸
    }

    if ( isMonochron() || max_color < 16 )
      print (fc::MediumShade);  // ▒
    else
      print (' ');
  }

  setColor (wc.scrollbar_bg, wc.scrollbar_fg);

  if ( isMonochron() )
    setReverse(false);

  for (z = 1; z <= int(slider_length); z++)
  {
    setPrintPos (1, 1 + slider_pos + z);

    if ( isNewFont() )
      print (' ');

    print (' ');
  }

  if ( isMonochron() )
    setReverse(true);

  setColor (wc.scrollbar_fg, wc.scrollbar_bg);

  for (z = slider_pos + int(slider_length) + 1; z <= int(bar_length); z++)
  {
    setPrintPos (1, 1 + z);

    if ( isNewFont() )
    {
      if ( isMonochron() || max_color < 16 )
        print (fc::MediumShade);  // ▒
      else
        print (fc::NF_border_line_left);  // ⎸
    }

    if ( isMonochron() || max_color < 16 )
      print (fc::MediumShade);
    else
      print (' ');
  }

  if ( isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
void FScrollbar::drawHorizontalBar()
{
  int z;
  setColor (wc.scrollbar_fg, wc.scrollbar_bg);

  if ( isNewFont() )
    setPrintPos (3, 1);
  else
    setPrintPos (2, 1);

  for (z = 0; z < slider_pos; z++)
  {
    if ( isNewFont() && max_color > 8 )
      print (fc::NF_border_line_upper);  // ¯
    else if ( isMonochron() || max_color < 16 )
      print (fc::MediumShade);  // ▒
    else
      print (' ');
  }

  setColor (wc.scrollbar_bg, wc.scrollbar_fg);

  if ( isMonochron() )
    setReverse(false);

  for (z = 0; z < int(slider_length); z++)
    print (' ');

  if ( isMonochron() )
    setReverse(true);

  setColor (wc.scrollbar_fg, wc.scrollbar_bg);
  z = slider_pos + int(slider_length) + 1;

  for (; z <= int(bar_length); z++)
  {
    if ( isNewFont() && max_color > 8 )
      print (fc::NF_border_line_upper);  // ¯
    else if ( isMonochron() || max_color < 16 )
      print (fc::MediumShade);  // ▒
    else
      print (' ');
  }

  if ( isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
void FScrollbar::drawBar()
{
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
  int mouse_x, mouse_y;

  if ( ev->getButton() != fc::LeftButton
    && ev->getButton() != fc::MiddleButton )
    return;

  if ( min == max )
    return;

  mouse_x = ev->getX();
  mouse_y = ev->getY();

  if ( ev->getButton() == fc::MiddleButton )
  {
    jumpToClickPos (mouse_x, mouse_y);
    return;
  }

  // Process left button
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
      if ( isNewFont() )
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
  int mouse_x, mouse_y, new_scroll_type;

  if ( ev->getButton() != fc::LeftButton
    && ev->getButton() != fc::MiddleButton )
    return;

  mouse_x = ev->getX();
  mouse_y = ev->getY();

  if ( ev->getButton() == fc::MiddleButton )
  {
    jumpToClickPos (mouse_x, mouse_y);
    return;
  }

  // Process left button
  new_scroll_type = getClickedScrollType(mouse_x, mouse_y);

  if ( scroll_type == FScrollbar::scrollJump )
  {
    int new_val;

    if ( bar_orientation == fc::vertical )
    {
      int dy = mouse_y - slider_click_pos;
      slider_click_pos = mouse_y;
      new_val = int( round ( double((max - min) * (slider_pos + dy))
                           / double(bar_length - slider_length) ) );
    }
    else  // horizontal
    {
      int dx = mouse_x - slider_click_pos;
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
  int wheel = ev->getWheel();

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
    int max_slider_pos = int(bar_length - slider_length);

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
  setGeometry(1, 1, getWidth(), getHeight());
}

//----------------------------------------------------------------------
void FScrollbar::draw()
{
  if ( length < 2 )
    return;

  drawButtons();
  current_slider_pos = -1;
  drawBar();
}

//----------------------------------------------------------------------
void FScrollbar::drawButtons()
{
  setColor (wc.scrollbar_button_fg, wc.scrollbar_button_bg);

  if ( isNewFont() )
  {
    setPrintPos (1, 1);

    if ( bar_orientation == fc::vertical )
    {
      print (fc::NF_rev_up_arrow1);
      print (fc::NF_rev_up_arrow2);
      setPrintPos (1, int(length));
      print (fc::NF_rev_down_arrow1);
      print (fc::NF_rev_down_arrow2);
    }
    else  // horizontal
    {
      print (fc::NF_rev_left_arrow1);
      print (fc::NF_rev_left_arrow2);
      setPrintPos (int(length) - 1, 1);
      print (fc::NF_rev_right_arrow1);
      print (fc::NF_rev_right_arrow2);
    }
  }
  else
  {
    setPrintPos (1, 1);

    if ( isMonochron() )
      setReverse(true);

    if ( bar_orientation == fc::vertical )
    {
      print (fc::BlackUpPointingTriangle);    // ▲
      setPrintPos (1, int(length));
      print (fc::BlackDownPointingTriangle);  // ▼
    }
    else  // horizontal
    {
      print (fc::BlackLeftPointingPointer);   // ◄
      setPrintPos (int(length), 1);
      print (fc::BlackRightPointingPointer);  // ►
    }

    if ( isMonochron() )
      setReverse(false);
  }
}

//----------------------------------------------------------------------
FScrollbar::sType FScrollbar::getClickedScrollType (int x, int y)
{
  if ( bar_orientation == fc::vertical )
  {
    return  getVerticalClickedScrollType(y);
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
  if ( isNewFont() )
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
    if ( isNewFont() )
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
  int new_val;

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
    int nf = isNewFont() ? 1 : 0;

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
    if ( isNewFont() )
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

}  // namespace finalcut
