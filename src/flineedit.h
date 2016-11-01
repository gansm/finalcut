// File: flineedit.h
// Provides: class FLineEdit
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
//      ▕▔▔▔▔▔▔▔▔▔▔▔▏
//      ▕ FLineEdit ▏
//      ▕▁▁▁▁▁▁▁▁▁▁▁▏

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
 public:
   // Enumeration
   enum label_o
   {
     label_above = 0,
     label_left = 1
   };

   // Constructor
   explicit FLineEdit (FWidget* = 0);
   FLineEdit (const FString&, FWidget* = 0);

   // Destructor
   virtual ~FLineEdit();

   // Accessors
   const char* getClassName() const;
   FString     getText() const;
   int         getLabelOrientation();

   // Mutators
   void        setText (FString);
   void        setLabelText (FString);
   void        setLabelOrientation(label_o);
   bool        setEnable(bool);
   bool        setEnable();
   bool        unsetEnable();
   bool        setDisable();
   bool        setFocus(bool);
   bool        setFocus();
   bool        unsetFocus();
   bool        setShadow(bool);
   bool        setShadow();
   bool        unsetShadow();

   // Inquiry
   bool        hasShadow();

   // Methods
   void        hide();
   void        clearText();

   // Event handlers
   void        onKeyPress (FKeyEvent*);
   void        onMouseDown (FMouseEvent*);
   void        onMouseUp (FMouseEvent*);
   void        onMouseMove (FMouseEvent*);
   void        onTimer (FTimerEvent*);
   void        onAccel (FAccelEvent*);
   void        onHide (FHideEvent*);
   void        onFocusIn (FFocusEvent*);
   void        onFocusOut (FFocusEvent*);

 protected:
   void        adjustLabel();
   void        adjustSize();

 private:
   // Enumeration
   enum dragScroll
   {
     noScroll    = 0,
     scrollLeft  = 1,
     scrollRight = 2
   };

   // Disable copy constructor
   FLineEdit (const FLineEdit&);

   // Disable assignment operator (=)
   FLineEdit& operator = (const FLineEdit&);

   // Methods
   void        init();
   bool        hasHotkey();
   void        draw();
   void        drawInputField();
   void        processActivate();
   void        processChanged();

   // Data Members
   FString     text;
   FString     label_text;
   FLabel*     label;
   label_o     label_orientation;
   dragScroll  drag_scroll;
   bool        scroll_timer;
   int         scroll_repeat;
   bool        insert_mode;
   int         cursor_pos;
   int         text_offset;
};
#pragma pack(pop)


// FLineEdit inline functions
//----------------------------------------------------------------------
inline const char* FLineEdit::getClassName() const
{ return "FLineEdit"; }

//----------------------------------------------------------------------
inline FString FLineEdit::getText() const
{ return text; }

//----------------------------------------------------------------------
inline int FLineEdit::getLabelOrientation()
{ return int(label_orientation); }

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
{ return ((flags & fc::shadow) != 0); }

#endif  // _FLINEEDIT_H
