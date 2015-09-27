// File: fmenubar.h
// Provides: class FMenuBar
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
//       ▕▔▔▔▔▔▔▔▔▔▏
//       ▕ FWidget ▏
//       ▕▁▁▁▁▁▁▁▁▁▏
//            ▲
//            │
//       ▕▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▔▔▏1     *▕▔▔▔▔▔▔▔▔▔▔▔▏
//       ▕ FWindow ▏ ▕ FMenuList ▏- - - -▕ FMenuItem ▏
//       ▕▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▁▁▏       ▕▁▁▁▁▁▁▁▁▁▁▁▏
//            ▲           ▲
//            │           │
//            └─────┬─────┘
//                  │
//            ▕▔▔▔▔▔▔▔▔▔▔▏
//            ▕ FMenuBar ▏
//            ▕▁▁▁▁▁▁▁▁▁▁▏

#ifndef _FMENUBAR_H
#define _FMENUBAR_H

#include "fwindow.h"
#include "fmenulist.h"


//----------------------------------------------------------------------
// class FMenuBar
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FMenuBar : public FWindow, public FMenuList
{
 private:
   bool   mouse_down;
   FPoint next_item_pos;
   int    x;

 private:
   FMenuBar (const FMenuBar&);
   FMenuBar& operator = (const FMenuBar&);
   void init();
   bool isMenu (FMenuItem*) const;
   int  getHotkeyPos (wchar_t*&, wchar_t*&, uInt);
   void draw();
   void drawItems();
   void adjustSize();

 public:
   explicit FMenuBar (FWidget* parent=0);  // constructor
   virtual ~FMenuBar();  // destructor
   virtual const char* getClassName() const;

   void onMouseDown (FMouseEvent*);
   void onMouseUp (FMouseEvent*);
   void onMouseMove (FMouseEvent*);
   void hide();
   // make every setGeometry from FWidget available
   using FWidget::setGeometry;
   void setGeometry (int, int, int, int, bool adjust=true);
   void cb_item_activated (FWidget*, void*);
};
#pragma pack(pop)


// FMenuBar inline functions
//----------------------------------------------------------------------
inline const char* FMenuBar::getClassName() const
{ return "FMenuBar"; }

#endif  // _FMENUBAR_H
