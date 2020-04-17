/***********************************************************************
* fwidget.cpp - Intermediate base class for all widget objects         *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2015-2020 Markus Gans                                      *
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

#include <vector>

#include "final/fapplication.h"
#include "final/fevent.h"
#include "final/fmenubar.h"
#include "final/fstatusbar.h"
#include "final/fstring.h"
#include "final/ftermdata.h"
#include "final/fwidget.h"
#include "final/fwidgetcolors.h"
#include "final/fwindow.h"

namespace finalcut
{

// global FWidget object
static FWidget* root_widget{nullptr};

// static class attributes
FStatusBar*           FWidget::statusbar{nullptr};
FMenuBar*             FWidget::menubar{nullptr};
FWidget*              FWidget::show_root_widget{nullptr};
FWidget*              FWidget::redraw_root_widget{nullptr};
FWidget::FWidgetList* FWidget::window_list{nullptr};
FWidget::FWidgetList* FWidget::dialog_list{nullptr};
FWidget::FWidgetList* FWidget::always_on_top_list{nullptr};
FWidget::FWidgetList* FWidget::close_widget{nullptr};
FWidgetColors         FWidget::wcolors{};
bool                  FWidget::init_desktop{false};
bool                  FWidget::hideable{false};
uInt                  FWidget::modal_dialog_counter{};

//----------------------------------------------------------------------
// class FWidget
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FWidget::FWidget (FWidget* parent, bool disable_alt_screen)
  : FVTerm( ! (bool(parent) || root_widget), disable_alt_screen)
  , FObject(parent)
{
  // init bit field with 0
  memset (&flags, 0, sizeof(flags));

  flags.active = true;          // Enable widget by default
  flags.visible = true;         // A widget is visible by default
  flags.focusable = true;       // A widget is focusable by default
  flags.visible_cursor = true;  // A widget has a visible cursor by default
  setWidgetProperty (true);     // This FObject is a widget

  if ( ! parent )
  {
    if ( root_widget )
    {
      auto ftermdata = FTerm::getFTermData();
      ftermdata->setExitMessage("FWidget: No parent defined! "
                                "There should be only one root object");
      FApplication::exit(EXIT_FAILURE);
      return;
    }

    root_widget = this;
    show_root_widget = nullptr;
    redraw_root_widget = nullptr;
    modal_dialog_counter = 0;
    statusbar = nullptr;
    initRootWidget();
  }
  else
  {
    flags.visible_cursor = ! hideable;
    woffset = parent->wclient_offset;
    double_flatline_mask.top.resize (getWidth(), false);
    double_flatline_mask.right.resize (getHeight(), false);
    double_flatline_mask.bottom.resize (getWidth(), false);
    double_flatline_mask.left.resize (getHeight(), false);
  }
}

//----------------------------------------------------------------------
FWidget::~FWidget()  // destructor
{
  processDestroy();
  delCallbacks();
  auto app_object = FApplication::getApplicationObject();
  app_object->removeQueuedEvent(this);

  // unset clicked widget
  if ( this == getClickedWidget() )
    setClickedWidget(nullptr);

  // unset the local window widget focus
  if ( flags.focus )
  {
    if ( auto window = FWindow::getWindowWidget(this) )
      if ( window != this )
        window->setWindowFocusWidget(nullptr);
  }

  // unset the global widget focus
  if ( this == FWidget::getFocusWidget() )
    FWidget::setFocusWidget(nullptr);

  // unset main widget
  if ( this == getMainWidget() )
  {
    setMainWidget(nullptr);
    quit();
  }

  accelerator_list.clear();

  // finish the program
  if ( root_widget == this )
    finish();
}


// public methods of FWidget
//----------------------------------------------------------------------
FWidget* FWidget::getRootWidget() const
{
  auto obj = const_cast<FWidget*>(this);
  auto p_obj = getParentWidget();

  while ( ! obj->isRootWidget() && p_obj )
  {
    obj = p_obj;
    p_obj = p_obj->getParentWidget();
  }

  return obj;
}

//----------------------------------------------------------------------
FWidget* FWidget::getParentWidget() const
{
  auto p_obj = getParent();

  if ( p_obj && p_obj->isWidget() )
    return static_cast<FWidget*>(p_obj);
  else
    return nullptr;
}

//----------------------------------------------------------------------
FWidget* FWidget::getFirstFocusableWidget (FObjectList list)
{
  if ( list.empty() )
    return nullptr;

  auto iter = list.begin();

  while ( iter != list.end() )
  {
    if ( (*iter)->isWidget() )
    {
      auto child = static_cast<FWidget*>(*iter);

      if ( child->isEnabled() && child->acceptFocus() )
        return child;
    }

    ++iter;
  }

  return nullptr;
}

//----------------------------------------------------------------------
FWidget* FWidget::getLastFocusableWidget (FObjectList list)
{
  if ( list.empty() )
    return nullptr;

  auto iter  = list.end();

  do
  {
    --iter;

    if ( ! (*iter)->isWidget() )
      continue;

    auto child = static_cast<FWidget*>(*iter);

    if ( child->isEnabled() && child->acceptFocus() )
      return child;
  }
  while ( iter != list.begin() );

  return nullptr;
}

//----------------------------------------------------------------------
std::vector<bool>& FWidget::doubleFlatLine_ref (fc::sides side)
{
  assert ( side == fc::top
        || side == fc::right
        || side == fc::bottom
        || side == fc::left );

  switch ( side )
  {
    case fc::top:
      return double_flatline_mask.top;

    case fc::right:
      return double_flatline_mask.right;

    case fc::bottom:
      return double_flatline_mask.bottom;

    case fc::left:
    default:
      return double_flatline_mask.left;
  }

  return double_flatline_mask.left;
}

//----------------------------------------------------------------------
FPoint FWidget::getPrintPos()
{
  const auto& cur = getPrintCursor();
  return FPoint ( cur.getX() - woffset.getX1() - getX() + 1
                , cur.getY() - woffset.getY1() - getY() + 1 );
}

//----------------------------------------------------------------------
void FWidget::setMainWidget (FWidget* obj)
{
  main_widget = obj;
  auto app_object = FApplication::getApplicationObject();

  if ( obj && app_object && ! getFocusWidget() )
    app_object->focusFirstChild();
}

//----------------------------------------------------------------------
bool FWidget::setVisible (bool enable)
{
  return (flags.visible = enable);
}

//----------------------------------------------------------------------
bool FWidget::setEnable (bool enable)
{
  return (flags.active = enable);
}

//----------------------------------------------------------------------
bool FWidget::setFocus (bool enable)
{
  if ( ! isEnabled() )
    return false;

  if ( flags.focus == enable )
    return true;

  auto last_focus = FWidget::getFocusWidget();

  // set widget focus
  if ( enable && ! flags.focus )
  {
    if ( last_focus )
      last_focus->unsetFocus();

    FWidget::setFocusWidget(this);
  }

  // Activates the window with the focused widget
  setWindowFocus (enable);

  // Set status bar text for widget focus
  setStatusbarText (enable);
  return (flags.focus = enable);
}

//----------------------------------------------------------------------
void FWidget::setColor()
{
  // Changes colors to the widget default colors
  setColor (foreground_color, background_color);
}

//----------------------------------------------------------------------
void FWidget::setX (int x, bool adjust)
{
  if ( getX() == x && wsize.getX() == x )
    return;

  if ( ! isWindowWidget() && x < 1 )
    x = 1;

  wsize.setX(x);
  adjust_wsize.setX(x);

  if ( adjust )
    adjustSize();
}

//----------------------------------------------------------------------
void FWidget::setY (int y, bool adjust)
{
  if ( getY() == y && wsize.getY() == y )
    return;

  if ( ! isWindowWidget() && y < 1 )
    y = 1;

  wsize.setY(y);
  adjust_wsize.setY(y);

  if ( adjust )
    adjustSize();
}

//----------------------------------------------------------------------
void FWidget::setPos (const FPoint& p, bool adjust)
{
  FPoint pos(p);

  if ( getX() == pos.getX() && wsize.getX() == pos.getX()
    && getY() == pos.getY() && wsize.getY() == pos.getY() )
  {
    return;
  }

  if ( ! isWindowWidget() )
  {
    if ( pos.getX() < 1 )
      pos.setX(1);

    if ( pos.getY() < 1 )
      pos.setY(1);
  }

  wsize.setPos(pos);
  adjust_wsize.setPos(pos);

  if ( adjust )
    adjustSize();
}

//----------------------------------------------------------------------
void FWidget::setWidth (std::size_t width, bool adjust)
{
  width = std::min (width, size_hints.max_width);
  width = std::max (width, size_hints.min_width);

  if ( getWidth() == width && wsize.getWidth() == width  )
    return;

  if ( width < 1 )
    width = 1;

  wsize.setWidth(width);
  adjust_wsize.setWidth(width);

  if ( adjust )
    adjustSize();

  double_flatline_mask.top.resize (getWidth(), false);
  double_flatline_mask.bottom.resize (getWidth(), false);
}

//----------------------------------------------------------------------
void FWidget::setHeight (std::size_t height, bool adjust)
{
  height = std::min (height, size_hints.max_height);
  height = std::max (height, size_hints.min_height);

  if ( getHeight() == height && wsize.getHeight() == height )
    return;

  if ( height < 1 )
    height = 1;

  wsize.setHeight(height);
  adjust_wsize.setHeight(height);

  if ( adjust )
    adjustSize();

  double_flatline_mask.right.resize (getHeight(), false);
  double_flatline_mask.left.resize (getHeight(), false);
}

//----------------------------------------------------------------------
void FWidget::setSize (const FSize& size, bool adjust)
{
  std::size_t width = size.getWidth();
  std::size_t height = size.getHeight();
  width  = std::min (width,  size_hints.max_width);
  width  = std::max (width,  size_hints.min_width);
  height = std::min (height, size_hints.max_height);
  height = std::max (height, size_hints.min_height);

  if ( getWidth() == width && wsize.getWidth() == width
    && getHeight() == height && wsize.getHeight() == height )
    return;

  if ( width < 1 )
    width = 1;

  if ( height < 1 )
    height = 1;

  wsize.setWidth(width);
  wsize.setHeight(height);
  adjust_wsize.setWidth(width);
  adjust_wsize.setHeight(height);

  if ( adjust )
    adjustSize();

  double_flatline_mask.top.resize (getWidth(), false);
  double_flatline_mask.right.resize (getHeight(), false);
  double_flatline_mask.bottom.resize (getWidth(), false);
  double_flatline_mask.left.resize (getHeight(), false);
}

//----------------------------------------------------------------------
void FWidget::setTopPadding (int top, bool adjust)
{
  if ( padding.top == top )
    return;

  padding.top = top;

  if ( adjust )
  {
    if ( isRootWidget() )
    {
      auto r = root_widget;
      r->wclient_offset.setY1 (r->padding.top);
      adjustSizeGlobal();
    }
    else
      adjustSize();
  }
}

//----------------------------------------------------------------------
void FWidget::setLeftPadding (int left, bool adjust)
{
  if ( padding.left == left )
    return;

  padding.left = left;

  if ( adjust )
  {
    if ( isRootWidget() )
    {
      auto r = root_widget;
      r->wclient_offset.setX1 (r->padding.left);
      adjustSizeGlobal();
    }
    else
      adjustSize();
  }
}

//----------------------------------------------------------------------
void FWidget::setBottomPadding (int bottom, bool adjust)
{
  if ( padding.bottom == bottom )
    return;

  padding.bottom = bottom;

  if ( adjust )
  {
    if ( isRootWidget() )
    {
      auto r = root_widget;
      r->wclient_offset.setY2 (int(r->getHeight()) - 1 - r->padding.bottom);
      adjustSizeGlobal();
    }
    else
      adjustSize();
  }
}

//----------------------------------------------------------------------
void FWidget::setRightPadding (int right, bool adjust)
{
  if ( padding.right == right )
    return;

  padding.right = right;

  if ( adjust )
  {
    if ( isRootWidget() )
    {
      auto r = root_widget;
      r->wclient_offset.setX2  (int(r->getWidth()) - 1 - r->padding.right);
      adjustSizeGlobal();
    }
    else
      adjustSize();
  }
}

//----------------------------------------------------------------------
void FWidget::setTermSize (const FSize& size)
{
  // Set xterm size to width x height

  if ( isXTerminal() )
  {
    root_widget->wsize.setRect(FPoint(1, 1), size);
    root_widget->adjust_wsize = root_widget->wsize;
    FTerm::setTermSize(size);  // width = columns / height = lines
    detectTermSize();
  }
}

//----------------------------------------------------------------------
void FWidget::setGeometry (const FPoint& p, const FSize& s, bool adjust)
{
  // Sets the geometry of the widget relative to its parent

  const int x = p.getX();
  const int y = p.getY();
  std::size_t w = s.getWidth();
  std::size_t h = s.getHeight();
  w = std::min (w, size_hints.max_width);
  w = std::max (w, size_hints.min_width);
  h = std::min (h, size_hints.max_height);
  h = std::max (h, size_hints.min_height);

  if ( getPos() == p && getWidth() == w && getHeight() == h )
    return;

  if ( ! isWindowWidget() )
  {
    ( x < 1 ) ? wsize.setX(1) : wsize.setX(x);
    ( y < 1 ) ? wsize.setY(1) : wsize.setY(y);
  }
  else
  {
    wsize.setX(x);
    wsize.setY(y);
  }

  ( w < 1 ) ? wsize.setWidth(1) : wsize.setWidth(w);
  ( h < 1 ) ? wsize.setHeight(1) : wsize.setHeight(h);

  adjust_wsize = wsize;
  const int term_x = getTermX();
  const int term_y = getTermY();

  wclient_offset.setCoordinates ( term_x - 1 + padding.left
                                , term_y - 1 + padding.top
                                , term_x - 2 + int(getWidth()) - padding.right
                                , term_y - 2 + int(getHeight()) - padding.bottom );

  double_flatline_mask.top.resize (getWidth(), false);
  double_flatline_mask.right.resize (getHeight(), false);
  double_flatline_mask.bottom.resize (getWidth(), false);
  double_flatline_mask.left.resize (getHeight(), false);

  if ( adjust )
    adjustSize();
}

//----------------------------------------------------------------------
bool FWidget::setCursorPos (const FPoint& pos)
{
  // sets the input cursor position

  widget_cursor_position.setPoint(pos);

  if ( ! flags.focus || isWindowWidget() )
    return false;

  if ( ! FWindow::getWindowWidget(this) )
    return false;

  const auto& area = getPrintArea();

  if ( area->widget )
  {
    int woffsetX = getTermX() - area->widget->getTermX();
    int woffsetY = getTermY() - area->widget->getTermY();

    if ( isChildPrintArea() )
    {
      woffsetX += (1 - area->widget->getLeftPadding());
      woffsetY += (1 - area->widget->getTopPadding());
    }

    setAreaCursor ( FPoint ( woffsetX + pos.getX()
                           , woffsetY + pos.getY() )
                  , flags.visible_cursor
                  , area );
    return true;
  }

  return false;
}

//----------------------------------------------------------------------
void FWidget::setPrintPos (const FPoint& pos)
{
  const FPoint p{ woffset.getX1() + getX() + pos.getX() - 1,
                  woffset.getY1() + getY() + pos.getY() - 1 };
  setPrintCursor(p);
}

//----------------------------------------------------------------------
void FWidget::setDoubleFlatLine (fc::sides side, bool bit)
{
  uLong length{};

  assert ( side == fc::top
        || side == fc::right
        || side == fc::bottom
        || side == fc::left );

  switch ( side )
  {
    case fc::top:
      length = double_flatline_mask.top.size();
      double_flatline_mask.top.assign(length, bit);
      break;

    case fc::right:
      length = double_flatline_mask.right.size();
      double_flatline_mask.right.assign(length, bit);
      break;

    case fc::bottom:
      length = double_flatline_mask.bottom.size();
      double_flatline_mask.bottom.assign(length, bit);
      break;

    case fc::left:
      length = double_flatline_mask.left.size();
      double_flatline_mask.left.assign(length, bit);
      break;

    default:
      break;
  }
}

//----------------------------------------------------------------------
void FWidget::setDoubleFlatLine (fc::sides side, int pos, bool bit)
{
  assert ( side == fc::top
        || side == fc::right
        || side == fc::bottom
        || side == fc::left );

  assert ( pos >= 1 );

  uLong length{};
  const uLong index = uLong(pos - 1);

  switch ( side )
  {
    case fc::top:
      length = double_flatline_mask.top.size();

      if ( index < length )
        double_flatline_mask.top[index] = bit;

      break;

    case fc::right:
      length = double_flatline_mask.right.size();

      if ( index < length )
        double_flatline_mask.right[index] = bit;

      break;

    case fc::bottom:
      length = double_flatline_mask.bottom.size();

      if ( index < length )
        double_flatline_mask.bottom[index] = bit;

      break;

    case fc::left:
      length = double_flatline_mask.left.size();

      if ( index < length )
        double_flatline_mask.left[index] = bit;

      break;

    default:
      break;
  }
}

//----------------------------------------------------------------------
FWidget* FWidget::childWidgetAt (const FPoint& pos)
{
  if ( ! hasChildren() )
    return nullptr;

  for (auto&& child : getChildren())
  {
    if ( ! child->isWidget() )
      continue;

    auto widget = static_cast<FWidget*>(child);

    if ( widget->isEnabled()
      && widget->isShown()
      && ! widget->isWindowWidget()
      && widget->getTermGeometry().contains(pos) )
    {
      auto sub_child = widget->childWidgetAt(pos);
      return ( sub_child != nullptr ) ? sub_child : widget;
    }
  }

  return nullptr;
}

//----------------------------------------------------------------------
int FWidget::numOfFocusableChildren()
{
  if ( ! hasChildren() )
    return 0;

  int num{0};

  for (auto&& child : getChildren())
  {
    if ( child->isWidget() )
    {
      const auto& widget = static_cast<FWidget*>(child);

      if ( widget->isShown()
        && widget->acceptFocus()
        && ! widget->isWindowWidget() )
        num++;
    }
  }

  return num;
}

//----------------------------------------------------------------------
bool FWidget::close()
{
  // Sends a close event and quits the application on acceptance

  FCloseEvent ev(fc::Close_Event);
  FApplication::sendEvent(this, &ev);

  if ( ev.isAccepted() )
  {
    if ( this == getMainWidget() )
      quit();
    else
    {
      hide();

      if ( ! flags.modal )
        close_widget->push_back(this);
    }

    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
void FWidget::addCallback ( const FString& cb_signal
                          , FCallback cb_function
                          , FDataPtr data )
{
  // Add a (normal) function pointer as callback

  FCallbackData obj{ cb_signal, nullptr, cb_function, data };
  callback_objects.push_back(obj);
}

//----------------------------------------------------------------------
void FWidget::addCallback ( const FString& cb_signal
                          , FWidget*  cb_instance
                          , FCallback cb_function
                          , FDataPtr data )
{
  // Add a member function pointer as callback

  FCallbackData obj{ cb_signal, cb_instance, cb_function, data };
  callback_objects.push_back(obj);
}

//----------------------------------------------------------------------
void FWidget::delCallback (FCallback cb_function)
{
  // Delete cb_function form callback list

  if ( callback_objects.empty() )
    return;

  auto iter = callback_objects.begin();

  while ( iter != callback_objects.end() )
  {
    if ( getCallbackPtr(iter->cb_function) == getCallbackPtr(cb_function) )
      iter = callback_objects.erase(iter);
    else
      ++iter;
  }
}

//----------------------------------------------------------------------
void FWidget::delCallback (const FWidget* cb_instance)
{
  // Delete all member function pointer from cb_instance

  if ( callback_objects.empty() )
    return;

  auto iter = callback_objects.begin();

  while ( iter != callback_objects.end() )
  {
    if ( iter->cb_instance == cb_instance )
      iter = callback_objects.erase(iter);
    else
      ++iter;
  }
}

//----------------------------------------------------------------------
void FWidget::delCallbacks()
{
  // Delete all callbacks from this widget

  callback_objects.clear();  // function pointer
}

//----------------------------------------------------------------------
void FWidget::emitCallback (const FString& emit_signal)
{
  // Initiate callback for the given signal

  if ( callback_objects.empty() )
    return;

  for (auto&& cback : callback_objects)
  {
    if ( cback.cb_signal == emit_signal )
    {
      // Calling the stored function pointer
      auto callback = cback.cb_function;
      callback (this, cback.data);
    }
  }
}

//----------------------------------------------------------------------
void FWidget::addAccelerator (FKey key, FWidget* obj)
{
  // Adding a keyboard accelerator for the given widget

  auto widget = static_cast<FWidget*>(FWindow::getWindowWidget(obj));
  FAccelerator accel = { key, obj };

  if ( ! widget || widget == statusbar || widget == menubar )
    widget = getRootWidget();

  if ( widget )
    widget->accelerator_list.push_back(accel);
}

//----------------------------------------------------------------------
void FWidget::delAccelerator (FWidget* obj)
{
  // Deletes all accelerators of the given widget

  auto widget = static_cast<FWidget*>(FWindow::getWindowWidget(this));

  if ( ! widget || widget == statusbar || widget == menubar )
    widget = getRootWidget();

  if ( widget
    && ! widget->accelerator_list.empty() )
  {
    auto iter = widget->accelerator_list.begin();

    while ( iter != widget->accelerator_list.end() )
    {
      if ( iter->object == obj )
        iter = widget->accelerator_list.erase(iter);
      else
        ++iter;
    }
  }
}

//----------------------------------------------------------------------
void FWidget::redraw()
{
  // Redraw the widget immediately unless it is hidden.

  if ( ! redraw_root_widget )
    redraw_root_widget = this;

  if ( isRootWidget() )
  {
    startTerminalUpdate();
    // clean desktop
    setColor (wcolors.term_fg, wcolors.term_bg);
    clearArea (getVirtualDesktop());
  }
  else if ( ! isShown() )
    return;

  draw();

  if ( isRootWidget() )
    drawWindows();
  else
    drawChildren();

  if ( isRootWidget() )
    finishTerminalUpdate();

  if ( redraw_root_widget == this )
  {
    updateTerminal();
    flush();
    redraw_root_widget = nullptr;
  }
}

//----------------------------------------------------------------------
void FWidget::resize()
{
  if ( isRootWidget() )
  {
    const FRect old_term_geometry (getTermGeometry());
    detectTermSize();
    FRect term_geometry (getTermGeometry());
    term_geometry.move (-1, -1);

    if ( old_term_geometry.getSize() == term_geometry.getSize() )
      return;

    resizeVTerm (term_geometry.getSize());
    resizeArea (term_geometry, getShadow(), getVirtualDesktop());
    adjustSizeGlobal();
  }
  else
    adjustSize();

  // resize the four double-flatline-masks
  double_flatline_mask.top.resize (getWidth(), false);
  double_flatline_mask.right.resize (getHeight(), false);
  double_flatline_mask.bottom.resize (getWidth(), false);
  double_flatline_mask.left.resize (getHeight(), false);
}

//----------------------------------------------------------------------
void FWidget::show()
{
  // Make the widget visible and draw it

  if ( ! isVisible() )
    return;

  if ( ! init_desktop )
  {
    // Sets the initial screen settings
    initScreenSettings();
    // Initializing vdesktop
    const auto& r = getRootWidget();
    setColor(r->getForegroundColor(), r->getBackgroundColor());
    clearArea (getVirtualDesktop());
    // Destop is now initialized
    init_desktop = true;
  }

  if ( ! show_root_widget )
  {
    startTerminalUpdate();
    show_root_widget = this;
  }

  draw();  // Draw the widget
  flags.hidden = false;
  flags.shown = true;

  if ( hasChildren() )
  {
    for (auto&& child : getChildren())
    {
      if ( child->isWidget() )
      {
        auto widget = static_cast<FWidget*>(child);

        if ( ! widget->flags.hidden )
          widget->show();
      }
    }
  }

  if ( show_root_widget && show_root_widget == this )
  {
    finishTerminalUpdate();
    updateTerminal();
    flush();
    show_root_widget = nullptr;
  }

  FShowEvent show_ev (fc::Show_Event);
  FApplication::sendEvent(this, &show_ev);
}

//----------------------------------------------------------------------
void FWidget::hide()
{
  // Hide the widget

  flags.hidden = true;

  if ( isVisible() )
  {
    flags.shown = false;

    if ( ! isDialogWidget()
      && FWidget::getFocusWidget() == this
      && ! focusPrevChild() )
    {
      if ( FWidget::getFocusWidget() )
        FWidget::getFocusWidget()->unsetFocus();

      FWidget::setFocusWidget(getParentWidget());
    }

    FHideEvent hide_ev (fc::Hide_Event);
    FApplication::sendEvent(this, &hide_ev);
  }
}

//----------------------------------------------------------------------
bool FWidget::focusFirstChild()
{
  if ( ! hasChildren() )
    return false;

  auto iter = FObject::begin();
  const auto last = FObject::end();

  while ( iter != last )
  {
    if ( ! (*iter)->isWidget() )
    {
      ++iter;
      continue;
    }

    auto widget = static_cast<FWidget*>(*iter);

    if ( widget->isEnabled()
      && widget->acceptFocus()
      && ! widget->isMenuWidget() )
    {
      widget->setFocus();

      if ( widget->numOfChildren() >= 1 )
      {
        if ( ! widget->focusFirstChild() && widget->isWindowWidget() )
        {
          ++iter;
          continue;
        }
      }

      return true;
    }

    ++iter;
  }

  return false;
}

//----------------------------------------------------------------------
bool FWidget::focusLastChild()
{
  if ( ! hasChildren() )
    return false;

  auto iter  = FObject::end();
  const auto first = FObject::begin();

  do
  {
    --iter;

    if ( ! (*iter)->isWidget() )
      continue;

    auto widget = static_cast<FWidget*>(*iter);

    if ( widget->isEnabled()
      && widget->acceptFocus()
      && ! widget->isMenuWidget() )
    {
      widget->setFocus();

      if ( widget->numOfChildren() >= 1 )
      {
        if ( ! widget->focusLastChild() && widget->isWindowWidget() )
          continue;
      }

      return true;
    }
  }
  while ( iter != first );

  return false;
}

//----------------------------------------------------------------------
void FWidget::move (const FPoint& pos)
{
  wsize.move(pos);
  adjust_wsize.move(pos);
}

//----------------------------------------------------------------------
void FWidget::quit()
{
  FApplication::exit(0);
}


// protected methods of FWidget
//----------------------------------------------------------------------
FVTerm::FTermArea* FWidget::getPrintArea()
{
  // returns the print area of this object

  if ( getCurrentPrintArea() )
    return getCurrentPrintArea();
  else
  {
    FWidget* obj{};
    FWidget* p_obj = this;

    do
    {
      obj = p_obj;
      p_obj = static_cast<FWidget*>(obj->getParent());
    }
    while ( ! obj->getVWin() && ! obj->getChildPrintArea() && p_obj );

    if ( obj->getVWin() )
    {
      setPrintArea (obj->getVWin());
      return getCurrentPrintArea();
    }
    else if ( obj->getChildPrintArea() )
    {
      setPrintArea (obj->getChildPrintArea());
      return getCurrentPrintArea();
    }
  }

  return getVirtualDesktop();
}

//----------------------------------------------------------------------
void FWidget::addPreprocessingHandler ( const FVTerm* instance
                                      , const FPreprocessingFunction& function )
{
  if ( ! getCurrentPrintArea() )
    FWidget::getPrintArea();

  FVTerm::addPreprocessingHandler (instance, function);
}

//----------------------------------------------------------------------
void FWidget::delPreprocessingHandler (const FVTerm* instance)
{
  if ( ! getCurrentPrintArea() )
    FWidget::getPrintArea();

  FVTerm::delPreprocessingHandler (instance);
}

//----------------------------------------------------------------------
bool FWidget::isChildPrintArea() const
{
  const auto& p_obj = static_cast<FWidget*>(getParent());

  if ( p_obj
    && p_obj->getChildPrintArea()
    && p_obj->getChildPrintArea() == getCurrentPrintArea() )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
void FWidget::setStatusBar (FStatusBar* sbar)
{
  if ( ! sbar || statusbar == sbar )
    return;

  if ( statusbar )
    delete statusbar;

  statusbar = sbar;
}

//----------------------------------------------------------------------
void FWidget::setMenuBar (FMenuBar* mbar)
{
  if ( ! mbar || menubar == mbar )
    return;

  if ( menubar )
    delete menubar;

  menubar = mbar;
}

//----------------------------------------------------------------------
void FWidget::setParentOffset()
{
  const auto& p = getParentWidget();

  if ( p )
    woffset = p->wclient_offset;
}

//----------------------------------------------------------------------
void FWidget::setTermOffset()
{
  const auto& r = getRootWidget();
  const int w = int(r->getWidth());
  const int h = int(r->getHeight());
  woffset.setCoordinates (0, 0, w - 1, h - 1);
}

//----------------------------------------------------------------------
void FWidget::setTermOffsetWithPadding()
{
  const auto& r = getRootWidget();
  woffset.setCoordinates ( r->getLeftPadding()
                         , r->getTopPadding()
                         , int(r->getWidth()) - 1 - r->getRightPadding()
                         , int(r->getHeight()) - 1 - r->getBottomPadding() );
}

//----------------------------------------------------------------------
void FWidget::adjustSize()
{
  if ( ! isRootWidget() )
  {
    const auto& p = getParentWidget();

    if ( isWindowWidget() )
    {
      if ( ignore_padding && ! isDialogWidget() )
        setTermOffset();
      else
        woffset = root_widget->wclient_offset;
    }
    else if ( ignore_padding && p )
    {
      woffset.setCoordinates ( p->getTermX() - 1
                             , p->getTermY() - 1
                             , p->getTermX() + int(p->getWidth()) - 2
                             , p->getTermY() + int(p->getHeight()) - 2 );
    }
    else if ( p )
      woffset = p->wclient_offset;

    adjust_wsize = wsize;
  }

  // Move and shrink in case of lack of space
  if ( ! hasChildPrintArea() )
    insufficientSpaceAdjust();

  wclient_offset.setCoordinates
  (
    getTermX() - 1 + padding.left,
    getTermY() - 1 + padding.top,
    getTermX() - 2 + int(getWidth()) - padding.right,
    getTermY() - 2 + int(getHeight()) - padding.bottom
  );

  if ( hasChildren() )
  {
    for (auto&& child : getChildren())
    {
      if ( child->isWidget() )
      {
        auto widget = static_cast<FWidget*>(child);

        if ( ! widget->isWindowWidget() )
          widget->adjustSize();
      }
    }
  }
}

//----------------------------------------------------------------------
void FWidget::adjustSizeGlobal()
{
  if ( ! isRootWidget() )
  {
    getRootWidget()->adjustSizeGlobal();
    return;
  }

  if ( window_list && ! window_list->empty() )
  {
    for (auto&& window : *window_list)
      window->adjustSize();
  }
}

//----------------------------------------------------------------------
void FWidget::hideArea (const FSize& size)
{
  if ( size.isEmpty() )
    return;

  FColor fg{};
  FColor bg{};
  const auto& parent_widget = getParentWidget();

  if ( parent_widget )
  {
    fg = parent_widget->getForegroundColor();
    bg = parent_widget->getBackgroundColor();
  }
  else
  {
    fg = wcolors.dialog_fg;
    bg = wcolors.dialog_bg;
  }

  setColor (fg, bg);

  if ( size.getWidth() == 0 )
    return;

  for (int y{0}; y < int(size.getHeight()); y++)
  {
    print() << FPoint(1, 1 + y) << FString(size.getWidth(), L' ');
  }

  flush();
}

//----------------------------------------------------------------------
bool FWidget::focusNextChild()
{
  if ( isDialogWidget() || ! hasParent() )
    return false;

  const auto& parent = getParentWidget();

  if ( ! parent
    || ! parent->hasChildren()
    || parent->numOfFocusableChildren() <= 1 )
    return false;

  auto iter = parent->begin();
  const auto last = parent->end();

  while ( iter != last )
  {
    if ( ! (*iter)->isWidget() )
    {
      ++iter;
      continue;
    }

    const auto& w = static_cast<FWidget*>(*iter);

    if ( w != this )
    {
      ++iter;
      continue;
    }

    FWidget* next{nullptr};
    auto next_element = iter;

    do
    {
      ++next_element;

      if ( next_element == parent->end() )
        next_element = parent->begin();

      if ( ! (*next_element)->isWidget() )
        continue;

      next = static_cast<FWidget*>(*next_element);
    } while ( ! next
           || ! next->isEnabled()
           || ! next->acceptFocus()
           || ! next->isShown()
           || next->isWindowWidget() );

    bool accpt = changeFocus (next, parent, fc::FocusNextWidget);

    if ( ! accpt )
      return false;

    break;  // The focus has been changed
  }

  return true;
}

//----------------------------------------------------------------------
bool FWidget::focusPrevChild()
{
  if ( isDialogWidget() || ! hasParent() )
    return false;

  const auto& parent = getParentWidget();

  if ( ! parent
    || ! parent->hasChildren()
    || parent->numOfFocusableChildren() <= 1 )
    return false;

  auto iter = parent->end();
  const auto first = parent->begin();

  do
  {
    --iter;

    if ( ! (*iter)->isWidget() )
      continue;

    const auto& w = static_cast<FWidget*>(*iter);

    if ( w != this )
      continue;

    FWidget* prev{nullptr};
    auto prev_element = iter;

    do
    {
      if ( ! (*prev_element)->isWidget() )
      {
        --prev_element;
        continue;
      }

      if ( prev_element == parent->begin() )
        prev_element = parent->end();

      --prev_element;
      prev = static_cast<FWidget*>(*prev_element);
    } while ( ! prev
           || ! prev->isEnabled()
           || ! prev->acceptFocus()
           || ! prev->isShown()
           || prev->isWindowWidget() );

    const bool accpt = changeFocus (prev, parent, fc::FocusPreviousWidget);

    if ( ! accpt )
      return false;

    break;  // The focus has been changed
  }
  while ( iter != first );

  return true;
}

//----------------------------------------------------------------------
bool FWidget::event (FEvent* ev)
{
  switch ( uInt(ev->type()) )
  {
    case fc::KeyPress_Event:
      KeyPressEvent (static_cast<FKeyEvent*>(ev));
      break;

    case fc::KeyUp_Event:
      onKeyUp (static_cast<FKeyEvent*>(ev));
      break;

    case fc::KeyDown_Event:
      KeyDownEvent (static_cast<FKeyEvent*>(ev));
      break;

    case fc::MouseDown_Event:
      emitCallback("mouse-press");
      onMouseDown (static_cast<FMouseEvent*>(ev));
      break;

    case fc::MouseUp_Event:
      emitCallback("mouse-release");
      onMouseUp (static_cast<FMouseEvent*>(ev));
      break;

    case fc::MouseDoubleClick_Event:
      onMouseDoubleClick (static_cast<FMouseEvent*>(ev));
      break;

    case fc::MouseWheel_Event:
      emitWheelCallback(static_cast<FWheelEvent*>(ev));
      onWheel (static_cast<FWheelEvent*>(ev));
      break;

    case fc::MouseMove_Event:
      emitCallback("mouse-move");
      onMouseMove (static_cast<FMouseEvent*>(ev));
      break;

    case fc::FocusIn_Event:
      emitCallback("focus-in");
      onFocusIn (static_cast<FFocusEvent*>(ev));
      break;

    case fc::FocusOut_Event:
      emitCallback("focus-out");
      onFocusOut (static_cast<FFocusEvent*>(ev));
      break;

    case fc::ChildFocusIn_Event:
      onChildFocusIn (static_cast<FFocusEvent*>(ev));
      break;

    case fc::ChildFocusOut_Event:
      onChildFocusOut (static_cast<FFocusEvent*>(ev));
      break;

    case fc::Accelerator_Event:
      onAccel (static_cast<FAccelEvent*>(ev));
      break;

    case fc::Resize_Event:
      onResize (static_cast<FResizeEvent*>(ev));
      break;

    case fc::Show_Event:
      onShow (static_cast<FShowEvent*>(ev));
      break;

    case fc::Hide_Event:
      onHide (static_cast<FHideEvent*>(ev));
      break;

    case fc::Close_Event:
      onClose (static_cast<FCloseEvent*>(ev));
      break;

    case fc::Timer_Event:
      onTimer (static_cast<FTimerEvent*>(ev));
      break;

    default:
      return false;
  }

  return true;
}

//----------------------------------------------------------------------
void FWidget::onKeyPress (FKeyEvent*)
{ }

//----------------------------------------------------------------------
void FWidget::onKeyUp (FKeyEvent*)
{ }

//----------------------------------------------------------------------
void FWidget::onKeyDown (FKeyEvent*)
{ }

//----------------------------------------------------------------------
void FWidget::onMouseDown (FMouseEvent*)
{ }

//----------------------------------------------------------------------
void FWidget::onMouseUp (FMouseEvent*)
{ }

//----------------------------------------------------------------------
void FWidget::onMouseDoubleClick (FMouseEvent*)
{ }

//----------------------------------------------------------------------
void FWidget::onWheel (FWheelEvent*)
{ }

//----------------------------------------------------------------------
void FWidget::onMouseMove (FMouseEvent*)
{ }

//----------------------------------------------------------------------
void FWidget::onFocusIn (FFocusEvent*)
{ }

//----------------------------------------------------------------------
void FWidget::onFocusOut (FFocusEvent*)
{ }

//----------------------------------------------------------------------
void FWidget::onChildFocusIn (FFocusEvent*)
{ }

//----------------------------------------------------------------------
void FWidget::onChildFocusOut (FFocusEvent*)
{ }

//----------------------------------------------------------------------
void FWidget::onAccel (FAccelEvent*)
{ }

//----------------------------------------------------------------------
void FWidget::onResize (FResizeEvent* ev)
{
  // The terminal was resized
  root_widget->resize();
  root_widget->redraw();
  ev->accept();
}

//----------------------------------------------------------------------
void FWidget::onShow (FShowEvent*)
{ }

//----------------------------------------------------------------------
void FWidget::onHide (FHideEvent*)
{ }

//----------------------------------------------------------------------
void FWidget::onClose (FCloseEvent* ev)
{
  ev->accept();
}


// private methods of FWidget
//----------------------------------------------------------------------
void FWidget::initRootWidget()
{
  try
  {
    // Initialize widget lists
    window_list        = new FWidgetList();
    dialog_list        = new FWidgetList();
    always_on_top_list = new FWidgetList();
    close_widget       = new FWidgetList();
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
    return;
  }

  hideable = isCursorHideable();
  flags.visible_cursor = ! hideable;

  // Determine width and height of the terminal
  detectTermSize();
  wsize.setRect(1, 1, getDesktopWidth(), getDesktopHeight());
  adjust_wsize = wsize;
  woffset.setRect(0, 0, getDesktopWidth(), getDesktopHeight());
  wclient_offset = woffset;

  double_flatline_mask.top.resize (getWidth(), false);
  double_flatline_mask.right.resize (getHeight(), false);
  double_flatline_mask.bottom.resize (getWidth(), false);
  double_flatline_mask.left.resize (getHeight(), false);

  // Initialize default widget colors
  setColorTheme();

  // Default foreground and background color of the desktop/terminal
  foreground_color = wcolors.term_fg;
  background_color = wcolors.term_bg;
  init_desktop = false;
}

//----------------------------------------------------------------------
void FWidget::finish()
{
  if ( close_widget )
  {
    delete close_widget;
    close_widget = nullptr;
  }

  if ( dialog_list )
  {
    delete dialog_list;
    dialog_list = nullptr;
  }

  if ( always_on_top_list )
  {
    delete always_on_top_list;
    always_on_top_list = nullptr;
  }

  if ( window_list )
  {
    delete window_list;
    window_list = nullptr;
  }
}

//----------------------------------------------------------------------
inline void FWidget::insufficientSpaceAdjust()
{
  // Move and shrink widget if there is not enough space available

  if ( isWindowWidget() )
    return;

  // move left if not enough space
  while ( getTermX() + int(getWidth()) - padding.right > woffset.getX2() + 2 )
  {
    adjust_wsize.x1_ref()--;
    adjust_wsize.x2_ref()--;

    if ( adjust_wsize.x1_ref() < 1 )
      adjust_wsize.x1_ref() = 1;
  }

  // move up if not enough space
  while ( getTermY() + int(getHeight()) - padding.bottom > woffset.getY2() + 2 )
  {
    adjust_wsize.y1_ref()--;
    adjust_wsize.y2_ref()--;

    if ( adjust_wsize.y1_ref() < 1 )
      adjust_wsize.y1_ref() = 1;
  }

  // reduce the width if not enough space
  while ( woffset.getX1() + int(getWidth()) - 1 > woffset.getX2() )
    adjust_wsize.x2_ref()--;

  if ( getWidth() < size_hints.min_width )
    adjust_wsize.setWidth(size_hints.min_width);

  if ( getWidth() == 0 )
    adjust_wsize.setWidth(1);

  // reduce the height if not enough space
  while ( woffset.getY1() + int(getHeight()) - 1 > woffset.getY2() )
    adjust_wsize.y2_ref()--;

  if ( getHeight() < size_hints.min_height )
    adjust_wsize.setHeight(size_hints.min_height);

  if ( getHeight() == 0 )
    adjust_wsize.setHeight(1);
}

//----------------------------------------------------------------------
void FWidget::KeyPressEvent (FKeyEvent* kev)
{
  FWidget* widget(this);

  while ( widget )
  {
    widget->onKeyPress(kev);

    if ( ! kev->isAccepted() )
    {
      const FKey key = kev->key();

      if ( [&] ()
           {
             if ( isFocusNextKey(key) )
               return focusNextChild();
             else if ( isFocusPrevKey(key) )
               return focusPrevChild();

             return false;
           }() )
      {
        return;
      }
    }

    if ( kev->isAccepted()
      || widget->isRootWidget()
      || widget->getFlags().modal )
      return;

    widget = widget->getParentWidget();
  }
}

//----------------------------------------------------------------------
void FWidget::KeyDownEvent (FKeyEvent* kev)
{
  FWidget* widget(this);

  while ( widget )
  {
    widget->onKeyDown(kev);

    if ( kev->isAccepted() || widget->isRootWidget() )
      break;

    widget = widget->getParentWidget();
  }
}

//----------------------------------------------------------------------
void FWidget::emitWheelCallback (const FWheelEvent* ev)
{
  const int wheel = ev->getWheel();

  if ( wheel == fc::WheelUp )
    emitCallback("mouse-wheel-up");
  else if ( wheel == fc::WheelDown )
    emitCallback("mouse-wheel-down");
}

//----------------------------------------------------------------------
void FWidget::setWindowFocus (bool enable)
{
  // set the window focus

  if ( ! enable )
    return;

  auto window = FWindow::getWindowWidget(this);

  if ( ! window )
    return;

  if ( ! window->isWindowActive() )
  {
    bool has_raised = window->raiseWindow();
    FWindow::setActiveWindow(window);

    if ( has_raised && window->isVisible() && window->isShown() )
      window->redraw();
  }

  window->setWindowFocusWidget(this);
}

//----------------------------------------------------------------------
FWidget::FCallbackPtr FWidget::getCallbackPtr (FCallback cb_function)
{
  return *cb_function.template target<FCallbackPtr>();
}

//----------------------------------------------------------------------
bool FWidget::changeFocus ( FWidget* follower, FWidget* parent
                          , fc::FocusTypes ft )
{
  FFocusEvent out (fc::FocusOut_Event);
  out.setFocusType(ft);
  FApplication::sendEvent(this, &out);

  FFocusEvent cfo (fc::ChildFocusOut_Event);
  cfo.setFocusType(ft);
  cfo.ignore();
  FApplication::sendEvent(parent, &cfo);

  if ( cfo.isAccepted() )
    out.ignore();

  if ( out.isAccepted() )
  {
    if ( follower == this )
      return false;

    follower->setFocus();
    FFocusEvent cfi (fc::ChildFocusIn_Event);
    FApplication::sendEvent(parent, &cfi);

    FFocusEvent in (fc::FocusIn_Event);
    in.setFocusType(ft);
    FApplication::sendEvent(follower, &in);

    if ( in.isAccepted() )
    {
      redraw();
      follower->redraw();
      updateTerminal();
      flush();
    }
  }

  return true;
}

//----------------------------------------------------------------------
void FWidget::draw()
{ }

//----------------------------------------------------------------------
void FWidget::drawWindows()
{
  // redraw windows
  FChar default_char{};
  default_char.ch           = ' ';
  default_char.fg_color     = fc::Black;
  default_char.bg_color     = fc::Black;
  default_char.attr.byte[0] = 0;
  default_char.attr.byte[1] = 0;

  if ( ! window_list || window_list->empty() )
    return;

  for (auto&& window : *window_list)
  {
    if ( window->isShown() )
    {
      auto v_win = window->getVWin();
      const int w = v_win->width  + v_win->right_shadow;
      const int h = v_win->height + v_win->bottom_shadow;
      std::fill_n (v_win->data, w * h, default_char);
      window->redraw();
    }
  }
}

//----------------------------------------------------------------------
void FWidget::drawChildren()
{
  // draw child elements
  if ( ! hasChildren() )
    return;

  for (auto&& child : getChildren())
  {
    if ( child->isWidget() )
    {
      auto widget = static_cast<FWidget*>(child);

      if ( widget->isShown() && ! widget->isWindowWidget() )
        widget->redraw();
    }
  }
}

//----------------------------------------------------------------------
void FWidget::setColorTheme()
{
  // Sets the default color theme

  if ( getMaxColor() < 16 )  // for 8 color mode
    wcolors.set8ColorTheme();
  else
    wcolors.set16ColorTheme();
}

//----------------------------------------------------------------------
void FWidget::setStatusbarText (bool enable)
{
  if ( ! isEnabled() || ! getStatusBar() )
    return;

  if ( enable )
  {
    const auto& msg = getStatusbarMessage();
    const auto& curMsg = getStatusBar()->getMessage();

    if ( curMsg != msg )
      getStatusBar()->setMessage(msg);
  }
  else
  {
    getStatusBar()->clearMessage();
  }
}


// non-member functions
//----------------------------------------------------------------------
void detectTermSize()
{
  const auto& r = root_widget;
  FTerm::detectTermSize();
  r->adjust_wsize.setRect (1, 1, r->getDesktopWidth(), r->getDesktopHeight());
  r->woffset.setRect (0, 0, r->getDesktopWidth(), r->getDesktopHeight());
  r->wclient_offset.setCoordinates
  (
    r->padding.left,
    r->padding.top,
    int(r->getDesktopWidth()) - 1 - r->padding.right,
    int(r->getDesktopHeight()) - 1 - r->padding.bottom
  );
}

}  // namespace finalcut
