// File: windows.cpp

#include "fapp.h"
#include "fdialog.h"
#include "fdialoglistmenu.h"
#include "flabel.h"
#include "fmenubar.h"
#include "fmenu.h"
#include "fmessagebox.h"
#include "fstatusbar.h"
#include "fstring.h"


//----------------------------------------------------------------------
// class Window
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class Window : public FDialog
{
 private:
   typedef struct
   {
     bool is_open;
     FString* title;
     FDialog* dgl;
   }
   win_data;
   std::vector<win_data*> windows;

 private:
   Window (const Window&);    // Disabled copy constructor
   Window& operator = (const Window&); // and operator '='
   void activateWindow (FDialog*);
   void onClose (FCloseEvent*);
   void cb_createWindows (FWidget*, void*);
   void cb_closeWindows (FWidget*, void*);
   void cb_next (FWidget*, void*);
   void cb_previous (FWidget*, void*);
   void cb_exitApp (FWidget*, void*);
   void cb_destroyWindow (FWidget*, void*);
   void adjustSize();

 public:
   explicit Window (FWidget* = 0);  // constructor
  ~Window();  // destructor
};
#pragma pack(pop)

//----------------------------------------------------------------------
Window::Window (FWidget* parent)
  : FDialog(parent)
  , windows()
{
  // menu bar
  FMenuBar* Menubar = new FMenuBar (this);

  // menu bar item
  FMenu* File = new FMenu ("&File", Menubar);
  File->setStatusbarMessage ("File management commands");

  // dialog list menu item
  FString drop_down_symbol = wchar_t(fc::BlackDownPointingTriangle);
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
  Next->addAccelerator (fc::Fmkey_npage); // Meta/Alt + PgDn
  Next->setStatusbarMessage ("Switch to the next window");

  FMenuItem* Previous = new FMenuItem ("&Previous window", File);
  Previous->addAccelerator (fc::Fmkey_ppage); // Meta/Alt + PgUp
  Previous->setStatusbarMessage ("Switch to the previous window");

  FMenuItem* Line2 = new FMenuItem (File);
  Line2->setSeparator();

  FMenuItem* Quit = new FMenuItem ("&Quit", File);
  Quit->addAccelerator (fc::Fmkey_x); // Meta/Alt + X
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
    _METHOD_CALLBACK (this, &Window::cb_createWindows)
  );

  Close->addCallback
  (
    "clicked",
    _METHOD_CALLBACK (this, &Window::cb_closeWindows)
  );

  Next->addCallback
  (
    "clicked",
    _METHOD_CALLBACK (this, &Window::cb_next)
  );

  Previous->addCallback
  (
    "clicked",
    _METHOD_CALLBACK (this, &Window::cb_previous)
  );

  Quit->addCallback
  (
    "clicked",
    _METHOD_CALLBACK (this, &Window::cb_exitApp)
  );

  // Add button callback
  CreateButton->addCallback
  (
    "clicked",
    _METHOD_CALLBACK (this, &Window::cb_createWindows)
  );

  CloseButton->addCallback
  (
    "clicked",
    _METHOD_CALLBACK (this, &Window::cb_closeWindows)
  );

  QuitButton->addCallback
  (
    "clicked",
    _METHOD_CALLBACK (this, &Window::cb_exitApp)
  );

  for (int n=1; n <= 6; n++)
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
  if ( win && ! win->isHiddenWindow() && ! win->isActiveWindow() )
  {
    bool has_raised = FWindow::raiseWindow(win);
    win->activateDialog();

    if ( has_raised )
      win->redraw();

    updateTerminal();
  }
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
void Window::cb_createWindows (FWidget*, void*)
{
  int w,h,dx,dy;
  std::vector<win_data*>::const_iterator iter, begin;
  iter = begin = windows.begin();
  w = getRootWidget()->getWidth();
  h = getRootWidget()->getHeight();
  dx = (w > 80) ? (w - 80) / 2 : 0;
  dy = (h > 24) ? (h - 24) / 2 : 0;

  while ( iter != windows.end() )
  {
    if ( ! (*iter)->is_open )
    {
      int x,y,n;
      win_data* win_dat = *iter;
      FDialog* win = new FDialog(this);
      win_dat->dgl = win;
      win_dat->is_open = true;
      win->setText(*(win_dat)->title);
      n = int(std::distance(begin, iter));
      x = dx + 5 + (n%3)*25 + int(n/3)*3;
      y = dy + 11 + int(n/3)*3;
      win->setGeometry (x, y, 20, 8);
      win->setResizeable();
      win->show();

      win->addCallback
      (
        "destroy",
        _METHOD_CALLBACK (this, &Window::cb_destroyWindow),
        static_cast<FWidget::data_ptr>(win_dat)
      );
    }

    ++iter;
  }

  activateWindow(this);
}

