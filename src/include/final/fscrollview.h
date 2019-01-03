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

#include <memory>

#include "final/fscrollbar.h"
#include "final/fwidget.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FScrollView
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FScrollView : public FWidget
{
  public:
    // Using-declaration
    using FWidget::setGeometry;
    using FWidget::setPrintPos;
    using FWidget::setPos;

    // Constructor
    explicit FScrollView (FWidget* = nullptr);

    // Disable copy constructor
    FScrollView (const FScrollView&) = delete;

    // Destructor
    virtual ~FScrollView();

    // Disable assignment operator (=)
    FScrollView& operator = (const FScrollView&) = delete;

    // Accessors
    virtual const char* getClassName() const override;
    std::size_t         getViewportWidth() const;
    std::size_t         getViewportHeight() const;
    std::size_t         getScrollWidth() const;
    std::size_t         getScrollHeight() const;
    const FPoint        getScrollPos() const;
    int                 getScrollX() const;
    int                 getScrollY() const;

    // Mutator
    virtual void        setScrollWidth (std::size_t);
    virtual void        setScrollHeight (std::size_t);
    virtual void        setScrollSize (std::size_t, std::size_t);
    virtual void        setX (int, bool = true) override;
    virtual void        setY (int, bool = true) override;
    virtual void        setPos (int, int, bool = true) override;
    virtual void        setWidth (std::size_t, bool = true) override;
    virtual void        setHeight (std::size_t, bool = true) override;
    virtual void        setSize (std::size_t, std::size_t, bool = true) override;
    virtual void        setGeometry ( int, int
                                    , std::size_t, std::size_t
                                    , bool = true ) override;
    void                setCursorPos (int, int);
    void                setPrintPos (int, int);
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
    virtual void        clearArea (int = ' ') override;
    void                scrollToX (int);
    void                scrollToY (int);
    void                scrollTo (const FPoint&);
    void                scrollTo (int, int);
    void                scrollBy (int, int);
    virtual void        draw() override;

    // Event handlers
    virtual void        onKeyPress (FKeyEvent*) override;
    virtual void        onWheel (FWheelEvent*) override;
    virtual void        onFocusIn (FFocusEvent*) override;
    virtual void        onChildFocusIn (FFocusEvent*) override;
    virtual void        onChildFocusOut (FFocusEvent*) override;

  protected:
    // Using-declaration
    using FVTerm::clearArea;

    // Accessor
    virtual term_area*  getPrintArea() override;

    // Method
    virtual void        adjustSize() override;
    void                copy2area();

  private:
    // Typedef
    typedef std::shared_ptr<FScrollbar> FScrollbarPtr;

    // Constants
    static constexpr int vertical_border_spacing = 2;
    static constexpr int horizontal_border_spacing = 2;

    // Accessors
    FPoint              getViewportCursorPos();

    // Methods
    void                init (FWidget*);
    void                init_scrollbar();
    void                calculateScrollbarPos();
    void                setHorizontalScrollBarVisibility();
    void                setVerticalScrollBarVisibility();
    void                setViewportCursor();
    void                redrawHBar();
    void                redrawVBar();
    void                drawHBar();
    void                drawVBar();

    // Callback methods
    void                cb_VBarChange (FWidget*, FDataPtr);
    void                cb_HBarChange (FWidget*, FDataPtr);

    // Data Members
    FRect             scroll_geometry{1, 1, 1, 1};
    FRect             viewport_geometry{};
    term_area*        viewport{nullptr};  // virtual scroll content
    FScrollbarPtr     vbar{nullptr};
    FScrollbarPtr     hbar{nullptr};
    uInt8             nf_offset{0};
    bool              border{true};
    bool              use_own_print_area{false};
    bool              update_scrollbar{true};
    fc::scrollBarMode vMode{fc::Auto};  // fc:Auto, fc::Hidden or fc::Scroll
    fc::scrollBarMode hMode{fc::Auto};
};
#pragma pack(pop)


// FScrollView inline functions
//----------------------------------------------------------------------
inline const char* FScrollView::getClassName() const
{ return "FScrollView"; }

//----------------------------------------------------------------------
inline std::size_t FScrollView::getViewportWidth() const
{ return getWidth() - vertical_border_spacing - std::size_t(nf_offset); }

//----------------------------------------------------------------------
inline std::size_t FScrollView::getViewportHeight() const
{ return getHeight() - horizontal_border_spacing; }

//----------------------------------------------------------------------
inline std::size_t FScrollView::getScrollWidth() const
{ return scroll_geometry.getWidth(); }

//----------------------------------------------------------------------
inline std::size_t FScrollView::getScrollHeight() const
{ return scroll_geometry.getHeight(); }

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

}  // namespace finalcut

#endif  // FSCROLLVIEW_H
