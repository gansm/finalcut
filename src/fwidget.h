// File: fwidget.h
// Provides: class FWidget
//
//  Inheritance diagram
//  ═══════════════════
//
// ▕▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▏
// ▕ FObject ▏ ▕  FTerm  ▏
// ▕▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▏
//      ▲           ▲
//      │           │
//      └─────┬─────┘
//            │
//       ▕▔▔▔▔▔▔▔▔▔▏ 1      1▕▔▔▔▔▔▔▔▔▔▔▔▔▏
//       ▕ FWidget ▏-┬- - - -▕ FStatusBar ▏
//       ▕▁▁▁▁▁▁▁▁▁▏ :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▏
//                   :
//                   :      1▕▔▔▔▔▔▔▔▔▔▔▏
//                   :- - - -▕ FMenuBar ▏
//                   :       ▕▁▁▁▁▁▁▁▁▁▁▏
//                   :
//                   :      *▕▔▔▔▔▔▔▔▔▔▏
//                   :- - - -▕ FString ▏
//                   :       ▕▁▁▁▁▁▁▁▁▁▏
//                   :
//                   :      *▕▔▔▔▔▔▔▔▔▏
//                   :- - - -▕ FEvent ▏
//                   :       ▕▁▁▁▁▁▁▁▁▏
//                   :
//                   :      *▕▔▔▔▔▔▔▔▔▔▔▔▏
//                   :- - - -▕ FKeyEvent ▏
//                   :       ▕▁▁▁▁▁▁▁▁▁▁▁▏
//                   :
//                   :      *▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
//                   :- - - -▕ FMouseEvent ▏
//                   :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
//                   :
//                   :      *▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
//                   :- - - -▕ FWheelEvent ▏
//                   :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
//                   :
//                   :      *▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
//                   :- - - -▕ FFocusEvent ▏
//                   :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
//                   :
//                   :      *▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
//                   :- - - -▕ FAccelEvent ▏
//                   :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
//                   :
//                   :      *▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
//                   :- - - -▕ FResizeEvent ▏
//                   :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
//                   :
//                   :      *▕▔▔▔▔▔▔▔▔▔▔▔▔▏
//                   :- - - -▕ FShowEvent ▏
//                   :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▏
//                   :
//                   :      *▕▔▔▔▔▔▔▔▔▔▔▔▔▏
//                   :- - - -▕ FHideEvent ▏
//                   :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▏
//                   :
//                   :      *▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
//                   └- - - -▕ FCloseEvent ▏
//                           ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏

#ifndef _FWIDGET_H
#define _FWIDGET_H

#include "fterm.h"


// widget flags
#define SHADOW        0x00000001
#define TRANS_SHADOW  0x00000002
#define ACTIVE        0x00000004
#define FOCUS         0x00000008
#define SCROLLABLE    0x00000010
#define RESIZEABLE    0x00000020
#define MODAL         0x00000040
#define FLAT          0x00000080
#define NO_UNDERLINE  0x00000100

// Callback macros
#define _FUNCTION_CALLBACK(h) \
           reinterpret_cast<FWidget::FCallback>((h))

#define _METHOD_CALLBACK(i,h) \
           reinterpret_cast<FWidget*>((i)) \
         , reinterpret_cast<FWidget::FMemberCallback>((h))

class FStatusBar;
class FMenuBar;

