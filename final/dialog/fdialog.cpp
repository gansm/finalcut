/***********************************************************************
* fdialog.cpp - Widget FDialog                                         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2012-2023 Markus Gans                                      *
*                                                                      *
* FINAL CUT is free software; you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as       *
* published by the Free Software Foundation; either version 3 of       *
* the License, or (at your option) any later version.                  *
*                                                                      *
* FINAL CUT is distributed in the hope that it will be useful, but     *
* WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU Lesser General Public License for more details.                  *
*                                                                      *
* You should have received a copy of the GNU Lesser General Public     *
* License along with this program.  If not, see                        *
* <http://www.gnu.org/licenses/>.                                      *
***********************************************************************/

#include <memory>

#include "final/dialog/fdialog.h"
#include "final/fapplication.h"
#include "final/fevent.h"
#include "final/ftypes.h"
#include "final/fwidgetcolors.h"
#include "final/input/fkeyboard.h"
#include "final/menu/fmenubar.h"
#include "final/menu/fmenuitem.h"
#include "final/util/flog.h"
#include "final/widget/fcombobox.h"
#include "final/widget/fstatusbar.h"
#include "final/widget/ftooltip.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FDialog
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FDialog::FDialog (FWidget* parent)
  : FWindow{parent}
{
  init();
}

//----------------------------------------------------------------------
FDialog::FDialog (FString&& txt, FWidget* parent)
  : FWindow{parent}
  , tb_text{std::move(txt)}
{
  init();
}

//----------------------------------------------------------------------
FDialog::~FDialog()  // destructor
{
  delete dialog_menu;
  dgl_menuitem = nullptr;

  if ( isModal() )
    unsetModal();

  if ( ! FApplication::isQuit() )
    switchToPrevWindow(this);

  auto has_entry = bool( getDialogList() && ! getDialogList()->empty() );
  delDialog(this);
  auto fapp = FApplication::getApplicationObject();

  if ( fapp && has_entry && noVisibleDialog() )
    fapp->emitCallback("last-dialog-closed");
}


// public methods of FDialog
//----------------------------------------------------------------------
auto FDialog::setDialogWidget (bool enable) -> bool
{
  if ( isDialogWidget() == enable )
    return true;

  setFlags().type.dialog_widget = enable;

  if ( enable )
    setTermOffsetWithPadding();
  else
    setParentOffset();

  return enable;
}

//----------------------------------------------------------------------
auto FDialog::setModal (bool enable) -> bool
{
  if ( isModal() == enable )
    return true;

  setFlags().visibility.modal = enable;

  if ( enable )
  {
    setModalDialogCounter()++;
    FKeyboard::getInstance().clearKeyBuffer();
    setFlags().feature.minimizable = false;
  }
  else
    setModalDialogCounter()--;

  return enable;
}

//----------------------------------------------------------------------
auto FDialog::setBorder (bool enable) -> bool
{
  if ( enable )
  {
    setTopPadding(2);
    setLeftPadding(1);
    setBottomPadding(1);
    setRightPadding(1);
  }
  else
  {
    setTopPadding(1);
    setLeftPadding(0);
    setBottomPadding(0);
    setRightPadding(0);
  }

  return ( setFlags().feature.no_border = (! enable) );
}

//----------------------------------------------------------------------
void FDialog::resetColors()
{
  const auto& wc = getColorTheme();
  setForegroundColor (wc->dialog_fg);
  setBackgroundColor (wc->dialog_bg);
  FWidget::resetColors();
}

//----------------------------------------------------------------------
auto FDialog::setResizeable (bool enable) -> bool
{
  FWindow::setResizeable (enable);

  if ( enable )
    zoom_item->setEnable();
  else
    zoom_item->setDisable();

  return enable;
}

//----------------------------------------------------------------------
auto FDialog::setMinimizable (bool enable) -> bool
{
  FWindow::setMinimizable (enable);

  if ( enable )
    minimize_item->setEnable();
  else
    minimize_item->setDisable();

  return enable;
}

//----------------------------------------------------------------------
void FDialog::show()
{
  if ( ! isVisible() )
    return;

  FWindow::show();

  if ( isModal() && ! FApplication::isQuit() )
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
  auto fapp = FApplication::getApplicationObject();

  if ( noVisibleDialog() )
     fapp->emitCallback("last-dialog-closed");

  if ( isModal() )
    fapp->exitLoop();
}

//----------------------------------------------------------------------
auto FDialog::exec() -> ResultCode
{
  result_code = ResultCode::Reject;
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

  // Save current position values
  const auto delta_pos = getPos() - pos;
  const auto old_geometry = getTermGeometryWithShadow();

  // Move to the new position
  FWindow::setPos (pos, false);

  // Copy dialog to virtual terminal
  putArea (getTermPos(), getVWin());

  // Restore background outside the dialog
  recoverBackgroundAfterMove (delta_pos, old_geometry);

  restoreOverlaidWindows();
  FWindow::adjustSize();
  setCursorToFocusWidget();

  if ( updateTerminal() )
    flush();
}

//----------------------------------------------------------------------
void FDialog::move (const FPoint& d_pos)
{
  setPos (getPos() + d_pos);
}

