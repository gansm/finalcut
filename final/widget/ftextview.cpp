/***********************************************************************
* ftextview.cpp - Widget FTextView (a multiline text viewer)           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2021 Markus Gans                                      *
*                                                                      *
* FINAL CUT is free software; you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as       *
* published by the Free Software Foundation; either version 3 of       *
* the License, or (at your option) any later version.                  *
*                                                                      *
* FINAL CUT is distributed in the hope that it will be useful, but     *
* WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU Lesser General Public License for more details.                  *
*                                                                      *
* You should have received a copy of the GNU Lesser General Public     *
* License along with this program.  If not, see                        *
* <http://www.gnu.org/licenses/>.                                      *
***********************************************************************/

#include <memory>

#include "final/dialog/fdialog.h"
#include "final/fapplication.h"
#include "final/fc.h"
#include "final/fevent.h"
#include "final/fwidgetcolors.h"
#include "final/util/fstring.h"
#include "final/vterm/fvtermbuffer.h"
#include "final/widget/fscrollbar.h"
#include "final/widget/fstatusbar.h"
#include "final/widget/ftextview.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FTextView
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FTextView::FTextView(FWidget* parent)
  : FWidget{parent}
{
  init();
}

//----------------------------------------------------------------------
FTextView::~FTextView() noexcept = default;  // destructor


// public methods of FTextView
//----------------------------------------------------------------------
FString FTextView::getText() const
{
  if ( data.empty() )
    return {""};

  std::size_t len{0};

  for (auto&& line : data)
    len += line.getLength() + 1;  // String length + '\n'

  FString s{len};  // Reserves storage
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
void FTextView::setSize (const FSize& size, bool adjust)
{
  // Sets the text view size

  FWidget::setSize (size, adjust);
  changeOnResize();
}

//----------------------------------------------------------------------
void FTextView::setGeometry ( const FPoint& pos, const FSize& size
                            , bool adjust)
{
  // Sets the text view geometry

  FWidget::setGeometry(pos, size, adjust);
  changeOnResize();
}

//----------------------------------------------------------------------
void FTextView::resetColors()
{
  const auto& wc = getColorTheme();
  setForegroundColor (wc->dialog_fg);
  setBackgroundColor (wc->dialog_bg);
  FWidget::resetColors();
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
  const bool changeX( x != xoffset );
  const bool changeY( y != yoffset );

  if ( ! isShown() || ! (changeX || changeY) )
    return;

  if ( changeX && isHorizontallyScrollable() )
  {
    const auto xoffset_end = int(max_line_width - getTextWidth());
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
    const auto yoffset_end = int(getRows() - getTextHeight());
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
  forceTerminalUpdate();
}

//----------------------------------------------------------------------
void FTextView::scrollToBegin()
{
  scrollToY (0);
}

//----------------------------------------------------------------------
void FTextView::scrollToEnd()
{
  scrollToY (int(getRows() - getTextHeight()));
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
    s = FString{str}.rtrim().expandTabs(getFOutput()->getTabstop());

  auto showHorizontallyScrollable = [this] ()
  {
    if ( isShown() && isHorizontallyScrollable() )
      hbar->show();
  };
  auto text_split = s.split("\n");

  for (auto&& line : text_split)  // Line loop
  {
    line = line.removeBackspaces()
               .removeDel()
               .replaceControlCodes()
               .rtrim();
    const auto column_width = getColumnWidth(line);

    if ( column_width > max_line_width )
    {
      max_line_width = column_width;

      if ( column_width > getTextWidth() )
      {
        const int hmax = ( max_line_width > getTextWidth() )
                         ? int(max_line_width) - int(getTextWidth())
                         : 0;
        hbar->setMaximum (hmax);
        hbar->setPageSize (int(max_line_width), int(getTextWidth()));
        hbar->calculateSliderValues();
        showHorizontallyScrollable();
      }
    }
  }

  auto iter = data.begin();
  data.insert (iter + pos, text_split.begin(), text_split.end());
  const int vmax = ( getRows() > getTextHeight() )
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
  try
  {
    deleteRange (from, to);
  }
  catch (const std::out_of_range&)
  {
    throw std::out_of_range("");  // Invalid range
  }

  insert(str, from);
}

//----------------------------------------------------------------------
void FTextView::deleteRange (int from, int to)
{
  if ( from > to || from >= int(getRows()) || to >= int(getRows()) )
    throw std::out_of_range("");  // Invalid range

  auto iter = data.begin();
  data.erase (iter + from, iter + to + 1);
}

//----------------------------------------------------------------------
void FTextView::clear()
{
  data.clear();
  data.shrink_to_fit();
  xoffset = 0;
  yoffset = 0;
  max_line_width = 0;

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

  const std::size_t size = getWidth() - 2;

  if ( size == 0 )
    return;

  for (auto y{0}; y < int(getTextHeight()); y++)
  {
    print() << FPoint{2, 2 - nf_offset + y}
            << FString{size, L' '};
  }

  processChanged();
}

//----------------------------------------------------------------------
void FTextView::onKeyPress (FKeyEvent* ev)
{
  const auto& entry = key_map[ev->key()];

  if ( entry )
  {
    entry();
    ev->accept();
  }
}

//----------------------------------------------------------------------
void FTextView::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != MouseButton::Left )
    return;

  setWidgetFocus(this);
  auto parent = getParentWidget();

  if ( ! parent )
    return;

  const auto& dialog = static_cast<FDialog*>(parent);

  if ( parent->isDialogWidget()
    && dialog->isResizeable()
    && ! dialog->isZoomed() )
  {
    const auto b = ev->getButton();
    const auto& tp = ev->getTermPos();
    const auto& p = parent->termToWidgetPos(tp);
    const auto& _ev = \
       std::make_shared<FMouseEvent>(Event::MouseDown, p, tp, b);
    FApplication::sendEvent (parent, _ev.get());
  }
}

