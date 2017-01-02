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
  , own_print_area(false)
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

  if ( scroll_size.getWidth() == width )
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

  if ( ! hbar->isVisible() )
    hbar->setVisible();
}

//----------------------------------------------------------------------
void FScrollView::setScrollHeight (int height)
{
  if ( height < getClientHeight() )
    height = getClientHeight();

  if ( scroll_size.getHeight() == height )
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

  if ( ! vbar->isVisible()  )
    vbar->setVisible();
}

//----------------------------------------------------------------------
void FScrollView::setScrollSize (int width, int height)
{
  if ( width < getClientWidth() )
    width = getClientWidth();

  if ( height < getClientHeight() )
    height = getClientHeight();

  if ( scroll_size.getWidth() == width
      && scroll_size.getHeight() == height )
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

  if ( ! hbar->isVisible() )
    hbar->setVisible();

  vbar->setMaximum (height - getClientHeight());
  vbar->setPageSize (height, getClientHeight());
  vbar->calculateSliderValues();

  if ( ! vbar->isVisible()  )
    vbar->setVisible();
}

//----------------------------------------------------------------------
void FScrollView::setX (int x, bool adjust)
{
  FWidget::setX (x, adjust);
  scroll_size.setX (getTermX() - 1);
}

//----------------------------------------------------------------------
void FScrollView::setY (int y, bool adjust)
{
  FWidget::setY (y, adjust);
  scroll_size.setY (getTermY() - 1);
}

//----------------------------------------------------------------------
void FScrollView::setPos (int x, int y, bool adjust)
{
  FWidget::setPos (x, y, adjust);
  scroll_size.setPos (getTermX() - 1, getTermY() - 1);
}

//----------------------------------------------------------------------
void FScrollView::setWidth (int w, bool adjust)
{
  FWidget::setWidth (w, adjust);
  calculateScrollbarPos();

  if ( scroll_size.getWidth() < getClientWidth() )
    setScrollWidth (getClientWidth());
}

//----------------------------------------------------------------------
void FScrollView::setHeight (int h, bool adjust)
{
  FWidget::setHeight (h, adjust);
  calculateScrollbarPos();

  if ( scroll_size.getHeight() < getClientHeight() )
    setScrollHeight (getClientHeight());
}

//----------------------------------------------------------------------
void FScrollView::setSize (int w, int h, bool adjust)
{
  FWidget::setSize (w, h, adjust);
  calculateScrollbarPos();

  if ( scroll_size.getWidth() < getClientWidth()
      || scroll_size.getHeight() < getClientHeight() )
    setScrollSize (getClientWidth(), getClientHeight());
}

//----------------------------------------------------------------------
void FScrollView::setGeometry (int x, int y, int w, int h, bool adjust)
{
  FWidget::setGeometry (x, y, w, h, adjust);
  scroll_size.setPos (getTermX() - 1, getTermY() - 1);
  calculateScrollbarPos();

  if ( scroll_size.getWidth() < getClientWidth()
      || scroll_size.getHeight() < getClientHeight() )
    setScrollSize (getClientWidth(), getClientHeight());
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
  own_print_area = true;

  if ( FWidget* p = getParentWidget() )
    setColor (p->getForegroundColor(), p->getBackgroundColor());
  else
    setColor (wc.dialog_fg, wc.dialog_bg);

  if ( isNewFont() )
    drawBorder (1, 1, getWidth() - 1, getHeight());
  else
    drawBorder();

  own_print_area = false;
  copy2area();

  if ( vbar->isVisible() )
    vbar->redraw();

  if ( hbar->isVisible() )
    hbar->redraw();

}

//----------------------------------------------------------------------
void FScrollView::onWheel (FWheelEvent* ev)
{
  int yoffset = scroll_offset.getY();
  int yoffset_before = yoffset;
  int wheel = ev->getWheel();

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
        int yoffset_end = scroll_size.getHeight() - getClientHeight();

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

  scroll_offset.setY (yoffset);

  if ( isVisible() )
  {
    if ( viewport && yoffset_before != yoffset )
      viewport->has_changes = true;

    copy2area();
  }

  vbar->setValue (yoffset);

  if ( vbar->isVisible() && yoffset_before != yoffset )
    vbar->drawBar();

  updateTerminal();
}