//----------------------------------------------------------------------
inline auto FDialog::moveUp (int n) -> bool
{
  if ( isBottomOutside() )
  {
    const auto y_max = int(getMaxHeight());
    FWindow::setY(y_max, false);
    putArea (getTermPos(), getVWin());
    restoreOverlaidWindows();
  }
  else
    move ({0, -n});

  return ! setPos_error;
}

//----------------------------------------------------------------------
inline auto FDialog::moveDown (int n) -> bool
{
  move ({0, n});
  return ! setPos_error;
}

//----------------------------------------------------------------------
inline auto FDialog::moveLeft (int n) -> bool
{
  if ( isLeftOutside() )
  {
    const auto x_max = int(getMaxWidth());
    FWindow::setX(x_max, false);
    putArea (getTermPos(), getVWin());
    restoreOverlaidWindows();
  }
  else
    move ({-n, 0});

  return ! setPos_error;
}

//----------------------------------------------------------------------
inline auto FDialog::moveRight (int n) -> bool
{
  move ({n, 0});
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

  const int x = getTermX();
  const int y = getTermY();
  const int dw = int(getWidth()) - int(size.getWidth());
  const int dh = int(getHeight()) - int(size.getHeight());
  const auto& shadow = getShadow();
  const std::size_t old_width = getWidth() + shadow.getWidth();
  const std::size_t old_height = getHeight() + shadow.getHeight();
  FWindow::setSize (size, false);

  // get adjust width and height with shadow
  const std::size_t width = getWidth() + shadow.getWidth();
  const std::size_t height = getHeight() + shadow.getHeight();

  // dw > 0 : scale down width
  // dw = 0 : scale only height
  // dw < 0 : scale up width
  // dh > 0 : scale down height
  // dh = 0 : scale only width
  // dh < 0 : scale up height

  setTerminalUpdates (FVTerm::TerminalUpdate::Stop);

  // restoring the non-covered terminal areas
  if ( dw > 0 )
    restoreVTerm ({x + int(width), y, std::size_t(dw), old_height});  // restore right

  if ( dh > 0 )
    restoreVTerm ({x, y + int(height), old_width, std::size_t(dh)});  // restore bottom

  if ( adjust )    // Adjust the size after restoreVTerm(),
    adjustSize();  // because adjustSize() can also change x and y

  redraw();

  // handle overlaid windows
  restoreOverlaidWindows();

  // set the cursor to the focus widget
  setCursorToFocusWidget();

  setTerminalUpdates (FVTerm::TerminalUpdate::Start);
}

//----------------------------------------------------------------------
auto FDialog::reduceHeight (int n) -> bool
{
  if ( ! isResizeable() )
    return false;

  setSize (FSize{getWidth(), getHeight() - std::size_t(n)});
  return ! setSize_error;
}

//----------------------------------------------------------------------
auto FDialog::expandHeight (int n) -> bool
{
  if ( ! isResizeable() || getHeight() + std::size_t(getY()) > getMaxHeight() )
    return false;

  setSize (FSize{getWidth(), getHeight() + std::size_t(n)});
  return ! setSize_error;
}

//----------------------------------------------------------------------
auto FDialog::reduceWidth (int n) -> bool
{
  if ( ! isResizeable() )
    return false;

  setSize (FSize{getWidth() - std::size_t(n), getHeight()});
  return ! setSize_error;
}

//----------------------------------------------------------------------
auto FDialog::expandWidth (int n) -> bool
{
  if ( ! isResizeable() || getWidth() + std::size_t(getX()) > getMaxWidth() )
    return false;

  setSize (FSize{getWidth() + std::size_t(n), getHeight()});
  return ! setSize_error;
}

//----------------------------------------------------------------------
auto FDialog::zoomWindow() -> bool
{
  bool ret_val = FWindow::zoomWindow();
  setZoomItem();
  return ret_val;
}

//----------------------------------------------------------------------
auto FDialog::minimizeWindow() -> bool
{
  bool ret_val = FWindow::minimizeWindow();
  setMinimizeItem();
  return ret_val;
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

  drawStatusBarMessage();
}

//----------------------------------------------------------------------
void FDialog::onKeyPress (FKeyEvent* ev)
{
  if ( ! isEnabled() )
    return;

  cancelMouseResize();
  const auto key = ev->key();

  if ( titlebar_buttons && isDialogMenuKey(key) )
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

  if ( ! ev->isAccepted() && isEscapeKey(key) )
  {
    ev->accept();
    clearStatusBar();

    if ( isModal() )
      done (ResultCode::Reject);
    else
      close();
  }
}

//----------------------------------------------------------------------
void FDialog::onMouseDown (FMouseEvent* ev)
{
  const auto& ms = initMouseStates(*ev, false);
  deactivateMinimizeButton();
  deactivateZoomButton();

  if ( ev->getButton() == MouseButton::Left )
  {
    handleLeftMouseDown(ms);
  }
  else
  {
    handleRightAndMiddleMouseDown(ev->getButton(), ms);
  }
}

