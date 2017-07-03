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
FDialog::FDialog (FWidget* parent)
  : FWindow(parent)
  , tb_text()
  , result_code(FDialog::Reject)
  , zoom_button_pressed(false)
  , zoom_button_active(false)
  , titlebar_click_pos()
  , resize_click_pos()
  , save_geometry()
  , dialog_menu()
  , dgl_menuitem()
  , move_size_item()
  , zoom_item()
  , close_item()
  , tooltip()
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
  , titlebar_click_pos()
  , resize_click_pos()
  , save_geometry()
  , dialog_menu()
  , dgl_menuitem()
  , move_size_item()
  , zoom_item()
  , close_item()
  , tooltip(0)
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

  delDialog(this);

  if ( isModal() )
    unsetModal();
}


// public methods of FDialog
//----------------------------------------------------------------------
bool FDialog::setDialogWidget (bool on)
{
  if ( isDialogWidget() == on )
    return true;

  if ( on )
  {
    flags |= fc::dialog_widget;
    setTermOffsetWithPadding();
  }
  else
  {
    flags &= ~fc::dialog_widget;
    setParentOffset();
  }

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
  FWindow::setResizeable (on);

  if ( on )
    zoom_item->setEnable();
  else
    zoom_item->setDisable();

  return on;
}

//----------------------------------------------------------------------
void FDialog::show()
{
  if ( ! isVisible() )
    return;

  FWindow::show();

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
void FDialog::setPos (int x, int y, bool)
{
  int dx, dy, old_x, old_y, rsw, bsh, width, height;
  FRect old_geometry;

  if ( getX() == x && getY() == y )
    return;

  width = getWidth();
  height = getHeight();

  // Avoid to move widget completely outside the terminal
  if ( x+width <= 1 || x > getMaxWidth() || y < 1 || y > getMaxHeight() )
    return;

  if ( isZoomed() )
    return;

  dx = getX() - x;
  dy = getY() - y;
  old_x = getTermX();
  old_y = getTermY();
  const FPoint& shadow = getShadow();
  rsw = shadow.getX();  // right shadow width;
  bsh = shadow.getY();  // bottom shadow height
  old_geometry = getTermGeometryWithShadow();

  // move to the new position
  FWindow::setPos(x, y, false);
  putArea (getTermPos(), vwin);

  // restoring the non-covered terminal areas
  if ( getTermGeometry().overlap(old_geometry) )
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
        restoreVTerm (old_x+width+rsw-dx, old_y, dx, getHeight()+bsh-dy);
      else
        restoreVTerm (old_x+width+rsw-dx, old_y+std::abs(dy), dx, height+bsh-std::abs(dy));
    }
    else
    {
      if ( dy > 0 )
        restoreVTerm (old_x, old_y, std::abs(dx), height+bsh-dy);
      else
        restoreVTerm (old_x, old_y+std::abs(dy), std::abs(dx), height+bsh-std::abs(dy));
    }

    if ( dy > 0 )
      restoreVTerm (old_x, old_y+height+bsh-dy, width+rsw, dy);
    else
      restoreVTerm (old_x, old_y, width+rsw, std::abs(dy));
  }
  else
  {
    restoreVTerm (old_geometry);
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
        putArea ((*iter)->getTermPos(), (*iter)->getVWin());

      if ( vwin == (*iter)->getVWin() )
        overlaid = true;

      ++iter;
    }
  }

  FWindow::adjustSize();

  // set the cursor to the focus widget
  FWidget* focus_widget = FWidget::getFocusWidget();

  if ( focus_widget
      && focus_widget->isVisible()
      && focus_widget->hasVisibleCursor() )
  {
    FPoint cursor_pos = focus_widget->getCursorPos();
    focus_widget->setCursorPos(cursor_pos);
    updateVTermCursor(vwin);
  }

  updateTerminal();
}

//----------------------------------------------------------------------
void FDialog::move (int dx, int dy)
{
  setPos (getX() + dx, getY() + dy);
}

