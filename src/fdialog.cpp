// fdialog.cpp
// class FDialog

#include "fapp.h"
#include "fdialog.h"
#include "fstatusbar.h"


//----------------------------------------------------------------------
// class FDialog
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FDialog::FDialog(FWidget* parent) : FWindow(parent)
{
  this->init();
}

//----------------------------------------------------------------------
FDialog::FDialog (const FString& txt, FWidget* parent) : FWindow(parent)
{
  this->init();
  setText(txt);
}

//----------------------------------------------------------------------
FDialog::~FDialog()  // destructor
{
  delete accelerator_list;
  accelerator_list = 0;
  activatePrevWindow();
  delWindow(this);

  FApplication* fapp = static_cast<FApplication*>(getRootWidget());
  if ( ! fapp->quit_now )
  {
    const FRect& geometry = getGeometryGlobalShadow();
    restoreVTerm (geometry);

    parentWidget()->redraw();
  }

  if ( window_list && ! window_list->empty() )
  {
    widgetList::const_iterator iter, end;
    iter = window_list->begin();
    end  = window_list->end();

    while ( iter != end )
    {
      putArea ((*iter)->getGlobalPos(), (*iter)->getVWin());
      if ( ! maximized && ((*iter)->getFlags() & SHADOW) != 0 )
        static_cast<FDialog*>(*iter)->drawDialogShadow();
      ++iter;
    }
    setFocusWidget(0);
    updateTerminal();
    flush_out();
  }

  if ( vwin != 0 )
  {
    if ( vwin->changes != 0 )
      delete[] vwin->changes;
    if ( vwin->text != 0 )
      delete[] vwin->text;
    delete vwin;
  }
  if ( isModal() )
    unsetModal();
}

// private methods of FDialog
//----------------------------------------------------------------------
void FDialog::init()
{
  flags = 0;
  result_code = FDialog::Reject;
  width  = 10;
  height = 10;
  xmin = 1;
  ymin = 1;
  xmax = width;
  ymax = height;
  client_xmin = 1;
  client_ymin = 1;
  client_xmax = width;
  client_ymax = height;
  top_padding    = 2;
  left_padding   = 1;
  bottom_padding = 1;
  right_padding  = 1;
  createArea (vwin);
  setGeometry (1, 1, 10, 10, false);  // initialize geometry values
  focus_widget = 0;
  this->text = "";
  maximized = false;
  ignore_padding = true;
  window_object  = true;
  addWindow(this);
  setActiveWindow(this);

  foregroundColor = wc.dialog_fg;
  backgroundColor = wc.dialog_bg;

  if ( hasFocus() )
    this->flags |= FOCUS;
  if ( isEnabled() )
    this->flags |= ACTIVE;

  FWidget* old_focus = FWidget::getFocusWidget();
  if ( old_focus )
  {
    setFocus();
    old_focus->redraw();
  }
  accelerator_list = new Accelerators;
}

//----------------------------------------------------------------------
void FDialog::drawBorder()
{
  int x1, x2, y1, y2;
  x1 = xpos+xmin-1;
  x2 = xpos+xmin-2+width;
  y1 = ypos+ymin;
  y2 = ypos+ymin-2+height;

  if ( isNewFont() )
  {
    int fg;
    if ( ! isRootWidget() )
      fg = parentWidget()->getForegroundColor();
    else
      fg = wc.term_fg;
    for (int y=y1; y <= y2; y++)
    {
      setColor (fg, backgroundColor);
      gotoxy (x1, y);
      // border left ⎸
      print (fc::NF_border_line_left);
      gotoxy (x2, y);
      // border right⎹
      print (fc::NF_rev_border_line_right);
    }
    if ( (flags & SHADOW) == 0 )
    {
      setColor (fg, backgroundColor);
      gotoxy (x1, y2);
      // lower left corner border ⎣
      print (fc::NF_border_corner_lower_left);
      for (int x=1; x < width-1; x++) // low line _
        print (fc::NF_border_line_bottom);
      gotoxy (x2, y2);
      // lower right corner border ⎦
      print (fc::NF_rev_border_corner_lower_right);
    }
  }
  else
  {
    gotoxy (x1, y1);
    print (fc::BoxDrawingsDownAndRight); // ┌
    for (int x=x1+1; x < x2; x++)
      print (fc::BoxDrawingsHorizontal); // ─
    print (fc::BoxDrawingsDownAndLeft);  // ┐

    gotoxy (x1, y2);
    print (fc::BoxDrawingsUpAndRight);   // └
    for (int x=x1+1; x < x2; x++)
      print (fc::BoxDrawingsHorizontal); // ─
    print (fc::BoxDrawingsUpAndLeft);    // ┘

    for (int y=y1+1; y < y2; y++)
    {
      gotoxy (x1, y);
      print (fc::BoxDrawingsVertical);   // │
      gotoxy (x2, y);
      print (fc::BoxDrawingsVertical);   // │
    }
  }
}

