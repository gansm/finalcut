/***********************************************************************
* fdialog.h - Widget FDialog                                           *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2012-2018 Markus Gans                                      *
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
    using FWindow::move;
    using FWindow::setPos;

    // Enumeration
    enum DialogCode
    {
      Reject = 0,
      Accept = 1
    };

    // Constructors
    explicit FDialog (FWidget* = 0);
    explicit FDialog (const FString&, FWidget* = 0);

    // Destructor
    virtual ~FDialog();

    // Accessors
    virtual const char* getClassName() const;
    FString             getText() const;

    // Mutators
    bool                setDialogWidget (bool);
    bool                setDialogWidget();
    bool                unsetDialogWidget();
    bool                setModal (bool);
    bool                setModal();
    bool                unsetModal();
    virtual bool        setResizeable (bool);
    bool                setScrollable (bool);
    bool                setScrollable();
    bool                unsetScrollable();
    void                setText (const FString&);

    // Inquiries
    bool                isModal();
    bool                isScrollable();

    // Methods
    virtual void        show();
    virtual void        hide();
    DialogCode          exec();
    virtual void        setPos (int, int, bool = true);
    virtual void        move (int, int);
    bool                moveUp (int);
    bool                moveDown (int);
    bool                moveLeft (int);
    bool                moveRight (int);
    virtual void        setSize (std::size_t, std::size_t, bool = true);
    bool                reduceHeight (int);
    bool                expandHeight (int);
    bool                reduceWidth (int);
    bool                expandWidth (int);
    void                activateDialog();

    // Event handlers
    virtual void        onKeyPress (FKeyEvent*);
    virtual void        onMouseDown (FMouseEvent*);
    virtual void        onMouseUp (FMouseEvent*);
    virtual void        onMouseMove (FMouseEvent*);
    virtual void        onMouseDoubleClick (FMouseEvent*);
    virtual void        onAccel (FAccelEvent*);
    virtual void        onWindowActive (FEvent*);
    virtual void        onWindowInactive (FEvent*);
    virtual void        onWindowRaised (FEvent*);
    virtual void        onWindowLowered (FEvent*);

  protected:
    // Methods
    virtual void        done (DialogCode);
    virtual void        draw();
    void                drawDialogShadow();

    // Event handlers
    virtual void        onShow (FShowEvent*);
    virtual void        onHide (FHideEvent*);
    virtual void        onClose (FCloseEvent*);

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
    static const std::size_t MENU_BTN = 3;
    static const bool PRINT_WIN_NUMBER = false;  // Only for debug

    // Using-declaration
    using FWidget::drawBorder;

    // Disable copy constructor
    FDialog (const FDialog&);

    // Disable assignment operator (=)
    FDialog& operator = (const FDialog&);

    // Methods
    void                init();
    void                initDialogMenu();
    void                initMoveSizeMenuItem (FMenu*);
    void                initZoomMenuItem (FMenu*);
    void                initCloseMenuItem (FMenu*);
    virtual void        drawBorder();
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
    void                activateZoomButton (mouseStates&);
    void                deactivateZoomButton();
    void                leaveZoomButton (mouseStates&);
    void                pressZoomButton (mouseStates&);
    bool                isMouseOverMenu (const FPoint&);
    void                passEventToSubMenu (mouseStates&, FMouseEvent*);
    void                moveSizeKey (FKeyEvent*);
    void                raiseActivateDialog();
    void                lowerActivateDialog();
    bool                isLowerRightResizeCorner (mouseStates&);
    void                resizeMouseDown (mouseStates&);
    void                resizeMouseUpMove (mouseStates&, bool = false);
    void                cancelMouseResize();
    void                acceptMoveSize();
    void                cancelMoveSize();
    static void         addDialog (FWidget*);
    static void         delDialog (FWidget*);

    // Callback methods
    void                cb_move (FWidget*, data_ptr);
    void                cb_zoom (FWidget*, data_ptr);
    void                cb_close (FWidget*, data_ptr);

    // Data Members
    FString             tb_text;        // title bar text
    DialogCode          result_code;
    bool                zoom_button_pressed;
    bool                zoom_button_active;
    bool                setPos_error;
    bool                setSize_error;
    FPoint              titlebar_click_pos;
    FPoint              resize_click_pos;
    FRect               save_geometry;  // required by keyboard move/size
    FMenu*              dialog_menu;
    FMenuItem*          dgl_menuitem;
    FMenuItem*          move_size_item;
    FMenuItem*          zoom_item;
    FMenuItem*          close_item;
    FToolTip*           tooltip;

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
