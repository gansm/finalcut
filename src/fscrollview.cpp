// File: fscrollview.cpp
// Provides: class FScrollView

#include "fscrollview.h"


//----------------------------------------------------------------------
// class FScrollView
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FScrollView::FScrollView (FWidget* parent)
  : FWidget(parent)
  , scroll_size(1,1,1,1)
  , scroll_offset()
  , viewport(0)
  , vbar(0)
  , hbar(0)
  , nf_offset(0)
  , use_own_print_area(false)
  , vMode(fc::Auto)
  , hMode(fc::Auto)
{
  init();
}

//----------------------------------------------------------------------
FScrollView::~FScrollView() // destructor
{
  delete vbar;
  delete hbar;
  removeArea (viewport);
}


// public methods of FScrollView
//----------------------------------------------------------------------
void FScrollView::setScrollWidth (int width)
{
  if ( width < getClientWidth() )
    width = getClientWidth();

  if ( getScrollWidth() == width )
    return;

  if ( viewport )
  {
    FPoint no_shadow(0,0);
    scroll_size.setWidth (width);
    resizeArea (scroll_size, no_shadow, viewport);
  }

  hbar->setMaximum (width - getClientWidth());
  hbar->setPageSize (width, getClientWidth());
  hbar->calculateSliderValues();
  setHorizontalScrollBarVisibility();
}

//----------------------------------------------------------------------
void FScrollView::setScrollHeight (int height)
{
  if ( height < getClientHeight() )
    height = getClientHeight();

  if ( getScrollHeight() == height )
    return;

  if ( viewport )
  {
    FPoint no_shadow(0,0);
    scroll_size.setHeight (height);
    resizeArea (scroll_size, no_shadow, viewport);
  }

  vbar->setMaximum (height - getClientHeight());
  vbar->setPageSize (height, getClientHeight());
  vbar->calculateSliderValues();
  setVerticalScrollBarVisibility();
}

//----------------------------------------------------------------------
void FScrollView::setScrollSize (int width, int height)
{
  if ( width < getClientWidth() )
    width = getClientWidth();

  if ( height < getClientHeight() )
    height = getClientHeight();

  if ( getScrollWidth() == width && getScrollHeight() == height )
    return;

  if ( viewport )
  {
    FPoint no_shadow(0,0);
    scroll_size.setSize (width, height);
    resizeArea (scroll_size, no_shadow, viewport);
  }

  hbar->setMaximum (width - getClientWidth());
  hbar->setPageSize (width, getClientWidth());
  hbar->calculateSliderValues();
  setHorizontalScrollBarVisibility();

  vbar->setMaximum (height - getClientHeight());
  vbar->setPageSize (height, getClientHeight());
  vbar->calculateSliderValues();
  setVerticalScrollBarVisibility();
}

//----------------------------------------------------------------------
void FScrollView::setX (int x, bool adjust)
{
  FWidget::setX (x, adjust);

  if ( ! adjust )
  {
    scroll_size.setX (getTermX() - 1);

    if ( viewport )
    {
      viewport->x_offset = scroll_size.getX();
      viewport->y_offset = scroll_size.getY();
    }
  }
}

//----------------------------------------------------------------------
void FScrollView::setY (int y, bool adjust)
{
  FWidget::setY (y, adjust);

  if ( ! adjust )
  {
    scroll_size.setY (getTermY() - 1);

    if ( viewport )
    {
      viewport->x_offset = scroll_size.getX();
      viewport->y_offset = scroll_size.getY();
    }
  }
}

//----------------------------------------------------------------------
void FScrollView::setPos (int x, int y, bool adjust)
{
  FWidget::setPos (x, y, adjust);
  scroll_size.setPos (getTermX() - 1, getTermY() - 1);

  if ( ! adjust )
  {
    if ( viewport )
    {
      viewport->x_offset = scroll_size.getX();
      viewport->y_offset = scroll_size.getY();
    }
  }
}

//----------------------------------------------------------------------
void FScrollView::setWidth (int w, bool adjust)
{
  FWidget::setWidth (w, adjust);
  calculateScrollbarPos();

  if ( getScrollWidth() < getClientWidth() )
    setScrollWidth (getClientWidth());
}

//----------------------------------------------------------------------
void FScrollView::setHeight (int h, bool adjust)
{
  FWidget::setHeight (h, adjust);
  calculateScrollbarPos();

  if ( getScrollHeight() < getClientHeight() )
    setScrollHeight (getClientHeight());
}

//----------------------------------------------------------------------
void FScrollView::setSize (int w, int h, bool adjust)
{
  FWidget::setSize (w, h, adjust);
  calculateScrollbarPos();

  if ( getScrollWidth() < getClientWidth()
      || getScrollHeight() < getClientHeight() )
    setScrollSize (getClientWidth(), getClientHeight());
}

