// File: fwindow.cpp
// Provides: class FWindow

#include "fapp.h"
#include "fmenubar.h"
#include "fstatusbar.h"
#include "fwindow.h"

// static attributes
FWindow* FWindow::previous_widget = 0;

//----------------------------------------------------------------------
// class FWindow
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FWindow::FWindow(FWidget* parent)
  : FWidget(parent)
  , window_active(false)
{ }

//----------------------------------------------------------------------
FWindow::~FWindow()  // destructor
{ }


// protected methods of FWindow
//----------------------------------------------------------------------
bool FWindow::event (FEvent* ev)
{
  switch ( ev->type() )
  {
    case fc::WindowActive_Event:
      onWindowActive (ev);
      break;

    case fc::WindowInactive_Event:
      onWindowInactive (ev);
      break;

    case fc::WindowRaised_Event:
      onWindowRaised (ev);
      break;

    case fc::WindowLowered_Event:
      onWindowLowered (ev);
      break;

    default:
      return FWidget::event(ev);
  }
  return true;
}

//----------------------------------------------------------------------
void FWindow::onWindowActive (FEvent*)
{ }

//----------------------------------------------------------------------
void FWindow::onWindowInactive (FEvent*)
{ }

//----------------------------------------------------------------------
void FWindow::onWindowRaised (FEvent*)
{ }

//----------------------------------------------------------------------
void FWindow::onWindowLowered (FEvent*)
{ }


// public methods of FWindow
//----------------------------------------------------------------------
void FWindow::show()
{
  term_area* area = getVWin();
  if ( area )
    area->visible = true;
  FWidget::show();
}

//----------------------------------------------------------------------
void FWindow::hide()
{
  term_area* area = getVWin();
  if ( area )
    area->visible = false;
  FWidget::hide();
}

//----------------------------------------------------------------------
FWindow* FWindow::getWindowWidgetAt (int x, int y)
{
  // returns the window object to the corresponding coordinates
  if ( statusBar() && statusBar()->getGeometryGlobal().contains(x,y) )
    return statusBar();

  if ( menuBar() && menuBar()->getGeometryGlobal().contains(x,y) )
    return menuBar();

  if ( window_list && ! window_list->empty() )
  {
    widgetList::const_iterator iter, begin;
    iter  = window_list->end();
    begin = window_list->begin();

    do
    {
      --iter;
      if ( *iter )
      {
        FWindow* w = static_cast<FWindow*>(*iter);
        if ( ! w->isHiddenWindow()
           && w->getGeometryGlobal().contains(x,y) )
          return w;
      }
    }
    while ( iter != begin );
  }
  return 0;
}

//----------------------------------------------------------------------
void FWindow::addWindow (FWidget* obj)
{
  // add the window object obj to the window list
  if ( window_list )
    window_list->push_back(obj);
}

//----------------------------------------------------------------------
void FWindow::delWindow (FWidget* obj)
{
  // delete the window object obj from the window list
  if ( window_list && ! window_list->empty() )
  {
    widgetList::iterator iter;
    iter = window_list->begin();

    while ( iter != window_list->end() )
    {
      if ( (*iter) == obj )
      {
        window_list->erase(iter);
        return;
      }
      ++iter;
    }
  }
}

//----------------------------------------------------------------------
FWindow* FWindow::getWindowWidget (FWidget* obj)
{
  // returns the window object to the given widget obj
  FWidget* p_obj = obj->parentWidget();
  while ( ! obj->isWindow() && p_obj )
  {
    obj = p_obj;
    p_obj = p_obj->parentWidget();
  }
  if ( obj->isWindow() )
    return static_cast<FWindow*>(obj);
  else
    return 0;
}

//----------------------------------------------------------------------
int FWindow::getWindowLayer (FWidget* obj)
{
  // returns the window layer from the widget obj
  widgetList::iterator iter, end;
  FWidget* window;

  if ( ! window_list )
    return -1;
  if ( window_list->empty() )
    return -1;

  if ( ! obj->isWindow() )
  {
    if ( (window = getWindowWidget(obj)) == 0 )
      return -1;
  }
  else
    window = obj;

  iter = window_list->begin();
  end  = window_list->end();

  while ( iter != end )
  {
    if ( *iter == window )
      break;
    ++iter;
  }

  return int(std::distance(window_list->begin(), iter) + 1);
}

