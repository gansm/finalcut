// File: fcheckmenuitem.h
// Provides: class FCheckMenuItem
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
//    ▕ FCheckMenuItem ▏- - - - -▕ FMenu ▏
//    ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏         ▕▁▁▁▁▁▁▁▏

#ifndef _FCHECKMENUITEM_H
#define _FCHECKMENUITEM_H

#include "fmenuitem.h"


//----------------------------------------------------------------------
// class FCheckMenuItem
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FCheckMenuItem : public FMenuItem
{
 private:
   // Disable copy constructor
   FCheckMenuItem (const FCheckMenuItem&);
   // Disable assignment operator (=)
   FCheckMenuItem& operator = (const FCheckMenuItem&);

   void init (FWidget*);
   void processToggle();
   void processClicked();

 public:
   // Constructor
   explicit FCheckMenuItem (FWidget* = 0);
   FCheckMenuItem (FString&, FWidget* = 0);
   FCheckMenuItem (const std::string&, FWidget* = 0);
   FCheckMenuItem (const char*, FWidget* = 0);
   // Destructor
   virtual ~FCheckMenuItem();

   const char* getClassName() const;
};
#pragma pack(pop)


// FCheckMenuItem inline functions
//----------------------------------------------------------------------
inline const char* FCheckMenuItem::getClassName() const
{ return "FCheckMenuItem"; }

#endif  // _FCHECKMENUITEM_H
