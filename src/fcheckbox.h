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

#ifndef FCHECKBOX_H
#define FCHECKBOX_H

#include "ftogglebutton.h"


//----------------------------------------------------------------------
// class FCheckBox
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FCheckBox : public FToggleButton
{
 public:
   // Constructors
   explicit FCheckBox (FWidget* = 0);
   FCheckBox (const FString&, FWidget* = 0);

   // Destructor
   virtual ~FCheckBox();

   // Accessor
   const char* getClassName() const;

 private:
   // Disable copy constructor
   FCheckBox (const FCheckBox&);

   // Disable assignment operator (=)
   FCheckBox& operator = (const FCheckBox&);

   // Methods
   void init();
   void draw();
   void drawCheckButton();
};
#pragma pack(pop)


// FCheckBox inline functions
//----------------------------------------------------------------------
inline const char* FCheckBox::getClassName() const
{ return "FCheckBox"; }

#endif  // FCHECKBOX_H
