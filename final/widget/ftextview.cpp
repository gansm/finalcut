/***********************************************************************
* ftextview.cpp - Widget FTextView (a multiline text viewer)           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2024 Markus Gans                                      *
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
auto FTextView::getText() const -> FString
{
  if ( data.empty() )
    return {""};

  std::size_t len{0};

  for (auto&& line : data)
    len += line.text.getLength() + 1;  // String length + '\n'

  FString s{len};  // Reserves storage
  auto iter = s.begin();

  for (auto&& line : data)
  {
    if ( ! line.text.isEmpty() )
    {
      if ( iter != s.begin() )
      {
        *iter = '\n';
        ++iter;
      }

      std::copy (line.text.begin(), line.text.end(), iter);
      iter += std::distance(line.text.begin(), line.text.end());
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
  setForegroundColor (wc->dialog.fg);
  setBackgroundColor (wc->dialog.bg);
  FWidget::resetColors();
}

//----------------------------------------------------------------------
void FTextView::setText (const FString& str)
{
  clear();
  insert(str, -1);
}

//----------------------------------------------------------------------
void FTextView::addHighlight (std::size_t line, const FTextHighlight& hgl)
{
  if ( line >= data.size() )
    return;

  data[line].highlight.emplace_back(hgl);
}

//----------------------------------------------------------------------
void FTextView::resetHighlight (std::size_t line)
{
  if ( line >= data.size() )
    return;

  data[line].highlight.clear();
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
  const auto xoffset_end = int(max_line_width - getTextWidth());
  const auto yoffset_end = int(getRows() - getTextHeight());
  const bool changeX( x != xoffset );
  const bool changeY( y != yoffset );

  if ( ! isShown() || ! (changeX || changeY) )
    return;

  xoffset = std::max(0, std::min(x, xoffset_end));
  yoffset = std::max(0, std::min(y, yoffset_end));

  if ( update_scrollbar && changeX && isHorizontallyScrollable() )
  {
    hbar->setValue(xoffset);
    hbar->drawBar();
  }

  if ( update_scrollbar && changeY && isVerticallyScrollable() )
  {
    vbar->setValue(yoffset);
    vbar->drawBar();
  }

  drawText();
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
void FTextView::append (const FString& str)
{
  insert(str, -1);
}

//----------------------------------------------------------------------
void FTextView::insert (const FString& str, int pos)
{
  if ( pos < 0 || pos >= int(getRows()) )
    pos = int(getRows());

  for (auto&& line : splitTextLines(str))  // Line loop
  {
    processLine(std::move(line), pos);
    pos++;
  }

  updateVerticalScrollBar();
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

  auto iter = data.cbegin();
  data.erase (iter + from, iter + to + 1);
}

//----------------------------------------------------------------------
void FTextView::onKeyPress (FKeyEvent* ev)
{
  const auto& iter = key_map.find(ev->key());

  if ( iter != key_map.end() )
  {
    iter->second();
    ev->accept();
  }
}

//----------------------------------------------------------------------
void FTextView::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != MouseButton::Left )
    return;

  setWidgetFocus(this);
  // Event handover to parent dialog
  passResizeCornerEventToDialog(this, *ev);
}

//----------------------------------------------------------------------
void FTextView::onMouseUp (FMouseEvent* ev)
{
  // Event handover to parent dialog
  passResizeCornerEventToDialog(this, *ev);

  vbar->redraw();
  hbar->redraw();
}

//----------------------------------------------------------------------
void FTextView::onMouseMove (FMouseEvent* ev)
{
  // Event handover to parent dialog
  passResizeCornerEventToDialog(this, *ev);
}

//----------------------------------------------------------------------
void FTextView::onWheel (FWheelEvent* ev)
{
  static constexpr int distance = 4;
  const auto& wheel = ev->getWheel();

  if ( wheel == MouseWheel::Up )
    scrollBy (0, -distance);
  else if ( wheel == MouseWheel::Down )
    scrollBy (0, distance);
  else if ( wheel == MouseWheel::Left )
    scrollBy (-distance, 0);
  else if ( wheel == MouseWheel::Right )
    scrollBy (distance, 0);

  if ( isShown() )
    drawText();
}


// protected methods of FTextView
//----------------------------------------------------------------------
void FTextView::initLayout()
{
  nf_offset = FVTerm::getFOutput()->isNewFont() ? 1 : 0;
  setTopPadding(1);
  setLeftPadding(1);
  setBottomPadding(1);
  setRightPadding(1 + nf_offset);

  if ( data.empty() )
    return;

  for (const auto& line : data)
  {
    const auto column_width = getColumnWidth(line.text);
    max_line_width = std::max(column_width, max_line_width);
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
  const auto xoffset_end = max_width - int(width) - nf_offset - 1;
  const auto yoffset_end = last_line - int(height) - nf_offset + 2;
  xoffset = std::max(0, std::min(xoffset, xoffset_end));
  yoffset = std::max(0, std::min(yoffset, yoffset_end));

  if ( height < 3 )
    return;

  vbar->setMaximum (getScrollBarMaxVertical());
  vbar->setPageSize (last_line, int(height) - 2 + nf_offset);
  vbar->setX (int(width));
  vbar->setHeight (height - 2 + std::size_t(nf_offset), false);
  vbar->setValue (yoffset);
  vbar->resize();

  if ( width < 3 )
    return;

  hbar->setMaximum (getScrollBarMaxHorizontal());
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
auto FTextView::getTextHeight() const -> std::size_t
{
  return getHeight() - 2 + std::size_t(nf_offset);
}

//----------------------------------------------------------------------
auto FTextView::getTextWidth() const -> std::size_t
{
  return getWidth() - 2 - std::size_t(nf_offset);
}

//----------------------------------------------------------------------
void FTextView::init()
{
  initScrollbar (vbar, Orientation::Vertical, this, &FTextView::cb_vbarChange);
  initScrollbar (hbar, Orientation::Horizontal, this, &FTextView::cb_hbarChange);
  FTextView::resetColors();
  mapKeyFunctions();
}

//----------------------------------------------------------------------
inline void FTextView::mapKeyFunctions()
{
  key_map =
  {
    { FKey::Up        , [this] { scrollBy (0, -1); } },
    { FKey::Down      , [this] { scrollBy (0, 1); } },
    { FKey::Left      , [this] { scrollBy (-1, 0); } },
    { FKey::Right     , [this] { scrollBy (1, 0); } },
    { FKey::Page_up   , [this] { scrollBy (0, -int(getTextHeight())); } },
    { FKey::Page_down , [this] { scrollBy (0, int(getTextHeight())); } },
    { FKey::Home      , [this] { scrollToBegin(); } },
    { FKey::End       , [this] { scrollToEnd(); } }
  };
}

//----------------------------------------------------------------------
void FTextView::draw()
{
  setColor();
  drawBorder();
  drawScrollbars();
  drawText();
  updateStatusbar(this);
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
  if ( canSkipDrawing() )
    return;

  setColor();

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(true);

  auto num = std::min(getTextHeight(), getRows());

  for (std::size_t y{0}; y < num; y++)  // Line loop
    printLine (y);

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
inline auto FTextView::canSkipDrawing() const -> bool
{
  return data.empty()
      || getHeight() < 3
      || getWidth() < 3;
}

//----------------------------------------------------------------------
inline void FTextView::printLine (std::size_t y)
{
  const std::size_t n = std::size_t(yoffset) + y;
  const std::size_t pos = std::size_t(xoffset) + 1;
  const auto text_width = getTextWidth();
  const FString line(getColumnSubString(data[n].text, pos, text_width));
  print() << FPoint{2, 2 - nf_offset + int(y)};
  FVTermBuffer line_buffer{};
  line_buffer.print(line);

  for (auto&& fchar : line_buffer)  // Column loop
    if ( ! isPrintable(fchar.ch[0]) )
      fchar.ch[0] = L'.';

  const auto column_width = getColumnWidth(line);

  if ( column_width <= text_width )
  {
    auto trailing_whitespace = text_width - column_width;
    line_buffer.print() << FString{trailing_whitespace, L' '};
  }

  printHighlighted (line_buffer, data[n].highlight);
}

//----------------------------------------------------------------------
inline void FTextView::printHighlighted ( FVTermBuffer& line_buffer
                                        , const std::vector<FTextHighlight>& highlight )
{
  for (auto&& hgl : highlight)
  {
    for (std::size_t i{0}; i < hgl.length; i++)
    {
      if ( hgl.index + i < std::size_t(xoffset) )
        continue;

      auto index = hgl.index + i - std::size_t(xoffset);

      if ( index >= line_buffer.getLength() )
        break;

      auto& fchar = line_buffer[index];
      fchar.fg_color = hgl.attributes.fg_color;
      fchar.bg_color = hgl.attributes.bg_color;
      fchar.attr = hgl.attributes.attr;
    }
  }

  print(line_buffer);
}

//----------------------------------------------------------------------
inline auto FTextView::useFDialogBorder() const -> bool
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
inline auto FTextView::isPrintable (wchar_t ch) const -> bool
{
  // Check for printable characters

  const bool utf8 = (FVTerm::getFOutput()->getEncoding() == Encoding::UTF8);
  return ( (utf8 && finalcut::isPrintable(ch))
        || (! utf8 && finalcut::isPrintable(char(ch))) );
}

//----------------------------------------------------------------------
inline auto FTextView::splitTextLines (const FString& str) const -> FStringList
{
  if ( str.isEmpty() )
  {
    FStringList list{};
    list.emplace_back("");
    return list;
  }

  const auto& string = str.rtrim().expandTabs(getFOutput()->getTabstop());
  return string.split("\n");
}

//----------------------------------------------------------------------
inline void FTextView::processLine (FString&& line, int pos)
{
  line = line.removeBackspaces()
             .removeDel()
             .replaceControlCodes()
             .rtrim();
  updateHorizontalScrollBar (getColumnWidth(line));
  data.emplace (data.cbegin() + pos, std::move(line));
}

//----------------------------------------------------------------------
inline auto FTextView::getScrollBarMaxHorizontal() const noexcept -> int
{
  const auto max_width = int(max_line_width);
  const auto width = int(getWidth());
  return max_width >= width - nf_offset - 1
         ? max_width - width + nf_offset + 2
         : 0;
}

//----------------------------------------------------------------------
inline auto FTextView::getScrollBarMaxVertical() const noexcept -> int
{
  const auto last_line = int(getRows());
  const auto height = int(getHeight());
  return last_line > height - 2 + nf_offset
         ? last_line - height + 2 - nf_offset
         : 0;
}

//----------------------------------------------------------------------
inline void FTextView::updateVerticalScrollBar() const
{
  vbar->setMaximum (getScrollBarMaxVertical());
  vbar->setPageSize (int(getRows()), int(getTextHeight()));
  vbar->calculateSliderValues();

  if ( isShown() && ! vbar->isShown() && isVerticallyScrollable() )
    vbar->show();

  if ( isShown() && vbar->isShown() && ! isVerticallyScrollable() )
    vbar->hide();
}

//----------------------------------------------------------------------
inline void FTextView::updateHorizontalScrollBar (std::size_t column_width)
{
  if ( column_width <= max_line_width )
    return;

  max_line_width = column_width;

  if ( column_width <= getTextWidth() )
    return;

  hbar->setMaximum (getScrollBarMaxHorizontal());
  hbar->setPageSize (int(max_line_width), int(getTextWidth()));
  hbar->calculateSliderValues();

  if ( isShown() && isHorizontallyScrollable() )
    hbar->show();
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
  const auto scroll_type = vbar->getScrollType();
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
    case FScrollbar::ScrollType::WheelLeft:
      scrollBy (0, -wheel_distance);
      break;

    case FScrollbar::ScrollType::WheelDown:
    case FScrollbar::ScrollType::WheelRight:
      scrollBy (0, wheel_distance);
      break;

    default:
      throw std::invalid_argument{"Invalid scroll type"};
  }

  update_scrollbar = true;
}

//----------------------------------------------------------------------
void FTextView::cb_hbarChange (const FWidget*)
{
  const auto scroll_type = hbar->getScrollType();
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
    case FScrollbar::ScrollType::WheelLeft:
      scrollBy (-wheel_distance, 0);
      break;

    case FScrollbar::ScrollType::WheelDown:
    case FScrollbar::ScrollType::WheelRight:
      scrollBy (wheel_distance, 0);
      break;

    default:
      throw std::invalid_argument{"Invalid scroll type"};
  }

  update_scrollbar = true;
}

}  // namespace finalcut