//----------------------------------------------------------------------
void FDialog::drawTitleBar()
{
  int i,x;
  uInt length = text.getLength();

  // draw the title button
  gotoxy (xpos+xmin-1, ypos+ymin-1);
  setColor (wc.titlebar_button_fg, wc.titlebar_button_bg);
  if ( isNewFont() )
  {
    print (fc::NF_rev_menu_button1);
    print (fc::NF_rev_menu_button2);
    print (fc::NF_rev_menu_button3);
  }
  else
    print (" - ");

  // fill with spaces (left of the title)
  if ( isActiveWindow() )
    setColor (wc.titlebar_active_fg, wc.titlebar_active_bg);
  else
    setColor (wc.titlebar_inactive_fg, wc.titlebar_inactive_bg);

  if ( isMonochron() )
    setReverse(true);

  i = width - 3 - int(length);
  i = int(i/2);

  for (x=1; x <= i; x++)
    print (' ');

  // the title bar text
  print (text);

  // fill the rest of the bar
  for (; x+1+int(length) <= width-2; x++)
    print (' ');

  if ( isMonochron() )
    setReverse(false);

/* print the number of window in stack */
//gotoxy(xpos+xmin+width-4, ypos+ymin-1);
//printf ("(%d)", getWindowLayer(this));

}

// protected methods of FDialog
//----------------------------------------------------------------------
void FDialog::done(int result)
{
  hide();
  result_code = result;
}

//----------------------------------------------------------------------
void FDialog::drawDialogShadow()
{
  if ((flags & TRANS_SHADOW) != 0)
  {
    // transparent shadow
    drawShadow();
    if ( isNewFont() && ((flags & SCROLLABLE) == 0) )
    {
      FTerm::char_data ch;
      // left of the shaddow ▀▀
      gotoxy (xpos+xmin-1, ypos+ymin-1+height);
      for (int x=0; x <= 1; x++)
      {
        ch = getCoveredCharacter (xpos+xmin-1+x, ypos+ymin-1+height, this);
        setColor (wc.shadow_fg, ch.bg_color);
        // high line ⎺
        print (fc::NF_border_line_upper);
      }
    }
  }
  else
  {
    drawShadow();
    FTerm::char_data ch;
    ch = getCoveredCharacter (xpos+xmin-1, ypos+ymin-1+height, this);
    // left of the shaddow ▀▀
    gotoxy (xpos+xmin-1, ypos+ymin-1+height);

    if ( isNewFont() && ((flags & SCROLLABLE) == 0) )
    {
      setColor (wc.shadow_fg, ch.bg_color);
      // high line ⎺
      print (fc::NF_border_line_upper);
    }
    else
    {
      setColor(ch.fg_color, ch.bg_color);

      if ( ch.bold )
        setBold();
      if ( ch.reverse )
        setReverse();
      if ( ch.underline )
        setUnderline();

      print (ch.code);

      if ( ch.underline )
        unsetUnderline();
      if ( ch.reverse )
        unsetReverse();
      if ( ch.bold )
        unsetBold();
    }
  }
}

