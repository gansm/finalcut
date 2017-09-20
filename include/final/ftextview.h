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

#ifndef FTEXTVIEW_H
#define FTEXTVIEW_H

#include <vector>

#include "final/fapplication.h"
#include "final/fscrollbar.h"
#include "final/fstatusbar.h"
#include "final/fstring.h"
#include "final/fwidget.h"


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

    // Constructor
    explicit FTextView (FWidget* = 0);

    // Destructor
    ~FTextView();

    // Accessors
    const char*        getClassName() const;
    uInt               getColumns() const;
    uInt               getRows() const;
    const FString      getText() const;
    const FStringList& getLines() const;

    // Mutators
    void               setGeometry (int, int, int, int, bool = true);
    void               setPosition (int);
    void               setText (const FString&);

    // Methods
    void               hide();
    void               append (const FString&);
    void               insert (const FString&, int);
    void               replaceRange (const FString&, int, int);
    void               deleteRange (int, int);
    void               deleteLine (int);
    void               clear();

    // Event handlers
    void               onKeyPress (FKeyEvent*);
    void               onMouseDown (FMouseEvent*);
    void               onMouseUp (FMouseEvent*);
    void               onMouseMove (FMouseEvent*);
    void               onWheel (FWheelEvent*);
    void               onFocusIn (FFocusEvent*);
    void               onFocusOut (FFocusEvent*);

  protected:
    // Method
    void               adjustSize();

  private:
    // Disable copy constructor
    FTextView (const FTextView&);

    // Disable assignment operator (=)
    FTextView& operator = (const FTextView&);

    // Methods
    void               init();
    void               draw();
    void               drawText();
    void               processChanged();

    // Callback methods
    void               cb_VBarChange (FWidget*, data_ptr);
    void               cb_HBarChange (FWidget*, data_ptr);

    // Data Members
    FStringList        data;
    FScrollbar*        vbar;
    FScrollbar*        hbar;
    int                xoffset;
    int                yoffset;
    int                nf_offset;
    uInt               maxLineWidth;
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
inline const FStringList& FTextView::getLines() const
{ return data; }

//----------------------------------------------------------------------
inline void FTextView::deleteRange (int from, int to)
{ replaceRange (FString(), from, to); }

//----------------------------------------------------------------------
inline void FTextView::deleteLine (int pos)
{ deleteRange (pos, pos); }

#endif  // FTEXTVIEW_H
