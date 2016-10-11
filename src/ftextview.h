// File: ftextview.h
// Provides: class FTextView
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
//      ▕▔▔▔▔▔▔▔▔▔▔▔▏
//      ▕ FTextView ▏
//      ▕▁▁▁▁▁▁▁▁▁▁▁▏

#ifndef _FTEXTVIEW_H
#define _FTEXTVIEW_H

#include "fapp.h"
#include "fscrollbar.h"
#include "fstatusbar.h"
#include "fstring.h"
#include "fwidget.h"


//----------------------------------------------------------------------
// class FTextView
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FTextView : public FWidget
{
 private:
   typedef std::vector<FString> stringLines;
   stringLines data;
   FScrollbar* vbar;
   FScrollbar* hbar;
   int         xoffset;
   int         yoffset;
   int         nf_offset;
   uInt        maxLineWidth;

 private:
   // Disable copy constructor
   FTextView (const FTextView&);
   // Disable assignment operator (=)
   FTextView& operator = (const FTextView&);

   void        init();
   void        draw();
   void        drawText();
   void        processChanged();

   // Callback methods
   void        cb_VBarChange (FWidget*, void*);
   void        cb_HBarChange (FWidget*, void*);

 protected:
   void adjustSize();

 public:
   // Constructor
   explicit FTextView (FWidget* = 0);
   // Destructor
  ~FTextView();

   const char* getClassName() const;
   void        hide();

   // Event handlers
   void        onKeyPress (FKeyEvent*);
   void        onMouseDown (FMouseEvent*);
   void        onMouseUp (FMouseEvent*);
   void        onMouseMove (FMouseEvent*);
   void        onWheel (FWheelEvent*);
   void        onFocusIn (FFocusEvent*);
   void        onFocusOut (FFocusEvent*);

   // make every setGeometry from FWidget available
   using FWidget::setGeometry;
   void        setGeometry (int, int, int, int, bool = true);
   uInt        getColumns() const;
   uInt        getRows() const;
   void        setPosition (int);
   void        setText (const FString&);
   FString     getText() const;
   void        append (const FString&);
   void        insert (const FString&, int);
   void        replaceRange (const FString&, int, int);
   void        deleteRange (int, int);
   void        deleteLine (int);
   stringLines getLines() const;
   void        clear();
};
#pragma pack(pop)


// FListBox inline functions
//----------------------------------------------------------------------
inline const char* FTextView::getClassName() const
{ return "FTextView"; }

//----------------------------------------------------------------------
inline uInt FTextView::getColumns() const
{ return maxLineWidth; }

//----------------------------------------------------------------------
inline uInt FTextView::getRows() const
{ return uInt(data.size()); }

//----------------------------------------------------------------------
inline void FTextView::deleteRange (int start, int end)
{ replaceRange (FString(), start, end); }

//----------------------------------------------------------------------
inline void FTextView::deleteLine (int pos)
{ deleteRange (pos, pos); }

//----------------------------------------------------------------------
inline FTextView::stringLines FTextView::getLines() const
{ return data; }

#endif  // _FTEXTVIEW_H
