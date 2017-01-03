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
   using FWidget::setPos;

   // Constructor
   explicit FScrollView (FWidget* = 0);

   // Destructor
   virtual ~FScrollView();

   // Accessors
   const char*       getClassName() const;
   int               getScrollWidth();
   int               getScrollHeight();

   // Mutator
   void              setScrollWidth (int);
   void              setScrollHeight (int);
   void              setScrollSize (int, int);
   void              setScrollOffset (FPoint);
   void              setScrollOffset (int, int);
   virtual void      setX (int, bool = true);
   virtual void      setY (int, bool = true);
   virtual void      setPos (int, int, bool = true);
   virtual void      setWidth (int, bool = true);
   virtual void      setHeight (int, bool = true);
   virtual void      setSize (int, int, bool = true);
   void              setGeometry (int, int, int, int, bool = true);
   void              setHorizontalScrollBarMode (fc::scrollBarMode);
   void              setVerticalScrollBarMode (fc::scrollBarMode);

   // Method
   virtual void      clearArea (int = ' ');
   virtual void      draw();

   // Event handlers
   void              onWheel (FWheelEvent*);

 protected:
   // Using-declaration
   using FVTerm::clearArea;

   // Accessor
   term_area*        getPrintArea();

   // Method
   void              adjustSize();
   void              copy2area();

 private:
   // Disable copy constructor
   FScrollView (const FScrollView&);

   // Disable assignment operator (=)
   FScrollView& operator = (const FScrollView&);

   // Methods
   void              init();
   void              calculateScrollbarPos();
   void              setHorizontalScrollBarVisibility();
   void              setVerticalScrollBarVisibility();

   // Callback methods
   void              cb_VBarChange (FWidget*, void*);
   void              cb_HBarChange (FWidget*, void*);

   // Data Members
   FRect             scroll_size;
   FPoint            scroll_offset;
   term_area*        viewport;  // virtual scroll content
   FScrollbar*       vbar;
   FScrollbar*       hbar;
   int               nf_offset;
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
inline int FScrollView::getScrollWidth()
{ return scroll_size.getWidth(); }

//----------------------------------------------------------------------
inline int FScrollView::getScrollHeight()
{ return scroll_size.getHeight(); }

//----------------------------------------------------------------------
inline void FScrollView::setScrollOffset (FPoint pos)
{ scroll_offset = pos; }

//----------------------------------------------------------------------
inline void FScrollView::setScrollOffset (int x, int y)
{ scroll_offset.setPoint (x, y); }

#endif  // _FSCROLLVIEW_H
