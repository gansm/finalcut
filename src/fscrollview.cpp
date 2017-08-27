// File: fscrollview.cpp
// Provides: class FScrollView

#include "fscrollview.h"
#include "fwindow.h"


//----------------------------------------------------------------------
// class FScrollView
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FScrollView::FScrollView (FWidget* parent)
  : FWidget(parent)
  , scroll_geometry(1,1,1,1)
  , viewport_geometry()
  , viewport(0)
  , vbar(0)
  , hbar(0)
  , nf_offset(0)
  , border(true)
  , use_own_print_area(false)
  , vMode(fc::Auto)
  , hMode(fc::Auto)
{
  init(parent);
}

//----------------------------------------------------------------------
FScrollView::~FScrollView() // destructor
{
  delete vbar;
  delete hbar;
  removeArea (viewport);
  child_print_area = viewport = 0;
}


// public methods of FScrollView
//----------------------------------------------------------------------
void FScrollView::setScrollWidth (int width)
{
  if ( width < getViewportWidth() )
    width = getViewportWidth();

  if ( getScrollWidth() == width )
    return;

  if ( viewport )
  {
    FPoint no_shadow(0,0);
    scroll_geometry.setWidth (width);
    resizeArea (scroll_geometry, no_shadow, viewport);

    addPreprocessingHandler
    (
      F_PREPROC_HANDLER (this, &FScrollView::copy2area)
    );
    child_print_area = viewport;
  }

  hbar->setMaximum (width - getViewportWidth());
  hbar->setPageSize (width, getViewportWidth());
  hbar->calculateSliderValues();
  setHorizontalScrollBarVisibility();
}

//----------------------------------------------------------------------
void FScrollView::setScrollHeight (int height)
{
  if ( height < getViewportHeight() )
    height = getViewportHeight();

  if ( getScrollHeight() == height )
    return;

  if ( viewport )
  {
    FPoint no_shadow(0,0);
    scroll_geometry.setHeight (height);
    resizeArea (scroll_geometry, no_shadow, viewport);
    addPreprocessingHandler
    (
      F_PREPROC_HANDLER (this, &FScrollView::copy2area)
    );
    child_print_area = viewport;
  }

  vbar->setMaximum (height - getViewportHeight());
  vbar->setPageSize (height, getViewportHeight());
  vbar->calculateSliderValues();
  setVerticalScrollBarVisibility();
}

