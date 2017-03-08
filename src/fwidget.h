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
//       ▕▔▔▔▔▔▔▔▔▏
//       ▕ FVTerm ▏
//       ▕▁▁▁▁▁▁▁▁▏
//            ▲
//            │
//       ▕▔▔▔▔▔▔▔▔▔▏1       1▕▔▔▔▔▔▔▔▔▔▔▔▔▏
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

#include "fvterm.h"


// Callback macros
#define _FUNCTION_CALLBACK(h) \
           reinterpret_cast<FWidget::FCallback>((h))

#define _METHOD_CALLBACK(i,h) \
           reinterpret_cast<FWidget*>((i)) \
         , reinterpret_cast<FWidget::FMemberCallback>((h))

// class forward declaration
class FStatusBar;
class FMenuBar;

//----------------------------------------------------------------------
// class FWidget
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FWidget : public FVTerm
{
 public:
    // Using-declaration
   using FVTerm::setColor;

   struct accelerator
   {
     int key;
     FWidget* object;
   };

   // Typedefs
   typedef std::vector<FWidget*> widgetList;
   typedef void* data_ptr;
   typedef void (*FCallback)(FWidget*, data_ptr);
   typedef void (FWidget::*FMemberCallback)(FWidget*, data_ptr);
   typedef std::vector<accelerator> Accelerators;

   // Constructor
   explicit FWidget (FWidget* = 0);

   // Destructor
  ~FWidget();

   // Accessors
   const char*        getClassName() const;
   FWidget*           getRootWidget() const;
   FWidget*           getParentWidget() const;
   static FWidget*    getMainWidget();
   virtual FWidget*   getFocusWidget() const;
   virtual FWidget*   getFirstFocusableWidget (FObjectList);
   virtual FWidget*   getLastFocusableWidget (FObjectList);
   static FWidget*    getClickedWidget();
   static FWidget*    getMoveSizeWidget();
   static FWidget*    getOpenMenu();
   static FMenuBar*   getMenuBar();
   static FStatusBar* getStatusBar();
   FString            getStatusbarMessage() const;
   short              getForegroundColor() const;  // get the primary
   short              getBackgroundColor() const;  // widget colors
   int                getX() const;  // positioning
   int                getY() const;
   const FPoint       getPos() const;
   int                getTermX() const;
   int                getTermY() const;
   const FPoint       getTermPos() const;
   int                getWidth() const;
   int                getHeight() const;
   int                getTopPadding() const;
   int                getLeftPadding() const;
   int                getBottomPadding() const;
   int                getRightPadding() const;
   int                getClientWidth() const;
   int                getClientHeight() const;
   int                getMaxWidth() const;
   int                getMaxHeight() const;
   const FPoint&      getShadow() const;
   const FRect&       getGeometry() const;
   const FRect&       getGeometryWithShadow();
   const FRect&       getTermGeometry();
   const FRect&       getTermGeometryWithShadow();
   int                getFlags() const;
   FPoint             getCursorPos();
   FPoint             getPrintPos();
   std::vector<bool>& doubleFlatLine_ref (fc::sides);

   // Mutators
   static void        setMainWidget (FWidget*);
   virtual void       setFocusWidget (FWidget*);
   static void        setClickedWidget (FWidget*);
   static void        setMoveSizeWidget (FWidget*);
   static void        setOpenMenu (FWidget*);
   virtual void       setStatusbarMessage (FString);
   bool               setVisible();
   virtual bool       setEnable (bool);
   virtual bool       setEnable();
   virtual bool       unsetEnable();
   virtual bool       setDisable();
   virtual bool       setVisibleCursor (bool);  // input cursor visibility
   virtual bool       setVisibleCursor();       // for the widget
   virtual bool       unsetVisibleCursor();
   virtual bool       setFocus (bool);
   virtual bool       setFocus();
   virtual bool       unsetFocus();
   void               setFocusable();
   void               unsetFocusable();
   bool               ignorePadding (bool);    // ignore padding from
   bool               ignorePadding();         // the parent widget
   bool               acceptPadding();
   void               setForegroundColor (short);
   void               setBackgroundColor (short);
   void               setColor();
   virtual void       setX (int, bool = true);  // positioning
   virtual void       setY (int, bool = true);
   virtual void       setPos (const FPoint&, bool = true);
   virtual void       setPos (int, int, bool = true);
   virtual void       setWidth (int, bool = true);
   virtual void       setHeight (int, bool = true);
   virtual void       setSize (int, int, bool = true);
   void               setTopPadding (int, bool = true);
   void               setLeftPadding (int, bool = true);
   void               setBottomPadding (int, bool = true);
   void               setRightPadding (int, bool = true);
   void               setParentOffset();
   void               setTermOffset();
   void               setTermOffsetWithPadding();
   void               setTermSize (int, int);
   virtual void       setGeometry (const FRect&, bool = true);
   virtual void       setGeometry (int, int, int, int, bool = true);
   virtual void       setShadowSize (int, int);
   void               setMinimumWidth (int);
   void               setMinimumHeight (int);
   void               setMinimumSize (int, int);
   void               setMaximumWidth (int);
   void               setMaximumHeight (int);
   void               setMaximumSize (int, int);
   void               setFixedSize (int, int);
   bool               setCursorPos (const FPoint&);
   bool               setCursorPos (register int, register int);
   void               unsetCursorPos();
   void               setPrintPos (const FPoint&);
   void               setPrintPos (register int, register int);
   void               setDoubleFlatLine (fc::sides, bool = true);
   void               unsetDoubleFlatLine (fc::sides);
   void               setDoubleFlatLine (fc::sides, int, bool = true);
   void               unsetDoubleFlatLine (fc::sides, int);

   // Inquiries
   bool               isRootWidget() const;
   bool               isWindowWidget() const;
   bool               isDialogWidget() const;
   bool               isMenuWidget() const;
   bool               isVisible() const;
   bool               isShown() const;
   bool               isEnabled() const;
   bool               hasVisibleCursor() const;
   bool               hasFocus() const;
   bool               acceptFocus() const;  // is focusable
   bool               isPaddingIgnored();

   // Methods
   static FWidget*    childWidgetAt (FWidget*, const FPoint&);
   static FWidget*    childWidgetAt (FWidget*, int, int);
   int                numOfFocusableChildren();
   virtual bool       close();
   void               clearStatusbarMessage();
   void               addCallback ( FString
                                  , FCallback
                                  , data_ptr = null );
   void               addCallback ( FString
                                  , FWidget*
                                  , FMemberCallback
                                  , data_ptr = null );
   void               delCallback (FCallback);
   void               delCallback (FWidget*);
   void               delCallbacks();
   void               emitCallback (FString);
   void               addAccelerator (int);
   virtual void       addAccelerator (int, FWidget*);
   void               delAccelerator ();
   virtual void       delAccelerator (FWidget*);
   virtual void       redraw();
   virtual void       resize();
   virtual void       show();
   virtual void       hide();
   virtual bool       focusFirstChild();  // widget focusing
   virtual bool       focusLastChild();
   FPoint             termToWidgetPos (const FPoint&);
   void               detectTermSize();
   virtual void       move (const FPoint&);
   virtual void       move (int, int);
   void               drawShadow();
   void               clearShadow();
   void               drawFlatBorder();
   void               clearFlatBorder();
   virtual void       drawBorder (int, int, int, int);
   virtual void       drawBorder();
   static void        quit();

   // Data Members
   static widgetList* window_list;
   Accelerators*      accelerator_list;

 protected:
   struct callback_data
   {
     FString   cb_signal;
     FCallback cb_handler;
     data_ptr  data;
   };

   struct member_callback_data
   {
     FString         cb_signal;
     FWidget*        cb_instance;
     FMemberCallback cb_handler;
     data_ptr        data;
   };

   // Typedefs
   typedef std::vector<callback_data> CallbackObjects;
   typedef std::vector<member_callback_data> MemberCallbackObjects;

   // Mutators
   virtual void       setStatusBar (FStatusBar*);
   virtual void       setMenuBar (FMenuBar*);

   // Methods
   virtual void       adjustSize();
   void               adjustSizeGlobal();
   virtual bool       focusNextChild();  // Change child
   virtual bool       focusPrevChild();  // focus

   // Event handlers
   bool               event (FEvent*);
   virtual void       onKeyPress (FKeyEvent*);
   virtual void       onKeyUp (FKeyEvent*);
   virtual void       onKeyDown (FKeyEvent*);
   virtual void       onMouseDown (FMouseEvent*);
   virtual void       onMouseUp (FMouseEvent*);
   virtual void       onMouseDoubleClick (FMouseEvent*);
   virtual void       onWheel (FWheelEvent*);
   virtual void       onMouseMove (FMouseEvent*);
   virtual void       onFocusIn (FFocusEvent*);
   virtual void       onFocusOut (FFocusEvent*);
   virtual void       onChildFocusIn (FFocusEvent*);
   virtual void       onChildFocusOut (FFocusEvent*);
   virtual void       onAccel (FAccelEvent*);
   virtual void       onResize (FResizeEvent*);
   virtual void       onShow (FShowEvent*);
   virtual void       onHide (FHideEvent*);
   virtual void       onClose (FCloseEvent*);

   // Data Members
   static struct widget_colors
   {
     short term_fg;
     short term_bg;
     short list_fg;
     short list_bg;
     short selected_list_fg;
     short selected_list_bg;
     short current_element_focus_fg;
     short current_element_focus_bg;
     short current_element_fg;
     short current_element_bg;
     short current_inc_search_element_fg;
     short selected_current_element_focus_fg;
     short selected_current_element_focus_bg;
     short selected_current_element_fg;
     short selected_current_element_bg;
     short label_fg;
     short label_bg;
     short label_inactive_fg;
     short label_inactive_bg;
     short label_hotkey_fg;
     short label_hotkey_bg;
     short label_emphasis_fg;
     short label_ellipsis_fg;
     short inputfield_active_focus_fg;
     short inputfield_active_focus_bg;
     short inputfield_active_fg;
     short inputfield_active_bg;
     short inputfield_inactive_fg;
     short inputfield_inactive_bg;
     short dialog_fg;
     short dialog_resize_fg;
     short dialog_emphasis_fg;
     short dialog_bg;
     short error_box_fg;
     short error_box_emphasis_fg;
     short error_box_bg;
     short tooltip_fg;
     short tooltip_bg;
     short shadow_fg;
     short shadow_bg;
     short toggle_button_active_focus_fg;
     short toggle_button_active_focus_bg;
     short toggle_button_active_fg;
     short toggle_button_active_bg;
     short toggle_button_inactive_fg;
     short toggle_button_inactive_bg;
     short button_active_focus_fg;
     short button_active_focus_bg;
     short button_active_fg;
     short button_active_bg;
     short button_inactive_fg;
     short button_inactive_bg;
     short button_hotkey_fg;
     short titlebar_active_fg;
     short titlebar_active_bg;
     short titlebar_inactive_fg;
     short titlebar_inactive_bg;
     short titlebar_button_fg;
     short titlebar_button_bg;
     short titlebar_button_focus_fg;
     short titlebar_button_focus_bg;
     short menu_active_focus_fg;
     short menu_active_focus_bg;
     short menu_active_fg;
     short menu_active_bg;
     short menu_inactive_fg;
     short menu_inactive_bg;
     short menu_hotkey_fg;
     short menu_hotkey_bg;
     short statusbar_fg;
     short statusbar_bg;
     short statusbar_hotkey_fg;
     short statusbar_hotkey_bg;
     short statusbar_separator_fg;
     short statusbar_active_fg;
     short statusbar_active_bg;
     short statusbar_active_hotkey_fg;
     short statusbar_active_hotkey_bg;
     short scrollbar_fg;
     short scrollbar_bg;
     short scrollbar_button_fg;
     short scrollbar_button_bg;
     short progressbar_fg;
     short progressbar_bg;
   } wc;
   // widget_colors wc;

   int                   flags;
   static uInt           modal_dialogs;
   static widgetList*    dialog_list;
   static widgetList*    always_on_top_list;
   static widgetList*    close_widget;
   CallbackObjects       callback_objects;
   MemberCallbackObjects member_callback_objects;

 private:
   // Disable copy constructor
   FWidget (const FWidget&);

   // Disable assignment operator (=)
   FWidget& operator = (const FWidget&);

   // Methods
   void               init();
   void               finish();
   void               insufficientSpaceAdjust();
   void               processDestroy();
   virtual void       draw();
   static void        setColorTheme();

   // Data Members
   bool               enable;
   bool               visible;
   bool               shown;
   bool               focus;
   bool               focusable;
   bool               visible_cursor;
   FPoint             widget_cursor_position;

   struct widget_size_hints
   {
     widget_size_hints()
     : min_width  (INT_MIN)
     , min_height (INT_MIN)
     , max_width  (INT_MAX)
     , max_height (INT_MAX)
     { }

    ~widget_size_hints()
     { }

     void setMinimum (int w, int h)
     {
       min_width = w;
       min_height = h;
     }

     void setMaximum (int w, int h)
     {
       max_width = w;
       max_height = h;
     }

     int min_width;
     int min_height;
     int max_width;
     int max_height;
   } size_hints;

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

   struct widget_padding
   {
     widget_padding() : top(0), left(0), bottom(0), right(0)
     { }

    ~widget_padding()
     { }

     int top;
     int left;
     int bottom;
     int right;
   } padding;

   bool               ignore_padding;

   // widget size
   FRect              wsize;
   FRect              adjust_wsize;
   FRect              adjust_wsize_term;
   FRect              adjust_wsize_shadow;
   FRect              adjust_wsize_term_shadow;
   // widget offset
   FRect              offset;
   // offset of the widget client area
   FRect              client_offset;
   // widget shadow size (on the right and bottom side)
   FPoint             wshadow;

   // default widget foreground and background color
   short              foreground_color;
   short              background_color;

   FString            statusbar_message;
   static FStatusBar* statusbar;
   static FMenuBar*   menubar;
   static FWidget*    show_root_widget;
   static FWidget*    redraw_root_widget;
   static bool        init_desktop;
   static bool        hideable;

   // Friend class
   friend class FToggleButton;
   friend class FScrollView;
};

