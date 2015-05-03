// fradiobutton.h
// class FRadioButton

#ifndef _FRADIOBUTTON_H
#define _FRADIOBUTTON_H

#include "ftogglebutton.h"

//----------------------------------------------------------------------
// class FRadioButton
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FRadioButton : public FToggleButton
{
 private:
   FRadioButton (const FRadioButton&);
   FRadioButton& operator = (const FRadioButton&);
   void draw();
   void drawRadioButton();

 public:
   FRadioButton (FWidget* parent=0);  // constructor
   FRadioButton (const FString&, FWidget* parent=0);  // constructor
   virtual ~FRadioButton();  // destructor
   const char* getClassName() const;
};
#pragma pack(pop)


// FRadioButton inline functions
//----------------------------------------------------------------------
inline const char* FRadioButton::getClassName() const
{ return "FRadioButton"; }

#endif  // _FRADIOBUTTON_H
