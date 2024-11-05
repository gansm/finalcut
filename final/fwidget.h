/***********************************************************************
* fwidget.h - Intermediate base class for all widget objects           *
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
#include <unordered_map>
#include <utility>
#include <vector>

#include "final/fobject.h"
#include "final/ftypes.h"
#include "final/fwidget_flags.h"
#include "final/fwidget_functions.h"
#include "final/output/foutput.h"
#include "final/util/fcallback.h"
#include "final/util/fpoint.h"
#include "final/util/frect.h"
#include "final/util/fsize.h"
#include "final/vterm/fvterm.h"

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

class FWidget : public FVTerm
              , public FObject
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

    // Constructor
    explicit FWidget (FWidget* = nullptr);

    // Destructor
    ~FWidget() override;

    // Accessors
    auto  getClassName() const -> FString override;
    auto  getRootWidget() const -> FWidget*;
    auto  getParentWidget() const -> FWidget*;
    static auto  getMainWidget() -> FWidget*&;
    static auto  getActiveWindow() -> FWidget*&;
    static auto  getFocusWidget() -> FWidget*&;
    static auto  getClickedWidget() -> FWidget*&;
    static auto  getOpenMenu() -> FWidget*&;
    static auto  getMoveResizeWidget() -> FWidget*&;
    static auto  getMenuBar() -> FMenuBar*;
    static auto  getStatusBar() -> FStatusBar*;
    static auto  getColorTheme() -> std::shared_ptr<FWidgetColors>&;
    auto  getAcceleratorList() const & -> const FAcceleratorList&;
    auto  getStatusbarMessage() const -> FString;
    auto  getForegroundColor() const noexcept -> FColor;  // get the primary
    auto  getBackgroundColor() const noexcept -> FColor;  // widget colors
    auto  doubleFlatLine_ref (Side) -> std::vector<bool>&;
    // Positioning and sizes accessors...
    auto  getX() const -> int;
    auto  getY() const -> int;
    auto  getPos() const -> FPoint;
    auto  getTermX() const -> int;
    auto  getTermY() const -> int;
    auto  getTermPos() const -> FPoint;
    auto  getWidth() const -> std::size_t;
    auto  getHeight() const -> std::size_t;
    auto  getSize() const -> FSize;
    auto  getTopPadding() const noexcept -> int;
    auto  getLeftPadding() const noexcept -> int;
    auto  getBottomPadding() const noexcept -> int;
    auto  getRightPadding() const noexcept -> int;
    auto  getClientWidth() const -> std::size_t;
    auto  getClientHeight() const -> std::size_t;
    auto  getClientSize() const -> FSize;
    auto  getMaxWidth() const -> std::size_t;
    auto  getMaxHeight() const -> std::size_t;
    auto  getShadow() const & -> const FSize&;
    auto  getGeometry() const & -> const FRect&;
    auto  getGeometryWithShadow() & -> const FRect&;
    auto  getTermGeometry() & -> const FRect&;
    auto  getTermGeometryWithShadow() & -> const FRect&;
    auto  getDesktopWidth() const -> std::size_t;
    auto  getDesktopHeight() const -> std::size_t;
    auto  getFlags() const & -> const FWidgetFlags&;
    auto  getCursorPos() const -> FPoint;
    auto  getPrintPos() -> FPoint;

    // Mutators
    static void  setMainWidget (FWidget*);
    static void  setFocusWidget (FWidget*);
    static void  setClickedWidget (FWidget*);
    static void  setMoveSizeWidget (FWidget*);
    static void  setActiveWindow (FWidget*);
    static void  setOpenMenu (FWidget*);
    template <typename ClassT>
    static void  setColorTheme();
    auto  setAcceleratorList() & -> FAcceleratorList&;
    virtual void setStatusbarMessage (const FString&);
    void  setVisible (bool = true);
    void  unsetVisible();
    virtual void setEnable (bool = true);
    virtual void unsetEnable();
    virtual void setDisable();
    virtual void setVisibleCursor (bool = true);  // input cursor visibility
    virtual void unsetVisibleCursor();            // for the widget
    virtual auto setFocus ( bool = true
                          , FocusTypes = FocusTypes::DefiniteWidget) -> bool;
    virtual auto unsetFocus() -> bool;
    void  setFocusable (bool = true);
    void  unsetFocusable();
    auto  ignorePadding (bool = true) -> bool;    // ignore padding from
    auto  acceptPadding() -> bool;                // the parent widget
    virtual void setForegroundColor (FColor);
    virtual void setBackgroundColor (FColor);
    virtual void resetColors();
    void  useParentWidgetColor();
    void  setColor() const;
    auto  setFlags() & -> FWidgetFlags&;
    // Positioning and sizes mutators...
    virtual void setX (int, bool = true);
    virtual void setY (int, bool = true);
    virtual void setPos (const FPoint&, bool = true);
    virtual void setWidth (std::size_t, bool = true);
    virtual void setHeight (std::size_t, bool = true);
    virtual void setSize (const FSize&, bool = true);
    void  setTopPadding (int, bool = true);
    void  setLeftPadding (int, bool = true);
    void  setBottomPadding (int, bool = true);
    void  setRightPadding (int, bool = true);
    void  setTerminalSize (const FSize&) const;
    virtual void setGeometry (const FRect&, bool = true);
    virtual void setGeometry (const FPoint&, const FSize&, bool = true);
    virtual void setShadowSize (const FSize&);
    void  setMinimumWidth (std::size_t);
    void  setMinimumHeight (std::size_t);
    void  setMinimumSize (const FSize&);
    void  setMaximumWidth (std::size_t);
    void  setMaximumHeight (std::size_t);
    void  setMaximumSize (const FSize&);
    void  setFixedSize (const FSize&);
    virtual auto setCursorPos (const FPoint&) -> bool;
    void  unsetCursorPos();
    virtual void setPrintPos (const FPoint&);
    void  setDoubleFlatLine (Side, bool = true);
    void  unsetDoubleFlatLine (Side);
    void  setDoubleFlatLine (Side, int, bool = true);
    void  unsetDoubleFlatLine (Side, int);

    // Inquiries
    auto  isRootWidget() const -> bool;
    auto  isWindowWidget() const -> bool;
    auto  isDialogWidget() const -> bool;
    auto  isMenuWidget() const -> bool;
    auto  isVisible() const -> bool;
    auto  isShown() const -> bool;
    auto  isHidden() const -> bool;
    auto  isEnabled() const -> bool;
    auto  hasVisibleCursor() const -> bool;
    auto  hasFocus() const -> bool;
    auto  acceptFocus() const -> bool;  // is focusable
    auto  isPaddingIgnored() const -> bool;

    // Methods
    auto  childWidgetAt (const FPoint&) & -> FWidget*;
    auto  numOfFocusableChildren() & -> int;
    virtual auto close() -> bool;
    void  clearStatusbarMessage();
    template <typename... Args>
    void  addCallback (FString&&, Args&&...) & noexcept;
    template <typename... Args>
    void  delCallback (Args&&...) & noexcept;
    void  emitCallback (const FString&) const &;
    void  addAccelerator (FKey) &;
    virtual void addAccelerator (FKey, FWidget*) &;
    void  delAccelerator () &;
    virtual void delAccelerator (FWidget*) &;
    virtual void flushChanges();
    virtual void redraw();
    virtual void resize();
    virtual void show();
    virtual void hide();
    virtual auto focusNextChild() -> bool;  // Change child...
    virtual auto focusPrevChild() -> bool;  // ...focus
    virtual auto focusFirstChild() & -> bool;
    virtual auto focusLastChild() & -> bool;
    auto  termToWidgetPos (const FPoint&) const -> FPoint;
    void  print (const FPoint&) override;
    virtual void move (const FPoint&);
    virtual void drawBorder();
    static void  quit();

  protected:
    // Accessor
    auto  getPrintArea() -> FTermArea* override;
    static auto getModalDialogCounter() -> uInt;
    static auto getDialogList() -> FWidgetList*&;
    static auto getAlwaysOnTopList() -> FWidgetList*&;
    static auto getWidgetCloseList() -> FWidgetList*&;
    void  addPreprocessingHandler ( const FVTerm*
                                  , FPreprocessingFunction&& ) override;
    void  delPreprocessingHandler (const FVTerm*) override;

    // Inquiry
    auto  isChildPrintArea() const -> bool;

    // Mutators
    virtual void setStatusBar (FStatusBar*);
    virtual void setMenuBar (FMenuBar*);
    static auto  setModalDialogCounter() -> uInt&;
    void  setParentOffset();
    void  setTermOffset();
    void  setTermOffsetWithPadding();

    // Methods
    void  initTerminal() override;
    void  initDesktop();
    virtual void initLayout();
    virtual void adjustSize();
    void  adjustSizeGlobal();
    void  hideArea (const FSize&);

    // Event handlers
    auto  event (FEvent*) -> bool override;
    virtual void onKeyPress (FKeyEvent*);
    virtual void onKeyUp (FKeyEvent*);
    virtual void onKeyDown (FKeyEvent*);
    virtual void onMouseDown (FMouseEvent*);
    virtual void onMouseUp (FMouseEvent*);
    virtual void onMouseDoubleClick (FMouseEvent*);
    virtual void onWheel (FWheelEvent*);
    virtual void onMouseMove (FMouseEvent*);
    virtual void onFocusIn (FFocusEvent*);
    virtual void onFocusOut (FFocusEvent*);
    virtual void onChildFocusIn (FFocusEvent*);
    virtual void onChildFocusOut (FFocusEvent*);
    virtual void onTermFocusIn (FFocusEvent*);
    virtual void onTermFocusOut (FFocusEvent*);
    virtual void onFailAtChildFocus (FFocusEvent*);
    virtual void onAccel (FAccelEvent*);
    virtual void onResize (FResizeEvent*);
    virtual void onShow (FShowEvent*);
    virtual void onHide (FHideEvent*);
    virtual void onClose (FCloseEvent*);

  private:
    // Using-declaration
    using EventHandler = std::function<void(FEvent*)>;
    using EventMap = std::unordered_map<Event, EventHandler, EnumHash<Event>>;

    struct WidgetSizeHints
    {
      WidgetSizeHints() = default;
      ~WidgetSizeHints() = default;

      inline void setMinimum (const FSize& s)
      {
        min_width = s.getWidth();
        min_height = s.getHeight();
      }

      inline void setMaximum (const FSize& s)
      {
        max_width = s.getWidth();
        max_height = s.getHeight();
      }

      std::size_t min_width{0};
      std::size_t min_height{0};
      std::size_t max_width{INT_MAX};
      std::size_t max_height{INT_MAX};
    };

    struct DoubleLineMask
    {
      DoubleLineMask() = default;
      ~DoubleLineMask() = default;

      inline void setWidth (std::size_t width)
      {
        if ( top.size() == width && bottom.size() == width )
          return;

        top.resize (width, false);
        bottom.resize (width, false);
      }

      inline void setHeight (std::size_t height)
      {
        if ( right.size() == height && left.size() == height )
          return;

        right.resize (height, false);
        left.resize (height, false);
      }

      inline void setSize (std::size_t width, std::size_t height)
      {
        setWidth (width);
        setHeight (height);
      }

      std::vector<bool> top{};
      std::vector<bool> right{};
      std::vector<bool> bottom{};
      std::vector<bool> left{};
    };

    struct WidgetPadding
    {
      WidgetPadding() = default;
      ~WidgetPadding() = default;

      int top{0};
      int left{0};
      int bottom{0};
      int right{0};
    };

    // Methods
    void  determineDesktopSize();
    void  mapEventFunctions();
    void  mapKeyEvents();
    void  mapMouseEvents();
    void  mapFocusEvents();
    void  mapWidgetEvents();
    void  initRootWidget();
    void  initWidgetLayout();
    void  initDesktopOnShown() const;
    void  finish();
    void  startShow();
    void  finalizeShow() const;
    void  showChildWidgets();
    void  moveLeftIfNotEnoughSpace();
    void  moveUpIfNotEnoughSpace();
    void  reduceWidthIfNotEnoughSpace();
    void  reduceHeightIfNotEnoughSpace();
    void  insufficientSpaceAdjust();
    void  KeyPressEvent (FKeyEvent*);
    void  KeyDownEvent (FKeyEvent*);
    void  emitWheelCallback (const FWheelEvent*) const;
    void  setWindowFocus (bool = true);
    auto  searchForwardForWidget ( const FWidget*
                                 , const FWidget*) const  -> FObjectList::const_iterator;
    auto  searchBackwardsForWidget ( const FWidget*
                                   , const FWidget* ) const -> FObjectList::const_reverse_iterator;
    auto  isViewable() const -> bool;
    auto  canReceiveFocus (const FWidget*) const -> bool;
    void  setFocusOnThisWidget (FocusTypes);
    auto  sendFailAtChildFocusEvent (FWidget*, FocusTypes) const -> bool;
    auto  sendFocusOutEvent (FWidget*, FocusTypes) const -> bool;
    auto  sendFocusInEvent (FWidget*, FocusTypes) const -> bool;
    void  processDestroy() const;
    virtual void draw();
    void  drawWindows() const;
    void  drawChildren();
    void  adjustWidget();
    void  adjustSizeWithinArea (FRect&) const;
    void  adjustChildWidgetSizes();
    void  setWindowOffset();
    void  setWidgetOffset (const FWidget*);
    void  setClientOffset();
    static auto  isDefaultTheme() -> bool;
    static void  initColorTheme();
    void  removeQueuedEvent() const;
    void  setStatusbarText (bool = true) const;

    // Data members
    struct FWidgetFlags  flags{};
    FPoint               widget_cursor_position{-1, -1};
    WidgetSizeHints      size_hints{};
    DoubleLineMask       double_flatline_mask{};
    WidgetPadding        padding{};

    // widget size
    FRect                wsize{1, 1, 1, 1};
    FRect                adjust_wsize{1, 1, 1, 1};
    FRect                adjust_wsize_term{};
    FRect                adjust_wsize_shadow{};
    FRect                adjust_wsize_term_shadow{};
    // widget offset
    FRect                woffset{};
    // offset of the widget client area
    FRect                wclient_offset{};
    // widget shadow size (on the right and bottom side)
    FSize                wshadow{0, 0};

    // default widget foreground and background color
    FColor               foreground_color{FColor::Default};
    FColor               background_color{FColor::Default};
    FString              statusbar_message{};
    FAcceleratorList     accelerator_list{};
    EventMap             event_map{};
    FCallback            callback_impl{};

    static FStatusBar*   statusbar;
    static FMenuBar*     menubar;
    static FWidget*      main_widget;
    static FWidget*      active_window;
    static FWidget*      focus_widget;
    static FWidget*      clicked_widget;
    static FWidget*      open_menu;
    static FWidget*      move_resize_widget;
    static FWidget*      first_shown_widget;
    static FWidget*      redraw_root_widget;
    static FWidgetList*  dialog_list;
    static FWidgetList*  always_on_top_list;
    static FWidgetList*  close_widget_list;
    static uInt          modal_dialog_counter;
    static bool          init_terminal;
    static bool          init_desktop;

    // Friend functions
    friend void  detectTerminalSize();
    friend void  drawShadow (FWidget*);
    friend void  drawTransparentShadow (FWidget*);
    friend void  drawGenericBox (FWidget*, const FRect&, const std::array<wchar_t, 8>&);
    friend void  drawGenericBlockShadow (FWidget*, const std::array<FChar, 4>&);
    friend void  drawFlatBorder (FWidget*);
    friend void  clearFlatBorder (FWidget*);
};


// FWidget inline functions
//----------------------------------------------------------------------
inline auto FWidget::getClassName() const -> FString
{ return "FWidget"; }

//----------------------------------------------------------------------
inline auto FWidget::getMainWidget() -> FWidget*&
{ return main_widget; }

//----------------------------------------------------------------------
inline auto FWidget::getActiveWindow() -> FWidget*&  // returns active FWindow object
{ return active_window; }

//----------------------------------------------------------------------
inline auto FWidget::getFocusWidget() -> FWidget*&
{ return focus_widget; }

//----------------------------------------------------------------------
inline auto FWidget::getClickedWidget() -> FWidget*&
{ return clicked_widget; }

//----------------------------------------------------------------------
inline auto FWidget::getOpenMenu() -> FWidget*&
{ return open_menu; }

//----------------------------------------------------------------------
inline auto FWidget::getMoveResizeWidget() -> FWidget*&
{ return move_resize_widget; }

//----------------------------------------------------------------------
inline auto FWidget::getMenuBar() -> FMenuBar*
{ return menubar; }

//----------------------------------------------------------------------
inline auto FWidget::getStatusBar() -> FStatusBar*
{ return statusbar; }

//----------------------------------------------------------------------
inline auto FWidget::getAcceleratorList() const & -> const FAcceleratorList&
{ return accelerator_list; }

//----------------------------------------------------------------------
inline auto FWidget::setAcceleratorList() & -> FAcceleratorList&
{ return accelerator_list; }

//----------------------------------------------------------------------
inline auto FWidget::getStatusbarMessage() const -> FString
{ return statusbar_message; }

//----------------------------------------------------------------------
inline auto FWidget::getForegroundColor() const noexcept -> FColor
{ return foreground_color; }

//----------------------------------------------------------------------
inline auto FWidget::getBackgroundColor() const noexcept -> FColor
{ return background_color; }

//----------------------------------------------------------------------
inline auto FWidget::getX() const -> int  // x-position relative to the widget
{ return adjust_wsize.getX(); }

//----------------------------------------------------------------------
inline auto FWidget::getY() const -> int  // y-position relative to the widget
{ return adjust_wsize.getY(); }

//----------------------------------------------------------------------
inline auto FWidget::getPos() const -> FPoint  // position relative to the widget
{
  const FPoint& pos = adjust_wsize.getPos();  // initialize pos
  return pos;
}

//----------------------------------------------------------------------
inline auto FWidget::getTermX() const -> int  // x-position on terminal
{ return woffset.getX1() + adjust_wsize.getX(); }

//----------------------------------------------------------------------
inline auto FWidget::getTermY() const -> int  // y-position on terminal
{ return woffset.getY1() + adjust_wsize.getY(); }

//----------------------------------------------------------------------
inline auto FWidget::getTermPos() const -> FPoint  // position on terminal
{ return {getTermX(), getTermY()}; }

//----------------------------------------------------------------------
inline auto FWidget::getWidth() const -> std::size_t
{ return adjust_wsize.getWidth(); }

//----------------------------------------------------------------------
inline auto FWidget::getHeight() const -> std::size_t
{ return adjust_wsize.getHeight(); }

//----------------------------------------------------------------------
inline auto FWidget::getSize() const -> FSize
{
  const FSize& size = adjust_wsize.getSize();  // initialize size
  return size;
}

//----------------------------------------------------------------------
inline auto FWidget::getTopPadding() const noexcept -> int
{ return padding.top; }

//----------------------------------------------------------------------
inline auto FWidget::getLeftPadding() const noexcept -> int
{ return padding.left; }

//----------------------------------------------------------------------
inline auto FWidget::getBottomPadding() const noexcept -> int
{ return padding.bottom; }

//----------------------------------------------------------------------
inline auto FWidget::getRightPadding() const noexcept -> int
{ return padding.right; }

//----------------------------------------------------------------------
inline auto FWidget::getClientWidth() const -> std::size_t
{ return wclient_offset.getWidth(); }

//----------------------------------------------------------------------
inline auto FWidget::getClientHeight() const -> std::size_t
{ return wclient_offset.getHeight(); }

//----------------------------------------------------------------------
inline auto FWidget::getClientSize() const -> FSize
{
  const FSize& size = wclient_offset.getSize();  // initialize size
  return size;
}

//----------------------------------------------------------------------
inline auto FWidget::getMaxWidth() const -> std::size_t
{ return woffset.getWidth(); }

//----------------------------------------------------------------------
inline auto FWidget::getMaxHeight() const -> std::size_t
{ return woffset.getHeight(); }

//----------------------------------------------------------------------
inline auto FWidget::getShadow() const & -> const FSize&
{ return wshadow; }

//----------------------------------------------------------------------
inline auto FWidget::getGeometry() const & -> const FRect&
{ return adjust_wsize; }

//----------------------------------------------------------------------
inline auto FWidget::getGeometryWithShadow() & -> const FRect&
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
inline auto FWidget::getTermGeometry() & -> const FRect&
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
inline auto FWidget::getTermGeometryWithShadow() & -> const FRect&
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
inline auto FWidget::getDesktopWidth() const -> std::size_t
{ return FVTerm::getFOutput()->getColumnNumber(); }

//----------------------------------------------------------------------
inline auto FWidget::getDesktopHeight() const -> std::size_t
{ return FVTerm::getFOutput()->getLineNumber(); }

//----------------------------------------------------------------------
inline auto FWidget::getFlags() const & -> const FWidgetFlags&
{ return flags; }

//----------------------------------------------------------------------
inline auto FWidget::getCursorPos() const -> FPoint
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
{ move_resize_widget = obj; }

//----------------------------------------------------------------------
inline void FWidget::setStatusbarMessage (const FString& msg)
{ statusbar_message = msg; }

//----------------------------------------------------------------------
inline void FWidget::unsetVisible()
{ setVisible(false); }

//----------------------------------------------------------------------
inline void FWidget::unsetEnable()
{ setEnable(false); }

//----------------------------------------------------------------------
inline void FWidget::setDisable()
{ setEnable(false); }

//----------------------------------------------------------------------
inline void FWidget::setVisibleCursor (bool enable)
{ flags.visibility.visible_cursor = enable; }

//----------------------------------------------------------------------
inline void FWidget::unsetVisibleCursor()
{ setVisibleCursor(false); }

//----------------------------------------------------------------------
inline auto FWidget::unsetFocus() -> bool
{ return setFocus(false, FocusTypes::DefiniteWidget); }

//----------------------------------------------------------------------
inline void FWidget::setFocusable (bool enable)
{ flags.focus.focusable = enable; }

//----------------------------------------------------------------------
inline void FWidget::unsetFocusable()
{ flags.focus.focusable = false; }

//----------------------------------------------------------------------
inline auto FWidget::ignorePadding (bool enable) -> bool
{ return (flags.feature.ignore_padding = enable); }

//----------------------------------------------------------------------
inline auto FWidget::acceptPadding() -> bool
{ return (flags.feature.ignore_padding = false); }

//----------------------------------------------------------------------
inline void FWidget::setForegroundColor (FColor color)
{
  // valid colors -1..255
  if ( color == FColor::Default || (color >> 8) == FColor::Black )
    foreground_color = color;
}

//----------------------------------------------------------------------
inline void FWidget::setBackgroundColor (FColor color)
{
  // valid colors -1..255
  if ( color == FColor::Default || (color >> 8) == FColor::Black )
    background_color = color;
}

//----------------------------------------------------------------------
inline auto FWidget::setFlags() & -> FWidgetFlags&
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
inline auto FWidget::isRootWidget() const -> bool
{ return (! hasParent()); }

//----------------------------------------------------------------------
inline auto FWidget::isVisible() const -> bool
{ return flags.visibility.visible; }

//----------------------------------------------------------------------
inline auto FWidget::isShown() const -> bool
{ return flags.visibility.shown; }

//----------------------------------------------------------------------
inline auto FWidget::isHidden() const -> bool
{ return flags.visibility.hidden; }

//----------------------------------------------------------------------
inline auto FWidget::isWindowWidget() const -> bool
{ return flags.type.window_widget; }

//----------------------------------------------------------------------
inline auto FWidget::isDialogWidget() const -> bool
{ return flags.type.dialog_widget; }

//----------------------------------------------------------------------
inline auto FWidget::isMenuWidget() const -> bool
{ return flags.type.menu_widget; }

//----------------------------------------------------------------------
inline auto FWidget::isEnabled() const -> bool
{ return flags.feature.active; }

//----------------------------------------------------------------------
inline auto FWidget::hasVisibleCursor() const -> bool
{ return flags.visibility.visible_cursor; }

//----------------------------------------------------------------------
inline auto FWidget::hasFocus() const -> bool
{ return flags.focus.focus; }

//----------------------------------------------------------------------
inline auto FWidget::acceptFocus() const -> bool  // is focusable
{ return flags.focus.focusable; }

//----------------------------------------------------------------------
inline auto FWidget::isPaddingIgnored() const -> bool
{ return flags.feature.ignore_padding; }

//----------------------------------------------------------------------
inline void FWidget::clearStatusbarMessage()
{ statusbar_message.clear(); }

//----------------------------------------------------------------------
template <typename... Args>
inline void FWidget::addCallback (FString&& cb_signal, Args&&... args) & noexcept
{
  callback_impl.addCallback (std::move(cb_signal), std::forward<Args>(args)...);
}

//----------------------------------------------------------------------
template <typename... Args>
inline void FWidget::delCallback (Args&&... args) & noexcept
{
  callback_impl.delCallback(std::forward<Args>(args)...);
}

//----------------------------------------------------------------------
inline void FWidget::emitCallback (const FString& emit_signal) const &
{
  callback_impl.emitCallback(emit_signal);
}

//----------------------------------------------------------------------
inline void FWidget::addAccelerator (FKey key) &
{ addAccelerator (key, this); }

//----------------------------------------------------------------------
inline void FWidget::delAccelerator() &
{ delAccelerator(this); }

//----------------------------------------------------------------------
inline auto FWidget::termToWidgetPos (const FPoint& tPos) const -> FPoint
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
inline auto FWidget::getModalDialogCounter() -> uInt
{ return modal_dialog_counter; }

//----------------------------------------------------------------------
inline auto FWidget::getDialogList() -> FWidgetList*&
{ return dialog_list; }

//----------------------------------------------------------------------
inline auto FWidget::getAlwaysOnTopList() -> FWidgetList*&
{ return always_on_top_list; }

//----------------------------------------------------------------------
inline auto FWidget::getWidgetCloseList() -> FWidgetList*&
{ return close_widget_list; }

//----------------------------------------------------------------------
inline auto FWidget::setModalDialogCounter() -> uInt&
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
