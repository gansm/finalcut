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
  , zoom_button_pressed(false)
  , zoom_button_active(false)
  , TitleBarClickPos()
  , oldGeometry()
  , dialog_menu()
  , dgl_menuitem()
  , zoom_item()
  , close_item()
{
  init();
}

//----------------------------------------------------------------------
FDialog::FDialog (const FString& txt, FWidget* parent)
  : FWindow(parent)
  , tb_text(txt)
  , result_code(FDialog::Reject)
  , zoom_button_pressed(false)
  , zoom_button_active(false)
  , TitleBarClickPos()
  , oldGeometry()
  , dialog_menu()
  , dgl_menuitem()
  , zoom_item()
  , close_item()
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
  dialog_object  = true;
  addDialog(this);
  addWindow(this);
  setActiveWindow(this);
  setTransparentShadow();

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
  // Add the dialog menu
  dialog_menu = new FMenu ("-", this);
  dialog_menu->move (xpos, ypos+1);
  dgl_menuitem = dialog_menu->getItem();

  if ( dgl_menuitem )
  {
    dgl_menuitem->ignorePadding();
    dgl_menuitem->unsetFocusable();
  }

  zoom_item = new FMenuItem ("&Zoom", dialog_menu);
  zoom_item->setStatusbarMessage ("Enlarge or restore the window size");
  zoom_item->setDisable();

  zoom_item->addCallback
  (
    "clicked",
    _METHOD_CALLBACK (this, &FDialog::cb_zoom)
  );

  close_item = new FMenuItem ("&Close", dialog_menu);
  close_item->setStatusbarMessage ("Close this window");

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

  //if ( resize )
  //  setColor (wc.dialog_resize_fg, backgroundColor);

  if ( isNewFont() )
  {
    for (int y=y1; y <= y2; y++)
    {
      gotoxy (x1, y);
      // border left ⎸
      print (fc::NF_border_line_left);
      gotoxy (x2, y);
      // border right⎹
      print (fc::NF_rev_border_line_right);
    }

    gotoxy (x1, y2);
    // lower left corner border ⎣
    print (fc::NF_border_corner_lower_left);

    for (int x=1; x < width-1; x++) // low line _
      print (fc::NF_border_line_bottom);

    gotoxy (x2, y2);
    // lower right corner border ⎦
    print (fc::NF_rev_border_corner_lower_right);
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
  int i,x,length, zoom_btn;
  const int menu_btn = 3;

  // draw the title button
  gotoxy (xpos+xmin-1, ypos+ymin-1);

  if ( dialog_menu && dialog_menu->isVisible() )
    setColor (wc.titlebar_button_focus_fg, wc.titlebar_button_focus_bg);
  else
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

  if ( isActiveWindow() || (dialog_menu && dialog_menu->isVisible()) )
    setColor (wc.titlebar_active_fg, wc.titlebar_active_bg);
  else
    setColor (wc.titlebar_inactive_fg, wc.titlebar_inactive_bg);

  if ( (flags & fc::resizeable) == 0 )
    zoom_btn = 0;
  else if ( isNewFont() )
    zoom_btn = 2;
  else
    zoom_btn = 3;

  length = int(tb_text.getLength());
  i = width - length - menu_btn  - zoom_btn;
  i = int(i/2);

  for (x=1; x <= i; x++)
    print (' ');

  // the title bar text
  if ( tb_text )
    print (tb_text);

  // fill the rest of the bar
  for (; x+1+length < width-zoom_btn-1; x++)
    print (' ');

  if ( getMaxColor() < 16 )
    unsetBold();

  // draw the zoom/unzoom button
  if ( (flags & fc::resizeable) != 0 )
  {
    if ( zoom_button_pressed )
      setColor (wc.titlebar_button_focus_fg, wc.titlebar_button_focus_bg);
    else
      setColor (wc.titlebar_button_fg, wc.titlebar_button_bg);

    if ( isZoomed() )
    {
      if ( isNewFont() )
      {
        print (fc::NF_rev_down_pointing_triangle1);
        print (fc::NF_rev_down_pointing_triangle2);
      }
      else
      {
        if ( isMonochron() )
        {
          print ('[');
          print (fc::BlackDownPointingTriangle);  // ▼
          print (']');
        }
        else
        {
          print (' ');

          if ( isCygwinTerminal() )
            print ('v');
          else
            print (fc::BlackDownPointingTriangle);  // ▼

          print (' ');
        }
      }
    }
    else // is not zoomed
    {
      if ( isNewFont() )
      {
        print (fc::NF_rev_up_pointing_triangle1);
        print (fc::NF_rev_up_pointing_triangle2);
      }
      else
      {
        if ( isMonochron() )
        {
          print ('[');
          print (fc::BlackUpPointingTriangle);  // ▲
          print (']');
        }
        else
        {
          print (' ');

          if ( isCygwinTerminal() )
            print ('^');
          else
            print (fc::BlackUpPointingTriangle);  // ▲

          print (' ');
        }
      }
    }
  }

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

  if ( getWindowFocusWidget() )
    getWindowFocusWidget()->setFocus();

  redraw();

  if ( statusBar() )
    statusBar()->drawMessage();

  updateTerminal();
  flush_out();
}

//----------------------------------------------------------------------
void FDialog::cb_zoom (FWidget*, void*)
{
  dialog_menu->unselectItem();
  dialog_menu->hide();
  setClickedWidget(0);
  drawTitleBar();
  zoomWindow();
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
  if ( isMonochron() && (flags & fc::trans_shadow) == 0 )
    return;

  drawShadow();
  gotoxy (xpos+xmin-1, ypos+ymin-1+height);
  setTransparent();
  print(' ');
  unsetTransparent();
}

