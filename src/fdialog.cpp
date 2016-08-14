// File: fdialog.cpp
// Provides: class FDialog

#include "fapp.h"
#include "fdialog.h"
#include "fstatusbar.h"


//----------------------------------------------------------------------
// class FDialog
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FDialog::FDialog(FWidget* parent)
  : FWindow(parent)
  , tb_text()
  , result_code(FDialog::Reject)
  , maximized(false)
  , TitleBarClickPos()
  , oldGeometry()
  , dialog_menu()
  , dgl_menuitem()
{
  init();
}

//----------------------------------------------------------------------
FDialog::FDialog (const FString& txt, FWidget* parent)
  : FWindow(parent)
  , tb_text(txt)
  , result_code(FDialog::Reject)
  , maximized(false)
  , TitleBarClickPos()
  , oldGeometry()
  , dialog_menu()
  , dgl_menuitem()
{
  init();
}

//----------------------------------------------------------------------
FDialog::~FDialog()  // destructor
{
  FApplication* fapp = static_cast<FApplication*>(getRootWidget());
  bool is_quit = fapp->isQuit();
  delete dialog_menu;
  dgl_menuitem = 0;
  delete accelerator_list;
  accelerator_list = 0;

  if ( ! is_quit )
    switchToPrevWindow();

  delWindow(this);
  delDialog(this);

  if ( ! is_quit )
  {
    const FRect& geometry = getGeometryGlobalShadow();
    restoreVTerm (geometry);
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
  FMenuItem* close_item;
  FWidget*   old_focus;
  FWidget*   rootObj = getRootWidget();

  xmin = 1 + rootObj->getLeftPadding();
  ymin = 1 + rootObj->getTopPadding();
  xmax = rootObj->getWidth();
  ymax = rootObj->getHeight();
  width  = 10;
  height = 10;
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
  ignore_padding = true;
  window_object  = true;
  dialog_object  = true;
  addDialog(this);
  addWindow(this);
  setActiveWindow(this);

  foregroundColor = wc.dialog_fg;
  backgroundColor = wc.dialog_bg;

  if ( hasFocus() )
    flags |= fc::focus;

  if ( isEnabled() )
    flags |= fc::active;

  old_focus = FWidget::getFocusWidget();

  if ( old_focus )
  {
    setFocus();
    old_focus->redraw();
  }

  accelerator_list = new Accelerators();
  dialog_menu = new FMenu ("-", this);
  dialog_menu->move (xpos, ypos+1);
  dgl_menuitem = dialog_menu->getItem();

  if ( dgl_menuitem )
  {
    dgl_menuitem->ignorePadding();
    dgl_menuitem->unsetFocusable();
  }

  close_item = new FMenuItem ("&Close", dialog_menu);
  close_item->setStatusbarMessage ("Close window");

  close_item->addCallback
  (
    "clicked",
    _METHOD_CALLBACK (this, &FDialog::cb_close)
  );
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
    short fg;

    if ( ! isRootWidget() )
      fg = getParentWidget()->getForegroundColor();
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

    if ( (flags & fc::shadow) == 0 )
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
  uInt length = tb_text.getLength();

  // draw the title button
  gotoxy (xpos+xmin-1, ypos+ymin-1);
  setColor (wc.titlebar_button_fg, wc.titlebar_button_bg);

  if ( isMonochron() )
  {
    if ( isActiveWindow() )
      setReverse(false);
    else
      setReverse(true);
  }


  if ( isNewFont() )
  {
    print (fc::NF_rev_menu_button1);
    print (fc::NF_rev_menu_button2);
    print (fc::NF_rev_menu_button3);
  }
  else if ( isMonochron() )
  {
    print ('[');

    if ( dgl_menuitem )
      print (dgl_menuitem->getText());
    else
      print ('-');

    print (']');
  }
  else
  {
    print (' ');

    if ( dgl_menuitem )
      print (dgl_menuitem->getText());
    else
      print ('-');

    print (' ');
  }

  // fill with spaces (left of the title)
  if ( getMaxColor() < 16 )
    setBold();

  if ( isActiveWindow() || dialog_menu->isVisible() )
    setColor (wc.titlebar_active_fg, wc.titlebar_active_bg);
  else
    setColor (wc.titlebar_inactive_fg, wc.titlebar_inactive_bg);

  i = width - 3 - int(length);
  i = int(i/2);

  for (x=1; x <= i; x++)
    print (' ');

  // the title bar text
  if ( tb_text )
    print (tb_text);

  // fill the rest of the bar
  for (; x+1+int(length) < width-1; x++)
    print (' ');

  if ( getMaxColor() < 16 )
    unsetBold();

  if ( isMonochron() )
    setReverse(false);

/* print the number of window in stack */
//gotoxy(xpos+xmin+width-4, ypos+ymin-1);
//printf ("(%d)", getWindowLayer(this));

}

//----------------------------------------------------------------------
void FDialog::leaveMenu()
{
  dialog_menu->unselectItem();
  dialog_menu->hide();
  activateWindow();
  raiseWindow();
  getWindowFocusWidget()->setFocus();
  redraw();

  if ( statusBar() )
    statusBar()->drawMessage();

  updateTerminal();
  flush_out();
}

//----------------------------------------------------------------------
void FDialog::cb_close (FWidget*, void*)
{
  dialog_menu->unselectItem();
  dialog_menu->hide();
  setClickedWidget(0);
  drawTitleBar();
  close();
}

//----------------------------------------------------------------------
void FDialog::addDialog (FWidget* obj)
{
  // add the dialog object obj to the dialog list
  if ( dialog_list )
    dialog_list->push_back(obj);
}

//----------------------------------------------------------------------
void FDialog::delDialog (FWidget* obj)
{
  // delete the dialog object obj from the dialog list
  if ( ! dialog_list || dialog_list->empty() )
    return;

  widgetList::iterator iter;
  iter = dialog_list->begin();

  while ( iter != dialog_list->end() )
  {
    if ( (*iter) == obj )
    {
      dialog_list->erase(iter);
      return;
    }

    ++iter;
  }
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
  if ((flags & fc::trans_shadow) != 0)
  {
    // transparent shadow
    drawShadow();

    if ( isNewFont() && ((flags & fc::scrollable) == 0) )
    {
      FOptiAttr::char_data ch;
      // left of the shadow ▀▀
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
    FOptiAttr::char_data ch;

    if ( isMonochron() )
      return;

    drawShadow();
    ch = getCoveredCharacter (xpos+xmin-1, ypos+ymin-1+height, this);
    // left of the shadow ▀▀
    gotoxy (xpos+xmin-1, ypos+ymin-1+height);

    if ( isNewFont() && ((flags & fc::scrollable) == 0) )
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

      if ( ch.dim )
        setDim();

      if ( ch.italic )
        setItalic();

      if ( ch.underline )
        setUnderline();

      if ( ch.blink )
        setBlink();

      if ( ch.reverse )
        setReverse();

      if ( ch.standout )
        setStandout();

      if ( ch.invisible )
        setInvisible();

      if ( ch.protect )
        setProtected();

      if ( ch.crossed_out )
        setCrossedOut();

      if ( ch.dbl_underline )
        setDoubleUnderline();

      if ( ch.alt_charset )
        setAltCharset (true);

      if ( ch.pc_charset )
        setPCcharset (true);

      print (ch.code);
      setNormal();
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

  updateVTerm(false);
  // fill the background
  setColor (foregroundColor, backgroundColor);

  if ( isMonochron() )
    setReverse(true);

  clearArea();
  drawBorder();
  drawTitleBar();

  if ( ! maximized && (flags & fc::shadow) != 0 )
    drawDialogShadow();

  if ( (flags & fc::resizeable) != 0 )
  {
    if ( isMonochron() )
      setReverse(false);

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

  if ( isMonochron() )
    setReverse(false);

  updateVTerm(true);
}

//----------------------------------------------------------------------
void FDialog::onShow (FShowEvent*)
{ }

//----------------------------------------------------------------------
void FDialog::onHide (FHideEvent*)
{ }

//----------------------------------------------------------------------
void FDialog::onClose (FCloseEvent* ev)
{
  ev->accept();
  result_code = FDialog::Reject;
}

// public methods of FDialog
//----------------------------------------------------------------------
void FDialog::onKeyPress (FKeyEvent* ev)
{
  if ( ! isEnabled() || this == getMainWidget() )
    return;

  if (  ev->key() == fc::Fkey_escape
     || ev->key() == fc::Fkey_escape_mintty )
  {
    ev->accept();

    if ( isModal() )
      done (FDialog::Reject);
    else
      close();
  }
}

//----------------------------------------------------------------------
void FDialog::onMouseDown (FMouseEvent* ev)
{
  int mouse_x = ev->getX();
  int mouse_y = ev->getY();

  if ( ev->getButton() == fc::LeftButton )
  {
    bool has_raised;

    // click on titlebar or window: raise + activate
    if ( mouse_x >= 4 && mouse_x <= width && mouse_y == 1 )
      TitleBarClickPos.setPoint (ev->getGlobalX(), ev->getGlobalY());
    else
      TitleBarClickPos.setPoint (0,0);

    has_raised = raiseWindow();

    if ( ! isActiveWindow() )
      activateDialog();

    if ( has_raised )
      redraw();

    // click on titlebar menu button
    if ( mouse_x < 4  && mouse_y == 1 )
    {
      if ( dialog_menu->isVisible() )
        leaveMenu();
      else
      {
        setOpenMenu(dialog_menu);
        dialog_menu->move (xpos, ypos+1);
        dialog_menu->setVisible();
        dialog_menu->show();
        dialog_menu->raiseWindow(dialog_menu);
        dialog_menu->redraw();
      }
    }
  }
  else  // ev->getButton() != fc::LeftButton
  {
    // click on titlebar menu button
    if ( mouse_x < 4  && mouse_y == 1 && dialog_menu->isVisible() )
      leaveMenu();  // close menu
  }

  if ( ev->getButton() == fc::RightButton )
  {
    // click on titlebar: just activate
    if ( mouse_x >= 4 && mouse_x <= width && mouse_y == 1 )
    {
      if ( ! isActiveWindow() )
        activateDialog();
    }
  }

  if ( ev->getButton() == fc::MiddleButton )
  {
    // click on titlebar: lower + activate
    if ( mouse_x >= 4 && mouse_x <= width && mouse_y == 1 )
    {
      bool has_lowered = lowerWindow();

      if ( ! isActiveWindow() )
        activateDialog();
      else if ( has_lowered )
        updateTerminal();
    }
  }
}

//----------------------------------------------------------------------
void FDialog::onMouseUp (FMouseEvent* ev)
{
  int titlebar_x = TitleBarClickPos.getX();
  int titlebar_y = TitleBarClickPos.getY();

  if ( ev->getButton() == fc::LeftButton )
  {
    int mouse_x = ev->getX();
    int mouse_y = ev->getY();

    if (  ! TitleBarClickPos.isNull()
       && titlebar_x > xpos+xmin+2
       && titlebar_x < xpos+xmin+width
       && titlebar_y == ypos+ymin-1 )
    {
      FPoint currentPos(getGeometry().getX(), getGeometry().getY());
      FPoint deltaPos = ev->getGlobalPos() - TitleBarClickPos;
      move (currentPos + deltaPos);
      TitleBarClickPos = ev->getGlobalPos();
    }

    // click on titlebar menu button
    if (  mouse_x < 4
       && mouse_y == 1
       && dialog_menu->isVisible()
       && ! dialog_menu->hasSelectedItem() )
    {
      FMenuItem* first_item;
      dialog_menu->selectFirstItem();
      first_item = dialog_menu->getSelectedItem();

      if ( first_item )
        first_item->setFocus();

      dialog_menu->redraw();

      if ( statusBar() )
        statusBar()->drawMessage();

      updateTerminal();
      flush_out();
    }
  }
}

//----------------------------------------------------------------------
void FDialog::onMouseMove (FMouseEvent* ev)
{
  if ( ev->getButton() == fc::LeftButton )
  {
    if ( ! TitleBarClickPos.isNull() )
    {
      FPoint currentPos(getGeometry().getX(), getGeometry().getY());
      FPoint deltaPos = ev->getGlobalPos() - TitleBarClickPos;
      move (currentPos + deltaPos);
      TitleBarClickPos = ev->getGlobalPos();
    }

    if ( dialog_menu->isVisible() && dialog_menu->isShown() )
    {
      // Mouse event handover to the menu
      const FRect& menu_geometry = dialog_menu->getGeometryGlobal();

      if (  dialog_menu->count() > 0
         && menu_geometry.contains(ev->getGlobalPos()) )
      {
        const FPoint& g = ev->getGlobalPos();
        const FPoint& p = dialog_menu->globalToLocalPos(g);
        int b = ev->getButton();
        FMouseEvent* _ev = new FMouseEvent (fc::MouseMove_Event, p, g, b);
        dialog_menu->mouse_down = true;
        setClickedWidget(dialog_menu);
        dialog_menu->onMouseMove(_ev);
        delete _ev;
      }
    }
  }
}

//----------------------------------------------------------------------
void FDialog::onMouseDoubleClick (FMouseEvent* ev)
{
  int x, y;

  if ( ev->getButton() != fc::LeftButton )
    return;

  x = xpos + xmin - 1;
  y = ypos + ymin - 1;
  FRect title_button(x, y, 3, 1);
  FPoint gPos = ev->getGlobalPos();

  if ( title_button.contains(gPos) )
  {
    dialog_menu->unselectItem();
    dialog_menu->hide();
    activateWindow();
    raiseWindow();
    getWindowFocusWidget()->setFocus();
    setClickedWidget(0);

    if ( isModal() )
      done (FDialog::Reject);
    else
      close();
  }
}

//----------------------------------------------------------------------
void FDialog::onAccel (FAccelEvent*)
{
  if ( ! isHiddenWindow() && ! isActiveWindow() )
  {
    bool has_raised = raiseWindow();
    activateDialog();

    if ( has_raised )
      redraw();

    updateTerminal();
  }
}

//----------------------------------------------------------------------
void FDialog::onWindowActive (FEvent*)
{
  if ( isVisible() && isShown() )
    drawTitleBar();

  if ( ! FWidget::getFocusWidget() )
  {
    if ( getWindowFocusWidget()
       && getWindowFocusWidget()->isVisible()
       && getWindowFocusWidget()->isShown() )
    {
      FWidget* win_focus_widget = getWindowFocusWidget();
      win_focus_widget->setFocus();
      win_focus_widget->redraw();
    }
    else
      focusFirstChild();
  }

  if ( statusBar() )
    statusBar()->drawMessage();

  updateTerminal();
}

//----------------------------------------------------------------------
void FDialog::onWindowInactive (FEvent*)
{
  if ( ! dialog_menu->isVisible() )
    FWindow::previous_widget = this;

  if ( isVisible() && isEnabled() )
    drawTitleBar();

  if ( hasFocus() )
    unsetFocus();
}

//----------------------------------------------------------------------
void FDialog::onWindowRaised (FEvent*)
{
  widgetList::const_iterator iter, end;

  if ( ! isVisible() || ! isShown() )
    return;

  putArea (getGlobalPos(), vwin);

  if ( ! window_list )
    return;

  if ( window_list->empty() )
    return;

  // redraw shadow of the other windows
  iter = window_list->begin();
  end  = window_list->end();

  while ( iter != end )
  {
    if ( *iter != this && ! maximized
       && ((*iter)->getFlags() & fc::shadow) != 0 )
    {
      static_cast<FDialog*>(*iter)->drawDialogShadow();
    }

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

    if ( ! maximized && ((*iter)->getFlags() & fc::shadow) != 0 )
      static_cast<FDialog*>(*iter)->drawDialogShadow();

    ++iter;
  }
}

//----------------------------------------------------------------------
void FDialog::show()
{
  if ( ! isVisible() )
    return;

  FWindow::show();

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
  FWindow::hide();

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
  old_x = getGlobalX();
  old_y = getGlobalY();
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

  if ( ! maximized && (flags & fc::shadow) != 0 )
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

        if ( ! maximized && ((*iter)->getFlags() & fc::shadow) != 0 )
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
void FDialog::activateDialog()
{
  FWidget* old_focus = FWidget::getFocusWidget();
  FWidget* win_focus_widget = getWindowFocusWidget();
  setActiveWindow(this);
  setFocus();

  if ( win_focus_widget && numOfFocusableChildren() > 1 )
  {
    win_focus_widget->setFocus();
    win_focus_widget->redraw();

    if ( old_focus )
      old_focus->redraw();
  }
  else if ( old_focus )
  {
    if ( ! focusFirstChild() )
      old_focus->unsetFocus();

    if ( ! old_focus->isWindow() )
      old_focus->redraw();
  }

  if ( statusBar() )
    statusBar()->drawMessage();

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
bool FDialog::setFocus (bool on)
{
  FWidget::setFocus(on);

  if ( on )
    flags |= fc::focus;
  else
    flags &= ~fc::focus;

  return on;
}

//----------------------------------------------------------------------
bool FDialog::setModal (bool on)
{
  if ( isModal() == on )
    return true;

  if ( on )
  {
    flags |= fc::modal;
    modal_dialogs++;
  }
  else
  {
    flags &= ~fc::modal;
    modal_dialogs--;
  }

  return on;
}

//----------------------------------------------------------------------
bool FDialog::setTransparentShadow (bool on)
{
  if ( on )
  {
    flags |= fc::shadow;
    flags |= fc::trans_shadow;
    shadow.setPoint(2,1);
    adjustWidgetSizeShadow = getGeometry() + getShadow();
    adjustWidgetSizeGlobalShadow = getGeometryGlobal() + getShadow();
  }
  else
  {
    flags &= ~fc::shadow;
    flags &= ~fc::trans_shadow;
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
  if ( isMonochron() )
    return false;

  if ( on )
  {
    flags |= fc::shadow;
    flags &= ~fc::trans_shadow;
    shadow.setPoint(1,1);
    adjustWidgetSizeShadow = getGeometry() + getShadow();
    adjustWidgetSizeGlobalShadow = getGeometryGlobal() + getShadow();
  }
  else
  {
    flags &= ~fc::shadow;
    flags &= ~fc::trans_shadow;
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
    flags |= fc::scrollable;
  else
    flags &= ~fc::scrollable;

  return on;
}

//----------------------------------------------------------------------
bool FDialog::setResizeable (bool on)
{
  if ( on )
    flags |= fc::resizeable;
  else
    flags &= ~fc::resizeable;

  return on;
}

//----------------------------------------------------------------------
bool FDialog::setMaximized()
{
  if ( maximized )
    return true;

  maximized = true;
  //setGeometry (1, 1, xmax, ymax);

  return maximized;
}
