/***********************************************************************
* fwidget.h - Intermediate base class for all widget objects           *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2015-2019 Markus Gans                                      *
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
 *       ▕▔▔▔▔▔▔▔▔▔▏1       1▕▔▔▔▔▔▔▔▔▔▔▔▔▏
 *       ▕ FWidget ▏-┬- - - -▕ FStatusBar ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏ :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▏
 *                   :
 *                   :      1▕▔▔▔▔▔▔▔▔▔▔▏
 *                   :- - - -▕ FMenuBar ▏
 *                   :       ▕▁▁▁▁▁▁▁▁▁▁▏
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

#include <utility>
#include <vector>

#include "final/fvterm.h"
#include "final/fobject.h"
#include "final/fpoint.h"
#include "final/frect.h"
#include "final/fsize.h"
#include "final/ftypes.h"

// Callback macros
#define F_FUNCTION_CALLBACK(h) \
           reinterpret_cast<finalcut::FWidget::FCallback>((h))

#define F_METHOD_CALLBACK(i,h) \
           reinterpret_cast<finalcut::FWidget*>((i)) \
         , reinterpret_cast<finalcut::FWidget::FMemberCallback>((h))

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

#pragma pack(push)
#pragma pack(1)

class FWidget : public FVTerm, public FObject
{
  public:
     // Using-declaration
    using FVTerm::setColor;
    using FVTerm::print;

    struct accelerator
    {
      FKey key;
      FWidget* object;
    };

    // Typedefs
    typedef std::vector<FWidget*> widgetList;
    typedef std::vector<accelerator> Accelerators;
    typedef void (*FCallback)(FWidget*, FDataPtr);
    typedef void (FWidget::*FMemberCallback)(FWidget*, FDataPtr);

    struct widget_flags  // Properties of a widget ⚑
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
      uInt32 modal          : 1;
      uInt32 visible_cursor : 1;
      uInt32 window_widget  : 1;
      uInt32 dialog_widget  : 1;
      uInt32 menu_widget    : 1;
      uInt32 always_on_top  : 1;
      uInt32 flat           : 1;
      uInt32 no_underline   : 1;
      uInt32                : 14;  // padding bits
    };

    // Constructor
    explicit FWidget (FWidget* = nullptr, bool = false);

    // Disable copy constructor
    FWidget (const FWidget&) = delete;

    // Destructor
    virtual ~FWidget();

    // Disable assignment operator (=)
    FWidget& operator = (const FWidget&) = delete;

    // Accessors
    const char*           getClassName() const override;
    FWidget*              getRootWidget() const;
    FWidget*              getParentWidget() const;
    static FWidget*&      getMainWidget();
    static FWidget*&      getActiveWindow();
    static FWidget*&      getFocusWidget();
    static FWidget*&      getClickedWidget();
    static FWidget*&      getOpenMenu();
    static FWidget*&      getMoveSizeWidget();
    virtual FWidget*      getFirstFocusableWidget (FObjectList);
    virtual FWidget*      getLastFocusableWidget (FObjectList);
    Accelerators*         getAcceleratorList() const;
    static widgetList*&   getWindowList();
    static FMenuBar*      getMenuBar();
    static FStatusBar*    getStatusBar();
    FString               getStatusbarMessage() const;
    FColor                getForegroundColor() const;  // get the primary
    FColor                getBackgroundColor() const;  // widget colors
    int                   getX() const;  // positioning
    int                   getY() const;
    const FPoint          getPos() const;
    int                   getTermX() const;
    int                   getTermY() const;
    const FPoint          getTermPos() const;
    std::size_t           getWidth() const;
    std::size_t           getHeight() const;
    const FSize           getSize() const;
    int                   getTopPadding() const;
    int                   getLeftPadding() const;
    int                   getBottomPadding() const;
    int                   getRightPadding() const;
    std::size_t           getClientWidth() const;
    std::size_t           getClientHeight() const;
    std::size_t           getMaxWidth() const;
    std::size_t           getMaxHeight() const;
    const FSize&          getShadow() const;
    const FRect&          getGeometry() const;
    const FRect&          getGeometryWithShadow();
    const FRect&          getTermGeometry();
    const FRect&          getTermGeometryWithShadow();
    std::size_t           getDesktopWidth();
    std::size_t           getDesktopHeight();
    const widget_flags&   getFlags() const;
    FPoint                getCursorPos();
    FPoint                getPrintPos();
    std::vector<bool>&    doubleFlatLine_ref (fc::sides);

    // Mutators
    static void           setMainWidget (FWidget*);
    static void           setFocusWidget (FWidget*);
    static void           setClickedWidget (FWidget*);
    static void           setMoveSizeWidget (FWidget*);
    static void           setActiveWindow (FWidget*);
    static void           setOpenMenu (FWidget*);
    virtual void          setStatusbarMessage (const FString&);
    bool                  setVisible (bool);
    bool                  setVisible();
    bool                  unsetVisible();
    virtual bool          setEnable (bool);
    virtual bool          setEnable();
    virtual bool          unsetEnable();
    virtual bool          setDisable();
    virtual bool          setVisibleCursor (bool);  // input cursor visibility
    virtual bool          setVisibleCursor();       // for the widget
    virtual bool          unsetVisibleCursor();
    virtual bool          setFocus (bool);
    virtual bool          setFocus();
    virtual bool          unsetFocus();
    void                  setFocusable();
    void                  unsetFocusable();
    bool                  ignorePadding (bool);    // ignore padding from
    bool                  ignorePadding();         // the parent widget
    bool                  acceptPadding();
    void                  setForegroundColor (FColor);
    void                  setBackgroundColor (FColor);
    void                  setColor();
    virtual void          setX (int, bool = true);  // positioning
    virtual void          setY (int, bool = true);
    virtual void          setPos (const FPoint&, bool = true);
    virtual void          setWidth (std::size_t, bool = true);
    virtual void          setHeight (std::size_t, bool = true);
    virtual void          setSize (const FSize&, bool = true);
    void                  setTopPadding (int, bool = true);
    void                  setLeftPadding (int, bool = true);
    void                  setBottomPadding (int, bool = true);
    void                  setRightPadding (int, bool = true);
    void                  setParentOffset();
    void                  setTermOffset();
    void                  setTermOffsetWithPadding();
    void                  setTermSize (const FSize&);
    virtual void          setGeometry (const FRect&, bool = true);
    virtual void          setGeometry (const FPoint&, const FSize&, bool = true);
    virtual void          setShadowSize (const FSize&);
    void                  setMinimumWidth (std::size_t);
    void                  setMinimumHeight (std::size_t);
    void                  setMinimumSize (const FSize&);
    void                  setMaximumWidth (std::size_t);
    void                  setMaximumHeight (const FSize&);
    void                  setMaximumSize (std::size_t, std::size_t);
    void                  setFixedSize (const FSize&);
    bool                  setCursorPos (const FPoint&);
    void                  unsetCursorPos();
    void                  setPrintPos (const FPoint&);
    void                  setDoubleFlatLine (fc::sides, bool = true);
    void                  unsetDoubleFlatLine (fc::sides);
    void                  setDoubleFlatLine (fc::sides, int, bool = true);
    void                  unsetDoubleFlatLine (fc::sides, int);
    widget_flags&         setFlags();

    // Inquiries
    bool                  isRootWidget() const;
    bool                  isWindowWidget() const;
    bool                  isDialogWidget() const;
    bool                  isMenuWidget() const;
    bool                  isVisible() const;
    bool                  isShown() const;
    bool                  isEnabled() const;
    bool                  hasVisibleCursor() const;
    bool                  hasFocus() const;
    bool                  acceptFocus() const;  // is focusable
    bool                  isPaddingIgnored();

    // Methods
    static FWidget*       childWidgetAt (FWidget*, const FPoint&);
    int                   numOfFocusableChildren();
    virtual bool          close();
    void                  clearStatusbarMessage();
    void                  addCallback ( const FString&
                                      , FCallback
                                      , FDataPtr = nullptr );
    void                  addCallback ( const FString&
                                      , FWidget*
                                      , FMemberCallback
                                      , FDataPtr = nullptr );
    void                  delCallback (FCallback);
    void                  delCallback (FWidget*);
    void                  delCallbacks();
    void                  emitCallback (const FString&);
    void                  addAccelerator (FKey);
    virtual void          addAccelerator (FKey, FWidget*);
    void                  delAccelerator ();
    virtual void          delAccelerator (FWidget*);
    virtual void          redraw();
    virtual void          resize();
    virtual void          show();
    virtual void          hide();
    virtual bool          focusFirstChild();  // widget focusing
    virtual bool          focusLastChild();
    FPoint                termToWidgetPos (const FPoint&);
    void                  detectTermSize();
    void                  print (const FPoint&) override;
    virtual void          move (const FPoint&);
    void                  drawShadow();
    void                  clearShadow();
    void                  drawFlatBorder();
    void                  clearFlatBorder();
    virtual void          drawBorder();
    static void           quit();

  protected:
    struct callback_data
    {
      FString   cb_signal;
      FCallback cb_handler;
      FDataPtr  data;
    };

    struct member_callback_data
    {
      FString         cb_signal;
      FWidget*        cb_instance;
      FMemberCallback cb_handler;
      FDataPtr        data;
    };

    // Typedefs
    typedef std::vector<callback_data> CallbackObjects;
    typedef std::vector<member_callback_data> MemberCallbackObjects;

    // Accessor
    term_area*            getPrintArea() override;
    const FWidgetColors&  getFWidgetColors() const;
    static uInt           getModalDialogCounter();
    static widgetList*&   getDialogList();
    static widgetList*&   getAlwaysOnTopList();
    static widgetList*&   getWidgetCloseList();
    void                  addPreprocessingHandler ( FVTerm*
                                                  , FPreprocessingHandler ) override;
    void                  delPreprocessingHandler (FVTerm*) override;

    // Inquiry
    bool                  isChildPrintArea() const;

    // Mutators
    virtual void          setStatusBar (FStatusBar*);
    virtual void          setMenuBar (FMenuBar*);
    FWidgetColors&        setFWidgetColors();
    static uInt&          setModalDialogCounter();

    // Methods
    virtual void          adjustSize();
    void                  adjustSizeGlobal();
    void                  hideArea (const FSize&);
    void                  createWidgetAcceleratorList();
    virtual bool          focusNextChild();  // Change child...
    virtual bool          focusPrevChild();  // ...focus

    // Event handlers
    bool                  event (FEvent*) override;
    virtual void          onKeyPress (FKeyEvent*);
    virtual void          onKeyUp (FKeyEvent*);
    virtual void          onKeyDown (FKeyEvent*);
    virtual void          onMouseDown (FMouseEvent*);
    virtual void          onMouseUp (FMouseEvent*);
    virtual void          onMouseDoubleClick (FMouseEvent*);
    virtual void          onWheel (FWheelEvent*);
    virtual void          onMouseMove (FMouseEvent*);
    virtual void          onFocusIn (FFocusEvent*);
    virtual void          onFocusOut (FFocusEvent*);
    virtual void          onChildFocusIn (FFocusEvent*);
    virtual void          onChildFocusOut (FFocusEvent*);
    virtual void          onAccel (FAccelEvent*);
    virtual void          onResize (FResizeEvent*);
    virtual void          onShow (FShowEvent*);
    virtual void          onHide (FHideEvent*);
    virtual void          onClose (FCloseEvent*);

  private:
    // Methods
    void                  init();
    void                  finish();
    void                  insufficientSpaceAdjust();
    void                  KeyPressEvent (FKeyEvent*);
    void                  KeyDownEvent (FKeyEvent*);
    bool                  changeFocus (FWidget*, FWidget*, fc::FocusTypes);
    void                  processDestroy();
    virtual void          draw();
    void                  drawWindows();
    void                  drawChildren();
    void                  drawTransparentShadow (int, int, int, int);
    void                  drawBlockShadow (int, int, int, int);
    static void           setColorTheme();

    // Data members
    FPoint                widget_cursor_position{-1, -1};

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
    } size_hints{};

    struct dbl_line_mask
    {
      dbl_line_mask() = default;
      ~dbl_line_mask() = default;

      std::vector<bool> top{};
      std::vector<bool> right{};
      std::vector<bool> bottom{};
      std::vector<bool> left{};
    } double_flatline_mask{};

    struct widget_padding
    {
      widget_padding() = default;
      ~widget_padding() = default;

      int top{0};
      int left{0};
      int bottom{0};
      int right{0};
    } padding{};

    struct widget_flags   flags{};
    bool                  ignore_padding{false};

    // widget size
    FRect                 wsize{1, 1, 1, 1};
    FRect                 adjust_wsize{1, 1, 1, 1};
    FRect                 adjust_wsize_term{};
    FRect                 adjust_wsize_shadow{};
    FRect                 adjust_wsize_term_shadow{};
    // widget offset
    FRect                 offset{};
    // offset of the widget client area
    FRect                 client_offset{};
    // widget shadow size (on the right and bottom side)
    FSize                 wshadow{0, 0};

    // default widget foreground and background color
    FColor                foreground_color{fc::Default};
    FColor                background_color{fc::Default};
    FString               statusbar_message{};
    Accelerators*         accelerator_list{nullptr};
    MemberCallbackObjects member_callback_objects{};
    CallbackObjects       callback_objects{};

    static FStatusBar*    statusbar;
    static FMenuBar*      menubar;
    static FWidget*       main_widget;
    static FWidget*       active_window;
    static FWidget*       focus_widget;
    static FWidget*       clicked_widget;
    static FWidget*       open_menu;
    static FWidget*       move_size_widget;
    static FWidget*       show_root_widget;
    static FWidget*       redraw_root_widget;
    static widgetList*    window_list;
    static widgetList*    dialog_list;
    static widgetList*    always_on_top_list;
    static widgetList*    close_widget;
    static FWidgetColors  wc;
    static uInt           modal_dialog_counter;
    static bool           init_desktop;
    static bool           hideable;

    // Friend classes
    friend class FToggleButton;
    friend class FScrollView;
};

#pragma pack(pop)

// non-member function forward declarations
//----------------------------------------------------------------------
char*       createBlankArray (std::size_t);
void        destroyBlankArray (char[]);
FKey        getHotkey (const FString&);
std::size_t getHotkeyPos (wchar_t[], wchar_t[], std::size_t);
void        drawBorder (FWidget*, FRect);

// FWidget inline functions
//----------------------------------------------------------------------
inline const char* FWidget::getClassName() const
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
inline FWidget::Accelerators* FWidget::getAcceleratorList() const
{ return accelerator_list; }

//----------------------------------------------------------------------
inline FWidget::widgetList*& FWidget::getWindowList()
{ return window_list; }

//----------------------------------------------------------------------
inline FMenuBar* FWidget::getMenuBar()
{ return menubar; }

//----------------------------------------------------------------------
inline FStatusBar* FWidget::getStatusBar()
{ return statusbar; }

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
inline const FPoint FWidget::getPos() const  // position relative to the widget
{ return adjust_wsize.getPos(); }

//----------------------------------------------------------------------
inline int FWidget::getTermX() const  // x-position on terminal
{ return offset.getX1() + adjust_wsize.getX(); }

//----------------------------------------------------------------------
inline int FWidget::getTermY() const  // y-position on terminal
{ return offset.getY1() + adjust_wsize.getY(); }

//----------------------------------------------------------------------
inline const FPoint FWidget::getTermPos() const  // position on terminal
{ return FPoint(getTermX(), getTermY()); }

//----------------------------------------------------------------------
inline std::size_t FWidget::getWidth() const
{ return adjust_wsize.getWidth(); }

//----------------------------------------------------------------------
inline std::size_t FWidget::getHeight() const
{ return adjust_wsize.getHeight(); }

//----------------------------------------------------------------------
inline const FSize FWidget::getSize() const
{ return adjust_wsize.getSize(); }

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
{ return client_offset.getWidth(); }

//----------------------------------------------------------------------
inline std::size_t FWidget::getClientHeight() const
{ return client_offset.getHeight(); }

//----------------------------------------------------------------------
inline std::size_t FWidget::getMaxWidth() const
{ return offset.getWidth(); }

//----------------------------------------------------------------------
inline std::size_t FWidget::getMaxHeight() const
{ return offset.getHeight(); }

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
    adjust_wsize.x1_ref() + offset.x1_ref(),
    adjust_wsize.y1_ref() + offset.y1_ref(),
    adjust_wsize.x2_ref() + offset.x1_ref(),
    adjust_wsize.y2_ref() + offset.y1_ref()
  );

  return adjust_wsize_term;
}

//----------------------------------------------------------------------
inline const FRect& FWidget::getTermGeometryWithShadow()
{
  adjust_wsize_term_shadow.setCoordinates
  (
    adjust_wsize.x1_ref() + offset.x1_ref(),
    adjust_wsize.y1_ref() + offset.y1_ref(),
    adjust_wsize.x2_ref() + offset.x1_ref() + int(wshadow.width_ref()),
    adjust_wsize.y2_ref() + offset.y1_ref() + int(wshadow.height_ref())
  );

  return adjust_wsize_term_shadow;
}

//----------------------------------------------------------------------
inline std::size_t FWidget::getDesktopWidth()
{ return getColumnNumber(); }

//----------------------------------------------------------------------
inline std::size_t FWidget::getDesktopHeight()
{ return getLineNumber(); }

//----------------------------------------------------------------------
inline const FWidget::widget_flags& FWidget::getFlags() const
{ return flags; }

//----------------------------------------------------------------------
inline FPoint FWidget::getCursorPos()
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
inline void FWidget::setMoveSizeWidget (FWidget* obj)
{ move_size_widget = obj; }

//----------------------------------------------------------------------
inline void FWidget::setStatusbarMessage (const FString& msg)
{ statusbar_message = msg; }

//----------------------------------------------------------------------
inline bool FWidget::setVisible()
{ return setVisible(true); }

//----------------------------------------------------------------------
inline bool FWidget::unsetVisible()
{ return setVisible(false); }

//----------------------------------------------------------------------
inline bool FWidget::setEnable()
{ return setEnable(true); }

//----------------------------------------------------------------------
inline bool FWidget::unsetEnable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FWidget::setDisable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FWidget::setVisibleCursor (bool enable)
{
  flags.visible_cursor = ( enable ) ? true : (( hideable ) ? false : true);
  return flags.visible_cursor;
}

//----------------------------------------------------------------------
inline bool FWidget::setVisibleCursor()
{ return setVisibleCursor(true); }

//----------------------------------------------------------------------
inline bool FWidget::unsetVisibleCursor()
{ return setVisibleCursor(false); }

//----------------------------------------------------------------------
inline bool FWidget::setFocus()
{ return setFocus(true); }

//----------------------------------------------------------------------
inline bool FWidget::unsetFocus()
{ return setFocus(false); }

//----------------------------------------------------------------------
inline void FWidget::setFocusable()
{ flags.focusable = true; }

//----------------------------------------------------------------------
inline void FWidget::unsetFocusable()
{ flags.focusable = false; }

//----------------------------------------------------------------------
inline bool FWidget::ignorePadding (bool enable)
{ return (ignore_padding = enable); }

//----------------------------------------------------------------------
inline bool FWidget::ignorePadding()
{ return (ignore_padding = true); }

//----------------------------------------------------------------------
inline bool FWidget::acceptPadding()
{ return (ignore_padding = false); }

//----------------------------------------------------------------------
inline void FWidget::setForegroundColor (FColor color)
{
  // valid colors -1..254
  if ( color == fc::Default || color >> 8 == 0 )
    foreground_color = color;
}

//----------------------------------------------------------------------
inline void FWidget::setBackgroundColor (FColor color)
{
  // valid colors -1..254
  if ( color == fc::Default || color >> 8 == 0 )
    background_color = color;
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
{ size_hints.setMinimum (size); }

//----------------------------------------------------------------------
inline void FWidget::setMaximumWidth (std::size_t max_width)
{ size_hints.setMaximum (FSize(max_width, size_hints.max_height)); }

//----------------------------------------------------------------------
inline void FWidget::setMaximumHeight (const FSize& size)
{ size_hints.setMaximum (size); }

//----------------------------------------------------------------------
inline void FWidget::setMaximumSize ( std::size_t max_width
                                    , std::size_t max_height )
{ size_hints.setMaximum (FSize(max_width, max_height)); }

//----------------------------------------------------------------------
inline void FWidget::setFixedSize (const FSize& size)
{
  size_hints.setMinimum (size);
  size_hints.setMaximum (size);
}

//----------------------------------------------------------------------
inline void FWidget::unsetCursorPos()
{ setCursorPos (FPoint(-1, -1)); }

//----------------------------------------------------------------------
inline void FWidget::unsetDoubleFlatLine (fc::sides side)
{ setDoubleFlatLine(side, false); }

//----------------------------------------------------------------------
inline void FWidget::unsetDoubleFlatLine (fc::sides side, int pos)
{ setDoubleFlatLine(side, pos, false); }

//----------------------------------------------------------------------
inline FWidget::widget_flags& FWidget::setFlags()
{ return flags; }

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
inline bool FWidget::isPaddingIgnored()
{ return ignore_padding; }

//----------------------------------------------------------------------
inline void FWidget::clearStatusbarMessage()
{ statusbar_message.clear(); }

//----------------------------------------------------------------------
inline void FWidget::addAccelerator (FKey key)
{ addAccelerator (key, this); }

//----------------------------------------------------------------------
inline void FWidget::delAccelerator()
{ delAccelerator(this); }

//----------------------------------------------------------------------
inline FPoint FWidget::termToWidgetPos (const FPoint& tPos)
{
  return FPoint ( tPos.getX() + 1 - offset.getX1() - adjust_wsize.getX()
                , tPos.getY() + 1 - offset.getY1() - adjust_wsize.getY() );
}

//----------------------------------------------------------------------
inline void FWidget::print (const FPoint& pos)
{
  setPrintPos (pos);
}

//----------------------------------------------------------------------
inline void FWidget::drawBorder()
{
  finalcut::drawBorder (this, FRect(FPoint(1, 1), getSize()));
}

//----------------------------------------------------------------------
inline const FWidgetColors& FWidget::getFWidgetColors() const
{ return wc; }

//----------------------------------------------------------------------
inline uInt FWidget::getModalDialogCounter()
{ return modal_dialog_counter; }

//----------------------------------------------------------------------
inline FWidget::widgetList*& FWidget::getDialogList()
{ return dialog_list; }

//----------------------------------------------------------------------
inline FWidget::widgetList*& FWidget::getAlwaysOnTopList()
{ return always_on_top_list; }

//----------------------------------------------------------------------
inline FWidget::widgetList*& FWidget::getWidgetCloseList()
{ return close_widget; }

//----------------------------------------------------------------------
inline FWidgetColors& FWidget::setFWidgetColors()
{ return wc; }

//----------------------------------------------------------------------
inline uInt& FWidget::setModalDialogCounter()
{ return modal_dialog_counter; }

//----------------------------------------------------------------------
inline void FWidget::processDestroy()
{ emitCallback("destroy"); }


// Non-member elements for NewFont
//----------------------------------------------------------------------
const wchar_t NF_Drive[5] =
{
  fc::NF_shadow_box_left,
  fc::NF_shadow_box_middle,
  fc::NF_shadow_box_hdd,
  fc::NF_shadow_box_right,
  '\0'
};

const wchar_t NF_CD_ROM[5] =
{
  fc::NF_shadow_box_left,
  fc::NF_shadow_box_middle,
  fc::NF_shadow_box_cd,
  fc::NF_shadow_box_right,
  '\0'
};

const wchar_t NF_Net_Drive[5] =
{
  fc::NF_shadow_box_left,
  fc::NF_shadow_box_middle,
  fc::NF_shadow_box_net,
  fc::NF_shadow_box_right,
  '\0'
};

const wchar_t CHECKBOX[4] =
{
  fc::NF_shadow_box_left,
  fc::NF_shadow_box_middle,
  fc::NF_shadow_box_right,
  '\0'
};

const wchar_t CHECKBOX_ON[4] =
{
  fc::NF_shadow_box_left,
  fc::NF_shadow_box_checked,
  fc::NF_shadow_box_right,
  '\0'
};

const wchar_t RADIO_BUTTON[4] =
{
  fc::NF_radio_button1,
  fc::NF_radio_button2,
  fc::NF_radio_button3,
  '\0'
};

const wchar_t CHECKED_RADIO_BUTTON[4] =
{
  fc::NF_radio_button1,
  fc::NF_radio_button2_checked,
  fc::NF_radio_button3,
  '\0'
};

}  // namespace finalcut

#endif  // FWIDGET_H