//----------------------------------------------------------------------
void Window::cb_closeWindows (FWidget*, void*)
{
  if ( ! dialog_list || dialog_list->empty() )
    return;

  widgetList::const_iterator iter, begin;
  iter = dialog_list->end();
  begin = dialog_list->begin();
  activateWindow(this);

  do
  {
    --iter;

    if ( (*iter) != this )
      (*iter)->close();
  }
  while ( iter != begin );
}

//----------------------------------------------------------------------
void Window::cb_next (FWidget*, void*)
{
  if ( ! dialog_list || dialog_list->empty() )
    return;

  widgetList::const_iterator iter;
  iter = dialog_list->begin();

  while ( iter != dialog_list->end() )
  {
    if ( static_cast<FWindow*>(*iter)->isActiveWindow() )
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
      } while (  ! next->isEnabled()
              || ! next->acceptFocus()
              || ! next->isVisible()
              || ! next->isWindow() );

      activateWindow(next);
      break;
    }

    ++iter;
  }
}

//----------------------------------------------------------------------
void Window::cb_previous (FWidget*, void*)
{
  if ( ! dialog_list || dialog_list->empty() )
    return;

  widgetList::const_iterator iter;
  iter = dialog_list->end();

  do
  {
    --iter;

    if ( (*iter)->isDialog()
       && static_cast<FWindow*>(*iter)->isActiveWindow() )
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
      } while (  ! prev->isEnabled()
              || ! prev->acceptFocus()
              || ! prev->isVisible()
              || ! prev->isWindow() );

      activateWindow(prev);
      break;
    }
  }
  while ( iter != dialog_list->begin() );
}

//----------------------------------------------------------------------
void Window::cb_exitApp (FWidget*, void*)
{
  close();
}

//----------------------------------------------------------------------
void Window::cb_destroyWindow (FWidget*, void* data_ptr)
{
  win_data* win_dat = static_cast<win_data*>(data_ptr);
  if ( win_dat )
    win_dat->is_open = false;
}

//----------------------------------------------------------------------
void Window::adjustSize()
{
  int w,h,X,Y,dx,dy;
  std::vector<win_data*>::const_iterator iter, begin;
  w  = getRootWidget()->getWidth();
  h  = getRootWidget()->getHeight();
  X  = int(1 + (w - 40) / 2);
  Y  = int(1 + (h - 22) / 2);
  dx = (w > 80) ? (w - 80) / 2 : 0;
  dy = (h > 24) ? (h - 24) / 2 : 0;

  if ( Y < 2)
    Y = 2;

  setPos (X, Y, false);
  iter = begin = windows.begin();

  while ( iter != windows.end() )
  {
    if ( (*iter)->is_open )
    {
      int x,y,n;
      n = int(std::distance(begin, iter));
      x = dx + 5 + (n%3)*25 + int(n/3)*3;
      y = dy + 11 + int(n/3)*3;
      (*iter)->dgl->setPos (x, y, false);
    }

    ++iter;
  }

  FDialog::adjustSize();
}

//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

int main (int argc, char* argv[])
{
  if ( argv[1] && (  strcmp(argv[1], "--help") == 0
                  || strcmp(argv[1], "-h") == 0 ) )
  {
    std::cout << "Generic options:" << std::endl
              << "  -h, --help                  "
              << "Display this help and exit" << std::endl;
    FApplication::print_cmd_Options();
    exit(EXIT_SUCCESS);
  }

  FApplication app (argc, argv);

  Window main_dlg (&app);
  main_dlg.setText ("Main window");
  main_dlg.setGeometry (int(1+(app.getWidth()-40)/2), 2, 40, 6);

  app.setMainWidget (&main_dlg);
  main_dlg.show();

  return app.exec();
}
