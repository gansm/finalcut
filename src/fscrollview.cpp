/***********************************************************************
* fscrollview.cpp - Widget FScrollView (a scrolling area with          *
*                   on-demand scroll bars)                             *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2017-2018 Markus Gans                                      *
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

#include "final/fscrollview.h"
#include "final/fwindow.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FScrollView
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FScrollView::FScrollView (FWidget* parent)
  : FWidget(parent)
  , scroll_geometry(1, 1, 1, 1)
  , viewport_geometry()
  , viewport(0)
  , vbar(0)
  , hbar(0)
  , nf_offset(0)
  , border(true)
  , use_own_print_area(false)
  , update_scrollbar(true)
  , vMode(fc::Auto)
  , hMode(fc::Auto)
{
  init(parent);
}

//----------------------------------------------------------------------
FScrollView::~FScrollView()  // destructor
{
  delete vbar;
  delete hbar;
  removeArea (viewport);
  child_print_area = viewport = 0;
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
    FPoint no_shadow(0, 0);
    scroll_geometry.setWidth (width);
    resizeArea (scroll_geometry, no_shadow, viewport);

    addPreprocessingHandler
    (
      F_PREPROC_HANDLER (this, &FScrollView::copy2area)
    );
    child_print_area = viewport;
  }

  hbar->setMaximum (int(width - getViewportWidth()));
  hbar->setPageSize (int(width), int(getViewportWidth()));
  hbar->calculateSliderValues();
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
    FPoint no_shadow(0, 0);
    scroll_geometry.setHeight (height);
    resizeArea (scroll_geometry, no_shadow, viewport);
    addPreprocessingHandler
    (
      F_PREPROC_HANDLER (this, &FScrollView::copy2area)
    );
    child_print_area = viewport;
  }

  vbar->setMaximum (int(height - getViewportHeight()));
  vbar->setPageSize (int(height), int(getViewportHeight()));
  vbar->calculateSliderValues();
  setVerticalScrollBarVisibility();
}

//----------------------------------------------------------------------
void FScrollView::setScrollSize (std::size_t width, std::size_t height)
{
  int xoffset_end
    , yoffset_end;

  if ( width < getViewportWidth() )
    width = getViewportWidth();

  if ( height < getViewportHeight() )
    height = getViewportHeight();

  if ( getScrollWidth() == width && getScrollHeight() == height )
    return;

  if ( viewport )
  {
    FPoint no_shadow(0, 0);
    scroll_geometry.setSize (width, height);
    resizeArea (scroll_geometry, no_shadow, viewport);
    addPreprocessingHandler
    (
      F_PREPROC_HANDLER (this, &FScrollView::copy2area)
    );
    child_print_area = viewport;
  }

  xoffset_end = int(getScrollWidth() - getViewportWidth());
  yoffset_end = int(getScrollHeight() - getViewportHeight());
  setTopPadding (1 - getScrollY());
  setLeftPadding (1 - getScrollX());
  setBottomPadding (1 - (yoffset_end - getScrollY()));
  setRightPadding (1 - (xoffset_end - getScrollX()) + int(nf_offset));

  hbar->setMaximum (int(width - getViewportWidth()));
  hbar->setPageSize (int(width), int(getViewportWidth()));
  hbar->calculateSliderValues();
  setHorizontalScrollBarVisibility();

  vbar->setMaximum (int(height - getViewportHeight()));
  vbar->setPageSize (int(height), int(getViewportHeight()));
  vbar->calculateSliderValues();
  setVerticalScrollBarVisibility();
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
void FScrollView::setPos (int x, int y, bool adjust)
{
  FWidget::setPos (x, y, adjust);
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
void FScrollView::setSize (std::size_t w, std::size_t h, bool adjust)
{
  FWidget::setSize (w, h, adjust);
  viewport_geometry.setSize ( w - vertical_border_spacing - nf_offset
                            , h - horizontal_border_spacing );
  calculateScrollbarPos();

  if ( getScrollWidth() < getViewportWidth()
    || getScrollHeight() < getViewportHeight() )
    setScrollSize (getViewportWidth(), getViewportHeight());
}

//----------------------------------------------------------------------
void FScrollView::setGeometry ( int x, int y
                              , std::size_t w, std::size_t h
                              , bool adjust )
{
  // Set the scroll view geometry

  FWidget::setGeometry (x, y, w, h, adjust);
  scroll_geometry.setPos ( getTermX() + getLeftPadding() - 1
                         , getTermY() + getTopPadding() - 1 );
  viewport_geometry.setSize ( w - vertical_border_spacing - nf_offset
                            , h - horizontal_border_spacing );
  calculateScrollbarPos();

  if ( getScrollWidth() < getViewportWidth()
    || getScrollHeight() < getViewportHeight() )
  {
    setScrollSize (getViewportWidth(), getViewportHeight());
  }
  else if ( ! adjust && viewport )
  {
    viewport->offset_left = scroll_geometry.getX();
    viewport->offset_top = scroll_geometry.getY();
  }
}

//----------------------------------------------------------------------
void FScrollView::setCursorPos (int x, int y)
{
  FWidget::setCursorPos (x + getLeftPadding(), y + getTopPadding());
}

//----------------------------------------------------------------------
void FScrollView::setPrintPos (int x, int y)
{
  FWidget::setPrintPos (x + getLeftPadding(), y + getTopPadding());
}

//----------------------------------------------------------------------
bool FScrollView::setViewportPrint (bool on)
{
  return (use_own_print_area = ! on);
}

//----------------------------------------------------------------------
bool FScrollView::setBorder (bool on)
{
  return (border = on);
}

//----------------------------------------------------------------------
void FScrollView::setHorizontalScrollBarMode (fc::scrollBarMode mode)
{
  hMode = mode;
  setHorizontalScrollBarVisibility();
}

//----------------------------------------------------------------------
void FScrollView::setVerticalScrollBarMode (fc::scrollBarMode mode)
{
  vMode = mode;
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
  bool changeX = false;
  bool changeY = false;
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

  changeX = bool(xoffset_before != xoffset);
  changeY = bool(yoffset_before != yoffset);

  if ( ! isVisible() || ! viewport || ! (changeX || changeY) )
    return;

  if ( changeX )
  {
    viewport_geometry.setWidth(save_width);
    setLeftPadding (1 - xoffset);
    setRightPadding (1 - (xoffset_end - xoffset) + int(nf_offset));

    if ( update_scrollbar )
    {
      hbar->setValue (xoffset);
      drawHBar();
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
      drawVBar();
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

  if ( FWidget* p = getParentWidget() )
    setColor (p->getForegroundColor(), p->getBackgroundColor());
  else
    setColor();

  if ( border )
  {
    if ( isNewFont() )
      drawBorder (1, 1, int(getWidth()) - 1, int(getHeight()));
    else
      drawBorder();
  }

  if ( isMonochron() )
    setReverse(false);

  setViewportPrint();
  copy2area();
  redrawVBar();
  redrawHBar();
}

//----------------------------------------------------------------------
void FScrollView::onKeyPress (FKeyEvent* ev)
{
  int yoffset_end = int(getScrollHeight() - getViewportHeight());

  switch ( ev->key() )
  {
    case fc::Fkey_up:
      scrollBy (0, -1);
      ev->accept();
      break;

    case fc::Fkey_down:
      scrollBy (0, 1);
      ev->accept();
      break;

    case fc::Fkey_left:
      scrollBy (-1, 0);
      ev->accept();
      break;

    case fc::Fkey_right:
      scrollBy (1, 0);
      ev->accept();
      break;

    case fc::Fkey_ppage:
      scrollBy (0, int(-getViewportHeight()));
      ev->accept();
      break;

    case fc::Fkey_npage:
      scrollBy (0, int(getViewportHeight()));
      ev->accept();
      break;

    case fc::Fkey_home:
      scrollToY (1);
      ev->accept();
      break;

    case fc::Fkey_end:
      scrollToY (1 + yoffset_end);
      ev->accept();
      break;

    default:
      break;
  }
}

//----------------------------------------------------------------------
void FScrollView::onWheel (FWheelEvent* ev)
{
  int distance = 4;

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
    FWidget* prev_element = getFocusWidget();

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
  FWidget* focus_widget = FWidget::getFocusWidget();

  if ( ! focus_widget )
    return;

  widget_geometry = focus_widget->getGeometryWithShadow();
  vp_geometry = viewport_geometry;
  vp_geometry.move(1, 1);

  if ( ! vp_geometry.contains(widget_geometry) )
  {
    int x
      , y
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

  FWidget* focus_widget = FWidget::getFocusWidget();

  if ( out_ev->getFocusType() == fc::FocusNextWidget )
  {
    FWidget* last_widget = getLastFocusableWidget(getChildren());

    if ( focus_widget == last_widget )
    {
      out_ev->accept();
      focusNextChild();
    }
  }
  else if ( out_ev->getFocusType() == fc::FocusPreviousWidget )
  {
    FWidget* first_widget = getFirstFocusableWidget(getChildren());

    if ( focus_widget == first_widget )
    {
      out_ev->accept();
      focusPrevChild();
    }
  }
}


// protected methods of FScrollView
//----------------------------------------------------------------------
FVTerm::term_area* FScrollView::getPrintArea()
{
  // returns print area or viewport

  if ( use_own_print_area || ! viewport )
  {
    child_print_area = 0;
    term_area* area = FWidget::getPrintArea();
    child_print_area = viewport;
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
  setHorizontalScrollBarVisibility();

  vbar->setMaximum (int(getScrollHeight() - getViewportHeight()));
  vbar->setPageSize (int(getScrollHeight()), int(getViewportHeight()));
  vbar->setX (int(width));
  vbar->setHeight (height - 2, false);
  vbar->setValue (yoffset);
  vbar->resize();
  setVerticalScrollBarVisibility();
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

  int ax = getTermX() - print_area->offset_left
    , ay = getTermY() - print_area->offset_top
    , dx = viewport_geometry.getX()
    , dy = viewport_geometry.getY()
    , y_end = int(getViewportHeight())
    , x_end = int(getViewportWidth());

  // viewport width does not fit into the print_area
  if ( print_area->width <= ax + x_end )
    x_end = print_area->width - ax;

  // viewport height does not fit into the print_area
  if ( print_area->height <= ay + y_end )
    y_end = print_area->height - ay;

  for (int y = 0; y < y_end; y++)  // line loop
  {
    charData* vc;  // viewport character
    charData* ac;  // area character
    int v_line_len = viewport->width;
    int a_line_len = print_area->width + print_area->right_shadow;
    vc = &viewport->text[(dy + y) * v_line_len + dx];
    ac = &print_area->text[(ay + y) * a_line_len + ax];
    std::memcpy (ac, vc, sizeof(charData) * unsigned(x_end));

    if ( int(print_area->changes[ay + y].xmin) > ax )
      print_area->changes[ay + y].xmin = uInt(ax);

    if ( int(print_area->changes[ay + y].xmax) < ax + x_end - 1 )
      print_area->changes[ay + y].xmax = uInt(ax + x_end - 1);
  }

  setViewportCursor();
  viewport->has_changes = false;
  print_area->has_changes = true;
}


// private methods of FScrollView
//----------------------------------------------------------------------
inline FPoint FScrollView::getViewportCursorPos()
{
  FWidget* window = FWindow::getWindowWidget(this);

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
  int xoffset_end;
  int yoffset_end;

  assert ( parent != 0 );
  assert ( ! parent->isInstanceOf("FScrollView") );

  setForegroundColor (wc.dialog_fg);
  setBackgroundColor (wc.dialog_bg);
  init_scrollbar();
  setGeometry (1, 1, 4, 4);
  setMinimumSize (4, 4);
  xoffset_end = int(getScrollWidth() - getViewportWidth());
  yoffset_end = int(getScrollHeight() - getViewportHeight());
  nf_offset = isNewFont() ? 1 : 0;
  setTopPadding (1 - getScrollY());
  setLeftPadding (1 - getScrollX());
  setBottomPadding (1 - (yoffset_end - getScrollY()));
  setRightPadding (1 - (xoffset_end - getScrollX()) + nf_offset);

  FPoint no_shadow(0, 0);
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
    child_print_area = viewport;
}

//----------------------------------------------------------------------
void FScrollView::init_scrollbar()
{
  try
  {
    vbar = new FScrollbar(fc::vertical, this);
    vbar->setMinimum(0);
    vbar->setValue(0);
    vbar->hide();

    hbar = new FScrollbar(fc::horizontal, this);
    hbar->setMinimum(0);
    hbar->setValue(0);
    hbar->hide();
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
    return;
  }

  vbar->addCallback
  (
    "change-value",
    F_METHOD_CALLBACK (this, &FScrollView::cb_VBarChange)
  );

  hbar->addCallback
  (
    "change-value",
    F_METHOD_CALLBACK (this, &FScrollView::cb_HBarChange)
  );
}

//----------------------------------------------------------------------
void FScrollView::calculateScrollbarPos()
{
  std::size_t width  = getWidth();
  std::size_t height = getHeight();

  if ( isNewFont() )
  {
    vbar->setGeometry (int(width), 2, 2, height - 2);
    hbar->setGeometry (1, int(height), width - 2, 1);
  }
  else
  {
    vbar->setGeometry (int(width), 2, 1, height - 2);
    hbar->setGeometry (2, int(height), width - 2, 1);
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
        hbar->setVisible();
      else
        hbar->hide();
      break;

    case fc::Hidden:
      hbar->hide();
      break;

    case fc::Scroll:
      hbar->setVisible();
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
        vbar->setVisible();
      else
        vbar->hide();
      break;

    case fc::Hidden:
      vbar->hide();
      break;

    case fc::Scroll:
      vbar->setVisible();
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
  FPoint window_cursor_pos = getViewportCursorPos();
  print_area->input_cursor_x = window_cursor_pos.getX();
  print_area->input_cursor_y = window_cursor_pos.getY();

  if ( viewport->input_cursor_visible
    && viewport_geometry.contains(cursor_pos) )
    print_area->input_cursor_visible = true;
  else
    print_area->input_cursor_visible = false;
}

//----------------------------------------------------------------------
void FScrollView::cb_VBarChange (FWidget*, data_ptr)
{
  FScrollbar::sType scrollType = vbar->getScrollType();
  int distance = 1;
  int wheel_distance = 4;

  if ( scrollType >= FScrollbar::scrollStepBackward
    && scrollType <= FScrollbar::scrollWheelDown )
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
void FScrollView::cb_HBarChange (FWidget*, data_ptr)
{
  FScrollbar::sType scrollType = hbar->getScrollType();
  int distance = 1;
  int wheel_distance = 4;

  if ( scrollType >= FScrollbar::scrollStepBackward
    && scrollType <= FScrollbar::scrollWheelDown )
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

//----------------------------------------------------------------------
inline void FScrollView::redrawHBar()
{
  child_print_area = 0;

  if ( hbar->isVisible() )
    hbar->redraw();

  child_print_area = viewport;
}

//----------------------------------------------------------------------
inline void FScrollView::redrawVBar()
{
  child_print_area = 0;

  if ( vbar->isVisible() )
    vbar->redraw();

  child_print_area = viewport;
}

//----------------------------------------------------------------------
inline void FScrollView::drawHBar()
{
  child_print_area = 0;

  if ( hbar->isVisible() )
    hbar->drawBar();

  child_print_area = viewport;
}

//----------------------------------------------------------------------
inline void FScrollView::drawVBar()
{
  child_print_area = 0;

  if ( vbar->isVisible() )
    vbar->drawBar();

  child_print_area = viewport;
}

}  // namespace finalcut