//----------------------------------------------------------------------
void FDialog::setSize (int w, int h, bool adjust)
{
  int x, y, dw, dh, old_width, old_height, rsw, bsh;

  if ( getWidth() == w && getHeight() == h )
    return;

  if ( isZoomed() )
    return;

  x = getTermX();
  y = getTermY();
  old_width = getWidth();
  old_height = getHeight();
  dw = old_width - w;
  dh = old_height - h;
  const FPoint& shadow = getShadow();
  rsw = shadow.getX();  // right shadow width;
  bsh = shadow.getY();  // bottom shadow height

  FWindow::setSize (w, h, adjust);

  // get adjust width and height
  w = getWidth();
  h = getHeight();

  // dw > 0 : scale down width
  // dw = 0 : scale only height
  // dw < 0 : scale up width
  // dh > 0 : scale down height
  // dh = 0 : scale only width
  // dh < 0 : scale up height

  // restoring the non-covered terminal areas
  if ( dw > 0 )
    restoreVTerm (x+w+rsw, y, dw, h+bsh+dh);  // restore right

  if ( dh > 0 )
    restoreVTerm (x, y+h+bsh, w+rsw+dw, dh);  // restore bottom

  redraw();

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
        putArea ((*iter)->getTermPos(), (*iter)->getVWin());

      if ( vwin == (*iter)->getVWin() )
        overlaid = true;

      ++iter;
    }
  }

  // set the cursor to the focus widget
  FWidget* focus_widget = FWidget::getFocusWidget();
  if ( focus_widget
      && focus_widget->isVisible()
      && focus_widget->hasVisibleCursor() )
  {
    FPoint cursor_pos = focus_widget->getCursorPos();
    focus_widget->setCursorPos(cursor_pos);
  }
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

    if ( ! old_focus->isWindowWidget() )
      old_focus->redraw();
  }

  if ( getStatusBar() )
    getStatusBar()->drawMessage();

  updateTerminal();
}