//----------------------------------------------------------------------
void FDialog::draw()
{
  /*if ( isZoomed() && ! isRootWidget() )
  {
    xpos = 1;
    ypos = 1;
    width = xmax;
    height = ymax;
  }*/

  updateVTerm(false);
  // fill the background
  setColor (foregroundColor, backgroundColor);

  if ( isMonochron() )
    setReverse(true);

  clearArea();
  drawBorder();
  drawTitleBar();

  if ( (flags & fc::shadow) != 0 )
    drawDialogShadow();

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
  int zoom_btn;

  if ( (flags & fc::resizeable) == 0 )
    zoom_btn = 0;
  else if ( isNewFont() )
    zoom_btn = 2;
  else
    zoom_btn = 3;

  if ( zoom_button_pressed || zoom_button_active )
  {
    zoom_button_pressed = false;
    zoom_button_active = false;
    drawTitleBar();
  }

  if ( ev->getButton() == fc::LeftButton )
  {
    bool has_raised;

    // click on titlebar or window: raise + activate
    if ( mouse_x >= 4 && mouse_x <= width-zoom_btn && mouse_y == 1 )
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
      {
        leaveMenu();
        drawTitleBar();
      }
      else
      {
        setOpenMenu(dialog_menu);
        dialog_menu->move (xpos, ypos+1);
        dialog_menu->setVisible();
        drawTitleBar();
        dialog_menu->show();
        dialog_menu->raiseWindow(dialog_menu);
        dialog_menu->redraw();
      }
    }
    else if ( mouse_x > width-zoom_btn  && mouse_y == 1 )
    {
      zoom_button_pressed = true;
      zoom_button_active = true;
      drawTitleBar();
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
  int zoom_btn;

  if ( (flags & fc::resizeable) == 0 )
    zoom_btn = 0;
  else if ( isNewFont() )
    zoom_btn = 2;
  else
    zoom_btn = 3;

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
    else if ( mouse_x > width - zoom_btn
            && mouse_y == 1
            && zoom_button_pressed )
    {
      // zoom to maximum or restore the window size
      zoomWindow();
    }
  }

  if ( zoom_button_pressed || zoom_button_active )
  {
    zoom_button_pressed = false;
    zoom_button_active = false;
    drawTitleBar();
  }
}

//----------------------------------------------------------------------
void FDialog::onMouseMove (FMouseEvent* ev)
{
  int zoom_btn;

  if ( (flags & fc::resizeable) == 0 )
    zoom_btn = 0;
  else if ( isNewFont() )
    zoom_btn = 2;
  else
    zoom_btn = 3;

  if ( ev->getButton() == fc::LeftButton )
  {
    int mouse_x = ev->getX();
    int mouse_y = ev->getY();

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

    if ( mouse_x > width - zoom_btn && mouse_y == 1 && zoom_button_active )
    {
      zoom_button_pressed = true;
      drawTitleBar();
    }
    else if ( zoom_button_pressed )
    {
      zoom_button_pressed = false;
      drawTitleBar();
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
    FWidget* window_focus_widget;
    dialog_menu->unselectItem();
    dialog_menu->hide();
    activateWindow();
    raiseWindow();
    window_focus_widget = getWindowFocusWidget();

    if ( window_focus_widget )
      window_focus_widget->setFocus();

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
  if ( ! (isHiddenWindow() || isActiveWindow()) )
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
    FWidget* win_focus = getWindowFocusWidget();

    if ( win_focus
       && win_focus->isVisible()
       && win_focus->isShown() )
    {
      win_focus->setFocus();
      win_focus->redraw();
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
  if ( dialog_menu && ! dialog_menu->isVisible() )
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

  if ( ! (isVisible() && isShown()) )
    return;

  putArea (getGlobalPos(), vwin);

  if ( ! window_list )
    return;

  if ( window_list->empty() )
    return;
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
  FWidget* focus_widget = FWidget::getFocusWidget();

  if (  focus_widget
     && focus_widget->isVisible()
     && focus_widget->hasVisibleCursor()
     && focus_widget->isCursorInside() )
  {
    focus_widget->setCursor();
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

  if ( isZoomed() )
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
  putArea (getGlobalPos(), vwin);updateTerminal();

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
        putArea ((*iter)->getGlobalPos(), (*iter)->getVWin());

      if ( vwin == (*iter)->getVWin() )
        overlaid = true;

      ++iter;
    }
  }

  FWidget::adjustSize();

  // set the cursor to the focus widget
  FWidget* focus_widget = FWidget::getFocusWidget();
  if (  focus_widget
     && focus_widget->isVisible()
     && focus_widget->hasVisibleCursor() )
  {
    FPoint cursor_pos = focus_widget->getCursorPos();
    cursor_pos -= FPoint(dx,dy);

    if ( ! focus_widget->setCursorPos(cursor_pos) )
      hideCursor();
  }

  updateTerminal();
}

//----------------------------------------------------------------------
void FDialog::activateDialog()
{
  FWidget* old_focus = FWidget::getFocusWidget();
  FWidget* win_focus = getWindowFocusWidget();
  setActiveWindow(this);
  setFocus();
  setFocusWidget(this);

  if ( win_focus && numOfFocusableChildren() > 1 )
  {
    win_focus->setFocus();
    win_focus->redraw();

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
  {
    flags |= fc::resizeable;
    zoom_item->setEnable();
  }
  else
  {
    flags &= ~fc::resizeable;
    zoom_item->setDisable();
  }

  return on;
}
