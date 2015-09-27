// File: fdialog.h
// Provides: class FDialog
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
//       ▕▔▔▔▔▔▔▔▔▔▏
//       ▕ FWindow ▏
//       ▕▁▁▁▁▁▁▁▁▁▏
//            ▲
//            │
//       ▕▔▔▔▔▔▔▔▔▔▏
//       ▕ FDialog ▏
//       ▕▁▁▁▁▁▁▁▁▁▏

#ifndef _FDIALOG_H
#define _FDIALOG_H

#include "fwindow.h"


//----------------------------------------------------------------------
// class FDialog
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FDialog : public FWindow
{
 public:
   enum DialogCode
   {
     Reject = 0,
     Accept = 1
   };

 private:
   FString tb_text;         // title bar text
   int  result_code;
   bool maximized;
   FPoint TitleBarClickPos;
   FRect  oldGeometry;      // required by move()
   FWidget* focus_widget;

 private:
   FDialog (const FDialog&);
   FDialog& operator = (const FDialog&);
   void init();
   void drawBorder();
   void drawTitleBar();

 protected:
   virtual void done (int);
   virtual void draw();
   virtual void onShow (FShowEvent*);
   virtual void onHide (FHideEvent*);
   virtual void onClose (FCloseEvent*);

 public:
   explicit FDialog (FWidget* = 0);  // constructor
   FDialog (const FString&, FWidget* = 0);  // constructor
   virtual ~FDialog();  // destructor
   virtual const char* getClassName() const;

   void     onKeyPress (FKeyEvent*);
   void     onMouseDown (FMouseEvent*);
   void     onMouseUp (FMouseEvent*);
   void     onMouseMove (FMouseEvent*);
   void     onMouseDoubleClick (FMouseEvent*);
   void     onWindowActive (FEvent*);
   void     onWindowInactive (FEvent*);
   void     onWindowRaised (FEvent*);
   void     onWindowLowered (FEvent*);

   void     drawDialogShadow();
   void     show();
   void     hide();
   int      exec();
   void     move (const FPoint&);
   void     move (int, int);
   void     setWidth (int, bool = true);
   void     setHeight (int, bool = true);
   // make every setGeometry from FWidget available
   using FWidget::setGeometry;
   void     setGeometry (int, int, int, int, bool = true);
   FWidget* getFocusWidget() const;
   void     setFocusWidget (FWidget*);
   bool     setFocus(bool);
   bool     setFocus();
   bool     unsetFocus();
   bool     setModal(bool);
   bool     setModal();
   bool     unsetModal();
   bool     isModal();
   bool     setScrollable(bool);
   bool     setScrollable();
   bool     unsetScrollable();
   bool     isScrollable();
   bool     setResizeable(bool);
   bool     setResizeable();
   bool     unsetResizeable();
   bool     isResizeable();
   bool     setMaximized();
   bool     isMaximized() const;
   bool     setTransparentShadow(bool);
   bool     setTransparentShadow();
   bool     unsetTransparentShadow();
   bool     hasTransparentShadow();
   bool     setShadow(bool);
   bool     setShadow();
   bool     unsetShadow();
   bool     hasShadow();
   FString  getText() const;
   void     setText (const FString&);
};
#pragma pack(pop)

// FDialog inline functions
//----------------------------------------------------------------------
inline const char* FDialog::getClassName() const
{ return "FDialog"; }

//----------------------------------------------------------------------
inline bool FDialog::setFocus()
{ return setFocus(true); }

//----------------------------------------------------------------------
inline bool FDialog::unsetFocus()
{ return setFocus(false); }

//----------------------------------------------------------------------
inline bool FDialog::setModal()
{ return setModal(true); }

//----------------------------------------------------------------------
inline bool FDialog::unsetModal()
{ return setModal(false); }

//----------------------------------------------------------------------
inline bool FDialog::isModal()
{ return ((flags & MODAL) != 0); }

//----------------------------------------------------------------------
inline bool FDialog::setScrollable()
{ return setScrollable(true); }

//----------------------------------------------------------------------
inline bool FDialog::unsetScrollable()
{ return setScrollable(false); }

//----------------------------------------------------------------------
inline bool FDialog::isScrollable()
{ return ((flags & SCROLLABLE) != 0); }

//----------------------------------------------------------------------
inline bool FDialog::setResizeable()
{ return setResizeable(true); }

//----------------------------------------------------------------------
inline bool FDialog::unsetResizeable()
{ return setResizeable(false); }

//----------------------------------------------------------------------
inline bool FDialog::isResizeable()
{ return ((flags & RESIZEABLE) != 0); }

//----------------------------------------------------------------------
inline bool FDialog::isMaximized() const
{ return maximized; }

//----------------------------------------------------------------------
inline bool FDialog::setTransparentShadow()
{ return setTransparentShadow(true); }

//----------------------------------------------------------------------
inline bool FDialog::unsetTransparentShadow()
{ return setTransparentShadow(false); }

//----------------------------------------------------------------------
inline bool FDialog::hasTransparentShadow()
{ return ((flags & TRANS_SHADOW) != 0); }

//----------------------------------------------------------------------
inline bool FDialog::setShadow()
{ return setShadow(true); }

//----------------------------------------------------------------------
inline bool FDialog::unsetShadow()
{ return setShadow(false); }

//----------------------------------------------------------------------
inline bool FDialog::hasShadow()
{ return ((flags & SHADOW) != 0); }

//----------------------------------------------------------------------
inline FString FDialog::getText() const
{ return tb_text; }

//----------------------------------------------------------------------
inline void FDialog::setText (const FString& txt)
{ tb_text = txt; }


#endif  // _FDIALOG_H