// protected methods of FScrollView
//----------------------------------------------------------------------
FVTerm::term_area* FScrollView::getPrintArea()
{
  // returns the viewport print area

  if ( viewport && ! own_print_area )
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

  hbar->setMaximum (scroll_size.getWidth() - getClientWidth());
  hbar->setPageSize (scroll_size.getWidth(), getClientWidth());
  hbar->setY (height);
  hbar->setWidth (width - 2, false);
  hbar->setValue (xoffset);
  hbar->resize();

  vbar->setMaximum (scroll_size.getHeight() - getClientHeight());
  vbar->setPageSize (scroll_size.getHeight(), getClientHeight());
  vbar->setX (width);
  vbar->setHeight (height - 2, false);
  vbar->setValue (yoffset);
  vbar->resize();
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
void FScrollView::cb_VBarChange (FWidget*, void*)
{
  FScrollbar::sType scrollType;
  bool hasChanges = false;
  int  distance = 1;
  int  yoffset = scroll_offset.getY();
  int  yoffset_before = yoffset;
  int  yoffset_end = scroll_size.getHeight() - getClientHeight();
  scrollType = vbar->getScrollType();

  switch ( scrollType )
  {
    case FScrollbar::noScroll:
      break;

    case FScrollbar::scrollPageBackward:
      distance = getClientHeight();
      // fall through
    case FScrollbar::scrollStepBackward:
      yoffset -= distance;

      if ( yoffset < 0 )
        yoffset = 0;

      break;

    case FScrollbar::scrollPageForward:
      distance = getClientHeight();
      // fall through
    case FScrollbar::scrollStepForward:
      yoffset += distance;

      if ( yoffset > yoffset_end )
        yoffset = yoffset_end;

      break;

    case FScrollbar::scrollJump:
    {
      int val = vbar->getValue();

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

  scroll_offset.setY (yoffset);

  if ( isVisible() )
  {
    if ( viewport && yoffset_before != yoffset )
      viewport->has_changes = true;

    copy2area();
    hasChanges = true;
  }

  if ( scrollType >= FScrollbar::scrollStepBackward
      && scrollType <= FScrollbar::scrollWheelDown )
  {
    vbar->setValue (yoffset);
    hasChanges = true;

    if ( vbar->isVisible() && yoffset_before != yoffset )
      vbar->drawBar();
  }

  if ( hasChanges )
    updateTerminal();
}

//----------------------------------------------------------------------
void FScrollView::cb_HBarChange (FWidget*, void*)
{
  FScrollbar::sType scrollType;
  bool hasChanges = false;
  int  distance = 1;
  int  xoffset = scroll_offset.getX();
  int  xoffset_before = xoffset;
  int  xoffset_end = scroll_size.getWidth() - getClientWidth();
  scrollType = hbar->getScrollType();

  switch ( scrollType )
  {
    case FScrollbar::noScroll:
      break;

    case FScrollbar::scrollPageBackward:
      distance = getClientWidth();
      // fall through
    case FScrollbar::scrollStepBackward:
      xoffset -= distance;

      if ( xoffset < 0 )
        xoffset = 0;

      break;

    case FScrollbar::scrollPageForward:
      distance = getClientWidth();
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
      int val = hbar->getValue();

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

  scroll_offset.setX (xoffset);

  if ( isVisible() )
  {
    if ( viewport && xoffset_before != xoffset )
      viewport->has_changes = true;

    copy2area();
    hasChanges = true;
  }

  if ( scrollType >= FScrollbar::scrollStepBackward
      && scrollType <= FScrollbar::scrollWheelDown )
  {
    hbar->setValue (xoffset);
    hasChanges = true;

    if ( hbar->isVisible() && xoffset_before != xoffset )
      hbar->drawBar();
  }

  if ( hasChanges )
    updateTerminal();
}
