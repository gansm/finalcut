// File: flistview.cpp
// Provides: class FListViewItem
//           class FListView

#include <vector>

#include "final/fapplication.h"
#include "final/flistview.h"
#include "final/fscrollbar.h"
#include "final/fstatusbar.h"
#include "final/ftermbuffer.h"

//----------------------------------------------------------------------
// class FListViewItem
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FListViewItem::FListViewItem (const FListViewItem& item)
  : FObject(item.getParent())
  , column_list(item.column_list)
  , data_pointer(item.data_pointer)
  , visible_lines(1)
  , expandable(false)
  , is_expand(false)
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
  : FObject(0)
  , column_list()
  , data_pointer(0)
  , visible_lines(1)
  , expandable(false)
  , is_expand(false)
{
  insert (this, parent_iter);
}

//----------------------------------------------------------------------
FListViewItem::FListViewItem ( const std::vector<FString>& cols
                             , FWidget::data_ptr data
                             , FObjectIterator parent_iter )
  : FObject(0)
  , column_list(cols)
  , data_pointer(data)
  , visible_lines(1)
  , expandable(false)
  , is_expand(false)
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
FString FListViewItem::getText (int column) const
{
  if ( column < 1
     || column_list.empty()
     || column > int(column_list.size()) )
    return *fc::empty_string;

  column--;  // Convert column position to address offset (index)
  return column_list[uInt(column)];
}

//----------------------------------------------------------------------
void FListViewItem::setText (int column, const FString& text)
{
  if ( column < 1
     || column_list.empty()
     || column > int(column_list.size()) )
    return;

  FObject* parent = getParent();
  column--;  // Convert column position to address offset (index)

  if ( parent && parent->isInstanceOf("FListView") )
  {
    FListView* listview = static_cast<FListView*>(parent);

    if ( ! listview->header[uInt(column)].fixed_width )
    {
      int length = int(text.getLength());

      if ( length > listview->header[uInt(column)].width )
        listview->header[uInt(column)].width = length;
    }
  }

  column_list[uInt(column)] = text;
}

//----------------------------------------------------------------------
FObject::FObjectIterator FListViewItem::insert (FListViewItem* child)
{
  // Add a FListViewItem as child element
  if ( ! child )
    return FObjectIterator(0);

  return appendItem(child);
}

//----------------------------------------------------------------------
FObject::FObjectIterator FListViewItem::insert ( FListViewItem* child
                                               , FObjectIterator parent_iter )
{
  if ( parent_iter == FObjectIterator(0) )
    return FObjectIterator(0);

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

  return FObjectIterator(0);
}

//----------------------------------------------------------------------
void FListViewItem::expand()
{
  if ( is_expand || ! hasChildren() )
    return;

  is_expand = true;
}

//----------------------------------------------------------------------
void FListViewItem::collapse()
{
  if ( ! is_expand )
    return;

  is_expand = false;
}

// private methods of FListView
//----------------------------------------------------------------------
FObject::FObjectIterator FListViewItem::appendItem (FListViewItem* child)
{
  expandable = true;
  addChild (child);
  return --FObject::end();
}

//----------------------------------------------------------------------
void FListViewItem::replaceControlCodes()
{
  // Replace the control codes characters
  std::vector<FString>::iterator iter = column_list.begin();

  while ( iter != column_list.end() )
  {
    *iter = iter->replaceControlCodes();
    ++iter;
  }
}

//----------------------------------------------------------------------
int FListViewItem::getVisibleLines()
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
// class FListView
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FListView::FListView (FWidget* parent)
  : FWidget(parent)
  , root()
  , selflist()
  , itemlist()
  , header()
  , headerline()
  , vbar(0)
  , hbar(0)
  , drag_scroll(fc::noScroll)
  , scroll_repeat(100)
  , scroll_distance(1)
  , scroll_timer(false)
  , tree_view(false)
  , current(0)
  , xoffset(0)
  , yoffset(0)
  , nf_offset(0)
  , max_line_width(1)
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
fc::text_alignment FListView::getColumnAlignment (int column) const
{
  // Get the alignment for a column

  if ( column < 1 || header.empty() || column > int(header.size()) )
    return fc::alignLeft;

  column--;  // Convert column position to address offset (index)
  return header[uInt(column)].alignment;
}