//----------------------------------------------------------------------
void FDialog::onMouseUp (FMouseEvent* ev)
{
  const auto& ms = initMouseStates(*ev, false);

  if ( ev->getButton() == MouseButton::Left )
  {
    const int titlebar_x = titlebar_click_pos.getX();
    const int titlebar_y = titlebar_click_pos.getY();

    if ( ! titlebar_click_pos.isOrigin()
      && titlebar_x > getTermX() + int(ms.menu_btn)
      && titlebar_x < getTermX() + int(getWidth())
      && titlebar_y == getTermY() )
    {
      const FPoint deltaPos{ms.termPos - titlebar_click_pos};
      move (deltaPos);
      titlebar_click_pos = ms.termPos;
      ev->setPos(ev->getPos() - deltaPos);
    }

    // Click on titlebar menu button
    if ( titlebar_buttons && isMouseOverMenuButton(ms)
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
      // Minimize the window
      pressMinimizeButton(ms);
    }

    // Resize the dialog
    resizeMouseUpMove (ms, true);
  }

  deactivateMinimizeButton();
  deactivateZoomButton();
}

//----------------------------------------------------------------------
void FDialog::onMouseMove (FMouseEvent* ev)
{
  auto mouse_over_menu = isMouseOverMenu(ev->getTermPos());
  const auto& ms = initMouseStates(*ev, mouse_over_menu);

  if ( ev->getButton() != MouseButton::Left )
    return;

  if ( ! titlebar_click_pos.isOrigin() )
  {
    const FPoint deltaPos{ms.termPos - titlebar_click_pos};
    move (deltaPos);
    titlebar_click_pos = ms.termPos;
    ev->setPos(ev->getPos() - deltaPos);
  }

  // Mouse event handover to the menu
  if ( ms.mouse_over_menu )
    passEventToSubMenu (ms, *ev);

  leaveMinimizeButton(ms);  // Check minimize button pressed
  leaveZoomButton(ms);      // Check zoom button pressed
  resizeMouseUpMove(ms);    // Resize the dialog
}

//----------------------------------------------------------------------
void FDialog::onMouseDoubleClick (FMouseEvent* ev)
{
  const auto& ms = initMouseStates(*ev, false);

  if ( ev->getButton() != MouseButton::Left )
    return;

  const int x = getTermX();
  const int y = getTermY();
  const FRect title_button{x, y, 3, 1};

  if ( isMouseOverMenuButton(ms) )
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
    clearStatusBar();

    if ( isModal() )
      done (ResultCode::Reject);
    else
      close();
  }
  else if ( isResizeable() && isMouseOverTitlebar(ms) )
  {
    // Double click on titlebar
    zoomWindow();  // window zoom/unzoom
  }
}

//----------------------------------------------------------------------
void FDialog::onAccel (FAccelEvent*)
{
  if ( ! isWindowHidden() && isMinimized() )
    minimizeWindow();  // unminimize window

  if ( isWindowHidden() || isWindowActive() )
    return;

  const auto menu_bar = getMenuBar();

  if ( menu_bar )
  {
    menu_bar->resetMenu();
    menu_bar->redraw();
  }

  const bool has_raised = raiseWindow();
  activateDialog();

  if ( has_raised )
    redraw();
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

  drawStatusBarMessage();
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

  for (auto&& window : *getWindowList())
  {
    const auto win = static_cast<FWidget*>(window);
    putArea (win->getTermPos(), win->getVWin());
  }
}


// protected methods of FDialog
//----------------------------------------------------------------------
void FDialog::done (ResultCode result)
{
  hide();
  result_code = result;
}

