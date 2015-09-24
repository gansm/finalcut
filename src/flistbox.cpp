// flistbox.cpp
// class FListBoxItem
// class FListBox

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
  , VBar(0)
  , HBar(0)
  , text()
  , inc_search()
  , multiSelect(false)
  , mouseSelect(false)
  , dragScroll(FListBox::noScroll)
  , scrollTimer(false)
  , scrollRepeat(100)
  , scrollDistance(1)
  , current(0)
  , last_current(-1)
  , secect_from_item(-1)
  , xoffset(0)
  , yoffset(0)
  , last_yoffset(-1)
  , nf_offset(0)
  , maxLineWidth(0)
{
  init();
}

//----------------------------------------------------------------------
FListBox::~FListBox()  // destructor
{
  delAllTimer();
  delete VBar;
  delete HBar;
}


// private methods of FListBox
//----------------------------------------------------------------------
void FListBox::init()
{
  if ( hasFocus() )
    flags = FOCUS;
  if ( isEnabled() )
    flags |= ACTIVE;

  nf_offset = isNewFont() ? 1 : 0;

  foregroundColor = wc.dialog_fg;
  backgroundColor = wc.dialog_bg;

  VBar = new FScrollbar(fc::vertical, this);
  VBar->setMinimum(0);
  VBar->setValue(0);
  VBar->hide();

  HBar = new FScrollbar(fc::horizontal, this);
  HBar->setMinimum(0);
  HBar->setValue(0);
  HBar->hide();

  setGeometry (1, 1, 5, 4, false);  // initialize geometry values

  VBar->addCallback
  (
    "change-value",
    _METHOD_CALLBACK (this, &FListBox::cb_VBarChange)
  );
  HBar->addCallback
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

  setUpdateVTerm(false);
  setColor (foregroundColor, backgroundColor);
  if ( isNewFont() )
    width--;
  drawBorder();
  if ( isNewFont() )
  {
    width++;
    if ( ! VBar->isVisible() )
    {
      setColor (foregroundColor, backgroundColor);
      for (int y=ypos+ymin; y < ypos+ymin+height-2; y++)
      {
        gotoxy(xpos+xmin+width-2, y);
        print (' '); // clear right side of the scrollbar
      }
    }
  }
  drawLabel();
  setUpdateVTerm(true);

  if ( VBar->isVisible() )
    VBar->redraw();
  if ( HBar->isVisible() )
    HBar->redraw();

  drawList();

  isFocus = ((flags & FOCUS) != 0);

  if ( isFocus && statusBar() )
  {
    FString msg = getStatusbarMessage();
    FString curMsg = statusBar()->getMessage();
    if ( curMsg != msg )
    {
      statusBar()->setMessage(msg);
      statusBar()->drawMessage();
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

  if ( Encoding == fc::VT100 )
    unsetVT100altChar();

  txt = " " + text + " ";
  length = txt.getLength();
  gotoxy (xpos+xmin, ypos+ymin-1);

  if ( isEnabled() )
    setColor(wc.label_emphasis_fg, wc.label_bg);
  else
    setColor(wc.label_inactive_fg, wc.label_inactive_bg);
  if ( length <= uInt(width-2) )
    print (txt);
  else
  {
    print (text.left(uInt(width-4)));
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

  if ( data.empty() || height < 4 || width < 5 )
    return;

  isFocus = ((flags & FOCUS) != 0);
  start   = 0;
  end     = uInt(height-2);
  inc_len = inc_search.getLength();

  if ( end > count() )
    end = count();

  if (  last_yoffset >= 0
     && last_yoffset == yoffset
     && last_current != current )
  {
    // speed up: redraw only the changed rows
    uInt last_pos = uInt(current - yoffset) - 1;
    uInt current_pos = uInt(last_current - yoffset) - 1;
    start = std::min(last_pos, current_pos);
    end = std::max(last_pos, current_pos)+1;
  }

  setUpdateVTerm(false);
  for (uInt y=start; y < end; y++)
  {
    gotoxy (xpos+xmin, ypos+ymin+int(y));
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
      if ( isLineSelected )
      {
        if ( isMonochron() )
          setBold();
        else
          setColor ( wc.selected_current_element_fg
                   , wc.selected_current_element_bg );
      }
      else
      {
        if ( isMonochron() )
          unsetBold();
        else if ( isFocus )
        {
          setColor ( wc.current_element_focus_fg
                   , wc.current_element_focus_bg );
          if ( inc_len > 0 )
          {
            serach_mark = true;
            int b = ( lineHasBrackets ) ? 1: 0;
            setCursorPos ( xpos+xmin+int(inc_len)+b
                         , ypos+ymin+int(y) ); // last found character
          }
          else
            setCursorPos ( xpos+xmin+1
                         , ypos+ymin+int(y) ); // first character
        }
        else
          setColor ( wc.current_element_fg
                   , wc.current_element_bg );
      }
      if ( isMonochron() )
        setReverse(true);
    }
    else if ( isMonochron() )
      setReverse(false);

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
                                            , uInt(width-nf_offset-5) );
      }
      else
        element = data[y+uInt(yoffset)].getText()
                                       .mid ( uInt(xoffset)
                                            ,  uInt(width-nf_offset-4) );
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
      if ( b+i < uInt(width-nf_offset-4) && xoffset <= full_length+1 )
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
      for (; b+i < uInt(width-nf_offset-3); i++)
        print (' ');
    }
    else  // line has no brackets
    {
      const wchar_t* element_str;
      uInt i, len;
      element = data[y+uInt(yoffset)].getText()
                                     .mid( uInt(1+xoffset),
                                           uInt(width-nf_offset-4) );
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
      for (; i < uInt(width-nf_offset-3); i++)
        print (' ');
    }
  }

  if ( isMonochron() )  // unset for the last element
    setReverse(false);

  unsetBold();
  setUpdateVTerm(true);
  updateTerminal();
  flush_out();

  if ( hasFocus() )
    setCursor();

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

