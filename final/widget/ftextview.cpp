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
    if ( line.text.isEmpty() )
      continue;

    if ( iter != s.begin() )
    {
      *iter = '\n';  // Add newline character
      ++iter;
    }

    std::copy (line.text.begin(), line.text.end(), iter);
    iter += std::distance(line.text.begin(), line.text.end());
  }

  return s;
}

//----------------------------------------------------------------------
auto FTextView::getSelectedText() const -> FString
{
  if ( ! hasSelectedText() )
    return {};

  const auto start_row = std::min(selection_start.row, selection_end.row);
  const auto end_row = std::max(selection_start.row, selection_end.row);
  const bool wrong_order = hasWrongSelectionOrder();
  const auto start_col = wrong_order ? selection_end.column
                                     : selection_start.column;
  const auto end_col = wrong_order ? selection_start.column
                                   : selection_end.column;
  const auto first = &getLine(start_row);
  const auto last = &getLine(end_row);
  const auto end = last + 1;
  auto iter = first;
  FString selected_text{};
  std::wstring line{};

  while ( iter != end )
  {
    if ( iter == first )
    {
      if ( start_col >= iter->text.getLength() )
      {
        ++iter;
        continue;
      }

      line = iter->text.toWString().substr(start_col);
    }
    else
      line = iter->text.toWString();

    if ( iter == last )
      line.resize(end_col + 1);

    selected_text += FString(line) + L'\n';  // Add newline character
    ++iter;
  }

  return selected_text;
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
  setForegroundColor (wc->text.fg);
  setBackgroundColor (wc->text.bg);
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

  if ( isLowerRightResizeCorner(ev->getPos()) )
  {
    // Event handover to parent dialog
    passResizeCornerEventToDialog(this, *ev);
    select_click_pos.setPoint(-1, -1);  // Reset click position
    pass_to_dialog = true;
    return;
  }

  pass_to_dialog = false;

  if ( isSelectable() && isWithinTextBounds(ev->getPos()) )
  {
    select_click_pos.setPoint (convertMouse2TextPos(ev->getPos()));

    if ( isShown() )
      drawText();
  }
}

//----------------------------------------------------------------------
void FTextView::onMouseUp (FMouseEvent* ev)
{
  if ( isDragging(drag_scroll) )
    stopDragScroll();

  if ( pass_to_dialog )
  {
    // Event handover to parent dialog
    passResizeCornerEventToDialog(this, *ev);
    select_click_pos.setPoint(-1, -1);  // Reset click position
    pass_to_dialog = false;
    return;
  }

  // Handle text selection
  if ( ev->getButton() == MouseButton::Left )
    handleMouseWithinListBounds (ev->getPos());

  FPoint select_end_click_pos = convertMouse2TextPos(ev->getPos());

  if ( selection_start.row == static_cast<FTextViewList::size_type>(select_end_click_pos.getY())
    && selection_start.column == static_cast<FString::size_type>(select_end_click_pos.getX()) )
    resetSelection();

  select_click_pos.setPoint(-1, -1);  // Reset click position
  vbar->redraw();
  hbar->redraw();

  if ( isShown() )
    drawText();
}

//----------------------------------------------------------------------
void FTextView::onMouseMove (FMouseEvent* ev)
{
  if ( pass_to_dialog )
  {
    stopDragScroll();
    // Event handover to parent dialog
    passResizeCornerEventToDialog(this, *ev);
    select_click_pos.setPoint(-1, -1);  // Reset click position
    return;
  }

  // Handle text selection
  if ( ev->getButton() == MouseButton::Left )
    handleMouseWithinListBounds (ev->getPos());

  // Auto-scrolling when dragging mouse outside the widget
  handleMouseDragging (ev);

  if ( isShown() )
    drawText();
}