//----------------------------------------------------------------------
void FDialog::draw()
{
  if ( maximized && ! isRootWidget() )
  {
    xpos = 1;
    ypos = 1;
    width = xmax;
    height = ymax;
  }
  // fill the background
  setColor (foregroundColor, backgroundColor);
  setUpdateVTerm(false);
  clrscr();
  drawBorder();
  drawTitleBar();

  if ( ! maximized && (flags & SHADOW) != 0 )
    drawDialogShadow();

  if ( (flags & RESIZEABLE) != 0 )
  {
    if ( maximized )
    {
      if ( isNewFont() )
      {
        gotoxy (xpos+xmin+width-3, ypos+ymin-1);
        setColor (wc.titlebar_button_fg, wc.titlebar_button_bg);
        print (fc::NF_rev_down_pointing_triangle1);
        print (fc::NF_rev_down_pointing_triangle2);
      }
      else
      {
        gotoxy (xpos+xmin+width-4, ypos+ymin-1);
        setColor (wc.titlebar_button_fg, wc.titlebar_button_bg);
        print (' ');
        print (fc::BlackDownPointingTriangle);  // ▼
        print (' ');
      }
    }
    else
    {
      if ( isNewFont() )
      {
        gotoxy (xpos+xmin+width-3, ypos+ymin-1);
        setColor (wc.titlebar_button_fg, wc.titlebar_button_bg);
        print (fc::NF_rev_up_pointing_triangle1);
        print (fc::NF_rev_up_pointing_triangle2);
      }
      else
      {
        gotoxy (xpos+xmin+width-4, ypos+ymin-1);
        setColor (wc.titlebar_button_fg, wc.titlebar_button_bg);
        print (' ');
        print (fc::BlackUpPointingTriangle);  // ▲
        print (' ');
      }
    }
  }
  setUpdateVTerm(true);
}

//----------------------------------------------------------------------
void FDialog::onShow (FShowEvent*)
{
}

//----------------------------------------------------------------------
void FDialog::onHide (FHideEvent*)
{
}

//----------------------------------------------------------------------
void FDialog::onClose (FCloseEvent* event)
{
  event->accept();
  result_code = FDialog::Reject;
}

// public methods of FDialog
//----------------------------------------------------------------------
void FDialog::onKeyPress (FKeyEvent* event)
{
  if ( ! isEnabled() || this == getMainWidget() )
    return;

  if (  event->key() == fc::Fkey_escape
     || event->key() == fc::Fkey_escape_mintty )
  {
    event->accept();
    if ( isModal() )
      done (FDialog::Reject);
    else
      close();
  }
}

//----------------------------------------------------------------------
void FDialog::onMouseDown (FMouseEvent* event)
{
  int mouse_x = event->getX();
  int mouse_y = event->getY();

  if ( event->getButton() == LeftButton )
  {
    // click on titlebar or window: raise + activate
    if ( mouse_x >= 4 && mouse_x <= width && mouse_y == 1 )
      TitleBarClickPos.setPoint (event->getGlobalX(), event->getGlobalY());
    else
      TitleBarClickPos.setPoint (0,0);

    bool has_raised = raiseWindow();

    if ( ! isActiveWindow() )
    {
      FWidget* old_focus = FWidget::getFocusWidget();
      setActiveWindow(this);
      if ( focus_widget )
      {
        focus_widget->setFocus();
        focus_widget->redraw();
        if ( old_focus )
          old_focus->redraw();
      }
      else if ( focusFirstChild() )
        if ( old_focus )
          old_focus->redraw();
    }
    if ( has_raised )
      redraw();
  }

  if ( event->getButton() == RightButton )
  {
    // click on titlebar: just activate
    if ( mouse_x >= 4 && mouse_x <= width && mouse_y == 1 )
    {
      if ( ! isActiveWindow() )
      {
        FWidget* old_focus = FWidget::getFocusWidget();
        setActiveWindow(this);
        if ( focus_widget )
        {
          focus_widget->setFocus();
          focus_widget->redraw();
          if ( old_focus )
            old_focus->redraw();
        }
        else if ( focusFirstChild() )
          if ( old_focus )
            old_focus->redraw();
      }
    }
  }

  if ( event->getButton() == MiddleButton )
  {
    // click on titlebar: lower + activate
    if ( mouse_x >= 4 && mouse_x <= width && mouse_y == 1 )
    {
      bool has_lowered = lowerWindow();

      if ( ! isActiveWindow() )
      {
        FWidget* old_focus = FWidget::getFocusWidget();
        setActiveWindow(this);
        if ( focus_widget )
        {
          focus_widget->setFocus();
          focus_widget->redraw();
          if ( old_focus )
            old_focus->redraw();
        }
        else if ( focusFirstChild() )
          if ( old_focus )
            old_focus->redraw();
      };

      if ( has_lowered )
        updateTerminal();
    }
  }
}

