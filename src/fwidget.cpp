// File: fwidget.cpp
// Provides: class FWidget

#include "fapp.h"
#include "fmenubar.h"
#include "fstatusbar.h"
#include "fwidget.h"


// global FWidget object
static FWidget* rootObject = 0;

// static class attributes
uInt     FWidget::modal_dialogs;

FStatusBar*            FWidget::statusbar          = 0;
FMenuBar*              FWidget::menubar            = 0;
FWidget*               FWidget::show_root_widget   = 0;
FWidget*               FWidget::redraw_root_widget = 0;
FWidget::widgetList*   FWidget::window_list        = 0;
FWidget::widgetList*   FWidget::dialog_list        = 0;
FWidget::widgetList*   FWidget::always_on_top_list = 0;
FWidget::widgetList*   FWidget::close_widget       = 0;
FWidget::widget_colors FWidget::wc;

//----------------------------------------------------------------------
// class FWidget
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FWidget::FWidget (FWidget* parent)
  : FVTerm(parent)
  , callback_objects()
  , member_callback_objects()
  , accelerator_list(0)
  , flags(0)
  , enable(true)
  , visible(true)
  , shown(false)
  , focus(false)
  , focusable(true)
  , visible_cursor(false)
  , widget_cursor_position(-1,-1)
  , size_hints()
  , double_flatline_mask()
  , padding()
  , ignore_padding(false)
  , wsize(1,1,1,1)
  , adjust_wsize(1,1,1,1)
  , adjust_wsize_term()
  , adjust_wsize_shadow()
  , adjust_wsize_term_shadow()
  , offset()
  , client_offset()
  , wshadow(0,0)
  , foreground_color(fc::Default)
  , background_color(fc::Default)
  , statusbar_message()
{
  if ( ! parent )
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
    offset = parent->client_offset;
    double_flatline_mask.top.resize (uLong(getWidth()), false);
    double_flatline_mask.right.resize (uLong(getHeight()), false);
    double_flatline_mask.bottom.resize (uLong(getWidth()), false);
    double_flatline_mask.left.resize (uLong(getHeight()), false);
  }
}

//----------------------------------------------------------------------
FWidget::~FWidget()  // destructor
{
  processDestroy();
  FApplication::removeQueuedEvent(this);

  // unset clicked widget
  if ( this == getClickedWidget() )
    setClickedWidget(0);

  // unset the local window widget focus
  if ( focus )
  {
    if ( FWindow* window = FWindow::getWindowWidget(this) )
      window->setWindowFocusWidget(0);
  }

  // unset the global widget focus
  if ( this == FWidget::getFocusWidget() )
    FWidget::setFocusWidget(0);

  // unset main widget
  if ( this == getMainWidget() )
  {
    setMainWidget(0);
    quit();
  }

  // finish the program
  if ( rootObject == this )
    this->finish();
}

// private methods of FWidget
//----------------------------------------------------------------------
void FWidget::init()
{
  window_list        = new widgetList();
  dialog_list        = new widgetList();
  always_on_top_list = new widgetList();
  close_widget       = new widgetList();

  // determine width and height of the terminal
  detectTermSize();
  wsize.setRect(1, 1, getColumnNumber(), getLineNumber());
  adjust_wsize = wsize;
  offset.setRect(0, 0, getColumnNumber(), getLineNumber());
  client_offset = offset;

  double_flatline_mask.top.resize (uLong(getWidth()), false);
  double_flatline_mask.right.resize (uLong(getHeight()), false);
  double_flatline_mask.bottom.resize (uLong(getWidth()), false);
  double_flatline_mask.left.resize (uLong(getHeight()), false);

  // Initialize default widget colors
  setColorTheme();

  foreground_color = wc.term_fg;
  background_color = wc.term_bg;
  setColor();
  clearArea (vdesktop);

  accelerator_list = new Accelerators();
}

//----------------------------------------------------------------------
void FWidget::finish()
{
  delete accelerator_list;
  accelerator_list = 0;

  if ( close_widget )
  {
    delete close_widget;
    close_widget = 0;
  }

  if ( dialog_list )
  {
    delete dialog_list;
    dialog_list = 0;
  }

  if ( always_on_top_list )
  {
    delete always_on_top_list;
    always_on_top_list = 0;
  }

  if ( window_list )
  {
    delete window_list;
    window_list = 0;
  }
}

//----------------------------------------------------------------------
void FWidget::draw()
{ }

