/***********************************************************************
* fscrollview.h - Widget FScrollView (a scrolling area with on-demand  *
*                 scroll bars)                                         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2017-2021 Markus Gans                                      *
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

#include <unordered_map>

#include "final/fwidget.h"
#include "final/widget/fscrollbar.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FScrollView
//----------------------------------------------------------------------

class FScrollView : public FWidget
{
  public:
    // Using-declaration
    using FWidget::setGeometry;
    using FWidget::print;

    // Constructor
    explicit FScrollView (FWidget* = nullptr);

    // Disable copy constructor
    FScrollView (const FScrollView&) = delete;

    // Disable move constructor
    FScrollView (FScrollView&&) noexcept = delete;

    // Destructor
    ~FScrollView() override;

    // Disable copy assignment operator (=)
    FScrollView& operator = (const FScrollView&) = delete;

    // Disable move assignment operator (=)
    FScrollView& operator = (FScrollView&&) noexcept = delete;
    // Accessors
    FString             getClassName() const override;
    FString&            getText() &;
    std::size_t         getViewportWidth() const;
    std::size_t         getViewportHeight() const;
    FSize               getViewportSize() const;
    std::size_t         getScrollWidth() const;
    std::size_t         getScrollHeight() const;
    FSize               getScrollSize() const;
    FPoint              getScrollPos() const;
    int                 getScrollX() const;
    int                 getScrollY() const;

    // Mutator
    virtual void        setScrollWidth (std::size_t);
    virtual void        setScrollHeight (std::size_t);
    virtual void        setScrollSize (const FSize&);
    void                setX (int, bool = true) override;
    void                setY (int, bool = true) override;
    void                setPos (const FPoint&, bool = true) override;
    void                setWidth (std::size_t, bool = true) override;
    void                setHeight (std::size_t, bool = true) override;
    void                setSize (const FSize&, bool = true) override;
    void                setGeometry ( const FPoint&, const FSize&
                                    , bool = true ) override;
    bool                setCursorPos (const FPoint&) override;
    void                setPrintPos (const FPoint&) override;
    void                setText (const FString&);
    bool                setViewportPrint (bool = true);
    bool                unsetViewportPrint();
    void                resetColors() override;
    bool                setBorder (bool = true);
    bool                unsetBorder();
    bool                setFocus (bool = true) override;
    void                setHorizontalScrollBarMode (ScrollBarMode);
    void                setVerticalScrollBarMode (ScrollBarMode);

    // Inquiries
    bool                hasBorder() const;
    bool                isViewportPrint() const;

    // Methods
    void                clearArea (wchar_t = L' ') override;
    void                scrollToX (int);
    void                scrollToY (int);
    void                scrollTo (const FPoint&);
    void                scrollTo (int, int);
    void                scrollBy (int, int);
    void                print (const FPoint&) override;
    void                draw() override;
    void                drawBorder() override;
    void                drawLabel();

    // Event handlers
    void                onKeyPress (FKeyEvent*) override;
    void                onMouseDown (FMouseEvent*) override;
    void                onWheel (FWheelEvent*) override;
    void                onAccel (FAccelEvent*) override;
    void                onFocusIn (FFocusEvent*) override;
    void                onChildFocusIn (FFocusEvent*) override;
    void                onChildFocusOut (FFocusEvent*) override;

  protected:
    // Using-declaration
    using FVTerm::clearArea;

    // Accessor
    FTermArea*          getPrintArea() override;

    // Methods
    void                setHotkeyAccelerator();
    void                initLayout() override;
    void                adjustSize() override;
    void                copy2area();

  private:
    // Using-declaration
    using KeyMap = std::unordered_map<FKey, std::function<void()>, EnumHash<FKey>>;

    // Constants
    static constexpr std::size_t vertical_border_spacing = 2;
    static constexpr std::size_t horizontal_border_spacing = 2;

    // Accessors
    FPoint              getViewportCursorPos();

    // Methods
    void                init();
    void                drawText (const FString&, std::size_t);
    void                directFocus();
    void                mapKeyFunctions();
    void                calculateScrollbarPos() const;
    template <typename Callback>
    void                initScrollbar ( FScrollbarPtr&
                                      , Orientation
                                      , Callback );
    void                setHorizontalScrollBarVisibility() const;
    void                setVerticalScrollBarVisibility() const;
    void                setViewportCursor();

    // Callback methods
    void                cb_vbarChange (const FWidget*);
    void                cb_hbarChange (const FWidget*);

    // Data members
    FRect              scroll_geometry{1, 1, 1, 1};
    FRect              viewport_geometry{};
    FTermArea*         viewport{nullptr};  // virtual scroll content
    FString            text{};
    FScrollbarPtr      vbar{nullptr};
    FScrollbarPtr      hbar{nullptr};
    KeyMap             key_map{};
    uInt8              nf_offset{0};
    bool               use_own_print_area{false};
    bool               update_scrollbar{true};
    ScrollBarMode      v_mode{ScrollBarMode::Auto};  // fc:Auto, fc::Hidden or fc::Scroll
    ScrollBarMode      h_mode{ScrollBarMode::Auto};
};

// FScrollView inline functions
//----------------------------------------------------------------------
inline FString& FScrollView::getText() &
{ return text; }

//----------------------------------------------------------------------
inline FString FScrollView::getClassName() const
{ return "FScrollView"; }

//----------------------------------------------------------------------
inline std::size_t FScrollView::getViewportWidth() const
{
  return ( getScrollHeight() > getViewportHeight() )
       ? getWidth() - vertical_border_spacing - std::size_t(nf_offset)
       : getWidth() - vertical_border_spacing;
}

//----------------------------------------------------------------------
inline std::size_t FScrollView::getViewportHeight() const
{ return getHeight() - horizontal_border_spacing; }

//----------------------------------------------------------------------
inline FSize FScrollView::getViewportSize() const
{ return {getViewportWidth(), getViewportHeight()}; }

//----------------------------------------------------------------------
inline std::size_t FScrollView::getScrollWidth() const
{ return scroll_geometry.getWidth(); }

//----------------------------------------------------------------------
inline std::size_t FScrollView::getScrollHeight() const
{ return scroll_geometry.getHeight(); }

//----------------------------------------------------------------------
inline FSize FScrollView::getScrollSize() const
{ return scroll_geometry.getSize(); }

//----------------------------------------------------------------------
inline FPoint FScrollView::getScrollPos() const
{ return viewport_geometry.getPos(); }

//----------------------------------------------------------------------
inline int FScrollView::getScrollX() const
{ return viewport_geometry.getX(); }

//----------------------------------------------------------------------
inline int FScrollView::getScrollY() const
{ return viewport_geometry.getY(); }

//----------------------------------------------------------------------
inline bool FScrollView::unsetViewportPrint()
{ return setViewportPrint(false); }

//----------------------------------------------------------------------
inline bool FScrollView::unsetBorder()
{ return setBorder(false); }

//----------------------------------------------------------------------
inline bool FScrollView::hasBorder() const
{ return ! getFlags().no_border; }

//----------------------------------------------------------------------
inline bool FScrollView::isViewportPrint() const
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
  FTermArea* area = getPrintArea();
  bar->setPrintArea(area);
}

}  // namespace finalcut

#endif  // FSCROLLVIEW_H
