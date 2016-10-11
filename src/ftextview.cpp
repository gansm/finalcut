// File: ftextview.cpp
// Provides: class FTextView

#include "fdialog.h"
#include "fstatusbar.h"
#include "ftextview.h"


//----------------------------------------------------------------------
// class FTextView
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FTextView::FTextView(FWidget* parent)
  : FWidget(parent)
  , data()
  , vbar(0)
  , hbar(0)
  , xoffset(0)
  , yoffset(0)
  , nf_offset(0)
  , maxLineWidth(0)
{
  init();
}

//----------------------------------------------------------------------
FTextView::~FTextView()  // destructor
{
  delete vbar;
  delete hbar;
}

// private methods of FTextView
//----------------------------------------------------------------------
void FTextView::init()
{
  nf_offset = isNewFont() ? 1 : 0;

  setForegroundColor (wc.dialog_fg);
  setBackgroundColor (wc.dialog_bg);

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
    _METHOD_CALLBACK (this, &FTextView::cb_VBarChange)
  );
  hbar->addCallback
  (
    "change-value",
    _METHOD_CALLBACK (this, &FTextView::cb_HBarChange)
  );
}

//----------------------------------------------------------------------
void FTextView::draw()
{
  FWidget* parent = getParentWidget();
  bool is_text_dialog;
  updateVTerm(false);
  setColor();

  if ( isMonochron() )
    setReverse(true);

  if ( parent
     && parent->isDialogWidget()
     && isPaddingIgnored()
     && getGeometry() == FRect ( 1
                               , 2
                               , parent->getWidth()
                               , parent->getHeight()-1) )
  {
    is_text_dialog = true;
  }
  else
    is_text_dialog = false;

  if ( ! (is_text_dialog || isNewFont()) )
    drawBorder();

  if ( isMonochron() )
    setReverse(false);

  if ( vbar->isVisible() )
    vbar->redraw();

  if ( hbar->isVisible() )
    hbar->redraw();

  updateVTerm(true);
  drawText();

  if ( hasFocus() && statusBar() )
  {
    FString msg = getStatusbarMessage();
    FString curMsg = statusBar()->getMessage();

    if ( curMsg != msg )
    {
      updateVTerm(false);
      statusBar()->setMessage(msg);
      statusBar()->drawMessage();
      updateVTerm(true);
    }
  }

  setCursorPos (getWidth(), getHeight());
  updateTerminal();
  flush_out();
}

//----------------------------------------------------------------------
void FTextView::drawText()
{
  uInt start, end;

  if ( data.empty() || getHeight() <= 2 || getWidth() <= 2 )
    return;

  start = 0;
  end = uInt(getHeight() + nf_offset - 2);

  if ( end > getRows() )
    end = getRows();

  updateVTerm(false);
  setColor();

  if ( isMonochron() )
    setReverse(true);

  for (uInt y=start; y < end; y++)
  {
    uInt i, len;
    FString line;
    const wchar_t* line_str;
    setPrintPos (2, 2 - nf_offset + int(y));
    line = data[y+uInt(yoffset)].mid ( uInt(1 + xoffset)
                                     , uInt(getWidth() - nf_offset - 2) );
    line_str = line.wc_str();
    len = line.getLength();

    for (i=0; i < len; i++)
    {
      wchar_t ch = line_str[i];
      bool utf8 = (Encoding == fc::UTF8) ? true : false;

      // only printable and 1 column per character
      if (  (  (utf8 && std::iswprint(wint_t(ch)))
            || (!utf8 && ch < 256 && std::isprint(ch)) )
         && wcwidth(ch) == 1 )
      {
        print (ch);
      }
      else
        print ('.');
    }

    for (; i < uInt(getWidth() - nf_offset - 2); i++)
      print (' ');
  }

  if ( isMonochron() )
    setReverse(false);

  updateVTerm(true);
}

//----------------------------------------------------------------------
void FTextView::processChanged()
{
  emitCallback("changed");
}

