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

#include "fmenu.h"
#include "fmenulist.h"
#include "fwindow.h"


//----------------------------------------------------------------------
// class FMenuBar
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FMenuBar : public FWindow, public FMenuList
{
 public:
   // Constructor
   explicit FMenuBar (FWidget* = 0);

   // Destructor
   virtual ~FMenuBar();

   // Accessors
   virtual const char* getClassName() const;

   // Methods
   void                hide();
   void                resetMenu();
   void                adjustSize();

   // Event handlers
   void                onKeyPress (FKeyEvent*);
   void                onMouseDown (FMouseEvent*);
   void                onMouseUp (FMouseEvent*);
   void                onMouseMove (FMouseEvent*);
   void                onAccel (FAccelEvent*);

   // Callback methods
   void                cb_item_deactivated (FWidget*, void*);

 private:
   // Disable copy constructor
   FMenuBar (const FMenuBar&);

   // Disable assignment operator (=)
   FMenuBar& operator  = (const FMenuBar&);

   // Inquiry
   bool                isMenu (FMenuItem*) const;

   // Methods
   void                init();
   void                calculateDimensions();
   bool                selectNextItem();
   bool                selectPrevItem();
   bool                hotkeyMenu (FKeyEvent*&);
   int                 getHotkeyPos (wchar_t*&, wchar_t*&, uInt);
   void                draw();
   void                drawItems();
   void                adjustItems();
   void                leaveMenuBar();

   // Friend classes
   friend class FMenu;
   friend class FMenuItem;

   // Data Members
   bool   mouse_down;
   bool   drop_down;
};
#pragma pack(pop)


// FMenuBar inline functions
//----------------------------------------------------------------------
inline const char* FMenuBar::getClassName() const
{ return "FMenuBar"; }

//----------------------------------------------------------------------
inline bool FMenuBar::isMenu (FMenuItem* mi) const
{ return mi->hasMenu(); }

#endif  // _FMENUBAR_H
