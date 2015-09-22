// ftextview.cpp
// class FTextView

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
  , VBar(0)
  , HBar(0)
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
  delete VBar;
  delete HBar;
}

// private methods of FTextView
//----------------------------------------------------------------------
void FTextView::init()
{
  nf_offset = isNewFont() ? 1 : 0;

  foregroundColor = wc.dialog_fg;
  backgroundColor = wc.dialog_bg;

  VBar = new FScrollbar(fc::vertical, this);
  VBar->setMinimum(0);
  VBar->setValue(0);
  VBar->hide();

  HBar = new FScrollbar(fc::horizontal, this);
  HBar->setMinimum(0);
  HBar->setValue(0);
  HBar->hide();

  VBar->addCallback
  (
    "change-value",
    this,
    reinterpret_cast<FWidget::FMemberCallback>(&FTextView::cb_VBarChange),
    null
  );
  HBar->addCallback
  (
    "change-value",
    this,
    reinterpret_cast<FWidget::FMemberCallback>(&FTextView::cb_HBarChange),
    null
  );
}

//----------------------------------------------------------------------
void FTextView::draw()
{
  setUpdateVTerm(false);
  setColor (foregroundColor, backgroundColor);
  if ( ! isNewFont() )
    drawBorder();
  setUpdateVTerm(true);

  if ( VBar->isVisible() )
    VBar->redraw();
  if ( HBar->isVisible() )
    HBar->redraw();

  drawText();

  if ( hasFocus() && statusBar() )
  {
    FString msg = getStatusbarMessage();
    FString curMsg = statusBar()->getMessage();
    if ( curMsg != msg )
    {
      statusBar()->setMessage(msg);
      statusBar()->drawMessage();
    }
  }
}

//----------------------------------------------------------------------
void FTextView::drawText()
{
  uInt start, end;

  if ( data.empty() || height < 4 || width < 5 )
    return;
  start = 0;
  end = uInt(height+nf_offset-2);
  if ( end > getRows() )
    end = getRows();

  setUpdateVTerm(false);
  setColor (foregroundColor, backgroundColor);
  for (uInt y=start; y < end; y++)
  {
    gotoxy (xpos+xmin, ypos+ymin-nf_offset+int(y));
    uInt i;
    FString line = data[y+uInt(yoffset)].mid ( uInt(1+xoffset)
                                             , uInt(width-nf_offset-2) );
    const wchar_t* line_str = line.wc_str();
    uInt len = line.getLength();

    for (i=0; i < len; i++)
      if ( wcwidth(line_str[i]) == 1 )  // only 1 column per character
        print (line_str[i]);
      else
        print ('.');
    for (; i < uInt(width-nf_offset-2); i++)
      print (' ');
  }
  setUpdateVTerm(true);
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

  VBar->setMaximum(int(getRows())-height+2-nf_offset);
  VBar->setPageSize(int(getRows()), height-2+nf_offset);
  VBar->setX(width);
  VBar->setHeight (height-2+nf_offset, false);
  VBar->resize();

  HBar->setMaximum(int(maxLineWidth)-width+nf_offset+2);
  HBar->setPageSize(int(maxLineWidth), width-nf_offset-2);
  HBar->setY(height);
  HBar->setWidth (width-2, false);
  HBar->resize();

  if ( int(getRows()) <= height+nf_offset-2 )
    VBar->hide();
  else
    VBar->setVisible();

  if ( int(maxLineWidth) <= width-nf_offset-2 )
    HBar->hide();
  else
    HBar->setVisible();
}


