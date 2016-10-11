// File: fcheckbox.h
// Provides: class FCheckBox
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
//      ▕▔▔▔▔▔▔▔▔▔▔▔▏
//      ▕ FCheckBox ▏
//      ▕▁▁▁▁▁▁▁▁▁▁▁▏

#ifndef _FCHECKBOX_H
#define _FCHECKBOX_H

#include "ftogglebutton.h"


//----------------------------------------------------------------------
// class FCheckBox
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FCheckBox : public FToggleButton
{
 private:
   // Disable copy constructor
   FCheckBox (const FCheckBox&);
   // Disable assignment operator (=)
   FCheckBox& operator = (const FCheckBox&);

   void init();
   void draw();
   void drawCheckButton();

 public:
   // Constructors
   explicit FCheckBox (FWidget* = 0);
   FCheckBox (const FString&, FWidget* = 0);
   // Destructor
   virtual ~FCheckBox();

   const char* getClassName() const;
};
#pragma pack(pop)


// FCheckBox inline functions
//----------------------------------------------------------------------
inline const char* FCheckBox::getClassName() const
{ return "FCheckBox"; }

#endif  // _FCHECKBOX_H