//----------------------------------------------------------------------
FString FListView::getColumnText (int column) const
{
  // Get the text of column

  if ( column < 1 || header.empty() || column > int(header.size()) )
    return *fc::empty_string;

  column--;  // Convert column position to address offset (index)
  return header[uInt(column)].name;
}

//----------------------------------------------------------------------
void FListView::setGeometry (int x, int y, int w, int h, bool adjust)
{
  // Set the widget geometry

  FWidget::setGeometry(x, y, w, h, adjust);

  if ( isNewFont() )
  {
    vbar->setGeometry (getWidth(), 2, 2, getHeight() - 2);
    hbar->setGeometry (1, getHeight(), getWidth() - 2, 1);
  }
  else
  {
    vbar->setGeometry (getWidth(), 2, 1, getHeight() - 2);
    hbar->setGeometry (2, getHeight(), getWidth() - 2, 1);
  }
}

//----------------------------------------------------------------------
void FListView::setColumnAlignment (int column, fc::text_alignment align)
{
  // Set the alignment for a column

  if ( column < 1 || header.empty() || column > int(header.size()) )
    return;

  column--;  // Convert column position to address offset (index)
  header[uInt(column)].alignment = align;
}

//----------------------------------------------------------------------
void FListView::setColumnText (int column, const FString& label)
{
  // Set the text for a column

  if ( column < 1 || header.empty() || column > int(header.size()) )
    return;

  column--;  // Convert column position to address offset (index)

  if ( ! header[uInt(column)].fixed_width )
  {
    int length = int(label.getLength());

    if ( length > header[uInt(column)].width )
      header[uInt(column)].width = length;
  }

  header[uInt(column)].name = label;
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
  static const int padding_space = 1;
  int  line_width = padding_space;  // leading space
  uInt column_idx = 0;
  uInt entries = uInt(item->column_list.size());
  FObjectIterator item_iter;
  headerItems::iterator header_iter;

  if ( parent_iter == FObjectIterator(0) )
    return FObjectIterator(0);

  header_iter = header.begin();

  while ( header_iter != header.end() )
  {
    int width = (*header_iter).width;
    bool fixed_width = (*header_iter).fixed_width;

    if ( ! fixed_width )
    {
      int len;

      if ( column_idx < entries )
        len = int(item->column_list[column_idx].getLength());
      else
        len = 0;

      if ( len > width )
        (*header_iter).width = len;
    }

    line_width += (*header_iter).width + padding_space;  // width + tailing space
    column_idx++;
    ++header_iter;
  }

  recalculateHorizontalBar (line_width);

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
  }

  int element_count = int(getCount());
  recalculateVerticalBar (element_count);
  return item_iter;
}

