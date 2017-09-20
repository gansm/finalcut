// File: flistbox.cpp
// Provides: class FListBoxItem
//           class FListBox

#include <algorithm>

#include "final/fapplication.h"
#include "final/flistbox.h"
#include "final/fscrollbar.h"
#include "final/fstatusbar.h"


//----------------------------------------------------------------------
// class FListBoxItem
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FListBoxItem::FListBoxItem()
  : text()
  , data_pointer(0)
  , brackets(fc::NoBrackets)
  , selected(false)
{ }

//----------------------------------------------------------------------
FListBoxItem::FListBoxItem (const FListBoxItem& item)
  : text(item.text)
  , data_pointer(item.data_pointer)
  , brackets(item.brackets)
  , selected(item.selected)
{ }

//----------------------------------------------------------------------
FListBoxItem::FListBoxItem (const FString& txt, FWidget::data_ptr data)
  : text(txt)
  , data_pointer(data)
  , brackets(fc::NoBrackets)
  , selected(false)
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
  , convertToItem(0)
  , itemlist()
  , source_container(0)
  , conv_type(FListBox::no_convert)
  , vbar(0)
  , hbar(0)
  , text()
  , inc_search()
  , multi_select(false)
  , mouse_select(false)
  , drag_scroll(fc::noScroll)
  , scroll_timer(false)
  , scroll_repeat(100)
  , scroll_distance(1)
  , current(0)
  , last_current(-1)
  , secect_from_item(-1)
  , xoffset(0)
  , yoffset(0)
  , last_yoffset(-1)
  , nf_offset(0)
  , max_line_width(0)
{
  init();
}

//----------------------------------------------------------------------
FListBox::~FListBox()  // destructor
{
  delOwnTimer();
  delete vbar;
  delete hbar;
}


// public methods of FListBox
//----------------------------------------------------------------------
void FListBox::setCurrentItem (int index)
{
  int element_count;

  if ( index == current )
    return;

  element_count = int(getCount());

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

  if ( isVisible() )
    redraw();
}

//----------------------------------------------------------------------
void FListBox::setCurrentItem (listBoxItems::iterator iter)
{
  int index = int(std::distance(itemlist.begin(), iter) + 1);
  setCurrentItem(index);
}

//----------------------------------------------------------------------
void FListBox::showInsideBrackets ( int index
                                  , fc::brackets_type b )
{
  listBoxItems::iterator iter = index2iterator(index - 1);
  iter->brackets = b;

  if ( b == fc::NoBrackets )
    return;

  int len = int(iter->getText().getLength() + 2);

  if ( len > max_line_width )
  {
    max_line_width = len;

    if ( len >= getWidth() - nf_offset - 3 )
    {
      hbar->setMaximum (max_line_width - getWidth() + nf_offset + 4);
      hbar->setPageSize (max_line_width, getWidth() - nf_offset - 4);
      hbar->setValue (xoffset);

      if ( ! hbar->isVisible() )
        hbar->setVisible();
    }
  }
}

//----------------------------------------------------------------------
void FListBox::setGeometry (int x, int y, int w, int h, bool adjust)
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
bool FListBox::setFocus (bool on)
{
  FWidget::setFocus(on);

  if ( on )
  {
    if ( getStatusBar() )
    {
      const FString& msg = getStatusbarMessage();
      const FString& curMsg = getStatusBar()->getMessage();

      if ( curMsg != msg )
        getStatusBar()->setMessage(msg);
    }
  }
  else
  {
    if ( getStatusBar() )
      getStatusBar()->clearMessage();
  }

  return on;
}

//----------------------------------------------------------------------
void FListBox::setText (const FString& txt)
{
  text = txt;
}

//----------------------------------------------------------------------
void FListBox::hide()
{
  int n, size;
  short fg, bg;
  char* blank;
  FWidget* parent_widget = getParentWidget();

  FWidget::hide();

  if ( parent_widget )
  {
    fg = parent_widget->getForegroundColor();
    bg = parent_widget->getBackgroundColor();
  }
  else
  {
    fg = wc.dialog_fg;
    bg = wc.dialog_bg;
  }

  setColor (fg, bg);
  n = isNewFont() ? 1 : 0;
  size = getWidth() + n;

  if ( size < 0 )
    return;

  try
  {
    blank = new char[size + 1];
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
    return;
  }

  std::memset (blank, ' ', uLong(size));
  blank[size] = '\0';

  for (int y = 0; y < getHeight(); y++)
  {
    setPrintPos (1, 1 + y);
    print (blank);
  }

  delete[] blank;
}

