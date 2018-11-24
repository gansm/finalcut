/***********************************************************************
* transparent.cpp - Demonstrates transparent windows                   *
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

#include <final/final.h>


//----------------------------------------------------------------------
// class Transparent
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

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
    explicit Transparent (finalcut::FWidget* = 0, trans_type = transparent);

    // Destructor
    ~Transparent();

  private:
    // Disable copy constructor
    Transparent (const Transparent&);

    // Disable assignment operator (=)
    Transparent& operator = (const Transparent&);

    // Method
    virtual void draw();

    // Event handlers
    virtual void onKeyPress (finalcut::FKeyEvent* ev);

    // Data Members
    trans_type type;
};
#pragma pack(pop)

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
    setColor(wc.shadow_bg, wc.shadow_fg);
    setTransShadow();
  }
  else if ( type == inherit_background )
  {
    if ( getMaxColor() > 8 )
      setColor(finalcut::fc::Blue, finalcut::fc::Black);
    else
      setColor(finalcut::fc::Green, finalcut::fc::Black);

    setInheritBackground();
  }
  else
    setTransparent();

  finalcut::FString line(getClientWidth(), wchar_t('.'));

  for (int n = 1; n <= int(getClientHeight()); n++)
  {
    setPrintPos (2, 2 + n);
    print(line);
  }

  if ( type == shadow )
    unsetTransShadow();
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
    if ( getParentWidget()->close() )
      ev->accept();
    else
      ev->ignore();
  }
  else
    finalcut::FDialog::onKeyPress(ev);
}


//----------------------------------------------------------------------
// class MainWindow
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class MainWindow : public finalcut::FDialog
{
  public:
    // Constructor
    explicit MainWindow (finalcut::FWidget* = 0);
    // Destructor
    ~MainWindow();

  private:
    // Disable copy constructor
    MainWindow (const MainWindow&);
    // Disable assignment operator (=)
    MainWindow& operator = (const MainWindow&);

    virtual void draw();

    // Event handlers
    virtual void onClose (finalcut::FCloseEvent*);
    virtual void onShow  (finalcut::FShowEvent*);
    virtual void onTimer (finalcut::FTimerEvent*);
    virtual void onKeyPress (finalcut::FKeyEvent* ev)
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

    // Data Members
    finalcut::FString line1;
    finalcut::FString line2;
    Transparent* transpwin;
    Transparent* shadowwin;
    Transparent* ibg;
    finalcut::FStatusBar status_bar;
};
#pragma pack(pop)

//----------------------------------------------------------------------
MainWindow::MainWindow (finalcut::FWidget* parent)
  : FDialog(parent)
  , line1()
  , line2()
  , transpwin(0)
  , shadowwin(0)
  , ibg(0)
  , status_bar(this)
{
  line1 = "     .-.     .-.     .-.";
  line2 = "`._.'   `._.'   `._.'   ";

  // The memory allocation for the following three sub windows occurs
  // with the operator new. The lifetime of the generated widget
  // is managed by the parent object (this). The operator delete
  // is not required in this scope and would result in a double free.
  transpwin = new Transparent(this);
  transpwin->setText("transparent");
  transpwin->setGeometry (6, 3, 29, 12);
  transpwin->unsetTransparentShadow();

  shadowwin = new Transparent(this, Transparent::shadow);
  shadowwin->setText("shadow");
  shadowwin->setGeometry (46, 11, 29, 12);
  shadowwin->unsetTransparentShadow();

  ibg = new Transparent(this, Transparent::inherit_background);
  ibg->setText("inherit background");
  ibg->setGeometry (42, 3, 29, 7);
  ibg->unsetTransparentShadow();

  // Statusbar at the bottom
  status_bar.setMessage("Press Q to quit");

  addAccelerator('q');
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
  setPrintPos (2, 4);
  print(line1);
  setPrintPos (2, 5);
  print(line2);

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
  flush_out();
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
  main_dlg.setGeometry (8, 16, 26, 7);

  // Set dialog main_dlg as main widget
  app.setMainWidget (&main_dlg);

  // Show and start the application
  main_dlg.show();
  return app.exec();
}
