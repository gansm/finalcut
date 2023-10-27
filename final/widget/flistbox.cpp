/***********************************************************************
* flistbox.cpp - Widget FListBox and FListBoxItem                      *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2023 Markus Gans                                      *
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

#include <algorithm>
#include <memory>

#include "final/fapplication.h"
#include "final/fevent.h"
#include "final/fwidgetcolors.h"
#include "final/util/fstring.h"
#include "final/vterm/fcolorpair.h"
#include "final/widget/flistbox.h"
#include "final/widget/fstatusbar.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FListBox
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FListBox::FListBox (FWidget* parent)
  : FWidget{parent}
{
  init();
}

//----------------------------------------------------------------------
FListBox::~FListBox()  // destructor
{
  delete data.source_container;  // for lazy conversion
  data.source_container = nullptr;
  delOwnTimers();
}


// public methods of FListBox
//----------------------------------------------------------------------
void FListBox::setCurrentItem (std::size_t index)
{
  if ( index == selection.current )
    return;

  const std::size_t element_count = getCount();

  if ( index > element_count )
    selection.current = element_count;
  else if ( index < 1 )
    selection.current = 1;
  else
    selection.current = index;

  scroll.xoffset = 0;
  scroll.yoffset = 0;
  adjustSize();
  scroll.vbar->setValue(scroll.yoffset);
  redraw();
}

//----------------------------------------------------------------------
void FListBox::setCurrentItem (FListBoxItems::iterator iter)
{
  const auto index = std::size_t(std::distance(data.itemlist.begin(), iter)) + 1;
  setCurrentItem(index);
}

//----------------------------------------------------------------------
void FListBox::showInsideBrackets ( const std::size_t index
                                  , BracketType b )
{
  auto iter = index2iterator(index - 1);
  iter->brackets = b;

  if ( b == BracketType::None )
    return;

  const auto column_width = getColumnWidth(iter->getText()) + 2;

  if ( column_width > max_line_width )
  {
    max_line_width = column_width;

    if ( column_width >= getWidth() - nf_offset - 3 )
    {
      const int hmax = ( max_line_width > getWidth() - nf_offset - 4 )
                       ? int(max_line_width - getWidth() + nf_offset + 4)
                       : 0;
      scroll.hbar->setMaximum (hmax);
      scroll.hbar->setPageSize (int(max_line_width), int(getWidth() - nf_offset - 4));
      scroll.hbar->setValue (scroll.xoffset);

      if ( ! scroll.hbar->isShown() )
        scroll.hbar->show();
    }
  }
}

//----------------------------------------------------------------------
void FListBox::setSize (const FSize& size, bool adjust)
{
  FWidget::setSize (size, adjust);
  changeOnResize();
}

//----------------------------------------------------------------------
void FListBox::setGeometry ( const FPoint& pos, const FSize& size
                           , bool adjust )
{
  FWidget::setGeometry (pos, size, adjust);
  changeOnResize();
}

//----------------------------------------------------------------------
void FListBox::setText (const FString& txt)
{
  data.text.setString(txt);
}

//----------------------------------------------------------------------
void FListBox::hide()
{
  FWidget::hide();
  hideArea (getSize());
}

//----------------------------------------------------------------------
void FListBox::insert (const FListBoxItem& listItem)
{
  const auto column_width = getColumnWidth(listItem.text);
  const bool has_brackets(listItem.brackets != BracketType::None);
  recalculateHorizontalBar (column_width, has_brackets);

  data.itemlist.push_back (listItem);

  if ( selection.current == 0 )
    selection.current = 1;

  auto element_count = getCount();
  recalculateVerticalBar (element_count);
  processChanged();
}

//----------------------------------------------------------------------
void FListBox::remove (std::size_t item)
{
  if ( item > getCount() )
    return;

  data.itemlist.erase (data.itemlist.cbegin() + int(item) - 1);
  const std::size_t element_count = getCount();
  max_line_width = 0;

  for (const auto& listbox_item : data.itemlist)
  {
    const auto column_width = getColumnWidth(listbox_item.getText());

    if ( column_width > max_line_width )
      max_line_width = column_width;
  }

  const int hmax = ( max_line_width > getWidth() - nf_offset - 4 )
                   ? int(max_line_width - getWidth() + nf_offset + 4)
                   : 0;
  scroll.hbar->setMaximum (hmax);
  scroll.hbar->setPageSize (int(max_line_width), int(getWidth() - nf_offset - 4));

  if ( scroll.hbar->isShown() && isHorizontallyScrollable() )
    scroll.hbar->hide();

  const int vmax = ( element_count + 2 > getHeight() )
                   ? int(element_count - getHeight()) + 2
                   : 0;
  scroll.vbar->setMaximum (vmax);
  scroll.vbar->setPageSize (int(element_count), int(getHeight()) - 2);

  if ( scroll.vbar->isShown() && isVerticallyScrollable() )
    scroll.vbar->hide();

  if ( selection.current >= item && selection.current > 1 )
    selection.current--;

  if ( selection.current > element_count )
    selection.current = element_count;

  if ( scroll.yoffset > int(element_count - getHeight()) + 2 )
    scroll.yoffset = int(element_count - getHeight()) + 2;

  if ( scroll.yoffset < 0 )
    scroll.yoffset = 0;

  processChanged();
}

//----------------------------------------------------------------------
void FListBox::clear()
{
  data.itemlist.clear();
  data.itemlist.shrink_to_fit();
  selection.current = 0;
  scroll.xoffset = 0;
  scroll.yoffset = 0;
  max_line_width = 0;
  selection.last_current = -1;
  scroll.last_yoffset = -1;

  scroll.vbar->setMinimum(0);
  scroll.vbar->setValue(0);
  scroll.vbar->hide();

  scroll.hbar->setMinimum(0);
  scroll.hbar->setValue(0);
  scroll.hbar->hide();

  // clear list from screen
  const auto& wc = getColorTheme();
  setColor (wc->list_fg, wc->list_bg);
  const std::size_t size = getWidth() - 2;
  drawBorder();
  drawHeadline();

  if ( size == 0 )
    return;

  for (auto y{0}; y < int(getHeight()) - 2; y++)
  {
    print() << FPoint{2, 2 + y} << FString{size, L' '};
  }

  processChanged();
}

//----------------------------------------------------------------------
void FListBox::onKeyPress (FKeyEvent* ev)
{
  const std::size_t current_before = selection.current;
  const int xoffset_before = scroll.xoffset;
  const int yoffset_before = scroll.yoffset;
  processKeyAction(ev);  // Process the keystrokes

  if ( current_before != selection.current )
    processRowChanged();

  if ( ev->isAccepted() )
  {
    const bool draw_vbar( yoffset_before != scroll.yoffset );
    const bool draw_hbar( xoffset_before != scroll.xoffset );
    updateDrawing (draw_vbar, draw_hbar);
  }
}

//----------------------------------------------------------------------
void FListBox::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != MouseButton::Left
    && ev->getButton() != MouseButton::Right )
  {
    return;
  }

  if ( ev->getButton() == MouseButton::Right && ! isMultiSelection() )
    return;

  setWidgetFocus(this);
  const int yoffset_before = scroll.yoffset;
  const std::size_t current_before = selection.current;

  if ( isWithinListBounds(ev->getPos()) )
  {
    selection.click_on_list = true;
    const std::size_t element_count = getCount();
    selection.current = std::size_t(scroll.yoffset + ev->getY() - 1);

    if ( selection.current > element_count )
      selection.current = element_count;

    data.inc_search.clear();

    if ( ev->getButton() == MouseButton::Right )
      multiSelection(selection.current);

    if ( current_before != selection.current )
    {
      processRowChanged();
    }

    if ( isShown() )
      drawList();

    scroll.vbar->setValue (scroll.yoffset);

    if ( yoffset_before != scroll.yoffset )
      scroll.vbar->drawBar();

    forceTerminalUpdate();
  }
}

//----------------------------------------------------------------------
void FListBox::onMouseUp (FMouseEvent* ev)
{
  selection.click_on_list = false;

  if ( isDragging(drag_scroll) )
    stopDragScroll();

  if ( ev->getButton() != MouseButton::Left )
    return;

  if ( isWithinListBounds(ev->getPos())
    && ! isMultiSelection() )
    processSelect();
}

//----------------------------------------------------------------------
void FListBox::onMouseMove (FMouseEvent* ev)
{
  if ( ev->getButton() != MouseButton::Left
    && ev->getButton() != MouseButton::Right )
    return;

  if ( ev->getButton() == MouseButton::Right && ! isMultiSelection() )
    return;

  const std::size_t current_before = selection.current;
  const int yoffset_before = scroll.yoffset;
  const int mouse_y = ev->getY();

  if ( isWithinListBounds(ev->getPos()) )
  {
    selection.click_on_list = true;
    const std::size_t element_count = getCount();
    selection.current = std::size_t(scroll.yoffset + mouse_y - 1);

    if ( selection.current > element_count )
      selection.current = element_count;

    data.inc_search.clear();

    if ( current_before != selection.current )
    {
      // Handle multiple selections + changes
      if ( ev->getButton() == MouseButton::Right )
      {
        processRowChanged();
        multiSelectionUpTo(selection.current);
      }
      else if ( ev->getButton() == MouseButton::Left )
        processRowChanged();
    }

    if ( isShown() )
      drawList();

    scroll.vbar->setValue (scroll.yoffset);

    if ( yoffset_before != scroll.yoffset )
      scroll.vbar->drawBar();

    forceTerminalUpdate();
  }

  // Auto-scrolling when dragging mouse outside the widget
  if ( selection.click_on_list && mouse_y < 2 )
    dragUp (ev->getButton());
  else if ( selection.click_on_list && mouse_y >= int(getHeight()) )
    dragDown (ev->getButton());
  else
    stopDragScroll();
}

//----------------------------------------------------------------------
void FListBox::onMouseDoubleClick (FMouseEvent* ev)
{
  if ( ev->getButton() != MouseButton::Left
    || ! isWithinListBounds(ev->getPos()) )
    return;

  if ( scroll.yoffset + ev->getY() - 1 > int(getCount()) )
    return;

  processClick();
}

//----------------------------------------------------------------------
void FListBox::onTimer (FTimerEvent*)
{
  const std::size_t current_before = selection.current;
  const int yoffset_before = scroll.yoffset;

  if ( ( drag_scroll == DragScrollMode::Upward
      || drag_scroll == DragScrollMode::SelectUpward )
      && ! dragScrollUp() )
  {
    return;
  }

  if ( ( drag_scroll == DragScrollMode::Downward
      || drag_scroll == DragScrollMode::SelectDownward )
      && ! dragScrollDown() )
  {
    return;
  }

  if ( current_before != selection.current )
  {
    data.inc_search.clear();
    processRowChanged();

    // Handle multiple selections
    if ( drag_scroll == DragScrollMode::SelectUpward
      || drag_scroll == DragScrollMode::SelectDownward )
      multiSelectionUpTo(selection.current);
  }

  if ( isShown() )
    drawList();

  scroll.vbar->setValue (scroll.yoffset);

  if ( yoffset_before != scroll.yoffset )
    scroll.vbar->drawBar();

  forceTerminalUpdate();
}

//----------------------------------------------------------------------
void FListBox::onWheel (FWheelEvent* ev)
{
  const std::size_t current_before = selection.current;
  const int yoffset_before = scroll.yoffset;
  static constexpr int wheel_distance = 4;
  const auto& wheel = ev->getWheel();

  if ( isDragging(drag_scroll) )
    stopDragScroll();

  if ( wheel == MouseWheel::Up )
    wheelUp (wheel_distance);
  else if ( wheel == MouseWheel::Down )
    wheelDown (wheel_distance);
  else if ( wheel == MouseWheel::Left )
    wheelLeft (wheel_distance);
  else if ( wheel == MouseWheel::Right )
    wheelRight (wheel_distance);

  if ( current_before != selection.current )
  {
    data.inc_search.clear();
    processRowChanged();
  }

  if ( isShown() )
    drawList();

  scroll.vbar->setValue (scroll.yoffset);

  if ( yoffset_before != scroll.yoffset )
    scroll.vbar->drawBar();

  forceTerminalUpdate();
}

//----------------------------------------------------------------------
void FListBox::onFocusIn (FFocusEvent* in_ev)
{
  drawStatusBarMessage();
  data.inc_search.clear();
  FWidget::onFocusIn(in_ev);
}

//----------------------------------------------------------------------
void FListBox::onFocusOut (FFocusEvent* out_ev)
{
  delOwnTimers();
  data.inc_search.clear();
  FWidget::onFocusOut(out_ev);
}


// protected methods of FListBox
//----------------------------------------------------------------------
void FListBox::initLayout()
{
  nf_offset = FVTerm::getFOutput()->isNewFont() ? 1 : 0;
  setTopPadding(1);
  setLeftPadding(1);
  setBottomPadding(1);
  setRightPadding(1);
}

//----------------------------------------------------------------------
void FListBox::adjustYOffset (std::size_t element_count)
{
  const std::size_t height = getClientHeight();

  if ( height == 0 || element_count == 0 )
    return;

  if ( scroll.yoffset > int(element_count - height) )
    scroll.yoffset = int(element_count - height);

  if ( scroll.yoffset < 0 )
    scroll.yoffset = 0;

  if ( selection.current < std::size_t(scroll.yoffset) )
    selection.current = std::size_t(scroll.yoffset);

  if ( scroll.yoffset < int(selection.current - height) )
    scroll.yoffset = int(selection.current - height);
}

//----------------------------------------------------------------------
void FListBox::adjustSize()
{
  FWidget::adjustSize();
  const std::size_t element_count = getCount();
  const std::size_t width = getClientWidth();
  const std::size_t height = getClientHeight();

  adjustYOffset (element_count);

  const int vmax = ( element_count > height )
                   ? int(element_count - height)
                   : 0;
  scroll.vbar->setMaximum (vmax);
  scroll.vbar->setPageSize (int(element_count), int(height));
  scroll.vbar->setX (int(getWidth()));
  scroll.vbar->setHeight (height, false);
  scroll.vbar->resize();

  const int hmax = ( max_line_width + 2 > width )
                   ? int(max_line_width - width + 2)
                   : 0;
  scroll.hbar->setMaximum (hmax);
  scroll.hbar->setPageSize (int(max_line_width), int(width) - 2);
  scroll.hbar->setY (int(getHeight()));
  scroll.hbar->setWidth (width, false);
  scroll.hbar->resize();

  if ( isHorizontallyScrollable() )
    scroll.hbar->show();
  else
    scroll.hbar->hide();

  if ( isVerticallyScrollable() )
    scroll.vbar->show();
  else
    scroll.vbar->hide();
}


// private methods of FListBox
//----------------------------------------------------------------------
inline auto FListBox::getString (FListBoxItems::iterator iter) -> FString
{
  return iter->getText();
}

//----------------------------------------------------------------------
void FListBox::init()
{
  initScrollbar (scroll.vbar, Orientation::Vertical, this, &FListBox::cb_vbarChange);
  initScrollbar (scroll.hbar, Orientation::Horizontal, this, &FListBox::cb_hbarChange);
  FListBox::setGeometry (FPoint{1, 1}, FSize{5, 4}, false);  // initialize geometry values
  mapKeyFunctions();
}

//----------------------------------------------------------------------
inline void FListBox::mapKeyFunctions()
{
  data.key_map =
  {
    { FKey::Return    , [this] { acceptSelection(); } },
    { FKey::Enter     , [this] { acceptSelection(); } },
    { FKey::Up        , [this] { onePosUp(); } },
    { FKey::Down      , [this] { onePosDown(); } },
    { FKey::Left      , [this] { scrollLeft(); } },
    { FKey::Right     , [this] { scrollRight(); } },
    { FKey::Page_up   , [this] { onePageUp(); } },
    { FKey::Page_down , [this] { onePageDown(); } },
    { FKey::Home      , [this] { firstPos(); } },
    { FKey::End       , [this] { lastPos(); } }
  };

  data.key_map_result =
  {
    { FKey::Insert        , [this] { return changeSelectionAndPosition(); } },
    { FKey::Space         , [this] { return spacebarProcessing(); } },
    { FKey::Erase         , [this] { return deletePreviousCharacter(); } },
    { FKey::Backspace     , [this] { return deletePreviousCharacter(); } },
    { FKey::Escape        , [this] { return skipIncrementalSearch(); } },
    { FKey::Escape_mintty , [this] { return skipIncrementalSearch(); } }
  };
}

//----------------------------------------------------------------------
void FListBox::processKeyAction (FKeyEvent* ev)
{
  const auto idx = ev->key();
  const auto& iter = data.key_map.find(idx);

  if ( iter != data.key_map.end() )
  {
    iter->second();
    ev->accept();
  }
  else
  {
    const auto& iter_result = data.key_map_result.find(idx);

    if ( iter_result != data.key_map_result.end() )
    {
      if ( iter_result->second() )
        ev->accept();
    }
    else if ( keyIncSearchInput(idx) )
    {
      ev->accept();
    }
  }
}

//----------------------------------------------------------------------
void FListBox::draw()
{
  if ( selection.current < 1 )
    selection.current = 1;

  useParentWidgetColor();

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(true);

  drawBorder();

  if ( FVTerm::getFOutput()->isNewFont() && ! scroll.vbar->isShown() )
  {
    setColor();

    for (auto y{2}; y < int(getHeight()); y++)
    {
      print() << FPoint{int(getWidth()) - 1, y}
              << ' ';  // clear right side of the scrollbar
    }
  }

  drawHeadline();

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);

  drawScrollbars();
  drawList();
  updateStatusbar (this);
}

//----------------------------------------------------------------------
void FListBox::drawBorder()
{
  FRect box(FPoint{1, 1}, getSize());
  finalcut::drawListBorder (this, box);
}

//----------------------------------------------------------------------
void FListBox::drawScrollbars() const
{
  if ( ! scroll.hbar->isShown() && isHorizontallyScrollable() )
    scroll.hbar->show();
  else
    scroll.hbar->redraw();

  if ( ! scroll.vbar->isShown() && isVerticallyScrollable() )
    scroll.vbar->show();
  else
    scroll.vbar->redraw();
}


//----------------------------------------------------------------------
void FListBox::drawHeadline()
{
  if ( data.text.isEmpty() )
    return;

  const FString txt{" " + data.text + " "};
  const auto column_width = getColumnWidth(txt);
  print() << FPoint{2, 1};
  const auto& wc = getColorTheme();

  if ( isEnabled() )
    setColor(wc->label_emphasis_fg, wc->label_bg);
  else
    setColor(wc->label_inactive_fg, wc->label_inactive_bg);

  if ( column_width <= getClientWidth() )
    print (txt);
  else
  {
    // Print ellipsis
    print() << getColumnSubString (data.text, 1, getClientWidth() - 2)
            << FColorPair {wc->label_ellipsis_fg, wc->label_bg} << "..";
  }
}

//----------------------------------------------------------------------
void FListBox::drawList()
{
  if ( data.itemlist.empty() || getHeight() <= 2 || getWidth() <= 4 )
    return;

  std::size_t start{};
  std::size_t num(getHeight() - 2);

  if ( num > getCount() )
    num = getCount();

  if ( scroll.last_yoffset >= 0
    && scroll.last_yoffset == scroll.yoffset
    && selection.last_current != int(selection.current) )
  {
    // speed up: redraw only the changed rows
    const auto last_pos = selection.current - std::size_t(scroll.yoffset) - 1;
    const auto current_pos = std::size_t(selection.last_current - scroll.yoffset) - 1;
    start = std::min(last_pos, current_pos);
    num = std::max(last_pos, current_pos) + 1;
  }

  auto iter = index2iterator(start + std::size_t(scroll.yoffset));

  for (std::size_t y = start; y < num && iter != data.itemlist.end() ; y++)
  {
    bool serach_mark{false};
    const bool lineHasBrackets = hasBrackets(iter);

    // Import data via lazy conversion
    lazyConvert (iter, y);

    // Set screen position and attributes
    setLineAttributes ( int(y), isSelected(iter), lineHasBrackets
                      , serach_mark );

    // print the entry
    if ( lineHasBrackets )
    {
      drawListBracketsLine (int(y), iter, serach_mark);
    }
    else  // line has no brackets
    {
      drawListLine (int(y), iter, serach_mark);
    }

    ++iter;
  }

  unsetAttributes();
  scroll.last_yoffset = scroll.yoffset;
  selection.last_current = int(selection.current);
}

//----------------------------------------------------------------------
inline void FListBox::drawListLine ( int y
                                   , FListBoxItems::iterator iter
                                   , bool serach_mark )
{
  const std::size_t inc_len = data.inc_search.getLength();
  const auto& wc = getColorTheme();
  const bool isCurrentLine( y + scroll.yoffset + 1 == int(selection.current) );
  const std::size_t first = std::size_t(scroll.xoffset) + 1;
  const std::size_t max_width = getWidth() - nf_offset - 4;
  const FString element(getColumnSubString (getString(iter), first, max_width));
  auto column_width = getColumnWidth(element);

  if ( FVTerm::getFOutput()->isMonochron() && isCurrentLine && getFlags().focus.focus )
    print (UniChar::BlackRightPointingPointer);  // ►
  else
    print (' ');

  if ( serach_mark )
    setColor ( wc->current_inc_search_element_fg
             , wc->current_element_focus_bg );

  for (std::size_t i{0}; i < element.getLength(); i++)
  {
    if ( serach_mark && i == inc_len && getFlags().focus.focus  )
      setColor ( wc->current_element_focus_fg
               , wc->current_element_focus_bg );

    print (element[i]);
  }

  if ( FVTerm::getFOutput()->isMonochron() && isCurrentLine  && getFlags().focus.focus )
  {
    print (UniChar::BlackLeftPointingPointer);  // ◄
    column_width++;
  }

  for (; column_width < getWidth() - nf_offset - 3; column_width++)
    print (' ');
}

//----------------------------------------------------------------------
inline void FListBox::printLeftBracket (BracketType bracket_type)
{
  if ( bracket_type != BracketType::None )
    print ("\0[({<"[std::size_t(bracket_type)]);
}

//----------------------------------------------------------------------
inline void FListBox::printRightBracket (BracketType bracket_type)
{
  if ( bracket_type != BracketType::None )
    print ("\0])}>"[std::size_t(bracket_type)]);
}

//----------------------------------------------------------------------
inline void FListBox::drawListBracketsLine ( int y
                                           , FListBoxItems::iterator iter
                                           , bool serach_mark )
{
  std::size_t b{0};
  const std::size_t inc_len = data.inc_search.getLength();
  const bool isCurrentLine( y + scroll.yoffset + 1 == int(selection.current) );

  if ( FVTerm::getFOutput()->isMonochron() && isCurrentLine && getFlags().focus.focus )
    print (UniChar::BlackRightPointingPointer);  // ►
  else
    print (' ');

  if ( scroll.xoffset == 0 )
  {
    b = 1;  // Required bracket space
    printLeftBracket (iter->brackets);
  }

  const auto first = std::size_t(scroll.xoffset);
  const std::size_t max_width = getWidth() - nf_offset - 4 - b;
  const FString element(getColumnSubString (getString(iter), first, max_width));
  auto column_width = getColumnWidth(element);
  const std::size_t text_width = getColumnWidth(getString(iter));
  std::size_t i{0};
  const auto& wc = getColorTheme();

  for (; i < element.getLength(); i++)
  {
    if ( serach_mark && i == 0 )
      setColor ( wc->current_inc_search_element_fg
               , wc->current_element_focus_bg );

    if ( serach_mark && i == inc_len )
      setColor ( wc->current_element_focus_fg
               , wc->current_element_focus_bg );

    print (element[i]);
  }

  if ( b + column_width < getWidth() - nf_offset - 4
    && std::size_t(scroll.xoffset) <= text_width )
  {
    if ( serach_mark && i == inc_len )
      setColor ( wc->current_element_focus_fg
               , wc->current_element_focus_bg );

    printRightBracket (iter->brackets);
    column_width++;
  }

  if ( FVTerm::getFOutput()->isMonochron() && isCurrentLine && getFlags().focus.focus )
  {
    print (UniChar::BlackLeftPointingPointer);   // ◄
    column_width++;
  }

  for (; b + column_width < getWidth() - nf_offset - 3; column_width++)
    print (' ');
}

//----------------------------------------------------------------------
inline void FListBox::setInitialLineAttributes (bool isLineSelected) const
{
  const auto& wc = getColorTheme();

  if ( isLineSelected )
  {
    if ( FVTerm::getFOutput()->isMonochron() )
      setBold();
    else
      setColor (wc->selected_list_fg, wc->selected_list_bg);
  }
  else
  {
    if ( FVTerm::getFOutput()->isMonochron() )
      unsetBold();
    else
      setColor (wc->list_fg, wc->list_bg);
  }
}

//----------------------------------------------------------------------
inline void FListBox::setCurrentLineAttributes ( int y
                                               , bool isLineSelected
                                               , bool lineHasBrackets
                                               , bool& serach_mark )
{
  const auto& wc = getColorTheme();
  const std::size_t inc_len = data.inc_search.getLength();
  const std::size_t inc_width = getColumnWidth(data.inc_search);

  if ( getFlags().focus.focus && FVTerm::getFOutput()->getMaxColor() < 16 )
    setBold();

  if ( isLineSelected )
  {
    if ( FVTerm::getFOutput()->isMonochron() )
      setBold();
    else if ( getFlags().focus.focus )
      setColor ( wc->selected_current_element_focus_fg
               , wc->selected_current_element_focus_bg );
    else
      setColor ( wc->selected_current_element_fg
               , wc->selected_current_element_bg );

    setCursorPos ({3, 2 + y});  // first character
  }
  else
  {
    if ( FVTerm::getFOutput()->isMonochron() )
      unsetBold();

    if ( getFlags().focus.focus )
    {
      setColor ( wc->current_element_focus_fg
               , wc->current_element_focus_bg );
      const int b = lineHasBrackets ? 1 : 0;

      if ( inc_len > 0 )  // incremental search
      {
        serach_mark = true;
        // Place the cursor on the last found character
        setCursorPos ({2 + b + int(inc_width), 2 + y});
      }
      else  // only highlighted
        setCursorPos ({3 + b, 2 + y});  // first character
    }
    else
      setColor ( wc->current_element_fg
               , wc->current_element_bg );
  }

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
inline void FListBox::setLineAttributes ( int y
                                        , bool isLineSelected
                                        , bool lineHasBrackets
                                        , bool& serach_mark )
{
  const bool isCurrentLine( y + scroll.yoffset + 1 == int(selection.current) );
  print() << FPoint{2, 2 + y};
  setInitialLineAttributes(isLineSelected);

  if ( isCurrentLine )
  {
    setCurrentLineAttributes(y, isLineSelected, lineHasBrackets, serach_mark);
  }
  else
  {
    if ( FVTerm::getFOutput()->isMonochron() )
      setReverse(true);
    else if ( getFlags().focus.focus && FVTerm::getFOutput()->getMaxColor() < 16 )
      unsetBold();
  }
}

//----------------------------------------------------------------------
inline void FListBox::unsetAttributes() const
{
  if ( FVTerm::getFOutput()->isMonochron() )  // unset for the last element
    setReverse(false);

  unsetBold();
}

//----------------------------------------------------------------------
inline void FListBox::updateDrawing (bool draw_vbar, bool draw_hbar)
{
  if ( isShown() )
    drawList();

  scroll.vbar->setValue (scroll.yoffset);

  if ( draw_vbar )
    scroll.vbar->drawBar();

  scroll.hbar->setValue (scroll.xoffset);

  if ( draw_hbar )
    scroll.hbar->drawBar();

  forceTerminalUpdate();
}

//----------------------------------------------------------------------
void FListBox::recalculateHorizontalBar (std::size_t len, bool has_brackets)
{
  if ( has_brackets )
    len += 2;

  if ( len <= max_line_width )
    return;

  max_line_width = len;

  if ( len >= getWidth() - nf_offset - 3 )
  {
    const int hmax = ( max_line_width > getWidth() - nf_offset - 4 )
                     ? int(max_line_width - getWidth() + nf_offset + 4)
                     : 0;
    scroll.hbar->setMaximum (hmax);
    scroll.hbar->setPageSize (int(max_line_width), int(getWidth() - nf_offset) - 4);
    scroll.hbar->calculateSliderValues();

    if ( isShown() )
    {
      if ( isHorizontallyScrollable() )
        scroll.hbar->show();
      else
        scroll.hbar->hide();
    }
  }
}

//----------------------------------------------------------------------
void FListBox::recalculateVerticalBar (std::size_t element_count) const
{
  const int vmax = ( element_count + 2 > getHeight() )
                   ? int(element_count - getHeight() + 2)
                   : 0;
  scroll.vbar->setMaximum (vmax);
  scroll.vbar->setPageSize (int(element_count), int(getHeight()) - 2);
  scroll.vbar->calculateSliderValues();

  if ( isShown() )
  {
    if ( isVerticallyScrollable() )
      scroll.vbar->show();
    else
      scroll.vbar->hide();
  }
}

//----------------------------------------------------------------------
void FListBox::multiSelection (std::size_t pos)
{
  if ( ! isMultiSelection() )
    return;

  if ( isSelected(pos) )
  {
    selection.mouse_select = false;
    unselectItem(pos);
  }
  else
  {
    selection.mouse_select = true;
    selectItem(pos);
  }

  processSelect();
  selection.select_from_item = int(pos);
}

//----------------------------------------------------------------------
void FListBox::multiSelectionUpTo (std::size_t pos)
{
  std::size_t from{};
  std::size_t to{};

  if ( ! isMultiSelection() )
    return;

  if ( std::size_t(selection.select_from_item) > pos )
  {
    from = pos;
    to = std::size_t(selection.select_from_item) - 1;
  }
  else
  {
    from = std::size_t(selection.select_from_item) + 1;
    to = pos;
  }

  for (std::size_t i = from; i <= to; i++)
  {
    if ( selection.mouse_select )
    {
      selectItem(i);
      processSelect();
    }
    else
    {
      unselectItem(i);
      processSelect();
    }
  }

  selection.select_from_item = int(pos);
}

//----------------------------------------------------------------------
void FListBox::wheelUp (int pagesize)
{
  if ( scroll.yoffset == 0 )
    return;

  scroll.yoffset -= pagesize;

  if ( scroll.yoffset < 0 )
  {
    selection.current -= std::size_t(pagesize) + std::size_t(scroll.yoffset);
    scroll.yoffset = 0;
  }
  else
    selection.current -= std::size_t(pagesize);

  if ( selection.current < 1 )
    selection.current = 1;
}

//----------------------------------------------------------------------
void FListBox::wheelDown (int pagesize)
{
  const auto& element_count = getCount();
  auto yoffset_end = int(element_count - getClientHeight());

  if ( yoffset_end < 0 )
    yoffset_end = 0;

  if ( scroll.yoffset == yoffset_end )
    return;

  scroll.yoffset += pagesize;

  if ( scroll.yoffset > yoffset_end )
  {
    selection.current += std::size_t(pagesize - (scroll.yoffset - yoffset_end));
    scroll.yoffset = yoffset_end;
  }
  else
    selection.current += std::size_t(pagesize);

  if ( selection.current > element_count )
    selection.current = element_count;
}

//----------------------------------------------------------------------
void FListBox::wheelLeft (int pagesize)
{
  if ( getCount() == 0 || scroll.xoffset == 0 )
    return;

  const int xoffset_before = scroll.xoffset;
  scrollLeft (pagesize);
  const bool draw_hbar(xoffset_before != scroll.xoffset);
  updateDrawing (false, draw_hbar);
}

//----------------------------------------------------------------------
void FListBox::wheelRight (int pagesize)
{
  if ( getCount() == 0 )
    return;

  const int xoffset_before = scroll.xoffset;
  scrollRight (pagesize);
  const bool draw_hbar(xoffset_before != scroll.xoffset);
  updateDrawing (false, draw_hbar);
}

//----------------------------------------------------------------------
auto FListBox::dragScrollUp() -> bool
{
  if ( selection.current == 1 )
  {
    drag_scroll = DragScrollMode::None;
    return false;
  }

  prevListItem (scroll.distance);
  return true;
}

//----------------------------------------------------------------------
auto FListBox::dragScrollDown() -> bool
{
  const auto& element_count = getCount();

  if ( selection.current == element_count )
  {
    drag_scroll = DragScrollMode::None;
    return false;
  }

  nextListItem (scroll.distance);
  return true;
}

//----------------------------------------------------------------------
void FListBox::dragUp (MouseButton mouse_button)
{
  if ( isDragging(drag_scroll)
    && scroll.distance < int(getClientHeight()) )
    scroll.distance++;

  if ( ! scroll.timer && selection.current > 1 )
  {
    scroll.timer = true;
    addTimer(scroll.repeat);

    if ( mouse_button == MouseButton::Right )
      drag_scroll = DragScrollMode::SelectUpward;
    else
      drag_scroll = DragScrollMode::Upward;
  }

  if ( selection.current == 1 )
  {
    delOwnTimers();
    drag_scroll = DragScrollMode::None;
  }
}

//----------------------------------------------------------------------
void FListBox::dragDown (MouseButton mouse_button)
{
  if ( isDragging(drag_scroll)
    && scroll.distance < int(getClientHeight()) )
    scroll.distance++;

  if ( ! scroll.timer && selection.current < getCount() )
  {
    scroll.timer = true;
    addTimer(scroll.repeat);

    if ( mouse_button == MouseButton::Right )
      drag_scroll = DragScrollMode::SelectDownward;
    else
      drag_scroll = DragScrollMode::Downward;
  }

  if ( selection.current == getCount() )
  {
    delOwnTimers();
    drag_scroll = DragScrollMode::None;
  }
}

//----------------------------------------------------------------------
void FListBox::stopDragScroll()
{
  delOwnTimers();
  drag_scroll = DragScrollMode::None;
  scroll.distance = 1;
  scroll.timer = false;
}

//----------------------------------------------------------------------
void FListBox::prevListItem (int distance)
{
  if ( selection.current == 1 )
    return;

  if ( selection.current <= std::size_t(distance) )
    selection.current = 1;
  else
    selection.current -= std::size_t(distance);

  if ( selection.current <= std::size_t(scroll.yoffset) )
  {
    if ( scroll.yoffset < distance )
      scroll.yoffset = 0;
    else
      scroll.yoffset -= distance;
  }
}

//----------------------------------------------------------------------
void FListBox::nextListItem (int distance)
{
  const auto& element_count = getCount();
  const auto yoffset_end = int(element_count - getClientHeight());

  if ( selection.current == element_count )
    return;

  if ( selection.current + std::size_t(distance) > element_count  )
    selection.current = element_count;
  else
    selection.current += std::size_t(distance);

  if ( selection.current - std::size_t(scroll.yoffset) > getClientHeight() )
  {
    if ( scroll.yoffset > yoffset_end - distance )
      scroll.yoffset = yoffset_end;
    else
      scroll.yoffset += distance;
  }
}
//----------------------------------------------------------------------
void FListBox::scrollToX (int val)
{
  static constexpr std::size_t padding_space = 2;  // 1 leading + 1 trailing space
  const std::size_t xoffset_end = max_line_width - getClientWidth()
                                + padding_space;

  if ( scroll.xoffset == val )
    return;

  scroll.xoffset = val;

  if ( scroll.xoffset > int(xoffset_end) )
    scroll.xoffset = int(xoffset_end);

  if ( scroll.xoffset < 0 )
    scroll.xoffset = 0;
}

//----------------------------------------------------------------------
void FListBox::scrollToY (int val)
{
  const std::size_t element_count = getCount();
  const auto yoffset_end = int(element_count - getClientHeight());

  if ( scroll.yoffset == val )
    return;

  const int c = int(selection.current) - scroll.yoffset;
  scroll.yoffset = val;

  if ( scroll.yoffset > yoffset_end )
    scroll.yoffset = yoffset_end;

  if ( scroll.yoffset < 0 )
    scroll.yoffset = 0;

  selection.current = std::size_t(scroll.yoffset) + std::size_t(c);

  if ( selection.current < std::size_t(scroll.yoffset) )
    selection.current = std::size_t(scroll.yoffset);

  if ( selection.current > element_count )
    selection.current = element_count;
}

//----------------------------------------------------------------------
void FListBox::scrollLeft (int distance)
{
  if ( scroll.xoffset == 0 )
    return;

  scroll.xoffset -= distance;

  if ( scroll.xoffset < 0 )
    scroll.xoffset = 0;
}

//----------------------------------------------------------------------
void FListBox::scrollRight (int distance)
{
  static constexpr std::size_t padding_space = 2;  // 1 leading + 1 trailing space
  const std::size_t xoffset_end = max_line_width - getClientWidth()
                                + padding_space;
  scroll.xoffset += distance;

  if ( scroll.xoffset == int(xoffset_end) )
    return;

  if ( scroll.xoffset > int(xoffset_end) )
    scroll.xoffset = int(xoffset_end);

  if ( scroll.xoffset < 0 )
    scroll.xoffset = 0;
}

//----------------------------------------------------------------------
inline void FListBox::scrollLeft()
{
  scrollLeft(1);
  data.inc_search.clear();
}

//----------------------------------------------------------------------
inline void FListBox::scrollRight()
{
  scrollRight(1);
  data.inc_search.clear();
}

//----------------------------------------------------------------------
inline void FListBox::onePosUp()
{
  prevListItem (1);
  data.inc_search.clear();
}

//----------------------------------------------------------------------
inline void FListBox::onePosDown()
{
  nextListItem (1);
  data.inc_search.clear();
}

//----------------------------------------------------------------------
inline void FListBox::onePageUp()
{
  const int pagesize = int(getClientHeight()) - 1;
  prevListItem (pagesize);
  data.inc_search.clear();
}

//----------------------------------------------------------------------
inline void FListBox::onePageDown()
{
  const int pagesize = int(getClientHeight()) - 1;
  nextListItem (pagesize);
  data.inc_search.clear();
}

//----------------------------------------------------------------------
inline void FListBox::firstPos()
{
  selection.current = 1;
  scroll.yoffset = 0;
  data.inc_search.clear();
}

//----------------------------------------------------------------------
inline void FListBox::lastPos()
{
  const auto& element_count = getCount();
  const auto yoffset_end = int(element_count - getClientHeight());
  selection.current = element_count;

  if ( selection.current > getClientHeight() )
    scroll.yoffset = yoffset_end;

  data.inc_search.clear();
}

//----------------------------------------------------------------------
inline auto FListBox::isWithinListBounds (const FPoint& pos) const -> bool
{
  return pos.getX() > 1
      && pos.getX() < int(getWidth())
      && pos.getY() > 1
      && pos.getY() < int(getHeight());
}

//----------------------------------------------------------------------
inline auto FListBox::skipIncrementalSearch() -> bool
{
  if ( data.inc_search.getLength() > 0 )
  {
    data.inc_search.clear();
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
inline void FListBox::acceptSelection()
{
  processClick();
  data.inc_search.clear();
}

//----------------------------------------------------------------------
inline auto FListBox::spacebarProcessing() -> bool
{
  const auto& inc_len = data.inc_search.getLength();

  if ( inc_len > 0 )  // Enter a spacebar for incremental search
  {
    data.inc_search += L' ';
    bool inc_found{false};
    auto iter = data.itemlist.begin();

    while ( iter != data.itemlist.end() )
    {
      if ( ! inc_found
        && data.inc_search.toLower()
        == iter->getText().left(inc_len + 1).toLower() )
      {
        setCurrentItem(iter);
        inc_found = true;
        break;
      }

      ++iter;
    }

    if ( ! inc_found )
    {
      data.inc_search.remove(inc_len, 1);
      return false;
    }
  }
  else if ( isMultiSelection() )  // Change selection
  {
    if ( isSelected(selection.current) )
      unselectItem(selection.current);
    else
      selectItem(selection.current);

    processSelect();
    data.inc_search.clear();
  }

  return true;
}

//----------------------------------------------------------------------
inline auto FListBox::changeSelectionAndPosition() -> bool
{
  if ( isMultiSelection() )
  {
    const auto& element_count = getCount();

    if ( isSelected(selection.current) )
      unselectItem(selection.current);
    else
      selectItem(selection.current);

    processSelect();
    selection.current++;

    if ( selection.current > element_count )
      selection.current = element_count;

    if ( selection.current - std::size_t(scroll.yoffset) + 1 >= getHeight() )
      scroll.yoffset++;

    return true;
  }

  data.inc_search.clear();
  return false;
}

//----------------------------------------------------------------------
inline auto FListBox::deletePreviousCharacter() -> bool
{
  const auto& inc_len = data.inc_search.getLength();

  if ( inc_len == 0 )
    return false;

  data.inc_search.remove(inc_len - 1, 1);

  if ( inc_len > 1 )
  {
    auto iter = data.itemlist.begin();

    while ( iter != data.itemlist.end() )
    {
      if ( data.inc_search.toLower()
           == iter->getText().left(inc_len - 1).toLower() )
      {
        setCurrentItem(iter);
        break;
      }

      ++iter;
    }
  }

  return true;
}

//----------------------------------------------------------------------
inline auto FListBox::keyIncSearchInput (FKey key) -> bool
{
  if ( key <= 0x20 || key > 0x10fff )
    return false;

  // incremental search
  if ( data.inc_search.getLength() == 0 )
    data.inc_search = wchar_t(key);
  else
    data.inc_search += wchar_t(key);

  const auto& inc_len = data.inc_search.getLength();
  bool inc_found{false};
  auto iter = data.itemlist.begin();

  while ( iter != data.itemlist.end() )
  {
    if ( ! inc_found
      && data.inc_search.toLower()
      == iter->getText().left(inc_len).toLower() )
    {
      setCurrentItem(iter);
      inc_found = true;
      break;
    }

    ++iter;
  }

  if ( ! inc_found )
  {
    data.inc_search.remove(inc_len - 1, 1);
    return inc_len != 1;
  }

  return true;
}

//----------------------------------------------------------------------
void FListBox::processClick() const
{
  emitCallback("clicked");
}

//----------------------------------------------------------------------
void FListBox::processSelect() const
{
  emitCallback("row-selected");
}

//----------------------------------------------------------------------
void FListBox::processRowChanged() const
{
  emitCallback("row-changed");
}

//----------------------------------------------------------------------
void FListBox::processChanged() const
{
  emitCallback("changed");
}

//----------------------------------------------------------------------
void FListBox::changeOnResize() const
{
  if ( FVTerm::getFOutput()->isNewFont() )
  {
    scroll.vbar->setGeometry (FPoint{int(getWidth()), 2}, FSize{2, getHeight() - 2});
    scroll.hbar->setGeometry (FPoint{1, int(getHeight())}, FSize{getWidth() - 2, 1});
  }
  else
  {
    scroll.vbar->setGeometry (FPoint{int(getWidth()), 2}, FSize{1, getHeight() - 2});
    scroll.hbar->setGeometry (FPoint{2, int(getHeight())}, FSize{getWidth() - 2, 1});
  }
}

//----------------------------------------------------------------------
void FListBox::lazyConvert(FListBoxItems::iterator iter, std::size_t y)
{
  if ( conv_type != ConvertType::Lazy || ! iter->getText().isEmpty() )
    return;

  lazy_inserter (*iter, data.source_container, y + std::size_t(scroll.yoffset));
  const auto column_width = getColumnWidth(iter->text);
  recalculateHorizontalBar (column_width, hasBrackets(iter));

  if ( scroll.hbar->isShown() )
    scroll.hbar->redraw();
}

//----------------------------------------------------------------------
void FListBox::cb_vbarChange (const FWidget*)
{
  const auto scroll_type = scroll.vbar->getScrollType();
  static constexpr int wheel_distance = 4;
  const std::size_t current_before = selection.current;
  int distance{1};
  const int yoffset_before = scroll.yoffset;

  switch ( scroll_type )
  {
    case FScrollbar::ScrollType::PageBackward:
      distance = int(getClientHeight());
      // fall through
    case FScrollbar::ScrollType::StepBackward:
      prevListItem (distance);
      break;

    case FScrollbar::ScrollType::PageForward:
      distance = int(getClientHeight());
      // fall through
    case FScrollbar::ScrollType::StepForward:
      nextListItem (distance);
      break;

    case FScrollbar::ScrollType::Jump:
      scrollToY (scroll.vbar->getValue());
      break;

    case FScrollbar::ScrollType::WheelUp:
    case FScrollbar::ScrollType::WheelLeft:
      wheelUp (wheel_distance);
      break;

    case FScrollbar::ScrollType::WheelDown:
    case FScrollbar::ScrollType::WheelRight:
      wheelDown (wheel_distance);
      break;

    default:
      throw std::invalid_argument{"Invalid scroll type"};
  }

  if ( current_before != selection.current )
  {
    data.inc_search.clear();
    processRowChanged();
  }

  if ( isShown() )
    drawList();

  if ( scroll_type >= FScrollbar::ScrollType::StepBackward )
  {
    scroll.vbar->setValue (scroll.yoffset);

    if ( yoffset_before != scroll.yoffset )
      scroll.vbar->drawBar();

    forceTerminalUpdate();
  }
}

//----------------------------------------------------------------------
void FListBox::cb_hbarChange (const FWidget*)
{
  const auto scroll_type = scroll.hbar->getScrollType();
  static constexpr int wheel_distance = 4;
  static constexpr int padding_space = 2;  // 1 leading space + 1 trailing space
  int distance{1};
  const int xoffset_before = scroll.xoffset;

  switch ( scroll_type )
  {
    case FScrollbar::ScrollType::PageBackward:
      distance = int(getClientWidth()) - padding_space;
      // fall through
    case FScrollbar::ScrollType::StepBackward:
      scrollLeft (distance);
      break;

    case FScrollbar::ScrollType::PageForward:
      distance = int(getClientWidth()) - padding_space;
      // fall through
    case FScrollbar::ScrollType::StepForward:
      scrollRight (distance);
      break;

    case FScrollbar::ScrollType::Jump:
      scrollToX (scroll.hbar->getValue());
      break;

    case FScrollbar::ScrollType::WheelUp:
    case FScrollbar::ScrollType::WheelLeft:
      scrollLeft (wheel_distance);
      break;

    case FScrollbar::ScrollType::WheelDown:
    case FScrollbar::ScrollType::WheelRight:
      scrollRight (wheel_distance);
      break;

    default:
      throw std::invalid_argument{"Invalid scroll type"};
  }

  if ( xoffset_before != scroll.xoffset )
    data.inc_search.clear();

  if ( isShown() )
    drawList();


  if ( scroll_type >= FScrollbar::ScrollType::StepBackward )
  {
    scroll.hbar->setValue (scroll.xoffset);

    if ( xoffset_before != scroll.xoffset )
      scroll.hbar->drawBar();

    forceTerminalUpdate();
  }
}

}  // namespace finalcut