// protected methods of FTextView
//----------------------------------------------------------------------
void FTextView::adjustSize()
{
  FWidget::adjustSize();
  int width     = getWidth();
  int height    = getHeight();
  int last_line = int(getRows());
  int max_width = int(maxLineWidth);

  if ( xoffset >= max_width - width - nf_offset )
    xoffset = max_width - width - nf_offset - 1;

  if ( xoffset < 0 )
    xoffset = 0;

  if ( yoffset > last_line - height - nf_offset + 2 )
    yoffset = last_line - height - nf_offset + 2;

  if ( yoffset < 0 )
    yoffset = 0;

  vbar->setMaximum (last_line - height + 2 - nf_offset);
  vbar->setPageSize (last_line, height - 2 + nf_offset);
  vbar->setX (width);
  vbar->setHeight (height - 2 + nf_offset, false);
  vbar->setValue (yoffset);
  vbar->resize();

  hbar->setMaximum (max_width - width + nf_offset + 2);
  hbar->setPageSize (max_width, width - nf_offset - 2);
  hbar->setY (height);
  hbar->setWidth (width - 2, false);
  hbar->setValue (xoffset);
  hbar->resize();

  if ( last_line < height + nf_offset - 1 )
    vbar->hide();
  else
    vbar->setVisible();

  if ( max_width < width - nf_offset - 1 )
    hbar->hide();
  else
    hbar->setVisible();
}


// public methods of FTextView
//----------------------------------------------------------------------
void FTextView::hide()
{
  int n, size;
  short fg, bg;
  char* blank;
  FWidget* parent_widget = getParentWidget();

  FWidget::hide();

  if ( parent_widget )
  {
    fg = parent_widget->getForegroundColor();
    bg = parent_widget->getBackgroundColor();
  }
  else
  {
    fg = wc.dialog_fg;
    bg = wc.dialog_bg;
  }

  setColor (fg, bg);
  n = isNewFont() ? 1 : 0;
  size = getWidth() + n;

  if ( size < 0 )
    return;

  blank = new char[size+1];
  std::memset(blank, ' ', uLong(size));
  blank[size] = '\0';

  for (int y=0; y < getHeight(); y++)
  {
    setPrintPos (1, 1 + y);
    print (blank);
  }

  delete[] blank;
  flush_out();
}

//----------------------------------------------------------------------
void FTextView::onKeyPress (FKeyEvent* ev)
{
  int last_line = int(getRows());
  int key = ev->key();

  switch ( key )
  {
    case fc::Fkey_up:
      if ( yoffset > 0 )
        yoffset--;

      ev->accept();
      break;

    case fc::Fkey_down:
      if ( yoffset + getHeight() + nf_offset <= last_line + 1 )
        yoffset++;

      ev->accept();
      break;

    case fc::Fkey_right:
      if ( xoffset + getWidth() - nf_offset <= int(maxLineWidth) + 1 )
        xoffset++;

      ev->accept();
      break;

    case fc::Fkey_left:
      if ( xoffset > 0 )
        xoffset--;

      ev->accept();
      break;

    case fc::Fkey_ppage:
      yoffset -= getHeight()-2;

      if ( yoffset < 0 )
        yoffset = 0;

      ev->accept();
      break;

    case fc::Fkey_npage:
      if ( last_line >= getHeight() )
        yoffset += getHeight()-2;

      if ( yoffset > last_line - getHeight() - nf_offset + 2 )
        yoffset = last_line - getHeight() - nf_offset + 2;

      if ( yoffset < 0 )
        yoffset = 0;

      ev->accept();
      break;

    case fc::Fkey_home:
      yoffset = 0;
      ev->accept();
      break;

    case fc::Fkey_end:
      if ( last_line >= getHeight() )
        yoffset = last_line - getHeight() - nf_offset + 2;

      ev->accept();
      break;

    default:
      break;
  }

  if ( ev->isAccepted() )
  {
    if ( isVisible() )
      drawText();

    vbar->setValue (yoffset);

    if ( vbar->isVisible() )
      vbar->drawBar();

    hbar->setValue (xoffset);

    if ( hbar->isVisible() )
      hbar->drawBar();

    updateTerminal();
    flush_out();
  }
}

