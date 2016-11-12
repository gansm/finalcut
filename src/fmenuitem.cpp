// File: fmenuitem.cpp
// Provides: class FMenuItem

#include "fapp.h"
#include "fdialog.h"
#include "fmenu.h"
#include "fmenubar.h"
#include "fmenulist.h"
#include "fmenuitem.h"
#include "fstatusbar.h"

//----------------------------------------------------------------------
// class FMenuItem
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FMenuItem::FMenuItem (FWidget* parent)
  : FWidget(parent)
  , text()
  , selected(false)
  , separator(false)
  , checkable(false)
  , checked(false)
  , radio_button(false)
  , dialog_index(false)
  , text_length(0)
  , hotkey(0)
  , accel_key(0)
  , menu(0)
  , super_menu(0)
  , associated_window(0)
{
  init (parent);
}

//----------------------------------------------------------------------
FMenuItem::FMenuItem (FString& txt, FWidget* parent)
  : FWidget(parent)
  , text(txt)
  , selected(false)
  , separator(false)
  , checkable(false)
  , checked(false)
  , radio_button(false)
  , dialog_index(false)
  , text_length(0)
  , hotkey(0)
  , accel_key(0)
  , menu(0)
  , super_menu(0)
  , associated_window(0)
{
  init (parent);
}

//----------------------------------------------------------------------
FMenuItem::FMenuItem (const std::string& txt, FWidget* parent)
  : FWidget(parent)
  , text(txt)
  , selected(false)
  , separator(false)
  , checkable(false)
  , checked(false)
  , radio_button(false)
  , dialog_index(false)
  , text_length(0)
  , hotkey(0)
  , accel_key(0)
  , menu(0)
  , super_menu(0)
  , associated_window(0)
{
  init (parent);
}

//----------------------------------------------------------------------
FMenuItem::FMenuItem (const char* txt, FWidget* parent)
  : FWidget(parent)
  , text(txt)
  , selected(false)
  , separator(false)
  , checkable(false)
  , checked(false)
  , radio_button(false)
  , dialog_index(false)
  , text_length(0)
  , hotkey(0)
  , accel_key(0)
  , menu(0)
  , super_menu(0)
  , associated_window(0)
{
  init (parent);
}

//----------------------------------------------------------------------
FMenuItem::FMenuItem (int k, FString& txt, FWidget* parent)
  : FWidget(parent)
  , text(txt)
  , selected(false)
  , separator(false)
  , checkable(false)
  , checked(false)
  , radio_button(false)
  , dialog_index(false)
  , text_length(0)
  , hotkey(0)
  , accel_key(k)
  , menu(0)
  , super_menu(0)
  , associated_window(0)
{
  init (parent);
}

//----------------------------------------------------------------------
FMenuItem::FMenuItem (int k, const std::string& txt, FWidget* parent)
  : FWidget(parent)
  , text(txt)
  , selected(false)
  , separator(false)
  , checkable(false)
  , checked(false)
  , radio_button(false)
  , dialog_index(false)
  , text_length(0)
  , hotkey(0)
  , accel_key(k)
  , menu(0)
  , super_menu(0)
  , associated_window(0)
{
  init (parent);
}

//----------------------------------------------------------------------
FMenuItem::FMenuItem (int k, const char* txt, FWidget* parent)
  : FWidget(parent)
  , text(txt)
  , selected(false)
  , separator(false)
  , checkable(false)
  , checked(false)
  , radio_button(false)
  , dialog_index(false)
  , text_length(0)
  , hotkey(0)
  , accel_key(k)
  , menu(0)
  , super_menu(0)
  , associated_window(0)
{
  init (parent);
}

//----------------------------------------------------------------------
FMenuItem::~FMenuItem()  // destructor
{
  delAccelerator();

  // remove dialog list item callback from the dialog
  if ( associated_window )
    associated_window->delCallback(this);
}


// public methods of FMenuItem
//----------------------------------------------------------------------
bool FMenuItem::setEnable (bool on)
{
  FWidget::setEnable(on);
  FWidget* super = getSuperMenu();

  if ( on )
  {
    flags |= fc::active;

    if ( super && isMenuBar(super) )
    {
      // Meta + hotkey
      super->addAccelerator (fc::Fmkey_meta + std::tolower(hotkey), this);
    }
  }
  else
  {
    flags &= ~fc::active;

    if ( super && isMenuBar(super) )
      super->delAccelerator (this);
  }

  return on;
}

