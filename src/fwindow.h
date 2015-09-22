// fwindow.h
// class FWindow.h

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
   bool window_active;

 private:
   FWindow (const FWindow&);
   FWindow& operator = (const FWindow&);

 protected:
   bool            event (FEvent*);
   virtual void    onWindowActive (FEvent*);
   virtual void    onWindowInactive (FEvent*);
   virtual void    onWindowRaised (FEvent*);
   virtual void    onWindowLowered (FEvent*);

 public:
   explicit FWindow (FWidget* parent=0);  // constructor
  ~FWindow ();  // destructor

   const char*     getClassName() const;
   virtual void    show();
   virtual void    hide();
   static FWindow* windowWidgetAt (const FPoint&);
   static FWindow* windowWidgetAt (int, int);
   static void     addWindow (FWidget*);
   static void     delWindow (FWidget*);
   static FWindow* getWindowWidget (FWidget*);
   static int      getWindowLayer (FWidget*);
   static void     swapWindow (FWidget*, FWidget*);
   static bool     raiseWindow (FWidget*);
   bool            raiseWindow ();
   static bool     lowerWindow (FWidget*);
   bool            lowerWindow ();
   static void     setActiveWindow (FWindow*);
   static FWindow* getActiveWindow();
   static bool     activatePrevWindow();
   bool            activateWindow(bool);
   bool            activateWindow();
   bool            deactivateWindow();
   bool            isActiveWindow() const;
};
#pragma pack(pop)


// FWindow inline functions
//----------------------------------------------------------------------
inline const char* FWindow::getClassName() const
{ return "FWindow"; }

//----------------------------------------------------------------------
inline bool FWindow::raiseWindow()
{ return raiseWindow(this); }

//----------------------------------------------------------------------
inline bool FWindow::lowerWindow()
{ return lowerWindow(this); }

//----------------------------------------------------------------------
inline bool FWindow::activateWindow()
{ return activateWindow(true); }

//----------------------------------------------------------------------
inline bool FWindow::deactivateWindow()
{ return activateWindow(false); }

//----------------------------------------------------------------------
inline bool FWindow::isActiveWindow() const
{ return window_active; }


#endif  // _FWINDOW_H