//----------------------------------------------------------------------
void FScrollView::setGeometry (int x, int y, int w, int h, bool adjust)
{
  FWidget::setGeometry (x, y, w, h, adjust);
  scroll_size.setPos (getTermX() - 1, getTermY() - 1);
  calculateScrollbarPos();

  if ( getScrollWidth() < getClientWidth()
      || getScrollHeight() < getClientHeight() )
  {
    setScrollSize (getClientWidth(), getClientHeight());
  }
  else if ( ! adjust && viewport )
  {
    viewport->x_offset = scroll_size.getX();
    viewport->y_offset = scroll_size.getY();
  }
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
void FScrollView::draw()
{
  use_own_print_area = true;

  if ( FWidget* p = getParentWidget() )
    setColor (p->getForegroundColor(), p->getBackgroundColor());
  else
    setColor (wc.dialog_fg, wc.dialog_bg);

  if ( isNewFont() )
    drawBorder (1, 1, getWidth() - 1, getHeight());
  else
    drawBorder();

  use_own_print_area = false;
  copy2area();

  if ( vbar->isVisible() )
    vbar->redraw();

  if ( hbar->isVisible() )
    hbar->redraw();
}

//----------------------------------------------------------------------
void FScrollView::onKeyPress (FKeyEvent* ev)
{
  int    key = ev->key();
  bool   hasChanges = false;
  short& xoffset = scroll_offset.x_ref();
  short& yoffset = scroll_offset.y_ref();
  short  xoffset_before = xoffset;
  short  yoffset_before = yoffset;
  short  xoffset_end = short(getScrollWidth() - getClientWidth());
  short  yoffset_end = short(getScrollHeight() - getClientHeight());

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
      yoffset -= getClientHeight();

      if ( yoffset < 0 )
        yoffset = 0;

      ev->accept();
      break;

    case fc::Fkey_npage:
      yoffset += getClientHeight();

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
    if ( isVisible() && viewport
        && (xoffset_before != xoffset || yoffset_before != yoffset) )
    {
      viewport->has_changes = true;
      copy2area();
      hasChanges = true;
      vbar->setValue (yoffset);
      hbar->setValue (xoffset);

      if ( vbar->isVisible() )
        vbar->drawBar();

      if ( hbar->isVisible() )
        hbar->drawBar();
    }

    if ( hasChanges )
      updateTerminal();
  }
}

//----------------------------------------------------------------------
void FScrollView::onWheel (FWheelEvent* ev)
{
  bool   hasChanges = false;
  short& yoffset = scroll_offset.y_ref();
  short  yoffset_before = yoffset;
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
        short yoffset_end = short(getScrollHeight() - getClientHeight());

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
    viewport->has_changes = true;
    copy2area();
    hasChanges = true;
    vbar->setValue (yoffset);

    if ( vbar->isVisible() )
      vbar->drawBar();
  }

  if ( hasChanges )
    updateTerminal();
}


// protected methods of FScrollView
//----------------------------------------------------------------------
FVTerm::term_area* FScrollView::getPrintArea()
{
  // returns the viewport print area

  if ( viewport && ! use_own_print_area )
    return viewport;
  else
    return FVTerm::getPrintArea();
}

//----------------------------------------------------------------------
void FScrollView::adjustSize()
{
  FWidget::adjustSize();
  int width   = getWidth();
  int height  = getHeight();
  int xoffset = scroll_offset.getX();
  int yoffset = scroll_offset.getY();

  scroll_size.setPos (getTermX() - 1, getTermY() - 1);

  if ( viewport )
  {
    viewport->x_offset = scroll_size.getX();
    viewport->y_offset = scroll_size.getY();
  }

  hbar->setMaximum (getScrollWidth() - getClientWidth());
  hbar->setPageSize (getScrollWidth(), getClientWidth());
  hbar->setY (height);
  hbar->setWidth (width - 2, false);
  hbar->setValue (xoffset);
  hbar->resize();
  setHorizontalScrollBarVisibility();

  vbar->setMaximum (getScrollHeight() - getClientHeight());
  vbar->setPageSize (getScrollHeight(), getClientHeight());
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
  char_data* vc; // viewport character
  char_data* ac; // area character

  if ( ! print_area )
    FVTerm::getPrintArea();

  if ( ! (print_area && viewport) )
    return;

  if ( ! viewport->has_changes )
    return;

  ax = getTermX() - print_area->x_offset;
  ay = getTermY() - print_area->y_offset;
  dx = scroll_offset.getX();
  dy = scroll_offset.getY();
  y_end = getClientHeight();
  x_end = getClientWidth();

  for (int y=0; y < y_end; y++)  // line loop
  {
    int v_line_len = viewport->width;
    int a_line_len = print_area->width + print_area->right_shadow;
    vc = &viewport->text[(dy+y) * v_line_len + dx];
    ac = &print_area->text[(ay+y) * a_line_len + ax];
    std::memcpy (ac, vc, sizeof(char_data) * unsigned(x_end));

    if ( short(print_area->changes[ay+y].xmin) > ax )
      print_area->changes[ay+y].xmin = uInt(ax);

    if ( short(print_area->changes[ay+y].xmax) < ax+x_end-1 )
      print_area->changes[ay+y].xmax = uInt(ax+x_end-1);
  }

  viewport->has_changes = false;
  print_area->has_changes = true;
}


