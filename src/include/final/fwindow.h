/***********************************************************************
* fwindow.h - Intermediate base class for all window objects           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2020 Markus Gans                                      *
*                                                                      *
* FINAL CUT is free software; you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as       *
* published by the Free Software Foundation; either version 3 of       *
* the License, or (at your option) any later version.                  *
*                                                                      *
* FINAL CUT is distributed in the hope that it will be useful, but     *
* WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU Lesser General Public License for more details.                  *
*                                                                      *
* You should have received a copy of the GNU Lesser General Public     *
* License along with this program.  If not, see                        *
* <http://www.gnu.org/licenses/>.                                      *
***********************************************************************/

/*  Inheritance diagram
 *  ═══════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▏
 * ▕ FVTerm  ▏ ▕ FObject ▏
 * ▕▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▏
 *      ▲           ▲
 *      │           │
 *      └─────┬─────┘
 *            │
 *       ▕▔▔▔▔▔▔▔▔▔▏
 *       ▕ FWidget ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏
 *            ▲
 *            │
 *       ▕▔▔▔▔▔▔▔▔▔▏1       *▕▔▔▔▔▔▔▔▔▏
 *       ▕ FWindow ▏-┬- - - -▕ FEvent ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏ :       ▕▁▁▁▁▁▁▁▁▏
 *                   :
 *                   :      *▕▔▔▔▔▔▔▔▔▏
 *                   :- - - -▕ FPoint ▏
 *                   :       ▕▁▁▁▁▁▁▁▁▏
 *                   :
 *                   :      *▕▔▔▔▔▔▔▔▔▔▏
 *                   └- - - -▕ FWidget ▏
 *                           ▕▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FWINDOW_H
#define FWINDOW_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fwidget.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FWindow
//----------------------------------------------------------------------

class FWindow : public FWidget
{
  public:
    // Using-declaration
    using FWidget::setGeometry;

    // Constructor
    explicit FWindow (FWidget* = nullptr);

    // Disable copy constructor
    FWindow (const FWindow&) = delete;

    // Destructor
    ~FWindow () override;

    // Disable copy assignment operator (=)
    FWindow& operator = (const FWindow&) = delete;

    // Accessors
    const FString       getClassName() const override;
    static FWindow*     getWindowWidget (const FWidget*);
    static int          getWindowLayer (const FWidget*);
    FWidget*            getWindowFocusWidget() const;

    // Mutators
    bool                setWindowWidget (bool);
    bool                setWindowWidget();
    bool                unsetWindowWidget();
    static void         setActiveWindow (FWindow*);
    void                setWindowFocusWidget (FWidget*);
    bool                activateWindow (bool);
    bool                activateWindow();
    void                unsetActiveWindow() const;
    bool                deactivateWindow();
    virtual bool        setResizeable (bool);
    virtual bool        setResizeable();
    bool                unsetResizeable();
    bool                setTransparentShadow (bool);
    bool                setTransparentShadow();
    bool                unsetTransparentShadow();
    bool                setShadow (bool);
    bool                setShadow();
    bool                unsetShadow();
    bool                setAlwaysOnTop (bool);
    bool                setAlwaysOnTop();
    bool                unsetAlwaysOnTop();

    // Inquiries
    bool                isZoomed() const;
    bool                isWindowActive() const;
    bool                isWindowHidden() const;
    bool                isResizeable() const;
    bool                isAlwaysOnTop() const;
    bool                hasTransparentShadow() const;
    bool                hasShadow() const;

    // Methods
    void                drawBorder() override;
    void                show() override;
    void                hide() override;
    void                setX (int, bool = true) override;
    void                setY (int, bool = true) override;
    void                setPos (const FPoint&, bool = true) override;
    void                setWidth (std::size_t, bool = true) override;
    void                setHeight (std::size_t, bool = true) override;
    void                setSize (const FSize&, bool = true) override;
    void                setGeometry ( const FPoint&, const FSize&
                                    , bool = true ) override;
    void                move (const FPoint&) override;
    static FWindow*     getWindowWidgetAt (const FPoint&);
    static FWindow*     getWindowWidgetAt (int, int);
    static void         addWindow (FWidget*);
    static void         delWindow (const FWidget*);
    static void         swapWindow (const FWidget*, const FWidget*);
    static bool         raiseWindow (FWidget*);
    bool                raiseWindow ();
    static bool         lowerWindow (FWidget*);
    bool                lowerWindow ();
    bool                zoomWindow ();
    static void         switchToPrevWindow (FWidget*);
    static bool         activatePrevWindow();
    void                setShadowSize (const FSize&) override;

  protected:
    // Method
    void                adjustSize() override;

    // Mutator
    static void         setPreviousWindow (FWindow*);

    // Event handlers
    bool                event (FEvent*) override;
    virtual void        onWindowActive (FEvent*);
    virtual void        onWindowInactive (FEvent*);
    virtual void        onWindowRaised (FEvent*);
    virtual void        onWindowLowered (FEvent*);

  private:
    // Methods
    static void         deleteFromAlwaysOnTopList (const FWidget*);
    static void         processAlwaysOnTop();

    // Data members
    FWidget*            win_focus_widget{nullptr};
    FRect               normalGeometry{};
    static FWindow*     previous_window;
    bool                window_active{false};
    bool                zoomed{false};
};

// non-member function forward declarations
//----------------------------------------------------------------------
void closeDropDown (FWidget*, const FPoint&);


// FWindow inline functions
//----------------------------------------------------------------------
inline const FString FWindow::getClassName() const
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
{ return getFlags().resizeable; }

//----------------------------------------------------------------------
inline bool FWindow::isAlwaysOnTop() const
{ return getFlags().always_on_top; }

//----------------------------------------------------------------------
inline bool FWindow::hasTransparentShadow() const
{ return getFlags().trans_shadow; }

//----------------------------------------------------------------------
inline bool FWindow::hasShadow() const
{ return getFlags().shadow; }

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
inline void FWindow::setPreviousWindow (FWindow* w)
{ previous_window = w; }

}  // namespace finalcut

#endif  // FWINDOW_H
