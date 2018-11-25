/***********************************************************************
* ftextview.cpp - Widget FTextView (a multiline text viewer)           *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2014-2018 Markus Gans                                      *
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

#include "final/fdialog.h"
#include "final/fstatusbar.h"
#include "final/ftextview.h"

namespace finalcut
{

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
  , update_scrollbar(true)
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


// public methods of FTextView
//----------------------------------------------------------------------
const FString FTextView::getText() const
{
  std::size_t len, rows, idx;

  if ( data.empty() )
    return FString("");

  len = 0;
  rows = getRows();

  for (std::size_t i = 0 ; i < rows; i++)
    len += data[i].getLength() + 1;

  FString s(len + 1);
  idx = 0;

  for (std::size_t i = 0 ; i < rows; i++)
  {
    const wchar_t* p = data[i].wc_str();

    if ( p )
    {
      while ( (s[idx++] = *p++) != 0 );
      s[idx - 1] = '\n';
    }
    else
    {
      s[idx++] = '\n';
    }
  }

  s[idx - 1] = 0;
  return s;
}

//----------------------------------------------------------------------
void FTextView::setGeometry ( int x, int y
                            , std::size_t w, std::size_t h
                            , bool adjust )
{
  // Set the text view geometry

  FWidget::setGeometry(x, y, w, h, adjust);
  std::size_t width  = getWidth();
  std::size_t height = getHeight();

  if ( isNewFont() )
  {
    vbar->setGeometry (int(width), 1, 2, height - 1);
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
void FTextView::setText (const FString& str)
{
  clear();
  insert(str, -1);
}

//----------------------------------------------------------------------
void FTextView::scrollToX (int x)
{
  scrollTo (x, yoffset);
}

//----------------------------------------------------------------------
void FTextView::scrollToY (int y)
{
  scrollTo (xoffset, y);
}

//----------------------------------------------------------------------
void FTextView::scrollBy (int dx, int dy)
{
  scrollTo (xoffset + dx, yoffset + dy);
}

//----------------------------------------------------------------------
void FTextView::scrollTo (int x, int y)
{
  bool changeX = bool(x != xoffset);
  bool changeY = bool(y != yoffset);

  if ( ! isVisible() || ! (changeX || changeY) )
    return;

  if ( xoffset != x )
  {
    int xoffset_end = int(maxLineWidth - getTextWidth());
    xoffset = x;

    if ( xoffset < 0 )
      xoffset = 0;

    if ( xoffset > xoffset_end )
      xoffset = xoffset_end;

    if ( update_scrollbar )
    {
      hbar->setValue (xoffset);
      drawHBar();
    }
  }

  if ( yoffset != y )
  {
    int yoffset_end = int(getRows() - getTextHeight());
    yoffset = y;

    if ( yoffset < 0 )
      yoffset = 0;

    if ( yoffset > yoffset_end )
      yoffset = yoffset_end;

    if ( update_scrollbar )
    {
      vbar->setValue (yoffset);
      drawVBar();
    }
  }

  drawText();
  updateTerminal();
}

//----------------------------------------------------------------------
void FTextView::hide()
{
  std::size_t n, size;
  FColor fg, bg;
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

  if ( size == 0 )
    return;

  char* blank = createBlankArray(size + 1);

  for (std::size_t y = 0; y < getHeight(); y++)
  {
    setPrintPos (1, 1 + int(y));
    print (blank);
  }

  destroyBlankArray (blank);
  flush_out();
}

//----------------------------------------------------------------------
void FTextView::append (const FString& str)
{
  insert(str, -1);
}

//----------------------------------------------------------------------
void FTextView::insert (const FString& str, int pos)
{
  FStringList::iterator iter;
  FStringList text_split;
  FString s;
  std::size_t num;

  if ( pos < 0 || pos >= int(getRows()) )
    pos = int(getRows());

  if ( str.isEmpty() )
    s = "\n";
  else
    s = FString(str).rtrim().expandTabs(getTabstop());

  iter = data.begin();
  text_split = s.split("\r\n");
  num = text_split.size();

  for (std::size_t i = 0; i < num; i++)
  {
    std::size_t len;
    text_split[i] = text_split[i].removeBackspaces()
                                 .removeDel()
                                 .replaceControlCodes()
                                 .rtrim();
    len = text_split[i].getLength();

    if ( len > maxLineWidth )
    {
      maxLineWidth = len;

      if ( len > getTextWidth() )
      {
        hbar->setMaximum (int(maxLineWidth) - int(getTextWidth()));
        hbar->setPageSize (int(maxLineWidth), int(getTextWidth()));
        hbar->calculateSliderValues();

        if ( ! hbar->isVisible() )
          hbar->setVisible();
      }
    }
  }

  data.insert (iter + pos, text_split.begin(), text_split.end());
  vbar->setMaximum (int(getRows()) - int(getTextHeight()));
  vbar->setPageSize (int(getRows()), int(getTextHeight()));
  vbar->calculateSliderValues();

  if ( ! vbar->isVisible() && getRows() > getTextHeight() )
    vbar->setVisible();

  if ( vbar->isVisible() && getRows() <= getTextHeight() )
    vbar->hide();

  processChanged();
}

//----------------------------------------------------------------------
void FTextView::replaceRange (const FString& str, int from, int to)
{
  FStringList::iterator iter;

  if ( from > to || from >= int(getRows()) || to >= int(getRows()) )
    return;

  iter = data.begin();
  data.erase (iter + from, iter + to + 1);

  if ( ! str.isNull() )
    insert(str, from);
}

//----------------------------------------------------------------------
void FTextView::clear()
{
  std::size_t size;
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

  if ( size == 0 )
    return;

  char* blank = createBlankArray(size + 1);

  for (int y = 0; y < int(getTextHeight()); y++)
  {
    setPrintPos (2, 2 - nf_offset + y);
    print (blank);
  }

  destroyBlankArray (blank);
  processChanged();
}

//----------------------------------------------------------------------
void FTextView::onKeyPress (FKeyEvent* ev)
{
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
      scrollBy (0, int(-getTextHeight()));
      ev->accept();
      break;

    case fc::Fkey_npage:
      scrollBy (0, int(getTextHeight()));
      ev->accept();
      break;

    case fc::Fkey_home:
      scrollToY (0);
      ev->accept();
      break;

    case fc::Fkey_end:
      scrollToY (int(getRows() - getTextHeight()));
      ev->accept();
      break;

    default:
      break;
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

    if ( getStatusBar() )
      getStatusBar()->drawMessage();
  }

  parent = getParentWidget();

  if ( ! parent )
    return;

  dialog = static_cast<FDialog*>(parent);

  if ( parent->isDialogWidget()
    && dialog->isResizeable()
    && ! dialog->isZoomed() )
  {
    int b = ev->getButton();
    const FPoint& tp = ev->getTermPos();
    const FPoint& p = parent->termToWidgetPos(tp);
    parent->setFocus();

    try
    {
      FMouseEvent* _ev = new FMouseEvent (fc::MouseDown_Event, p, tp, b);
      FApplication::sendEvent (parent, _ev);
      delete _ev;
    }
    catch (const std::bad_alloc& ex)
    {
      std::cerr << bad_alloc_str << ex.what() << std::endl;
    }
  }
}

//----------------------------------------------------------------------
void FTextView::onMouseUp (FMouseEvent* ev)
{
  FWidget* parent = getParentWidget();

  if ( parent && parent->isDialogWidget() )
  {
    FDialog* dialog = static_cast<FDialog*>(parent);

    if ( dialog->isResizeable() && ! dialog->isZoomed() )
    {
      int b = ev->getButton();
      const FPoint& tp = ev->getTermPos();
      const FPoint& p = parent->termToWidgetPos(tp);
      parent->setFocus();

      try
      {
        FMouseEvent* _ev = new FMouseEvent (fc::MouseUp_Event, p, tp, b);
        FApplication::sendEvent (parent, _ev);
        delete _ev;
      }
      catch (const std::bad_alloc& ex)
      {
        std::cerr << bad_alloc_str << ex.what() << std::endl;
      }
    }
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

  if ( parent && parent->isDialogWidget() )
  {
    FDialog* dialog = static_cast<FDialog*>(parent);

    if ( dialog->isResizeable() && ! dialog->isZoomed() )
    {
      int b = ev->getButton();
      const FPoint& tp = ev->getTermPos();
      const FPoint& p = parent->termToWidgetPos(tp);
      parent->setFocus();

      try
      {
        FMouseEvent* _ev = new FMouseEvent (fc::MouseMove_Event, p, tp, b);
        FApplication::sendEvent (parent, _ev);
        delete _ev;
      }
      catch (const std::bad_alloc& ex)
      {
        std::cerr << bad_alloc_str << ex.what() << std::endl;
      }
    }
  }
}

//----------------------------------------------------------------------
void FTextView::onWheel (FWheelEvent* ev)
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

  if ( isVisible() )
    drawText();

  updateTerminal();
}

//----------------------------------------------------------------------
void FTextView::onFocusIn (FFocusEvent*)
{
  if ( getStatusBar() )
    getStatusBar()->drawMessage();
}

//----------------------------------------------------------------------
void FTextView::onFocusOut (FFocusEvent*)
{
  if ( getStatusBar() )
  {
    getStatusBar()->clearMessage();
    getStatusBar()->drawMessage();
  }
}


// protected methods of FTextView
//----------------------------------------------------------------------
void FTextView::adjustSize()
{
  FWidget::adjustSize();
  std::size_t width = getWidth();
  std::size_t height = getHeight();
  int last_line = int(getRows());
  int max_width = int(maxLineWidth);

  if ( xoffset >= max_width - int(width) - nf_offset )
    xoffset = max_width - int(width) - nf_offset - 1;

  if ( xoffset < 0 )
    xoffset = 0;

  if ( yoffset > last_line - int(height) - nf_offset + 2 )
    yoffset = last_line - int(height) - nf_offset + 2;

  if ( yoffset < 0 )
    yoffset = 0;

  if ( height < 3 )
    return;

  vbar->setMaximum (last_line - int(height) + 2 - nf_offset);
  vbar->setPageSize (last_line, int(height) - 2 + nf_offset);
  vbar->setX (int(width));
  vbar->setHeight (height - 2 + std::size_t(nf_offset), false);
  vbar->setValue (yoffset);
  vbar->resize();

  if ( width < 3 )
    return;

  hbar->setMaximum (max_width - int(width) + nf_offset + 2);
  hbar->setPageSize (max_width, int(width) - nf_offset - 2);
  hbar->setY (int(height));
  hbar->setWidth (width - 2, false);
  hbar->setValue (xoffset);
  hbar->resize();

  if ( last_line < int(height) + nf_offset - 1 )
    vbar->hide();
  else
    vbar->setVisible();

  if ( max_width < int(width) - nf_offset - 1 )
    hbar->hide();
  else
    hbar->setVisible();
}


// private methods of FTextView
//----------------------------------------------------------------------
std::size_t FTextView::getTextHeight()
{
  return getHeight() - 2 + std::size_t(nf_offset);
}

//----------------------------------------------------------------------
std::size_t FTextView::getTextWidth()
{
  return getWidth() - 2 - std::size_t(nf_offset);
}

//----------------------------------------------------------------------
void FTextView::init()
{
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
    std::cerr << bad_alloc_str << ex.what() << std::endl;
    return;
  }

  vbar->addCallback
  (
    "change-value",
    F_METHOD_CALLBACK (this, &FTextView::cb_VBarChange)
  );

  hbar->addCallback
  (
    "change-value",
    F_METHOD_CALLBACK (this, &FTextView::cb_HBarChange)
  );

  nf_offset = isNewFont() ? 1 : 0;
  setTopPadding(1);
  setLeftPadding(1);
  setBottomPadding(1);
  setRightPadding(1 + nf_offset);
}

//----------------------------------------------------------------------
void FTextView::draw()
{
  FWidget* parent = getParentWidget();
  bool is_text_dialog;
  setColor();

  if ( isMonochron() )
    setReverse(true);

  if ( parent
    && parent->isDialogWidget()
    && isPaddingIgnored()
    && getGeometry() == FRect ( 1
                              , 2
                              , parent->getWidth()
                              , parent->getHeight() - 1) )
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

  drawText();

  if ( hasFocus() && getStatusBar() )
  {
    const FString& msg = getStatusbarMessage();
    const FString& curMsg = getStatusBar()->getMessage();

    if ( curMsg != msg )
    {
      getStatusBar()->setMessage(msg);
      getStatusBar()->drawMessage();
    }
  }

  setCursorPos (int(getWidth()), int(getHeight()));
  updateTerminal();
  flush_out();
}

//----------------------------------------------------------------------
void FTextView::drawText()
{
  if ( data.empty() || getHeight() <= 2 || getWidth() <= 2 )
    return;

  std::size_t num = getTextHeight();

  if ( num > getRows() )
    num = getRows();

  setColor();

  if ( isMonochron() )
    setReverse(true);

  for (std::size_t y = 0; y < num; y++)
  {
    std::size_t i, len;
    FString line;
    const wchar_t* line_str;
    setPrintPos (2, 2 - nf_offset + int(y));
    line = data[y + std::size_t(yoffset)].mid ( std::size_t(1 + xoffset)
                                              , getTextWidth() );
    line_str = line.wc_str();
    len = line.getLength();

    for (i = 0; i < len; i++)
    {
      wchar_t ch = line_str[i];
      bool utf8 = ( getEncoding() == fc::UTF8 ) ? true : false;

      // only printable and 1 column per character
      if ( ( (utf8 && std::iswprint(wint_t(ch)))
          || (!utf8 && ch < 256 && std::isprint(ch)) )
          && wcwidth(ch) == 1 )
      {
        print (ch);
      }
      else
        print ('.');
    }

    for (; i < getTextWidth(); i++)
      print (' ');
  }

  if ( isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
void FTextView::processChanged()
{
  emitCallback("changed");
}

//----------------------------------------------------------------------
inline void FTextView::drawHBar()
{
  if ( hbar->isVisible() )
    hbar->drawBar();
}

//----------------------------------------------------------------------
inline void FTextView::drawVBar()
{
  if ( vbar->isVisible() )
    vbar->drawBar();
}

//----------------------------------------------------------------------
void FTextView::cb_VBarChange (FWidget*, data_ptr)
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
      distance = int(getClientHeight());
      // fall through
    case FScrollbar::scrollStepBackward:
      scrollBy (0, -distance);
      break;

    case FScrollbar::scrollPageForward:
      distance = int(getClientHeight());
      // fall through
    case FScrollbar::scrollStepForward:
      scrollBy (0, distance);
      break;

    case FScrollbar::scrollJump:
      scrollToY (vbar->getValue());
      break;

    case FScrollbar::scrollWheelUp:
    {
      scrollBy (0, -wheel_distance);
      break;
    }

    case FScrollbar::scrollWheelDown:
    {
      scrollBy (0, wheel_distance);
      break;
    }
  }

  update_scrollbar = true;
}

//----------------------------------------------------------------------
void FTextView::cb_HBarChange (FWidget*, data_ptr)
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
      distance = int(getClientWidth());
      // fall through
    case FScrollbar::scrollStepBackward:
      scrollBy (-distance, 0);
      break;

    case FScrollbar::scrollPageForward:
      distance = int(getClientWidth());
      // fall through
    case FScrollbar::scrollStepForward:
      scrollBy (distance, 0);
      break;

    case FScrollbar::scrollJump:
      scrollToX (hbar->getValue());
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
