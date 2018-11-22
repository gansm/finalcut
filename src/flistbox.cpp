/***********************************************************************
* flistbox.cpp - Widget FListBox and FListBoxItem                      *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2014-2018 Markus Gans                                      *
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

#include "final/fapplication.h"
#include "final/flistbox.h"
#include "final/fscrollbar.h"
#include "final/fstatusbar.h"

namespace finalcut
{

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
void FListBox::setCurrentItem (std::size_t index)
{
  std::size_t element_count;

  if ( index == current )
    return;

  element_count = getCount();

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
  std::size_t index = std::size_t(std::distance(itemlist.begin(), iter)) + 1;
  setCurrentItem(index);
}

//----------------------------------------------------------------------
void FListBox::showInsideBrackets ( std::size_t index
                                  , fc::brackets_type b )
{
  listBoxItems::iterator iter = index2iterator(index - 1);
  iter->brackets = b;

  if ( b == fc::NoBrackets )
    return;

  std::size_t len = iter->getText().getLength() + 2;

  if ( len > max_line_width )
  {
    max_line_width = len;

    if ( len >= getWidth() - nf_offset - 3 )
    {
      hbar->setMaximum (int(max_line_width - getWidth() + nf_offset + 4));
      hbar->setPageSize (int(max_line_width), int(getWidth() - nf_offset - 4));
      hbar->setValue (xoffset);

      if ( ! hbar->isVisible() )
        hbar->setVisible();
    }
  }
}

//----------------------------------------------------------------------
void FListBox::setGeometry (int x, int y, std::size_t w, std::size_t h, bool adjust)
{
  // Set the widget geometry

  FWidget::setGeometry(x, y, w, h, adjust);

  if ( isNewFont() )
  {
    vbar->setGeometry (int(getWidth()), 2, 2, getHeight() - 2);
    hbar->setGeometry (1, int(getHeight()), getWidth() - 2 - nf_offset, 1);
  }
  else
  {
    vbar->setGeometry (int(getWidth()), 2, 1, getHeight() - 2);
    hbar->setGeometry (2, int(getHeight()), getWidth() - 2, 1);
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
  std::size_t n, size;
  FColor fg, bg;
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

  if ( size == 0 )
    return;

  char* blank = createBlankArray(size + 1);

  for (int y = 0; y < int(getHeight()); y++)
  {
    setPrintPos (1, 1 + y);
    print (blank);
  }

  destroyBlankArray (blank);
}

//----------------------------------------------------------------------
void FListBox::insert (FListBoxItem listItem)
{
  std::size_t len = listItem.text.getLength();
  bool has_brackets = bool(listItem.brackets);
  recalculateHorizontalBar (len, has_brackets);

  itemlist.push_back (listItem);

  std::size_t element_count = getCount();
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
void FListBox::remove (std::size_t item)
{
  std::size_t element_count;

  if ( item > getCount() )
    return;

  itemlist.erase (itemlist.begin() + int(item) - 1);
  element_count = getCount();
  max_line_width = 0;

  listBoxItems::iterator iter = itemlist.begin();

  while ( iter != itemlist.end() )
  {
    std::size_t len = iter->getText().getLength();

    if ( len > max_line_width )
      max_line_width = len;

    ++iter;
  }

  hbar->setMaximum (int(max_line_width - getWidth() + nf_offset + 4));
  hbar->setPageSize (int(max_line_width), int(getWidth() - nf_offset - 4));

  if ( hbar->isVisible() && max_line_width < getWidth() - nf_offset - 3 )
    hbar->hide();

  vbar->setMaximum (int(element_count - getHeight()) + 2);
  vbar->setPageSize (int(element_count), int(getHeight()) - 2);

  if ( vbar->isVisible() && element_count < getHeight() - 1 )
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
  std::size_t size;
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

  if ( size == 0 )
    return;

  char* blank = createBlankArray(size + 1);

  std::memset (blank, ' ', size);
  blank[size] = '\0';

  for (int y = 0; y < int(getHeight()) - 2; y++)
  {
    setPrintPos (2, 2 + y);
    print (blank);
  }

  destroyBlankArray (blank);
}

//----------------------------------------------------------------------
void FListBox::onKeyPress (FKeyEvent* ev)
{
  std::size_t current_before = current;
  int xoffset_before = xoffset;
  int yoffset_before = yoffset;
  FKey key = ev->key();

  switch ( key )
  {
    case fc::Fkey_return:
    case fc::Fkey_enter:
      keyEnter();
      ev->accept();
      break;

    case fc::Fkey_up:
      keyUp();
      ev->accept();
      break;

    case fc::Fkey_down:
      keyDown();
      ev->accept();
      break;

    case fc::Fkey_left:
      keyLeft();
      ev->accept();
      break;

    case fc::Fkey_right:
      keyRight();
      ev->accept();
      break;

    case fc::Fkey_ppage:
      keyPgUp();
      ev->accept();
      break;

    case fc::Fkey_npage:
      keyPgDn();
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

    case fc::Fkey_ic:  // insert key
      if ( keyInsert() )
        ev->accept();
      break;

    case fc::Fkey_space:
      if ( keySpace() )
        ev->accept();
      break;

    case fc::Fkey_erase:
    case fc::Fkey_backspace:
      if ( keyBackspace() )
        ev->accept();
      break;

    case fc::Fkey_escape:
    case fc::Fkey_escape_mintty:
      if ( keyEsc() )
        ev->accept();
      break;

    default:
      if ( keyIncSearchInput(key) )
        ev->accept();
  }

  if ( current_before != current )
  {
    processChanged();

    if ( ! isMultiSelection() )
      processSelect();
  }

  if ( ev->isAccepted() )
  {
    bool draw_vbar = yoffset_before != yoffset;
    bool draw_hbar = xoffset_before != xoffset;
    updateDrawing (draw_vbar, draw_hbar);
  }
}

//----------------------------------------------------------------------
void FListBox::onMouseDown (FMouseEvent* ev)
{
  int yoffset_before
    , mouse_x
    , mouse_y;

  if ( ev->getButton() != fc::LeftButton
    && ev->getButton() != fc::RightButton )
  {
    return;
  }

  if ( ev->getButton() == fc::RightButton && ! isMultiSelection() )
    return;

  getWidgetFocus();

  yoffset_before = yoffset;
  mouse_x = ev->getX();
  mouse_y = ev->getY();

  if ( mouse_x > 1 && mouse_x < int(getWidth())
    && mouse_y > 1 && mouse_y < int(getHeight()) )
  {
    std::size_t element_count = getCount();
    current = std::size_t(yoffset + mouse_y - 1);

    if ( current > element_count )
      current = element_count;

    inc_search.clear();

    if ( ev->getButton() == fc::RightButton )
      multiSelection(current);

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
    stopDragScroll();

  if ( ev->getButton() == fc::LeftButton )
  {
    int mouse_x = ev->getX();
    int mouse_y = ev->getY();

    if ( mouse_x > 1 && mouse_x < int(getWidth())
      && mouse_y > 1 && mouse_y < int(getHeight()) )
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
  if ( ev->getButton() != fc::LeftButton
    && ev->getButton() != fc::RightButton )
    return;

  if ( ev->getButton() == fc::RightButton && ! isMultiSelection() )
    return;

  std::size_t current_before = current;
  int yoffset_before = yoffset;
  int mouse_x = ev->getX();
  int mouse_y = ev->getY();

  if ( mouse_x > 1 && mouse_x < int(getWidth())
    && mouse_y > 1 && mouse_y < int(getHeight()) )
  {
    std::size_t element_count = getCount();
    current = std::size_t(yoffset + mouse_y - 1);

    if ( current > element_count )
      current = element_count;

    inc_search.clear();

    // Handle multiple selections
    if ( ev->getButton() == fc::RightButton
      && current_before != current )
    {
      multiSelectionUpTo(current);
    }

    if ( isVisible() )
      drawList();

    vbar->setValue (yoffset);

    if ( vbar->isVisible() && yoffset_before != yoffset )
      vbar->drawBar();

    updateTerminal();
    flush_out();
  }

  // Auto-scrolling when dragging mouse outside the widget
  if ( mouse_y < 2 )
    dragUp (ev->getButton());
  else if ( mouse_y >= int(getHeight()) )
    dragDown (ev->getButton());
  else
    stopDragScroll();
}

//----------------------------------------------------------------------
void FListBox::onMouseDoubleClick (FMouseEvent* ev)
{
  int mouse_x, mouse_y;

  if ( ev->getButton() != fc::LeftButton )
    return;

  mouse_x = ev->getX();
  mouse_y = ev->getY();

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
  std::size_t current_before = current;
  int yoffset_before = yoffset;

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

    // Handle multiple selections
    if ( drag_scroll == fc::scrollUpSelect
      || drag_scroll == fc::scrollDownSelect )
      multiSelectionUpTo(current);
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
  std::size_t current_before = current;
  int wheel
    , yoffset_before = yoffset
    , pagesize = 4;

  wheel = ev->getWheel();

  if ( drag_scroll != fc::noScroll )
    stopDragScroll();

  switch ( wheel )
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

  if ( current_before != current )
  {
    inc_search.clear();
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
  std::size_t element_count = getCount();

  if ( element_count == 0 || getClientHeight() == 0 )
    return;

  if ( yoffset > int(element_count - getClientHeight()) )
    yoffset = int(element_count - getClientHeight());

  if ( yoffset < 0 )
    yoffset = 0;

  if ( current < std::size_t(yoffset) )
    current = std::size_t(yoffset);

  if ( yoffset < int(current - getClientHeight()) )
    yoffset = int(current - getClientHeight());
}

//----------------------------------------------------------------------
void FListBox::adjustSize()
{
  std::size_t element_count;
  FWidget::adjustSize();
  adjustYOffset();

  element_count = getCount();
  vbar->setMaximum (int(element_count - getClientHeight()));
  vbar->setPageSize (int(element_count), int(getClientHeight()));
  vbar->setX (int(getWidth()));
  vbar->setHeight (getClientHeight(), false);
  vbar->resize();

  hbar->setMaximum (int(max_line_width - getClientWidth() + 2));
  hbar->setPageSize (int(max_line_width), int(getClientWidth()) - 2);
  hbar->setY (int(getHeight()));
  hbar->setWidth (getClientWidth() + nf_offset, false);
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
    std::cerr << bad_alloc_str << ex.what() << std::endl;
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
  setRightPadding(1 + int(nf_offset));
}

//----------------------------------------------------------------------
void FListBox::draw()
{
  if ( current < 1 )
    current = 1;

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

  drawHeadline();

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
void FListBox::drawHeadline()
{
  if ( text.isNull() || text.isEmpty() )
    return;

  FString txt = " " + text + " ";
  std::size_t length = txt.getLength();
  setPrintPos (2, 1);

  if ( isEnabled() )
    setColor(wc.label_emphasis_fg, wc.label_bg);
  else
    setColor(wc.label_inactive_fg, wc.label_inactive_bg);

  if ( length <= uInt(getClientWidth()) )
    print (txt);
  else
  {
    // Print ellipsis
    print (text.left(uInt(getClientWidth() - 2)));
    setColor (wc.label_ellipsis_fg, wc.label_bg);
    print("..");
  }
}

//----------------------------------------------------------------------
void FListBox::drawList()
{
  std::size_t start, num;
  listBoxItems::iterator iter;

  if ( itemlist.empty() || getHeight() <= 2 || getWidth() <= 4 )
    return;

  start = 0;
  num   = uInt(getHeight() - 2);

  if ( num > getCount() )
    num = getCount();

  if ( last_yoffset >= 0
    && last_yoffset == yoffset
    && last_current != int(current) )
  {
    // speed up: redraw only the changed rows
    std::size_t last_pos = current - std::size_t(yoffset) - 1;
    std::size_t current_pos = std::size_t(last_current - yoffset) - 1;
    start = std::min(last_pos, current_pos);
    num = std::max(last_pos, current_pos) + 1;
  }

  iter = index2iterator(start + std::size_t(yoffset));

  for (std::size_t y = start; y < num && iter != itemlist.end() ; y++)
  {
    bool serach_mark = false;
    bool lineHasBrackets = hasBrackets(iter);

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
  std::size_t i, len;
  std::size_t inc_len = inc_search.getLength();
  bool isCurrentLine = bool(y + yoffset + 1 == int(current));
  FString element;
  element = getString(iter).mid ( std::size_t(1 + xoffset)
                                , getWidth() - nf_offset - 4 );
  const wchar_t* const& element_str = element.wc_str();
  len = element.getLength();

  if ( isMonochron() && isCurrentLine && flags.focus )
    print (fc::BlackRightPointingPointer);  // ►
  else
    print (' ');

  if ( serach_mark )
    setColor ( wc.current_inc_search_element_fg
             , wc.current_element_focus_bg );

  for (i = 0; i < len; i++)
  {
    if ( serach_mark && i == inc_len && flags.focus  )
      setColor ( wc.current_element_focus_fg
               , wc.current_element_focus_bg );

    print (element_str[i]);
  }

  if ( isMonochron() && isCurrentLine  && flags.focus )
  {
    print (fc::BlackLeftPointingPointer);   // ◄
    i++;
  }

  for (; i < getWidth() - nf_offset - 3; i++)
    print (' ');
}

//----------------------------------------------------------------------
inline void FListBox::printLeftBracket (fc::brackets_type bracket_type)
{
  switch ( bracket_type )
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
}

//----------------------------------------------------------------------
inline void FListBox::printRightBracket (fc::brackets_type bracket_type)
{
  switch ( bracket_type )
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
}

//----------------------------------------------------------------------
inline void FListBox::drawListBracketsLine ( int y
                                           , listBoxItems::iterator iter
                                           , bool serach_mark )
{
  FString element;
  std::size_t len
            , full_length
            , inc_len = inc_search.getLength()
            , i = 0
            , b = 0;
  bool isCurrentLine = bool(y + yoffset + 1 == int(current));

  if ( isMonochron() && isCurrentLine && flags.focus )
    print (fc::BlackRightPointingPointer);  // ►
  else
    print (' ');

  if ( xoffset == 0 )
  {
    b = 1;
    printLeftBracket (iter->brackets);

    element = getString(iter).mid ( std::size_t(xoffset) + 1
                                  , getWidth() - nf_offset - 5 );
  }
  else
    element = getString(iter).mid ( std::size_t(xoffset)
                                  , getWidth() - nf_offset - 4 );

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

  full_length = getString(iter).getLength();

  if ( b + i < getWidth() - nf_offset - 4
    && std::size_t(xoffset) <= full_length + 1 )
  {
    if ( serach_mark && i == inc_len )
      setColor ( wc.current_element_focus_fg
               , wc.current_element_focus_bg );

    printRightBracket (iter->brackets);
    i++;
  }

  if ( isMonochron() && isCurrentLine && flags.focus )
  {
    print (fc::BlackLeftPointingPointer);   // ◄
    i++;
  }

  for (; b + i < getWidth() - nf_offset - 3; i++)
    print (' ');
}

//----------------------------------------------------------------------
inline void FListBox::setLineAttributes ( int y
                                        , bool isLineSelected
                                        , bool lineHasBrackets
                                        , bool& serach_mark )
{
  bool isCurrentLine = bool(y + yoffset + 1 == int(current));
  std::size_t inc_len = inc_search.getLength();
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
    if ( flags.focus && getMaxColor() < 16 )
      setBold();

    if ( isLineSelected )
    {
      if ( isMonochron() )
        setBold();
      else if ( flags.focus )
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

      if ( flags.focus )
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
    else if ( flags.focus && getMaxColor() < 16 )
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
  if ( isVisible() )
    drawList();

  vbar->setValue (yoffset);

  if ( vbar->isVisible() && draw_vbar )
    vbar->drawBar();

  hbar->setValue (xoffset);

  if ( hbar->isVisible() && draw_hbar )
    hbar->drawBar();

  updateTerminal();
  flush_out();
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
    hbar->setMaximum (int(max_line_width - getWidth() + nf_offset + 4));
    hbar->setPageSize (int(max_line_width), int(getWidth() - nf_offset - 4));
    hbar->calculateSliderValues();

    if ( ! hbar->isVisible() )
      hbar->setVisible();
  }
}

//----------------------------------------------------------------------
void FListBox::recalculateVerticalBar (std::size_t element_count)
{
  vbar->setMaximum (int(element_count - getHeight() + 2));
  vbar->setPageSize (int(element_count), int(getHeight()) - 2);
  vbar->calculateSliderValues();

  if ( ! vbar->isVisible() && element_count >= getHeight() - 1 )
    vbar->setVisible();
}

//----------------------------------------------------------------------
inline void FListBox::getWidgetFocus()
{
  if ( hasFocus() )
    return;

  FWidget* focused_widget = getFocusWidget();
  FFocusEvent out (fc::FocusOut_Event);
  FApplication::queueEvent(focused_widget, &out);
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
  std::size_t from, to;

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
    current -= std::size_t(pagesize + yoffset);
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
  std::size_t element_count = getCount();
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
  std::size_t element_count = getCount();

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
  std::size_t element_count = getCount();
  int yoffset_end = int(element_count - getClientHeight());

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
  static const std::size_t padding_space = 2;  // 1 leading + 1 trailing space
  std::size_t xoffset_end = max_line_width - getClientWidth() + padding_space;

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
  std::size_t element_count = getCount();
  int yoffset_end = int(element_count - getClientHeight());

  if ( yoffset == val )
    return;

  int c = int(current) - yoffset;
  yoffset = val;

  if ( yoffset > yoffset_end )
    yoffset = yoffset_end;

  if ( yoffset < 0 )
    yoffset = 0;

  current = std::size_t(yoffset + c);

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
  static const std::size_t padding_space = 2;  // 1 leading + 1 trailing space
  std::size_t xoffset_end = max_line_width - getClientWidth() + padding_space;
  xoffset += distance;

  if ( xoffset == int(xoffset_end) )
    return;

  if ( xoffset > int(xoffset_end) )
    xoffset = int(xoffset_end);

  if ( xoffset < 0 )
    xoffset = 0;
}

//----------------------------------------------------------------------
inline void FListBox::keyUp()
{
  prevListItem (1);
  inc_search.clear();
}

//----------------------------------------------------------------------
inline void FListBox::keyDown()
{
  nextListItem (1);
  inc_search.clear();
}

//----------------------------------------------------------------------
inline void FListBox::keyLeft()
{
  scrollLeft(1);
  inc_search.clear();
}

//----------------------------------------------------------------------
inline void FListBox::keyRight()
{
  scrollRight(1);
  inc_search.clear();
}

//----------------------------------------------------------------------
inline void FListBox::keyPgUp()
{
  int pagesize = int(getClientHeight()) - 1;
  prevListItem (pagesize);
  inc_search.clear();
}

//----------------------------------------------------------------------
inline void FListBox::keyPgDn()
{
  int pagesize = int(getClientHeight()) - 1;
  nextListItem (pagesize);
  inc_search.clear();
}

//----------------------------------------------------------------------
inline void FListBox::keyHome()
{
  current = 1;
  yoffset = 0;
  inc_search.clear();
}

//----------------------------------------------------------------------
inline void FListBox::keyEnd()
{
  std::size_t element_count = getCount();
  int yoffset_end = int(element_count - getClientHeight());
  current = element_count;

  if ( current > getClientHeight() )
    yoffset = yoffset_end;

  inc_search.clear();
}

//----------------------------------------------------------------------
inline bool FListBox::keyEsc()
{
  if ( inc_search.getLength() > 0 )
  {
    inc_search.clear();
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
inline void FListBox::keyEnter()
{
  processClick();
  inc_search.clear();
}

//----------------------------------------------------------------------
inline bool FListBox::keySpace()
{
  std::size_t inc_len = inc_search.getLength();

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
      return false;
    }
  }
  else if ( isMultiSelection() )
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
inline bool FListBox::keyInsert()
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

    if ( current - std::size_t(yoffset) >= getHeight() - 1 )
      yoffset++;

    return true;
  }

  inc_search.clear();
  return false;
}

//----------------------------------------------------------------------
inline bool FListBox::keyBackspace()
{
  std::size_t inc_len = inc_search.getLength();

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
void FListBox::lazyConvert(listBoxItems::iterator iter, int y)
{
  if ( conv_type != lazy_convert || ! iter->getText().isNull() )
    return;

  convertToItem (*iter, source_container, y + yoffset);
  std::size_t len = iter->text.getLength();
  recalculateHorizontalBar (len, hasBrackets(iter));

  if ( hbar->isVisible() )
    hbar->redraw();
}

//----------------------------------------------------------------------
void FListBox::cb_VBarChange (FWidget*, data_ptr)
{
  FScrollbar::sType scrollType;
  std::size_t current_before = current;
  int distance = 1
    , pagesize = 4
    , yoffset_before = yoffset;
  scrollType = vbar->getScrollType();

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
      wheelUp (pagesize);
      break;

    case FScrollbar::scrollWheelDown:
      wheelDown (pagesize);
      break;
  }

  if ( current_before != current )
  {
    inc_search.clear();
    processChanged();

    if ( ! isMultiSelection() )
      processSelect();
  }

  if ( isVisible() )
    drawList();

  if ( scrollType >= FScrollbar::scrollStepBackward
    && scrollType <= FScrollbar::scrollWheelDown )
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
  static const int padding_space = 2;  // 1 leading space + 1 trailing space
  FScrollbar::sType scrollType;
  int distance = 1
    , pagesize = 4
    , xoffset_before = xoffset;
  scrollType = hbar->getScrollType();

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
      scrollLeft (pagesize);
      break;

    case FScrollbar::scrollWheelDown:
      scrollRight (pagesize);
      break;
  }

  if ( xoffset_before != xoffset )
    inc_search.clear();

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

}  // namespace finalcut