//----------------------------------------------------------------------
// class FWidget
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FWidget : public FObject, public FTerm
{
 public:
   typedef std::vector<FWidget*> widgetList;
   static widgetList* window_list;
   static widgetList* close_widget;

   typedef void (*FCallback)(FWidget*, void*);
   typedef void (FWidget::*FMemberCallback)(FWidget*, void*);
   typedef void* data_ptr;

   struct callback_data
   {
     FString   cb_signal;
     FCallback cb_handler;
     data_ptr  data;
   };

   typedef std::vector<callback_data> CallbackObjects;
   CallbackObjects callbackObjects;

   struct member_callback_data
   {
     FString         cb_signal;
     FWidget*        cb_instance;
     FMemberCallback cb_handler;
     data_ptr        data;
   };

   typedef std::vector<member_callback_data> MemberCallbackObjects;
   MemberCallbackObjects memberCallbackObjects;

   struct accelerator
   {
     int key;
     FWidget* object;
   };

   typedef std::vector<accelerator> Accelerators;
   Accelerators* accelerator_list;

   static struct widget_colors
   {
     uChar term_fg;
     uChar term_bg;
     uChar list_fg;
     uChar list_bg;
     uChar selected_list_fg;
     uChar selected_list_bg;
     uChar current_element_focus_fg;
     uChar current_element_focus_bg;
     uChar current_element_fg;
     uChar current_element_bg;
     uChar current_inc_search_element_fg;
     uChar selected_current_element_fg;
     uChar selected_current_element_bg;
     uChar label_fg;
     uChar label_bg;
     uChar label_inactive_fg;
     uChar label_inactive_bg;
     uChar label_hotkey_fg;
     uChar label_hotkey_bg;
     uChar label_emphasis_fg;
     uChar label_ellipsis_fg;
     uChar inputfield_active_focus_fg;
     uChar inputfield_active_focus_bg;
     uChar inputfield_active_fg;
     uChar inputfield_active_bg;
     uChar inputfield_inactive_fg;
     uChar inputfield_inactive_bg;
     uChar dialog_fg;
     uChar dialog_emphasis_fg;
     uChar dialog_bg;
     uChar error_box_fg;
     uChar error_box_emphasis_fg;
     uChar error_box_bg;
     uChar shadow_fg;
     uChar shadow_bg;
     uChar toggle_button_active_focus_fg;
     uChar toggle_button_active_focus_bg;
     uChar toggle_button_active_fg;
     uChar toggle_button_active_bg;
     uChar toggle_button_inactive_fg;
     uChar toggle_button_inactive_bg;
     uChar button_active_focus_fg;
     uChar button_active_focus_bg;
     uChar button_active_fg;
     uChar button_active_bg;
     uChar button_inactive_fg;
     uChar button_inactive_bg;
     uChar button_hotkey_fg;
     uChar titlebar_active_fg;
     uChar titlebar_active_bg;
     uChar titlebar_inactive_fg;
     uChar titlebar_inactive_bg;
     uChar titlebar_button_fg;
     uChar titlebar_button_bg;
     uChar menu_active_focus_fg;
     uChar menu_active_focus_bg;
     uChar menu_active_fg;
     uChar menu_active_bg;
     uChar menu_inactive_fg;
     uChar menu_inactive_bg;
     uChar menu_hotkey_fg;
     uChar menu_hotkey_bg;
     uChar statusbar_fg;
     uChar statusbar_bg;
     uChar statusbar_hotkey_fg;
     uChar statusbar_hotkey_bg;
     uChar statusbar_separator_fg;
     uChar statusbar_active_fg;
     uChar statusbar_active_bg;
     uChar statusbar_active_hotkey_fg;
     uChar statusbar_active_hotkey_bg;
     uChar scrollbar_fg;
     uChar scrollbar_bg;
     uChar scrollbar_button_fg;
     uChar scrollbar_button_bg;
     uChar progressbar_fg;
     uChar progressbar_bg;
   } wc;
   // widget_colors wc;

  struct dbl_line_mask
  {
    dbl_line_mask() : top(), right(), bottom(), left()
    { }
   ~dbl_line_mask()
    { }
    std::vector<bool> top;
    std::vector<bool> right;
    std::vector<bool> bottom;
    std::vector<bool> left;
  } double_flatline_mask;

 protected:
   int    xpos;
   int    ypos;
   int    width;
   int    height;
   int    xmin;
   int    ymin;
   int    xmax;
   int    ymax;
   int    top_padding;
   int    left_padding;
   int    bottom_padding;
   int    right_padding;
   int    client_xmin;
   int    client_ymin;
   int    client_xmax;
   int    client_ymax;
   FPoint shadow;
   FRect  adjustWidgetSizeShadow;
   FRect  adjustWidgetSizeGlobalShadow;
   bool   ignore_padding;
   bool   window_object;
   int    flags;
   int    foregroundColor;
   int    backgroundColor;
   static uInt modal_dialogs;

 private:
   bool    enable;
   bool    visible;
   bool    shown;
   bool    focus;
   bool    focusable;
   bool    visibleCursor;
   FPoint  widgetCursorPosition;
   FRect   widgetSize;
   FRect   adjustWidgetSize;
   FRect   adjustWidgetSizeGlobal;
   FString statusbar_message;

   static FStatusBar* statusbar;
   static FMenuBar* menubar;
   static FWidget* show_root_widget;
   static FWidget* redraw_root_widget;
   friend class FApplication;
   friend class FToggleButton;

 private:
   FWidget (const FWidget&);
   FWidget& operator = (const FWidget&);
   void         init();
   void         finish();
   void         processDestroy();
   virtual void draw();
   static void  setColorTheme();

 protected:
   virtual void adjustSize();
   virtual void setStatusBar (FStatusBar*);
   virtual void setMenuBar (FMenuBar*);
 // Event handlers
   bool         event (FEvent*);
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
   virtual void onAccel (FAccelEvent*);
   virtual void onResize (FResizeEvent*);
   virtual void onShow (FShowEvent*);
   virtual void onHide (FHideEvent*);
   virtual void onClose (FCloseEvent*);
   virtual bool focusNextChild (void);
   virtual bool focusPrevChild (void);

 public:
   explicit FWidget (FWidget* = 0);  // constructor
  ~FWidget();  // destructor

   const char*      getClassName() const;
   FWidget*         getRootWidget() const;
   static FWidget*  getMainWidget();
   static void      setMainWidget (FWidget*);
   static FWidget*  childWidgetAt (FWidget*, const FPoint&);
   static FWidget*  childWidgetAt (FWidget*, int, int);
   virtual FWidget* getFocusWidget() const;
   virtual void     setFocusWidget(FWidget*);
   static FWidget*  getClickedWidget();
   static void      setClickedWidget(FWidget*);
   int              numOfFocusableChildren();
   FWidget*         parentWidget() const;
   bool             isRootWidget() const;
   bool             isWindow() const;
   virtual bool     close();

   static FStatusBar* statusBar();
   static FMenuBar* menuBar();
   void           setStatusbarMessage (FString);
   void           clearStatusbarMessage();
   FString        getStatusbarMessage();

   void           addCallback ( FString
                              , FCallback
                              , void* = null );
   void           addCallback ( FString
                              , FWidget*
                              , FMemberCallback
                              , void* = null );
   void           delCallback (FCallback);
   void           delCallback (FWidget*);
   void           emitCallback (FString);

   void           addAccelerator (int key);
   void           addAccelerator (int, FWidget*);
   void           delAccelerator (FWidget*);

   virtual void   redraw();
   virtual void   resize();
   virtual void   show();
   virtual void   hide();
   bool           setVisible();
   bool           isVisible() const;
   bool           isShown() const;

   virtual bool   setEnable(bool);
   virtual bool   setEnable();
   virtual bool   unsetEnable();
   virtual bool   setDisable();
   bool           isEnabled() const;

   virtual bool   setVisibleCursor(bool);
   virtual bool   setVisibleCursor();
   virtual bool   unsetVisibleCursor();
   bool           hasVisibleCursor() const;

   virtual bool   focusFirstChild (void);
   virtual bool   focusLastChild (void);
   virtual bool   setFocus(bool);
   virtual bool   setFocus();
   virtual bool   unsetFocus();
   bool           hasFocus() const;
   bool           acceptFocus() const;
   void           setFocusable();
   void           unsetFocusable();

   bool           ignorePadding(bool);
   bool           ignorePadding();
   bool           acceptPadding();

   int            getForegroundColor() const;
   int            getBackgroundColor() const;
   int            getX() const;
   int            getY() const;
   const FPoint   getPos() const;
   int            getGlobalX() const;
   int            getGlobalY() const;
   const FPoint   getGlobalPos() const;
   int            getWidth() const;
   int            getHeight() const;
   int            getTopPadding() const;
   int            getLeftPadding() const;
   int            getBottomPadding() const;
   int            getRightPadding() const;
   int            getClientWidth() const;
   int            getClientHeight() const;
   const FPoint&  getShadow() const;
   const FRect&   getGeometry() const;
   const FRect&   getGeometryShadow() const;
   const FRect&   getGeometryGlobal() const;
   const FRect&   getGeometryGlobalShadow() const;
   FPoint         globalToLocalPos(const FPoint&);
   void           setForegroundColor (int);
   void           setBackgroundColor (int);
   void           setX (int, bool = true);
   void           setY (int, bool = true);
   void           setPos (const FPoint&, bool = true);
   void           setPos (int, int, bool = true);
   void           setWidth (int, bool = true);
   void           setHeight (int, bool = true);
   void           setTopPadding (int, bool = true);
   void           setLeftPadding (int, bool = true);
   void           setBottomPadding (int, bool = true);
   void           setRightPadding (int, bool = true);
   void           getTermGeometry();
   void           setTermGeometry (int, int);
   virtual void   setGeometry (const FRect&, bool = true);
   virtual void   setGeometry (int, int, int, int, bool = true);
   virtual void   move (const FPoint&);
   virtual void   move (int x, int y);
   int            getFlags() const;

   bool           setCursor();
   FPoint         getCursorPos();
   bool           setCursorPos (const FPoint&);
   bool           setCursorPos (register int, register int);
   void           unsetCursorPos();

   static void    gotoxy (const FPoint&);
   static void    gotoxy (register int, register int);
   void           clrscr();

   static bool    setBold(register bool);
   static bool    setBold();
   static bool    unsetBold();
   static bool    isBold();

   static bool    setReverse(register bool);
   static bool    setReverse();
   static bool    unsetReverse();
   static bool    isReverse();

   static bool    setUnderline(register bool);
   static bool    setUnderline();
   static bool    unsetUnderline();
   static bool    isUnderline();

   void           drawShadow();
   void           clearShadow();
   void           drawFlatBorder();
   void           clearFlatBorder();
   void           setDoubleFlatLine(int, bool = true);
   void           unsetDoubleFlatLine(int);
   std::vector<bool>& doubleFlatLine_ref(int);
   virtual void   drawBorder();

   static void    quit();
};

