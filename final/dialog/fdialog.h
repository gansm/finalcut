/***********************************************************************
* fdialog.h - Widget FDialog                                           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2012-2021 Markus Gans                                      *
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
    explicit FDialog (const FString&, FWidget* = nullptr);

    // Disable copy constructor
    FDialog (const FDialog&) = delete;

    // Disable move constructor
    FDialog (FDialog&&) noexcept = delete;

    // Destructor
    ~FDialog() override;

    // Disable copy assignment operator (=)
    FDialog& operator = (const FDialog&) = delete;

    // Disable move assignment operator (=)
    FDialog& operator = (FDialog&&) noexcept = delete;

    // Accessors
    FString               getClassName() const override;
    virtual FString       getText() const;

    // Mutators
    bool                  setDialogWidget (bool = true);
    bool                  unsetDialogWidget();
    bool                  setModal (bool = true);
    bool                  unsetModal();
    bool                  setResizeable (bool = true) override;
    bool                  setMinimizable (bool = true) override;
    bool                  setTitlebarButtonVisibility (bool = true);
    bool                  unsetTitlebarButtonVisibility();
    bool                  setBorder (bool = true);
    bool                  unsetBorder();
    void                  resetColors() override;
    virtual void          setText (const FString&);

    // Inquiries
    bool                  isModal() const;
    bool                  hasBorder() const;

    // Methods
    void                  show() override;
    void                  hide() override;
    ResultCode            exec();
    void                  setPos (const FPoint&, bool = true) override;
    void                  move (const FPoint&) override;
    bool                  moveUp (int);
    bool                  moveDown (int);
    bool                  moveLeft (int);
    bool                  moveRight (int);
    void                  setSize (const FSize&, bool = true) override;
    bool                  reduceHeight (int);
    bool                  expandHeight (int);
    bool                  reduceWidth (int);
    bool                  expandWidth (int);
    bool                  zoomWindow() override;
    bool                  minimizeWindow() override;
    void                  activateDialog();

    // Event handlers
    void                  onKeyPress (FKeyEvent*) override;
    void                  onMouseDown (FMouseEvent*) override;
    void                  onMouseUp (FMouseEvent*) override;
    void                  onMouseMove (FMouseEvent*) override;
    void                  onMouseDoubleClick (FMouseEvent*) override;
    void                  onAccel (FAccelEvent*) override;
    void                  onWindowActive (FEvent*) override;
    void                  onWindowInactive (FEvent*) override;
    void                  onWindowRaised (FEvent*) override;
    void                  onWindowLowered (FEvent*) override;

  protected:
    // Methods
    void                  done (ResultCode);
    void                  draw() override;
    void                  drawDialogShadow();

    // Event handlers
    void                  onClose (FCloseEvent*) override;

  private:
    struct MouseStates
    {
      int         mouse_x;
      int         mouse_y;
      FPoint      termPos;
      std::size_t minimize_btn;
      std::size_t zoom_btn;
      bool        mouse_over_menu;
    };

    // Using-declaration
    using KeyMap = std::unordered_map<FKey, std::function<void()>, FKeyHash>;

    // Constant
    static constexpr bool PRINT_WIN_NUMBER = false;  // Only for debug

    // Methods
    void                  init();
    void                  initDialogMenu();
    void                  initMoveSizeMenuItem (FMenu*);
    void                  initZoomMenuItem (FMenu*);
    void                  initMinimizeMenuItem (FMenu*);
    void                  initCloseMenuItem (FMenu*);
    MouseStates           initMouseStates(const FMouseEvent&, bool) const;
    void                  mapKeyFunctions();
    void                  drawBorder() override;
    void                  drawTitleBar();
    void                  drawBarButton();
    void                  drawZoomButton();
    void                  drawMinimizeButton();
    void                  printRestoreSizeButton();
    void                  printZoomedButton();
    void                  printMinimizeButton();
    void                  drawTextBar();
    void                  restoreOverlaidWindows();
    void                  setCursorToFocusWidget();
    void                  leaveMenu();
    void                  openMenu();
    void                  selectFirstMenuItem();
    void                  setMinimizeItem();
    void                  setZoomItem();
    std::size_t           getMenuButtonWidth() const;
    std::size_t           getZoomButtonWidth() const;
    std::size_t           getMinimizeButtonWidth() const;
    void                  activateMinimizeButton (const MouseStates&);
    void                  deactivateMinimizeButton();
    void                  leaveMinimizeButton (const MouseStates&);
    void                  pressMinimizeButton (const MouseStates&);
    void                  activateZoomButton (const MouseStates&);
    void                  deactivateZoomButton();
    void                  leaveZoomButton (const MouseStates&);
    void                  pressZoomButton (const MouseStates&);
    bool                  isMouseOverMenu (const FPoint&) const;
    bool                  isMouseOverMenuButton (const MouseStates&) const;
    bool                  isMouseOverZoomButton (const MouseStates&) const;
    bool                  isMouseOverMinimizeButton (const MouseStates&) const;
    bool                  isMouseOverTitlebar (const MouseStates&) const;
    void                  passEventToSubMenu ( const MouseStates&
                                             , const FMouseEvent& );
    void                  moveSizeKey (FKeyEvent*);
    void                  raiseActivateDialog();
    void                  lowerActivateDialog();
    bool                  isOutsideTerminal (const FPoint&) const;
    bool                  isLeftOutside() const;
    bool                  isBottomOutside() const;
    bool                  isLowerRightResizeCorner (const MouseStates&) const;
    void                  resizeMouseDown (const MouseStates&);
    void                  resizeMouseUpMove (const MouseStates&, bool = false);
    void                  cancelMouseResize();
    void                  acceptMoveSize();
    void                  cancelMoveSize();
    static void           addDialog (FWidget*);
    static void           delDialog (const FWidget*);

    // Callback methods
    void                  cb_move();
    void                  cb_minimize();
    void                  cb_zoom();
    void                  cb_close();

    // Data members
    FString               tb_text{};  // title bar text
    ResultCode            result_code{ResultCode::Reject};
    bool                  titlebar_buttons{true};
    bool                  zoom_button_pressed{false};
    bool                  zoom_button_active{false};
    bool                  minimize_button_pressed{false};
    bool                  minimize_button_active{false};
    bool                  setPos_error{false};
    bool                  setSize_error{false};
    FPoint                titlebar_click_pos{};
    FPoint                resize_click_pos{};
    FRect                 save_geometry{};  // required by keyboard move/size
    FMenu*                dialog_menu{nullptr};
    FMenuItem*            dgl_menuitem{nullptr};
    FMenuItem*            move_size_item{nullptr};
    FMenuItem*            zoom_item{nullptr};
    FMenuItem*            minimize_item{nullptr};
    FMenuItem*            close_item{nullptr};
    FToolTip*             tooltip{nullptr};
    KeyMap                key_map{};

    // Friend function from FMenu
    friend void FMenu::hideSuperMenus() const;
};

// FDialog inline functions
//----------------------------------------------------------------------
inline FString FDialog::getClassName() const
{ return "FDialog"; }

//----------------------------------------------------------------------
inline FString FDialog::getText() const
{ return tb_text; }

//----------------------------------------------------------------------
inline bool FDialog::unsetDialogWidget()
{ return setDialogWidget(false); }

//----------------------------------------------------------------------
inline bool FDialog::unsetModal()
{ return setModal(false); }

//----------------------------------------------------------------------
inline bool FDialog::setTitlebarButtonVisibility (bool enable)
{  return (titlebar_buttons = enable); }

//----------------------------------------------------------------------
inline bool FDialog::unsetTitlebarButtonVisibility()
{ return setTitlebarButtonVisibility(false); }

//----------------------------------------------------------------------
inline bool FDialog::unsetBorder()
{ return setBorder(false); }

//----------------------------------------------------------------------
inline void FDialog::setText (const FString& txt)
{ tb_text.setString(txt); }

//----------------------------------------------------------------------
inline bool FDialog::isModal() const
{ return getFlags().modal; }

//----------------------------------------------------------------------
inline bool FDialog::hasBorder() const
{ return ! getFlags().no_border; }

}  // namespace finalcut

#endif  // FDIALOG_H
