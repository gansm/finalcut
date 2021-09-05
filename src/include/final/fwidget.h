/***********************************************************************
* fwidget.h - Intermediate base class for all widget objects           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2021 Markus Gans                                      *
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
 *       ▕▔▔▔▔▔▔▔▔▔▏1       1▕▔▔▔▔▔▔▔▔▔▔▔▔▏
 *       ▕ FWidget ▏-┬- - - -▕ FStatusBar ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏ :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▏
 *                   :
 *                   :      1▕▔▔▔▔▔▔▔▔▔▔▏
 *                   :- - - -▕ FMenuBar ▏
 *                   :       ▕▁▁▁▁▁▁▁▁▁▁▏
 *
 *                   :      1▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *                   :- - - -▕ FWidgetColors ▏
 *                   :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *                   :
 *                   :      *▕▔▔▔▔▔▔▔▔▔▏
 *                   :- - - -▕ FString ▏
 *                   :       ▕▁▁▁▁▁▁▁▁▁▏
 *                   :
 *                   :      *▕▔▔▔▔▔▔▔▔▏
 *                   :- - - -▕ FEvent ▏
 *                   :       ▕▁▁▁▁▁▁▁▁▏
 *                   :
 *                   :      *▕▔▔▔▔▔▔▔▔▔▔▔▏
 *                   :- - - -▕ FKeyEvent ▏
 *                   :       ▕▁▁▁▁▁▁▁▁▁▁▁▏
 *                   :
 *                   :      *▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *                   :- - - -▕ FMouseEvent ▏
 *                   :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *                   :
 *                   :      *▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *                   :- - - -▕ FWheelEvent ▏
 *                   :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *                   :
 *                   :      *▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *                   :- - - -▕ FFocusEvent ▏
 *                   :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *                   :
 *                   :      *▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *                   :- - - -▕ FAccelEvent ▏
 *                   :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *                   :
 *                   :      *▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *                   :- - - -▕ FResizeEvent ▏
 *                   :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *                   :
 *                   :      *▕▔▔▔▔▔▔▔▔▔▔▔▔▏
 *                   :- - - -▕ FShowEvent ▏
 *                   :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▏
 *                   :
 *                   :      *▕▔▔▔▔▔▔▔▔▔▔▔▔▏
 *                   :- - - -▕ FHideEvent ▏
 *                   :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▏
 *                   :
 *                   :      *▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *                   └- - - -▕ FCloseEvent ▏
 *                           ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FWIDGET_H
#define FWIDGET_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include "final/fcallback.h"
#include "final/fobject.h"
#include "final/foutput.h"
#include "final/fpoint.h"
#include "final/frect.h"
#include "final/fsize.h"
#include "final/ftypes.h"
#include "final/fvterm.h"

namespace finalcut
{

// class forward declaration
class FMenuBar;
class FRect;
class FResizeEvent;
class FSize;
class FStatusBar;
class FWidgetColors;

//----------------------------------------------------------------------
// class FWidget
//----------------------------------------------------------------------

class FWidget : public FVTerm, public FObject
{
  public:
    struct FAccelerator
    {
      alignas(8) FKey key;
      FWidget* object;
    };

    // Using-declarations
    using FVTerm::setColor;
    using FVTerm::print;
    using FWidgetList = std::vector<FWidget*>;
    using FAcceleratorList = std::vector<FAccelerator>;

    struct FWidgetFlags  // Properties of a widget ⚑
    {
      uInt32 shadow         : 1;
      uInt32 trans_shadow   : 1;
      uInt32 active         : 1;
      uInt32 visible        : 1;
      uInt32 shown          : 1;
      uInt32 hidden         : 1;
      uInt32 focus          : 1;
      uInt32 focusable      : 1;
      uInt32 scrollable     : 1;
      uInt32 resizeable     : 1;
      uInt32 minimizable    : 1;
      uInt32 modal          : 1;
      uInt32 visible_cursor : 1;
      uInt32 window_widget  : 1;
      uInt32 dialog_widget  : 1;
      uInt32 menu_widget    : 1;
      uInt32 always_on_top  : 1;
      uInt32 flat           : 1;
      uInt32 no_border      : 1;
      uInt32 no_underline   : 1;
      uInt32                : 12;  // padding bits
    };

    // Constructor
    explicit FWidget (FWidget* = nullptr);

    // Disable copy constructor
    FWidget (const FWidget&) = delete;

    // Destructor
    ~FWidget() override;

    // Disable copy assignment operator (=)
    FWidget& operator = (const FWidget&) = delete;

    // Accessors
    FString                  getClassName() const override;
    FWidget*                 getRootWidget();
    FWidget*                 getParentWidget() const;
    static FWidget*&         getMainWidget();
    static FWidget*&         getActiveWindow();
    static FWidget*&         getFocusWidget();
    static FWidget*&         getClickedWidget();
    static FWidget*&         getOpenMenu();
    static FWidget*&         getMoveSizeWidget();
    static FMenuBar*         getMenuBar();
    static FStatusBar*       getStatusBar();
    static auto              getColorTheme() -> std::shared_ptr<FWidgetColors>&;
    virtual FWidget*         getFirstFocusableWidget (FObjectList);
    virtual FWidget*         getLastFocusableWidget (FObjectList);
    const FAcceleratorList&  getAcceleratorList() const;
    FString                  getStatusbarMessage() const;
    FColor                   getForegroundColor() const;  // get the primary
    FColor                   getBackgroundColor() const;  // widget colors
    std::vector<bool>&       doubleFlatLine_ref (Side) noexcept;
    // Positioning and sizes accessors...
    int                      getX() const;
    int                      getY() const;
    FPoint                   getPos() const;
    int                      getTermX() const;
    int                      getTermY() const;
    FPoint                   getTermPos() const;
    std::size_t              getWidth() const;
    std::size_t              getHeight() const;
    FSize                    getSize() const;
    int                      getTopPadding() const;
    int                      getLeftPadding() const;
    int                      getBottomPadding() const;
    int                      getRightPadding() const;
    std::size_t              getClientWidth() const;
    std::size_t              getClientHeight() const;
    FSize                    getClientSize() const;
    std::size_t              getMaxWidth() const;
    std::size_t              getMaxHeight() const;
    const FSize&             getShadow() const;
    const FRect&             getGeometry() const;
    const FRect&             getGeometryWithShadow();
    const FRect&             getTermGeometry();
    const FRect&             getTermGeometryWithShadow();
    std::size_t              getDesktopWidth() const;
    std::size_t              getDesktopHeight() const;
    const FWidgetFlags&      getFlags() const;
    FPoint                   getCursorPos() const;
    FPoint                   getPrintPos();

    // Mutators
    static void              setMainWidget (FWidget*);
    static void              setFocusWidget (FWidget*);
    static void              setClickedWidget (FWidget*);
    static void              setMoveSizeWidget (FWidget*);
    static void              setActiveWindow (FWidget*);
    static void              setOpenMenu (FWidget*);
    template <typename ClassT>
    static void              setColorTheme();
    FAcceleratorList&        setAcceleratorList();
    virtual void             setStatusbarMessage (const FString&);
    bool                     setVisible (bool = true);
    bool                     unsetVisible();
    virtual bool             setEnable (bool = true);
    virtual bool             unsetEnable();
    virtual bool             setDisable();
    virtual bool             setVisibleCursor (bool = true);  // input cursor visibility
    virtual bool             unsetVisibleCursor();            // for the widget
    virtual bool             setFocus (bool = true);
    virtual bool             unsetFocus();
    void                     setFocusable (bool = true);
    void                     unsetFocusable();
    bool                     ignorePadding (bool = true);    // ignore padding from
    bool                     acceptPadding();                // the parent widget
    virtual void             setForegroundColor (FColor);
    virtual void             setBackgroundColor (FColor);
    virtual void             resetColors();
    void                     useParentWidgetColor();
    void                     setColor() const;
    FWidgetFlags&            setFlags();
    // Positioning and sizes mutators...
    virtual void             setX (int, bool = true);
    virtual void             setY (int, bool = true);
    virtual void             setPos (const FPoint&, bool = true);
    virtual void             setWidth (std::size_t, bool = true);
    virtual void             setHeight (std::size_t, bool = true);
    virtual void             setSize (const FSize&, bool = true);
    void                     setTopPadding (int, bool = true);
    void                     setLeftPadding (int, bool = true);
    void                     setBottomPadding (int, bool = true);
    void                     setRightPadding (int, bool = true);
    void                     setTerminalSize (const FSize&) const;
    virtual void             setGeometry (const FRect&, bool = true);
    virtual void             setGeometry (const FPoint&, const FSize&, bool = true);
    virtual void             setShadowSize (const FSize&);
    void                     setMinimumWidth (std::size_t);
    void                     setMinimumHeight (std::size_t);
    void                     setMinimumSize (const FSize&);
    void                     setMaximumWidth (std::size_t);
    void                     setMaximumHeight (std::size_t);
    void                     setMaximumSize (const FSize&);
    void                     setFixedSize (const FSize&);
    virtual bool             setCursorPos (const FPoint&);
    void                     unsetCursorPos();
    virtual void             setPrintPos (const FPoint&);
    void                     setDoubleFlatLine (Side, bool = true) noexcept;
    void                     unsetDoubleFlatLine (Side);
    void                     setDoubleFlatLine (Side, int, bool = true) noexcept;
    void                     unsetDoubleFlatLine (Side, int);

    // Inquiries
    bool                     isRootWidget() const;
    bool                     isWindowWidget() const;
    bool                     isDialogWidget() const;
    bool                     isMenuWidget() const;
    bool                     isVisible() const;
    bool                     isShown() const;
    bool                     isHidden() const;
    bool                     isEnabled() const;
    bool                     hasVisibleCursor() const;
    bool                     hasFocus() const;
    bool                     acceptFocus() const;  // is focusable
    bool                     isPaddingIgnored() const;

    // Methods
    FWidget*                 childWidgetAt (const FPoint&);
    int                      numOfFocusableChildren();
    virtual bool             close();
    void                     clearStatusbarMessage();
    template <typename... Args>
    void                     addCallback (const FString&, Args&&...) noexcept;
    template <typename... Args>
    void                     delCallback (Args&&...) noexcept;
    void                     emitCallback (const FString&) const;
    void                     addAccelerator (FKey);
    virtual void             addAccelerator (FKey, FWidget*);
    void                     delAccelerator ();
    virtual void             delAccelerator (FWidget*);
    virtual void             redraw();
    virtual void             resize();
    virtual void             show();
    virtual void             hide();
    virtual bool             focusFirstChild();  // widget focusing
    virtual bool             focusLastChild();
    FPoint                   termToWidgetPos (const FPoint&) const;
    void                     print (const FPoint&) override;
    virtual void             move (const FPoint&);
    virtual void             drawBorder();
    static void              quit();

  protected:
    // Accessor
    FTermArea*               getPrintArea() override;
    static uInt              getModalDialogCounter();
    static FWidgetList*&     getDialogList();
    static FWidgetList*&     getAlwaysOnTopList();
    static FWidgetList*&     getWidgetCloseList();
    void                     addPreprocessingHandler ( const FVTerm*
                                                     , FPreprocessingFunction&& ) override;
    void                     delPreprocessingHandler (const FVTerm*) override;

    // Inquiry
    bool                     isChildPrintArea() const;

    // Mutators
    virtual void             setStatusBar (FStatusBar*);
    virtual void             setMenuBar (FMenuBar*);
    static uInt&             setModalDialogCounter();
    void                     setParentOffset();
    void                     setTermOffset();
    void                     setTermOffsetWithPadding();

    // Methods
    void                     initTerminal() override;
    void                     initDesktop();
    virtual void             initLayout();
    virtual void             adjustSize();
    void                     adjustSizeGlobal();
    void                     hideArea (const FSize&);
    virtual bool             focusNextChild();  // Change child...
    virtual bool             focusPrevChild();  // ...focus

    // Event handlers
    bool                     event (FEvent*) override;
    virtual void             onKeyPress (FKeyEvent*);
    virtual void             onKeyUp (FKeyEvent*);
    virtual void             onKeyDown (FKeyEvent*);
    virtual void             onMouseDown (FMouseEvent*);
    virtual void             onMouseUp (FMouseEvent*);
    virtual void             onMouseDoubleClick (FMouseEvent*);
    virtual void             onWheel (FWheelEvent*);
    virtual void             onMouseMove (FMouseEvent*);
    virtual void             onFocusIn (FFocusEvent*);
    virtual void             onFocusOut (FFocusEvent*);
    virtual void             onChildFocusIn (FFocusEvent*);
    virtual void             onChildFocusOut (FFocusEvent*);
    virtual void             onAccel (FAccelEvent*);
    virtual void             onResize (FResizeEvent*);
    virtual void             onShow (FShowEvent*);
    virtual void             onHide (FHideEvent*);
    virtual void             onClose (FCloseEvent*);

  private:
    struct widget_size_hints
    {
      widget_size_hints() = default;
      ~widget_size_hints() = default;

      void setMinimum (const FSize& s)
      {
        min_width = s.getWidth();
        min_height = s.getHeight();
      }

      void setMaximum (const FSize& s)
      {
        max_width = s.getWidth();
        max_height = s.getHeight();
      }

      std::size_t min_width{0};
      std::size_t min_height{0};
      std::size_t max_width{INT_MAX};
      std::size_t max_height{INT_MAX};
    };

    struct dbl_line_mask
    {
      dbl_line_mask() = default;
      ~dbl_line_mask() = default;

      std::vector<bool> top{};
      std::vector<bool> right{};
      std::vector<bool> bottom{};
      std::vector<bool> left{};
    };

    struct widget_padding
    {
      widget_padding() = default;
      ~widget_padding() = default;

      int top{0};
      int left{0};
      int bottom{0};
      int right{0};
    };

    // Methods
    void                     determineDesktopSize();
    void                     initRootWidget();
    void                     initWidgetLayout();
    void                     finish();
    void                     insufficientSpaceAdjust();
    void                     KeyPressEvent (FKeyEvent*);
    void                     KeyDownEvent (FKeyEvent*);
    void                     emitWheelCallback (const FWheelEvent*) const;
    void                     setWindowFocus (bool = true);
    bool                     changeFocus (FWidget*, FWidget*, FocusTypes);
    void                     processDestroy() const;
    virtual void             draw();
    void                     drawWindows() const;
    void                     drawChildren();
    static bool              isDefaultTheme();
    static void              initColorTheme();
    void                     removeQueuedEvent() const;
    void                     setStatusbarText (bool = true) const;

    // Data members
    struct FWidgetFlags      flags{};
    FPoint                   widget_cursor_position{-1, -1};
    widget_size_hints        size_hints{};
    dbl_line_mask            double_flatline_mask{};
    widget_padding           padding{};
    bool                     ignore_padding{false};

    // widget size
    FRect                    wsize{1, 1, 1, 1};
    FRect                    adjust_wsize{1, 1, 1, 1};
    FRect                    adjust_wsize_term{};
    FRect                    adjust_wsize_shadow{};
    FRect                    adjust_wsize_term_shadow{};
    // widget offset
    FRect                    woffset{};
    // offset of the widget client area
    FRect                    wclient_offset{};
    // widget shadow size (on the right and bottom side)
    FSize                    wshadow{0, 0};

    // default widget foreground and background color
    FColor                   foreground_color{FColor::Default};
    FColor                   background_color{FColor::Default};
    FString                  statusbar_message{};
    FAcceleratorList         accelerator_list{};
    FCallback                callback_impl{};

    static FStatusBar*       statusbar;
    static FMenuBar*         menubar;
    static FWidget*          main_widget;
    static FWidget*          active_window;
    static FWidget*          focus_widget;
    static FWidget*          clicked_widget;
    static FWidget*          open_menu;
    static FWidget*          move_size_widget;
    static FWidget*          show_root_widget;
    static FWidget*          redraw_root_widget;
    static FWidgetList*      dialog_list;
    static FWidgetList*      always_on_top_list;
    static FWidgetList*      close_widget;
    static uInt              modal_dialog_counter;
    static bool              init_terminal;
    static bool              init_desktop;

    // Friend classes
    friend class FToggleButton;
    friend class FScrollView;

    // Friend functions
    friend void detectTerminalSize();
    friend void drawShadow (FWidget*);
    friend void drawTransparentShadow (FWidget*);
    friend void drawBlockShadow (FWidget*);
    friend void clearShadow (FWidget*);
    friend void drawFlatBorder (FWidget*);
    friend void clearFlatBorder (FWidget*);
};


// non-member function forward declarations
// implemented in fwidget_functions.cpp
//----------------------------------------------------------------------
void          detectTerminalSize();
bool          isFocusNextKey (const FKey);
bool          isFocusPrevKey (const FKey);
bool          isInFWidgetList (const FWidget::FWidgetList*, const FWidget*);
FKey          getHotkey (const FString&);
std::size_t   getHotkeyPos (const FString& src, FString& dest);
void          setHotkeyViaString (FWidget*, const FString&);
void          setWidgetFocus (FWidget*);
void          drawShadow (FWidget*);
void          drawTransparentShadow (FWidget*);
void          drawBlockShadow (FWidget*);
void          clearShadow (FWidget*);
void          drawFlatBorder (FWidget*);
void          clearFlatBorder (FWidget*);
void          checkBorder (const FWidget*, FRect&);
void          drawBorder (FWidget*, const FRect&);
void          drawListBorder (FWidget*, const FRect&);
void          drawBox (FWidget*, const FRect&);
void          drawNewFontBox (FWidget*, const FRect&);
void          drawNewFontListBox (FWidget*, const FRect&);


// FWidget inline functions
//----------------------------------------------------------------------
inline FString FWidget::getClassName() const
{ return "FWidget"; }

//----------------------------------------------------------------------
inline FWidget*& FWidget::getMainWidget()
{ return main_widget; }

//----------------------------------------------------------------------
inline FWidget*& FWidget::getActiveWindow()  // returns active FWindow object
{ return active_window; }

//----------------------------------------------------------------------
inline FWidget*& FWidget::getFocusWidget()
{ return focus_widget; }

//----------------------------------------------------------------------
inline FWidget*& FWidget::getClickedWidget()
{ return clicked_widget; }

//----------------------------------------------------------------------
inline FWidget*& FWidget::getOpenMenu()
{ return open_menu; }

//----------------------------------------------------------------------
inline FWidget*& FWidget::getMoveSizeWidget()
{ return move_size_widget; }

//----------------------------------------------------------------------
inline FMenuBar* FWidget::getMenuBar()
{ return menubar; }

//----------------------------------------------------------------------
inline FStatusBar* FWidget::getStatusBar()
{ return statusbar; }

//----------------------------------------------------------------------
inline const FWidget::FAcceleratorList& FWidget::getAcceleratorList() const
{ return accelerator_list; }

//----------------------------------------------------------------------
inline FWidget::FAcceleratorList& FWidget::setAcceleratorList()
{ return accelerator_list; }

//----------------------------------------------------------------------
inline FString FWidget::getStatusbarMessage() const
{ return statusbar_message; }

//----------------------------------------------------------------------
inline FColor FWidget::getForegroundColor() const
{ return foreground_color; }

//----------------------------------------------------------------------
inline FColor FWidget::getBackgroundColor() const
{ return background_color; }

//----------------------------------------------------------------------
inline int FWidget::getX() const  // x-position relative to the widget
{ return adjust_wsize.getX(); }

//----------------------------------------------------------------------
inline int FWidget::getY() const  // y-position relative to the widget
{ return adjust_wsize.getY(); }

//----------------------------------------------------------------------
inline FPoint FWidget::getPos() const  // position relative to the widget
{
  const FPoint& pos = adjust_wsize.getPos();  // initialize pos
  return pos;
}

//----------------------------------------------------------------------
inline int FWidget::getTermX() const  // x-position on terminal
{ return woffset.getX1() + adjust_wsize.getX(); }

//----------------------------------------------------------------------
inline int FWidget::getTermY() const  // y-position on terminal
{ return woffset.getY1() + adjust_wsize.getY(); }

//----------------------------------------------------------------------
inline FPoint FWidget::getTermPos() const  // position on terminal
{ return {getTermX(), getTermY()}; }

//----------------------------------------------------------------------
inline std::size_t FWidget::getWidth() const
{ return adjust_wsize.getWidth(); }

//----------------------------------------------------------------------
inline std::size_t FWidget::getHeight() const
{ return adjust_wsize.getHeight(); }

//----------------------------------------------------------------------
inline FSize FWidget::getSize() const
{
  const FSize& size = adjust_wsize.getSize();  // initialize size
  return size;
}

//----------------------------------------------------------------------
inline int FWidget::getTopPadding() const
{ return padding.top; }

//----------------------------------------------------------------------
inline int FWidget::getLeftPadding() const
{ return padding.left; }

//----------------------------------------------------------------------
inline int FWidget::getBottomPadding() const
{ return padding.bottom; }

//----------------------------------------------------------------------
inline int FWidget::getRightPadding() const
{ return padding.right; }

//----------------------------------------------------------------------
inline std::size_t FWidget::getClientWidth() const
{ return wclient_offset.getWidth(); }

//----------------------------------------------------------------------
inline std::size_t FWidget::getClientHeight() const
{ return wclient_offset.getHeight(); }

//----------------------------------------------------------------------
inline FSize FWidget::getClientSize() const
{
  const FSize& size = wclient_offset.getSize();  // initialize size
  return size;
}

//----------------------------------------------------------------------
inline std::size_t FWidget::getMaxWidth() const
{ return woffset.getWidth(); }

//----------------------------------------------------------------------
inline std::size_t FWidget::getMaxHeight() const
{ return woffset.getHeight(); }

//----------------------------------------------------------------------
inline const FSize& FWidget::getShadow() const
{ return wshadow; }

//----------------------------------------------------------------------
inline const FRect& FWidget::getGeometry() const
{ return adjust_wsize; }

//----------------------------------------------------------------------
inline const FRect& FWidget::getGeometryWithShadow()
{
  adjust_wsize_shadow.setCoordinates
  (
    adjust_wsize.x1_ref(),
    adjust_wsize.y1_ref(),
    adjust_wsize.x2_ref() + int(wshadow.width_ref()),
    adjust_wsize.y2_ref() + int(wshadow.height_ref())
  );

  return adjust_wsize_shadow;
}

//----------------------------------------------------------------------
inline const FRect& FWidget::getTermGeometry()
{
  adjust_wsize_term.setCoordinates
  (
    adjust_wsize.x1_ref() + woffset.x1_ref(),
    adjust_wsize.y1_ref() + woffset.y1_ref(),
    adjust_wsize.x2_ref() + woffset.x1_ref(),
    adjust_wsize.y2_ref() + woffset.y1_ref()
  );

  return adjust_wsize_term;
}

//----------------------------------------------------------------------
inline const FRect& FWidget::getTermGeometryWithShadow()
{
  adjust_wsize_term_shadow.setCoordinates
  (
    adjust_wsize.x1_ref() + woffset.x1_ref(),
    adjust_wsize.y1_ref() + woffset.y1_ref(),
    adjust_wsize.x2_ref() + woffset.x1_ref() + int(wshadow.width_ref()),
    adjust_wsize.y2_ref() + woffset.y1_ref() + int(wshadow.height_ref())
  );

  return adjust_wsize_term_shadow;
}

//----------------------------------------------------------------------
inline std::size_t FWidget::getDesktopWidth() const
{ return FVTerm::getFOutput()->getColumnNumber(); }

//----------------------------------------------------------------------
inline std::size_t FWidget::getDesktopHeight() const
{ return FVTerm::getFOutput()->getLineNumber(); }

//----------------------------------------------------------------------
inline const FWidget::FWidgetFlags& FWidget::getFlags() const
{ return flags; }

//----------------------------------------------------------------------
inline FPoint FWidget::getCursorPos() const
{ return widget_cursor_position; }

//----------------------------------------------------------------------
inline void FWidget::setActiveWindow (FWidget* obj)
{ active_window = obj; }

//----------------------------------------------------------------------
inline void FWidget::setFocusWidget (FWidget* obj)
{ focus_widget = obj; }

//----------------------------------------------------------------------
inline void FWidget::setClickedWidget (FWidget* obj)
{ clicked_widget = obj; }

//----------------------------------------------------------------------
inline void FWidget::setOpenMenu (FWidget* obj)
{ open_menu = obj; }

//----------------------------------------------------------------------
template <typename ClassT>
inline void FWidget::setColorTheme()
{
  getColorTheme() = std::make_shared<ClassT>();
}

//----------------------------------------------------------------------
inline void FWidget::setMoveSizeWidget (FWidget* obj)
{ move_size_widget = obj; }

//----------------------------------------------------------------------
inline void FWidget::setStatusbarMessage (const FString& msg)
{ statusbar_message = msg; }

//----------------------------------------------------------------------
inline bool FWidget::unsetVisible()
{ return setVisible(false); }

//----------------------------------------------------------------------
inline bool FWidget::unsetEnable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FWidget::setDisable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FWidget::setVisibleCursor (bool enable)
{ return (flags.visible_cursor = enable); }

//----------------------------------------------------------------------
inline bool FWidget::unsetVisibleCursor()
{ return setVisibleCursor(false); }

//----------------------------------------------------------------------
inline bool FWidget::unsetFocus()
{ return setFocus(false); }

//----------------------------------------------------------------------
inline void FWidget::setFocusable (bool enable)
{ flags.focusable = enable; }

//----------------------------------------------------------------------
inline void FWidget::unsetFocusable()
{ flags.focusable = false; }

//----------------------------------------------------------------------
inline bool FWidget::ignorePadding (bool enable)
{ return (ignore_padding = enable); }

//----------------------------------------------------------------------
inline bool FWidget::acceptPadding()
{ return (ignore_padding = false); }

//----------------------------------------------------------------------
inline void FWidget::setForegroundColor (FColor color)
{
  // valid colors -1..254
  if ( color == FColor::Default || (color >> 8) == FColor::Black )
    foreground_color = color;
}

//----------------------------------------------------------------------
inline void FWidget::setBackgroundColor (FColor color)
{
  // valid colors -1..254
  if ( color == FColor::Default || (color >> 8) == FColor::Black )
    background_color = color;
}

//----------------------------------------------------------------------
inline FWidget::FWidgetFlags& FWidget::setFlags()
{
  // Gives direct access to the widget flags
  return flags;
}

//----------------------------------------------------------------------
inline void FWidget::setGeometry (const FRect& box, bool adjust)
{
  setGeometry (box.getPos(), box.getSize(), adjust);
}

//----------------------------------------------------------------------
inline void FWidget::setShadowSize (const FSize& size)
{
  // width = right shadow  /  height = bottom shadow
  wshadow.setSize (size);
}

//----------------------------------------------------------------------
inline void FWidget::setMinimumWidth (std::size_t min_width)
{ size_hints.setMinimum (FSize(min_width, size_hints.min_height)); }

//----------------------------------------------------------------------
inline void FWidget::setMinimumHeight (std::size_t min_height)
{ size_hints.setMinimum (FSize(size_hints.min_width, min_height)); }

//----------------------------------------------------------------------
inline void FWidget::setMinimumSize (const FSize& size)
{
  // Sets the lower size limit to which the widget can be resized
  size_hints.setMinimum (size);
}

//----------------------------------------------------------------------
inline void FWidget::setMaximumWidth (std::size_t max_width)
{ size_hints.setMaximum (FSize(max_width, size_hints.max_height)); }

//----------------------------------------------------------------------
inline void FWidget::setMaximumHeight (std::size_t max_height)
{ size_hints.setMaximum (FSize(size_hints.max_width, max_height)); }

//----------------------------------------------------------------------
inline void FWidget::setMaximumSize (const FSize& size)
{
  // Sets the upper size limit to which the widget can be resized
  size_hints.setMaximum (size);
}

//----------------------------------------------------------------------
inline void FWidget::setFixedSize (const FSize& size)
{
  // Sets the minimum and maximum size limit to fixed sizes to prevent
  // the widget resizing
  size_hints.setMinimum (size);
  size_hints.setMaximum (size);
}

//----------------------------------------------------------------------
inline void FWidget::unsetCursorPos()
{ setCursorPos ({-1, -1}); }

//----------------------------------------------------------------------
inline void FWidget::unsetDoubleFlatLine (Side side)
{ setDoubleFlatLine(side, false); }

//----------------------------------------------------------------------
inline void FWidget::unsetDoubleFlatLine (Side side, int pos)
{ setDoubleFlatLine(side, pos, false); }

//----------------------------------------------------------------------
inline bool FWidget::isRootWidget() const
{ return (! hasParent()); }

//----------------------------------------------------------------------
inline bool FWidget::isVisible() const
{ return flags.visible; }

//----------------------------------------------------------------------
inline bool FWidget::isShown() const
{ return flags.shown; }

//----------------------------------------------------------------------
inline bool FWidget::isHidden() const
{ return flags.hidden; }

//----------------------------------------------------------------------
inline bool FWidget::isWindowWidget() const
{ return flags.window_widget; }

//----------------------------------------------------------------------
inline bool FWidget::isDialogWidget() const
{ return flags.dialog_widget; }

//----------------------------------------------------------------------
inline bool FWidget::isMenuWidget() const
{ return flags.menu_widget; }

//----------------------------------------------------------------------
inline bool FWidget::isEnabled() const
{ return flags.active; }

//----------------------------------------------------------------------
inline bool FWidget::hasVisibleCursor() const
{ return flags.visible_cursor; }

//----------------------------------------------------------------------
inline bool FWidget::hasFocus() const
{ return flags.focus; }

//----------------------------------------------------------------------
inline bool FWidget::acceptFocus() const  // is focusable
{ return flags.focusable; }

//----------------------------------------------------------------------
inline bool FWidget::isPaddingIgnored() const
{ return ignore_padding; }

//----------------------------------------------------------------------
inline void FWidget::clearStatusbarMessage()
{ statusbar_message.clear(); }

//----------------------------------------------------------------------
template <typename... Args>
inline void FWidget::addCallback (const FString& cb_signal, Args&&... args) noexcept
{
  callback_impl.addCallback (cb_signal, std::forward<Args>(args)...);
}

//----------------------------------------------------------------------
template <typename... Args>
inline void FWidget::delCallback (Args&&... args) noexcept
{
  callback_impl.delCallback(std::forward<Args>(args)...);
}

//----------------------------------------------------------------------
inline void FWidget::emitCallback (const FString& emit_signal) const
{
  callback_impl.emitCallback(emit_signal);
}

//----------------------------------------------------------------------
inline void FWidget::addAccelerator (FKey key)
{ addAccelerator (key, this); }

//----------------------------------------------------------------------
inline void FWidget::delAccelerator()
{ delAccelerator(this); }

//----------------------------------------------------------------------
inline FPoint FWidget::termToWidgetPos (const FPoint& tPos) const
{
  return { tPos.getX() + 1 - woffset.getX1() - adjust_wsize.getX()
         , tPos.getY() + 1 - woffset.getY1() - adjust_wsize.getY() };
}

//----------------------------------------------------------------------
inline void FWidget::print (const FPoint& pos)
{
  setPrintPos (pos);
}

//----------------------------------------------------------------------
inline void FWidget::drawBorder()
{
  finalcut::drawBorder (this, FRect(FPoint{1, 1}, getSize()));
}

//----------------------------------------------------------------------
inline uInt FWidget::getModalDialogCounter()
{ return modal_dialog_counter; }

//----------------------------------------------------------------------
inline FWidget::FWidgetList*& FWidget::getDialogList()
{ return dialog_list; }

//----------------------------------------------------------------------
inline FWidget::FWidgetList*& FWidget::getAlwaysOnTopList()
{ return always_on_top_list; }

//----------------------------------------------------------------------
inline FWidget::FWidgetList*& FWidget::getWidgetCloseList()
{ return close_widget; }

//----------------------------------------------------------------------
inline uInt& FWidget::setModalDialogCounter()
{ return modal_dialog_counter; }

//----------------------------------------------------------------------
inline void FWidget::processDestroy() const
{ emitCallback("destroy"); }


// Non-member elements for NewFont
//----------------------------------------------------------------------
constexpr wchar_t NF_menu_button[]
{
  wchar_t(UniChar::NF_rev_menu_button1),
  wchar_t(UniChar::NF_rev_menu_button2),
  wchar_t(UniChar::NF_rev_menu_button3),
  '\0'
};

constexpr wchar_t NF_button_up[]
{
  wchar_t(UniChar::NF_rev_up_pointing_triangle1),
  wchar_t(UniChar::NF_rev_up_pointing_triangle2),
  '\0'
};

constexpr wchar_t NF_button_down[]
{
  wchar_t(UniChar::NF_rev_down_pointing_triangle1),
  wchar_t(UniChar::NF_rev_down_pointing_triangle2),
  '\0'
};

constexpr wchar_t NF_button_arrow_up[]
{
  wchar_t(UniChar::NF_rev_up_arrow1),
  wchar_t(UniChar::NF_rev_up_arrow2),
  '\0'
};

constexpr wchar_t NF_button_arrow_down[]
{
  wchar_t(UniChar::NF_rev_down_arrow1),
  wchar_t(UniChar::NF_rev_down_arrow2),
  '\0'
};

constexpr wchar_t NF_button_arrow_left[]
{
  wchar_t(UniChar::NF_rev_left_arrow1),
  wchar_t(UniChar::NF_rev_left_arrow2),
  '\0'
};

constexpr wchar_t NF_button_arrow_right[]
{
  wchar_t(UniChar::NF_rev_right_arrow1),
  wchar_t(UniChar::NF_rev_right_arrow2),
  '\0'
};

constexpr wchar_t NF_Drive[]
{
  wchar_t(UniChar::NF_shadow_box_left),
  wchar_t(UniChar::NF_shadow_box_middle),
  wchar_t(UniChar::NF_shadow_box_hdd),
  wchar_t(UniChar::NF_shadow_box_right),
  '\0'
};

constexpr wchar_t NF_CD_ROM[]
{
  wchar_t(UniChar::NF_shadow_box_left),
  wchar_t(UniChar::NF_shadow_box_middle),
  wchar_t(UniChar::NF_shadow_box_cd),
  wchar_t(UniChar::NF_shadow_box_right),
  '\0'
};

constexpr wchar_t NF_Net_Drive[]
{
  wchar_t(UniChar::NF_shadow_box_left),
  wchar_t(UniChar::NF_shadow_box_middle),
  wchar_t(UniChar::NF_shadow_box_net),
  wchar_t(UniChar::NF_shadow_box_right),
  '\0'
};

constexpr wchar_t CHECKBOX[]
{
  wchar_t(UniChar::NF_shadow_box_left),
  wchar_t(UniChar::NF_shadow_box_middle),
  wchar_t(UniChar::NF_shadow_box_right),
  '\0'
};

constexpr wchar_t CHECKBOX_ON[]
{
  wchar_t(UniChar::NF_shadow_box_left),
  wchar_t(UniChar::NF_shadow_box_checked),
  wchar_t(UniChar::NF_shadow_box_right),
  '\0'
};

constexpr wchar_t RADIO_BUTTON[]
{
  wchar_t(UniChar::NF_radio_button1),
  wchar_t(UniChar::NF_radio_button2),
  wchar_t(UniChar::NF_radio_button3),
  '\0'
};

constexpr wchar_t CHECKED_RADIO_BUTTON[]
{
  wchar_t(UniChar::NF_radio_button1),
  wchar_t(UniChar::NF_radio_button2_checked),
  wchar_t(UniChar::NF_radio_button3),
  '\0'
};

}  // namespace finalcut

#endif  // FWIDGET_H