//----------------------------------------------------------------------
void FDialog::onMouseUp (FMouseEvent* event)
{
  int titlebar_x = TitleBarClickPos.getX();
  int titlebar_y = TitleBarClickPos.getY();

  if ( event->getButton() == LeftButton )
  {
    if (  ! TitleBarClickPos.isNull()
       && titlebar_x >= xpos+xmin+3
       && titlebar_x <= xpos+xmin-1+width
       && titlebar_y == ypos+ymin-1 )
    {
      FPoint currentPos(getGeometry().getX(), getGeometry().getY());
      FPoint deltaPos = event->getGlobalPos() - TitleBarClickPos;
      move (currentPos + deltaPos);
      TitleBarClickPos = event->getGlobalPos();
    }
  }
}

//----------------------------------------------------------------------
void FDialog::onMouseMove (FMouseEvent* event)
{
  if ( event->getButton() == LeftButton )
  {
    if ( ! TitleBarClickPos.isNull() )
    {
      FPoint currentPos(getGeometry().getX(), getGeometry().getY());
      FPoint deltaPos = event->getGlobalPos() - TitleBarClickPos;
      move (currentPos + deltaPos);
      TitleBarClickPos = event->getGlobalPos();
    }
  }
}

//----------------------------------------------------------------------
void FDialog::onMouseDoubleClick (FMouseEvent* event)
{
  int x, y;

  if ( event->getButton() != LeftButton )
    return;

  x = xpos + xmin - 1;
  y = ypos + ymin - 1;
  FRect title_button(x, y, 3, 1);

  FPoint gPos = event->getGlobalPos();
  if ( title_button.contains(gPos) )
  {
    setClickedWidget(0);
    close();
  }
}

//----------------------------------------------------------------------
void FDialog::onWindowActive (FEvent*)
{
  if ( isVisible() && isShown() )
    drawTitleBar();

  if ( ! FWidget::getFocusWidget() )
  {
    if ( focus_widget && focus_widget->isVisible() && focus_widget->isShown() )
    {
      focus_widget->setFocus();
      focus_widget->redraw();
      if ( statusBar() )
        statusBar()->drawMessage();
    }
    else
      focusFirstChild();
  }
}

//----------------------------------------------------------------------
void FDialog::onWindowInactive (FEvent*)
{
  if ( isVisible() && isEnabled() )
    drawTitleBar();
}

//----------------------------------------------------------------------
void FDialog::onWindowRaised (FEvent*)
{
  widgetList::const_iterator iter, end;
  putArea (getGlobalPos(), vwin);

  if ( ! window_list )
    return;
  if ( window_list->empty() )
    return;

  // redraw shaddow of the other windows
  iter = window_list->begin();
  end  = window_list->end();

  while ( iter != end )
  {
    if ( *iter != this
    && ! maximized
    && ((*iter)->getFlags() & SHADOW) != 0 )
      static_cast<FDialog*>(*iter)->drawDialogShadow();
    ++iter;
  }
}

//----------------------------------------------------------------------
void FDialog::onWindowLowered (FEvent*)
{
  widgetList::const_iterator iter, end;

  if ( ! window_list )
    return;
  if ( window_list->empty() )
    return;

  iter = window_list->begin();
  end  = window_list->end();

  while ( iter != end )
  {
    putArea ((*iter)->getGlobalPos(), (*iter)->getVWin());
    if ( ! maximized && ((*iter)->getFlags() & SHADOW) != 0 )
      static_cast<FDialog*>(*iter)->drawDialogShadow();
    ++iter;
  }
}