// protected methods of FListBox
//----------------------------------------------------------------------
void FListBox::adjustYOffset()
{
  int element_count = int(count());
  if ( yoffset > element_count - height + 2 )
    yoffset = element_count - height + 2;
  if ( yoffset < 0 )
    yoffset = 0;
  if ( current < yoffset )
    current = yoffset;
  if ( current >= yoffset + height - 1 )
    yoffset = current - height + 2;
}

//----------------------------------------------------------------------
void FListBox::adjustSize()
{
  int element_count;
  adjustYOffset();

  FWidget::adjustSize();

  element_count = int(count());
  VBar->setMaximum(element_count - height + 2);
  VBar->setPageSize(element_count, height - 2);
  VBar->setX(width);
  VBar->setHeight (height-2, false);
  VBar->resize();

  HBar->setMaximum(maxLineWidth - width + nf_offset + 4);
  HBar->setPageSize(maxLineWidth, width - nf_offset - 4);
  HBar->setY(height);
  HBar->setWidth (width-2, false);
  HBar->resize();

  if ( element_count < height - 1 )
    VBar->hide();
  else
    VBar->setVisible();

  if ( maxLineWidth < width - nf_offset - 3 )
    HBar->hide();
  else
    HBar->setVisible();
}

// public methods of FListBox
//----------------------------------------------------------------------
void FListBox::setCurrentItem(int index)
{
  int element_count;
  if ( index == current )
    return;

  element_count = int(count());
  if ( index > element_count )
    current = element_count;
  else if ( index < 1 )
    current = 1;
  else
    current = index;
  xoffset = 0;
  yoffset = 0;
  adjustSize();

  VBar->setValue(yoffset);
  if ( isVisible() )
    redraw();
}

//----------------------------------------------------------------------
void FListBox::hide()
{
  int fg, bg, n, size;
  char* blank;

  FWidget::hide();

  fg = parentWidget()->getForegroundColor();
  bg = parentWidget()->getBackgroundColor();
  setColor (fg, bg);

  n = isNewFont() ? 1 : 0;
  size = width + n;
  blank = new char[size+1];
  memset(blank, ' ', uLong(size));
  blank[size] = '\0';
  for (int y=0; y < height; y++)
  {
    gotoxy (xpos+xmin-1, ypos+ymin-1+y);
    print (blank);
  }
  delete[] blank;
}

