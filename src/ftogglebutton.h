// File: ftogglebutton.h
// Provides: class FToggleButton.h
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
//    ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
//    ▕ FToggleButton ▏
//    ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏

#ifndef _FTOGGLEBUTTON_H
#define _FTOGGLEBUTTON_H

#include "fwidget.h"


class FButtonGroup;

//----------------------------------------------------------------------
// class FToggleButton - abstract class for FRadioButton, FCheckBox, ...
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FToggleButton : public FWidget
{
 private:
   bool focus_inside_group;

 protected:
   FString       text;
   bool          checked;
   int           label_offset_pos;
   int           button_width;  // plus margin spaces
   FButtonGroup* button_group;

 private:
   // Disable copy constructor
   FToggleButton (const FToggleButton&);
   // Disable assignment operator (=)
   FToggleButton& operator = (const FToggleButton&);

   void  init();
   friend class FButtonGroup;
   void setGroup (FButtonGroup*);

 protected:
   virtual void draw();
   uChar getHotkey();
   void  setHotkeyAccelerator();
   void  drawLabel();
   void  processClick();
   void  processToggle();
   FButtonGroup* group() const;
   bool  isRadioButton() const;
   bool  isCheckboxButton() const;
   virtual void onKeyPress (FKeyEvent*);

 public:
   // Constructors
   explicit FToggleButton (FWidget* = 0);
   FToggleButton (const FString&, FWidget* = 0);
   // Destructor
   virtual ~FToggleButton();

   virtual const char* getClassName() const;
   void hide();
   // make every setGeometry from FWidget available
   using FWidget::setGeometry;
   void setGeometry (int, int, int, int, bool = true);

   // Event handlers
   void onMouseDown (FMouseEvent*);
   void onMouseUp (FMouseEvent*);
   void onAccel (FAccelEvent*);
   void onHide (FHideEvent*);
   void onFocusIn (FFocusEvent*);
   void onFocusOut (FFocusEvent*);

   bool setNoUnderline (bool);
   bool setNoUnderline();
   bool unsetNoUnderline();
   bool setEnable (bool);
   bool setEnable();
   bool unsetEnable();
   bool setDisable();
   bool setFocus (bool);
   bool setFocus();
   bool unsetFocus();
   bool setChecked (bool);
   bool setChecked();
   bool unsetChecked();
   bool isChecked();

   virtual void setText (const FString);
   FString& getText();
};
#pragma pack(pop)


// FRadioButton inline functions
//----------------------------------------------------------------------
inline const char* FToggleButton::getClassName() const
{ return "FToggleButton"; }

//----------------------------------------------------------------------
inline bool FToggleButton::setNoUnderline()
{ return setNoUnderline(true); }

//----------------------------------------------------------------------
inline bool FToggleButton::unsetNoUnderline()
{ return setNoUnderline(false); }

//----------------------------------------------------------------------
inline bool FToggleButton::setEnable()
{ return setEnable(true); }

//----------------------------------------------------------------------
inline bool FToggleButton::unsetEnable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FToggleButton::setDisable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FToggleButton::setFocus()
{ return setFocus(true); }

//----------------------------------------------------------------------
inline bool FToggleButton::unsetFocus()
{ return setFocus(false); }

//----------------------------------------------------------------------
inline bool FToggleButton::setChecked()
{ return setChecked(true); }

//----------------------------------------------------------------------
inline bool FToggleButton::unsetChecked()
{ return setChecked(false); }

//----------------------------------------------------------------------
inline bool FToggleButton::isChecked()
{ return checked; }

//----------------------------------------------------------------------
inline FString& FToggleButton::getText()
{ return text; }

#endif  // _FTOGGLEBUTTON_H