//----------------------------------------------------------------------
void FDialog::show()
{
  if ( ! isVisible() )
    return;

  FWidget::show();

  // set the cursor to the focus widget
  if (  FWidget::getFocusWidget()
     && FWidget::getFocusWidget()->isVisible()
     && FWidget::getFocusWidget()->hasVisibleCursor()
     && FWidget::getFocusWidget()->isCursorInside() )
  {
    FWidget::getFocusWidget()->setCursor();
    showCursor();
    flush_out();
  }

  if ( isModal() )
  {
    FApplication* fapp = static_cast<FApplication*>(getRootWidget());
    fapp->enter_loop();

    if ( this == getMainWidget() )
      fapp->quit();
  }
}

//----------------------------------------------------------------------
void FDialog::hide()
{
  FWidget::hide();

  if ( isModal() )
  {
    FApplication* fapp = static_cast<FApplication*>(getRootWidget());
    fapp->exit_loop();
  }
}

//----------------------------------------------------------------------
int FDialog::exec()
{
  result_code = FDialog::Reject;
  show();
  return result_code;
}

//----------------------------------------------------------------------
void FDialog::move (const FPoint& pos)
{
  move( pos.getX(), pos.getY() );
}

//----------------------------------------------------------------------
void FDialog::move (int x, int y)
{
  int dx, dy, old_x, old_y;

  if ( x == xpos && y == ypos )
    return;
  if (  x+width < 1 || x > getColumnNumber() || y < 1 || y > getLineNumber() )
    return;

  dx = xpos - x;
  dy = ypos - y;
  old_x = xpos;
  old_y = ypos;
  short& rsw = shadow.x_ref();  // right shadow width;
  short& bsh = shadow.y_ref();  // bottom shadow height
  oldGeometry = getGeometryShadow();

  FWidget::move(x,y);
  xpos = x;
  ypos = y;

  putArea (getGlobalPos(), vwin);

  if ( getGeometry().overlap(oldGeometry) )
  {
    // dx > 0 : move left
    // dx = 0 : move vertical
    // dx < 0 : move right
    // dy > 0 : move up
    // dy = 0 : move horizontal
    // dy < 0 : move down

    if ( dx > 0 )
    {
      if ( dy > 0 )
        restoreVTerm (old_x+width+rsw-dx, old_y, dx, height+bsh-dy);
      else
        restoreVTerm (old_x+width+rsw-dx, old_y+abs(dy), dx, height+bsh-abs(dy));
    }
    else
    {
      if ( dy > 0 )
        restoreVTerm (old_x, old_y, abs(dx), height+bsh-dy);
      else
        restoreVTerm (old_x, old_y+abs(dy), abs(dx), height+bsh-abs(dy));
    }
    if ( dy > 0 )
      restoreVTerm (old_x, old_y+height+bsh-dy, width+rsw, dy);
    else
      restoreVTerm (old_x, old_y, width+rsw, abs(dy));
  }
  else
  {
    restoreVTerm (old_x, old_y, width+rsw, height+bsh);
  }

  if ( ! maximized && (flags & SHADOW) != 0 )
    drawDialogShadow();

  // handle overlaid windows
  if ( window_list && ! window_list->empty() )
  {
    bool overlaid = false;
    widgetList::const_iterator iter, end;
    iter = window_list->begin();
    end  = window_list->end();

    while ( iter != end )
    {
      if ( overlaid )
      {
        putArea ((*iter)->getGlobalPos(), (*iter)->getVWin());
        if ( ! maximized && ((*iter)->getFlags() & SHADOW) != 0 )
          static_cast<FDialog*>(*iter)->drawDialogShadow();
      }
      if ( vwin == (*iter)->getVWin() )
        overlaid = true;
      ++iter;
    }
  }

  FWidget::adjustSize();

  // set the cursor to the focus widget
  if (  FWidget::getFocusWidget()
     && FWidget::getFocusWidget()->isVisible()
     && FWidget::getFocusWidget()->hasVisibleCursor() )
  {
    FPoint cursor_pos = FWidget::getFocusWidget()->getCursorPos();
    cursor_pos -= FPoint(dx,dy);
    if ( ! FWidget::getFocusWidget()->setCursorPos(cursor_pos) )
      hideCursor();
  }
  updateTerminal();
}