//----------------------------------------------------------------------
void FWidget::setColorTheme()
{
  wc.term_fg                           = fc::Black;
  wc.term_bg                           = fc::LightBlue;
  wc.list_fg                           = fc::Black;
  wc.list_bg                           = fc::White;
  wc.selected_list_fg                  = fc::Cyan;
  wc.selected_list_bg                  = fc::White;
  wc.dialog_fg                         = fc::Black;
  wc.dialog_resize_fg                  = fc::Cyan;
  wc.dialog_emphasis_fg                = fc::Blue;
  wc.dialog_bg                         = fc::White;
  wc.error_box_fg                      = fc::White;
  wc.error_box_emphasis_fg             = fc::Yellow;
  wc.error_box_bg                      = fc::LightRed;
  wc.tooltip_fg                        = fc::Black;
  wc.tooltip_bg                        = fc::Yellow;
  wc.shadow_fg                         = fc::Black;
  wc.shadow_bg                         = fc::LightGray; // only for transparent shadow
  wc.current_element_focus_fg          = fc::White;
  wc.current_element_focus_bg          = fc::Blue;
  wc.current_element_fg                = fc::LightGray;
  wc.current_element_bg                = fc::Blue;
  wc.current_inc_search_element_fg     = fc::LightRed;
  wc.selected_current_element_focus_fg = fc::LightCyan;
  wc.selected_current_element_focus_bg = fc::Blue;
  wc.selected_current_element_fg       = fc::LightBlue;
  wc.selected_current_element_bg       = fc::Blue;
  wc.label_fg                          = fc::Black;
  wc.label_bg                          = fc::White;
  wc.label_inactive_fg                 = fc::LightGray;
  wc.label_inactive_bg                 = fc::White;
  wc.label_hotkey_fg                   = fc::Red;
  wc.label_hotkey_bg                   = fc::White;
  wc.label_emphasis_fg                 = fc::Blue;
  wc.label_ellipsis_fg                 = fc::DarkGray;
  wc.inputfield_active_focus_fg        = fc::White;
  wc.inputfield_active_focus_bg        = fc::Cyan;
  wc.inputfield_active_fg              = fc::Black;
  wc.inputfield_active_bg              = fc::LightGray;
  wc.inputfield_inactive_fg            = fc::DarkGray;
  wc.inputfield_inactive_bg            = fc::LightGray;
  wc.toggle_button_active_focus_fg     = fc::White;
  wc.toggle_button_active_focus_bg     = fc::Cyan;
  wc.toggle_button_active_fg           = fc::Black;
  wc.toggle_button_active_bg           = fc::White;
  wc.toggle_button_inactive_fg         = fc::LightGray;
  wc.toggle_button_inactive_bg         = fc::White;
  wc.button_active_focus_fg            = fc::LightGray;
  wc.button_active_focus_bg            = fc::Blue;
  wc.button_active_fg                  = fc::LightGray;
  wc.button_active_bg                  = fc::DarkGray;
  wc.button_inactive_fg                = fc::DarkGray;
  wc.button_inactive_bg                = fc::LightGray;
  wc.button_hotkey_fg                  = fc::White;
  wc.titlebar_active_fg                = fc::White;
  wc.titlebar_active_bg                = fc::Blue;
  wc.titlebar_inactive_fg              = fc::LightGray;
  wc.titlebar_inactive_bg              = fc::DarkGray;
  wc.titlebar_button_fg                = fc::DarkGray;
  wc.titlebar_button_bg                = fc::LightGray;
  wc.titlebar_button_focus_fg          = fc::LightGray;
  wc.titlebar_button_focus_bg          = fc::Black;
  wc.menu_active_focus_fg              = fc::White;
  wc.menu_active_focus_bg              = fc::Blue;
  wc.menu_active_fg                    = fc::Black;
  wc.menu_active_bg                    = fc::White;
  wc.menu_inactive_fg                  = fc::LightGray;
  wc.menu_inactive_bg                  = fc::White;
  wc.menu_hotkey_fg                    = fc::Red;
  wc.menu_hotkey_bg                    = fc::White;
  wc.statusbar_fg                      = fc::White;
  wc.statusbar_bg                      = fc::Blue;
  wc.statusbar_hotkey_fg               = fc::LightRed;
  wc.statusbar_hotkey_bg               = fc::Blue;
  wc.statusbar_separator_fg            = fc::Black;
  wc.statusbar_active_fg               = fc::Blue;
  wc.statusbar_active_bg               = fc::White;
  wc.statusbar_active_hotkey_fg        = fc::DarkGray;
  wc.statusbar_active_hotkey_bg        = fc::White;
  wc.scrollbar_fg                      = fc::DarkGray;
  wc.scrollbar_bg                      = fc::LightBlue;
  wc.scrollbar_button_fg               = fc::Black;
  wc.scrollbar_button_bg               = fc::LightGray;
  wc.progressbar_fg                    = fc::DarkGray;
  wc.progressbar_bg                    = fc::LightBlue;

  if ( isKdeTerminal() )
    wc.term_bg = fc::SteelBlue3;

  if ( isTeraTerm() )
    wc.term_bg = fc::LightBlue;

  if ( getMaxColor() < 16 )  // for 8 color mode
  {
    wc.term_fg                           = fc::Black;
    wc.term_bg                           = fc::Blue;
    wc.list_fg                           = fc::Black;
    wc.list_bg                           = fc::LightGray;
    wc.selected_list_fg                  = fc::Blue;
    wc.selected_list_bg                  = fc::LightGray;
    wc.dialog_fg                         = fc::Black;
    wc.dialog_resize_fg                  = fc::Red;
    wc.dialog_emphasis_fg                = fc::Blue;
    wc.dialog_bg                         = fc::LightGray;
    wc.error_box_fg                      = fc::Black;
    wc.error_box_emphasis_fg             = fc::Red;
    wc.error_box_bg                      = fc::LightGray;
    wc.tooltip_fg                        = fc::LightGray;
    wc.tooltip_bg                        = fc::Cyan;
    wc.shadow_fg                         = fc::Black;
    wc.shadow_bg                         = fc::LightGray; // only for transparent shadow
    wc.current_element_focus_fg          = fc::LightGray;
    wc.current_element_focus_bg          = fc::Red;
    wc.current_element_fg                = fc::LightGray;
    wc.current_element_bg                = fc::Blue;
    wc.current_inc_search_element_fg     = fc::Brown;
    wc.selected_current_element_focus_fg = fc::Blue;
    wc.selected_current_element_focus_bg = fc::Red;
    wc.selected_current_element_fg       = fc::Cyan;
    wc.selected_current_element_bg       = fc::Blue;
    wc.label_fg                          = fc::Black;
    wc.label_bg                          = fc::LightGray;
    wc.label_inactive_fg                 = fc::Cyan;
    wc.label_inactive_bg                 = fc::LightGray;
    wc.label_hotkey_fg                   = fc::Red;
    wc.label_hotkey_bg                   = fc::LightGray;
    wc.label_emphasis_fg                 = fc::Blue;
    wc.label_ellipsis_fg                 = fc::Black;
    wc.inputfield_active_focus_fg        = fc::LightGray;
    wc.inputfield_active_focus_bg        = fc::Blue;
    wc.inputfield_active_fg              = fc::Black;
    wc.inputfield_active_bg              = fc::Cyan;
    wc.inputfield_inactive_fg            = fc::Black;
    wc.inputfield_inactive_bg            = fc::LightGray;
    wc.toggle_button_active_focus_fg     = fc::LightGray;
    wc.toggle_button_active_focus_bg     = fc::Red;
    wc.toggle_button_active_fg           = fc::Black;
    wc.toggle_button_active_bg           = fc::LightGray;
    wc.toggle_button_inactive_fg         = fc::Cyan;
    wc.toggle_button_inactive_bg         = fc::LightGray;
    wc.button_active_focus_fg            = fc::LightGray;
    wc.button_active_focus_bg            = fc::Red;
    wc.button_active_fg                  = fc::LightGray;
    wc.button_active_bg                  = fc::Blue;
    wc.button_inactive_fg                = fc::Black;
    wc.button_inactive_bg                = fc::Blue;
    wc.button_hotkey_fg                  = fc::LightGray;
    wc.titlebar_active_fg                = fc::LightGray;
    wc.titlebar_active_bg                = fc::Red;
    wc.titlebar_inactive_fg              = fc::Black;
    wc.titlebar_inactive_bg              = fc::LightGray;
    wc.titlebar_button_fg                = fc::Black;
    wc.titlebar_button_bg                = fc::LightGray;
    wc.titlebar_button_focus_fg          = fc::LightGray;
    wc.titlebar_button_focus_bg          = fc::Black;
    wc.menu_active_focus_fg              = fc::LightGray;
    wc.menu_active_focus_bg              = fc::Blue;
    wc.menu_active_fg                    = fc::Black;
    wc.menu_active_bg                    = fc::LightGray;
    wc.menu_inactive_fg                  = fc::Cyan;
    wc.menu_inactive_bg                  = fc::LightGray;
    wc.menu_hotkey_fg                    = fc::Red;
    wc.menu_hotkey_bg                    = fc::LightGray;
    wc.statusbar_fg                      = fc::Black;
    wc.statusbar_bg                      = fc::LightGray;
    wc.statusbar_hotkey_fg               = fc::Red;
    wc.statusbar_hotkey_bg               = fc::LightGray;
    wc.statusbar_separator_fg            = fc::Black;
    wc.statusbar_active_fg               = fc::LightGray;
    wc.statusbar_active_bg               = fc::Black;
    wc.statusbar_active_hotkey_fg        = fc::Red;
    wc.statusbar_active_hotkey_bg        = fc::Black;
    wc.scrollbar_fg                      = fc::Black;
    wc.scrollbar_bg                      = fc::LightGray;
    wc.scrollbar_button_fg               = fc::Black;
    wc.scrollbar_button_bg               = fc::LightGray;
    wc.progressbar_fg                    = fc::Blue;
    wc.progressbar_bg                    = fc::LightGray;
  }
}


