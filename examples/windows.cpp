// File: windows.cpp

#include <vector>

#include <final/fapplication.h>
#include <final/fdialog.h>
#include <final/fdialoglistmenu.h>
#include <final/flabel.h>
#include <final/fmenubar.h>
#include <final/fmenu.h>
#include <final/fmessagebox.h>
#include <final/fstatusbar.h>
#include <final/fstring.h>

//----------------------------------------------------------------------
// class smallWindow
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class smallWindow : public FDialog
{
  public:
    // Constructor
    explicit smallWindow (FWidget* = 0);

    // Destructor
    ~smallWindow();

  private:
    // Disable copy constructor
    smallWindow (const smallWindow&);

    // Disable assignment operator (=)
    smallWindow& operator = (const smallWindow&);

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
smallWindow::smallWindow (FWidget* parent)
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
smallWindow::~smallWindow()
{
  delOwnTimer();
}

//----------------------------------------------------------------------
void smallWindow::adjustSize()
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
void smallWindow::onShow (FShowEvent*)
{
  addTimer(1000);
}

//----------------------------------------------------------------------
void smallWindow::onTimer (FTimerEvent*)
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
    // Typedef
    typedef struct
    {
      bool     is_open;
      FString* title;
      FDialog* dgl;
    }
    win_data;

    // Disable copy constructor
    Window (const Window&);

    // Disable assignment operator (=)
    Window& operator = (const Window&);

    // Method
    void activateWindow (FDialog*);
    void adjustSize();

    // Event handlers
    void onClose (FCloseEvent*);

    // Callback methods
    void cb_createWindows (FWidget*, data_ptr);
    void cb_closeWindows (FWidget*, data_ptr);
    void cb_next (FWidget*, data_ptr);
    void cb_previous (FWidget*, data_ptr);
    void cb_exitApp (FWidget*, data_ptr);
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
  FString drop_down_symbol;
  // menu bar
  FMenuBar* Menubar = new FMenuBar (this);

  // menu bar item
  FMenu* File = new FMenu ("&File", Menubar);
  File->setStatusbarMessage ("File management commands");

  // dialog list menu item
  drop_down_symbol = wchar_t(fc::BlackDownPointingTriangle);

  FDialogListMenu* DglList = new FDialogListMenu (drop_down_symbol, Menubar);
  DglList->setStatusbarMessage ("List of all the active dialogs");

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

  // Statusbar at the bottom
  FStatusBar* statusbar = new FStatusBar (this);
  statusbar->setMessage("Status bar message");

  // Buttons
  FButton* CreateButton = new FButton (this);
  CreateButton->setGeometry(2, 2, 9, 1);
  CreateButton->setText (L"&Create");

  FButton* CloseButton = new FButton (this);
  CloseButton->setGeometry(15, 2, 9, 1);
  CloseButton->setText (L"C&lose");

  FButton* QuitButton = new FButton (this);
  QuitButton->setGeometry(28, 2, 9, 1);
  QuitButton->setText (L"&Quit");

  // Add menu item callback
  New->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &Window::cb_createWindows)
  );

  Close->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &Window::cb_closeWindows)
  );

  Next->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &Window::cb_next)
  );

  Previous->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &Window::cb_previous)
  );

  Quit->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &Window::cb_exitApp)
  );

  // Add button callback
  CreateButton->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &Window::cb_createWindows)
  );

  CloseButton->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &Window::cb_closeWindows)
  );

  QuitButton->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &Window::cb_exitApp)
  );

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
    delete (*iter)->title;
    delete *iter;
    iter = windows.erase(iter);
  }
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
  int w, h, X, Y, dx, dy;
  std::vector<win_data*>::const_iterator iter, first;
  w  = getRootWidget()->getWidth();
  h  = getRootWidget()->getHeight();
  X  = int(1 + (w - 40) / 2);
  Y  = int(1 + (h - 22) / 2);
  dx = ( w > 80 ) ? (w - 80) / 2 : 0;
  dy = ( h > 24 ) ? (h - 24) / 2 : 0;

  if ( Y < 2)
    Y = 2;

  setPos (X, Y);
  iter = first = windows.begin();

  while ( iter != windows.end() )
  {
    if ( (*iter)->is_open )
    {
      int x, y, n;
      n = int(std::distance(first, iter));
      x = dx + 5 + (n % 3) * 25 + int(n / 3) * 3;
      y = dy + 11 + int(n / 3) * 3;
      (*iter)->dgl->setPos (x, y);
    }

    ++iter;
  }

  FDialog::adjustSize();
}

//----------------------------------------------------------------------
void Window::onClose (FCloseEvent* ev)
{
  int ret = FMessageBox::info ( this, "Quit"
                              , "Do you really want\n"
                                "to quit the program ?"
                              , FMessageBox::Yes
                              , FMessageBox::No );
  if ( ret == FMessageBox::Yes )
  {
    cb_closeWindows(0,0);
    ev->accept();
  }
  else
    ev->ignore();
}

//----------------------------------------------------------------------
void Window::cb_createWindows (FWidget*, data_ptr)
{
  int w, h, dx, dy;
  std::vector<win_data*>::const_iterator iter, first;
  iter = first = windows.begin();
  w = getRootWidget()->getWidth();
  h = getRootWidget()->getHeight();
  dx = ( w > 80 ) ? (w - 80) / 2 : 0;
  dy = ( h > 24 ) ? (h - 24) / 2 : 0;

  while ( iter != windows.end() )
  {
    if ( ! (*iter)->is_open )
    {
      int x, y, n;
      win_data* win_dat = *iter;
      FDialog* win = new smallWindow(this);
      win_dat->dgl = win;
      win_dat->is_open = true;
      win->setText(*(win_dat)->title);
      n = int(std::distance(first, iter));
      x = dx + 5 + (n % 3) * 25 + int(n / 3) * 3;
      y = dy + 11 + int(n / 3) * 3;
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
void Window::cb_exitApp (FWidget*, data_ptr)
{
  close();
}

//----------------------------------------------------------------------
void Window::cb_destroyWindow (FWidget*, data_ptr data)
{
  win_data* win_dat = static_cast<win_data*>(data);

  if ( win_dat )
    win_dat->is_open = false;
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
