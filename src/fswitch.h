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
 public:
   // Constructors
   explicit FSwitch (FWidget* = 0);
   FSwitch (const FString&, FWidget* = 0);

   // Destructor
   virtual ~FSwitch();

   // Accessor
   const char* getClassName() const;

   // Mutator
   void setText (const FString&);

   // Event handlers
   void onKeyPress (FKeyEvent*);
   void onMouseDown (FMouseEvent*);
   void onMouseUp (FMouseEvent*);

 private:
   // Disable copy constructor
   FSwitch (const FSwitch&);

   // Disable assignment operator (=)
   FSwitch& operator = (const FSwitch&);

   // Methods
   void draw();
   void drawCheckButton();

   // Data Members
   int  switch_offset_pos;
   bool button_pressed;
};
#pragma pack(pop)


// FSwitch inline functions
//----------------------------------------------------------------------
inline const char* FSwitch::getClassName() const
{ return "FSwitch"; }

#endif  // _FSWITCH_H
