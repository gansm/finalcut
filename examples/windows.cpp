/***********************************************************************
* windows.cpp - Shows window handling                                  *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2016-2020 Markus Gans                                      *
*                                                                      *
* FINAL CUT is free software; you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as       *
* published by the Free Software Foundation; either version 3 of       *
* the License, or (at your option) any later version.                  *
*                                                                      *
* FINAL CUT is distributed in the hope that it will be useful, but     *
* WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU Lesser General Public License for more details.                  *
*                                                                      *
* You should have received a copy of the GNU Lesser General Public     *
* License along with this program.  If not, see                        *
* <http://www.gnu.org/licenses/>.                                      *
***********************************************************************/

#include <utility>
#include <vector>

#include <final/final.h>

namespace fc = finalcut::fc;
using finalcut::FPoint;
using finalcut::FSize;


//----------------------------------------------------------------------
// class SmallWindow
//----------------------------------------------------------------------

class SmallWindow final : public finalcut::FDialog
{
  public:
    // Constructor
    explicit SmallWindow (finalcut::FWidget* = nullptr);

    // Disable copy constructor
    SmallWindow (const SmallWindow&) = delete;

    // Destructor
    ~SmallWindow() override = default;

    // Disable copy assignment operator (=)
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
  : finalcut::FDialog{parent}
{
  const auto& wc = getColorTheme();
  const auto arrow_up = finalcut::UniChar::BlackUpPointingTriangle;
  const auto arrow_down = finalcut::UniChar::BlackDownPointingTriangle;

  left_arrow = arrow_up;
  left_arrow.setForegroundColor (wc->label_inactive_fg);
  left_arrow.setEmphasis();
  left_arrow.ignorePadding();
  left_arrow.setGeometry (FPoint{2, 2}, FSize{1, 1});

  right_arrow = arrow_up;
  right_arrow.setForegroundColor (wc->label_inactive_fg);
  right_arrow.setEmphasis();
  right_arrow.ignorePadding();
  right_arrow.setGeometry (FPoint{int(getWidth()) - 1, 2}, FSize{1, 1});

  top_left_label.setText("menu");
  top_left_label.setForegroundColor (wc->label_inactive_fg);
  top_left_label.setEmphasis();
  top_left_label.setGeometry (FPoint{1, 1}, FSize{6, 1});

  top_right_label.setText("zoom");
  top_right_label.setAlignment (finalcut::Align::Right);
  top_right_label.setForegroundColor (wc->label_inactive_fg);
  top_right_label.setEmphasis();
  top_right_label.setGeometry (FPoint{int(getClientWidth()) - 5, 1}, FSize{6, 1});

  finalcut::FString bottom_label_text { "resize\n"
                                        "corner\n" };
  bottom_label_text += arrow_down;
  bottom_label = bottom_label_text;
  bottom_label.setAlignment (finalcut::Align::Right);
  bottom_label.setForegroundColor (wc->label_inactive_fg);
  bottom_label.setEmphasis();
  bottom_label.setGeometry (FPoint{13, 3}, FSize{6, 3});
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
    bottom_label.show();
  }

  finalcut::FDialog::adjustSize();
  right_arrow.setGeometry ( FPoint{int(getWidth()) - 1, 2}
                          , FSize{1, 1} );
  top_right_label.setGeometry ( FPoint{int(getClientWidth()) - 5, 1}
                              , FSize{6, 1} );
  bottom_label.setGeometry ( FPoint{1, int(getClientHeight()) - 2}
                           , FSize{getClientWidth(), 3} );
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
  delOwnTimers();
}


//----------------------------------------------------------------------
// class Window
//----------------------------------------------------------------------

class Window final : public finalcut::FDialog
{
  public:
    // Constructor
    explicit Window (finalcut::FWidget* = nullptr);

    // Disable copy constructor
    Window (const Window&) = delete;

    // Destructor
    ~Window() override;

    // Disable copy assignment operator (=)
    Window& operator = (const Window&) = delete;

  private:
    struct WinData
    {
        // Constructor
        WinData() = default;

        // copy constructor
        WinData (const WinData&) = default;

        // move constructor
        WinData (WinData&&) noexcept = default;

        // copy assignment operator (=)
        WinData& operator = (const WinData&) = default;

