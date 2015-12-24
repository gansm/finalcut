// File: term-attributes.cpp

#include "fapp.h"
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
   void adjustSize();

 public:
   explicit AttribDlg (FWidget* = 0);  // constructor
  ~AttribDlg();                        // destructor
   void onAccel (FAccelEvent*);
   void onClose (FCloseEvent*);
};
#pragma pack(pop)

//----------------------------------------------------------------------
AttribDlg::AttribDlg (FWidget* parent)
  : FDialog(parent)
{
  setText ("A terminal attributes test");
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
void AttribDlg::adjustSize()
{
  int h = parentWidget()->getHeight() - 2;
  int x = ((parentWidget()->getWidth() - getWidth()) / 2 );
  setGeometry(x, 2, 62, h, false);
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
   void draw();

 public:
   explicit AttribDemo (FWidget* parent = 0)  // constructor
     : FWidget(parent)
     , colors(getMaxColor())
   {
     if ( isMonochron() )
       colors = 1;
     else if ( colors > 16 )
       colors = 16;
   }

  ~AttribDemo()  // destructor
   { }
};
#pragma pack(pop)

//----------------------------------------------------------------------
void AttribDemo::printColorLine()
{
  int bg = getTermBackgroundColor();
  for (int color=0; color < colors; color++)
  {
    setColor(color, bg);
    print (" # ");
  }
}

//----------------------------------------------------------------------
void AttribDemo::draw()
{
  setUpdateVTerm(false);
  int color_loop = 0;
  std::vector<int> background;
  std::vector<int>::iterator iter;
  background.push_back(fc::White);
  background.push_back(fc::Black);
  background.push_back(-1);
  iter = background.begin();

  while ( iter != background.end() )
  {
    for (int y=color_loop*7; y < parentWidget()->getHeight()-3; y++)
    {
      if ( *iter == -1 )
        break;
      else if ( *iter == fc::Black )
      {
        if ( colors < 16 )
          setColor(fc::LightGray, fc::Black);
        else
          setColor(fc::White, fc::Black);
      }
      else
        setColor(fc::Black, *iter);

      gotoxy ( xpos + xmin - 1,
               ypos + ymin - 1 + y );

      switch (y - (color_loop*7))
      {
        case 0:
          print("       Dim: ");
          setDim();
          printColorLine();
          unsetDim();
          break;

        case 1:
          print("    Normal: ");
          printColorLine();
          break;

        case 2:
          print("      Bold: ");
          setBold();
          printColorLine();
          unsetBold();
          break;

        case 3:
          print("  Bold+Dim: ");
          setBold();
          setDim();
          printColorLine();
          unsetDim();
          unsetBold();
          break;

        case 4:
          print("    Italic: ");
          setItalic();
          printColorLine();
          unsetItalic();
          break;

        case 5:
          print(" Underline: ");
          setUnderline();
          printColorLine();
          unsetUnderline();
          break;

        case 6:
          print("   Reverse: ");
          setReverse();
          printColorLine();
          unsetReverse();
          break;
      }
    }
    color_loop++;
    ++iter;
  }

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

  dialog->setGeometry (6, 2, 62, 22);
  dialog->addAccelerator('q');  // press 'q' to quit
  dialog->setShadow();

  AttribDemo* demo = new AttribDemo(dialog);
  demo->ignorePadding(false);
  demo->setGeometry (1,1,63,20);

  app.setMainWidget(dialog);
  dialog->show();

  return app.exec();
}
