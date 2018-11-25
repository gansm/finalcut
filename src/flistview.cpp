/***********************************************************************
* flistview.cpp - Widget FListView and FListViewItem                   *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2017-2018 Markus Gans                                      *
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

#include <vector>

#include "final/fapplication.h"
#include "final/flistview.h"
#include "final/fscrollbar.h"
#include "final/fstatusbar.h"
#include "final/ftermbuffer.h"

namespace finalcut
{

// Static class attribute
FObject::FObjectIterator FListView::null_iter;

// Function prototypes
long firstNumberFromString (const FString&);
bool sortAscendingByName (const FObject*, const FObject*);
bool sortDescendingByName (const FObject*, const FObject*);
bool sortAscendingByNumber (const FObject*, const FObject*);
bool sortDescendingByNumber (const FObject*, const FObject*);

// non-member functions
//----------------------------------------------------------------------
long firstNumberFromString (const FString& str)
{
  const FString::iterator last = str.end();
  FString::iterator iter = str.begin();
  FString::iterator first_pos;
  FString::iterator last_pos;
  std::size_t pos;
  std::size_t length;
  long number;

  while ( iter != last )
  {
    if ( wchar_t(*iter) >= L'0' && wchar_t(*iter) <= L'9' )
    {
      if ( wchar_t(*(iter - 1)) == L'-' )
        --iter;

      break;
    }

    ++iter;
  }

  first_pos = iter;

  if ( first_pos == last )
    return 0;

  while ( iter != last )
  {
    if ( wchar_t(*iter) < L'0' || wchar_t(*iter) > L'9' )
      break;

    ++iter;
  }

  last_pos = iter;

  if ( last_pos == last )
    return 0;

  pos = std::size_t(std::distance(str.begin(), first_pos)) + 1;
  length = std::size_t(std::distance(first_pos, last_pos));
  const FString num_str = str.mid(pos, length);

  try
  {
    number = num_str.toLong();
  }
  catch (const std::exception&)
  {
    return 0;
  }

  return number;
}

//----------------------------------------------------------------------
bool sortAscendingByName (const FObject* lhs, const FObject* rhs)
{
  const FListViewItem* l_item = static_cast<const FListViewItem*>(lhs);
  const FListViewItem* r_item = static_cast<const FListViewItem*>(rhs);
  const int column = l_item->getSortColumn();
  const FString l_string = l_item->getText(column);
  const FString r_string = r_item->getText(column);

  // lhs < rhs
  return bool( strcasecmp(l_string.c_str(), r_string.c_str()) < 0 );
}

//----------------------------------------------------------------------
bool sortDescendingByName (const FObject* lhs, const FObject* rhs)
{
  const FListViewItem* l_item = static_cast<const FListViewItem*>(lhs);
  const FListViewItem* r_item = static_cast<const FListViewItem*>(rhs);
  const int column = l_item->getSortColumn();
  const FString l_string = l_item->getText(column);
  const FString r_string = r_item->getText(column);

  // lhs > rhs
  return bool( strcasecmp(l_string.c_str(), r_string.c_str()) > 0 );
}

//----------------------------------------------------------------------
bool sortAscendingByNumber (const FObject* lhs, const FObject* rhs)
{
  const FListViewItem* l_item = static_cast<const FListViewItem*>(lhs);
  const FListViewItem* r_item = static_cast<const FListViewItem*>(rhs);
  const int column = l_item->getSortColumn();
  const long l_number = firstNumberFromString(l_item->getText(column));
  const long r_number = firstNumberFromString(r_item->getText(column));

  // lhs < rhs
  return bool( l_number < r_number );
}

//----------------------------------------------------------------------
bool sortDescendingByNumber (const FObject* lhs, const FObject* rhs)
{
  const FListViewItem* l_item = static_cast<const FListViewItem*>(lhs);
  const FListViewItem* r_item = static_cast<const FListViewItem*>(rhs);
  const int column = l_item->getSortColumn();
  const long l_number = firstNumberFromString(l_item->getText(column));
  const long r_number = firstNumberFromString(r_item->getText(column));

  // lhs > rhs
  return bool( l_number > r_number );
}


//----------------------------------------------------------------------
// class FListViewItem
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FListViewItem::FListViewItem (const FListViewItem& item)
  : FObject(item.getParent())
  , column_list(item.column_list)
  , data_pointer(item.data_pointer)
  , root()
  , visible_lines(1)
  , expandable(false)
  , is_expand(false)
  , checkable(false)
  , is_checked(false)
{
  FObject* parent = getParent();

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
FListViewItem::FListViewItem (FObjectIterator parent_iter)
  : FObject((*parent_iter)->getParent())
  , column_list()
  , data_pointer(0)
  , root()
  , visible_lines(1)
  , expandable(false)
  , is_expand(false)
  , checkable(false)
  , is_checked(false)
{
  insert (this, parent_iter);
}

//----------------------------------------------------------------------
FListViewItem::FListViewItem ( const FStringList& cols
                             , FWidget::data_ptr data
                             , FObjectIterator parent_iter )
  : FObject(0)
  , column_list(cols)
  , data_pointer(data)
  , root()
  , visible_lines(1)
  , expandable(false)
  , is_expand(false)
  , checkable(false)
  , is_checked(false)
{
  if ( cols.empty() )
    return;

  replaceControlCodes();
  insert (this, parent_iter);
}

//----------------------------------------------------------------------
FListViewItem::~FListViewItem()  // destructor
{ }


// public methods of FListViewItem
//----------------------------------------------------------------------
int FListViewItem::getSortColumn() const
{
  if ( ! *root )
    return -1;

  FListView* root_obj = static_cast<FListView*>(*root);
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
  std::size_t index = uInt(column - 1);
  return column_list[index];
}

//----------------------------------------------------------------------
uInt FListViewItem::getDepth() const
{
  FObject* parent = getParent();

  if ( parent && parent->isInstanceOf("FListViewItem") )
  {
    FListViewItem* parent_item = static_cast<FListViewItem*>(parent);
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
  std::size_t index = uInt(column - 1);
  FObject* parent = getParent();

  if ( parent && parent->isInstanceOf("FListView") )
  {
    FListView* listview = static_cast<FListView*>(parent);

    if ( ! listview->header[index].fixed_width )
    {
      int length = int(text.getLength());

      if ( length > listview->header[index].width )
        listview->header[index].width = length;
    }
  }

  column_list[index] = text;
}

//----------------------------------------------------------------------
FObject::FObjectIterator FListViewItem::insert (FListViewItem* child)
{
  // Add a FListViewItem as child element
  if ( ! child )
    return FListView::null_iter;

  return appendItem(child);
}

//----------------------------------------------------------------------
FObject::FObjectIterator FListViewItem::insert ( FListViewItem* child
                                               , FObjectIterator parent_iter )
{
  if ( parent_iter == FListView::null_iter )
    return FListView::null_iter;

  if ( *parent_iter )
  {
    if ( (*parent_iter)->isInstanceOf("FListView") )
    {
      // Add FListViewItem to a FListView parent
      FListView* parent = static_cast<FListView*>(*parent_iter);
      return parent->insert (child);
    }
    else if ( (*parent_iter)->isInstanceOf("FListViewItem") )
    {
      // Add FListViewItem to a FListViewItem parent
      FListViewItem* parent = static_cast<FListViewItem*>(*parent_iter);
      return parent->insert (child);
    }
  }

  return FListView::null_iter;
}

//----------------------------------------------------------------------
void FListViewItem::expand()
{
  if ( isExpand() || ! hasChildren() )
    return;

  is_expand = true;
}

//----------------------------------------------------------------------
void FListViewItem::collapse()
{
  if ( ! isExpand() )
    return;

  is_expand = false;
  visible_lines = 1;
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
  FListViewIterator iter = begin();

  while ( iter != end() )
  {
    if ( *iter )
    {
      FListViewItem* item = static_cast<FListViewItem*>(*iter);
      item->sort(cmp);
    }

    ++iter;
  }
}

//----------------------------------------------------------------------
FObject::FObjectIterator FListViewItem::appendItem (FListViewItem* child)
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
  FStringList::iterator iter = column_list.begin();

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

  if ( ! isExpand() || ! hasChildren() )
  {
    visible_lines = 1;
    return visible_lines;
  }

  constFObjectIterator iter = FObject::begin();

  while ( iter != FObject::end() )
  {
    FListViewItem* child = static_cast<FListViewItem*>(*iter);
    visible_lines += child->getVisibleLines();
    ++iter;
  }

  return visible_lines;
}

//----------------------------------------------------------------------
void FListViewItem::setCheckable (bool on)
{
  checkable = on;

  if ( *root )
  {
    FListView* root_obj = static_cast<FListView*>(*root);

    if ( ! root_obj->hasCheckableItems() && isCheckable() )
      root_obj->has_checkable_items = true;
  }
}

//----------------------------------------------------------------------
void FListViewItem::resetVisibleLineCounter()
{
  visible_lines = 0;
  FObject* parent = getParent();

  if ( parent && parent->isInstanceOf("FListViewItem") )
  {
    FListViewItem* parent_item = static_cast<FListViewItem*>(parent);
    return parent_item->resetVisibleLineCounter();
  }
}


//----------------------------------------------------------------------
// class FListViewIterator
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FListViewIterator::FListViewIterator ()
  : iter_path()
  , node()
  , position(0)
{ }

//----------------------------------------------------------------------
FListViewIterator::FListViewIterator (FObjectIterator iter)
  : iter_path()
  , node(iter)
  , position(0)
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
FListViewIterator& FListViewIterator::operator += (volatile int n)
{
  while ( n > 0 )
  {
    nextElement(node);
    n--;
  }

  return *this;
}

//----------------------------------------------------------------------
FListViewIterator& FListViewIterator::operator -= (volatile int n)
{
  while ( n > 0 )
  {
    prevElement(node);
    n--;
  }

  return *this;
}

// private methods of FListViewIterator
//----------------------------------------------------------------------
void FListViewIterator::nextElement (FObjectIterator& iter)
{
  FListViewItem* item = static_cast<FListViewItem*>(*iter);

  if ( item->isExpandable() && item->isExpand() )
  {
    iter_path.push(iter);
    iter = item->begin();
    position++;
  }
  else
  {
    ++iter;
    position++;

    if ( ! iter_path.empty() )
    {
      FObjectIterator& parent_iter = iter_path.top();

      if ( iter == (*parent_iter)->end() )
      {
        iter = parent_iter;
        iter_path.pop();
        ++iter;
      }
    }
  }
}

//----------------------------------------------------------------------
void FListViewIterator::prevElement (FObjectIterator& iter)
{
  FListViewItem* item;
  FObjectIterator start_iter = iter;

  if ( ! iter_path.empty() )
  {
    FObjectIterator& parent_iter = iter_path.top();

    if ( start_iter == (*parent_iter)->begin() )
    {
      iter = parent_iter;
      position--;
      iter_path.pop();
      return;
    }
  }

  --iter;
  item = static_cast<FListViewItem*>(*iter);

  if ( iter == start_iter )
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

  FObjectIterator& parent_iter = iter_path.top();

  while ( node != parent_iter )
    prevElement(node);
}


//----------------------------------------------------------------------
// class FListView
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FListView::FListView (FWidget* parent)
  : FWidget(parent)
  , root()
  , selflist()
  , itemlist()
  , current_iter()
  , first_visible_line()
  , last_visible_line()
  , header()
  , headerline()
  , vbar(0)
  , hbar(0)
  , drag_scroll(fc::noScroll)
  , scroll_repeat(100)
  , scroll_distance(1)
  , scroll_timer(false)
  , tree_view(false)
  , hide_sort_indicator(false)
  , has_checkable_items(false)
  , clicked_expander_pos(-1, -1)
  , clicked_header_pos(-1, -1)
  , clicked_checkbox_item(0)
  , xoffset(0)
  , nf_offset(0)
  , max_line_width(1)
  , sort_column(-1)
  , sort_type()
  , sort_order(fc::unsorted)
  , user_defined_ascending(0)
  , user_defined_descending(0)
{
  init();
}

//----------------------------------------------------------------------
FListView::~FListView()  // destructor
{
  delOwnTimer();
  delete vbar;
  delete hbar;
}

// public methods of FListView
//----------------------------------------------------------------------
std::size_t FListView::getCount()
{
  int n = 0;
  FObjectIterator iter = itemlist.begin();

  while ( iter != itemlist.end() )
  {
    FListViewItem* item = static_cast<FListViewItem*>(*iter);
    n += item->getVisibleLines();
    ++iter;
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
  std::size_t index = uInt(column - 1);
  return header[index].alignment;
}

//----------------------------------------------------------------------
FString FListView::getColumnText (int column) const
{
  // Get the text of column

  if ( column < 1 || header.empty() || column > int(header.size()) )
    return fc::emptyFString::get();

  // Convert column position to address offset (index)
  std::size_t index = uInt(column - 1);
  return header[index].name;
}

//----------------------------------------------------------------------
fc::sorting_type FListView::getColumnSortType (int column) const
{
  fc::sorting_type type;
  std::size_t col = uInt(column);

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
void FListView::setGeometry ( int x, int y
                            , std::size_t w, std::size_t h
                            , bool adjust )
{
  // Set the widget geometry

  FWidget::setGeometry(x, y, w, h, adjust);

  if ( isNewFont() )
  {
    vbar->setGeometry (int(getWidth()), 2, 2, getHeight() - 2);
    hbar->setGeometry (1, int(getHeight()), getWidth() - 2, 1);
  }
  else
  {
    vbar->setGeometry (int(getWidth()), 2, 1, getHeight() - 2);
    hbar->setGeometry (2, int(getHeight()), getWidth() - 2, 1);
  }
}

//----------------------------------------------------------------------
void FListView::setColumnAlignment (int column, fc::text_alignment align)
{
  // Set the alignment for a column

  if ( column < 1 || header.empty() || column > int(header.size()) )
    return;

  // Convert column position to address offset (index)
  std::size_t index = uInt(column - 1);
  header[index].alignment = align;
}

//----------------------------------------------------------------------
void FListView::setColumnText (int column, const FString& label)
{
  // Set the text for a column

  if ( column < 1 || header.empty() || column > int(header.size()) )
    return;

  // Convert column position to address offset (index)
  std::size_t index = uInt(column - 1);

  if ( ! header[index].fixed_width )
  {
    int length = int(label.getLength());

    if ( length > header[index].width )
      header[index].width = length;
  }

  header[index].name = label;
}

//----------------------------------------------------------------------
void FListView::setColumnSortType (int column, fc::sorting_type type)
{
  // Sets the sort type by which the list is to be sorted

  if ( column < 1 || header.empty() || column > int(header.size()) )
    return;

  std::size_t size = uInt(column + 1);

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
  Header new_column;
  new_column.name = label;
  new_column.width = width;

  if ( new_column.width == USE_MAX_SIZE )
  {
    new_column.fixed_width = false;
    new_column.width = int(label.getLength());
  }
  else
    new_column.fixed_width = true;

  header.push_back (new_column);
  return int(std::distance(header.begin(), header.end()));
}

//----------------------------------------------------------------------
FObject::FObjectIterator FListView::insert ( FListViewItem* item
                                           , FObjectIterator parent_iter )
{
  FObjectIterator item_iter;

  if ( parent_iter == FListView::null_iter )
    return FListView::null_iter;

  beforeInsertion(item);  // preprocessing

  if  ( parent_iter == root )
  {
    item_iter = appendItem (item);
  }
  else if ( *parent_iter )
  {
    if ( (*parent_iter)->isInstanceOf("FListView") )
    {
      // Add FListViewItem to a FListView parent
      FListView* parent = static_cast<FListView*>(*parent_iter);
      item_iter = parent->appendItem (item);
    }
    else if ( (*parent_iter)->isInstanceOf("FListViewItem") )
    {
      // Add FListViewItem to a FListViewItem parent
      FListViewItem* parent = static_cast<FListViewItem*>(*parent_iter);
      item_iter = parent->appendItem (item);
    }
    else
      item_iter = FListView::null_iter;
  }
  else
    item_iter = FListView::null_iter;

  afterInsertion();  // post-processing
  return item_iter;
}

//----------------------------------------------------------------------
FObject::FObjectIterator FListView::insert ( const FStringList& cols
                                           , data_ptr d
                                           , FObjectIterator parent_iter )
{
  FListViewItem* item;

  if ( cols.empty() || parent_iter == FListView::null_iter )
    return FListView::null_iter;

  if ( ! *parent_iter )
    parent_iter = root;

  try
  {
    item = new FListViewItem (cols, d, FListView::null_iter);
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
    return FListView::null_iter;
  }

  item->replaceControlCodes();
  return insert(item, parent_iter);
}

//----------------------------------------------------------------------
FObject::FObjectIterator FListView::insert ( const std::vector<long>& cols
                                           , data_ptr d
                                           , FObjectIterator parent_iter )
{
  FObjectIterator item_iter;
  FStringList str_cols;

  if ( ! cols.empty() )
  {
    for (uInt i = 0; i < cols.size(); i++)
      str_cols.push_back (FString().setNumber(cols[i]));
  }

  item_iter = insert (str_cols, d, parent_iter);

  return item_iter;
}

//----------------------------------------------------------------------
void FListView::sort()
{
  // Sorts the list view according to the specified setting

  if ( sort_column < 1 && sort_column > int(header.size()) )
    return;

  switch ( getColumnSortType(sort_column) )
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
  int position_before = current_iter.getPosition()
    , xoffset_before = xoffset
    , first_line_position_before = first_visible_line.getPosition()
    , pagesize = int(getClientHeight()) - 1;
  FKey key = ev->key();
  clicked_expander_pos.setPoint(-1, -1);

  switch ( key )
  {
    case fc::Fkey_return:
    case fc::Fkey_enter:
      processClick();
      ev->accept();
      break;

    case fc::Fkey_space:
      keySpace();
      ev->accept();
      break;

    case fc::Fkey_up:
      stepBackward();
      ev->accept();
      break;

    case fc::Fkey_down:
      stepForward();
      ev->accept();
      break;

    case fc::Fkey_left:
      keyLeft (first_line_position_before);
      ev->accept();
      break;

    case fc::Fkey_right:
      keyRight(first_line_position_before);
      ev->accept();
      break;

    case fc::Fkey_ppage:
      stepBackward(pagesize);
      ev->accept();
      break;

    case fc::Fkey_npage:
      stepForward(pagesize);
      ev->accept();
      break;

    case fc::Fkey_home:
      keyHome();
      ev->accept();
      break;

    case fc::Fkey_end:
      keyEnd();
      ev->accept();
      break;

    case int('+'):
      if ( keyPlus() )
        ev->accept();
      break;

    case int('-'):
      if ( keyMinus() )
        ev->accept();
      break;

    default:
      ev->ignore();
  }

  if ( position_before != current_iter.getPosition() )
    processChanged();

  if ( ev->isAccepted() )
  {
    bool draw_vbar = first_line_position_before
                  != first_visible_line.getPosition();
    bool draw_hbar = xoffset_before != xoffset;
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
    FWidget* focused_widget = getFocusWidget();
    FFocusEvent out (fc::FocusOut_Event);
    FApplication::queueEvent(focused_widget, &out);
    setFocus();

    if ( focused_widget )
      focused_widget->redraw();

    if ( getStatusBar() )
      getStatusBar()->drawMessage();
  }

  int first_line_position_before = first_visible_line.getPosition()
    , mouse_x = ev->getX()
    , mouse_y = ev->getY();

  if ( mouse_x > 1 && mouse_x < int(getWidth()) )
  {
    if ( mouse_y == 1 )  // Header
    {
      clicked_header_pos = ev->getPos();
    }
    else if ( mouse_y > 1 && mouse_y < int(getHeight()) )  // List
    {
      int indent = 0;
      int new_pos = first_visible_line.getPosition() + mouse_y - 2;

      if ( new_pos < int(getCount()) )
        setRelativePosition (mouse_y - 2);

      const FListViewItem* item = getCurrentItem();

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

      if ( isVisible() )
        drawList();

      vbar->setValue (first_visible_line.getPosition());

      if ( vbar->isVisible()
        && first_line_position_before != first_visible_line.getPosition() )
        vbar->drawBar();

      updateTerminal();
      flush_out();
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
    int mouse_x = ev->getX();
    int mouse_y = ev->getY();

    if ( mouse_x > 1 && mouse_x < int(getWidth()) )
    {
      if ( mouse_y == 1 && clicked_header_pos == ev->getPos() )  // Header
      {
        mouseHeaderClicked();
      }
      else if ( mouse_y > 1 && mouse_y < int(getHeight()) )  // List
      {
        int indent = 0;
        FListViewItem* item = getCurrentItem();

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

            adjustSize();

            if ( isVisible() )
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

            if ( isVisible() )
              draw();
          }
        }

        processChanged();
      }
    }
  }

  clicked_expander_pos.setPoint(-1, -1);
  clicked_header_pos.setPoint(-1, -1);
  clicked_checkbox_item = 0;
}

//----------------------------------------------------------------------
void FListView::onMouseMove (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton )
  {
    clicked_expander_pos.setPoint(-1, -1);
    return;
  }

  int first_line_position_before = first_visible_line.getPosition()
    , mouse_x = ev->getX()
    , mouse_y = ev->getY();

  if ( mouse_x > 1 && mouse_x < int(getWidth())
    && mouse_y > 1 && mouse_y < int(getHeight()) )
  {
    int new_pos = first_visible_line.getPosition() + mouse_y - 2;

    if ( new_pos < int(getCount()) )
      setRelativePosition (mouse_y - 2);

    if ( isVisible() )
      drawList();

    vbar->setValue (first_visible_line.getPosition());

    if ( vbar->isVisible()
      && first_line_position_before != first_visible_line.getPosition() )
      vbar->drawBar();

    updateTerminal();
    flush_out();
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
  int mouse_x, mouse_y;

  if ( ev->getButton() != fc::LeftButton )
    return;

  mouse_x = ev->getX();
  mouse_y = ev->getY();

  if ( mouse_x > 1 && mouse_x < int(getWidth())
    && mouse_y > 1 && mouse_y < int(getHeight()) )
  {
    if ( first_visible_line.getPosition() + mouse_y - 1 > int(getCount()) )
      return;

    FListViewItem* item = getCurrentItem();

    if ( tree_view && item->isExpandable() )
    {
      if ( item->isExpand() )
        item->collapse();
      else
        item->expand();

      adjustSize();

      if ( isVisible() )
        draw();
    }

    processClick();
  }

  clicked_expander_pos.setPoint(-1, -1);
}

//----------------------------------------------------------------------
void FListView::onTimer (FTimerEvent*)
{
  int position_before = current_iter.getPosition()
    , first_line_position_before = first_visible_line.getPosition();

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

  if ( isVisible() )
    drawList();

  vbar->setValue (first_visible_line.getPosition());

  if ( vbar->isVisible()
    && first_line_position_before != first_visible_line.getPosition() )
    vbar->drawBar();

  updateTerminal();
  flush_out();
}

//----------------------------------------------------------------------
void FListView::onWheel (FWheelEvent* ev)
{
  int position_before = current_iter.getPosition()
    , first_line_position_before = first_visible_line.getPosition()
    , pagesize = 4;

  if ( drag_scroll != fc::noScroll )
    stopDragScroll();

  switch ( ev->getWheel() )
  {
    case fc::WheelUp:
      wheelUp (pagesize);
      break;

    case fc::WheelDown:
      wheelDown (pagesize);
      break;

    default:
      break;
  }

  if ( position_before != current_iter.getPosition() )
    processChanged();

  if ( isVisible() )
    drawList();

  vbar->setValue (first_visible_line.getPosition());

  if ( vbar->isVisible()
    && first_line_position_before != first_visible_line.getPosition() )
    vbar->drawBar();

  updateTerminal();
  flush_out();
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

  delOwnTimer();
}


// protected methods of FListView
//----------------------------------------------------------------------
void FListView::adjustViewport()
{
  int element_count = int(getCount());
  int height = int(getClientHeight());

  if ( element_count == 0 || height <= 0 )
    return;

  if ( element_count < height )
  {
    first_visible_line = itemlist.begin();
    last_visible_line = first_visible_line;
    last_visible_line += element_count - 1;
  }

  if ( first_visible_line.getPosition() > element_count - height  )
  {
    int difference = first_visible_line.getPosition()
                   - (element_count - height);

    if ( first_visible_line.getPosition() >= difference )
    {
      first_visible_line -= difference;
      last_visible_line -= difference;
    }
  }

  int after_last_visible_line = first_visible_line.getPosition()
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
void FListView::adjustSize()
{
  std::size_t element_count;
  FWidget::adjustSize();
  adjustViewport();

  element_count = getCount();
  vbar->setMaximum (int(element_count - getClientHeight()));
  vbar->setPageSize (int(element_count), int(getClientHeight()));
  vbar->setX (int(getWidth()));
  vbar->setHeight (getClientHeight(), false);
  vbar->resize();

  hbar->setMaximum (max_line_width - int(getClientWidth()));
  hbar->setPageSize (max_line_width, int(getClientWidth()));
  hbar->setY (int(getHeight()));
  hbar->setWidth (getClientWidth(), false);
  hbar->resize();

  if ( element_count <= getClientHeight() )
    vbar->hide();
  else
    vbar->setVisible();

  if ( max_line_width <= int(getClientWidth()) )
    hbar->hide();
  else
    hbar->setVisible();
}


// private methods of FListView
//----------------------------------------------------------------------
void FListView::init()
{
  selflist.push_back(this);
  root = selflist.begin();
  null_iter = selflist.end();

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

  setGeometry (1, 1, 5, 4, false);  // initialize geometry values

  vbar->addCallback
  (
    "change-value",
    F_METHOD_CALLBACK (this, &FListView::cb_VBarChange)
  );

  hbar->addCallback
  (
    "change-value",
    F_METHOD_CALLBACK (this, &FListView::cb_HBarChange)
  );

  nf_offset = isNewFont() ? 1 : 0;
  setTopPadding(1);
  setLeftPadding(1);
  setBottomPadding(1);
  setRightPadding(1 + nf_offset);
}

//----------------------------------------------------------------------
template <typename Compare>
void FListView::sort (Compare cmp)
{
  // Sort the top level
  itemlist.sort(cmp);

  // Sort the sublevels
  FListViewIterator iter = itemlist.begin();

  while ( iter != itemlist.end() )
  {
    if ( *iter )
    {
      FListViewItem* item = static_cast<FListViewItem*>(*iter);
      item->sort(cmp);
    }

    ++iter;
  }
}

//----------------------------------------------------------------------
std::size_t FListView::getAlignOffset ( fc::text_alignment align
                                      , std::size_t txt_length
                                      , std::size_t width )
{
  switch ( align )
  {
    case fc::alignLeft:
      return 0;

    case fc::alignCenter:
      if ( txt_length < width )
        return (width - txt_length) / 2;
      else
        return 0;

    case fc::alignRight:
      if ( txt_length < width )
        return width - txt_length;
      else
        return 0;
  }

  return 0;
}

//----------------------------------------------------------------------
void FListView::draw()
{
  if ( current_iter.getPosition() < 1 )
    current_iter = itemlist.begin();

  setColor();

  if ( isMonochron() )
    setReverse(true);

  if ( isNewFont() )
    drawBorder (1, 1, int(getWidth()) - 1, int(getHeight()));
  else
    drawBorder();

  if ( isNewFont() && ! vbar->isVisible() )
  {
    setColor();

    for (int y = 2; y < int(getHeight()); y++)
    {
      setPrintPos (int(getWidth()), y);
      print (' ');  // clear right side of the scrollbar
    }
  }

  drawHeadlines();

  if ( isMonochron() )
    setReverse(false);

  if ( vbar->isVisible() )
    vbar->redraw();

  if ( hbar->isVisible() )
    hbar->redraw();

  drawList();

  if ( flags.focus && getStatusBar() )
  {
    const FString& msg = getStatusbarMessage();
    const FString& curMsg = getStatusBar()->getMessage();

    if ( curMsg != msg )
    {
      getStatusBar()->setMessage(msg);
      getStatusBar()->drawMessage();
    }
  }
}

//----------------------------------------------------------------------
void FListView::drawHeadlines()
{
  std::vector<charData>::const_iterator first, last;
  headerItems::const_iterator iter;

  if ( header.empty()
    || getHeight() <= 2
    || getWidth() <= 4
    || max_line_width < 1 )
    return;

  iter = header.begin();
  headerline.clear();

  if ( hasCheckableItems() )
    drawHeaderBorder(4);

  while ( iter != header.end() )
  {
    const FString& text = iter->name;

    if ( text.isNull() || text.isEmpty() )
    {
      ++iter;
      continue;
    }

    drawHeadlineLabel(iter);
    ++iter;
  }

  std::vector<charData> h;
  h << headerline;
  first = h.begin() + xoffset;

  if ( h.size() <= getClientWidth() )
    last = h.end();
  else
  {
    int len = int(getClientWidth()) + xoffset - 1;

    if ( len > int(h.size()) )
      len = int(h.size());

    last = h.begin() + len;
  }

  setPrintPos (2, 1);
  print() << std::vector<charData>(first, last);
}

//----------------------------------------------------------------------
void FListView::drawList()
{
  uInt page_height, y;
  FListViewIterator iter;

  if ( itemlist.empty() || getHeight() <= 2 || getWidth() <= 4 )
    return;

  y = 0;
  page_height = uInt(getHeight() - 2);
  iter = first_visible_line;

  while ( iter != itemlist.end() && y < page_height )
  {
    bool is_current_line = bool( iter == current_iter );
    const FListViewItem* item = static_cast<FListViewItem*>(*iter);
    int tree_offset = ( tree_view ) ? int(item->getDepth() << 1) + 1 : 0;
    int checkbox_offset = ( item->isCheckable() ) ? 1 : 0;
    setPrintPos (2, 2 + int(y));

    // Draw one FListViewItem
    drawListLine (item, flags.focus, is_current_line);

    if ( flags.focus && is_current_line )
    {
      setVisibleCursor (item->isCheckable());
      setCursorPos ( 3 + tree_offset + checkbox_offset - xoffset
                   , 2 + int(y));  // first character
    }

    last_visible_line = iter;
    y++;
    ++iter;
  }

  // Reset color
  setColor();

  if ( isMonochron() )
    setReverse(true);

  // Clean empty space after last element
  while ( y < uInt(getClientHeight()) )
  {
    setPrintPos (2, 2 + int(y));
    print (FString(std::size_t(getClientWidth()), ' '));
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
  std::size_t indent = item->getDepth() << 1;  // indent = 2 * depth
  FString line = getLinePrefix (item, indent);

  // Print columns
  if ( ! item->column_list.empty() )
  {
    for (std::size_t col = 0; col < item->column_list.size(); )
    {
      static const std::size_t leading_space = 1;
      static const std::size_t checkbox_space = 4;
      static const std::size_t ellipsis_length = 2;

      const FString& text = item->column_list[col];
      std::size_t width = std::size_t(header[col].width);
      std::size_t txt_length = text.getLength();
      // Increment the value of i for the column position
      // and the next iteration
      col++;
      fc::text_alignment align = getColumnAlignment(int(col));
      std::size_t align_offset = getAlignOffset (align, txt_length, width);

      if ( tree_view && col == 1 )
      {
        width -= (indent + 1);

        if ( item->isCheckable() )
          width -= checkbox_space;
      }

      // Insert alignment spaces
      if ( align_offset > 0 )
        line += FString(align_offset, L' ');

      if ( align_offset + txt_length <= width )
      {
        // Insert text and trailing space
        line += text.left(width);
        line += FString ( leading_space + width
                        - align_offset - txt_length, L' ');
      }
      else if ( align == fc::alignRight )
      {
        // Ellipse right align text
        line += FString (L"..");
        line += text.right(width - ellipsis_length);
        line += L' ';
      }
      else
      {
        // Ellipse left align text and center text
        line += text.left(width - ellipsis_length);
        line += FString (L".. ");
      }
    }
  }

  line = line.mid ( std::size_t(xoffset) + 1
                  , getWidth() - std::size_t(nf_offset) - 2);
  const wchar_t* const& element_str = line.wc_str();
  std::size_t len = line.getLength();
  std::size_t i;

  for (i = 0; i < len; i++)
    *this << element_str[i];

  for (; i < getWidth() - std::size_t(nf_offset) - 2; i++)
    print (' ');
}

//----------------------------------------------------------------------
inline void FListView::setLineAttributes ( bool is_current
                                         , bool is_focus )
{
  setColor (wc.list_fg, wc.list_bg);

  if ( is_current )
  {
    if ( is_focus && getMaxColor() < 16 )
      setBold();

    if ( isMonochron() )
      unsetBold();

    if ( is_focus )
    {
      setColor ( wc.current_element_focus_fg
               , wc.current_element_focus_bg );
    }
    else
      setColor ( wc.current_element_fg
               , wc.current_element_bg );

    if ( isMonochron() )
      setReverse(false);
  }
  else
  {
    if ( isMonochron() )
      setReverse(true);
    else if ( is_focus && getMaxColor() < 16 )
      unsetBold();
  }
}

//----------------------------------------------------------------------
inline FString FListView::getCheckBox (const FListViewItem* item)
{
  FString checkbox;

  if ( isNewFont() )
  {
    checkbox = ( item->isChecked() ) ? CHECKBOX_ON : CHECKBOX;
    checkbox += L' ';
  }
  else
  {
    checkbox = L"[ ] ";

    if ( item->isChecked() )
      checkbox[1] = wchar_t(fc::Times);  // Times ×
  }

  return checkbox;
}

//----------------------------------------------------------------------
inline FString FListView::getLinePrefix ( const FListViewItem* item
                                        , std::size_t indent )
{
  FString line;

  if ( tree_view )
  {
    if ( indent > 0 )
      line = FString(indent, L' ');

    if ( item->isExpandable()  )
    {
      if ( item->isExpand() )
      {
        line += wchar_t(fc::BlackDownPointingTriangle);  // ▼
        line += L' ';
      }
      else
      {
        line += wchar_t(fc::BlackRightPointingPointer);  // ►
        line += L' ';
      }
    }
    else
      line += L"  ";
  }
  else
    line = L" ";

  if ( item->isCheckable() )
    line += getCheckBox(item);

  return line;
}

//----------------------------------------------------------------------
inline void FListView::drawSortIndicator ( std::size_t& length
                                         , std::size_t  column_width )
{
  if ( length >= column_width )
    return;

  setColor();
  length++;

  if ( sort_order == fc::ascending )
    headerline << wchar_t(fc::BlackUpPointingTriangle);    // ▲
  else if ( sort_order == fc::descending )
    headerline << wchar_t(fc::BlackDownPointingTriangle);  // ▼

  if ( length < column_width  )
  {
    length++;
    headerline << ' ';
  }
}

//----------------------------------------------------------------------
inline void FListView::drawHeaderBorder (std::size_t length)
{
  setColor();
  const FString line (length, wchar_t(fc::BoxDrawingsHorizontal));
  headerline << line;  // horizontal line
}

//----------------------------------------------------------------------
void FListView::drawHeadlineLabel (headerItems::const_iterator& iter)
{
  // Print lable text
  static const std::size_t leading_space = 1;
  const FString& text = iter->name;
  FString txt = " " + text;
  std::size_t width = std::size_t(iter->width);
  std::size_t txt_length = txt.getLength();
  std::size_t column_width = leading_space + width;
  headerItems::const_iterator first = header.begin();
  int column = int(std::distance(first, iter)) + 1;
  bool has_sort_indicator = bool ( sort_column == column
                                && ! hide_sort_indicator );

  if ( isEnabled() )
    setColor (wc.label_emphasis_fg, wc.label_bg);
  else
    setColor (wc.label_inactive_fg, wc.label_inactive_bg);

  if ( has_sort_indicator && txt_length >= column_width - 1 && txt_length > 1 )
  {
    txt_length = column_width - 2;
    txt = txt.left(txt_length);
  }

  if ( txt_length <= column_width )
  {
    std::size_t length = txt_length;
    headerline << txt;

    if ( length < column_width )
    {
      length++;
      headerline << ' ';  // trailing space
    }

    if ( has_sort_indicator )
      drawSortIndicator (length, column_width );

    if ( length < column_width )
      drawHeaderBorder (column_width - length);
  }
  else
    drawColumnEllipsis (iter, text);  // Print ellipsis
}

//----------------------------------------------------------------------
void FListView::drawColumnEllipsis ( headerItems::const_iterator& iter
                                   , const FString& text )
{
  // Print lable ellipsis
  static const int ellipsis_length = 2;
  int width = iter->width;

  headerline << ' ';
  headerline << text.left(uInt(width - ellipsis_length));
  setColor (wc.label_ellipsis_fg, wc.label_bg);
  headerline << "..";

  if ( iter == header.end() - 1 )  // Last element
    headerline << ' ';
}

//----------------------------------------------------------------------
void FListView::updateDrawing (bool draw_vbar, bool draw_hbar)
{
  if ( isVisible() )
    draw();

  vbar->setValue (first_visible_line.getPosition());

  if ( vbar->isVisible() && draw_vbar )
    vbar->drawBar();

  hbar->setValue (xoffset);

  if ( hbar->isVisible() && draw_hbar )
    hbar->drawBar();

  updateTerminal();
  flush_out();
}

//----------------------------------------------------------------------
int FListView::determineLineWidth (FListViewItem* item)
{
  static const int padding_space = 1;
  int  line_width = padding_space;  // leading space
  uInt column_idx = 0;
  uInt entries = uInt(item->column_list.size());
  headerItems::iterator header_iter;
  header_iter = header.begin();

  while ( header_iter != header.end() )
  {
    int width = header_iter->width;
    bool fixed_width = header_iter->fixed_width;

    if ( ! fixed_width )
    {
      int len;

      if ( column_idx < entries )
        len = int(item->column_list[column_idx].getLength());
      else
        len = 0;

      if ( len > width )
        header_iter->width = len;
    }

    line_width += header_iter->width + padding_space;  // width + trailing space
    column_idx++;
    ++header_iter;
  }

  return line_width;
}

//----------------------------------------------------------------------
inline void FListView::beforeInsertion (FListViewItem* item)
{
  int line_width = determineLineWidth (item);
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

  int element_count = int(getCount());
  recalculateVerticalBar (element_count);
}

//----------------------------------------------------------------------
void FListView::recalculateHorizontalBar (int len)
{
  if ( len <= max_line_width )
    return;

  max_line_width = len;

  if ( len >= int(getWidth()) - nf_offset - 3 )
  {
    hbar->setMaximum (max_line_width - int(getWidth()) + nf_offset + 4);
    hbar->setPageSize (max_line_width, int(getWidth()) - nf_offset - 4);
    hbar->calculateSliderValues();

    if ( ! hbar->isVisible() )
      hbar->setVisible();
  }
}

//----------------------------------------------------------------------
void FListView::recalculateVerticalBar (int element_count)
{
  vbar->setMaximum (element_count - int(getHeight()) + 2);
  vbar->setPageSize (element_count, int(getHeight()) - 2);
  vbar->calculateSliderValues();

  if ( ! vbar->isVisible() && element_count >= int(getHeight()) - 1 )
    vbar->setVisible();
}

//----------------------------------------------------------------------
void FListView::mouseHeaderClicked()
{
  int column = 1;
  int checkbox_offset = ( hasCheckableItems() ) ? 4 : 0;
  int header_start = 2 + checkbox_offset;
  int header_pos = clicked_header_pos.getX() + xoffset;
  headerItems::const_iterator iter;
  iter = header.begin();

  while ( iter != header.end() )
  {
    static const int leading_space = 1;
    bool has_sort_indicator = bool( column == sort_column );
    int click_width = int(iter->name.getLength());

    if ( has_sort_indicator )
      click_width += 2;

    if ( click_width > iter->width )
      click_width = iter->width;

    if ( header_pos > header_start
      && header_pos <= header_start + click_width )
    {
      if ( has_sort_indicator && sort_order == fc::ascending )
        setColumnSort (column, fc::descending);
      else
        setColumnSort (column, fc::ascending);

      sort();

      if ( isVisible() )
      {
        drawHeadlines();
        drawList();
        updateTerminal();
        flush_out();
      }
      break;
    }

    header_start += leading_space + iter->width;
    column++;
    ++iter;
  }
}

//----------------------------------------------------------------------
void FListView::wheelUp (int pagesize)
{
  if ( current_iter.getPosition() == 0 )
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
    int ry = current_iter.getPosition() - first_visible_line.getPosition();
    // Save difference from top
    int difference = first_visible_line.getPosition();
    first_visible_line -= difference;
    last_visible_line -= difference;
    setRelativePosition(ry);
  }
}

//----------------------------------------------------------------------
void FListView::wheelDown (int pagesize)
{
  int element_count = int(getCount());

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
    int ry = current_iter.getPosition() - first_visible_line.getPosition();
    // Save difference from bottom
    int differenz = element_count - last_visible_line.getPosition() - 1;
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
  int element_count = int(getCount());

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
    delOwnTimer();
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
    delOwnTimer();
    drag_scroll = fc::noScroll;
  }
}

//----------------------------------------------------------------------
void FListView::stopDragScroll()
{
  delOwnTimer();
  scroll_timer = false;
  scroll_distance = 1;
  drag_scroll = fc::noScroll;
}

//----------------------------------------------------------------------
FObject::FObjectIterator FListView::appendItem (FListViewItem* item)
{
  item->root = root;
  addChild (item);
  itemlist.push_back (item);
  return --itemlist.end();
}

//----------------------------------------------------------------------
void FListView::processClick()
{
  emitCallback("clicked");
}

//----------------------------------------------------------------------
void FListView::processChanged()
{
  emitCallback("row-changed");
}

//----------------------------------------------------------------------
inline void FListView::keySpace()
{
  FListViewItem* item = getCurrentItem();

  if ( item->isCheckable() )
    item->setChecked(! item->isChecked());
}

//----------------------------------------------------------------------
inline void FListView::keyLeft (int& first_line_position_before)
{
  int position_before = current_iter.getPosition();
  FListViewItem* item = getCurrentItem();

  if ( xoffset == 0 )
  {
    if ( tree_view && item->isExpandable() && item->isExpand() )
    {
      // Collapse element
      item->collapse();
      adjustSize();
      int element_count = int(getCount());
      recalculateVerticalBar (element_count);
      // Force vertical scrollbar redraw
      first_line_position_before = -1;
    }
    else if ( item->hasParent() )
    {
      // Jump to parent element
      FObject* parent = item->getParent();

      if ( parent->isInstanceOf("FListViewItem") )
      {
        current_iter.parentElement();

        if ( current_iter.getPosition() < first_line_position_before )
        {
          int difference = position_before - current_iter.getPosition();

          if ( first_visible_line.getPosition() - difference >= 0 )
          {
            first_visible_line -= difference;
            last_visible_line -= difference;
          }
          else
          {
            int d = first_visible_line.getPosition();
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
inline void FListView::keyRight (int& first_line_position_before)
{
  int xoffset_end = max_line_width - int(getClientWidth());
  FListViewItem* item = getCurrentItem();

  if ( tree_view && item->isExpandable() && ! item->isExpand() )
  {
    // Expand element
    item->expand();
    adjustSize();
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
inline void FListView::keyHome()
{
  current_iter -= current_iter.getPosition();
  int difference = first_visible_line.getPosition();
  first_visible_line -= difference;
  last_visible_line -= difference;
}

//----------------------------------------------------------------------
inline void FListView::keyEnd()
{
  int element_count = int(getCount());
  current_iter += element_count - current_iter.getPosition() - 1;
  int difference = element_count - last_visible_line.getPosition() - 1;
  first_visible_line += difference;
  last_visible_line += difference;
}

//----------------------------------------------------------------------
inline bool FListView::keyPlus()
{
  FListViewItem* item = getCurrentItem();

  if ( tree_view && item->isExpandable() && ! item->isExpand() )
  {
    item->expand();
    adjustSize();
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
inline bool FListView::keyMinus()
{
  FListViewItem* item = getCurrentItem();

  if ( tree_view && item->isExpandable() && item->isExpand() )
  {
    item->collapse();
    adjustSize();
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
  int element_count = int(getCount());

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
      int differenz = element_count - last_visible_line.getPosition() - 1;
      first_visible_line += differenz;
      last_visible_line += differenz;
    }
  }
}

//----------------------------------------------------------------------
void FListView::stepBackward (int distance)
{
  if ( current_iter.getPosition() == 0 )
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
      int difference = first_visible_line.getPosition();
      first_visible_line -= difference;
      last_visible_line -= difference;
    }
  }
}

//----------------------------------------------------------------------
void FListView::scrollToX (int x)
{
  int xoffset_end = max_line_width - int(getClientWidth());

  if ( xoffset == x )
    return;

  xoffset = x;

  if ( xoffset > xoffset_end )
    xoffset = xoffset_end;

  if ( xoffset < 0 )
    xoffset = 0;
}

//----------------------------------------------------------------------
void FListView::scrollToY (int y)
{
  int pagesize = int(getClientHeight()) - 1;
  int element_count = int(getCount());

  if ( first_visible_line.getPosition() == y )
    return;

  // Save relative position from the top line
  int ry = current_iter.getPosition() - first_visible_line.getPosition();

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
    int differenz = element_count - last_visible_line.getPosition() - 1;
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
void FListView::cb_VBarChange (FWidget*, data_ptr)
{
  FScrollbar::sType scrollType = vbar->getScrollType();
  int distance = 1
    , pagesize = 4
    , first_line_position_before = first_visible_line.getPosition();

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
    {
      int value = vbar->getValue();
      scrollToY (value);
      break;
    }

    case FScrollbar::scrollWheelUp:
    wheelUp (pagesize);
    break;

    case FScrollbar::scrollWheelDown:
    wheelDown (pagesize);
    break;
  }

  if ( isVisible() )
    drawList();

  if ( scrollType >= FScrollbar::scrollStepBackward
    && scrollType <= FScrollbar::scrollPageForward )
  {
    vbar->setValue (first_visible_line.getPosition());

    if ( vbar->isVisible()
      && first_line_position_before != first_visible_line.getPosition() )
      vbar->drawBar();

    updateTerminal();
    flush_out();
  }
}

//----------------------------------------------------------------------
void FListView::cb_HBarChange (FWidget*, data_ptr)
{
  FScrollbar::sType scrollType = hbar->getScrollType();
  int distance = 1
    , pagesize = 4
    , xoffset_before = xoffset;

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
    {
      int value = hbar->getValue();
      scrollToX(value);
      break;
    }

    case FScrollbar::scrollWheelUp:
      scrollBy (-pagesize, 0);
      break;

    case FScrollbar::scrollWheelDown:
      scrollBy (pagesize, 0);
      break;
  }

  if ( isVisible() )
  {
    drawHeadlines();
    drawList();
    updateTerminal();
    flush_out();
  }

  if ( scrollType >= FScrollbar::scrollStepBackward
    && scrollType <= FScrollbar::scrollWheelDown )
  {
    hbar->setValue (xoffset);

    if ( hbar->isVisible() && xoffset_before != xoffset )
      hbar->drawBar();

    updateTerminal();
    flush_out();
  }
}

}  // namespace finalcut