//----------------------------------------------------------------------
void FListBox::showInsideBrackets ( int index
                                  , fc::brackets_type b )
{
  data[uInt(index-1)].brackets = b;

  if ( b != fc::NoBrackets )
  {
    int len = int(data[uInt(index-1)].getText().getLength() + 2);

    if ( len > maxLineWidth )
    {
      maxLineWidth = len;
      if ( len >= width - nf_offset - 3 )
      {
        HBar->setMaximum(maxLineWidth - width + nf_offset + 4);
        HBar->setPageSize(maxLineWidth, width - nf_offset - 4);
        HBar->setValue (xoffset);
        if ( ! HBar->isVisible() )
          HBar->setVisible();
      }
    }
  }
}

//----------------------------------------------------------------------
void FListBox::setGeometry (int x, int y, int w, int h, bool adjust)
{
  FWidget::setGeometry(x, y, w, h, adjust);
  if ( isNewFont() )
  {
    VBar->setGeometry(width, 2, 2, height-2);
    HBar->setGeometry(1, height, width-2, 1);
  }
  else
  {
    VBar->setGeometry(width, 2, 1, height-2);
    HBar->setGeometry(2, height, width-2, 1);
  }
}

//----------------------------------------------------------------------
bool FListBox::setEnable (bool on)
{
  FWidget::setEnable(on);

  if ( on )
    flags |= ACTIVE;
  else
    flags &= ~ACTIVE;
  return on;
}

//----------------------------------------------------------------------
bool FListBox::setFocus (bool on)
{
  FWidget::setFocus(on);

  if ( on )
  {
    flags |= FOCUS;

    if ( statusBar() )
    {
      FString msg = getStatusbarMessage();
      FString curMsg = statusBar()->getMessage();
      if ( curMsg != msg )
        statusBar()->setMessage(msg);
    }
  }
  else
  {
    flags &= ~FOCUS;

    if ( statusBar() )
      statusBar()->clearMessage();
  }
  return on;
}

//----------------------------------------------------------------------
bool FListBox::setShadow (bool on)
{
  if ( on )
    flags |= SHADOW;
  else
    flags &= ~SHADOW;
  return on;
}