//----------------------------------------------------------------------
void FListBox::insert (FListBoxItem listItem)
{
  int len = int(listItem.text.getLength());
  bool has_brackets = bool(listItem.brackets);
  recalculateHorizontalBar (len, has_brackets);

  itemlist.push_back (listItem);

  int element_count = int(getCount());
  recalculateVerticalBar (element_count);
}

//----------------------------------------------------------------------
void FListBox::insert ( const FString& item
                      , fc::brackets_type b
                      , bool s
                      , data_ptr d )
{
  FListBoxItem listItem (item, d);
  listItem.brackets = b;
  listItem.selected = s;
  insert (listItem);
}

//----------------------------------------------------------------------
void FListBox::insert ( long item
                      , fc::brackets_type b
                      , bool s
                      , data_ptr d )
{
  insert (FString() << item, b, s, d);
}

//----------------------------------------------------------------------
void FListBox::remove (int item)
{
  int element_count;

  if ( int(getCount()) < item )
    return;

  itemlist.erase (itemlist.begin() + item - 1);
  element_count = int(getCount());
  max_line_width = 0;

  listBoxItems::iterator iter = itemlist.begin();

  while ( iter != itemlist.end() )
  {
    int len = int(iter->getText().getLength());

    if ( len > max_line_width )
      max_line_width = len;

    ++iter;
  }

  hbar->setMaximum (max_line_width - getWidth() + nf_offset + 4);
  hbar->setPageSize (max_line_width, getWidth() - nf_offset - 4);

  if ( hbar->isVisible() && max_line_width < getWidth() - nf_offset - 3 )
    hbar->hide();

  vbar->setMaximum (element_count - getHeight() + 2);
  vbar->setPageSize (element_count, getHeight() - 2);

  if ( vbar->isVisible() && element_count < getHeight() - 1 )
    vbar->hide();

  if ( current >= item && current > 1 )
    current--;

  if ( current > element_count )
    current = element_count;

  if ( yoffset > element_count - getHeight() + 2 )
    yoffset = element_count - getHeight() + 2;

  if ( yoffset < 0 )
    yoffset = 0;
}

//----------------------------------------------------------------------
void FListBox::clear()
{
  int size;
  char* blank;

  itemlist.clear();

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
  setColor (wc.list_fg, wc.list_bg);
  size = getWidth() - 2;

  if ( size < 0 )
    return;

  try
  {
    blank = new char[size + 1];
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << "not enough memory to alloc " << ex.what() << std::endl;
    return;
  }

  std::memset (blank, ' ', uLong(size));
  blank[size] = '\0';

  for (int y = 0; y < getHeight() - 2; y++)
  {
    setPrintPos (2, 2 + y);
    print (blank);
  }

  delete[] blank;
}

