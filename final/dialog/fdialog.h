/***********************************************************************
* fdialog.h - Widget FDialog                                           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2012-2023 Markus Gans                                      *
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
 *       ▕▔▔▔▔▔▔▔▔▔▏
 *       ▕ FWindow ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏
 *            ▲
 *            │
 *       ▕▔▔▔▔▔▔▔▔▔▏
 *       ▕ FDialog ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FDIALOG_H
#define FDIALOG_H
#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <unordered_map>

#include "final/menu/fmenu.h"
#include "final/widget/fwindow.h"

namespace finalcut
{

// class forward declaration
class FToolTip;

//----------------------------------------------------------------------
// class FDialog
//----------------------------------------------------------------------

class FDialog : public FWindow
{
  public:
    // Using-declaration
    using FWindow::setResizeable;
    using FWindow::setMinimizable;

    // Enumeration
    enum class ResultCode : int
    {
      Reject = 0,
      Accept = 1
    };

    // Constructors
    explicit FDialog (FWidget* = nullptr);
    explicit FDialog (FString&&, FWidget* = nullptr);

    // Disable copy constructor
    FDialog (const FDialog&) = delete;

    // Disable move constructor
    FDialog (FDialog&&) noexcept = delete;

    // Destructor
    ~FDialog() override;

    // Disable copy assignment operator (=)
    auto operator = (const FDialog&) -> FDialog& = delete;

    // Disable move assignment operator (=)
    auto operator = (FDialog&&) noexcept -> FDialog& = delete;

    // Accessors
    auto getClassName() const -> FString override;
    virtual auto getText() const -> FString;

    // Mutators
    auto setDialogWidget (bool = true) -> bool;
    auto unsetDialogWidget() -> bool;
    auto setModal (bool = true) -> bool;
    auto unsetModal() -> bool;
    auto setResizeable (bool = true) -> bool override;
    auto setMinimizable (bool = true) -> bool override;
    auto setTitlebarButtonVisibility (bool = true) -> bool;
    auto unsetTitlebarButtonVisibility() -> bool;
    auto setBorder (bool = true) -> bool;
    auto unsetBorder() -> bool;
    void resetColors() override;
    virtual void setText (const FString&);

    // Inquiries
    auto isModal() const -> bool;
    auto hasBorder() const -> bool;

    // Methods
    void show() override;
    void hide() override;
    auto exec() -> ResultCode;
    void setPos (const FPoint&, bool = true) override;
    void move (const FPoint&) override;
    auto moveUp (int) -> bool;
    auto moveDown (int) -> bool;
    auto moveLeft (int) -> bool;
    auto moveRight (int) -> bool;
    void setSize (const FSize&, bool = true) override;
    auto reduceHeight (int) -> bool;
    auto expandHeight (int) -> bool;
    auto reduceWidth (int) -> bool;
    auto expandWidth (int) -> bool;
    auto zoomWindow() -> bool override;
    auto minimizeWindow() -> bool override;
    void flushChanges() override;
    void activateDialog();

    // Event handlers
    void onKeyPress (FKeyEvent*) override;
    void onMouseDown (FMouseEvent*) override;
    void onMouseUp (FMouseEvent*) override;
    void onMouseMove (FMouseEvent*) override;
    void onMouseDoubleClick (FMouseEvent*) override;
    void onAccel (FAccelEvent*) override;
    void onWindowActive (FEvent*) override;
    void onWindowInactive (FEvent*) override;
    void onWindowRaised (FEvent*) override;
    void onWindowLowered (FEvent*) override;

  protected:
    // Methods
    void done (ResultCode);
    void draw() override;
    void drawDialogShadow();

    // Event handlers
    void onClose (FCloseEvent*) override;

  private:
    struct MouseStates
    {
      int         mouse_x;
      int         mouse_y;
      FPoint      termPos;
      std::size_t menu_btn;
      std::size_t minimize_btn;
      std::size_t zoom_btn;
      bool        mouse_over_menu;
    };

    // Using-declaration
    using KeyMap = std::unordered_map<FKey, std::function<void()>, EnumHash<FKey>>;

    // Constant
    static constexpr bool PRINT_WIN_NUMBER = false;  // Only for debug

    // Methods
    void init();
    void initDialogMenu();
    void initMoveSizeMenuItem (FMenu*);
    void initZoomMenuItem (FMenu*);
    void initMinimizeMenuItem (FMenu*);
    void initCloseMenuItem (FMenu*);
    auto initMouseStates (const FMouseEvent&, bool) const -> MouseStates;
    void mapKeyFunctions();
    void recoverBackgroundAfterMove (const FPoint&, const FRect&);
    void drawBorder() override;
    void drawTitleBar();
    void drawBarButton();
    void drawZoomButton();
    void drawMinimizeButton();
    void printRestoreSizeButton();
    void printZoomedButton();
    void printMinimizeButton();
    void drawTextBar();
    void clearStatusBar() const;
    void setCursorToFocusWidget();
    void leaveMenu();
    void openMenu();
    void selectFirstMenuItem();
    void setMinimizeItem();
    void setZoomItem();
    auto getMenuButtonWidth() const -> std::size_t;
    auto getZoomButtonWidth() const -> std::size_t;
    auto getMinimizeButtonWidth() const -> std::size_t;
    void activateMinimizeButton (const MouseStates&);
    void deactivateMinimizeButton();
    void leaveMinimizeButton (const MouseStates&);
    void pressMinimizeButton (const MouseStates&);
    void activateZoomButton (const MouseStates&);
    void deactivateZoomButton();
    void leaveZoomButton (const MouseStates&);
    void pressZoomButton (const MouseStates&);
    auto isMouseOverMenu (const FPoint&) const -> bool;
    auto isMouseOverMenuButton (const MouseStates&) const -> bool;
    auto isMouseOverZoomButton (const MouseStates&) const -> bool;
    auto isMouseOverMinimizeButton (const MouseStates&) const -> bool;
    auto isMouseOverTitlebar (const MouseStates&) const -> bool;
    void passEventToSubMenu ( const MouseStates&
                            , const FMouseEvent& );
    void handleLeftMouseDown (const MouseStates&);
    void handleRightAndMiddleMouseDown (const MouseButton&, const MouseStates&);
    void moveSizeKey (FKeyEvent*);
    void raiseActivateDialog();
    void lowerActivateDialog();
    auto isOutsideTerminal (const FPoint&) const -> bool;
    auto isLeftOutside() const -> bool;
    auto isBottomOutside() const -> bool;
    auto isLowerRightResizeCorner (const MouseStates&) const -> bool;
    auto noVisibleDialog() const -> bool;
    void resizeMouseDown (const MouseStates&);
    void resizeMouseUpMove (const MouseStates&, bool = false);
    void cancelMouseResize();
    void acceptMoveSize();
    void cancelMoveSize();
    static void addDialog (FWidget*);
    static void delDialog (const FWidget*);

    // Callback methods
    void cb_move();
    void cb_minimize();
    void cb_zoom();
    void cb_close();

    // Data members
    FString     tb_text{};  // title bar text
    ResultCode  result_code{ResultCode::Reject};
    bool        titlebar_buttons{true};
    bool        zoom_button_pressed{false};
    bool        zoom_button_active{false};
    bool        minimize_button_pressed{false};
    bool        minimize_button_active{false};
    bool        setPos_error{false};
    bool        setSize_error{false};
    FPoint      titlebar_click_pos{};
    FPoint      resize_click_pos{};
    FPoint      new_pos{};
    FSize       new_size{};
    FRect       save_geometry{};  // required by keyboard move/size
    FMenu*      dialog_menu{nullptr};
    FMenuItem*  dgl_menuitem{nullptr};
    FMenuItem*  move_size_item{nullptr};
    FMenuItem*  zoom_item{nullptr};
    FMenuItem*  minimize_item{nullptr};
    FMenuItem*  close_item{nullptr};
    FToolTip*   tooltip{nullptr};
    KeyMap      key_map{};

    // Friend function from FMenu
    friend void FMenu::hideSuperMenus() const;
};

// FDialog inline functions
//----------------------------------------------------------------------
inline auto FDialog::getClassName() const -> FString
{ return "FDialog"; }

//----------------------------------------------------------------------
inline auto FDialog::getText() const -> FString
{ return tb_text; }

//----------------------------------------------------------------------
inline auto FDialog::unsetDialogWidget() -> bool
{ return setDialogWidget(false); }

//----------------------------------------------------------------------
inline auto FDialog::unsetModal() -> bool
{ return setModal(false); }

//----------------------------------------------------------------------
inline auto FDialog::setTitlebarButtonVisibility (bool enable) -> bool
{  return (titlebar_buttons = enable); }

//----------------------------------------------------------------------
inline auto FDialog::unsetTitlebarButtonVisibility() -> bool
{ return setTitlebarButtonVisibility(false); }

//----------------------------------------------------------------------
inline auto FDialog::unsetBorder() -> bool
{ return setBorder(false); }

//----------------------------------------------------------------------
inline void FDialog::setText (const FString& txt)
{ tb_text.setString(txt); }

//----------------------------------------------------------------------
inline auto FDialog::isModal() const -> bool
{ return getFlags().visibility.modal; }

//----------------------------------------------------------------------
inline auto FDialog::hasBorder() const -> bool
{ return ! getFlags().feature.no_border; }

}  // namespace finalcut

#endif  // FDIALOG_H
