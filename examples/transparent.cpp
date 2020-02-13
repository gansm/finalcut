/***********************************************************************
* transparent.cpp - Demonstrates transparent windows                   *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2016-2020 Markus Gans                                      *
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

#include <final/final.h>

namespace fc = finalcut::fc;
using finalcut::FPoint;
using finalcut::FSize;


//----------------------------------------------------------------------
// class Transparent
//----------------------------------------------------------------------

class Transparent : public finalcut::FDialog
{
  public:
    // Typedef and Enumeration
    typedef enum ttype
    {
      transparent        = 0,
      shadow             = 1,
      inherit_background = 2
    } trans_type;

    // Constructor
    explicit Transparent ( finalcut::FWidget* = nullptr
                         , trans_type = transparent );

    // Disable copy constructor
    Transparent (const Transparent&) = delete;

    // Destructor
    ~Transparent();

    // Disable assignment operator (=)
    Transparent& operator = (const Transparent&) = delete;

  private:
    // Method
    void draw() override;

    // Event handlers
    void onKeyPress (finalcut::FKeyEvent* ev) override;

    // Data members
    trans_type type;
};

//----------------------------------------------------------------------
Transparent::Transparent ( finalcut::FWidget* parent
                         , Transparent::trans_type tt )
  : finalcut::FDialog(parent)
  , type(tt)
{
  setStatusbarMessage("Press Q to quit");
}

//----------------------------------------------------------------------
Transparent::~Transparent()
{ }

//----------------------------------------------------------------------
void Transparent::draw()
{
  finalcut::FDialog::draw();

  if ( isMonochron() )
    setReverse(true);

  if ( type == shadow )
  {
    const auto& wc = getFWidgetColors();
    setColor(wc.shadow_bg, wc.shadow_fg);
    setColorOverlay();
  }
  else if ( type == inherit_background )
  {
    if ( getMaxColor() > 8 )
      setColor(fc::Blue, fc::Black);
    else
      setColor(fc::Green, fc::Black);

    setInheritBackground();
  }
  else
    setTransparent();

  const finalcut::FString line(getClientWidth(), '.');

  for (int n{1}; n <= int(getClientHeight()); n++)
  {
    print() << FPoint(2, 2 + n) << line;
  }

  if ( type == shadow )
    unsetColorOverlay();
  else if ( type == inherit_background )
    unsetInheritBackground();
  else
    unsetTransparent();

  if ( isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
void Transparent::onKeyPress (finalcut::FKeyEvent* ev)
{
  if ( ! ev )
    return;

  if ( ev->key() == 'q' && getParentWidget() )
  {
    getParentWidget()->close();
    ev->accept();
  }
  else
    finalcut::FDialog::onKeyPress(ev);
}


//----------------------------------------------------------------------
// class MainWindow
//----------------------------------------------------------------------

class MainWindow : public finalcut::FDialog
{
  public:
    // Constructor
    explicit MainWindow (finalcut::FWidget* = nullptr);

    // Disable copy constructor
    MainWindow (const MainWindow&) = delete;

    // Destructor
    ~MainWindow();

    // Disable assignment operator (=)
    MainWindow& operator = (const MainWindow&) = delete;

  private:
    // Method
    void draw() override;

    // Event handlers
    void onClose (finalcut::FCloseEvent*) override;
    void onShow  (finalcut::FShowEvent*) override;
    void onTimer (finalcut::FTimerEvent*) override;
    void onKeyPress (finalcut::FKeyEvent* ev) override
    {
      if ( ! ev )
        return;

      if ( ev->key() == 'q' )
      {
        close();
        ev->accept();
      }
      else
        finalcut::FDialog::onKeyPress(ev);
    }

    // Data members
    finalcut::FString line1{};
    finalcut::FString line2{};
    Transparent* transpwin{nullptr};
    Transparent* shadowwin{nullptr};
    Transparent* ibg{nullptr};
    finalcut::FStatusBar status_bar{this};
};


//----------------------------------------------------------------------
MainWindow::MainWindow (finalcut::FWidget* parent)
  : FDialog(parent)
{
  line1 = "     .-.     .-.     .-.";
  line2 = "`._.'   `._.'   `._.'   ";

  // The memory allocation for the following three sub windows occurs
  // with the operator new. The lifetime of the generated widget
  // is managed by the parent object (this). The operator delete
  // is not required in this class and would result in a double free.
  transpwin = new Transparent(this);
  transpwin->setText("transparent");
  transpwin->setGeometry (FPoint(6, 3), FSize(29, 12));
  transpwin->unsetTransparentShadow();

  shadowwin = new Transparent(this, Transparent::shadow);
  shadowwin->setText("shadow");
  shadowwin->setGeometry (FPoint(46, 11), FSize(29, 12));
  shadowwin->unsetTransparentShadow();

  ibg = new Transparent(this, Transparent::inherit_background);
  ibg->setText("inherit background");
  ibg->setGeometry (FPoint(42, 3), FSize(29, 7));
  ibg->unsetTransparentShadow();

  // Statusbar at the bottom
  status_bar.setMessage("Press Q to quit");

  unsetTransparentShadow();
  activateDialog();
}

//----------------------------------------------------------------------
MainWindow::~MainWindow()
{ }

//----------------------------------------------------------------------
void MainWindow::draw()
{
  finalcut::FDialog::draw();

  if ( isMonochron() )
    setReverse(true);

  setColor();
  print() << FPoint(2, 4) << line1;
  print() << FPoint(2, 5) << line2;

  if ( isMonochron() )
    setReverse(false);

  updateTerminal();
}

//----------------------------------------------------------------------
void MainWindow::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void MainWindow::onShow (finalcut::FShowEvent*)
{
  addTimer(100);
}

//----------------------------------------------------------------------
void MainWindow::onTimer (finalcut::FTimerEvent*)
{
  wchar_t first_Char[2];
  std::size_t length = line1.getLength();
  first_Char[0] = line1[0];
  first_Char[1] = line2[0];
  line1 = line1.right(length - 1) + first_Char[0];
  line2 = line2.right(length - 1) + first_Char[1];
  redraw();
  flush();
}

//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

int main (int argc, char* argv[])
{
  // Create the application object
  finalcut::FApplication app (argc, argv);

  // Create main dialog object
  MainWindow main_dlg (&app);
  main_dlg.setText ("non-transparent");
  main_dlg.setGeometry (FPoint(8, 16), FSize(26, 7));

  // Set dialog main_dlg as main widget
  app.setMainWidget (&main_dlg);

  // Show and start the application
  main_dlg.show();
  return app.exec();
}
