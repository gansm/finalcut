/***********************************************************************
* windows.cpp - Shows window handling                                  *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2016-2019 Markus Gans                                      *
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

namespace fc = finalcut::fc;
using finalcut::FPoint;
using finalcut::FSize;


//----------------------------------------------------------------------
// class SmallWindow
//----------------------------------------------------------------------

class SmallWindow : public finalcut::FDialog
{
  public:
    // Constructor
    explicit SmallWindow (finalcut::FWidget* = nullptr);

    // Disable copy constructor
    SmallWindow (const SmallWindow&) = delete;

    // Destructor
    ~SmallWindow();

    // Disable assignment operator (=)
    SmallWindow& operator = (const SmallWindow&) = delete;

  private:
    // Method
    void adjustSize() override;

    // Event handlers
    void onShow (finalcut::FShowEvent*) override;
    void onTimer (finalcut::FTimerEvent*) override;

    // Data members
    finalcut::FLabel left_arrow{this};
    finalcut::FLabel right_arrow{this};
    finalcut::FLabel top_left_label{this};
    finalcut::FLabel top_right_label{this};
    finalcut::FLabel bottom_label{this};
};


//----------------------------------------------------------------------
SmallWindow::SmallWindow (finalcut::FWidget* parent)
  : finalcut::FDialog(parent)
{
  const finalcut::FWidgetColors& wc = getFWidgetColors();
  wchar_t arrow_up, arrow_down;
  arrow_up = fc::BlackUpPointingTriangle;
  arrow_down = fc::BlackDownPointingTriangle;

  left_arrow = arrow_up;
  left_arrow.setForegroundColor (wc.label_inactive_fg);
  left_arrow.setEmphasis();
  left_arrow.ignorePadding();
  left_arrow.setGeometry (FPoint(2, 2), FSize(1, 1));

  right_arrow = arrow_up;
  right_arrow.setForegroundColor (wc.label_inactive_fg);
  right_arrow.setEmphasis();
  right_arrow.ignorePadding();
  right_arrow.setGeometry (FPoint(int(getWidth()) - 1, 2), FSize(1, 1));

  top_left_label = "menu";
  top_left_label.setForegroundColor (wc.label_inactive_fg);
  top_left_label.setEmphasis();
  top_left_label.setGeometry (FPoint(1, 1), FSize(6, 1));

  top_right_label = "zoom";
  top_right_label.setAlignment (fc::alignRight);
  top_right_label.setForegroundColor (wc.label_inactive_fg);
  top_right_label.setEmphasis();
  top_right_label.setGeometry (FPoint(int(getClientWidth()) - 5, 1), FSize(6, 1));

  finalcut::FString bottom_label_text ( "resize\n"
                                        "corner\n" );
  bottom_label_text += arrow_down;
  bottom_label = bottom_label_text;
  bottom_label.setAlignment (fc::alignRight);
  bottom_label.setForegroundColor (wc.label_inactive_fg);
  bottom_label.setEmphasis();
  bottom_label.setGeometry (FPoint(13, 3), FSize(6, 3));
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
    top_right_label = "unzoom";
    bottom_label.hide();
  }
  else
  {
    top_right_label = "zoom";
    bottom_label.setVisible();
  }

  finalcut::FDialog::adjustSize();
  right_arrow.setGeometry ( FPoint(int(getWidth()) - 1, 2)
                          , FSize(1, 1) );
  top_right_label.setGeometry ( FPoint(int(getClientWidth()) - 5, 1)
                              , FSize(6, 1) );
  bottom_label.setGeometry ( FPoint(1, int(getClientHeight()) - 2)
                          , FSize(getClientWidth(), 3) );
}

//----------------------------------------------------------------------
void SmallWindow::onShow (finalcut::FShowEvent*)
{
  addTimer(1000);
}

//----------------------------------------------------------------------
void SmallWindow::onTimer (finalcut::FTimerEvent*)
{
  left_arrow.unsetEmphasis();
  left_arrow.redraw();
  right_arrow.unsetEmphasis();
  right_arrow.redraw();
  top_left_label.unsetEmphasis();
  top_left_label.redraw();
  top_right_label.unsetEmphasis();
  top_right_label.redraw();
  bottom_label.unsetEmphasis();
  bottom_label.redraw();
  updateTerminal();
  delOwnTimer();
}


//----------------------------------------------------------------------
// class Window
//----------------------------------------------------------------------

class Window : public finalcut::FDialog
{
  public:
    // Constructor
    explicit Window (finalcut::FWidget* = nullptr);

    // Disable copy constructor
    Window (const Window&) = delete;

    // Destructor
    ~Window();

    // Disable assignment operator (=)
    Window& operator = (const Window&) = delete;

  private:
    // Typedefs
    typedef void (Window::*WindowCallback)(finalcut::FWidget*, FDataPtr);
    typedef void (finalcut::FApplication::*FAppCallback)(finalcut::FWidget*, FDataPtr);

    struct win_data
    {
        // Constructor
        win_data() = default;
        // Disable copy constructor
        win_data (const win_data&) = delete;

        // Disable assignment operator (=)
        win_data& operator = (const win_data&) = delete;

        // Data members
        bool is_open{false};
        finalcut::FString title{};
        SmallWindow* dgl{nullptr};
    };

    // Method
    void configureFileMenuItems();
    void configureDialogButtons();
    void activateWindow (finalcut::FDialog*);
    void adjustSize() override;
    void addClickedCallback (finalcut::FWidget*, WindowCallback);
    void addClickedCallback (finalcut::FWidget*, FAppCallback);

    // Event handlers
    void onClose (finalcut::FCloseEvent*) override;

    // Callback methods
    void cb_createWindows (finalcut::FWidget*, FDataPtr);
    void cb_closeWindows (finalcut::FWidget*, FDataPtr);
    void cb_next (finalcut::FWidget*, FDataPtr);
    void cb_previous (finalcut::FWidget*, FDataPtr);
    void cb_destroyWindow (finalcut::FWidget*, FDataPtr);

    // Data members
    std::vector<win_data*>    windows{};
    finalcut::FString         drop_down_symbol{fc::BlackDownPointingTriangle};
    finalcut::FMenuBar        Menubar{this};
    finalcut::FMenu           File{"&File", &Menubar};
    finalcut::FDialogListMenu DglList{drop_down_symbol, &Menubar};
    finalcut::FStatusBar      Statusbar{this};
    finalcut::FMenuItem       New{"&New", &File};
    finalcut::FMenuItem       Close{"&Close", &File};
    finalcut::FMenuItem       Line1{&File};
    finalcut::FMenuItem       Next{"Ne&xt window", &File};
    finalcut::FMenuItem       Previous{"&Previous window", &File};
    finalcut::FMenuItem       Line2{&File};
    finalcut::FMenuItem       Quit{"&Quit", &File};
    finalcut::FButton         CreateButton{this};
    finalcut::FButton         CloseButton{this};
    finalcut::FButton         QuitButton{this};
};


//----------------------------------------------------------------------
Window::Window (finalcut::FWidget* parent)
  : finalcut::FDialog(parent)
{
  // Menu bar item
  File.setStatusbarMessage ("File management commands");

  // Dialog list menu item
  DglList.setStatusbarMessage ("List of all the active dialogs");

  // File menu items
  configureFileMenuItems();

  // Dialog buttons
  configureDialogButtons();

  // Statusbar at the bottom
  Statusbar.setMessage("Status bar message");

  // Generate data vector for the windows
  for (int n{1}; n <= 6; n++)
  {
    auto win_dat = new win_data;
    win_dat->title.sprintf("Window %1d", n);
    windows.push_back(win_dat);
  }
}

//----------------------------------------------------------------------
Window::~Window()
{
  auto iter = windows.begin();

  while ( iter != windows.end() )
  {
    auto win_dat = *iter;

    // Remove all callbacks before Window::cb_destroyWindow() will be called
    if ( win_dat->is_open && win_dat->dgl )
      win_dat->dgl->delCallbacks();

    delete win_dat;
    iter = windows.erase(iter);
  }
}

//----------------------------------------------------------------------
void Window::configureFileMenuItems()
{
  // "File" menu item setting
  New.setStatusbarMessage ("Create the windows");
  Close.setStatusbarMessage ("Close the windows");
  Line1.setSeparator();
  Next.addAccelerator (fc::Fmkey_npage);  // Meta/Alt + PgDn
  Next.setStatusbarMessage ("Switch to the next window");
  Previous.addAccelerator (fc::Fmkey_ppage);  // Meta/Alt + PgUp
  Previous.setStatusbarMessage ("Switch to the previous window");
  Line2.setSeparator();
  Quit.addAccelerator (fc::Fmkey_x);  // Meta/Alt + X
  Quit.setStatusbarMessage ("Exit the program");

  // Add menu item callback
  addClickedCallback (&New, &Window::cb_createWindows);
  addClickedCallback (&Close, &Window::cb_closeWindows);
  addClickedCallback (&Next, &Window::cb_next);
  addClickedCallback (&Previous, &Window::cb_previous);
  addClickedCallback (&Quit, &finalcut::FApplication::cb_exitApp);
}

//----------------------------------------------------------------------
void Window::configureDialogButtons()
{
  // Dialog buttons
  CreateButton.setGeometry (FPoint(2, 2), FSize(9, 1));
  CreateButton.setText (L"&Create");
  CloseButton.setGeometry (FPoint(15, 2), FSize(9, 1));
  CloseButton.setText (L"C&lose");
  QuitButton.setGeometry (FPoint(28, 2), FSize(9, 1));
  QuitButton.setText (L"&Quit");

  // Add button callback
  addClickedCallback (&CreateButton, &Window::cb_createWindows);
  addClickedCallback (&CloseButton, &Window::cb_closeWindows);
  addClickedCallback (&QuitButton, &finalcut::FApplication::cb_exitApp);
}

//----------------------------------------------------------------------
void Window::activateWindow (finalcut::FDialog* win)
{
  if ( ! win || win->isWindowHidden() || win->isWindowActive() )
    return;

  bool has_raised = finalcut::FWindow::raiseWindow(win);
  win->activateDialog();

  if ( has_raised )
    win->redraw();

  updateTerminal();
}

//----------------------------------------------------------------------
void Window::adjustSize()
{
  std::size_t w  = getDesktopWidth();
  std::size_t h  = getDesktopHeight();
  int X  = int(1 + (w - 40) / 2)
    , Y  = int(1 + (h - 22) / 2)
    , dx = ( w > 80 ) ? int(w - 80) / 2 : 0
    , dy = ( h > 24 ) ? int(h - 24) / 2 : 0;

  if ( Y < 2 )
    Y = 2;

  setPos (FPoint(X, Y));
  auto first = windows.begin();
  auto iter = first;

  while ( iter != windows.end() )
  {
    if ( (*iter)->is_open )
    {
      int n = int(std::distance(first, iter))
        , x = dx + 5 + (n % 3) * 25 + int(n / 3) * 3
        , y = dy + 11 + int(n / 3) * 3;
      (*iter)->dgl->setPos (FPoint(x, y));
    }

    ++iter;
  }

  finalcut::FDialog::adjustSize();
}

//----------------------------------------------------------------------
void Window::addClickedCallback ( finalcut::FWidget* widget
                                , WindowCallback call )
{
  FMemberCallback callback
      = reinterpret_cast<finalcut::FWidget::FMemberCallback>(call);

  widget->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, callback)
  );
}

//----------------------------------------------------------------------
void Window::addClickedCallback ( finalcut::FWidget* widget
                                , FAppCallback call )
{
  FMemberCallback callback
      = reinterpret_cast<finalcut::FWidget::FMemberCallback>(call);

  widget->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, callback)
  );
}

//----------------------------------------------------------------------
void Window::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void Window::cb_createWindows (finalcut::FWidget*, FDataPtr)
{
  auto first = windows.begin();
  auto iter = first;
  auto w = getRootWidget()->getWidth();
  auto h = getRootWidget()->getHeight();
  int dx = ( w > 80 ) ? int(w - 80) / 2 : 0;
  int dy = ( h > 24 ) ? int(h - 24) / 2 : 0;

  while ( iter != windows.end() )
  {
    if ( ! (*iter)->is_open )
    {
      auto win_dat = *iter;
      auto win = new SmallWindow(this);
      win_dat->dgl = win;
      win_dat->is_open = true;
      win->setText(win_dat->title);
      int n = int(std::distance(first, iter))
        , x = dx + 5 + (n % 3) * 25 + int(n / 3) * 3
        , y = dy + 11 + int(n / 3) * 3;
      win->setGeometry (FPoint(x, y), FSize(20, 8));
      win->setMinimumSize (FSize(20, 8));
      win->setResizeable();
      win->show();

      win->addCallback
      (
        "destroy",
        F_METHOD_CALLBACK (this, &Window::cb_destroyWindow),
        static_cast<FDataPtr>(win_dat)
      );
    }

    ++iter;
  }

  activateWindow(this);
}

//----------------------------------------------------------------------
void Window::cb_closeWindows (finalcut::FWidget*, FDataPtr)
{
  if ( ! getDialogList() || getDialogList()->empty() )
    return;

  auto iter = getDialogList()->end();
  auto first = getDialogList()->begin();
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
void Window::cb_next (finalcut::FWidget*, FDataPtr)
{
  if ( ! getDialogList() || getDialogList()->empty() )
    return;

  auto iter = getDialogList()->begin();

  while ( iter != getDialogList()->end() )
  {
    if ( static_cast<finalcut::FWindow*>(*iter)->isWindowActive() )
    {
      finalcut::FDialog* next;
      auto next_element = iter;

      do
      {
        ++next_element;

        if ( next_element == getDialogList()->end() )
          next_element = getDialogList()->begin();

        next = static_cast<finalcut::FDialog*>(*next_element);
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
void Window::cb_previous (finalcut::FWidget*, FDataPtr)
{
  if ( ! getDialogList() || getDialogList()->empty() )
    return;

  auto iter = getDialogList()->end();

  do
  {
    --iter;

    if ( (*iter)->isDialogWidget()
      && static_cast<finalcut::FWindow*>(*iter)->isWindowActive() )
    {
      finalcut::FDialog* prev;
      auto prev_element = iter;

      do
      {
        if ( prev_element == getDialogList()->begin() )
          prev_element = getDialogList()->end();

        --prev_element;
        prev = static_cast<finalcut::FDialog*>(*prev_element);
      } while ( ! prev->isEnabled()
             || ! prev->acceptFocus()
             || ! prev->isVisible()
             || ! prev->isWindowWidget() );

      activateWindow(prev);
      break;
    }
  }
  while ( iter != getDialogList()->begin() );
}

//----------------------------------------------------------------------
void Window::cb_destroyWindow (finalcut::FWidget*, FDataPtr data)
{
  auto win_dat = static_cast<win_data*>(data);

  if ( win_dat )
  {
    win_dat->is_open = false;
    win_dat->dgl = nullptr;
  }
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

int main (int argc, char* argv[])
{
  // Create the application object
  finalcut::FApplication app (argc, argv);

  // Create main dialog object
  Window main_dlg (&app);
  main_dlg.setText ("Main window");
  main_dlg.setGeometry ( FPoint(int(1 + (app.getWidth() - 40) / 2), 2)
                       , FSize(40, 6) );

  // Set dialog main_dlg as main widget
  app.setMainWidget (&main_dlg);

  // Show and start the application
  main_dlg.show();
  return app.exec();
}
