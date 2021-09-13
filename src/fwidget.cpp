/***********************************************************************
* fwidget.cpp - Intermediate base class for all widget objects         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2021 Markus Gans                                      *
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

#include <algorithm>
#include <vector>

#include "final/fapplication.h"
#include "final/fevent.h"
#include "final/flog.h"
#include "final/fmenubar.h"
#include "final/fstartoptions.h"
#include "final/fstatusbar.h"
#include "final/fstring.h"
#include "final/ftermdata.h"
#include "final/fwidget.h"
#include "final/fwidgetcolors.h"
#include "final/fwindow.h"

namespace finalcut
{

namespace internal
{

struct var
{
  static FWidget* root_widget;  // global FWidget object
};

FWidget* var::root_widget{nullptr};

}  // namespace internal

// static class attributes
FStatusBar*           FWidget::statusbar{nullptr};
FMenuBar*             FWidget::menubar{nullptr};
FWidget*              FWidget::show_root_widget{nullptr};
FWidget*              FWidget::redraw_root_widget{nullptr};
FWidget::FWidgetList* FWidget::dialog_list{nullptr};
FWidget::FWidgetList* FWidget::always_on_top_list{nullptr};
FWidget::FWidgetList* FWidget::close_widget{nullptr};
bool                  FWidget::init_terminal{false};
bool                  FWidget::init_desktop{false};
uInt                  FWidget::modal_dialog_counter{};

//----------------------------------------------------------------------
// class FWidget
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FWidget::FWidget (FWidget* parent)
  : FVTerm{}
  , FObject{parent}
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
    if ( internal::var::root_widget )
    {
      static auto& fterm_data = FTermData::getInstance();
      fterm_data.setExitMessage("FWidget: No parent defined! "
                                "There should be only one root object");
      FApplication::exit(EXIT_FAILURE);
      return;
    }

    initRootWidget();
  }
  else
  {
    woffset = parent->wclient_offset;
  }

  flags.visible_cursor = false;
  double_flatline_mask.top.resize (getWidth(), false);
  double_flatline_mask.right.resize (getHeight(), false);
  double_flatline_mask.bottom.resize (getWidth(), false);
  double_flatline_mask.left.resize (getHeight(), false);
}

//----------------------------------------------------------------------
FWidget::~FWidget()  // destructor
{
  processDestroy();
  delCallback();
  removeQueuedEvent();

  // unset clicked widget
  if ( this == getClickedWidget() )
    setClickedWidget(nullptr);

  // unset keyboard widget
  if ( this == FApplication::getKeyboardWidget() )
    FApplication::setKeyboardWidget(nullptr);

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
  if ( internal::var::root_widget == this )
    finish();
}


// public methods of FWidget
//----------------------------------------------------------------------
FWidget* FWidget::getRootWidget()
{
  FWidget* obj = this;
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
auto FWidget::getColorTheme() -> std::shared_ptr<FWidgetColors>&
{
  static const auto& color_theme = make_unique<std::shared_ptr<FWidgetColors>>();
  return *color_theme;
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
std::vector<bool>& FWidget::doubleFlatLine_ref (Side side)
{
  assert ( side == Side::Top
        || side == Side::Right
        || side == Side::Bottom
        || side == Side::Left );

  switch ( side )
  {
    case Side::Top:
      return double_flatline_mask.top;

    case Side::Right:
      return double_flatline_mask.right;

    case Side::Bottom:
      return double_flatline_mask.bottom;

    case Side::Left:
      return double_flatline_mask.left;
  }

  return double_flatline_mask.top;
}

//----------------------------------------------------------------------
FPoint FWidget::getPrintPos()
{
  const auto& cur = getPrintCursor();
  return { cur.getX() - woffset.getX1() - getX() + 1
         , cur.getY() - woffset.getY1() - getY() + 1 };
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
  if ( enable )
    emitCallback("enable");
  else
    emitCallback("disable");

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
void FWidget::resetColors()
{
  if ( ! hasChildren() )
    return;

  for (auto&& child : getChildren())
  {
    if ( child->isWidget() )
    {
      auto widget = static_cast<FWidget*>(child);
      widget->resetColors();
    }
  }
}

//----------------------------------------------------------------------
void FWidget::useParentWidgetColor()
{
  const auto& parent_widget = getParentWidget();

  if ( parent_widget )
  {
    setForegroundColor (parent_widget->getForegroundColor());
    setBackgroundColor (parent_widget->getBackgroundColor());
  }
  else  // Fallback
  {
    const auto& wc = getColorTheme();
    setForegroundColor (wc->dialog_fg);
    setBackgroundColor (wc->dialog_bg);
  }

  setColor();
}

//----------------------------------------------------------------------
void FWidget::setColor() const
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
  FPoint pos{p};

  if ( getX() == pos.getX() && wsize.getX() == pos.getX()
    && getY() == pos.getY() && wsize.getY() == pos.getY() )
  {
    return;
  }

  if ( ! isWindowWidget() )  // A widgets must be inside the client area
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

  if ( width < 1 )  // A width can never be narrower than 1 character
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

  if ( height < 1 )  // A height can never be narrower than 1 character
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

  if ( width < 1 )  // A width can never be narrower than 1 character
    width = 1;

  if ( height < 1 )  // A height can never be narrower than 1 character
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
      auto r = internal::var::root_widget;
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
      auto r = internal::var::root_widget;
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
      auto r = internal::var::root_widget;
      auto root_height = int(r->getHeight());
      auto root_pb = r->padding.bottom;

      if ( root_height > 1 + root_pb )
        r->wclient_offset.setY2 (root_height - 1 - root_pb);

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
      auto r = internal::var::root_widget;
      auto root_width = int(r->getWidth());
      auto root_pr = r->padding.right;

      if ( root_width > 1 + root_pr )
        r->wclient_offset.setX2  (root_width - 1 - root_pr);

      adjustSizeGlobal();
    }
    else
      adjustSize();
  }
}

//----------------------------------------------------------------------
void FWidget::setTerminalSize (const FSize& size) const
{
  // Set terminal size to width x height

  if ( FVTerm::getFOutput()->allowsTerminalSizeManipulation() )
  {
    internal::var::root_widget->wsize.setRect(FPoint{1, 1}, size);
    internal::var::root_widget->adjust_wsize = internal::var::root_widget->wsize;
    FVTerm::getFOutput()->setTerminalSize(size);
    detectTerminalSize();
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

  if ( isWindowWidget() )  // A window widget can be outside
  {
    wsize.setX(x);
    wsize.setY(y);
  }
  else  // A normal widget must be inside the client area
  {
    wsize.setX(std::max(x, 1));
    wsize.setY(std::max(y, 1));
  }

  wsize.setWidth(std::max(w, std::size_t(1u)));
  wsize.setHeight(std::max(h, std::size_t(1u)));
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

  if ( ! flags.focus || flags.hidden || isWindowWidget() )
    return false;

  if ( ! FWindow::getWindowWidget(this) )
    return false;

  const auto& area = getPrintArea();

  if ( area->hasOwner() )
  {
    const auto area_owner = area->getOwner<FWidget*>();
    int woffsetX = getTermX() - area_owner->getTermX();
    int woffsetY = getTermY() - area_owner->getTermY();

    if ( isChildPrintArea() )
    {
      woffsetX += (1 - area_owner->getLeftPadding());
      woffsetY += (1 - area_owner->getTopPadding());
    }

    bool visible = ! isCursorHideable() || flags.visible_cursor;
    setAreaCursor ( { woffsetX + pos.getX()
                    , woffsetY + pos.getY() }
                  , visible
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
  setCursor(p);
}

//----------------------------------------------------------------------
void FWidget::setDoubleFlatLine (Side side, bool bit)
{
  assert ( side == Side::Top
        || side == Side::Right
        || side == Side::Bottom
        || side == Side::Left );

  uLong length{};

  switch ( side )
  {
    case Side::Top:
      length = double_flatline_mask.top.size();
      double_flatline_mask.top.assign(length, bit);
      break;

    case Side::Right:
      length = double_flatline_mask.right.size();
      double_flatline_mask.right.assign(length, bit);
      break;

    case Side::Bottom:
      length = double_flatline_mask.bottom.size();
      double_flatline_mask.bottom.assign(length, bit);
      break;

    case Side::Left:
      length = double_flatline_mask.left.size();
      double_flatline_mask.left.assign(length, bit);
      break;
  }
}

//----------------------------------------------------------------------
void FWidget::setDoubleFlatLine (Side side, int pos, bool bit)
{
  assert ( side == Side::Top
        || side == Side::Right
        || side == Side::Bottom
        || side == Side::Left );

  assert ( pos >= 1 );

  uLong length{};
  const auto index = uLong(pos - 1);

  switch ( side )
  {
    case Side::Top:
      length = double_flatline_mask.top.size();

      if ( index < length )
        double_flatline_mask.top[index] = bit;

      break;

    case Side::Right:
      length = double_flatline_mask.right.size();

      if ( index < length )
        double_flatline_mask.right[index] = bit;

      break;

    case Side::Bottom:
      length = double_flatline_mask.bottom.size();

      if ( index < length )
        double_flatline_mask.bottom[index] = bit;

      break;

    case Side::Left:
      length = double_flatline_mask.left.size();

      if ( index < length )
        double_flatline_mask.left[index] = bit;

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

  FCloseEvent ev(Event::Close);
  FApplication::sendEvent(this, &ev);

  if ( ev.isAccepted() )
  {
    if ( this == getMainWidget() )
      quit();
    else
    {
      hide();

      if ( ! flags.modal && ! isInFWidgetList(close_widget, this) )
        close_widget->push_back(this);
    }

    return true;
  }
  else
    return false;
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
    startDrawing();
    // clean desktop
    auto color_theme = getColorTheme();
    setColor (color_theme->term_fg, color_theme->term_bg);
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
    finishDrawing();

  if ( redraw_root_widget == this )
    redraw_root_widget = nullptr;
}

//----------------------------------------------------------------------
void FWidget::resize()
{
  if ( isRootWidget() )
  {
    const FRect old_term_geometry {getTermGeometry()};
    determineDesktopSize();
    FRect term_geometry {getTermGeometry()};
    term_geometry.move (-1, -1);

    if ( old_term_geometry.getSize() == term_geometry.getSize() )
      return;

    resizeVTerm (term_geometry.getSize());
    resizeArea (term_geometry, getShadow(), getVirtualDesktop());
    startDrawing();  // Avoid flickering - no update during adjustment
    adjustSizeGlobal();
    finishDrawing();
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

  if ( ! isVisible() || isShown() || FApplication::isQuit() )
    return;

  // Initialize desktop on first call
  if ( ! init_desktop && internal::var::root_widget )
    internal::var::root_widget->initDesktop();

  if ( ! show_root_widget )
  {
    startDrawing();
    show_root_widget = this;
  }

  initWidgetLayout();  // Makes initial layout settings
  adjustSize();        // Alignment before drawing
  draw();              // Draw the widget
  flags.hidden = false;
  flags.shown = true;

  if ( hasChildren() )
  {
    for (auto&& child : getChildren())
    {
      auto child_widget = static_cast<FWidget*>(child);

      if ( child->isWidget() && ! child_widget->flags.hidden )
        child_widget->show();
    }
  }

  if ( show_root_widget && show_root_widget == this )
  {
    finishDrawing();
    forceTerminalUpdate();
    show_root_widget = nullptr;
  }

  FShowEvent show_ev (Event::Show);
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

    if ( flags.visible_cursor && FWidget::getFocusWidget() == this )
    {
      getPrintArea()->input_cursor_visible = false;
    }

    if ( ! isDialogWidget()
      && FWidget::getFocusWidget() == this
      && ! focusPrevChild() )
    {
      if ( FWidget::getFocusWidget() )
        FWidget::getFocusWidget()->unsetFocus();

      FWidget::setFocusWidget(getParentWidget());
    }

    FHideEvent hide_ev (Event::Hide);
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

      if ( widget->numOfChildren() >= 1
        && ! widget->focusFirstChild()
        && widget->isWindowWidget() )
      {
        ++iter;
        continue;
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

      if ( widget->numOfChildren() >= 1
        && ! widget->focusLastChild() && widget->isWindowWidget() )
        continue;

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
                                      , FPreprocessingFunction&& function )
{
  if ( ! getCurrentPrintArea() )
    FWidget::getPrintArea();

  FVTerm::addPreprocessingHandler (instance, std::move(function));
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
  const auto w = int(r->getWidth());
  const auto h = int(r->getHeight());
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
void FWidget::initTerminal()
{
  if ( hasParent() || init_terminal )
    return;

  // Initialize the physical and virtual terminal
  FVTerm::initTerminal();

  // Initialize default widget colors (after terminal detection)
  initColorTheme();

  // Set default foreground and background color of the desktop/terminal
  auto color_theme = getColorTheme();
  internal::var::root_widget->foreground_color = color_theme->term_fg;
  internal::var::root_widget->background_color = color_theme->term_bg;
  resetColors();

  // The terminal is now initialized
  init_terminal = true;
}

//----------------------------------------------------------------------
void FWidget::initDesktop()
{
  if ( hasParent() || init_desktop )
    return;

  if ( ! init_terminal )
    initTerminal();

  // Sets the initial screen settings
  FVTerm::getFOutput()->initScreenSettings();

  // Initializing vdesktop
  const auto& r = getRootWidget();
  setColor(r->getForegroundColor(), r->getBackgroundColor());
  clearArea (getVirtualDesktop());

  // Destop is now initialized
  init_desktop = true;
}

//----------------------------------------------------------------------
void FWidget::initLayout()
{
  // This method must be reimplemented in a subclass to automatically
  // set the widget layouts, before the initial drawing on the terminal
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
        woffset = internal::var::root_widget->wclient_offset;
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
      auto widget = static_cast<FWidget*>(child);

      if ( child->isWidget() && ! widget->isWindowWidget() )
        widget->adjustSize();
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

  adjustSize();  // Root widget / FApplication object

  if ( getWindowList() && ! getWindowList()->empty() )
  {
    for (auto&& window : *getWindowList())
      static_cast<FWidget*>(window)->adjustSize();
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
    auto color_theme = getColorTheme();
    fg = color_theme->dialog_fg;
    bg = color_theme->dialog_bg;
  }

  setColor (fg, bg);

  if ( size.getWidth() == 0 )
    return;

  for (auto y{0}; y < int(size.getHeight()); y++)
  {
    print() << FPoint{1, 1 + y} << FString{size.getWidth(), L' '};
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

    bool accpt = changeFocus (next, parent, FocusTypes::NextWidget);

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

    const bool accpt = changeFocus (prev, parent, FocusTypes::PreviousWidget);

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
  auto event_type = ev->getType();

  if ( event_type == Event::KeyPress )
  {
    KeyPressEvent (static_cast<FKeyEvent*>(ev));
  }
  else if ( event_type == Event::KeyUp )
  {
    onKeyUp (static_cast<FKeyEvent*>(ev));
  }
  else if ( event_type == Event::KeyDown )
  {
    KeyDownEvent (static_cast<FKeyEvent*>(ev));
  }
  else if ( event_type== Event::MouseDown )
  {
    emitCallback("mouse-press");
    onMouseDown (static_cast<FMouseEvent*>(ev));
  }
  else if ( event_type == Event::MouseUp )
  {
    emitCallback("mouse-release");
    onMouseUp (static_cast<FMouseEvent*>(ev));
  }
  else if ( event_type== Event::MouseDoubleClick )
  {
    onMouseDoubleClick (static_cast<FMouseEvent*>(ev));
  }
  else if ( event_type == Event::MouseWheel )
  {
    emitWheelCallback(static_cast<FWheelEvent*>(ev));
    onWheel (static_cast<FWheelEvent*>(ev));
  }
  else if ( event_type == Event::MouseMove )
  {
    emitCallback("mouse-move");
    onMouseMove (static_cast<FMouseEvent*>(ev));
  }
  else if ( event_type == Event::FocusIn )
  {
    emitCallback("focus-in");
    onFocusIn (static_cast<FFocusEvent*>(ev));
  }
  else if ( event_type == Event::FocusOut )
  {
    emitCallback("focus-out");
    onFocusOut (static_cast<FFocusEvent*>(ev));
  }
  else if ( event_type == Event::ChildFocusIn )
  {
    onChildFocusIn (static_cast<FFocusEvent*>(ev));
  }
  else if ( event_type == Event::ChildFocusOut )
  {
    onChildFocusOut (static_cast<FFocusEvent*>(ev));
  }
  else if ( event_type == Event::Accelerator )
  {
    onAccel (static_cast<FAccelEvent*>(ev));
  }
  else if ( event_type == Event::Resize )
  {
    onResize (static_cast<FResizeEvent*>(ev));
  }
  else if ( event_type == Event::Show )
  {
    onShow (static_cast<FShowEvent*>(ev));
  }
  else if ( event_type == Event::Hide )
  {
    onHide (static_cast<FHideEvent*>(ev));
  }
  else if ( event_type == Event::Close )
  {
    onClose (static_cast<FCloseEvent*>(ev));
  }
  else
  {
    return FObject::event(ev);
  }

  return true;
}

//----------------------------------------------------------------------
void FWidget::onKeyPress (FKeyEvent*)
{
  // This event handler can be reimplemented in a subclass
  // to receive key press events for the widget
}

//----------------------------------------------------------------------
void FWidget::onKeyUp (FKeyEvent*)
{
  // This event handler can be reimplemented in a subclass
  // to receive key up events for the widget
}

//----------------------------------------------------------------------
void FWidget::onKeyDown (FKeyEvent*)
{
  // This event handler can be reimplemented in a subclass
  // to receive key down events for the widget
}

//----------------------------------------------------------------------
void FWidget::onMouseDown (FMouseEvent*)
{
  // This event handler can be reimplemented in a subclass
  // to receive mouse down events for the widget
}

//----------------------------------------------------------------------
void FWidget::onMouseUp (FMouseEvent*)
{
  // This event handler can be reimplemented in a subclass
  // to receive mouse up events for the widget
}

//----------------------------------------------------------------------
void FWidget::onMouseDoubleClick (FMouseEvent*)
{
  // This event handler can be reimplemented in a subclass
  // to receive mouse double clicks events for the widget
}

//----------------------------------------------------------------------
void FWidget::onWheel (FWheelEvent*)
{
  // This event handler can be reimplemented in a subclass
  // to receive mouse wheel events for the widget
}

//----------------------------------------------------------------------
void FWidget::onMouseMove (FMouseEvent*)
{
  // This event handler can be reimplemented in a subclass
  // to receive mouse move events for the widget
}

//----------------------------------------------------------------------
void FWidget::onFocusIn (FFocusEvent*)
{
  // This event handler can be reimplemented in a subclass
  // to receive a widget focus event (get focus)
}

//----------------------------------------------------------------------
void FWidget::onFocusOut (FFocusEvent*)
{
  // This event handler can be reimplemented in a subclass
  // to receive a widget focus event (lost focus)
}

//----------------------------------------------------------------------
void FWidget::onChildFocusIn (FFocusEvent*)
{
  // This event handler can be reimplemented in a subclass
  // to receive a child widget focus event (get focus)
}

//----------------------------------------------------------------------
void FWidget::onChildFocusOut (FFocusEvent*)
{
  // This event handler can be reimplemented in a subclass
  // to receive a child widget focus event (lost focus)
}

//----------------------------------------------------------------------
void FWidget::onAccel (FAccelEvent*)
{
  // This event handler can be reimplemented in a subclass to receive
  // an event when an acceleration key is pressed for this widget
}

//----------------------------------------------------------------------
void FWidget::onResize (FResizeEvent* ev)
{
  // The terminal was resized
  internal::var::root_widget->resize();
  internal::var::root_widget->redraw();
  ev->accept();
}

//----------------------------------------------------------------------
void FWidget::onShow (FShowEvent*)
{
  // This event handler can be reimplemented in a subclass
  // to receive a widget show event
}

//----------------------------------------------------------------------
void FWidget::onHide (FHideEvent*)
{
  // This event handler can be reimplemented in a subclass
  // to receive a widget hide event
}

//----------------------------------------------------------------------
void FWidget::onClose (FCloseEvent* ev)
{
  // This event handler can be reimplemented in a subclass
  // to receive a widget close event
  ev->accept();
}


// private methods of FWidget
//----------------------------------------------------------------------
void FWidget::determineDesktopSize()
{
  // Determine width and height of the terminal

  detectTerminalSize();
  auto width = getDesktopWidth();
  auto height = getDesktopHeight();
  wsize.setRect(1, 1, width, height);
  adjust_wsize = wsize;
  woffset.setRect(0, 0, width, height);
  auto r = internal::var::root_widget;
  wclient_offset.setRect(r->padding.left, r->padding.top, width, height);
}

//----------------------------------------------------------------------
void FWidget::initRootWidget()
{
  try
  {
    // Initialize widget lists
    dialog_list        = new FWidgetList();
    always_on_top_list = new FWidgetList();
    close_widget       = new FWidgetList();
  }
  catch (const std::bad_alloc&)
  {
    badAllocOutput ("FWidgetList");
    return;
  }

  // Initialize default widget colors
  // (before terminal detection and internal::var::root_widget is set)
  initColorTheme();

  // Root widget basic initialization
  internal::var::root_widget = this;
  show_root_widget = nullptr;
  redraw_root_widget = nullptr;
  modal_dialog_counter = 0;
  statusbar = nullptr;

  // Determine width and height of the terminal
  determineDesktopSize();
}

//----------------------------------------------------------------------
void FWidget::initWidgetLayout()
{
  initLayout();

  if ( ! hasChildren() )
    return;

  for (auto&& child : getChildren())
  {
    if ( child->isWidget() )
    {
      auto widget = static_cast<FWidget*>(child);
      widget->initWidgetLayout();
    }
  }
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
    if ( adjust_wsize.x1_ref() < 2 )
      adjust_wsize.x1_ref() = 1;
    else
      adjust_wsize.x1_ref()--;

    adjust_wsize.x2_ref()--;
  }

  // move up if not enough space
  while ( getTermY() + int(getHeight()) - padding.bottom > woffset.getY2() + 2 )
  {
    if ( adjust_wsize.y1_ref() < 2 )
      adjust_wsize.y1_ref() = 1;
    else
      adjust_wsize.y1_ref()--;

    adjust_wsize.y2_ref()--;
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
  auto change_focus = [this] (const FKey key)
  {
    if ( isFocusNextKey(key) )
      return focusNextChild();
    else if ( isFocusPrevKey(key) )
      return focusPrevChild();

    return false;
  };

  FWidget* widget(this);

  while ( widget )
  {
    widget->onKeyPress(kev);

    if ( ! kev->isAccepted() && change_focus(kev->key()) )
      return;

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
void FWidget::emitWheelCallback (const FWheelEvent* ev) const
{
  const MouseWheel wheel = ev->getWheel();

  if ( wheel == MouseWheel::Up )
    emitCallback("mouse-wheel-up");
  else if ( wheel == MouseWheel::Down )
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
bool FWidget::changeFocus ( FWidget* follower, FWidget* parent
                          , FocusTypes ft )
{
  FFocusEvent out (Event::FocusOut);
  out.setFocusType(ft);
  FApplication::sendEvent(this, &out);

  FFocusEvent cfo (Event::ChildFocusOut);
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
    FFocusEvent cfi (Event::ChildFocusIn);
    FApplication::sendEvent(parent, &cfi);

    FFocusEvent in (Event::FocusIn);
    in.setFocusType(ft);
    FApplication::sendEvent(follower, &in);

    if ( in.isAccepted() )
    {
      redraw();
      follower->redraw();
    }
  }

  return true;
}

//----------------------------------------------------------------------
void FWidget::draw()
{
  // This method must be reimplemented in a subclass
  // for drawing the widget
}

//----------------------------------------------------------------------
void FWidget::drawWindows() const
{
  // redraw windows
  FChar default_char{};
  default_char.ch[0]        = L' ';
  default_char.fg_color     = FColor::Black;
  default_char.bg_color     = FColor::Black;
  default_char.attr.byte[0] = 0;
  default_char.attr.byte[1] = 0;

  if ( ! getWindowList() || getWindowList()->empty() )
    return;

  for (auto&& window : *getWindowList())
  {
    const auto win = static_cast<FWidget*>(window);

    if ( win->isShown() )
    {
      auto v_win = win->getVWin();
      const int w = v_win->width  + v_win->right_shadow;
      const int h = v_win->height + v_win->bottom_shadow;
      std::fill_n (v_win->data, w * h, default_char);
      win->redraw();
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
inline bool FWidget::isDefaultTheme()
{
  FStringList default_themes
  {
    "default8ColorTheme",
    "default16ColorTheme",
    "default8ColorDarkTheme",
    "default16ColorDarkTheme"
  };

  auto iter = std::find ( default_themes.begin()
                        , default_themes.end()
                        , getColorTheme()->getClassName() );

  if ( iter == default_themes.end() )  // No default theme
    return false;

  return true;
}

//----------------------------------------------------------------------
void FWidget::initColorTheme()
{
  // Sets the default color theme

  if ( getColorTheme().use_count() > 0 && ! isDefaultTheme() )
    return;  // A user theme is in use

  if ( FStartOptions::getInstance().dark_theme )
  {
    if ( FVTerm::getFOutput()->getMaxColor() < 16 )  // for 8 color mode
      setColorTheme<default8ColorDarkTheme>();
    else
      setColorTheme<default16ColorDarkTheme>();
  }
  else  // Default theme
  {
    if ( FVTerm::getFOutput()->getMaxColor() < 16 )  // for 8 color mode
      setColorTheme<default8ColorTheme>();
    else
      setColorTheme<default16ColorTheme>();
  }
}

//----------------------------------------------------------------------
void FWidget::removeQueuedEvent() const
{
  auto app_object = FApplication::getApplicationObject();

  if ( app_object )
    app_object->removeQueuedEvent(this);
}

//----------------------------------------------------------------------
void FWidget::setStatusbarText (bool enable) const
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
void detectTerminalSize()
{
  const auto& r = internal::var::root_widget;
  FVTerm::getFOutput()->detectTerminalSize();
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
