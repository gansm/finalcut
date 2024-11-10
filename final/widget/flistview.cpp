/***********************************************************************
* flistview.cpp - Widget FListView and FListViewItem                   *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2017-2024 Markus Gans                                      *
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
#include <limits>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "final/fapplication.h"
#include "final/fevent.h"
#include "final/fwidgetcolors.h"
#include "final/util/emptyfstring.h"
#include "final/util/fstring.h"
#include "final/vterm/fcolorpair.h"
#include "final/vterm/fvtermbuffer.h"
#include "final/widget/flistview.h"
#include "final/widget/fstatusbar.h"

namespace finalcut
{

// Function prototypes
auto firstNumberFromString (const FString&) -> uInt64;
auto sortByName (const FObject*, const FObject*) -> int;
auto sortAscendingByName (const FObject*, const FObject*) -> bool;
auto sortDescendingByName (const FObject*, const FObject*) -> bool;
auto sortByNumber (const FObject* lhs, const FObject* rhs) -> int;
auto sortAscendingByNumber (const FObject*, const FObject*) -> bool;
auto sortDescendingByNumber (const FObject*, const FObject*) -> bool;

// non-member functions
//----------------------------------------------------------------------
auto firstNumberFromString (const FString& str) -> uInt64
{
  auto iter = str.cbegin();

  while ( iter != str.cend() )
  {
    if ( wchar_t(*iter) >= L'0' && wchar_t(*iter) <= L'9' )
    {
      if ( iter != str.cbegin() && wchar_t(*(iter - 1)) == L'-' )
        --iter;

      break;
    }

    ++iter;
  }

  auto first_pos = iter;

  if ( first_pos == str.cend() )
    return 0;

  while ( iter != str.cend() )
  {
    if ( wchar_t(*iter) < L'0' || wchar_t(*iter) > L'9' )
      break;

    ++iter;
  }

  auto last_pos = iter;

  if ( last_pos == str.cend() )
    return 0;

  uInt64 number;
  const auto pos = std::size_t(std::distance(str.cbegin(), first_pos)) + 1;
  const auto length = std::size_t(std::distance(first_pos, last_pos));
  const auto& num_str = str.mid(pos, length);

  try
  {
    number = uInt64(num_str.toLong());
  }
  catch (const std::invalid_argument&)
  {
    return 0;
  }
  catch (const std::underflow_error&)
  {
    return std::numeric_limits<uInt64>::min();
  }
  catch (const std::overflow_error&)
  {
    return std::numeric_limits<uInt64>::max();
  }

  return number;
}

//----------------------------------------------------------------------
auto sortByName (const FObject* lhs, const FObject* rhs) -> int
{
  const auto& l_item = static_cast<const FListViewItem*>(lhs);
  const auto& r_item = static_cast<const FListViewItem*>(rhs);
  const int column = l_item->getSortColumn();
  const auto& l_string = l_item->getText(column);
  const auto& r_string = r_item->getText(column);

  // Compare lhs with rhs
  return FStringCaseCompare(l_string, r_string);
}

//----------------------------------------------------------------------
auto sortAscendingByName (const FObject* lhs, const FObject* rhs) -> bool
{
  // lhs < rhs
  return sortByName(lhs, rhs) < 0;
}

//----------------------------------------------------------------------
auto sortDescendingByName (const FObject* lhs, const FObject* rhs) -> bool
{
  // lhs > rhs
  return sortByName(lhs, rhs) > 0;
}

//----------------------------------------------------------------------
auto sortByNumber (const FObject* lhs, const FObject* rhs) -> int
{
  const auto& l_item = static_cast<const FListViewItem*>(lhs);
  const auto& r_item = static_cast<const FListViewItem*>(rhs);
  const int column = l_item->getSortColumn();
  const auto& l_number = firstNumberFromString(l_item->getText(column));
  const auto& r_number = firstNumberFromString(r_item->getText(column));

  // Compare lhs with rhs
  if ( l_number < r_number )
    return -1;

  return l_number == r_number ? 0 : 1;
}

//----------------------------------------------------------------------
auto sortAscendingByNumber (const FObject* lhs, const FObject* rhs) -> bool
{
  // lhs < rhs
  return sortByNumber(lhs, rhs) < 0;
}

//----------------------------------------------------------------------
auto sortDescendingByNumber (const FObject* lhs, const FObject* rhs) -> bool
{
  // lhs > rhs
  return sortByNumber(lhs, rhs) > 0;
}


//----------------------------------------------------------------------
// class FListViewItem
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FListViewItem::FListViewItem (const FListViewItem& item)
  : FObject{item.getParent()}
  , column_list{item.column_list}
  , data_pointer{item.data_pointer}
{
  auto parent = getParent();

  if ( ! parent )
    return;

  if ( parent->isInstanceOf("FListView") )
  {
    static_cast<FListView*>(parent)->insert (this);
  }
  else if ( parent->isInstanceOf("FListViewItem") )
  {
    static_cast<FListViewItem*>(parent)->insert (this);
  }
}

//----------------------------------------------------------------------
FListViewItem::FListViewItem (iterator parent_iter)
  : FObject{(*parent_iter)->getParent()}
{
  insert (this, parent_iter);
}

//----------------------------------------------------------------------
FListViewItem::~FListViewItem()  // destructor
{
  // Remove from parent itemlist

  auto parent = getParent();

  if ( ! parent )
    return;

  try
  {
    if ( parent->isInstanceOf("FListView") )
    {
      static_cast<FListView*>(parent)->remove (this);
    }
    else if ( parent->isInstanceOf("FListViewItem") )
    {
      static_cast<FListViewItem*>(parent)->remove (this);
    }
  }
  catch (const std::exception& ex)
  {
    std::clog << FLog::LogLevel::Error
              << "Exception on removing this element: " << ex.what();
  }
}


// public methods of FListViewItem
//----------------------------------------------------------------------
auto FListViewItem::getSortColumn() const -> int
{
  if ( ! *root )
    return -1;

  const auto& root_obj = static_cast<FListView*>(*root);
  return root_obj->getSortColumn();
}

//----------------------------------------------------------------------
auto FListViewItem::getText (int column) const -> FString
{
  if ( column < 1
    || column_list.empty()
    || column > int(column_list.size()) )
    return fc::emptyFString::get();

  // Convert column position to address offset (index)
  const auto index = std::size_t(column - 1);
  return column_list[index];
}

//----------------------------------------------------------------------
auto FListViewItem::getDepth() const -> uInt
{
  const auto& parent = getParent();

  if ( parent && parent->isInstanceOf("FListViewItem") )
  {
    const auto& parent_item = static_cast<FListViewItem*>(parent);
    return parent_item->getDepth() + 1;
  }

  return 0;
}

//----------------------------------------------------------------------
void FListViewItem::setText (int column, const FString& text)
{
  if ( column < 1
    || column_list.empty()
    || column > int(column_list.size()) )
    return;

  // Convert column position to address offset (index)
  const auto index = std::size_t(column - 1);
  auto parent = getParent();

  if ( parent && parent->isInstanceOf("FListView") )
  {
    auto listview = static_cast<FListView*>(parent);

    if ( ! listview->data.header[index].fixed_width )
    {
      const auto column_width = int(getColumnWidth(text));

      if ( column_width > listview->data.header[index].width )
        listview->data.header[index].width = column_width;
    }
  }

  column_list[index] = text;
}

//----------------------------------------------------------------------
auto FListViewItem::insert (FListViewItem* child) -> FObject::iterator
{
  // Add a FListViewItem as child element
  if ( ! child )
    return FListView::getNullIterator();

  return appendItem(child);
}

//----------------------------------------------------------------------
auto FListViewItem::insert ( FListViewItem* child
                           , iterator parent_iter ) const -> FObject::iterator
{
  if ( parent_iter == FListView::getNullIterator() )
    return FListView::getNullIterator();

  if ( *parent_iter )
  {
    if ( (*parent_iter)->isInstanceOf("FListView") )
    {
      // Add FListViewItem to a FListView parent
      auto parent = static_cast<FListView*>(*parent_iter);
      return parent->insert (child);
    }

    if ( (*parent_iter)->isInstanceOf("FListViewItem") )
    {
      // Add FListViewItem to a FListViewItem parent
      auto parent = static_cast<FListViewItem*>(*parent_iter);
      return parent->insert (child);
    }
  }

  return FListView::getNullIterator();
}

//----------------------------------------------------------------------
void FListViewItem::remove (FListViewItem* item) const
{
  if ( item == nullptr || item == *FListView::getNullIterator() )
    return;

  auto parent = item->getParent();

  // Search for a FListView parent in my object tree
  while ( parent && ! parent->isInstanceOf("FListView") )
  {
    parent = parent->getParent();
  }

  if ( parent == nullptr )
    return;

  if ( parent->isInstanceOf("FListView") )
  {
    auto listview = static_cast<FListView*>(parent);
    listview->remove(item);
  }
  else
  {
    parent = item->getParent();
    parent->delChild(item);
    auto parent_item = static_cast<FListViewItem*>(parent);
    parent_item->visible_lines--;

    if ( ! parent_item->hasChildren() )
    {
      parent_item->expandable = false;
      parent_item->is_expand = false;
    }
  }
}

//----------------------------------------------------------------------
void FListViewItem::expand()
{
  if ( isExpand() || ! hasChildren() )
    return;

  resetVisibleLineCounter();
  is_expand = true;
}

//----------------------------------------------------------------------
void FListViewItem::collapse()
{
  if ( ! isExpand() )
    return;

  resetVisibleLineCounter();
  is_expand = false;
}

// private methods of FListView
//----------------------------------------------------------------------
template <typename Compare>
void FListViewItem::sort (Compare cmp)
{
  if ( ! isExpandable() )
    return;

  // Sort the top level
  auto& children = getChildren();

  if ( ! children.empty() )
    std::sort(children.begin(), children.end(), cmp);

  // Sort the sublevels
  for (auto&& item : children)
    static_cast<FListViewItem*>(item)->sort(cmp);
}

//----------------------------------------------------------------------
auto FListViewItem::appendItem (FListViewItem* child) -> FObject::iterator
{
  expandable = true;
  resetVisibleLineCounter();
  child->root = root;
  addChild (child);
  // Return iterator to child/last element
  return --FObject::end();
}

//----------------------------------------------------------------------
void FListViewItem::replaceControlCodes()
{
  // Replace the control codes characters
  auto iter = column_list.begin();

  while ( iter != column_list.end() )
  {
    *iter = iter->replaceControlCodes();
    ++iter;
  }
}

//----------------------------------------------------------------------
auto FListViewItem::getVisibleLines() -> std::size_t
{
  if ( visible_lines > 1 )
    return visible_lines;

  visible_lines = 1;

  if ( ! isExpand() || ! hasChildren() )
    return visible_lines;

  auto iter = FObject::cbegin();

  while ( iter != FObject::cend() )
  {
    const auto& child = static_cast<FListViewItem*>(*iter);
    visible_lines += child->getVisibleLines();
    ++iter;
  }

  return visible_lines;
}

//----------------------------------------------------------------------
void FListViewItem::setCheckable (bool enable)
{
  checkable = enable;

  if ( *root )
  {
    auto root_obj = static_cast<FListView*>(*root);

    if ( ! root_obj->hasCheckableItems() && isCheckable() )
      root_obj->has_checkable_items = true;
  }
}

//----------------------------------------------------------------------
void FListViewItem::resetVisibleLineCounter()
{
  visible_lines = 0;
  auto parent = getParent();

  if ( parent && parent->isInstanceOf("FListViewItem") )
  {
    auto parent_item = static_cast<FListViewItem*>(parent);
    return parent_item->resetVisibleLineCounter();
  }
}


//----------------------------------------------------------------------
// class FListViewIterator
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FListViewIterator::FListViewIterator (Iterator iter)
  : node{iter}
{ }

// FListViewIterator operators
//----------------------------------------------------------------------
auto FListViewIterator::operator = (Iterator iter) -> FListViewIterator&
{
  iter_path = IteratorStack{};
  node = iter;
  position = 0;
  return *this;
}

//----------------------------------------------------------------------
auto FListViewIterator::operator ++ () -> FListViewIterator&  // prefix
{
  nextElement(node);
  return *this;
}

//----------------------------------------------------------------------
auto FListViewIterator::operator ++ (int) -> FListViewIterator  // postfix
{
  FListViewIterator tmp = *this;
  ++(*this);
  return tmp;
}

//----------------------------------------------------------------------
auto FListViewIterator::operator -- () -> FListViewIterator&  // prefix
{
  prevElement(node);
  return *this;
}

//----------------------------------------------------------------------
auto FListViewIterator::operator -- (int) -> FListViewIterator  // postfix
{
  FListViewIterator tmp = *this;
  --(*this);
  return tmp;
}

//----------------------------------------------------------------------
auto FListViewIterator::operator += (int n) -> FListViewIterator&
{
  for (int i = n; i > 0 ; i--)
    nextElement(node);

  return *this;
}

//----------------------------------------------------------------------
auto FListViewIterator::operator -= (int n) -> FListViewIterator&
{
  for (int i = n; i > 0 ; i--)
    prevElement(node);

  return *this;
}

// private methods of FListViewIterator
//----------------------------------------------------------------------
void FListViewIterator::nextElement (Iterator& iter)
{
  const auto& item = static_cast<FListViewItem*>(*iter);

  if ( ! item )
    return;

  if ( item->isExpandable() && item->isExpand() )
  {
    iter_path.push(iter);
    iter = item->begin();
    position++;
    return;
  }

  position++;
  bool forward{};

  do
  {
    forward = false;  // Reset forward
    ++iter;

    if ( iter_path.empty() )
      continue;

    const auto& parent_iter = iter_path.top();

    if ( iter == (*parent_iter)->cend() )
    {
      iter = parent_iter;
      iter_path.pop();
      forward = true;
    }
  }
  while ( forward );
}

//----------------------------------------------------------------------
void FListViewIterator::prevElement (Iterator& iter)
{
  auto start_iter = iter;

  if ( ! iter_path.empty() )
  {
    const auto& parent_iter = iter_path.top();

    if ( start_iter == (*parent_iter)->cbegin() )  // First sub-item
    {
      iter = parent_iter;
      position--;
      iter_path.pop();
      return;
    }
  }

  --iter;
  auto item = static_cast<FListViewItem*>(*iter);

  if ( iter == start_iter )  // No changes
    return;

  position--;

  while ( item->isExpandable() && item->isExpand() )
  {
    iter_path.push(iter);
    iter = item->end();
    --iter;
    item = static_cast<FListViewItem*>(*iter);
  }
}

//----------------------------------------------------------------------
void FListViewIterator::parentElement()
{
  if ( iter_path.empty() )
    return;

  const auto& parent_iter = iter_path.top();

  while ( node != parent_iter )
    prevElement(node);
}


//----------------------------------------------------------------------
// class FListView
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FListView::FListView (FWidget* parent)
  : FWidget{parent}
{
  init();
}

//----------------------------------------------------------------------
FListView::~FListView()  // destructor
{
  delOwnTimers();
}

// public methods of FListView
//----------------------------------------------------------------------
auto FListView::getCount() const -> std::size_t
{
  std::size_t n{0};

  for (auto&& item : data.itemlist)
  {
    const auto& listitem = static_cast<FListViewItem*>(item);
    n += listitem->getVisibleLines();
  }

  return n;
}

//----------------------------------------------------------------------
auto FListView::getColumnAlignment (int column) const -> Align
{
  // Get the alignment for a column

  if ( isColumnIndexInvalid(column) )
    return Align::Left;

  // Convert column position to address offset (index)
  const auto index = std::size_t(column - 1);
  return data.header[index].alignment;
}

//----------------------------------------------------------------------
auto FListView::getColumnText (int column) const -> FString
{
  // Get the text of column

  if ( isColumnIndexInvalid(column) )
    return fc::emptyFString::get();

  // Convert column position to address offset (index)
  const auto index = std::size_t(column - 1);
  return data.header[index].name;
}

//----------------------------------------------------------------------
auto FListView::getColumnSortType (int column) const -> SortType
{
  SortType s_type;
  const auto col = std::size_t(column);

  try
  {
    s_type = sorting.type.at(col);
  }
  catch (const std::out_of_range&)
  {
    s_type = SortType::Unknown;
  }

  return s_type;
}

//----------------------------------------------------------------------
void FListView::setSize (const FSize& size, bool adjust)
{
  FWidget::setSize (size, adjust);
  changeOnResize();
}

//----------------------------------------------------------------------
void FListView::setGeometry ( const FPoint& pos, const FSize& size
                            , bool adjust)
{
  FWidget::setGeometry (pos, size, adjust);
  changeOnResize();
}

//----------------------------------------------------------------------
void FListView::setColumnAlignment (int column, Align align)
{
  // Set the alignment for a column

  if ( isColumnIndexInvalid(column) )
    return;

  // Convert column position to address offset (index)
  const auto index = std::size_t(column - 1);
  data.header[index].alignment = align;
}

//----------------------------------------------------------------------
void FListView::setColumnText (int column, const FString& label)
{
  // Set the text for a column

  if ( isColumnIndexInvalid(column) )
    return;

  // Convert column position to address offset (index)
  auto index = std::size_t(column - 1);

  if ( ! data.header[index].fixed_width )
  {
    const auto column_width = int(getColumnWidth(label));

    if ( column_width > data.header[index].width )
      data.header[index].width = column_width;
  }

  data.header[index].name = label;
}

//----------------------------------------------------------------------
void FListView::setColumnSortType (int column, SortType type)
{
  // Sets the sort type by which the list is to be sorted

  if ( isColumnIndexInvalid(column) )
    return;

  const std::size_t size = std::size_t(column) + 1;

  if ( sorting.type.empty() || sorting.type.size() < size )
    sorting.type.resize(size);

  sorting.type[uInt(column)] = type;
}

//----------------------------------------------------------------------
void FListView::setColumnSort (int column, SortOrder order)
{
  // Sets the column to sort by + the sorting order

  if ( isColumnIndexInvalid(column) )
    column = -1;

  sorting.column = column;
  sorting.order = order;
}

//----------------------------------------------------------------------
void FListView::showColumn (int column)
{
  // Shows the column with the given column value

  if ( isColumnIndexInvalid(column) )
    return;

  // Convert column position to address offset (index)
  const auto index = std::size_t(column - 1);
  data.header[index].visible = true;
  // Update and redraw the layout of the list view
  updateLayout();
}

//----------------------------------------------------------------------
void FListView::hideColumn (int column)
{
  // Hides the column with the given column value

  if ( isColumnIndexInvalid(column) )
    return;

  // Convert column position to address offset (index)
  const auto index = std::size_t(column - 1);
  data.header[index].visible = false;
  // Update and redraw the layout of the list view
  updateLayout();
}

//----------------------------------------------------------------------
auto FListView::isColumnHidden (int column) const -> bool
{
  if ( isColumnIndexInvalid(column) )
    return true;

  // Convert column position to address offset (index)
  const auto index = std::size_t(column - 1);
  return ! data.header[index].visible;
}

//----------------------------------------------------------------------
auto FListView::addColumn (const FString& label, int width) -> int
{
  Header new_column{};
  new_column.name = label;
  new_column.width = width;

  if ( new_column.width == USE_MAX_SIZE )
    new_column.width = int(getColumnWidth(label));
  else
    new_column.fixed_width = true;

  data.header.push_back (new_column);
  return int(data.header.size());
}

//----------------------------------------------------------------------
auto FListView::removeColumn (int column) -> int
{
  // Deletes a column from the list and empties it if all columns are gone

  if ( isColumnIndexInvalid(column) )
    return int(data.header.size());

  if ( data.header.size() == 1 )
  {
    data.header.clear();
    clear();
    return 0;
  }

  data.header.erase (data.header.begin() + column - 1);
  max_line_width = 0;
  auto iter = data.itemlist.begin();

  while ( iter != data.itemlist.end() )
  {
    const auto& item = static_cast<FListViewItem*>(*iter);
    item->column_list.erase (item->column_list.begin() + column - 1);
    std::size_t line_width = determineLineWidth (item);
    recalculateHorizontalBar (line_width);
    ++iter;
  }

  selection.current_iter = data.itemlist.begin();
  scroll.first_visible_line = data.itemlist.begin();
  adjustScrollbars(getCount());
  drawList();
  drawBorder();
  drawHeadlines();
  drawScrollbars();
  processChanged();
  return int(data.header.size());
}

//----------------------------------------------------------------------
void FListView::removeAllColumns()
{
  // Deletes all columns and clears the list

  max_line_width = 0;
  adjustScrollbars(0);
  data.header.clear();
  clear();
}

//----------------------------------------------------------------------
void FListView::hide()
{
  FWidget::hide();
  hideArea (getSize());
}

//----------------------------------------------------------------------
auto FListView::insert ( FListViewItem* item
                       , iterator parent_iter ) -> FObject::iterator
{
  iterator item_iter;

  if ( parent_iter == getNullIterator() )
    return getNullIterator();

  beforeInsertion(item);  // preprocessing

  if ( parent_iter == data.root )
  {
    item_iter = appendItem (item);
  }
  else if ( *parent_iter )
  {
    if ( (*parent_iter)->isInstanceOf("FListView") )
    {
      // Add FListViewItem to a FListView parent
      auto parent = static_cast<FListView*>(*parent_iter);
      item_iter = parent->appendItem (item);
    }
    else if ( (*parent_iter)->isInstanceOf("FListViewItem") )
    {
      // Add FListViewItem to a FListViewItem parent
      auto parent = static_cast<FListViewItem*>(*parent_iter);
      item_iter = parent->appendItem (item);
    }
    else
      item_iter = getNullIterator();
  }
  else
    item_iter = getNullIterator();

  afterInsertion();  // post-processing
  return item_iter;
}

//----------------------------------------------------------------------
void FListView::remove (FListViewItem* item)
{
  if ( ! item || isItemListEmpty() )
    return;

  adjustListBeforeRemoval (item);
  removeItemFromParent (item);
  updateListAfterRemoval();
  processChanged();
}

//----------------------------------------------------------------------
void FListView::clear()
{
  data.itemlist.clear();
  selection.current_iter = getNullIterator();
  scroll.first_visible_line = getNullIterator();
  scroll.last_visible_line = getNullIterator();
  recalculateVerticalBar (0);
  scroll.first_line_position_before = -1;
  scroll.xoffset = 0;
  scroll.vbar->setMinimum(0);
  scroll.vbar->setValue(0);
  scroll.vbar->hide();
  clearList();
  processChanged();
}

//----------------------------------------------------------------------
void FListView::sort()
{
  // Sorts the list view according to the specified setting

  if ( sorting.column < 1 || sorting.column > int(data.header.size()) )
    return;

  SortType column_sort_type = getColumnSortType(sorting.column);
  std::function<bool(const FObject*, const FObject*)> comparator;

  switch ( column_sort_type )
  {
    case SortType::Unknown:
    case SortType::Name:
      comparator = ( sorting.order == SortOrder::Ascending )
                 ? sortAscendingByName
                 : sortDescendingByName;
      break;

    case SortType::Number:
      comparator = ( sorting.order == SortOrder::Ascending )
                 ? sortAscendingByNumber
                 : sortDescendingByNumber;
      break;

    case SortType::UserDefined:
      comparator = ( sorting.order == SortOrder::Ascending )
                 ? user_defined_ascending
                 : user_defined_descending;
      break;

    default:
      throw std::invalid_argument{"Invalid sort type"};
  }

  sort(std::move(comparator));
  selection.current_iter = data.itemlist.begin();
  scroll.first_visible_line = data.itemlist.begin();
  processChanged();
}

//----------------------------------------------------------------------
void FListView::onKeyPress (FKeyEvent* ev)
{
  const int position_before = selection.current_iter.getPosition();
  const int xoffset_before = scroll.xoffset;
  scroll.first_line_position_before = scroll.first_visible_line.getPosition();
  selection.clicked_expander_pos.setPoint(-1, -1);
  processKeyAction(ev);  // Process the keystrokes

  if ( position_before != selection.current_iter.getPosition() )
    processRowChanged();

  if ( ev->isAccepted() )
  {
    const bool draw_vbar( scroll.first_line_position_before
                       != scroll.first_visible_line.getPosition() );
    const bool draw_hbar(xoffset_before != scroll.xoffset);
    updateDrawing (draw_vbar, draw_hbar);
  }
}

//----------------------------------------------------------------------
void FListView::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != MouseButton::Left )
  {
    selection.clicked_expander_pos.setPoint(-1, -1);
    return;
  }

  setWidgetFocus(this);
  scroll.first_line_position_before = scroll.first_visible_line.getPosition();

  if ( isWithinHeaderBounds(ev->getPos()) )
  {
    selection.clicked_header_pos = ev->getPos();  // Handle events in the header
  }
  else if ( isWithinListBounds(ev->getPos()) && ! isItemListEmpty() )
  {
    handleListEvent(ev);  // Handle events in the list
  }
}

//----------------------------------------------------------------------
void FListView::onMouseUp (FMouseEvent* ev)
{
  if ( isDragging(drag_scroll) )
    stopDragScroll();

  if ( ev->getButton() != MouseButton::Left )
  {
    resetClickedPositions();
    return;
  }

  if ( isWithinHeaderBounds(ev->getPos())
    && selection.clicked_header_pos == ev->getPos() )  // Header
  {
    mouseHeaderClicked();
    return;
  }

  if ( ! isWithinListBounds(ev->getPos()) )  // List
  {
    resetClickedPositions();
    return;
  }

  if ( isItemListEmpty() )
    return;

  handleTreeExpanderClick(ev);
  handleCheckboxClick(ev);
  processRowChanged();
  resetClickedPositions();
}

//----------------------------------------------------------------------
void FListView::onMouseMove (FMouseEvent* ev)
{
  if ( ev->getButton() != MouseButton::Left )
  {
    selection.clicked_expander_pos.setPoint(-1, -1);
    return;
  }

  const int mouse_y = ev->getY();
  scroll.first_line_position_before = scroll.first_visible_line.getPosition();

  if ( isWithinListBounds(ev->getPos()) )
  {
    const int new_pos = scroll.first_visible_line.getPosition() + mouse_y - 2;

    if ( new_pos < int(getCount()) )
      setRelativePosition (mouse_y - 2);

    if ( isShown() )
      drawList();

    scroll.vbar->setValue (scroll.first_visible_line.getPosition());

    if ( scroll.first_line_position_before != scroll.first_visible_line.getPosition() )
      scroll.vbar->drawBar();

    forceTerminalUpdate();
  }

  // auto-scrolling when dragging mouse outside the widget
  if ( mouse_y < 2 )
    dragUp (ev->getButton());
  else if ( mouse_y >= int(getHeight()) )
    dragDown (ev->getButton());
  else
    stopDragScroll();
}

//----------------------------------------------------------------------
void FListView::onMouseDoubleClick (FMouseEvent* ev)
{
  if ( ev->getButton() != MouseButton::Left )
    return;

  if ( isWithinListBounds(ev->getPos()) )
  {
    if ( scroll.first_visible_line.getPosition() + ev->getY() - 1 > int(getCount()) )
      return;

    if ( isItemListEmpty() )
      return;

    auto item = getCurrentItem();

    if ( isTreeView() && item->isExpandable() )
    {
      toggleItemExpandState(item);
      adjustScrollbars (getCount());  // after expand or collapse

      if ( isShown() )
        draw();
    }

    processClick();
  }

  selection.clicked_expander_pos.setPoint(-1, -1);
}

//----------------------------------------------------------------------
void FListView::onTimer (FTimerEvent*)
{
  scroll.first_line_position_before = scroll.first_visible_line.getPosition();

  if ( canSkipDragScrolling() )
    return;

  if ( isShown() )
    drawList();

  scroll.vbar->setValue (scroll.first_visible_line.getPosition());

  if ( scroll.first_line_position_before != scroll.first_visible_line.getPosition() )
    scroll.vbar->drawBar();

  forceTerminalUpdate();
}

//----------------------------------------------------------------------
void FListView::onWheel (FWheelEvent* ev)
{
  const int position_before = selection.current_iter.getPosition();
  static constexpr int wheel_distance = 4;
  const auto& wheel = ev->getWheel();
  scroll.first_line_position_before = scroll.first_visible_line.getPosition();

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

  if ( position_before != selection.current_iter.getPosition() )
    processRowChanged();

  if ( isShown() )
    drawList();

  scroll.vbar->setValue (scroll.first_visible_line.getPosition());

  if ( scroll.first_line_position_before != scroll.first_visible_line.getPosition() )
    scroll.vbar->drawBar();

  forceTerminalUpdate();
}

//----------------------------------------------------------------------
void FListView::onFocusOut (FFocusEvent* out_ev)
{
  delOwnTimers();
  FWidget::onFocusOut(out_ev);
}


// protected methods of FListView
//----------------------------------------------------------------------
void FListView::initLayout()
{
  nf_offset = FVTerm::getFOutput()->isNewFont() ? 1 : 0;
  setTopPadding(1);
  setLeftPadding(1);
  setBottomPadding(1);
  setRightPadding(1);
}

//----------------------------------------------------------------------
void FListView::adjustViewport (const int element_count)
{
  const auto height = int(getClientHeight());

  if ( height <= 0 || element_count == 0 )
    return;

  if ( element_count < height )
  {
    scroll.first_visible_line = data.itemlist.begin();
    scroll.last_visible_line = scroll.first_visible_line + element_count - 1;
  }

  if ( scroll.first_visible_line.getPosition() > element_count - height  )
  {
    const int difference = scroll.first_visible_line.getPosition()
                         - (element_count - height);

    if ( scroll.first_visible_line.getPosition() >= difference )
    {
      scroll.first_visible_line -= difference;
      scroll.last_visible_line -= difference;
    }
  }

  const int after_last_visible_line = scroll.first_visible_line.getPosition()
                                    + height;

  if ( scroll.last_visible_line.getPosition() >= after_last_visible_line )
    scroll.last_visible_line = scroll.first_visible_line + height - 1;

  if ( selection.current_iter.getPosition() > scroll.last_visible_line.getPosition() )
    selection.current_iter = scroll.last_visible_line;
}

//----------------------------------------------------------------------
void FListView::adjustScrollbars (const std::size_t element_count) const
{
  const std::size_t width = getClientWidth();
  const std::size_t height = getClientHeight();
  scroll.vbar->setMaximum (getScrollBarMaxVertical(element_count));
  scroll.vbar->setPageSize (int(element_count), int(height));
  scroll.vbar->setX (int(getWidth()));
  scroll.vbar->setHeight (height, false);
  scroll.vbar->resize();

  scroll.hbar->setMaximum (getScrollBarMaxHorizontal());
  scroll.hbar->setPageSize (int(max_line_width), int(width));
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

//----------------------------------------------------------------------
void FListView::adjustSize()
{
  FWidget::adjustSize();
  const std::size_t element_count = getCount();
  adjustViewport (int(element_count));
  adjustScrollbars (element_count);
}


// private methods of FListView
//----------------------------------------------------------------------
auto FListView::getNullIterator() -> FObject::iterator&
{
  static iterator null_iter;  // Saves the global null iterator
  return null_iter;
}

//----------------------------------------------------------------------
void FListView::setNullIterator (const iterator& null_iter)
{
  getNullIterator() = null_iter;
}

//----------------------------------------------------------------------
inline auto FListView::canSkipDragScrolling() -> bool
{
  const int position_before = selection.current_iter.getPosition();
  bool is_upward_scroll ( drag_scroll == DragScrollMode::Upward
                       || drag_scroll == DragScrollMode::SelectUpward );
  bool is_downward_scroll ( drag_scroll == DragScrollMode::Downward
                         || drag_scroll == DragScrollMode::SelectDownward );
  return ( is_upward_scroll && ! dragScrollUp(position_before) )
      || ( is_downward_scroll && ! dragScrollDown(position_before) );
}

//----------------------------------------------------------------------
void FListView::init()
{
  initScrollbar (scroll.vbar, Orientation::Vertical, this, &FListView::cb_vbarChange);
  initScrollbar (scroll.hbar, Orientation::Horizontal, this, &FListView::cb_hbarChange);
  data.selflist.push_back(this);
  data.root = data.selflist.begin();
  getNullIterator() = data.selflist.end();
  FListView::setGeometry (FPoint{1, 1}, FSize{5, 4}, false);  // initialize geometry values
  mapKeyFunctions();
}

//----------------------------------------------------------------------
inline void FListView::mapKeyFunctions()
{
  data.key_map =
  {
    { FKey::Return    , [this] { processClick(); } },
    { FKey::Enter     , [this] { processClick(); } },
    { FKey::Space     , [this] { toggleCheckbox(); } },
    { FKey::Up        , [this] { stepBackward(); } },
    { FKey::Down      , [this] { stepForward(); } },
    { FKey::Left      , [this] { collapseAndScrollLeft(); } },
    { FKey::Right     , [this] { expandAndScrollRight(); } },
    { FKey::Page_up   , [this] { stepBackward(int(getClientHeight()) - 1); } },
    { FKey::Page_down , [this] { stepForward(int(getClientHeight()) - 1); } },
    { FKey::Home      , [this] { firstPos(); } },
    { FKey::End       , [this] { lastPos(); } }
  };

  data.key_map_result =
  {
    { FKey('+'), [this] { return expandSubtree(); } },
    { FKey('-'), [this] { return collapseSubtree(); } }
  };
}

//----------------------------------------------------------------------
void FListView::processKeyAction (FKeyEvent* ev)
{
  const auto idx = ev->key();
  const auto& iter = data.key_map.find(idx);

  if ( iter != data.key_map.end() )
  {
    iter->second();
    ev->accept();
    return;
  }

  const auto& iter_result = data.key_map_result.find(idx);

  if ( iter_result != data.key_map_result.end() )
  {
    if ( iter_result->second() )
      ev->accept();

    return;
  }

  ev->ignore();
}

//----------------------------------------------------------------------
template <typename Compare>
void FListView::sort (Compare cmp)
{
  // Sort the top level
  std::sort(data.itemlist.begin(), data.itemlist.end(), cmp);

  // Sort the sublevels
  for (auto&& item : data.itemlist)
    static_cast<FListViewItem*>(item)->sort(cmp);
}

//----------------------------------------------------------------------
auto FListView::getAlignOffset ( const Align align
                               , const std::size_t column_width
                               , const std::size_t width ) const -> std::size_t
{
  if ( align == Align::Center )
  {
    if ( column_width < width )
      return (width - column_width) / 2;
  }
  else if ( align == Align::Right && column_width < width )
  {
    return width - column_width;
  }

  return 0;
}

//----------------------------------------------------------------------
auto FListView::getListEnd (const FListViewItem* item) -> FObject::iterator
{
  auto parent = item->getParent();

  if ( ! parent )
    return getNullIterator();

  if ( this == parent )
    return data.itemlist.end();

  if ( parent->isInstanceOf("FListViewItem") )
    return static_cast<FListViewItem*>(parent)->end();

  return getNullIterator();
}

//----------------------------------------------------------------------
void FListView::draw()
{
  if ( selection.current_iter.getPosition() < 1 )
    selection.current_iter = data.itemlist.begin();

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

  drawHeadlines();

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);

  drawScrollbars();
  drawList();
  updateStatusbar (this);
}

//----------------------------------------------------------------------
void FListView::drawBorder()
{
  const FRect box(FPoint{1, 1}, getSize());
  finalcut::drawListBorder (this, box);
}

//----------------------------------------------------------------------
void FListView::drawScrollbars() const
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
void FListView::drawHeadlines()
{
  if ( data.header.empty()
    || getHeight() <= 2
    || getWidth() <= 4
    || max_line_width < 1 )
    return;

  auto iter = data.header.cbegin();
  data.headerline.clear();

  if ( hasCheckableItems() )
    drawHeaderBorder(4);  // Draw into FVTermBuffer object

  while ( iter != data.header.cend() )
  {
    if ( iter->visible && ! iter->name.isEmpty() )
      drawHeadlineLabel(iter);  // Draw into FVTermBuffer object

    ++iter;
  }

  // Print the FVTermBuffer object
  drawBufferedHeadline();
}

//----------------------------------------------------------------------
void FListView::drawList()
{
  if ( canSkipListDrawing() )
    return;

  int y{0};
  const auto page_height = int(getHeight()) - 2;
  const auto& itemlist_end = data.itemlist.end();
  auto path_end = itemlist_end;
  auto iter = scroll.first_visible_line;

  while ( iter != path_end && iter != itemlist_end && y < page_height )
  {
    const auto is_current_line = bool( iter == selection.current_iter );
    const auto& item = static_cast<FListViewItem*>(*iter);
    path_end = getListEnd(item);
    print() << FPoint{2, 2 + y};

    // Draw one FListViewItem
    drawListLine (item, getFlags().focus.focus, is_current_line);

    // Place the input cursor at the beginning of the line
    setInputCursor (item, y, is_current_line);

    scroll.last_visible_line = iter;
    y++;
    ++iter;
  }

  finalizeListDrawing(y);
}

//----------------------------------------------------------------------
inline void FListView::setInputCursor ( const FListViewItem* item
                                      , int y, bool is_current_line )
{
  if ( ! (getFlags().focus.focus && is_current_line) )
    return;

  const int tree_offset = isTreeView() ? int(item->getDepth() << 1u) + 1 : 0;
  const int checkbox_offset = item->isCheckable() ? 1 : 0;
  int xpos = 3 + tree_offset + checkbox_offset - scroll.xoffset;

  if ( xpos < 2 )  // Hide the cursor
    xpos = -9999;  // by moving it outside the visible area

  setVisibleCursor (item->isCheckable());
  setCursorPos ({xpos, 2 + y});  // first character
}

//----------------------------------------------------------------------
inline void FListView::finalizeListDrawing (int y)
{
  // Reset color
  setColor();

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(true);

  // Clean empty space after last element
  while ( y < int(getClientHeight()) )
  {
    print() << FPoint{2, 2 + y}
            << FString{std::size_t(getClientWidth()), ' '};
    y++;
  }
}

//----------------------------------------------------------------------
inline void FListView::adjustWidthForTreeView ( std::size_t& width
                                              , std::size_t indent
                                              , bool is_checkable ) const
{
  width -= (indent + 1);

  if ( is_checkable )
    width -= checkbox_space;
}

//----------------------------------------------------------------------
void FListView::drawListLine ( const FListViewItem* item
                             , bool is_focus
                             , bool is_current )
{
  // Set line color and attributes
  setLineAttributes (is_current, is_focus);

  // Create a string that contains the columns
  FString line = createColumnsString(item);

  // Print the entry
  printColumnsString (line);
}

//----------------------------------------------------------------------
auto FListView::createColumnsString (const FListViewItem* item) -> FString
{
  if ( item->column_list.empty() )
    return {};

  // Get prefix
  const std::size_t indent = item->getDepth() << 1u;  // indent = 2 * depth
  FString line{getLinePrefix (item, indent)};

  for (std::size_t col{0}; col < item->column_list.size(); )
  {
    if ( ! data.header[col].visible )
    {
      col++;
      continue;
    }

    static constexpr std::size_t ellipsis_length = 2;
    const auto& text = item->column_list[col];
    auto width = std::size_t(data.header[col].width);
    const std::size_t column_width = getColumnWidth(text);
    // Increment the value of col for the column position
    // and the next iteration
    col++;
    const auto align = getColumnAlignment(int(col));
    const std::size_t align_offset = getAlignOffset (align, column_width, width);

    if ( isTreeView() && col == 1 )
      adjustWidthForTreeView (width, indent, item->isCheckable());

    // Insert alignment spaces
    if ( align_offset > 0 )
      line += FString{align_offset, L' '};

    if ( align_offset + column_width <= width )
    {
      // Insert text and trailing space
      static constexpr std::size_t leading_space = 1;
      line += getColumnSubString (text, 1, width);
      line += FString { leading_space + width
                      - align_offset - column_width, L' '};
    }
    else if ( align == Align::Right )
    {
      // Ellipse right align text
      const std::size_t first = getColumnWidth(text) + 1 - width;
      line += FString {L".."};
      line += getColumnSubString (text, first, width - ellipsis_length);
      line += L' ';
    }
    else
    {
      // Ellipse left align text and center text
      line += getColumnSubString (text, 1, width - ellipsis_length);
      line += FString {L".. "};
    }
  }

  return line;
}

//----------------------------------------------------------------------
void FListView::printColumnsString (FString& line)
{
  const std::size_t width = getWidth() - nf_offset - 2;
  line = getColumnSubString (line, std::size_t(scroll.xoffset) + 1, width);
  const std::size_t len = line.getLength();
  std::size_t char_width{0};

  for (std::size_t i{0}; i < len; i++)
  {
    try
    {
      char_width += getColumnWidth(line[i]);
      print() << line[i];
    }
    catch (const std::out_of_range&)
    {
      return;
    }
  }

  for (std::size_t i = char_width; i < width; i++)
    print (' ');
}

//----------------------------------------------------------------------
void FListView::clearList()
{
  // Clear list from terminal screen

  const auto& wc_list = getColorTheme()->list;
  setColor (wc_list.fg, wc_list.bg);
  const std::size_t size = getWidth() - 2;
  drawBorder();
  drawHeadlines();

  if ( size == 0 )
    return;

  for (auto y{0}; y < int(getHeight()) - 2; y++)
  {
    print() << FPoint{2, 2 + y} << FString{size, L' '};
  }

  drawScrollbars();
}

//----------------------------------------------------------------------
inline void FListView::setLineAttributes ( bool is_current
                                         , bool is_focus ) const
{
  // Set line color and attributes

  const auto& wc = getColorTheme();
  const auto& wc_list = wc->list;
  const auto& wc_current_element = wc->current_element;
  setColor (wc_list.fg, wc_list.bg);

  if ( is_current )
  {
    if ( is_focus && FVTerm::getFOutput()->getMaxColor() < 16 )
      setBold();

    if ( FVTerm::getFOutput()->isMonochron() )
      unsetBold();

    if ( is_focus )
    {
      setColor (wc_current_element.focus_fg, wc_current_element.focus_bg);
    }
    else
      setColor (wc_current_element.fg, wc_current_element.bg);

    if ( FVTerm::getFOutput()->isMonochron() )
      setReverse(false);
  }
  else
  {
    if ( FVTerm::getFOutput()->isMonochron() )
      setReverse(true);
    else if ( is_focus && FVTerm::getFOutput()->getMaxColor() < 16 )
      unsetBold();
  }
}

//----------------------------------------------------------------------
inline auto FListView::getCheckBox (const FListViewItem* item) const -> FString
{
  FString checkbox{""};

  if ( FVTerm::getFOutput()->isNewFont() )
  {
    checkbox = ( item->isChecked() ) ? CHECKBOX_ON : CHECKBOX;
    checkbox += L' ';
  }
  else
  {
    checkbox.setString("[ ] ");

    if ( item->isChecked() )
    {
      try
      {
        checkbox[1] = wchar_t(UniChar::Times);  // Times ×
      }
      catch (const std::out_of_range&)
      {
        return checkbox;
      }
    }
  }

  return checkbox;
}

//----------------------------------------------------------------------
inline auto FListView::getLinePrefix ( const FListViewItem* item
                                     , std::size_t indent ) const -> FString
{
  FString line{""};

  if ( isTreeView() )
  {
    if ( indent > 0 )
      line = FString{indent, L' '};

    if ( item->isExpandable()  )
    {
      if ( item->isExpand() )
      {
        line += UniChar::BlackDownPointingTriangle;  // ▼
        line += L' ';
      }
      else
      {
        line += UniChar::BlackRightPointingPointer;  // ►
        line += L' ';
      }
    }
    else
      line += L"  ";
  }
  else
    line.setString(" ");

  if ( item->isCheckable() )
    line += getCheckBox(item);

  return line;
}

//----------------------------------------------------------------------
inline void FListView::drawSortIndicator ( std::size_t& length
                                         , std::size_t  column_max )
{
  if ( length >= column_max )
    return;

  setColor();
  length++;

  if ( sorting.order == SortOrder::Ascending )
    data.headerline << UniChar::BlackUpPointingTriangle;    // ▲
  else if ( sorting.order == SortOrder::Descending )
    data.headerline << UniChar::BlackDownPointingTriangle;  // ▼

  if ( length < column_max  )
  {
    length++;
    data.headerline << ' ';
  }
}

//----------------------------------------------------------------------
inline void FListView::drawHeaderBorder (std::size_t length)
{
  setColor();
  const FString line {length, UniChar::BoxDrawingsHorizontal};
  data.headerline << line;  // horizontal line
}


//----------------------------------------------------------------------
void FListView::drawHeadlineLabel (const HeaderItems::const_iterator& iter)
{
  // Print label text
  static constexpr std::size_t leading_space = 1;
  const auto& text = iter->name;
  FString txt{" " + text};
  const auto width = std::size_t(iter->width);
  std::size_t column_width = getColumnWidth(txt);
  const std::size_t column_max = leading_space + width;
  const auto first = data.header.cbegin();
  const auto column = int(std::distance(first, iter)) + 1;
  const bool has_sort_indicator( sorting.column == column && ! sorting.hide_sort_indicator );
  const auto& wc_label = getColorTheme()->label;

  if ( isEnabled() )
    setColor (wc_label.emphasis_fg, wc_label.bg);
  else
    setColor (wc_label.inactive_fg, wc_label.inactive_bg);

  if ( has_sort_indicator && column_width >= column_max - 1 && column_width > 1 )
  {
    column_width = column_max - 2;
    txt = getColumnSubString (txt, 1, column_width);
  }

  if ( column_width <= column_max )
  {
    data.headerline << txt;

    if ( column_width < column_max )
    {
      column_width++;
      data.headerline << ' ';  // trailing space
    }

    if ( has_sort_indicator )
      drawSortIndicator (column_width, column_max );

    if ( column_width < column_max )
      drawHeaderBorder (column_max - column_width);
  }
  else
    drawColumnEllipsis (iter, text);  // Print ellipsis
}

//----------------------------------------------------------------------
inline auto FListView::findHeaderStartPos (bool& left_trunc) -> FVTermBuffer::iterator
{
  std::size_t column_offset{0};
  std::size_t offset{0};
  bool stop = false;

  for (const auto& term_char : data.headerline)
  {
    if ( stop )
      break;

    if ( scroll.xoffset == 0 )
      stop = true;
    else
    {
      column_offset += getColumnWidth(term_char);
      offset++;

      if ( column_offset == std::size_t(scroll.xoffset) )
        stop = true;
      else if ( column_offset > std::size_t(scroll.xoffset) && column_offset >= 2 )
      {
        left_trunc = true;
        stop = true;
      }
    }
  }

  auto first = data.headerline.begin();
  using distance_type = FVTermBuffer::difference_type;
  std::advance(first, distance_type(offset));
  return first;
}

//----------------------------------------------------------------------
inline auto FListView::findHeaderEndPos ( FVTermBuffer::iterator first
                                        , bool left_trunc
                                        , bool& right_trunc ) -> FVTermBuffer::iterator
{
  std::size_t character{0};
  std::size_t column_width{0};
  auto last = data.headerline.end();

  if ( getColumnWidth(data.headerline) <= getClientWidth() )
    return last;

  if ( left_trunc )
    column_width++;

  for (const auto& tc : FVTermBuffer(first, last))
  {
    const uInt8 char_width = tc.attr.bit.char_width;

    if ( column_width + char_width > getClientWidth() )
    {
      right_trunc = true;
      break;
    }

    column_width += char_width;
    character++;

    if ( column_width == getClientWidth() )
      break;
  }

  last = first;
  using distance_type = FVTermBuffer::difference_type;
  std::advance(last, distance_type(character));
  return last;
}

//----------------------------------------------------------------------
void FListView::drawBufferedHeadline()
{
  // Print the FVTermBuffer object

  if ( data.headerline.isEmpty() )
    return;

  bool left_trunc{false};  // left truncated full-width
  bool right_trunc{false};  // right truncated full-width

  // Search for the start position
  auto first = findHeaderStartPos(left_trunc);

  // Search for the end position
  auto last = findHeaderEndPos (first, left_trunc, right_trunc);

  // Print the header line
  print() << FPoint{2, 1};

  if ( left_trunc )
    print (UniChar::SingleLeftAngleQuotationMark);  // ‹

  print() << FVTermBuffer(first, last);

  if ( right_trunc )
    print (UniChar::SingleRightAngleQuotationMark);  // ›

  for ( std::size_t column_width = getColumnWidth(data.headerline);
        column_width < getClientWidth();
        column_width++ )
  {
    setColor();
    print(UniChar::BoxDrawingsHorizontal);
  }
}

//----------------------------------------------------------------------
void FListView::drawColumnEllipsis ( const HeaderItems::const_iterator& iter
                                   , const FString& text )
{
  // Print label ellipsis
  static constexpr int ellipsis_length = 2;
  const int width = iter->width;
  const auto& wc_label = getColorTheme()->label;

  data.headerline << ' '
                  << getColumnSubString (text, 1, uInt(width - ellipsis_length))
                  << FColorPair {wc_label.ellipsis_fg, wc_label.bg}
                  << "..";

  if ( iter == data.header.cend() - 1 )  // Last element
    data.headerline << ' ';
}

//----------------------------------------------------------------------
void FListView::updateLayout()
{
  max_line_width = 0;
  scroll.xoffset = 0;
  std::for_each ( data.itemlist.begin()
                , data.itemlist.end()
                , [this] (FObject* obj_item)
                  {
                    const auto& item = static_cast<FListViewItem*>(obj_item);
                    std::size_t line_width = determineLineWidth (item);
                    recalculateHorizontalBar (line_width);
                  }
                );
  adjustScrollbars(getCount());
  drawList();
  drawBorder();
  drawHeadlines();
  drawScrollbars();
}

//----------------------------------------------------------------------
void FListView::updateDrawing (bool draw_vbar, bool draw_hbar)
{
  if ( isShown() )
    draw();

  scroll.vbar->setValue (scroll.first_visible_line.getPosition());

  if ( draw_vbar )
    scroll.vbar->drawBar();

  scroll.hbar->setValue (scroll.xoffset);

  if ( draw_hbar )
    scroll.hbar->drawBar();

  forceTerminalUpdate();
}

//----------------------------------------------------------------------
auto FListView::determineLineWidth (FListViewItem* item) -> std::size_t
{
  std::size_t padding_space = 1;
  std::size_t line_width = padding_space;  // leading space
  std::size_t column_idx{0};
  const auto entries = std::size_t(item->column_list.size());

  if ( hasCheckableItems() )
    line_width += checkbox_space;

  for (auto&& header_item : data.header)
  {
    const auto width = std::size_t(header_item.width);
    const bool fixed_width = header_item.fixed_width;

    if ( ! fixed_width )
    {
      std::size_t len{0};

      if ( column_idx < entries )
        len = getColumnWidth(item->column_list[column_idx]);

      if ( len > width )
        header_item.width = int(len);
    }

    if ( &header_item == &data.header.back() )  // Last column
      padding_space = 0;

    // width + trailing space
    if ( header_item.visible )
      line_width += std::size_t(header_item.width) + padding_space;

    column_idx++;
  }

  return line_width;
}

//----------------------------------------------------------------------
inline void FListView::beforeInsertion (FListViewItem* item)
{
  std::size_t line_width = determineLineWidth (item);
  recalculateHorizontalBar (line_width);
}

//----------------------------------------------------------------------
inline void FListView::afterInsertion()
{
  if ( data.itemlist.size() == 1 )  // Select first item on insert
    selection.current_iter = data.itemlist.begin();

  // The visible area of the list begins with the first element
  scroll.first_visible_line = data.itemlist.begin();

  // Sort list by a column (only if activated)
  sort();

  const std::size_t element_count = getCount();
  recalculateVerticalBar (element_count);
  processChanged();
}

//----------------------------------------------------------------------
void FListView::adjustListBeforeRemoval (const FListViewItem* item)
{
  const auto* current_item = static_cast<FListViewItem*>(*selection.current_iter);
  const auto* first_item = data.itemlist.front();
  auto end_iter = static_cast<FListViewIterator>(data.itemlist.end());
  const auto& last_item = *(--end_iter);
  const bool is_current_line( item == current_item );
  const bool is_first_line( item == first_item );
  const bool is_last_line( item == last_item );

  if ( is_current_line )
  {
    if ( is_last_line || current_item == data.itemlist.back() )
      stepBackward();
    else
    {
      collapseSubtree();
      stepForward();
    }
  }

  if ( is_first_line )
    ++scroll.first_visible_line;
}

//----------------------------------------------------------------------
void FListView::removeItemFromParent (FListViewItem* item)
{
  auto parent = item->getParent();

  if ( ! parent )
    return;

  if ( this == parent )
  {
    auto last = std::remove (data.itemlist.begin(), data.itemlist.end(), item);
    data.itemlist.erase(last, data.itemlist.end());
    delChild(item);
    selection.current_iter.getPosition()--;
    return;
  }

  parent->delChild(item);
  auto parent_item = static_cast<FListViewItem*>(parent);
  parent_item->visible_lines--;
  selection.current_iter.getPosition()--;

  if ( parent_item->hasChildren() )
    return;

  parent_item->expandable = false;
  parent_item->is_expand = false;
}

//----------------------------------------------------------------------
void FListView::updateListAfterRemoval()
{
  const std::size_t element_count = getCount();
  recalculateVerticalBar (element_count);

  if ( isItemListEmpty() )
  {
    selection.current_iter = getNullIterator();
    scroll.first_visible_line = getNullIterator();
    scroll.last_visible_line = getNullIterator();
    clearList();
  }
  else
  {
    drawList();
    drawBorder();
    drawHeadlines();
    drawScrollbars();
  }
}

//----------------------------------------------------------------------
void FListView::recalculateHorizontalBar (std::size_t len)
{
  if ( len <= max_line_width )
    return;

  max_line_width = len;

  if ( len >= getWidth() - nf_offset - 3 )
  {
    scroll.hbar->setMaximum (getScrollBarMaxHorizontal());
    scroll.hbar->setPageSize (int(max_line_width), int(getWidth() - nf_offset) - 4);
    scroll.hbar->calculateSliderValues();

    if ( ! isShown() )
      return;

    if ( isHorizontallyScrollable() )
      scroll.hbar->show();
    else
      scroll.hbar->hide();
  }
}

//----------------------------------------------------------------------
void FListView::recalculateVerticalBar (std::size_t element_count) const
{
  const std::size_t height = getClientHeight();
  scroll.vbar->setMaximum (getScrollBarMaxVertical(element_count));
  scroll.vbar->setPageSize (int(element_count), int(height));
  scroll.vbar->calculateSliderValues();

  if ( ! isShown() )
    return;

  if ( isVerticallyScrollable() )
    scroll.vbar->show();
  else
    scroll.vbar->hide();
}

//----------------------------------------------------------------------
void FListView::mouseHeaderClicked()
{
  int column{1};
  const int checkbox_offset = ( hasCheckableItems() ) ? checkbox_space : 0;
  const int header_pos = selection.clicked_header_pos.getX() + scroll.xoffset;
  int header_start = 2 + checkbox_offset;

  for (const auto& item : data.header)
  {
    if ( isColumnHidden(column) )
    {
      column++;
      continue;
    }

    const auto click_width = getHeaderClickWidth (item, column);

    if ( isPositionWithinHeader(header_start, click_width, header_pos) )
    {
      handleColumnSort(column);
      break;
    }

    static constexpr int leading_space = 1;
    header_start += leading_space + item.width;
    column++;
  }
}

//----------------------------------------------------------------------
inline auto FListView::getHeaderClickWidth (const Header& item, int column) const -> int
{
  const bool has_sort_indicator( column == sorting.column );
  auto click_width = int(getColumnWidth(item.name));

  if ( has_sort_indicator )
    click_width += 2;

  if ( click_width > item.width )
    return item.width;

  return click_width;
}

//----------------------------------------------------------------------
inline auto FListView::isPositionWithinHeader (int start, int width, int pos) const -> bool
{
  const int end = start + width;
  return pos > start && pos <= end;
}

//----------------------------------------------------------------------
void FListView::handleColumnSort (int column)
{
  const bool has_sort_indicator( column == sorting.column );

  if ( has_sort_indicator && sorting.order == SortOrder::Ascending )
    setColumnSort (column, SortOrder::Descending);
  else
    setColumnSort (column, SortOrder::Ascending);

  sort();

  if ( isShown() )
    updateDrawing (true, false);
}

//----------------------------------------------------------------------
void FListView::handleTreeExpanderClick (const FMouseEvent* ev)
{
  const auto& item = getCurrentItem();

  if ( ! isTreeView()
    || ! item->isExpandable()
    || selection.clicked_expander_pos != ev->getPos() )
    return;

  toggleItemExpandState(item);
  adjustScrollbars (getCount());

  if ( isShown() )
    draw();
}

//----------------------------------------------------------------------
void FListView::handleCheckboxClick (const FMouseEvent* ev)
{
  const auto& item = getCurrentItem();
  int indent = isTreeView() ? int(item->getDepth() << 1u)  // indent = 2 * depth
                            : 0;

  if ( ! ( hasCheckableItems()
        && selection.clicked_checkbox_item == item
        && isCheckboxClicked(ev->getX(), indent) ) )
    return;

  toggleItemCheckState(item);

  if ( isShown() )
    draw();
}

//----------------------------------------------------------------------
void FListView::wheelUp (int pagesize)
{
  if ( isItemListEmpty() || selection.current_iter.getPosition() == 0 )
    return;

  if ( scroll.first_visible_line.getPosition() >= pagesize )
  {
    selection.current_iter -= pagesize;
    scroll.first_visible_line -= pagesize;
    scroll.last_visible_line -= pagesize;
  }
  else
  {
    // Save relative position from the first line
    const int ry = selection.current_iter.getPosition() - scroll.first_visible_line.getPosition();
    // Save difference from top
    const int difference = scroll.first_visible_line.getPosition();
    scroll.first_visible_line -= difference;
    scroll.last_visible_line -= difference;
    setRelativePosition(ry);
  }
}

//----------------------------------------------------------------------
void FListView::wheelDown (int pagesize)
{
  if ( isItemListEmpty() )
    return;

  const auto element_count = int(getCount());

  if ( selection.current_iter.getPosition() + 1 == element_count )
    return;

  if ( scroll.last_visible_line.getPosition() < element_count - pagesize )
  {
    selection.current_iter += pagesize;
    scroll.first_visible_line += pagesize;
    scroll.last_visible_line += pagesize;
  }
  else
  {
    // Save relative position from the first line
    const int ry = selection.current_iter.getPosition() - scroll.first_visible_line.getPosition();
    // Save difference from bottom
    const int differenz = element_count - scroll.last_visible_line.getPosition() - 1;
    scroll.first_visible_line += differenz;
    scroll.last_visible_line += differenz;
    setRelativePosition(ry);
  }
}

//----------------------------------------------------------------------
void FListView::wheelLeft (int pagesize)
{
  if ( isItemListEmpty() || scroll.xoffset == 0 )
    return;

  const int xoffset_before = scroll.xoffset;
  scrollBy (-pagesize, 0);
  const bool draw_hbar(xoffset_before != scroll.xoffset);
  updateDrawing (false, draw_hbar);
}

//----------------------------------------------------------------------
void FListView::wheelRight (int pagesize)
{
  if ( isItemListEmpty() )
    return;

  const int xoffset_before = scroll.xoffset;
  scrollBy (pagesize, 0);
  const bool draw_hbar(xoffset_before != scroll.xoffset);
  updateDrawing (false, draw_hbar);
}

//----------------------------------------------------------------------
auto FListView::dragScrollUp (int position_before) -> bool
{
  if ( position_before == 0 )
  {
    drag_scroll = DragScrollMode::None;
    return false;
  }

  stepBackward(scroll.distance);
  return true;
}

//----------------------------------------------------------------------
auto FListView::dragScrollDown (int position_before) -> bool
{
  const auto element_count = int(getCount());

  if ( position_before + 1 == element_count )
  {
    drag_scroll = DragScrollMode::None;
    return false;
  }

  stepForward(scroll.distance);
  return true;
}

//----------------------------------------------------------------------
void FListView::dragUp (MouseButton mouse_button)
{
  if ( isDragging(drag_scroll)
    && scroll.distance < int(getClientHeight()) )
    scroll.distance++;

  if ( ! scroll.timer && selection.current_iter.getPosition() > 0 )
  {
    scroll.timer = true;
    addTimer(scroll.repeat);

    if ( mouse_button == MouseButton::Right )
      drag_scroll = DragScrollMode::SelectUpward;
    else
      drag_scroll = DragScrollMode::Upward;
  }

  if ( selection.current_iter.getPosition() == 0 )
  {
    delOwnTimers();
    drag_scroll = DragScrollMode::None;
  }
}

//----------------------------------------------------------------------
void FListView::dragDown (MouseButton mouse_button)
{
  if ( isDragging(drag_scroll)
    && scroll.distance < int(getClientHeight()) )
    scroll.distance++;

  if ( ! scroll.timer && selection.current_iter.getPosition() <= int(getCount()) )
  {
    scroll.timer = true;
    addTimer(scroll.repeat);

    if ( mouse_button == MouseButton::Right )
      drag_scroll = DragScrollMode::SelectDownward;
    else
      drag_scroll = DragScrollMode::Downward;
  }

  if ( selection.current_iter.getPosition() - 1 == int(getCount()) )
  {
    delOwnTimers();
    drag_scroll = DragScrollMode::None;
  }
}

//----------------------------------------------------------------------
inline void FListView::stopDragScroll()
{
  delOwnTimers();
  scroll.timer = false;
  scroll.distance = 1;
  drag_scroll = DragScrollMode::None;
}

//----------------------------------------------------------------------
inline void FListView::toggleItemExpandState (FListViewItem* item) const
{
  if ( item->isExpand() )
    item->collapse();
  else
    item->expand();
}

//----------------------------------------------------------------------
inline auto FListView::isCheckboxClicked (int mouse_x, int indent) const -> bool
{
  if ( isTreeView() )
    indent++;  // Plus one space

  return mouse_x >= 3 + indent - scroll.xoffset
      && mouse_x <= 5 + indent - scroll.xoffset;
}

//----------------------------------------------------------------------
inline void FListView::resetClickedPositions()
{
  selection.clicked_expander_pos.setPoint(-1, -1);
  selection.clicked_header_pos.setPoint(-1, -1);
  selection.clicked_checkbox_item = nullptr;
}

//----------------------------------------------------------------------
auto FListView::isWithinHeaderBounds (const FPoint& pos) const -> bool
{
  return pos.getX() > 1
      && pos.getX() < int(getWidth())
      && pos.getY() == 1;
}

//----------------------------------------------------------------------
auto FListView::isWithinListBounds (const FPoint& pos) const -> bool
{
  return pos.getX() > 1
      && pos.getX() < int(getWidth())
      && pos.getY() > 1
      && pos.getY() < int(getHeight());
}

//----------------------------------------------------------------------
auto FListView::appendItem (FListViewItem* item) -> FObject::iterator
{
  item->root = data.root;
  addChild (item);
  data.itemlist.push_back (item);
  return --data.itemlist.end();
}

//----------------------------------------------------------------------
void FListView::handleListEvent (const FMouseEvent* ev)
{
  const int new_pos = scroll.first_visible_line.getPosition() + ev->getY() - 2;

  if ( new_pos < int(getCount()) )
    setRelativePosition (ev->getY() - 2);

  const auto& item = getCurrentItem();

  // Handle specialized events
  handleTreeViewEvents(ev, item);
  handleCheckableItemsEvents(ev, item);

  // Redraw the list and update the vertical scrollbar
  if ( isShown() )
    drawList();

  scroll.vbar->setValue (scroll.first_visible_line.getPosition());

  if ( scroll.first_line_position_before != scroll.first_visible_line.getPosition() )
    scroll.vbar->drawBar();

  forceTerminalUpdate();
}

//----------------------------------------------------------------------
void FListView::handleTreeViewEvents (const FMouseEvent* ev, const FListViewItem* item)
{
  if ( ! isTreeView() )
    return;

  const auto indent = int(item->getDepth() << 1u);  // indent = 2 * depth

  if ( item->isExpandable() && ev->getX() - 2 == indent - scroll.xoffset )
    selection.clicked_expander_pos = ev->getPos();
}

//----------------------------------------------------------------------
void FListView::handleCheckableItemsEvents (const FMouseEvent* ev, const FListViewItem* item)
{
  if ( ! hasCheckableItems() )
    return;

  auto indent = int(item->getDepth() << 1u);  // indent = 2 * depth

  if ( isTreeView() )
    indent++;  // Plus one space

  if ( item->isCheckable() && isCheckboxClicked(ev->getX(), indent) )
  {
    selection.clicked_checkbox_item = item;
  }
}

//----------------------------------------------------------------------
void FListView::processClick() const
{
  if ( isItemListEmpty() )
    return;

  emitCallback("clicked");
}

//----------------------------------------------------------------------
void FListView::processRowChanged() const
{
  emitCallback("row-changed");
}

//----------------------------------------------------------------------
void FListView::processChanged() const
{
  emitCallback("changed");
}

//----------------------------------------------------------------------
void FListView::changeOnResize() const
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
inline void FListView::toggleCheckbox()
{
  if ( isItemListEmpty() )
    return;

  const auto item = getCurrentItem();

  if ( item->isCheckable() )
    toggleItemCheckState(item);
}

//----------------------------------------------------------------------
inline void FListView::collapseAndScrollLeft()
{
  const auto item = getCurrentItem();

  if ( scroll.xoffset != 0 || ! item || isItemListEmpty() )
  {
    if ( scroll.xoffset > 0 )  // Scroll left
      scroll.xoffset--;

    return;
  }

  if ( isTreeView() && item->isExpandable() && item->isExpand() )
  {
    // Collapse element
    item->collapse();
    adjustSize();
    scroll.vbar->calculateSliderValues();
    // Force vertical scrollbar redraw
    scroll.first_line_position_before = -1;
    return;
  }

  jumpToParentElement(item);
}

//----------------------------------------------------------------------
inline void FListView::jumpToParentElement (const FListViewItem* item)
{
  if ( ! item->hasParent()
    || ! item->getParent()->isInstanceOf("FListViewItem") )
    return;

  const int position_before = selection.current_iter.getPosition();
  selection.current_iter.parentElement();  // Set the iterator to the parent

  if ( selection.current_iter.getPosition() >= scroll.first_line_position_before )
    return;

  const int difference = position_before - selection.current_iter.getPosition();

  if ( scroll.first_visible_line.getPosition() - difference >= 0 )
  {
    scroll.first_visible_line -= difference;
    scroll.last_visible_line -= difference;
  }
  else
  {
    const int d = scroll.first_visible_line.getPosition();
    scroll.first_visible_line -= d;
    scroll.last_visible_line -= d;
  }
}

//----------------------------------------------------------------------
inline void FListView::expandAndScrollRight()
{
  const int xoffset_end = int(max_line_width) - int(getClientWidth());
  const auto item = getCurrentItem();

  if ( isTreeView() && ! isItemListEmpty() && item
    && item->isExpandable() && ! item->isExpand() )
  {
    // Expand element
    item->expand();
    adjustScrollbars (getCount());
    // Force vertical scrollbar redraw
    scroll.first_line_position_before = -1;
  }
  else
  {
    // Scroll right
    if ( scroll.xoffset < xoffset_end )
      scroll.xoffset++;

    if ( scroll.xoffset < 0 )
      scroll.xoffset = 0;
  }
}

//----------------------------------------------------------------------
inline void FListView::firstPos()
{
  if ( isItemListEmpty() )
    return;

  selection.current_iter -= selection.current_iter.getPosition();
  const int difference = scroll.first_visible_line.getPosition();
  scroll.first_visible_line -= difference;
  scroll.last_visible_line -= difference;
}

//----------------------------------------------------------------------
inline void FListView::lastPos()
{
  if ( isItemListEmpty() )
    return;

  const auto element_count = int(getCount());
  selection.current_iter += element_count - selection.current_iter.getPosition() - 1;
  const int difference = element_count - scroll.last_visible_line.getPosition() - 1;
  scroll.first_visible_line += difference;
  scroll.last_visible_line += difference;
}

//----------------------------------------------------------------------
inline auto FListView::expandSubtree() -> bool
{
  if ( isItemListEmpty() )
    return false;

  auto item = getCurrentItem();

  if ( isTreeView() && item->isExpandable() && ! item->isExpand() )
  {
    item->expand();
    adjustScrollbars (getCount());
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
inline auto FListView::collapseSubtree() -> bool
{
  if ( isItemListEmpty() )
    return false;

  auto item = getCurrentItem();

  if ( isTreeView() && item->isExpandable() && item->isExpand() )
  {
    item->collapse();
    adjustScrollbars (getCount());
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
void FListView::setRelativePosition (int ry)
{
  selection.current_iter = scroll.first_visible_line + ry;
}

//----------------------------------------------------------------------
void FListView::stepForward()
{
  if ( isItemListEmpty() )
    return;

  if ( selection.current_iter == scroll.last_visible_line )
  {
    ++scroll.last_visible_line;

    if ( scroll.last_visible_line == data.itemlist.end() )
      --scroll.last_visible_line;
    else
      ++scroll.first_visible_line;
  }

  ++selection.current_iter;

  if ( selection.current_iter == data.itemlist.end() )
    --selection.current_iter;
}

//----------------------------------------------------------------------
void FListView::stepBackward()
{
  if ( isItemListEmpty() )
    return;

  if ( selection.current_iter == scroll.first_visible_line
    && selection.current_iter != data.itemlist.begin() )
  {
    --scroll.first_visible_line;
    --scroll.last_visible_line;
  }

  if ( selection.current_iter != data.itemlist.begin() )
    --selection.current_iter;
}

//----------------------------------------------------------------------
void FListView::stepForward (int distance)
{
  if ( isItemListEmpty() )
    return;

  const auto element_count = int(getCount());

  if ( selection.current_iter.getPosition() + 1 == element_count )
    return;

  if ( selection.current_iter.getPosition() + distance < element_count )
  {
    selection.current_iter += distance;
  }
  else
  {
    selection.current_iter += element_count - selection.current_iter.getPosition() - 1;
  }

  if ( selection.current_iter.getPosition() > scroll.last_visible_line.getPosition() )
  {
    if ( scroll.last_visible_line.getPosition() + distance < element_count )
    {
      scroll.first_visible_line += distance;
      scroll.last_visible_line += distance;
    }
    else
    {
      const int differenz = element_count - scroll.last_visible_line.getPosition() - 1;
      scroll.first_visible_line += differenz;
      scroll.last_visible_line += differenz;
    }
  }
}

//----------------------------------------------------------------------
void FListView::stepBackward (int distance)
{
  if ( isItemListEmpty() || selection.current_iter.getPosition() == 0 )
    return;

  if ( selection.current_iter.getPosition() - distance >= 0 )
  {
    selection.current_iter -= distance;
  }
  else
  {
    selection.current_iter -= selection.current_iter.getPosition();
  }

  if ( selection.current_iter.getPosition() < scroll.first_visible_line.getPosition() )
  {
    if ( scroll.first_visible_line.getPosition() - distance >= 0 )
    {
      scroll.first_visible_line -= distance;
      scroll.last_visible_line -= distance;
    }
    else
    {
      const int difference = scroll.first_visible_line.getPosition();
      scroll.first_visible_line -= difference;
      scroll.last_visible_line -= difference;
    }
  }
}

//----------------------------------------------------------------------
void FListView::scrollToX (int x)
{
  if ( scroll.xoffset == x )
    return;

  scroll.xoffset = x;
  const int xoffset_end = int(max_line_width) - int(getClientWidth());
  scroll.xoffset = std::max(0, std::min(scroll.xoffset, xoffset_end));
}

//----------------------------------------------------------------------
void FListView::scrollToY (int y)
{
  const int pagesize = int(getClientHeight()) - 1;
  const auto element_count = int(getCount());

  if ( scroll.first_visible_line.getPosition() == y )
    return;

  // Save relative position from the top line
  const int ry = selection.current_iter.getPosition() - scroll.first_visible_line.getPosition();

  if ( y + pagesize <= element_count )
  {
    scroll.first_visible_line = FListViewIterator(data.itemlist.begin()) + y;
    setRelativePosition (ry);
    scroll.last_visible_line = scroll.first_visible_line + pagesize;
  }
  else
  {
    const int differenz = element_count - scroll.last_visible_line.getPosition() - 1;
    selection.current_iter += differenz;
    scroll.first_visible_line += differenz;
    scroll.last_visible_line += differenz;
  }
}

//----------------------------------------------------------------------
void FListView::scrollTo (int x, int y)
{
  scrollToX(x);
  scrollToY(y);
}

//----------------------------------------------------------------------
void FListView::scrollBy (int dx, int dy)
{
  scrollToX(scroll.xoffset + dx);

  if ( dy > 0 )
    stepForward(dy);

  if ( dy < 0 )
    stepBackward(-dy);
}

//----------------------------------------------------------------------
inline auto FListView::getScrollBarMaxHorizontal() const noexcept -> int
{
  const std::size_t width = getClientWidth();
  return max_line_width > width
         ? int(max_line_width - width)
         : 0;
}

//----------------------------------------------------------------------
inline auto FListView::getScrollBarMaxVertical (const std::size_t element_count) const noexcept -> int
{
  const std::size_t height = getClientHeight();
  return element_count > height
         ? int(element_count - height)
         : 0;
}

//----------------------------------------------------------------------
inline void FListView::updateViewAfterVBarChange (const FScrollbar::ScrollType scroll_type)
{
  if ( isShown() )
    drawList();

  if ( scroll_type >= FScrollbar::ScrollType::StepBackward
    && scroll_type <= FScrollbar::ScrollType::PageForward )
  {
    scroll.vbar->setValue (scroll.first_visible_line.getPosition());

    if ( scroll.first_line_position_before != scroll.first_visible_line.getPosition() )
      scroll.vbar->drawBar();

    forceTerminalUpdate();
  }
}

//----------------------------------------------------------------------
inline void FListView::updateViewAfterHBarChange ( const FScrollbar::ScrollType scroll_type
                                                 , const int xoffset_before )
{
  if ( isShown() )
  {
    drawHeadlines();
    drawList();
  }

  if ( scroll_type >= FScrollbar::ScrollType::StepBackward )
  {
    scroll.hbar->setValue (scroll.xoffset);

    if ( xoffset_before != scroll.xoffset )
      scroll.hbar->drawBar();

    forceTerminalUpdate();
  }
}

//----------------------------------------------------------------------
inline auto FListView::getVerticalScrollDistance (const FScrollbar::ScrollType scroll_type) const -> int
{
  if ( scroll_type == FScrollbar::ScrollType::PageBackward
    || scroll_type == FScrollbar::ScrollType::PageForward )
  {
    return int(getClientHeight());
  }

  return 1;
}

//----------------------------------------------------------------------
inline auto FListView::getHorizontalScrollDistance (const FScrollbar::ScrollType scroll_type) const -> int
{
  if ( scroll_type == FScrollbar::ScrollType::PageBackward
    || scroll_type == FScrollbar::ScrollType::PageForward )
  {
    return int(getClientWidth());
  }

  return 1;
}

//----------------------------------------------------------------------
void FListView::cb_vbarChange (const FWidget*)
{
  const FScrollbar::ScrollType scroll_type = scroll.vbar->getScrollType();
  static constexpr int wheel_distance = 4;
  scroll.first_line_position_before = scroll.first_visible_line.getPosition();
  int distance = getVerticalScrollDistance(scroll_type);

  switch ( scroll_type )
  {
    case FScrollbar::ScrollType::PageBackward:
    case FScrollbar::ScrollType::StepBackward:
      stepBackward(distance);
      break;

    case FScrollbar::ScrollType::PageForward:
    case FScrollbar::ScrollType::StepForward:
      stepForward(distance);
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

  updateViewAfterVBarChange (scroll_type);
}

//----------------------------------------------------------------------
void FListView::cb_hbarChange (const FWidget*)
{
  const FScrollbar::ScrollType scroll_type = scroll.hbar->getScrollType();
  static constexpr int wheel_distance = 4;
  const int xoffset_before = scroll.xoffset;
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
      scrollToX (scroll.hbar->getValue());
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

  updateViewAfterHBarChange (scroll_type, xoffset_before);
}

}  // namespace finalcut