//----------------------------------------------------------------------
void FDialog::draw()
{
  if ( ! getMoveSizeWidget() )
  {
    delete tooltip;
    tooltip = nullptr;
  }

  // Fill the background
  setColor();

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(true);

  clearArea();
  drawBorder();
  drawTitleBar();
  setCursorPos({2, int(getHeight()) - 1});

  if ( getFlags().shadow.shadow )
    drawDialogShadow();

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
void FDialog::drawDialogShadow()
{
  if ( FVTerm::getFOutput()->isMonochron() && ! getFlags().shadow.trans_shadow )
    return;

  if ( isMinimized() )
    clearShadow(this);
  else
    drawShadow(this);
}

//----------------------------------------------------------------------
void FDialog::onClose (FCloseEvent* ev)
{
  ev->accept();
  result_code = ResultCode::Reject;
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
  setGeometry (FPoint{1, 1}, FSize{10, 10}, false);
  setMinimumSize (FSize{15, 4});
  mapKeyFunctions();
  addDialog(this);
  setActiveWindow(this);
  setTransparentShadow();
  FDialog::resetColors();
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
  catch (const std::bad_alloc&)
  {
    badAllocOutput ("FMenu");
    return;
  }

  FPoint p{getPos()};
  p.y_ref()++;
  dialog_menu->setPos(p);
  dgl_menuitem = dialog_menu->getItem();
  dgl_menuitem->ignorePadding();
  dgl_menuitem->unsetFocusable();
  initMoveSizeMenuItem (dialog_menu);  // Add the move/size menu item
  initZoomMenuItem (dialog_menu);      // Add the zoom menu item
  initMinimizeMenuItem (dialog_menu);  // Add the minimize menu item
  initCloseMenuItem (dialog_menu);     // Add the close menu item
}

//----------------------------------------------------------------------
void FDialog::initMoveSizeMenuItem (FMenu* menu)
{
  try
  {
    move_size_item = new FMenuItem (menu);
  }
  catch (const std::bad_alloc&)
  {
    badAllocOutput ("FMenuItem");
    return;
  }

  move_size_item->setText ("&Move/Size");
  move_size_item->setStatusbarMessage ("Move or change the size of the window");
  move_size_item->addCallback
  (
    "clicked",
    this,
    &FDialog::cb_move
  );
}

//----------------------------------------------------------------------
void FDialog::initZoomMenuItem (FMenu* menu)
{
  try
  {
    zoom_item = new FMenuItem (menu);
  }
  catch (const std::bad_alloc&)
  {
    badAllocOutput ("FMenuItem");
    return;
  }

  setZoomItem();
  zoom_item->setDisable();

  zoom_item->addCallback
  (
    "clicked",
    this,
    &FDialog::cb_zoom
  );
}

//----------------------------------------------------------------------
void FDialog::initMinimizeMenuItem (FMenu* menu)
{
  try
  {
    minimize_item = new FMenuItem (menu);
  }
  catch (const std::bad_alloc&)
  {
    badAllocOutput ("FMenuItem");
    return;
  }

  setMinimizeItem();
  minimize_item->setDisable();

  minimize_item->addCallback
  (
    "clicked",
    this,
    &FDialog::cb_minimize
  );
}

//----------------------------------------------------------------------
void FDialog::initCloseMenuItem (FMenu* menu)
{
  try
  {
    close_item = new FMenuItem ("&Close", menu);
  }
  catch (const std::bad_alloc&)
  {
    badAllocOutput ("FMenuItem");
    return;
  }

  close_item->setStatusbarMessage ("Close this window");

  close_item->addCallback
  (
    "clicked",
    this,
    &FDialog::cb_close
  );
}

//----------------------------------------------------------------------
inline auto
    FDialog::initMouseStates (const FMouseEvent& ev, bool mouse_over_menu) const -> MouseStates
{
  return {
           ev.getX(),
           ev.getY(),
           ev.getTermPos(),
           getMenuButtonWidth(),
           getMinimizeButtonWidth(),
           getZoomButtonWidth(),
           mouse_over_menu
         };
}

//----------------------------------------------------------------------
inline void FDialog::mapKeyFunctions()
{
  key_map =
  {
    { FKey::Up            , [this] () { moveUp(1); } },
    { FKey::Down          , [this] () { moveDown(1); } },
    { FKey::Left          , [this] () { moveLeft(1); } },
    { FKey::Right         , [this] () { moveRight(1); } },
    { FKey::Meta_up       , [this] () { reduceHeight(1); } },
    { FKey::Shift_up      , [this] () { reduceHeight(1); } },
    { FKey::Meta_down     , [this] () { expandHeight(1); } },
    { FKey::Shift_down    , [this] () { expandHeight(1); } },
    { FKey::Meta_left     , [this] () { reduceWidth(1); } },
    { FKey::Shift_left    , [this] () { reduceWidth(1); } },
    { FKey::Meta_right    , [this] () { expandWidth(1); } },
    { FKey::Shift_right   , [this] () { expandWidth(1); } },
    { FKey::Return        , [this] () { acceptMoveSize(); } },
    { FKey::Enter         , [this] () { acceptMoveSize(); } },
    { FKey::Escape        , [this] () { cancelMoveSize(); } },
    { FKey::Escape_mintty , [this] () { cancelMoveSize(); } }
  };
}

//----------------------------------------------------------------------
inline void FDialog::recoverBackgroundAfterMove ( const FPoint& delta_pos
                                                , const FRect& old_geometry )
{
  // restoring the non-covered terminal areas
  if ( getTermGeometry().overlap(old_geometry) && ! isMinimized() )
  {
    const auto& shadow = getShadow();
    const auto width = getWidth() + shadow.getWidth();  // width + right shadow
    const auto height = getHeight() + shadow.getHeight();  // height + bottom shadow
    const int dx = delta_pos.getX();
    const int dy = delta_pos.getY();
    const int old_x = old_geometry.getX();
    const int old_y = old_geometry.getY();
    const auto delta_width = std::size_t(std::abs(dx));
    const auto delta_height = std::size_t(std::abs(dy));

    // dx > 0 : move left
    // dx = 0 : move vertical
    // dx < 0 : move right
    // dy > 0 : move up
    // dy = 0 : move horizontal
    // dy < 0 : move down
    int x1 = ( dx > 0 ) ? old_x + int(width) - dx : old_x;
    int y1 = ( dy > 0 ) ? old_y : old_y - dy;
    int x2 = old_x;
    int y2 = ( dy > 0 ) ? old_y + int(height) - dy : old_y;
    FRect vertical{x1, y1, delta_width, height - delta_height};
    FRect horizontal{x2, y2, width, delta_height};
    // Restore the terminal in the two rectangles
    restoreVTerm (vertical);
    restoreVTerm (horizontal);
  }
  else
  {
    // If the terminal does not overlap or the dialog is minimized,
    // restore the entire old geometry
    restoreVTerm (old_geometry);
  }
}

//----------------------------------------------------------------------
void FDialog::drawBorder()
{
  if ( ! hasBorder() )
    return;

  if ( (getMoveSizeWidget() == this || ! resize_click_pos.isOrigin() )
    && ! isZoomed() )
  {
    const auto& wc = getColorTheme();
    setColor (wc->dialog_resize_fg, getBackgroundColor());
  }
  else
    setColor();

  if ( FVTerm::getFOutput()->isNewFont() )  // Draw a newfont U-shaped frame
  {
    const FRect r{{1, 1}, getSize()};

    for (auto y = r.getY1() + 1; y < r.getY2(); y++)
    {
      print() << FPoint{r.getX1(), y}
              << UniChar::NF_border_line_left        // border left ⎸
              << FPoint{r.getX2(), y}
              << UniChar::NF_rev_border_line_right;  // border right⎹
    }

    print() << r.getLowerLeftPos()
            << UniChar::NF_border_corner_lower_left        // ⎣
            << FString{r.getWidth() - 2, UniChar::NF_border_line_bottom}  // _
            << UniChar::NF_rev_border_corner_lower_right;  // ⎦
  }
  else
  {
    FRect box{{1, 2}, getSize()};
    box.scaleBy(0, -1);
    finalcut::drawBorder(this, box);
  }
}

//----------------------------------------------------------------------
void FDialog::drawTitleBar()
{
  print() << FPoint{1, 1};

  if ( FVTerm::getFOutput()->isMonochron() )
  {
    if ( isWindowActive() )
      setReverse(false);
    else
      setReverse(true);
  }

  if ( titlebar_buttons )
  {
    drawBarButton();       // Draw the title button
    drawTextBar();         // Print the text bar
    drawMinimizeButton();  // Draw the minimize button
    drawZoomButton();      // Draw the zoom/unzoom button
  }
  else
    drawTextBar();         // Print the text bar

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);

#if DEBUG
  if ( PRINT_WIN_NUMBER )
  {
    // Print the number of window in stack
    print() << FPoint{int(getWidth()) - 2, 1};
    printf ("(%d)", getWindowLayer(this));
  }
#endif  // DEBUG
}

