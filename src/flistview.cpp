/***********************************************************************
* flistview.cpp - Widget FListView and FListViewItem                   *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2017-2020 Markus Gans                                      *
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

#if defined(__CYGWIN__)
  #include <strings.h>  // need for strcasecmp
#endif

#include <limits>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "final/emptyfstring.h"
#include "final/fapplication.h"
#include "final/fcolorpair.h"
#include "final/fevent.h"
#include "final/flistview.h"
#include "final/fstatusbar.h"
#include "final/fstring.h"
#include "final/ftermbuffer.h"
#include "final/fwidgetcolors.h"

namespace finalcut
{

// Function prototypes
uInt64 firstNumberFromString (const FString&);
bool sortAscendingByName (const FObject*, const FObject*);
bool sortDescendingByName (const FObject*, const FObject*);
bool sortAscendingByNumber (const FObject*, const FObject*);
bool sortDescendingByNumber (const FObject*, const FObject*);

// non-member functions
//----------------------------------------------------------------------
uInt64 firstNumberFromString (const FString& str)
{
  auto iter = str.begin();

  while ( iter != str.end() )
  {
    if ( wchar_t(*iter) >= L'0' && wchar_t(*iter) <= L'9' )
    {
      if ( iter != str.begin() && wchar_t(*(iter - 1)) == L'-' )
        --iter;

      break;
    }

    ++iter;
  }

  auto first_pos = iter;

  if ( first_pos == str.end() )
    return 0;

  while ( iter != str.end() )
  {
    if ( wchar_t(*iter) < L'0' || wchar_t(*iter) > L'9' )
      break;

    ++iter;
  }

  auto last_pos = iter;

  if ( last_pos == str.end() )
    return 0;

  uInt64 number;
  const auto pos = std::size_t(std::distance(str.begin(), first_pos)) + 1;
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
bool sortAscendingByName (const FObject* lhs, const FObject* rhs)
{
  const auto& l_item = static_cast<const FListViewItem*>(lhs);
  const auto& r_item = static_cast<const FListViewItem*>(rhs);
  const int column = l_item->getSortColumn();
  const auto& l_string = l_item->getText(column);
  const auto& r_string = r_item->getText(column);

  // lhs < rhs
  return bool( strcasecmp(l_string.c_str(), r_string.c_str()) < 0 );
}

//----------------------------------------------------------------------
bool sortDescendingByName (const FObject* lhs, const FObject* rhs)
{
  const auto& l_item = static_cast<const FListViewItem*>(lhs);
  const auto& r_item = static_cast<const FListViewItem*>(rhs);
  const int column = l_item->getSortColumn();
  const auto& l_string = l_item->getText(column);
  const auto& r_string = r_item->getText(column);

  // lhs > rhs
  return bool( strcasecmp(l_string.c_str(), r_string.c_str()) > 0 );
}

//----------------------------------------------------------------------
bool sortAscendingByNumber (const FObject* lhs, const FObject* rhs)
{
  const auto& l_item = static_cast<const FListViewItem*>(lhs);
  const auto& r_item = static_cast<const FListViewItem*>(rhs);
  const int column = l_item->getSortColumn();
  const auto& l_number = firstNumberFromString(l_item->getText(column));
  const auto& r_number = firstNumberFromString(r_item->getText(column));

  // lhs < rhs
  return bool( l_number < r_number );
}

//----------------------------------------------------------------------
bool sortDescendingByNumber (const FObject* lhs, const FObject* rhs)
{
  const auto& l_item = static_cast<const FListViewItem*>(lhs);
  const auto& r_item = static_cast<const FListViewItem*>(rhs);
  const int column = l_item->getSortColumn();
  const auto& l_number = firstNumberFromString(l_item->getText(column));
  const auto& r_number = firstNumberFromString(r_item->getText(column));

  // lhs > rhs
  return bool( l_number > r_number );
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

  if ( parent->isInstanceOf("FListView") )
  {
    static_cast<FListView*>(parent)->remove (this);
  }
  else if ( parent->isInstanceOf("FListViewItem") )
  {
    static_cast<FListViewItem*>(parent)->remove (this);
  }
}


// public methods of FListViewItem
//----------------------------------------------------------------------
int FListViewItem::getSortColumn() const
{
  if ( ! *root )
    return -1;

  const auto& root_obj = static_cast<FListView*>(*root);
  return root_obj->getSortColumn();
}

//----------------------------------------------------------------------
FString FListViewItem::getText (int column) const
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
uInt FListViewItem::getDepth() const
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
FObject::iterator FListViewItem::insert (FListViewItem* child)
{
  // Add a FListViewItem as child element
  if ( ! child )
    return FListView::getNullIterator();

  return appendItem(child);
}

//----------------------------------------------------------------------
FObject::iterator FListViewItem::insert ( FListViewItem* child
                                        , iterator parent_iter ) const
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
    else if ( (*parent_iter)->isInstanceOf("FListViewItem") )
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
  FObject::FObjectList& children = getChildren();

  if ( ! children.empty() )
    children.sort(cmp);

  // Sort the sublevels
  for (auto&& item : children)
    static_cast<FListViewItem*>(item)->sort(cmp);
}

//----------------------------------------------------------------------
FObject::iterator FListViewItem::appendItem (FListViewItem* child)
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
std::size_t FListViewItem::getVisibleLines()
{
  if ( visible_lines > 1 )
    return visible_lines;

  visible_lines = 1;

  if ( ! isExpand() || ! hasChildren() )
    return visible_lines;

  auto iter = FObject::begin();

  while ( iter != FObject::end() )
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
FListViewIterator::FListViewIterator (iterator iter)
  : node{iter}
{ }

// FListViewIterator operators
//----------------------------------------------------------------------
FListViewIterator& FListViewIterator::operator ++ ()  // prefix
{
  nextElement(node);
  return *this;
}

//----------------------------------------------------------------------
FListViewIterator FListViewIterator::operator ++ (int)  // postfix
{
  FListViewIterator tmp = *this;
  ++(*this);
  return tmp;
}

//----------------------------------------------------------------------
FListViewIterator& FListViewIterator::operator -- ()  // prefix
{
  prevElement(node);
  return *this;
}

//----------------------------------------------------------------------
FListViewIterator FListViewIterator::operator -- (int)  // postfix
{
  FListViewIterator tmp = *this;
  --(*this);
  return tmp;
}

//----------------------------------------------------------------------
FListViewIterator& FListViewIterator::operator += (int n)
{
  for (int i = n; i > 0 ; i--)
    nextElement(node);

  return *this;
}

//----------------------------------------------------------------------
FListViewIterator& FListViewIterator::operator -= (int n)
{
  for (int i = n; i > 0 ; i--)
    prevElement(node);

  return *this;
}

// private methods of FListViewIterator
//----------------------------------------------------------------------
void FListViewIterator::nextElement (iterator& iter)
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

        if ( iter == (*parent_iter)->end() )
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
void FListViewIterator::prevElement (iterator& iter)
{
  auto start_iter = iter;

  if ( ! iter_path.empty() )
  {
    const auto& parent_iter = iter_path.top();

    if ( start_iter == (*parent_iter)->begin() )  // First sub-item
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
  else
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
std::size_t FListView::getCount() const
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
fc::text_alignment FListView::getColumnAlignment (int column) const
{
  // Get the alignment for a column

  if ( column < 1 || header.empty() || column > int(header.size()) )
    return fc::alignLeft;

  // Convert column position to address offset (index)
  const auto index = std::size_t(column - 1);
  return header[index].alignment;
}

//----------------------------------------------------------------------
FString FListView::getColumnText (int column) const
{
  // Get the text of column

  if ( column < 1 || header.empty() || column > int(header.size()) )
    return fc::emptyFString::get();

  // Convert column position to address offset (index)
  const auto index = std::size_t(column - 1);
  return header[index].name;
}

//----------------------------------------------------------------------
fc::sorting_type FListView::getColumnSortType (int column) const
{
  fc::sorting_type type;
  const auto col = std::size_t(column);

  try
  {
    type = sort_type.at(col);
  }
  catch (const std::out_of_range&)
  {
    type = fc::unknown;
  }

  return type;
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
void FListView::setColumnAlignment (int column, fc::text_alignment align)
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
void FListView::setColumnSortType (int column, fc::sorting_type type)
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
void FListView::setColumnSort (int column, fc::sorting_order order)
{
  // Sets the column to sort by + the sorting order

  if ( column < 1 || header.empty() || column > int(header.size()) )
    column = -1;

  sort_column = column;
  sort_order = order;
}

//----------------------------------------------------------------------
int FListView::addColumn (const FString& label, int width)
{
  Header new_column{};
  new_column.name = label;
  new_column.width = width;

  if ( new_column.width == USE_MAX_SIZE )
    new_column.width = int(getColumnWidth(label));
  else
    new_column.fixed_width = true;

  header.push_back (new_column);
  return int(std::distance(header.begin(), header.end()));
}

//----------------------------------------------------------------------
void FListView::hide()
{
  FWidget::hide();
  hideArea (getSize());
}

//----------------------------------------------------------------------
FObject::iterator FListView::insert ( FListViewItem* item
                                    , iterator parent_iter )
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
  if ( item == nullptr || itemlist.empty() )
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
      itemlist.remove(item);
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

  if ( itemlist.empty() )
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
}

//----------------------------------------------------------------------
void FListView::sort()
{
  // Sorts the list view according to the specified setting

  if ( sort_column < 1 && sort_column > int(header.size()) )
    return;

  fc::sorting_type column_sort_type = getColumnSortType(sort_column);
  assert ( column_sort_type == fc::by_name
        || column_sort_type == fc::by_number
        || column_sort_type == fc::user_defined
        || column_sort_type == fc::unknown );

  switch ( column_sort_type )
  {
    case fc::unknown:
    case fc::by_name:
      if ( sort_order == fc::ascending )
      {
        sort (sortAscendingByName);
      }
      else if ( sort_order == fc::descending )
      {
        sort (sortDescendingByName);
      }
      break;

    case fc::by_number:
      if ( sort_order == fc::ascending )
      {
        sort (sortAscendingByNumber);
      }
      else if ( sort_order == fc::descending )
      {
        sort (sortDescendingByNumber);
      }
      break;

    case fc::user_defined:
      if ( sort_order == fc::ascending && user_defined_ascending )
      {
        sort (user_defined_ascending);
      }
      else if ( sort_order == fc::descending && user_defined_descending )
      {
        sort (user_defined_descending);
      }
      break;
  }

  current_iter = itemlist.begin();
  first_visible_line = itemlist.begin();
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
    processChanged();

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
  if ( ev->getButton() != fc::LeftButton )
  {
    clicked_expander_pos.setPoint(-1, -1);
    return;
  }

  if ( ! hasFocus() )
  {
    auto focused_widget = getFocusWidget();
    setFocus();

    if ( focused_widget )
      focused_widget->redraw();

    if ( getStatusBar() )
      getStatusBar()->drawMessage();
  }

  const int mouse_x = ev->getX();
  const int mouse_y = ev->getY();
  first_line_position_before = first_visible_line.getPosition();

  if ( mouse_x > 1 && mouse_x < int(getWidth()) )
  {
    if ( mouse_y == 1 )  // Header
    {
      clicked_header_pos = ev->getPos();
    }
    else if ( mouse_y > 1 && mouse_y < int(getHeight()) )  // List
    {
      if ( itemlist.empty() )
        return;

      int indent = 0;
      const int new_pos = first_visible_line.getPosition() + mouse_y - 2;

      if ( new_pos < int(getCount()) )
        setRelativePosition (mouse_y - 2);

      const auto& item = getCurrentItem();

      if ( tree_view )
      {
        indent = int(item->getDepth() << 1);  // indent = 2 * depth

        if ( item->isExpandable() && mouse_x - 2 == indent - xoffset )
          clicked_expander_pos = ev->getPos();
      }

      if ( hasCheckableItems() )
      {
        if ( tree_view )
          indent++;  // Plus one space

        if ( mouse_x >= 3 + indent - xoffset
          && mouse_x <= 5 + indent - xoffset
          && item->isCheckable() )
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
}

//----------------------------------------------------------------------
void FListView::onMouseUp (FMouseEvent* ev)
{
  if ( drag_scroll != fc::noScroll )
    stopDragScroll();

  if ( ev->getButton() == fc::LeftButton )
  {
    const int mouse_x = ev->getX();
    const int mouse_y = ev->getY();

    if ( mouse_x > 1 && mouse_x < int(getWidth()) )
    {
      if ( mouse_y == 1 && clicked_header_pos == ev->getPos() )  // Header
      {
        mouseHeaderClicked();
      }
      else if ( mouse_y > 1 && mouse_y < int(getHeight()) )  // List
      {
        if ( itemlist.empty() )
          return;

        int indent{0};
        const auto& item = getCurrentItem();

        if ( tree_view )
        {
          indent = int(item->getDepth() << 1);  // indent = 2 * depth

          if ( item->isExpandable()
            && clicked_expander_pos == ev->getPos() )
          {
            if ( item->isExpand() )
              item->collapse();
            else
              item->expand();

            adjustScrollbars (getCount());

            if ( isShown() )
              draw();
          }
        }

        if ( hasCheckableItems() )
        {
          if ( tree_view )
            indent++;  // Plus one space

          if ( mouse_x >= 3 + indent - xoffset
            && mouse_x <= 5 + indent - xoffset
            && clicked_checkbox_item == item )
          {
            item->setChecked(! item->isChecked());

            if ( isShown() )
              draw();
          }
        }

        processChanged();
      }
    }
  }

  clicked_expander_pos.setPoint(-1, -1);
  clicked_header_pos.setPoint(-1, -1);
  clicked_checkbox_item = nullptr;
}

//----------------------------------------------------------------------
void FListView::onMouseMove (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton )
  {
    clicked_expander_pos.setPoint(-1, -1);
    return;
  }

  const int mouse_x = ev->getX();
  const int mouse_y = ev->getY();
  first_line_position_before = first_visible_line.getPosition();

  if ( mouse_x > 1 && mouse_x < int(getWidth())
    && mouse_y > 1 && mouse_y < int(getHeight()) )
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
  if ( ev->getButton() != fc::LeftButton )
    return;

  const int mouse_x = ev->getX();
  const int mouse_y = ev->getY();

  if ( mouse_x > 1 && mouse_x < int(getWidth())
    && mouse_y > 1 && mouse_y < int(getHeight()) )
  {
    if ( first_visible_line.getPosition() + mouse_y - 1 > int(getCount()) )
      return;

    if ( itemlist.empty() )
      return;

    auto item = getCurrentItem();

    if ( tree_view && item->isExpandable() )
    {
      if ( item->isExpand() )
        item->collapse();
      else
        item->expand();

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

  switch ( int(drag_scroll) )
  {
    case fc::noScroll:
      return;

    case fc::scrollUp:
    case fc::scrollUpSelect:
      if ( ! dragScrollUp(position_before) )
        return;
      break;

    case fc::scrollDown:
    case fc::scrollDownSelect:
      if ( ! dragScrollDown(position_before) )
        return;
      break;

    default:
      break;
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

  if ( drag_scroll != fc::noScroll )
    stopDragScroll();

  switch ( ev->getWheel() )
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

  if ( position_before != current_iter.getPosition() )
    processChanged();

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
FObject::iterator& FListView::getNullIterator()
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
  initScrollbar (vbar, fc::vertical, this, &FListView::cb_vbarChange);
  initScrollbar (hbar, fc::horizontal, this, &FListView::cb_hbarChange);
  selflist.push_back(this);
  root = selflist.begin();
  getNullIterator() = selflist.end();
  setGeometry (FPoint{1, 1}, FSize{5, 4}, false);  // initialize geometry values
  nf_offset = FTerm::isNewFont() ? 1 : 0;
  setTopPadding(1);
  setLeftPadding(1);
  setBottomPadding(1);
  setRightPadding(1);
  mapKeyFunctions();
}

//----------------------------------------------------------------------
inline void FListView::mapKeyFunctions()
{
  key_map[fc::Fkey_return]  = std::bind(&FListView::processClick, this);
  key_map[fc::Fkey_enter]   = std::bind(&FListView::processClick, this);
  key_map[fc::Fkey_space]   = std::bind(&FListView::toggleCheckbox, this);
  key_map[fc::Fkey_up]      = [this] { stepBackward(); };
  key_map[fc::Fkey_down]    = [this] { stepForward(); };
  key_map[fc::Fkey_left]    = std::bind(&FListView::collapseAndScrollLeft, this);
  key_map[fc::Fkey_right]   = std::bind(&FListView::expandAndScrollRight, this);
  key_map[fc::Fkey_ppage]   = [this] { stepBackward(int(getClientHeight()) - 1); };
  key_map[fc::Fkey_npage]   = [this] { stepForward(int(getClientHeight()) - 1); };
  key_map[fc::Fkey_home]    = std::bind(&FListView::firstPos, this);
  key_map[fc::Fkey_end]     = std::bind(&FListView::lastPos, this);
  key_map_result[FKey('+')] = std::bind(&FListView::expandSubtree, this);
  key_map_result[FKey('-')] = std::bind(&FListView::collapseSubtree, this);
}

//----------------------------------------------------------------------
void FListView::processKeyAction (FKeyEvent* ev)
{
  const auto idx = int(ev->key());

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
  else
  {
    ev->ignore();
  }
}

//----------------------------------------------------------------------
template <typename Compare>
void FListView::sort (Compare cmp)
{
  // Sort the top level
  itemlist.sort(cmp);

  // Sort the sublevels
  for (auto&& item : itemlist)
    static_cast<FListViewItem*>(item)->sort(cmp);
}

//----------------------------------------------------------------------
std::size_t FListView::getAlignOffset ( const fc::text_alignment align
                                      , const std::size_t column_width
                                      , const std::size_t width ) const
{
  assert ( align == fc::alignLeft
        || align == fc::alignCenter
        || align == fc::alignRight );

  switch ( align )
  {
    case fc::alignLeft:
      return 0;

    case fc::alignCenter:
      if ( column_width < width )
        return (width - column_width) / 2;
      else
        return 0;

    case fc::alignRight:
      if ( column_width < width )
        return width - column_width;
      else
        return 0;
  }

  return 0;
}

//----------------------------------------------------------------------
FObject::iterator FListView::getListEnd (const FListViewItem* item)
{
  auto parent = item->getParent();

  if ( ! parent )
    return getNullIterator();

  if ( this == parent )
    return itemlist.end();
  else if ( parent->isInstanceOf("FListViewItem") )
    return static_cast<FListViewItem*>(parent)->end();
  else
    return getNullIterator();
}

//----------------------------------------------------------------------
void FListView::draw()
{
  if ( current_iter.getPosition() < 1 )
    current_iter = itemlist.begin();

  useParentWidgetColor();

  if ( FTerm::isMonochron() )
    setReverse(true);

  drawBorder();

  if ( FTerm::isNewFont() && ! vbar->isShown() )
  {
    setColor();

    for (auto y{2}; y < int(getHeight()); y++)
    {
      print() << FPoint{int(getWidth()) - 1, y}
              << ' ';  // clear right side of the scrollbar
    }
  }

  drawHeadlines();

  if ( FTerm::isMonochron() )
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

  HeaderItems::const_iterator iter = header.begin();
  headerline.clear();

  if ( hasCheckableItems() )
    drawHeaderBorder(4);  // Draw into FTermBuffer object

  while ( iter != header.end() )
  {
    const auto& text = iter->name;

    if ( text.isNull() || text.isEmpty() )
    {
      ++iter;
      continue;
    }

    drawHeadlineLabel(iter);  // Draw into FTermBuffer object
    ++iter;
  }

  // Print the FTermBuffer object
  drawBufferedHeadline();
}

//----------------------------------------------------------------------
void FListView::drawList()
{
  if ( itemlist.empty() || getHeight() <= 2 || getWidth() <= 4 )
    return;

  uInt y{0};
  const uInt page_height = uInt(getHeight()) - 2;
  const auto& itemlist_end = itemlist.end();
  auto path_end = itemlist_end;
  auto iter = first_visible_line;

  while ( iter != path_end && iter != itemlist_end && y < page_height )
  {
    const bool is_current_line( iter == current_iter );
    const auto& item = static_cast<FListViewItem*>(*iter);
    const int tree_offset = ( tree_view ) ? int(item->getDepth() << 1) + 1 : 0;
    const int checkbox_offset = ( item->isCheckable() ) ? 1 : 0;
    path_end = getListEnd(item);
    print() << FPoint{2, 2 + int(y)};

    // Draw one FListViewItem
    drawListLine (item, getFlags().focus, is_current_line);

    if ( getFlags().focus && is_current_line )
    {
      int xpos = 3 + tree_offset + checkbox_offset - xoffset;

      if ( xpos < 2 )  // Hide the cursor
        xpos = -9999;  // by moving it outside the visible area

      setVisibleCursor (item->isCheckable());
      setCursorPos ({xpos, 2 + int(y)});  // first character
    }

    last_visible_line = iter;
    y++;
    ++iter;
  }

  // Reset color
  setColor();

  if ( FTerm::isMonochron() )
    setReverse(true);

  // Clean empty space after last element
  while ( y < uInt(getClientHeight()) )
  {
    print() << FPoint{2, 2 + int(y)}
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
  const std::size_t indent = item->getDepth() << 1;  // indent = 2 * depth
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
      const fc::text_alignment align = getColumnAlignment(int(col));
      const std::size_t align_offset = getAlignOffset (align, column_width, width);

      if ( tree_view && col == 1 )
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
      else if ( align == fc::alignRight )
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
    if ( is_focus && FTerm::getMaxColor() < 16 )
      setBold();

    if ( FTerm::isMonochron() )
      unsetBold();

    if ( is_focus )
    {
      setColor ( wc->current_element_focus_fg
               , wc->current_element_focus_bg );
    }
    else
      setColor ( wc->current_element_fg
               , wc->current_element_bg );

    if ( FTerm::isMonochron() )
      setReverse(false);
  }
  else
  {
    if ( FTerm::isMonochron() )
      setReverse(true);
    else if ( is_focus && FTerm::getMaxColor() < 16 )
      unsetBold();
  }
}

//----------------------------------------------------------------------
inline FString FListView::getCheckBox (const FListViewItem* item) const
{
  FString checkbox{""};

  if ( FTerm::isNewFont() )
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
        checkbox[1] = fc::Times;  // Times ×
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
inline FString FListView::getLinePrefix ( const FListViewItem* item
                                        , std::size_t indent ) const
{
  FString line{""};

  if ( tree_view )
  {
    if ( indent > 0 )
      line = FString{indent, L' '};

    if ( item->isExpandable()  )
    {
      if ( item->isExpand() )
      {
        line += fc::BlackDownPointingTriangle;  // ▼
        line += L' ';
      }
      else
      {
        line += fc::BlackRightPointingPointer;  // ►
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

  if ( sort_order == fc::ascending )
    headerline << fc::BlackUpPointingTriangle;    // ▲
  else if ( sort_order == fc::descending )
    headerline << fc::BlackDownPointingTriangle;  // ▼

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
  const FString line {length, fc::BoxDrawingsHorizontal};
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
  const HeaderItems::const_iterator first = header.begin();
  const int column = int(std::distance(first, iter)) + 1;
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
  // Print the FTermBuffer object

  if ( headerline.isEmpty() )
    return;

  std::size_t column_offset{0};
  std::size_t column_width{0};
  std::size_t offset{0};
  bool left_truncated_fullwidth{false};
  bool right_truncated_fullwidth{false};
  std::vector<FChar>::const_iterator first{};
  std::vector<FChar>::const_iterator last{};
  last = headerline.end();

  // Search for the start position
  for (auto&& tc : headerline)
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

  first = headerline.begin();
  std::advance(first, offset);

  // Search for the end position
  if ( getColumnWidth(headerline) > getClientWidth() )
  {
    std::size_t character{0};

    if ( left_truncated_fullwidth )
      column_width++;

    for (auto&& tc : FTermBuffer(first, last))
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
    print (fc::SingleLeftAngleQuotationMark);  // ‹

  print() << FTermBuffer(first, last);

  if ( right_truncated_fullwidth )
    print (fc::SingleRightAngleQuotationMark);  // ›

  while ( column_width < getClientWidth() )
  {
    setColor();
    print(fc::BoxDrawingsHorizontal);
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

  if ( iter == header.end() - 1 )  // Last element
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
std::size_t FListView::determineLineWidth (FListViewItem* item)
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

  for (auto&& item : header)
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
      if ( has_sort_indicator && sort_order == fc::ascending )
        setColumnSort (column, fc::descending);
      else
        setColumnSort (column, fc::ascending);

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
  if ( itemlist.empty() || current_iter.getPosition() == 0 )
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
  if ( itemlist.empty() )
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
bool FListView::dragScrollUp (int position_before)
{
  if ( position_before == 0 )
  {
    drag_scroll = fc::noScroll;
    return false;
  }

  stepBackward(scroll_distance);
  return true;
}

//----------------------------------------------------------------------
bool FListView::dragScrollDown (int position_before)
{
  const auto element_count = int(getCount());

  if ( position_before + 1 == element_count )
  {
    drag_scroll = fc::noScroll;
    return false;
  }

  stepForward(scroll_distance);
  return true;
}

//----------------------------------------------------------------------
void FListView::dragUp (int mouse_button)
{
  if ( drag_scroll != fc::noScroll
    && scroll_distance < int(getClientHeight()) )
    scroll_distance++;

  if ( ! scroll_timer && current_iter.getPosition() > 0 )
  {
    scroll_timer = true;
    addTimer(scroll_repeat);

    if ( mouse_button == fc::RightButton )
      drag_scroll = fc::scrollUpSelect;
    else
      drag_scroll = fc::scrollUp;
  }

  if ( current_iter.getPosition() == 0 )
  {
    delOwnTimers();
    drag_scroll = fc::noScroll;
  }
}

//----------------------------------------------------------------------
void FListView::dragDown (int mouse_button)
{
  if ( drag_scroll != fc::noScroll
    && scroll_distance < int(getClientHeight()) )
    scroll_distance++;

  if ( ! scroll_timer && current_iter.getPosition() <= int(getCount()) )
  {
    scroll_timer = true;
    addTimer(scroll_repeat);

    if ( mouse_button == fc::RightButton )
      drag_scroll = fc::scrollDownSelect;
    else
      drag_scroll = fc::scrollDown;
  }

  if ( current_iter.getPosition() - 1 == int(getCount()) )
  {
    delOwnTimers();
    drag_scroll = fc::noScroll;
  }
}

//----------------------------------------------------------------------
void FListView::stopDragScroll()
{
  delOwnTimers();
  scroll_timer = false;
  scroll_distance = 1;
  drag_scroll = fc::noScroll;
}

//----------------------------------------------------------------------
FObject::iterator FListView::appendItem (FListViewItem* item)
{
  item->root = root;
  addChild (item);
  itemlist.push_back (item);
  return --itemlist.end();
}

//----------------------------------------------------------------------
void FListView::processClick() const
{
  if ( itemlist.empty() )
    return;

  emitCallback("clicked");
}

//----------------------------------------------------------------------
void FListView::processChanged() const
{
  emitCallback("row-changed");
}

//----------------------------------------------------------------------
void FListView::changeOnResize() const
{
  if ( FTerm::isNewFont() )
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
  if ( itemlist.empty() )
    return;

  auto item = getCurrentItem();

  if ( item->isCheckable() )
    item->setChecked(! item->isChecked());
}

//----------------------------------------------------------------------
inline void FListView::collapseAndScrollLeft()
{
  const int position_before = current_iter.getPosition();
  auto item = getCurrentItem();

  if ( xoffset == 0 && item && ! itemlist.empty() )
  {
    if ( tree_view && item->isExpandable() && item->isExpand() )
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

  if ( tree_view && ! itemlist.empty() && item
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
  if ( itemlist.empty() )
    return;

  current_iter -= current_iter.getPosition();
  const int difference = first_visible_line.getPosition();
  first_visible_line -= difference;
  last_visible_line -= difference;
}

//----------------------------------------------------------------------
inline void FListView::lastPos()
{
  if ( itemlist.empty() )
    return;

  const auto element_count = int(getCount());
  current_iter += element_count - current_iter.getPosition() - 1;
  const int difference = element_count - last_visible_line.getPosition() - 1;
  first_visible_line += difference;
  last_visible_line += difference;
}

//----------------------------------------------------------------------
inline bool FListView::expandSubtree()
{
  if ( itemlist.empty() )
    return false;

  auto item = getCurrentItem();

  if ( tree_view && item->isExpandable() && ! item->isExpand() )
  {
    item->expand();
    adjustScrollbars (getCount());
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
inline bool FListView::collapseSubtree()
{
  if ( itemlist.empty() )
    return false;

  auto item = getCurrentItem();

  if ( tree_view && item->isExpandable() && item->isExpand() )
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
  if ( itemlist.empty() )
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
  if ( itemlist.empty() )
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
  if ( itemlist.empty() )
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
  if ( itemlist.empty() || current_iter.getPosition() == 0 )
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
  FScrollbar::sType scrollType = vbar->getScrollType();
  static constexpr int wheel_distance = 4;
  int distance{1};
  first_line_position_before = first_visible_line.getPosition();
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
      stepBackward(distance);
      break;

    case FScrollbar::scrollPageForward:
      distance = int(getClientHeight());
      // fall through
    case FScrollbar::scrollStepForward:
      stepForward(distance);
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

  if ( isShown() )
    drawList();

  if ( scrollType >= FScrollbar::scrollStepBackward
    && scrollType <= FScrollbar::scrollPageForward )
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
  FScrollbar::sType scrollType = hbar->getScrollType();
  static constexpr int wheel_distance = 4;
  int distance{1};
  const int xoffset_before = xoffset;
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

  if ( isShown() )
  {
    drawHeadlines();
    drawList();
  }

  if ( scrollType >= FScrollbar::scrollStepBackward )
  {
    hbar->setValue (xoffset);

    if ( xoffset_before != xoffset )
      hbar->drawBar();

    forceTerminalUpdate();
  }
}

}  // namespace finalcut
