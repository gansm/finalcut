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
   FDialogListMenu (const FDialogListMenu&);
   FDialogListMenu& operator = (const FDialogListMenu&);
   void init();

 public:
   explicit FDialogListMenu (FWidget* = 0);  // constructor
   FDialogListMenu (FString&, FWidget* = 0);
   FDialogListMenu (const std::string&, FWidget* = 0);
   FDialogListMenu (const char*, FWidget* = 0);
   virtual ~FDialogListMenu();  // destructor
   virtual const char* getClassName() const;
};
#pragma pack(pop)


// FDialogListMenu inline functions
//----------------------------------------------------------------------
inline const char* FDialogListMenu::getClassName() const
{ return "FDialogListMenu"; }

#endif  // _FDIALOGLISTMENU_H
