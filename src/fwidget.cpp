// fwidget.cpp
// class FWidget

#include "fapp.h"
#include "fstatusbar.h"
#include "fwidget.h"
#include "fwindow.h"

// global FWidget object
static FWidget* rootObject = 0;

// static class attributes
uInt     FWidget::modal_dialogs;

FStatusBar*            FWidget::statusbar          = 0;
FWidget*               FWidget::show_root_widget   = 0;
FWidget*               FWidget::redraw_root_widget = 0;
FWidget::widgetList*   FWidget::window_list        = 0;
FWidget::widgetList*   FWidget::close_widget       = 0;
FWidget::widget_colors FWidget::wc;

//----------------------------------------------------------------------
// class FWidget
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FWidget::FWidget (FWidget* parent) : FObject(parent)
{
  vwin = 0;
  widgetCursorPosition.setPoint(-1,-1);

  resize_term      = false;
  enable           = true;
  visible          = true;
  shown            = false;
  focus            = false;
  focusable        = true;
  ignore_padding   = false;
  visibleCursor    = false;
  window_object    = false;

  if ( parent == 0 )
  {
    assert ( ! rootObject
           && "FTerm: There should be only one root object" );

    rootObject = this;
    show_root_widget = 0;
    redraw_root_widget = 0;
    modal_dialogs = 0;
    statusbar = 0;
    init();
  }
  else
  {
    accelerator_list = 0;
    flags            = 0;
    xpos             = 1;
    ypos             = 1;
    width            = 1;
    height           = 1;
    top_padding      = 0;
    left_padding     = 0;
    bottom_padding   = 0;
    right_padding    = 0;
    client_xmin      = 0;
    client_ymin      = 0;
    client_xmax      = 0;
    client_ymax      = 0;
    this->xmin = parent->client_xmin;
    this->ymin = parent->client_ymin;
    this->xmax = parent->client_xmax;
    this->ymax = parent->client_ymax;
    double_flatline_mask.top.resize (width, false);
    double_flatline_mask.right.resize (height, false);
    double_flatline_mask.bottom.resize (width, false);
    double_flatline_mask.left.resize (height, false);
  }
}

//----------------------------------------------------------------------
FWidget::~FWidget()  // destructor
{
  processDestroy();

  FApplication::removeQueuedEvent(this);

  if ( this == getClickedWidget() )
    setClickedWidget(0);

  if ( this == FWidget::getFocusWidget() )
    FWidget::setFocusWidget(0);

  if ( this == getMainWidget() )
  {
    setMainWidget(0);
    quit();
  }

  if ( rootObject == this )
    this->finish();
}

// private methods of FWidget
//----------------------------------------------------------------------
void FWidget::init()
{
  window_list    = new widgetList;
  close_widget   = new widgetList;

  getTermGeometry();

  xpos           = xmin;
  ypos           = ymin;
  width          = xmax;
  height         = ymax;
  top_padding    = 0;
  left_padding   = 0;
  bottom_padding = 0;
  right_padding  = 0;
  client_xmin    = xmin;
  client_ymin    = ymin;
  client_xmax    = xmax;
  client_ymax    = ymax;

  widgetSize.setRect(xpos, ypos, width, height);
  adjustWidgetSize.setRect(xpos, ypos, width, height);
  adjustWidgetSizeShadow = adjustWidgetSize;
  adjustWidgetSizeGlobal.setRect ( xpos + xmin - 1,
                                   ypos + ymin - 1,
                                   width, height );
  adjustWidgetSizeGlobalShadow = adjustWidgetSizeGlobal;

  double_flatline_mask.top.resize (width, false);
  double_flatline_mask.right.resize (height, false);
  double_flatline_mask.bottom.resize (width, false);
  double_flatline_mask.left.resize (height, false);

  // default widget colors
  setColorTheme();

  foregroundColor = wc.term_fg;
  backgroundColor = wc.term_bg;
  setColor (foregroundColor, backgroundColor);
  clrscr();

  accelerator_list = new Accelerators;
}

//----------------------------------------------------------------------
void FWidget::finish()
{
  delete accelerator_list;

  if ( close_widget )
  {
    delete close_widget;
    close_widget = 0;
  }
  if ( window_list )
  {
    delete window_list;
    window_list = 0;
  }
}

//----------------------------------------------------------------------
void FWidget::processDestroy()
{
  emitCallback("destroy");
}

//----------------------------------------------------------------------
void FWidget::draw()
{
}

