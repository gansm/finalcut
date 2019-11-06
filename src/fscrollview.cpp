/***********************************************************************
* fscrollview.cpp - Widget FScrollView (a scrolling area with          *
*                   on-demand scroll bars)                             *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2017-2019 Markus Gans                                      *
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

#include <memory>

#include "final/fevent.h"
#include "final/fscrollview.h"
#include "final/fwindow.h"
#include "final/fwidgetcolors.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FScrollView
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FScrollView::FScrollView (FWidget* parent)
  : FWidget(parent)
{
  init(parent);
}

//----------------------------------------------------------------------
FScrollView::~FScrollView()  // destructor
{
  removeArea (viewport);
  viewport = nullptr;
  setChildPrintArea (viewport);
}


// public methods of FScrollView
//----------------------------------------------------------------------
void FScrollView::setScrollWidth (std::size_t width)
{
  if ( width < getViewportWidth() )
    width = getViewportWidth();

  if ( getScrollWidth() == width )
    return;

  if ( viewport )
  {
    FSize no_shadow(0, 0);
    scroll_geometry.setWidth (width);
    resizeArea (scroll_geometry, no_shadow, viewport);

    addPreprocessingHandler
    (
      F_PREPROC_HANDLER (this, &FScrollView::copy2area)
    );
    setChildPrintArea (viewport);
  }

  hbar->setMaximum (int(width - getViewportWidth()));
  hbar->setPageSize (int(width), int(getViewportWidth()));
  hbar->calculateSliderValues();

  if ( isShown() )
    setHorizontalScrollBarVisibility();
}

//----------------------------------------------------------------------
void FScrollView::setScrollHeight (std::size_t height)
{
  if ( height < getViewportHeight() )
    height = getViewportHeight();

  if ( getScrollHeight() == height )
    return;

  if ( viewport )
  {
    FSize no_shadow(0, 0);
    scroll_geometry.setHeight (height);
    resizeArea (scroll_geometry, no_shadow, viewport);
    addPreprocessingHandler
    (
      F_PREPROC_HANDLER (this, &FScrollView::copy2area)
    );
    setChildPrintArea (viewport);
  }

  vbar->setMaximum (int(height - getViewportHeight()));
  vbar->setPageSize (int(height), int(getViewportHeight()));
  vbar->calculateSliderValues();

  if ( isShown() )
    setVerticalScrollBarVisibility();
}

//----------------------------------------------------------------------
void FScrollView::setScrollSize (const FSize& size)
{
  std::size_t width = size.getWidth();
  std::size_t height = size.getHeight();

  if ( width < getViewportWidth() )
    width = getViewportWidth();

  if ( height < getViewportHeight() )
    height = getViewportHeight();

  if ( getScrollWidth() == width && getScrollHeight() == height )
    return;

  if ( viewport )
  {
    FSize no_shadow(0, 0);
    scroll_geometry.setSize (width, height);
    resizeArea (scroll_geometry, no_shadow, viewport);
    addPreprocessingHandler
    (
      F_PREPROC_HANDLER (this, &FScrollView::copy2area)
    );
    setChildPrintArea (viewport);
  }

  int xoffset_end = int(getScrollWidth() - getViewportWidth());
  int yoffset_end = int(getScrollHeight() - getViewportHeight());
  setTopPadding (1 - getScrollY());
  setLeftPadding (1 - getScrollX());
  setBottomPadding (1 - (yoffset_end - getScrollY()));
  setRightPadding (1 - (xoffset_end - getScrollX()) + int(nf_offset));

  hbar->setMaximum (int(width - getViewportWidth()));
  hbar->setPageSize (int(width), int(getViewportWidth()));
  hbar->calculateSliderValues();

  vbar->setMaximum (int(height - getViewportHeight()));
  vbar->setPageSize (int(height), int(getViewportHeight()));
  vbar->calculateSliderValues();

  if ( isShown() )
  {
    setHorizontalScrollBarVisibility();
    setVerticalScrollBarVisibility();
  }
}

//----------------------------------------------------------------------
void FScrollView::setX (int x, bool adjust)
{
  FWidget::setX (x, adjust);

  if ( ! adjust )
  {
    scroll_geometry.setX (getTermX() + getLeftPadding() - 1);

    if ( viewport )
    {
      viewport->offset_left = scroll_geometry.getX();
      viewport->offset_top = scroll_geometry.getY();
    }
  }
}

//----------------------------------------------------------------------
void FScrollView::setY (int y, bool adjust)
{
  FWidget::setY (y, adjust);

  if ( ! adjust )
  {
    scroll_geometry.setY (getTermY() + getTopPadding() - 1);

    if ( viewport )
    {
      viewport->offset_left = scroll_geometry.getX();
      viewport->offset_top = scroll_geometry.getY();
    }
  }
}

//----------------------------------------------------------------------
void FScrollView::setPos (const FPoint& p, bool adjust)
{
  FWidget::setPos (p, adjust);
  scroll_geometry.setPos ( getTermX() + getLeftPadding() - 1
                         , getTermY() + getTopPadding() - 1 );

  if ( ! adjust )
  {
    if ( viewport )
    {
      viewport->offset_left = scroll_geometry.getX();
      viewport->offset_top = scroll_geometry.getY();
    }
  }
}

//----------------------------------------------------------------------
void FScrollView::setWidth (std::size_t w, bool adjust)
{
  FWidget::setWidth (w, adjust);
  viewport_geometry.setWidth(w - vertical_border_spacing - nf_offset);
  calculateScrollbarPos();

  if ( getScrollWidth() < getViewportWidth() )
    setScrollWidth (getViewportWidth());
}

//----------------------------------------------------------------------
void FScrollView::setHeight (std::size_t h, bool adjust)
{
  FWidget::setHeight (h, adjust);
  viewport_geometry.setHeight(h - horizontal_border_spacing);
  calculateScrollbarPos();

  if ( getScrollHeight() < getViewportHeight() )
    setScrollHeight (getViewportHeight());
}

//----------------------------------------------------------------------
void FScrollView::setSize (const FSize& size, bool adjust)
{
  std::size_t w = size.getWidth();
  std::size_t h = size.getHeight();
  FWidget::setSize (size, adjust);
  viewport_geometry.setSize ( w - vertical_border_spacing - nf_offset
                            , h - horizontal_border_spacing );
  calculateScrollbarPos();

  if ( getScrollWidth() < getViewportWidth()
    || getScrollHeight() < getViewportHeight() )
    setScrollSize (getViewportSize());
}

//----------------------------------------------------------------------
void FScrollView::setGeometry ( const FPoint& pos, const FSize& size
                              , bool adjust )
{
  // Set the scroll view geometry

  FWidget::setGeometry (pos, size, adjust);
  std::size_t w = size.getWidth();
  std::size_t h = size.getHeight();
  scroll_geometry.setPos ( getTermX() + getLeftPadding() - 1
                         , getTermY() + getTopPadding() - 1 );
  viewport_geometry.setSize ( w - vertical_border_spacing - nf_offset
                            , h - horizontal_border_spacing );
  calculateScrollbarPos();

  if ( getScrollWidth() < getViewportWidth()
    || getScrollHeight() < getViewportHeight() )
  {
    setScrollSize (getViewportSize());
  }
  else if ( ! adjust && viewport )
  {
    viewport->offset_left = scroll_geometry.getX();
    viewport->offset_top = scroll_geometry.getY();
  }
}

//----------------------------------------------------------------------
void FScrollView::setCursorPos (const FPoint& p)
{
  FWidget::setCursorPos (FPoint ( p.getX() + getLeftPadding()
                                , p.getY() + getTopPadding() ));
}

//----------------------------------------------------------------------
void FScrollView::setPrintPos (const FPoint& p)
{
  FWidget::setPrintPos (FPoint( p.getX() + getLeftPadding()
                              , p.getY() + getTopPadding() ));
}

//----------------------------------------------------------------------
bool FScrollView::setViewportPrint (bool enable)
{
  return (use_own_print_area = ! enable);
}

//----------------------------------------------------------------------
bool FScrollView::setBorder (bool enable)
{
  return (border = enable);
}

//----------------------------------------------------------------------
void FScrollView::setHorizontalScrollBarMode (fc::scrollBarMode mode)
{
  hMode = mode;

  if ( isShown() )
    setHorizontalScrollBarVisibility();
}

//----------------------------------------------------------------------
void FScrollView::setVerticalScrollBarMode (fc::scrollBarMode mode)
{
  vMode = mode;

  if ( isShown() )
    setVerticalScrollBarVisibility();
}

//----------------------------------------------------------------------
void FScrollView::clearArea (int fillchar)
{
  if ( viewport )
    clearArea (viewport, fillchar);
}

//----------------------------------------------------------------------
void FScrollView::scrollToX (int x)
{
  scrollTo (x, viewport_geometry.getY() + 1);
}

//----------------------------------------------------------------------
void FScrollView::scrollToY (int y)
{
  scrollTo (viewport_geometry.getX() + 1, y);
}

//----------------------------------------------------------------------
void FScrollView::scrollTo (int x, int y)
{
  int& xoffset = viewport_geometry.x1_ref();
  int& yoffset = viewport_geometry.y1_ref();
  int  xoffset_before = xoffset;
  int  yoffset_before = yoffset;
  int  xoffset_end = int(getScrollWidth() - getViewportWidth());
  int  yoffset_end = int(getScrollHeight() - getViewportHeight());
  std::size_t save_width = viewport_geometry.getWidth();
  std::size_t save_height = viewport_geometry.getHeight();
  bool changeX{false};
  bool changeY{false};
  x--;
  y--;

  if ( xoffset == x && yoffset == y )
    return;

  xoffset = x;
  yoffset = y;

  if ( yoffset < 0 )
    yoffset = 0;

  if ( yoffset > yoffset_end )
    yoffset = yoffset_end;

  if ( xoffset < 0 )
    xoffset = 0;

  if ( xoffset > xoffset_end )
    xoffset = xoffset_end;

  changeX = bool( xoffset_before != xoffset );
  changeY = bool( yoffset_before != yoffset );

  if ( ! isShown() || ! viewport || ! (changeX || changeY) )
    return;

  if ( changeX )
  {
    viewport_geometry.setWidth(save_width);
    setLeftPadding (1 - xoffset);
    setRightPadding (1 - (xoffset_end - xoffset) + int(nf_offset));

    if ( update_scrollbar )
    {
      hbar->setValue (xoffset);
      hbar->drawBar();
    }
  }

  if ( changeY )
  {
    viewport_geometry.setHeight(save_height);
    setTopPadding (1 - yoffset);
    setBottomPadding (1 - (yoffset_end - yoffset));

    if ( update_scrollbar )
    {
      vbar->setValue (yoffset);
      vbar->drawBar();
    }
  }

  viewport->has_changes = true;
  copy2area();
  updateTerminal();
}

//----------------------------------------------------------------------
void FScrollView::scrollBy (int dx, int dy)
{
  scrollTo (1 + getScrollX() + dx, 1 + getScrollY() + dy);
}

//----------------------------------------------------------------------
void FScrollView::draw()
{
  unsetViewportPrint();

  if ( isMonochron() )
    setReverse(true);

  if ( auto p = getParentWidget() )
    setColor (p->getForegroundColor(), p->getBackgroundColor());
  else
    setColor();

  if ( border )
    drawBorder();

  if ( isMonochron() )
    setReverse(false);

  setViewportPrint();
  copy2area();

  if ( ! hbar->isShown() )
    setHorizontalScrollBarVisibility();

  if ( ! vbar->isShown() )
    setVerticalScrollBarVisibility();

  vbar->redraw();
  hbar->redraw();
}

//----------------------------------------------------------------------
void FScrollView::drawBorder()
{
  FRect box(FPoint(1, 1), getSize());
  finalcut::drawListBorder (this, box);
}

//----------------------------------------------------------------------
void FScrollView::onKeyPress (FKeyEvent* ev)
{
  int idx = int(ev->key());

  if ( key_map.find(idx) != key_map.end() )
  {
    key_map[idx]();
    ev->accept();
  }
}

//----------------------------------------------------------------------
void FScrollView::onWheel (FWheelEvent* ev)
{
  int distance{4};

  switch ( ev->getWheel() )
  {
    case fc::WheelUp:
      scrollBy (0, -distance);
      break;

    case fc::WheelDown:
      scrollBy (0, distance);
      break;

    default:
      break;
  }
}

//----------------------------------------------------------------------
void FScrollView::onFocusIn (FFocusEvent* in_ev)
{
  // Sets the focus to a child widget if it exists

  if ( hasChildren() )
  {
    auto prev_element = getFocusWidget();

    if ( in_ev->getFocusType() == fc::FocusNextWidget )
      focusFirstChild();
    else if ( in_ev->getFocusType() == fc::FocusPreviousWidget )
      focusLastChild();

    if ( prev_element )
      prev_element->redraw();

    if ( getFocusWidget() )
      getFocusWidget()->redraw();

    FFocusEvent cfi (fc::ChildFocusIn_Event);
    onChildFocusIn(&cfi);
  }
}

//----------------------------------------------------------------------
void FScrollView::onChildFocusIn (FFocusEvent*)
{
  // Scrolls the viewport so that the focused widget is visible

  FRect widget_geometry;
  FRect vp_geometry;
  auto focus = FWidget::getFocusWidget();

  if ( ! focus )
    return;

  widget_geometry = focus->getGeometryWithShadow();
  vp_geometry = viewport_geometry;
  vp_geometry.move(1, 1);

  if ( ! vp_geometry.contains(widget_geometry) )
  {
    int x{}
      , y{}
      , vx = vp_geometry.getX()
      , vy = vp_geometry.getY()
      , wx = widget_geometry.getX()
      , wy = widget_geometry.getY();

    if ( wx > vx )
      x = widget_geometry.getX2() - int(vp_geometry.getWidth()) + 1;
    else
      x = wx;

    if ( wy > vy )
      y = widget_geometry.getY2() - int(vp_geometry.getHeight()) + 1;
    else
      y = wy;

    scrollTo (x, y);
  }
}

//----------------------------------------------------------------------
void FScrollView::onChildFocusOut (FFocusEvent* out_ev)
{
  // Change the focus away from FScrollView to another widget

  auto focus = FWidget::getFocusWidget();

  if ( out_ev->getFocusType() == fc::FocusNextWidget )
  {
    auto last_widget = getLastFocusableWidget(getChildren());

    if ( focus == last_widget )
    {
      out_ev->accept();
      focusNextChild();
    }
  }
  else if ( out_ev->getFocusType() == fc::FocusPreviousWidget )
  {
    auto first_widget = getFirstFocusableWidget(getChildren());

    if ( focus == first_widget )
    {
      out_ev->accept();
      focusPrevChild();
    }
  }
}


// protected methods of FScrollView
//----------------------------------------------------------------------
FVTerm::FTermArea* FScrollView::getPrintArea()
{
  // returns print area or viewport

  if ( use_own_print_area || ! viewport )
  {
    setChildPrintArea (nullptr);
    auto area = FWidget::getPrintArea();
    setChildPrintArea (viewport);
    return area;
  }
  else
    return viewport;
}

//----------------------------------------------------------------------
void FScrollView::adjustSize()
{
  FWidget::adjustSize();
  std::size_t width = getWidth();
  std::size_t height = getHeight();
  int xoffset = viewport_geometry.getX();
  int yoffset = viewport_geometry.getY();

  scroll_geometry.setPos ( getTermX() + getLeftPadding() - 1
                         , getTermY() + getTopPadding() - 1 );

  if ( viewport )
  {
    viewport->offset_left = scroll_geometry.getX();
    viewport->offset_top = scroll_geometry.getY();
  }

  hbar->setMaximum (int(getScrollWidth() - getViewportWidth()));
  hbar->setPageSize (int(getScrollWidth()), int(getViewportWidth()));
  hbar->setY (int(height));
  hbar->setWidth (width - 2, false);
  hbar->setValue (xoffset);
  hbar->resize();

  vbar->setMaximum (int(getScrollHeight() - getViewportHeight()));
  vbar->setPageSize (int(getScrollHeight()), int(getViewportHeight()));
  vbar->setX (int(width));
  vbar->setHeight (height - 2, false);
  vbar->setValue (yoffset);
  vbar->resize();

  if ( isShown() )
  {
    setHorizontalScrollBarVisibility();
    setVerticalScrollBarVisibility();
  }
}

//----------------------------------------------------------------------
void FScrollView::copy2area()
{
  // copy viewport to area

  if ( ! hasPrintArea() )
    FWidget::getPrintArea();

  if ( ! (hasPrintArea() && viewport) )
    return;

  if ( ! viewport->has_changes )
    return;

  auto printarea = getCurrentPrintArea();
  int ax = getTermX() - printarea->offset_left
    , ay = getTermY() - printarea->offset_top
    , dx = viewport_geometry.getX()
    , dy = viewport_geometry.getY()
    , y_end = int(getViewportHeight())
    , x_end = int(getViewportWidth());

  // viewport width does not fit into the printarea
  if ( printarea->width <= ax + x_end )
    x_end = printarea->width - ax;

  // viewport height does not fit into the printarea
  if ( printarea->height <= ay + y_end )
    y_end = printarea->height - ay;

  for (int y{0}; y < y_end; y++)  // line loop
  {
    FChar* vc{};  // viewport character
    FChar* ac{};  // area character
    int v_line_len = viewport->width;
    int a_line_len = printarea->width + printarea->right_shadow;
    vc = &viewport->data[(dy + y) * v_line_len + dx];
    ac = &printarea->data[(ay + y) * a_line_len + ax];
    std::memcpy (ac, vc, sizeof(FChar) * unsigned(x_end));

    if ( int(printarea->changes[ay + y].xmin) > ax )
      printarea->changes[ay + y].xmin = uInt(ax);

    if ( int(printarea->changes[ay + y].xmax) < ax + x_end - 1 )
      printarea->changes[ay + y].xmax = uInt(ax + x_end - 1);
  }

  setViewportCursor();
  viewport->has_changes = false;
  printarea->has_changes = true;
}


// private methods of FScrollView
//----------------------------------------------------------------------
inline FPoint FScrollView::getViewportCursorPos()
{
  auto window = FWindow::getWindowWidget(this);

  if ( window )
  {
    int widget_offsetX = getTermX() - window->getTermX()
      , widget_offsetY = getTermY() - window->getTermY()
      , x = widget_offsetX + viewport->input_cursor_x
          - viewport_geometry.getX()
      , y = widget_offsetY + viewport->input_cursor_y
          - viewport_geometry.getY();
    return FPoint (x, y);
  }
  else
    return FPoint (-1, -1);
}

//----------------------------------------------------------------------
void FScrollView::init (FWidget* parent)
{
  assert ( parent != 0 );
  assert ( ! parent->isInstanceOf("FScrollView") );

  initScrollbar (vbar, fc::vertical, &FScrollView::cb_VBarChange);
  initScrollbar (hbar, fc::horizontal, &FScrollView::cb_HBarChange);
  mapKeyFunctions();
  const auto& wc = getFWidgetColors();
  setForegroundColor (wc.dialog_fg);
  setBackgroundColor (wc.dialog_bg);
  setGeometry (FPoint(1, 1), FSize(4, 4));
  setMinimumSize (FSize(4, 4));
  int xoffset_end = int(getScrollWidth() - getViewportWidth());
  int yoffset_end = int(getScrollHeight() - getViewportHeight());
  nf_offset = isNewFont() ? 1 : 0;
  setTopPadding (1 - getScrollY());
  setLeftPadding (1 - getScrollX());
  setBottomPadding (1 - (yoffset_end - getScrollY()));
  setRightPadding (1 - (xoffset_end - getScrollX()) + nf_offset);
  FSize no_shadow(0, 0);
  std::size_t w = getViewportWidth();
  std::size_t h = getViewportHeight();

  if ( w < 1 )
    w = 1;

  if ( h < 1 )
    h = 1;

  scroll_geometry.setRect (0, 0, w, h);
  createArea (scroll_geometry, no_shadow, viewport);
  addPreprocessingHandler
  (
    F_PREPROC_HANDLER (this, &FScrollView::copy2area)
  );

  if ( viewport )
    setChildPrintArea (viewport);
}

//----------------------------------------------------------------------
inline void FScrollView::mapKeyFunctions()
{
  key_map[fc::Fkey_up]    = [&] { scrollBy (0, -1); };
  key_map[fc::Fkey_down]  = [&] { scrollBy (0, 1); };
  key_map[fc::Fkey_left]  = [&] { scrollBy (-1, 0); };
  key_map[fc::Fkey_right] = [&] { scrollBy (1, 0); };
  key_map[fc::Fkey_ppage] = [&] { scrollBy (0, int(-getViewportHeight())); };
  key_map[fc::Fkey_npage] = [&] { scrollBy (0, int(getViewportHeight())); };
  key_map[fc::Fkey_home]  = [&] { scrollToY (1); };
  key_map[fc::Fkey_end]   = \
      [&] ()
      {
        int yoffset_end = int(getScrollHeight() - getViewportHeight());
        scrollToY (1 + yoffset_end);
      };
}

//----------------------------------------------------------------------
void FScrollView::calculateScrollbarPos()
{
  std::size_t width  = getWidth();
  std::size_t height = getHeight();

  if ( isNewFont() )
  {
    vbar->setGeometry (FPoint(int(width), 2), FSize(2, height - 2));
    hbar->setGeometry (FPoint(1, int(height)), FSize(width - 2, 1));
  }
  else
  {
    vbar->setGeometry (FPoint(int(width), 2), FSize(1, height - 2));
    hbar->setGeometry (FPoint(2, int(height)), FSize(width - 2, 1));
  }

  vbar->resize();
  hbar->resize();
}

//----------------------------------------------------------------------
void FScrollView::setHorizontalScrollBarVisibility()
{
  switch ( hMode )
  {
    case fc::Auto:
      if ( getScrollWidth() > getViewportWidth() )
        hbar->show();
      else
        hbar->hide();
      break;

    case fc::Hidden:
      hbar->hide();
      break;

    case fc::Scroll:
      hbar->show();
      break;
  }
}

//----------------------------------------------------------------------
void FScrollView::setVerticalScrollBarVisibility()
{
  switch ( vMode )
  {
    case fc::Auto:
      if ( getScrollHeight() > getViewportHeight() )
        vbar->show();
      else
        vbar->hide();
      break;

    case fc::Hidden:
      vbar->hide();
      break;

    case fc::Scroll:
      vbar->show();
      break;
  }
}

//----------------------------------------------------------------------
void FScrollView::setViewportCursor()
{
  if ( ! isChild(getFocusWidget()) )
    return;

  FPoint cursor_pos ( viewport->input_cursor_x - 1
                    , viewport->input_cursor_y - 1 );
  FPoint window_cursor_pos(getViewportCursorPos());
  auto printarea = getCurrentPrintArea();
  printarea->input_cursor_x = window_cursor_pos.getX();
  printarea->input_cursor_y = window_cursor_pos.getY();

  if ( viewport->input_cursor_visible
    && viewport_geometry.contains(cursor_pos) )
    printarea->input_cursor_visible = true;
  else
    printarea->input_cursor_visible = false;
}

//----------------------------------------------------------------------
void FScrollView::cb_VBarChange (FWidget*, FDataPtr)
{
  FScrollbar::sType scrollType = vbar->getScrollType();
  int distance{1};
  int wheel_distance{4};

  if ( scrollType >= FScrollbar::scrollStepBackward )
  {
    update_scrollbar = true;
  }
  else
  {
    update_scrollbar = false;
  }

  switch ( scrollType )
  {
    case FScrollbar::noScroll:
      break;

    case FScrollbar::scrollPageBackward:
      distance = int(getViewportHeight());
      // fall through
    case FScrollbar::scrollStepBackward:
      scrollBy (0, -distance);
      break;

    case FScrollbar::scrollPageForward:
      distance = int(getViewportHeight());
      // fall through
    case FScrollbar::scrollStepForward:
      scrollBy (0, distance);
      break;

    case FScrollbar::scrollJump:
      scrollToY (1 + int(vbar->getValue()));
      break;

    case FScrollbar::scrollWheelUp:
      scrollBy (0, -wheel_distance);
      break;

    case FScrollbar::scrollWheelDown:
      scrollBy (0, wheel_distance);
      break;
  }

  update_scrollbar = true;
}

//----------------------------------------------------------------------
void FScrollView::cb_HBarChange (FWidget*, FDataPtr)
{
  FScrollbar::sType scrollType = hbar->getScrollType();
  int distance{1};
  int wheel_distance{4};

  if ( scrollType >= FScrollbar::scrollStepBackward )
  {
    update_scrollbar = true;
  }
  else
  {
    update_scrollbar = false;
  }

  switch ( scrollType )
  {
    case FScrollbar::noScroll:
      break;

    case FScrollbar::scrollPageBackward:
      distance = int(getViewportWidth());
      // fall through
    case FScrollbar::scrollStepBackward:
      scrollBy (-distance, 0);
      break;

    case FScrollbar::scrollPageForward:
      distance = int(getViewportWidth());
      // fall through
    case FScrollbar::scrollStepForward:
      scrollBy (distance, 0);
      break;

    case FScrollbar::scrollJump:
      scrollToX (1 + int(hbar->getValue()));
      break;

    case FScrollbar::scrollWheelUp:
      scrollBy (-wheel_distance, 0);
      break;

    case FScrollbar::scrollWheelDown:
      scrollBy (wheel_distance, 0);
      break;
  }

  update_scrollbar = true;
}

}  // namespace finalcut