//----------------------------------------------------------------------
void FListBox::onKeyPress (FKeyEvent* ev)
{
  static const int padding_space = 2;  // 1 leading space + 1 tailing space
  int element_count = int(getCount());
  int current_before = current;
  int xoffset_before = xoffset;
  int xoffset_end = max_line_width - getClientWidth() + padding_space;
  int yoffset_before = yoffset;
  int yoffset_end = element_count - getClientHeight();
  int key = ev->key();

  switch ( key )
  {
    case fc::Fkey_return:
    case fc::Fkey_enter:
      processClick();
      inc_search.clear();
      ev->accept();
      break;

    case fc::Fkey_up:
      current--;

      if ( current < 1 )
        current=1;

      if ( current <= yoffset )
        yoffset--;

      inc_search.clear();
      ev->accept();
      break;

    case fc::Fkey_down:
      current++;

      if ( current > element_count )
        current = element_count;

      if ( current - yoffset > getClientHeight() )
        yoffset++;

      inc_search.clear();
      ev->accept();
      break;

    case fc::Fkey_left:
      xoffset--;

      if ( xoffset < 0 )
        xoffset = 0;

      inc_search.clear();
      ev->accept();
      break;

    case fc::Fkey_right:
      xoffset++;

      if ( xoffset > xoffset_end )
        xoffset = xoffset_end;

      if ( xoffset < 0 )
        xoffset = 0;

      inc_search.clear();
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

      inc_search.clear();
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

      inc_search.clear();
      ev->accept();
      break;

    case fc::Fkey_home:
      current = 1;
      yoffset = 0;
      inc_search.clear();
      ev->accept();
      break;

    case fc::Fkey_end:
      current = element_count;

      if ( current > getClientHeight() )
        yoffset = yoffset_end;

      inc_search.clear();
      ev->accept();
      break;

    case fc::Fkey_ic:  // insert key
      if ( isMultiSelection() )
      {
        if ( isSelected(current) )
          unselectItem(current);
        else
          selectItem(current);

        processSelect();
        current++;

        if ( current > element_count )
          current = element_count;

        if ( current - yoffset >= getHeight() - 1 )
          yoffset++;

        ev->accept();
      }

      inc_search.clear();
      break;

    case fc::Fkey_space:
      {
        uInt inc_len = inc_search.getLength();

        if ( inc_len > 0 )
        {
          inc_search += L' ';
          bool inc_found = false;
          listBoxItems::iterator iter = itemlist.begin();

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
            ev->ignore();
          }
          else
            ev->accept();
        }
        else if ( isMultiSelection() )
        {
          if ( isSelected(current) )
            unselectItem(current);
          else
            selectItem(current);

          processSelect();
          inc_search.clear();
          ev->accept();
        }
      }
      break;

    case fc::Fkey_erase:
    case fc::Fkey_backspace:
      {
        uInt inc_len = inc_search.getLength();

        if ( inc_len > 0 )
        {
          inc_search.remove(inc_len - 1, 1);

          if ( inc_len > 1 )
          {
            listBoxItems::iterator iter = itemlist.begin();

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

          ev->accept();
        }
        else
          ev->ignore();
      }
      break;

    case fc::Fkey_escape:
    case fc::Fkey_escape_mintty:
      if ( inc_search.getLength() > 0 )
      {
        inc_search.clear();
        ev->accept();
      }
      break;

    default:
      if ( key > 0x20 && key <= 0x10fff )
      {
        // incremental search
        if ( inc_search.getLength() == 0 )
          inc_search = wchar_t(key);
        else
          inc_search += wchar_t(key);

        uInt inc_len = inc_search.getLength();
        bool inc_found = false;
        listBoxItems::iterator iter = itemlist.begin();

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
            ev->ignore();
          else
            ev->accept();
        }
        else
          ev->accept();
      }
      else
        ev->ignore();
  }

  if ( current_before != current )
  {
    processChanged();

    if ( ! isMultiSelection() )
      processSelect();
  }

  if ( ev->isAccepted() )
  {
    if ( isVisible() )
      drawList();

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
void FListBox::onMouseDown (FMouseEvent* ev)
{
  int yoffset_before, mouse_x, mouse_y;

  if ( ev->getButton() != fc::LeftButton
      && ev->getButton() != fc::RightButton )
  {
    return;
  }

  if ( ev->getButton() == fc::RightButton && ! isMultiSelection() )
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
    inc_search.clear();

    if ( ev->getButton() == fc::RightButton )
    {
      if ( isMultiSelection() )
      {
        if ( isSelected(current) )
        {
          mouse_select = false;
          unselectItem(current);
        }
        else
        {
          mouse_select = true;
          selectItem(current);
        }

        processSelect();
        secect_from_item = current;
      }
    }

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
void FListBox::onMouseUp (FMouseEvent* ev)
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

      if ( ! isMultiSelection() )
        processSelect();
    }
  }
}