// public methods of FTextView
//----------------------------------------------------------------------
void FTextView::hide()
{
  int fg, bg, n, size;
  char* blank;

  FWidget::hide();

  fg = parentWidget()->getForegroundColor();
  bg = parentWidget()->getBackgroundColor();
  setColor (fg, bg);

  n = isNewFont() ? 1 : 0;
  size = width + n;
  blank = new char[size+1];
  memset(blank, ' ', uLong(size));
  blank[size] = '\0';

  for (int y=0; y < height; y++)
  {
    gotoxy (xpos+xmin-1, ypos+ymin-1+y);
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
      if ( yoffset+height+nf_offset-2 < last_line )
        yoffset++;
      ev->accept();
      break;

    case fc::Fkey_right:
      if ( xoffset+width-nf_offset-2 < int(maxLineWidth) )
        xoffset++;
      ev->accept();
      break;

    case fc::Fkey_left:
      if ( xoffset > 0 )
        xoffset--;
      ev->accept();
      break;

    case fc::Fkey_ppage:
      yoffset -= height-2;
      if ( yoffset < 0 )
        yoffset = 0;
      ev->accept();
      break;

    case fc::Fkey_npage:
      if ( last_line >= height )
        yoffset += height-2;
      if ( yoffset > last_line - height - nf_offset + 2 )
        yoffset = last_line - height - nf_offset + 2;
      if ( yoffset < 0 )
        yoffset = 0;
      ev->accept();
      break;

    case fc::Fkey_home:
      yoffset = 0;
      ev->accept();
      break;

    case fc::Fkey_end:
      if ( last_line >= height )
        yoffset = last_line - height - nf_offset + 2;
      ev->accept();
      break;

    default:
      break;
  }

  if ( ev->isAccepted() )
  {
    if ( isVisible() )
      drawText();

    VBar->setValue (yoffset);
    if ( VBar->isVisible() )
      VBar->drawBar();
    HBar->setValue (xoffset);
    if ( HBar->isVisible() )
      HBar->drawBar();
    updateTerminal();
  }
}

//----------------------------------------------------------------------
void FTextView::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != LeftButton )
    return;

  if ( ! hasFocus() )
  {
    FWidget* focused_widget = getFocusWidget();
    FFocusEvent out (FocusOut_Event);
    FApplication::queueEvent(focused_widget, &out);
    setFocus();
    if ( focused_widget )
      focused_widget->redraw();
    if ( statusBar() )
      statusBar()->drawMessage();
  }
}

