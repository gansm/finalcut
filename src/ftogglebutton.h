// ftogglebutton.h
// class FToggleButton.h

#ifndef _FTOGGLEBUTTON_H
#define _FTOGGLEBUTTON_H

#include "fwidget.h"


class FButtonGroup;

//----------------------------------------------------------------------
// class FToggleButton - abstract class for FToggleButton + FCheckBox
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FToggleButton : public FWidget
{
 private:
  bool focus_inside_group;

 protected:
   FString text;
   bool checked;
   FButtonGroup* button_group;

 private:
   FToggleButton (const FToggleButton&);
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
   virtual void onKeyPress (FKeyEvent*);

 public:
   explicit FToggleButton (FWidget* parent=0);  // constructor
   FToggleButton (const FString&, FWidget* parent=0);  // constructor
   virtual ~FToggleButton();  // destructor
   virtual const char* getClassName() const;

   void hide();
   void setGeometry (int, int, int, int, bool adjust=true);

   void onMouseDown (FMouseEvent*);
   void onMouseUp (FMouseEvent*);
   void onAccel (FAccelEvent*);
   void onHide (FHideEvent*);
   void onFocusIn (FFocusEvent*);
   void onFocusOut (FFocusEvent*);

   bool setNoUnderline(bool);
   bool setNoUnderline();
   bool unsetNoUnderline();

   bool setEnable(bool);
   bool setEnable();
   bool unsetEnable();
   bool setDisable();
   bool setFocus(bool);
   bool setFocus();
   bool unsetFocus();
   bool setChecked(bool);
   bool setChecked();
   bool unsetChecked();
   bool isChecked();

   void setText (const FString);
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
{ return this->text; }

#endif  // _FTOGGLEBUTTON_H
