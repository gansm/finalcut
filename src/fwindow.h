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
//       ▕▔▔▔▔▔▔▔▔▔▏ 1      *▕▔▔▔▔▔▔▔▔▏
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

#ifndef _FWINDOW_H
#define _FWINDOW_H

#include "fwidget.h"


//----------------------------------------------------------------------
// class FWindow
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FWindow : public FWidget
{
 private:
   bool     window_active;
   bool     zoomed;
   FWidget* win_focus_widget;
   FRect    normalGeometry;

 protected:
   static FWindow* previous_window;

 private:
   // Disable copy constructor
   FWindow (const FWindow&);
   // Disable assignment operator (=)
   FWindow& operator = (const FWindow&);

   static void     deleteFromAlwaysOnTopList (FWidget*);
   static void     processAlwaysOnTop();

 protected:
   // Event handlers
   bool            event (FEvent*);
   virtual void    onWindowActive (FEvent*);
   virtual void    onWindowInactive (FEvent*);
   virtual void    onWindowRaised (FEvent*);
   virtual void    onWindowLowered (FEvent*);

   virtual void    adjustSize();

 public:
   // Constructor
   explicit FWindow (FWidget* = 0);
   // Destructor
  ~FWindow ();

   const char*     getClassName() const;
   // make every drawBorder from FWidget available
   using FWidget::drawBorder;
   virtual void    drawBorder();
   virtual void    show();
   virtual void    hide();
   virtual void    setX (int, bool = true);
   virtual void    setY (int, bool = true);
   virtual void    setPos (int, int, bool = true);
   // make every setPos from FWidget available
   using FWidget::setPos;
   virtual void    setWidth (int, bool = true);
   virtual void    setHeight (int, bool = true);
   virtual void    setSize (int, int, bool = true);
   // make every setGeometry from FWidget available
   using FWidget::setGeometry;
   void            setGeometry (int, int, int, int, bool = true);
   virtual void    move (int, int);
   // make every move from FWidget available
   using FWidget::move;
   static FWindow* getWindowWidgetAt (const FPoint&);
   static FWindow* getWindowWidgetAt (int, int);
   static void     addWindow (FWidget*);
   static void     delWindow (FWidget*);
   static FWindow* getWindowWidget (FWidget*);
   static int      getWindowLayer (FWidget*);
   static void     swapWindow (FWidget*, FWidget*);
   static bool     raiseWindow (FWidget*);
   bool            raiseWindow ();
   static bool     lowerWindow (FWidget*);
   bool            lowerWindow ();
   bool            zoomWindow ();
   bool            isZoomed() const;
   bool            setWindowWidget (bool);
   bool            setWindowWidget();
   bool            unsetWindowWidget();
   static FWindow* getActiveWindow();
   static void     setActiveWindow (FWindow*);
   FWidget*        getWindowFocusWidget() const;
   void            setWindowFocusWidget (FWidget*);
   static void     switchToPrevWindow();
   static bool     activatePrevWindow();
   bool            activateWindow (bool);
   bool            activateWindow();
   bool            deactivateWindow();
   bool            isWindowActive() const;
   bool            isWindowHidden() const;
   virtual bool    setResizeable (bool);
   virtual bool    setResizeable();
   bool            unsetResizeable();
   bool            isResizeable();
   bool            setTransparentShadow (bool);
   bool            setTransparentShadow();
   bool            unsetTransparentShadow();
   bool            hasTransparentShadow();
   bool            setShadow (bool);
   bool            setShadow();
   bool            unsetShadow();
   bool            hasShadow();
   virtual void    setShadowSize (int, int);
   bool            setAlwaysOnTop (bool);
   bool            setAlwaysOnTop();
   bool            unsetAlwaysOnTop();
   bool            isAlwaysOnTop();
};
#pragma pack(pop)


// FWindow inline functions
//----------------------------------------------------------------------
inline const char* FWindow::getClassName() const
{ return "FWindow"; }

//----------------------------------------------------------------------
inline FWindow* FWindow::getWindowWidgetAt (const FPoint& pos)
{ return getWindowWidgetAt (pos.getX(), pos.getY()); }

//----------------------------------------------------------------------
inline bool FWindow::raiseWindow()
{ return raiseWindow(this); }

//----------------------------------------------------------------------
inline bool FWindow::lowerWindow()
{ return lowerWindow(this); }

//----------------------------------------------------------------------
inline bool FWindow::isZoomed() const
{ return zoomed; }

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
inline bool FWindow::isWindowActive() const
{ return window_active; }

//----------------------------------------------------------------------
inline bool FWindow::setResizeable()
{ return setResizeable(true); }

//----------------------------------------------------------------------
inline bool FWindow::unsetResizeable()
{ return setResizeable(false); }

//----------------------------------------------------------------------
inline bool FWindow::isResizeable()
{ return ((flags & fc::resizeable) != 0); }

//----------------------------------------------------------------------
inline bool FWindow::setTransparentShadow()
{ return setTransparentShadow(true); }

//----------------------------------------------------------------------
inline bool FWindow::unsetTransparentShadow()
{ return setTransparentShadow(false); }

//----------------------------------------------------------------------
inline bool FWindow::hasTransparentShadow()
{ return ((flags & fc::trans_shadow) != 0); }

//----------------------------------------------------------------------
inline bool FWindow::setShadow()
{ return setShadow(true); }

//----------------------------------------------------------------------
inline bool FWindow::unsetShadow()
{ return setShadow(false); }

//----------------------------------------------------------------------
inline bool FWindow::hasShadow()
{ return ((flags & fc::shadow) != 0); }

//----------------------------------------------------------------------
inline bool FWindow::setAlwaysOnTop()
{ return setAlwaysOnTop(true); }

//----------------------------------------------------------------------
inline bool FWindow::unsetAlwaysOnTop()
{ return setAlwaysOnTop(false); }

//----------------------------------------------------------------------
inline bool FWindow::isAlwaysOnTop()
{ return ((flags & fc::always_on_top) != 0); }


#endif  // _FWINDOW_H