//----------------------------------------------------------------------
void FDialog::onKeyPress (FKeyEvent* ev)
{
  if ( ! isEnabled() )
    return;

  // cancel resize by mouse
  if ( ! resize_click_pos.isNull() )
  {
    resize_click_pos.setPoint (0,0);
    drawBorder();
    updateTerminal();
  }

  if ( ev->key() == fc::Fckey_caret )  // Ctrl+^ (Ctrl+6)
  {
    ev->accept();
    // open the titlebar menu
    openMenu();
    // focus to the first enabled item
    selectFirstMenuItem();
  }

  if ( getMoveSizeWidget() )
  {
    switch ( ev->key() )
    {
      case fc::Fkey_up:
        move (0, -1);
        ev->accept();
        break;

      case fc::Fkey_down:
        move (0, 1);
        ev->accept();
        break;

      case fc::Fkey_left:
        move (-1, 0);
        ev->accept();
        break;

      case fc::Fkey_right:
        move (1, 0);
        ev->accept();
        break;

      case fc::Fmkey_up:
      case fc::Fkey_sr:
        if ( isResizeable() )
        {
          setSize (getWidth(), getHeight() - 1);
          ev->accept();
        }
        break;

      case fc::Fmkey_down:
      case fc::Fkey_sf:
        if ( isResizeable() && getHeight() + getY() <= getMaxHeight() )
        {
          setSize (getWidth(), getHeight() + 1);
          ev->accept();
        }
        break;

      case fc::Fmkey_left:
      case fc::Fkey_sleft:
        if ( isResizeable() )
        {
          setSize (getWidth() - 1, getHeight());
          ev->accept();
        }
        break;

      case fc::Fmkey_right:
      case fc::Fkey_sright:
        if ( isResizeable() && getWidth() + getX() <= getMaxWidth() )
        {
          setSize (getWidth() + 1, getHeight());
          ev->accept();
        }
        break;

      case fc::Fkey_return:
      case fc::Fkey_enter:
        setMoveSizeWidget(0);

        if ( tooltip )
          delete tooltip;

        tooltip = 0;
        redraw();
        ev->accept();
        break;

      case fc::Fkey_escape:
      case fc::Fkey_escape_mintty:
        setMoveSizeWidget(0);

        if ( tooltip )
          delete tooltip;

        tooltip = 0;
        setPos (save_geometry.getPos());

        if ( isResizeable() )
          setSize (save_geometry.getWidth(), save_geometry.getHeight());

        redraw();
        ev->accept();
        return;

      default:
        break;
    }
  }

  if ( this == getMainWidget() )
    return;

  if ( ev->key() == fc::Fkey_escape
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
  int zoom_btn;;

  if ( ! isResizeable() )
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
    if ( mouse_x >= 4 && mouse_x <= getWidth()-zoom_btn && mouse_y == 1 )
      titlebar_click_pos.setPoint (ev->getTermX(), ev->getTermY());
    else
      titlebar_click_pos.setPoint (0,0);

    has_raised = raiseWindow();

    if ( ! isWindowActive() )
      activateDialog();

    if ( has_raised )
      redraw();

    // click on titlebar menu button
    if ( mouse_x < 4 && mouse_y == 1 )
      openMenu();
    else if ( mouse_x > getWidth()-zoom_btn && mouse_y == 1 )
    {
      zoom_button_pressed = true;
      zoom_button_active = true;
      drawTitleBar();
    }

    // click on the lower right resize corner
    if ( isResizeable()
        && ( (mouse_x == getWidth() && mouse_y == getHeight())
            || (mouse_x == getWidth()-1 && mouse_y == getHeight())
            || (mouse_x == getWidth() && mouse_y == getHeight()-1) ) )
    {
      resize_click_pos = ev->getTermPos();
      FPoint lower_right_pos = getTermGeometry().getLowerRightPos();

      if ( ev->getTermPos() != lower_right_pos )
      {
        FPoint deltaPos = ev->getTermPos() - lower_right_pos;
        int w = lower_right_pos.getX() + deltaPos.getX() - getTermX() + 1;
        int h = lower_right_pos.getY() + deltaPos.getY() - getTermY() + 1;
        setSize (w, h);
      }
      else
        drawBorder();
    }
    else
      resize_click_pos.setPoint (0,0);
  }
  else  // ev->getButton() != fc::LeftButton
  {
    // click on titlebar menu button
    if ( mouse_x < 4 && mouse_y == 1 && dialog_menu->isVisible() )
      leaveMenu();  // close menu

    // cancel resize
    if ( ! resize_click_pos.isNull() )
    {
      resize_click_pos.setPoint (0,0);
      drawBorder();
      updateTerminal();
    }
  }

  if ( ev->getButton() == fc::RightButton )
  {
    // click on titlebar: just activate
    if ( mouse_x >= 4 && mouse_x <= getWidth() && mouse_y == 1 )
    {
      if ( ! isWindowActive() )
        activateDialog();
    }
  }

  if ( ev->getButton() == fc::MiddleButton )
  {
    // click on titlebar: lower + activate
    if ( mouse_x >= 4 && mouse_x <= getWidth() && mouse_y == 1 )
    {
      bool has_lowered = lowerWindow();

      if ( ! isWindowActive() )
        activateDialog();
      else if ( has_lowered )
        updateTerminal();
    }
  }
}

