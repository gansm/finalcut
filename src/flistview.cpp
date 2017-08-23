// File: flistview.cpp
// Provides: class FListViewItem
//           class FListView

#include "fapplication.h"
#include "flistview.h"
#include "fscrollbar.h"
#include "fstatusbar.h"
#include "ftermbuffer.h"

//----------------------------------------------------------------------
// class FListViewItem
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FListViewItem::FListViewItem (const FListViewItem& item)
  : FObject(item.getParent())
  , column_line(item.column_line)
  , data_pointer(item.data_pointer)
  , visible_lines(1)
  , expandable(false)
  , is_expand(false)
{
  FObject* parent = getParent();

  if ( parent && parent->isInstanceOf("FListView") )
    static_cast<FListView*>(parent)->insert (this);
}

//----------------------------------------------------------------------
FListViewItem::FListViewItem (FListViewItem* parent)
  : FObject(parent)
  , column_line()
  , data_pointer(0)
  , visible_lines(1)
  , expandable(false)
  , is_expand(false)
{
  // Add the FListViewItem to the parent
  if ( ! parent )
    return;

  parent->addChild (this);
  parent->expandable = true;
}

//----------------------------------------------------------------------
FListViewItem::FListViewItem (FListView* parent)
  : FObject(parent)
  , column_line()
  , data_pointer(0)
  , visible_lines(1)
  , expandable(false)
  , is_expand(false)
{
  // Add the FListViewItem to the parent
  if ( parent )
    parent->insert (this);
}

//----------------------------------------------------------------------
FListViewItem::FListViewItem ( const std::vector<FString>& cols
                             , FWidget::data_ptr data
                             , FListView* parent )
  : FObject(parent)
  , column_line(cols)
  , data_pointer(data)
  , visible_lines(1)
  , expandable(false)
  , is_expand(false)
{
  // Replace the control codes characters
  std::vector<FString>::iterator iter = column_line.begin();

  while ( iter != column_line.end() )
  {
    *iter = iter->replaceControlCodes();
    ++iter;
  }

  // Add the FListViewItem to the parent
  if ( parent )
    parent->insert (this);
}

//----------------------------------------------------------------------
FListViewItem::FListViewItem ( const std::vector<FString>& cols
                             , FWidget::data_ptr data
                             , FListViewItem* parent )
  : FObject(parent)
  , column_line(cols)
  , data_pointer(data)
  , visible_lines(1)
  , expandable(false)
  , is_expand(false)
{
  // Replace the control codes characters
  std::vector<FString>::iterator iter = column_line.begin();

  while ( iter != column_line.end() )
  {
    *iter = iter->replaceControlCodes();
    ++iter;
  }

  if ( parent )
    parent->expandable = true;
}

//----------------------------------------------------------------------
FListViewItem::~FListViewItem()
{ }


// public methods of FListViewItem
//----------------------------------------------------------------------
FString FListViewItem::getText (int column) const
{
  if ( column < 0
     || column_line.empty()
     || column >= int(column_line.size()) )
    return *fc::empty_string;

  return column_line[uInt(column)];
}

//----------------------------------------------------------------------
void FListViewItem::setText (int column, const FString& text)
{
  if ( column < 0
     || column_line.empty()
     || column >= int(column_line.size()) )
    return;

  FObject* parent = getParent();

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

  column_line[uInt(column)] = text;
}

//----------------------------------------------------------------------
void FListViewItem::insert (FListViewItem* child)
{
   // Add a FListViewItem as child element
   if ( ! child || ! hasChildren() )
     return;

  addChild (child);
  expandable = true;
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

  FObjectList children = this->getChildren();
  FObjectList::const_iterator iter = children.begin();

  while ( iter != children.end() )
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
  , data()
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

  if ( column < 0 || header.empty() || column >= int(header.size()) )
    return fc::alignLeft;

  return header[uInt(column)].alignment;
}

//----------------------------------------------------------------------
FString FListView::getColumnText (int column) const
{
  // Get the text of column

  if ( column < 0 || header.empty() || column >= int(header.size()) )
    return *fc::empty_string;

  return header[uInt(column)].name;
}

//----------------------------------------------------------------------
void FListView::setGeometry (int x, int y, int w, int h, bool adjust)
{
  // Set the widget geometry

  FWidget::setGeometry(x, y, w, h, adjust);

  if ( isNewFont() )
  {
    vbar->setGeometry (getWidth(), 2, 2, getHeight()-2);
    hbar->setGeometry (1, getHeight(), getWidth()-2, 1);
  }
  else
  {
    vbar->setGeometry (getWidth(), 2, 1, getHeight()-2);
    hbar->setGeometry (2, getHeight(), getWidth()-2, 1);
  }
}

