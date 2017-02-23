// File: fscrollview.h
// Provides: class FScrollView
//
//  Inheritance diagram
//  ═══════════════════
//
// ▕▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▏
// ▕ FObject ▏ ▕  FTerm  ▏
// ▕▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▏
//      ▲           ▲
//      │           │
//      └─────┬─────┘
//            │
//       ▕▔▔▔▔▔▔▔▔▏
//       ▕ FVTerm ▏
//       ▕▁▁▁▁▁▁▁▁▏
//            ▲
//            │
//       ▕▔▔▔▔▔▔▔▔▔▏
//       ▕ FWidget ▏
//       ▕▁▁▁▁▁▁▁▁▁▏
//            ▲
//            │
//     ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
//     ▕ FScrollView ▏
//     ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏


#ifndef _FSCROLLVIEW_H
#define _FSCROLLVIEW_H

#include "fscrollbar.h"
#include "fwidget.h"


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
   explicit FScrollView (FWidget* = 0);

   // Destructor
   virtual ~FScrollView();

   // Accessors
   const char*       getClassName() const;
   int               getViewportWidth() const;
   int               getViewportHeight() const;
   int               getScrollWidth() const;
   int               getScrollHeight() const;
   const FPoint      getScrollPos() const;
   int               getScrollX() const;
   int               getScrollY() const;

   // Mutator
   virtual void      setScrollWidth (int);
   virtual void      setScrollHeight (int);
   virtual void      setScrollSize (int, int);
   virtual void      setX (int, bool = true);
   virtual void      setY (int, bool = true);
   virtual void      setPos (int, int, bool = true);
   virtual void      setWidth (int, bool = true);
   virtual void      setHeight (int, bool = true);
   virtual void      setSize (int, int, bool = true);
   void              setGeometry (int, int, int, int, bool = true);
   void              setCursorPos (register int, register int);
   void              setPrintPos (register int, register int);
   bool              setViewportPrint (bool);
   bool              setViewportPrint();
   bool              unsetViewportPrint();
   bool              setBorder (bool);
   bool              setBorder();
   bool              unsetBorder();
   void              setHorizontalScrollBarMode (fc::scrollBarMode);
   void              setVerticalScrollBarMode (fc::scrollBarMode);

   // Inquiries
   bool              hasBorder();
   bool              isViewportPrint();

   // Method
   virtual void      clearArea (int = ' ');
   void              scrollToX (int);
   void              scrollToY (int);
   void              scrollTo (FPoint);
   void              scrollTo (int, int);
   void              scrollBy (int, int);
   virtual void      draw();

   // Event handlers
   void              onKeyPress (FKeyEvent*);
   void              onWheel (FWheelEvent*);
   void              onFocusIn (FFocusEvent*);
   void              onChildFocusIn (FFocusEvent*);
   void              onChildFocusOut (FFocusEvent*);

 protected:
   // Using-declaration
   using FVTerm::clearArea;

   // Accessor
   term_area*        getPrintArea();

   // Method
   void              adjustSize();
   void              copy2area();

 private:
   // Constants
   static const int vertical_border_spacing = 2;
   static const int horizontal_border_spacing = 2;

   // Disable copy constructor
   FScrollView (const FScrollView&);

   // Disable assignment operator (=)
   FScrollView& operator = (const FScrollView&);

   // Methods
   void              init();
   void              calculateScrollbarPos();
   void              setHorizontalScrollBarVisibility();
   void              setVerticalScrollBarVisibility();
   void              redrawHBar();
   void              redrawVBar();
   void              drawHBar();
   void              drawVBar();

   // Callback methods
   void              cb_VBarChange (FWidget*, data_ptr);
   void              cb_HBarChange (FWidget*, data_ptr);

   // Data Members
   FRect             scroll_geometry;
   FRect             viewport_geometry;
   term_area*        viewport;  // virtual scroll content
   FScrollbar*       vbar;
   FScrollbar*       hbar;
   int               nf_offset;
   bool              border;
   bool              use_own_print_area;
   fc::scrollBarMode vMode;  // fc:Auto, fc::Hidden or fc::Scroll
   fc::scrollBarMode hMode;
};
#pragma pack(pop)


// FScrollView inline functions
//----------------------------------------------------------------------
inline const char* FScrollView::getClassName() const
{ return "FScrollView"; }

//----------------------------------------------------------------------
inline int FScrollView::getViewportWidth() const
{ return getWidth() - vertical_border_spacing - nf_offset; }

//----------------------------------------------------------------------
inline int FScrollView::getViewportHeight() const
{ return getHeight() - horizontal_border_spacing; }

//----------------------------------------------------------------------
inline int FScrollView::getScrollWidth() const
{ return scroll_geometry.getWidth(); }

//----------------------------------------------------------------------
inline int FScrollView::getScrollHeight() const
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
inline void FScrollView::scrollTo (FPoint pos)
{ scrollTo(pos.getX(), pos.getY()); }


#endif  // _FSCROLLVIEW_H
