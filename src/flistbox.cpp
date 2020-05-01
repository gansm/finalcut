/***********************************************************************
* flistbox.cpp - Widget FListBox and FListBoxItem                      *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2014-2020 Markus Gans                                      *
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

#include <algorithm>
#include <memory>

#include "final/fapplication.h"
#include "final/fcolorpair.h"
#include "final/fevent.h"
#include "final/flistbox.h"
#include "final/fstring.h"
#include "final/fstatusbar.h"
#include "final/fwidgetcolors.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FListBoxItem
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FListBoxItem::FListBoxItem()
{ }

//----------------------------------------------------------------------
FListBoxItem::FListBoxItem (const FListBoxItem& item)
  : text(item.text)
  , data_pointer(item.data_pointer)
  , brackets(item.brackets)
  , selected(item.selected)
{ }

//----------------------------------------------------------------------
FListBoxItem::FListBoxItem (const FString& txt, FDataPtr data)
  : text(txt)
  , data_pointer(data)
{ }

//----------------------------------------------------------------------
FListBoxItem::~FListBoxItem()  // destructor
{ }

// public methods of FListBoxItem
//----------------------------------------------------------------------
FListBoxItem& FListBoxItem::operator = (const FListBoxItem& item)
{
  if ( &item == this )
  {
    return *this;
  }
  else
  {
    text         = item.text;
    data_pointer = item.data_pointer;
    brackets     = item.brackets;
    selected     = item.selected;
    return *this;
  }
}


//----------------------------------------------------------------------
// class FListBox
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FListBox::FListBox (FWidget* parent)
  : FWidget(parent)
{
  init();
}

//----------------------------------------------------------------------
FListBox::~FListBox()  // destructor
{
  delOwnTimer();
}


// public methods of FListBox
//----------------------------------------------------------------------
void FListBox::setCurrentItem (std::size_t index)
{
  if ( index == current )
    return;

  const std::size_t element_count = getCount();

  if ( index > element_count )
    current = element_count;
  else if ( index < 1 )
    current = 1;
  else
    current = index;

  xoffset = 0;
  yoffset = 0;
  adjustSize();
  vbar->setValue(yoffset);
  redraw();
}

//----------------------------------------------------------------------
void FListBox::setCurrentItem (listBoxItems::iterator iter)
{
  const auto index = std::size_t(std::distance(itemlist.begin(), iter)) + 1;
  setCurrentItem(index);
}

//----------------------------------------------------------------------
void FListBox::showInsideBrackets ( const std::size_t index
                                  , fc::brackets_type b )
{
  auto iter = index2iterator(index - 1);
  iter->brackets = b;

  if ( b == fc::NoBrackets )
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
      hbar->setMaximum (hmax);
      hbar->setPageSize (int(max_line_width), int(getWidth() - nf_offset - 4));
      hbar->setValue (xoffset);

      if ( ! hbar->isShown() )
        hbar->show();
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
  text.setString(txt);
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
  const std::size_t column_width = getColumnWidth(listItem.text);
  const bool has_brackets(listItem.brackets);
  recalculateHorizontalBar (column_width, has_brackets);

  itemlist.push_back (listItem);

  if ( current == 0 )
    current = 1;

  std::size_t element_count = getCount();
  recalculateVerticalBar (element_count);
}

//----------------------------------------------------------------------
void FListBox::remove (std::size_t item)
{
  if ( item > getCount() )
    return;

  itemlist.erase (itemlist.begin() + int(item) - 1);
  const std::size_t element_count = getCount();
  max_line_width = 0;

  for (auto&& listbox_item : itemlist)
  {
    const auto column_width = getColumnWidth(listbox_item.getText());

    if ( column_width > max_line_width )
      max_line_width = column_width;
  }

  const int hmax = ( max_line_width > getWidth() - nf_offset - 4 )
                   ? int(max_line_width - getWidth() + nf_offset + 4)
                   : 0;
  hbar->setMaximum (hmax);
  hbar->setPageSize (int(max_line_width), int(getWidth() - nf_offset - 4));

  if ( hbar->isShown() && isHorizontallyScrollable() )
    hbar->hide();

  const int vmax = ( element_count + 2 > getHeight() )
                   ? int(element_count - getHeight()) + 2
                   : 0;
  vbar->setMaximum (vmax);
  vbar->setPageSize (int(element_count), int(getHeight()) - 2);

  if ( vbar->isShown() && isVerticallyScrollable() )
    vbar->hide();

  if ( current >= item && current > 1 )
    current--;

  if ( current > element_count )
    current = element_count;

  if ( yoffset > int(element_count - getHeight()) + 2 )
    yoffset = int(element_count - getHeight()) + 2;

  if ( yoffset < 0 )
    yoffset = 0;
}

//----------------------------------------------------------------------
void FListBox::clear()
{
  itemlist.clear();
  itemlist.shrink_to_fit();
  current = 0;
  xoffset = 0;
  yoffset = 0;
  max_line_width = 0;
  last_current = -1;
  last_yoffset = -1;

  vbar->setMinimum(0);
  vbar->setValue(0);
  vbar->hide();

  hbar->setMinimum(0);
  hbar->setValue(0);
  hbar->hide();

  // clear list from screen
  const auto& wc = getFWidgetColors();
  setColor (wc.list_fg, wc.list_bg);
  const std::size_t size = getWidth() - 2;
  drawBorder();
  drawHeadline();

  if ( size == 0 )
    return;

  for (int y{0}; y < int(getHeight()) - 2; y++)
  {
    print() << FPoint{2, 2 + y} << FString{size, L' '};
  }
}

//----------------------------------------------------------------------
void FListBox::onKeyPress (FKeyEvent* ev)
{
  const std::size_t current_before = current;
  const int xoffset_before = xoffset;
  const int yoffset_before = yoffset;
  processKeyAction(ev);  // Process the keystrokes

  if ( current_before != current )
    processChanged();

  if ( ev->isAccepted() )
  {
    const bool draw_vbar( yoffset_before != yoffset );
    const bool draw_hbar( xoffset_before != xoffset );
    updateDrawing (draw_vbar, draw_hbar);
  }
}

//----------------------------------------------------------------------
void FListBox::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton
    && ev->getButton() != fc::RightButton )
  {
    return;
  }

  if ( ev->getButton() == fc::RightButton && ! isMultiSelection() )
    return;

  getWidgetFocus();

  const int yoffset_before = yoffset;
  const std::size_t current_before = current;
  const int mouse_x = ev->getX();
  const int mouse_y = ev->getY();

  if ( mouse_x > 1 && mouse_x < int(getWidth())
    && mouse_y > 1 && mouse_y < int(getHeight()) )
  {
    click_on_list = true;
    const std::size_t element_count = getCount();
    current = std::size_t(yoffset + mouse_y - 1);

    if ( current > element_count )
      current = element_count;

    inc_search.clear();

    if ( ev->getButton() == fc::RightButton )
      multiSelection(current);

    if ( current_before != current )
    {
      processChanged();
    }

    if ( isShown() )
      drawList();

    vbar->setValue (yoffset);

    if ( yoffset_before != yoffset )
      vbar->drawBar();

    updateTerminal();
    flush();
  }
}

//----------------------------------------------------------------------
void FListBox::onMouseUp (FMouseEvent* ev)
{
  click_on_list = false;

  if ( drag_scroll != fc::noScroll )
    stopDragScroll();

  if ( ev->getButton() == fc::LeftButton )
  {
    const int mouse_x = ev->getX();
    const int mouse_y = ev->getY();

    if ( mouse_x > 1 && mouse_x < int(getWidth())
      && mouse_y > 1 && mouse_y < int(getHeight())
      && ! isMultiSelection() )
    {
      processSelect();
    }
  }
}

//----------------------------------------------------------------------
void FListBox::onMouseMove (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton
    && ev->getButton() != fc::RightButton )
    return;

  if ( ev->getButton() == fc::RightButton && ! isMultiSelection() )
    return;

  const std::size_t current_before = current;
  const int yoffset_before = yoffset;
  const int mouse_x = ev->getX();
  const int mouse_y = ev->getY();

  if ( mouse_x > 1 && mouse_x < int(getWidth())
    && mouse_y > 1 && mouse_y < int(getHeight()) )
  {
    click_on_list = true;
    const std::size_t element_count = getCount();
    current = std::size_t(yoffset + mouse_y - 1);

    if ( current > element_count )
      current = element_count;

    inc_search.clear();

    if ( current_before != current )
    {
      // Handle multiple selections + changes
      if ( ev->getButton() == fc::RightButton)
      {
        processChanged();
        multiSelectionUpTo(current);
      }
      else if ( ev->getButton() == fc::LeftButton)
        processChanged();
    }

    if ( isShown() )
      drawList();

    vbar->setValue (yoffset);

    if ( yoffset_before != yoffset )
      vbar->drawBar();

    updateTerminal();
    flush();
  }

  // Auto-scrolling when dragging mouse outside the widget
  if ( click_on_list && mouse_y < 2 )
    dragUp (ev->getButton());
  else if ( click_on_list && mouse_y >= int(getHeight()) )
    dragDown (ev->getButton());
  else
    stopDragScroll();
}

//----------------------------------------------------------------------
void FListBox::onMouseDoubleClick (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton )
    return;

  const int mouse_x = ev->getX();
  const int mouse_y = ev->getY();

  if ( mouse_x > 1 && mouse_x < int(getWidth())
    && mouse_y > 1 && mouse_y < int(getHeight()) )
  {
    if ( yoffset + mouse_y - 1 > int(getCount()) )
      return;

    processClick();
  }
}

//----------------------------------------------------------------------
void FListBox::onTimer (FTimerEvent*)
{
  const std::size_t current_before = current;
  const int yoffset_before = yoffset;

  switch ( int(drag_scroll) )
  {
    case fc::noScroll:
      return;

    case fc::scrollUp:
    case fc::scrollUpSelect:
      if ( ! dragScrollUp() )
        return;
      break;

    case fc::scrollDown:
    case fc::scrollDownSelect:
      if ( ! dragScrollDown() )
        return;
      break;

    default:
      break;
  }

  if ( current_before != current )
  {
    inc_search.clear();
    processChanged();

    // Handle multiple selections
    if ( drag_scroll == fc::scrollUpSelect
      || drag_scroll == fc::scrollDownSelect )
      multiSelectionUpTo(current);
  }

  if ( isShown() )
    drawList();

  vbar->setValue (yoffset);

  if ( yoffset_before != yoffset )
    vbar->drawBar();

  updateTerminal();
  flush();
}

//----------------------------------------------------------------------
void FListBox::onWheel (FWheelEvent* ev)
{
  const std::size_t current_before = current;
  const int yoffset_before = yoffset;
  static constexpr int wheel_distance = 4;
  const int wheel = ev->getWheel();

  if ( drag_scroll != fc::noScroll )
    stopDragScroll();

  switch ( wheel )
  {
    case fc::WheelUp:
      wheelUp (wheel_distance);
      break;

    case fc::WheelDown:
      wheelDown (wheel_distance);
      break;

    default:
      break;
  }

  if ( current_before != current )
  {
    inc_search.clear();
    processChanged();
  }

  if ( isShown() )
    drawList();

  vbar->setValue (yoffset);

  if ( yoffset_before != yoffset )
    vbar->drawBar();

  updateTerminal();
  flush();
}

//----------------------------------------------------------------------
void FListBox::onFocusIn (FFocusEvent*)
{
  if ( getStatusBar() )
    getStatusBar()->drawMessage();

  inc_search.clear();
}

//----------------------------------------------------------------------
void FListBox::onFocusOut (FFocusEvent*)
{
  if ( getStatusBar() )
  {
    getStatusBar()->clearMessage();
    getStatusBar()->drawMessage();
  }

  delOwnTimer();
  inc_search.clear();
}


// protected methods of FListBox
//----------------------------------------------------------------------
void FListBox::adjustYOffset (std::size_t element_count)
{
  const std::size_t height = getClientHeight();

  if ( height == 0 || element_count == 0 )
    return;

  if ( yoffset > int(element_count - height) )
    yoffset = int(element_count - height);

  if ( yoffset < 0 )
    yoffset = 0;

  if ( current < std::size_t(yoffset) )
    current = std::size_t(yoffset);

  if ( yoffset < int(current - height) )
    yoffset = int(current - height);
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
  vbar->setMaximum (vmax);
  vbar->setPageSize (int(element_count), int(height));
  vbar->setX (int(getWidth()));
  vbar->setHeight (height, false);
  vbar->resize();

  const int hmax = ( max_line_width + 2 > width )
                   ? int(max_line_width - width + 2)
                   : 0;
  hbar->setMaximum (hmax);
  hbar->setPageSize (int(max_line_width), int(width) - 2);
  hbar->setY (int(getHeight()));
  hbar->setWidth (width, false);
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


// private methods of FListBox
//----------------------------------------------------------------------
inline FString& FListBox::getString (listBoxItems::iterator iter)
{
  return iter->getText();
}

//----------------------------------------------------------------------
void FListBox::init()
{
  initScrollbar (vbar, fc::vertical, this, &FListBox::cb_vbarChange);
  initScrollbar (hbar, fc::horizontal, this, &FListBox::cb_hbarChange);
  setGeometry (FPoint{1, 1}, FSize{5, 4}, false);  // initialize geometry values
  const auto& wc = getFWidgetColors();
  setForegroundColor (wc.dialog_fg);
  setBackgroundColor (wc.dialog_bg);
  nf_offset = isNewFont() ? 1 : 0;
  setTopPadding(1);
  setLeftPadding(1);
  setBottomPadding(1);
  setRightPadding(1);
  mapKeyFunctions();
}

//----------------------------------------------------------------------
inline void FListBox::mapKeyFunctions()
{
  key_map[fc::Fkey_return] = std::bind(&FListBox::acceptSelection, this);
  key_map[fc::Fkey_enter]  = std::bind(&FListBox::acceptSelection, this);
  key_map[fc::Fkey_up]     = std::bind(&FListBox::onePosUp, this);
  key_map[fc::Fkey_down]   = std::bind(&FListBox::onePosDown, this);
  auto left = static_cast<void(FListBox::*)()>(&FListBox::scrollLeft);
  key_map[fc::Fkey_left]   = std::bind(left, this);
  auto right = static_cast<void(FListBox::*)()>(&FListBox::scrollRight);
  key_map[fc::Fkey_right]  = std::bind(right, this);
  key_map[fc::Fkey_ppage]  = std::bind(&FListBox::onePageUp, this);
  key_map[fc::Fkey_npage]  = std::bind(&FListBox::onePageDown, this);
  key_map[fc::Fkey_home]   = std::bind(&FListBox::firstPos, this);
  key_map[fc::Fkey_end]    = std::bind(&FListBox::lastPos, this);
  key_map_result[fc::Fkey_ic] = \
      std::bind(&FListBox::changeSelectionAndPosition, this);
  key_map_result[fc::Fkey_space] = \
      std::bind(&FListBox::spacebarProcessing, this);
  key_map_result[fc::Fkey_erase] = \
      std::bind(&FListBox::deletePreviousCharacter, this);
  key_map_result[fc::Fkey_backspace] = \
      std::bind(&FListBox::deletePreviousCharacter, this);
  key_map_result[fc::Fkey_escape] = \
      std::bind(&FListBox::skipIncrementalSearch, this);
  key_map_result[fc::Fkey_escape_mintty] = \
      std::bind(&FListBox::skipIncrementalSearch, this);
}

//----------------------------------------------------------------------
void FListBox::processKeyAction (FKeyEvent* ev)
{
  const int idx = int(ev->key());

  if ( key_map.find(idx) != key_map.end() )
  {
    key_map[idx]();
    ev->accept();
  }
  else if ( key_map_result.find(idx) != key_map_result.end() )
  {
    if ( key_map_result[idx]() )
      ev->accept();
  }
  else if ( keyIncSearchInput(ev->key()) )
  {
    ev->accept();
  }
}

//----------------------------------------------------------------------
void FListBox::draw()
{
  if ( current < 1 )
    current = 1;

  setColor();

  if ( isMonochron() )
    setReverse(true);

  drawBorder();

  if ( isNewFont() && ! vbar->isShown() )
  {
    setColor();

    for (int y{2}; y < int(getHeight()); y++)
    {
      print() << FPoint{int(getWidth()) - 1, y}
              << ' ';  // clear right side of the scrollbar
    }
  }

  drawHeadline();

  if ( isMonochron() )
    setReverse(false);

  drawScrollbars();
  drawList();

  if ( getFlags().focus && getStatusBar() )
  {
    const auto& msg = getStatusbarMessage();
    const auto& curMsg = getStatusBar()->getMessage();

    if ( curMsg != msg )
    {
      getStatusBar()->setMessage(msg);
      getStatusBar()->drawMessage();
    }
  }
}

//----------------------------------------------------------------------
void FListBox::drawBorder()
{
  FRect box(FPoint{1, 1}, getSize());
  finalcut::drawListBorder (this, box);
}

//----------------------------------------------------------------------
void FListBox::drawScrollbars()
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
void FListBox::drawHeadline()
{
  if ( text.isNull() || text.isEmpty() )
    return;

  const FString txt{" " + text + " "};
  const auto column_width = getColumnWidth(txt);
  print() << FPoint{2, 1};
  const auto& wc = getFWidgetColors();

  if ( isEnabled() )
    setColor(wc.label_emphasis_fg, wc.label_bg);
  else
    setColor(wc.label_inactive_fg, wc.label_inactive_bg);

  if ( column_width <= getClientWidth() )
    print (txt);
  else
  {
    // Print ellipsis
    print() << getColumnSubString (text, 1, getClientWidth() - 2)
            << FColorPair {wc.label_ellipsis_fg, wc.label_bg} << "..";
  }
}

//----------------------------------------------------------------------
void FListBox::drawList()
{
  if ( itemlist.empty() || getHeight() <= 2 || getWidth() <= 4 )
    return;

  std::size_t start{};
  std::size_t num(getHeight() - 2);

  if ( num > getCount() )
    num = getCount();

  if ( last_yoffset >= 0
    && last_yoffset == yoffset
    && last_current != int(current) )
  {
    // speed up: redraw only the changed rows
    const auto last_pos = current - std::size_t(yoffset) - 1;
    const auto current_pos = std::size_t(last_current - yoffset) - 1;
    start = std::min(last_pos, current_pos);
    num = std::max(last_pos, current_pos) + 1;
  }

  auto iter = index2iterator(start + std::size_t(yoffset));

  for (std::size_t y = start; y < num && iter != itemlist.end() ; y++)
  {
    bool serach_mark{false};
    const bool lineHasBrackets = hasBrackets(iter);

    // Import data via lazy conversion
    lazyConvert (iter, int(y));

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
  last_yoffset = yoffset;
  last_current = int(current);
}

//----------------------------------------------------------------------
inline void FListBox::drawListLine ( int y
                                   , listBoxItems::iterator iter
                                   , bool serach_mark )
{
  const std::size_t inc_len = inc_search.getLength();
  const auto& wc = getFWidgetColors();
  const bool isCurrentLine( y + yoffset + 1 == int(current) );
  const std::size_t first = std::size_t(xoffset) + 1;
  const std::size_t max_width = getWidth() - nf_offset - 4;
  const FString element(getColumnSubString (getString(iter), first, max_width));
  std::size_t column_width = getColumnWidth(element);

  if ( isMonochron() && isCurrentLine && getFlags().focus )
    print (fc::BlackRightPointingPointer);  // ►
  else
    print (' ');

  if ( serach_mark )
    setColor ( wc.current_inc_search_element_fg
             , wc.current_element_focus_bg );

  for (std::size_t i{0}; i < element.getLength(); i++)
  {
    if ( serach_mark && i == inc_len && getFlags().focus  )
      setColor ( wc.current_element_focus_fg
               , wc.current_element_focus_bg );

    print (element[i]);
  }

  if ( isMonochron() && isCurrentLine  && getFlags().focus )
  {
    print (fc::BlackLeftPointingPointer);  // ◄
    column_width++;
  }

  for (; column_width < getWidth() - nf_offset - 3; column_width++)
    print (' ');
}

//----------------------------------------------------------------------
inline void FListBox::printLeftBracket (fc::brackets_type bracket_type)
{
  if ( bracket_type != fc::NoBrackets )
    print ("\0[({<"[bracket_type]);
}

//----------------------------------------------------------------------
inline void FListBox::printRightBracket (fc::brackets_type bracket_type)
{
  if ( bracket_type != fc::NoBrackets )
    print ("\0])}>"[bracket_type]);
}

//----------------------------------------------------------------------
inline void FListBox::drawListBracketsLine ( int y
                                           , listBoxItems::iterator iter
                                           , bool serach_mark )
{
  std::size_t b{0};
  const std::size_t inc_len = inc_search.getLength();
  const bool isCurrentLine( y + yoffset + 1 == int(current) );

  if ( isMonochron() && isCurrentLine && getFlags().focus )
    print (fc::BlackRightPointingPointer);  // ►
  else
    print (' ');

  if ( xoffset == 0 )
  {
    b = 1;  // Required bracket space
    printLeftBracket (iter->brackets);
  }

  const std::size_t first = std::size_t(xoffset);
  const std::size_t max_width = getWidth() - nf_offset - 4 - b;
  const FString element(getColumnSubString (getString(iter), first, max_width));
  std::size_t column_width = getColumnWidth(element);
  const std::size_t text_width = getColumnWidth(getString(iter));
  std::size_t i{0};
  const auto& wc = getFWidgetColors();

  for (; i < element.getLength(); i++)
  {
    if ( serach_mark && i == 0 )
      setColor ( wc.current_inc_search_element_fg
               , wc.current_element_focus_bg );

    if ( serach_mark && i == inc_len )
      setColor ( wc.current_element_focus_fg
               , wc.current_element_focus_bg );

    print (element[i]);
  }

  if ( b + column_width < getWidth() - nf_offset - 4
    && std::size_t(xoffset) <= text_width )
  {
    if ( serach_mark && i == inc_len )
      setColor ( wc.current_element_focus_fg
               , wc.current_element_focus_bg );

    printRightBracket (iter->brackets);
    column_width++;
  }

  if ( isMonochron() && isCurrentLine && getFlags().focus )
  {
    print (fc::BlackLeftPointingPointer);   // ◄
    column_width++;
  }

  for (; b + column_width < getWidth() - nf_offset - 3; column_width++)
    print (' ');
}

//----------------------------------------------------------------------
inline void FListBox::setLineAttributes ( int y
                                        , bool isLineSelected
                                        , bool lineHasBrackets
                                        , bool& serach_mark )
{
  const bool isCurrentLine( y + yoffset + 1 == int(current) );
  const std::size_t inc_len = inc_search.getLength();
  const std::size_t inc_width = getColumnWidth(inc_search);
  const auto& wc = getFWidgetColors();
  print() << FPoint{2, 2 + int(y)};

  if ( isLineSelected )
  {
    if ( isMonochron() )
      setBold();
    else
      setColor (wc.selected_list_fg, wc.selected_list_bg);
  }
  else
  {
    if ( isMonochron() )
      unsetBold();
    else
      setColor (wc.list_fg, wc.list_bg);
  }

  if ( isCurrentLine )
  {
    if ( getFlags().focus && getMaxColor() < 16 )
      setBold();

    if ( isLineSelected )
    {
      if ( isMonochron() )
        setBold();
      else if ( getFlags().focus )
        setColor ( wc.selected_current_element_focus_fg
                 , wc.selected_current_element_focus_bg );
      else
        setColor ( wc.selected_current_element_fg
                 , wc.selected_current_element_bg );

      setCursorPos ({3, 2 + int(y)});  // first character
    }
    else
    {
      if ( isMonochron() )
        unsetBold();

      if ( getFlags().focus )
      {
        setColor ( wc.current_element_focus_fg
                 , wc.current_element_focus_bg );
        const int b = ( lineHasBrackets ) ? 1: 0;

        if ( inc_len > 0 )  // incremental search
        {
          serach_mark = true;
          // Place the cursor on the last found character
          setCursorPos ({2 + b + int(inc_width), 2 + int(y)});
        }
        else  // only highlighted
          setCursorPos ({3 + b, 2 + int(y)});  // first character
      }
      else
        setColor ( wc.current_element_fg
                 , wc.current_element_bg );
    }

    if ( isMonochron() )
      setReverse(false);
  }
  else
  {
    if ( isMonochron() )
      setReverse(true);
    else if ( getFlags().focus && getMaxColor() < 16 )
      unsetBold();
  }
}

//----------------------------------------------------------------------
inline void FListBox::unsetAttributes()
{
  if ( isMonochron() )  // unset for the last element
    setReverse(false);

  unsetBold();
}

//----------------------------------------------------------------------
inline void FListBox::updateDrawing (bool draw_vbar, bool draw_hbar)
{
  if ( isShown() )
    drawList();

  vbar->setValue (yoffset);

  if ( draw_vbar )
    vbar->drawBar();

  hbar->setValue (xoffset);

  if ( draw_hbar )
    hbar->drawBar();

  updateTerminal();
  flush();
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
    hbar->setMaximum (hmax);
    hbar->setPageSize (int(max_line_width), int(getWidth() - nf_offset) - 4);
    hbar->calculateSliderValues();

    if ( isShown() )
    {
      if ( isHorizontallyScrollable() )
        hbar->show();
      else
        hbar->hide();
    }
  }
}

//----------------------------------------------------------------------
void FListBox::recalculateVerticalBar (std::size_t element_count)
{
  const int vmax = ( element_count + 2 > getHeight() )
                   ? int(element_count - getHeight() + 2)
                   : 0;
  vbar->setMaximum (vmax);
  vbar->setPageSize (int(element_count), int(getHeight()) - 2);
  vbar->calculateSliderValues();

  if ( isShown() )
  {
    if ( isVerticallyScrollable() )
      vbar->show();
    else
      vbar->hide();
  }
}

//----------------------------------------------------------------------
inline void FListBox::getWidgetFocus()
{
  if ( hasFocus() )
    return;

  auto focused_widget = getFocusWidget();
  setFocus();

  if ( focused_widget )
    focused_widget->redraw();

  if ( getStatusBar() )
    getStatusBar()->drawMessage();
}

//----------------------------------------------------------------------
void FListBox::multiSelection (std::size_t pos)
{
  if ( ! isMultiSelection() )
    return;

  if ( isSelected(pos) )
  {
    mouse_select = false;
    unselectItem(pos);
  }
  else
  {
    mouse_select = true;
    selectItem(pos);
  }

  processSelect();
  secect_from_item = int(pos);
}

//----------------------------------------------------------------------
void FListBox::multiSelectionUpTo (std::size_t pos)
{
  std::size_t from{};
  std::size_t to{};

  if ( ! isMultiSelection() )
    return;

  if ( std::size_t(secect_from_item) > pos )
  {
    from = pos;
    to = std::size_t(secect_from_item) - 1;
  }
  else
  {
    from = std::size_t(secect_from_item) + 1;
    to = pos;
  }

  for (std::size_t i = from; i <= to; i++)
  {
    if ( mouse_select )
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

  secect_from_item = int(pos);
}

//----------------------------------------------------------------------
void FListBox::wheelUp (int pagesize)
{
  if ( yoffset == 0 )
    return;

  yoffset -= pagesize;

  if ( yoffset < 0 )
  {
    current -= std::size_t(pagesize) + std::size_t(yoffset);
    yoffset = 0;
  }
  else
    current -= std::size_t(pagesize);

  if ( current < 1 )
    current = 1;
}

//----------------------------------------------------------------------
void FListBox::wheelDown (int pagesize)
{
  const std::size_t element_count = getCount();
  int yoffset_end = int(element_count - getClientHeight());

  if ( yoffset_end < 0 )
    yoffset_end = 0;

  if ( yoffset == yoffset_end )
    return;

  yoffset += pagesize;

  if ( yoffset > yoffset_end )
  {
    current += std::size_t(pagesize - (yoffset - yoffset_end));
    yoffset = yoffset_end;
  }
  else
    current += std::size_t(pagesize);

  if ( current > element_count )
    current = element_count;
}

//----------------------------------------------------------------------
bool FListBox::dragScrollUp()
{
  if ( current == 1 )
  {
    drag_scroll = fc::noScroll;
    return false;
  }

  prevListItem (scroll_distance);
  return true;
}

//----------------------------------------------------------------------
bool FListBox::dragScrollDown()
{
  const std::size_t element_count = getCount();

  if ( current == element_count )
  {
    drag_scroll = fc::noScroll;
    return false;
  }

  nextListItem (scroll_distance);
  return true;
}

//----------------------------------------------------------------------
void FListBox::dragUp (int mouse_button)
{
  if ( drag_scroll != fc::noScroll
    && scroll_distance < int(getClientHeight()) )
    scroll_distance++;

  if ( ! scroll_timer && current > 1 )
  {
    scroll_timer = true;
    addTimer(scroll_repeat);

    if ( mouse_button == fc::RightButton )
      drag_scroll = fc::scrollUpSelect;
    else
      drag_scroll = fc::scrollUp;
  }

  if ( current == 1 )
  {
    delOwnTimer();
    drag_scroll = fc::noScroll;
  }
}

//----------------------------------------------------------------------
void FListBox::dragDown (int mouse_button)
{
  if ( drag_scroll != fc::noScroll
    && scroll_distance < int(getClientHeight()) )
    scroll_distance++;

  if ( ! scroll_timer && current < getCount() )
  {
    scroll_timer = true;
    addTimer(scroll_repeat);

    if ( mouse_button == fc::RightButton )
      drag_scroll = fc::scrollDownSelect;
    else
      drag_scroll = fc::scrollDown;
  }

  if ( current == getCount() )
  {
    delOwnTimer();
    drag_scroll = fc::noScroll;
  }
}

//----------------------------------------------------------------------
void FListBox::stopDragScroll()
{
  delOwnTimer();
  drag_scroll = fc::noScroll;
  scroll_distance = 1;
  scroll_timer = false;
}

//----------------------------------------------------------------------
void FListBox::prevListItem (int distance)
{
  if ( current == 1 )
    return;

  if ( current <= std::size_t(distance) )
    current = 1;
  else
    current -= std::size_t(distance);

  if ( current <= std::size_t(yoffset) )
  {
    if ( yoffset < distance )
      yoffset = 0;
    else
      yoffset -= distance;
  }
}

//----------------------------------------------------------------------
void FListBox::nextListItem (int distance)
{
  const std::size_t element_count = getCount();
  const int yoffset_end = int(element_count - getClientHeight());

  if ( current == element_count )
    return;

  if ( current + std::size_t(distance) > element_count  )
    current = element_count;
  else
    current += std::size_t(distance);

  if ( current - std::size_t(yoffset) > getClientHeight() )
  {
    if ( yoffset > yoffset_end - distance )
      yoffset = yoffset_end;
    else
      yoffset += distance;
  }
}
//----------------------------------------------------------------------
void FListBox::scrollToX (int val)
{
  static constexpr std::size_t padding_space = 2;  // 1 leading + 1 trailing space
  const std::size_t xoffset_end = max_line_width - getClientWidth()
                                + padding_space;

  if ( xoffset == val )
    return;

  xoffset = val;

  if ( xoffset > int(xoffset_end) )
    xoffset = int(xoffset_end);

  if ( xoffset < 0 )
    xoffset = 0;
}

//----------------------------------------------------------------------
void FListBox::scrollToY (int val)
{
  const std::size_t element_count = getCount();
  const int yoffset_end = int(element_count - getClientHeight());

  if ( yoffset == val )
    return;

  const int c = int(current) - yoffset;
  yoffset = val;

  if ( yoffset > yoffset_end )
    yoffset = yoffset_end;

  if ( yoffset < 0 )
    yoffset = 0;

  current = std::size_t(yoffset) + std::size_t(c);

  if ( current < std::size_t(yoffset) )
    current = std::size_t(yoffset);

  if ( current > element_count )
    current = element_count;
}

//----------------------------------------------------------------------
void FListBox::scrollLeft (int distance)
{
  if ( xoffset == 0 )
    return;

  xoffset -= distance;

  if ( xoffset < 0 )
    xoffset = 0;
}

//----------------------------------------------------------------------
void FListBox::scrollRight (int distance)
{
  static constexpr std::size_t padding_space = 2;  // 1 leading + 1 trailing space
  const std::size_t xoffset_end = max_line_width - getClientWidth()
                                + padding_space;
  xoffset += distance;

  if ( xoffset == int(xoffset_end) )
    return;

  if ( xoffset > int(xoffset_end) )
    xoffset = int(xoffset_end);

  if ( xoffset < 0 )
    xoffset = 0;
}

//----------------------------------------------------------------------
inline void FListBox::scrollLeft()
{
  scrollLeft(1);
  inc_search.clear();
}

//----------------------------------------------------------------------
inline void FListBox::scrollRight()
{
  scrollRight(1);
  inc_search.clear();
}

//----------------------------------------------------------------------
inline void FListBox::onePosUp()
{
  prevListItem (1);
  inc_search.clear();
}

//----------------------------------------------------------------------
inline void FListBox::onePosDown()
{
  nextListItem (1);
  inc_search.clear();
}

//----------------------------------------------------------------------
inline void FListBox::onePageUp()
{
  const int pagesize = int(getClientHeight()) - 1;
  prevListItem (pagesize);
  inc_search.clear();
}

//----------------------------------------------------------------------
inline void FListBox::onePageDown()
{
  const int pagesize = int(getClientHeight()) - 1;
  nextListItem (pagesize);
  inc_search.clear();
}

//----------------------------------------------------------------------
inline void FListBox::firstPos()
{
  current = 1;
  yoffset = 0;
  inc_search.clear();
}

//----------------------------------------------------------------------
inline void FListBox::lastPos()
{
  const std::size_t element_count = getCount();
  const int yoffset_end = int(element_count - getClientHeight());
  current = element_count;

  if ( current > getClientHeight() )
    yoffset = yoffset_end;

  inc_search.clear();
}

//----------------------------------------------------------------------
inline bool FListBox::skipIncrementalSearch()
{
  if ( inc_search.getLength() > 0 )
  {
    inc_search.clear();
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
inline void FListBox::acceptSelection()
{
  processClick();
  inc_search.clear();
}

//----------------------------------------------------------------------
inline bool FListBox::spacebarProcessing()
{
  const std::size_t inc_len = inc_search.getLength();

  if ( inc_len > 0 )  // Enter a spacebar for incremental search
  {
    inc_search += L' ';
    bool inc_found{false};
    auto iter = itemlist.begin();

    while ( iter != itemlist.end() )
    {
      if ( ! inc_found
        && inc_search.toLower()
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
      inc_search.remove(inc_len, 1);
      return false;
    }
  }
  else if ( isMultiSelection() )  // Change selection
  {
    if ( isSelected(current) )
      unselectItem(current);
    else
      selectItem(current);

    processSelect();
    inc_search.clear();
  }

  return true;
}

//----------------------------------------------------------------------
inline bool FListBox::changeSelectionAndPosition()
{
  if ( isMultiSelection() )
  {
    std::size_t element_count = getCount();

    if ( isSelected(current) )
      unselectItem(current);
    else
      selectItem(current);

    processSelect();
    current++;

    if ( current > element_count )
      current = element_count;

    if ( current - std::size_t(yoffset) + 1 >= getHeight() )
      yoffset++;

    return true;
  }

  inc_search.clear();
  return false;
}

//----------------------------------------------------------------------
inline bool FListBox::deletePreviousCharacter()
{
  const std::size_t inc_len = inc_search.getLength();

  if ( inc_len > 0 )
  {
    inc_search.remove(inc_len - 1, 1);

    if ( inc_len > 1 )
    {
      auto iter = itemlist.begin();

      while ( iter != itemlist.end() )
      {
        if ( inc_search.toLower()
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

  return false;
}

//----------------------------------------------------------------------
inline bool FListBox::keyIncSearchInput (FKey key)
{
  if ( key <= 0x20 || key > 0x10fff )
    return false;

  // incremental search
  if ( inc_search.getLength() == 0 )
    inc_search = wchar_t(key);
  else
    inc_search += wchar_t(key);

  std::size_t inc_len = inc_search.getLength();
  bool inc_found{false};
  auto iter = itemlist.begin();

  while ( iter != itemlist.end() )
  {
    if ( ! inc_found
      && inc_search.toLower()
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
    inc_search.remove(inc_len - 1, 1);

    if ( inc_len == 1 )
      return false;
    else
      return true;
  }

  return true;
}

//----------------------------------------------------------------------
void FListBox::processClick()
{
  emitCallback("clicked");
}

//----------------------------------------------------------------------
void FListBox::processSelect()
{
  emitCallback("row-selected");
}

//----------------------------------------------------------------------
void FListBox::processChanged()
{
  emitCallback("row-changed");
}

//----------------------------------------------------------------------
void FListBox::changeOnResize()
{
  if ( isNewFont() )
  {
    vbar->setGeometry (FPoint{int(getWidth()), 2}, FSize{2, getHeight() - 2});
    hbar->setGeometry (FPoint{1, int(getHeight())}, FSize{getWidth() - 2, 1});
  }
  else
  {
    vbar->setGeometry (FPoint{int(getWidth()), 2}, FSize{1, getHeight() - 2});
    hbar->setGeometry (FPoint{2, int(getHeight())}, FSize{getWidth() - 2, 1});
  }
}

//----------------------------------------------------------------------
void FListBox::lazyConvert(listBoxItems::iterator iter, int y)
{
  if ( conv_type != lazy_convert || ! iter->getText().isNull() )
    return;

  lazy_inserter (*iter, source_container, y + yoffset);
  const auto column_width = getColumnWidth(iter->text);
  recalculateHorizontalBar (column_width, hasBrackets(iter));

  if ( hbar->isShown() )
    hbar->redraw();
}

//----------------------------------------------------------------------
void FListBox::cb_vbarChange (const FWidget*, const FDataPtr)
{
  FScrollbar::sType scrollType;
  const std::size_t current_before = current;
  static constexpr int wheel_distance = 4;
  int distance{1};
  const int yoffset_before = yoffset;
  scrollType = vbar->getScrollType();
  assert ( scrollType == FScrollbar::noScroll
        || scrollType == FScrollbar::scrollJump
        || scrollType == FScrollbar::scrollStepBackward
        || scrollType == FScrollbar::scrollStepForward
        || scrollType == FScrollbar::scrollPageBackward
        || scrollType == FScrollbar::scrollPageForward
        || scrollType == FScrollbar::scrollWheelUp
        || scrollType == FScrollbar::scrollWheelDown );

  switch ( scrollType )
  {
    case FScrollbar::noScroll:
      break;

    case FScrollbar::scrollPageBackward:
      distance = int(getClientHeight());
      // fall through
    case FScrollbar::scrollStepBackward:
      prevListItem (distance);
      break;

    case FScrollbar::scrollPageForward:
      distance = int(getClientHeight());
      // fall through
    case FScrollbar::scrollStepForward:
      nextListItem (distance);
      break;

    case FScrollbar::scrollJump:
      scrollToY (vbar->getValue());
      break;

    case FScrollbar::scrollWheelUp:
      wheelUp (wheel_distance);
      break;

    case FScrollbar::scrollWheelDown:
      wheelDown (wheel_distance);
      break;
  }

  if ( current_before != current )
  {
    inc_search.clear();
    processChanged();
  }

  if ( isShown() )
    drawList();

  if ( scrollType >= FScrollbar::scrollStepBackward )
  {
    vbar->setValue (yoffset);

    if ( yoffset_before != yoffset )
      vbar->drawBar();

    updateTerminal();
    flush();
  }
}

//----------------------------------------------------------------------
void FListBox::cb_hbarChange (const FWidget*, const FDataPtr)
{
  static constexpr int padding_space = 2;  // 1 leading space + 1 trailing space
  static constexpr int wheel_distance = 4;
  FScrollbar::sType scrollType;
  int distance{1};
  const int xoffset_before = xoffset;
  scrollType = hbar->getScrollType();
  assert ( scrollType == FScrollbar::noScroll
        || scrollType == FScrollbar::scrollJump
        || scrollType == FScrollbar::scrollStepBackward
        || scrollType == FScrollbar::scrollStepForward
        || scrollType == FScrollbar::scrollPageBackward
        || scrollType == FScrollbar::scrollPageForward
        || scrollType == FScrollbar::scrollWheelUp
        || scrollType == FScrollbar::scrollWheelDown );

  switch ( scrollType )
  {
    case FScrollbar::noScroll:
      break;

    case FScrollbar::scrollPageBackward:
      distance = int(getClientWidth()) - padding_space;
      // fall through
    case FScrollbar::scrollStepBackward:
      scrollLeft (distance);
      break;

    case FScrollbar::scrollPageForward:
      distance = int(getClientWidth()) - padding_space;
      // fall through
    case FScrollbar::scrollStepForward:
      scrollRight (distance);
      break;

    case FScrollbar::scrollJump:
      scrollToX (hbar->getValue());
      break;

    case FScrollbar::scrollWheelUp:
      scrollLeft (wheel_distance);
      break;

    case FScrollbar::scrollWheelDown:
      scrollRight (wheel_distance);
      break;
  }

  if ( xoffset_before != xoffset )
    inc_search.clear();

  if ( isShown() )
  {
    drawList();
    updateTerminal();
    flush();
  }

  if ( scrollType >= FScrollbar::scrollStepBackward )
  {
    hbar->setValue (xoffset);

    if ( xoffset_before != xoffset )
      hbar->drawBar();

    updateTerminal();
    flush();
  }
}

}  // namespace finalcut