//----------------------------------------------------------------------
void FDialog::drawBarButton()
{
  // Print the title button
  const auto& wc = getColorTheme();

  if ( dialog_menu && dialog_menu->isShown() )
    setColor (wc->titlebar_button_focus_fg, wc->titlebar_button_focus_bg);
  else
    setColor (wc->titlebar_button_fg, wc->titlebar_button_bg);

  if ( FVTerm::getFOutput()->isNewFont() )
  {
    print (finalcut::NF_menu_button);
  }
  else if ( FVTerm::getFOutput()->isMonochron() )
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

  const auto& wc = getColorTheme();

  if ( zoom_button_pressed )
    setColor (wc->titlebar_button_focus_fg, wc->titlebar_button_focus_bg);
  else
    setColor (wc->titlebar_button_fg, wc->titlebar_button_bg);

  if ( isZoomed() )
    printRestoreSizeButton();
  else
    printZoomedButton();
}

//----------------------------------------------------------------------
void FDialog::drawMinimizeButton()
{
  // Draw the minimize/unminimize button

  if ( ! isMinimizable() )
    return;

  const auto& wc = getColorTheme();

  if ( minimize_button_pressed )
    setColor (wc->titlebar_button_focus_fg, wc->titlebar_button_focus_bg);
  else
    setColor (wc->titlebar_button_fg, wc->titlebar_button_bg);

  printMinimizeButton();
}

//----------------------------------------------------------------------
inline void FDialog::printRestoreSizeButton()
{
  if ( FVTerm::getFOutput()->isNewFont() )
  {
    print (finalcut::NF_button_down);
  }
  else
  {
    if ( FVTerm::getFOutput()->isMonochron() )
    {
      print ('[');
      print (UniChar::BlackDiamondSuit);  // ◆
      print (']');
    }
    else
    {
      print (' ');
      print (UniChar::BlackDiamondSuit);  // ◆
      print (' ');
    }
  }
}

//----------------------------------------------------------------------
inline void FDialog::printZoomedButton()
{
  if ( FVTerm::getFOutput()->isNewFont() )
  {
    print (finalcut::NF_button_up);
  }
  else
  {
    if ( FVTerm::getFOutput()->isMonochron() )
    {
      print ('[');
      print (UniChar::BlackUpPointingTriangle);  // ▲
      print (']');
    }
    else
    {
      print (' ');
      print (UniChar::BlackUpPointingTriangle);  // ▲
      print (' ');
    }
  }
}

//----------------------------------------------------------------------
inline void FDialog::printMinimizeButton()
{
  if ( FVTerm::getFOutput()->isNewFont() )
  {
    print (finalcut::NF_button_down);
  }
  else
  {
    if ( FVTerm::getFOutput()->isMonochron() )
    {
      print ('[');
      print (UniChar::BlackDownPointingTriangle);  // ▼
      print (']');
    }
    else
    {
      print (' ');
      print (UniChar::BlackDownPointingTriangle);  // ▼
      print (' ');
    }
  }
}