//----------------------------------------------------------------------
void FDialog::onMouseUp (FMouseEvent* ev)
{
  int zoom_btn;

  if ( ! isResizeable() )
    zoom_btn = 0;
  else if ( isNewFont() )
    zoom_btn = 2;
  else
    zoom_btn = 3;

  if ( ev->getButton() == fc::LeftButton )
  {
    int mouse_x    = ev->getX();
    int mouse_y    = ev->getY();
    int titlebar_x = titlebar_click_pos.getX();
    int titlebar_y = titlebar_click_pos.getY();

    if ( ! titlebar_click_pos.isNull()
        && titlebar_x > getTermX() + 3
        && titlebar_x < getTermX() + getWidth()
        && titlebar_y == getTermY() )
    {
      FPoint deltaPos = ev->getTermPos() - titlebar_click_pos;
      move (deltaPos);
      titlebar_click_pos = ev->getTermPos();
    }

    // click on titlebar menu button
    if ( mouse_x < 4
        && mouse_y == 1
        && dialog_menu->isVisible()
        && ! dialog_menu->hasSelectedItem() )
    {
      // Sets focus to the first item
      selectFirstMenuItem();
    }
    else if ( mouse_x > getWidth() - zoom_btn
             && mouse_y == 1
             && zoom_button_pressed )
    {
      // zoom to maximum or restore the window size
      zoomWindow();
      setZoomItem();
    }

    // resize the dialog
    if ( isResizeable() && ! resize_click_pos.isNull() )
    {
      FWidget* r = getRootWidget();
      resize_click_pos = ev->getTermPos();
      int x2 = resize_click_pos.getX();
      int y2 = resize_click_pos.getY();
      int x2_offset = 0;
      int y2_offset = 0;

      if ( r )
      {
        x2_offset = r->getLeftPadding();
        y2_offset = r->getTopPadding();
      }

      if ( ev->getTermPos() != getTermGeometry().getLowerRightPos() )
      {
        int w, h;
        FPoint deltaPos = ev->getTermPos() - resize_click_pos;

        if ( x2 - x2_offset <= getMaxWidth() )
          w = resize_click_pos.getX() + deltaPos.getX() - getTermX() + 1;
        else
          w = getMaxWidth() - getTermX() + x2_offset + 1;

        if ( y2 - y2_offset <= getMaxHeight() )
          h = resize_click_pos.getY() + deltaPos.getY() - getTermY() + 1;
        else
          h = getMaxHeight() - getTermY() + y2_offset + 1;

        setSize (w, h);
      }

      // reset the border color
      resize_click_pos.setPoint (0,0);

      // redraw() is required to draw the standard (black) border
      // and client objects with ignorePadding() option.
      redraw();
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

  if ( ! isResizeable() )
    zoom_btn = 0;
  else if ( isNewFont() )
    zoom_btn = 2;
  else
    zoom_btn = 3;

  if ( ev->getButton() == fc::LeftButton )
  {
    int mouse_x = ev->getX();
    int mouse_y = ev->getY();

    if ( ! titlebar_click_pos.isNull() )
    {
      FPoint deltaPos = ev->getTermPos() - titlebar_click_pos;
      move (deltaPos);
      titlebar_click_pos = ev->getTermPos();
    }

    if ( dialog_menu->isVisible() && dialog_menu->isShown() )
    {
      // Mouse event handover to the menu
      const FRect& menu_geometry = dialog_menu->getTermGeometry();

      if ( dialog_menu->getCount() > 0
          && menu_geometry.contains(ev->getTermPos()) )
      {
        const FPoint& g = ev->getTermPos();
        const FPoint& p = dialog_menu->termToWidgetPos(g);
        int b = ev->getButton();
        FMouseEvent* _ev = new FMouseEvent (fc::MouseMove_Event, p, g, b);
        dialog_menu->mouse_down = true;
        setClickedWidget(dialog_menu);
        dialog_menu->onMouseMove(_ev);
        delete _ev;
      }
    }

    if ( mouse_x > getWidth() - zoom_btn && mouse_y == 1 && zoom_button_active )
      zoom_button_pressed = true;
    else if ( zoom_button_pressed )
      zoom_button_pressed = false;

    drawTitleBar();

    // resize the dialog
    if ( isResizeable() && ! resize_click_pos.isNull()
        && ev->getTermPos() != getTermGeometry().getLowerRightPos() )
    {
      FWidget* r = getRootWidget();
      resize_click_pos = ev->getTermPos();
      int x2 = resize_click_pos.getX();
      int y2 = resize_click_pos.getY();
      int x2_offset = 0;
      int y2_offset = 0;

      if ( r )
      {
        x2_offset = r->getLeftPadding();
        y2_offset = r->getTopPadding();
      }

      int w, h;
      FPoint deltaPos = ev->getTermPos() - resize_click_pos;

      if ( x2 - x2_offset <= getMaxWidth() )
        w = resize_click_pos.getX() + deltaPos.getX() - getTermX() + 1;
      else
        w = getMaxWidth() - getTermX() + x2_offset + 1;

      if ( y2 - y2_offset <= getMaxHeight() )
        h = resize_click_pos.getY() + deltaPos.getY() - getTermY() + 1;
      else
        h = getMaxHeight() - getTermY() + y2_offset + 1;

      setSize (w, h);
    }
  }
}

//----------------------------------------------------------------------
void FDialog::onMouseDoubleClick (FMouseEvent* ev)
{
  int x, y, mouse_x, mouse_y, zoom_btn;

  if ( ev->getButton() != fc::LeftButton )
    return;

  mouse_x = ev->getX();
  mouse_y = ev->getY();

  if ( ! isResizeable() )
    zoom_btn = 0;
  else if ( isNewFont() )
    zoom_btn = 2;
  else
    zoom_btn = 3;

  x = getTermX();
  y = getTermY();
  FRect title_button(x, y, 3, 1);
  FPoint tPos = ev->getTermPos();

  if ( title_button.contains(tPos) )
  {
    // double click on title button
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
  else if ( isResizeable()
           && mouse_x >= 4
           && mouse_x <= getWidth() - zoom_btn
           && mouse_y == 1 )
  {
    // double click on titlebar
    zoomWindow();  // window zoom/unzoom
    setZoomItem();
  }
}

//----------------------------------------------------------------------
void FDialog::onAccel (FAccelEvent*)
{
  if ( ! (isWindowHidden() || isWindowActive()) )
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

  if ( getStatusBar() )
    getStatusBar()->drawMessage();

  updateTerminal();
}

//----------------------------------------------------------------------
void FDialog::onWindowInactive (FEvent*)
{
  if ( dialog_menu && ! dialog_menu->isVisible() )
    FWindow::previous_window = this;

  if ( isVisible() && isEnabled() )
    drawTitleBar();

  if ( hasFocus() )
    unsetFocus();
}

//----------------------------------------------------------------------
void FDialog::onWindowRaised (FEvent*)
{
  if ( ! (isVisible() && isShown()) )
    return;

  putArea (getTermPos(), vwin);

  // handle always-on-top windows
  if ( always_on_top_list && ! always_on_top_list->empty() )
  {
    widgetList::const_iterator iter, end;
    iter = always_on_top_list->begin();
    end  = always_on_top_list->end();

    while ( iter != end )
    {
      putArea ((*iter)->getTermPos(), (*iter)->getVWin());
      ++iter;
    }
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
    putArea ((*iter)->getTermPos(), (*iter)->getVWin());
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
void FDialog::draw()
{
  if ( tooltip && ! getMoveSizeWidget() )
  {
    if ( tooltip )
      delete tooltip;

    tooltip = 0;
  }

  // fill the background
  setColor();

  if ( isMonochron() )
    setReverse(true);

  clearArea();
  drawBorder();
  drawTitleBar();
  setCursorPos(2, getHeight() - 1);

  if ( (flags & fc::shadow) != 0 )
    drawDialogShadow();

  if ( isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
void FDialog::drawDialogShadow()
{
  if ( isMonochron() && (flags & fc::trans_shadow) == 0 )
    return;

  drawShadow();
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


// private methods of FDialog
//----------------------------------------------------------------------
void FDialog::init()
{
  FWidget* old_focus;
  setTopPadding(2);
  setLeftPadding(1);
  setBottomPadding(1);
  setRightPadding(1);
  ignorePadding();
  setDialogWidget();
  // initialize geometry values
  setGeometry (1, 1, 10, 10, false);
  setMinimumSize (15, 4);
  addDialog(this);
  setActiveWindow(this);
  setTransparentShadow();
  setForegroundColor (wc.dialog_fg);
  setBackgroundColor (wc.dialog_bg);
  old_focus = FWidget::getFocusWidget();

  if ( old_focus )
  {
    setFocus();
    old_focus->redraw();
  }

  accelerator_list = new Accelerators();
  // Add the dialog menu
  dialog_menu = new FMenu ("-", this);
  dialog_menu->setPos (getX(), getY()+1);
  dgl_menuitem = dialog_menu->getItem();

  if ( dgl_menuitem )
  {
    dgl_menuitem->ignorePadding();
    dgl_menuitem->unsetFocusable();
  }

  move_size_item = new FMenuItem (dialog_menu);
  move_size_item->setText ("&Move/Size");
  move_size_item->setStatusbarMessage ("Move or change the size of the window");

  move_size_item->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &FDialog::cb_move)
  );

  zoom_item = new FMenuItem (dialog_menu);
  setZoomItem();
  zoom_item->setDisable();

  zoom_item->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &FDialog::cb_zoom)
  );

  close_item = new FMenuItem ("&Close", dialog_menu);
  close_item->setStatusbarMessage ("Close this window");

  close_item->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &FDialog::cb_close)
  );
}

//----------------------------------------------------------------------
void FDialog::drawBorder()
{
  int x1 = 1;
  int x2 = 1 + getWidth() - 1;
  int y1 = 2;
  int y2 = 1 + getHeight() - 1;

  if ( (getMoveSizeWidget() == this || ! resize_click_pos.isNull())
      && ! isZoomed() )
    setColor (wc.dialog_resize_fg, getBackgroundColor());
  else
    setColor();

  if ( isNewFont() )
  {
    for (int y=y1; y < y2; y++)
    {
      setPrintPos (x1, y);
      // border left ⎸
      print (fc::NF_border_line_left);
      setPrintPos (x2, y);
      // border right⎹
      print (fc::NF_rev_border_line_right);
    }

    setPrintPos (x1, y2);
    // lower left corner border ⎣
    print (fc::NF_border_corner_lower_left);

    for (int x=1; x < getWidth()-1; x++) // low line _
      print (fc::NF_border_line_bottom);

    setPrintPos (x2, y2);
    // lower right corner border ⎦
    print (fc::NF_rev_border_corner_lower_right);
  }
  else
  {
    FWidget::drawBorder(x1, y1, x2, y2);
  }
}

//----------------------------------------------------------------------
void FDialog::drawTitleBar()
{
  static const int menu_btn = 3;
  int i,x,length, zoom_btn;

  // draw the title button
  setPrintPos (1, 1);

  if ( dialog_menu && dialog_menu->isVisible() )
    setColor (wc.titlebar_button_focus_fg, wc.titlebar_button_focus_bg);
  else
    setColor (wc.titlebar_button_fg, wc.titlebar_button_bg);

  if ( isMonochron() )
  {
    if ( isWindowActive() )
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

  if ( isWindowActive() || (dialog_menu && dialog_menu->isVisible()) )
    setColor (wc.titlebar_active_fg, wc.titlebar_active_bg);
  else
    setColor (wc.titlebar_inactive_fg, wc.titlebar_inactive_bg);

  if ( ! isResizeable() )
    zoom_btn = 0;
  else if ( isNewFont() )
    zoom_btn = 2;
  else
    zoom_btn = 3;

  length = int(tb_text.getLength());
  i = getWidth() - length - menu_btn - zoom_btn;
  i = int(i/2);

  for (x=1; x <= i; x++)
    print (' ');

  // the title bar text
  if ( tb_text )
  {
    if ( length <= getWidth() - menu_btn - zoom_btn )
      print (tb_text);
    else
    {
      print (tb_text.left(getWidth() - menu_btn - zoom_btn - 2));
      print ("..");
    }
  }

  // fill the rest of the bar
  for (; x+1+length < getWidth()-zoom_btn-1; x++)
    print (' ');

  if ( getMaxColor() < 16 )
    unsetBold();

  // draw the zoom/unzoom button
  if ( isResizeable() )
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
          print (fc::BlackUpPointingTriangle);  // ▲
          print (' ');
        }
      }
    }
  }

  if ( isMonochron() )
    setReverse(false);