//----------------------------------------------------------------------
void FListBox::onMouseMove (FMouseEvent* ev)
{
  int current_before, yoffset_before, mouse_x, mouse_y;

  if ( ev->getButton() != fc::LeftButton
      && ev->getButton() != fc::RightButton )
  {
    return;
  }

  if ( ev->getButton() == fc::RightButton && ! isMultiSelection() )
    return;

  current_before = current;
  yoffset_before = yoffset;
  mouse_x = ev->getX();
  mouse_y = ev->getY();

  if ( mouse_x > 1 && mouse_x < getWidth()
      && mouse_y > 1 && mouse_y < getHeight() )
  {
    current = yoffset + mouse_y - 1;

    if ( current > int(getCount()) )
      current = int(getCount());

    inc_search.clear();

    // handle multiple selections
    if ( ev->getButton() == fc::RightButton
        && isMultiSelection()
        && current_before != current )
    {
      int from, to;

      if ( secect_from_item > current )
      {
        from = current;
        to = secect_from_item - 1;
      }
      else
      {
        from = secect_from_item + 1;
        to = current;
      }
      for (int i = from; i <= to; i++)
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

      secect_from_item = current;
    }

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
void FListBox::onMouseDoubleClick (FMouseEvent* ev)
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
void FListBox::onTimer (FTimerEvent*)
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

  // handle multiple selections
  if ( drag_scroll == fc::scrollUpSelect
      || drag_scroll == fc::scrollDownSelect )
  {
    if ( isMultiSelection() && current_before != current )
    {
      int from, to;

      if ( secect_from_item > current )
      {
        from = current;
        to = secect_from_item - 1;
      }
      else
      {
        from = secect_from_item + 1;
        to = current;
      }

      for (int i=from; i <= to; i++)
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

      secect_from_item = current;
    }
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
void FListBox::onWheel (FWheelEvent* ev)
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

      inc_search.clear();
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

      inc_search.clear();
      break;

    default:
      break;
  }

  if ( current_before != current )
  {
    processChanged();

    if ( ! isMultiSelection() )
      processSelect();
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
void FListBox::adjustYOffset()
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
void FListBox::adjustSize()
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

  hbar->setMaximum (max_line_width - getClientWidth() + 2);
  hbar->setPageSize (max_line_width, getClientWidth() - 2);
  hbar->setY (getHeight());
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


// private methods of FListBox
//----------------------------------------------------------------------
inline FString& FListBox::getString (listBoxItems::iterator iter)
{
  return iter->getText();
}

//----------------------------------------------------------------------
void FListBox::init()
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
    F_METHOD_CALLBACK (this, &FListBox::cb_VBarChange)
  );

  hbar->addCallback
  (
    "change-value",
    F_METHOD_CALLBACK (this, &FListBox::cb_HBarChange)
  );

  nf_offset = isNewFont() ? 1 : 0;
  setTopPadding(1);
  setLeftPadding(1);
  setBottomPadding(1);
  setRightPadding(1 + nf_offset);
}

//----------------------------------------------------------------------
void FListBox::draw()
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

  drawLabel();

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
void FListBox::drawLabel()
{
  FString txt;
  uInt length;

  if ( text.isNull() || text.isEmpty() )
    return;

  txt = " " + text + " ";
  length = txt.getLength();
  setPrintPos (2, 1);

  if ( isEnabled() )
    setColor(wc.label_emphasis_fg, wc.label_bg);
  else
    setColor(wc.label_inactive_fg, wc.label_inactive_bg);

  if ( length <= uInt(getClientWidth()) )
    print (txt);
  else
  {
    print (text.left(uInt(getClientWidth() - 2)));
    setColor (wc.label_ellipsis_fg, wc.label_bg);
    print("..");
  }
}

//----------------------------------------------------------------------
void FListBox::drawList()
{
  FString element;
  uInt start, num, inc_len;
  bool isFocus;
  listBoxItems::iterator iter;

  if ( itemlist.empty() || getHeight() <= 2 || getWidth() <= 4 )
    return;

  isFocus = ((flags & fc::focus) != 0);
  start   = 0;
  num     = uInt(getHeight() - 2);
  inc_len = inc_search.getLength();

  if ( num > getCount() )
    num = getCount();

  if ( last_yoffset >= 0
      && last_yoffset == yoffset
      && last_current != current )
  {
    // speed up: redraw only the changed rows
    uInt last_pos = uInt(current - yoffset) - 1;
    uInt current_pos = uInt(last_current - yoffset) - 1;
    start = std::min(last_pos, current_pos);
    num = std::max(last_pos, current_pos) + 1;
  }

  iter = index2iterator(int(start) + yoffset);

  for (uInt y = start; y < num; y++)
  {
    bool serach_mark = false;
    bool lineHasBrackets = hasBrackets(iter);
    bool isLineSelected = isSelected(iter);
    bool isCurrentLine = bool(y + uInt(yoffset) + 1 == uInt(current));

    if ( conv_type == lazy_convert && iter->getText().isNull() )
    {
      convertToItem (*iter, source_container, int(y) + yoffset);
      int len = int(iter->text.getLength());
      recalculateHorizontalBar (len, lineHasBrackets);

      if ( hbar->isVisible() )
        hbar->redraw();
    }

    setPrintPos (2, 2 + int(y));

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
      if ( isFocus && getMaxColor() < 16 )
        setBold();

      if ( isLineSelected )
      {
        if ( isMonochron() )
          setBold();
        else if ( isFocus )
          setColor ( wc.selected_current_element_focus_fg
                   , wc.selected_current_element_focus_bg );
        else
          setColor ( wc.selected_current_element_fg
                   , wc.selected_current_element_bg );

        setCursorPos (3, 2 + int(y));  // first character
      }
      else
      {
        if ( isMonochron() )
          unsetBold();

        if ( isFocus )
        {
          setColor ( wc.current_element_focus_fg
                   , wc.current_element_focus_bg );
          int b = ( lineHasBrackets ) ? 1: 0;

          if ( inc_len > 0 )  // incremental search
          {
            serach_mark = true;
            // Place the cursor on the last found character
            setCursorPos (2 + b + int(inc_len), 2 + int(y));
          }
          else  // only highlighted
            setCursorPos (3 + b, 2 + int(y));  // first character
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
      else if ( isFocus && getMaxColor() < 16 )
        unsetBold();
    }

    // print the entry
    if ( isMonochron() && isCurrentLine )
      print ('>');
    else
      print (' ');

    if ( lineHasBrackets )
    {
      int full_length;
      uInt len;
      uInt i = 0;
      uInt b = 0;

      if ( xoffset == 0 )
      {
        b=1;

        switch ( iter->brackets )
        {
          case fc::NoBrackets:
            break;

          case fc::SquareBrackets:
            print ('[');
            break;

          case fc::Parenthesis:
            print ('(');
            break;

          case fc::CurlyBrackets:
            print ('{');
            break;

          case fc::AngleBrackets:
            print ('<');
            break;
        }

        element = getString(iter).mid ( uInt(1 + xoffset)
                                      , uInt(getWidth() - nf_offset - 5) );
      }
      else
        element = getString(iter).mid ( uInt(xoffset)
                                      , uInt(getWidth() - nf_offset - 4) );

      const wchar_t* const& element_str = element.wc_str();
      len = element.getLength();

      for (; i < len; i++)
      {
        if ( serach_mark && i == 0 )
          setColor ( wc.current_inc_search_element_fg
                   , wc.current_element_focus_bg );

        if ( serach_mark && i == inc_len )
          setColor ( wc.current_element_focus_fg
                   , wc.current_element_focus_bg );

        print (element_str[i]);
      }

      full_length = int(getString(iter).getLength());

      if ( b + i < uInt(getWidth() - nf_offset - 4)
          && xoffset <= full_length + 1 )
      {
        if ( serach_mark && i == inc_len )
          setColor ( wc.current_element_focus_fg
                   , wc.current_element_focus_bg );

        switch ( iter->brackets )
        {
          case fc::NoBrackets:
            break;

          case fc::SquareBrackets:
            print (']');
            break;

          case fc::Parenthesis:
            print (')');
            break;

          case fc::CurlyBrackets:
            print ('}');
            break;

          case fc::AngleBrackets:
            print ('>');
            break;
        }

        i++;
      }

      if ( isMonochron() && isCurrentLine )
      {
        print ('<');
        i++;
      }

      for (; b + i < uInt(getWidth() - nf_offset - 3); i++)
        print (' ');
    }
    else  // line has no brackets
    {
      uInt i, len;
      element = getString(iter).mid ( uInt(1 + xoffset)
                                    , uInt(getWidth() - nf_offset - 4) );
      const wchar_t* const& element_str = element.wc_str();
      len = element.getLength();

      if ( serach_mark )
        setColor ( wc.current_inc_search_element_fg
                 , wc.current_element_focus_bg );

      for (i = 0; i < len; i++)
      {
        if ( serach_mark && i == inc_len )
          setColor ( wc.current_element_focus_fg
                   , wc.current_element_focus_bg );

        print (element_str[i]);
      }

      if ( isMonochron() && isCurrentLine )
      {
        print ('<');
        i++;
      }

      for (; i < uInt(getWidth() - nf_offset - 3); i++)
        print (' ');
    }

    ++iter;
  }

  if ( isMonochron() )  // unset for the last element
    setReverse(false);

  unsetBold();
  last_yoffset = yoffset;
  last_current = current;
}

//----------------------------------------------------------------------
void FListBox::recalculateHorizontalBar (int len, bool has_brackets)
{
  if ( has_brackets )
    len += 2;

  if ( len > max_line_width )
  {
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
}

//----------------------------------------------------------------------
void FListBox::recalculateVerticalBar (int element_count)
{
  vbar->setMaximum (element_count - getHeight() + 2);
  vbar->setPageSize (element_count, getHeight() - 2);
  vbar->calculateSliderValues();

  if ( ! vbar->isVisible() && element_count >= getHeight() - 1 )
    vbar->setVisible();
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
void FListBox::cb_VBarChange (FWidget*, data_ptr)
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
void FListBox::cb_HBarChange (FWidget*, data_ptr)
{
  static const int padding_space = 2;  // 1 leading space + 1 tailing space
  FScrollbar::sType scrollType;
  int distance = 1;
  int xoffset_before = xoffset;
  int xoffset_end = max_line_width - getClientWidth() + padding_space;
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