//----------------------------------------------------------------------
void FListBox::onKeyPress (FKeyEvent* ev)
{
  int element_count = int(count());
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
      if ( current - yoffset >= height - 1 )
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
      if ( xoffset > maxLineWidth - width + nf_offset + 4 )
        xoffset = maxLineWidth - width + nf_offset + 4;
      if ( xoffset < 0 )
        xoffset = 0;
      inc_search.clear();
      ev->accept();
      break;

    case fc::Fkey_ppage:
      current -= height-3;
      if ( current < 1 )
        current=1;
      if ( current <= yoffset )
      {
        yoffset -= height-3;
        if ( yoffset < 0 )
          yoffset=0;
      }
      inc_search.clear();
      ev->accept();
      break;

    case fc::Fkey_npage:
      current += height-3;
      if ( current > element_count )
        current = element_count;
      if ( current - yoffset >= height - 1 )
      {
        yoffset += height-3;
        if ( yoffset > element_count - height + 2 )
          yoffset = element_count - height + 2;
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
      if ( current >= height - 1 )
        yoffset = element_count - height + 2;
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
        if ( current-yoffset >= height - 1 )
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
          uInt end = count();
          for (uInt i=0; i < end; i++)
          {
            if (  ! inc_found
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
            uInt end = count();
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
        uInt end = count();
        for (uInt i=0; i < end; i++)
        {
          if (  ! inc_found
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
    VBar->setValue (yoffset);
    if ( VBar->isVisible() && yoffset_before != yoffset )
      VBar->drawBar();
    HBar->setValue (xoffset);
    if ( HBar->isVisible() && xoffset_before != xoffset )
      HBar->drawBar();

    updateTerminal();
    flush_out();
  }
}

//----------------------------------------------------------------------
void FListBox::onMouseDown (FMouseEvent* ev)
{
  int yoffset_before, mouse_x, mouse_y;

  if (  ev->getButton() != LeftButton
     && ev->getButton() != RightButton )
  {
    return;
  }
  if ( ev->getButton() == RightButton && ! isMultiSelection() )
    return;

  if ( ! hasFocus() )
  {
    FWidget* focused_widget = getFocusWidget();
    FFocusEvent out (FocusOut_Event);
    FApplication::queueEvent(focused_widget, &out);
    setFocus();
    if ( focused_widget )
      focused_widget->redraw();
    if ( statusBar() )
      statusBar()->drawMessage();
  }

  yoffset_before = yoffset;
  mouse_x = ev->getX();
  mouse_y = ev->getY();
  if (  mouse_x > 1 && mouse_x < width
     && mouse_y > 1 && mouse_y < height )
  {
    current = yoffset + mouse_y - 1;
    if ( current > int(count()) )
      current = int(count());
    inc_search.clear();
    if ( ev->getButton() == RightButton )
    {
      if ( isMultiSelection() )
      {
        if ( isSelected(current) )
        {
          mouseSelect = false;
          unselectItem(current);
        }
        else
        {
          mouseSelect = true;
          selectItem(current);
        }
        processSelect();
        secect_from_item = current;
      }
    }
    if ( isVisible() )
      drawList();

    VBar->setValue (yoffset);
    if ( VBar->isVisible() && yoffset_before != yoffset )
      VBar->drawBar();
    updateTerminal();
    flush_out();
  }
}

//----------------------------------------------------------------------
void FListBox::onMouseUp (FMouseEvent* ev)
{
  if ( dragScroll != FListBox::noScroll )
  {
    delAllTimer();
    dragScroll = FListBox::noScroll;
    scrollDistance = 1;
    scrollTimer = false;
  }

  if ( ev->getButton() == LeftButton )
  {
    int mouse_x = ev->getX();
    int mouse_y = ev->getY();
    if (  mouse_x > 1 && mouse_x < width
       && mouse_y > 1 && mouse_y < height )
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

  if (  ev->getButton() != LeftButton
     && ev->getButton() != RightButton )
  {
    return;
  }
  if ( ev->getButton() == RightButton && ! isMultiSelection() )
    return;

  current_before = current;
  yoffset_before = yoffset;
  mouse_x = ev->getX();
  mouse_y = ev->getY();

  if (  mouse_x > 1 && mouse_x < width
     && mouse_y > 1 && mouse_y < height )
  {
    current = yoffset + mouse_y - 1;
    if ( current > int(count()) )
      current = int(count());
    inc_search.clear();

    // handle multiple selections
    if (  ev->getButton() == RightButton
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
        if ( mouseSelect )
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
    VBar->setValue (yoffset);
    if ( VBar->isVisible() && yoffset_before != yoffset )
      VBar->drawBar();
    updateTerminal();
    flush_out();
  }

  // auto-scrolling when dragging mouse outside the widget
  if ( mouse_y < 2 )
  {
    // drag up
    if ( dragScroll != FListBox::noScroll && scrollDistance < height-2 )
      scrollDistance++;
    if ( ! scrollTimer && current > 1 )
    {
      scrollTimer = true;
      addTimer(scrollRepeat);
      if ( ev->getButton() == RightButton )
        dragScroll = FListBox::scrollUpSelect;
      else
        dragScroll = FListBox::scrollUp;
    }
    if ( current == 1 )
    {
      delAllTimer();
      dragScroll = FListBox::noScroll;
    }
  }
  else if ( mouse_y >= height )
  {
    // drag down
    if ( dragScroll != FListBox::noScroll && scrollDistance < height-2 )
      scrollDistance++;
    if ( ! scrollTimer && current < int(count()) )
    {
      scrollTimer = true;
      addTimer(scrollRepeat);
      if ( ev->getButton() == RightButton )
        dragScroll = FListBox::scrollDownSelect;
      else
        dragScroll = FListBox::scrollDown;
    }
    if ( current == int(count()) )
    {
      delAllTimer();
      dragScroll = FListBox::noScroll;
    }
  }
  else
  {
    // no dragging
    delAllTimer();
    scrollTimer = false;
    scrollDistance = 1;
    dragScroll = FListBox::noScroll;
  }
}

//----------------------------------------------------------------------
void FListBox::onMouseDoubleClick (FMouseEvent* ev)
{
  int mouse_x, mouse_y;

  if ( ev->getButton() != LeftButton )
    return;

  mouse_x = ev->getX();
  mouse_y = ev->getY();

  if (  mouse_x > 1 && mouse_x < width
     && mouse_y > 1 && mouse_y < height )
  {
    if ( yoffset + mouse_y - 1 > int(count()) )
      return;
    processClick();
  }
}

//----------------------------------------------------------------------
void FListBox::onTimer (FTimerEvent*)
{
  int element_count = int(count());
  int current_before = current;
  int yoffset_before = yoffset;

  switch ( dragScroll )
  {
    case FListBox::noScroll:
      return;

    case FListBox::scrollUp:
    case FListBox::scrollUpSelect:
      if ( current_before == 1)
      {
        dragScroll = FListBox::noScroll;
        return;
      }
      current -= scrollDistance;
      if ( current < 1 )
        current=1;
      if ( current <= yoffset )
        yoffset -= scrollDistance;
      if ( yoffset < 0 )
        yoffset=0;
      break;

    case FListBox::scrollDown:
    case FListBox::scrollDownSelect:
      if ( current_before == element_count )
      {
        dragScroll = FListBox::noScroll;
        return;
      }
      current += scrollDistance;
      if ( current > element_count )
        current = element_count;
      if ( current - yoffset >= height - 1 )
        yoffset += scrollDistance;
      if ( yoffset > element_count - height + 2 )
        yoffset = element_count - height + 2;

    default:
      break;
  }

  // handle multiple selections
  if (  dragScroll == FListBox::scrollUpSelect
     || dragScroll == FListBox::scrollDownSelect )
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
        if ( mouseSelect )
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
  VBar->setValue (yoffset);
  if ( VBar->isVisible() && yoffset_before != yoffset )
    VBar->drawBar();
  updateTerminal();
  flush_out();
}

//----------------------------------------------------------------------
void FListBox::onWheel (FWheelEvent* ev)
{
  int element_count, current_before, yoffset_before, yoffset_end, wheel;

  element_count = int(count());
  current_before = current;
  yoffset_before = yoffset;
  yoffset_end = element_count - height + 2;

  if ( yoffset_end < 0 )
    yoffset_end = 0;

  wheel = ev->getWheel();

  if ( dragScroll != FListBox::noScroll )
  {
    delAllTimer();
    scrollTimer = false;
    scrollDistance = 1;
    dragScroll = FListBox::noScroll;
  }

  switch ( wheel )
  {
    case WheelUp:
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

    case WheelDown:
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

  VBar->setValue (yoffset);
  if ( VBar->isVisible() && yoffset_before != yoffset )
    VBar->drawBar();
  updateTerminal();
  flush_out();
}

//----------------------------------------------------------------------
void FListBox::onFocusIn (FFocusEvent*)
{
  if ( statusBar() )
    statusBar()->drawMessage();
  inc_search.clear();
}

//----------------------------------------------------------------------
void FListBox::onFocusOut (FFocusEvent*)
{
  if ( statusBar() )
  {
    statusBar()->clearMessage();
    statusBar()->drawMessage();
  }
  delAllTimer();
  inc_search.clear();
}

//----------------------------------------------------------------------
void FListBox::cb_VBarChange (FWidget*, void*)
{
  int distance = 1;
  int element_count = int(count());
  int yoffset_before = yoffset;
  int scrollType = VBar->getScrollType();

  switch ( scrollType )
  {
    case FScrollbar::scrollPageBackward:
      distance = height-2;
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
      distance = height-2;
    case FScrollbar::scrollStepForward:
      current += distance;
      if ( current > element_count )
        current = element_count;
      if ( current - yoffset >= height - 1 )
        yoffset += distance;
      if ( yoffset > element_count - height + 2 )
        yoffset = element_count - height + 2;
      break;

    case FScrollbar::scrollJump:
    {
      int val = VBar->getValue();
      if ( yoffset == val )
        break;
      int c = current - yoffset;
      yoffset = val;
      if ( yoffset > element_count - height + 2 )
        yoffset = element_count - height + 2;
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
      FWheelEvent wheel_ev (MouseWheel_Event, FPoint(2,2), WheelUp);
      onWheel(&wheel_ev);
    }
    break;

    case FScrollbar::scrollWheelDown:
    {
      FWheelEvent wheel_ev (MouseWheel_Event, FPoint(2,2), WheelDown);
      onWheel(&wheel_ev);
    }
    break;

    default:
      break;
  }

  if ( isVisible() )
    drawList();

  if (  scrollType >= FScrollbar::scrollStepBackward
     && scrollType <= FScrollbar::scrollPageForward )
  {
    VBar->setValue (yoffset);
    if ( VBar->isVisible() && yoffset_before != yoffset )
      VBar->drawBar();
    updateTerminal();
    flush_out();
  }
}

//----------------------------------------------------------------------
void FListBox::cb_HBarChange (FWidget*, void*)
{
  int distance = 1;
  int xoffset_before = xoffset;
  int xoffset_end = maxLineWidth - width + nf_offset + 4;
  int scrollType = HBar->getScrollType();

  switch ( scrollType )
  {
    case FScrollbar::scrollPageBackward:
      distance = width - nf_offset - 4;
    case FScrollbar::scrollStepBackward:
      xoffset -= distance;
      if ( xoffset < 0 )
        xoffset = 0;
      break;

    case FScrollbar::scrollPageForward:
      distance = width - nf_offset - 4;
    case FScrollbar::scrollStepForward:
      xoffset += distance;
      if ( xoffset > maxLineWidth - width + nf_offset + 4 )
        xoffset = maxLineWidth - width + nf_offset + 4;
      if ( xoffset < 0 )
        xoffset = 0;
      break;

    case FScrollbar::scrollJump:
    {
      int val = HBar->getValue();
      if ( xoffset == val )
        break;
      xoffset = val;
      if ( xoffset > maxLineWidth - width + nf_offset + 4 )
        xoffset = maxLineWidth - width + nf_offset + 4;
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

  if (  scrollType >= FScrollbar::scrollStepBackward
     && scrollType <= FScrollbar::scrollWheelDown )
  {
    HBar->setValue (xoffset);
    if ( HBar->isVisible() && xoffset_before != xoffset )
      HBar->drawBar();
    updateTerminal();
    flush_out();
  }
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

  if ( len > maxLineWidth )
  {
    maxLineWidth = len;
    if ( len >= width - nf_offset - 3 )
    {
      HBar->setMaximum(maxLineWidth - width + nf_offset + 4);
      HBar->setPageSize(maxLineWidth, width - nf_offset - 4);
      HBar->calculateSliderValues();
      if ( ! HBar->isVisible() )
        HBar->setVisible();
    }
  }
  FListBoxItem listItem (item);
  listItem.brackets = b;
  listItem.selected = s;
  data.push_back (listItem);

  element_count = int(count());
  VBar->setMaximum(element_count - height + 2);
  VBar->setPageSize(element_count, height - 2);
  VBar->calculateSliderValues();
  if ( ! VBar->isVisible() && element_count >= height - 1 )
    VBar->setVisible();
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

  if ( int(count()) < item )
    return;

  data.erase (data.begin() + item - 1);

  element_count = int(count());
  maxLineWidth = 0;

  for (int i=0; i < element_count; i++)
  {
    int len = int(data[uInt(i)].getText().getLength());
    if ( len > maxLineWidth )
      maxLineWidth = len;
  }
  HBar->setMaximum(maxLineWidth - width + nf_offset + 4);
  HBar->setPageSize(maxLineWidth, width - nf_offset - 4);
  if ( HBar->isVisible() && maxLineWidth < width - nf_offset - 3 )
    HBar->hide();

  VBar->setMaximum(element_count - height + 2);
  VBar->setPageSize(element_count, height - 2);
  if ( VBar->isVisible() && element_count < height - 1 )
    VBar->hide();

  if ( current >= item && current > 1 )
    current--;
  if ( current > element_count )
    current = element_count;
  if ( yoffset > element_count - height + 2 )
    yoffset = element_count - height + 2;
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
  maxLineWidth = 0;
  last_current = -1;
  last_yoffset = -1;

  VBar->setMinimum(0);
  VBar->setValue(0);
  VBar->hide();

  HBar->setMinimum(0);
  HBar->setValue(0);
  HBar->hide();

  // clear list from screen
  setColor (wc.list_fg, wc.list_bg);
  size = width - 2;
  blank = new char[size+1];
  memset(blank, ' ', uLong(size));
  blank[size] = '\0';

  for (int y=0; y < height-2; y++)
  {
    gotoxy (xpos+xmin, ypos+ymin+y);
    print (blank);
  }
  delete[] blank;
}

//----------------------------------------------------------------------
void FListBox::setText (FString txt)
{
  text = txt;
}
