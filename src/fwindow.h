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
   static FWindow* previous_widget;

 private:
   // Disable copy constructor
   FWindow (const FWindow&);
   // Disable assignment operator (=)
   FWindow& operator = (const FWindow&);

   void            deleteFromAlwaysOnTopList (FWidget*);

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
   virtual void    show();
   virtual void    hide();
   virtual void    setWidth (int, bool = true);
   virtual void    setHeight (int, bool = true);
   virtual void    setSize (int, int, bool = true);
   // make every setGeometry from FWidget available
   using FWidget::setGeometry;
   void            setGeometry (int, int, int, int, bool = true);
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
   static void     alwaysOnTop();
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
   bool            isActiveWindow() const;
   bool            isHiddenWindow() const;
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
inline bool FWindow::isActiveWindow() const
{ return window_active; }

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
