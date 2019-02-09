/***********************************************************************
* fdialog.h - Widget FDialog                                           *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2012-2019 Markus Gans                                      *
*                                                                      *
* The Final Cut is free software; you can redistribute it and/or       *
* modify it under the terms of the GNU Lesser General Public License   *
* as published by the Free Software Foundation; either version 3 of    *
* the License, or (at your option) any later version.                  *
*                                                                      *
* The Final Cut is distributed in the hope that it will be useful,     *
* but WITHOUT ANY WARRANTY; without even the implied warranty of       *
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
 * ▕▔▔▔▔▔▔▔▔▔▏
 * ▕  FTerm  ▏
 * ▕▁▁▁▁▁▁▁▁▁▏
 *      ▲
 *      │
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

#include "final/fmenu.h"
#include "final/fmenuitem.h"
#include "final/ftooltip.h"
#include "final/fwindow.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FDialog
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FDialog : public FWindow
{
  public:
     // Using-declaration
    using FWindow::setResizeable;

    // Enumeration
    enum DialogCode
    {
      Reject = 0,
      Accept = 1
    };

    // Constructors
    explicit FDialog (FWidget* = nullptr);
    explicit FDialog (const FString&, FWidget* = nullptr);

    // Disable copy constructor
    FDialog (const FDialog&) = delete;

    // Destructor
    virtual ~FDialog();

    // Disable assignment operator (=)
    FDialog& operator = (const FDialog&) = delete;

    // Accessors
    virtual const char* getClassName() const override;
    FString             getText() const;

    // Mutators
    bool                setDialogWidget (bool);
    bool                setDialogWidget();
    bool                unsetDialogWidget();
    bool                setModal (bool);
    bool                setModal();
    bool                unsetModal();
    virtual bool        setResizeable (bool) override;
    bool                setScrollable (bool);
    bool                setScrollable();
    bool                unsetScrollable();
    void                setText (const FString&);

    // Inquiries
    bool                isModal();
    bool                isScrollable();

    // Methods
    virtual void        show() override;
    virtual void        hide() override;
    int                 exec();
    virtual void        setPos (const FPoint&, bool = true) override;
    virtual void        move (const FPoint&) override;
    bool                moveUp (int);
    bool                moveDown (int);
    bool                moveLeft (int);
    bool                moveRight (int);
    virtual void        setSize (const FSize&, bool = true) override;
    bool                reduceHeight (int);
    bool                expandHeight (int);
    bool                reduceWidth (int);
    bool                expandWidth (int);
    void                activateDialog();

    // Event handlers
    virtual void        onKeyPress (FKeyEvent*) override;
    virtual void        onMouseDown (FMouseEvent*) override;
    virtual void        onMouseUp (FMouseEvent*) override;
    virtual void        onMouseMove (FMouseEvent*) override;
    virtual void        onMouseDoubleClick (FMouseEvent*) override;
    virtual void        onAccel (FAccelEvent*) override;
    virtual void        onWindowActive (FEvent*) override;
    virtual void        onWindowInactive (FEvent*) override;
    virtual void        onWindowRaised (FEvent*) override;
    virtual void        onWindowLowered (FEvent*) override;

  protected:
    // Methods
    virtual void        done (int);
    virtual void        draw() override;
    void                drawDialogShadow();

    // Event handlers
    virtual void        onShow (FShowEvent*) override;
    virtual void        onHide (FHideEvent*) override;
    virtual void        onClose (FCloseEvent*) override;

  private:
    // Typedef
    typedef struct
    {
      int         mouse_x;
      int         mouse_y;
      FPoint      termPos;
      std::size_t zoom_btn;
      bool        mouse_over_menu;
    } mouseStates;

    // Constant
    static constexpr std::size_t MENU_BTN = 3;
    static constexpr bool PRINT_WIN_NUMBER = false;  // Only for debug

    // Using-declaration
    using FWidget::drawBorder;

    // Methods
    void                init();
    void                initDialogMenu();
    void                initMoveSizeMenuItem (FMenu*);
    void                initZoomMenuItem (FMenu*);
    void                initCloseMenuItem (FMenu*);
    virtual void        drawBorder() override;
    void                drawTitleBar();
    void                drawBarButton();
    void                drawZoomButton();
    void                drawRestoreSizeButton();
    void                drawZoomedButton();
    void                drawTextBar();
    void                restoreOverlaidWindows();
    void                setCursorToFocusWidget();
    void                leaveMenu();
    void                openMenu();
    void                selectFirstMenuItem();
    void                setZoomItem();
    std::size_t         getZoomButtonWidth();
    void                activateZoomButton (const mouseStates&);
    void                deactivateZoomButton();
    void                leaveZoomButton (const mouseStates&);
    void                pressZoomButton (const mouseStates&);
    bool                isMouseOverMenu (const FPoint&);
    void                passEventToSubMenu (const mouseStates&, FMouseEvent*);
    void                moveSizeKey (FKeyEvent*);
    void                raiseActivateDialog();
    void                lowerActivateDialog();
    bool                isOutsideTerminal (const FPoint&);
    bool                isLowerRightResizeCorner (const mouseStates&);
    void                resizeMouseDown (const mouseStates&);
    void                resizeMouseUpMove (const mouseStates&, bool = false);
    void                cancelMouseResize();
    void                acceptMoveSize();
    void                cancelMoveSize();
    static void         addDialog (FWidget*);
    static void         delDialog (FWidget*);

    // Callback methods
    void                cb_move (FWidget*, FDataPtr);
    void                cb_zoom (FWidget*, FDataPtr);
    void                cb_close (FWidget*, FDataPtr);

    // Data Members
    FString             tb_text{};  // title bar text
    int                 result_code{FDialog::Reject};
    bool                zoom_button_pressed{false};
    bool                zoom_button_active{false};
    bool                setPos_error{false};
    bool                setSize_error{false};
    FPoint              titlebar_click_pos{};
    FPoint              resize_click_pos{};
    FRect               save_geometry{};  // required by keyboard move/size
    FMenu*              dialog_menu{nullptr};
    FMenuItem*          dgl_menuitem{nullptr};
    FMenuItem*          move_size_item{nullptr};
    FMenuItem*          zoom_item{nullptr};
    FMenuItem*          close_item{nullptr};
    FToolTip*           tooltip{nullptr};

    // Friend function from FMenu
    friend void FMenu::hideSuperMenus();
};
#pragma pack(pop)

// FDialog inline functions
//----------------------------------------------------------------------
inline const char* FDialog::getClassName() const
{ return "FDialog"; }

//----------------------------------------------------------------------
inline FString FDialog::getText() const
{ return tb_text; }

//----------------------------------------------------------------------
inline bool FDialog::setDialogWidget()
{ return setDialogWidget(true); }

//----------------------------------------------------------------------
inline bool FDialog::unsetDialogWidget()
{ return setDialogWidget(false); }

//----------------------------------------------------------------------
inline bool FDialog::setModal()
{ return setModal(true); }

//----------------------------------------------------------------------
inline bool FDialog::unsetModal()
{ return setModal(false); }

//----------------------------------------------------------------------
inline bool FDialog::setScrollable()
{ return setScrollable(true); }

//----------------------------------------------------------------------
inline bool FDialog::unsetScrollable()
{ return setScrollable(false); }

//----------------------------------------------------------------------
inline void FDialog::setText (const FString& txt)
{ tb_text = txt; }

//----------------------------------------------------------------------
inline bool FDialog::isModal()
{ return flags.modal; }

//----------------------------------------------------------------------
inline bool FDialog::isScrollable()
{ return flags.scrollable; }

}  // namespace finalcut

#endif  // FDIALOG_H