#pragma pack(pop)

//----------------------------------------------------------------------
inline const char* FWidget::getClassName() const
{ return "FWidget"; }

//----------------------------------------------------------------------
inline FWidget* FWidget::parentWidget() const
{ return static_cast<FWidget*>(getParent()); }

//----------------------------------------------------------------------
inline bool FWidget::isRootWidget() const
{ return (! hasParent()); }

//----------------------------------------------------------------------
inline void FWidget::setStatusbarMessage(FString msg)
{ statusbar_message = msg; }

//----------------------------------------------------------------------
inline void FWidget::clearStatusbarMessage()
{ statusbar_message.clear(); }

//----------------------------------------------------------------------
inline FString FWidget::getStatusbarMessage()
{ return statusbar_message; }

//----------------------------------------------------------------------
inline bool FWidget::setVisible()
{ return visible = true; }

//----------------------------------------------------------------------
inline bool FWidget::isVisible() const
{ return visible; }

//----------------------------------------------------------------------
inline bool FWidget::isShown() const
{ return shown; }

//----------------------------------------------------------------------
inline bool FWidget::isWindow() const
{ return window_object; }

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
inline bool FWidget::isEnabled() const
{ return enable; }

//----------------------------------------------------------------------
inline bool FWidget::setVisibleCursor()
{ return setVisibleCursor(true); }