// private methods of FScrollView
//----------------------------------------------------------------------
void FScrollView::init()
{
  vbar = new FScrollbar(fc::vertical, this);
  vbar->setMinimum(0);
  vbar->setValue(0);
  vbar->hide();

  hbar = new FScrollbar(fc::horizontal, this);
  hbar->setMinimum(0);
  hbar->setValue(0);
  hbar->hide();

  vbar->addCallback
  (
    "change-value",
    _METHOD_CALLBACK (this, &FScrollView::cb_VBarChange)
  );
  hbar->addCallback
  (
    "change-value",
    _METHOD_CALLBACK (this, &FScrollView::cb_HBarChange)
  );

  nf_offset = isNewFont() ? 1 : 0;
  setTopPadding(1);
  setLeftPadding(1);
  setBottomPadding(1);
  setRightPadding(1 + nf_offset);

  FPoint no_shadow(0,0);
  scroll_size.setRect (0, 0, getClientWidth(), getClientHeight());
  createArea (scroll_size, no_shadow, viewport);
}

//----------------------------------------------------------------------
void FScrollView::calculateScrollbarPos()
{
  int width  = getWidth();
  int height = getHeight();

  if ( isNewFont() )
  {
    vbar->setGeometry (width, 2, 2, height-2);
    hbar->setGeometry (1, height, width-2, 1);
  }
  else
  {
    vbar->setGeometry (width, 2, 1, height-2);
    hbar->setGeometry (2, height, width-2, 1);
  }

  setRightPadding (1 + nf_offset);
  vbar->resize();
  hbar->resize();
}

//----------------------------------------------------------------------
void FScrollView::setHorizontalScrollBarVisibility()
{
  switch ( hMode )
  {
    case fc::Auto:
      if ( getScrollWidth() > getClientWidth() )
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
      if ( getScrollHeight() > getClientHeight() )
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
void FScrollView::cb_VBarChange (FWidget*, void*)
{
  FScrollbar::sType scrollType;
  bool   hasChanges = false;
  short  distance = 1;
  short& yoffset = scroll_offset.y_ref();
  short  yoffset_before = yoffset;
  short  yoffset_end = short(getScrollHeight() - getClientHeight());
  scrollType = vbar->getScrollType();

  switch ( scrollType )
  {
    case FScrollbar::noScroll:
      break;

    case FScrollbar::scrollPageBackward:
      distance = short(getClientHeight());
      // fall through
    case FScrollbar::scrollStepBackward:
      yoffset -= distance;

      if ( yoffset < 0 )
        yoffset = 0;

      break;

    case FScrollbar::scrollPageForward:
      distance = short(getClientHeight());
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
    viewport->has_changes = true;
    copy2area();
    hasChanges = true;
  }

  if ( scrollType >= FScrollbar::scrollStepBackward
      && scrollType <= FScrollbar::scrollWheelDown
      && hasChanges )
  {
    vbar->setValue (yoffset);

    if ( vbar->isVisible() )
      vbar->drawBar();
  }

  if ( hasChanges )
    updateTerminal();
}

//----------------------------------------------------------------------
void FScrollView::cb_HBarChange (FWidget*, void*)
{
  FScrollbar::sType scrollType;
  bool   hasChanges = false;
  short  distance = 1;
  short& xoffset = scroll_offset.x_ref();
  short  xoffset_before = xoffset;
  short  xoffset_end = short(getScrollWidth() - getClientWidth());
  scrollType = hbar->getScrollType();

  switch ( scrollType )
  {
    case FScrollbar::noScroll:
      break;

    case FScrollbar::scrollPageBackward:
      distance = short(getClientWidth());
      // fall through
    case FScrollbar::scrollStepBackward:
      xoffset -= distance;

      if ( xoffset < 0 )
        xoffset = 0;

      break;

    case FScrollbar::scrollPageForward:
      distance = short(getClientWidth());
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
    viewport->has_changes = true;
    copy2area();
    hasChanges = true;
  }

  if ( scrollType >= FScrollbar::scrollStepBackward
      && scrollType <= FScrollbar::scrollWheelDown
      && hasChanges )
  {
    hbar->setValue (xoffset);

    if ( hbar->isVisible() )
      hbar->drawBar();
  }

  if ( hasChanges )
    updateTerminal();
}
