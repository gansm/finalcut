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

#include "fwidget.h"


class FToggleButton;

//----------------------------------------------------------------------
// class FButtonGroup
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FButtonGroup : public FWidget
{
 private:
   FString text;
   bool border;
   typedef std::vector<FToggleButton*> FButtonList;
   FButtonGroup::FButtonList buttonlist;

 private:
   FButtonGroup (const FButtonGroup&);
   FButtonGroup& operator = (const FButtonGroup&);
   void init();
   bool isRadioButton(FToggleButton*) const;
   void directFocus();

 protected:
   virtual void draw();
   uChar getHotkey();
   void  setHotkeyAccelerator();
   void  drawLabel();

 public:
   explicit FButtonGroup (FWidget* = 0);  // constructor
   FButtonGroup (const FString&, FWidget* = 0);  // constructor
   virtual ~FButtonGroup();  // destructor
   const char* getClassName() const;

   void hide();

   FToggleButton* getFirstButton();
   FToggleButton* getLastButton();
   bool hasFocusedButton();
   bool hasCheckedButton();
   void onMouseDown (FMouseEvent*);
   void onAccel (FAccelEvent*);
   void onFocusIn (FFocusEvent*);
   void onFocusOut (FFocusEvent*);

   void insert (FToggleButton*);
   void remove (FToggleButton*);

   void cb_buttonToggled (FWidget*, void*);

   bool setEnable(bool);
   bool setEnable();
   bool unsetEnable();
   bool setDisable();
   bool setBorder(bool);
   bool setBorder();
   bool unsetBorder();
   void setText (const FString&);
   FString& getText();
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
inline bool FButtonGroup::setBorder()
{ return setBorder(true); }

//----------------------------------------------------------------------
inline bool FButtonGroup::unsetBorder()
{ return setBorder(false); }

//----------------------------------------------------------------------
inline FString& FButtonGroup::getText()
{ return text; }

#endif  // _FBUTTONGROUP_H