//----------------------------------------------------------------------
inline bool FWidget::unsetVisibleCursor()
{ return setVisibleCursor(false); }

//----------------------------------------------------------------------
inline bool FWidget::hasVisibleCursor() const
{ return visibleCursor; }

//----------------------------------------------------------------------
inline bool FWidget::setFocus()
{ return setFocus(true); }

//----------------------------------------------------------------------
inline bool FWidget::unsetFocus()
{ return setFocus(false); }

//----------------------------------------------------------------------
inline bool FWidget::hasFocus() const
{ return focus; }

//----------------------------------------------------------------------
inline bool FWidget::acceptFocus() const
{ return focusable; }

//----------------------------------------------------------------------
inline bool FWidget::ignorePadding (bool on)
{ return ignore_padding = on; }

//----------------------------------------------------------------------
inline bool FWidget::ignorePadding()
{ return ignore_padding = true; }

//----------------------------------------------------------------------
inline bool FWidget::acceptPadding()
{ return ignore_padding = false; }

//----------------------------------------------------------------------
inline void FWidget::setFocusable()
{ focusable = true; }

//----------------------------------------------------------------------
inline void FWidget::unsetFocusable()
{ focusable = false; }

//----------------------------------------------------------------------
inline int FWidget::getForegroundColor() const
{ return foregroundColor; }

