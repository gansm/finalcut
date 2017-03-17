// File: fradiomenuitem.h
// Provides: class FRadioMenuItem
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
//      ▕ FMenuItem ▏
//      ▕▁▁▁▁▁▁▁▁▁▁▁▏
//            ▲
//            │
//    ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏*       1▕▔▔▔▔▔▔▔▏
//    ▕ FRadioMenuItem ▏- - - - -▕ FMenu ▏
//    ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏         ▕▁▁▁▁▁▁▁▏

#ifndef _FRADIOMENUITEM_H
#define _FRADIOMENUITEM_H

#include "fmenuitem.h"


//----------------------------------------------------------------------
// class FRadioMenuItem
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FRadioMenuItem : public FMenuItem
{
 public:
   // Constructors
   explicit FRadioMenuItem (FWidget* = 0);
   FRadioMenuItem (const FString&, FWidget* = 0);

   // Destructor
   virtual ~FRadioMenuItem();

   // Accessor
   const char* getClassName() const;

 private:
   // Disable copy constructor
   FRadioMenuItem (const FRadioMenuItem&);

   // Disable assignment operator (=)
   FRadioMenuItem& operator = (const FRadioMenuItem&);

   // Methods
   void init (FWidget*);
   void processToggle();
   void processClicked();
};
#pragma pack(pop)


// FRadioMenuItem inline functions
//----------------------------------------------------------------------
inline const char* FRadioMenuItem::getClassName() const
{ return "FRadioMenuItem"; }

#endif  // _FRADIOMENUITEM_H
