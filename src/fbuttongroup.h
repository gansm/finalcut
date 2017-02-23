// File: fbuttongroup.h
// Provides: class FButtonGroup
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
//     ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
//     ▕ FButtonGroup ▏
//     ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏

#ifndef _FBUTTONGROUP_H
#define _FBUTTONGROUP_H

#include "fscrollview.h"


// class forward declaration
class FToggleButton;

//----------------------------------------------------------------------
// class FButtonGroup
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FButtonGroup : public FScrollView
{
 public:
   // Constructors
   explicit FButtonGroup (FWidget* = 0);
   FButtonGroup (const FString&, FWidget* = 0);

   // Destructor
   virtual ~FButtonGroup();

   // Accessor
   const char*    getClassName() const;
   FToggleButton* getFirstButton();
   FToggleButton* getLastButton();
   FString&       getText();

   // Mutator
   bool           setEnable(bool);
   bool           setEnable();
   bool           unsetEnable();
   bool           setDisable();
   void           setText (const FString&);

   // Inquiries
   bool           hasFocusedButton();
   bool           hasCheckedButton();

   // Methods
   void           hide();
   void           insert (FToggleButton*);
   void           remove (FToggleButton*);

   // Event handlers
   void           onMouseDown (FMouseEvent*);
   void           onAccel (FAccelEvent*);
   void           onFocusIn (FFocusEvent*);

   // Callback method
   void           cb_buttonToggled (FWidget*, data_ptr);

 protected:
   // Accessor
   uChar          getHotkey();

   // Mutator
   void           setHotkeyAccelerator();

   // Methods
   virtual void   draw();
   void           drawLabel();

 private:
   // Disable copy constructor
   FButtonGroup (const FButtonGroup&);

   // Disable assignment operator (=)
   FButtonGroup& operator = (const FButtonGroup&);

   // Inquiries
   bool isRadioButton (FToggleButton*) const;

   // Methods
   void init();
   void directFocus();

   // Data Members
   FString     text;
   FObjectList buttonlist;
};
#pragma pack(pop)


// FButtonGroup inline functions
//----------------------------------------------------------------------
inline const char* FButtonGroup::getClassName() const
{ return "FButtonGroup"; }

//----------------------------------------------------------------------
inline bool FButtonGroup::setEnable()
{ return setEnable(true); }

//----------------------------------------------------------------------
inline bool FButtonGroup::unsetEnable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FButtonGroup::setDisable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline FString& FButtonGroup::getText()
{ return text; }

#endif  // _FBUTTONGROUP_H