//----------------------------------------------------------------------
void FTextView::onMouseDown (FMouseEvent* ev)
{
  FWidget* parent;
  FDialog* dialog;

  if ( ev->getButton() != fc::LeftButton )
    return;

  if ( ! hasFocus() )
  {
    FWidget* focused_widget = getFocusWidget();
    FFocusEvent out (fc::FocusOut_Event);
    FApplication::queueEvent(focused_widget, &out);
    setFocus();

    if ( focused_widget )
      focused_widget->redraw();

    if ( statusBar() )
      statusBar()->drawMessage();
  }

  parent = getParentWidget();

  if ( parent
     && parent->isDialogWidget()
     && (dialog = static_cast<FDialog*>(parent)) != 0
     && dialog->isResizeable()
     && ! dialog->isZoomed() )
  {
    int b = ev->getButton();
    const FPoint& tp = ev->getTermPos();
    const FPoint& p = parent->termToWidgetPos(tp);
    parent->setFocus();
    FMouseEvent* _ev = new FMouseEvent (fc::MouseDown_Event, p, tp, b);
    FApplication::sendEvent (parent, _ev);
    delete _ev;
  }
}

//----------------------------------------------------------------------
void FTextView::onMouseUp (FMouseEvent* ev)
{
  FWidget* parent = getParentWidget();
  FDialog* dialog;

  if ( parent
     && parent->isDialogWidget()
     && (dialog = static_cast<FDialog*>(parent)) != 0
     && dialog->isResizeable()
     && ! dialog->isZoomed() )
  {
    int b = ev->getButton();
    const FPoint& tp = ev->getTermPos();
    const FPoint& p = parent->termToWidgetPos(tp);
    parent->setFocus();
    FMouseEvent* _ev = new FMouseEvent (fc::MouseUp_Event, p, tp, b);
    FApplication::sendEvent (parent, _ev);
    delete _ev;
  }

  if ( vbar->isVisible() )
    vbar->redraw();

  if ( hbar->isVisible() )
    hbar->redraw();
}

//----------------------------------------------------------------------
void FTextView::onMouseMove (FMouseEvent* ev)
{
  FWidget* parent = getParentWidget();
  FDialog* dialog;

  if ( parent
     && parent->isDialogWidget()
     && (dialog = static_cast<FDialog*>(parent)) != 0
     && dialog->isResizeable()
     && ! dialog->isZoomed() )
  {
    int b = ev->getButton();
    const FPoint& tp = ev->getTermPos();
    const FPoint& p = parent->termToWidgetPos(tp);
    parent->setFocus();
    FMouseEvent* _ev = new FMouseEvent (fc::MouseMove_Event, p, tp, b);
    FApplication::sendEvent (parent, _ev);
    delete _ev;
  }
}

//----------------------------------------------------------------------
void FTextView::onWheel (FWheelEvent* ev)
{
  int last_line = int(getRows());
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
        int yoffset_end = last_line - getHeight() - nf_offset + 2;

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

  if ( isVisible() )
    drawText();

  vbar->setValue (yoffset);

  if ( vbar->isVisible() )
    vbar->drawBar();

  hbar->setValue (xoffset);

  if ( hbar->isVisible() )
    hbar->drawBar();

  updateTerminal();
}

//----------------------------------------------------------------------
void FTextView::onFocusIn (FFocusEvent*)
{
  if ( statusBar() )
    statusBar()->drawMessage();
}

//----------------------------------------------------------------------
void FTextView::onFocusOut (FFocusEvent*)
{
  if ( statusBar() )
  {
    statusBar()->clearMessage();
    statusBar()->drawMessage();
  }
}

