/***********************************************************************
* xpmview.cpp - Image Viewer for X PixMap (XPM) images                 *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2022 Markus Gans                                           *
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

#include <final/final.h>
#include "xpmimage.h"

using finalcut::FPoint;
using finalcut::FRect;
using finalcut::FSize;


//----------------------------------------------------------------------
// class XpmPicture
//----------------------------------------------------------------------

class XpmPicture final : public finalcut::FScrollView
{
  public:
    // Constructor
    explicit XpmPicture (finalcut::FWidget* = nullptr);

    // Method
    void open (finalcut::FString);
    auto getImageSize() -> finalcut::FSize;
    std::size_t getImageColors();

  private:
    // Methods
    void draw() override;

    // Data member
    XpmImage xmp_image{};
};

//----------------------------------------------------------------------
XpmPicture::XpmPicture (finalcut::FWidget* parent)
  : finalcut::FScrollView{parent}
{ }

//----------------------------------------------------------------------
void XpmPicture::open (finalcut::FString filename)
{
  auto&& data = xmp_image.xpmFileToVector (filename.toString());
  xmp_image.parseXPM3(data);
}

//----------------------------------------------------------------------
auto XpmPicture::getImageSize() -> finalcut::FSize
{
  auto size = xmp_image.getSize();
  return size;
}

//----------------------------------------------------------------------
std::size_t XpmPicture::getImageColors()
{
  return xmp_image.getColorCount();
}

//----------------------------------------------------------------------
void XpmPicture::draw()
{
  if ( finalcut::FVTerm::getFOutput()->isMonochron() )
    setReverse(true);

  const auto& wc = getColorTheme();
  setColor (wc->label_inactive_fg, wc->dialog_bg);
  clearArea();
  print() << FPoint{1, 1};
  const auto& term_buffer = xmp_image.getTermBuffer();
  print(term_buffer);

  if ( finalcut::FVTerm::getFOutput()->isMonochron() )
    setReverse(false);

  FScrollView::draw();
}


//----------------------------------------------------------------------
// class XpmWindow
//----------------------------------------------------------------------

class XpmWindow final : public finalcut::FDialog
{
  public:
    // Using-declaration
    using finalcut::FDialog::setGeometry;

    // Constructor
    explicit XpmWindow (finalcut::FString, finalcut::FWidget* = nullptr);

    // Method
    void setSize (const FSize&, bool = true) override;
    void setGeometry (const FPoint&, const FSize&, bool = true) override;

  private:
    // Method
    void initLayout() override;
    void adjustSize() override;
    void open (finalcut::FString);

    // Event handler
    void onKeyPress (finalcut::FKeyEvent*) override;

    // Data members
    XpmPicture xpm{this};
};


//----------------------------------------------------------------------
XpmWindow::XpmWindow ( finalcut::FString filename
                     , finalcut::FWidget* parent )
  : finalcut::FDialog{parent}
{
  open(filename);
}

//----------------------------------------------------------------------
void XpmWindow::setSize (const FSize& size, bool adjust)
{
  finalcut::FDialog::setSize (size, adjust);
  xpm.setSize(FSize{getWidth(), getHeight() - 1});
}

//----------------------------------------------------------------------
void XpmWindow::setGeometry (const FPoint& p, const FSize& size, bool adjust)
{
  finalcut::FDialog::setGeometry (p, size, adjust);
  xpm.setGeometry(FPoint{1, 2}, FSize{getWidth(), getHeight() - 1});
}

//----------------------------------------------------------------------
void XpmWindow::initLayout()
{
  // The scrolling viewport widget
  setResizeable();
  setMinimumSize (FSize{15, 5});
  xpm.ignorePadding();
  xpm.setGeometry(FPoint{1, 2}, FSize{getWidth(), getHeight() - 1});
  FDialog::initLayout();
}

//----------------------------------------------------------------------
void XpmWindow::adjustSize()
{
  FDialog::adjustSize();
  xpm.setGeometry(FPoint{1, 2}, FSize{getWidth(), getHeight() - 1});

  if ( isZoomed() )
    return;

  auto desktop_size = FSize{getDesktopWidth(), getDesktopHeight() - 2};
  FRect screen(FPoint{1, 1}, desktop_size);

  // Centering of the window when it is not in the visible area
  if ( ! screen.contains(getPos()) )
  {
    int x = 1 + int((getDesktopWidth() - getWidth()) / 2);
    int y = 1 + int((getDesktopHeight() -2 - getHeight()) / 2);
    setPos(FPoint{x, y});
  }
}

//----------------------------------------------------------------------
void XpmWindow::open (finalcut::FString filename)
{
  xpm.open(filename);
  const auto base_name = finalcut::FString(basename(filename.c_str()));
  FDialog::setText(base_name);
  const auto size = xpm.getImageSize();
  const auto img_width = size.getWidth();
  const auto img_heigth = size.getHeight() / 2;
  xpm.setScrollSize({img_width, img_heigth});
  setMaximumSize({img_width + 2, img_heigth + 3});
  const auto width = std::min(img_width + 2, getDesktopWidth());
  const auto height = std::min(img_heigth + 3, getDesktopHeight() - 2);
  setSize({width, height});
  const auto x = int((getDesktopWidth() - img_width) / 2);
  const auto y = int((getDesktopHeight() - img_heigth - 2) / 2);
  setPos({x, y});
  auto msg = finalcut::FString(std::to_string(img_width))
           + finalcut::UniChar::Times
           + std::to_string(size.getHeight())
           + L" pixels, "
           + std::to_string(xpm.getImageColors())
           + L" colors";
  setStatusbarMessage(msg);

  if ( getStatusBar() )
  {
    getStatusBar()->setMessage(msg);
    getStatusBar()->drawMessage();
  }
}

//----------------------------------------------------------------------
void XpmWindow::onKeyPress (finalcut::FKeyEvent* ev)
{
  if ( ! ev )
    return;

  if ( ev->key() == finalcut::FKey::Ctrl_w )
  {
    close();
    ev->accept();
  }
  else
  {
    xpm.onKeyPress(ev);

    if ( ! ev->isAccepted() )
      finalcut::FDialog::onKeyPress(ev);
  }
}


//----------------------------------------------------------------------
// class MainWidget
//----------------------------------------------------------------------

class MainWidget final : public finalcut::FWidget
{
  public:
    explicit MainWidget (finalcut::FWidget* parent = nullptr);
    void open (finalcut::FString);

  private:
    // Event handler
    void onAccel (finalcut::FAccelEvent*) override;
    void onClose (finalcut::FCloseEvent*) override;

    // Callback method
    void cb_fileOpen();

    finalcut::FMenuBar        Menubar{this};
    finalcut::FMenu           File{"&File", &Menubar};
    finalcut::FDialogListMenu Windowlist{"&Window", &Menubar};
    finalcut::FMenuItem       Open{"&Open", &File};
    finalcut::FMenuItem       Line{&File};
    finalcut::FMenuItem       Quit{"&Quit", &File};
    finalcut::FStatusBar      Statusbar{this};
    finalcut::FString         directory{L"."};
};

//----------------------------------------------------------------------
MainWidget::MainWidget (finalcut::FWidget* parent)
  : finalcut::FWidget{parent}
{
  File.setStatusbarMessage ("File management commands");
  Windowlist.setStatusbarMessage ("List of all windows");
  Open.setStatusbarMessage ("Locate and open a text file");
  Quit.setStatusbarMessage("Exit the program");
  Open.addAccelerator (finalcut::FKey::Ctrl_o);  // Ctrl + O
  Quit.addAccelerator (finalcut::FKey::Ctrl_q);  // Ctrl + Q
  addAccelerator (finalcut::FKey('q'));  // Q
  Line.setSeparator();
  Open.addCallback("clicked", this, &MainWidget::cb_fileOpen);

  Quit.addCallback
  (
    "clicked",
    finalcut::getFApplication(),
    &finalcut::FApplication::cb_exitApp,
    this
  );
}

//----------------------------------------------------------------------
void MainWidget::open (finalcut::FString filename)
{
  const auto& xpm_window = new XpmWindow(filename, this);
  xpm_window->show();
}

//----------------------------------------------------------------------
void MainWidget::onAccel (finalcut::FAccelEvent* ev)
{
  close();
  ev->accept();
}

//----------------------------------------------------------------------
void MainWidget::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void MainWidget::cb_fileOpen()
{
  finalcut::FString filename = [this] ()
  {
    finalcut::FString filter("*.xpm");
    return finalcut::FFileDialog::fileOpenChooser(this, directory, filter);
  }();

  if ( filename.isEmpty() )
    return;

  directory = dirname(filename.c_str());
  open(filename);
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

int main (int argc, char* argv[])
{
  if ( argv[1] && ( strcmp(argv[1], "--help") == 0
                 || strcmp(argv[1], "-h") == 0 ) )
  {
    std::cout << "XPM viewer\n"
              << "Usage: " << basename(argv[0]) << " [FILE]...\n\n";
  }

  // Create the application object
  finalcut::FApplication app{argc, argv};

  // Create the main widget object
  MainWidget xpmviewer(&app);
  finalcut::FWidget::setMainWidget(&xpmviewer);

  // Show the main widget
  xpmviewer.show();

  for (int i{1}; i < argc; i++)
    xpmviewer.open(argv[i]);

  return app.exec();
}