// protected methods of FWidget
//----------------------------------------------------------------------
void FWidget::adjustSize()
{
  if ( ! isRootWidget() )
  {
    FWidget* p = getParentWidget();

    if ( isWindowWidget() )
    {
      if ( ignore_padding && ! isDialogWidget() )
        setTermOffset();
      else
        offset = rootObject->client_offset;
    }
    else if ( ignore_padding && p )
    {
      offset.setCoordinates ( p->getTermX() - 1
                            , p->getTermY() - 1
                            , p->getTermX() + p->getWidth() - 2
                            , p->getTermY() + p->getHeight() - 2 );
    }
    else if ( p )
      offset = p->client_offset;

    adjust_wsize = wsize;
  }

  if ( ! isWindowWidget() )
  {
    // move left if not enough space
    while ( getTermX()+getWidth()-padding.right > offset.getX2()+2 )
    {
      adjust_wsize.x1_ref()--;
      adjust_wsize.x2_ref()--;

      if ( adjust_wsize.x1_ref() < 1 )
        adjust_wsize.x1_ref() = 1;
    }

    // move up if not enough space
    while ( getTermY()+getHeight()-padding.bottom > offset.getY2()+2 )
    {
      adjust_wsize.y1_ref()--;
      adjust_wsize.y2_ref()--;

      if ( adjust_wsize.y1_ref() < 1 )
        adjust_wsize.y1_ref() = 1;
    }

    // reduce the width if not enough space
    while ( offset.getX1()+getWidth()-1 > offset.getX2() )
      adjust_wsize.x2_ref()--;

    if ( getWidth() < size_hints.min_width )
      adjust_wsize.setWidth(size_hints.min_width);

    if ( getWidth() <= 0 )
      adjust_wsize.setWidth(1);

    // reduce the height if not enough space
    while ( offset.getY1()+getHeight()-1 > offset.getY2() )
      adjust_wsize.y2_ref()--;

    if ( getHeight() < size_hints.min_height )
      adjust_wsize.setWidth(size_hints.min_height);

    if ( getHeight() <= 0 )
      adjust_wsize.setHeight(1);
  }

  client_offset.setCoordinates
  (
    getTermX() - 1 + padding.left,
    getTermY() - 1 + padding.top,
    getTermX() - 2 + getWidth() - padding.right,
    getTermY() - 2 + getHeight() - padding.bottom
  );

  if ( this->hasChildren() )
  {
    FObject::object_list children;
    FObject::object_list::const_iterator iter, end;

    children = this->getChildren();
    iter = children.begin();
    end  = children.end();

    while ( iter != end )
    {
      FWidget* widget = static_cast<FWidget*>(*iter);

      if ( ! widget->isWindowWidget() )
        widget->adjustSize();

      ++iter;
    }
  }
}

//----------------------------------------------------------------------
void FWidget::adjustSizeGlobal()
{
  if ( ! isRootWidget() )
  {
    getRootWidget()->adjustSizeGlobal();
    return;
  }

  if ( window_list && ! window_list->empty() )
  {
    widgetList::const_iterator iter, end;
    iter = window_list->begin();
    end  = window_list->end();

    while ( iter != end )
    {
      (*iter)->adjustSize();
      ++iter;
    }
  }
}

//----------------------------------------------------------------------
void FWidget::setStatusBar (FStatusBar* sbar)
{
  if ( ! sbar || statusbar == sbar )
    return;

  if ( statusbar && sbar != 0 )
    delete statusbar;

  statusbar = sbar;
}

//----------------------------------------------------------------------
void FWidget::setMenuBar (FMenuBar* mbar)
{
  if ( ! mbar || menubar == mbar )
    return;

  if ( menubar )
    delete menubar;

  menubar = mbar;
}

//----------------------------------------------------------------------
bool FWidget::event (FEvent* ev)
{
  switch ( ev->type() )
  {
    case fc::KeyPress_Event:
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

          widget = widget->getParentWidget();
        }
      }
      break;

    case fc::KeyUp_Event:
      onKeyUp ( static_cast<FKeyEvent*>(ev) );
      break;

    case fc::KeyDown_Event:
      {
        FKeyEvent* kev = static_cast<FKeyEvent*>(ev);
        FWidget* widget = this;

        while ( widget )
        {
          widget->onKeyDown(kev);

          if ( kev->isAccepted() || widget->isRootWidget() )
            break;

          widget = widget->getParentWidget();
        }
      }
      break;

    case fc::MouseDown_Event:
      onMouseDown ( static_cast<FMouseEvent*>(ev) );
      break;

    case fc::MouseUp_Event:
      onMouseUp ( static_cast<FMouseEvent*>(ev) );
      break;

    case fc::MouseDoubleClick_Event:
      onMouseDoubleClick ( static_cast<FMouseEvent*>(ev) );
      break;

    case fc::MouseWheel_Event:
      onWheel ( static_cast<FWheelEvent*>(ev) );
      break;

    case fc::MouseMove_Event:
      onMouseMove ( static_cast<FMouseEvent*>(ev) );
      break;

    case fc::FocusIn_Event:
      onFocusIn ( static_cast<FFocusEvent*>(ev) );
      break;

    case fc::FocusOut_Event:
      onFocusOut ( static_cast<FFocusEvent*>(ev) );
      break;

    case fc::Accelerator_Event:
      onAccel ( static_cast<FAccelEvent*>(ev) );
      break;

    case fc::Resize_Event:
      onResize ( static_cast<FResizeEvent*>(ev) );
      break;

    case fc::Show_Event:
      onShow ( static_cast<FShowEvent*>(ev) );
      break;

    case fc::Hide_Event:
      onHide ( static_cast<FHideEvent*>(ev) );
      break;

    case fc::Close_Event:
      onClose ( static_cast<FCloseEvent*>(ev) );
      break;

    case fc::Timer_Event:
      onTimer ( static_cast<FTimerEvent*>(ev) );
      break;

    default:
      return false;
  }
  return true;
}

//----------------------------------------------------------------------
void FWidget::onKeyPress (FKeyEvent*)
{ }

//----------------------------------------------------------------------
void FWidget::onKeyUp (FKeyEvent*)
{ }

//----------------------------------------------------------------------
void FWidget::onKeyDown (FKeyEvent*)
{ }

//----------------------------------------------------------------------
void FWidget::onMouseDown (FMouseEvent*)
{ }

//----------------------------------------------------------------------
void FWidget::onMouseUp (FMouseEvent*)
{ }

//----------------------------------------------------------------------
void FWidget::onMouseDoubleClick (FMouseEvent*)
{ }

//----------------------------------------------------------------------
void FWidget::onWheel (FWheelEvent*)
{ }

//----------------------------------------------------------------------
void FWidget::onMouseMove (FMouseEvent*)
{ }

