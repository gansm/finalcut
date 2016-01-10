// File: term-attributes.cpp

#include "fapp.h"
#include "fbutton.h"
#include "fdialog.h"
#include "fmessagebox.h"

//----------------------------------------------------------------------
// class AttribDlg
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class AttribDlg : public FDialog
{
 private:
   FButton* next_button;
   FButton* back_button;

 public:
   short bgcolor;

 private:
   AttribDlg (const AttribDlg&);             // Disabled copy constructor
   AttribDlg& operator = (const AttribDlg&); // and operator '='
   void adjustSize();

 protected:
   friend class AttribDemo;

 public:
   explicit AttribDlg (FWidget* = 0);  // constructor
  ~AttribDlg();                        // destructor
   void onAccel (FAccelEvent*);
   void onWheel (FWheelEvent*);
   void onClose (FCloseEvent*);
   void cb_next (FWidget* = 0, void* = 0);
   void cb_back (FWidget* = 0, void* = 0);
};
#pragma pack(pop)

//----------------------------------------------------------------------
AttribDlg::AttribDlg (FWidget* parent)
  : FDialog(parent)
  , next_button()
  , back_button()
  , bgcolor(wc.label_bg)
{
  resetXTermForeground();
  resetXTermBackground();
  setText ( "A terminal attributes test ("
          + FString(getTermType())
          + ")");

  next_button = new FButton("&Next >", this);
  next_button->setGeometry(getWidth()-13, getHeight()-4, 10, 1);
  next_button->setShadow();
  next_button->addAccelerator(fc::Fkey_right);
  back_button = new FButton("< &Back", this);
  back_button->setGeometry(getWidth()-25, getHeight()-4, 10, 1);
  back_button->setShadow();
  back_button->addAccelerator(fc::Fkey_left);

  // Add function callbacks
  next_button->addCallback
  (
    "clicked",
    _METHOD_CALLBACK (this, &AttribDlg::cb_next)
  );
  back_button->addCallback
  (
    "clicked",
    _METHOD_CALLBACK (this, &AttribDlg::cb_back)
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

  if ( wheel == WheelUp )
    cb_next();
  else if ( wheel == WheelDown )
    cb_back();
}

//----------------------------------------------------------------------
void AttribDlg::onClose (FCloseEvent* ev)
{
  int ret = FMessageBox::info ( this, "Quit"
                              , "Do you really want\n"
                                "to quit the program ?"
                              , FMessageBox::Yes
                              , FMessageBox::No );
  if ( ret == FMessageBox::Yes )
    ev->accept();
  else
    ev->ignore();
}

//----------------------------------------------------------------------
void AttribDlg::cb_next (FWidget*, void*)
{
  if ( isMonochron() )
    return;
  bgcolor++;
  if ( bgcolor >= getMaxColor() )
    bgcolor = fc::Default;
  redraw();
}

//----------------------------------------------------------------------
void AttribDlg::cb_back (FWidget*, void*)
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
  int x = ((parentWidget()->getWidth() - getWidth()) / 2 );
  int y = ((parentWidget()->getHeight() - getHeight()) / 2 ) + 1;

  setGeometry(x, y, 69, 21, false);
  next_button->setGeometry(getWidth()-13, getHeight()-4, 10, 1, false);
  back_button->setGeometry(getWidth()-25, getHeight()-4, 10, 1, false);
  FDialog::adjustSize();
}


//----------------------------------------------------------------------
// class AttribDemo
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class AttribDemo : public FWidget
{
 private:
   int colors;

 private:
   void printColorLine();
   void printAltCharset();
   void draw();

 public:
   explicit AttribDemo (FWidget* = 0);  // constructor
  ~AttribDemo()                         // destructor
   { }
   void onWheel (FWheelEvent* ev)
   {
     AttribDlg* p = dynamic_cast<AttribDlg*>(parentWidget());
     if ( p )
       p->onWheel(ev);
   }
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

  for (short color=0; color < colors; color++)
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

  gotoxy (xpos + xmin - 1, ypos + ymin - 1);
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
  setUpdateVTerm(false);

  // test alternate character set
  printAltCharset();

  for (int y=0; y < parentWidget()->getHeight()-7; y++)
  {
    gotoxy ( xpos + xmin - 1,
             ypos + ymin + y );

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
  gotoxy (xpos + xmin - 1, ypos + ymin + 13);
  printf ( " Background color: %d  ",
           static_cast<AttribDlg*>(getParent())->bgcolor );
  gotoxy (xpos + xmin + 14, ypos + ymin + 15);
  print ("Change background color ->");

  setUpdateVTerm(true);
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

  AttribDemo* demo = new AttribDemo(dialog);
  demo->setGeometry (1,1,67,19);

  app.setMainWidget(dialog);
  dialog->show();

  return app.exec();
}
