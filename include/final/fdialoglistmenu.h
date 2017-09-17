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

#ifndef FDIALOGLISTMENU_H
#define FDIALOGLISTMENU_H

#include "final/fmenu.h"


//----------------------------------------------------------------------
// class FDialogListMenu
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FDialogListMenu : public FMenu
{
  public:
    // Constructors
    explicit FDialogListMenu (FWidget* = 0);
    FDialogListMenu (const FString&, FWidget* = 0);

    // Destructor
    virtual ~FDialogListMenu();

    // Accessors
    virtual const char* getClassName() const;

  private:
    // Disable copy constructor
    FDialogListMenu (const FDialogListMenu&);

    // Disable assignment operator (=)
    FDialogListMenu& operator = (const FDialogListMenu&);

    // Method
    void init();
};
#pragma pack(pop)


// FDialogListMenu inline functions
//----------------------------------------------------------------------
inline const char* FDialogListMenu::getClassName() const
{ return "FDialogListMenu"; }

#endif  // FDIALOGLISTMENU_H