//----------------------------------------------------------------------
void FDialog::drawTextBar()
{
  // Fill with spaces (left of the title)

  if ( FVTerm::getFOutput()->getMaxColor() < 16 )
    setBold();

  const auto& wc = getColorTheme();

  if ( isWindowActive() || (dialog_menu && dialog_menu->isShown()) )
    setColor (wc->titlebar_active_fg, wc->titlebar_active_bg);
  else
    setColor (wc->titlebar_inactive_fg, wc->titlebar_inactive_bg);

  const auto width = getWidth();
  const auto menu_btn = getMenuButtonWidth();
  const auto zoom_btn = getZoomButtonWidth();
  const auto minimize_btn = getMinimizeButtonWidth();
  const auto tb_width = width - menu_btn - minimize_btn - zoom_btn;
  auto text_width = getColumnWidth(tb_text);
  std::size_t leading_space{0};

  if ( width > text_width + menu_btn + minimize_btn + zoom_btn )
    leading_space = (tb_width - text_width) / 2;

  // Print leading whitespace
  print (FString(leading_space, L' '));

  // Print title bar text
  if ( ! tb_text.isEmpty() )
  {
    if ( text_width <= tb_width )
      print (tb_text);
    else
    {
      // Print ellipsis
      const auto len = getLengthFromColumnWidth (tb_text, tb_width - 2);
      print (tb_text.left(len));
      print ("..");
      text_width = len + 2;
    }
  }

  // Print trailing whitespace
  std::size_t trailing_space = width - leading_space - text_width
                             - menu_btn - minimize_btn - zoom_btn;
  print (FString(trailing_space, L' '));

  if ( FVTerm::getFOutput()->getMaxColor() < 16 )
    unsetBold();
}

//----------------------------------------------------------------------
void FDialog::clearStatusBar() const
{
  if ( ! getStatusBar() )
    return;

  getStatusBar()->clearMessage();
  getStatusBar()->drawMessage();
}

