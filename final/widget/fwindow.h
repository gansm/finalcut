/***********************************************************************
* fwindow.h - Intermediate base class for all window objects           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2024 Markus Gans                                      *
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

    // Disable move constructor
    FWindow (FWindow&&) noexcept = delete;

    // Destructor
    ~FWindow () override;

    // Disable copy assignment operator (=)
    auto operator = (const FWindow&) -> FWindow& = delete;

    // Disable move assignment operator (=)
    auto operator = (FWindow&&) noexcept -> FWindow& = delete;

    // Accessors
    auto         getClassName() const -> FString override;
    template<typename WidgetT>
    static auto  getWindowWidget (WidgetT*) -> FWindow*;
    template<typename WidgetT>
    static auto  getWindowLayer (WidgetT*) -> int;
    auto         getWindowFocusWidget() const -> FWidget*;

    // Mutators
    void         setWindowWidget (bool = true);
    void         unsetWindowWidget();
    static void  setActiveWindow (FWindow*);
    void         setWindowFocusWidget (FWidget*);
    auto         activateWindow (bool = true) -> bool;
    void         unsetActiveWindow() const;
    auto         deactivateWindow() -> bool;
    virtual void setResizeable (bool = true);
    void         unsetResizeable();
    virtual void setMinimizable (bool = true);
    void         unsetMinimizable();
    void         setTransparentShadow (bool = true);
    void         unsetTransparentShadow();
    void         setShadow (bool = true);
    void         unsetShadow();
    void         setAlwaysOnTop (bool = true);
    void         unsetAlwaysOnTop();

    // Inquiries
    auto         isZoomed() const noexcept -> bool;
    auto         isMinimized() const -> bool;
    auto         isWindowActive() const noexcept -> bool;
    auto         isWindowHidden() const -> bool;
    auto         isResizeable() const -> bool;
    auto         isMinimizable() const -> bool;
    auto         isAlwaysOnTop() const -> bool;
    auto         hasTransparentShadow() const -> bool;
    auto         hasShadow() const -> bool;

    // Methods
    void         drawBorder() override;
    void         show() override;
    void         hide() override;
    void         setX (int, bool = true) override;
    void         setY (int, bool = true) override;
    void         setPos (const FPoint&, bool = true) override;
    void         setWidth (std::size_t, bool = true) override;
    void         setHeight (std::size_t, bool = true) override;
    void         setSize (const FSize&, bool = true) override;
    void         setGeometry ( const FPoint&, const FSize&
                             , bool = true ) override;
    void         move (const FPoint&) override;
    static auto  getWindowWidgetAt (const FPoint&) -> FWindow*;
    static auto  getWindowWidgetAt (int, int) -> FWindow*;
    static void  addWindow (FWidget*);
    static void  delWindow (const FWidget*);
    static void  swapWindow (const FWidget*, const FWidget*);
    static auto  raiseWindow (FWidget*) -> bool;
    auto         raiseWindow() -> bool;
    static auto  lowerWindow (FWidget*) -> bool;
    auto         lowerWindow() -> bool;
    virtual auto zoomWindow() -> bool;
    virtual auto minimizeWindow() -> bool;
    static void  switchToPrevWindow (const FWidget*);
    static auto  activatePrevWindow() -> bool;
    void         setShadowSize (const FSize&) override;

  protected:
    // Method
    void         adjustSize() override;

    // Mutator
    static void  setPreviousWindow (FWindow*);

    // Event handlers
    auto         event (FEvent*) -> bool override;
    virtual void onWindowActive (FEvent*);
    virtual void onWindowInactive (FEvent*);
    virtual void onWindowRaised (FEvent*);
    virtual void onWindowLowered (FEvent*);

  private:
    // Methods
    void         createVWin() noexcept;
    static auto  getVisibleTermGeometry (FWindow*) -> FRect;
    static void  deleteFromAlwaysOnTopList (const FWidget*);
    static void  processAlwaysOnTop();
    static auto  getWindowWidgetImpl (FWidget*) -> FWindow*;
    static auto  getWindowLayerImpl (FWidget*) -> int;
    static void  activateTopWindow (const FWindow*);
    static auto  isWindowActivatable (const FWindow*, const FWindow*) -> bool;
    static void  reactivateWindow (FWindow*);

    // Data members
    FWidget*              win_focus_widget{nullptr};
    FRect                 normalGeometry{};
    static FWindow*       previous_window;
    bool                  window_active{false};
    bool                  zoomed{false};
};

// non-member function forward declarations
//----------------------------------------------------------------------
class FMouseData;  // class forward declaration
void closeDropDownMouseHandler (const FMouseData&);
void closeDropDown (const FWidget*, const FPoint&);
void unselectMenubarItemsMouseHandler (const FMouseData&);
void unselectMenubarItems (const FWidget*, const FPoint&);

// FWindow inline functions
//----------------------------------------------------------------------
inline auto FWindow::getClassName() const -> FString
{ return "FWindow"; }

//----------------------------------------------------------------------

template<typename WidgetT>
inline auto FWindow::getWindowWidget (WidgetT* obj) -> FWindow*
{
  return getWindowWidgetImpl (static_cast<FWidget*>(obj));
}

//----------------------------------------------------------------------
template<typename WidgetT>
inline auto FWindow::getWindowLayer (WidgetT* obj) -> int
{
  return getWindowLayerImpl (static_cast<FWidget*>(obj));
}

//----------------------------------------------------------------------
inline void FWindow::unsetWindowWidget()
{ setWindowWidget(false); }

//----------------------------------------------------------------------
inline auto FWindow::deactivateWindow() -> bool
{ return activateWindow(false); }

//----------------------------------------------------------------------
inline void FWindow::unsetResizeable()
{ setResizeable(false); }

//----------------------------------------------------------------------
inline void FWindow::unsetMinimizable()
{ setMinimizable(false); }

//----------------------------------------------------------------------
inline void FWindow::unsetTransparentShadow()
{ setTransparentShadow(false); }

//----------------------------------------------------------------------
inline void FWindow::unsetShadow()
{ setShadow(false); }

//----------------------------------------------------------------------
inline void FWindow::unsetAlwaysOnTop()
{ setAlwaysOnTop(false); }

//----------------------------------------------------------------------
inline auto FWindow::isZoomed() const noexcept -> bool
{ return zoomed; }

//----------------------------------------------------------------------
inline auto FWindow::isWindowActive() const noexcept -> bool
{ return window_active; }

//----------------------------------------------------------------------
inline auto FWindow::isResizeable() const -> bool
{ return getFlags().feature.resizeable; }

//----------------------------------------------------------------------
inline auto FWindow::isMinimizable() const -> bool
{ return getFlags().feature.minimizable; }

//----------------------------------------------------------------------
inline auto FWindow::isAlwaysOnTop() const -> bool
{ return getFlags().visibility.always_on_top; }

//----------------------------------------------------------------------
inline auto FWindow::hasTransparentShadow() const -> bool
{ return getFlags().shadow.trans_shadow; }

//----------------------------------------------------------------------
inline auto FWindow::hasShadow() const -> bool
{ return getFlags().shadow.shadow; }

//----------------------------------------------------------------------
inline auto FWindow::getWindowWidgetAt (const FPoint& pos) -> FWindow*
{ return getWindowWidgetAt (pos.getX(), pos.getY()); }

//----------------------------------------------------------------------
inline auto FWindow::raiseWindow() -> bool
{ return raiseWindow(this); }

//----------------------------------------------------------------------
inline auto FWindow::lowerWindow() -> bool
{ return lowerWindow(this); }

//----------------------------------------------------------------------
inline void FWindow::setPreviousWindow (FWindow* w)
{ previous_window = w; }

}  // namespace finalcut

#endif  // FWINDOW_H