//----------------------------------------------------------------------
void FWindow::swapWindow (FWidget* obj1, FWidget* obj2)
{
  // swaps the window layer between obj1 and obj2
  widgetList::iterator iter, iter1, iter2, end;

  if ( ! window_list )
    return;
  if ( window_list->empty() )
    return;
  if ( (obj1->getFlags() & fc::modal) != 0 )
    return;
  if ( (obj2->getFlags() & fc::modal) != 0 )
    return;

  iter  = window_list->begin();
  end   = window_list->end();
  iter1 = end;
  iter2 = end;

  while ( iter != end )
  {
    if ( (*iter) == obj1 )
      iter1 = iter;
    else if ( (*iter) == obj2 )
      iter2 = iter;
    ++iter;
  }

  if ( iter1 != end && iter2 != end )
    std::swap (iter1, iter2);
}

//----------------------------------------------------------------------
bool FWindow::raiseWindow (FWidget* obj)
{
  // raises the window widget obj to the top
  widgetList::iterator iter;

  if ( ! window_list )
    return false;
  if ( window_list->empty() )
    return false;
  if ( ! obj->isWindow() )
    return false;
  if ( window_list->back() == obj )
    return false;
  if ( (window_list->back()->getFlags() & fc::modal) != 0
     && ! obj->isMenu() )
    return false;

  iter = window_list->begin();

  while ( iter != window_list->end() )
  {
    if ( *iter == obj )
    {
      window_list->erase (iter);
      window_list->push_back (obj);
      FEvent ev(fc::WindowRaised_Event);
      FApplication::sendEvent(obj, &ev);
      return true;
    }
    ++iter;
  }
  return false;
}

//----------------------------------------------------------------------
bool FWindow::lowerWindow (FWidget* obj)
{
  // lowers the window widget obj to the bottom
  widgetList::iterator iter;

  if ( ! window_list )
    return false;
  if ( window_list->empty() )
    return false;
  if ( ! obj->isWindow() )
    return false;
  if ( window_list->front() == obj )
    return false;
  if ( (obj->getFlags() & fc::modal) != 0 )
    return false;

  iter = window_list->begin();

  while ( iter != window_list->end() )
  {
    if ( *iter == obj )
    {
      window_list->erase (iter);
      window_list->insert (window_list->begin(), obj);
      FEvent ev(fc::WindowLowered_Event);
      FApplication::sendEvent(obj, &ev);
      return true;
    }
    ++iter;
  }
  return false;
}

//----------------------------------------------------------------------
void FWindow::setActiveWindow (FWindow* window)
{
  // activate FWindow object window
  widgetList::const_iterator iter, end;

  if ( ! window_list )
    return;
  if ( window_list->empty() )
    return;

  iter = window_list->begin();
  end  = window_list->end();

  while ( iter != end )
  {
    if ( *iter == window )
    {
      if ( ! window->isActiveWindow() )
      {
        window->activateWindow();
        FEvent ev(fc::WindowActive_Event);
        FApplication::sendEvent(window, &ev);
      }
    }
    else
    {
      FWindow* w = static_cast<FWindow*>(*iter);
      if ( w->isActiveWindow() )
      {
        w->deactivateWindow();
        FEvent ev(fc::WindowInactive_Event);
        FApplication::sendEvent(*iter, &ev);
      }
    }
    ++iter;
  }
}

//----------------------------------------------------------------------
FWindow* FWindow::getActiveWindow()
{
  // returns the active FWindow object
  FWindow* active_window = static_cast<FWindow*>(FApplication::active_window);
  return active_window;
}

//----------------------------------------------------------------------
void FWindow::switchToPrevWindow()
{
  // switch to previous window
  activatePrevWindow();

  FWindow* active_window = getActiveWindow();
  if ( active_window )
  {
    FWidget* focus_widget = active_window->getFocusWidget();
    if ( ! active_window->isActiveWindow() )
      setActiveWindow(active_window);
    raiseWindow (active_window);
    if ( focus_widget )
      focus_widget->setFocus();
    active_window->redraw();
  }
}

//----------------------------------------------------------------------
bool FWindow::activatePrevWindow()
{
  // activate the previous window
  FWindow* w = previous_widget;

  if ( w && ! w->isHiddenWindow() && ! w->isActiveWindow() )
  {
    setActiveWindow(w);
    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
bool FWindow::activateWindow (bool on)
{
  // activate/deactivate this window
  if ( on )
    FApplication::active_window = this;

  return window_active = (on) ? true : false;
}

//----------------------------------------------------------------------
bool FWindow::isHiddenWindow() const
{
  // returns the window hidden state
  term_area* area = getVWin();
  if ( area )
    return ! area->visible;
  else
    return false;
}
