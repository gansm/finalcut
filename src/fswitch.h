// fswitch.h
// class FSwitch

#ifndef _FSWITCH_H
#define _FSWITCH_H

#include "ftogglebutton.h"


//----------------------------------------------------------------------
// class FSwitch
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FSwitch : public FToggleButton
{
 private:
   int switch_offset_pos;

 private:
   FSwitch (const FSwitch&);
   FSwitch& operator = (const FSwitch&);
   void draw();
   void drawCheckButton();

 public:
   explicit FSwitch (FWidget* parent=0);  // constructor
   FSwitch (const FString&, FWidget* parent=0);  // constructor
   virtual ~FSwitch();  // destructor
   const char* getClassName() const;
   void onKeyPress (FKeyEvent*);
   void setText (FString);
};
#pragma pack(pop)


// FSwitch inline functions
//----------------------------------------------------------------------
inline const char* FSwitch::getClassName() const
{ return "FSwitch"; }

#endif  // _FSWITCH_H