//----------------------------------------------------------------------
void FTextView::onMouseDoubleClick (FMouseEvent* ev)
{
  using size_type = std::wstring::size_type;

  // Word selection exclusion characters
  const std::wstring select_exclusion_chars = LR"( !"#$%&'()*+,@~:;=^<>`|[]{}\)";

  if ( ! isSelectable()
    || ! isWithinTextBounds(ev->getPos())
    || ev->getButton() != MouseButton::Left )
    return;

  FPoint click_pos = convertMouse2TextPos(ev->getPos());
  setSelectionStartInt (click_pos.getY(), click_pos.getX());
  setSelectionEndInt (click_pos.getY(), click_pos.getX());

  if ( selection_start.row >= getRows()
    || selection_start.column >= data[selection_start.row].text.getLength() )
  {
    resetSelection();
    return;
  }

  const auto& string = data[selection_start.row].text.toWString();
  auto start_pos = string.find_last_of( select_exclusion_chars
                                      , selection_start.column );

  if ( start_pos == std::wstring::npos )
    start_pos = 0;
  else if ( start_pos != selection_start.column )
    start_pos++;

  auto end_pos = string.find_first_of(select_exclusion_chars, start_pos);

  if ( end_pos == std::wstring::npos )
    end_pos = string.length();

  selection_start.column = start_pos;
  selection_end.column = std::max(static_cast<size_type>(0), end_pos - 1);

  if ( isShown() )
    drawText();
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

  // Handle text selection
  handleMouseWithinListBounds (ev->getPos());

  if ( isShown() )
    drawText();
}

//----------------------------------------------------------------------
void FTextView::onTimer (FTimerEvent*)
{
  if ( drag_scroll == DragScrollMode::Leftward )
    dragLeft();
  else if ( drag_scroll == DragScrollMode::Rightward )
    dragRight();
  else if ( drag_scroll == DragScrollMode::Upward )
    dragUp();
  else if ( drag_scroll == DragScrollMode::Downward )
    dragDown();

  hbar->setValue(xoffset);
  vbar->setValue(yoffset);

  if ( isShown() )
  {
    drawScrollbars();
    drawText();
    forceTerminalUpdate();
  }
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
inline auto FTextView::isWithinTextBounds (const FPoint& pos) const -> bool
{
  return pos.getX() > 1
      && pos.getX() < int(getWidth())
      && pos.getY() > 1
      && pos.getY() < int(getHeight());
}

//----------------------------------------------------------------------
inline auto FTextView::isLowerRightResizeCorner (const FPoint& mouse_pos) const -> bool
{
  // 3 characters in the lower right corner  |
  //                                         x
  //                                   -----xx

  return ( (mouse_pos.getX() == int(getWidth()) && mouse_pos.getY() == int(getHeight()) - 1)
        || ( ( mouse_pos.getX() == int(getWidth()) - 1
            || mouse_pos.getX() == int(getWidth()) ) && mouse_pos.getY() == int(getHeight()) ) );
}

//----------------------------------------------------------------------
inline auto FTextView::hasWrongSelectionOrder() const -> bool
{
  bool wrong_column_order = selection_start.row == selection_end.row
                         && selection_start.column > selection_end.column;
  bool wrong_row_order = selection_start.row > selection_end.row;
  return wrong_column_order || wrong_row_order;
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
  if ( useFDialogBorder() )
    return;

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(true);

  const FRect box{FPoint{1, 1}, getSize()};
  finalcut::drawListBorder (this, box);

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);
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

  addHighlighting (line_buffer, data[n].highlight);
  addSelection (line_buffer, n);
  print(line_buffer);
}

//----------------------------------------------------------------------
inline void FTextView::addHighlighting ( FVTermBuffer& line_buffer
                                       , const std::vector<FTextHighlight>& highlight ) const
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
}