//----------------------------------------------------------------------
void FScrollView::setScrollSize (int width, int height)
{
  if ( width < getViewportWidth() )
    width = getViewportWidth();

  if ( height < getViewportHeight() )
    height = getViewportHeight();

  if ( getScrollWidth() == width && getScrollHeight() == height )
    return;

  if ( viewport )
  {
    FPoint no_shadow(0,0);
    scroll_geometry.setSize (width, height);
    resizeArea (scroll_geometry, no_shadow, viewport);
    addPreprocessingHandler
    (
      F_PREPROC_HANDLER (this, &FScrollView::copy2area)
    );
    child_print_area = viewport;
  }

  setTopPadding (1 - getScrollY());
  setLeftPadding (1 - getScrollX());
  setBottomPadding (1 - (getScrollHeight() - getViewportHeight() - getScrollY()));
  setRightPadding (1 - (getScrollWidth() - getViewportWidth() - getScrollX()) + nf_offset);

  hbar->setMaximum (width - getViewportWidth());
  hbar->setPageSize (width, getViewportWidth());
  hbar->calculateSliderValues();
  setHorizontalScrollBarVisibility();

  vbar->setMaximum (height - getViewportHeight());
  vbar->setPageSize (height, getViewportHeight());
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
void FScrollView::setWidth (int w, bool adjust)
{
  FWidget::setWidth (w, adjust);
  viewport_geometry.setWidth(w - vertical_border_spacing - nf_offset);
  calculateScrollbarPos();

  if ( getScrollWidth() < getViewportWidth() )
    setScrollWidth (getViewportWidth());
}

//----------------------------------------------------------------------
void FScrollView::setHeight (int h, bool adjust)
{
  FWidget::setHeight (h, adjust);
  viewport_geometry.setHeight(h - horizontal_border_spacing);
  calculateScrollbarPos();

  if ( getScrollHeight() < getViewportHeight() )
    setScrollHeight (getViewportHeight());
}

//----------------------------------------------------------------------
void FScrollView::setSize (int w, int h, bool adjust)
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
void FScrollView::setGeometry (int x, int y, int w, int h, bool adjust)
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
void FScrollView::setCursorPos (register int x, register int y)
{
  FWidget::setCursorPos (x + getLeftPadding(), y + getTopPadding());
}

//----------------------------------------------------------------------
void FScrollView::setPrintPos (register int x, register int y)
{
  FWidget::setPrintPos (x + getLeftPadding(), y + getTopPadding());
}

//----------------------------------------------------------------------
bool FScrollView::setViewportPrint (bool on)
{
  return use_own_print_area = ! on;
}

//----------------------------------------------------------------------
bool FScrollView::setBorder (bool on)
{
  return border = on;
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
  short& xoffset = viewport_geometry.x1_ref();
  short& yoffset = viewport_geometry.y1_ref();
  short  xoffset_end = short(getScrollWidth() - getViewportWidth());
  short  yoffset_end = short(getScrollHeight() - getViewportHeight());
  int    save_width = viewport_geometry.getWidth();
  int    save_height = viewport_geometry.getHeight();
  x--;
  y--;

  if ( xoffset == short(x) && yoffset == short(y) )
    return;

  xoffset = short(x);
  yoffset = short(y);

  if ( yoffset < 0 )
    yoffset = 0;

  if ( yoffset > yoffset_end )
    yoffset = yoffset_end;

  if ( xoffset < 0 )
    xoffset = 0;

  if ( xoffset > xoffset_end )
    xoffset = xoffset_end;

  viewport_geometry.setWidth(save_width);
  viewport_geometry.setHeight(save_height);
  viewport->has_changes = true;
  setTopPadding (1 - yoffset);
  setLeftPadding (1 - xoffset);
  setBottomPadding (1 - (getScrollHeight() - getViewportHeight() - yoffset));
  setRightPadding (1 - (getScrollWidth() - getViewportWidth() - xoffset) + nf_offset);
  copy2area();
  hbar->setValue (xoffset);
  vbar->setValue (yoffset);
  drawHBar();
  drawVBar();
  updateTerminal();
}

//----------------------------------------------------------------------
void FScrollView::scrollBy (int dx, int dy)
{
  scrollTo (getScrollX() + dx, getScrollY() + dy);
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
      drawBorder (1, 1, getWidth() - 1, getHeight());
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
  int    key = ev->key();
  short& xoffset = viewport_geometry.x1_ref();
  short& yoffset = viewport_geometry.y1_ref();
  short  xoffset_before = xoffset;
  short  yoffset_before = yoffset;
  short  xoffset_end = short(getScrollWidth() - getViewportWidth());
  short  yoffset_end = short(getScrollHeight() - getViewportHeight());
  int    save_width = viewport_geometry.getWidth();
  int    save_height = viewport_geometry.getHeight();

  switch ( key )
  {
    case fc::Fkey_up:
      if ( yoffset > 0 )
        yoffset--;

      ev->accept();
      break;

    case fc::Fkey_down:
      if ( yoffset < yoffset_end )
        yoffset++;

      ev->accept();
      break;

    case fc::Fkey_left:
      if ( xoffset > 0 )
        xoffset--;

      ev->accept();
      break;

    case fc::Fkey_right:
      if ( xoffset < xoffset_end )
        xoffset++;

      ev->accept();
      break;

    case fc::Fkey_ppage:
      yoffset -= getViewportHeight();

      if ( yoffset < 0 )
        yoffset = 0;

      ev->accept();
      break;

    case fc::Fkey_npage:
      yoffset += getViewportHeight();

      if ( yoffset > yoffset_end )
        yoffset = yoffset_end;

      ev->accept();
      break;

    case fc::Fkey_home:
      yoffset = 0;
      ev->accept();
      break;

    case fc::Fkey_end:
      yoffset = yoffset_end;
      ev->accept();
      break;

    default:
      break;
  }

  if ( ev->isAccepted() )
  {
    bool hasChanges = false;

    if ( isVisible() && viewport
        && (xoffset_before != xoffset || yoffset_before != yoffset) )
    {
      viewport_geometry.setWidth(save_width);
      viewport_geometry.setHeight(save_height);
      viewport->has_changes = true;
      setTopPadding (1 - yoffset);
      setLeftPadding (1 - xoffset);
      setBottomPadding (1 - (getScrollHeight() - getViewportHeight() - yoffset));
      setRightPadding (1 - (getScrollWidth() - getViewportWidth() - xoffset) + nf_offset);
      copy2area();
      hasChanges = true;
      vbar->setValue (yoffset);
      hbar->setValue (xoffset);
      drawVBar();
      drawHBar();
    }

    if ( hasChanges )
      updateTerminal();
  }
}

//----------------------------------------------------------------------
void FScrollView::onWheel (FWheelEvent* ev)
{
  bool   hasChanges = false;
  short& yoffset = viewport_geometry.y1_ref();
  short  yoffset_before = yoffset;
  int    save_height = viewport_geometry.getHeight();
  int    wheel = ev->getWheel();

  switch ( wheel )
  {
    case fc::WheelUp:
      if ( yoffset == 0 )
        break;

      yoffset -= 4;

      if ( yoffset < 0 )
        yoffset=0;

      break;

    case fc::WheelDown:
      {
        short yoffset_end = short(getScrollHeight() - getViewportHeight());

        if ( yoffset_end < 0 )
          yoffset_end = 0;

        if ( yoffset == yoffset_end )
          break;

        yoffset += 4;

        if ( yoffset > yoffset_end )
          yoffset = yoffset_end;
      }
      break;

    default:
      break;
  }

  if ( isVisible() && viewport && yoffset_before != yoffset )
  {
    viewport_geometry.setHeight(save_height);
    viewport->has_changes = true;
    setTopPadding (1 - yoffset);
    setBottomPadding (1 - (getScrollHeight() - getViewportHeight() - yoffset));
    copy2area();
    hasChanges = true;
    vbar->setValue (yoffset);
    drawVBar();
  }

  if ( hasChanges )
    updateTerminal();
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
  vp_geometry.move(1,1);

  if ( ! vp_geometry.contains(widget_geometry) )
  {
    int x, y;
    int vx = vp_geometry.getX();
    int vy = vp_geometry.getY();
    int wx = widget_geometry.getX();
    int wy = widget_geometry.getY();

    if ( wx > vx )
      x = widget_geometry.getX2() - vp_geometry.getWidth() + 1;
    else
      x = wx;

    if ( wy > vy )
      y = widget_geometry.getY2() - vp_geometry.getHeight() + 1;
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
    term_area* area = FVTerm::getPrintArea();
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
  int width   = getWidth();
  int height  = getHeight();
  int xoffset = viewport_geometry.getX();
  int yoffset = viewport_geometry.getY();

  scroll_geometry.setPos ( getTermX() + getLeftPadding() - 1
                         , getTermY() + getTopPadding() - 1 );

  if ( viewport )
  {
    viewport->offset_left = scroll_geometry.getX();
    viewport->offset_top = scroll_geometry.getY();
  }

  hbar->setMaximum (getScrollWidth() - getViewportWidth());
  hbar->setPageSize (getScrollWidth(), getViewportWidth());
  hbar->setY (height);
  hbar->setWidth (width - 2, false);
  hbar->setValue (xoffset);
  hbar->resize();
  setHorizontalScrollBarVisibility();

  vbar->setMaximum (getScrollHeight() - getViewportHeight());
  vbar->setPageSize (getScrollHeight(), getViewportHeight());
  vbar->setX (width);
  vbar->setHeight (height - 2, false);
  vbar->setValue (yoffset);
  vbar->resize();
  setVerticalScrollBarVisibility();
}

//----------------------------------------------------------------------
void FScrollView::copy2area()
{
  // copy viewport to area

  int ax, ay, dx, dy, y_end, x_end;

  if ( ! hasPrintArea() )
    FVTerm::getPrintArea();

  if ( ! (hasPrintArea() && viewport) )
    return;

  if ( ! viewport->has_changes )
    return;

  ax = getTermX() - print_area->offset_left;
  ay = getTermY() - print_area->offset_top;
  dx = viewport_geometry.getX();
  dy = viewport_geometry.getY();
  y_end = getViewportHeight();
  x_end = getViewportWidth();

  for (int y = 0; y < y_end; y++)  // line loop
  {
    char_data* vc; // viewport character
    char_data* ac; // area character
    int v_line_len = viewport->width;
    int a_line_len = print_area->width + print_area->right_shadow;
    vc = &viewport->text[(dy + y) * v_line_len + dx];
    ac = &print_area->text[(ay + y) * a_line_len + ax];
    std::memcpy (ac, vc, sizeof(char_data) * unsigned(x_end));

    if ( short(print_area->changes[ay + y].xmin) > ax )
      print_area->changes[ay + y].xmin = uInt(ax);

    if ( short(print_area->changes[ay + y].xmax) < ax + x_end - 1 )
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
    int widget_offsetX = getTermX() - window->getTermX();
    int widget_offsetY = getTermY() - window->getTermY();
    int x = widget_offsetX + viewport->input_cursor_x
          - viewport_geometry.getX();
    int y = widget_offsetY + viewport->input_cursor_y
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

  setForegroundColor (wc.dialog_fg);
  setBackgroundColor (wc.dialog_bg);

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
    std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
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

  nf_offset = isNewFont() ? 1 : 0;
  setTopPadding (1 - getScrollY());
  setLeftPadding (1 - getScrollX());
  setBottomPadding (1 - (getScrollHeight() - getViewportHeight() - getScrollY()));
  setRightPadding (1 - (getScrollWidth() - getViewportWidth() - getScrollX()) + nf_offset);

  FPoint no_shadow(0,0);
  int w = getViewportWidth();
  int h = getViewportHeight();

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
void FScrollView::calculateScrollbarPos()
{
  int width  = getWidth();
  int height = getHeight();

  if ( isNewFont() )
  {
    vbar->setGeometry (width, 2, 2, height - 2);
    hbar->setGeometry (1, height, width - 2, 1);
  }
  else
  {
    vbar->setGeometry (width, 2, 1, height - 2);
    hbar->setGeometry (2, height, width - 2, 1);
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
  FScrollbar::sType scrollType;
  bool   hasChanges = false;
  short  distance = 1;
  short& yoffset = viewport_geometry.y1_ref();
  short  yoffset_before = yoffset;
  short  yoffset_end = short(getScrollHeight() - getViewportHeight());
  int    save_height = viewport_geometry.getHeight();
  scrollType = vbar->getScrollType();

  switch ( scrollType )
  {
    case FScrollbar::noScroll:
      break;

    case FScrollbar::scrollPageBackward:
      distance = short(getViewportHeight());
      // fall through
    case FScrollbar::scrollStepBackward:
      yoffset -= distance;

      if ( yoffset < 0 )
        yoffset = 0;

      break;

    case FScrollbar::scrollPageForward:
      distance = short(getViewportHeight());
      // fall through
    case FScrollbar::scrollStepForward:
      yoffset += distance;

      if ( yoffset > yoffset_end )
        yoffset = yoffset_end;

      break;

    case FScrollbar::scrollJump:
    {
      short val = short(vbar->getValue());

      if ( yoffset == val )
        break;

      yoffset = val;

      if ( yoffset > yoffset_end )
        yoffset = yoffset_end;

      if ( yoffset < 0 )
        yoffset = 0;

      break;
    }

    case FScrollbar::scrollWheelUp:
      if ( yoffset == 0 )
        break;

      yoffset -= 4;

      if ( yoffset < 0 )
        yoffset=0;

      break;

    case FScrollbar::scrollWheelDown:
      if ( yoffset_end < 0 )
        yoffset_end = 0;

      if ( yoffset == yoffset_end )
        break;

      yoffset += 4;

      if ( yoffset > yoffset_end )
        yoffset = yoffset_end;

      break;
  }

  if ( isVisible() && viewport && yoffset_before != yoffset )
  {
    viewport_geometry.setHeight(save_height);
    viewport->has_changes = true;
    setTopPadding (1 - yoffset);
    setBottomPadding (1 - (getScrollHeight() - getViewportHeight() - yoffset));
    copy2area();
    hasChanges = true;
  }

  if ( scrollType >= FScrollbar::scrollStepBackward
      && scrollType <= FScrollbar::scrollWheelDown
      && hasChanges )
  {
    vbar->setValue (yoffset);
    drawVBar();
  }

  if ( hasChanges )
    updateTerminal();
}

//----------------------------------------------------------------------
void FScrollView::cb_HBarChange (FWidget*, data_ptr)
{
  FScrollbar::sType scrollType;
  bool   hasChanges = false;
  short  distance = 1;
  short& xoffset = viewport_geometry.x1_ref();
  short  xoffset_before = xoffset;
  short  xoffset_end = short(getScrollWidth() - getViewportWidth());
  int    save_width = viewport_geometry.getWidth();
  scrollType = hbar->getScrollType();

  switch ( scrollType )
  {
    case FScrollbar::noScroll:
      break;

    case FScrollbar::scrollPageBackward:
      distance = short(getViewportWidth());
      // fall through
    case FScrollbar::scrollStepBackward:
      xoffset -= distance;

      if ( xoffset < 0 )
        xoffset = 0;

      break;

    case FScrollbar::scrollPageForward:
      distance = short(getViewportWidth());
      // fall through
    case FScrollbar::scrollStepForward:
      xoffset += distance;

      if ( xoffset > xoffset_end )
        xoffset = xoffset_end;

      if ( xoffset < 0 )
        xoffset = 0;

      break;

    case FScrollbar::scrollJump:
    {
      short val = short(hbar->getValue());

      if ( xoffset == val )
        break;

      xoffset = val;

      if ( xoffset > xoffset_end )
        xoffset = xoffset_end;

      if ( xoffset < 0 )
        xoffset = 0;

      break;
    }

    case FScrollbar::scrollWheelUp:
      if ( xoffset == 0 )
        break;

      xoffset -= 4;

      if ( xoffset < 0 )
        xoffset=0;

      break;

    case FScrollbar::scrollWheelDown:
      if ( xoffset == xoffset_end )
        break;

      xoffset += 4;

      if ( xoffset > xoffset_end )
        xoffset = xoffset_end;

      break;
  }

  if ( isVisible() && viewport && xoffset_before != xoffset )
  {
    viewport_geometry.setWidth(save_width);
    viewport->has_changes = true;
    setLeftPadding (1 - xoffset);
    setRightPadding (1 - (getScrollWidth() - getViewportWidth() - xoffset) + nf_offset);
    copy2area();
    hasChanges = true;
  }

  if ( scrollType >= FScrollbar::scrollStepBackward
      && scrollType <= FScrollbar::scrollWheelDown
      && hasChanges )
  {
    hbar->setValue (xoffset);
    drawHBar();
  }

  if ( hasChanges )
    updateTerminal();
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
