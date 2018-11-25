/***********************************************************************
* term-attributes.cpp - Test the video attributes of the terminal      *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2015-2018 Markus Gans                                      *
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
// class AttribDlg
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class AttribDlg : public finalcut::FDialog
{
  public:
    // Constructor
    explicit AttribDlg (finalcut::FWidget* = 0);

    // Destructor
    ~AttribDlg();

    // Event handlers
    virtual void onAccel (finalcut::FAccelEvent*);
    virtual void onWheel (finalcut::FWheelEvent*);
    virtual void onClose (finalcut::FCloseEvent*);

    // Callback methods
    void cb_next (finalcut::FWidget* = 0, data_ptr = 0);
    void cb_back (finalcut::FWidget* = 0, data_ptr = 0);

    // Data Members
    FColor bgcolor;

  private:
    // Disable copy constructor
    AttribDlg (const AttribDlg&);
    // Disable assignment operator (=)
    AttribDlg& operator = (const AttribDlg&);

    // Method
    virtual void adjustSize();

    // Data Members
    finalcut::FButton next_button;
    finalcut::FButton back_button;
};
#pragma pack(pop)

//----------------------------------------------------------------------
AttribDlg::AttribDlg (finalcut::FWidget* parent)
  : finalcut::FDialog(parent)
  , bgcolor(wc.label_bg)
  , next_button("&Next >", this)
  , back_button("< &Back", this)
{
  setText ( "A terminal attributes test ("
          + finalcut::FString(getTermType())
          + ")");

  next_button.setGeometry(int(getWidth()) - 13, int(getHeight()) - 4, 10, 1);
  next_button.addAccelerator(finalcut::fc::Fkey_right);
  back_button.setGeometry(int(getWidth()) - 25, int(getHeight()) - 4, 10, 1);
  back_button.addAccelerator(finalcut::fc::Fkey_left);

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
void AttribDlg::onAccel (finalcut::FAccelEvent* ev)
{
  close();
  ev->accept();
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
void AttribDlg::cb_next (finalcut::FWidget*, data_ptr)
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
void AttribDlg::cb_back (finalcut::FWidget*, data_ptr)
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

  setGeometry(x, y, 69, 21, false);
  next_button.setGeometry ( int(getWidth()) - 13, int(getHeight()) - 4
                          , 10, 1, false );
  back_button.setGeometry ( int(getWidth()) - 25, int(getHeight()) - 4
                          , 10, 1, false );
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
    explicit AttribDemo (FWidget* = 0);

    // Destructor
    ~AttribDemo()
    { }

    // Event handler
    virtual void onWheel (finalcut::FWheelEvent* ev)
    {
      AttribDlg* p = static_cast<AttribDlg*>(getParentWidget());

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
    virtual void draw();

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
  AttribDlg* parent = static_cast<AttribDlg*>(getParent());

  for (FColor color = 0; color < colors; color++)
  {
    setColor (color, parent->bgcolor);
    print (" # ");
  }
}

//----------------------------------------------------------------------
void AttribDemo::printAltCharset()
{
  AttribDlg* parent = static_cast<AttribDlg*>(getParent());

  if ( ! isMonochron() )
    setColor (wc.label_fg, wc.label_bg);

  setPrintPos (1, 1);
  print("alternate charset: ");

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

  for (int y = 0; y < int(getParentWidget()->getHeight()) - 7; y++)
  {
    setPrintPos (1, 2 + y);

    if ( ! isMonochron() )
      setColor (wc.label_fg, wc.label_bg);

    switch (y)
    {
      case 0:
        printDim();
        break;

      case 1:
        printNormal();
        break;

      case 2:
        printBold();
        break;

      case 3:
        printBoldDim();
        break;

      case 4:
        printItalic();
        break;

      case 5:
        printUnderline();
        break;

      case 6:
        printDblUnderline();
        break;

      case 7:
        printCrossesOut();
        break;

      case 8:
        printBlink();
        break;

      case 9:
        printReverse();
        break;

      case 10:
        printStandout();
        break;

      case 11:
        printInvisible();
        break;

      case 12:
        printProtected();
        break;
    }
  }

  if ( ! isMonochron() )
    setColor(wc.label_fg, wc.label_bg);

  setPrintPos (1, 15);
  FColor bg = static_cast<AttribDlg*>(getParent())->bgcolor;
  print (" Background color:");

  if ( bg == finalcut::fc::Default )
    print (" default");
  else
    printf ( " %d", bg);

  setPrintPos (16, 17);
  print ("Change background color ->");
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

  dialog.setGeometry (6, 2, 69, 21);
  dialog.addAccelerator('q');  // press 'q' to quit
  dialog.setShadow();

  // Create the attribute demo widget as a child object from the dialog
  AttribDemo demo(&dialog);
  demo.setGeometry (1, 1, 67, 19);

  // Set the dialog object as main widget
  app.setMainWidget(&dialog);

  // Show and start the application
  dialog.show();
  return app.exec();
}
