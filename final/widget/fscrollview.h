/***********************************************************************
* fscrollview.h - Widget FScrollView (a scrolling area with on-demand  *
*                 scroll bars)                                         *
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

/*  Inheritance diagram
 *  ═══════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▏
 * ▕ FVTerm  ▏ ▕ FObject ▏
 * ▕▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▏
 *      ▲           ▲
 *      │           │
 *      └─────┬─────┘
 *            │
 *       ▕▔▔▔▔▔▔▔▔▔▏
 *       ▕ FWidget ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏
 *            ▲
 *            │
 *     ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *     ▕ FScrollView ▏
 *     ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FSCROLLVIEW_H
#define FSCROLLVIEW_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <memory>
#include <unordered_map>

#include "final/fwidget.h"
#include "final/widget/fscrollbar.h"

namespace finalcut
{

// FVTerm friend function forward declaration
void setPrintArea (FWidget&, FVTerm::FTermArea*);

//----------------------------------------------------------------------
// class FScrollView
//----------------------------------------------------------------------

class FScrollView : public FWidget
{
  public:
    // Using-declaration
    using FWidget::delAccelerator;
    using FWidget::print;
    using FWidget::setGeometry;

    // Constructor
    explicit FScrollView (FWidget* = nullptr);

    // Disable copy constructor
    FScrollView (const FScrollView&) = delete;

    // Disable move constructor
    FScrollView (FScrollView&&) noexcept = delete;

    // Destructor
    ~FScrollView() override;

    // Disable copy assignment operator (=)
    auto operator = (const FScrollView&) -> FScrollView& = delete;

    // Disable move assignment operator (=)
    auto operator = (FScrollView&&) noexcept -> FScrollView& = delete;

    // Accessors
    auto getClassName() const -> FString override;
    auto getText() & -> FString&;
    auto getViewportWidth() const -> std::size_t;
    auto getViewportHeight() const -> std::size_t;
    auto getViewportSize() const -> FSize;
    auto getScrollWidth() const -> std::size_t;
    auto getScrollHeight() const -> std::size_t;
    auto getScrollSize() const -> FSize;
    auto getScrollPos() const -> FPoint;
    auto getScrollX() const -> int;
    auto getScrollY() const -> int;

    // Mutator
    virtual void setScrollWidth (std::size_t);
    virtual void setScrollHeight (std::size_t);
    virtual void setScrollSize (const FSize&);
    void setX (int, bool = true) override;
    void setY (int, bool = true) override;
    void setPos (const FPoint&, bool = true) override;
    void setWidth (std::size_t, bool = true) override;
    void setHeight (std::size_t, bool = true) override;
    void setSize (const FSize&, bool = true) override;
    void setGeometry ( const FPoint&, const FSize&
                     , bool = true ) override;
    auto setCursorPos (const FPoint&) -> bool override;
    void setPrintPos (const FPoint&) override;
    void setText (const FString&);
    void setViewportPrint (bool = true);
    void unsetViewportPrint();
    void resetColors() override;
    void setBorder (bool = true);
    void unsetBorder();
    void setHorizontalScrollBarMode (ScrollBarMode);
    void setVerticalScrollBarMode (ScrollBarMode);

    // Inquiries
    auto hasBorder() const -> bool;
    auto isViewportPrint() const -> bool;

    // Methods
    void clearArea (wchar_t = L' ') override;
    void scrollToX (int);
    void scrollToY (int);
    void scrollTo (const FPoint&);
    void scrollTo (int, int);
    void scrollBy (int, int);
    void print (const FPoint&) override;
    void draw() override;
    void drawBorder() override;
    void drawLabel();

    // Event handlers
    void onKeyPress (FKeyEvent*) override;
    void onMouseDown (FMouseEvent*) override;
    void onMouseUp (FMouseEvent*) override;
    void onMouseMove (FMouseEvent*) override;
    void onWheel (FWheelEvent*) override;
    void onAccel (FAccelEvent*) override;
    void onFocusIn (FFocusEvent*) override;
    void onChildFocusIn (FFocusEvent*) override;
    void onChildFocusOut (FFocusEvent*) override;
    void onFailAtChildFocus (FFocusEvent*) override;

  protected:
    // Using-declaration
    using FVTerm::clearArea;

    // Accessor
    auto getPrintArea() -> FTermArea* override;

    // Methods
    void setHotkeyAccelerator();
    void initLayout() override;
    void adjustSize() override;
    void copy2area();

  private:
    // Using-declaration
    using KeyMap = std::unordered_map<FKey, std::function<void()>, EnumHash<FKey>>;

    // Constants
    static constexpr std::size_t vertical_border_spacing = 2;
    static constexpr std::size_t horizontal_border_spacing = 2;

    // Accessors
    auto getViewportCursorPos() -> FPoint;

    // Methods
    void init();
    void addLocalPreprocessingHandler();
    void createViewport (const FSize&) noexcept;
    void drawText (const FString&, std::size_t);
    auto getDisplayedTextLength (const FString&, const std::size_t) const -> std::size_t;
    void setLabelStyle() const;
    void printLabel (const FString&, std::size_t);
    void printEllipsis (const FString&);
    void directFocus();
    void mapKeyFunctions();
    void changeSize (const FSize&, bool);
    void adjustOffsets (int, int, int, int);
    auto isChangePosition (const int, const int) const -> bool;
    auto isChangeXPosition (const int) const -> bool;
    auto isChangeYPosition (const int) const -> bool;
    void changeX (const std::size_t, const int);
    void changeY (const std::size_t, const int);
    void calculateScrollbarPos() const;
    template <typename Callback>
    void initScrollbar (FScrollbarPtr&, Orientation, Callback);
    void setHorizontalScrollBarVisibility() const;
    void setVerticalScrollBarVisibility() const;
    void setViewportCursor();
    auto shouldUpdateScrollbar (FScrollbar::ScrollType) const -> bool;
    auto getVerticalScrollDistance (const FScrollbar::ScrollType) const -> int;
    auto getHorizontalScrollDistance (const FScrollbar::ScrollType) const -> int;

    // Callback methods
    void cb_vbarChange (const FWidget*);
    void cb_hbarChange (const FWidget*);

    // Data members
    FRect                      scroll_geometry{1, 1, 1, 1};
    FRect                      viewport_geometry{};
    std::unique_ptr<FTermArea> viewport{};  // virtual scroll content
    FString                    text{};
    FScrollbarPtr              vbar{nullptr};
    FScrollbarPtr              hbar{nullptr};
    KeyMap                     key_map{};
    uInt8                      nf_offset{0};
    bool                       use_own_print_area{false};
    bool                       update_scrollbar{true};
    ScrollBarMode              v_mode{ScrollBarMode::Auto};  // fc:Auto, fc::Hidden or fc::Scroll
    ScrollBarMode              h_mode{ScrollBarMode::Auto};
};

// FScrollView inline functions
//----------------------------------------------------------------------
inline auto FScrollView::getText() & -> FString&
{ return text; }

//----------------------------------------------------------------------
inline auto FScrollView::getClassName() const -> FString
{ return "FScrollView"; }

//----------------------------------------------------------------------
inline auto FScrollView::getViewportWidth() const -> std::size_t
{
  return ( getScrollHeight() > getViewportHeight() )
         ? static_cast<std::size_t>(std::max (1, getGeometry().getX2() -
                                                 (getGeometry().getX1() - 1) -
                                                 static_cast<int>(vertical_border_spacing) -
                                                 static_cast<int>(nf_offset)))
         : static_cast<std::size_t>(std::max (1, getGeometry().getX2() -
                                                 (getGeometry().getX1() - 1) -
                                                 static_cast<int>(vertical_border_spacing)));
}

//----------------------------------------------------------------------
inline auto FScrollView::getViewportHeight() const -> std::size_t
{
  return static_cast<std::size_t>(std::max (1, getGeometry().getY2() -
                                               (getGeometry().getY1() - 1) -
                                               static_cast<int>(horizontal_border_spacing)));
}

//----------------------------------------------------------------------
inline auto FScrollView::getViewportSize() const -> FSize
{ return { FSize{getViewportWidth(), getViewportHeight()} }; }

//----------------------------------------------------------------------
inline auto FScrollView::getScrollWidth() const -> std::size_t
{ return scroll_geometry.getWidth(); }

//----------------------------------------------------------------------
inline auto FScrollView::getScrollHeight() const -> std::size_t
{ return scroll_geometry.getHeight(); }

//----------------------------------------------------------------------
inline auto FScrollView::getScrollSize() const -> FSize
{ return scroll_geometry.getSize(); }

//----------------------------------------------------------------------
inline auto FScrollView::getScrollPos() const -> FPoint
{ return viewport_geometry.getPos(); }

//----------------------------------------------------------------------
inline auto FScrollView::getScrollX() const -> int
{ return viewport_geometry.getX(); }

//----------------------------------------------------------------------
inline auto FScrollView::getScrollY() const -> int
{ return viewport_geometry.getY(); }

//----------------------------------------------------------------------
inline void FScrollView::unsetViewportPrint()
{ setViewportPrint(false); }

//----------------------------------------------------------------------
inline void FScrollView::unsetBorder()
{ setBorder(false); }

//----------------------------------------------------------------------
inline auto FScrollView::hasBorder() const -> bool
{ return ! getFlags().feature.no_border; }

//----------------------------------------------------------------------
inline auto FScrollView::isViewportPrint() const -> bool
{ return ! use_own_print_area; }

//----------------------------------------------------------------------
inline void FScrollView::scrollTo (const FPoint& pos)
{ scrollTo(pos.getX(), pos.getY()); }

//----------------------------------------------------------------------
inline void FScrollView::print (const FPoint& pos)
{
  if ( use_own_print_area )
    FWidget::setPrintPos(pos);
  else
    setPrintPos(pos);
}

//----------------------------------------------------------------------
template <typename Callback>
inline void FScrollView::initScrollbar ( FScrollbarPtr& bar
                                       , Orientation o
                                       , Callback cb_handler )
{
  finalcut::initScrollbar (bar, o, this, cb_handler);
  FTermArea* area = FScrollView::getPrintArea();
  finalcut::setPrintArea (*bar, area);
}

}  // namespace finalcut

#endif  // FSCROLLVIEW_H