//----------------------------------------------------------------------
void FTextView::cb_VBarChange (FWidget*, void*)
{
  FScrollbar::sType scrollType;
  int distance = 1;
  int last_line = int(getRows());
  int yoffset_before = yoffset;
  scrollType = vbar->getScrollType();

  switch ( int(scrollType) )
  {
    case FScrollbar::scrollPageBackward:
      distance = getHeight() + nf_offset - 2;
      // fall through
    case FScrollbar::scrollStepBackward:
      yoffset -= distance;

      if ( yoffset < 0 )
        yoffset = 0;

      break;

    case FScrollbar::scrollPageForward:
      distance = getHeight() + nf_offset - 2;
      // fall through
    case FScrollbar::scrollStepForward:
      yoffset += distance;

      if ( yoffset > last_line - getHeight() - nf_offset + 2 )
        yoffset = last_line - getHeight() - nf_offset + 2;

      break;

    case FScrollbar::scrollJump:
    {
      int val = vbar->getValue();

      if ( yoffset == val )
        break;

      yoffset = val;

      if ( yoffset > last_line - getHeight() - nf_offset + 2 )
        yoffset = last_line - getHeight() - nf_offset + 2;

      if ( yoffset < 0 )
        yoffset = 0;

      break;
    }

    case FScrollbar::scrollWheelUp:
    {
      FWheelEvent wheel_ev (fc::MouseWheel_Event, FPoint(2,2), fc::WheelUp);
      onWheel(&wheel_ev);
      break;
    }

    case FScrollbar::scrollWheelDown:
    {
      FWheelEvent wheel_ev (fc::MouseWheel_Event, FPoint(2,2), fc::WheelDown);
      onWheel(&wheel_ev);
      break;
    }

    default:
      break;
  }

  if ( isVisible() )
  {
    drawText();
    updateTerminal();
  }

  if (  scrollType >= FScrollbar::scrollStepBackward
     && scrollType <= FScrollbar::scrollPageForward )
  {
    vbar->setValue (yoffset);

    if ( vbar->isVisible() && yoffset_before != yoffset )
      vbar->drawBar();

    updateTerminal();
  }
}