//----------------------------------------------------------------------
void FListView::setColumnAlignment (int column, fc::text_alignment align)
{
  // Set the alignment for a column

  if ( column < 0 || header.empty() || column > int(header.size()) )
    return;

  header[uInt(column)].alignment = align;
}

//----------------------------------------------------------------------
void FListView::setColumnText (int column, const FString& label)
{
  // Set the text for a column

  if ( column < 0 || header.empty() || column > int(header.size()) )
    return;

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
  Header column;
  column.name = label;
  column.width = width;

  if ( column.width == USE_MAX_SIZE )
  {
    column.fixed_width = false;
    column.width = int(label.getLength());
  }
  else
    column.fixed_width = true;

  header.push_back (column);
  return int(std::distance(header.begin(), header.end()));
}

//----------------------------------------------------------------------
void FListView::insert (FListViewItem* item)
{
  static const int padding_space = 2; // 1 leading space + 1 tailing space
  int  line_width = padding_space;
  uInt column  = 1;
  uInt entries = uInt(item->column_line.size());
  headerItems::iterator iter;

  iter = header.begin();

  while ( iter != header.end() )
  {
    int width = (*iter).width;
    bool fixed_width = (*iter).fixed_width;
    FString text = (*iter).name;

    if ( ! fixed_width )
    {
      int len;

      if ( column <= entries )
        len = int(item->column_line[column - 1].getLength());
      else
        len = 0;

      if ( len > width )
        (*iter).width = len;
    }

    line_width += (*iter).width;
    column++;
    ++iter;
  }

  recalculateHorizontalBar (line_width);

  data.push_back (item);

  int element_count = int(data.size());
  recalculateVerticalBar (element_count);
}

//----------------------------------------------------------------------
void FListView::insert ( const std::vector<FString>& cols
                       , data_ptr d
                       , FListView* parent )
{
  try
  {
    if ( parent == 0 )
      new FListViewItem (cols, d, this);
    else
      new FListViewItem (cols, d, parent);
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
    return;
  }
}

//----------------------------------------------------------------------
void FListView::insert ( const std::vector<long>& cols
                       , data_ptr d
                       , FListView* parent )
{
  std::vector<FString> str_cols;

  if ( ! cols.empty() )
  {
    for (uInt i=0; i < cols.size(); i++)
      str_cols.push_back (FString().setNumber(cols[i]));
  }

  if ( parent == 0 )
    insert (str_cols, d, this);
  else
    insert (str_cols, d, parent);
}

//----------------------------------------------------------------------
void FListView::onKeyPress (FKeyEvent* ev)
{
  static const int padding_space = 2; // 1 leading space + 1 tailing space
  int element_count = int(data.size());
  int current_before = current;
  int xoffset_before = xoffset;
  int xoffset_end = max_line_width - getClientWidth() + padding_space + 2;
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
        current=1;

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
        current=1;

      if ( current <= yoffset )
      {
        yoffset -= getClientHeight() - 1;

        if ( yoffset < 0 )
          yoffset=0;
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

    if ( current > int(data.size()) )
      current = int(data.size());

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

    if ( current > int(data.size()) )
      current = int(data.size());

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

    if ( ! scroll_timer && current < int(data.size()) )
    {
      scroll_timer = true;
      addTimer(scroll_repeat);

      if ( ev->getButton() == fc::RightButton )
        drag_scroll = fc::scrollDownSelect;
      else
        drag_scroll = fc::scrollDown;
    }

    if ( current == int(data.size()) )
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
    if ( yoffset + mouse_y - 1 > int(data.size()) )
      return;

    processClick();
  }
}