//----------------------------------------------------------------------
bool FMenuItem::setFocus (bool on)
{
  FWidget::setFocus(on);

  if ( on )
  {
    flags |= fc::focus;

    if ( isEnabled() )
    {
      if ( ! selected )
      {
        FMenuList* menu_list = dynamic_cast<FMenuList*>(getSuperMenu());
        setSelected();

        if ( menu_list )
        {
          menu_list->unselectItem();
          menu_list->setSelectedItem(this);
        }

        if ( getStatusBar() )
          getStatusBar()->drawMessage();

        FWidget* parent = getSuperMenu();

        if ( isMenuBar(parent) )
        {
          FMenuBar* menubar_ptr = dynamic_cast<FMenuBar*>(parent);

          if ( menubar_ptr )
            menubar_ptr->redraw();
        }
        else if ( isMenu(parent) )
        {
          FMenu* menu_ptr = dynamic_cast<FMenu*>(parent);

          if ( menu_ptr )
            menu_ptr->redraw();
        }
      }

      if ( getStatusBar() )
      {
        FString msg = getStatusbarMessage();
        FString curMsg = getStatusBar()->getMessage();

        if ( curMsg != msg )
          getStatusBar()->setMessage(msg);
      }
    }
  }
  else
  {
    flags &= ~fc::focus;

    if ( isEnabled() && getStatusBar() )
      getStatusBar()->clearMessage();
  }

  return on;
}

//----------------------------------------------------------------------
void FMenuItem::setSelected()
{
  if ( isEnabled() )
  {
    selected = true;
    processActivate();
  }
}

//----------------------------------------------------------------------
void FMenuItem::unsetSelected()
{
  selected = false;
  unsetCursorPos();
  processDeactivate();
}

//----------------------------------------------------------------------
void FMenuItem::setText (FString& txt)
{
  text = txt;
  text_length = text.getLength();
  hotkey = hotKey();

  if ( hotkey )
    text_length--;

  setWidth(int(text_length));
}

//----------------------------------------------------------------------
void FMenuItem::setText (const std::string& txt)
{
  FString s = FString(txt);
  setText (s);
}

//----------------------------------------------------------------------
void FMenuItem::setText (const char* txt)
{
  FString s = FString(txt);
  setText (s);
}
//----------------------------------------------------------------------

void FMenuItem::addAccelerator (int key, FWidget* obj)
{
  FWidget* root = getRootWidget();
  accelerator accel = { key, obj };

  if ( root && root->accelerator_list )
  {
    accel_key = key;
    root->accelerator_list->push_back(accel);
  }

  if ( isMenu(super_menu) )
  {
    FMenu* menu_ptr = dynamic_cast<FMenu*>(super_menu);

    if ( menu_ptr )
      menu_ptr->calculateDimensions();
  }
}

//----------------------------------------------------------------------
void FMenuItem::delAccelerator (FWidget* obj)
{
  FWidget* root = getRootWidget();

  if (  root
     && root->accelerator_list
     && ! root->accelerator_list->empty() )
  {
    FWidget::Accelerators::iterator iter;
    iter = root->accelerator_list->begin();

    while ( iter != root->accelerator_list->end() )
    {
      if ( iter->object == obj )
      {
        accel_key = 0;
        iter = root->accelerator_list->erase(iter);
      }
      else
        ++iter;
    }
  }

  if ( isMenu(super_menu) )
  {
    FMenu* menu_ptr = dynamic_cast<FMenu*>(super_menu);

    if ( menu_ptr )
      menu_ptr->calculateDimensions();
  }
}

//----------------------------------------------------------------------
void FMenuItem::openMenu()
{
  FMenu* dd_menu;  // Drop-down menu
  FMenu* open_menu;

  if ( ! hasMenu() )
    return;

  dd_menu = getMenu();  // Drop-down menu

  if ( dd_menu->isVisible() )
    return;

  open_menu = static_cast<FMenu*>(getOpenMenu());

  if ( open_menu && open_menu != dd_menu )
  {
    open_menu->hide();
    open_menu->hideSubMenus();
  }

  if ( dialog_index )
    createDialogList (dd_menu);

  setOpenMenu(dd_menu);
  dd_menu->setVisible();
  dd_menu->show();
  dd_menu->raiseWindow(dd_menu);
  dd_menu->redraw();
  updateTerminal();
  flush_out();
}

