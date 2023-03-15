/***********************************************************************
* fwidget.cpp - Intermediate base class for all widget objects         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2023 Markus Gans                                      *
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
#include "final/fstartoptions.h"
#include "final/fwidgetcolors.h"
#include "final/fwidget.h"
#include "final/menu/fmenubar.h"
#include "final/output/tty/ftermdata.h"
#include "final/util/flog.h"
#include "final/util/fstring.h"
#include "final/widget/fstatusbar.h"
#include "final/widget/fwindow.h"

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
FWidget::FWidgetList* FWidget::close_widget_list{nullptr};
bool                  FWidget::init_terminal{false};
bool                  FWidget::init_desktop{false};
uInt                  FWidget::modal_dialog_counter{};

//----------------------------------------------------------------------
// class FWidget
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FWidget::FWidget (FWidget* parent)
  : FObject{parent}
{
  // init bit field with 0
  memset (&flags.feature, 0, sizeof(flags.feature));
  memset (&flags.visibility, 0, sizeof(flags.visibility));
  memset (&flags.focus, 0, sizeof(flags.focus));
  memset (&flags.shadow, 0, sizeof(flags.shadow));
  memset (&flags.type, 0, sizeof(flags.type));

  flags.feature.active = true;             // Enable widget by default
  flags.visibility.visible = true;         // A widget is visible by default
  flags.focus.focusable = true;            // A widget is focusable by default
  flags.visibility.visible_cursor = true;  // A widget has a visible cursor by default
  setWidgetProperty (true);                // This FObject is a widget

  if ( ! parent )
  {
    if ( internal::var::root_widget )
    {
      setExitMessage("FWidget: No parent defined! "
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

  mapEventFunctions();
  flags.visibility.visible_cursor = false;
  double_flatline_mask.setSize (getWidth(), getHeight());
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
  if ( flags.focus.focus )
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
auto FWidget::getRootWidget() -> FWidget*
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
auto FWidget::getParentWidget() const -> FWidget*
{
  auto p_obj = getParent();

  if ( p_obj && p_obj->isWidget() )
    return static_cast<FWidget*>(p_obj);

  return nullptr;
}

//----------------------------------------------------------------------
auto FWidget::getColorTheme() -> std::shared_ptr<FWidgetColors>&
{
  static const auto& color_theme = std::make_unique<std::shared_ptr<FWidgetColors>>();
  return *color_theme;
}

//----------------------------------------------------------------------
auto FWidget::doubleFlatLine_ref (Side side) -> std::vector<bool>&
{
  auto& mask = double_flatline_mask;

  switch ( side )
  {
    case Side::Top:
      return mask.top;

    case Side::Right:
      return mask.right;

    case Side::Bottom:
      return mask.bottom;

    case Side::Left:
      return mask.left;

    default:
      throw std::invalid_argument{"Invalid side"};
  }
}

//----------------------------------------------------------------------
auto FWidget::getPrintPos() -> FPoint
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
auto FWidget::setVisible (bool enable) -> bool
{
  return (flags.visibility.visible = enable);
}

//----------------------------------------------------------------------
auto FWidget::setEnable (bool enable) -> bool
{
  if ( enable )
    emitCallback("enable");
  else
    emitCallback("disable");

  return (flags.feature.active = enable);
}

//----------------------------------------------------------------------
auto FWidget::setFocus (bool enable, FocusTypes ft) -> bool
{
  // Check if the widget is inactive
  if ( ! isEnabled() )
    return false;

  // The widget already has the focus
  if ( hasFocus() == enable )
    return true;

  // Set widget focus
  if ( enable && ! hasFocus() )
    setFocusOnThisWidget(ft);
  else
    flags.focus.focus = false;

  // Set status bar text for widget focus
  setStatusbarText (enable);
  return enable;
}

//----------------------------------------------------------------------
void FWidget::resetColors()
{
  if ( ! hasChildren() )
    return;

  for (auto* child : getChildren())
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

  if ( ! isWindowWidget() )
    x = std::max(x, 1);

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

  if ( ! isWindowWidget() )
    y = std::max(y, 1);

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
    pos.setX(std::max(pos.getX(), 1));
    pos.setY(std::max(pos.getY(), 1));
  }

  wsize.setPos(pos);
  adjust_wsize.setPos(pos);

  if ( adjust )
    adjustSize();
}

//----------------------------------------------------------------------
void FWidget::setWidth (std::size_t width, bool adjust)
{
  width = std::max ( std::min (width, size_hints.max_width)
                   , size_hints.min_width );

  if ( getWidth() == width && wsize.getWidth() == width  )
    return;

  // A width can never be narrower than 1 character
  width = std::max(width, std::size_t(1));
  // Set the width
  wsize.setWidth(width);
  adjust_wsize.setWidth(width);

  if ( adjust )
    adjustSize();

  double_flatline_mask.setWidth (getWidth());
}

//----------------------------------------------------------------------
void FWidget::setHeight (std::size_t height, bool adjust)
{
  height = std::max ( std::min (height, size_hints.max_height)
                    , size_hints.min_height );

  if ( getHeight() == height && wsize.getHeight() == height )
    return;

  // A height can never be narrower than 1 character
  height = std::max(height, std::size_t(1));
  // Set the height
  wsize.setHeight(height);
  adjust_wsize.setHeight(height);

  if ( adjust )
    adjustSize();

  double_flatline_mask.setHeight (getHeight());
}

//----------------------------------------------------------------------
void FWidget::setSize (const FSize& size, bool adjust)
{
  std::size_t width = std::max ( std::min (size.getWidth(), size_hints.max_width)
                               , size_hints.min_width );
  std::size_t height = std::max ( std::min (size.getHeight(), size_hints.max_height)
                                , size_hints.min_height );

  if ( getWidth() == width && wsize.getWidth() == width
    && getHeight() == height && wsize.getHeight() == height )
    return;

  // A width or a height can never be narrower than 1 character
  wsize.setSize ( std::max(width, std::size_t(1))
                , std::max(height, std::size_t(1)) );
  adjust_wsize = wsize;
  double_flatline_mask.setSize (getWidth(), getHeight());

  if ( adjust )
    adjustSize();
}

//----------------------------------------------------------------------
void FWidget::setTopPadding (int top, bool adjust)
{
  if ( padding.top == top )
    return;

  padding.top = top;

  if ( ! adjust )
    return;

  if ( isRootWidget() )
  {
    auto r = internal::var::root_widget;
    r->wclient_offset.setY1 (r->padding.top);
    adjustSizeGlobal();
  }
  else
    adjustSize();
}

//----------------------------------------------------------------------
void FWidget::setLeftPadding (int left, bool adjust)
{
  if ( padding.left == left )
    return;

  padding.left = left;

  if ( ! adjust )
    return;

  if ( isRootWidget() )
  {
    auto r = internal::var::root_widget;
    r->wclient_offset.setX1 (r->padding.left);
    adjustSizeGlobal();
  }
  else
    adjustSize();
}

//----------------------------------------------------------------------
void FWidget::setBottomPadding (int bottom, bool adjust)
{
  if ( padding.bottom == bottom )
    return;

  padding.bottom = bottom;

  if ( ! adjust )
    return;

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

//----------------------------------------------------------------------
void FWidget::setRightPadding (int right, bool adjust)
{
  if ( padding.right == right )
    return;

  padding.right = right;

  if ( ! adjust )
    return;

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

//----------------------------------------------------------------------
void FWidget::setTerminalSize (const FSize& size) const
{
  // Set terminal size to width x height

  if ( ! FVTerm::getFOutput()->allowsTerminalSizeManipulation() )
    return;

  internal::var::root_widget->wsize.setRect(FPoint{1, 1}, size);
  internal::var::root_widget->adjust_wsize = internal::var::root_widget->wsize;
  FVTerm::getFOutput()->setTerminalSize(size);
  detectTerminalSize();
}

//----------------------------------------------------------------------
void FWidget::setGeometry (const FPoint& p, const FSize& s, bool adjust)
{
  // Sets the geometry of the widget relative to its parent

  const int x = p.getX();
  const int y = p.getY();
  std::size_t w = std::max ( std::min (s.getWidth(), size_hints.max_width)
                           , size_hints.min_width );
  std::size_t h = std::max ( std::min (s.getHeight(), size_hints.max_height)
                           , size_hints.min_height );

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

  wsize.setSize ( std::max(w, std::size_t(1u))
                , std::max(h, std::size_t(1u)) );
  adjust_wsize = wsize;
  const int term_x = getTermX();
  const int term_y = getTermY();

  wclient_offset.setCoordinates ( term_x - 1 + padding.left
                                , term_y - 1 + padding.top
                                , term_x - 2 + int(getWidth()) - padding.right
                                , term_y - 2 + int(getHeight()) - padding.bottom );

  double_flatline_mask.setSize (getWidth(), getHeight());

  if ( adjust )
    adjustSize();
}

//----------------------------------------------------------------------
auto FWidget::setCursorPos (const FPoint& pos) -> bool
{
  // sets the input cursor position

  widget_cursor_position.setPoint(pos);

  if ( ! flags.focus.focus
    || flags.visibility.hidden
    || isWindowWidget()
    || ! FWindow::getWindowWidget(this) )
    return false;

  const auto& area = getPrintArea();

  if ( ! area->hasOwner() )
    return false;

  const auto& area_owner = area->getOwner<FWidget*>();
  int woffsetX = getTermX() - area_owner->getTermX();
  int woffsetY = getTermY() - area_owner->getTermY();

  if ( isChildPrintArea() )
  {
    woffsetX += (1 - area_owner->getLeftPadding());
    woffsetY += (1 - area_owner->getTopPadding());
  }

  bool visible = ! isCursorHideable() || flags.visibility.visible_cursor;
  setAreaCursor ( { woffsetX + pos.getX()
                  , woffsetY + pos.getY() }
                , visible
                , area );
  return true;
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
  auto& mask = double_flatline_mask;

  switch ( side )
  {
    case Side::Top:
      std::fill(mask.top.begin(), mask.top.end(), bit);
      break;

    case Side::Right:
      std::fill(mask.right.begin(), mask.right.end(), bit);
      break;

    case Side::Bottom:
      std::fill(mask.bottom.begin(), mask.bottom.end(), bit);
      break;

    case Side::Left:
      std::fill(mask.left.begin(), mask.left.end(), bit);
      break;

    default:
      throw std::invalid_argument{"Invalid side"};
  }
}

//----------------------------------------------------------------------
void FWidget::setDoubleFlatLine (Side side, int pos, bool bit)
{
  if ( pos < 1 )
    return;

  const auto index = uLong(pos - 1);
  auto& mask = double_flatline_mask;

  auto& side_line = [&mask, side] () -> std::vector<bool>&
  {
    switch ( side )
    {
      case Side::Top:
        return mask.top;

      case Side::Right:
        return mask.right;

      case Side::Bottom:
        return mask.bottom;

      case Side::Left:
        return mask.left;

      default:
        throw std::invalid_argument{"Invalid side"};
    }
  }();

  if ( index < side_line.size() )
    side_line[index] = bit;
}

//----------------------------------------------------------------------
auto FWidget::childWidgetAt (const FPoint& pos) & -> FWidget*
{
  if ( ! hasChildren() )
    return nullptr;

  for (auto* child : getChildren())
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
auto FWidget::numOfFocusableChildren() & -> int
{
  if ( ! hasChildren() )
    return 0;

  int num{0};

  for (auto* child : getChildren())
  {
    if ( child->isWidget() )
    {
      const auto& widget = static_cast<FWidget*>(child);

      if ( widget->isEnabled()
        && widget->isShown()
        && widget->acceptFocus()
        && ! widget->isWindowWidget() )
        num++;
    }
  }

  return num;
}

//----------------------------------------------------------------------
auto FWidget::close() -> bool
{
  // Sends a close event and quits the application on acceptance

  FCloseEvent ev(Event::Close);
  FApplication::sendEvent(this, &ev);

  if ( ! ev.isAccepted() )
    return false;

  if ( this == getMainWidget() )
    quit();
  else
  {
    hide();

    if ( ! flags.visibility.modal && ! isInFWidgetList(close_widget_list, this) )
      close_widget_list->push_back(this);
  }

  return true;
}

//----------------------------------------------------------------------
void FWidget::addAccelerator (FKey key, FWidget* obj) &
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
void FWidget::delAccelerator (FWidget* obj) &
{
  // Deletes all accelerators of the given widget

  auto widget = static_cast<FWidget*>(FWindow::getWindowWidget(this));

  if ( ! widget || widget == statusbar || widget == menubar )
    widget = getRootWidget();

  if ( ! widget || widget->accelerator_list.empty() )
    return;

  auto iter = widget->accelerator_list.cbegin();

  while ( iter != widget->accelerator_list.cend() )
  {
    if ( iter->object == obj )
      iter = widget->accelerator_list.erase(iter);
    else
      ++iter;
  }
}

//----------------------------------------------------------------------
void FWidget::flushChanges()
{
  // This method can be reimplemented in a subclass
  // to handle pending changes
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
    // The screen content is now unknown
    auto foutput_ptr = FVTerm::getFOutput();
    foutput_ptr->clearTerminalState();
    foutput_ptr->clearTerminalAttributes();
    // Determine the new terminal size
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
  double_flatline_mask.setSize (getWidth(), getHeight());
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
  flags.visibility.hidden = false;
  flags.visibility.shown = true;

  if ( hasChildren() )
  {
    for (auto* child : getChildren())
    {
      auto child_widget = static_cast<FWidget*>(child);

      if ( child->isWidget() && ! child_widget->flags.visibility.hidden )
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

  flags.visibility.hidden = true;

  if ( ! isVisible() )
    return;

  flags.visibility.shown = false;

  if ( flags.visibility.visible_cursor && FWidget::getFocusWidget() == this )
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

//----------------------------------------------------------------------
auto FWidget::focusNextChild() -> bool
{
  // Focusing the next widget

  if ( isDialogWidget() || ! hasParent() )
    return false;

  const auto& parent = getParentWidget();

  if ( ! parent
    || ! parent->hasChildren()
    || parent->numOfFocusableChildren() < 1 )
    return false;

  FWidget* next = nullptr;
  constexpr auto ft = FocusTypes::NextWidget;
  auto iter = searchForwardForWidget(parent, this);
  auto iter_of_this_widget = iter;

  do  // Search the next focusable widget
  {
    ++iter;

    if ( iter == parent->cend() )
      iter = parent->cbegin();

    if ( (*iter)->isWidget() )
      next = static_cast<FWidget*>(*iter);
  } while ( iter != iter_of_this_widget && canReceiveFocus(next) );

  // Focus exception handling
  if ( iter == iter_of_this_widget && next && next->hasFocus() )
    return sendFailAtChildFocusEvent (parent, ft);

  // Change focus to the next widget and return true if successful
  return next
       ? next->setFocus (true, ft)
       : false;
}

//----------------------------------------------------------------------
auto FWidget::focusPrevChild() -> bool
{
  // Focusing the previous widget

  if ( isDialogWidget() || ! hasParent() )
    return false;

  const auto& parent = getParentWidget();

  if ( ! parent
    || ! parent->hasChildren()
    || parent->numOfFocusableChildren() < 1 )
    return false;

  FWidget* prev{nullptr};
  constexpr auto ft = FocusTypes::PreviousWidget;
  auto iter = searchBackwardsForWidget(parent, this);
  auto iter_of_this_widget = iter;

  do  // Search the previous focusable widget
  {
    ++iter;

    if ( iter == parent->crend() )
      iter = parent->crbegin();

    if ( (*iter)->isWidget() )
      prev = static_cast<FWidget*>(*iter);
  } while ( iter != iter_of_this_widget && canReceiveFocus(prev) );

  if ( iter == iter_of_this_widget && prev && prev->hasFocus() )
    return sendFailAtChildFocusEvent (parent, ft);  // Send event to the parent widget

  // Change focus to the previous widget and return true if successful
  return prev
       ? prev->setFocus (true, ft)
       : false;
}

//----------------------------------------------------------------------
auto FWidget::focusFirstChild() & -> bool
{
  // Focusing the first child widget

  if ( ! hasChildren() )
    return false;

  for (auto* item : getChildren())
  {
    if ( ! item->isWidget() )  // Skip non-widget elements
      continue;

    auto first = static_cast<FWidget*>(item);

    if ( first->isEnabled()
      && first->acceptFocus()
      && ! first->isMenuWidget() )
    {
      if ( first->numOfChildren() >= 1 && first->focusFirstChild() )
        return true;

      // Change focus to the first widget and return true if successful
      return first->setFocus (true, FocusTypes::NextWidget);
    }
  }

  return false;
}

//----------------------------------------------------------------------
auto FWidget::focusLastChild() & -> bool
{
  // Focusing the last child widget

  if ( ! hasChildren() )
    return false;

  for ( auto iter = FObject::crbegin();
        iter != FObject::crend();
        ++iter )
  {
    if ( ! (*iter)->isWidget() )  // Skip non-widget elements
      continue;

    auto last = static_cast<FWidget*>(*iter);

    if ( last->isEnabled()
      && last->acceptFocus()
      && ! last->isMenuWidget() )
    {
      if ( last->numOfChildren() >= 1 && last->focusLastChild() )
        return true;

      // Change focus to the last widget and return true if successful
      return last->setFocus (true, FocusTypes::PreviousWidget);
    }
  }

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
auto FWidget::getPrintArea() -> FVTerm::FTermArea*
{
  // returns the print area of this object

  if ( getCurrentPrintArea() )
    return getCurrentPrintArea();

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

  if ( obj->getChildPrintArea() )
  {
    setPrintArea (obj->getChildPrintArea());
    return getCurrentPrintArea();
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
auto FWidget::isChildPrintArea() const -> bool
{
  const auto& p_obj = static_cast<FWidget*>(getParent());
  return ( p_obj
        && p_obj->getChildPrintArea()
        && p_obj->getChildPrintArea() == getCurrentPrintArea() );
}

//----------------------------------------------------------------------
void FWidget::setStatusBar (FStatusBar* sbar)
{
  if ( ! sbar || statusbar == sbar )
    return;

  delete statusbar;
  statusbar = sbar;
}

//----------------------------------------------------------------------
void FWidget::setMenuBar (FMenuBar* mbar)
{
  if ( ! mbar || menubar == mbar )
    return;

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
      if ( flags.feature.ignore_padding && ! isDialogWidget() )
        setTermOffset();
      else
        woffset = internal::var::root_widget->wclient_offset;
    }
    else if ( flags.feature.ignore_padding && p )
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
    for (auto* child : getChildren())
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

  const auto& parent_widget = getParentWidget();

  if ( parent_widget )
  {
    FColor fg = parent_widget->getForegroundColor();
    FColor bg = parent_widget->getBackgroundColor();
    setColor (fg, bg);
  }
  else
  {
    auto color_theme = getColorTheme();
    FColor fg = color_theme->dialog_fg;
    FColor bg = color_theme->dialog_bg;
    setColor (fg, bg);
  }

  if ( size.getWidth() == 0 )
    return;

  for (auto y{0}; y < int(size.getHeight()); y++)
  {
    print() << FPoint{1, 1 + y} << FString{size.getWidth(), L' '};
  }

  flush();
}

//----------------------------------------------------------------------
auto FWidget::event (FEvent* ev) -> bool
{
  auto iter = event_map.find(ev->getType());

  // If the event type is not found,
  // call the base class event handler
  if ( iter == event_map.end() )
    return FObject::event(ev);

  iter->second(ev);
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

  redraw();  // Redraw widget when it gets the focus
}

//----------------------------------------------------------------------
void FWidget::onFocusOut (FFocusEvent*)
{
  // This event handler can be reimplemented in a subclass
  // to receive a widget focus event (lost focus)

  redraw();  // Redraw widget when focus is lost
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
void FWidget::onFailAtChildFocus (FFocusEvent*)
{
  // This event handler can be reimplemented in a subclass
  // to receive a child widget focus event (focus failure)
}

//----------------------------------------------------------------------
void FWidget::onTermFocusIn (FFocusEvent*)
{
  // This event handler can be reimplemented in a subclass
  // to receive a terminal focus-in event (terminal get focus)
}

//----------------------------------------------------------------------
void FWidget::onTermFocusOut (FFocusEvent*)
{
  // This event handler can be reimplemented in a subclass
  // to receive a terminal focus-out event (terminal lost focus)
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
inline void FWidget::mapEventFunctions()
{
  mapKeyEvents();
  mapMouseEvents();
  mapFocusEvents();
  mapWidgetEvents();
}

//----------------------------------------------------------------------
inline void FWidget::mapKeyEvents()
{
  // Key events

  event_map.insert (
  {
    { Event::KeyPress,
      [this] (FEvent* ev)
      {
        KeyPressEvent (static_cast<FKeyEvent*>(ev));
      }
    },
    { Event::KeyUp,
      [this] (FEvent* ev)
      {
        onKeyUp (static_cast<FKeyEvent*>(ev));
      }
    },
    { Event::KeyDown,
      [this] (FEvent* ev)
      {
        KeyDownEvent(static_cast<FKeyEvent*>(ev));
      }
    }
  } );
}

//----------------------------------------------------------------------
inline void FWidget::mapMouseEvents()
{
  // Mouse events

  event_map.insert (
  {
    { Event::MouseDown,
      [this] (FEvent* ev)
      {
        emitCallback("mouse-press");
        onMouseDown (static_cast<FMouseEvent*>(ev));
      }
    },
    { Event::MouseUp,
      [this] (FEvent* ev)
      {
        emitCallback("mouse-release");
        onMouseUp (static_cast<FMouseEvent*>(ev));
      }
    },
    { Event::MouseDoubleClick,
      [this] (FEvent* ev)
      {
        onMouseDoubleClick (static_cast<FMouseEvent*>(ev));
      }
    },
    { Event::MouseWheel,
      [this] (FEvent* ev)
      {
        emitWheelCallback(static_cast<FWheelEvent*>(ev));
        onWheel (static_cast<FWheelEvent*>(ev));
      }
    },
    { Event::MouseMove,
      [this] (FEvent* ev)
      {
        emitCallback("mouse-move");
        onMouseMove (static_cast<FMouseEvent*>(ev));
      }
    }
  } );
}

//----------------------------------------------------------------------
inline void FWidget::mapFocusEvents()
{
  // Focus events

  event_map.insert (
  {
    { Event::FocusIn,
      [this] (FEvent* ev)
      {
        emitCallback("focus-in");
        onFocusIn (static_cast<FFocusEvent*>(ev));
      }
    },
    { Event::FocusOut,
      [this] (FEvent* ev)
      {
        emitCallback("focus-out");
        onFocusOut (static_cast<FFocusEvent*>(ev));
      }
    },
    { Event::ChildFocusIn,
      [this] (FEvent* ev)
      {
        onChildFocusIn (static_cast<FFocusEvent*>(ev));
      }
    },
    { Event::ChildFocusOut,
      [this] (FEvent* ev)
      {
        onChildFocusOut (static_cast<FFocusEvent*>(ev));
      }
    },
    { Event::FailAtChildFocus,
      [this] (FEvent* ev)
      {
        onFailAtChildFocus (static_cast<FFocusEvent*>(ev));
      }
    },
    { Event::TerminalFocusOut,
      [this] (FEvent* ev)
      {
        onTermFocusOut (static_cast<FFocusEvent*>(ev));
      }
    },
    { Event::TerminalFocusIn,
      [this] (FEvent* ev)
      {
        onTermFocusIn (static_cast<FFocusEvent*>(ev));
      }
    }
  } );
}

//----------------------------------------------------------------------
inline void FWidget::mapWidgetEvents()
{
  // Widget events

  event_map.insert (
  {
    { Event::Accelerator,
      [this] (FEvent* ev)
      {
        onAccel (static_cast<FAccelEvent*>(ev));
      }
    },
    { Event::Resize,
      [this] (FEvent* ev)
      {
        onResize (static_cast<FResizeEvent*>(ev));
      }
    },
    { Event::Show,
      [this] (FEvent* ev)
      {
        onShow (static_cast<FShowEvent*>(ev));
      }
    },
    { Event::Hide,
      [this] (FEvent* ev)
      {
        onHide (static_cast<FHideEvent*>(ev));
      }
    },
    { Event::Close,
      [this] (FEvent* ev)
      {
        onClose (static_cast<FCloseEvent*>(ev));
      }
    }
  } );
}

//----------------------------------------------------------------------
void FWidget::initRootWidget()
{
  try
  {
    // Initialize widget lists
    dialog_list        = new FWidgetList();
    always_on_top_list = new FWidgetList();
    close_widget_list  = new FWidgetList();
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

  for (auto* child : getChildren())
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
  delete close_widget_list;
  close_widget_list = nullptr;
  delete dialog_list;
  dialog_list = nullptr;
  delete always_on_top_list;
  always_on_top_list = nullptr;
  internal::var::root_widget = nullptr;
}

//----------------------------------------------------------------------
inline void FWidget::moveLeftIfNotEnoughSpace()
{
  int diff = getTermX() + int(getWidth()) - padding.right - (woffset.getX2() + 2);

  if ( diff <= 0 )
    return;

  adjust_wsize.x1_ref() = std::max(adjust_wsize.getX1() - diff, 1);
  adjust_wsize.x2_ref() -= diff;
}

//----------------------------------------------------------------------
inline void FWidget::moveUpIfNotEnoughSpace()
{
  int diff = getTermY() + int(getHeight()) - padding.bottom - (woffset.getY2() + 2);

  if ( diff <= 0 )
    return;

  adjust_wsize.y1_ref() = std::max(adjust_wsize.getY1() - diff, 1);
  adjust_wsize.y2_ref() -= diff;
}

//----------------------------------------------------------------------
inline void FWidget::reduceWidthIfNotEnoughSpace()
{
  int diff = woffset.getX1() + int(getWidth()) - 1 - woffset.getX2();
  adjust_wsize.x2_ref() = std::min ( adjust_wsize.getX2() - diff
                                   , adjust_wsize.getX2() );

  // Handle the minimum hint value
  adjust_wsize.x2_ref() = adjust_wsize.getX1()
                        + int(std::max(getWidth(), size_hints.min_width)) - 1;

  // The minimum with is 1
  adjust_wsize.x2_ref() = adjust_wsize.getX1() + std::max(int(getWidth()), 1) - 1;
}

//----------------------------------------------------------------------
inline void FWidget::reduceHeightIfNotEnoughSpace()
{
  int diff = woffset.getY1() + int(getHeight()) - 1 - woffset.getY2();
  adjust_wsize.y2_ref() = std::min ( adjust_wsize.getY2() - diff
                                   , adjust_wsize.getY2() );

  // Handle the minimum hint value
  adjust_wsize.y2_ref() = adjust_wsize.getY1()
                        + int(std::max(getHeight(), size_hints.min_height)) - 1;

  // The minimum with is 1
  adjust_wsize.y2_ref() = adjust_wsize.getY1() + std::max(int(getHeight()), 1) - 1;
}

//----------------------------------------------------------------------
inline void FWidget::insufficientSpaceAdjust()
{
  // Move and shrink widget if there is not enough space available

  if ( isWindowWidget() )
    return;

  moveLeftIfNotEnoughSpace();      // move left if not enough space
  moveUpIfNotEnoughSpace();        // move up if not enough space
  reduceWidthIfNotEnoughSpace();   // reduce the width if not enough space
  reduceHeightIfNotEnoughSpace();  // reduce the height if not enough space
}

//----------------------------------------------------------------------
void FWidget::KeyPressEvent (FKeyEvent* kev)
{
  auto change_focus = [this] (const auto& key)
  {
    if ( isFocusNextKey(key) )
      return focusNextChild();

    if ( isFocusPrevKey(key) )
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
      || widget->getFlags().visibility.modal )
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
  const auto& wheel = ev->getWheel();

  if ( wheel == MouseWheel::Up )
    emitCallback("mouse-wheel-up");
  else if ( wheel == MouseWheel::Down )
    emitCallback("mouse-wheel-down");
}

//----------------------------------------------------------------------
void FWidget::setWindowFocus (bool enable)
{
  // Set the focus of the window that contains this FWidget object

  if ( ! enable )
    return;

  // Get the FWindow object that contains this FWidget object
  auto window = FWindow::getWindowWidget(this);

  if ( ! window )
    return;

  if ( ! window->isWindowActive() )
  {
    // Raise the window, set it active and redraw it
    bool has_raised = window->raiseWindow();
    FWindow::setActiveWindow(window);

    if ( has_raised && window->isVisible() && window->isShown() )
      window->redraw();
  }

  // set focus widget of this window
  window->setWindowFocusWidget(this);
}

//----------------------------------------------------------------------
inline auto FWidget::searchForwardForWidget ( const FWidget* parent
                                            , const FWidget* widget ) const -> FObjectList::const_iterator
{
  auto iter = parent->cbegin();
  const auto last = parent->cend();

  while ( iter != last )  // Search forward for this widget
  {
    if ( ! (*iter)->isWidget() )  // Skip non-widget elements
    {
      ++iter;
      continue;
    }

    if ( static_cast<FWidget*>(*iter) == widget )
      break;  // Stop search when we reach this widget

    ++iter;
  }

  return iter;
}

//----------------------------------------------------------------------
inline auto FWidget::searchBackwardsForWidget ( const FWidget* parent
                                              , const FWidget* widget ) const -> FObjectList::const_reverse_iterator
{
  auto iter = parent->crbegin();
  const auto first = parent->crend();

  while ( iter != first )  // Search backwards for this widget
  {
    if ( ! (*iter)->isWidget() )  // Skip non-widget elements
    {
      ++iter;
      continue;
    }

    if ( static_cast<FWidget*>(*iter) == widget )
      break;  // Stop search when we reach this widget

    ++iter;
  }

  return iter;
}

//----------------------------------------------------------------------
inline auto FWidget::canReceiveFocus (const FWidget* widget) const -> bool
{
  return ! widget
      || ! widget->isEnabled()
      || ! widget->acceptFocus()
      || ! widget->isShown()
      || widget->isWindowWidget();
}

//----------------------------------------------------------------------
inline void FWidget::setFocusOnThisWidget (FocusTypes ft)
{
  auto last_focus = FWidget::getFocusWidget();

  if ( last_focus )
  {
    // Unfocus the currently focused widget
    last_focus->unsetFocus();

    // Send events for this widget
    if ( ! sendFocusOutEvent(last_focus, ft) )
      return;
  }

  // Use this widget for global focus
  FWidget::setFocusWidget(this);
  flags.focus.focus = true;

  // Activates the window with the focused widget
  setWindowFocus();

  // Send events for the follow widget
  sendFocusInEvent(this, ft);
}

//----------------------------------------------------------------------
inline auto FWidget::sendFailAtChildFocusEvent ( FWidget* widget
                                               , FocusTypes ft ) const -> bool
{
  if ( ! widget )
    return false;

  // Send event to the given widget
  FFocusEvent ncfc (Event::FailAtChildFocus);
  ncfc.setFocusType(ft);
  FApplication::sendEvent(widget, &ncfc);
  return ncfc.isAccepted();
}

//----------------------------------------------------------------------
inline auto FWidget::sendFocusOutEvent ( FWidget* widget
                                       , FocusTypes ft ) const -> bool
{
  if ( ! widget )
    return false;

  // Send event to the focus widget
  FFocusEvent f_out (Event::FocusOut);  // isAccepted() is default
  f_out.setFocusType(ft);
  FApplication::sendEvent(widget, &f_out);

  const auto& parent_widget = getParentWidget();

  if ( parent_widget )
  {
    // Send event to parent focus
    FFocusEvent cf_out (Event::ChildFocusOut);
    cf_out.setFocusType(ft);
    cf_out.ignore();
    FApplication::sendEvent(parent_widget, &cf_out);

    if ( cf_out.isAccepted() )
      f_out.ignore();
  }

  return f_out.isAccepted();
}

//----------------------------------------------------------------------
inline auto FWidget::sendFocusInEvent ( FWidget* widget
                                      , FocusTypes ft ) const -> bool
{
  if ( ! widget )
    return false;

  const auto& parent_widget = widget->getParentWidget();

  if ( parent_widget )
  {
    // Send event to the parent of the follow widget
    FFocusEvent cf_in (Event::ChildFocusIn);
    cf_in.setFocusType(ft);
    FApplication::sendEvent(parent_widget, &cf_in);
  }

  // Send event to the follow widget
  FFocusEvent f_in (Event::FocusIn);
  f_in.setFocusType(ft);
  FApplication::sendEvent(widget, &f_in);

  return f_in.isAccepted();
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
  default_char.fg_color     = FColor::Default;
  default_char.bg_color     = FColor::Default;
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
      std::fill_n (v_win->data.begin(), w * h, default_char);
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

  for (auto* child : getChildren())
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
inline auto FWidget::isDefaultTheme() -> bool
{
  FStringList default_themes
  {
    "default8ColorTheme",
    "default16ColorTheme",
    "default8ColorDarkTheme",
    "default16ColorDarkTheme"
  };

  auto iter = std::find ( default_themes.cbegin()
                        , default_themes.cend()
                        , getColorTheme()->getClassName() );
  return iter != default_themes.cend();  // Default theme found
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

  getStatusBar()->drawMessage();
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
