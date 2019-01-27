/***********************************************************************
* term-attributes.cpp - Test the video attributes of the terminal      *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2015-2019 Markus Gans                                      *
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

#include <functional>
#include <final/final.h>

using finalcut::FPoint;
using finalcut::FSize;


//----------------------------------------------------------------------
// class AttribDlg
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class AttribDlg : public finalcut::FDialog
{
  public:
    // Constructor
    explicit AttribDlg (finalcut::FWidget* = nullptr);

    // Disable copy constructor
    AttribDlg (const AttribDlg&) = delete;

    // Destructor
    ~AttribDlg();

    // Disable assignment operator (=)
    AttribDlg& operator = (const AttribDlg&) = delete;

    // Event handlers
    virtual void onKeyPress (finalcut::FKeyEvent*) override;
    virtual void onWheel (finalcut::FWheelEvent*) override;
    virtual void onClose (finalcut::FCloseEvent*) override;

    // Callback methods
    void cb_next (finalcut::FWidget* = nullptr, FDataPtr = nullptr);
    void cb_back (finalcut::FWidget* = nullptr, FDataPtr = nullptr);

    // Data Members
    FColor bgcolor;

  private:
    // Method
    virtual void adjustSize() override;

    // Data Members
    finalcut::FButton next_button{"&Next >", this};
    finalcut::FButton back_button{"< &Back", this};
};
#pragma pack(pop)

//----------------------------------------------------------------------
AttribDlg::AttribDlg (finalcut::FWidget* parent)
  : finalcut::FDialog(parent)
  , bgcolor(wc.label_bg)
{
  setText ( "A terminal attributes test ("
          + finalcut::FString(getTermType())
          + ")");

  next_button.setGeometry ( FPoint(int(getWidth()) - 13, int(getHeight()) - 4)
                          , FSize(10, 1) );
  next_button.addAccelerator (finalcut::fc::Fkey_right);
  back_button.setGeometry ( FPoint(int(getWidth()) - 25, int(getHeight()) - 4)
                          , FSize(10, 1) );
  back_button.addAccelerator (finalcut::fc::Fkey_left);

  // Add function callbacks
  next_button.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &AttribDlg::cb_next)
  );

  back_button.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &AttribDlg::cb_back)
  );
}

//----------------------------------------------------------------------
AttribDlg::~AttribDlg()
{ }

//----------------------------------------------------------------------
void AttribDlg::onKeyPress (finalcut::FKeyEvent* ev)
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

//----------------------------------------------------------------------
void AttribDlg::onWheel (finalcut::FWheelEvent* ev)
{
  int wheel = ev->getWheel();

  if ( wheel == finalcut::fc::WheelUp )
    cb_next();
  else if ( wheel == finalcut::fc::WheelDown )
    cb_back();
}

//----------------------------------------------------------------------
void AttribDlg::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void AttribDlg::cb_next (finalcut::FWidget*, FDataPtr)
{
  if ( isMonochron() )
    return;

  if ( bgcolor == FColor(getMaxColor() - 1) )
    bgcolor = finalcut::fc::Default;
  else if ( bgcolor == finalcut::fc::Default )
    bgcolor = 0;
  else
    bgcolor++;

  redraw();
}

//----------------------------------------------------------------------
void AttribDlg::cb_back (finalcut::FWidget*, FDataPtr)
{
  if ( isMonochron() )
    return;

  if ( bgcolor == 0 )
    bgcolor = finalcut::fc::Default;
  else if ( bgcolor == finalcut::fc::Default )
    bgcolor = FColor(getMaxColor() - 1);
  else
    bgcolor--;

  redraw();
}

//----------------------------------------------------------------------
void AttribDlg::adjustSize()
{
  int x = int((getParentWidget()->getWidth() - getWidth()) / 2);
  int y = int((getParentWidget()->getHeight() - getHeight()) / 2) + 1;

  if ( x < 1 )
    x = 1;

  if ( y < 1 )
    y = 1;

  setGeometry(FPoint(x, y), FSize(69, 21), false);
  next_button.setGeometry ( FPoint(int(getWidth()) - 13, int(getHeight()) - 4)
                          , FSize(10, 1), false );
  back_button.setGeometry ( FPoint(int(getWidth()) - 25, int(getHeight()) - 4)
                          , FSize(10, 1), false );
  finalcut::FDialog::adjustSize();
}


//----------------------------------------------------------------------
// class AttribDemo
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class AttribDemo : public finalcut::FWidget
{
  public:
    // Constructor
    explicit AttribDemo (FWidget* = nullptr);

    // Destructor
    ~AttribDemo()
    { }

    // Event handler
    virtual void onWheel (finalcut::FWheelEvent* ev) override
    {
      auto p = static_cast<AttribDlg*>(getParentWidget());

      if ( p )
        p->onWheel(ev);
    }

  private:
    // Methods
    void printColorLine();
    void printAltCharset();
    void printDim();
    void printNormal();
    void printBold();
    void printBoldDim();
    void printItalic();
    void printUnderline();
    void printDblUnderline();
    void printCrossesOut();
    void printBlink();
    void printReverse();
    void printStandout();
    void printInvisible();
    void printProtected();
    virtual void draw() override;

    // Data Member
    int colors;
};
#pragma pack(pop)

//----------------------------------------------------------------------
AttribDemo::AttribDemo (finalcut::FWidget* parent)
  : finalcut::FWidget(parent)
  , colors(getMaxColor())
{
  if ( isMonochron() )
    colors = 1;
  else if ( colors > 16 )
    colors = 16;

  unsetFocusable();
}

//----------------------------------------------------------------------
void AttribDemo::printColorLine()
{
  auto parent = static_cast<AttribDlg*>(getParent());

  for (FColor color = 0; color < colors; color++)
  {
    setColor (color, parent->bgcolor);
    print (" # ");
  }
}

//----------------------------------------------------------------------
void AttribDemo::printAltCharset()
{
  auto parent = static_cast<AttribDlg*>(getParent());

  if ( ! isMonochron() )
    setColor (wc.label_fg, wc.label_bg);

  print() << FPoint(1, 1) << "alternate charset: ";

  if ( parent->bgcolor == finalcut::fc::Default )
  {
    setColor (finalcut::fc::Default, finalcut::fc::Default);
  }
  else
  {
    if ( (parent->bgcolor <= 8)
      || (parent->bgcolor >= 16 && parent->bgcolor <= 231
        && (parent->bgcolor - 16) % 36 <= 17)
      || (parent->bgcolor >= 232 && parent->bgcolor <= 243) )
      setColor (finalcut::fc::White, parent->bgcolor);
    else
      setColor (finalcut::fc::Black, parent->bgcolor);
  }

  setAltCharset();
  print("`abcdefghijklmnopqrstuvwxyz{|}~");
  unsetAltCharset();
  print("                 ");
}

//----------------------------------------------------------------------
void AttribDemo::printDim()
{
  print("              Dim: ");
  setDim();
  printColorLine();
  unsetDim();
}

//----------------------------------------------------------------------
void AttribDemo::printNormal()
{
  print("           Normal: ");
  setNormal();
  printColorLine();
}

//----------------------------------------------------------------------
void AttribDemo::printBold()
{
  print("             Bold: ");
  setBold();
  printColorLine();
  unsetBold();
}

//----------------------------------------------------------------------
void AttribDemo::printBoldDim()
{
  print("         Bold+Dim: ");
  setBold();
  setDim();
  printColorLine();
  unsetDim();
  unsetBold();
}

//----------------------------------------------------------------------
void AttribDemo::printItalic()
{
  print("           Italic: ");
  setItalic();
  printColorLine();
  unsetItalic();
}

//----------------------------------------------------------------------
void AttribDemo::printUnderline()
{
  print("        Underline: ");
  setUnderline();
  printColorLine();
  unsetUnderline();
}

//----------------------------------------------------------------------
void AttribDemo::printDblUnderline()
{
  print(" Double underline: ");
  setDoubleUnderline();
  printColorLine();
  unsetDoubleUnderline();
}

//----------------------------------------------------------------------
void AttribDemo::printCrossesOut()
{
  print("      Crossed-out: ");
  setCrossedOut();
  printColorLine();
  unsetCrossedOut();
}

//----------------------------------------------------------------------
void AttribDemo::printBlink()
{
  print("            Blink: ");
  setBlink();
  printColorLine();
  unsetBlink();
}

//----------------------------------------------------------------------
void AttribDemo::printReverse()
{
  print("          Reverse: ");
  setReverse();
  printColorLine();
  unsetReverse();
}

//----------------------------------------------------------------------
void AttribDemo::printStandout()
{
  print("         Standout: ");
  setStandout();
  printColorLine();
  unsetStandout();
}

//----------------------------------------------------------------------
void AttribDemo::printInvisible()
{
  print("        Invisible: ");
  setInvisible();
  printColorLine();
  unsetInvisible();
}

//----------------------------------------------------------------------
void AttribDemo::printProtected()
{
  print("        Protected: ");
  setProtected();
  printColorLine();
  unsetProtected();
}

//----------------------------------------------------------------------
void AttribDemo::draw()
{
  // test alternate character set
  printAltCharset();

  std::vector<std::function<void()> > effect
  {
    [&] { printDim(); },
    [&] { printNormal(); },
    [&] { printBold(); },
    [&] { printBoldDim(); },
    [&] { printItalic(); },
    [&] { printUnderline(); },
    [&] { printDblUnderline(); },
    [&] { printCrossesOut(); },
    [&] { printBlink(); },
    [&] { printReverse(); },
    [&] { printStandout(); },
    [&] { printInvisible(); },
    [&] { printProtected(); },
  };

  for (std::size_t y = 0; y < getParentWidget()->getHeight() - 7; y++)
  {
    print() << FPoint(1, 2 + int(y));

    if ( ! isMonochron() )
      setColor (wc.label_fg, wc.label_bg);

    if ( y < effect.size() )
      effect[y]();
  }

  if ( ! isMonochron() )
    setColor(wc.label_fg, wc.label_bg);

  print() << FPoint(1, 15);
  FColor bg = static_cast<AttribDlg*>(getParent())->bgcolor;
  print (" Background color:");

  if ( bg == finalcut::fc::Default )
    print (" default");
  else
    printf ( " %d", bg);

  print() << FPoint(16, 17) << "Change background color ->";
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
{
  // Create the application object
  finalcut::FApplication app (argc, argv);

  // Create a dialog box object.
  // This object will be automatically deleted by
  // the parent object "app" (FObject destructor).
  AttribDlg dialog(&app);

  dialog.setGeometry (FPoint(6, 2), FSize(69, 21));
  dialog.setShadow();

  // Create the attribute demo widget as a child object from the dialog
  AttribDemo demo(&dialog);
  demo.setGeometry (FPoint(1, 1), FSize(67, 19));

  // Set the dialog object as main widget
  app.setMainWidget(&dialog);

  // Show and start the application
  dialog.show();
  return app.exec();
}
