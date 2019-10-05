/***********************************************************************
* fscrollview.h - Widget FScrollView (a scrolling area with on-demand  *
*                 scroll bars)                                         *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2017-2019 Markus Gans                                      *
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

/*  Inheritance diagram
 *  ═══════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▏
 * ▕  FTerm  ▏
 * ▕▁▁▁▁▁▁▁▁▁▏
 *      ▲
 *      │
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

#include "final/fscrollbar.h"
#include "final/fwidget.h"

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

    // Destructor
    virtual ~FScrollView();

    // Disable assignment operator (=)
    FScrollView& operator = (const FScrollView&) = delete;

    // Accessors
    const FString       getClassName() const override;
    std::size_t         getViewportWidth() const;
    std::size_t         getViewportHeight() const;
    const FSize         getViewportSize();
    std::size_t         getScrollWidth() const;
    std::size_t         getScrollHeight() const;
    const FSize         getScrollSize() const;
    const FPoint        getScrollPos() const;
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
    void                setCursorPos (const FPoint&);
    void                setPrintPos (const FPoint&);
    bool                setViewportPrint (bool);
    bool                setViewportPrint();
    bool                unsetViewportPrint();
    bool                setBorder (bool);
    bool                setBorder();
    bool                unsetBorder();
    void                setHorizontalScrollBarMode (fc::scrollBarMode);
    void                setVerticalScrollBarMode (fc::scrollBarMode);

    // Inquiries
    bool                hasBorder();
    bool                isViewportPrint();

    // Method
    void                clearArea (int = ' ') override;
    void                scrollToX (int);
    void                scrollToY (int);
    void                scrollTo (const FPoint&);
    void                scrollTo (int, int);
    void                scrollBy (int, int);
    void                print (const FPoint&) override;
    void                draw() override;
    void                drawBorder() override;

    // Event handlers
    void                onKeyPress (FKeyEvent*) override;
    void                onWheel (FWheelEvent*) override;
    void                onFocusIn (FFocusEvent*) override;
    void                onChildFocusIn (FFocusEvent*) override;
    void                onChildFocusOut (FFocusEvent*) override;

  protected:
    // Using-declaration
    using FVTerm::clearArea;

    // Accessor
    term_area*          getPrintArea() override;

    // Method
    void                adjustSize() override;
    void                copy2area();

  private:
    // Typedefs
    typedef std::unordered_map<int, std::function<void()>> keyMap;

    // Constants
    static constexpr int vertical_border_spacing = 2;
    static constexpr int horizontal_border_spacing = 2;

    // Accessors
    FPoint              getViewportCursorPos();

    // Methods
    void                init (FWidget*);
    void                mapKeyFunctions();
    void                calculateScrollbarPos();
    template<typename Callback>
    void                initScrollbar ( FScrollbarPtr&
                                      , fc::orientation
                                      , Callback );
    void                setHorizontalScrollBarVisibility();
    void                setVerticalScrollBarVisibility();
    void                setViewportCursor();

    // Callback methods
    void                cb_VBarChange (FWidget*, FDataPtr);
    void                cb_HBarChange (FWidget*, FDataPtr);

    // Data members
    FRect              scroll_geometry{1, 1, 1, 1};
    FRect              viewport_geometry{};
    term_area*         viewport{nullptr};  // virtual scroll content
    FScrollbarPtr      vbar{nullptr};
    FScrollbarPtr      hbar{nullptr};
    keyMap             key_map{};
    uInt8              nf_offset{0};
    bool               border{true};
    bool               use_own_print_area{false};
    bool               update_scrollbar{true};
    fc::scrollBarMode  vMode{fc::Auto};  // fc:Auto, fc::Hidden or fc::Scroll
    fc::scrollBarMode  hMode{fc::Auto};
};

// FScrollView inline functions
//----------------------------------------------------------------------
inline const FString FScrollView::getClassName() const
{ return "FScrollView"; }

//----------------------------------------------------------------------
inline std::size_t FScrollView::getViewportWidth() const
{ return getWidth() - vertical_border_spacing - std::size_t(nf_offset); }

//----------------------------------------------------------------------
inline std::size_t FScrollView::getViewportHeight() const
{ return getHeight() - horizontal_border_spacing; }

//----------------------------------------------------------------------
inline const FSize FScrollView::getViewportSize()
{ return FSize(getViewportWidth(), getViewportHeight()); }

//----------------------------------------------------------------------
inline std::size_t FScrollView::getScrollWidth() const
{ return scroll_geometry.getWidth(); }

//----------------------------------------------------------------------
inline std::size_t FScrollView::getScrollHeight() const
{ return scroll_geometry.getHeight(); }

//----------------------------------------------------------------------
inline const FSize FScrollView::getScrollSize() const
{ return scroll_geometry.getSize(); }

//----------------------------------------------------------------------
inline const FPoint FScrollView::getScrollPos() const
{ return viewport_geometry.getPos(); }

//----------------------------------------------------------------------
inline int FScrollView::getScrollX() const
{ return viewport_geometry.getX(); }

//----------------------------------------------------------------------
inline int FScrollView::getScrollY() const
{ return viewport_geometry.getY(); }

//----------------------------------------------------------------------
inline bool FScrollView::setViewportPrint()
{ return setViewportPrint(true); }

//----------------------------------------------------------------------
inline bool FScrollView::unsetViewportPrint()
{ return setViewportPrint(false); }

//----------------------------------------------------------------------
inline bool FScrollView::setBorder()
{ return setBorder(true); }

//----------------------------------------------------------------------
inline bool FScrollView::unsetBorder()
{ return setBorder(false); }

//----------------------------------------------------------------------
inline bool FScrollView::hasBorder()
{ return border; }

//----------------------------------------------------------------------
inline bool FScrollView::isViewportPrint()
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
template<typename Callback>
inline void FScrollView::initScrollbar ( FScrollbarPtr& bar
                                       , fc::orientation o
                                       , Callback cb_handler )
{
  finalcut::initScrollbar (bar, o, this, cb_handler);
  term_area* area = getPrintArea();
  bar->setPrintArea(area);
}

}  // namespace finalcut

#endif  // FSCROLLVIEW_H
