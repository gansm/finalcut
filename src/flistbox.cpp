// File: flistbox.cpp
// Provides: class FListBoxItem
//           class FListBox

#include "fapp.h"
#include "flistbox.h"
#include "fscrollbar.h"
#include "fstatusbar.h"


//----------------------------------------------------------------------
// class FListBoxItem
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FListBoxItem::FListBoxItem()
  : text()
  , brackets(fc::NoBrackets)
  , selected(false)
{ }

//----------------------------------------------------------------------
FListBoxItem::FListBoxItem (FString& txt)
  : text(txt)
  , brackets(fc::NoBrackets)
  , selected(false)
{ }

//----------------------------------------------------------------------
FListBoxItem::FListBoxItem (const std::string& txt)
  : text(txt)
  , brackets(fc::NoBrackets)
  , selected(false)
{ }

//----------------------------------------------------------------------
FListBoxItem::FListBoxItem (const char* txt)
  : text(txt)
  , brackets(fc::NoBrackets)
  , selected(false)
{ }

//----------------------------------------------------------------------
FListBoxItem::~FListBoxItem()
{ }


//----------------------------------------------------------------------
// class FListBox
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FListBox::FListBox(FWidget* parent)
  : FWidget(parent)
  , data()
  , vbar(0)
  , hbar(0)
  , text()
  , inc_search()
  , multi_select(false)
  , mouse_select(false)
  , drag_scroll(FListBox::noScroll)
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
void FListBox::setCurrentItem(int index)
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
void FListBox::showInsideBrackets ( int index
                                  , fc::brackets_type b )
{
  data[uInt(index-1)].brackets = b;

  if ( b == fc::NoBrackets )
    return;

  int len = int(data[uInt(index-1)].getText().getLength() + 2);

  if ( len > max_line_width )
  {
    max_line_width = len;

    if ( len >= getWidth() - nf_offset - 3 )
    {
      hbar->setMaximum(max_line_width - getWidth() + nf_offset + 4);
      hbar->setPageSize(max_line_width, getWidth() - nf_offset - 4);
      hbar->setValue (xoffset);

      if ( ! hbar->isVisible() )
        hbar->setVisible();
    }
  }
}

//----------------------------------------------------------------------
void FListBox::setGeometry (int x, int y, int w, int h, bool adjust)
{
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
bool FListBox::setEnable (bool on)
{
  FWidget::setEnable(on);

  if ( on )
    flags |= fc::active;
  else
    flags &= ~fc::active;

  return on;
}

//----------------------------------------------------------------------
bool FListBox::setFocus (bool on)
{
  FWidget::setFocus(on);

  if ( on )
  {
    flags |= fc::focus;

    if ( getStatusBar() )
    {
      FString msg = getStatusbarMessage();
      FString curMsg = getStatusBar()->getMessage();

      if ( curMsg != msg )
        getStatusBar()->setMessage(msg);
    }
  }
  else
  {
    flags &= ~fc::focus;

    if ( getStatusBar() )
      getStatusBar()->clearMessage();
  }

  return on;
}

//----------------------------------------------------------------------
void FListBox::setText (FString txt)
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

  blank = new char[size+1];
  std::memset (blank, ' ', uLong(size));
  blank[size] = '\0';

  for (int y=0; y < getHeight(); y++)
  {
    setPrintPos (1, 1 + y);
    print (blank);
  }

  delete[] blank;
}

