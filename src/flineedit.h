// flineedit.h
// class FLineEdit

#ifndef _FLINEEDIT_H
#define _FLINEEDIT_H

#include "fwidget.h"
#include "flabel.h"

//----------------------------------------------------------------------
// class FLineEdit
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FLineEdit : public FWidget
{
 private:
   FString text;
   FString label_text;
   FLabel* label;
   enum drag_scroll
   {
     noScroll    = 0,
     scrollLeft  = 1,
     scrollRight = 2
   };
   int  dragScroll;
   bool scrollTimer;
   int  scrollRepeat;
   bool insert_mode;
   int  cursor_pos;
   int  offset;

 public:
   enum label_o
   {
     label_above = 0,
     label_left = 1
   };
   label_o label_orientation;

 private:
   FLineEdit (const FLineEdit&);
   FLineEdit& operator = (const FLineEdit&);
   void  init();
   bool  hasHotkey();
   void  draw();
   void  drawInputField();
   void  processActivate();
   void  processChanged();

 protected:
   void adjustLabel();
   void adjustSize();

 public:
   explicit FLineEdit (FWidget* parent=0);  // constructor
   FLineEdit (const FString&, FWidget* parent=0);  // constructor
   virtual ~FLineEdit();  // destructor
   const char* getClassName() const;

   void hide();

   bool setEnable(bool);
   bool setEnable();
   bool unsetEnable();
   bool setDisable();
   bool setFocus(bool);
   bool setFocus();
   bool unsetFocus();
   bool setShadow(bool);
   bool setShadow();
   bool unsetShadow();
   bool hasShadow();

   void onKeyPress (FKeyEvent*);
   void onMouseDown (FMouseEvent*);
   void onMouseUp (FMouseEvent*);
   void onMouseMove (FMouseEvent*);
   void onTimer (FTimerEvent*);
   void onAccel (FAccelEvent*);
   void onHide (FHideEvent*);
   void onFocusIn (FFocusEvent*);
   void onFocusOut (FFocusEvent*);

   void setText (FString);
   FString getText() const;
   void setLabelText (FString);
   void setLabelOrientation(label_o);
   int  getLabelOrientation();
};
#pragma pack(pop)


// FLineEdit inline functions
//----------------------------------------------------------------------
inline const char* FLineEdit::getClassName() const
{ return "FLineEdit"; }

//----------------------------------------------------------------------
inline bool FLineEdit::setEnable()
{ return setEnable(true); }

//----------------------------------------------------------------------
inline bool FLineEdit::unsetEnable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FLineEdit::setDisable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FLineEdit::setFocus()
{ return setFocus(true); }

//----------------------------------------------------------------------
inline bool FLineEdit::unsetFocus()
{ return setFocus(false); }

//----------------------------------------------------------------------
inline bool FLineEdit::setShadow()
{ return setShadow(true); }

//----------------------------------------------------------------------
inline bool FLineEdit::unsetShadow()
{ return setShadow(false); }

//----------------------------------------------------------------------
inline bool FLineEdit::hasShadow()
{ return ((flags & SHADOW) != 0); }

//----------------------------------------------------------------------
inline FString FLineEdit::getText() const
{ return text; }

//----------------------------------------------------------------------
inline int FLineEdit::getLabelOrientation()
{ return int(label_orientation); }

#endif  // _FLINEEDIT_H
