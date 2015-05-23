// fwindow.cpp
// class FWindow

#include "fapp.h"
#include "fstatusbar.h"
#include "fwindow.h"

//----------------------------------------------------------------------
// class FWindow
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FWindow::FWindow(FWidget* parent) : FWidget(parent)
{
  window_active = false;
}

//----------------------------------------------------------------------
FWindow::~FWindow()  // destructor
{
}

// protected methods of FWindow
//----------------------------------------------------------------------
bool FWindow::event (FEvent* ev)
{
  switch ( ev->type() )
  {
    case WindowActive_Event:
      onWindowActive (ev);
      break;

    case WindowInactive_Event:
      onWindowInactive (ev);
      break;

    case WindowRaised_Event:
      onWindowRaised (ev);
      break;

    case WindowLowered_Event:
      onWindowLowered (ev);
      break;

    default:
      return FWidget::event(ev);
  }
  return true;
}

//----------------------------------------------------------------------
void FWindow::onWindowActive (FEvent*)
{
}

//----------------------------------------------------------------------
void FWindow::onWindowInactive (FEvent*)
{
}

//----------------------------------------------------------------------
void FWindow::onWindowRaised (FEvent*)
{
}

//----------------------------------------------------------------------
void FWindow::onWindowLowered (FEvent*)
{
}

// public methods of FWindow
//----------------------------------------------------------------------
FWindow* FWindow::windowWidgetAt(const FPoint& pos)
{
  return windowWidgetAt(pos.getX(), pos.getY());
}

//----------------------------------------------------------------------
FWindow* FWindow::windowWidgetAt(int x, int y)
{
  if ( statusBar() && statusBar()->getGeometryGlobal().contains(x,y) )
    return statusBar();

  if ( window_list && ! window_list->empty() )
  {
    widgetList::const_iterator iter, begin;
    iter  = window_list->end();
    begin = window_list->begin();

    do
    {
      --iter;
      if ( *iter && (*iter)->getGeometryGlobal().contains(x,y) )
        return static_cast<FWindow*>(*iter);
    }
    while ( iter != begin );
  }
  return 0;
}

//----------------------------------------------------------------------
void FWindow::addWindow (FWidget* obj)
{
  if ( window_list )
    window_list->push_back(obj);
}

//----------------------------------------------------------------------
void FWindow::delWindow (FWidget* obj)
{
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
  widgetList::iterator iter, iter1, iter2, end;

  if ( ! window_list )
    return;
  if ( window_list->empty() )
    return;
  if ( (obj1->getFlags() & MODAL) != 0 )
    return;
  if ( (obj2->getFlags() & MODAL) != 0 )
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
  widgetList::iterator iter;

  if ( ! window_list )
    return false;
  if ( window_list->empty() )
    return false;
  if ( ! obj->isWindow() )
    return false;
  if ( window_list->back() == obj )
    return false;
  if ( (window_list->back()->getFlags() & MODAL) != 0 )
    return false;

  iter = window_list->begin();

  while ( iter != window_list->end() )
  {
    if ( *iter == obj )
    {
      window_list->erase (iter);
      window_list->push_back (obj);
      FEvent ev(WindowRaised_Event);
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
  widgetList::iterator iter;

  if ( ! window_list )
    return false;
  if ( window_list->empty() )
    return false;
  if ( ! obj->isWindow() )
    return false;
  if ( window_list->front() == obj )
    return false;
  if ( (obj->getFlags() & MODAL) != 0 )
    return false;

  iter = window_list->begin();

  while ( iter != window_list->end() )
  {
    if ( *iter == obj )
    {
      window_list->erase (iter);
      window_list->insert (window_list->begin(), obj);
      FEvent ev(WindowLowered_Event);
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
        FEvent ev(WindowActive_Event);
        FApplication::sendEvent(window, &ev);
      }
    }
    else
    {
      FWindow* w = static_cast<FWindow*>(*iter);
      if ( w->isActiveWindow() )
      {
        w->deactivateWindow();
        FEvent ev(WindowInactive_Event);
        FApplication::sendEvent(*iter, &ev);
      }
    }
    ++iter;
  }
}

//----------------------------------------------------------------------
FWindow* FWindow::getActiveWindow()
{
  FWindow* active_window = static_cast<FWindow*>(FApplication::active_window);
  return active_window;
}

//----------------------------------------------------------------------
bool FWindow::activatePrevWindow()
{
  if ( window_list && window_list->size() > 1 )
  {
    widgetList::const_iterator iter, begin;
    iter  = window_list->end();
    begin = window_list->begin();
    --iter;
    do
    {
      --iter;
      FWindow* w = static_cast<FWindow*>(*iter);
      if ( w && ! w->isActiveWindow() )
      {
        setActiveWindow(w);
        return true;
      }
    }
    while ( iter != begin );
  }
  return false;
}

//----------------------------------------------------------------------
bool FWindow::activateWindow(bool on)
{
  if ( on )
    FApplication::active_window = this;

  return window_active = (on) ? true : false;
}