//----------------------------------------------------------------------
inline int FWidget::getBackgroundColor() const
{ return backgroundColor; }

//----------------------------------------------------------------------
inline int FWidget::getX() const
{ return xpos; }

//----------------------------------------------------------------------
inline int FWidget::getY() const
{ return ypos; }

//----------------------------------------------------------------------
inline const FPoint FWidget::getPos() const
{ return adjustWidgetSize.getPos(); }

//----------------------------------------------------------------------
inline int FWidget::getGlobalX() const
{ return xpos+xmin-1; }

//----------------------------------------------------------------------
inline int FWidget::getGlobalY() const
{ return ypos+ymin-1; }

//----------------------------------------------------------------------
inline const FPoint FWidget::getGlobalPos() const
{ return FPoint(xpos+xmin-1, ypos+ymin-1); }

//----------------------------------------------------------------------
inline int FWidget::getWidth() const
{ return width; }

//----------------------------------------------------------------------
inline int FWidget::getHeight() const
{ return height; }

//----------------------------------------------------------------------
inline int FWidget::getTopPadding() const
{ return top_padding; }

//----------------------------------------------------------------------
inline int FWidget::getLeftPadding() const
{ return left_padding; }

//----------------------------------------------------------------------
inline int FWidget::getBottomPadding() const
{ return bottom_padding; }

//----------------------------------------------------------------------
inline int FWidget::getRightPadding() const
{ return right_padding; }

//----------------------------------------------------------------------
inline int FWidget::getClientWidth() const
{ return client_xmax-client_xmin+1; }

//----------------------------------------------------------------------
inline int FWidget::getClientHeight() const
{ return client_ymax-client_ymin+1; }

//----------------------------------------------------------------------
inline const FPoint& FWidget::getShadow() const
{ return shadow; }

//----------------------------------------------------------------------
inline const FRect& FWidget::getGeometry() const
{ return adjustWidgetSize; }

//----------------------------------------------------------------------
inline const FRect& FWidget::getGeometryShadow() const
{ return adjustWidgetSizeShadow; }

//----------------------------------------------------------------------
inline const FRect& FWidget::getGeometryGlobal() const
{ return adjustWidgetSizeGlobal; }

//----------------------------------------------------------------------
inline const FRect& FWidget::getGeometryGlobalShadow() const
{ return adjustWidgetSizeGlobalShadow; }

//----------------------------------------------------------------------
inline int FWidget::getFlags() const
{ return flags; }

//----------------------------------------------------------------------
inline FPoint FWidget::getCursorPos()
{ return widgetCursorPosition; }

//----------------------------------------------------------------------
inline bool FWidget::setBold()
{ return setBold(true); }

//----------------------------------------------------------------------
inline bool FWidget::unsetBold()
{ return setBold(false); }

//----------------------------------------------------------------------
inline bool FWidget::isBold()
{ return bold; }

//----------------------------------------------------------------------
inline bool FWidget::setReverse()
{ return setReverse(true); }

//----------------------------------------------------------------------
inline bool FWidget::unsetReverse()
{ return setReverse(false); }

//----------------------------------------------------------------------
inline bool FWidget::isReverse()
{ return reverse; }

//----------------------------------------------------------------------
inline bool FWidget::setUnderline()
{ return setUnderline(true); }

//----------------------------------------------------------------------
inline bool FWidget::unsetUnderline()
{ return setUnderline(false); }

//----------------------------------------------------------------------
inline bool FWidget::isUnderline()
{ return underline; }

//----------------------------------------------------------------------
inline void FWidget::unsetDoubleFlatLine(int side)
{ setDoubleFlatLine(side, false); }


// NewFont elements
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

#endif  // _FWIDGET_H