//----------------------------------------------------------------------
void FListBox::insert ( FString item
                      , fc::brackets_type b
                      , bool s )
{
  int len, element_count;

  len = int(item.getLength());

  if ( b )
    len += 2;

  if ( len > max_line_width )
  {
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
  FListBoxItem listItem (item);
  listItem.brackets = b;
  listItem.selected = s;
  data.push_back (listItem);

  element_count = int(getCount());
  vbar->setMaximum(element_count - getHeight() + 2);
  vbar->setPageSize(element_count, getHeight() - 2);
  vbar->calculateSliderValues();

  if ( ! vbar->isVisible() && element_count >= getHeight() - 1 )
    vbar->setVisible();
}

//----------------------------------------------------------------------
void FListBox::insert ( long item
                      , fc::brackets_type b
                      , bool s )
{
  insert (FString().setNumber(item), b, s);
}

//----------------------------------------------------------------------
void FListBox::remove (int item)
{
  int element_count;

  if ( int(getCount()) < item )
    return;

  data.erase (data.begin() + item - 1);
  element_count = int(getCount());
  max_line_width = 0;

  for (int i=0; i < element_count; i++)
  {
    int len = int(data[uInt(i)].getText().getLength());

    if ( len > max_line_width )
      max_line_width = len;
  }

  hbar->setMaximum(max_line_width - getWidth() + nf_offset + 4);
  hbar->setPageSize(max_line_width, getWidth() - nf_offset - 4);

  if ( hbar->isVisible() && max_line_width < getWidth() - nf_offset - 3 )
    hbar->hide();

  vbar->setMaximum(element_count - getHeight() + 2);
  vbar->setPageSize(element_count, getHeight() - 2);

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

  data.clear();

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

  blank = new char[size+1];
  std::memset (blank, ' ', uLong(size));
  blank[size] = '\0';

  for (int y=0; y < getHeight()-2; y++)
  {
    setPrintPos (2, 2 + y);
    print (blank);
  }

  delete[] blank;
}

//----------------------------------------------------------------------
void FListBox::onKeyPress (FKeyEvent* ev)
{
  int element_count = int(getCount());
  int current_before = current;
  int xoffset_before = xoffset;
  int yoffset_before = yoffset;
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

      if ( current - yoffset >= getHeight() - 1 )
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

      if ( xoffset > max_line_width - getWidth() + nf_offset + 4 )
        xoffset = max_line_width - getWidth() + nf_offset + 4;

      if ( xoffset < 0 )
        xoffset = 0;

      inc_search.clear();
      ev->accept();
      break;

    case fc::Fkey_ppage:
      current -= getHeight()-3;

      if ( current < 1 )
        current=1;

      if ( current <= yoffset )
      {
        yoffset -= getHeight()-3;

        if ( yoffset < 0 )
          yoffset=0;
      }

      inc_search.clear();
      ev->accept();
      break;

    case fc::Fkey_npage:
      current += getHeight()-3;

      if ( current > element_count )
        current = element_count;

      if ( current - yoffset >= getHeight() - 1 )
      {
        yoffset += getHeight()-3;

        if ( yoffset > element_count - getHeight() + 2 )
          yoffset = element_count - getHeight() + 2;
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

      if ( current >= getHeight() - 1 )
        yoffset = element_count - getHeight() + 2;

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

        if ( current-yoffset >= getHeight() - 1 )
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
          uInt end = getCount();

          for (uInt i=0; i < end; i++)
          {
            if ( ! inc_found
                && inc_search.toLower()
                == data[i].getText().left(inc_len+1).toLower() )
            {
              setCurrentItem(int(i+1));
              inc_found = true;
              break;
            }
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
          inc_search.remove(inc_len-1, 1);

          if ( inc_len > 1 )
          {
            uInt end = getCount();

            for (uInt i=0; i < end; i++)
            {
              if ( inc_search.toLower()
                   == data[i].getText().left(inc_len-1).toLower() )
              {
                setCurrentItem(int(i+1));
                break;
              }
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
        uInt end = getCount();

        for (uInt i=0; i < end; i++)
        {
          if ( ! inc_found
              && inc_search.toLower()
              == data[i].getText().left(inc_len).toLower() )
          {
            setCurrentItem(int(i+1));
            inc_found = true;
            break;
          }
        }

        if ( ! inc_found )
        {
          inc_search.remove(inc_len-1, 1);

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
  if ( drag_scroll != FListBox::noScroll )
  {
    delOwnTimer();
    drag_scroll = FListBox::noScroll;
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
    if ( drag_scroll != FListBox::noScroll
        && scroll_distance < getHeight()-2 )
      scroll_distance++;

    if ( ! scroll_timer && current > 1 )
    {
      scroll_timer = true;
      addTimer(scroll_repeat);

      if ( ev->getButton() == fc::RightButton )
        drag_scroll = FListBox::scrollUpSelect;
      else
        drag_scroll = FListBox::scrollUp;
    }

    if ( current == 1 )
    {
      delOwnTimer();
      drag_scroll = FListBox::noScroll;
    }
  }
  else if ( mouse_y >= getHeight() )
  {
    // drag down
    if ( drag_scroll != FListBox::noScroll
        && scroll_distance < getHeight()-2 )
      scroll_distance++;

    if ( ! scroll_timer && current < int(getCount()) )
    {
      scroll_timer = true;
      addTimer(scroll_repeat);

      if ( ev->getButton() == fc::RightButton )
        drag_scroll = FListBox::scrollDownSelect;
      else
        drag_scroll = FListBox::scrollDown;
    }

    if ( current == int(getCount()) )
    {
      delOwnTimer();
      drag_scroll = FListBox::noScroll;
    }
  }
  else
  {
    // no dragging
    delOwnTimer();
    scroll_timer = false;
    scroll_distance = 1;
    drag_scroll = FListBox::noScroll;
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

  switch ( int(drag_scroll) )
  {
    case FListBox::noScroll:
      return;

    case FListBox::scrollUp:
    case FListBox::scrollUpSelect:
      if ( current_before == 1)
      {
        drag_scroll = FListBox::noScroll;
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

    case FListBox::scrollDown:
    case FListBox::scrollDownSelect:
      if ( current_before == element_count )
      {
        drag_scroll = FListBox::noScroll;
        return;
      }

      current += scroll_distance;

      if ( current > element_count )
        current = element_count;

      if ( current - yoffset >= getHeight() - 1 )
        yoffset += scroll_distance;

      if ( yoffset > element_count - getHeight() + 2 )
        yoffset = element_count - getHeight() + 2;

      break;

    default:
      break;
  }

  // handle multiple selections
  if (  drag_scroll == FListBox::scrollUpSelect
     || drag_scroll == FListBox::scrollDownSelect )
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
  yoffset_end = element_count - getHeight() + 2;

  if ( yoffset_end < 0 )
    yoffset_end = 0;

  wheel = ev->getWheel();

  if ( drag_scroll != FListBox::noScroll )
  {
    delOwnTimer();
    scroll_timer = false;
    scroll_distance = 1;
    drag_scroll = FListBox::noScroll;
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

//----------------------------------------------------------------------
void FListBox::cb_VBarChange (FWidget*, void*)
{
  int distance = 1;
  int element_count = int(getCount());
  int yoffset_before = yoffset;
  int scrollType = vbar->getScrollType();

  switch ( scrollType )
  {
    case FScrollbar::scrollPageBackward:
      distance = getHeight()-2;
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
      distance = getHeight()-2;
      // fall through
    case FScrollbar::scrollStepForward:
      current += distance;

      if ( current > element_count )
        current = element_count;

      if ( current - yoffset >= getHeight() - 1 )
        yoffset += distance;

      if ( yoffset > element_count - getHeight() + 2 )
        yoffset = element_count - getHeight() + 2;

      break;

    case FScrollbar::scrollJump:
    {
      int val = vbar->getValue();

      if ( yoffset == val )
        break;

      int c = current - yoffset;
      yoffset = val;

      if ( yoffset > element_count - getHeight() + 2 )
        yoffset = element_count - getHeight() + 2;

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

    default:
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
void FListBox::cb_HBarChange (FWidget*, void*)
{
  int distance = 1;
  int xoffset_before = xoffset;
  int xoffset_end = max_line_width - getWidth() + nf_offset + 4;
  int scrollType = hbar->getScrollType();

  switch ( scrollType )
  {
    case FScrollbar::scrollPageBackward:
      distance = getWidth() - nf_offset - 4;
      // fall through
    case FScrollbar::scrollStepBackward:
      xoffset -= distance;

      if ( xoffset < 0 )
        xoffset = 0;
      break;

    case FScrollbar::scrollPageForward:
      distance = getWidth() - nf_offset - 4;
      // fall through
    case FScrollbar::scrollStepForward:
      xoffset += distance;

      if ( xoffset > max_line_width - getWidth() + nf_offset + 4 )
        xoffset = max_line_width - getWidth() + nf_offset + 4;

      if ( xoffset < 0 )
        xoffset = 0;

      break;

    case FScrollbar::scrollJump:
    {
      int val = hbar->getValue();

      if ( xoffset == val )
        break;

      xoffset = val;

      if ( xoffset > max_line_width - getWidth() + nf_offset + 4 )
        xoffset = max_line_width - getWidth() + nf_offset + 4;

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

    default:
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


// protected methods of FListBox
//----------------------------------------------------------------------
void FListBox::adjustYOffset()
{
  int element_count = int(getCount());

  if ( yoffset > element_count - getHeight() + 2 )
    yoffset = element_count - getHeight() + 2;

  if ( yoffset < 0 )
    yoffset = 0;

  if ( current < yoffset )
    current = yoffset;

  if ( current >= yoffset + getHeight() - 1 )
    yoffset = current - getHeight() + 2;
}

//----------------------------------------------------------------------
void FListBox::adjustSize()
{
  int element_count;
  adjustYOffset();
  FWidget::adjustSize();

  element_count = int(getCount());
  vbar->setMaximum(element_count - getHeight() + 2);
  vbar->setPageSize(element_count, getHeight() - 2);
  vbar->setX(getWidth());
  vbar->setHeight (getHeight()-2, false);
  vbar->resize();

  hbar->setMaximum(max_line_width - getWidth() + nf_offset + 4);
  hbar->setPageSize(max_line_width, getWidth() - nf_offset - 4);
  hbar->setY(getHeight());
  hbar->setWidth (getWidth()-2, false);
  hbar->resize();

  if ( element_count < getHeight() - 1 )
    vbar->hide();
  else
    vbar->setVisible();

  if ( max_line_width < getWidth() - nf_offset - 3 )
    hbar->hide();
  else
    hbar->setVisible();
}


// private methods of FListBox
//----------------------------------------------------------------------
void FListBox::init()
{
  if ( hasFocus() )
    flags = fc::focus;

  if ( isEnabled() )
    flags |= fc::active;

  nf_offset = isNewFont() ? 1 : 0;

  setForegroundColor (wc.dialog_fg);
  setBackgroundColor (wc.dialog_bg);

  vbar = new FScrollbar(fc::vertical, this);
  vbar->setMinimum(0);
  vbar->setValue(0);
  vbar->hide();

  hbar = new FScrollbar(fc::horizontal, this);
  hbar->setMinimum(0);
  hbar->setValue(0);
  hbar->hide();

  setGeometry (1, 1, 5, 4, false);  // initialize geometry values

  vbar->addCallback
  (
    "change-value",
    _METHOD_CALLBACK (this, &FListBox::cb_VBarChange)
  );

  hbar->addCallback
  (
    "change-value",
    _METHOD_CALLBACK (this, &FListBox::cb_HBarChange)
  );
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

    for (int y=2; y < getHeight(); y++)
    {
      setPrintPos (getWidth(),y);
      print (' '); // clear right side of the scrollbar
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
    FString msg = getStatusbarMessage();
    FString curMsg = getStatusBar()->getMessage();

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

  if ( length <= uInt(getWidth()-2) )
    print (txt);
  else
  {
    print (text.left(uInt(getWidth()-4)));
    setColor (wc.label_ellipsis_fg, wc.label_bg);
    print("..");
  }
}

//----------------------------------------------------------------------
void FListBox::drawList()
{
  FString element;
  uInt start, end, inc_len;
  bool isFocus;

  if ( data.empty() || getHeight() <= 2 || getWidth() <= 4 )
    return;

  isFocus = ((flags & fc::focus) != 0);
  start   = 0;
  end     = uInt(getHeight()-2);
  inc_len = inc_search.getLength();

  if ( end > getCount() )
    end = getCount();

  if ( last_yoffset >= 0
      && last_yoffset == yoffset
      && last_current != current )
  {
    // speed up: redraw only the changed rows
    uInt last_pos = uInt(current - yoffset) - 1;
    uInt current_pos = uInt(last_current - yoffset) - 1;
    start = std::min(last_pos, current_pos);
    end = std::max(last_pos, current_pos)+1;
  }

  for (uInt y=start; y < end; y++)
  {
    setPrintPos (2, 2 + int(y));
    bool serach_mark = false;
    bool lineHasBrackets = hasBrackets(int(y) + yoffset + 1);
    bool isLineSelected = isSelected(int(y) + yoffset + 1);
    bool isCurrentLine = bool(uInt(y) + uInt(yoffset) + 1 == uInt(current));

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

        setCursorPos (3, 2 + int(y)); // first character
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
            setCursorPos (2 + b + int(inc_len), 2 + int(y)); // last found character
          }
          else  // only highlighted
            setCursorPos (3 + b, 2 + int(y)); // first character
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
      const wchar_t* element_str;
      int full_length;
      uInt len;
      uInt i = 0;
      uInt b = 0;

      if ( xoffset == 0 )
      {
        b=1;

        switch ( data[y+uInt(yoffset)].brackets )
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

        element = data[y+uInt(yoffset)].getText()
                                       .mid ( uInt(1+xoffset)
                                            , uInt(getWidth()-nf_offset-5) );
      }
      else
        element = data[y+uInt(yoffset)].getText()
                                       .mid ( uInt(xoffset)
                                            ,  uInt(getWidth()-nf_offset-4) );

      element_str = element.wc_str();
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

      full_length = int(data[y+uInt(yoffset)].getText().getLength());

      if ( b+i < uInt(getWidth()-nf_offset-4) && xoffset <= full_length+1 )
      {
        if ( serach_mark && i == inc_len )
          setColor ( wc.current_element_focus_fg
                   , wc.current_element_focus_bg );

        switch ( data[y+uInt(yoffset)].brackets )
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

      for (; b+i < uInt(getWidth()-nf_offset-3); i++)
        print (' ');
    }
    else  // line has no brackets
    {
      const wchar_t* element_str;
      uInt i, len;
      element = data[y+uInt(yoffset)].getText()
                                     .mid ( uInt(1+xoffset)
                                          , uInt(getWidth()-nf_offset-4) );
      element_str = element.wc_str();
      len = element.getLength();

      if ( serach_mark )
        setColor ( wc.current_inc_search_element_fg
                 , wc.current_element_focus_bg );

      for (i=0; i < len; i++)
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

      for (; i < uInt(getWidth()-nf_offset-3); i++)
        print (' ');
    }
  }

  if ( isMonochron() )  // unset for the last element
    setReverse(false);

  unsetBold();
  last_yoffset = yoffset;
  last_current = current;
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