/* print the number of window in stack */
//setPrintPos (getWidth()-2, 1);
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

  if ( getStatusBar() )
    getStatusBar()->drawMessage();

  updateTerminal();
  flush_out();
}

//----------------------------------------------------------------------
void FDialog::openMenu()
{
  // open the titlebar menu
  if ( ! dialog_menu )
    return;

  if ( dialog_menu->isVisible() )
  {
    leaveMenu();
    drawTitleBar();
  }
  else
  {
    setOpenMenu(dialog_menu);
    dialog_menu->setPos (getX(), getY()+1);
    dialog_menu->setVisible();
    drawTitleBar();
    dialog_menu->show();
    dialog_menu->raiseWindow(dialog_menu);
    dialog_menu->redraw();
  }
}

//----------------------------------------------------------------------
void FDialog::selectFirstMenuItem()
{
  // focus to the first enabled menu item
  FMenuItem* first_item;
  dialog_menu->selectFirstItem();
  first_item = dialog_menu->getSelectedItem();

  if ( first_item )
    first_item->setFocus();

  dialog_menu->redraw();

  if ( getStatusBar() )
    getStatusBar()->drawMessage();

  updateTerminal();
  flush_out();
}

//----------------------------------------------------------------------
void FDialog::setZoomItem()
{
  if ( isZoomed() )
  {
    zoom_item->setText ("&Unzoom");
    zoom_item->setStatusbarMessage ("Restore the window size");
    move_size_item->setDisable();
  }
  else
  {
    zoom_item->setText ("&Zoom");
    zoom_item->setStatusbarMessage ("Enlarge the window to the entire desktop");
    move_size_item->setEnable();
  }
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

//----------------------------------------------------------------------
void FDialog::cb_move (FWidget*, data_ptr)
{
  if ( isZoomed() )
    return;

  setMoveSizeWidget(this);

  if ( isMonochron() )
      setReverse(true);

  drawBorder();

  if ( isMonochron() )
      setReverse(false);

  save_geometry = getGeometry();
  tooltip = new FToolTip(this);

  if ( isResizeable() )
  {
    if ( isLinuxTerm() )
      tooltip->setText ( "        Arrow keys: Move\n"
                         "Shift + Arrow keys: Resize\n"
                         "             Enter: Done\n"
                         "               Esc: Cancel" );
    else
      tooltip->setText ( "       Arrow keys: Move\n"
                         "Meta + Arrow keys: Resize\n"
                         "            Enter: Done\n"
                         "              Esc: Cancel" );
  }
  else
  {
    tooltip->setText ( "Arrow keys: Move\n"
                       "     Enter: Done\n"
                       "       Esc: Cancel" );
  }

  tooltip->show();
}

//----------------------------------------------------------------------
void FDialog::cb_zoom (FWidget*, data_ptr)
{
  dialog_menu->unselectItem();
  dialog_menu->hide();
  setClickedWidget(0);
  drawTitleBar();
  zoomWindow();
  setZoomItem();
}

//----------------------------------------------------------------------
void FDialog::cb_close (FWidget*, data_ptr)
{
  dialog_menu->unselectItem();
  dialog_menu->hide();
  setClickedWidget(0);
  drawTitleBar();
  close();
}