//----------------------------------------------------------------------
void FWidget::setColorTheme()
{
  wc.term_fg                       = fc::Black;
  wc.term_bg                       = fc::LightBlue;
  wc.list_fg                       = fc::Black;
  wc.list_bg                       = fc::White;
  wc.selected_list_fg              = fc::Cyan;
  wc.selected_list_bg              = fc::White;
  wc.dialog_fg                     = fc::Black;
  wc.dialog_emphasis_fg            = fc::Blue;
  wc.dialog_bg                     = fc::White;
  wc.error_box_fg                  = fc::White;
  wc.error_box_emphasis_fg         = fc::Yellow;
  wc.error_box_bg                  = fc::LightRed;
  wc.shadow_fg                     = fc::Black;
  wc.shadow_bg                     = fc::LightGray; // only for transparent shadow
  wc.current_element_focus_fg      = fc::White;
  wc.current_element_focus_bg      = fc::Blue;
  wc.current_element_fg            = fc::LightGray;
  wc.current_element_bg            = fc::Blue;
  wc.current_inc_search_element_fg = fc::LightRed;
  wc.selected_current_element_fg   = fc::LightBlue;
  wc.selected_current_element_bg   = fc::Blue;
  wc.label_fg                      = fc::Black;
  wc.label_bg                      = fc::White;
  wc.label_inactive_fg             = fc::LightGray;
  wc.label_inactive_bg             = fc::White;
  wc.label_hotkey_fg               = fc::Red;
  wc.label_hotkey_bg               = fc::White;
  wc.label_emphasis_fg             = fc::Blue;
  wc.label_ellipsis_fg             = fc::DarkGray;
  wc.inputfield_active_focus_fg    = fc::White;
  wc.inputfield_active_focus_bg    = fc::Cyan;
  wc.inputfield_active_fg          = fc::Black;
  wc.inputfield_active_bg          = fc::LightGray;
  wc.inputfield_inactive_fg        = fc::DarkGray;
  wc.inputfield_inactive_bg        = fc::LightGray;
  wc.toggle_button_active_focus_fg = fc::White;
  wc.toggle_button_active_focus_bg = fc::Cyan;
  wc.toggle_button_active_fg       = fc::Black;
  wc.toggle_button_active_bg       = fc::White;
  wc.toggle_button_inactive_fg     = fc::LightGray;
  wc.toggle_button_inactive_bg     = fc::White;
  wc.button_active_focus_fg        = fc::LightGray;
  wc.button_active_focus_bg        = fc::Blue;
  wc.button_active_fg              = fc::LightGray;
  wc.button_active_bg              = fc::DarkGray;
  wc.button_inactive_fg            = fc::DarkGray;
  wc.button_inactive_bg            = fc::LightGray;
  wc.button_hotkey_fg              = fc::White;
  wc.titlebar_active_fg            = fc::White;
  wc.titlebar_active_bg            = fc::Blue;
  wc.titlebar_inactive_fg          = fc::LightGray;
  wc.titlebar_inactive_bg          = fc::DarkGray;
  wc.titlebar_button_fg            = fc::Black;
  wc.titlebar_button_bg            = fc::LightGray;
  wc.statusbar_fg                  = fc::White;
  wc.statusbar_bg                  = fc::Blue;
  wc.statusbar_hotkey_fg           = fc::LightRed;
  wc.statusbar_hotkey_bg           = fc::Blue;
  wc.statusbar_separator_fg        = fc::Black;
  wc.statusbar_active_fg           = fc::Blue;
  wc.statusbar_active_bg           = fc::White;
  wc.statusbar_active_hotkey_fg    = fc::DarkGray;
  wc.statusbar_active_hotkey_bg    = fc::White;
  wc.scrollbar_fg                  = fc::DarkGray;
  wc.scrollbar_bg                  = fc::LightBlue;
  wc.scrollbar_button_fg           = fc::Black;
  wc.scrollbar_button_bg           = fc::LightGray;
  wc.progressbar_fg                = fc::DarkGray;
  wc.progressbar_bg                = fc::LightBlue;

  if ( kde_konsole )
    wc.term_bg = fc::SteelBlue3;

  if ( tera_terminal )
    wc.term_bg = fc::LightBlue;

  if ( max_color < 16 )  // for 8 color mode
  {
    wc.term_fg                       = fc::Black;
    wc.term_bg                       = fc::Cyan;
    wc.list_fg                       = fc::Black;
    wc.list_bg                       = fc::LightGray;
    wc.selected_list_fg              = fc::Blue;
    wc.selected_list_bg              = fc::LightGray;
    wc.dialog_fg                     = fc::Black;
    wc.dialog_emphasis_fg            = fc::Blue;
    wc.dialog_bg                     = fc::LightGray;
    wc.error_box_fg                  = fc::LightGray;
    wc.error_box_emphasis_fg         = fc::Brown;
    wc.error_box_bg                  = fc::Red;
    wc.shadow_fg                     = fc::Black;
    wc.shadow_bg                     = fc::LightGray; // only for transparent shadow
    wc.current_element_focus_fg      = fc::LightGray;
    wc.current_element_focus_bg      = fc::Blue;
    wc.current_element_fg            = fc::LightGray;
    wc.current_element_bg            = fc::Blue;
    wc.current_inc_search_element_fg = fc::Red;
    wc.selected_current_element_fg   = fc::Cyan;
    wc.selected_current_element_bg   = fc::Blue;
    wc.label_fg                      = fc::Black;
    wc.label_bg                      = fc::LightGray;
    wc.label_inactive_fg             = fc::Cyan;
    wc.label_inactive_bg             = fc::LightGray;
    wc.label_hotkey_fg               = fc::Red;
    wc.label_hotkey_bg               = fc::LightGray;
    wc.label_emphasis_fg             = fc::Blue;
    wc.label_ellipsis_fg             = fc::DarkGray;
    wc.inputfield_active_focus_fg    = fc::LightGray;
    wc.inputfield_active_focus_bg    = fc::Blue;
    wc.inputfield_active_fg          = fc::Black;
    wc.inputfield_active_bg          = fc::Cyan;
    wc.inputfield_inactive_fg        = fc::Black;
    wc.inputfield_inactive_bg        = fc::LightGray;
    wc.toggle_button_active_focus_fg = fc::LightGray;
    wc.toggle_button_active_focus_bg = fc::Blue;
    wc.toggle_button_active_fg       = fc::Black;
    wc.toggle_button_active_bg       = fc::LightGray;
    wc.toggle_button_inactive_fg     = fc::Cyan;
    wc.toggle_button_inactive_bg     = fc::LightGray;
    wc.button_active_focus_fg        = fc::LightGray;
    wc.button_active_focus_bg        = fc::Blue;
    wc.button_active_fg              = fc::Black;
    wc.button_active_bg              = fc::Cyan;
    wc.button_inactive_fg            = fc::Black;
    wc.button_inactive_bg            = fc::LightGray;
    wc.button_hotkey_fg              = fc::Red;
    wc.titlebar_active_fg            = fc::LightGray;
    wc.titlebar_active_bg            = fc::Blue;
    wc.titlebar_inactive_fg          = fc::Black;
    wc.titlebar_inactive_bg          = fc::LightGray;
    wc.titlebar_button_fg            = fc::Black;
    wc.titlebar_button_bg            = fc::LightGray;
    wc.statusbar_fg                  = fc::LightGray;
    wc.statusbar_bg                  = fc::Blue;
    wc.statusbar_hotkey_fg           = fc::Red;
    wc.statusbar_hotkey_bg           = fc::Blue;
    wc.statusbar_separator_fg        = fc::Black;
    wc.statusbar_active_fg           = fc::Blue;
    wc.statusbar_active_bg           = fc::LightGray;
    wc.statusbar_active_hotkey_fg    = fc::Red;
    wc.statusbar_active_hotkey_bg    = fc::LightGray;
    wc.scrollbar_fg                  = fc::Blue;
    wc.scrollbar_bg                  = fc::LightGray;
    wc.scrollbar_button_fg           = fc::LightGray;
    wc.scrollbar_button_bg           = fc::Black;
    wc.progressbar_fg                = fc::Blue;
    wc.progressbar_bg                = fc::LightGray;
  }
}

// protected methods of FWidget
//----------------------------------------------------------------------
void FWidget::adjustSize()
{
  if ( ! isRootWidget() )
  {
    if ( isWindow() )
    {
      xmin = rootObject->client_xmin;
      ymin = rootObject->client_ymin;
      xmax = rootObject->client_xmax;
      ymax = rootObject->client_ymax;
    }
    else if ( ignore_padding )
    {
      xmin = parentWidget()->xpos + parentWidget()->xmin - 1;
      ymin = parentWidget()->ypos + parentWidget()->ymin - 1;
      xmax = parentWidget()->xpos + parentWidget()->xmin - 2
           + parentWidget()->width;
      ymax = parentWidget()->ypos + parentWidget()->ymin - 2
           + parentWidget()->height;
    }
    else
    {
      xmin = parentWidget()->client_xmin;
      ymin = parentWidget()->client_ymin;
      xmax = parentWidget()->client_xmax;
      ymax = parentWidget()->client_ymax;
    }
    xpos   = widgetSize.getX();
    ypos   = widgetSize.getY();
    width  = widgetSize.getWidth();
    height = widgetSize.getHeight();
  }

  if ( ! isWindow() )
  {
    while ( xpos+xmin-1+width > xmax+1 )
    {
      xpos--;
      if ( xpos < 1 )
      {
        xpos = 1;
        width--;
      }
    }

    while ( ypos+ymin-1+height > ymax+1 )
    {
      ypos--;
      if ( ypos < 1 )
      {
        ypos = 1;
        height--;
      }
    }

    while ( xmin+width-1 > xmax )
      width--;
    while ( ymin+height-1 > ymax )
      height--;
  }

  adjustWidgetSize.setRect(xpos, ypos, width, height);
  adjustWidgetSizeShadow = adjustWidgetSize + shadow;

  adjustWidgetSizeGlobal.setRect ( xpos + xmin - 1,
                                   ypos + ymin - 1,
                                   width, height );
  adjustWidgetSizeGlobalShadow = adjustWidgetSizeGlobal + shadow;

  client_xmin = xpos + xmin - 1 + left_padding;
  client_ymin = ypos + ymin - 1 + top_padding;
  client_xmax = xpos + xmin - 2 + width  - right_padding;
  client_ymax = ypos + ymin - 2 + height - bottom_padding;

  if ( this->hasChildren() )
  {
    FObject::object_list children;
    FObject::object_list::const_iterator iter, end;

    children = this->children();
    iter = children.begin();
    end  = children.end();

    while ( iter != end )
    {
      FWidget* widget = static_cast<FWidget*>(*iter);
      if ( ! widget->isWindow() )
        widget->adjustSize();
      ++iter;
    }
  }
}