//----------------------------------------------------------------------
void FMenuItem::onKeyPress (FKeyEvent* ev)
{
  if ( ! super_menu )
    return;

  if ( isMenu(super_menu) )
  {
    FMenu* smenu = dynamic_cast<FMenu*>(super_menu);

    if ( smenu )
      smenu->onKeyPress(ev);
  }

  if ( isMenuBar(super_menu) )
  {
    FMenuBar* mbar = dynamic_cast<FMenuBar*>(super_menu);

    if ( mbar )
    {
      if ( mbar->hotkeyMenu(ev) )
        return;

      mbar->onKeyPress(ev);
    }
  }
}

//----------------------------------------------------------------------
void FMenuItem::onMouseDoubleClick (FMouseEvent* ev)
{
  if ( ! super_menu )
    return;

  const FPoint& t = ev->getTermPos();
  int b = ev->getButton();

  if ( isMenu(super_menu) )
  {
    FMenu* smenu = dynamic_cast<FMenu*>(super_menu);

    if ( smenu )
    {
      const FPoint& p2 = smenu->termToWidgetPos(t);
      FMouseEvent* _ev = new FMouseEvent (fc::MouseDoubleClick_Event, p2, t, b);
      smenu->onMouseDoubleClick(_ev);
      delete _ev;
    }
  }

  if ( isMenuBar(super_menu) )
  {
    FMenuBar* mbar = dynamic_cast<FMenuBar*>(super_menu);

    if ( mbar )
    {
      const FPoint& p2 = mbar->termToWidgetPos(t);
      FMouseEvent* _ev = new FMouseEvent (fc::MouseDoubleClick_Event, p2, t, b);
      mbar->onMouseDoubleClick(_ev);
      delete _ev;
    }
  }

  if ( isWindowsMenu(super_menu) )
  {
    FDialog* dgl = dynamic_cast<FDialog*>(super_menu);

    if ( dgl )
    {
      const FPoint& p2 = dgl->termToWidgetPos(t);
      FMouseEvent* _ev = new FMouseEvent (fc::MouseDoubleClick_Event, p2, t, b);
      dgl->onMouseDoubleClick(_ev);
      delete _ev;
    }
  }
}

//----------------------------------------------------------------------
void FMenuItem::onMouseDown (FMouseEvent* ev)
{
  if ( ! super_menu )
    return;

  const FPoint& t = ev->getTermPos();
  int b = ev->getButton();

  if ( isMenu(super_menu) )
  {
    FMenu* smenu = dynamic_cast<FMenu*>(super_menu);

    if ( smenu )
    {
      const FPoint& p2 = smenu->termToWidgetPos(t);
      FMouseEvent* _ev = new FMouseEvent (fc::MouseDown_Event, p2, t, b);
      smenu->onMouseDown(_ev);
      delete _ev;
    }
  }

  if ( isMenuBar(super_menu) )
  {
    FMenuBar* mbar = dynamic_cast<FMenuBar*>(super_menu);

    if ( mbar )
    {
      const FPoint& p2 = mbar->termToWidgetPos(t);
      FMouseEvent* _ev = new FMouseEvent (fc::MouseDown_Event, p2, t, b);
      mbar->onMouseDown(_ev);
      delete _ev;
    }
  }

  if ( isWindowsMenu(super_menu) )
  {
    FDialog* dgl = dynamic_cast<FDialog*>(super_menu);

    if ( dgl )
    {
      const FPoint& p2 = dgl->termToWidgetPos(t);
      FMouseEvent* _ev = new FMouseEvent (fc::MouseDown_Event, p2, t, b);
      dgl->onMouseDown(_ev);
      delete _ev;
    }
  }
}

//----------------------------------------------------------------------
void FMenuItem::onMouseUp (FMouseEvent* ev)
{
  if ( ! super_menu )
    return;

  const FPoint& t = ev->getTermPos();
  int b = ev->getButton();

  if ( isMenu(super_menu) )
  {
    FMenu* smenu = dynamic_cast<FMenu*>(super_menu);

    if ( smenu )
    {
      const FPoint& p2 = smenu->termToWidgetPos(t);
      FMouseEvent* _ev = new FMouseEvent (fc::MouseUp_Event, p2, t, b);
      smenu->onMouseUp(_ev);
      delete _ev;
    }
  }

  if ( isMenuBar(super_menu) )
  {
    FMenuBar* mbar = dynamic_cast<FMenuBar*>(super_menu);

    if ( mbar )
    {
      const FPoint& p2 = mbar->termToWidgetPos(t);
      FMouseEvent* _ev = new FMouseEvent (fc::MouseUp_Event, p2, t, b);
      mbar->onMouseUp(_ev);
      delete _ev;
    }
  }

  if ( isWindowsMenu(super_menu) )
  {
    FDialog* dgl = dynamic_cast<FDialog*>(super_menu);

    if ( dgl )
    {
      const FPoint& p2 = dgl->termToWidgetPos(t);
      FMouseEvent* _ev = new FMouseEvent (fc::MouseUp_Event, p2, t, b);
      dgl->onMouseUp(_ev);
      delete _ev;
    }
  }
}

