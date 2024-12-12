/***********************************************************************
* fscrollview.cpp - Widget FScrollView (a scrolling area with          *
*                   on-demand scroll bars)                             *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2017-2024 Markus Gans                                      *
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
#include <memory>

#include "final/fevent.h"
#include "final/fwidgetcolors.h"
#include "final/vterm/fcolorpair.h"
#include "final/widget/fscrollview.h"
#include "final/widget/fstatusbar.h"
#include "final/widget/fwindow.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FScrollView
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FScrollView::FScrollView (FWidget* parent)
  : FWidget{parent}
{
  init();
}

//----------------------------------------------------------------------
FScrollView::~FScrollView()  // destructor
{
  setChildPrintArea (viewport.get());
}


// public methods of FScrollView
//----------------------------------------------------------------------
void FScrollView::setScrollWidth (std::size_t width)
{
  width = std::max(width, getViewportWidth());

  if ( getScrollWidth() == width )
    return;

  if ( viewport )
  {
    scroll_geometry.setWidth (width);
    resizeArea (scroll_geometry, viewport.get());
    setColor();
    FScrollView::clearArea();
    addLocalPreprocessingHandler();
    setChildPrintArea (viewport.get());
  }

  hbar->setMaximum (int(width - getViewportWidth()));
  hbar->setPageSize (int(width), int(getViewportWidth()));
  hbar->calculateSliderValues();

  if ( isShown() )
    setHorizontalScrollBarVisibility();
}

//----------------------------------------------------------------------
void FScrollView::setScrollHeight (std::size_t height)
{
  height = std::max(height, getViewportHeight());

  if ( getScrollHeight() == height )
    return;

  if ( viewport )
  {
    scroll_geometry.setHeight (height);
    resizeArea (scroll_geometry, viewport.get());
    setColor();
    clearArea();
    addLocalPreprocessingHandler();
    setChildPrintArea (viewport.get());
  }

  vbar->setMaximum (int(height - getViewportHeight()));
  vbar->setPageSize (int(height), int(getViewportHeight()));
  vbar->calculateSliderValues();

  if ( isShown() )
    setVerticalScrollBarVisibility();
}

//----------------------------------------------------------------------
void FScrollView::setScrollSize (const FSize& size)
{
  std::size_t width = std::max(size.getWidth(), getViewportWidth());
  std::size_t height = std::max(size.getHeight(), getViewportHeight());

  if ( getScrollWidth() == width && getScrollHeight() == height )
    return;

  if ( viewport )
  {
    scroll_geometry.setSize (width, height);
    resizeArea (scroll_geometry, viewport.get());
    setColor();
    FScrollView::clearArea();
    addLocalPreprocessingHandler();
    setChildPrintArea (viewport.get());
  }

  const auto xoffset_end = int(getScrollWidth() - getViewportWidth());
  const auto yoffset_end = int(getScrollHeight() - getViewportHeight());
  setTopPadding (1 - getScrollY());
  setLeftPadding (1 - getScrollX());
  setBottomPadding (1 - (yoffset_end - getScrollY()));
  setRightPadding (1 - (xoffset_end - getScrollX()) + int(nf_offset));

  hbar->setMaximum (int(width - getViewportWidth()));
  hbar->setPageSize (int(width), int(getViewportWidth()));
  hbar->calculateSliderValues();

  vbar->setMaximum (int(height - getViewportHeight()));
  vbar->setPageSize (int(height), int(getViewportHeight()));
  vbar->calculateSliderValues();

  if ( isShown() )
  {
    setHorizontalScrollBarVisibility();
    setVerticalScrollBarVisibility();
  }
}

//----------------------------------------------------------------------
void FScrollView::setX (int x, bool adjust)
{
  FWidget::setX (x, adjust);

  if ( adjust )
    return;

  scroll_geometry.setX (getTermX() + getLeftPadding() - 1);

  if ( viewport )
  {
    viewport->position.x = scroll_geometry.getX();
    viewport->position.y = scroll_geometry.getY();
  }
}

//----------------------------------------------------------------------
void FScrollView::setY (int y, bool adjust)
{
  FWidget::setY (y, adjust);

  if ( adjust )
    return;

  scroll_geometry.setY (getTermY() + getTopPadding() - 1);

  if ( viewport )
  {
    viewport->position.x = scroll_geometry.getX();
    viewport->position.y = scroll_geometry.getY();
  }
}

//----------------------------------------------------------------------
void FScrollView::setPos (const FPoint& p, bool adjust)
{
  FWidget::setPos (p, adjust);
  scroll_geometry.setPos ( getTermX() + getLeftPadding() - 1
                         , getTermY() + getTopPadding() - 1 );

  if ( adjust || ! viewport )
    return;

  viewport->position.x = scroll_geometry.getX();
  viewport->position.y = scroll_geometry.getY();
}

//----------------------------------------------------------------------
void FScrollView::setWidth (std::size_t w, bool adjust)
{
  if ( w <= vertical_border_spacing + nf_offset )
    return;

  FWidget::setWidth (w, adjust);
  viewport_geometry.setWidth(w - vertical_border_spacing - nf_offset);
  calculateScrollbarPos();

  if ( getScrollWidth() < getViewportWidth() )
    setScrollWidth (getViewportWidth());

  if ( ! viewport )
    return;

  // Insufficient space scrolling
  viewport_geometry.x1_ref() = \
      std::min ( int(getScrollWidth() - getViewportWidth())
               , viewport_geometry.getX1() );
}

//----------------------------------------------------------------------
void FScrollView::setHeight (std::size_t h, bool adjust)
{
  if ( h <= horizontal_border_spacing )
    return;

  FWidget::setHeight (h, adjust);
  viewport_geometry.setHeight(h - horizontal_border_spacing);
  calculateScrollbarPos();

  if ( getScrollHeight() < getViewportHeight() )
    setScrollHeight (getViewportHeight());

  if ( ! viewport )
    return;

  // Insufficient space scrolling
  viewport_geometry.y1_ref() = \
      std::min ( int(getScrollHeight() - getViewportHeight())
               , viewport_geometry.getY1() );
}

//----------------------------------------------------------------------
void FScrollView::setSize (const FSize& size, bool adjust)
{
  // Set the scroll view size

   if ( size.getWidth() <= vertical_border_spacing + nf_offset
     || size.getHeight() <= horizontal_border_spacing )
     return;

  FWidget::setSize (size, adjust);
  changeSize (size, adjust);
}

//----------------------------------------------------------------------
void FScrollView::setGeometry ( const FPoint& pos, const FSize& size
                              , bool adjust )
{
  // Set the scroll view geometry

  FWidget::setGeometry (pos, size, adjust);
  scroll_geometry.setPos ( getTermX() + getLeftPadding() - 1
                         , getTermY() + getTopPadding() - 1 );
  changeSize (size, adjust);
}

//----------------------------------------------------------------------
auto FScrollView::setCursorPos (const FPoint& p) -> bool
{
  return FWidget::setCursorPos ({ p.getX() + getLeftPadding()
                                , p.getY() + getTopPadding() });
}

//----------------------------------------------------------------------
void FScrollView::setPrintPos (const FPoint& p)
{
  FWidget::setPrintPos (FPoint { p.getX() + getLeftPadding()
                               , p.getY() + getTopPadding() });
}

//----------------------------------------------------------------------
void FScrollView::setText (const FString& txt)
{
  text.setString(txt);

  if ( isEnabled() )
  {
    delAccelerator();
    setHotkeyAccelerator();
  }
}

//----------------------------------------------------------------------
void FScrollView::setViewportPrint (bool enable)
{
  use_own_print_area = ! enable;
}

//----------------------------------------------------------------------
void FScrollView::resetColors()
{
  const auto& wc_dialog = getColorTheme()->dialog;
  FWidget::setForegroundColor (wc_dialog.fg);
  FWidget::setBackgroundColor (wc_dialog.bg);
  FWidget::resetColors();
}

//----------------------------------------------------------------------
void FScrollView::setBorder (bool enable)
{
  setFlags().feature.no_border = ! enable;
}

//----------------------------------------------------------------------
void FScrollView::setHorizontalScrollBarMode (ScrollBarMode mode)
{
  h_mode = mode;

  if ( isShown() )
    setHorizontalScrollBarVisibility();
}

//----------------------------------------------------------------------
void FScrollView::setVerticalScrollBarMode (ScrollBarMode mode)
{
  v_mode = mode;

  if ( isShown() )
    setVerticalScrollBarVisibility();
}

//----------------------------------------------------------------------
void FScrollView::clearArea (wchar_t fillchar)
{
  if ( viewport )
    FScrollView::clearArea (viewport.get(), fillchar);
}

//----------------------------------------------------------------------
void FScrollView::scrollToX (int x)
{
  scrollTo (x, viewport_geometry.getY() + 1);
}

//----------------------------------------------------------------------
void FScrollView::scrollToY (int y)
{
  scrollTo (viewport_geometry.getX() + 1, y);
}

//----------------------------------------------------------------------
void FScrollView::scrollTo (int x, int y)
{
  const int xoffset_before = viewport_geometry.getX1();
  const int yoffset_before = viewport_geometry.getY1();
  const auto xoffset_end = int(getScrollWidth() - getViewportWidth());
  const auto yoffset_end = int(getScrollHeight() - getViewportHeight());
  const std::size_t save_width = viewport_geometry.getWidth();
  const std::size_t save_height = viewport_geometry.getHeight();
  x--;
  y--;

  if ( ! isChangePosition(x, y) )
    return;

  adjustOffsets(x, y, xoffset_end, yoffset_end);
  const bool change_x = isChangeXPosition(xoffset_before);
  const bool change_y = isChangeYPosition(yoffset_before);

  if ( ! isShown() || ! viewport || ! (change_x || change_y) )
    return;

  if ( change_x )
    changeX (save_width, xoffset_end);

  if ( change_y )
    changeY (save_height, yoffset_end);

  viewport->has_changes = true;
  copy2area();
}

//----------------------------------------------------------------------
void FScrollView::scrollBy (int dx, int dy)
{
  scrollTo (1 + getScrollX() + dx, 1 + getScrollY() + dy);
}

//----------------------------------------------------------------------
void FScrollView::draw()
{
  unsetViewportPrint();

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(true);

  if ( const auto& p = getParentWidget() )
    setColor (p->getForegroundColor(), p->getBackgroundColor());
  else
    setColor();

  if ( hasBorder() )
    drawBorder();

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(false);

  setViewportPrint();
  copy2area();

  if ( ! hbar->isShown() )
    setHorizontalScrollBarVisibility();

  if ( ! vbar->isShown() )
    setVerticalScrollBarVisibility();

  vbar->redraw();
  hbar->redraw();
  drawLabel();
}

//----------------------------------------------------------------------
void FScrollView::drawBorder()
{
  const FRect box(FPoint{1, 1}, getSize());
  finalcut::drawListBorder (this, box);
}

//----------------------------------------------------------------------
void FScrollView::drawLabel()
{
  if ( text.isEmpty() )
    return;

  FString label_text{};
  const FString txt{" " + text + " "};
  unsetViewportPrint();
  const auto hotkeypos = finalcut::getHotkeyPos(txt, label_text);

  if ( hasBorder() )
    FWidget::setPrintPos (FPoint{2, 1});
  else
    FWidget::setPrintPos (FPoint{0, 1});

  drawText (label_text, hotkeypos);
  setViewportPrint();
}

//----------------------------------------------------------------------
void FScrollView::onKeyPress (FKeyEvent* ev)
{
  const auto& iter = key_map.find(ev->key());

  if ( iter != key_map.end() )
  {
    iter->second();
    ev->accept();
  }
}

//----------------------------------------------------------------------
void FScrollView::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != MouseButton::Left )
    return;

  const int mouse_x = ev->getX();
  const int mouse_y = ev->getY();

  if ( mouse_x == 1 || mouse_x == int(getWidth())
    || mouse_y == 1 || mouse_y == int(getHeight()) )
  {
    directFocus();
  }

  // Event handover to parent dialog
  passResizeCornerEventToDialog(this, *ev);
}

//----------------------------------------------------------------------
void FScrollView::onMouseUp (FMouseEvent* ev)
{
  // Event handover to parent dialog
  passResizeCornerEventToDialog(this, *ev);
}

//----------------------------------------------------------------------
void FScrollView::onMouseMove (FMouseEvent* ev)
{
  // Event handover to parent dialog
  passResizeCornerEventToDialog(this, *ev);
}

//----------------------------------------------------------------------
void FScrollView::onWheel (FWheelEvent* ev)
{
  static constexpr int distance = 4;

  if ( ev->getWheel() == MouseWheel::Up )
  {
    scrollBy (0, -distance);
  }
  else if ( ev->getWheel() == MouseWheel::Down )
  {
    scrollBy (0, distance);
  }
  else if ( ev->getWheel() == MouseWheel::Left )
  {
    scrollBy (-distance, 0);
  }
  else if ( ev->getWheel() == MouseWheel::Right )
  {
    scrollBy (distance, 0);
  }
}

//----------------------------------------------------------------------
void FScrollView::onAccel (FAccelEvent*)
{
  directFocus();
}

//----------------------------------------------------------------------
void FScrollView::onFocusIn (FFocusEvent* in_ev)
{
  // Sets the focus to a child widget if it exists

  if ( ! hasChildren() )
    FWidget::onFocusIn(in_ev);

  if ( in_ev->getFocusType() == FocusTypes::NextWidget )
  {
    focusFirstChild() ? in_ev->accept() : in_ev->ignore();
  }
  else if ( in_ev->getFocusType() == FocusTypes::PreviousWidget )
  {
    focusLastChild() ? in_ev->accept() : in_ev->ignore();
  }
}

//----------------------------------------------------------------------
void FScrollView::onChildFocusIn (FFocusEvent*)
{
  // Scrolls the viewport so that the focused widget is visible

  const auto& focus = FWidget::getFocusWidget();

  if ( ! focus )
    return;

  const auto& widget_geometry = focus->getGeometryWithShadow();
  FRect vp_geometry = viewport_geometry;
  vp_geometry.move(1, 1);

  if ( ! vp_geometry.contains(widget_geometry) )
  {
    const int vx = vp_geometry.getX();
    const int vy = vp_geometry.getY();
    const int wx = widget_geometry.getX();
    const int wy = widget_geometry.getY();
    const auto width = int(vp_geometry.getWidth());
    const auto height = int(vp_geometry.getHeight());
    const int x = ( wx > vx ) ? widget_geometry.getX2() - width + 1 : wx;
    const int y = ( wy > vy ) ? widget_geometry.getY2() - height + 1 : wy;
    scrollTo (x, y);
  }
}

//----------------------------------------------------------------------
void FScrollView::onChildFocusOut (FFocusEvent* out_ev)
{
  // Change the focus away from FScrollView to another widget

  const auto* focus = FWidget::getFocusWidget();

  if ( out_ev->getFocusType() == FocusTypes::NextWidget )
  {
    const auto* last_widget = getLastFocusableWidget(getChildren());

    if ( focus != last_widget )
      return;

    out_ev->accept();
    focusNextChild();
  }
  else if ( out_ev->getFocusType() == FocusTypes::PreviousWidget )
  {
    const auto* first_widget = getFirstFocusableWidget(getChildren());

    if ( focus != first_widget )
      return;

    out_ev->accept();
    focusPrevChild();
  }
}

//----------------------------------------------------------------------
void FScrollView::onFailAtChildFocus (FFocusEvent* fail_ev)
{
  // Change the focus away from FScrollView to another widget

  if ( fail_ev->getFocusType() == FocusTypes::NextWidget )
  {
    fail_ev->accept();
    focusNextChild();
  }
  else if ( fail_ev->getFocusType() == FocusTypes::PreviousWidget )
  {
    fail_ev->accept();
    focusPrevChild();
  }
}


// protected methods of FScrollView
//----------------------------------------------------------------------
auto FScrollView::getPrintArea() -> FVTerm::FTermArea*
{
  // returns print area or viewport

  if ( use_own_print_area || ! viewport )
  {
    setChildPrintArea (nullptr);
    auto area = FWidget::getPrintArea();
    setChildPrintArea (viewport.get());
    return area;
  }

  return viewport.get();
}

//----------------------------------------------------------------------
void FScrollView::setHotkeyAccelerator()
{
  setHotkeyViaString (this, text);
}

//----------------------------------------------------------------------
void FScrollView::initLayout()
{
  nf_offset = FVTerm::getFOutput()->isNewFont() ? 1 : 0;
  const auto xoffset_end = int(getScrollWidth() - getViewportWidth());
  const auto yoffset_end = int(getScrollHeight() - getViewportHeight());
  setTopPadding (1 - getScrollY());
  setLeftPadding (1 - getScrollX());
  setBottomPadding (1 - (yoffset_end - getScrollY()));
  setRightPadding (1 - (xoffset_end - getScrollX()) + nf_offset);
  calculateScrollbarPos();
}

//----------------------------------------------------------------------
void FScrollView::adjustSize()
{
  FWidget::adjustSize();
  const std::size_t width = getWidth();
  const std::size_t height = getHeight();
  const int xoffset = viewport_geometry.getX();
  const int yoffset = viewport_geometry.getY();
  scroll_geometry.setPos ( getTermX() + getLeftPadding() - 1
                         , getTermY() + getTopPadding() - 1 );

  if ( viewport )
  {
    viewport->position.x = scroll_geometry.getX();
    viewport->position.y = scroll_geometry.getY();
  }

  vbar->setMaximum (int(getScrollHeight() - getViewportHeight()));
  vbar->setPageSize (int(getScrollHeight()), int(getViewportHeight()));
  vbar->setX (int(width));
  vbar->setHeight (height > 3 ? height - 2 : 1, false);
  vbar->setValue (yoffset);
  vbar->resize();

  hbar->setMaximum (int(getScrollWidth() - getViewportWidth()));
  hbar->setPageSize (int(getScrollWidth()), int(getViewportWidth()));
  hbar->setY (int(height));
  hbar->setWidth (width > 3 ? width - 2 : 1, false);
  hbar->setValue (xoffset);
  hbar->resize();

  setVerticalScrollBarVisibility();
  setHorizontalScrollBarVisibility();
}

//----------------------------------------------------------------------
void FScrollView::copy2area()
{
  // copy viewport to area

  if ( ! hasPrintArea() )
    FWidget::getPrintArea();

  if ( ! (hasPrintArea() && viewport && viewport->has_changes) )
    return;

  auto* printarea = getCurrentPrintArea();
  const auto* area_owner = printarea->getOwner<const FVTerm*>();
  const auto* area_widget = static_cast<const FWidget*>(area_owner);

  const bool ignore_padding = getFlags().feature.ignore_padding;
  const int xoffset = ignore_padding ? 0 : area_widget->getLeftPadding();
  const int yoffset = ignore_padding ? 0 : area_widget->getTopPadding();
  const int ax = xoffset + getX();
  const int ay = yoffset + getY();
  const int dx = viewport_geometry.getX();
  const int dy = viewport_geometry.getY();
  const int rsh = printarea->shadow.width;
  const int area_width = printarea->size.width;
  const int area_height = printarea->size.height;
  const auto viewport_width = int(getViewportWidth());
  const auto viewport_height = int(getViewportHeight());

  // Calculate effective viewport dimensions within the printarea
  const int x_end = std::min(viewport_width, std::max(0, area_width - ax));
  const int y_end = std::min(viewport_height, std::max(0, area_height - ay));

  if ( x_end <= 0 || y_end <= 0 )
    return;  // Early exit if nothing needs copying

  const auto line_start = uInt(ax);
  const auto line_end = uInt(ax + x_end - 1);
  const auto max_limit = uInt(area_width + rsh - 1);

  for (int y{0}; y < y_end; y++)  // line loop
  {
    // Direct access to viewport and area characters
    const auto* vc = &viewport->getFChar(dx, dy + y);  // Viewport character
    auto* ac = &printarea->getFChar(ax, ay + y);       // Area character

    // Copy a line of characters in one operation
    std::memcpy (ac, vc, sizeof(FChar) * unsigned(x_end));

    // Update line changes
    auto& line_changes = printarea->changes[unsigned(ay + y)];
    line_changes.xmin = std::min({line_changes.xmin, line_start, max_limit});
    line_changes.xmax = std::min(std::max(line_changes.xmax, line_end), max_limit);
  }

  setViewportCursor();
  viewport->has_changes = false;
  printarea->has_changes = true;
}


// private methods of FScrollView
//----------------------------------------------------------------------
inline auto FScrollView::getViewportCursorPos() -> FPoint
{
  auto window = FWindow::getWindowWidget(this);

  if ( window )
  {
    const int widget_offsetX = getTermX() - window->getTermX();
    const int widget_offsetY = getTermY() - window->getTermY();
    const int x = widget_offsetX + viewport->input_cursor.x
                - viewport_geometry.getX();
    const int y = widget_offsetY + viewport->input_cursor.y
                - viewport_geometry.getY();
    return { x, y };
  }

  return { -1, -1 };
}

//----------------------------------------------------------------------
void FScrollView::init()
{
  const auto& parent = getParentWidget();

  assert ( parent != nullptr );
  assert ( ! parent->isInstanceOf("FScrollView") );

  initScrollbar (vbar, Orientation::Vertical, &FScrollView::cb_vbarChange);
  initScrollbar (hbar, Orientation::Horizontal, &FScrollView::cb_hbarChange);
  mapKeyFunctions();
  FScrollView::resetColors();
  FScrollView::setGeometry (FPoint{1, 1}, FSize{4, 4});
  setMinimumSize (FSize{4, 4});
  std::size_t width = std::max(std::size_t(1), getViewportWidth());
  std::size_t height = std::max(std::size_t(1), getViewportHeight());
  createViewport({ FSize{width, height} });

  if ( hasPreprocessingHandler(this) )
    addLocalPreprocessingHandler();

  if ( viewport )
    setChildPrintArea (viewport.get());
}

//----------------------------------------------------------------------
inline void FScrollView::addLocalPreprocessingHandler()
{
  FWidget::addPreprocessingHandler
  (
    F_PREPROC_HANDLER (this, &FScrollView::copy2area)
  );
}

//----------------------------------------------------------------------
inline void FScrollView::createViewport (const FSize& size) noexcept
{
  // Initialization of the scrollable viewport

  scroll_geometry.setSize(size);
  viewport = createArea(scroll_geometry);
  setColor();
  FScrollView::clearArea();
}

//----------------------------------------------------------------------
void FScrollView::drawText ( const FString& label_text
                           , std::size_t hotkeypos )
{
  setLabelStyle();
  printLabel (label_text, hotkeypos);
  printEllipsis (label_text);
}

//----------------------------------------------------------------------
inline auto FScrollView::getDisplayedTextLength ( const FString& label_text
                                                , const std::size_t column_width ) const -> std::size_t
{
  if ( column_width <= getClientWidth() )
    return label_text.getLength();

  const std::size_t len = getClientWidth() - 3;
  const FString s = finalcut::getColumnSubString (label_text, 1, len);
  return s.getLength();
}

//----------------------------------------------------------------------
void FScrollView::setLabelStyle() const
{
  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(true);

  const auto& wc_label = getColorTheme()->label;

  if ( isEnabled() )
    setColor(wc_label.emphasis_fg, wc_label.bg);
  else
    setColor(wc_label.inactive_fg, wc_label.inactive_bg);
}

//----------------------------------------------------------------------
void FScrollView::printLabel ( const FString& label_text,
                               std::size_t hotkeypos )
{
  const auto& wc_label = getColorTheme()->label;
  const std::size_t column_width = getColumnWidth(label_text);
  std::size_t length = getDisplayedTextLength (label_text, column_width);
  const auto underline = ! getFlags().feature.no_underline;

  for (std::size_t z{0}; z < length; z++)
  {
    if ( z == hotkeypos && getFlags().feature.active )
    {
      setColor (wc_label.hotkey_fg, wc_label.hotkey_bg);

      if ( underline )
        setUnderline();

      print (label_text[z]);

      if ( underline )
        unsetUnderline();

      setColor (wc_label.emphasis_fg, wc_label.bg);
    }
    else
      print (label_text[z]);
  }
}

//----------------------------------------------------------------------
void FScrollView::printEllipsis (const FString& label_text)
{
  const auto& wc_label = getColorTheme()->label;
  const std::size_t column_width = getColumnWidth(label_text);
  const bool ellipsis{column_width > getClientWidth()};

  if ( ellipsis )  // Print ellipsis
    print() << FColorPair {wc_label.ellipsis_fg, wc_label.bg} << "..";

  if ( FVTerm::getFOutput()->isMonochron() )
    setReverse(true);
}

//----------------------------------------------------------------------
void FScrollView::directFocus()
{
  auto focused_widget = getFocusWidget();
  focusFirstChild();

  if ( focused_widget )
    focused_widget->redraw();

  focused_widget = getFocusWidget();

  if ( focused_widget )
    focused_widget->redraw();

  drawStatusBarMessage();
}

//----------------------------------------------------------------------
inline void FScrollView::mapKeyFunctions()
{
  auto scrollToEnd = [this] ()
  {
    auto yoffset_end = int(getScrollHeight() - getViewportHeight());
    scrollToY (1 + yoffset_end);
  };

  key_map =
  {
    { FKey::Up        , [this] { scrollBy (0, -1); } },
    { FKey::Down      , [this] { scrollBy (0, 1); } },
    { FKey::Left      , [this] { scrollBy (-1, 0); } },
    { FKey::Right     , [this] { scrollBy (1, 0); } },
    { FKey::Page_up   , [this] { scrollBy (0, -int(getViewportHeight())); } },
    { FKey::Page_down , [this] { scrollBy (0, int(getViewportHeight())); } },
    { FKey::Home      , [this] { scrollToY (1); } },
    { FKey::End       , scrollToEnd }
  };
}

//----------------------------------------------------------------------
void FScrollView::changeSize (const FSize& size, bool adjust)
{
  const std::size_t w = size.getWidth();
  const std::size_t h = size.getHeight();
  viewport_geometry.setSize ( w - vertical_border_spacing - nf_offset
                            , h - horizontal_border_spacing );
  calculateScrollbarPos();

  if ( getScrollWidth() < getViewportWidth()
    || getScrollHeight() < getViewportHeight() )
  {
    FScrollView::setScrollSize (getViewportSize());
  }
  else if ( ! adjust && viewport )
  {
    viewport->position.x = scroll_geometry.getX();
    viewport->position.y = scroll_geometry.getY();
  }

  if ( ! viewport )
    return;

  // Insufficient space scrolling
  viewport_geometry.x1_ref() = \
      std::min ( int(getScrollWidth() - getViewportWidth())
               , viewport_geometry.getX1() );
  viewport_geometry.y1_ref() = \
      std::min ( int(getScrollHeight() - getViewportHeight())
               , viewport_geometry.getY1() );
}

//----------------------------------------------------------------------
inline void FScrollView::adjustOffsets ( int x, int y
                                       , int xoffset_end
                                       , int yoffset_end )
{
  int& xoffset = viewport_geometry.x1_ref();
  int& yoffset = viewport_geometry.y1_ref();
  xoffset = std::max(x, 0);
  xoffset = std::min(xoffset, xoffset_end);
  yoffset = std::max(y, 0);
  yoffset = std::min(yoffset, yoffset_end);
}

//----------------------------------------------------------------------
inline auto FScrollView::isChangePosition (const int x, const int y) const -> bool
{
  return isChangeXPosition(x) || isChangeYPosition(y);
}

//----------------------------------------------------------------------
inline auto FScrollView::isChangeXPosition (const int x) const -> bool
{
  return viewport_geometry.getX1() != x;
}

//----------------------------------------------------------------------
inline auto FScrollView::isChangeYPosition (const int y) const -> bool
{
  return viewport_geometry.getY1() != y;
}

//----------------------------------------------------------------------
inline void FScrollView::changeX (const std::size_t width, const int xoffset_end)
{
  const int xoffset = viewport_geometry.getX1();
  viewport_geometry.setWidth(width);
  setLeftPadding (1 - xoffset);
  setRightPadding (1 - (xoffset_end - xoffset) + int(nf_offset));

  if ( ! update_scrollbar )
    return;

  hbar->setValue (xoffset);
  hbar->drawBar();
}

//----------------------------------------------------------------------
inline void FScrollView::changeY (const std::size_t height, const int yoffset_end)
{
  const int yoffset = viewport_geometry.getY1();
  viewport_geometry.setHeight(height);
  setTopPadding (1 - yoffset);
  setBottomPadding (1 - (yoffset_end - yoffset));

  if ( ! update_scrollbar )
    return;

  vbar->setValue (yoffset);
  vbar->drawBar();
}

//----------------------------------------------------------------------
void FScrollView::calculateScrollbarPos() const
{
  const std::size_t width  = std::max(std::size_t(3), getWidth());
  const std::size_t height = std::max(std::size_t(3), getHeight());

  if ( nf_offset )
  {
    vbar->setGeometry (FPoint{int(width), 2}, FSize{2, height - 2});
    hbar->setGeometry (FPoint{1, int(height)}, FSize{width - 2, 1});
  }
  else
  {
    vbar->setGeometry (FPoint{int(width), 2}, FSize{1, height - 2});
    hbar->setGeometry (FPoint{2, int(height)}, FSize{width - 2, 1});
  }

  vbar->resize();
  hbar->resize();
}

//----------------------------------------------------------------------
void FScrollView::setHorizontalScrollBarVisibility() const
{
  if ( h_mode == ScrollBarMode::Auto )
  {
    if ( getScrollWidth() > getViewportWidth() )
      hbar->show();
    else
      hbar->hide();
  }
  else if ( h_mode == ScrollBarMode::Hidden )
  {
    hbar->hide();
  }
  else if ( h_mode == ScrollBarMode::Scroll )
  {
    hbar->show();
  }
}

//----------------------------------------------------------------------
void FScrollView::setVerticalScrollBarVisibility() const
{
  if ( v_mode == ScrollBarMode::Auto )
  {
    if ( getScrollHeight() > getViewportHeight() )
      vbar->show();
    else
      vbar->hide();
  }
  else if ( v_mode == ScrollBarMode::Hidden )
  {
    vbar->hide();
  }
  else if ( v_mode == ScrollBarMode::Scroll )
  {
    vbar->show();
  }
}

//----------------------------------------------------------------------
void FScrollView::setViewportCursor()
{
  if ( ! isChild(getFocusWidget()) )
    return;

  const FPoint cursor_pos { viewport->input_cursor.x - 1
                          , viewport->input_cursor.y - 1 };
  const FPoint window_cursor_pos{ getViewportCursorPos() };
  auto printarea = getCurrentPrintArea();
  printarea->setInputCursorPos ( window_cursor_pos.getX()
                               , window_cursor_pos.getY() );

  if ( viewport->input_cursor_visible
    && viewport_geometry.contains(cursor_pos) )
    printarea->input_cursor_visible = true;
  else
    printarea->input_cursor_visible = false;
}

//----------------------------------------------------------------------
inline auto FScrollView::shouldUpdateScrollbar (FScrollbar::ScrollType scroll_type) const -> bool
{
  return scroll_type >= FScrollbar::ScrollType::StepBackward;
}

//----------------------------------------------------------------------
inline auto FScrollView::getVerticalScrollDistance (const FScrollbar::ScrollType scroll_type) const -> int
{
  if ( scroll_type == FScrollbar::ScrollType::PageBackward
    || scroll_type == FScrollbar::ScrollType::PageForward )
  {
    return int(getViewportHeight());
  }

  return 1;
}

//----------------------------------------------------------------------
inline auto FScrollView::getHorizontalScrollDistance (const FScrollbar::ScrollType scroll_type) const -> int
{
  if ( scroll_type == FScrollbar::ScrollType::PageBackward
    || scroll_type == FScrollbar::ScrollType::PageForward )
  {
    return int(getViewportWidth());
  }

  return 1;
}

//----------------------------------------------------------------------
void FScrollView::cb_vbarChange (const FWidget*)
{
  auto scroll_type = vbar->getScrollType();
  update_scrollbar = shouldUpdateScrollbar(scroll_type);
  static constexpr int wheel_distance = 4;
  int distance = getVerticalScrollDistance(scroll_type);

  switch ( scroll_type )
  {
    case FScrollbar::ScrollType::PageBackward:
    case FScrollbar::ScrollType::StepBackward:
      scrollBy (0, -distance);
      break;

    case FScrollbar::ScrollType::PageForward:
    case FScrollbar::ScrollType::StepForward:
      scrollBy (0, distance);
      break;

    case FScrollbar::ScrollType::Jump:
      scrollToY (1 + int(vbar->getValue()));
      break;

    case FScrollbar::ScrollType::WheelUp:
    case FScrollbar::ScrollType::WheelLeft:
      scrollBy (0, -wheel_distance);
      break;

    case FScrollbar::ScrollType::WheelDown:
    case FScrollbar::ScrollType::WheelRight:
      scrollBy (0, wheel_distance);
      break;

    default:
      throw std::invalid_argument{"Invalid scroll type"};
  }

  update_scrollbar = true;
}

//----------------------------------------------------------------------
void FScrollView::cb_hbarChange (const FWidget*)
{
  auto scroll_type = hbar->getScrollType();
  update_scrollbar = shouldUpdateScrollbar(scroll_type);
  static constexpr int wheel_distance = 4;
  int distance = getHorizontalScrollDistance(scroll_type);

  switch ( scroll_type )
  {
    case FScrollbar::ScrollType::PageBackward:
    case FScrollbar::ScrollType::StepBackward:
      scrollBy (-distance, 0);
      break;

    case FScrollbar::ScrollType::PageForward:
    case FScrollbar::ScrollType::StepForward:
      scrollBy (distance, 0);
      break;

    case FScrollbar::ScrollType::Jump:
      scrollToX (1 + int(hbar->getValue()));
      break;

    case FScrollbar::ScrollType::WheelUp:
    case FScrollbar::ScrollType::WheelLeft:
      scrollBy (-wheel_distance, 0);
      break;

    case FScrollbar::ScrollType::WheelDown:
    case FScrollbar::ScrollType::WheelRight:
      scrollBy (wheel_distance, 0);
      break;

    default:
      throw std::invalid_argument{"Invalid scroll type"};
  }

  update_scrollbar = true;
}

// FVTerm friend function definition
//----------------------------------------------------------------------
void setPrintArea (FWidget& widget, FVTerm::FTermArea* area)
{
  widget.print_area = area;
}

}  // namespace finalcut