//----------------------------------------------------------------------
void FWidget::setStatusBar (FStatusBar* sbar)
{
  if ( ! sbar || statusbar == sbar )
    return;
  if ( statusbar )
    delete statusbar;

  statusbar = sbar;
}

//----------------------------------------------------------------------
bool FWidget::event (FEvent* ev)
{
  switch ( ev->type() )
  {
    case KeyPress_Event:
      {
        FKeyEvent* kev = static_cast<FKeyEvent*>(ev);
        bool accpt_focus = false;
        if ( kev->key() == fc::Fkey_tab )
          accpt_focus = focusNextChild();
        else if ( kev->key() == fc::Fkey_btab )
          accpt_focus = focusPrevChild();
        if ( accpt_focus )
          break;
        FWidget* widget = this;
        while ( widget )
        {
          widget->onKeyPress(kev);
          if ( ! kev->isAccepted() )
          {
            if (  kev->key() == fc::Fkey_right
               || kev->key() == fc::Fkey_down )
              accpt_focus = focusNextChild();
            else if (  kev->key() == fc::Fkey_left
                    || kev->key() == fc::Fkey_up )
              accpt_focus = focusPrevChild();
            if ( accpt_focus )
              break;
          }
          if ( kev->isAccepted() || widget->isRootWidget() )
            break;
          widget = widget->parentWidget();
        }
      }
      break;

    case KeyUp_Event:
      onKeyUp ( static_cast<FKeyEvent*>(ev) );
      break;

    case KeyDown_Event:
      {
        FKeyEvent* kev = static_cast<FKeyEvent*>(ev);
        FWidget* widget = this;
        while ( widget )
        {
          widget->onKeyDown(kev);
          if ( kev->isAccepted() || widget->isRootWidget() )
            break;
          widget = widget->parentWidget();
        }
      }
      break;

    case MouseDown_Event:
      onMouseDown ( static_cast<FMouseEvent*>(ev) );
      break;

    case MouseUp_Event:
      onMouseUp ( static_cast<FMouseEvent*>(ev) );
      break;

    case MouseDoubleClick_Event:
      onMouseDoubleClick ( static_cast<FMouseEvent*>(ev) );
      break;

    case MouseWheel_Event:
      onWheel ( static_cast<FWheelEvent*>(ev) );
      break;

    case MouseMove_Event:
      onMouseMove ( static_cast<FMouseEvent*>(ev) );
      break;

    case FocusIn_Event:
      onFocusIn ( static_cast<FFocusEvent*>(ev) );
      break;

    case FocusOut_Event:
      onFocusOut ( static_cast<FFocusEvent*>(ev) );
      break;

    case Accelerator_Event:
      onAccel ( static_cast<FAccelEvent*>(ev) );
      break;

    case Resize_Event:
      onResize ( static_cast<FResizeEvent*>(ev) );
      break;

    case Show_Event:
      onShow ( static_cast<FShowEvent*>(ev) );
      break;

    case Hide_Event:
      onHide ( static_cast<FHideEvent*>(ev) );
      break;

    case Close_Event:
      onClose ( static_cast<FCloseEvent*>(ev) );
      break;

    case Timer_Event:
      onTimer ( static_cast<FTimerEvent*>(ev) );
      break;

    default:
      return false;
  }
  return true;
}

//----------------------------------------------------------------------
void FWidget::onKeyPress (FKeyEvent*)
{
}

//----------------------------------------------------------------------
void FWidget::onKeyUp (FKeyEvent*)
{
}

//----------------------------------------------------------------------
void FWidget::onKeyDown (FKeyEvent*)
{
}

//----------------------------------------------------------------------
void FWidget::onMouseDown (FMouseEvent*)
{
}

//----------------------------------------------------------------------
void FWidget::onMouseUp (FMouseEvent*)
{
}

//----------------------------------------------------------------------
void FWidget::onMouseDoubleClick (FMouseEvent*)
{
}

//----------------------------------------------------------------------
void FWidget::onWheel (FWheelEvent*)
{
}

//----------------------------------------------------------------------
void FWidget::onMouseMove (FMouseEvent*)
{
}

//----------------------------------------------------------------------
void FWidget::onFocusIn (FFocusEvent*)
{
}

//----------------------------------------------------------------------
void FWidget::onFocusOut (FFocusEvent*)
{
}

//----------------------------------------------------------------------
void FWidget::onAccel (FAccelEvent*)
{
}

//----------------------------------------------------------------------
void FWidget::onResize (FResizeEvent* event)
{
  if ( openConsole() == 0 )
  {
    getTermGeometry();
    closeConsole();
  }
  rootObject->resize();
  rootObject->redraw();
  event->accept();
}

//----------------------------------------------------------------------
void FWidget::onShow (FShowEvent*)
{
}

//----------------------------------------------------------------------
void FWidget::onHide (FHideEvent*)
{
}

//----------------------------------------------------------------------
void FWidget::onClose (FCloseEvent* event)
{
  event->accept();
}

//----------------------------------------------------------------------
bool FWidget::focusNextChild(void)
{
  if ( hasParent() )
  {
    if ( parent()->hasChildren() )
    {
      FObject::object_list children;
      FObject::object_list::iterator iter, end;

      children = parent()->children();
      iter = children.begin();
      end  = children.end();

      while ( iter != end )
      {
        FWidget* w = static_cast<FWidget*>(*iter);
        if ( w == this )
        {
          FWidget* next;
          FObject::object_list::const_iterator next_element;

          next_element = iter;
          do
          {
            ++next_element;
            if ( next_element == children.end() )
              next_element = children.begin();
            next = static_cast<FWidget*>(*next_element);
          } while (  ! next->isEnabled()
                  || ! next->acceptFocus()
                  || ! next->isVisible()
                  || next->isWindow() );
          FFocusEvent out (FocusOut_Event);
          out.setFocusType(FocusNextWidget);
          FApplication::sendEvent(this, &out);
          if ( out.isAccepted() )
          {
            if ( next == this )
              return false;
            next->setFocus();
            FFocusEvent in (FocusIn_Event);
            in.setFocusType(FocusNextWidget);
            FApplication::sendEvent(next, &in);
            if ( in.isAccepted() )
            {
              this->draw();
              next->draw();
              updateTerminal();
              flush_out();
            }
          }
          break;
        }
        ++iter;
      }
    }
  }
  return true;
}