#pragma pack(pop)

// FWidget inline functions
//----------------------------------------------------------------------
inline const char* FWidget::getClassName() const
{ return "FWidget"; }

//----------------------------------------------------------------------
inline FWidget* FWidget::getParentWidget() const
{ return static_cast<FWidget*>(getParent()); }

//----------------------------------------------------------------------
inline FString FWidget::getStatusbarMessage() const
{ return statusbar_message; }

//----------------------------------------------------------------------
inline short FWidget::getForegroundColor() const
{ return foreground_color; }

//----------------------------------------------------------------------
inline short FWidget::getBackgroundColor() const
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
inline int FWidget::getWidth() const
{ return adjust_wsize.getWidth(); }

//----------------------------------------------------------------------
inline int FWidget::getHeight() const
{ return adjust_wsize.getHeight(); }

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
inline int FWidget::getClientWidth() const
{ return client_offset.getWidth(); }

//----------------------------------------------------------------------
inline int FWidget::getClientHeight() const
{ return client_offset.getHeight(); }

//----------------------------------------------------------------------
inline int FWidget::getMaxWidth() const
{ return offset.getWidth(); }

//----------------------------------------------------------------------
inline int FWidget::getMaxHeight() const
{ return offset.getHeight(); }

//----------------------------------------------------------------------
inline const FPoint& FWidget::getShadow() const
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
    adjust_wsize.x2_ref() + wshadow.x_ref(),
    adjust_wsize.y2_ref() + wshadow.y_ref()
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
    adjust_wsize.x2_ref() + offset.x1_ref() + wshadow.x_ref(),
    adjust_wsize.y2_ref() + offset.y1_ref() + wshadow.y_ref()
  );
  return adjust_wsize_term_shadow;
}