//----------------------------------------------------------------------
inline void FTextView::addSelection (FVTermBuffer& line_buffer, std::size_t n) const
{
  const auto start_row = std::min(selection_start.row, selection_end.row);
  const auto end_row = std::max(selection_start.row, selection_end.row);

  if ( ! hasSelectedText() || n < start_row || n > end_row )
    return;

  const bool wrong_order = hasWrongSelectionOrder();
  const auto start_column = wrong_order ? selection_end.column
                                        : selection_start.column;
  const auto end_column = wrong_order ? selection_start.column
                                      : selection_end.column;
  const auto col_pos = static_cast<std::size_t>(xoffset);
  const std::size_t start_col = (start_column > col_pos)
                              ? start_column - col_pos : 0U;
  const std::size_t end_col = (end_column >= col_pos)
                            ? end_column - col_pos + 1 : 0U;
  const auto& wc = getColorTheme();
  const auto has_focus = hasFocus();
  const auto& selected_fg = has_focus
                          ? wc->text.selected_focus_fg
                          : wc->text.selected_fg;
  const auto& selected_bg = has_focus
                          ? wc->text.selected_focus_bg
                          : wc->text.selected_bg;
  const std::size_t start_index = (n == start_row) ? start_col : 0U;
  const std::size_t end_index = (n == end_row)
                              ? std::min(end_col, line_buffer.getLength())
                              : line_buffer.getLength();

  auto select = [&selected_fg, &selected_bg] (auto& fchar)
  {
    fchar.fg_color = selected_fg;
    fchar.bg_color = selected_bg;
  };

  std::for_each (&line_buffer[start_index], &line_buffer[end_index], select);
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
inline auto FTextView::convertMouse2TextPos (const FPoint& pos) const -> FPoint
{
  const FPoint point_delta{ xoffset - getLeftPadding() - 1
                          , yoffset - getTopPadding() - 1 };
  return pos + point_delta;
}

//----------------------------------------------------------------------
void FTextView::handleMouseWithinListBounds (const FPoint& pos)
{
  if ( isSelectable()
    && select_click_pos != FPoint(-1, -1)
    && isWithinTextBounds(pos) )
  {
    FPoint select_end_click_pos = convertMouse2TextPos(pos);

    if ( select_end_click_pos == select_click_pos )
    {
      setSelectionStartInt (select_click_pos.getY(), select_click_pos.getX());
      setSelectionEndInt (select_end_click_pos.getY(), select_end_click_pos.getX());
    }
    else
    {
      const bool wrong_order = select_click_pos.getY() > select_end_click_pos.getY()
                            || select_click_pos.getX() > select_end_click_pos.getX();
      const auto start_column = wrong_order ? select_click_pos.getX() - 1
                                            : select_click_pos.getX();
      const auto end_column = wrong_order ? select_end_click_pos.getX()
                                          : select_end_click_pos.getX() - 1;
      setSelectionStartInt (select_click_pos.getY(), start_column);
      setSelectionEndInt (select_end_click_pos.getY(), end_column);
    }

    if ( isDragging(drag_scroll) )
      stopDragScroll();
  }
}

//----------------------------------------------------------------------
void FTextView::handleMouseDragging (const FMouseEvent* ev)
{
  // Auto-scrolling when dragging mouse outside the widget

  if ( ! isSelectable()
    || select_click_pos == FPoint(-1, -1)
    || isWithinTextBounds(ev->getPos())
    || ev->getButton() != MouseButton::Left )
    return;

  const int mouse_x = ev->getX();
  const int mouse_y = ev->getY();

  if ( mouse_x < 2 )  // drag left
    handleLeftDragScroll();
  else if ( mouse_x >= int(getWidth()) )  // drag right
    handleRightDragScroll();
  else if ( mouse_y < 2 )  // drag up
    handleUpDragScroll();
  else if ( mouse_y >= int(getHeight()) )  // drag down
    handleDownDragScroll();
  else
    stopDragScroll();  // no dragging
}

//----------------------------------------------------------------------
inline void FTextView::handleLeftDragScroll()
{
  if ( xoffset > 0 )
  {
    drag_scroll = DragScrollMode::Leftward;
    delOwnTimers();
    addTimer(scroll_repeat);
  }

  if ( xoffset == 0 )
  {
    delOwnTimers();
    drag_scroll = DragScrollMode::None;
  }
}

//----------------------------------------------------------------------
inline void FTextView::handleRightDragScroll()
{
  const auto xoffset_end = int(max_line_width - getTextWidth());

  if ( xoffset < xoffset_end )
  {
    drag_scroll = DragScrollMode::Rightward;
    delOwnTimers();
    addTimer(scroll_repeat);
  }

  if ( xoffset == xoffset_end )
  {
    delOwnTimers();
    drag_scroll = DragScrollMode::None;
  }
}

//----------------------------------------------------------------------
inline void FTextView::handleUpDragScroll()
{
  if ( yoffset > 0 )
  {
    drag_scroll = DragScrollMode::Upward;
    delOwnTimers();
    addTimer(scroll_repeat);
  }

  if ( yoffset == 0 )
  {
    delOwnTimers();
    drag_scroll = DragScrollMode::None;
  }
}

//----------------------------------------------------------------------
inline void FTextView::handleDownDragScroll()
{
  const auto yoffset_end = int(getRows() - getTextHeight());

  if ( yoffset < yoffset_end )
  {
    drag_scroll = DragScrollMode::Downward;
    delOwnTimers();
    addTimer(scroll_repeat);
  }

  if ( yoffset == yoffset_end )
  {
    delOwnTimers();
    drag_scroll = DragScrollMode::None;
  }
}

//----------------------------------------------------------------------
void FTextView::dragLeft()
{
  if ( xoffset > 0 )
  {
    xoffset--;
    selection_end.column--;
  }

  if ( xoffset == 0 )
  {
    drag_scroll = DragScrollMode::None;
    stopDragScroll();
    return;
  }
}

//----------------------------------------------------------------------
void FTextView::dragRight()
{
  const auto xoffset_end = int(max_line_width - getTextWidth());

  if ( xoffset < xoffset_end )
  {
    xoffset++;
    selection_end.column++;
  }

  if ( xoffset == xoffset_end )
  {
    drag_scroll = DragScrollMode::None;
    stopDragScroll();
    return;
  }
}

//----------------------------------------------------------------------
void FTextView::dragUp()
{
  if ( yoffset > 0 )
  {
    yoffset--;
    const auto start_row = std::size_t(yoffset);
    setSelectionEnd (start_row, 0);
  }

  if ( yoffset == 0 )
  {
    drag_scroll = DragScrollMode::None;
    stopDragScroll();
    return;
  }
}

//----------------------------------------------------------------------
void FTextView::dragDown()
{
  const auto yoffset_end = int(getRows() - getTextHeight());

  if ( yoffset < yoffset_end )
  {
    yoffset++;
    const auto end_column = max_line_width - 1;
    const std::size_t end_row = std::size_t(yoffset)
                              + std::min(getTextHeight(), getRows()) - 1;
    setSelectionEnd (end_row, end_column);
  }

  if ( yoffset == yoffset_end )
  {
    drag_scroll = DragScrollMode::None;
    stopDragScroll();
    return;
  }
}

//----------------------------------------------------------------------
void FTextView::stopDragScroll()
{
  delOwnTimers();
  drag_scroll = DragScrollMode::None;
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
inline auto FTextView::shouldUpdateScrollbar (FScrollbar::ScrollType scroll_type) const -> bool
{
  return scroll_type >= FScrollbar::ScrollType::StepBackward;
}

//----------------------------------------------------------------------
inline auto FTextView::getVerticalScrollDistance (const FScrollbar::ScrollType scroll_type) const -> int
{
  if ( scroll_type == FScrollbar::ScrollType::PageBackward
    || scroll_type == FScrollbar::ScrollType::PageForward )
  {
    return int(getClientHeight());
  }

  return 1;
}

//----------------------------------------------------------------------
inline auto FTextView::getHorizontalScrollDistance (const FScrollbar::ScrollType scroll_type) const -> int
{
  if ( scroll_type == FScrollbar::ScrollType::PageBackward
    || scroll_type == FScrollbar::ScrollType::PageForward )
  {
    return int(getClientWidth());
  }

  return 1;
}

//----------------------------------------------------------------------
void FTextView::cb_vbarChange (const FWidget*)
{
  const auto scroll_type = vbar->getScrollType();
  update_scrollbar = shouldUpdateScrollbar(scroll_type);
  static constexpr int wheel_distance = 4;
  int distance = getVerticalScrollDistance(scroll_type);

  switch ( scroll_type )
  {
    case FScrollbar::ScrollType::PageBackward:
    case FScrollbar::ScrollType::StepBackward:
      scrollBy (0, -distance);
      break;

    case FScrollbar::ScrollType::PageForward:
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
  update_scrollbar = shouldUpdateScrollbar(scroll_type);
  static constexpr int wheel_distance = 4;
  int distance = getHorizontalScrollDistance(scroll_type);

  switch ( scroll_type )
  {
    case FScrollbar::ScrollType::PageBackward:
    case FScrollbar::ScrollType::StepBackward:
      scrollBy (-distance, 0);
      break;

    case FScrollbar::ScrollType::PageForward:
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