//----------------------------------------------------------------------
void FMenuItem::onMouseMove (FMouseEvent* ev)
{
  if ( ! super_menu )
    return;

  const FPoint& t = ev->getTermPos();
  int b = ev->getButton();

  if ( isMenu(super_menu) )
  {
    FMenu* smenu = dynamic_cast<FMenu*>(super_menu);

    if ( smenu )
    {
      const FPoint& p2 = smenu->termToWidgetPos(t);
      FMouseEvent* _ev = new FMouseEvent (fc::MouseMove_Event, p2, t, b);
      smenu->onMouseMove(_ev);
      delete _ev;
    }
  }

  if ( isMenuBar(super_menu) )
  {
    FMenuBar* mbar = dynamic_cast<FMenuBar*>(super_menu);

    if ( mbar )
    {
      const FPoint& p2 = mbar->termToWidgetPos(t);
      FMouseEvent* _ev = new FMouseEvent (fc::MouseMove_Event, p2, t, b);
      mbar->onMouseMove(_ev);
      delete _ev;
    }
  }

  if ( isWindowsMenu(super_menu) )
  {
    FDialog* dgl = dynamic_cast<FDialog*>(super_menu);

    if ( dgl )
    {
      const FPoint& p2 = dgl->termToWidgetPos(t);
      FMouseEvent* _ev = new FMouseEvent (fc::MouseMove_Event, p2, t, b);
      dgl->onMouseMove(_ev);
      delete _ev;
    }
  }
}

//----------------------------------------------------------------------
void FMenuItem::onAccel (FAccelEvent* ev)
{
  if ( ! isEnabled() || isSelected() )
    return;

  if ( ! super_menu || ! isMenuBar(super_menu) )
  {
    processClicked();
    return;
  }

  FMenuBar* mbar = dynamic_cast<FMenuBar*>(super_menu);

  if ( ! mbar )
    return;

  if ( menu )
  {
    FWidget* focused_widget;

    if ( mbar->getSelectedItem() )
      mbar->getSelectedItem()->unsetSelected();

    setSelected();
    mbar->selected_item = this;
    openMenu();

    focused_widget = static_cast<FWidget*>(ev->focusedWidget());
    FFocusEvent out (fc::FocusOut_Event);
    FApplication::queueEvent(focused_widget, &out);
    menu->unselectItem();
    menu->selectFirstItem();

    if ( menu->getSelectedItem() )
      menu->getSelectedItem()->setFocus();

    if ( focused_widget )
      focused_widget->redraw();

    menu->redraw();

    if ( getStatusBar() )
      getStatusBar()->drawMessage();

    mbar->redraw();
    mbar->drop_down = true;
  }
  else
  {
    unsetSelected();
    mbar->selected_item = 0;
    mbar->redraw();
    processClicked();
    mbar->drop_down = false;
  }
  ev->accept();
}

//----------------------------------------------------------------------
void FMenuItem::onFocusIn (FFocusEvent*)
{
  if ( getStatusBar() )
    getStatusBar()->drawMessage();
}

//----------------------------------------------------------------------
void FMenuItem::onFocusOut (FFocusEvent*)
{
  unsetSelected();

  if ( super_menu && isMenuBar(super_menu) )
  {
    FMenuBar* mbar = dynamic_cast<FMenuBar*>(super_menu);

    if ( mbar )
      mbar->redraw();
  }

  if ( getStatusBar() )
  {
    getStatusBar()->clearMessage();
    getStatusBar()->drawMessage();
  }
}


// protected methods of FMenuItem
//----------------------------------------------------------------------
bool FMenuItem::isWindowsMenu (FWidget* w) const
{
  return ( ! w ) ? false : w->isDialogWidget();
}

//----------------------------------------------------------------------
bool FMenuItem::isMenuBar (FWidget* w) const
{
  if ( ! w )
    return false;
  else
    return bool( std::strcmp ( w->getClassName()
                             , const_cast<char*>("FMenuBar") ) == 0 );
}