//----------------------------------------------------------------------
void FTextView::onMouseUp (FMouseEvent* ev)
{
  auto parent = getParentWidget();

  if ( parent && parent->isDialogWidget() )
  {
    const auto& dialog = static_cast<FDialog*>(parent);

    if ( dialog->isResizeable() && ! dialog->isZoomed() )
    {
      const auto b = ev->getButton();
      const auto& tp = ev->getTermPos();
      const auto& p = parent->termToWidgetPos(tp);
      const auto& _ev = \
          std::make_shared<FMouseEvent>(Event::MouseUp, p, tp, b);
      FApplication::sendEvent (parent, _ev.get());
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
    const auto& dialog = static_cast<FDialog*>(parent);

    if ( dialog->isResizeable() && ! dialog->isZoomed() )
    {
      const auto b = ev->getButton();
      const auto& tp = ev->getTermPos();
      const auto& p = parent->termToWidgetPos(tp);
      const auto& _ev = \
          std::make_shared<FMouseEvent>(Event::MouseMove, p, tp, b);
      FApplication::sendEvent (parent, _ev.get());
    }
  }
}

//----------------------------------------------------------------------
void FTextView::onWheel (FWheelEvent* ev)
{
  static constexpr int distance = 4;
  const MouseWheel wheel = ev->getWheel();

  if ( wheel == MouseWheel::Up )
    scrollBy (0, -distance);
  else if ( wheel == MouseWheel::Down )
    scrollBy (0, distance);

  if ( isShown() )
    drawText();

  forceTerminalUpdate();
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
void FTextView::initLayout()
{
  if ( data.empty() )
    return;

  for (auto&& line : data)
  {
    const auto column_width = getColumnWidth(line);

    if ( column_width > max_line_width )
      max_line_width = column_width;
  }
}

//----------------------------------------------------------------------
void FTextView::adjustSize()
{
  FWidget::adjustSize();
  const std::size_t width = getWidth();
  const std::size_t height = getHeight();
  const auto last_line = int(getRows());
  const auto max_width = int(max_line_width);

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

  const int vmax = ( last_line > int(height) - 2 + nf_offset )
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

  const int hmax = ( max_width >= int(width) - nf_offset - 1 )
                   ? max_width - int(width) + nf_offset + 2
                   : 0;
  hbar->setMaximum (hmax);
  hbar->setPageSize (max_width, int(width) - nf_offset - 2);
  hbar->setY (int(height));
  hbar->setWidth (width - 2, false);
  hbar->setValue (xoffset);
  hbar->resize();

  if ( isHorizontallyScrollable() )
    hbar->show();
  else
    hbar->hide();

  if ( isVerticallyScrollable() )
    vbar->show();
  else
    vbar->hide();
}


// private methods of FTextView
//----------------------------------------------------------------------
std::size_t FTextView::getTextHeight() const
{
  return getHeight() - 2 + std::size_t(nf_offset);
}

//----------------------------------------------------------------------
std::size_t FTextView::getTextWidth() const
{
  return getWidth() - 2 - std::size_t(nf_offset);
}

//----------------------------------------------------------------------
void FTextView::init()
{
  initScrollbar (vbar, Orientation::Vertical, this, &FTextView::cb_vbarChange);
  initScrollbar (hbar, Orientation::Horizontal, this, &FTextView::cb_hbarChange);
  FTextView::resetColors();
  nf_offset = FVTerm::getFOutput()->isNewFont() ? 1 : 0;
  setTopPadding(1);
  setLeftPadding(1);
  setBottomPadding(1);
  setRightPadding(1 + nf_offset);
  mapKeyFunctions();
}

//----------------------------------------------------------------------
inline void FTextView::mapKeyFunctions()
{
  key_map[FKey::Up]        = [this] { scrollBy (0, -1); };
  key_map[FKey::Down]      = [this] { scrollBy (0, 1); };
  key_map[FKey::Left]      = [this] { scrollBy (-1, 0); };
  key_map[FKey::Right]     = [this] { scrollBy (1, 0); };
  key_map[FKey::Page_up]   = [this] { scrollBy (0, -int(getTextHeight())); };
  key_map[FKey::Page_down] = [this] { scrollBy (0, int(getTextHeight())); };
  key_map[FKey::Home]      = [this] { scrollToBegin(); };
  key_map[FKey::End]       = [this] { scrollToEnd(); };
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

  setCursorPos ({int(getWidth()), int(getHeight())});
}

//----------------------------------------------------------------------
void FTextView::drawBorder()
{
  if ( ! useFDialogBorder() )
  {
    if ( FVTerm::getFOutput()->isMonochron() )
      setReverse(true);

    const FRect box{FPoint{1, 1}, getSize()};
    finalcut::drawListBorder (this, box);

    if ( FVTerm::getFOutput()->isMonochron() )
      setReverse(false);
  }
}

//----------------------------------------------------------------------
void FTextView::drawScrollbars() const
{
  if ( ! hbar->isShown() && isHorizontallyScrollable() )
    hbar->show();
  else
    hbar->redraw();

  if ( ! vbar->isShown() && isVerticallyScrollable() )
    vbar->show();
  else
    vbar->redraw();
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

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(true);

  for (std::size_t y{0}; y < num; y++)  // Line loop
  {
    const std::size_t n = std::size_t(yoffset) + y;
    const std::size_t pos = std::size_t(xoffset) + 1;
    const auto text_width = getTextWidth();
    const FString line(getColumnSubString(data[n], pos, text_width));
    std::size_t trailing_whitespace{0};
    print() << FPoint{2, 2 - nf_offset + int(y)};
    FVTermBuffer line_buffer{};
    line_buffer.write(line);

    for (auto&& fchar : line_buffer)  // Column loop
      if ( ! isPrintable(fchar.ch[0]) )
        fchar.ch[0] = L'.';

    print(line_buffer);
    const auto column_width = getColumnWidth(line);

    if ( column_width <= text_width )
      trailing_whitespace = text_width - column_width;

    print() << FString{trailing_whitespace, L' '};
  }

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
inline bool FTextView::useFDialogBorder() const
{
  const auto& parent = getParentWidget();
  bool use_fdialog_border{false};

  if ( parent
    && parent->isDialogWidget()
    && isPaddingIgnored()
    && getGeometry() == FRect { 1
                              , 2
                              , parent->getWidth()
                              , parent->getHeight() - 1} )
  {
    use_fdialog_border = true;
  }

  return use_fdialog_border;
}

//----------------------------------------------------------------------
inline bool FTextView::isPrintable (wchar_t ch) const
{
  // Check for printable characters

  const bool utf8 = (FVTerm::getFOutput()->getEncoding() == Encoding::UTF8);
  return ( (utf8 && std::iswprint(std::wint_t(ch)))
        || (! utf8 && std::isprint(char(ch))) );
}

//----------------------------------------------------------------------
void FTextView::processChanged() const
{
  emitCallback("changed");
}

//----------------------------------------------------------------------
void FTextView::changeOnResize() const
{
  const std::size_t width  = getWidth();
  const std::size_t height = getHeight();

  if ( FVTerm::getFOutput()->isNewFont() )
  {
    vbar->setGeometry (FPoint{int(width), 1}, FSize{2, height - 1});
    hbar->setGeometry (FPoint{1, int(height)}, FSize{width - 2, 1});
  }
  else
  {
    vbar->setGeometry (FPoint{int(width), 2}, FSize{1, height - 2});
    hbar->setGeometry (FPoint{2, int(height)}, FSize{width - 2, 1});
  }

  vbar->resize();
  hbar->resize();
}

//----------------------------------------------------------------------
void FTextView::cb_vbarChange (const FWidget*)
{
  const FScrollbar::ScrollType scroll_type = vbar->getScrollType();
  static constexpr int wheel_distance = 4;
  int distance{1};

  if ( scroll_type >= FScrollbar::ScrollType::StepBackward )
    update_scrollbar = true;
  else
    update_scrollbar = false;

  switch ( scroll_type )
  {
    case FScrollbar::ScrollType::PageBackward:
      distance = int(getClientHeight());
      // fall through
    case FScrollbar::ScrollType::StepBackward:
      scrollBy (0, -distance);
      break;

    case FScrollbar::ScrollType::PageForward:
      distance = int(getClientHeight());
      // fall through
    case FScrollbar::ScrollType::StepForward:
      scrollBy (0, distance);
      break;

    case FScrollbar::ScrollType::Jump:
      scrollToY (vbar->getValue());
      break;

    case FScrollbar::ScrollType::WheelUp:
      scrollBy (0, -wheel_distance);
      break;

    case FScrollbar::ScrollType::WheelDown:
      scrollBy (0, wheel_distance);
      break;

    default:
      break;
  }

  update_scrollbar = true;
}

//----------------------------------------------------------------------
void FTextView::cb_hbarChange (const FWidget*)
{
  const FScrollbar::ScrollType scroll_type = hbar->getScrollType();
  static constexpr int wheel_distance = 4;
  int distance{1};

  if ( scroll_type >= FScrollbar::ScrollType::StepBackward )
    update_scrollbar = true;
  else
    update_scrollbar = false;

  switch ( scroll_type )
  {
    case FScrollbar::ScrollType::PageBackward:
      distance = int(getClientWidth());
      // fall through
    case FScrollbar::ScrollType::StepBackward:
      scrollBy (-distance, 0);
      break;

    case FScrollbar::ScrollType::PageForward:
      distance = int(getClientWidth());
      // fall through
    case FScrollbar::ScrollType::StepForward:
      scrollBy (distance, 0);
      break;

    case FScrollbar::ScrollType::Jump:
      scrollToX (hbar->getValue());
      break;

    case FScrollbar::ScrollType::WheelUp:
      scrollBy (-wheel_distance, 0);
      break;

    case FScrollbar::ScrollType::WheelDown:
      scrollBy (wheel_distance, 0);
      break;

    default:
      break;
  }

  update_scrollbar = true;
}

}  // namespace finalcut
