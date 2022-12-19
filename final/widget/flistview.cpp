/***********************************************************************
* flistview.cpp - Widget FListView and FListViewItem                   *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2017-2022 Markus Gans                                      *
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
auto sortAscendingByName (const FObject*, const FObject*) -> bool;
auto sortDescendingByName (const FObject*, const FObject*) -> bool;
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
auto sortAscendingByName (const FObject* lhs, const FObject* rhs) -> bool
{
  const auto& l_item = static_cast<const FListViewItem*>(lhs);
  const auto& r_item = static_cast<const FListViewItem*>(rhs);
  const int column = l_item->getSortColumn();
  const auto& l_string = l_item->getText(column);
  const auto& r_string = r_item->getText(column);

  // lhs < rhs
  return FStringCaseCompare(l_string, r_string) < 0;
}

//----------------------------------------------------------------------
auto sortDescendingByName (const FObject* lhs, const FObject* rhs) -> bool
{
  const auto& l_item = static_cast<const FListViewItem*>(lhs);
  const auto& r_item = static_cast<const FListViewItem*>(rhs);
  const int column = l_item->getSortColumn();
  const auto& l_string = l_item->getText(column);
  const auto& r_string = r_item->getText(column);

  // lhs > rhs
  return FStringCaseCompare(l_string, r_string) > 0;
}

//----------------------------------------------------------------------
auto sortAscendingByNumber (const FObject* lhs, const FObject* rhs) -> bool
{
  const auto& l_item = static_cast<const FListViewItem*>(lhs);
  const auto& r_item = static_cast<const FListViewItem*>(rhs);
  const int column = l_item->getSortColumn();
  const auto& l_number = firstNumberFromString(l_item->getText(column));
  const auto& r_number = firstNumberFromString(r_item->getText(column));

  // lhs < rhs
  return l_number < r_number;
}

//----------------------------------------------------------------------
auto sortDescendingByNumber (const FObject* lhs, const FObject* rhs) -> bool
{
  const auto& l_item = static_cast<const FListViewItem*>(lhs);
  const auto& r_item = static_cast<const FListViewItem*>(rhs);
  const int column = l_item->getSortColumn();
  const auto& l_number = firstNumberFromString(l_item->getText(column));
  const auto& r_number = firstNumberFromString(r_item->getText(column));

  // lhs > rhs
  return l_number > r_number;
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

    if ( ! listview->header[index].fixed_width )
    {
      const auto column_width = int(getColumnWidth(text));

      if ( column_width > listview->header[index].width )
        listview->header[index].width = column_width;
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

  if ( item->isExpandable() && item->isExpand() )
  {
    iter_path.push(iter);
    iter = item->begin();
    position++;
  }
  else
  {
    position++;
    bool forward{};

    do
    {
      forward = false;  // Reset forward
      ++iter;

      if ( ! iter_path.empty() )
      {
        const auto& parent_iter = iter_path.top();

        if ( iter == (*parent_iter)->cend() )
        {
          iter = parent_iter;
          iter_path.pop();
          forward = true;
        }
      }
    }
    while ( forward );
  }
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
  int n{0};

  for (auto&& item : itemlist)
  {
    const auto& listitem = static_cast<FListViewItem*>(item);
    n += listitem->getVisibleLines();
  }

  return std::size_t(n);
}

//----------------------------------------------------------------------
auto FListView::getColumnAlignment (int column) const -> Align
{
  // Get the alignment for a column

  if ( column < 1 || header.empty() || column > int(header.size()) )
    return Align::Left;

  // Convert column position to address offset (index)
  const auto index = std::size_t(column - 1);
  return header[index].alignment;
}

//----------------------------------------------------------------------
auto FListView::getColumnText (int column) const -> FString
{
  // Get the text of column

  if ( column < 1 || header.empty() || column > int(header.size()) )
    return fc::emptyFString::get();

  // Convert column position to address offset (index)
  const auto index = std::size_t(column - 1);
  return header[index].name;
}

//----------------------------------------------------------------------
auto FListView::getColumnSortType (int column) const -> SortType
{
  SortType s_type;
  const auto col = std::size_t(column);

  try
  {
    s_type = sort_type.at(col);
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

  if ( column < 1 || header.empty() || column > int(header.size()) )
    return;

  // Convert column position to address offset (index)
  const auto index = std::size_t(column - 1);
  header[index].alignment = align;
}

//----------------------------------------------------------------------
void FListView::setColumnText (int column, const FString& label)
{
  // Set the text for a column

  if ( column < 1 || header.empty() || column > int(header.size()) )
    return;

  // Convert column position to address offset (index)
  auto index = std::size_t(column - 1);

  if ( ! header[index].fixed_width )
  {
    const auto column_width = int(getColumnWidth(label));

    if ( column_width > header[index].width )
      header[index].width = column_width;
  }

  header[index].name = label;
}

//----------------------------------------------------------------------
void FListView::setColumnSortType (int column, SortType type)
{
  // Sets the sort type by which the list is to be sorted

  if ( column < 1 || header.empty() || column > int(header.size()) )
    return;

  const std::size_t size = std::size_t(column) + 1;

  if ( sort_type.empty() || sort_type.size() < size )
    sort_type.resize(size);

  sort_type[uInt(column)] = type;
}

//----------------------------------------------------------------------
void FListView::setColumnSort (int column, SortOrder order)
{
  // Sets the column to sort by + the sorting order

  if ( column < 1 || header.empty() || column > int(header.size()) )
    column = -1;

  sort_column = column;
  sort_order = order;
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

  header.push_back (new_column);
  return int(std::distance(header.cbegin(), header.cend()));
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

  if ( parent_iter == root )
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

  auto parent = item->getParent();
  const auto& current_item = static_cast<FListViewItem*>(*current_iter);
  const auto& first_item = itemlist.front();
  auto end_iter = static_cast<FListViewIterator>(itemlist.end());
  const auto& last_item = *(--end_iter);
  const bool is_current_line( item == current_item );
  const bool is_first_line( item == first_item );
  const bool is_last_line( item == last_item );

  if ( is_current_line )
  {
    if ( is_last_line || current_item == itemlist.back() )
      stepBackward();
    else
    {
      collapseSubtree();
      stepForward();
    }
  }

  if ( is_first_line )
    ++first_visible_line;

  if ( parent )
  {
    if ( this == parent )
    {
      auto last = std::remove (itemlist.begin(), itemlist.end(), item);
      itemlist.erase(last, itemlist.end());
      delChild(item);
      current_iter.getPosition()--;
    }
    else
    {
      parent->delChild(item);
      auto parent_item = static_cast<FListViewItem*>(parent);
      parent_item->visible_lines--;
      current_iter.getPosition()--;

      if ( ! parent_item->hasChildren() )
      {
        parent_item->expandable = false;
        parent_item->is_expand = false;
      }
    }
  }

  const std::size_t element_count = getCount();
  recalculateVerticalBar (element_count);

  if ( isItemListEmpty() )
  {
    current_iter = getNullIterator();
    first_visible_line = getNullIterator();
    last_visible_line = getNullIterator();
    clearList();
  }
  else
  {
    drawList();
    drawBorder();
    drawHeadlines();
    drawScrollbars();
  }

  processChanged();
}

//----------------------------------------------------------------------
void FListView::clear()
{
  itemlist.clear();
  current_iter = getNullIterator();
  first_visible_line = getNullIterator();
  last_visible_line = getNullIterator();
  recalculateVerticalBar (0);
  first_line_position_before = -1;
  xoffset = 0;
  vbar->setMinimum(0);
  vbar->setValue(0);
  vbar->hide();
  clearList();
  processChanged();
}

//----------------------------------------------------------------------
void FListView::sort()
{
  // Sorts the list view according to the specified setting

  if ( sort_column < 1 || sort_column > int(header.size()) )
    return;

  SortType column_sort_type = getColumnSortType(sort_column);
  std::function<bool(const FObject*, const FObject*)> comparator;

  switch ( column_sort_type )
  {
    case SortType::Unknown:
    case SortType::Name:
      comparator = ( sort_order == SortOrder::Ascending )
                 ? sortAscendingByName
                 : sortDescendingByName;
      break;

    case SortType::Number:
      comparator = ( sort_order == SortOrder::Ascending )
                 ? sortAscendingByNumber
                 : sortDescendingByNumber;
      break;

    case SortType::UserDefined:
      comparator = ( sort_order == SortOrder::Ascending )
                 ? user_defined_ascending
                 : user_defined_descending;
      break;

    default:
      return;
  }

  sort(comparator);
  current_iter = itemlist.begin();
  first_visible_line = itemlist.begin();
  processChanged();
}

//----------------------------------------------------------------------
void FListView::onKeyPress (FKeyEvent* ev)
{
  const int position_before = current_iter.getPosition();
  const int xoffset_before = xoffset;
  first_line_position_before = first_visible_line.getPosition();
  clicked_expander_pos.setPoint(-1, -1);
  processKeyAction(ev);  // Process the keystrokes

  if ( position_before != current_iter.getPosition() )
    processRowChanged();

  if ( ev->isAccepted() )
  {
    const bool draw_vbar( first_line_position_before
                       != first_visible_line.getPosition() );
    const bool draw_hbar(xoffset_before != xoffset);
    updateDrawing (draw_vbar, draw_hbar);
  }
}

//----------------------------------------------------------------------
void FListView::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != MouseButton::Left )
  {
    clicked_expander_pos.setPoint(-1, -1);
    return;
  }

  setWidgetFocus(this);
  first_line_position_before = first_visible_line.getPosition();

  if ( isWithinHeaderBounds(ev->getPos()) )  // Header
  {
    clicked_header_pos = ev->getPos();
  }
  else if ( isWithinListBounds(ev->getPos()) )  // List
  {
    if ( isItemListEmpty() )
      return;

    int indent = 0;
    const int new_pos = first_visible_line.getPosition() + ev->getY() - 2;

    if ( new_pos < int(getCount()) )
      setRelativePosition (ev->getY() - 2);

    const auto& item = getCurrentItem();

    if ( isTreeView() )
    {
      indent = int(item->getDepth() << 1u);  // indent = 2 * depth

      if ( item->isExpandable() && ev->getX() - 2 == indent - xoffset )
        clicked_expander_pos = ev->getPos();
    }

    if ( hasCheckableItems() )
    {
      if ( isTreeView() )
        indent++;  // Plus one space

      if ( item->isCheckable() && isCheckboxClicked(ev->getX(), indent) )
      {
        clicked_checkbox_item = item;
      }
    }

    if ( isShown() )
      drawList();

    vbar->setValue (first_visible_line.getPosition());

    if ( first_line_position_before != first_visible_line.getPosition() )
      vbar->drawBar();

    forceTerminalUpdate();
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
    && clicked_header_pos == ev->getPos() )  // Header
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

  int indent{0};
  const auto& item = getCurrentItem();

  if ( isTreeView() )
  {
    indent = int(item->getDepth() << 1u);  // indent = 2 * depth

    if ( item->isExpandable()
      && clicked_expander_pos == ev->getPos() )
    {
      toggleItemExpandState(item);
      adjustScrollbars (getCount());

      if ( isShown() )
        draw();
    }
  }

  if ( hasCheckableItems() && clicked_checkbox_item == item
    && isCheckboxClicked(ev->getX(), indent) )
  {
    toggleItemCheckState(item);

    if ( isShown() )
      draw();
  }

  processRowChanged();
  resetClickedPositions();
}

//----------------------------------------------------------------------
void FListView::onMouseMove (FMouseEvent* ev)
{
  if ( ev->getButton() != MouseButton::Left )
  {
    clicked_expander_pos.setPoint(-1, -1);
    return;
  }

  const int mouse_y = ev->getY();
  first_line_position_before = first_visible_line.getPosition();

  if ( isWithinListBounds(ev->getPos()) )
  {
    const int new_pos = first_visible_line.getPosition() + mouse_y - 2;

    if ( new_pos < int(getCount()) )
      setRelativePosition (mouse_y - 2);

    if ( isShown() )
      drawList();

    vbar->setValue (first_visible_line.getPosition());

    if ( first_line_position_before != first_visible_line.getPosition() )
      vbar->drawBar();

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
    if ( first_visible_line.getPosition() + ev->getY() - 1 > int(getCount()) )
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

  clicked_expander_pos.setPoint(-1, -1);
}

//----------------------------------------------------------------------
void FListView::onTimer (FTimerEvent*)
{
  const int position_before = current_iter.getPosition();
  first_line_position_before = first_visible_line.getPosition();

  if ( ( drag_scroll == DragScrollMode::Upward
      || drag_scroll == DragScrollMode::SelectUpward )
      && ! dragScrollUp(position_before) )
  {
    return;
  }

  if ( ( drag_scroll == DragScrollMode::Downward
      || drag_scroll == DragScrollMode::SelectDownward )
      && ! dragScrollDown(position_before) )
  {
    return;
  }

  if ( isShown() )
    drawList();

  vbar->setValue (first_visible_line.getPosition());

  if ( first_line_position_before != first_visible_line.getPosition() )
    vbar->drawBar();

  forceTerminalUpdate();
}

//----------------------------------------------------------------------
void FListView::onWheel (FWheelEvent* ev)
{
  const int position_before = current_iter.getPosition();
  static constexpr int wheel_distance = 4;
  first_line_position_before = first_visible_line.getPosition();

  if ( isDragging(drag_scroll) )
    stopDragScroll();

  if ( ev->getWheel() == MouseWheel::Up )
    wheelUp (wheel_distance);
  else if ( ev->getWheel() == MouseWheel::Down )
    wheelDown (wheel_distance);

  if ( position_before != current_iter.getPosition() )
    processRowChanged();

  if ( isShown() )
    drawList();

  vbar->setValue (first_visible_line.getPosition());

  if ( first_line_position_before != first_visible_line.getPosition() )
    vbar->drawBar();

  forceTerminalUpdate();
}

//----------------------------------------------------------------------
void FListView::onFocusIn (FFocusEvent*)
{
  if ( getStatusBar() )
    getStatusBar()->drawMessage();
}

//----------------------------------------------------------------------
void FListView::onFocusOut (FFocusEvent*)
{
  if ( getStatusBar() )
  {
    getStatusBar()->clearMessage();
    getStatusBar()->drawMessage();
  }

  delOwnTimers();
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
    first_visible_line = itemlist.begin();
    last_visible_line = first_visible_line;
    last_visible_line += element_count - 1;
  }

  if ( first_visible_line.getPosition() > element_count - height  )
  {
    const int difference = first_visible_line.getPosition()
                         - (element_count - height);

    if ( first_visible_line.getPosition() >= difference )
    {
      first_visible_line -= difference;
      last_visible_line -= difference;
    }
  }

  const int after_last_visible_line = first_visible_line.getPosition()
                                    + height;

  if ( last_visible_line.getPosition() >= after_last_visible_line )
  {
    last_visible_line = first_visible_line;
    last_visible_line += height - 1;
  }

  if ( current_iter.getPosition() > last_visible_line.getPosition() )
    current_iter = last_visible_line;
}

//----------------------------------------------------------------------
void FListView::adjustScrollbars (const std::size_t element_count) const
{
  const std::size_t width = getClientWidth();
  const std::size_t height = getClientHeight();
  const int vmax = ( element_count > height )
                   ? int(element_count - height)
                   : 0;
  vbar->setMaximum (vmax);
  vbar->setPageSize (int(element_count), int(height));
  vbar->setX (int(getWidth()));
  vbar->setHeight (height, false);
  vbar->resize();

  const int hmax = ( max_line_width > width )
                   ? int(max_line_width - width)
                   : 0;
  hbar->setMaximum (hmax);
  hbar->setPageSize (int(max_line_width), int(width));
  hbar->setY (int(getHeight()));
  hbar->setWidth (width, false);
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
void FListView::init()
{
  initScrollbar (vbar, Orientation::Vertical, this, &FListView::cb_vbarChange);
  initScrollbar (hbar, Orientation::Horizontal, this, &FListView::cb_hbarChange);
  selflist.push_back(this);
  root = selflist.begin();
  getNullIterator() = selflist.end();
  FListView::setGeometry (FPoint{1, 1}, FSize{5, 4}, false);  // initialize geometry values
  mapKeyFunctions();
}

//----------------------------------------------------------------------
inline void FListView::mapKeyFunctions()
{
  key_map =
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

  key_map_result =
  {
    { FKey('+'), [this] { return expandSubtree(); } },
    { FKey('-'), [this] { return collapseSubtree(); } }
  };
}

//----------------------------------------------------------------------
void FListView::processKeyAction (FKeyEvent* ev)
{
  const auto idx = ev->key();
  const auto& entry = key_map[idx];

  if ( entry )
  {
    entry();
    ev->accept();
  }
  else
  {
    const auto& entry_result = key_map_result[idx];

    if ( entry_result )
    {
      if ( entry_result() )
        ev->accept();
    }
    else
    {
      ev->ignore();
    }
  }
}

//----------------------------------------------------------------------
template <typename Compare>
void FListView::sort (Compare cmp)
{
  // Sort the top level
  std::sort(itemlist.begin(), itemlist.end(), cmp);

  // Sort the sublevels
  for (auto&& item : itemlist)
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
    return itemlist.end();

  if ( parent->isInstanceOf("FListViewItem") )
    return static_cast<FListViewItem*>(parent)->end();

  return getNullIterator();
}

//----------------------------------------------------------------------
void FListView::draw()
{
  if ( current_iter.getPosition() < 1 )
    current_iter = itemlist.begin();

  useParentWidgetColor();

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(true);

  drawBorder();

  if ( FVTerm::getFOutput()->isNewFont() && ! vbar->isShown() )
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
void FListView::drawHeadlines()
{
  if ( header.empty()
    || getHeight() <= 2
    || getWidth() <= 4
    || max_line_width < 1 )
    return;

  auto iter = header.cbegin();
  headerline.clear();

  if ( hasCheckableItems() )
    drawHeaderBorder(4);  // Draw into FVTermBuffer object

  while ( iter != header.cend() )
  {
    if ( ! iter->name.isEmpty() )
      drawHeadlineLabel(iter);  // Draw into FVTermBuffer object

    ++iter;
  }

  // Print the FVTermBuffer object
  drawBufferedHeadline();
}

//----------------------------------------------------------------------
void FListView::drawList()
{
  if ( isItemListEmpty() || getHeight() <= 2 || getWidth() <= 4 )
    return;

  int y{0};
  const auto page_height = int(getHeight()) - 2;
  const auto& itemlist_end = itemlist.end();
  auto path_end = itemlist_end;
  auto iter = first_visible_line;

  while ( iter != path_end && iter != itemlist_end && y < page_height )
  {
    const bool is_current_line( iter == current_iter );
    const auto& item = static_cast<FListViewItem*>(*iter);
    const int tree_offset = isTreeView() ? int(item->getDepth() << 1u) + 1 : 0;
    const int checkbox_offset = item->isCheckable() ? 1 : 0;
    path_end = getListEnd(item);
    print() << FPoint{2, 2 + y};

    // Draw one FListViewItem
    drawListLine (item, getFlags().focus, is_current_line);

    if ( getFlags().focus && is_current_line )
    {
      int xpos = 3 + tree_offset + checkbox_offset - xoffset;

      if ( xpos < 2 )  // Hide the cursor
        xpos = -9999;  // by moving it outside the visible area

      setVisibleCursor (item->isCheckable());
      setCursorPos ({xpos, 2 + y});  // first character
    }

    last_visible_line = iter;
    y++;
    ++iter;
  }

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
void FListView::drawListLine ( const FListViewItem* item
                             , bool is_focus
                             , bool is_current )
{
  // Set line color and attributes
  setLineAttributes (is_current, is_focus);

  // Print the entry
  const std::size_t indent = item->getDepth() << 1u;  // indent = 2 * depth
  FString line{getLinePrefix (item, indent)};

  // Print columns
  if ( ! item->column_list.empty() )
  {
    for (std::size_t col{0}; col < item->column_list.size(); )
    {
      static constexpr std::size_t ellipsis_length = 2;
      const auto& text = item->column_list[col];
      auto width = std::size_t(header[col].width);
      const std::size_t column_width = getColumnWidth(text);
      // Increment the value of col for the column position
      // and the next iteration
      col++;
      const auto align = getColumnAlignment(int(col));
      const std::size_t align_offset = getAlignOffset (align, column_width, width);

      if ( isTreeView() && col == 1 )
      {
        width -= (indent + 1);

        if ( item->isCheckable() )
          width -= checkbox_space;
      }

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
  }

  const std::size_t width = getWidth() - nf_offset - 2;
  line = getColumnSubString ( line, std::size_t(xoffset) + 1, width );
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

  const auto& wc = getColorTheme();
  setColor (wc->list_fg, wc->list_bg);
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
  const auto& wc = getColorTheme();
  setColor (wc->list_fg, wc->list_bg);

  if ( is_current )
  {
    if ( is_focus && FVTerm::getFOutput()->getMaxColor() < 16 )
      setBold();

    if ( FVTerm::getFOutput()->isMonochron() )
      unsetBold();

    if ( is_focus )
    {
      setColor ( wc->current_element_focus_fg
               , wc->current_element_focus_bg );
    }
    else
      setColor ( wc->current_element_fg
               , wc->current_element_bg );

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

  if ( sort_order == SortOrder::Ascending )
    headerline << UniChar::BlackUpPointingTriangle;    // ▲
  else if ( sort_order == SortOrder::Descending )
    headerline << UniChar::BlackDownPointingTriangle;  // ▼

  if ( length < column_max  )
  {
    length++;
    headerline << ' ';
  }
}

//----------------------------------------------------------------------
inline void FListView::drawHeaderBorder (std::size_t length)
{
  setColor();
  const FString line {length, UniChar::BoxDrawingsHorizontal};
  headerline << line;  // horizontal line
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
  const auto first = header.cbegin();
  const auto column = int(std::distance(first, iter)) + 1;
  const bool has_sort_indicator( sort_column == column && ! hide_sort_indicator );
  const auto& wc = getColorTheme();

  if ( isEnabled() )
    setColor (wc->label_emphasis_fg, wc->label_bg);
  else
    setColor (wc->label_inactive_fg, wc->label_inactive_bg);

  if ( has_sort_indicator && column_width >= column_max - 1 && column_width > 1 )
  {
    column_width = column_max - 2;
    txt = getColumnSubString (txt, 1, column_width);
  }

  if ( column_width <= column_max )
  {
    headerline << txt;

    if ( column_width < column_max )
    {
      column_width++;
      headerline << ' ';  // trailing space
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
void FListView::drawBufferedHeadline()
{
  // Print the FVTermBuffer object

  if ( headerline.isEmpty() )
    return;

  std::size_t column_offset{0};
  std::size_t column_width{0};
  std::size_t offset{0};
  bool left_truncated_fullwidth{false};
  bool right_truncated_fullwidth{false};
  auto last = headerline.end();

  // Search for the start position
  for (const auto& tc : headerline)
  {
    if ( xoffset == 0 )
      break;

    column_offset += getColumnWidth(tc);
    offset++;

    if ( column_offset == std::size_t(xoffset) )
      break;

    if ( column_offset > std::size_t(xoffset) && column_offset >= 2 )
    {
      left_truncated_fullwidth = true;
      break;
    }
  }

  auto first = headerline.begin();
  std::advance(first, offset);

  // Search for the end position
  if ( getColumnWidth(headerline) > getClientWidth() )
  {
    std::size_t character{0};

    if ( left_truncated_fullwidth )
      column_width++;

    for (const auto& tc : FVTermBuffer(first, last))
    {
      const uInt8 char_width = tc.attr.bit.char_width;

      if ( column_width + char_width > getClientWidth() )
      {
        column_width++;
        right_truncated_fullwidth = true;
        break;
      }

      column_width += char_width;
      character++;

      if ( column_width == getClientWidth() )
        break;
    }

    last = first;
    std::advance(last, character);
  }
  else
    column_width = getColumnWidth(headerline);

  // Print the header line
  print() << FPoint{2, 1};

  if ( left_truncated_fullwidth )
    print (UniChar::SingleLeftAngleQuotationMark);  // ‹

  print() << FVTermBuffer(first, last);

  if ( right_truncated_fullwidth )
    print (UniChar::SingleRightAngleQuotationMark);  // ›

  while ( column_width < getClientWidth() )
  {
    setColor();
    print(UniChar::BoxDrawingsHorizontal);
    column_width++;
  }
}

//----------------------------------------------------------------------
void FListView::drawColumnEllipsis ( const HeaderItems::const_iterator& iter
                                   , const FString& text )
{
  // Print label ellipsis
  static constexpr int ellipsis_length = 2;
  const int width = iter->width;
  const auto& wc = getColorTheme();

  headerline << ' '
             << getColumnSubString (text, 1, uInt(width - ellipsis_length))
             << FColorPair {wc->label_ellipsis_fg, wc->label_bg}
             << "..";

  if ( iter == header.cend() - 1 )  // Last element
    headerline << ' ';
}

//----------------------------------------------------------------------
void FListView::updateDrawing (bool draw_vbar, bool draw_hbar)
{
  if ( isShown() )
    draw();

  vbar->setValue (first_visible_line.getPosition());

  if ( draw_vbar )
    vbar->drawBar();

  hbar->setValue (xoffset);

  if ( draw_hbar )
    hbar->drawBar();

  forceTerminalUpdate();
}

//----------------------------------------------------------------------
auto FListView::determineLineWidth (FListViewItem* item) -> std::size_t
{
  static constexpr std::size_t padding_space = 1;
  std::size_t line_width = padding_space;  // leading space
  std::size_t column_idx{0};
  const auto entries = std::size_t(item->column_list.size());

  if ( hasCheckableItems() )
    line_width += checkbox_space;

  for (auto&& header_item : header)
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

    // width + trailing space
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
  if ( itemlist.size() == 1 )
  {
    // Select first item on insert
    current_iter = itemlist.begin();
    // The visible area of the list begins with the first element
    first_visible_line = itemlist.begin();
  }

  // Sort list by a column (only if activated)
  sort();

  const std::size_t element_count = getCount();
  recalculateVerticalBar (element_count);
  processChanged();
}

//----------------------------------------------------------------------
void FListView::recalculateHorizontalBar (std::size_t len)
{
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
void FListView::recalculateVerticalBar (std::size_t element_count) const
{
  const std::size_t height = getClientHeight();
  const int vmax = ( element_count > height )
                   ? int(element_count - height)
                   : 0;
  vbar->setMaximum (vmax);
  vbar->setPageSize (int(element_count), int(height));
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
void FListView::mouseHeaderClicked()
{
  int column{1};
  const int checkbox_offset = ( hasCheckableItems() ) ? checkbox_space : 0;
  const int header_pos = clicked_header_pos.getX() + xoffset;
  int header_start = 2 + checkbox_offset;

  for (const auto& item : header)
  {
    static constexpr int leading_space = 1;
    const bool has_sort_indicator( column == sort_column );
    auto click_width = int(getColumnWidth(item.name));

    if ( has_sort_indicator )
      click_width += 2;

    if ( click_width > item.width )
      click_width = item.width;

    if ( header_pos > header_start
      && header_pos <= header_start + click_width )
    {
      if ( has_sort_indicator && sort_order == SortOrder::Ascending )
        setColumnSort (column, SortOrder::Descending);
      else
        setColumnSort (column, SortOrder::Ascending);

      sort();

      if ( isShown() )
        updateDrawing (true, false);

      break;
    }

    header_start += leading_space + item.width;
    column++;
  }
}

//----------------------------------------------------------------------
void FListView::wheelUp (int pagesize)
{
  if ( isItemListEmpty() || current_iter.getPosition() == 0 )
    return;

  if ( first_visible_line.getPosition() >= pagesize )
  {
    current_iter -= pagesize;
    first_visible_line -= pagesize;
    last_visible_line -= pagesize;
  }
  else
  {
    // Save relative position from the first line
    const int ry = current_iter.getPosition() - first_visible_line.getPosition();
    // Save difference from top
    const int difference = first_visible_line.getPosition();
    first_visible_line -= difference;
    last_visible_line -= difference;
    setRelativePosition(ry);
  }
}

//----------------------------------------------------------------------
void FListView::wheelDown (int pagesize)
{
  if ( isItemListEmpty() )
    return;

  const auto element_count = int(getCount());

  if ( current_iter.getPosition() + 1 == element_count )
    return;

  if ( last_visible_line.getPosition() < element_count - pagesize )
  {
    current_iter += pagesize;
    first_visible_line += pagesize;
    last_visible_line += pagesize;
  }
  else
  {
    // Save relative position from the first line
    const int ry = current_iter.getPosition() - first_visible_line.getPosition();
    // Save difference from bottom
    const int differenz = element_count - last_visible_line.getPosition() - 1;
    first_visible_line += differenz;
    last_visible_line += differenz;
    setRelativePosition(ry);
  }
}

//----------------------------------------------------------------------
auto FListView::dragScrollUp (int position_before) -> bool
{
  if ( position_before == 0 )
  {
    drag_scroll = DragScrollMode::None;
    return false;
  }

  stepBackward(scroll_distance);
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

  stepForward(scroll_distance);
  return true;
}

//----------------------------------------------------------------------
void FListView::dragUp (MouseButton mouse_button)
{
  if ( isDragging(drag_scroll)
    && scroll_distance < int(getClientHeight()) )
    scroll_distance++;

  if ( ! scroll_timer && current_iter.getPosition() > 0 )
  {
    scroll_timer = true;
    addTimer(scroll_repeat);

    if ( mouse_button == MouseButton::Right )
      drag_scroll = DragScrollMode::SelectUpward;
    else
      drag_scroll = DragScrollMode::Upward;
  }

  if ( current_iter.getPosition() == 0 )
  {
    delOwnTimers();
    drag_scroll = DragScrollMode::None;
  }
}

//----------------------------------------------------------------------
void FListView::dragDown (MouseButton mouse_button)
{
  if ( isDragging(drag_scroll)
    && scroll_distance < int(getClientHeight()) )
    scroll_distance++;

  if ( ! scroll_timer && current_iter.getPosition() <= int(getCount()) )
  {
    scroll_timer = true;
    addTimer(scroll_repeat);

    if ( mouse_button == MouseButton::Right )
      drag_scroll = DragScrollMode::SelectDownward;
    else
      drag_scroll = DragScrollMode::Downward;
  }

  if ( current_iter.getPosition() - 1 == int(getCount()) )
  {
    delOwnTimers();
    drag_scroll = DragScrollMode::None;
  }
}

//----------------------------------------------------------------------
inline void FListView::stopDragScroll()
{
  delOwnTimers();
  scroll_timer = false;
  scroll_distance = 1;
  drag_scroll = DragScrollMode::None;
}

//----------------------------------------------------------------------
inline void FListView::toggleItemExpandState (FListViewItem* item)
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

  return mouse_x >= 3 + indent - xoffset
      && mouse_x <= 5 + indent - xoffset;
}

//----------------------------------------------------------------------
inline void FListView::resetClickedPositions()
{
  clicked_expander_pos.setPoint(-1, -1);
  clicked_header_pos.setPoint(-1, -1);
  clicked_checkbox_item = nullptr;
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
  item->root = root;
  addChild (item);
  itemlist.push_back (item);
  return --itemlist.end();
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
inline void FListView::toggleCheckbox()
{
  if ( isItemListEmpty() )
    return;

  auto item = getCurrentItem();

  if ( item->isCheckable() )
    toggleItemCheckState(item);
}

//----------------------------------------------------------------------
inline void FListView::collapseAndScrollLeft()
{
  const int position_before = current_iter.getPosition();
  auto item = getCurrentItem();

  if ( xoffset == 0 && item && ! isItemListEmpty() )
  {
    if ( isTreeView() && item->isExpandable() && item->isExpand() )
    {
      // Collapse element
      item->collapse();
      adjustSize();
      vbar->calculateSliderValues();
      // Force vertical scrollbar redraw
      first_line_position_before = -1;
    }
    else if ( item->hasParent() )
    {
      // Jump to parent element
      const auto& parent = item->getParent();

      if ( parent->isInstanceOf("FListViewItem") )
      {
        current_iter.parentElement();

        if ( current_iter.getPosition() < first_line_position_before )
        {
          const int difference = position_before - current_iter.getPosition();

          if ( first_visible_line.getPosition() - difference >= 0 )
          {
            first_visible_line -= difference;
            last_visible_line -= difference;
          }
          else
          {
            const int d = first_visible_line.getPosition();
            first_visible_line -= d;
            last_visible_line -= d;
          }
        }
      }
    }
  }
  else
  {
    // Scroll left
    if ( xoffset > 0 )
      xoffset--;
  }
}

//----------------------------------------------------------------------
inline void FListView::expandAndScrollRight()
{
  const int xoffset_end = int(max_line_width) - int(getClientWidth());
  auto item = getCurrentItem();

  if ( isTreeView() && ! isItemListEmpty() && item
    && item->isExpandable() && ! item->isExpand() )
  {
    // Expand element
    item->expand();
    adjustScrollbars (getCount());
    // Force vertical scrollbar redraw
    first_line_position_before = -1;
  }
  else
  {
    // Scroll right
    if ( xoffset < xoffset_end )
      xoffset++;

    if ( xoffset < 0 )
      xoffset = 0;
  }
}

//----------------------------------------------------------------------
inline void FListView::firstPos()
{
  if ( isItemListEmpty() )
    return;

  current_iter -= current_iter.getPosition();
  const int difference = first_visible_line.getPosition();
  first_visible_line -= difference;
  last_visible_line -= difference;
}

//----------------------------------------------------------------------
inline void FListView::lastPos()
{
  if ( isItemListEmpty() )
    return;

  const auto element_count = int(getCount());
  current_iter += element_count - current_iter.getPosition() - 1;
  const int difference = element_count - last_visible_line.getPosition() - 1;
  first_visible_line += difference;
  last_visible_line += difference;
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
  current_iter = first_visible_line;
  current_iter += ry;
}

//----------------------------------------------------------------------
void FListView::stepForward()
{
  if ( isItemListEmpty() )
    return;

  if ( current_iter == last_visible_line )
  {
    ++last_visible_line;

    if ( last_visible_line == itemlist.end() )
      --last_visible_line;
    else
      ++first_visible_line;
  }

  ++current_iter;

  if ( current_iter == itemlist.end() )
    --current_iter;
}

//----------------------------------------------------------------------
void FListView::stepBackward()
{
  if ( isItemListEmpty() )
    return;

  if ( current_iter == first_visible_line
    && current_iter != itemlist.begin() )
  {
    --first_visible_line;
    --last_visible_line;
  }

  if ( current_iter != itemlist.begin() )
    --current_iter;
}

//----------------------------------------------------------------------
void FListView::stepForward (int distance)
{
  if ( isItemListEmpty() )
    return;

  const auto element_count = int(getCount());

  if ( current_iter.getPosition() + 1 == element_count )
    return;

  if ( current_iter.getPosition() + distance < element_count )
  {
    current_iter += distance;
  }
  else
  {
    current_iter += element_count - current_iter.getPosition() - 1;
  }

  if ( current_iter.getPosition() > last_visible_line.getPosition() )
  {
    if ( last_visible_line.getPosition() + distance < element_count )
    {
      first_visible_line += distance;
      last_visible_line += distance;
    }
    else
    {
      const int differenz = element_count - last_visible_line.getPosition() - 1;
      first_visible_line += differenz;
      last_visible_line += differenz;
    }
  }
}

//----------------------------------------------------------------------
void FListView::stepBackward (int distance)
{
  if ( isItemListEmpty() || current_iter.getPosition() == 0 )
    return;

  if ( current_iter.getPosition() - distance >= 0 )
  {
    current_iter -= distance;
  }
  else
  {
    current_iter -= current_iter.getPosition();
  }

  if ( current_iter.getPosition() < first_visible_line.getPosition() )
  {
    if ( first_visible_line.getPosition() - distance >= 0 )
    {
      first_visible_line -= distance;
      last_visible_line -= distance;
    }
    else
    {
      const int difference = first_visible_line.getPosition();
      first_visible_line -= difference;
      last_visible_line -= difference;
    }
  }
}

//----------------------------------------------------------------------
void FListView::scrollToX (int x)
{
  if ( xoffset == x )
    return;

  xoffset = x;
  const int xoffset_end = int(max_line_width) - int(getClientWidth());

  if ( xoffset > xoffset_end )
    xoffset = xoffset_end;

  if ( xoffset < 0 )
    xoffset = 0;
}

//----------------------------------------------------------------------
void FListView::scrollToY (int y)
{
  const int pagesize = int(getClientHeight()) - 1;
  const auto element_count = int(getCount());

  if ( first_visible_line.getPosition() == y )
    return;

  // Save relative position from the top line
  const int ry = current_iter.getPosition() - first_visible_line.getPosition();

  if ( y + pagesize <= element_count )
  {
    first_visible_line = itemlist.begin();
    first_visible_line += y;
    setRelativePosition (ry);
    last_visible_line = first_visible_line;
    last_visible_line += pagesize;
  }
  else
  {
    const int differenz = element_count - last_visible_line.getPosition() - 1;
    current_iter += differenz;
    first_visible_line += differenz;
    last_visible_line += differenz;
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
  scrollToX(xoffset + dx);

  if ( dy > 0 )
    stepForward(dy);

  if ( dy < 0 )
    stepBackward(-dy);
}

//----------------------------------------------------------------------
void FListView::cb_vbarChange (const FWidget*)
{
  const FScrollbar::ScrollType scroll_type = vbar->getScrollType();
  static constexpr int wheel_distance = 4;
  int distance{1};
  first_line_position_before = first_visible_line.getPosition();

  switch ( scroll_type )
  {
    case FScrollbar::ScrollType::PageBackward:
      distance = int(getClientHeight());
      // fall through
    case FScrollbar::ScrollType::StepBackward:
      stepBackward(distance);
      break;

    case FScrollbar::ScrollType::PageForward:
      distance = int(getClientHeight());
      // fall through
    case FScrollbar::ScrollType::StepForward:
      stepForward(distance);
      break;

    case FScrollbar::ScrollType::Jump:
      scrollToY (vbar->getValue());
      break;

    case FScrollbar::ScrollType::WheelUp:
      wheelUp (wheel_distance);
      break;

    case FScrollbar::ScrollType::WheelDown:
      wheelDown (wheel_distance);
      break;

    default:
      break;
  }

  if ( isShown() )
    drawList();

  if ( scroll_type >= FScrollbar::ScrollType::StepBackward
    && scroll_type <= FScrollbar::ScrollType::PageForward )
  {
    vbar->setValue (first_visible_line.getPosition());

    if ( first_line_position_before != first_visible_line.getPosition() )
      vbar->drawBar();

    forceTerminalUpdate();
  }
}

//----------------------------------------------------------------------
void FListView::cb_hbarChange (const FWidget*)
{
  const FScrollbar::ScrollType scroll_type = hbar->getScrollType();
  static constexpr int wheel_distance = 4;
  int distance{1};
  const int xoffset_before = xoffset;

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

  if ( isShown() )
  {
    drawHeadlines();
    drawList();
  }

  if ( scroll_type >= FScrollbar::ScrollType::StepBackward )
  {
    hbar->setValue (xoffset);

    if ( xoffset_before != xoffset )
      hbar->drawBar();

    forceTerminalUpdate();
  }
}

}  // namespace finalcut