//----------------------------------------------------------------------
bool FMenuItem::isMenu (FWidget* w) const
{
  if ( ! w )
    return false;

  bool m1 = ( std::strcmp ( w->getClassName()
                          , const_cast<char*>("FMenu") ) == 0 );
  bool m2 = ( std::strcmp ( w->getClassName()
                          , const_cast<char*>("FDialogListMenu") ) == 0 );
  return bool( m1 || m2 );
}


// private methods of FMenuItem
//----------------------------------------------------------------------
void FMenuItem::init (FWidget* parent)
{
  text_length = text.getLength();
  hotkey = hotKey();

  if ( hotkey )
    text_length--;

  setGeometry (1,1,int(text_length+2),1, false);

  if ( parent )
  {
    FMenuList* menu_list;
    setSuperMenu (parent);

    if ( accel_key  )
      addAccelerator (accel_key);

    menu_list = dynamic_cast<FMenuList*>(parent);

    if ( menu_list )
      menu_list->insert(this);

    if ( isMenuBar(parent) ) // Parent is menubar
    {
      FMenuBar* menubar_ptr = dynamic_cast<FMenuBar*>(parent);

      if ( menubar_ptr )
      {
        menubar_ptr->calculateDimensions();

        if ( hotkey )  // Meta + hotkey
          menubar_ptr->addAccelerator (fc::Fmkey_meta + std::tolower(hotkey), this);
      }

      this->addCallback
      (
        "deactivate",
        _METHOD_CALLBACK (parent, &FMenuBar::cb_item_deactivated)
      );
    }
    else if ( isMenu(parent) ) // Parent is menu
    {
      FMenu* menu_ptr = dynamic_cast<FMenu*>(parent);

      if ( menu_ptr )
        menu_ptr->calculateDimensions();
    }
  }

  if ( hasFocus() )
    flags = fc::focus;

  if ( isEnabled() )
    flags |= fc::active;
}

//----------------------------------------------------------------------
uChar FMenuItem::hotKey()
{
  uInt length;

  if ( text.isEmpty() )
    return 0;

  length = text.getLength();

  for (uInt i=0; i < length; i++)
  {
    try
    {
      if ( (i+1 < length) && (text[i] == '&') )
        return uChar(text[++i]);
    }
    catch (const std::out_of_range&)
    {
      return 0;;
    }
  }

  return 0;
}

//----------------------------------------------------------------------
void FMenuItem::processActivate()
{
  emitCallback("activate");
}

//----------------------------------------------------------------------
void FMenuItem::processDeactivate()
{
  emitCallback("deactivate");
}

//----------------------------------------------------------------------
void FMenuItem::createDialogList (FMenu* winmenu)
{
  winmenu->clear();

  if ( dialog_list && ! dialog_list->empty() )
  {
    widgetList::const_iterator iter, begin;
    iter = begin = dialog_list->begin();

    while ( iter != dialog_list->end() && *iter )
    {
      FDialog* win = dynamic_cast<FDialog*>(*iter);

      if ( win )
      {
        int n = int(std::distance(begin, iter));
        // get the dialog title
        FString name = win->getText();
        // create a new dialog list item
        FMenuItem* win_item = new FMenuItem (name, winmenu);

        if ( n < 9 )
          win_item->addAccelerator (fc::Fmkey_1 + n); // Meta + 1..9

        win_item->addCallback
        (
          "clicked",
          _METHOD_CALLBACK (win_item, &FMenuItem::cb_switchToDialog),
          dynamic_cast<FWidget::data_ptr>(win)
        );

        win->addCallback
        (
          "destroy",
          _METHOD_CALLBACK (win_item, &FMenuItem::cb_destroyDialog)
        );

        win_item->associated_window = win;
      }

      ++iter;
    }
  }

  winmenu->calculateDimensions();
}

//----------------------------------------------------------------------
void FMenuItem::cb_switchToDialog (FWidget*, void* data_ptr)
{
  FDialog* win = static_cast<FDialog*>(data_ptr);

  if ( win )
  {
    FWidget* focus_widget = getFocusWidget();
    FAccelEvent a_ev (fc::Accelerator_Event, focus_widget);
    FApplication::sendEvent (win, &a_ev);
  }
}

//----------------------------------------------------------------------
void FMenuItem::cb_destroyDialog (FWidget* widget, void*)
{
  FDialog* win = static_cast<FDialog*>(widget);
  FApplication* fapp = static_cast<FApplication*>(getRootWidget());

  if ( win && fapp )
  {
    delAccelerator(win);
    delCallback(win);
    associated_window = 0;
  }
}

//----------------------------------------------------------------------
void FMenuItem::processClicked()
{
  emitCallback("clicked");
}