//----------------------------------------------------------------------
FObject::FObjectIterator FListView::insert ( const std::vector<FString>& cols
                                           , data_ptr d
                                           , FObjectIterator parent_iter )
{
  FListViewItem* item;

  if ( cols.empty() || parent_iter == FObjectIterator(0) )
    return FObjectIterator(0);

  if ( ! *parent_iter )
    parent_iter = root;

  try
  {
    item = new FListViewItem (cols, d, FObjectIterator(0));
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
    return FObjectIterator(0);
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
  std::vector<FString> str_cols;

  if ( ! cols.empty() )
  {
    for (uInt i = 0; i < cols.size(); i++)
      str_cols.push_back (FString().setNumber(cols[i]));
  }

  item_iter = insert (str_cols, d, parent_iter);

  return item_iter;
}

//----------------------------------------------------------------------
void FListView::onKeyPress (FKeyEvent* ev)
{
  int element_count = int(getCount());
  int current_before = current;
  int xoffset_before = xoffset;
  int xoffset_end = max_line_width - getClientWidth();
  int yoffset_before = yoffset;
  int yoffset_end = element_count - getClientHeight();
  int key = ev->key();

  switch ( key )
  {
    case fc::Fkey_return:
    case fc::Fkey_enter:
      processClick();
      ev->accept();
      break;

    case fc::Fkey_up:
      current--;

      if ( current < 1 )
        current = 1;

      if ( current <= yoffset )
        yoffset--;

      ev->accept();
      break;

    case fc::Fkey_down:
      current++;

      if ( current > element_count )
        current = element_count;

      if ( current - yoffset > getClientHeight() )
        yoffset++;

      ev->accept();
      break;

    case fc::Fkey_left:
      xoffset--;

      if ( xoffset < 0 )
        xoffset = 0;

      ev->accept();
      break;

    case fc::Fkey_right:
      xoffset++;

      if ( xoffset > xoffset_end )
        xoffset = xoffset_end;

      if ( xoffset < 0 )
        xoffset = 0;

      ev->accept();
      break;

    case fc::Fkey_ppage:
      current -= getClientHeight() - 1;

      if ( current < 1 )
        current = 1;

      if ( current <= yoffset )
      {
        yoffset -= getClientHeight() - 1;

        if ( yoffset < 0 )
          yoffset = 0;
      }

      ev->accept();
      break;

    case fc::Fkey_npage:
      current += getClientHeight() - 1;

      if ( current > element_count )
        current = element_count;

      if ( current - yoffset > getClientHeight() )
      {
        yoffset += getClientHeight() - 1;

        if ( yoffset > yoffset_end )
          yoffset = yoffset_end;
      }

      ev->accept();
      break;

    case fc::Fkey_home:
      current = 1;
      yoffset = 0;
      ev->accept();
      break;

    case fc::Fkey_end:
      current = element_count;

      if ( current > getClientHeight() )
        yoffset = yoffset_end;

      ev->accept();
      break;

    case int('+'):
      {
        FListViewItem* item = getCurrentItem();
        item->expand();
        ev->accept();
      }
      break;

    case int('-'):
      {
        FListViewItem* item = getCurrentItem();
        item->collapse();
        ev->accept();
      }
      break;

    default:
      ev->ignore();
  }

  if ( current_before != current )
    processChanged();

  if ( ev->isAccepted() )
  {
    if ( isVisible() )
    {
      drawColumnLabels();
      drawList();
    }

    vbar->setValue (yoffset);

    if ( vbar->isVisible() && yoffset_before != yoffset )
      vbar->drawBar();

    hbar->setValue (xoffset);

    if ( hbar->isVisible() && xoffset_before != xoffset )
      hbar->drawBar();

    updateTerminal();
    flush_out();
  }
}

//----------------------------------------------------------------------
void FListView::onMouseDown (FMouseEvent* ev)
{
  int yoffset_before, mouse_x, mouse_y;

  if ( ev->getButton() != fc::LeftButton
      && ev->getButton() != fc::RightButton )
  {
    return;
  }

  if ( ev->getButton() == fc::RightButton )
    return;

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

  yoffset_before = yoffset;
  mouse_x = ev->getX();
  mouse_y = ev->getY();

  if ( mouse_x > 1 && mouse_x < getWidth()
      && mouse_y > 1 && mouse_y < getHeight() )
  {
    current = yoffset + mouse_y - 1;

    if ( current > int(getCount()) )
      current = int(getCount());

    if ( isVisible() )
      drawList();

    vbar->setValue (yoffset);

    if ( vbar->isVisible() && yoffset_before != yoffset )
      vbar->drawBar();

    updateTerminal();
    flush_out();
  }
}

//----------------------------------------------------------------------
void FListView::onMouseUp (FMouseEvent* ev)
{
  if ( drag_scroll != fc::noScroll )
  {
    delOwnTimer();
    drag_scroll = fc::noScroll;
    scroll_distance = 1;
    scroll_timer = false;
  }

  if ( ev->getButton() == fc::LeftButton )
  {
    int mouse_x = ev->getX();
    int mouse_y = ev->getY();

    if ( mouse_x > 1 && mouse_x < getWidth()
        && mouse_y > 1 && mouse_y < getHeight() )
    {
      processChanged();
    }
  }
}

//----------------------------------------------------------------------
void FListView::onMouseMove (FMouseEvent* ev)
{
  int yoffset_before, mouse_x, mouse_y;

  if ( ev->getButton() != fc::LeftButton
      && ev->getButton() != fc::RightButton )
  {
    return;
  }

  if ( ev->getButton() == fc::RightButton )
    return;

  yoffset_before = yoffset;
  mouse_x = ev->getX();
  mouse_y = ev->getY();

  if ( mouse_x > 1 && mouse_x < getWidth()
      && mouse_y > 1 && mouse_y < getHeight() )
  {
    current = yoffset + mouse_y - 1;

    if ( current > int(getCount()) )
      current = int(getCount());

    if ( isVisible() )
      drawList();

    vbar->setValue (yoffset);

    if ( vbar->isVisible() && yoffset_before != yoffset )
      vbar->drawBar();

    updateTerminal();
    flush_out();
  }

  // auto-scrolling when dragging mouse outside the widget
  if ( mouse_y < 2 )
  {
    // drag up
    if ( drag_scroll != fc::noScroll
        && scroll_distance < getClientHeight() )
      scroll_distance++;

    if ( ! scroll_timer && current > 1 )
    {
      scroll_timer = true;
      addTimer(scroll_repeat);

      if ( ev->getButton() == fc::RightButton )
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
  else if ( mouse_y >= getHeight() )
  {
    // drag down
    if ( drag_scroll != fc::noScroll
        && scroll_distance < getClientHeight() )
      scroll_distance++;

    if ( ! scroll_timer && current < int(getCount()) )
    {
      scroll_timer = true;
      addTimer(scroll_repeat);

      if ( ev->getButton() == fc::RightButton )
        drag_scroll = fc::scrollDownSelect;
      else
        drag_scroll = fc::scrollDown;
    }

    if ( current == int(getCount()) )
    {
      delOwnTimer();
      drag_scroll = fc::noScroll;
    }
  }
  else
  {
    // no dragging
    delOwnTimer();
    scroll_timer = false;
    scroll_distance = 1;
    drag_scroll = fc::noScroll;
  }
}

//----------------------------------------------------------------------
void FListView::onMouseDoubleClick (FMouseEvent* ev)
{
  int mouse_x, mouse_y;

  if ( ev->getButton() != fc::LeftButton )
    return;

  mouse_x = ev->getX();
  mouse_y = ev->getY();

  if ( mouse_x > 1 && mouse_x < getWidth()
      && mouse_y > 1 && mouse_y < getHeight() )
  {
    if ( yoffset + mouse_y - 1 > int(getCount()) )
      return;

    processClick();
  }
}

//----------------------------------------------------------------------
void FListView::onTimer (FTimerEvent*)
{
  int element_count = int(getCount());
  int current_before = current;
  int yoffset_before = yoffset;
  int yoffset_end = element_count - getClientHeight();

  switch ( int(drag_scroll) )
  {
    case fc::noScroll:
      return;

    case fc::scrollUp:
    case fc::scrollUpSelect:
      if ( current_before == 1)
      {
        drag_scroll = fc::noScroll;
        return;
      }

      current -= scroll_distance;

      if ( current < 1 )
        current = 1;

      if ( current <= yoffset )
        yoffset -= scroll_distance;

      if ( yoffset < 0 )
        yoffset = 0;
      break;

    case fc::scrollDown:
    case fc::scrollDownSelect:
      if ( current_before == element_count )
      {
        drag_scroll = fc::noScroll;
        return;
      }

      current += scroll_distance;

      if ( current > element_count )
        current = element_count;

      if ( current - yoffset > getClientHeight() )
        yoffset += scroll_distance;

      if ( yoffset > yoffset_end )
        yoffset = yoffset_end;

      break;

    default:
      break;
  }

  if ( isVisible() )
    drawList();

  vbar->setValue (yoffset);

  if ( vbar->isVisible() && yoffset_before != yoffset )
    vbar->drawBar();

  updateTerminal();
  flush_out();
}

//----------------------------------------------------------------------
void FListView::onWheel (FWheelEvent* ev)
{
  int element_count, current_before, yoffset_before, yoffset_end, wheel;
  element_count = int(getCount());
  current_before = current;
  yoffset_before = yoffset;
  yoffset_end = element_count - getClientHeight();

  if ( yoffset_end < 0 )
    yoffset_end = 0;

  wheel = ev->getWheel();

  if ( drag_scroll != fc::noScroll )
  {
    delOwnTimer();
    scroll_timer = false;
    scroll_distance = 1;
    drag_scroll = fc::noScroll;
  }

  switch ( wheel )
  {
    case fc::WheelUp:
      if ( yoffset == 0 )
        break;

      yoffset -= 4;

      if ( yoffset < 0 )
      {
        current -= 4 + yoffset;
        yoffset=0;
      }
      else
        current -= 4;

      if ( current < 1 )
        current=1;

      break;

    case fc::WheelDown:
      if ( yoffset == yoffset_end )
        break;

      yoffset += 4;

      if ( yoffset > yoffset_end )
      {
        current += 4 - (yoffset - yoffset_end);
        yoffset = yoffset_end;
      }
      else
        current += 4;

      if ( current > element_count )
        current = element_count;

      break;

    default:
      break;
  }

  if ( current_before != current )
    processChanged();

  if ( isVisible() )
    drawList();

  vbar->setValue (yoffset);

  if ( vbar->isVisible() && yoffset_before != yoffset )
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
void FListView::adjustYOffset()
{
  int element_count = int(getCount());

  if ( yoffset > element_count - getClientHeight() )
    yoffset = element_count - getClientHeight();

  if ( yoffset < 0 )
    yoffset = 0;

  if ( current < yoffset )
    current = yoffset;

  if ( yoffset < current - getClientHeight() )
    yoffset = current - getClientHeight();
}

//----------------------------------------------------------------------
void FListView::adjustSize()
{
  int element_count;
  FWidget::adjustSize();
  adjustYOffset();

  element_count = int(getCount());
  vbar->setMaximum (element_count - getClientHeight());
  vbar->setPageSize (element_count, getClientHeight());
  vbar->setX (getWidth());
  vbar->setHeight (getClientHeight(), false);
  vbar->resize();

  hbar->setMaximum (max_line_width - getClientWidth());
  hbar->setPageSize (max_line_width, getClientWidth());
  hbar->setY (getHeight() );
  hbar->setWidth (getClientWidth(), false);
  hbar->resize();

  if ( element_count <= getClientHeight() )
    vbar->hide();
  else
    vbar->setVisible();

  if ( max_line_width < getClientWidth() - 1 )
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
    std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
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
uInt FListView::getAlignOffset ( fc::text_alignment align
                               , uInt txt_length
                               , uInt width )
{
  switch ( align )
  {
    case fc::alignLeft:
      return 0;

    case fc::alignCenter:
      if ( txt_length < width )
        return uInt((width - txt_length) / 2);
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
  bool isFocus;

  if ( current < 1 )
    current = 1;

  setColor();

  if ( isMonochron() )
    setReverse(true);

  if ( isNewFont() )
    drawBorder (1, 1, getWidth() - 1, getHeight());
  else
    drawBorder();

  if ( isNewFont() && ! vbar->isVisible() )
  {
    setColor();

    for (int y = 2; y < getHeight(); y++)
    {
      setPrintPos (getWidth(), y);
      print (' ');  // clear right side of the scrollbar
    }
  }

  drawColumnLabels();

  if ( isMonochron() )
    setReverse(false);

  if ( vbar->isVisible() )
    vbar->redraw();

  if ( hbar->isVisible() )
    hbar->redraw();

  drawList();
  isFocus = ((flags & fc::focus) != 0);

  if ( isFocus && getStatusBar() )
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
void FListView::drawColumnLabels()
{
  static const int leading_space = 1;
  static const int tailing_space = 1;
  static const int ellipsis_length = 2;
  std::vector<char_data>::const_iterator first, last;
  headerItems::const_iterator iter;
  FString txt;
  uInt txt_length;

  if ( header.empty()
      || getHeight() <= 2
      || getWidth() <= 4
      || max_line_width < 1 )
    return;

  iter = header.begin();
  headerline.clear();

  while ( iter != header.end() )
  {
    const FString& text = (*iter).name;
    int width = (*iter).width;
    int column_width;

    if ( text.isNull() || text.isEmpty() )
    {
      ++iter;
      continue;
    }

    txt = " " + text;
    txt_length = txt.getLength();
    column_width = leading_space + width;

    if ( isEnabled() )
      setColor (wc.label_emphasis_fg, wc.label_bg);
    else
      setColor (wc.label_inactive_fg, wc.label_inactive_bg);

    if ( txt_length <= uInt(column_width) )
    {
      headerline << txt;

      if ( txt_length < uInt(column_width) )
        headerline << ' ';  // tailing space

      if ( txt_length + tailing_space < uInt(column_width) )
      {
        setColor();
        const FString line ( uInt(column_width) - tailing_space - txt_length
                           , wchar_t(fc::BoxDrawingsHorizontal) );
        headerline << line;  // horizontal line
      }
    }
    else
    {
      headerline << ' ';
      headerline << text.left(uInt(width - ellipsis_length));
      setColor (wc.label_ellipsis_fg, wc.label_bg);
      headerline << "..";

      if ( iter == header.end() - 1 )  // Last element
        headerline << ' ';
    }

    ++iter;
  }

  std::vector<char_data> h;
  h << headerline;
  first = h.begin() + xoffset;

  if ( int(h.size()) <= getClientWidth() )
    last = h.end() - 1;
  else
  {
    int len = getClientWidth() + xoffset - 1;

    if ( len > int(h.size()) )
      len = int(h.size());

    last = h.begin() + len;
  }

  setPrintPos (2, 1);
  print() << std::vector<char_data>(first, last);
}

//----------------------------------------------------------------------
void FListView::drawList()
{
  uInt page_height, y;
  bool is_focus;
  FObjectIterator iter;

  if ( itemlist.empty() || getHeight() <= 2 || getWidth() <= 4 )
    return;

  y           = 0;
  page_height = uInt(getHeight() - 2);
  is_focus    = ((flags & fc::focus) != 0);
  iter        = index2iterator(yoffset);

  while ( iter != itemlist.end() && y < page_height )
  {
    bool is_current_line = bool( y + uInt(yoffset) + 1 == uInt(current) );
    const FListViewItem* item = static_cast<FListViewItem*>(*iter);
    setPrintPos (2, 2 + int(y));

    // Draw one FListViewItem
    drawListLine (item, is_focus, is_current_line);

    if ( is_focus && is_current_line )
      setCursorPos (3, 2 + int(y));  // first character

    y++;
    nextElement(iter);
  }
}

//----------------------------------------------------------------------
void FListView::drawListLine ( const FListViewItem* item
                             , bool is_focus
                             , bool is_current )
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

  // print the entry
  FString line;

  if ( tree_view )
  {
    if ( item->expandable  )
    {
      line = wchar_t(fc::BlackRightPointingPointer);
      line += L' ';
    }
    else
      line = L"  ";
  }
  else
    line = L" ";

  // print columns
  if ( ! item->column_list.empty() )
  {
    for (uInt i = 0; i < item->column_list.size(); )
    {
      static const int leading_space = 1;
      static const int ellipsis_length = 2;

      const FString& text = item->column_list[i];
      int width = header[i].width;
      uInt txt_length = text.getLength();
      // Increment the value of i for the column position
      // and the next iteration
      i++;
      fc::text_alignment align = getColumnAlignment(int(i));
      uInt align_offset = getAlignOffset (align, txt_length, uInt(width));

      if ( tree_view && i == 1 )
      {
        width--;
      }

      // Insert alignment spaces
      if ( align_offset > 0 )
        line += FString(align_offset, L' ');

      if ( align_offset + txt_length <= uInt(width) )
      {
        // Insert text and tailing space
        line += text.left(width);
        line += FString ( leading_space + width
                         - int(align_offset + txt_length), L' ');
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

  line = line.mid ( uInt(1 + xoffset)
                  , uInt(getWidth() - nf_offset - 2) );
  const wchar_t* const& element_str = line.wc_str();
  uInt len = line.getLength();
  uInt i;

  for (i = 0; i < len; i++)
    *this << element_str[i];

  for (; i < uInt(getWidth() - nf_offset - 2); i++)
    print (' ');
}

//----------------------------------------------------------------------
void FListView::recalculateHorizontalBar (int len)
{
  if ( len <= max_line_width )
    return;

  max_line_width = len;

  if ( len >= getWidth() - nf_offset - 3 )
  {
    hbar->setMaximum (max_line_width - getWidth() + nf_offset + 4);
    hbar->setPageSize (max_line_width, getWidth() - nf_offset - 4);
    hbar->calculateSliderValues();

    if ( ! hbar->isVisible() )
      hbar->setVisible();
  }
}

//----------------------------------------------------------------------
void FListView::recalculateVerticalBar (int element_count)
{
  vbar->setMaximum (element_count - getHeight() + 2);
  vbar->setPageSize (element_count, getHeight() - 2);
  vbar->calculateSliderValues();

  if ( ! vbar->isVisible() && element_count >= getHeight() - 1 )
    vbar->setVisible();
}

//----------------------------------------------------------------------
FObject::FObjectIterator FListView::appendItem (FListViewItem* item)
{
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
void FListView::cb_VBarChange (FWidget*, data_ptr)
{
  FScrollbar::sType scrollType;
  int distance = 1;
  int element_count = int(getCount());
  int yoffset_before = yoffset;
  int yoffset_end = element_count - getClientHeight();
  scrollType = vbar->getScrollType();

  switch ( scrollType )
  {
    case FScrollbar::noScroll:
      break;

    case FScrollbar::scrollPageBackward:
      distance = getClientHeight();
      // fall through
    case FScrollbar::scrollStepBackward:
      current -= distance;

      if ( current < 1 )
        current=1;

      if ( current <= yoffset )
        yoffset -= distance;

      if ( yoffset < 0 )
        yoffset = 0;

      break;

    case FScrollbar::scrollPageForward:
      distance = getClientHeight();
      // fall through
    case FScrollbar::scrollStepForward:
      current += distance;

      if ( current > element_count )
        current = element_count;

      if ( current - yoffset > getClientHeight() )
        yoffset += distance;

      if ( yoffset > yoffset_end )
        yoffset = yoffset_end;

      break;

    case FScrollbar::scrollJump:
    {
      int val = vbar->getValue();

      if ( yoffset == val )
        break;

      int c = current - yoffset;
      yoffset = val;

      if ( yoffset > yoffset_end )
        yoffset = yoffset_end;

      if ( yoffset < 0 )
        yoffset = 0;

      current = yoffset + c;

      if ( current < yoffset )
        current = yoffset;

      if ( current > element_count )
        current = element_count;

      break;
    }

    case FScrollbar::scrollWheelUp:
    {
      FWheelEvent wheel_ev (fc::MouseWheel_Event, FPoint(2,2), fc::WheelUp);
      onWheel(&wheel_ev);
    }
    break;

    case FScrollbar::scrollWheelDown:
    {
      FWheelEvent wheel_ev (fc::MouseWheel_Event, FPoint(2,2), fc::WheelDown);
      onWheel(&wheel_ev);
    }
    break;
  }

  if ( isVisible() )
    drawList();

  if ( scrollType >= FScrollbar::scrollStepBackward
      && scrollType <= FScrollbar::scrollPageForward )
  {
    vbar->setValue (yoffset);

    if ( vbar->isVisible() && yoffset_before != yoffset )
      vbar->drawBar();

    updateTerminal();
    flush_out();
  }
}

//----------------------------------------------------------------------
void FListView::cb_HBarChange (FWidget*, data_ptr)
{
  FScrollbar::sType scrollType;
  int distance = 1;
  int xoffset_before = xoffset;
  int xoffset_end = max_line_width - getClientWidth();
  scrollType = hbar->getScrollType();

  switch ( scrollType )
  {
    case FScrollbar::noScroll:
      break;

    case FScrollbar::scrollPageBackward:
      distance = getClientWidth();
      // fall through
    case FScrollbar::scrollStepBackward:
      xoffset -= distance;

      if ( xoffset < 0 )
        xoffset = 0;
      break;

    case FScrollbar::scrollPageForward:
      distance = getClientWidth();
      // fall through
    case FScrollbar::scrollStepForward:
      xoffset += distance;

      if ( xoffset > xoffset_end )
        xoffset = xoffset_end;

      if ( xoffset < 0 )
        xoffset = 0;

      break;

    case FScrollbar::scrollJump:
    {
      int val = hbar->getValue();

      if ( xoffset == val )
        break;

      xoffset = val;

      if ( xoffset > xoffset_end )
        xoffset = xoffset_end;

      if ( xoffset < 0 )
        xoffset = 0;

      break;
    }

    case FScrollbar::scrollWheelUp:
      if ( xoffset == 0 )
        break;

      xoffset -= 4;

      if ( xoffset < 0 )
        xoffset = 0;

      break;

    case FScrollbar::scrollWheelDown:
      if ( xoffset == xoffset_end )
        break;

      xoffset += 4;

      if ( xoffset > xoffset_end )
        xoffset = xoffset_end;

      break;
  }

  if ( isVisible() )
  {
    drawColumnLabels();
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
