/***********************************************************************
* fdialog.cpp - Widget FDialog                                         *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2012-2018 Markus Gans                                      *
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

#include "final/fapplication.h"
#include "final/fdialog.h"
#include "final/fstatusbar.h"

namespace finalcut
{

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
  , setPos_error(false)
  , setSize_error(false)
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
  , setPos_error(false)
  , setSize_error(false)
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
    switchToPrevWindow(this);

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

  flags.dialog_widget = on;

  if ( on )
    setTermOffsetWithPadding();
  else
    setParentOffset();

  return on;
}

//----------------------------------------------------------------------
bool FDialog::setModal (bool on)
{
  if ( isModal() == on )
    return true;

  flags.modal = on;

  if ( on )
    modal_dialogs++;
  else
    modal_dialogs--;

  return on;
}


//----------------------------------------------------------------------
bool FDialog::setScrollable (bool on)
{
  return (flags.scrollable = on);
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
FDialog::DialogCode FDialog::exec()
{
  result_code = FDialog::Reject;
  show();
  return result_code;
}

//----------------------------------------------------------------------
void FDialog::setPos (int x, int y, bool)
{
  int rsw, bsh, width, height;
  FRect old_geometry;
  setPos_error = false;

  if ( getX() == x && getY() == y )
  {
    setPos_error = true;
    return;
  }

  width = int(getWidth());
  height = int(getHeight());

  // Avoid to move widget completely outside the terminal
  if ( x + width <= 1
    || x > int(getMaxWidth())
    || y < 1
    || y > int(getMaxHeight()) )
  {
    setPos_error = true;
    return;
  }

  if ( isZoomed() )
  {
    setPos_error = true;
    return;
  }

  int dx = getX() - x
    , dy = getY() - y
    , old_x = getTermX()
    , old_y = getTermY();
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
        restoreVTerm ( old_x + width + rsw - dx, old_y
                     , dx, height + bsh - dy );
      else
        restoreVTerm ( old_x + width + rsw - dx, old_y + std::abs(dy)
                     , dx, height + bsh - std::abs(dy));
    }
    else
    {
      if ( dy > 0 )
        restoreVTerm (old_x, old_y, std::abs(dx), height + bsh - dy);
      else
        restoreVTerm ( old_x, old_y + std::abs(dy)
                     , std::abs(dx), height + bsh - std::abs(dy) );
    }

    if ( dy > 0 )
      restoreVTerm (old_x, old_y + height + bsh - dy, width + rsw, dy);
    else
      restoreVTerm (old_x, old_y, width + rsw, std::abs(dy));
  }
  else
  {
    restoreVTerm (old_geometry);
  }

  restoreOverlaidWindows();
  FWindow::adjustSize();
  setCursorToFocusWidget();
  updateTerminal();
}

//----------------------------------------------------------------------
void FDialog::move (int dx, int dy)
{
  setPos (getX() + dx, getY() + dy);
}

//----------------------------------------------------------------------
inline bool FDialog::moveUp (int n)
{
  move (0, -n);
  return ! setPos_error;
}

//----------------------------------------------------------------------
inline bool FDialog::moveDown (int n)
{
  move (0, n);
  return ! setPos_error;
}

//----------------------------------------------------------------------
inline bool FDialog::moveLeft (int n)
{
  move (-n, 0);
  return ! setPos_error;
}

//----------------------------------------------------------------------
inline bool FDialog::moveRight (int n)
{
  move (n, 0);
  return ! setPos_error;
}

//----------------------------------------------------------------------
void FDialog::setSize (std::size_t w, std::size_t h, bool adjust)
{
  setSize_error = false;

  if ( getWidth() == w && getHeight() == h )
  {
    setSize_error = true;
    return;
  }

  if ( isZoomed() )
  {
    setSize_error = true;
    return;
  }

  int x = getTermX()
    , y = getTermY()
    , old_width = int(getWidth())
    , old_height = int(getHeight())
    , dw = old_width - int(w)
    , dh = old_height - int(h);
  const FPoint& shadow = getShadow();
  int rsw = shadow.getX();  // right shadow width;
  int bsh = shadow.getY();  // bottom shadow height

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
    restoreVTerm (x + int(w) + rsw, y, dw, int(h) + bsh + dh);  // restore right

  if ( dh > 0 )
    restoreVTerm (x, y + int(h) + bsh, int(w) + rsw + dw, dh);  // restore bottom

  redraw();

  // handle overlaid windows
  if ( window_list && ! window_list->empty() )
  {
    bool overlaid = false;
    widgetList::const_iterator iter, last;
    iter = window_list->begin();
    last = window_list->end();

    while ( iter != last )
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
bool FDialog::reduceHeight (int n)
{
  if ( ! isResizeable() )
    return false;

  setSize (getWidth(), getHeight() - std::size_t(n));
  return ! setSize_error;
}

//----------------------------------------------------------------------
bool FDialog::expandHeight (int n)
{
  if ( ! isResizeable() || getHeight() + std::size_t(getY()) > getMaxHeight() )
    return false;

  setSize (getWidth(), getHeight() + std::size_t(n));
  return ! setSize_error;
}

//----------------------------------------------------------------------
bool FDialog::reduceWidth (int n)
{
  if ( ! isResizeable() )
    return false;

  setSize (getWidth() - std::size_t(n), getHeight());
  return ! setSize_error;
}

//----------------------------------------------------------------------
bool FDialog::expandWidth (int n)
{
  if ( ! isResizeable() || getWidth() + std::size_t(getX()) > getMaxWidth() )
    return false;

  setSize (getWidth() + std::size_t(n), getHeight());
  return ! setSize_error;
}

//----------------------------------------------------------------------
void FDialog::activateDialog()
{
  if ( isWindowActive() )
    return;

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

  cancelMouseResize();

  if ( ev->key() == fc::Fckey_caret  // Ctrl+^ (Ctrl+6)
    || ev->key() == fc::Fkey_f22 )   // Shift+F10
  {
    ev->accept();
    // open the titlebar menu
    openMenu();
    // focus to the first enabled item
    selectFirstMenuItem();
  }

  // Dialog move and resize functions
  if ( getMoveSizeWidget() )
    moveSizeKey(ev);

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
  int width = int(getWidth());

  mouseStates ms =
  {
    ev->getX(),
    ev->getY(),
    ev->getTermPos(),
    getZoomButtonWidth(),
    false  // mouse_over_menu
  };

  deactivateZoomButton();

  if ( ev->getButton() == fc::LeftButton )
  {
    // Click on titlebar or window: raise + activate
    raiseActivateDialog();

    if ( ms.mouse_x >= 4
      && ms.mouse_x <= width - int(ms.zoom_btn)
      && ms.mouse_y == 1 )
      titlebar_click_pos.setPoint (ev->getTermX(), ev->getTermY());
    else
      titlebar_click_pos.setPoint (0, 0);

    // Click on titlebar menu button
    if ( ms.mouse_x < 4 && ms.mouse_y == 1 )
      openMenu();
    else
      activateZoomButton(ms);

    // Click on the lower right resize corner
    resizeMouseDown(ms);
  }
  else  // ev->getButton() != fc::LeftButton
  {
    // Click on titlebar menu button
    if ( ms.mouse_x < 4 && ms.mouse_y == 1
      && dialog_menu->isVisible() )
      leaveMenu();  // close menu

    cancelMouseResize();  // Cancel resize
  }

  // Click on titlebar: just activate
  if ( ev->getButton() == fc::RightButton
    && ms.mouse_x >= 4
    && ms.mouse_x <= width
    && ms.mouse_y == 1 )
      activateDialog();

  // Click on titlebar: lower + activate
  if ( ev->getButton() == fc::MiddleButton
    && ms.mouse_x >= 4 && ms.mouse_x <= width
    && ms.mouse_y == 1 )
      lowerActivateDialog();
}

//----------------------------------------------------------------------
void FDialog::onMouseUp (FMouseEvent* ev)
{
  mouseStates ms =
  {
    ev->getX(),
    ev->getY(),
    ev->getTermPos(),
    getZoomButtonWidth(),
    false  // mouse_over_menu
  };

  if ( ev->getButton() == fc::LeftButton )
  {
    int titlebar_x = titlebar_click_pos.getX()
      , titlebar_y = titlebar_click_pos.getY();

    if ( ! titlebar_click_pos.isNull()
      && titlebar_x > int(getTermX()) + 3
      && titlebar_x < getTermX() + int(getWidth())
      && titlebar_y == int(getTermY()) )
    {
      FPoint deltaPos = ms.termPos - titlebar_click_pos;
      move (deltaPos);
      titlebar_click_pos = ms.termPos;
    }

    // Click on titlebar menu button
    if ( ms.mouse_x < 4
      && ms.mouse_y == 1
      && dialog_menu->isVisible()
      && ! dialog_menu->hasSelectedItem() )
    {
      // Sets focus to the first item
      selectFirstMenuItem();
    }
    else
    {
      // Zoom to maximum or restore the window size
      pressZoomButton(ms);
    }

    // Resize the dialog
    resizeMouseUpMove (ms, true);
  }

  deactivateZoomButton();
}

//----------------------------------------------------------------------
void FDialog::onMouseMove (FMouseEvent* ev)
{
  mouseStates ms =
  {
    ev->getX(),
    ev->getY(),
    ev->getTermPos(),
    getZoomButtonWidth(),
    isMouseOverMenu(ev->getTermPos())
  };

  if ( ev->getButton() != fc::LeftButton )
    return;

  if ( ! titlebar_click_pos.isNull() )
  {
    FPoint deltaPos = ms.termPos - titlebar_click_pos;
    move (deltaPos);
    titlebar_click_pos = ms.termPos;
  }

  // Mouse event handover to the menu
  if ( ms.mouse_over_menu )
    passEventToSubMenu (ms, ev);

  leaveZoomButton(ms);    // Check zoom button pressed
  resizeMouseUpMove(ms);  // Resize the dialog
}

//----------------------------------------------------------------------
void FDialog::onMouseDoubleClick (FMouseEvent* ev)
{
  mouseStates ms =
  {
    ev->getX(),
    ev->getY(),
    ev->getTermPos(),
    getZoomButtonWidth(),
    false  // mouse_over_menu
  };
  int x, y;

  if ( ev->getButton() != fc::LeftButton )
    return;

  x = getTermX();
  y = getTermY();
  FRect title_button(x, y, 3, 1);
  FPoint tPos = ms.termPos;

  if ( title_button.contains(tPos) )
  {
    // Double click on title button
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
         && ms.mouse_x >= 4
         && ms.mouse_x <= int(getWidth() - ms.zoom_btn)
         && ms.mouse_y == 1 )
  {
    // Double click on titlebar
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

  // Handle always-on-top windows
  if ( always_on_top_list && ! always_on_top_list->empty() )
  {
    widgetList::const_iterator iter, last;
    iter = always_on_top_list->begin();
    last  = always_on_top_list->end();

    while ( iter != last )
    {
      putArea ((*iter)->getTermPos(), (*iter)->getVWin());
      ++iter;
    }
  }
}

//----------------------------------------------------------------------
void FDialog::onWindowLowered (FEvent*)
{
  widgetList::const_iterator iter, last;

  if ( ! window_list )
    return;

  if ( window_list->empty() )
    return;

  iter = window_list->begin();
  last = window_list->end();

  while ( iter != last )
  {
    putArea ((*iter)->getTermPos(), (*iter)->getVWin());
    ++iter;
  }
}


// protected methods of FDialog
//----------------------------------------------------------------------
void FDialog::done(DialogCode result)
{
  hide();
  result_code = result;
}

//----------------------------------------------------------------------
void FDialog::draw()
{
  if ( tooltip && ! getMoveSizeWidget() )
  {
    delete tooltip;
    tooltip = 0;
  }

  // Fill the background
  setColor();

  if ( isMonochron() )
    setReverse(true);

  clearArea();
  drawBorder();
  drawTitleBar();
  setCursorPos(2, int(getHeight()) - 1);

  if ( flags.shadow )
    drawDialogShadow();

  if ( isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
void FDialog::drawDialogShadow()
{
  if ( isMonochron() && ! flags.trans_shadow )
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
  // Initialize geometry values
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

  try
  {
    accelerator_list = new Accelerators();
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
    return;
  }

  // Add the dialog menu
  initDialogMenu();
}

//----------------------------------------------------------------------
void FDialog::initDialogMenu()
{
  // Create the dialog Menu (access via Shift-F10 or Ctrl-^)

  try
  {
    dialog_menu = new FMenu ("-", this);
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
    return;
  }

  dialog_menu->setPos (getX(), getY() + 1);
  dgl_menuitem = dialog_menu->getItem();
  dgl_menuitem->ignorePadding();
  dgl_menuitem->unsetFocusable();

  // Add the move/size menu item
  initMoveSizeMenuItem (dialog_menu);

  // Add the zoom menu item
  initZoomMenuItem (dialog_menu);

  // Add the close menu item
  initCloseMenuItem (dialog_menu);
}

//----------------------------------------------------------------------
void FDialog::initMoveSizeMenuItem (FMenu* menu)
{
  try
  {
    move_size_item = new FMenuItem (menu);
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
    return;
  }

  move_size_item->setText ("&Move/Size");
  move_size_item->setStatusbarMessage ("Move or change the size of the window");

  move_size_item->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &FDialog::cb_move)
  );
}

//----------------------------------------------------------------------
void FDialog::initZoomMenuItem (FMenu* menu)
{
  try
  {
    zoom_item = new FMenuItem (menu);
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
    return;
  }

  setZoomItem();
  zoom_item->setDisable();

  zoom_item->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &FDialog::cb_zoom)
  );
}

//----------------------------------------------------------------------
void FDialog::initCloseMenuItem (FMenu* menu)
{
  try
  {
    close_item = new FMenuItem ("&Close", menu);
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
    return;
  }

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
  int x1 = 1
    , x2 = 1 + int(getWidth()) - 1
    , y1 = 2
    , y2 = 1 + int(getHeight()) - 1;

  if ( (getMoveSizeWidget() == this || ! resize_click_pos.isNull() )
    && ! isZoomed() )
    setColor (wc.dialog_resize_fg, getBackgroundColor());
  else
    setColor();

  if ( isNewFont() )
  {
    for (int y = y1; y < y2; y++)
    {
      setPrintPos (x1, y);
      // Border left ⎸
      print (fc::NF_border_line_left);
      setPrintPos (x2, y);
      // Border right⎹
      print (fc::NF_rev_border_line_right);
    }

    setPrintPos (x1, y2);
    // Lower left corner border ⎣
    print (fc::NF_border_corner_lower_left);

    for (std::size_t x = 1; x < getWidth() - 1; x++)  // low line _
      print (fc::NF_border_line_bottom);

    setPrintPos (x2, y2);
    // Lower right corner border ⎦
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
  // Draw the title button
  drawBarButton();
  // Print the text bar
  drawTextBar();
  // Draw the zoom/unzoom button
  drawZoomButton();

  if ( isMonochron() )
    setReverse(false);

#if DEBUG
  if ( PRINT_WIN_NUMBER )
  {
    // Print the number of window in stack
    setPrintPos (getWidth() - 2, 1);
    printf ("(%d)", getWindowLayer(this));
  }
#endif  // DEBUG
}

//----------------------------------------------------------------------
void FDialog::drawBarButton()
{
  // Print the title button
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
}

//----------------------------------------------------------------------
void FDialog::drawZoomButton()
{
  // Draw the zoom/unzoom button

  if ( ! isResizeable() )
    return;

  if ( zoom_button_pressed )
    setColor (wc.titlebar_button_focus_fg, wc.titlebar_button_focus_bg);
  else
    setColor (wc.titlebar_button_fg, wc.titlebar_button_bg);

  if ( isZoomed() )
    drawRestoreSizeButton();
  else
    drawZoomedButton();
}

//----------------------------------------------------------------------
inline void FDialog::drawRestoreSizeButton()
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

//----------------------------------------------------------------------
inline void FDialog::drawZoomedButton()
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

//----------------------------------------------------------------------
void FDialog::drawTextBar()
{
  // Fill with spaces (left of the title)
  std::size_t center_offset
            , width
            , zoom_btn
            , length
            , x;

  if ( getMaxColor() < 16 )
    setBold();

  if ( isWindowActive() || (dialog_menu && dialog_menu->isVisible()) )
    setColor (wc.titlebar_active_fg, wc.titlebar_active_bg);
  else
    setColor (wc.titlebar_inactive_fg, wc.titlebar_inactive_bg);

  width = std::size_t(getWidth());
  zoom_btn = getZoomButtonWidth();
  length = tb_text.getLength();
  center_offset = (width - length - MENU_BTN - zoom_btn) / 2;

  for (x = 1; x <= center_offset; x++)
    print (' ');

  // Print title bar text
  if ( ! tb_text.isEmpty() )
  {
    if ( length <= width - MENU_BTN - zoom_btn )
      print (tb_text);
    else
    {
      // Print ellipsis
      print (tb_text.left(width - MENU_BTN - zoom_btn - 2));
      print ("..");
    }
  }

  // Fill the rest of the bar
  for ( ; x + 1 + length < width - zoom_btn - 1; x++)
    print (' ');

  if ( getMaxColor() < 16 )
    unsetBold();
}

//----------------------------------------------------------------------
void FDialog::restoreOverlaidWindows()
{
  // Restoring overlaid windows

  if ( ! window_list || window_list->empty() )
    return;

  bool overlaid = false;
  widgetList::const_iterator iter, last;
  iter = window_list->begin();
  last = window_list->end();

  while ( iter != last )
  {
    if ( overlaid )
      putArea ((*iter)->getTermPos(), (*iter)->getVWin());

    if ( vwin == (*iter)->getVWin() )
      overlaid = true;

    ++iter;
  }
}

//----------------------------------------------------------------------
void FDialog::setCursorToFocusWidget()
{
  // Set the cursor to the focus widget

  FWidget* focus_widget = FWidget::getFocusWidget();

  if ( focus_widget
    && focus_widget->isVisible()
    && focus_widget->hasVisibleCursor() )
  {
    FPoint cursor_pos = focus_widget->getCursorPos();
    focus_widget->setCursorPos(cursor_pos);
    updateVTermCursor(vwin);
  }
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
  // Open the titlebar menu
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
    dialog_menu->setPos (getX(), getY() + 1);
    dialog_menu->setVisible();
    drawTitleBar();
    dialog_menu->show();
    dialog_menu->raiseWindow();
    dialog_menu->redraw();
  }
}

//----------------------------------------------------------------------
void FDialog::selectFirstMenuItem()
{
  // Focus to the first enabled menu item
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
inline std::size_t FDialog::getZoomButtonWidth()
{
  if ( ! isResizeable() )
    return 0;
  else if ( isNewFont() )
    return 2;
  else
    return 3;
}

//----------------------------------------------------------------------
inline void FDialog::deactivateZoomButton()
{
  if ( ! zoom_button_pressed && ! zoom_button_active )
    return;

  zoom_button_pressed = false;
  zoom_button_active = false;
  drawTitleBar();
}

//----------------------------------------------------------------------
inline void FDialog::activateZoomButton (mouseStates& ms)
{
  if ( ms.mouse_x <= int(getWidth() - ms.zoom_btn)
    || ms.mouse_y != 1 )
    return;

  zoom_button_pressed = true;
  zoom_button_active = true;
  drawTitleBar();
}

//----------------------------------------------------------------------
inline void FDialog::leaveZoomButton (mouseStates& ms)
{
  bool zoom_button_pressed_before = zoom_button_pressed;

  if ( ms.mouse_x > int(getWidth() - ms.zoom_btn)
    && ms.mouse_x <= int(getWidth())
    && ms.mouse_y == 1
    && zoom_button_active )
    zoom_button_pressed = true;
  else if ( zoom_button_pressed )
    zoom_button_pressed = false;

  if ( zoom_button_pressed_before != zoom_button_pressed )
    drawTitleBar();
}

//----------------------------------------------------------------------
void FDialog::pressZoomButton (mouseStates& ms)
{
  if ( ms.mouse_x <= int(getWidth() - ms.zoom_btn)
    || ms.mouse_y != 1
    || ! zoom_button_pressed )
    return;

  // Zoom to maximum or restore the window size
  zoomWindow();
  setZoomItem();
}

//----------------------------------------------------------------------
inline bool FDialog::isMouseOverMenu (const FPoint& termpos)
{
  const FRect& menu_geometry = dialog_menu->getTermGeometry();

  if ( dialog_menu->getCount() > 0 && menu_geometry.contains(termpos) )
    return true;

  return false;
}

//----------------------------------------------------------------------
inline void FDialog::passEventToSubMenu ( mouseStates& ms
                                        , FMouseEvent* ev )
{
  // Mouse event handover to the dialog menu
  if ( ! ms.mouse_over_menu
    || ! dialog_menu->isVisible()
    || ! dialog_menu->isShown() )
    return;

  const FPoint& g = ms.termPos;
  const FPoint& p = dialog_menu->termToWidgetPos(g);
  int b = ev->getButton();

  try
  {
    FMouseEvent* _ev = new FMouseEvent (fc::MouseMove_Event, p, g, b);
    dialog_menu->mouse_down = true;
    setClickedWidget(dialog_menu);
    dialog_menu->onMouseMove(_ev);
    delete _ev;
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
    return;
  }
}

//----------------------------------------------------------------------
inline void FDialog::moveSizeKey (FKeyEvent* ev)
{
  switch ( ev->key() )
  {
    case fc::Fkey_up:
      moveUp(1);
      ev->accept();
      break;

    case fc::Fkey_down:
      moveDown(1);
      ev->accept();
      break;

    case fc::Fkey_left:
      moveLeft(1);
      ev->accept();
      break;

    case fc::Fkey_right:
      moveRight(1);
      ev->accept();
      break;

    case fc::Fmkey_up:
    case fc::Fkey_sr:
      if ( reduceHeight(1) )
        ev->accept();
      break;

    case fc::Fmkey_down:
    case fc::Fkey_sf:
      if ( expandHeight(1) )
        ev->accept();
      break;

    case fc::Fmkey_left:
    case fc::Fkey_sleft:
      if ( reduceWidth(1) )
        ev->accept();
      break;

    case fc::Fmkey_right:
    case fc::Fkey_sright:
      if ( expandWidth(1) )
        ev->accept();
      break;

    case fc::Fkey_return:
    case fc::Fkey_enter:
      acceptMoveSize();
      ev->accept();
      break;

    case fc::Fkey_escape:
    case fc::Fkey_escape_mintty:
      cancelMoveSize();
      ev->accept();
      return;

    default:
      break;
  }
}

//----------------------------------------------------------------------
inline void FDialog::raiseActivateDialog()
{
  bool has_raised = raiseWindow();
  activateDialog();

  if ( has_raised )
    redraw();
}

//----------------------------------------------------------------------
inline void FDialog::lowerActivateDialog()
{
  bool has_lowered = lowerWindow();

  if ( ! isWindowActive() )
    activateDialog();
  else if ( has_lowered )
    updateTerminal();
}

//----------------------------------------------------------------------
bool FDialog::isLowerRightResizeCorner (mouseStates& ms)
{
  // 3 characters in the lower right corner  |
  //                                         x
  //                                   -----xx

  if ( (ms.mouse_x == int(getWidth()) && ms.mouse_y == int(getHeight()) - 1)
    || ( ( ms.mouse_x == int(getWidth()) - 1
        || ms.mouse_x == int(getWidth()) ) && ms.mouse_y == int(getHeight()) ) )
  {
    return true;
  }
  else
  {
    return false;
  }
}

//----------------------------------------------------------------------
void FDialog::resizeMouseDown (mouseStates& ms)
{
  // Click on the lower right resize corner

  if ( isResizeable() && isLowerRightResizeCorner(ms) )
  {
    resize_click_pos = ms.termPos;
    FPoint lower_right_pos = getTermGeometry().getLowerRightPos();

    if ( ms.termPos != lower_right_pos )
    {
      FPoint deltaPos = ms.termPos - lower_right_pos;
      int w = lower_right_pos.getX() + deltaPos.getX() - getTermX() + 1;
      int h = lower_right_pos.getY() + deltaPos.getY() - getTermY() + 1;
      setSize (std::size_t(w), std::size_t(h));
    }
    else
      drawBorder();
  }
  else
    resize_click_pos.setPoint (0, 0);
}

//----------------------------------------------------------------------
void FDialog::resizeMouseUpMove (mouseStates& ms, bool mouse_up)
{
  // Resize the dialog
  if ( isResizeable() && ! resize_click_pos.isNull() )
  {
    FWidget* r = getRootWidget();
    resize_click_pos = ms.termPos;
    int x2 = resize_click_pos.getX()
      , y2 = resize_click_pos.getY()
      , x2_offset = 0
      , y2_offset = 0;

    if ( r )
    {
      x2_offset = r->getLeftPadding();
      y2_offset = r->getTopPadding();
    }

    if ( ms.termPos != getTermGeometry().getLowerRightPos() )
    {
      int w, h;
      FPoint deltaPos = ms.termPos - resize_click_pos;

      if ( x2 - x2_offset <= int(getMaxWidth()) )
        w = resize_click_pos.getX() + deltaPos.getX() - getTermX() + 1;
      else
        w = int(getMaxWidth()) - getTermX() + x2_offset + 1;

      if ( y2 - y2_offset <= int(getMaxHeight()) )
        h = resize_click_pos.getY() + deltaPos.getY() - getTermY() + 1;
      else
        h = int(getMaxHeight()) - getTermY() + y2_offset + 1;

      setSize ( ( w >= 0) ? std::size_t(w) : 0
              , ( h >= 0) ? std::size_t(h) : 0 );
    }

    if ( mouse_up )
    {
      // Reset the border color
      resize_click_pos.setPoint (0, 0);

      // redraw() is required to draw the standard (black) border
      // and client objects with ignorePadding() option.
      redraw();
    }
  }
}

//----------------------------------------------------------------------
void FDialog::cancelMouseResize()
{
  // Cancel resize by mouse

  if ( resize_click_pos.isNull() )
    return;

  resize_click_pos.setPoint (0, 0);
  drawBorder();
  updateTerminal();
}

//----------------------------------------------------------------------
inline void FDialog::acceptMoveSize()
{
  setMoveSizeWidget(0);

  if ( tooltip )
    delete tooltip;

  tooltip = 0;
  redraw();
}

//----------------------------------------------------------------------
inline void FDialog::cancelMoveSize()
{
  setMoveSizeWidget(0);

  if ( tooltip )
    delete tooltip;

  tooltip = 0;
  setPos (save_geometry.getPos());

  if ( isResizeable() )
    setSize (save_geometry.getWidth(), save_geometry.getHeight());

  redraw();
}

//----------------------------------------------------------------------
void FDialog::addDialog (FWidget* obj)
{
  // Add the dialog object obj to the dialog list
  if ( dialog_list )
    dialog_list->push_back(obj);
}

//----------------------------------------------------------------------
void FDialog::delDialog (FWidget* obj)
{
  // Delete the dialog object obj from the dialog list
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

  try
  {
    tooltip = new FToolTip(this);
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
    return;
  }

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

}  // namespace finalcut
