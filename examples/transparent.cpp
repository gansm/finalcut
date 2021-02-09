/***********************************************************************
* transparent.cpp - Demonstrates transparent windows                   *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2016-2021 Markus Gans                                      *
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

using finalcut::FColorPair;
using finalcut::FColor;
using finalcut::FPoint;
using finalcut::FSize;
using finalcut::FStyle;


//----------------------------------------------------------------------
// class Transparent
//----------------------------------------------------------------------

class Transparent final : public finalcut::FDialog
{
  public:
    // Enumeration
    enum class Type
    {
      Transparent       = 0,
      Shadow            = 1,
      InheritBackground = 2
    };

    // Constructor
    explicit Transparent ( finalcut::FWidget* = nullptr
                         , Type = Type::Transparent );

    // Disable copy constructor
    Transparent (const Transparent&) = delete;

    // Destructor
    ~Transparent() override = default;

    // Disable copy assignment operator (=)
    Transparent& operator = (const Transparent&) = delete;

  private:
    // Method
    void draw() override;

    // Event handlers
    void onKeyPress (finalcut::FKeyEvent* ev) override;

    // Data members
    Type type;
};

//----------------------------------------------------------------------
Transparent::Transparent ( finalcut::FWidget* parent
                         , Transparent::Type tt )
  : finalcut::FDialog{parent}
  , type{tt}
{
  // Set statusbar text for this window
  // Avoids calling a virtual function from the constructor
  // (CERT, OOP50-CPP)
  FWidget::setStatusbarMessage("Press Q to quit");
}

//----------------------------------------------------------------------
void Transparent::draw()
{
  finalcut::FDialog::draw();

  if ( finalcut::FTerm::isMonochron() )
    setReverse(true);

  if ( type == Type::Shadow )
  {
    const auto& wc = getColorTheme();
    print() << FColorPair {wc->shadow_bg, wc->shadow_fg}
            << FStyle {finalcut::Style::ColorOverlay};
  }
  else if ( type == Type::InheritBackground )
  {
    if ( finalcut::FTerm::getMaxColor() > 8 )
      print() << FColorPair {FColor::Blue, FColor::Black};
    else
      print() << FColorPair {FColor::Green, FColor::Black};

    print() << FStyle {finalcut::Style::InheritBackground};
  }
  else
    print() << FStyle {finalcut::Style::Transparent};

  const finalcut::FString line{getClientWidth(), '.'};

  // Fill window area
  for (auto n{1}; n <= int(getClientHeight()); n++)
  {
    print() << FPoint{2, 2 + n}
            << line;
  }

  print() <<  FStyle{finalcut::Style::None};
}

//----------------------------------------------------------------------
void Transparent::onKeyPress (finalcut::FKeyEvent* ev)
{
  if ( ! ev )
    return;

  if ( ev->key() == finalcut::FKey('q') && getParentWidget() )
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

class MainWindow final : public finalcut::FDialog
{
  public:
    // Constructor
    explicit MainWindow (finalcut::FWidget* = nullptr);

    // Disable copy constructor
    MainWindow (const MainWindow&) = delete;

    // Destructor
    ~MainWindow() override = default;

    // Disable copy assignment operator (=)
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

      if ( ev->key() == finalcut::FKey('q') )
      {
        close();
        ev->accept();
      }
      else
        finalcut::FDialog::onKeyPress(ev);
    }

    // Data members
    finalcut::FString line1{"     .-.     .-.     .-."};
    finalcut::FString line2{"`._.'   `._.'   `._.'   "};
    Transparent* transpwin{nullptr};
    Transparent* shadowwin{nullptr};
    Transparent* ibg{nullptr};
    finalcut::FStatusBar status_bar{this};
};


//----------------------------------------------------------------------
MainWindow::MainWindow (finalcut::FWidget* parent)
  : FDialog{parent}
{
  // The memory allocation for the following three sub windows occurs
  // with the operator new. The lifetime of the generated widget
  // is managed by the parent object (this). The operator delete
  // is not required in this class and would result in a double free.
  transpwin = new Transparent(this);
  transpwin->setText("transparent");
  transpwin->setGeometry (FPoint{6, 3}, FSize{29, 12});
  transpwin->unsetTransparentShadow();

  shadowwin = new Transparent(this, Transparent::Type::Shadow);
  shadowwin->setText("shadow");
  shadowwin->setGeometry (FPoint{46, 11}, FSize{29, 12});
  shadowwin->unsetTransparentShadow();

  ibg = new Transparent(this, Transparent::Type::InheritBackground);
  ibg->setText("inherit background");
  ibg->setGeometry (FPoint{42, 3}, FSize{29, 7});
  ibg->unsetTransparentShadow();

  // Set statusbar text for this window
  FDialog::setStatusbarMessage("Press Q to quit");

  unsetTransparentShadow();
  activateDialog();
}

//----------------------------------------------------------------------
void MainWindow::draw()
{
  finalcut::FDialog::draw();

  if ( finalcut::FTerm::isMonochron() )
    setReverse(true);

  setColor();
  print() << FPoint{2, 4} << line1;
  print() << FPoint{2, 5} << line2;

  if ( finalcut::FTerm::isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
void MainWindow::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void MainWindow::onShow (finalcut::FShowEvent*)
{
  addTimer(90);  // Call onTimer() every 90 ms
}

//----------------------------------------------------------------------
void MainWindow::onTimer (finalcut::FTimerEvent*)
{
  std::size_t length = line1.getLength();
  const wchar_t first_char1 = line1[0];
  const wchar_t first_char2 = line2[0];
  line1 = line1.right(length - 1) + first_char1;
  line2 = line2.right(length - 1) + first_char2;
  redraw();
  flush();
}

//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

int main (int argc, char* argv[])
{
  // Create the application object
  finalcut::FApplication app {argc, argv};
  app.setNonBlockingRead();

  // Create main dialog object
  MainWindow main_dlg {&app};
  main_dlg.setText ("non-transparent");
  main_dlg.setGeometry (FPoint{8, 16}, FSize{26, 7});

  // Set dialog main_dlg as main widget
  finalcut::FWidget::setMainWidget (&main_dlg);

  // Show and start the application
  main_dlg.show();
  return app.exec();
}
