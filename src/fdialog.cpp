/***********************************************************************
* fdialog.cpp - Widget FDialog                                         *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2012-2019 Markus Gans                                      *
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

#include <memory>

#include "final/fapplication.h"
#include "final/fdialog.h"
#include "final/fevent.h"
#include "final/fmenuitem.h"
#include "final/fstatusbar.h"
#include "final/ftooltip.h"
#include "final/fwidgetcolors.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FDialog
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FDialog::FDialog (FWidget* parent)
  : FWindow(parent)
{
  init();
}

//----------------------------------------------------------------------
FDialog::FDialog (const FString& txt, FWidget* parent)
  : FWindow(parent)
  , tb_text(txt)
{
  init();
}

//----------------------------------------------------------------------
FDialog::~FDialog()  // destructor
{
  auto fapp = FApplication::getApplicationObject();
  bool is_quit = fapp->isQuit();
  delete dialog_menu;
  dgl_menuitem = nullptr;

  if ( ! is_quit )
    switchToPrevWindow(this);

  delDialog(this);

  if ( isModal() )
    unsetModal();
}


// public methods of FDialog
//----------------------------------------------------------------------
bool FDialog::setDialogWidget (bool enable)
{
  if ( isDialogWidget() == enable )
    return true;

  setFlags().dialog_widget = enable;

  if ( enable )
    setTermOffsetWithPadding();
  else
    setParentOffset();

  return enable;
}

//----------------------------------------------------------------------
bool FDialog::setModal (bool enable)
{
  if ( isModal() == enable )
    return true;

  setFlags().modal = enable;

  if ( enable )
    setModalDialogCounter()++;
  else
    setModalDialogCounter()--;

  return enable;
}


//----------------------------------------------------------------------
bool FDialog::setScrollable (bool enable)
{
  return (setFlags().scrollable = enable);
}

//----------------------------------------------------------------------
bool FDialog::setResizeable (bool enable)
{
  FWindow::setResizeable (enable);

  if ( enable )
    zoom_item->setEnable();
  else
    zoom_item->setDisable();

  return enable;
}

//----------------------------------------------------------------------
void FDialog::show()
{
  if ( ! isVisible() )
    return;

  FWindow::show();

  if ( isModal() )
  {
    auto fapp = FApplication::getApplicationObject();
    fapp->enterLoop();

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
    auto fapp = FApplication::getApplicationObject();
    fapp->exitLoop();
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
void FDialog::setPos (const FPoint& pos, bool)
{
  setPos_error = false;

  // Avoid to move widget completely outside the terminal
  // or moving a zoomed dialog or a motionless dialog
  if ( isOutsideTerminal(pos) || isZoomed() || getPos() == pos )
  {
    setPos_error = true;
    return;
  }

  int dx = getX() - pos.getX();
  int dy = getY() - pos.getY();
  int old_x = getTermX();
  int old_y = getTermY();
  const auto& shadow = getShadow();
  std::size_t width = getWidth() + shadow.getWidth();  // width + right shadow
  std::size_t height = getHeight() + shadow.getHeight();  // height + bottom shadow
  FRect old_geometry (getTermGeometryWithShadow());

  // move to the new position
  FWindow::setPos(pos, false);
  putArea (getTermPos(), getVWin());

  // restoring the non-covered terminal areas
  if ( getTermGeometry().overlap(old_geometry) )
  {
    FRect restore{};
    std::size_t d_width = std::size_t(std::abs(dx));
    std::size_t d_height = std::size_t(std::abs(dy));

    // dx > 0 : move left
    // dx = 0 : move vertical
    // dx < 0 : move right
    // dy > 0 : move up
    // dy = 0 : move horizontal
    // dy < 0 : move down

    if ( dx > 0 )
    {
      if ( dy > 0 )
        restore.setRect ( old_x + int(width) - dx, old_y
                        , d_width, height - d_height );
      else
        restore.setRect ( old_x + int(width) - dx, old_y - dy
                        , d_width, height - d_height );
    }
    else
    {
      if ( dy > 0 )
        restore.setRect (old_x, old_y, d_width, height - d_height);
      else
        restore.setRect (old_x, old_y - dy, d_width, height - d_height);
    }

    restoreVTerm (restore);

    if ( dy > 0 )
      restore.setRect ( old_x, old_y + int(height) - dy, width, d_height);
    else
      restore.setRect ( old_x, old_y, width, d_height);

    restoreVTerm (restore);
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
void FDialog::move (const FPoint& d_pos)
{
  setPos (getPos() + d_pos);
}

//----------------------------------------------------------------------
inline bool FDialog::moveUp (int n)
{
  move (FPoint(0, -n));
  return ! setPos_error;
}

//----------------------------------------------------------------------
inline bool FDialog::moveDown (int n)
{
  move (FPoint(0, n));
  return ! setPos_error;
}

//----------------------------------------------------------------------
inline bool FDialog::moveLeft (int n)
{
  move (FPoint(-n, 0));
  return ! setPos_error;
}

//----------------------------------------------------------------------
inline bool FDialog::moveRight (int n)
{
  move (FPoint(n, 0));
  return ! setPos_error;
}

//----------------------------------------------------------------------
void FDialog::setSize (const FSize& size, bool adjust)
{
  setSize_error = false;

  if ( getSize() == size || isZoomed() )
  {
    setSize_error = true;
    return;
  }

  int x = getTermX();
  int y = getTermY();
  int dw = int(getWidth()) - int(size.getWidth());
  int dh = int(getHeight()) - int(size.getHeight());
  const auto& shadow = getShadow();
  FWindow::setSize (size, adjust);

  // get adjust width and height
  std::size_t w = getWidth() + shadow.getWidth();
  std::size_t h = getHeight() + shadow.getHeight();

  // dw > 0 : scale down width
  // dw = 0 : scale only height
  // dw < 0 : scale up width
  // dh > 0 : scale down height
  // dh = 0 : scale only width
  // dh < 0 : scale up height

  std::size_t d_width = std::size_t(dw);
  std::size_t d_height = std::size_t(dh);

  // restoring the non-covered terminal areas
  if ( dw > 0 )
    restoreVTerm (FRect(x + int(w), y, d_width, h + d_height));  // restore right

  if ( dh > 0 )
    restoreVTerm (FRect(x, y + int(h), w + d_width, d_height));  // restore bottom

  redraw();

  // handle overlaid windows
  restoreOverlaidWindows();

  // set the cursor to the focus widget
  setCursorToFocusWidget();
}

//----------------------------------------------------------------------
bool FDialog::reduceHeight (int n)
{
  if ( ! isResizeable() )
    return false;

  setSize (FSize(getWidth(), getHeight() - std::size_t(n)));
  return ! setSize_error;
}

//----------------------------------------------------------------------
bool FDialog::expandHeight (int n)
{
  if ( ! isResizeable() || getHeight() + std::size_t(getY()) > getMaxHeight() )
    return false;

  setSize (FSize(getWidth(), getHeight() + std::size_t(n)));
  return ! setSize_error;
}

//----------------------------------------------------------------------
bool FDialog::reduceWidth (int n)
{
  if ( ! isResizeable() )
    return false;

  setSize (FSize(getWidth() - std::size_t(n), getHeight()));
  return ! setSize_error;
}

//----------------------------------------------------------------------
bool FDialog::expandWidth (int n)
{
  if ( ! isResizeable() || getWidth() + std::size_t(getX()) > getMaxWidth() )
    return false;

  setSize (FSize(getWidth() + std::size_t(n), getHeight()));
  return ! setSize_error;
}

//----------------------------------------------------------------------
void FDialog::activateDialog()
{
  if ( isWindowActive() )
    return;

  auto old_focus = FWidget::getFocusWidget();
  auto win_focus = getWindowFocusWidget();
  setActiveWindow(this);
  setFocus();
  setFocusWidget(this);

  if ( win_focus )
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
      && dialog_menu->isShown() )
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

    if ( ! titlebar_click_pos.isOrigin()
      && titlebar_x > int(getTermX()) + 3
      && titlebar_x < getTermX() + int(getWidth())
      && titlebar_y == int(getTermY()) )
    {
      FPoint deltaPos(ms.termPos - titlebar_click_pos);
      move (deltaPos);
      titlebar_click_pos = ms.termPos;
    }

    // Click on titlebar menu button
    if ( ms.mouse_x < 4
      && ms.mouse_y == 1
      && dialog_menu->isShown()
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

  if ( ! titlebar_click_pos.isOrigin() )
  {
    FPoint deltaPos(ms.termPos - titlebar_click_pos);
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

  if ( ev->getButton() != fc::LeftButton )
    return;

  int x = getTermX();
  int y = getTermY();
  FRect title_button(x, y, 3, 1);
  FPoint tPos(ms.termPos);

  if ( title_button.contains(tPos) )
  {
    // Double click on title button
    dialog_menu->unselectItem();
    dialog_menu->hide();
    activateWindow();
    raiseWindow();
    auto window_focus_widget = getWindowFocusWidget();

    if ( window_focus_widget )
      window_focus_widget->setFocus();

    setClickedWidget(nullptr);

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
  if ( isShown() )
    drawTitleBar();

  if ( ! FWidget::getFocusWidget() )
  {
    auto win_focus = getWindowFocusWidget();

    if ( win_focus
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
  if ( dialog_menu && ! dialog_menu->isShown() )
    FWindow::setPreviousWindow(this);

  if ( isShown() && isEnabled() )
    drawTitleBar();

  if ( hasFocus() )
    unsetFocus();
}

//----------------------------------------------------------------------
void FDialog::onWindowRaised (FEvent*)
{
  if ( ! isShown() )
    return;

  putArea (getTermPos(), getVWin());

  // Handle always-on-top windows
  if ( getAlwaysOnTopList() && ! getAlwaysOnTopList()->empty() )
  {
    for (auto&& win : *getAlwaysOnTopList())
      putArea (win->getTermPos(), win->getVWin());
  }
}

//----------------------------------------------------------------------
void FDialog::onWindowLowered (FEvent*)
{
  if ( ! getWindowList() )
    return;

  if ( getWindowList()->empty() )
    return;

  for (auto&& win : *getWindowList())
    putArea (win->getTermPos(), win->getVWin());
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
    delete tooltip;
    tooltip = nullptr;
  }

  // Fill the background
  setColor();

  if ( isMonochron() )
    setReverse(true);

  clearArea();
  drawBorder();
  drawTitleBar();
  setCursorPos(FPoint(2, int(getHeight()) - 1));

  if ( getFlags().shadow )
    drawDialogShadow();

  if ( isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
void FDialog::drawDialogShadow()
{
  if ( isMonochron() && ! getFlags().trans_shadow )
    return;

  drawShadow(this);
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
  setTopPadding(2);
  setLeftPadding(1);
  setBottomPadding(1);
  setRightPadding(1);
  ignorePadding();
  setDialogWidget();
  // Initialize geometry values
  setGeometry (FPoint(1, 1), FSize(10, 10), false);
  setMinimumSize (FSize(15, 4));
  addDialog(this);
  setActiveWindow(this);
  setTransparentShadow();
  const auto& wc = getFWidgetColors();
  setForegroundColor (wc.dialog_fg);
  setBackgroundColor (wc.dialog_bg);
  auto old_focus = FWidget::getFocusWidget();

  if ( old_focus )
  {
    setFocus();
    old_focus->redraw();
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

  FPoint p(getPos());
  p.y_ref()++;
  dialog_menu->setPos(p);
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
  if ( (getMoveSizeWidget() == this || ! resize_click_pos.isOrigin() )
    && ! isZoomed() )
  {
    const auto& wc = getFWidgetColors();
    setColor (wc.dialog_resize_fg, getBackgroundColor());
  }
  else
    setColor();

  if ( isNewFont() )  // Draw a newfont U-shaped frame
  {
    FRect r(FPoint(1, 1), getSize());

    for (int y = r.getY1() + 1; y < r.getY2(); y++)
    {
      print() << FPoint(r.getX1(), y)
              << fc::NF_border_line_left        // border left ⎸
              << FPoint(r.getX2(), y)
              << fc::NF_rev_border_line_right;  // border right⎹
    }

    print() << r.getLowerLeftPos()
            << fc::NF_border_corner_lower_left        // ⎣
            << FString(r.getWidth() - 2, fc::NF_border_line_bottom)  // _
            << fc::NF_rev_border_corner_lower_right;  // ⎦
  }
  else
  {
    FRect box(FPoint(1, 2), getSize());
    box.scaleBy(0, -1);
    finalcut::drawBorder(this, box);
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
    print() << FPoint(int(getWidth()) - 2, 1);
    printf ("(%d)", getWindowLayer(this));
  }
#endif  // DEBUG
}

//----------------------------------------------------------------------
void FDialog::drawBarButton()
{
  // Print the title button
  print() << FPoint(1, 1);
  const auto& wc = getFWidgetColors();

  if ( dialog_menu && dialog_menu->isShown() )
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

  const auto& wc = getFWidgetColors();

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
  std::size_t center_offset{0};
  std::size_t x{1};
  const auto& wc = getFWidgetColors();

  if ( getMaxColor() < 16 )
    setBold();

  if ( isWindowActive() || (dialog_menu && dialog_menu->isShown()) )
    setColor (wc.titlebar_active_fg, wc.titlebar_active_bg);
  else
    setColor (wc.titlebar_inactive_fg, wc.titlebar_inactive_bg);

  auto width = getWidth();
  auto zoom_btn = getZoomButtonWidth();
  auto length = getColumnWidth(tb_text);

  if ( width > length + MENU_BTN + zoom_btn )
    center_offset = (width - length - MENU_BTN - zoom_btn) / 2;

  for ( ; x <= center_offset; x++)
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

  if ( ! getWindowList() || getWindowList()->empty() )
    return;

  bool overlaid{false};

  for (auto&& win : *getWindowList())
  {
    if ( overlaid )
      putArea (win->getTermPos(), win->getVWin());

    if ( getVWin() == win->getVWin() )
      overlaid = true;
  }
}

//----------------------------------------------------------------------
void FDialog::setCursorToFocusWidget()
{
  // Set the cursor to the focus widget

  auto focus = FWidget::getFocusWidget();

  if ( focus
    && focus->isShown()
    && focus->hasVisibleCursor() )
  {
    FPoint cursor_pos(focus->getCursorPos());
    focus->setCursorPos(cursor_pos);
    updateVTermCursor(getVWin());
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
  flushOutputBuffer();
}

//----------------------------------------------------------------------
void FDialog::openMenu()
{
  // Open the titlebar menu
  if ( ! dialog_menu )
    return;

  if ( dialog_menu->isShown() )
  {
    leaveMenu();
    drawTitleBar();
  }
  else
  {
    setOpenMenu(dialog_menu);
    FPoint pos(getPos());
    pos.y_ref()++;
    dialog_menu->setPos (pos);
    dialog_menu->setVisible();
    dialog_menu->show();
    dialog_menu->raiseWindow();
    dialog_menu->redraw();
    drawTitleBar();
  }
}

//----------------------------------------------------------------------
void FDialog::selectFirstMenuItem()
{
  // Focus to the first enabled menu item
  dialog_menu->selectFirstItem();
  auto first_item = dialog_menu->getSelectedItem();

  if ( first_item )
    first_item->setFocus();

  dialog_menu->redraw();

  if ( getStatusBar() )
    getStatusBar()->drawMessage();

  updateTerminal();
  flushOutputBuffer();
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
inline void FDialog::activateZoomButton (const mouseStates& ms)
{
  if ( ms.mouse_x <= int(getWidth() - ms.zoom_btn)
    || ms.mouse_y != 1 )
    return;

  zoom_button_pressed = true;
  zoom_button_active = true;
  drawTitleBar();
}

//----------------------------------------------------------------------
inline void FDialog::leaveZoomButton (const mouseStates& ms)
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
void FDialog::pressZoomButton (const mouseStates& ms)
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
  const auto& menu_geometry = dialog_menu->getTermGeometry();

  if ( dialog_menu->getCount() > 0 && menu_geometry.contains(termpos) )
    return true;

  return false;
}

//----------------------------------------------------------------------
inline void FDialog::passEventToSubMenu ( const mouseStates& ms
                                        , FMouseEvent* ev )
{
  // Mouse event handover to the dialog menu
  if ( ! ms.mouse_over_menu
    || ! dialog_menu->isShown() )
    return;

  const auto& g = ms.termPos;
  const auto& p = dialog_menu->termToWidgetPos(g);
  int b = ev->getButton();

  try
  {
    const auto& _ev = \
        std::make_shared<FMouseEvent>(fc::MouseMove_Event, p, g, b);
    dialog_menu->mouse_down = true;
    setClickedWidget(dialog_menu);
    dialog_menu->onMouseMove(_ev.get());
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
bool FDialog::isOutsideTerminal (const FPoint& pos)
{
  if ( pos.getX() + int(getWidth()) <= 1
    || pos.getX() > int(getMaxWidth())
    || pos.getY() < 1
    || pos.getY() > int(getMaxHeight()) )
    return true;

  return false;
}

//----------------------------------------------------------------------
bool FDialog::isLowerRightResizeCorner (const mouseStates& ms)
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
void FDialog::resizeMouseDown (const mouseStates& ms)
{
  // Click on the lower right resize corner

  if ( isResizeable() && isLowerRightResizeCorner(ms) )
  {
    resize_click_pos = ms.termPos;
    FPoint lower_right_pos(getTermGeometry().getLowerRightPos());

    if ( ms.termPos != lower_right_pos )
    {
      FPoint deltaPos(ms.termPos - lower_right_pos);
      int w = lower_right_pos.getX() + deltaPos.getX() - getTermX() + 1;
      int h = lower_right_pos.getY() + deltaPos.getY() - getTermY() + 1;
      const FSize& size = FSize(std::size_t(w), std::size_t(h));
      setSize (size, true);
    }
    else
      drawBorder();
  }
  else
    resize_click_pos.setPoint (0, 0);
}

//----------------------------------------------------------------------
void FDialog::resizeMouseUpMove (const mouseStates& ms, bool mouse_up)
{
  // Resize the dialog
  if ( isResizeable() && ! resize_click_pos.isOrigin() )
  {
    auto r = getRootWidget();
    resize_click_pos = ms.termPos;
    int x2 = resize_click_pos.getX()
      , y2 = resize_click_pos.getY()
      , x2_offset{0}
      , y2_offset{0};

    if ( r )
    {
      x2_offset = r->getLeftPadding();
      y2_offset = r->getTopPadding();
    }

    if ( ms.termPos != getTermGeometry().getLowerRightPos() )
    {
      int w{}, h{};
      FPoint deltaPos(ms.termPos - resize_click_pos);

      if ( x2 - x2_offset <= int(getMaxWidth()) )
        w = resize_click_pos.getX() + deltaPos.getX() - getTermX() + 1;
      else
        w = int(getMaxWidth()) - getTermX() + x2_offset + 1;

      if ( y2 - y2_offset <= int(getMaxHeight()) )
        h = resize_click_pos.getY() + deltaPos.getY() - getTermY() + 1;
      else
        h = int(getMaxHeight()) - getTermY() + y2_offset + 1;

      const FSize size ( ( w >= 0) ? std::size_t(w) : 0
                       , ( h >= 0) ? std::size_t(h) : 0 );
      setSize (size);
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

  if ( resize_click_pos.isOrigin() )
    return;

  resize_click_pos.setPoint (0, 0);
  drawBorder();
  updateTerminal();
}

//----------------------------------------------------------------------
inline void FDialog::acceptMoveSize()
{
  setMoveSizeWidget(nullptr);

  if ( tooltip )
    delete tooltip;

  tooltip = nullptr;
  redraw();
}

//----------------------------------------------------------------------
inline void FDialog::cancelMoveSize()
{
  setMoveSizeWidget(nullptr);

  if ( tooltip )
    delete tooltip;

  tooltip = nullptr;
  setPos (save_geometry.getPos());

  if ( isResizeable() )
    setSize (save_geometry.getSize());

  redraw();
}

//----------------------------------------------------------------------
void FDialog::addDialog (FWidget* obj)
{
  // Add the dialog object obj to the dialog list
  if ( getDialogList() )
    getDialogList()->push_back(obj);
}

//----------------------------------------------------------------------
void FDialog::delDialog (FWidget* obj)
{
  // Delete the dialog object obj from the dialog list
  if ( ! getDialogList() || getDialogList()->empty() )
    return;

  auto iter = getDialogList()->begin();

  while ( iter != getDialogList()->end() )
  {
    if ( (*iter) == obj )
    {
      getDialogList()->erase(iter);
      return;
    }

    ++iter;
  }
}

//----------------------------------------------------------------------
void FDialog::cb_move (FWidget*, FDataPtr)
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
void FDialog::cb_zoom (FWidget*, FDataPtr)
{
  dialog_menu->unselectItem();
  dialog_menu->hide();
  setClickedWidget(nullptr);
  drawTitleBar();
  zoomWindow();
  setZoomItem();
}

//----------------------------------------------------------------------
void FDialog::cb_close (FWidget*, FDataPtr)
{
  dialog_menu->unselectItem();
  dialog_menu->hide();
  setClickedWidget(nullptr);
  drawTitleBar();
  close();
}

}  // namespace finalcut
