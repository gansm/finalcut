// File: fradiobutton.h
// Provides: class FRadioButton
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
//     ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
//     ▕ FRadioButton ▏
//     ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏

#ifndef FRADIOBUTTON_H
#define FRADIOBUTTON_H

#include "final/ftogglebutton.h"


//----------------------------------------------------------------------
// class FRadioButton
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FRadioButton : public FToggleButton
{
  public:
    // Constructors
    explicit FRadioButton (FWidget* = 0);
    FRadioButton (const FString&, FWidget* = 0);

    // Destructor
    virtual ~FRadioButton();

    // Accessor
    const char* getClassName() const;

  private:
    // Disable copy constructor
    FRadioButton (const FRadioButton&);

    // Disable assignment operator (=)
    FRadioButton& operator = (const FRadioButton&);

    // Methods
    void init();
    void draw();
    void drawRadioButton();
};
#pragma pack(pop)


// FRadioButton inline functions
//----------------------------------------------------------------------
inline const char* FRadioButton::getClassName() const
{ return "FRadioButton"; }

#endif  // FRADIOBUTTON_H
