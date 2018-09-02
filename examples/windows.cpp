/***********************************************************************
* windows.cpp - Shows window handling                                  *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2016-2018 Markus Gans                                      *
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

#include <vector>
#include <final/final.h>


//----------------------------------------------------------------------
// class SmallWindow
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class SmallWindow : public FDialog
{
  public:
    // Constructor
    explicit SmallWindow (FWidget* = 0);

    // Destructor
    ~SmallWindow();

  private:
    // Disable copy constructor
    SmallWindow (const SmallWindow&);

    // Disable assignment operator (=)
    SmallWindow& operator = (const SmallWindow&);

    // Method
    void adjustSize();

    // Event handlers
    void onShow (FShowEvent*);
    void onTimer (FTimerEvent*);

    // Data Members
    FLabel* left_arrow;
    FLabel* right_arrow;
    FLabel* top_left_label;
    FLabel* top_right_label;
    FLabel* bottom_label;
};
#pragma pack(pop)

//----------------------------------------------------------------------
SmallWindow::SmallWindow (FWidget* parent)
  : FDialog(parent)
  , left_arrow()
  , right_arrow()
  , top_left_label()
  , top_right_label()
  , bottom_label()
{
  wchar_t arrow_up, arrow_down;

  arrow_up = fc::BlackUpPointingTriangle;
  arrow_down = fc::BlackDownPointingTriangle;

  left_arrow = new FLabel (arrow_up, this);
  left_arrow->setForegroundColor (wc.label_inactive_fg);
  left_arrow->setEmphasis();
  left_arrow->ignorePadding();
  left_arrow->setGeometry (2, 2, 1, 1);

  right_arrow = new FLabel (arrow_up, this);
  right_arrow->setForegroundColor (wc.label_inactive_fg);
  right_arrow->setEmphasis();
  right_arrow->ignorePadding();
  right_arrow->setGeometry (getWidth() - 1, 2, 1, 1);

  const FString& top_left_label_text = "menu";
  top_left_label = new FLabel (top_left_label_text, this);
  top_left_label->setForegroundColor (wc.label_inactive_fg);
  top_left_label->setEmphasis();
  top_left_label->setGeometry (1, 1, 6, 1);

  const FString& top_right_label_text = "zoom";
  top_right_label = new FLabel (top_right_label_text, this);
  top_right_label->setAlignment (fc::alignRight);
  top_right_label->setForegroundColor (wc.label_inactive_fg);
  top_right_label->setEmphasis();
  top_right_label->setGeometry (getClientWidth() - 5, 1, 6, 1);

  FString bottom_label_text = "resize\n"
                              "corner\n";
  bottom_label_text += arrow_down;
  bottom_label = new FLabel (bottom_label_text, this);
  bottom_label->setAlignment (fc::alignRight);
  bottom_label->setForegroundColor (wc.label_inactive_fg);
  bottom_label->setEmphasis();
  bottom_label->setGeometry (13, 3, 6, 3);
}

//----------------------------------------------------------------------
SmallWindow::~SmallWindow()
{
  // Remove own timer
  delOwnTimer();
}

//----------------------------------------------------------------------
void SmallWindow::adjustSize()
{
  if ( isZoomed() )
  {
    *top_right_label = "unzoom";
    bottom_label->hide();
  }
  else
  {
    *top_right_label = "zoom";
    bottom_label->setVisible();
  }

  FDialog::adjustSize();
  right_arrow->setGeometry (getWidth() - 1, 2, 1, 1);
  top_right_label->setGeometry (getClientWidth() - 5, 1, 6, 1);
  bottom_label->setGeometry (1, getClientHeight() - 2, getClientWidth(), 3);
}

//----------------------------------------------------------------------
void SmallWindow::onShow (FShowEvent*)
{
  addTimer(1000);
}

//----------------------------------------------------------------------
void SmallWindow::onTimer (FTimerEvent*)
{
  left_arrow->unsetEmphasis();
  left_arrow->redraw();
  right_arrow->unsetEmphasis();
  right_arrow->redraw();
  top_left_label->unsetEmphasis();
  top_left_label->redraw();
  top_right_label->unsetEmphasis();
  top_right_label->redraw();
  bottom_label->unsetEmphasis();
  bottom_label->redraw();
  updateTerminal();
  delOwnTimer();
}


//----------------------------------------------------------------------
// class Window
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class Window : public FDialog
{
  public:
    // Constructor
    explicit Window (FWidget* = 0);

    // Destructor
    ~Window();

  private:
    // Typedefs
    typedef void (Window::*WindowCallback)(FWidget*, data_ptr);
    typedef void (FApplication::*FAppCallback)(FWidget*, data_ptr);
    typedef struct
    {
      bool is_open;
      FString* title;
      SmallWindow* dgl;
    }
    win_data;

    // Disable copy constructor
    Window (const Window&);

    // Disable assignment operator (=)
    Window& operator = (const Window&);

    // Method
    void createFileMenuItems (FMenu*);
    void createDialogButtons();
    void activateWindow (FDialog*);
    void adjustSize();
    void addClickedCallback (FWidget*, WindowCallback);
    void addClickedCallback (FWidget*, FAppCallback);

    // Event handlers
    void onClose (FCloseEvent*);

    // Callback methods
    void cb_createWindows (FWidget*, data_ptr);
    void cb_closeWindows (FWidget*, data_ptr);
    void cb_next (FWidget*, data_ptr);
    void cb_previous (FWidget*, data_ptr);
    void cb_destroyWindow (FWidget*, data_ptr);

    // Data Members
    std::vector<win_data*> windows;
};
#pragma pack(pop)

//----------------------------------------------------------------------
Window::Window (FWidget* parent)
  : FDialog(parent)
  , windows()
{
  FMenu* File;
  FDialogListMenu* DglList;
  FString drop_down_symbol;
  FMenuBar* Menubar;
  FStatusBar* Statusbar;

  // Menu bar
  Menubar = new FMenuBar (this);

  // Menu bar item
  File = new FMenu ("&File", Menubar);
  File->setStatusbarMessage ("File management commands");

  // Dialog list menu item
  drop_down_symbol = wchar_t(fc::BlackDownPointingTriangle);
  DglList = new FDialogListMenu (drop_down_symbol, Menubar);
  DglList->setStatusbarMessage ("List of all the active dialogs");

  // File menu items
  createFileMenuItems (File);

  // Dialog buttons
  createDialogButtons();

  // Statusbar at the bottom
  Statusbar = new FStatusBar (this);
  Statusbar->setMessage("Status bar message");

  // Generate data vector for the windows
  for (int n = 1; n <= 6; n++)
  {
    win_data* win_dat = new win_data;
    win_dat->is_open = false;
    win_dat->title = new FString();
    win_dat->title->sprintf("Window %d", n);
    windows.push_back(win_dat);
  }
}

//----------------------------------------------------------------------
Window::~Window()
{
  std::vector<win_data*>::iterator iter;
  iter = windows.begin();

  while ( iter != windows.end() )
  {
    win_data* win_dat = *iter;

    // Remove all callbacks before Window::cb_destroyWindow() will be called
    if ( win_dat->is_open && win_dat->dgl )
      win_dat->dgl->delCallbacks();

    delete win_dat->title;
    delete win_dat;
    iter = windows.erase(iter);
  }
}

//----------------------------------------------------------------------
void Window::createFileMenuItems (FMenu* File)
{
  // "File" menu item
  FMenuItem* New = new FMenuItem ("&New", File);
  New->setStatusbarMessage ("Create the windows");

  FMenuItem* Close = new FMenuItem ("&Close", File);
  Close->setStatusbarMessage ("Close the windows");

  FMenuItem* Line1 = new FMenuItem (File);
  Line1->setSeparator();

  FMenuItem* Next = new FMenuItem ("Ne&xt window", File);
  Next->addAccelerator (fc::Fmkey_npage);  // Meta/Alt + PgDn
  Next->setStatusbarMessage ("Switch to the next window");

  FMenuItem* Previous = new FMenuItem ("&Previous window", File);
  Previous->addAccelerator (fc::Fmkey_ppage);  // Meta/Alt + PgUp
  Previous->setStatusbarMessage ("Switch to the previous window");

  FMenuItem* Line2 = new FMenuItem (File);
  Line2->setSeparator();

  FMenuItem* Quit = new FMenuItem ("&Quit", File);
  Quit->addAccelerator (fc::Fmkey_x);  // Meta/Alt + X
  Quit->setStatusbarMessage ("Exit the program");

  // Add menu item callback
  addClickedCallback (New, &Window::cb_createWindows);
  addClickedCallback (Close, &Window::cb_closeWindows);
  addClickedCallback (Next, &Window::cb_next);
  addClickedCallback (Previous, &Window::cb_previous);
  addClickedCallback (Quit, &FApplication::cb_exitApp);
}

//----------------------------------------------------------------------
void Window::createDialogButtons()
{
  // Dialog buttons
  FButton* CreateButton = new FButton (this);
  CreateButton->setGeometry(2, 2, 9, 1);
  CreateButton->setText (L"&Create");

  FButton* CloseButton = new FButton (this);
  CloseButton->setGeometry(15, 2, 9, 1);
  CloseButton->setText (L"C&lose");

  FButton* QuitButton = new FButton (this);
  QuitButton->setGeometry(28, 2, 9, 1);
  QuitButton->setText (L"&Quit");

  // Add button callback
  addClickedCallback (CreateButton, &Window::cb_createWindows);
  addClickedCallback (CloseButton, &Window::cb_closeWindows);
  addClickedCallback (QuitButton, &FApplication::cb_exitApp);
}

//----------------------------------------------------------------------
void Window::activateWindow (FDialog* win)
{
  if ( ! win || win->isWindowHidden() || win->isWindowActive() )
    return;

  bool has_raised = FWindow::raiseWindow(win);
  win->activateDialog();

  if ( has_raised )
    win->redraw();

  updateTerminal();
}

//----------------------------------------------------------------------
void Window::adjustSize()
{
  std::vector<win_data*>::const_iterator iter, first;
  int w  = getRootWidget()->getWidth()
    , h  = getRootWidget()->getHeight()
    , X  = int(1 + (w - 40) / 2)
    , Y  = int(1 + (h - 22) / 2)
    , dx = ( w > 80 ) ? (w - 80) / 2 : 0
    , dy = ( h > 24 ) ? (h - 24) / 2 : 0;

  if ( Y < 2 )
    Y = 2;

  setPos (X, Y);
  iter = first = windows.begin();

  while ( iter != windows.end() )
  {
    if ( (*iter)->is_open )
    {
      int n = int(std::distance(first, iter))
        , x = dx + 5 + (n % 3) * 25 + int(n / 3) * 3
        , y = dy + 11 + int(n / 3) * 3;
      (*iter)->dgl->setPos (x, y);
    }

    ++iter;
  }

  FDialog::adjustSize();
}

//----------------------------------------------------------------------
void Window::addClickedCallback (FWidget* widget, WindowCallback call)
{
  FMemberCallback callback
      = reinterpret_cast<FWidget::FMemberCallback>(call);

  widget->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, callback)
  );
}

//----------------------------------------------------------------------
void Window::addClickedCallback (FWidget* widget, FAppCallback call)
{
  FMemberCallback callback
      = reinterpret_cast<FWidget::FMemberCallback>(call);

  widget->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, callback)
  );
}

//----------------------------------------------------------------------
void Window::onClose (FCloseEvent* ev)
{
  FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void Window::cb_createWindows (FWidget*, data_ptr)
{
  std::vector<win_data*>::const_iterator iter, first;
  iter = first = windows.begin();
  int w = getRootWidget()->getWidth()
    , h = getRootWidget()->getHeight()
    , dx = ( w > 80 ) ? (w - 80) / 2 : 0
    , dy = ( h > 24 ) ? (h - 24) / 2 : 0;

  while ( iter != windows.end() )
  {
    if ( ! (*iter)->is_open )
    {
      win_data* win_dat = *iter;
      SmallWindow* win = new SmallWindow(this);
      win_dat->dgl = win;
      win_dat->is_open = true;
      win->setText(*(win_dat)->title);
      int n = int(std::distance(first, iter))
        , x = dx + 5 + (n % 3) * 25 + int(n / 3) * 3
        , y = dy + 11 + int(n / 3) * 3;
      win->setGeometry (x, y, 20, 8);
      win->setMinimumSize (20, 8);
      win->setResizeable();
      win->show();

      win->addCallback
      (
        "destroy",
        F_METHOD_CALLBACK (this, &Window::cb_destroyWindow),
        static_cast<FWidget::data_ptr>(win_dat)
      );
    }

    ++iter;
  }

  activateWindow(this);
}

//----------------------------------------------------------------------
void Window::cb_closeWindows (FWidget*, data_ptr)
{
  if ( ! dialog_list || dialog_list->empty() )
    return;

  widgetList::const_iterator iter, first;
  iter = dialog_list->end();
  first = dialog_list->begin();
  activateWindow(this);

  do
  {
    --iter;

    if ( (*iter) != this )
      (*iter)->close();
  }
  while ( iter != first );
}

//----------------------------------------------------------------------
void Window::cb_next (FWidget*, data_ptr)
{
  if ( ! dialog_list || dialog_list->empty() )
    return;

  widgetList::const_iterator iter;
  iter = dialog_list->begin();

  while ( iter != dialog_list->end() )
  {
    if ( static_cast<FWindow*>(*iter)->isWindowActive() )
    {
      FDialog* next;
      widgetList::const_iterator next_element;
      next_element = iter;

      do
      {
        ++next_element;

        if ( next_element == dialog_list->end() )
          next_element = dialog_list->begin();

        next = static_cast<FDialog*>(*next_element);
      } while ( ! next->isEnabled()
             || ! next->acceptFocus()
             || ! next->isVisible()
             || ! next->isWindowWidget() );

      activateWindow(next);
      break;
    }

    ++iter;
  }
}

//----------------------------------------------------------------------
void Window::cb_previous (FWidget*, data_ptr)
{
  if ( ! dialog_list || dialog_list->empty() )
    return;

  widgetList::const_iterator iter;
  iter = dialog_list->end();

  do
  {
    --iter;

    if ( (*iter)->isDialogWidget()
      && static_cast<FWindow*>(*iter)->isWindowActive() )
    {
      FDialog* prev;
      widgetList::const_iterator prev_element;
      prev_element = iter;

      do
      {
        if ( prev_element == dialog_list->begin() )
          prev_element = dialog_list->end();

        --prev_element;
        prev = static_cast<FDialog*>(*prev_element);
      } while ( ! prev->isEnabled()
             || ! prev->acceptFocus()
             || ! prev->isVisible()
             || ! prev->isWindowWidget() );

      activateWindow(prev);
      break;
    }
  }
  while ( iter != dialog_list->begin() );
}

//----------------------------------------------------------------------
void Window::cb_destroyWindow (FWidget*, data_ptr data)
{
  win_data* win_dat = static_cast<win_data*>(data);

  if ( win_dat )
  {
    win_dat->is_open = false;
    win_dat->dgl = 0;
  }
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

int main (int argc, char* argv[])
{
  // Create the application object
  FApplication app (argc, argv);

  // Create main dialog object
  Window main_dlg (&app);
  main_dlg.setText ("Main window");
  main_dlg.setGeometry (int(1 + (app.getWidth() - 40) / 2), 2, 40, 6);

  // Set dialog main_dlg as main widget
  app.setMainWidget (&main_dlg);

  // Show and start the application
  main_dlg.show();
  return app.exec();
}
