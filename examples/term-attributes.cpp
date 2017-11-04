/***********************************************************************
* term-attributes.cpp - Test the video attributes of the terminal      *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2015-2017 Markus Gans                                      *
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

class AttribDlg : public FDialog
{
  public:
    // Constructor
    explicit AttribDlg (FWidget* = 0);

    // Destructor
    ~AttribDlg();

    // Event handlers
    void onAccel (FAccelEvent*);
    void onWheel (FWheelEvent*);
    void onClose (FCloseEvent*);

    // Callback methods
    void cb_next (FWidget* = 0, data_ptr = 0);
    void cb_back (FWidget* = 0, data_ptr = 0);

    // Data Members
    short bgcolor;

  private:
    // Disable copy constructor
    AttribDlg (const AttribDlg&);
    // Disable assignment operator (=)
    AttribDlg& operator = (const AttribDlg&);

    // Method
    void adjustSize();

    // Data Members
    FButton* next_button;
    FButton* back_button;
};
#pragma pack(pop)

//----------------------------------------------------------------------
AttribDlg::AttribDlg (FWidget* parent)
  : FDialog(parent)
  , bgcolor(wc.label_bg)
  , next_button()
  , back_button()
{
  resetXTermForeground();
  resetXTermBackground();
  setText ( "A terminal attributes test ("
          + FString(getTermType())
          + ")");

  next_button = new FButton("&Next >", this);
  next_button->setGeometry(getWidth() - 13, getHeight() - 4, 10, 1);
  next_button->addAccelerator(fc::Fkey_right);
  back_button = new FButton("< &Back", this);
  back_button->setGeometry(getWidth() - 25, getHeight() - 4, 10, 1);
  back_button->addAccelerator(fc::Fkey_left);

  // Add function callbacks
  next_button->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &AttribDlg::cb_next)
  );

  back_button->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &AttribDlg::cb_back)
  );
}

//----------------------------------------------------------------------
AttribDlg::~AttribDlg()
{ }

//----------------------------------------------------------------------
void AttribDlg::onAccel (FAccelEvent* ev)
{
  close();
  ev->accept();
}

//----------------------------------------------------------------------
void AttribDlg::onWheel (FWheelEvent* ev)
{
  int wheel = ev->getWheel();

  if ( wheel == fc::WheelUp )
    cb_next();
  else if ( wheel == fc::WheelDown )
    cb_back();
}

//----------------------------------------------------------------------
void AttribDlg::onClose (FCloseEvent* ev)
{
  FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void AttribDlg::cb_next (FWidget*, data_ptr)
{
  if ( isMonochron() )
    return;

  bgcolor++;

  if ( bgcolor >= getMaxColor() )
    bgcolor = fc::Default;

  redraw();
}

//----------------------------------------------------------------------
void AttribDlg::cb_back (FWidget*, data_ptr)
{
  if ( isMonochron() )
    return;

  bgcolor--;

  if ( bgcolor < fc::Default )
    bgcolor = short(getMaxColor() - 1);

  redraw();
}

//----------------------------------------------------------------------
void AttribDlg::adjustSize()
{
  int x = ((getParentWidget()->getWidth() - getWidth()) / 2);
  int y = ((getParentWidget()->getHeight() - getHeight()) / 2) + 1;

  if ( x < 1 )
    x = 1;

  if ( y < 1 )
    y = 1;

  setGeometry(x, y, 69, 21, false);
  next_button->setGeometry(getWidth() - 13, getHeight() - 4, 10, 1, false);
  back_button->setGeometry(getWidth() - 25, getHeight() - 4, 10, 1, false);
  FDialog::adjustSize();
}


//----------------------------------------------------------------------
// class AttribDemo
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class AttribDemo : public FWidget
{
  public:
    // Constructor
    explicit AttribDemo (FWidget* = 0);

    // Destructor
    ~AttribDemo()
    { }

    // Event handler
    void onWheel (FWheelEvent* ev)
    {
      AttribDlg* p = static_cast<AttribDlg*>(getParentWidget());

      if ( p )
        p->onWheel(ev);
    }

  private:
    // Methods
    void printColorLine();
    void printAltCharset();
    void draw();

    // Data Member
    int colors;
};
#pragma pack(pop)

//----------------------------------------------------------------------
AttribDemo::AttribDemo (FWidget* parent)
  : FWidget(parent)
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

  for (short color = 0; color < colors; color++)
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

  setPrintPos (1,1);
  print("alternate charset: ");

  if ( parent->bgcolor == fc::Default )
  {
    setColor (fc::Default, fc::Default);
  }
  else
  {
    if ( parent->bgcolor == 0 || parent->bgcolor == 16 )
      setColor (fc::White, parent->bgcolor);
    else
      setColor (fc::Black, parent->bgcolor);
  }

  setAltCharset();
  print("`abcdefghijklmnopqrstuvwxyz{|}~");
  unsetAltCharset();
  print("                 ");
}

//----------------------------------------------------------------------
void AttribDemo::draw()
{
  // test alternate character set
  printAltCharset();

  for (int y = 0; y < getParentWidget()->getHeight() - 7; y++)
  {
    setPrintPos (1, 2 + y);

    if ( ! isMonochron() )
      setColor (wc.label_fg, wc.label_bg);

    switch (y)
    {
      case 0:
        print("              Dim: ");
        setDim();
        printColorLine();
        unsetDim();
        break;

      case 1:
        print("           Normal: ");
        setNormal();
        printColorLine();
        break;

      case 2:
        print("             Bold: ");
        setBold();
        printColorLine();
        unsetBold();
        break;

      case 3:
        print("         Bold+Dim: ");
        setBold();
        setDim();
        printColorLine();
        unsetDim();
        unsetBold();
        break;

      case 4:
        print("           Italic: ");
        setItalic();
        printColorLine();
        unsetItalic();
        break;

      case 5:
        print("        Underline: ");
        setUnderline();
        printColorLine();
        unsetUnderline();
        break;

      case 6:
        print(" Double underline: ");
        setDoubleUnderline();
        printColorLine();
        unsetDoubleUnderline();
        break;

      case 7:
        print("      Crossed-out: ");
        setCrossedOut();
        printColorLine();
        unsetCrossedOut();
        break;

      case 8:
        print("            Blink: ");
        setBlink();
        printColorLine();
        unsetBlink();
        break;

      case 9:
        print("          Reverse: ");
        setReverse();
        printColorLine();
        unsetReverse();
        break;

      case 10:
        print("         Standout: ");
        setStandout();
        printColorLine();
        unsetStandout();
        break;

      case 11:
        print("        Invisible: ");
        setInvisible();
        printColorLine();
        unsetInvisible();
        break;

      case 12:
        print("        Protected: ");
        setProtected();
        printColorLine();
        unsetProtected();
        break;
    }
  }

  if ( ! isMonochron() )
    setColor(wc.label_fg, wc.label_bg);

  setPrintPos (1, 15);
  short bg = static_cast<AttribDlg*>(getParent())->bgcolor;
  print (" Background color:");

  if ( bg == fc::Default )
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
  FApplication app (argc, argv);

  // Create a dialog box object.
  // This object will be automatically deleted by
  // the parent object "app" (FObject destructor).
  AttribDlg* dialog = new AttribDlg(&app);

  dialog->setGeometry (6, 2, 69, 21);
  dialog->addAccelerator('q');  // press 'q' to quit
  dialog->setShadow();

  // Create the attribute demo widget as a child object from the dialog
  AttribDemo* demo = new AttribDemo(dialog);
  demo->setGeometry (1, 1, 67, 19);

  // Set the dialog object as main widget
  app.setMainWidget(dialog);

  // Show and start the application
  dialog->show();
  return app.exec();
}