//----------------------------------------------------------------------
void FTextView::cb_HBarChange (FWidget*, void*)
{
  int distance = 1;
  int xoffset_before = xoffset;
  int xoffset_end = int(maxLineWidth) - getWidth() + nf_offset + 4;
  int scrollType = hbar->getScrollType();

  switch ( scrollType )
  {
    case FScrollbar::scrollPageBackward:
      distance = getWidth() - nf_offset - 4;
      // fall through
    case FScrollbar::scrollStepBackward:
      xoffset -= distance;

      if ( xoffset < 0 )
        xoffset = 0;

      break;

    case FScrollbar::scrollPageForward:
      distance = getWidth() - nf_offset - 4;
      // fall through
    case FScrollbar::scrollStepForward:
      xoffset += distance;

      if ( xoffset > int(maxLineWidth) - getWidth() + nf_offset + 4 )
        xoffset = int(maxLineWidth) - getWidth() + nf_offset + 4;

      if ( xoffset < 0 )
        xoffset = 0;

      break;

    case FScrollbar::scrollJump:
    {
      int val = hbar->getValue();

      if ( xoffset == val )
        break;

      xoffset = val;

      if ( xoffset > int(maxLineWidth) - getWidth() + nf_offset + 4 )
        xoffset = int(maxLineWidth) - getWidth() + nf_offset + 4;

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

    default:
      break;
  }

  if ( isVisible() )
  {
    drawText();
    updateTerminal();
  }

  if (  scrollType >= FScrollbar::scrollStepBackward
     && scrollType <= FScrollbar::scrollWheelDown )
  {
    hbar->setValue (xoffset);

    if ( hbar->isVisible() && xoffset_before != xoffset )
      hbar->drawBar();

    updateTerminal();
  }
}

//----------------------------------------------------------------------
void FTextView::setGeometry (int x, int y, int w, int h, bool adjust)
{
  FWidget::setGeometry(x, y, w, h, adjust);
  int width  = getWidth();
  int height = getHeight();

  if ( isNewFont() )
  {
    vbar->setGeometry (width, 1, 2, height-1);
    hbar->setGeometry (1, height, width-2, 1);
  }
  else
  {
    vbar->setGeometry (width, 2, 1, height-2);
    hbar->setGeometry (2, height, width-2, 1);
  }

  vbar->resize();
  hbar->resize();
}

//----------------------------------------------------------------------
void FTextView::setPosition (int pos)
{
  int last_line = int(getRows());

  if ( pos < 0 || pos > last_line - getHeight() + 2 )
    return;

  yoffset = pos;

  if ( isVisible() )
    drawText();

  vbar->setValue (yoffset);

  if ( vbar->isVisible() )
    vbar->drawBar();

  flush_out();
}

//----------------------------------------------------------------------
void FTextView::setText (const FString& str)
{
  clear();
  insert(str, -1);
}

//----------------------------------------------------------------------
FString FTextView::getText() const
{
  uInt len, rows, idx;

  if ( data.empty() )
    return FString("");

  len = 0;
  rows = getRows();

  for (uInt i=0 ; i < rows; i++)
    len += data[i].getLength() + 1;

  FString s(len + 1);
  idx = 0;

  for (uInt i=0 ; i < rows; i++)
  {
    const wchar_t* p = data[i].wc_str();

    if ( p )
    {
      while ( (s[idx++] = *p++) != 0 );
      s[idx-1] = '\n';
    }
    else
    {
      s[idx++] = '\n';
    }
  }

  s[idx-1] = 0;
  return s;
}

//----------------------------------------------------------------------
void FTextView::append (const FString& str)
{
  insert(str, -1);
}

//----------------------------------------------------------------------
void FTextView::insert (const FString& str, int pos)
{
  stringLines::iterator iter;
  stringLines text_split;
  FString s;
  uLong end;

  if ( pos < 0 || pos >= int(getRows()) )
    pos = int(getRows());

  if ( str.isEmpty() )
    s = "\n";
  else
    s = FString(str).rtrim().expandTabs(tabstop);

  iter = data.begin();
  text_split = s.split("\r\n");
  end = text_split.size();

  for (uInt i=0; i < end; i++)
  {
    uInt len;
    text_split[i] = text_split[i].removeBackspaces()
                                 .removeDel()
                                 .replaceControlCodes()
                                 .rtrim();
    len = text_split[i].getLength();

    if ( len > maxLineWidth )
    {
      maxLineWidth = len;

      if ( len > uInt(getWidth() - nf_offset - 2) )
      {
        hbar->setMaximum (int(maxLineWidth) - getWidth() + nf_offset + 2);
        hbar->setPageSize (int(maxLineWidth), getWidth() - nf_offset - 2);
        hbar->calculateSliderValues();

        if ( ! hbar->isVisible() )
          hbar->setVisible();
      }
    }
  }

  data.insert (iter + pos, text_split.begin(), text_split.end());
  vbar->setMaximum (int(getRows()) - getHeight() + 2 - nf_offset);
  vbar->setPageSize (int(getRows()), getHeight() - 2 + nf_offset);
  vbar->calculateSliderValues();

  if ( ! vbar->isVisible() && int(getRows()) >= getHeight() + nf_offset - 1 )
    vbar->setVisible();

  if ( vbar->isVisible() && int(getRows()) < getHeight() + nf_offset - 1 )
    vbar->hide();

  processChanged();
}

//----------------------------------------------------------------------
void FTextView::replaceRange (const FString& str, int start, int end)
{
  stringLines::iterator iter;

  if ( start > end )
    return;

  if ( start < 0 || start >= int(getRows()) )
    return;

  if ( end < 0 || end >= int(getRows()) )
    return;

  iter = data.begin();
  data.erase (iter+start, iter+end+1);

  if ( ! str.isNull() )
    insert(str, start);
}

//----------------------------------------------------------------------
void FTextView::clear()
{
  int size;
  char* blank;

  data.clear();
  xoffset = 0;
  yoffset = 0;
  maxLineWidth = 0;

  vbar->setMinimum(0);
  vbar->setValue(0);
  vbar->hide();

  hbar->setMinimum(0);
  hbar->setValue(0);
  hbar->hide();

  // clear list from screen
  setColor();
  size = getWidth() - 2;

  if ( size < 0 )
    return;

  blank = new char[size+1];
  std::memset(blank, ' ', uLong(size));
  blank[size] = '\0';

  for (int y=0; y < getHeight() + nf_offset - 2; y++)
  {
    setPrintPos (2, 2 - nf_offset + y);
    print (blank);
  }

  delete[] blank;
  processChanged();
}