//----------------------------------------------------------------------
bool FWidget::focusPrevChild(void)
{
  if ( hasParent() )
  {
    if ( parent()->hasChildren() )
    {
      FObject::object_list children;
      FObject::object_list::iterator iter, begin;

      children = parent()->children();
      iter  = children.end();
      begin = children.begin();
      do
      {
        --iter;
        FWidget* w = static_cast<FWidget*>(*iter);
        if ( w == this )
        {
          FWidget* prev;
          FObject::object_list::const_iterator prev_element;

          prev_element = iter;
          do
          {
            if ( prev_element == children.begin() )
              prev_element = children.end();
            --prev_element;
            prev = static_cast<FWidget*>(*prev_element);
          } while (  ! prev->isEnabled()
                  || ! prev->acceptFocus()
                  || ! prev->isVisible()
                  || prev->isWindow() );
          FFocusEvent out (FocusOut_Event);
          out.setFocusType(FocusPreviousWidget);
          FApplication::sendEvent(this, &out);
          if ( out.isAccepted() )
          {
            if ( prev == this )
              return false;
            prev->setFocus();
            FFocusEvent in (FocusIn_Event);
            in.setFocusType(FocusPreviousWidget);
            FApplication::sendEvent(prev, &in);
            if ( in.isAccepted() )
            {
              this->draw();
              prev->draw();
              updateTerminal();
              flush_out();
            }
          }
          break;
        }
      } while ( iter != begin );
    }
  }
  return true;
}


// public methods of FWidget
//----------------------------------------------------------------------
FWidget* FWidget::getRootWidget() const
{
  FWidget* obj = const_cast<FWidget*>(this);
  FWidget* p_obj = parentWidget();

  while ( ! obj->isRootWidget() && p_obj )
  {
    obj = p_obj;
    p_obj = p_obj->parentWidget();
  }
  return obj;
}

//----------------------------------------------------------------------
FWidget* FWidget::getMainWidget()
{
  FWidget* main_widget = static_cast<FWidget*>(FApplication::main_widget);
  return main_widget;
}

//----------------------------------------------------------------------
void FWidget::setMainWidget(FWidget* obj)
{
  FApplication* fapp = static_cast<FApplication*>(rootObject);
  fapp->setMainWidget(obj);
}

//----------------------------------------------------------------------
FWidget* FWidget::childWidgetAt (FWidget* p, const FPoint& pos)
{
  return childWidgetAt (p, pos.getX(), pos.getY());
}

//----------------------------------------------------------------------
FWidget* FWidget::childWidgetAt (FWidget* p, int x, int y)
{
  if ( p && p->hasChildren() )
  {
    FObject::object_list children;
    FObject::object_list::const_iterator iter, end;

    children = p->children();
    iter = children.begin();
    end  = children.end();

    while ( iter != end )
    {
      FWidget* widget = static_cast<FWidget*>(*iter);
      if (  widget->isEnabled()
         && widget->isVisible()
         && ! widget->isWindow()
         && widget->getGeometryGlobal().contains(x,y) )
      {
        FWidget* child = childWidgetAt(widget, x, y);
        return (child != 0) ? child : widget;
      }
      ++iter;
    }
  }
  return 0;
}

//----------------------------------------------------------------------
FWidget* FWidget::getFocusWidget() const
{
  FWidget* focus_widget = static_cast<FWidget*>(FApplication::focus_widget);
  return focus_widget;
}

//----------------------------------------------------------------------
void FWidget::setFocusWidget(FWidget* obj)
{
  FApplication::focus_widget = obj;
}

//----------------------------------------------------------------------
FWidget* FWidget::getClickedWidget()
{
  FWidget* clicked_widget = static_cast<FWidget*>(FApplication::clicked_widget);
  return clicked_widget;
}

//----------------------------------------------------------------------
void FWidget::setClickedWidget(FWidget* obj)
{
  FApplication::clicked_widget = obj;
}

//----------------------------------------------------------------------
int FWidget::numOfFocusableChildren()
{
  FObject::object_list children;
  FObject::object_list::const_iterator iter, end;

  if ( ! this->hasChildren() )
    return 0;

  int num = 0;

  children = this->children();
  iter = children.begin();
  end  = children.end();

  while ( iter != end )
  {
    FWidget* widget = static_cast<FWidget*>(*iter);
    if ( widget->acceptFocus() )
      num++;
    ++iter;
  }
  return num;
}

