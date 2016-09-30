// File: fswitch.h
// Provides: class FSwitch
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
//    ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
//    ▕ FToggleButton ▏
//    ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
//            ▲
//            │
//       ▕▔▔▔▔▔▔▔▔▔▏
//       ▕ FSwitch ▏
//       ▕▁▁▁▁▁▁▁▁▁▏

#ifndef _FSWITCH_H
#define _FSWITCH_H

#include "ftogglebutton.h"


//----------------------------------------------------------------------
// class FSwitch
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FSwitch : public FToggleButton
{
 private:
   int  switch_offset_pos;
   bool button_pressed;

 private:
   // Disable copy constructor
   FSwitch (const FSwitch&);
   // Disable assignment operator (=)
   FSwitch& operator = (const FSwitch&);

   void draw();
   void drawCheckButton();

 public:
   // Constructors
   explicit FSwitch (FWidget* = 0);
   FSwitch (const FString&, FWidget* = 0);
   // Destructor
   virtual ~FSwitch();

   const char* getClassName() const;
   void setText (FString);

   // Event handlers
   void onKeyPress (FKeyEvent*);
   void onMouseDown (FMouseEvent*);
   void onMouseUp (FMouseEvent*);
};
#pragma pack(pop)


// FSwitch inline functions
//----------------------------------------------------------------------
inline const char* FSwitch::getClassName() const
{ return "FSwitch"; }

#endif  // _FSWITCH_H
