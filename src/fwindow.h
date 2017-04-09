// File: fwindow.h
// Provides: class FWindow
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
//       ▕▔▔▔▔▔▔▔▔▔▏1       *▕▔▔▔▔▔▔▔▔▏
//       ▕ FWindow ▏-┬- - - -▕ FEvent ▏
//       ▕▁▁▁▁▁▁▁▁▁▏ :       ▕▁▁▁▁▁▁▁▁▏
//                   :
//                   :      *▕▔▔▔▔▔▔▔▔▏
//                   :- - - -▕ FPoint ▏
//                   :       ▕▁▁▁▁▁▁▁▁▏
//                   :
//                   :      *▕▔▔▔▔▔▔▔▔▔▏
//                   └- - - -▕ FWidget ▏
//                           ▕▁▁▁▁▁▁▁▁▁▏

#ifndef FWINDOW_H
#define FWINDOW_H

#include "fwidget.h"


//----------------------------------------------------------------------
// class FWindow
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FWindow : public FWidget
{
 public:
   // Using-declaration
   using FWidget::drawBorder;
   using FWidget::setPos;
   using FWidget::setGeometry;
   using FWidget::move;

   // Constructor
   explicit FWindow (FWidget* = 0);

   // Destructor
  ~FWindow ();

   // Accessors
   const char*     getClassName() const;
   static FWindow* getWindowWidget (FWidget*);
   static int      getWindowLayer (FWidget*);
   static FWindow* getActiveWindow();
   FWidget*        getWindowFocusWidget() const;

   // Mutators
   bool            setWindowWidget (bool);
   bool            setWindowWidget();
   bool            unsetWindowWidget();
   static void     setActiveWindow (FWindow*);
   void            setWindowFocusWidget (FWidget*);
   bool            activateWindow (bool);
   bool            activateWindow();
   void            unsetActiveWindow();
   bool            deactivateWindow();
   virtual bool    setResizeable (bool);
   virtual bool    setResizeable();
   bool            unsetResizeable();
   bool            setTransparentShadow (bool);
   bool            setTransparentShadow();
   bool            unsetTransparentShadow();
   bool            setShadow (bool);
   bool            setShadow();
   bool            unsetShadow();
   bool            setAlwaysOnTop (bool);
   bool            setAlwaysOnTop();
   bool            unsetAlwaysOnTop();

   // Inquiries
   bool            isZoomed() const;
   bool            isWindowActive() const;
   bool            isWindowHidden() const;
   bool            isResizeable() const;
   bool            isAlwaysOnTop() const;
   bool            hasTransparentShadow() const;
   bool            hasShadow() const;

   // Methods
   virtual void    drawBorder();
   virtual void    show();
   virtual void    hide();
   virtual void    setX (int, bool = true);
   virtual void    setY (int, bool = true);
   virtual void    setPos (int, int, bool = true);
   virtual void    setWidth (int, bool = true);
   virtual void    setHeight (int, bool = true);
   virtual void    setSize (int, int, bool = true);
   void            setGeometry (int, int, int, int, bool = true);
   virtual void    move (int, int);
   static FWindow* getWindowWidgetAt (const FPoint&);
   static FWindow* getWindowWidgetAt (int, int);
   static void     addWindow (FWidget*);
   static void     delWindow (FWidget*);
   static void     swapWindow (FWidget*, FWidget*);
   static bool     raiseWindow (FWidget*);
   bool            raiseWindow ();
   static bool     lowerWindow (FWidget*);
   bool            lowerWindow ();
   bool            zoomWindow ();
   static void     switchToPrevWindow();
   static bool     activatePrevWindow();
   virtual void    setShadowSize (int, int);

 protected:
   // Method
   virtual void    adjustSize();

   // Event handlers
   bool            event (FEvent*);
   virtual void    onWindowActive (FEvent*);
   virtual void    onWindowInactive (FEvent*);
   virtual void    onWindowRaised (FEvent*);
   virtual void    onWindowLowered (FEvent*);

   // Data Members
   static FWindow* previous_window;

 private:
   // Disable copy constructor
   FWindow (const FWindow&);

   // Disable assignment operator (=)
   FWindow& operator = (const FWindow&);

   // Methods
   static void     deleteFromAlwaysOnTopList (FWidget*);
   static void     processAlwaysOnTop();

   // Data Members
   bool            window_active;
   bool            zoomed;
   FWidget*        win_focus_widget;
   FRect           normalGeometry;
};
#pragma pack(pop)


// FWindow inline functions
//----------------------------------------------------------------------
inline const char* FWindow::getClassName() const
{ return "FWindow"; }

//----------------------------------------------------------------------
inline bool FWindow::setWindowWidget()
{ return setWindowWidget(true); }

//----------------------------------------------------------------------
inline bool FWindow::unsetWindowWidget()
{ return setWindowWidget(false); }

//----------------------------------------------------------------------
inline bool FWindow::activateWindow()
{ return activateWindow(true); }

//----------------------------------------------------------------------
inline bool FWindow::deactivateWindow()
{ return activateWindow(false); }

//----------------------------------------------------------------------
inline bool FWindow::setResizeable()
{ return setResizeable(true); }

//----------------------------------------------------------------------
inline bool FWindow::unsetResizeable()
{ return setResizeable(false); }

//----------------------------------------------------------------------
inline bool FWindow::setTransparentShadow()
{ return setTransparentShadow(true); }

//----------------------------------------------------------------------
inline bool FWindow::unsetTransparentShadow()
{ return setTransparentShadow(false); }

//----------------------------------------------------------------------
inline bool FWindow::setShadow()
{ return setShadow(true); }

//----------------------------------------------------------------------
inline bool FWindow::unsetShadow()
{ return setShadow(false); }

//----------------------------------------------------------------------
inline bool FWindow::setAlwaysOnTop()
{ return setAlwaysOnTop(true); }

//----------------------------------------------------------------------
inline bool FWindow::unsetAlwaysOnTop()
{ return setAlwaysOnTop(false); }

//----------------------------------------------------------------------
inline bool FWindow::isZoomed() const
{ return zoomed; }

//----------------------------------------------------------------------
inline bool FWindow::isWindowActive() const
{ return window_active; }

//----------------------------------------------------------------------
inline bool FWindow::isResizeable() const
{ return ((flags & fc::resizeable) != 0); }

//----------------------------------------------------------------------
inline bool FWindow::isAlwaysOnTop() const
{ return ((flags & fc::always_on_top) != 0); }

//----------------------------------------------------------------------
inline bool FWindow::hasTransparentShadow() const
{ return ((flags & fc::trans_shadow) != 0); }

//----------------------------------------------------------------------
inline bool FWindow::hasShadow() const
{ return ((flags & fc::shadow) != 0); }

//----------------------------------------------------------------------
inline FWindow* FWindow::getWindowWidgetAt (const FPoint& pos)
{ return getWindowWidgetAt (pos.getX(), pos.getY()); }

//----------------------------------------------------------------------
inline bool FWindow::raiseWindow()
{ return raiseWindow(this); }

//----------------------------------------------------------------------
inline bool FWindow::lowerWindow()
{ return lowerWindow(this); }

#endif  // FWINDOW_H