//----------------------------------------------------------------------
void FWidget::onFocusIn (FFocusEvent*)
{ }

//----------------------------------------------------------------------
void FWidget::onFocusOut (FFocusEvent*)
{ }

//----------------------------------------------------------------------
void FWidget::onAccel (FAccelEvent*)
{ }

//----------------------------------------------------------------------
void FWidget::onResize (FResizeEvent* ev)
{
  rootObject->resize();
  rootObject->redraw();
  ev->accept();
}

//----------------------------------------------------------------------
void FWidget::onShow (FShowEvent*)
{ }

//----------------------------------------------------------------------
void FWidget::onHide (FHideEvent*)
{ }

//----------------------------------------------------------------------
void FWidget::onClose (FCloseEvent* ev)
{
  ev->accept();
}

//----------------------------------------------------------------------
bool FWidget::focusNextChild()
{
  if ( isDialogWidget() )
    return false;

  if ( hasParent() )
  {
    FWidget* parent = static_cast<FWidget*>(getParent());

    if ( parent->hasChildren() && parent->numOfFocusableChildren() > 1 )
    {
      FObject::object_list children;
      FObject::object_list::iterator iter, end;

      children = parent->getChildren();
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
                  || next->isWindowWidget() );

          FFocusEvent out (fc::FocusOut_Event);
          out.setFocusType(fc::FocusNextWidget);
          FApplication::sendEvent(this, &out);

          if ( out.isAccepted() )
          {
            if ( next == this )
              return false;

            next->setFocus();
            FFocusEvent in (fc::FocusIn_Event);
            in.setFocusType(fc::FocusNextWidget);
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
bool FWidget::focusPrevChild()
{
  if ( isDialogWidget() )
    return false;

  if ( hasParent() )
  {
    FWidget* parent = static_cast<FWidget*>(getParent());

    if ( parent->hasChildren() && parent->numOfFocusableChildren() > 1 )
    {
      FObject::object_list children;
      FObject::object_list::iterator iter, begin;

      children = parent->getChildren();
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
                  || prev->isWindowWidget() );

          FFocusEvent out (fc::FocusOut_Event);
          out.setFocusType(fc::FocusPreviousWidget);
          FApplication::sendEvent(this, &out);

          if ( out.isAccepted() )
          {
            if ( prev == this )
              return false;

            prev->setFocus();
            FFocusEvent in (fc::FocusIn_Event);
            in.setFocusType(fc::FocusPreviousWidget);
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
      }
      while ( iter != begin );
    }
  }

  return true;
}


// public methods of FWidget
//----------------------------------------------------------------------
FWidget* FWidget::getRootWidget() const
{
  FWidget* obj = const_cast<FWidget*>(this);
  FWidget* p_obj = getParentWidget();

  while ( ! obj->isRootWidget() && p_obj )
  {
    obj = p_obj;
    p_obj = p_obj->getParentWidget();
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
void FWidget::setMainWidget (FWidget* obj)
{
  FApplication* fapp = static_cast<FApplication*>(rootObject);
  fapp->setMainWidget(obj);
}

//----------------------------------------------------------------------
FWidget* FWidget::childWidgetAt (FWidget* p, int x, int y)
{
  if ( p && p->hasChildren() )
  {
    FObject::object_list children;
    FObject::object_list::const_iterator iter, end;

    children = p->getChildren();
    iter = children.begin();
    end  = children.end();

    while ( iter != end )
    {
      FWidget* widget = static_cast<FWidget*>(*iter);

      if (  widget->isEnabled()
         && widget->isVisible()
         && ! widget->isWindowWidget()
         && widget->getTermGeometry().contains(x,y) )
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
void FWidget::setFocusWidget (FWidget* obj)
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
void FWidget::setClickedWidget (FWidget* obj)
{
  FApplication::clicked_widget = obj;
}

//----------------------------------------------------------------------
FWidget* FWidget::getMoveSizeWidget()
{
  return FApplication::move_size_widget;
}

//----------------------------------------------------------------------
void FWidget::setMoveSizeWidget (FWidget* obj)
{
  FApplication::move_size_widget = obj;
}

//----------------------------------------------------------------------
FWidget* FWidget::getOpenMenu()
{
  FWidget* open_menu = static_cast<FWidget*>(FApplication::open_menu);
  return open_menu;
}

//----------------------------------------------------------------------
void FWidget::setOpenMenu (FWidget* obj)
{
  FApplication::open_menu = obj;
}

//----------------------------------------------------------------------
int FWidget::numOfFocusableChildren()
{
  FObject::object_list children;
  FObject::object_list::const_iterator iter, end;

  if ( ! this->hasChildren() )
    return 0;

  int num = 0;
  children = this->getChildren();
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
  FCloseEvent ev(fc::Close_Event);
  FApplication::sendEvent(this, &ev);

  if ( ev.isAccepted() )
  {
    if ( this == getMainWidget() )
      quit();
    else
    {
      hide();

      if ( (flags & fc::modal) == 0 )
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
FMenuBar* FWidget::menuBar()
{
  if ( menubar )
    return menubar;
  else
    return 0;
}

//----------------------------------------------------------------------
void FWidget::setStatusbarMessage (FString msg)
{
  statusbar_message = msg;
}

//----------------------------------------------------------------------
void FWidget::addCallback ( FString cb_signal
                          , FWidget::FCallback cb_handler
                          , data_ptr data )
{
  // add a (normal) function pointer as callback
  callback_data obj = { cb_signal, cb_handler, data };
  callback_objects.push_back(obj);
}

//----------------------------------------------------------------------
void FWidget::addCallback ( FString cb_signal
                          , FWidget* cb_instance
                          , FWidget::FMemberCallback cb_handler
                          , data_ptr data )
{
  // add a member function pointer as callback
  member_callback_data obj = { cb_signal, cb_instance, cb_handler, data };
  member_callback_objects.push_back(obj);
}

//----------------------------------------------------------------------
void FWidget::delCallback (FWidget::FCallback cb_handler)
{
  FWidget::CallbackObjects::iterator iter;

  // delete a cb_handler function pointer
  if ( callback_objects.empty() )
    return;

  iter = callback_objects.begin();

  while ( iter != callback_objects.end() )
  {
    if ( iter->cb_handler == cb_handler )
      iter = callback_objects.erase(iter);
    else
      ++iter;
  }
}

//----------------------------------------------------------------------
void FWidget::delCallback (FWidget* cb_instance)
{
  FWidget::MemberCallbackObjects::iterator iter;

  // delete all member function pointer from cb_instance
  if ( member_callback_objects.empty() )
    return;

  iter = member_callback_objects.begin();

  while ( iter != member_callback_objects.end() )
  {
    if ( iter->cb_instance == cb_instance )
      iter = member_callback_objects.erase(iter);
    else
      ++iter;
  }
}

//----------------------------------------------------------------------
inline void FWidget::delCallbacks()
{
  // delete all callbacks from this widget
  member_callback_objects.clear();  // member function pointer
  callback_objects.clear();        // function pointer
}

//----------------------------------------------------------------------
void FWidget::emitCallback (FString emit_signal)
{
  // member function pointer
  if ( ! member_callback_objects.empty() )
  {
    FWidget::MemberCallbackObjects::const_iterator m_iter, m_end;
    m_iter = member_callback_objects.begin();
    m_end = member_callback_objects.end();

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
  if ( ! callback_objects.empty() )
  {
    FWidget::CallbackObjects::const_iterator iter, end;
    iter = callback_objects.begin();
    end = callback_objects.end();

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
void FWidget::addAccelerator (int key, FWidget* obj)
{
  FWidget* widget = FWindow::getWindowWidget(obj);
  accelerator accel = { key, obj };

  if ( ! widget || widget == statusbar || widget == menubar )
    widget = getRootWidget();

  if ( widget && widget->accelerator_list )
    widget->accelerator_list->push_back(accel);
}

//----------------------------------------------------------------------
void FWidget::delAccelerator (FWidget* obj)
{
  FWidget* widget = FWindow::getWindowWidget(this);

  if ( ! widget || widget == statusbar || widget == menubar )
    widget = getRootWidget();

  if (  widget
     && widget->accelerator_list
     && ! widget->accelerator_list->empty() )
  {
    FWidget::Accelerators::iterator iter;
    iter = widget->accelerator_list->begin();

    while ( iter != widget->accelerator_list->end() )
    {
      if ( iter->object == obj )
        iter = widget->accelerator_list->erase(iter);
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
    startTerminalUpdate();
    // clean desktop
    setColor (wc.term_fg, wc.term_bg);
    clearArea (vdesktop);
  }
  else if ( ! visible )
    return;

  draw();

  if ( isRootWidget() )
  {
    // draw windows
    FOptiAttr::char_data default_char;
    default_char.code          = ' ';
    default_char.fg_color      = fc::Black;
    default_char.bg_color      = fc::Black;
    default_char.bold          = 0;
    default_char.dim           = 0;
    default_char.italic        = 0;
    default_char.underline     = 0;
    default_char.blink         = 0;
    default_char.reverse       = 0;
    default_char.standout      = 0;
    default_char.invisible     = 0;
    default_char.protect       = 0;
    default_char.crossed_out   = 0;
    default_char.dbl_underline = 0;
    default_char.alt_charset   = 0;
    default_char.pc_charset    = 0;
    default_char.transparent   = 0;
    default_char.trans_shadow  = 0;
    default_char.inherit_bg    = 0;

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
  }
  else
  {
    // draw child elements
    if ( this->hasChildren() )
    {
      FObject::object_list children;
      FObject::object_list::const_iterator iter, end;

      children = this->getChildren();
      iter = children.begin();
      end  = children.end();

      while ( iter != end )
      {
        FWidget* widget = static_cast<FWidget*>(*iter);

        if ( widget->isVisible() && ! widget->isWindowWidget() )
          widget->redraw();

        ++iter;
      }
    }
  }

  if ( isRootWidget() )
    finishTerminalUpdate();

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
  if ( isRootWidget() )
  {
    detectTermSize();
    FRect term_geometry = getTermGeometry();
    term_geometry.move(-1,-1);

    resizeVTerm (term_geometry);
    resizeArea (term_geometry, getShadow(), vdesktop);
    adjustSizeGlobal();
  }
  else
    adjustSize();

  // resize the four double-flatline-masks
  double_flatline_mask.top.resize (uLong(getWidth()), false);
  double_flatline_mask.right.resize (uLong(getHeight()), false);
  double_flatline_mask.bottom.resize (uLong(getWidth()), false);
  double_flatline_mask.left.resize (uLong(getHeight()), false);
}

//----------------------------------------------------------------------
void FWidget::show()
{
  if ( ! visible )
    return;

  // Important: Do not use setNewFont() or setVGAFont() after
  //            the console character mapping has been initialized
  if ( getMainWidget() == this )
    init_consoleCharMap();

  if ( ! show_root_widget )
  {
    startTerminalUpdate();
    show_root_widget = this;
  }

  draw();
  shown = true;

  if ( this->hasChildren() )
  {
    FObject::object_list children;
    FObject::object_list::const_iterator iter, end;

    children = this->getChildren();
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
    finishTerminalUpdate();
    updateTerminal();
    flush_out();
    show_root_widget = 0;
  }

  FShowEvent show_ev (fc::Show_Event);
  FApplication::sendEvent(this, &show_ev);
}

//----------------------------------------------------------------------
void FWidget::hide()
{
  if ( visible )
  {
    visible = false;
    shown = false;

    if ( ! isDialogWidget()
       && FWidget::getFocusWidget() == this
       && ! focusPrevChild() )
    {
      if ( FWidget::getFocusWidget() )
        FWidget::getFocusWidget()->unsetFocus();

      FWidget::setFocusWidget(getParentWidget());
    }

    FHideEvent hide_ev (fc::Hide_Event);
    FApplication::sendEvent(this, &hide_ev);
  }
}

//----------------------------------------------------------------------
bool FWidget::setEnable (bool on)
{
  return enable = (on) ? true : false;
}

//----------------------------------------------------------------------
bool FWidget::setEnable()
{
  return setEnable(true);
}

//----------------------------------------------------------------------
bool FWidget::unsetEnable()
{
  return setEnable(false);
}

//----------------------------------------------------------------------
bool FWidget::setDisable()
{
  return setEnable(false);
}

//----------------------------------------------------------------------
bool FWidget::focusFirstChild()
{
  FObject::object_list children;
  FObject::object_list::const_iterator iter, end;

  if ( ! this->hasChildren() )
    return false;

  children = this->getChildren();
  iter = children.begin();
  end  = children.end();

  while ( iter != end )
  {
    FWidget* widget = static_cast<FWidget*>(*iter);

    if (  widget->isEnabled()
       && widget->acceptFocus()
       && ! widget->isMenuWidget() )
    {
      widget->setFocus();

      if ( widget->numOfChildren() >= 1 )
      {
        if ( ! widget->focusFirstChild() && widget->isWindowWidget() )
        {
          ++iter;
          continue;
        }
      }

      return true;
    }

    // prefix increment (++) is faster
    // than postfix for non primitive type
    ++iter;
  }
  return false;
}

//----------------------------------------------------------------------
bool FWidget::focusLastChild()
{
  FObject::object_list children;
  FObject::object_list::const_iterator iter, begin;

  if ( ! this->hasChildren() )
    return false;

  children = this->getChildren();
  iter  = children.end();
  begin = children.begin();

  do
  {
    --iter;
    FWidget* widget = static_cast<FWidget*>(*iter);

    if (  widget->isEnabled()
       && widget->acceptFocus()
       && ! widget->isMenuWidget() )
    {
      widget->setFocus();

      if ( widget->numOfChildren() >= 1 )
      {
        if ( ! widget->focusLastChild() && widget->isWindowWidget() )
          continue;
      }

      return true;
    }
  }
  while ( iter != begin );

  return false;
}

//----------------------------------------------------------------------
bool FWidget::setFocus (bool on)
{
  FWindow* window;

  if ( ! enable )
    return false;

  if ( on == focus )
    return true;

  // set widget focus
  if ( on && ! focus )
  {
    int focusable_children = numOfFocusableChildren();

    if ( FWidget::getFocusWidget() )
      FWidget::getFocusWidget()->unsetFocus();

    if ( (!isDialogWidget() && focusable_children == 0)
       || (isDialogWidget() && focusable_children == 1) )
    {
      FWidget::setFocusWidget(this);
    }
  }

  window = FWindow::getWindowWidget(this);

  // set window focus
  if ( on && window )
  {
    if ( ! window->isWindowActive() )
    {
      bool has_raised = window->raiseWindow();
      FWindow::setActiveWindow(window);

      if ( has_raised && window->isVisible() && window->isShown() )
        window->redraw();
    }
    window->setWindowFocusWidget(this);
  }

  return focus = (on) ? true : false;
}

//----------------------------------------------------------------------
void FWidget::setColor ()
{
  // Changes colors to the widget default colors
  setColor (foreground_color, background_color);
}

//----------------------------------------------------------------------
void FWidget::setX (int x, bool adjust)
{
  if ( getX() == x && wsize.getX() == x )
    return;

  if ( ! isWindowWidget() && x < 1 )
    x = 1;

  wsize.setX(x);
  adjust_wsize.setX(x);

  if ( adjust )
    adjustSize();
}

//----------------------------------------------------------------------
void FWidget::setY (int y, bool adjust)
{
  if ( getY() == y && wsize.getY() == y )
    return;

  if ( ! isWindowWidget() && y < 1 )
    y = 1;

  wsize.setY(y);
  adjust_wsize.setY(y);

  if ( adjust )
    adjustSize();
}

//----------------------------------------------------------------------
void FWidget::setPos (int x, int y, bool adjust)
{
  if (  getX() == x && wsize.getX() == x
     && getY() == y && wsize.getY() == y )
  {
    return;
  }

  if ( ! isWindowWidget() )
  {
    if ( x < 1 )
      x = 1;

    if ( y < 1 )
      y = 1;
  }

  wsize.setPos(x,y);
  adjust_wsize.setPos(x,y);

  if ( adjust )
    adjustSize();
}

//----------------------------------------------------------------------
void FWidget::setWidth (int width, bool adjust)
{
  width = std::min (width, size_hints.max_width);
  width = std::max (width, size_hints.min_width);

  if ( getWidth() == width && wsize.getWidth() == width  )
    return;

  if ( width < 1 )
    width = 1;

  wsize.setWidth(width);
  adjust_wsize.setWidth(width);

  if ( adjust )
    adjustSize();

  double_flatline_mask.top.resize (uLong(getWidth()), false);
  double_flatline_mask.bottom.resize (uLong(getWidth()), false);
}

//----------------------------------------------------------------------
void FWidget::setHeight (int height, bool adjust)
{
  height = std::min (height, size_hints.max_height);
  height = std::max (height, size_hints.min_height);

  if ( getHeight() == height && wsize.getHeight() == height )
    return;

  if ( height < 1 )
    height = 1;

  wsize.setHeight(height);
  adjust_wsize.setHeight(height);

  if ( adjust )
    adjustSize();

  double_flatline_mask.right.resize (uLong(getHeight()), false);
  double_flatline_mask.left.resize (uLong(getHeight()), false);
}

//----------------------------------------------------------------------
void FWidget::setSize (int width, int height, bool adjust)
{
  width  = std::min (width,  size_hints.max_width);
  width  = std::max (width,  size_hints.min_width);
  height = std::min (height, size_hints.max_height);
  height = std::max (height, size_hints.min_height);

  if ( getWidth() == width && wsize.getWidth() == width
     && getHeight() == height && wsize.getHeight() == height )
    return;

  if ( width < 1 )
    width = 1;

  if ( height < 1 )
    height = 1;

  wsize.setWidth(width);
  wsize.setHeight(height);
  adjust_wsize.setWidth(width);
  adjust_wsize.setHeight(height);

  if ( adjust )
    adjustSize();

  double_flatline_mask.top.resize (uLong(getWidth()), false);
  double_flatline_mask.right.resize (uLong(getHeight()), false);
  double_flatline_mask.bottom.resize (uLong(getWidth()), false);
  double_flatline_mask.left.resize (uLong(getHeight()), false);
}

//----------------------------------------------------------------------
void FWidget::setTopPadding (int top, bool adjust)
{
  if ( padding.top == top )
    return;

  (top < 0) ? padding.top = 0 : padding.top = top;

  if ( adjust )
  {
    if ( isRootWidget() )
    {
      FWidget* r = rootObject;
      r->client_offset.setY1 (r->padding.top);
      adjustSizeGlobal();
    }
    else
      adjustSize();
  }
}

//----------------------------------------------------------------------
void FWidget::setLeftPadding (int left, bool adjust)
{
  if ( padding.left == left )
    return;

  (left < 0) ? padding.left = 0 : padding.left = left;

  if ( adjust )
  {
    if ( isRootWidget() )
    {
      FWidget* r = rootObject;
      r->client_offset.setX1 (r->padding.left);
      adjustSizeGlobal();
    }
    else
      adjustSize();
  }
}

//----------------------------------------------------------------------
void FWidget::setBottomPadding (int bottom, bool adjust)
{
  if ( padding.bottom == bottom )
    return;

  (bottom < 0) ? padding.bottom = 0 : padding.bottom = bottom;

  if ( adjust )
  {
    if ( isRootWidget() )
    {
      FWidget* r = rootObject;
      r->client_offset.setY2 (r->getHeight() - 1 - r->padding.bottom);
      adjustSizeGlobal();
    }
    else
      adjustSize();
  }
}

//----------------------------------------------------------------------
void FWidget::setRightPadding (int right, bool adjust)
{
  if ( padding.right == right )
    return;

  (right < 0) ? padding.right = 0 : padding.right = right;

  if ( adjust )
  {
    if ( isRootWidget() )
    {
      FWidget* r = rootObject;
      r->client_offset.setX2  (r->getWidth() - 1 - r->padding.right);
      adjustSizeGlobal();
    }
    else
      adjustSize();
  }
}

//----------------------------------------------------------------------
void FWidget::setParentOffset()
{
  FWidget* p = getParentWidget();

  if ( p )
    offset = p->client_offset;
}

//----------------------------------------------------------------------
void FWidget::setTermOffset()
{
  FWidget* r = getRootWidget();
  int w = r->getWidth();
  int h = r->getHeight();
  offset.setCoordinates (0, 0, w - 1, h - 1);
}

//----------------------------------------------------------------------
void FWidget::setTermOffsetWithPadding()
{
  FWidget* r = getRootWidget();
  offset.setCoordinates ( r->getLeftPadding()
                        , r->getTopPadding()
                        , r->getWidth() - 1 - r->getRightPadding()
                        , r->getHeight() - 1  - r->getBottomPadding() );
}

//----------------------------------------------------------------------
void FWidget::detectTermSize()
{
  FWidget* r = rootObject;
  FTerm::detectTermSize();
  r->adjust_wsize.setRect (1, 1, getColumnNumber(), getLineNumber());
  r->offset.setRect (0, 0, getColumnNumber(), getLineNumber());
  r->client_offset.setCoordinates
  (
    r->padding.left,
    r->padding.top,
    getColumnNumber() - 1 - r->padding.right,
    getLineNumber() - 1 - r->padding.bottom
  );
}

//----------------------------------------------------------------------
void FWidget::setTermSize (int w, int h)
{
  // Set xterm size to w x h
  if ( isXTerminal() )
  {
    rootObject->wsize.setRect(1, 1, w, h);
    rootObject->adjust_wsize = rootObject->wsize;
    FTerm::setTermSize (w, h);  // w = columns / h = lines
    detectTermSize();
  }
}

//----------------------------------------------------------------------
void FWidget::setGeometry (int x, int y, int w, int h, bool adjust)
{
  int term_x, term_y;

  w = std::min (w, size_hints.max_width);
  w = std::max (w, size_hints.min_width);
  h = std::min (h, size_hints.max_height);
  h = std::max (h, size_hints.min_height);

  if ( getX() == x && getY() == y && getWidth() == w && getHeight() == h )
    return;

  if ( ! isWindowWidget() )
  {
    (x < 1) ? wsize.setX(1) : wsize.setX(x);
    (y < 1) ? wsize.setY(1) : wsize.setY(y);
  }
  else
  {
    wsize.setX(x);
    wsize.setY(y);
  }

  (w < 1) ? wsize.setWidth(1) : wsize.setWidth(w);
  (h < 1) ? wsize.setHeight(1) : wsize.setHeight(h);

  adjust_wsize = wsize;
  term_x = getTermX();
  term_y = getTermY();

  client_offset.setCoordinates ( term_x - 1 + padding.left
                               , term_y - 1 + padding.top
                               , term_x - 2 + getWidth() - padding.right
                               , term_y - 2 + getHeight() - padding.bottom );

  double_flatline_mask.top.resize (uLong(getWidth()), false);
  double_flatline_mask.right.resize (uLong(getHeight()), false);
  double_flatline_mask.bottom.resize (uLong(getWidth()), false);
  double_flatline_mask.left.resize (uLong(getHeight()), false);

  if ( adjust )
    adjustSize();
}

//----------------------------------------------------------------------
void FWidget::move (int dx, int dy)
{
  wsize.move(dx,dy);
  adjust_wsize.move(dx,dy);
}

//----------------------------------------------------------------------
bool FWidget::setCursorPos (register int x, register int y)
{
  // sets the input cursor position
  widget_cursor_position.setPoint(x,y);

  if ( (flags & fc::focus) != 0 && ! isWindowWidget() )
  {
    FWidget* window = FWindow::getWindowWidget(this);

    if ( window )
    {
      if ( term_area* area = window->getVWin() )
      {
        setAreaCursor ( getTermX() - window->getTermX() + x
                      , getTermY() - window->getTermY() + y
                      , visible_cursor
                      , area );
        return true;
      }
    }
  }

  return false;
}

//----------------------------------------------------------------------
void FWidget::setPrintPos (register int x, register int y)
{
  setPrintCursor ( offset.getX1() + getX() + x - 1,
                   offset.getY1() + getY() + y - 1 );
}

//----------------------------------------------------------------------
FPoint FWidget::getPrintPos()
{
  const FPoint cur = getPrintCursor();
  int cx = cur.getX();
  int cy = cur.getY();
  return FPoint ( cx - offset.getX1() - getX() + 1
                , cy - offset.getY1() - getY() + 1 );

}

//----------------------------------------------------------------------
void FWidget::drawShadow()
{
  int x1, x2, y1, y2;
  bool trans_shadow = ((flags & fc::trans_shadow) != 0);

  if ( isMonochron() && ! trans_shadow )
    return;

  if (  (Encoding == fc::VT100 && ! (trans_shadow || isTeraTerm()) )
     || (Encoding == fc::ASCII && ! trans_shadow) )
  {
    clearShadow();
    return;
  }

  x1 = 1;
  x2 = getWidth();
  y1 = 1;
  y2 = getHeight();

  if ( trans_shadow )
  {
    // transparent shadow
    setPrintPos (x2+1, y1);
    setTransparent();
    print ("  ");
    unsetTransparent();

    setColor (wc.shadow_bg, wc.shadow_fg);
    setTransShadow();

    for (int i=1; i < getHeight(); i++)
    {
      setPrintPos (x2+1, y1+i);
      print ("  ");
    }

    unsetTransShadow();
    setPrintPos (x1, y2+1);
    setTransparent();
    print ("  ");
    unsetTransparent();

    setColor (wc.shadow_bg, wc.shadow_fg);
    setTransShadow();

    for (int i=2; i <= getWidth()+1; i++)
      print (' ');

    unsetTransShadow();

    if ( isMonochron() )
      setReverse(false);
  }
  else
  {
    // non-transparent shadow
    int block;
    setPrintPos (x2+1, y1);

    if ( isWindowWidget() )
    {
      setColor (wc.shadow_fg, wc.shadow_bg);
      setInheritBackground();  // current background color will be ignored
    }
    else if ( FWidget* p = getParentWidget() )
      setColor (wc.shadow_fg, p->getBackgroundColor());


    if ( isTeraTerm() )
    {
      block = 0xdb; // █
      print (0xdc); // ▄
    }
    else
    {
      block = fc::FullBlock; // █
      print (fc::LowerHalfBlock); // ▄
    }

    if ( isWindowWidget() )
      unsetInheritBackground();

    for (int i=1; i < getHeight(); i++)
    {
      setPrintPos (x2+1, y1+i);
      print (block); // █
    }

    setPrintPos (x1+1, y2+1);

    if ( isWindowWidget() )
      setInheritBackground();

    for (int i=1; i <= getWidth(); i++)
    {
      if ( isTeraTerm() )
        print (0xdf); // ▀
      else
        print (fc::UpperHalfBlock); // ▀
    }

    if ( isWindowWidget() )
      unsetInheritBackground();
  }
}

//----------------------------------------------------------------------
void FWidget::clearShadow()
{
  int x1, x2, y1, y2;

  if ( isMonochron() )
    return;

  x1 = 1;
  x2 = getWidth();
  y1 = 1;
  y2 = getHeight();

  if ( isWindowWidget() )
  {
    setColor (wc.shadow_fg, wc.shadow_bg);
    setInheritBackground();  // current background color will be ignored
  }
  else if ( FWidget* p = getParentWidget() )
    setColor (wc.shadow_fg, p->getBackgroundColor());

  if ( x2 <= offset.getX2() )
  {
    for (int i=0; i < getHeight(); i++)
    {
      setPrintPos (x2+1, y1+i);
      print  (' ');  // clear █
    }
  }

  if ( y2 <= offset.getY2() )
  {
    setPrintPos (x1+1, y2+1);

    for (int i=1; i <= getWidth(); i++)
      print (' '); // clear ▀
  }

  if ( isWindowWidget() )
    unsetInheritBackground();
}

//----------------------------------------------------------------------
void FWidget::drawFlatBorder()
{
  int x1, x2, y1, y2;

  if ( ! isNewFont() )
    return;

  x1 = 1;
  x2 = getWidth() + 1;
  y1 = 0;
  y2 = getHeight() + 1;

  if ( FWidget* p = getParentWidget() )
    setColor (wc.dialog_fg, p->getBackgroundColor());
  else
    setColor (wc.dialog_fg, wc.dialog_bg);

  for (int y=0; y < getHeight(); y++)
  {
    setPrintPos (x1-1, y1+y+1);

    if ( double_flatline_mask.left[uLong(y)] )
      print (fc::NF_rev_border_line_right_and_left); // left+right line (on left side)
    else
      print (fc::NF_rev_border_line_right); // right line (on left side)
  }

  setPrintPos (x2, y1+1);

  for (int y=0; y < getHeight(); y++)
  {
    if ( double_flatline_mask.right[uLong(y)] )
      print (fc::NF_rev_border_line_right_and_left); // left+right line (on right side)
    else
      print (fc::NF_border_line_left); // left line (on right side)

    setPrintPos (x2, y1+y+2);
  }

  setPrintPos (x1, y1);

  for (int x=0; x < getWidth(); x++)
  {
    if ( double_flatline_mask.top[uLong(x)] )
      print (fc::NF_border_line_up_and_down); // top+bottom line (at top)
    else
      print (fc::NF_border_line_bottom); // bottom line (at top)
  }

  setPrintPos (x1, y2);

  for (int x=0; x < getWidth(); x++)
  {
    if ( double_flatline_mask.bottom[uLong(x)] )
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

  x1 = 1;
  x2 = getWidth() + 1;
  y1 = 0;
  y2 = getHeight() + 1;

  if ( FWidget* p = getParentWidget() )
    setColor (wc.dialog_fg, p->getBackgroundColor());
  else
    setColor (wc.dialog_fg, wc.dialog_bg);

  // clear on left side
  for (register int y=0; y < getHeight(); y++)
  {
    setPrintPos (x1-1, y1+y+1);

    if ( double_flatline_mask.left[uLong(y)] )
      print (fc::NF_border_line_left);
    else
      print (' ');
  }

  // clear on right side
  for (register int y=0; y < getHeight(); y++)
  {
    setPrintPos (x2, y1+y+1);

    if ( double_flatline_mask.right[uLong(y)] )
      print (fc::NF_rev_border_line_right);
    else
      print (' ');
  }

  // clear at top
  setPrintPos (x1, y1);

  for (register int x=0; x < getWidth(); x++)
  {
    if ( double_flatline_mask.top[uLong(x)] )
      print (fc::NF_border_line_upper);
    else
      print (' ');
  }

  // clear at bottom
  setPrintPos (x1, y2);

  for (register int x=0; x < getWidth(); x++)
  {
    if ( double_flatline_mask.bottom[uLong(x)] )
      print (fc::NF_border_line_bottom);
    else
      print (' ');
  }
}

//----------------------------------------------------------------------
void FWidget::setDoubleFlatLine (int side, bool bit)
{
  uLong length;

  assert (  side == fc::top
         || side == fc::right
         || side == fc::bottom
         || side == fc::left );

  switch ( side )
  {
    case fc::top:
      length = double_flatline_mask.top.size();
      double_flatline_mask.top.assign(length, bit);
      break;

    case fc::right:
      length = double_flatline_mask.right.size();
      double_flatline_mask.right.assign(length, bit);
      break;

    case fc::bottom:
      length = double_flatline_mask.bottom.size();
      double_flatline_mask.bottom.assign(length, bit);
      break;

    case fc::left:
      length = double_flatline_mask.left.size();
      double_flatline_mask.left.assign(length, bit);
      break;

    default:
      break;
  }
}

//----------------------------------------------------------------------
void FWidget::setDoubleFlatLine (int side, int pos, bool bit)
{
  uLong length, index;

  assert (  side == fc::top
         || side == fc::right
         || side == fc::bottom
         || side == fc::left );

  assert ( pos >= 1 );

  index = uLong(pos - 1);

  switch ( side )
  {
    case fc::top:
      length = double_flatline_mask.top.size();

      if ( index < length )
        double_flatline_mask.top[index] = bit;

      break;

    case fc::right:
      length = double_flatline_mask.right.size();

      if ( index < length )
        double_flatline_mask.right[index] = bit;

      break;

    case fc::bottom:
      length = double_flatline_mask.bottom.size();

      if ( index < length )
        double_flatline_mask.bottom[index] = bit;

      break;

    case fc::left:
      length = double_flatline_mask.left.size();

      if ( index < length )
        double_flatline_mask.left[index] = bit;

      break;

    default:
      break;
  }
}

//----------------------------------------------------------------------
std::vector<bool>& FWidget::doubleFlatLine_ref (int side)
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
void FWidget::drawBorder (int x1, int y1, int x2, int y2)
{
  if ( x1 > x2 )
    std::swap (x1, x2);

  if ( y1 > y2 )
    std::swap (y1, y2);

  if ( x1 < 1 )
    x1 = 1;

  if ( y1 < 1 )
    y1 = 1;

  if ( x2 > getWidth() )
    x2 = getWidth();

  if ( y2 > getHeight() )
    y2 = getHeight();

  if ( isNewFont() )
  {
    setPrintPos (x1, y1);
    print (fc::NF_border_corner_middle_upper_left); // ┌

    for (int x=x1+1; x < x2; x++)
      print (fc::BoxDrawingsHorizontal); // ─

    print (fc::NF_border_corner_middle_upper_right); // ┐

    for (int y=y1+1; y <= y2; y++)
    {
      setPrintPos (x1, y);
      print (fc::NF_border_line_left); // border left ⎸
      setPrintPos (x2, y);
      print (fc::NF_rev_border_line_right); // border right⎹
    }

    setPrintPos (x1, y2);
    print (fc::NF_border_corner_middle_lower_left); // └

    for (int x=x1+1; x < x2; x++)
      print (fc::BoxDrawingsHorizontal); // ─

    print (fc::NF_border_corner_middle_lower_right); // ┘
  }
  else
  {
    setPrintPos (x1, y1);
    print (fc::BoxDrawingsDownAndRight); // ┌

    for (int x=x1+1; x < x2; x++)
      print (fc::BoxDrawingsHorizontal); // ─

    print (fc::BoxDrawingsDownAndLeft); // ┐

    for (int y=y1+1; y < y2; y++)
    {
      setPrintPos (x1, y);
      print (fc::BoxDrawingsVertical); // │
      setPrintPos (x2, y);
      print (fc::BoxDrawingsVertical); // │
    }

    setPrintPos (x1, y2);
    print (fc::BoxDrawingsUpAndRight); // └

    for (int x=x1+1; x < x2; x++)
      print (fc::BoxDrawingsHorizontal); // ─

    print (fc::BoxDrawingsUpAndLeft); // ┘

    for (int x=x1+1; x < x2; x++)
    {
      setPrintPos (x, y1);
      print (fc::BoxDrawingsHorizontal); // ─
      setPrintPos (x, y2);
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