//----------------------------------------------------------------------
void FListView::onTimer (FTimerEvent*)
{
  int element_count = int(data.size());
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
        current=1;

      if ( current <= yoffset )
        yoffset -= scroll_distance;

      if ( yoffset < 0 )
        yoffset=0;
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
  element_count = int(data.size());
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
        current -= 4+yoffset;
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
  int element_count = int(data.size());

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

  element_count = int(data.size());
  vbar->setMaximum(element_count - getClientHeight());
  vbar->setPageSize(element_count, getClientHeight());
  vbar->setX(getWidth());
  vbar->setHeight (getClientHeight(), false);
  vbar->resize();

  hbar->setMaximum(max_line_width - getClientWidth() + 2);
  hbar->setPageSize(max_line_width, getClientWidth() - 2);
  hbar->setY(getHeight() );
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

    for (int y=2; y < getHeight(); y++)
    {
      setPrintPos (getWidth(),y);
      print (' '); // clear right side of the scrollbar
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
    FString text = (*iter).name;
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
        FString line ( uInt(column_width) - tailing_space - txt_length
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
    last = h.begin() + getClientWidth() + xoffset - 1;

  setPrintPos (2, 1);
  print() << std::vector<char_data>(first, last);
}

//----------------------------------------------------------------------
void FListView::drawList()
{
  uInt start, end;
  bool isFocus;
  listViewItems::const_iterator iter;

  if ( data.empty() || getHeight() <= 2 || getWidth() <= 4 )
    return;

  isFocus = ((flags & fc::focus) != 0);
  start   = 0;
  end     = uInt(getHeight() - 2);

  if ( end > data.size() )
    end = uInt(data.size());

  iter = index2iterator(int(start) + yoffset);

  for (uInt y=start; y < end; y++)
  {
    bool isCurrentLine = bool(y + uInt(yoffset) + 1 == uInt(current));
    setPrintPos (2, 2 + int(y));
    setColor (wc.list_fg, wc.list_bg);

    if ( isCurrentLine )
    {
      if ( isFocus && getMaxColor() < 16 )
        setBold();

      if ( isMonochron() )
        unsetBold();

      if ( isFocus )
      {
        setColor ( wc.current_element_focus_fg
                 , wc.current_element_focus_bg );
        setCursorPos (3, 2 + int(y)); // first character
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
      else if ( isFocus && getMaxColor() < 16 )
        unsetBold();
    }

    // print the entry
    FString line = " ";

    // print columns
    if ( ! (*iter)->column_line.empty() )
    {
      for (uInt i=0; i < (*iter)->column_line.size(); i++)
      {
        static const int leading_space = 1;
        static const int ellipsis_length = 2;
        FString text = (*iter)->column_line[i];
        int width = header[i].width;
        uInt txt_length = text.getLength();
        fc::text_alignment align = getColumnAlignment(int(i+1));
        uInt align_offset = getAlignOffset (align, txt_length, uInt(width));

        if ( align_offset > 0 )
          line += FString(align_offset, ' ');

        if ( align_offset + txt_length <= uInt(width) )
        {
          line += text.left(width);
          line += FString (leading_space + width - int(align_offset + txt_length), ' ');
        }
        else if ( align == fc::alignRight )
        {
          line += FString ("..");
          line += text.right(width - ellipsis_length);
          line += ' ';
        }
        else
        {
          line += text.left(width - ellipsis_length);
          line += FString (".. ");
        }
      }
    }

    line = line.mid ( uInt(1 + xoffset)
                    , uInt(getWidth() - nf_offset - 2) );
    const wchar_t* const& element_str = line.wc_str();
    uInt len = line.getLength();
    uInt i;

    for (i=0; i < len; i++)
      print (element_str[i]);

    for (; i < uInt(getWidth() - nf_offset - 2); i++)
      print (' ');

    ++iter;
  }
}

//----------------------------------------------------------------------
void FListView::recalculateHorizontalBar (int len)
{
  if ( len <= max_line_width )
    return;

  max_line_width = len;

  if ( len >= getWidth() - nf_offset - 3 )
  {
    hbar->setMaximum(max_line_width - getWidth() + nf_offset + 4);
    hbar->setPageSize(max_line_width, getWidth() - nf_offset - 4);
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
  int element_count = int(data.size());
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
  static const int padding_space = 2; // 1 leading space + 1 tailing space
  FScrollbar::sType scrollType;
  int distance = 1;
  int xoffset_before = xoffset;
  int xoffset_end = max_line_width - getClientWidth() + padding_space + 2;
  scrollType = hbar->getScrollType();

  switch ( scrollType )
  {
    case FScrollbar::noScroll:
      break;

    case FScrollbar::scrollPageBackward:
      distance = getClientWidth() - padding_space;
      // fall through
    case FScrollbar::scrollStepBackward:
      xoffset -= distance;

      if ( xoffset < 0 )
        xoffset = 0;
      break;

    case FScrollbar::scrollPageForward:
      distance = getClientWidth() - padding_space;
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
        xoffset=0;

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