//----------------------------------------------------------------------
void FTextView::onWheel (FWheelEvent* ev)
{
  int last_line = int(getRows());
  int wheel = ev->getWheel();

  switch ( wheel )
  {
    case WheelUp:
      if ( yoffset == 0 )
        break;
      yoffset -= 4;
      if ( yoffset < 0 )
        yoffset=0;
      break;

    case WheelDown:
      {
        int yoffset_end = last_line - height - nf_offset + 2;
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

  VBar->setValue (yoffset);
  if ( VBar->isVisible() )
    VBar->drawBar();
  HBar->setValue (xoffset);
  if ( HBar->isVisible() )
    HBar->drawBar();
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
  int distance = 1;
  int last_line = int(getRows());
  int yoffset_before = yoffset;
  int scrollType = VBar->getScrollType();

  switch ( scrollType )
  {
    case FScrollbar::scrollPageBackward:
      distance = height+nf_offset-2;
    case FScrollbar::scrollStepBackward:
      yoffset -= distance;
      if ( yoffset < 0 )
        yoffset = 0;
      break;

    case FScrollbar::scrollPageForward:
      distance = height+nf_offset-2;
    case FScrollbar::scrollStepForward:
      yoffset += distance;
      if ( yoffset > last_line - height - nf_offset + 2 )
        yoffset = last_line - height - nf_offset + 2;
      break;

    case FScrollbar::scrollJump:
    {
      int val = VBar->getValue();
      if ( yoffset == val )
        break;
      yoffset = val;
      if ( yoffset > last_line - height - nf_offset + 2 )
        yoffset = last_line - height - nf_offset + 2;
      if ( yoffset < 0 )
        yoffset = 0;
      break;
    }

    case FScrollbar::scrollWheelUp:
    {
      FWheelEvent wheel_ev (MouseWheel_Event, FPoint(2,2), WheelUp);
      onWheel(&wheel_ev);
      break;
    }

    case FScrollbar::scrollWheelDown:
    {
      FWheelEvent wheel_ev (MouseWheel_Event, FPoint(2,2), WheelDown);
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
    VBar->setValue (yoffset);
    if ( VBar->isVisible() && yoffset_before != yoffset )
      VBar->drawBar();
    updateTerminal();
  }
}

//----------------------------------------------------------------------
void FTextView::cb_HBarChange (FWidget*, void*)
{
  int distance = 1;
  int xoffset_before = xoffset;
  int xoffset_end = int(maxLineWidth)-width+nf_offset+4;
  int scrollType = HBar->getScrollType();

  switch ( scrollType )
  {
    case FScrollbar::scrollPageBackward:
      distance = width-nf_offset-4;
    case FScrollbar::scrollStepBackward:
      xoffset -= distance;
      if ( xoffset < 0 )
        xoffset = 0;
      break;

    case FScrollbar::scrollPageForward:
      distance = width-nf_offset-4;
    case FScrollbar::scrollStepForward:
      xoffset += distance;
      if ( xoffset > int(maxLineWidth)-width+nf_offset+4 )
        xoffset = int(maxLineWidth)-width+nf_offset+4;
      if ( xoffset < 0 )
        xoffset = 0;
      break;

    case FScrollbar::scrollJump:
    {
      int val = HBar->getValue();
      if ( xoffset == val )
        break;
      xoffset = val;
      if ( xoffset > int(maxLineWidth)-width+nf_offset+4 )
        xoffset = int(maxLineWidth)-width+nf_offset+4;
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
    HBar->setValue (xoffset);
    if ( HBar->isVisible() && xoffset_before != xoffset )
      HBar->drawBar();
    updateTerminal();
  }
}

//----------------------------------------------------------------------
void FTextView::setGeometry (int x, int y, int w, int h, bool adjust)
{
  FWidget::setGeometry(x, y, w, h, adjust);

  if ( isNewFont() )
  {
    VBar->setGeometry(width, 1, 2, height-1);
    HBar->setGeometry(1, height, width-2, 1);
  }
  else
  {
    VBar->setGeometry(width, 2, 1, height-2);
    HBar->setGeometry(2, height, width-2, 1);
  }
}

//----------------------------------------------------------------------
void FTextView::setPosition (int pos)
{
  int last_line = int(getRows());
  if ( pos < 0 || pos > last_line - height + 2 )
    return;

  yoffset = pos;

  if ( isVisible() )
    drawText();

  VBar->setValue (yoffset);
  if ( VBar->isVisible() )
    VBar->drawBar();

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
      if ( len > uInt(width-nf_offset-2) )
      {
        HBar->setMaximum(int(maxLineWidth)-width+nf_offset+2);
        HBar->setPageSize(int(maxLineWidth), width-nf_offset-2);
        HBar->calculateSliderValues();
        if ( ! HBar->isVisible() )
          HBar->setVisible();
      }
    }
  }
  data.insert (iter+pos, text_split.begin(), text_split.end());

  VBar->setMaximum(int(getRows())-height+2-nf_offset);
  VBar->setPageSize(int(getRows()), height-2+nf_offset);
  VBar->calculateSliderValues();
  if ( ! VBar->isVisible() && int(getRows()) > height+nf_offset-2 )
    VBar->setVisible();
  if ( VBar->isVisible() && int(getRows()) <= height+nf_offset-2 )
    VBar->hide();
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

  VBar->setMinimum(0);
  VBar->setValue(0);
  VBar->hide();

  HBar->setMinimum(0);
  HBar->setValue(0);
  HBar->hide();

  // clear list from screen
  setColor (foregroundColor, backgroundColor);
  size = width - 2;
  blank = new char[size+1];
  memset(blank, ' ', uLong(size));
  blank[size] = '\0';

  for (int y=0; y < height+nf_offset-2; y++)
  {
    gotoxy (xpos+xmin, ypos+ymin-nf_offset+y);
    print (blank);
  }
  delete[] blank;
  processChanged();
}