//----------------------------------------------------------------------
inline int FWidget::getFlags() const
{ return flags; }

//----------------------------------------------------------------------
inline FPoint FWidget::getCursorPos()
{ return widget_cursor_position; }

//----------------------------------------------------------------------
inline bool FWidget::setVisible()
{ return visible = true; }

//----------------------------------------------------------------------
inline bool FWidget::setEnable (bool on)
{ return enable = (on) ? true : false; }

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
inline bool FWidget::setVisibleCursor (bool on)
{ return visible_cursor = (on) ? true : ((hideable) ? false : true); }

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
{ focusable = true; }

//----------------------------------------------------------------------
inline void FWidget::unsetFocusable()
{ focusable = false; }

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
inline void FWidget::setForegroundColor (short color)
{
  // valid colors -1..254
  if ( color == fc::Default || color >> 8 == 0 )
    foreground_color = color;
}

//----------------------------------------------------------------------
inline void FWidget::setBackgroundColor (short color)
{
  // valid colors -1..254
  if ( color == fc::Default || color >> 8 == 0 )
    background_color = color;
}

//----------------------------------------------------------------------
inline void FWidget::setPos (const FPoint& p, bool adjust)
{ setPos (p.getX(), p.getY(), adjust); }

//----------------------------------------------------------------------
inline void FWidget::setGeometry (const FRect& box, bool adjust)
{
  setGeometry ( box.getX()
              , box.getY()
              , box.getWidth()
              , box.getHeight()
              , adjust );
}

