/***********************************************************************
* ftextview.cpp - Widget FTextView (a multiline text viewer)           *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2014-2019 Markus Gans                                      *
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

#include "final/fapplication.h"
#include "final/fc.h"
#include "final/fdialog.h"
#include "final/fevent.h"
#include "final/fstring.h"
#include "final/fscrollbar.h"
#include "final/fstatusbar.h"
#include "final/ftextview.h"
#include "final/fwidgetcolors.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FTextView
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FTextView::FTextView(FWidget* parent)
  : FWidget(parent)
{
  init();
}

//----------------------------------------------------------------------
FTextView::~FTextView()  // destructor
{ }


// public methods of FTextView
//----------------------------------------------------------------------
const FString FTextView::getText() const
{
  if ( data.empty() )
    return FString("");

  std::size_t len{0};

  for (auto&& line : data)
    len += line.getLength() + 1;  // String length + '\n'

  FString s(len);  // Reserves storage
  auto iter = s.begin();

  for (auto&& line : data)
  {
    if ( ! line.isEmpty() )
    {
      if ( iter != s.begin() )
      {
        *iter = '\n';
        ++iter;
      }

      std::copy (line.begin(), line.end(), iter);
      iter += std::distance(line.begin(), line.end());
    }
  }

  return s;
}

//----------------------------------------------------------------------
void FTextView::setGeometry ( const FPoint& pos, const FSize& size
                            , bool adjust)
{
  // Set the text view geometry

  FWidget::setGeometry(pos, size, adjust);
  std::size_t width  = getWidth();
  std::size_t height = getHeight();

  if ( isNewFont() )
  {
    vbar->setGeometry (FPoint(int(width), 1), FSize(2, height - 1));
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
  bool changeX( x != xoffset );
  bool changeY( y != yoffset );

  if ( ! isShown() || ! (changeX || changeY) )
    return;

  if ( changeX && isHorizontallyScrollable() )
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
      hbar->drawBar();
    }
  }

  if ( changeY && isVerticallyScrollable() )
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
      vbar->drawBar();
    }
  }

  drawText();
  updateTerminal();
}

//----------------------------------------------------------------------
void FTextView::hide()
{
  FWidget::hide();
  hideArea (getSize());
}

//----------------------------------------------------------------------
void FTextView::append (const FString& str)
{
  insert(str, -1);
}

//----------------------------------------------------------------------
void FTextView::insert (const FString& str, int pos)
{
  FString s{};

  if ( pos < 0 || pos >= int(getRows()) )
    pos = int(getRows());

  if ( str.isEmpty() )
    s = "\n";
  else
    s = FString(str).rtrim().expandTabs(getTabstop());


  auto text_split = s.split("\r\n");

  for (auto&& line : text_split)  // Line loop
  {
    line = line.removeBackspaces()
               .removeDel()
               .replaceControlCodes()
               .rtrim();
    auto column_width = getColumnWidth(line);

    if ( column_width > maxLineWidth )
    {
      maxLineWidth = column_width;

      if ( column_width > getTextWidth() )
      {
        int hmax = ( maxLineWidth > getTextWidth() )
                   ? int(maxLineWidth) - int(getTextWidth())
                   : 0;
        hbar->setMaximum (hmax);
        hbar->setPageSize (int(maxLineWidth), int(getTextWidth()));
        hbar->calculateSliderValues();

        if ( isShown() && isHorizontallyScrollable() )
          hbar->show();
      }
    }
  }

  auto iter = data.begin();
  data.insert (iter + pos, text_split.begin(), text_split.end());
  int vmax = ( getRows() > getTextHeight() )
             ? int(getRows()) - int(getTextHeight())
             : 0;
  vbar->setMaximum (vmax);
  vbar->setPageSize (int(getRows()), int(getTextHeight()));
  vbar->calculateSliderValues();

  if ( isShown() && ! vbar->isShown() && isVerticallyScrollable() )
    vbar->show();

  if ( isShown() && vbar->isShown() && ! isVerticallyScrollable() )
    vbar->hide();

  processChanged();
}

//----------------------------------------------------------------------
void FTextView::replaceRange (const FString& str, int from, int to)
{
  if ( from > to || from >= int(getRows()) || to >= int(getRows()) )
    return;

  auto iter = data.begin();
  data.erase (iter + from, iter + to + 1);

  if ( ! str.isNull() )
    insert(str, from);
}

//----------------------------------------------------------------------
void FTextView::clear()
{
  data.clear();
  data.shrink_to_fit();
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


  if ( useFDialogBorder() )
  {
    auto parent = getParentWidget();

    if ( parent )
      static_cast<FDialog*>(parent)->redraw();
  }
  else
    drawBorder();

  std::size_t size = getWidth() - 2;

  if ( size == 0 )
    return;

  for (int y{0}; y < int(getTextHeight()); y++)
  {
    print() << FPoint(2, 2 - nf_offset + y)
            << FString(size, L' ');
  }

  updateTerminal();
  processChanged();
}

//----------------------------------------------------------------------
void FTextView::onKeyPress (FKeyEvent* ev)
{
  int idx = int(ev->key());

  if ( key_map.find(idx) != key_map.end() )
  {
    key_map[idx]();
    ev->accept();
  }
}

//----------------------------------------------------------------------
void FTextView::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton )
    return;

  if ( ! hasFocus() )
  {
    FWidget* focused_widget = getFocusWidget();
    setFocus();

    if ( focused_widget )
      focused_widget->redraw();

    if ( getStatusBar() )
      getStatusBar()->drawMessage();
  }

  auto parent = getParentWidget();

  if ( ! parent )
    return;

  auto dialog = static_cast<FDialog*>(parent);

  if ( parent->isDialogWidget()
    && dialog->isResizeable()
    && ! dialog->isZoomed() )
  {
    int b = ev->getButton();
    const auto& tp = ev->getTermPos();
    const auto& p = parent->termToWidgetPos(tp);
    parent->setFocus();

    try
    {
      const auto& _ev = \
         std::make_shared<FMouseEvent>(fc::MouseDown_Event, p, tp, b);
      FApplication::sendEvent (parent, _ev.get());
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
  auto parent = getParentWidget();

  if ( parent && parent->isDialogWidget() )
  {
    auto dialog = static_cast<FDialog*>(parent);

    if ( dialog->isResizeable() && ! dialog->isZoomed() )
    {
      int b = ev->getButton();
      const auto& tp = ev->getTermPos();
      const auto& p = parent->termToWidgetPos(tp);
      parent->setFocus();

      try
      {
        const auto& _ev = \
            std::make_shared<FMouseEvent>(fc::MouseUp_Event, p, tp, b);
        FApplication::sendEvent (parent, _ev.get());
      }
      catch (const std::bad_alloc& ex)
      {
        std::cerr << bad_alloc_str << ex.what() << std::endl;
      }
    }
  }

  vbar->redraw();
  hbar->redraw();
}

//----------------------------------------------------------------------
void FTextView::onMouseMove (FMouseEvent* ev)
{
  auto parent = getParentWidget();

  if ( parent && parent->isDialogWidget() )
  {
    auto dialog = static_cast<FDialog*>(parent);

    if ( dialog->isResizeable() && ! dialog->isZoomed() )
    {
      int b = ev->getButton();
      const auto& tp = ev->getTermPos();
      const auto& p = parent->termToWidgetPos(tp);
      parent->setFocus();

      try
      {
        const auto& _ev = \
            std::make_shared<FMouseEvent>(fc::MouseMove_Event, p, tp, b);
        FApplication::sendEvent (parent, _ev.get());
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

  if ( isShown() )
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

  int vmax = ( last_line > int(height) - 2 + nf_offset )
             ? last_line - int(height) + 2 - nf_offset
             : 0;
  vbar->setMaximum (vmax);
  vbar->setPageSize (last_line, int(height) - 2 + nf_offset);
  vbar->setX (int(width));
  vbar->setHeight (height - 2 + std::size_t(nf_offset), false);
  vbar->setValue (yoffset);
  vbar->resize();

  if ( width < 3 )
    return;

  int hmax = ( max_width > int(width) - nf_offset - 2 )
             ? max_width - int(width) + nf_offset + 2
             : 0;
  hbar->setMaximum (hmax);
  hbar->setPageSize (max_width, int(width) - nf_offset - 2);
  hbar->setY (int(height));
  hbar->setWidth (width - 2, false);
  hbar->setValue (xoffset);
  hbar->resize();

  if ( isShown() )
  {
    if ( isHorizontallyScrollable() )
      hbar->show();
    else
      hbar->hide();

    if ( isVerticallyScrollable() )
      vbar->show();
    else
      vbar->hide();
  }
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
  initScrollbar (vbar, fc::vertical, this, &FTextView::cb_VBarChange);
  initScrollbar (hbar, fc::horizontal, this, &FTextView::cb_HBarChange);
  const auto& wc = getFWidgetColors();
  setForegroundColor (wc.dialog_fg);
  setBackgroundColor (wc.dialog_bg);
  nf_offset = isNewFont() ? 1 : 0;
  setTopPadding(1);
  setLeftPadding(1);
  setBottomPadding(1);
  setRightPadding(1 + nf_offset);
  mapKeyFunctions();
}

//----------------------------------------------------------------------
inline void FTextView::mapKeyFunctions()
{
  key_map[fc::Fkey_up]    = [&] { scrollBy (0, -1); };
  key_map[fc::Fkey_down]  = [&] { scrollBy (0, 1); };
  key_map[fc::Fkey_left]  = [&] { scrollBy (-1, 0); };
  key_map[fc::Fkey_right] = [&] { scrollBy (1, 0); };
  key_map[fc::Fkey_ppage] = [&] { scrollBy (0, int(-getTextHeight())); };
  key_map[fc::Fkey_npage] = [&] { scrollBy (0, int(getTextHeight())); };
  key_map[fc::Fkey_home]  = [&] { scrollToY (0); };
  key_map[fc::Fkey_end]   = [&] { scrollToY (int(getRows() - getTextHeight())); };
}

//----------------------------------------------------------------------
void FTextView::draw()
{
  setColor();
  drawBorder();
  drawScrollbars();
  drawText();

  if ( hasFocus() && getStatusBar() )
  {
    const auto& msg = getStatusbarMessage();
    const auto& curMsg = getStatusBar()->getMessage();

    if ( curMsg != msg )
    {
      getStatusBar()->setMessage(msg);
      getStatusBar()->drawMessage();
    }
  }

  setCursorPos (FPoint(int(getWidth()), int(getHeight())));
  updateTerminal();
  flushOutputBuffer();
}

//----------------------------------------------------------------------
void FTextView::drawBorder()
{
  if ( ! useFDialogBorder() )
  {
    if ( isMonochron() )
      setReverse(true);

    FRect box(FPoint(1, 1), getSize());
    finalcut::drawListBorder (this, box);

    if ( isMonochron() )
      setReverse(false);
  }
}

//----------------------------------------------------------------------
void FTextView::drawScrollbars()
{
  if ( ! hbar->isShown() && isHorizontallyScrollable() )
    hbar->show();
  else
    vbar->redraw();

  if ( ! vbar->isShown() && isVerticallyScrollable() )
    vbar->show();
  else
    hbar->redraw();
}

//----------------------------------------------------------------------
void FTextView::drawText()
{
  if ( data.empty() || getHeight() <= 2 || getWidth() <= 2 )
    return;

  auto num = getTextHeight();

  if ( num > getRows() )
    num = getRows();

  setColor();

  if ( isMonochron() )
    setReverse(true);

  for (std::size_t y{0}; y < num; y++)  // Line loop
  {
    std::size_t n = std::size_t(yoffset) + y;
    std::size_t pos = std::size_t(xoffset) + 1;
    std::size_t trailing_whitespace{0};
    auto text_width = getTextWidth();
    FString line(getColumnSubString(data[n], pos, text_width));
    auto column_width = getColumnWidth(line);
    print() << FPoint(2, 2 - nf_offset + int(y));

    for (auto&& ch : line)  // Column loop
    {
      if ( getColumnWidth(ch) == 0 )
        continue;
      else if ( isPrintable(ch) )
        print (ch);
      else
        print ('.');
    }

    if ( column_width <= text_width )
      trailing_whitespace = text_width - column_width;

    print() << FString(trailing_whitespace, L' ');
  }

  if ( isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
inline bool FTextView::useFDialogBorder()
{
  auto parent = getParentWidget();
  bool use_fdialog_border{false};

  if ( parent
    && parent->isDialogWidget()
    && isPaddingIgnored()
    && getGeometry() == FRect ( 1
                              , 2
                              , parent->getWidth()
                              , parent->getHeight() - 1) )
  {
    use_fdialog_border = true;
  }

  return use_fdialog_border;
}

//----------------------------------------------------------------------
inline bool FTextView::isPrintable (wchar_t ch)
{
  // Check for printable characters

  bool utf8 = ( getEncoding() == fc::UTF8 ) ? true : false;

  if ( (utf8 && std::iswprint(std::wint_t(ch)))
    || (!utf8 && std::isprint(ch)) )
    return true;

  return false;
}

//----------------------------------------------------------------------
void FTextView::processChanged()
{
  emitCallback("changed");
}

//----------------------------------------------------------------------
void FTextView::cb_VBarChange (FWidget*, FDataPtr)
{
  FScrollbar::sType scrollType = vbar->getScrollType();
  int distance{1};
  int wheel_distance{4};

  if ( scrollType >= FScrollbar::scrollStepBackward )
    update_scrollbar = true;
  else
    update_scrollbar = false;

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
void FTextView::cb_HBarChange (FWidget*, FDataPtr)
{
  FScrollbar::sType scrollType = hbar->getScrollType();
  int distance{1};
  int wheel_distance{4};

  if ( scrollType >= FScrollbar::scrollStepBackward )
    update_scrollbar = true;
  else
    update_scrollbar = false;

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