//----------------------------------------------------------------------
void FDialog::setWidth (int w, bool adjust)
{
  int old_width = width;
  FWidget::setWidth (w, adjust);
  if ( vwin && width != old_width )
    resizeArea (vwin);
}

//----------------------------------------------------------------------
void FDialog::setHeight (int h, bool adjust)
{
  int old_height = height;
  FWidget::setHeight (h, adjust);
  if ( vwin && height != old_height )
    resizeArea (vwin);
}

//----------------------------------------------------------------------
void FDialog::setGeometry (int x, int y, int w, int h, bool adjust)
{
  int old_width = width;
  int old_height = height;
  FWidget::setGeometry (x, y, w, h, adjust);
  if ( vwin && (width != old_width || height != old_height) )
    resizeArea (vwin);
}

//----------------------------------------------------------------------
FWidget* FDialog::getFocusWidget() const
{
  return static_cast<FWidget*>(focus_widget);
}

//----------------------------------------------------------------------
void FDialog::setFocusWidget (FWidget* obj)
{
  focus_widget = obj;
}

//----------------------------------------------------------------------
bool FDialog::setFocus (bool on)
{
  FWidget::setFocus(on);

  if ( on )
    this->flags |= FOCUS;
  else
    this->flags &= ~FOCUS;
  return on;
}

//----------------------------------------------------------------------
bool FDialog::setModal (bool on)
{
  if ( isModal() == on )
    return true;

  if ( on )
  {
    this->flags |= MODAL;
    modal_dialogs++;
  }
  else
  {
    this->flags &= ~MODAL;
    modal_dialogs--;
  }
  return on;
}

//----------------------------------------------------------------------
bool FDialog::setTransparentShadow (bool on)
{
  if ( on )
  {
    this->flags |= SHADOW;
    this->flags |= TRANS_SHADOW;
    shadow.setPoint(2,1);
    adjustWidgetSizeShadow = getGeometry() + getShadow();
    adjustWidgetSizeGlobalShadow = getGeometryGlobal() + getShadow();
  }
  else
  {
    this->flags &= ~SHADOW;
    this->flags &= ~TRANS_SHADOW;
    shadow.setPoint(0,0);
    adjustWidgetSizeShadow = getGeometry() + getShadow();
    adjustWidgetSizeGlobalShadow = getGeometryGlobal() + getShadow();
  }
  resizeArea (vwin);
  return on;
}

//----------------------------------------------------------------------
bool FDialog::setShadow (bool on)
{
  if ( on )
  {
    this->flags |= SHADOW;
    this->flags &= ~TRANS_SHADOW;
    shadow.setPoint(1,1);
    adjustWidgetSizeShadow = getGeometry() + getShadow();
    adjustWidgetSizeGlobalShadow = getGeometryGlobal() + getShadow();
  }
  else
  {
    this->flags &= ~SHADOW;
    this->flags &= ~TRANS_SHADOW;
    shadow.setPoint(0,0);
    adjustWidgetSizeShadow = getGeometry() + getShadow();
    adjustWidgetSizeGlobalShadow = getGeometryGlobal() + getShadow();
  }
  resizeArea (vwin);
  return on;
}

//----------------------------------------------------------------------
bool FDialog::setScrollable (bool on)
{
  if ( on )
    this->flags |= SCROLLABLE;
  else
    this->flags &= ~SCROLLABLE;
  return on;
}

//----------------------------------------------------------------------
bool FDialog::setResizeable (bool on)
{
  if ( on )
    this->flags |= RESIZEABLE;
  else
    this->flags &= ~RESIZEABLE;
  return on;
}

//----------------------------------------------------------------------
bool FDialog::setMaximized()
{
  if ( maximized )
    return true;

  maximized = true;
 // setGeometry(1, 1, xmax, ymax);
  return maximized;
}