        // move assignment operator (=)
        WinData& operator = (WinData&&) noexcept = default;

        // Data members
        bool is_open{false};
        finalcut::FString title{};
        SmallWindow* dgl{nullptr};
    };

    // Method
    void configureFileMenuItems();
    void configureDialogButtons();
    void activateWindow (finalcut::FDialog*) const;
    void adjustSize() override;
    template <typename InstanceT
            , typename CallbackT
            , typename... Args>
    void addClickedCallback ( finalcut::FWidget*
                            , InstanceT&&, CallbackT&&, Args&&... );
    template <typename IteratorT>
    finalcut::FDialog* getNext (IteratorT);
    template <typename IteratorT>
    finalcut::FDialog* getPrevious (IteratorT iter);

    // Event handlers
    void onClose (finalcut::FCloseEvent*) override;

    // Callback methods
    void cb_createWindows();
    void cb_closeWindows();
    void cb_next();
    void cb_previous();
    void cb_destroyWindow (WinData&) const;

    // Data members
    std::vector<WinData>      windows{};
    finalcut::FString         drop_down_symbol{finalcut::UniChar::BlackDownPointingTriangle};
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
  : finalcut::FDialog{parent}
{
  FDialog::setSize ({40, 6});

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
  for (uInt n{1}; n < 7; n++)
  {
    WinData win_dat;
    win_dat.title.sprintf("Window %1u", n);
    windows.emplace_back(std::move(win_dat));
  }
}

//----------------------------------------------------------------------
Window::~Window()
{
  auto iter = windows.begin();

  while ( iter != windows.end() )
  {
    auto& win_dat = *iter;

    // Remove all callbacks before Window::cb_destroyWindow() will be called
    if ( win_dat.is_open && win_dat.dgl )
      win_dat.dgl->delCallback();

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
  Next.addAccelerator (finalcut::FKey::Meta_page_down);  // Meta/Alt + PgDn
  Next.setStatusbarMessage ("Switch to the next window");
  Previous.addAccelerator (finalcut::FKey::Meta_page_up);  // Meta/Alt + PgUp
  Previous.setStatusbarMessage ("Switch to the previous window");
  Line2.setSeparator();
  Quit.addAccelerator (finalcut::FKey::Meta_x);  // Meta/Alt + X
  Quit.setStatusbarMessage ("Exit the program");

  // Add menu item callback
  addClickedCallback (&New, this, &Window::cb_createWindows);
  addClickedCallback (&Close, this, &Window::cb_closeWindows);
  addClickedCallback (&Next, this, &Window::cb_next);
  addClickedCallback (&Previous, this, &Window::cb_previous);
  addClickedCallback ( &Quit
                     , finalcut::getFApplication()
                     , &finalcut::FApplication::cb_exitApp
                     , this );
}

//----------------------------------------------------------------------
void Window::configureDialogButtons()
{
  // Dialog buttons
  CreateButton.setGeometry (FPoint{2, 2}, FSize{9, 1});
  CreateButton.setText (L"&Create");
  CloseButton.setGeometry (FPoint{15, 2}, FSize{9, 1});
  CloseButton.setText (L"C&lose");
  QuitButton.setGeometry (FPoint{28, 2}, FSize{9, 1});
  QuitButton.setText (L"&Quit");

  // Add button callback
  addClickedCallback (&CreateButton, this, &Window::cb_createWindows);
  addClickedCallback (&CloseButton, this, &Window::cb_closeWindows);
  addClickedCallback ( &QuitButton
                     , finalcut::getFApplication()
                     , &finalcut::FApplication::cb_exitApp
                     , this );
}

//----------------------------------------------------------------------
void Window::activateWindow (finalcut::FDialog* win) const
{
  if ( ! win || win->isWindowHidden() || win->isWindowActive() )
    return;

  const bool has_raised = finalcut::FWindow::raiseWindow(win);
  win->activateDialog();

  if ( has_raised )
    win->redraw();
}

//----------------------------------------------------------------------
void Window::adjustSize()
{
  finalcut::FDialog::adjustSize();

  const std::size_t w = getDesktopWidth();
  const std::size_t h = getDesktopHeight();
  const auto X = int(1 + (w - 40) / 2);
  auto Y = int(1 + (h - 22) / 2);
  const int dx = ( w > 80 ) ? int(w - 80) / 2 : 0;
  const int dy = ( h > 24 ) ? int(h - 24) / 2 : 0;

  if ( Y < 2 )
    Y = 2;

  setPos (FPoint{X, Y});
  const auto& first = windows.begin();
  auto iter = first;

  while ( iter != windows.end() )
  {
    if ( (*iter).is_open )
    {
      const auto n = int(std::distance(first, iter));
      const int x = dx + 5 + (n % 3) * 25 + int(n / 3) * 3;
      const int y = dy + 11 + int(n / 3) * 3;
      (*iter).dgl->setPos (FPoint{x, y});
    }

    ++iter;
  }
}

//----------------------------------------------------------------------
template <typename InstanceT
        , typename CallbackT
        , typename... Args>
void Window::addClickedCallback ( finalcut::FWidget* widget
                                , InstanceT&& instance
                                , CallbackT&& callback
                                , Args&&... args )
{
  widget->addCallback
  (
    "clicked",
    std::bind ( std::forward<CallbackT>(callback)
              , std::forward<InstanceT>(instance)
              , std::forward<Args>(args)... )
  );
}

//----------------------------------------------------------------------
template <typename IteratorT>
finalcut::FDialog* Window::getNext (IteratorT iter)
{
  auto next_element = iter;
  finalcut::FDialog* next{nullptr};

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

  return next;
}

//----------------------------------------------------------------------
template <typename IteratorT>
finalcut::FDialog* Window::getPrevious (IteratorT iter)
{
  auto prev_element = iter;
  finalcut::FDialog* prev;

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

  return prev;
}

//----------------------------------------------------------------------
void Window::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void Window::cb_createWindows()
{
  const auto& first = windows.begin();
  auto iter = first;
  const auto w = getRootWidget()->getWidth();
  const auto h = getRootWidget()->getHeight();
  const int dx = ( w > 80 ) ? int(w - 80) / 2 : 0;
  const int dy = ( h > 24 ) ? int(h - 24) / 2 : 0;

  while ( iter != windows.end() )
  {
    if ( ! (*iter).is_open )
    {
      auto& win_dat = *iter;
      auto win = new SmallWindow(this);
      win_dat.dgl = win;
      win_dat.is_open = true;
      win->setText(win_dat.title);
      const auto n = int(std::distance(first, iter));
      const int x = dx + 5 + (n % 3) * 25 + int(n / 3) * 3;
      const int y = dy + 11 + int(n / 3) * 3;
      win->setGeometry (FPoint{x, y}, FSize{20, 8});
      win->setMinimumSize (FSize{20, 8});
      win->setResizeable();
      win->show();

      win->addCallback
      (
        "destroy",
        this, &Window::cb_destroyWindow,
        std::ref(win_dat)
      );
    }

    ++iter;
  }

  activateWindow(this);
}

//----------------------------------------------------------------------
void Window::cb_closeWindows()
{
  if ( ! getDialogList() || getDialogList()->empty() )
    return;

  auto iter = getDialogList()->end();
  const auto& first = getDialogList()->begin();
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
void Window::cb_next()
{
  if ( ! getDialogList() || getDialogList()->empty() )
    return;

  auto iter = getDialogList()->begin();

  while ( iter != getDialogList()->end() )
  {
    if ( static_cast<finalcut::FWindow*>(*iter)->isWindowActive() )
    {
      activateWindow(getNext(iter));
      break;
    }

    ++iter;
  }
}

//----------------------------------------------------------------------
void Window::cb_previous()
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
      activateWindow(getPrevious(iter));
      break;
    }
  }
  while ( iter != getDialogList()->begin() );
}

//----------------------------------------------------------------------
void Window::cb_destroyWindow (WinData& win_dat) const
{
  win_dat.is_open = false;
  win_dat.dgl = nullptr;
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

int main (int argc, char* argv[])
{
  // Create the application object
  finalcut::FApplication app {argc, argv};

  // Create main dialog object
  Window main_dlg {&app};
  main_dlg.setText ("Main window");

  // Set dialog main_dlg as main widget
  finalcut::FWidget::setMainWidget (&main_dlg);

  // Show and start the application
  main_dlg.show();
  return app.exec();
}
