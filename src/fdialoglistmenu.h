// File: fdialoglistmenu.h
// Provides: class FDialogListMenu
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
//       ▕▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▔▔▏
//       ▕ FWindow ▏ ▕ FMenuList ▏
//       ▕▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▁▁▏
//            ▲           ▲
//            │           │
//            └─────┬─────┘
//                  │
//              ▕▔▔▔▔▔▔▔▏
//              ▕ FMenu ▏
//              ▕▁▁▁▁▁▁▁▏
//                  ▲
//                  │
//         ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏           ▕▔▔▔▔▔▔▔▔▔▔▔▏
//         ▕ FDialogListMenu ▏- - - - - -▕ FMenuItem ▏
//         ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏1         1▕▁▁▁▁▁▁▁▁▁▁▁▏

#ifndef _FDIALOGLISTMENU_H
#define _FDIALOGLISTMENU_H

#include "fmenu.h"


//----------------------------------------------------------------------
// class FDialogListMenu
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FDialogListMenu : public FMenu
{
 private:
   // Disable copy constructor
   FDialogListMenu (const FDialogListMenu&);
   // Disable assignment operator (=)
   FDialogListMenu& operator = (const FDialogListMenu&);

   void init();

 public:
   // Constructors
   explicit FDialogListMenu (FWidget* = 0);
   FDialogListMenu (FString&, FWidget* = 0);
   FDialogListMenu (const std::string&, FWidget* = 0);
   FDialogListMenu (const char*, FWidget* = 0);
   // Destructor
   virtual ~FDialogListMenu();

   virtual const char* getClassName() const;
};
#pragma pack(pop)


// FDialogListMenu inline functions
//----------------------------------------------------------------------
inline const char* FDialogListMenu::getClassName() const
{ return "FDialogListMenu"; }

#endif  // _FDIALOGLISTMENU_H