//----------------------------------------------------------------------
bool FWidget::close()
{
  FCloseEvent event(Close_Event);
  FApplication::sendEvent(this, &event);

  if ( event.isAccepted() )
  {
    if ( this == getMainWidget() )
      quit();
    else
    {
      hide();
      if ( (flags & MODAL) == 0 )
        close_widget->push_back(this);
    }
    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
FStatusBar* FWidget::statusBar()
{
  if ( statusbar )
    return statusbar;
  else
    return 0;
}

//----------------------------------------------------------------------
void FWidget::addCallback ( FString cb_signal,
                            FWidget::FCallback cb_handler,
                            data_ptr data )
{
  // add a (normal) function pointer as callback
  callback_data obj = { cb_signal, cb_handler, data };
  this->callbackObjects.push_back(obj);
}

//----------------------------------------------------------------------
void FWidget::addCallback ( FString cb_signal,
                            FWidget* cb_instance,
                            FWidget::FMemberCallback cb_handler,
                            data_ptr data )
{
  // add a member function pointer as callback
  member_callback_data obj = { cb_signal, cb_instance, cb_handler, data };
  this->memberCallbackObjects.push_back(obj);
}

//----------------------------------------------------------------------
void FWidget::delCallback (FWidget::FCallback cb_handler)
{
  FWidget::CallbackObjects::iterator iter;

  // delete a cb_handler function pointer
  if ( callbackObjects.empty() )
    return;

  iter = callbackObjects.begin();

  while ( iter != callbackObjects.end() )
  {
    if ( iter->cb_handler == cb_handler )
      iter = this->callbackObjects.erase(iter);
    else
      ++iter;
  }
}

//----------------------------------------------------------------------
void FWidget::delCallback (FWidget* cb_instance)
{
  FWidget::MemberCallbackObjects::iterator iter;

  // delete all member function pointer from cb_instance
  if ( memberCallbackObjects.empty() )
    return;

  iter = memberCallbackObjects.begin();

  while ( iter != memberCallbackObjects.end() )
  {
    if ( iter->cb_instance == cb_instance )
      iter = memberCallbackObjects.erase(iter);
    else
      ++iter;
  }
}

//----------------------------------------------------------------------
void FWidget::emitCallback (FString emit_signal)
{
  // member function pointer
  if ( ! memberCallbackObjects.empty() )
  {
    FWidget::MemberCallbackObjects::const_iterator m_iter, m_end;
    m_iter = memberCallbackObjects.begin();
    m_end = memberCallbackObjects.end();

    while ( m_iter != m_end )
    {
      if ( m_iter->cb_signal == emit_signal )
      {
        FMemberCallback callback = m_iter->cb_handler;
        // call the member function pointer
        (m_iter->cb_instance->*callback)(this, m_iter->data);
      }
      ++m_iter;
    }
  }
  // function pointer
  if ( ! callbackObjects.empty() )
  {
    FWidget::CallbackObjects::const_iterator iter, end;
    iter = callbackObjects.begin();
    end = callbackObjects.end();

    while ( iter != end )
    {
      if ( iter->cb_signal == emit_signal )
      {
        FCallback callback = iter->cb_handler;
        // call the function pointer
        callback(this, iter->data);
      }
      ++iter;
    }
  }
}

//----------------------------------------------------------------------
void FWidget::addAccelerator (int key)
{
  FWidget* window = FWindow::getWindowWidget(this);
  accelerator accel = { key, this };

  if ( ! window )
    window = getRootWidget();
  if ( window == statusbar )
    window = FWindow::getWindowWidget(parentWidget());
  if ( window && window->accelerator_list )
    window->accelerator_list->push_back(accel);
}

//----------------------------------------------------------------------
void FWidget::addAccelerator (int key, FWidget* obj)
{
  FWidget* window = FWindow::getWindowWidget(obj);
  accelerator accel = { key, obj };

  if ( ! window )
    window = getRootWidget();
  if ( window == statusbar )
    window = FWindow::getWindowWidget(parentWidget());
  if ( window && window->accelerator_list )
    window->accelerator_list->push_back(accel);
}

//----------------------------------------------------------------------
void FWidget::delAccelerator (FWidget* obj)
{
  FWidget* window = FWindow::getWindowWidget(this);

  if ( ! window )
    window = getRootWidget();
  if ( window == statusbar )
    window = FWindow::getWindowWidget(parentWidget());
  if (  window
     && window->accelerator_list
     && ! window->accelerator_list->empty() )
  {
    FWidget::Accelerators::iterator iter;
    iter = window->accelerator_list->begin();

    while ( iter != window->accelerator_list->end() )
    {
      if ( iter->object == obj )
        iter = window->accelerator_list->erase(iter);
      else
        ++iter;
    }
  }
}

//----------------------------------------------------------------------
void FWidget::redraw()
{
  if ( ! redraw_root_widget )
    redraw_root_widget = this;

  if ( isRootWidget() )
  {
    terminal_updates = false;
    setColor (wc.term_fg, wc.term_bg);
    clrscr();
  }
  else if ( ! visible )
    return;

  draw();

  if ( isRootWidget() )
  {
    // draw windows
    char_data default_char;
    default_char.code      = ' ';
    default_char.fg_color  = fc::Black;
    default_char.bg_color  = fc::Black;
    default_char.bold      = 0;
    default_char.reverse   = 0;
    default_char.underline = 0;

    if ( window_list && ! window_list->empty() )
    {
      widgetList::const_iterator iter, end;
      iter = window_list->begin();
      end  = window_list->end();

      while ( iter != end )
      {
        if ( (*iter)->isVisible() )
        {
          term_area* win = (*iter)->getVWin();
          int w = win->width  + win->right_shadow;
          int h = win->height + win->bottom_shadow;
          std::fill_n (win->text, w * h, default_char);

          (*iter)->redraw();
        }
        ++iter;
      }
    }
    if ( statusbar && vstatusbar )
    {
      int w = vstatusbar->width;
      int h = vstatusbar->height;
      std::fill_n (vstatusbar->text, w * h, default_char);
      statusbar->redraw();
    }
  }
  else
  {
    // draw child elements
    if ( this->hasChildren() )
    {
      FObject::object_list children;
      FObject::object_list::const_iterator iter, end;

      children = this->children();
      iter = children.begin();
      end  = children.end();

      while ( iter != end )
      {
        FWidget* widget = static_cast<FWidget*>(*iter);
        if ( widget->isVisible() && ! widget->isWindow() )
          widget->redraw();
        ++iter;
      }
    }
  }

  if ( isRootWidget() )
    terminal_updates = true;

  if ( redraw_root_widget == this )
  {
    updateTerminal();
    flush_out();
    redraw_root_widget = 0;
  }
}

//----------------------------------------------------------------------
void FWidget::resize()
{
  if ( isRootWidget() && openConsole() == 0 )
  {
    getTermGeometry();
    closeConsole();
    resizeVTerm();
    resizeArea (vdesktop);
    if ( statusbar )
    {
      statusbar->setGeometry(1, height, width, 1, false);
      if ( vstatusbar )
        resizeArea(vstatusbar);
    }
    if ( window_list && ! window_list->empty() )
    {
      widgetList::const_iterator iter, end;
      iter = window_list->begin();
      end  = window_list->end();

      while ( iter != end )
      {
        if ( (*iter)->isVisible() )
          (*iter)->adjustSize();
        ++iter;
      }
    }
  }
  else
    adjustSize();
  double_flatline_mask.top.resize (width, false);
  double_flatline_mask.right.resize (height, false);
  double_flatline_mask.bottom.resize (width, false);
  double_flatline_mask.left.resize (height, false);
}

//----------------------------------------------------------------------
void FWidget::show()
{
  if ( ! visible )
    return;

  if ( getMainWidget() == this && ! NewFont && ! VGAFont )
    init_consoleCharMap();

  if ( ! show_root_widget )
  {
    terminal_updates = false;
    show_root_widget = this;
  }

  draw();
  shown = true;

  if ( this->hasChildren() )
  {
    FObject::object_list children;
    FObject::object_list::const_iterator iter, end;

    children = this->children();
    iter = children.begin();
    end  = children.end();

    while ( iter != end )
    {
      FWidget* widget = static_cast<FWidget*>(*iter);
      widget->show();
      ++iter;
    }
  }

  if ( show_root_widget && show_root_widget == this )
  {
    terminal_updates = true;
    updateTerminal();
    flush_out();
    show_root_widget = 0;
  }

  FShowEvent show (Show_Event);
  FApplication::sendEvent( this, &show );
}

//----------------------------------------------------------------------
void FWidget::hide()
{
  if ( visible )
  {
    visible = false;
    shown = false;

    if ( FWidget::getFocusWidget() == this )
    {
      if ( ! focusPrevChild() )
      {
        FWidget::getFocusWidget()->unsetFocus();
        FWidget::setFocusWidget(parentWidget());
      }
    }
    FHideEvent hide (Hide_Event);
    FApplication::sendEvent( this, &hide );
  }
}

//----------------------------------------------------------------------
bool FWidget::setEnable(bool on)
{
  return enable = (on) ? true : false;
}

//----------------------------------------------------------------------
bool FWidget::setVisibleCursor(bool on)
{
  return visibleCursor = (on) ? true : false;
}

//----------------------------------------------------------------------
bool FWidget::focusFirstChild (void)
{
  FObject::object_list children;
  FObject::object_list::const_iterator iter, end;

  if ( ! this->hasChildren() )
    return false;

  children = this->children();
  iter = children.begin();
  end  = children.end();

  while ( iter != end )
  {
    FWidget* widget = static_cast<FWidget*>(*iter);
    if (  widget->isEnabled()
       && widget->acceptFocus()
       && ! widget->isWindow() )
    {
      widget->setFocus(true);
      if ( widget->numOfChildren() >= 1 )
        widget->focusFirstChild();
      return true;
    }
    // prefix increment (++) is faster
    // than postfix for non primitive type
    ++iter;
  }
  return false;
}

//----------------------------------------------------------------------
bool FWidget::focusLastChild (void)
{
  FObject::object_list children;
  FObject::object_list::const_iterator iter, begin;

  if ( ! this->hasChildren() )
    return false;

  children = this->children();
  iter  = children.end();
  begin = children.begin();

  do
  {
    --iter;
    FWidget* widget = static_cast<FWidget*>(*iter);
    if (  widget->isEnabled()
       && widget->acceptFocus()
       && ! widget->isWindow() )
    {
      widget->setFocus(true);
      if ( widget->numOfChildren() >= 1 )
        widget->focusLastChild();
      return true;
    }
  }
  while ( iter != begin );

  return false;
}

//----------------------------------------------------------------------
bool FWidget::setFocus(bool on)
{
  FWindow* window;

  if ( ! enable )
    return false;
  if ( on == focus )
    return true;

  if ( on && ! focus )
  {
    if ( FWidget::getFocusWidget() != 0 )
      FWidget::getFocusWidget()->unsetFocus();
    if ( numOfFocusableChildren() == 0 )
      FWidget::setFocusWidget(this);
  }

  window = FWindow::getWindowWidget(this);

  if ( on && window )
  {
    if ( ! window->isActiveWindow() )
    {
      bool has_raised = window->raiseWindow();
      FWindow::setActiveWindow(window);
      if ( has_raised )
        window->redraw();
    }
    window->setFocusWidget(this);
  }

  return focus = (on) ? true : false;
}

//----------------------------------------------------------------------
FPoint FWidget::globalToLocalPos (const FPoint& gPos)
{
  return FPoint ( gPos.getX() - xpos - xmin + 2,
                  gPos.getY() - ypos - ymin + 2 );
}

//----------------------------------------------------------------------
void FWidget::setForegroundColor (int color)
{
  if ( color >> 8 == 0 )  // valid colors 0..254
    foregroundColor = color;
}

//----------------------------------------------------------------------
void FWidget::setBackgroundColor (int color)
{
  if ( color >> 8 == 0 )  // valid colors 0..254
    backgroundColor = color;
}

//----------------------------------------------------------------------
void FWidget::setX (int x, bool adjust)
{
  if ( xpos == x && widgetSize.getX() == x )
    return;

  if ( ! isWindow() )
  {
    (x > 0) ? xpos = x : xpos = 1;
  }
  else
    xpos = x;

  widgetSize.setX(xpos);
  adjustWidgetSize.setX(xpos);
  adjustWidgetSizeShadow = adjustWidgetSize + shadow;
  adjustWidgetSizeGlobal.setX(xpos + xmin - 1);
  adjustWidgetSizeGlobalShadow = adjustWidgetSizeGlobal + shadow;

  if ( adjust )
    adjustSize();
}

//----------------------------------------------------------------------
void FWidget::setY (int y, bool adjust)
{
  if ( ypos == y && widgetSize.getY() == y )
    return;

  if ( ! isWindow() )
  {
    (y > 0) ? ypos = y : ypos = 1;
  }
  else
    ypos = y;

  widgetSize.setY(ypos);
  adjustWidgetSize.setY(ypos);
  adjustWidgetSizeShadow = adjustWidgetSize + shadow;
  adjustWidgetSizeGlobal.setY(ypos + ymin - 1);
  adjustWidgetSizeGlobalShadow = adjustWidgetSizeGlobal + shadow;

  if ( adjust )
    adjustSize();
}

//----------------------------------------------------------------------
void FWidget::setWidth (int w, bool adjust)
{
  if ( width == w && widgetSize.getWidth() == w  )
    return;

  (w > 0) ? width = w : width = 1;

  widgetSize.setWidth(width);
  adjustWidgetSize.setWidth(width);
  adjustWidgetSizeShadow = adjustWidgetSize + shadow;
  adjustWidgetSizeGlobal.setWidth(width);
  adjustWidgetSizeGlobalShadow = adjustWidgetSizeGlobal + shadow;

  if ( adjust )
    adjustSize();

  double_flatline_mask.top.resize (width, false);
  double_flatline_mask.bottom.resize (width, false);
}

//----------------------------------------------------------------------
void FWidget::setHeight (int h, bool adjust)
{
  if ( height == h && widgetSize.getHeight() == h )
    return;

  (h > 0) ? height = h : height = 1;

  widgetSize.setHeight(height);
  adjustWidgetSize.setHeight(height);
  adjustWidgetSizeShadow = adjustWidgetSize + shadow;
  adjustWidgetSizeGlobal.setHeight(height);
  adjustWidgetSizeGlobalShadow = adjustWidgetSizeGlobal + shadow;

  if ( adjust )
    adjustSize();

  double_flatline_mask.right.resize (height, false);
  double_flatline_mask.left.resize (height, false);
}

//----------------------------------------------------------------------
void FWidget::setTopPadding (int t, bool adjust)
{
  if ( top_padding == t )
    return;
  (t > 0) ? top_padding = t : top_padding = 1;
  if ( adjust )
    adjustSize();
}

//----------------------------------------------------------------------
void FWidget::setLeftPadding (int l, bool adjust)
{
  if ( left_padding == l )
    return;
  (l > 0) ? left_padding = l : left_padding = 1;
  if ( adjust )
    adjustSize();
}

//----------------------------------------------------------------------
void FWidget::setBottomPadding (int b, bool adjust)
{
  if ( bottom_padding == b )
    return;
  (b > 0) ? bottom_padding = b : bottom_padding = 1;
  if ( adjust )
    adjustSize();
}

//----------------------------------------------------------------------
void FWidget::setRightPadding (int r, bool adjust)
{
  if ( right_padding == r )
    return;
  (r > 0) ? right_padding = r : right_padding = 1;
  if ( adjust )
    adjustSize();
}

//----------------------------------------------------------------------
void FWidget::getTermGeometry()
{
  if ( openConsole() == 0 )
  {
    getTermSize();
    closeConsole();
  }

  rootObject->width  = term->getWidth();
  rootObject->height = term->getHeight();
  rootObject->xmin = 1;
  rootObject->ymin = 1;
  rootObject->xmax = rootObject->width;
  rootObject->ymax = rootObject->height;
  rootObject->client_xmin = 1;
  rootObject->client_ymin = 1;
  rootObject->client_xmax = rootObject->width;
  rootObject->client_ymax = rootObject->height;
}

//----------------------------------------------------------------------
void FWidget::setTermGeometry (int w, int h)
{
  // Set xterm size to w x h
  if ( xterm )
  {
    rootObject->xpos = 1;
    rootObject->ypos = 1;
    rootObject->width = w;  // columns
    rootObject->height = h; // lines

    setTermSize (w, h);
    getTermGeometry();
  }
}

//----------------------------------------------------------------------
void FWidget::setGeometry (const FRect& box, bool adjust)
{
  setGeometry ( box.getX(),
                box.getY(),
                box.getWidth(),
                box.getHeight(),
                adjust );
}

//----------------------------------------------------------------------
void FWidget::setGeometry (int x, int y, int w, int h, bool adjust)
{
  if ( xpos == x && ypos == y && width == w && height == h )
    return;

  if ( ! isWindow() )
  {
    (x > 0) ? xpos = x : xpos = 1;
    (y > 0) ? ypos = y : ypos = 1;
  }
  else
  {
    xpos = x;
    ypos = y;
  }
  (w > 0) ? width  = w : width  = 1;
  (h > 0) ? height = h : height = 1;

  client_xmin = xpos + xmin - 1 + left_padding;
  client_ymin = ypos + ymin - 1 + top_padding;
  client_xmax = xpos + xmin - 2 + width  - right_padding;
  client_ymax = ypos + ymin - 2 + height - bottom_padding;

  widgetSize.setRect(xpos, ypos, width, height);
  adjustWidgetSize.setRect(xpos, ypos, width, height);
  adjustWidgetSizeShadow = adjustWidgetSize + shadow;
  adjustWidgetSizeGlobal.setRect ( xpos + xmin - 1,
                                   ypos + ymin - 1,
                                   width, height );
  adjustWidgetSizeGlobalShadow = adjustWidgetSizeGlobal + shadow;

  if ( adjust )
    adjustSize();

  double_flatline_mask.top.resize (width, false);
  double_flatline_mask.right.resize (height, false);
  double_flatline_mask.bottom.resize (width, false);
  double_flatline_mask.left.resize (height, false);
}

//----------------------------------------------------------------------
void FWidget::move (const FPoint& pos)
{
  move( pos.getX(), pos.getY() );
}

//----------------------------------------------------------------------
void FWidget::move (int x, int y)
{
  if ( x == xpos && y == ypos )
    return;
  // Avoid to move widget completely outside the terminal
  if ( x+width < 1 || x > term->getWidth() || y < 1 || y > term->getHeight() )
    return;

  xpos = x;
  ypos = y;
  widgetSize.setX(x);
  widgetSize.setY(y);
  adjustWidgetSize.setX(x);
  adjustWidgetSize.setY(y);
  adjustWidgetSizeShadow = adjustWidgetSize + shadow;
  adjustWidgetSizeGlobal.setX(x + xmin - 1);
  adjustWidgetSizeGlobal.setY(y + ymin - 1);
  adjustWidgetSizeGlobalShadow = adjustWidgetSizeGlobal + shadow;
}

//----------------------------------------------------------------------
bool FWidget::setCursor()
{
  FPoint* wcursor = &widgetCursorPosition;

  if ( isCursorInside() )
  {
    setTermXY (wcursor->getX()-1, wcursor->getY()-1);
    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
bool FWidget::setCursorPos (const FPoint& pos)
{
  return setCursorPos (pos.getX(), pos.getY());
}

//----------------------------------------------------------------------
bool FWidget::setCursorPos (register int x, register int y)
{
  widgetCursorPosition.setPoint(x,y);

  if ( isCursorInside() )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
void FWidget::unsetCursorPos()
{
  widgetCursorPosition.setPoint(-1,-1);
}

//----------------------------------------------------------------------
void FWidget::gotoxy (const FPoint& pos)
{
  gotoxy (pos.getX(), pos.getY());
}

//----------------------------------------------------------------------
void FWidget::gotoxy (register int x, register int y)
{
  cursor->setPoint(x,y);
}

//----------------------------------------------------------------------
void FWidget::clrscr()
{
  term_area* area;
  FWindow*   area_widget;
  char_data  default_char;

  default_char.code      = ' ';
  default_char.fg_color  = uChar(fg_color);
  default_char.bg_color  = uChar(bg_color);
  default_char.bold      = bold;
  default_char.reverse   = reverse;
  default_char.underline = underline;

  area_widget = FWindow::getWindowWidget(this);

  if ( area_widget )
    area = area_widget->getVWin();
  else
    area = vdesktop;
  if ( ! area )
    return;

  if ( area->right_shadow == 0 )
  {
    int area_size = area->width * area->height;
    std::fill_n (area->text, area_size, default_char);
  }
  else
  {
    for (int y=0; y < area->height; y++)
    {
      int pos = y * (area->width + area->right_shadow);
      std::fill_n (&area->text[pos], area->width, default_char);
    }
  }

  for (int i=0; i < area->height; i++)
  {
    area->changes[i].xmin = 0;
    area->changes[i].xmax = uInt(area->width + area->right_shadow - 1);
  }
  putArea (xpos+xmin-1, ypos+ymin-1, area);
}

//----------------------------------------------------------------------
bool FWidget::setBold (register bool on)
{
  return (bold = on);
}

//----------------------------------------------------------------------
bool FWidget::setReverse (register bool on)
{
  return (reverse = on);
}

//----------------------------------------------------------------------
bool FWidget::setUnderline (register bool on)
{
  return (underline = on);
}

//----------------------------------------------------------------------
void FWidget::drawShadow()
{
  FTerm::char_data ch;
  int x1, x2, y1, y2;
  bool trans_shadow;

  trans_shadow = bool((flags & TRANS_SHADOW) != 0);

  if (  (Encoding == fc::VT100 && ! trans_shadow)
     || (Encoding == fc::ASCII && ! trans_shadow)
     || monochron )
  {
    clearShadow();
    return;
  }

  x1 = xpos+xmin-1;
  x2 = xpos+xmin-2+width;
  y1 = ypos+ymin-1;
  y2 = ypos+ymin-2+height;

  if ( trans_shadow )
  {
    // transparent shadow
    if ( x2 < xmax )
    {
      gotoxy (x2+1, y1);
      for (int x=1; x <= 2; x++)
      {
        ch = getCoveredCharacter (x2+x, y1, this);
        setColor (ch.fg_color, ch.bg_color);
        print (ch.code);
      }
      setColor (wc.shadow_bg, wc.shadow_fg);
      for (int i=1; i < height && y1+i <= ymax; i++)
      {
        gotoxy (x2+1, y1+i);
        for (int x=1; x <= 2; x++)
        {
          ch = getCoveredCharacter (x2+x, y1+i, this);
          if (  ch.code == fc::LowerHalfBlock
             || ch.code == fc::UpperHalfBlock
             || ch.code == fc::LeftHalfBlock
             || ch.code == fc::RightHalfBlock
             || ch.code == fc::FullBlock )
            print (' ');
          else
            print (ch.code);
        }
      }
    }
    if ( y2 < ymax )
    {
      gotoxy (x1, y2+1);
      for (int x=0; x <= 1; x++)
      {
        ch = getCoveredCharacter (x1+x, y2+1, this);
        setColor (ch.fg_color, ch.bg_color);
        print (ch.code);
      }
      setColor (wc.shadow_bg, wc.shadow_fg);
      for (int i=2; i < width+2 && x1+i <= xmax; i++)
      {
        ch = getCoveredCharacter (x1+i, y2+1, this);
        if (  ch.code == fc::LowerHalfBlock
           || ch.code == fc::UpperHalfBlock
           || ch.code == fc::LeftHalfBlock
           || ch.code == fc::RightHalfBlock
           || ch.code == fc::FullBlock )
          print (' ');
        else
          print (ch.code);
      }
    }
  }
  else
  {
    if ( x2 < xmax )
    {
      gotoxy (x2+1, y1);
      ch = getCoveredCharacter (x2+1, y1, this);
      setColor (wc.shadow_fg, ch.bg_color);
      print (fc::LowerHalfBlock); // ▄

      for (int i=1; i < height && y1+i <= ymax; i++)
      {
        gotoxy (x2+1, y1+i);
        print (fc::FullBlock); // █
      }
    }

    if ( y2 < ymax )
    {
      gotoxy (x1+1, y2+1);

      for (int i=1; i <= width && x1+i <= xmax; i++)
      {
        ch = getCoveredCharacter (x1+i, y2+1, this);
        setColor(wc.shadow_fg, ch.bg_color);
        print (fc::UpperHalfBlock); // ▀
      }
    }
  }
}

//----------------------------------------------------------------------
void FWidget::clearShadow()
{
  FTerm::char_data ch;
  int x1, x2, y1, y2;

  x1 = xpos+xmin-1;
  x2 = xpos+xmin-2+width;
  y1 = ypos+ymin-1;
  y2 = ypos+ymin-2+height;

  if ( x2 < xmax )
  {
    for (int i=0; i < height && y1+i <= ymax; i++)
    {
      gotoxy (x2+1, y1+i);
      ch = getCoveredCharacter (x2+1, y1+i, this);
      setColor (wc.shadow_fg, ch.bg_color);
      print  (' ');  // clear █
    }
  }
  if ( y2 < ymax )
  {
    gotoxy (x1+1, y2+1);
    for (int i=1; i <= width && x1+i <= xmax; i++)
    {
      ch = getCoveredCharacter (x1+i, y2+1, this);
      setColor (wc.shadow_fg, ch.bg_color);
      print (' '); // clear ▀
    }
  }
}

//----------------------------------------------------------------------
void FWidget::drawFlatBorder()
{
  int x1, x2, y1, y2;

  if ( ! isNewFont() )
    return;

  x1 = xpos+xmin-1;
  x2 = xpos+xmin-1+width;
  y1 = ypos+ymin-2;
  y2 = ypos+ymin-1+height;

  setColor (wc.dialog_fg, wc.dialog_bg);
  for (int y=0; y < height; y++)
  {
    gotoxy (x1-1, y1+y+1);
    if ( double_flatline_mask.left[y] )
      print (fc::NF_rev_border_line_right); // || is not yet defined
    else
      print (fc::NF_rev_border_line_right); // right line (on left side)
  }

  gotoxy (x2, y1+1);
  for (int y=0; y < height; y++)
  {
    if ( double_flatline_mask.right[y] )
      print (fc::NF_border_line_left); // || is not yet defined
    else
      print (fc::NF_border_line_left); // left line (on right side)
    gotoxy (x2, y1+y+2);
  }

  gotoxy (x1, y1);
  for (int x=0; x < width; x++)
  {
    if ( double_flatline_mask.top[x] )
      print (fc::NF_border_line_up_and_down); // top+bottom line (at top)
    else
      print (fc::NF_border_line_bottom); // bottom line (at top)
  }

  gotoxy (x1, y2);
  for (int x=0; x < width; x++)
  {
    if ( double_flatline_mask.bottom[x] )
      print (fc::NF_border_line_up_and_down); // top+bottom line (at bottom)
    else
      print (fc::NF_border_line_upper); // top line (at bottom)
  }
}

//----------------------------------------------------------------------
void FWidget::clearFlatBorder()
{
  int x1, x2, y1, y2;

  if ( ! isNewFont() )
    return;

  x1 = xpos+xmin-1;
  x2 = xpos+xmin-1+width;
  y1 = ypos+ymin-2;
  y2 = ypos+ymin-1+height;

  setColor (wc.dialog_fg, wc.dialog_bg);
  for (register int y=0; y <= height-1; y++)
  {
    gotoxy (x1-1, y1+y+1);
    print (' '); // clear on left side
  }

  gotoxy (x2, y1+1);
  for (register int y=1; y <= height; y++)
  {
    print (' ');  // clear on right side
    gotoxy (x2, y1+y+1);
  }

  gotoxy (x1, y1);
  for (register int x=0; x < width; x++)
    print (' '); // clear at bottom

  gotoxy (x1, y2);
  for (register int x=0; x < width; x++)
    print (' '); // clear at top
}

//----------------------------------------------------------------------
void FWidget::setDoubleFlatLine(int side, bool bit)
{
  int size;

  assert (  side == fc::top
         || side == fc::right
         || side == fc::bottom
         || side == fc::left );

  switch ( side )
  {
    case fc::top:
      size = double_flatline_mask.top.size();
      double_flatline_mask.top.assign(size, bit);
      break;

    case fc::right:
      size = double_flatline_mask.right.size();
      double_flatline_mask.right.assign(size, bit);
      break;

    case fc::bottom:
      size = double_flatline_mask.bottom.size();
      double_flatline_mask.bottom.assign(size, bit);
      break;

    case fc::left:
      size = double_flatline_mask.left.size();
      double_flatline_mask.left.assign(size, bit);
      break;
  }
}
//----------------------------------------------------------------------
std::vector<bool>& FWidget::doubleFlatLine_ref(int side)
{
  assert (  side == fc::top
         || side == fc::right
         || side == fc::bottom
         || side == fc::left );

  switch ( side )
  {
    case fc::top:
      return double_flatline_mask.top;

    case fc::right:
      return double_flatline_mask.right;

    case fc::bottom:
      return double_flatline_mask.bottom;

    case fc::left:
    default:
      return double_flatline_mask.left;
  }
}

//----------------------------------------------------------------------
void FWidget::drawBorder()
{
  int x1, x2, y1, y2;

  x1 = xpos+xmin-1;
  x2 = xpos+xmin-2+width;
  y1 = ypos+ymin-1;
  y2 = ypos+ymin-2+height;

  if ( x1 < xmin )
    x1 = xmin;
  if ( y1 < ymin )
    y1 = ymin;
  if ( x2 > xmax )
    x2 = xmax;
  if ( y2 > ymax )
    y2 = ymax;

  if ( isNewFont() )
  {
    setColor (wc.dialog_fg, wc.dialog_bg);
    gotoxy (x1, y1);
    print (fc::NF_border_corner_middle_upper_left); // ┌
    for (int x=x1+1; x < x2; x++)
      print (fc::BoxDrawingsHorizontal); // ─
    print (fc::NF_border_corner_middle_upper_right); // ┐
    for (int y=y1+1; y < y2+1; y++)
    {
      gotoxy (x1, y);
      print (fc::NF_border_line_left); // border left ⎸
      gotoxy (x2, y);
      print (fc::NF_rev_border_line_right); // border right⎹
    }
    gotoxy (x1, y2);
    print (fc::NF_border_corner_middle_lower_left); // └
    for (int x=x1+1; x < x2; x++)
      print (fc::BoxDrawingsHorizontal); // ─
    print (fc::NF_border_corner_middle_lower_right); // ┘
  }
  else
  {
    gotoxy (x1, y1);
    print (fc::BoxDrawingsDownAndRight); // ┌

    for (int x=x1+1; x < x2; x++)
      print (fc::BoxDrawingsHorizontal); // ─
    print (fc::BoxDrawingsDownAndLeft); // ┐
    for (int y=y1+1; y < y2; y++)
    {
      gotoxy (x1, y);
      print (fc::BoxDrawingsVertical); // │
      gotoxy (x2, y);
      print (fc::BoxDrawingsVertical); // │
    }
    gotoxy (x1, y2);
    print (fc::BoxDrawingsUpAndRight); // └
    for (int x=x1+1; x < x2; x++)
      print (fc::BoxDrawingsHorizontal); // ─
    print (fc::BoxDrawingsUpAndLeft); // ┘

    for (int x=x1+1; x < x2; x++)
    {
      gotoxy (x, y1);
      print (fc::BoxDrawingsHorizontal); // ─
      gotoxy (x, y2);
      print (fc::BoxDrawingsHorizontal); // ─
    }
  }
}

//----------------------------------------------------------------------
void FWidget::quit()
{
  FApplication* fapp = static_cast<FApplication*>(rootObject);
  fapp->exit(0);
}