//----------------------------------------------------------------------
inline void FWidget::setShadowSize (int right, int bottom)
{ wshadow.setPoint (right, bottom); }

//----------------------------------------------------------------------
inline void FWidget::setMinimumWidth (int min_width)
{ size_hints.setMinimum (min_width, size_hints.min_height); }

//----------------------------------------------------------------------
inline void FWidget::setMinimumHeight (int min_height)
{ size_hints.setMinimum (size_hints.min_width, min_height); }

//----------------------------------------------------------------------
inline void FWidget::setMinimumSize (int min_width, int min_height)
{ size_hints.setMinimum (min_width, min_height); }

//----------------------------------------------------------------------
inline void FWidget::setMaximumWidth (int max_width)
{ size_hints.setMaximum (max_width, size_hints.max_height); }

//----------------------------------------------------------------------
inline void FWidget::setMaximumHeight (int max_height)
{ size_hints.setMaximum (size_hints.max_width, max_height); }

//----------------------------------------------------------------------
inline void FWidget::setMaximumSize (int max_width, int max_height)
{ size_hints.setMaximum (max_width, max_height); }

//----------------------------------------------------------------------
inline void FWidget::setFixedSize (int width, int height)
{
  size_hints.setMinimum (width, height);
  size_hints.setMaximum (width, height);
}

