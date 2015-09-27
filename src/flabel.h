// File: flabel.h
// Provides: class FLabel
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
//       ▕▔▔▔▔▔▔▔▔▏
//       ▕ FLabel ▏
//       ▕▁▁▁▁▁▁▁▁▏

#ifndef _FLABEL_H
#define _FLABEL_H

#include "fwidget.h"


#define NO_EMPHASIS  0x00000000
#define EMPHASIS     0x00000001


//----------------------------------------------------------------------
// class FLabel
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FLabel : public FWidget
{
 private:
   std::vector<FString> multiline_text;
   bool     multiline;
   FString  text;
   int      emphasis;
   uInt     alignment;
   uChar    emphasis_color;
   uChar    ellipsis_color;
   bool     reverse_mode;
   FWidget* accel_widget;

 private:
   FLabel (const FLabel&);
   FLabel& operator = (const FLabel&);

   void  init();
   uChar getHotkey();
   int   getHotkeyPos (wchar_t*&, wchar_t*&, uInt);
   void  setHotkeyAccelerator();
   int   getXOffset (int);
   void  printLine (wchar_t*&, uInt, int, int = 0);
   void  draw();

 public:
   explicit FLabel (FWidget* = 0);  // constructor
   FLabel (const FString&, FWidget* = 0);  // constructor
   virtual ~FLabel();  // destructor
   const char* getClassName() const;

   void hide();
   void onMouseDown (FMouseEvent*);
   void onAccel (FAccelEvent*);
   void cb_accel_widget_destroyed (FWidget*, void*);
   void setAccelWidget (FWidget* = 0);
   FTerm* getAccelWidget();
   void setAlignment(uInt);
   uInt getAlignment();
   bool setEmphasis(bool);
   bool setEmphasis();
   bool unsetEmphasis();
   bool hasEmphasis();
   bool setReverseMode(bool);
   bool setReverseMode();
   bool unsetReverseMode();
   bool hasReverseMode();
   using FWidget::setEnable;
   bool setEnable (bool);
   void setNumber(long);
   void setText (const FString&);
   FString& getText();
};
#pragma pack(pop)


// FLabel inline functions
//----------------------------------------------------------------------
inline const char* FLabel::getClassName() const
{ return "FLabel"; }

//----------------------------------------------------------------------
inline FTerm* FLabel::getAccelWidget ()
{ return accel_widget; }

//----------------------------------------------------------------------
inline uInt FLabel::getAlignment()
{ return alignment; }

//----------------------------------------------------------------------
inline bool FLabel::setEmphasis()
{ return setEmphasis(true); }

//----------------------------------------------------------------------
inline bool FLabel::unsetEmphasis()
{ return setEmphasis(false); }

//----------------------------------------------------------------------
inline bool FLabel::hasEmphasis()
{ return ((emphasis & EMPHASIS) != 0); }

//----------------------------------------------------------------------
inline bool FLabel::setReverseMode()
{ return setReverseMode(true); }

//----------------------------------------------------------------------
inline bool FLabel::unsetReverseMode()
{ return setReverseMode(false); }

//----------------------------------------------------------------------
inline bool FLabel::hasReverseMode()
{ return reverse_mode; }

//----------------------------------------------------------------------
inline FString& FLabel::getText()
{ return text; }

#endif  // _FLABEL_H
