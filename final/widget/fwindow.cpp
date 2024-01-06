/***********************************************************************
* fwindow.cpp - Intermediate base class for all window objects         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2024 Markus Gans                                      *
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

#include <utility>

#include "final/fapplication.h"
#include "final/fevent.h"
#include "final/input/fmouse.h"
#include "final/menu/fmenubar.h"
#include "final/menu/fmenu.h"
#include "final/widget/fcombobox.h"
#include "final/widget/fstatusbar.h"
#include "final/widget/fwindow.h"

namespace finalcut
{

namespace internal
{

struct var
{
  static bool fwindow_init_flag;  // FWindow init state
};

bool var::fwindow_init_flag{false};

}  // namespace internal

// static attributes
FWindow* FWindow::previous_window{nullptr};

//----------------------------------------------------------------------
// class FWindow
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FWindow::FWindow(FWidget* parent)
  : FWidget{parent}
{
  if ( ! internal::var::fwindow_init_flag )
  {
    auto app_object = FApplication::getApplicationObject();

    if ( ! app_object )
      return;

    app_object->registerMouseHandler (closeDropDownMouseHandler);
    app_object->registerMouseHandler (unselectMenubarItemsMouseHandler);
    internal::var::fwindow_init_flag = true;
  }

  setWindowWidget();
  createVWin();
  addWindow (this);
}

//----------------------------------------------------------------------
FWindow::~FWindow()  // destructor
{
  if ( previous_window == this )
    previous_window = nullptr;

  if ( isAlwaysOnTop() )
    deleteFromAlwaysOnTopList (this);

  // unset the global active window
  if ( this == FWindow::getActiveWindow() )
    unsetActiveWindow();

  delWindow (this);

  if ( ! FApplication::isQuit() )
  {
    const auto& t_geometry = getTermGeometryWithShadow();
    restoreVTerm (t_geometry);
  }
}


// public methods of FWindow
//----------------------------------------------------------------------
auto FWindow::getWindowFocusWidget() const -> FWidget*
{
  // returns the focused widget of this window
  return win_focus_widget;
}

//----------------------------------------------------------------------
void FWindow::setWindowWidget (bool enable)
{
  if ( isWindowWidget() == enable )
    return;

  setFlags().type.window_widget = enable;

  if ( enable )
    setTermOffset();
  else
    setParentOffset();
}

//----------------------------------------------------------------------
void FWindow::setActiveWindow (FWindow* window)
{
  // activate FWindow object window

  if ( ! getWindowList() || getWindowList()->empty() )
    return;

  for (auto&& win : *getWindowList())
  {
    if ( win == window )
    {
      if ( ! window->isWindowActive() )
      {
        window->activateWindow();
        FEvent ev(Event::WindowActive);
        FApplication::sendEvent(window, &ev);
      }
    }
    else
    {
      auto w = static_cast<FWindow*>(win);

      if ( w->isWindowActive() )
      {
        w->deactivateWindow();
        FEvent ev(Event::WindowInactive);
        FApplication::sendEvent(static_cast<FWidget*>(win), &ev);
      }
    }
  }
}

//----------------------------------------------------------------------
void FWindow::setWindowFocusWidget (FWidget* obj)
{
  // set focus widget of this window
  win_focus_widget = obj;
}

//----------------------------------------------------------------------
auto FWindow::activateWindow (bool enable) -> bool
{
  // activate/deactivate this window
  if ( enable )
  {
    FWidget::setActiveWindow (this);
    setActiveArea (getVWin());
  }

  return (window_active = enable);
}

//----------------------------------------------------------------------
void FWindow::unsetActiveWindow() const
{
  // unset the active FWindow object
  FWidget::setActiveWindow (nullptr);
}

//----------------------------------------------------------------------
void FWindow::setResizeable (bool enable)
{
  setFlags().feature.resizeable = enable;
}

//----------------------------------------------------------------------
void FWindow::setMinimizable (bool enable)
{
  setFlags().feature.minimizable = enable;
}

//----------------------------------------------------------------------
void FWindow::setTransparentShadow (bool enable)
{
  setFlags().shadow.shadow = setFlags().shadow.trans_shadow = enable;

  if ( enable )
    setShadowSize (FSize{2, 1});
  else
    setShadowSize (FSize{0, 0});
}

//----------------------------------------------------------------------
void FWindow::setShadow (bool enable)
{
  if ( FVTerm::getFOutput()->isMonochron() )
    return;

  if ( enable )
  {
    setFlags().shadow.shadow = true;
    setFlags().shadow.trans_shadow = false;
    setShadowSize (FSize{1, 1});
  }
  else
  {
    setFlags().shadow.shadow = false;
    setFlags().shadow.trans_shadow = false;
    setShadowSize (FSize{0, 0});
  }
}

//----------------------------------------------------------------------
void FWindow::setAlwaysOnTop (bool enable)
{
  if ( isAlwaysOnTop() == enable )
    return;

  setFlags().visibility.always_on_top = enable;

  if ( enable )
  {
    if ( getAlwaysOnTopList() )
    {
      deleteFromAlwaysOnTopList (this);
      getAlwaysOnTopList()->push_back (this);
    }

    unsetMinimizable();
  }
  else
    deleteFromAlwaysOnTopList (this);
}

//----------------------------------------------------------------------
auto FWindow::isMinimized() const -> bool
{
  // returns the window minimized state

  if ( isVirtualWindow() )
    return getVWin()->minimized;

  return false;
}

//----------------------------------------------------------------------
auto FWindow::isWindowHidden() const -> bool
{
  // returns the window hidden state

  if ( isVirtualWindow() )
    return ! getVWin()->visible;

  return false;
}

//----------------------------------------------------------------------
void FWindow::drawBorder()
{
  if ( FVTerm::getFOutput()->isNewFont() )  // Draw a newfont outer frame
  {
    const FRect r{FPoint{1, 1}, getSize()};
    print() << r.getUpperLeftPos()
            << UniChar::NF_border_corner_upper_left                      // ⎡
            << FString{r.getWidth() - 2, UniChar::NF_border_line_upper}  // ¯
            << UniChar::NF_rev_border_corner_upper_right;                // ⎤

    for (auto y = r.getY1() + 1; y < r.getY2(); y++)
    {
      print() << FPoint{r.getX1(), y}
              << UniChar::NF_border_line_left        // border left ⎸
              << FPoint{r.getX2(), y}
              << UniChar::NF_rev_border_line_right;  // border right⎹
    }

    print() << r.getLowerLeftPos()
            << UniChar::NF_border_corner_lower_left                       // ⎣
            << FString{r.getWidth() - 2, UniChar::NF_border_line_bottom}  // _
            << UniChar::NF_rev_border_corner_lower_right;                 // ⎦
  }
  else
  {
    FWidget::drawBorder();
  }
}

//----------------------------------------------------------------------
void FWindow::show()
{
  if ( isVirtualWindow() )
    getVWin()->visible = true;

  FWidget::show();
}

//----------------------------------------------------------------------
void FWindow::hide()
{
  const auto& virtual_win = getVWin();

  if ( isActive(virtual_win)
    && virtual_win->visible
    && virtual_win->input_cursor_visible )
  {
    hideVTermCursor();
  }

  if ( isVirtualWindow() )
    virtual_win->visible = false;

  FWidget::hide();
  const auto& t_geometry = getTermGeometryWithShadow();
  restoreVTerm (t_geometry);
}

//----------------------------------------------------------------------
void FWindow::setX (int x, bool adjust)
{
  FWidget::setX (x, adjust);

  if ( isVirtualWindow() )
    getVWin()->position.x = getTermX() - 1;
}

//----------------------------------------------------------------------
void FWindow::setY (int y, bool adjust)
{
  y = std::max(y, 1);
  FWidget::setY (y, adjust);

  if ( isVirtualWindow() )
    getVWin()->position.y = getTermY() - 1;
}

//----------------------------------------------------------------------
void FWindow::setPos (const FPoint& p, bool adjust)
{
  FPoint pos{p};
  pos.y_ref() = std::max(pos.y_ref(), 1);
  FWidget::setPos (pos, adjust);

  if ( isVirtualWindow() )
  {
    auto virtual_win = getVWin();
    virtual_win->position.x = getTermX() - 1;
    virtual_win->position.y = getTermY() - 1;
  }
}

//----------------------------------------------------------------------
void FWindow::setWidth (std::size_t w, bool adjust)
{
  const std::size_t old_width = getWidth();
  FWidget::setWidth (w, adjust);

  if ( isVirtualWindow() && getWidth() != old_width )
  {
    FRect geometry {getTermGeometry()};
    geometry.move(-1, -1);
    resizeArea ({geometry, getShadow()}, getVWin());
  }
}

//----------------------------------------------------------------------
void FWindow::setHeight (std::size_t h, bool adjust)
{
  const std::size_t old_height = getHeight();
  FWidget::setHeight (h, adjust);

  if ( isVirtualWindow() && getHeight() != old_height )
  {
    FRect geometry {getTermGeometry()};
    geometry.move(-1, -1);
    resizeArea ({geometry, getShadow()}, getVWin());
  }
}

//----------------------------------------------------------------------
void FWindow::setSize (const FSize& size, bool adjust)
{
  const FSize old_size{getSize()};
  FWidget::setSize (size, adjust);

  if ( isVirtualWindow() && getSize() != old_size )
  {
    FRect geometry {getTermGeometry()};
    geometry.move(-1, -1);
    resizeArea ({geometry, getShadow()}, getVWin());
  }
}

//----------------------------------------------------------------------
void FWindow::setGeometry ( const FPoint& p, const FSize& size, bool adjust)
{
  // Sets the geometry of the widget

  const int old_x = getX();
  const int old_y = getY();
  FPoint pos{p};
  const FSize old_size{getSize()};
  pos.y_ref() = std::max(pos.y_ref(), 1);
  FWidget::setGeometry (pos, size, adjust);

  if ( ! isVirtualWindow() )
    return;

  if ( getSize() != old_size )
  {
    FRect geometry {getTermGeometry()};
    geometry.move(-1, -1);
    resizeArea ({geometry, getShadow()}, getVWin());
  }
  else
  {
    if ( getX() != old_x )
      getVWin()->position.x = getTermX() - 1;

    if ( getY() != old_y )
      getVWin()->position.y = getTermY() - 1;
  }
}

//----------------------------------------------------------------------
void FWindow::move (const FPoint& pos)
{
  FWidget::move(pos);

  if ( isVirtualWindow() )
  {
    auto virtual_win = getVWin();
    virtual_win->position.x = getTermX() - 1;
    virtual_win->position.y = getTermY() - 1;
  }
}

//----------------------------------------------------------------------
auto FWindow::getWindowWidgetAt (int x, int y) -> FWindow*
{
  // returns the window object to the corresponding coordinates

  if ( ! getWindowList() || getWindowList()->empty() )
    return nullptr;

  auto iter = getWindowList()->cend();
  const auto begin = getWindowList()->cbegin();

  do
  {
    --iter;
    auto w = static_cast<FWindow*>(*iter);

    if ( *iter && ! w->isWindowHidden()
      && getVisibleTermGeometry(w).contains(x, y) )
    {
      return w;
    }
  }
  while ( iter != begin );

  return nullptr;
}

//----------------------------------------------------------------------
void FWindow::addWindow (FWidget* obj)
{
  // add the window object obj to the window list
  if ( getWindowList() )
    getWindowList()->push_back(obj);

  processAlwaysOnTop();
}

//----------------------------------------------------------------------
void FWindow::delWindow (const FWidget* obj)
{
  // delete the window object obj from the window list
  if ( ! getWindowList() || getWindowList()->empty() )
    return;

  auto iter = getWindowList()->cbegin();

  while ( iter != getWindowList()->cend() )
  {
    if ( (*iter) == obj )
    {
      getWindowList()->erase(iter);
      determineWindowLayers();
      return;
    }

    ++iter;
  }
}

//----------------------------------------------------------------------
void FWindow::swapWindow (const FWidget* obj1, const FWidget* obj2)
{
  // swaps the window layer between obj1 and obj2

  if ( ! getWindowList()
    || getWindowList()->empty()
    || obj1->getFlags().visibility.modal
    || obj2->getFlags().visibility.modal )
    return;

  auto iter = getWindowList()->cbegin();
  const auto end = getWindowList()->cend();
  auto iter1 = end;
  auto iter2 = end;

  while ( iter != end )
  {
    if ( (*iter) == obj1 )
      iter1 = iter;
    else if ( (*iter) == obj2 )
      iter2 = iter;

    ++iter;
  }

  if ( iter1 != end && iter2 != end )
  {
    std::swap (iter1, iter2);
    determineWindowLayers();
  }
}

//----------------------------------------------------------------------
auto FWindow::raiseWindow (FWidget* obj) -> bool
{
  // raises the window widget obj to the top

  if ( ! getWindowList()
    || getWindowList()->empty()
    || ! obj->isWindowWidget() )
    return false;

  const auto last = static_cast<FWidget*>(getWindowList()->back());

  if ( last == obj || (last->getFlags().visibility.modal && ! obj->isMenuWidget()) )
    return false;

  auto iter = getWindowList()->cbegin();

  while ( iter != getWindowList()->cend() )
  {
    if ( static_cast<FWidget*>(*iter) == obj )
    {
      getWindowList()->erase (iter);
      getWindowList()->push_back (obj);
      processAlwaysOnTop();
      FEvent ev(Event::WindowRaised);
      FApplication::sendEvent(obj, &ev);
      return true;
    }

    ++iter;
  }

  return false;
}

//----------------------------------------------------------------------
auto FWindow::lowerWindow (FWidget* obj) -> bool
{
  // lowers the window widget obj to the bottom

  if ( ! getWindowList()
    || getWindowList()->empty()
    || ! obj->isWindowWidget()
    || getWindowList()->front() == obj
    || obj->getFlags().visibility.modal )
    return false;

  auto iter = getWindowList()->cbegin();

  while ( iter != getWindowList()->cend() )
  {
    if ( *iter == obj )
    {
      getWindowList()->erase (iter);
      getWindowList()->insert (getWindowList()->cbegin(), obj);
      determineWindowLayers();
      FEvent ev(Event::WindowLowered);
      FApplication::sendEvent(obj, &ev);
      return true;
    }

    ++iter;
  }

  return false;
}

//----------------------------------------------------------------------
auto FWindow::zoomWindow() -> bool
{
  if ( zoomed )
  {
    zoomed = false;
    const FRect oldGeometry (getTermGeometryWithShadow());
    setGeometry (normalGeometry);
    redraw();
    restoreVTerm (oldGeometry);
  }
  else
  {
    if ( isMinimized() )
      minimizeWindow();  // unminimize window

    zoomed = true;
    // save the current geometry
    normalGeometry = getGeometry();
    const FRect oldGeometry (getTermGeometryWithShadow());
    setGeometry (FPoint{1, 1}, FSize{getMaxWidth(), getMaxHeight()});
    redraw();
    restoreVTerm (oldGeometry);
  }

  return zoomed;
}

//----------------------------------------------------------------------
auto FWindow::minimizeWindow() -> bool
{
  if ( ! isVirtualWindow() )
    return false;

  if ( zoomed )
    zoomWindow();  // unzoom window

  const auto& virtual_win = getVWin();
  virtual_win->minimized = bool( ! isMinimized() );
  const auto& t_geometry = getTermGeometryWithShadow();
  restoreVTerm (t_geometry);

  return virtual_win->minimized;
}

//----------------------------------------------------------------------
void FWindow::switchToPrevWindow (const FWidget* widget)
{
  // switch to previous window

  // Disable terminal updates to avoid flickering
  // when redrawing the focused widget
  if ( widget )
    widget->setTerminalUpdates (FVTerm::TerminalUpdate::Stop);

  const bool is_activated = activatePrevWindow();
  auto active_win = static_cast<FWindow*>(getActiveWindow());

  if ( ! is_activated && getWindowList() && getWindowList()->size() > 1 )
  {
    // no previous window -> looking for another window
    auto iter = getWindowList()->cend();
    const auto begin = getWindowList()->cbegin();

    do
    {
      --iter;
      auto w = static_cast<FWindow*>(*iter);

      if ( w
        && w != active_win
        && ! (w->isWindowHidden() || w->isWindowActive())
        && w != static_cast<FWindow*>(getStatusBar())
        && w != static_cast<FWindow*>(getMenuBar()) )
      {
        FWindow::setActiveWindow(w);
        break;
      }
    }
    while ( iter != begin );
  }

  reactivateWindow (active_win);

  // Enable terminal updates again
  if ( widget )
    widget->setTerminalUpdates (FVTerm::TerminalUpdate::Continue);
}

//----------------------------------------------------------------------
auto FWindow::activatePrevWindow() -> bool
{
  // activate the previous window
  const auto& w = previous_window;

  if ( w )
  {
    if ( w->isWindowActive() )
      return true;

    if ( ! w->isWindowHidden() )
    {
      FWindow::setActiveWindow(w);
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------
void FWindow::setShadowSize (const FSize& size)
{
  const FSize& old_shadow = getShadow();
  FWidget::setShadowSize (size);
  const FSize& new_shadow = getShadow();

  if ( isVirtualWindow() && (new_shadow != old_shadow) )
  {
    auto geometry = getTermGeometry();
    geometry.move(-1, -1);
    resizeArea ({geometry, getShadow()}, getVWin());
  }
}


// protected methods of FWindow
//----------------------------------------------------------------------
void FWindow::adjustSize()
{
  const int old_x = getTermX();
  const int old_y = getTermY();
  FWidget::adjustSize();

  if ( zoomed )
    setGeometry (FPoint{1, 1}, FSize{getMaxWidth(), getMaxHeight()}, false);
  else if ( isVirtualWindow() )
  {
    if ( getTermX() != old_x )
      getVWin()->position.x = getTermX() - 1;

    if ( getTermY() != old_y )
      getVWin()->position.y = getTermY() - 1;
  }
}

//----------------------------------------------------------------------
auto FWindow::event (FEvent* ev) -> bool
{
  auto event_type = ev->getType();

  if ( event_type == Event::WindowActive )
  {
    onWindowActive (ev);
  }
  else if ( event_type == Event::WindowInactive )
  {
    onWindowInactive (ev);
  }
  else if ( event_type == Event::WindowRaised )
  {
    onWindowRaised (ev);
  }
  else if ( event_type == Event::WindowLowered )
  {
    onWindowLowered (ev);
  }
  else
    return FWidget::event(ev);

  return true;
}

//----------------------------------------------------------------------
void FWindow::onWindowActive (FEvent*)
{
  // This event handler can be reimplemented in a subclass
  // to receive activation events for this window
}

//----------------------------------------------------------------------
void FWindow::onWindowInactive (FEvent*)
{
  // This event handler can be reimplemented in a subclass
  // to receive deactivation events for this window
}

//----------------------------------------------------------------------
void FWindow::onWindowRaised (FEvent*)
{
  // This event handler can be reimplemented in a subclass
  // to receive window raising events for this window
}

//----------------------------------------------------------------------
void FWindow::onWindowLowered (FEvent*)
{
  // This event handler can be reimplemented in a subclass
  // to receive window lowering events for this window
}


// private methods of FWindow
//----------------------------------------------------------------------
inline void FWindow::createVWin() noexcept
{
  // Initialize virtual window

  FRect geometry {getTermGeometry()};
  geometry.move(-1, -1);
  setVWin(createArea({geometry, getShadow()}));
}

//----------------------------------------------------------------------
inline auto FWindow::getVisibleTermGeometry (FWindow* win) -> FRect
{
  auto& term_geometry = win->getTermGeometry();

  if ( win->isMinimized() )
  {
    FRect minimized_term_geometry(term_geometry);
    auto min_height = std::size_t(win->getVWin()->min_size.height);
    minimized_term_geometry.setHeight(min_height);
    return minimized_term_geometry;
  }

  return term_geometry;
}

//----------------------------------------------------------------------
void FWindow::deleteFromAlwaysOnTopList (const FWidget* obj)
{
  // delete the window object obj from the always-on-top list
  if ( ! getAlwaysOnTopList() || getAlwaysOnTopList()->empty() )
    return;

  auto iter = getAlwaysOnTopList()->cbegin();

  while ( iter != getAlwaysOnTopList()->cend() )
  {
    if ( *iter == obj )
    {
      getAlwaysOnTopList()->erase (iter);
      return;
    }

    ++iter;
  }
}

//----------------------------------------------------------------------
void FWindow::processAlwaysOnTop()
{
  // Raise all always-on-top windows
  if ( ! getAlwaysOnTopList() || getAlwaysOnTopList()->empty() )
  {
    determineWindowLayers();
    return;
  }

  auto iter = getAlwaysOnTopList()->cbegin();

  while ( iter != getAlwaysOnTopList()->cend() )
  {
    delWindow (*iter);

    if ( getWindowList() )
      getWindowList()->push_back(*iter);

    ++iter;
  }

  determineWindowLayers();
}

//----------------------------------------------------------------------
auto FWindow::getWindowWidgetImpl (FWidget* obj) -> FWindow*
{
  // returns the window object to the given widget obj
  auto p_obj = obj->getParentWidget();

  while ( ! obj->isWindowWidget() && p_obj )
  {
    obj = p_obj;
    p_obj = p_obj->getParentWidget();
  }

  if ( obj->isWindowWidget() )
    return static_cast<FWindow*>(obj);

  return nullptr;
}

//----------------------------------------------------------------------
auto FWindow::getWindowLayerImpl (FWidget* obj) -> int
{
  // returns the window layer from the widget obj

  FWidget* window;

  if ( ! obj->isWindowWidget() )
  {
    if ( (window = getWindowWidget(obj)) == nullptr )
      return -1;
  }
  else
    window = obj;

  return FVTerm::getLayer(*window);
}

//----------------------------------------------------------------------
void FWindow::reactivateWindow (FWindow* active_win)
{
  if ( ! active_win )
    return;

  auto focus = active_win->getWindowFocusWidget();

  if ( ! active_win->isWindowActive() )
    FWindow::setActiveWindow(active_win);

  if ( focus && ! focus->isInstanceOf("FMenuItem") )
  {
    // Renew the focus of the focused widget in the current window
    auto last_focus = FWidget::getFocusWidget();

    if ( last_focus )
      last_focus->setFlags().focus.focus = false;

    FWidget::setFocusWidget(focus);
    focus->setFlags().focus.focus = true;
    active_win->setWindowFocusWidget (focus);
    FFocusEvent f_in (Event::FocusIn);
    FApplication::sendEvent (focus, &f_in);
  }
}


// non-member functions
//----------------------------------------------------------------------
void closeDropDownMouseHandler (const FMouseData& md)
{
  if ( md.isMoved() )
    return;

  const auto& mouse_position = md.getPos();
  auto app_object = FApplication::getApplicationObject();
  finalcut::closeDropDown (app_object, mouse_position);
}

//----------------------------------------------------------------------
void closeDropDown (const FWidget* widget, const FPoint& mouse_position)
{
  // Close the pop down windows

  bool is_dialog_menu{false};
  auto openmenu = FWidget::getOpenMenu();

  if ( ! openmenu )
    return;

  if ( openmenu->isInstanceOf("FMenu")
    || openmenu->isInstanceOf("FDialogListMenu") )
  {
    bool contains_menu_structure;
    auto menu = static_cast<FMenu*>(openmenu);
    std::tie(contains_menu_structure, is_dialog_menu) = \
        closeOpenMenus (menu, mouse_position);

    if ( contains_menu_structure )
      return;
  }

  if ( openmenu->isInstanceOf("FDropDownListBox") )
  {
    auto drop_down = static_cast<FDropDownListBox*>(openmenu);

    if ( ! closeComboBox(drop_down, mouse_position) )
      return;
  }

  // No widget was been clicked and the menu is no dialog menu
  if ( ! (FWidget::getClickedWidget() || is_dialog_menu) )
    FWindow::switchToPrevWindow(widget);

  drawStatusBarMessage();
}

//----------------------------------------------------------------------
void unselectMenubarItemsMouseHandler (const FMouseData& md)
{
  if ( md.isMoved() )
    return;

  const auto& mouse_position = md.getPos();
  auto app_object = FApplication::getApplicationObject();
  finalcut::unselectMenubarItems (app_object, mouse_position);
}

//----------------------------------------------------------------------
void unselectMenubarItems (const FWidget* widget, const FPoint& mouse_position)
{
  // Unselect the menu bar items

  auto menu_bar = FWidget::getMenuBar();

  if ( FWidget::getOpenMenu()
    || ! (menu_bar && menu_bar->hasSelectedItem())
    || menu_bar->getTermGeometry().contains(mouse_position) )
    return;

  if ( FWidget::getStatusBar() )
    FWidget::getStatusBar()->clearMessage();

  menu_bar->resetMenu();
  menu_bar->redraw();

  // No widget was been clicked
  if ( ! FWidget::getClickedWidget() )
    FWindow::switchToPrevWindow(widget);

  drawStatusBarMessage();
}

}  // namespace finalcut