//----------------------------------------------------------------------
inline bool FWidget::setCursorPos (const FPoint& pos)
{ return setCursorPos (pos.getX(), pos.getY()); }

//----------------------------------------------------------------------
inline void FWidget::unsetCursorPos()
{ setCursorPos(-1,-1); }

//----------------------------------------------------------------------
inline void FWidget::setPrintPos (const FPoint& pos)
{ setPrintPos (pos.getX(), pos.getY()); }

//----------------------------------------------------------------------
inline void FWidget::unsetDoubleFlatLine (fc::sides side)
{ setDoubleFlatLine(side, false); }

//----------------------------------------------------------------------
inline void FWidget::unsetDoubleFlatLine (fc::sides side, int pos)
{ setDoubleFlatLine(side, pos, false); }

//----------------------------------------------------------------------
inline bool FWidget::isRootWidget() const
{ return (! hasParent()); }

//----------------------------------------------------------------------
inline bool FWidget::isVisible() const
{ return visible; }

//----------------------------------------------------------------------
inline bool FWidget::isShown() const
{ return shown; }

//----------------------------------------------------------------------
inline bool FWidget::isWindowWidget() const
{ return ((flags & fc::window_widget) != 0); }

//----------------------------------------------------------------------
inline bool FWidget::isDialogWidget() const
{ return ((flags & fc::dialog_widget) != 0); }

//----------------------------------------------------------------------
inline bool FWidget::isMenuWidget() const
{ return ((flags & fc::menu_widget) != 0); }

//----------------------------------------------------------------------
inline bool FWidget::isEnabled() const
{ return enable; }

//----------------------------------------------------------------------
inline bool FWidget::hasVisibleCursor() const
{ return visible_cursor; }

//----------------------------------------------------------------------
inline bool FWidget::hasFocus() const
{ return focus; }

//----------------------------------------------------------------------
inline bool FWidget::acceptFocus() const  // is focusable
{ return focusable; }

//----------------------------------------------------------------------
inline bool FWidget::isPaddingIgnored()
{ return ignore_padding; }

//----------------------------------------------------------------------
inline FWidget* FWidget::childWidgetAt (FWidget* p, const FPoint& pos)
{ return childWidgetAt (p, pos.getX(), pos.getY()); }

//----------------------------------------------------------------------
inline void FWidget::clearStatusbarMessage()
{ statusbar_message.clear(); }

//----------------------------------------------------------------------
inline void FWidget::addAccelerator (int key)
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
inline void FWidget::move (const FPoint& pos)
{ move( pos.getX(), pos.getY() ); }

//----------------------------------------------------------------------
inline void FWidget::drawBorder()
{ drawBorder (1, 1, getWidth(), getHeight()); }

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

#endif  // _FWIDGET_H
