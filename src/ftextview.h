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
 public:
   // Using-declarations
   using FWidget::setGeometry;

   // Typedef
   typedef std::vector<FString> stringLines;

   // Constructor
   explicit FTextView (FWidget* = 0);

   // Destructor
  ~FTextView();

   // Accessors
   const char* getClassName() const;
   uInt        getColumns() const;
   uInt        getRows() const;
   FString     getText() const;
   stringLines getLines() const;

   // Mutators
   void        setGeometry (int, int, int, int, bool = true);
   void        setPosition (int);
   void        setText (const FString&);

   // Methods
   void        hide();
   void        append (const FString&);
   void        insert (const FString&, int);
   void        replaceRange (const FString&, int, int);
   void        deleteRange (int, int);
   void        deleteLine (int);
   void        clear();

   // Event handlers
   void        onKeyPress (FKeyEvent*);
   void        onMouseDown (FMouseEvent*);
   void        onMouseUp (FMouseEvent*);
   void        onMouseMove (FMouseEvent*);
   void        onWheel (FWheelEvent*);
   void        onFocusIn (FFocusEvent*);
   void        onFocusOut (FFocusEvent*);

 protected:
   // Method
   void adjustSize();

 private:
   // Disable copy constructor
   FTextView (const FTextView&);

   // Disable assignment operator (=)
   FTextView& operator = (const FTextView&);

   // Methods
   void        init();
   void        draw();
   void        drawText();
   void        processChanged();

   // Callback methods
   void        cb_VBarChange (FWidget*, data_ptr);
   void        cb_HBarChange (FWidget*, data_ptr);

   // Data Members
   stringLines data;
   FScrollbar* vbar;
   FScrollbar* hbar;
   int         xoffset;
   int         yoffset;
   int         nf_offset;
   uInt        maxLineWidth;
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
inline FTextView::stringLines FTextView::getLines() const
{ return data; }

//----------------------------------------------------------------------
inline void FTextView::deleteRange (int start, int end)
{ replaceRange (FString(), start, end); }

//----------------------------------------------------------------------
inline void FTextView::deleteLine (int pos)
{ deleteRange (pos, pos); }

#endif  // _FTEXTVIEW_H