//----------------------------------------------------------------------
void FDialog::restoreOverlaidWindows()
{
  // Restoring overlaid windows

  if ( ! getWindowList() || getWindowList()->empty() )
    return;

  bool overlaid{false};

  for (auto&& window : *getWindowList())
  {
    const auto win = static_cast<FWidget*>(window);

    if ( overlaid )
      putArea (win->getTermPos(), win->getVWin());
    else if ( getVWin() == win->getVWin() )
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
    const FPoint cursor_pos{focus->getCursorPos()};
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
  drawStatusBarMessage();
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
    finalcut::closeOpenComboBox();
    setOpenMenu(dialog_menu);
    FPoint pos{getPos()};
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
  drawStatusBarMessage();
}

//----------------------------------------------------------------------
void FDialog::setMinimizeItem()
{
  if ( isMinimized() )
  {
    minimize_item->setText ("&Unminimize");
    minimize_item->setStatusbarMessage ("Restore the original window size");
  }
  else
  {
    minimize_item->setText ("&Minimize");
    minimize_item->setStatusbarMessage ("Minimizes the window");
  }

  if ( getFlags().shadow.shadow )
    drawDialogShadow();
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
inline auto FDialog::getMenuButtonWidth() const -> std::size_t
{
  return titlebar_buttons ? 3 : 0;
}

//----------------------------------------------------------------------
inline auto FDialog::getZoomButtonWidth() const -> std::size_t
{
  if ( titlebar_buttons && isResizeable() )
    return FVTerm::getFOutput()->isNewFont() ? 2 : 3;

  return 0;
}

//----------------------------------------------------------------------
inline auto FDialog::getMinimizeButtonWidth() const -> std::size_t
{
  if ( titlebar_buttons && isMinimizable() )
    return FVTerm::getFOutput()->isNewFont() ? 2 : 3;

  return 0;
}

//----------------------------------------------------------------------
inline void FDialog::activateMinimizeButton (const MouseStates& ms)
{
  if ( ! isMouseOverMinimizeButton(ms) )
    return;

  minimize_button_pressed = true;
  minimize_button_active = true;
  zoom_button_pressed = false;
  zoom_button_active = false;
  drawTitleBar();
}

//----------------------------------------------------------------------
inline void FDialog::deactivateMinimizeButton()
{
  if ( ! minimize_button_pressed && ! minimize_button_active )
    return;

  minimize_button_pressed = false;
  minimize_button_active = false;
  drawTitleBar();
}

//----------------------------------------------------------------------
inline void FDialog::leaveMinimizeButton (const MouseStates& ms)
{
  bool minimize_button_pressed_before = minimize_button_pressed;

  if ( isMouseOverMinimizeButton(ms) && minimize_button_active )
    minimize_button_pressed = true;
  else if ( minimize_button_pressed )
    minimize_button_pressed = false;

  if ( minimize_button_pressed_before != minimize_button_pressed )
    drawTitleBar();
}

//----------------------------------------------------------------------
void FDialog::pressMinimizeButton (const MouseStates& ms)
{
  if ( ! isMouseOverMinimizeButton(ms) || ! minimize_button_pressed )
    return;

  // Zoom to maximum or restore the window size
  minimizeWindow();
  setMinimizeItem();
  minimize_button_active = false;
}

//----------------------------------------------------------------------
inline void FDialog::activateZoomButton (const MouseStates& ms)
{
  if ( ! isMouseOverZoomButton(ms) )
    return;

  minimize_button_pressed = false;
  minimize_button_active = false;
  zoom_button_pressed = true;
  zoom_button_active = true;
  drawTitleBar();
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
inline void FDialog::leaveZoomButton (const MouseStates& ms)
{
  bool zoom_button_pressed_before = zoom_button_pressed;

  if ( isMouseOverZoomButton(ms) && zoom_button_active )
    zoom_button_pressed = true;
  else if ( zoom_button_pressed )
    zoom_button_pressed = false;

  if ( zoom_button_pressed_before != zoom_button_pressed )
    drawTitleBar();
}

//----------------------------------------------------------------------
void FDialog::pressZoomButton (const MouseStates& ms)
{
  if ( ! isMouseOverZoomButton(ms) || ! zoom_button_pressed )
    return;

  // Zoom to maximum or restore the window size
  zoomWindow();
  setZoomItem();
  zoom_button_active = false;
}

//----------------------------------------------------------------------
inline auto FDialog::isMouseOverMenu (const FPoint& termpos) const -> bool
{
  auto menu_geometry = dialog_menu->getTermGeometry();

  return ( dialog_menu->getCount() > 0
        && menu_geometry.contains(termpos) );
}

//----------------------------------------------------------------------
inline auto FDialog::isMouseOverMenuButton (const MouseStates& ms) const -> bool
{
  return ( ms.mouse_x <= int(ms.menu_btn) && ms.mouse_y == 1 );
}

//----------------------------------------------------------------------
inline auto FDialog::isMouseOverZoomButton (const MouseStates& ms) const -> bool
{
  return ( isResizeable()
        && ms.mouse_x > int(getWidth() - ms.zoom_btn)
        && ms.mouse_x <= int(getWidth())
        && ms.mouse_y == 1 );
}

//----------------------------------------------------------------------
inline auto FDialog::isMouseOverMinimizeButton (const MouseStates& ms)  const -> bool
{
  return ( isMinimizable()
        && ms.mouse_x > int(getWidth() - ms.minimize_btn - ms.zoom_btn)
        && ms.mouse_x <= int(getWidth() - ms.zoom_btn)
        && ms.mouse_y == 1 );
}

//----------------------------------------------------------------------
inline auto FDialog::isMouseOverTitlebar (const MouseStates& ms) const -> bool
{
  return ( ms.mouse_x > int(ms.menu_btn)
        && ms.mouse_x <= int(getWidth() - ms.minimize_btn - ms.zoom_btn)
        && ms.mouse_y == 1 );
}

//----------------------------------------------------------------------
inline void FDialog::passEventToSubMenu ( const MouseStates& ms
                                        , const FMouseEvent& ev )
{
  // Mouse event handover to the dialog menu
  if ( ! ms.mouse_over_menu || ! dialog_menu->isShown() )
    return;

  const auto& g = ms.termPos;
  const auto& p = dialog_menu->termToWidgetPos(g);
  const auto b = ev.getButton();
  const auto& new_ev = \
      std::make_shared<FMouseEvent>(Event::MouseMove, p, g, b);
  dialog_menu->mouse_down = true;
  setClickedWidget(dialog_menu);
  dialog_menu->onMouseMove(new_ev.get());
}

//----------------------------------------------------------------------
inline void FDialog::handleLeftMouseDown (const MouseStates& ms)
{
  // Click on titlebar or window: raise + activate
  raiseActivateDialog();

  if ( isMouseOverTitlebar(ms) )
    titlebar_click_pos.setPoint (ms.termPos);
  else
    titlebar_click_pos.setPoint (0, 0);

  // Click on titlebar menu button
  if ( isMouseOverMenuButton(ms) )
    openMenu();
  else
  {
    activateZoomButton(ms);
    activateMinimizeButton(ms);
  }

  // Click on the lower right resize corner
  resizeMouseDown(ms);
}

//----------------------------------------------------------------------
inline void FDialog::handleRightAndMiddleMouseDown ( const MouseButton& button
                                                   , const MouseStates& ms )
{
  // Click on titlebar menu button
  if ( isMouseOverMenuButton(ms) && dialog_menu->isShown() )
    leaveMenu();  // close menu

  cancelMouseResize();  // Cancel resize
  const auto width = int(getWidth());
  const auto first = int(getMenuButtonWidth() + 1);

  // Click on titlebar: just activate
  if ( button == MouseButton::Right
    && ms.mouse_x >= first && ms.mouse_x <= width && ms.mouse_y == 1 )
      activateDialog();

  // Click on titlebar: lower + activate
  if ( button == MouseButton::Middle
    && ms.mouse_x >= first && ms.mouse_x <= width && ms.mouse_y == 1 )
      lowerActivateDialog();
}

//----------------------------------------------------------------------
inline void FDialog::moveSizeKey (FKeyEvent* ev)
{
  const auto& entry = key_map[ev->key()];

  if ( entry )
    entry();

  // Accept for all, so that parent widgets will not receive keystrokes
  ev->accept();
}

//----------------------------------------------------------------------
inline void FDialog::raiseActivateDialog()
{
  const bool has_raised = raiseWindow();
  activateDialog();

  if ( has_raised )
    redraw();
}

//----------------------------------------------------------------------
inline void FDialog::lowerActivateDialog()
{
  lowerWindow();

  if ( ! isWindowActive() )
    activateDialog();
}

//----------------------------------------------------------------------
auto FDialog::isOutsideTerminal (const FPoint& pos) const -> bool
{
  return ( pos.getX() + int(getWidth()) <= 1
        || pos.getX() > int(getMaxWidth())
        || pos.getY() < 1
        || pos.getY() > int(getMaxHeight()) );
}

//----------------------------------------------------------------------
auto FDialog::isLeftOutside() const -> bool
{
  return getX() > int(getMaxWidth());
}

//----------------------------------------------------------------------
auto FDialog::isBottomOutside() const -> bool
{
  return getY() > int(getMaxHeight());
}

//----------------------------------------------------------------------
auto FDialog::isLowerRightResizeCorner (const MouseStates& ms) const -> bool
{
  // 3 characters in the lower right corner  |
  //                                         x
  //                                   -----xx

  return ( (ms.mouse_x == int(getWidth()) && ms.mouse_y == int(getHeight()) - 1)
        || ( ( ms.mouse_x == int(getWidth()) - 1
            || ms.mouse_x == int(getWidth()) ) && ms.mouse_y == int(getHeight()) ) );
}

//----------------------------------------------------------------------
auto FDialog::noVisibleDialog() const -> bool
{
  // Is true when there is no visible dialog

  if ( ! getDialogList() || getDialogList()->empty() )
    return true;

  // The number of iterations should be very low with only
  // one visible element
  for (const auto& dgl : *getDialogList())
  {
    const auto& win = static_cast<FDialog*>(dgl);

    if ( ! win->isWindowHidden() )
      return false;
  }

  return true;
}

//----------------------------------------------------------------------
void FDialog::resizeMouseDown (const MouseStates& ms)
{
  // Click on the lower right resize corner (mouse button down)

  if ( isResizeable() && isLowerRightResizeCorner(ms) )
  {
    resize_click_pos = ms.termPos;
    const FPoint lower_right_pos{getTermGeometry().getLowerRightPos()};

    if ( ms.termPos != lower_right_pos )
    {
      const FPoint deltaPos{ms.termPos - lower_right_pos};
      const int w = lower_right_pos.getX() + deltaPos.getX() - getTermX() + 1;
      const int h = lower_right_pos.getY() + deltaPos.getY() - getTermY() + 1;
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
void FDialog::resizeMouseUpMove (const MouseStates& ms, bool mouse_up)
{
  // Resize dialog on mouse button up or on mouse movements

  if ( isResizeable() && ! resize_click_pos.isOrigin() )
  {
    const auto& r = getRootWidget();
    resize_click_pos = ms.termPos;
    const int x2 = resize_click_pos.getX();
    const int y2 = resize_click_pos.getY();
    int x2_offset{0};
    int y2_offset{0};

    if ( r )
    {
      x2_offset = r->getLeftPadding();
      y2_offset = r->getTopPadding();
    }

    if ( ms.termPos != getTermGeometry().getLowerRightPos() )
    {
      int w{};
      int h{};
      const FPoint deltaPos{ms.termPos - resize_click_pos};

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
}

//----------------------------------------------------------------------
inline void FDialog::acceptMoveSize()
{
  setMoveSizeWidget(nullptr);
  delete tooltip;
  tooltip = nullptr;
  redraw();
}

//----------------------------------------------------------------------
inline void FDialog::cancelMoveSize()
{
  setMoveSizeWidget(nullptr);
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

  if ( ! getDialogList() )
    return;

  if ( getDialogList()->empty() )
  {
    auto fapp = FApplication::getApplicationObject();

    if ( fapp )
      fapp->emitCallback("first-dialog-opened");
  }

  getDialogList()->push_back(obj);
}

//----------------------------------------------------------------------
void FDialog::delDialog (const FWidget* obj)
{
  // Delete the dialog object obj from the dialog list

  if ( ! getDialogList() || getDialogList()->empty() )
    return;

  auto iter = getDialogList()->cbegin();

  while ( iter != getDialogList()->cend() )
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
void FDialog::cb_move()
{
  if ( isZoomed() )
    return;

  setMoveSizeWidget(this);

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(true);

  drawBorder();

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);

  save_geometry = getGeometry();

  try
  {
    tooltip = new FToolTip(this);
  }
  catch (const std::bad_alloc&)
  {
    badAllocOutput ("FToolTip");
    return;
  }

  if ( isResizeable() )
  {
    if ( FVTerm::getFOutput()->areMetaAndArrowKeysSupported() )
      tooltip->setText ( "       Arrow keys: Move\n"
                         "Meta + Arrow keys: Resize\n"
                         "            Enter: Done\n"
                         "              Esc: Cancel" );
    else
      tooltip->setText ( "        Arrow keys: Move\n"
                         "Shift + Arrow keys: Resize\n"
                         "             Enter: Done\n"
                         "               Esc: Cancel" );
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
void FDialog::cb_minimize()
{
  dialog_menu->unselectItem();
  dialog_menu->hide();
  setClickedWidget(nullptr);
  drawTitleBar();
  minimizeWindow();
  setZoomItem();
}

//----------------------------------------------------------------------
void FDialog::cb_zoom()
{
  dialog_menu->unselectItem();
  dialog_menu->hide();
  setClickedWidget(nullptr);
  drawTitleBar();
  zoomWindow();
  setMinimizeItem();
}

//----------------------------------------------------------------------
void FDialog::cb_close()
{
  dialog_menu->unselectItem();
  dialog_menu->hide();
  setClickedWidget(nullptr);
  drawTitleBar();
  clearStatusBar();
  close();
}

}  // namespace finalcut
