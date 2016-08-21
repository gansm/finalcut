// File: transparent.cpp

#include "fapp.h"
#include "fdialog.h"
#include "flabel.h"
#include "fmessagebox.h"
#include "fstatusbar.h"
#include "fstring.h"


//----------------------------------------------------------------------
// class Transparent
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class Transparent : public FDialog
{
 private:
   bool shadow_win;

 private:
   Transparent (const Transparent&);    // Disabled copy constructor
   Transparent& operator = (const Transparent&); // and operator '='
   void draw();
   void onAccel (FAccelEvent* ev)
   {
     if ( getParentWidget() )
     {
       if ( getParentWidget()->close() )
         ev->accept();
       else
          ev->ignore();
     }
   }

 public:
   explicit Transparent (FWidget* = 0, bool = false);  // constructor
  ~Transparent();  // destructor
};
#pragma pack(pop)

//----------------------------------------------------------------------
Transparent::Transparent (FWidget* parent, bool shadow)
  : FDialog(parent)
  , shadow_win(shadow)
{
  setStatusbarMessage("Press Q to quit");
  addAccelerator('q');
}

//----------------------------------------------------------------------
Transparent::~Transparent()
{ }

//----------------------------------------------------------------------
void Transparent::draw()
{
  FDialog::draw();
  updateVTerm(false);

  if ( isMonochron() )
    setReverse(true);

  if ( shadow_win )
  {
    setColor(wc.shadow_bg, wc.shadow_fg);
    setTransShadow();
  }
  else
    setTransparent();

  FString line(getClientWidth(), wchar_t(' '));

  for (int n=1; n <= getClientHeight(); n++)
  {
    gotoxy(xpos+xmin, ypos+ymin+n);
    print(line);
  }

  if ( shadow_win )
    unsetTransShadow();
  else
    unsetTransparent();

  if ( isMonochron() )
    setReverse(false);

  updateVTerm(true);
}


//----------------------------------------------------------------------
// class MainWindow
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class MainWindow : public FDialog
{
 private:
   FString line1;
   FString line2;

 private:
   MainWindow (const MainWindow&);    // Disabled copy constructor
   MainWindow& operator = (const MainWindow&); // and operator '='
   void draw();
   void onClose (FCloseEvent*);
   void onShow  (FShowEvent*);
   void onTimer (FTimerEvent*);
   void onAccel (FAccelEvent* ev)
   {
     close();
     ev->accept();
   }

 public:
   explicit MainWindow (FWidget* = 0);  // constructor
  ~MainWindow();  // destructor
};
#pragma pack(pop)

//----------------------------------------------------------------------
MainWindow::MainWindow (FWidget* parent)
  : FDialog(parent)
  , line1()
  , line2()
{
  line1 = "     .-.     .-.     .-.";
  line2 = "`._.'   `._.'   `._.'   ";

  Transparent* transpwin = new Transparent(this);
  transpwin->setText("transparent");
  transpwin->setGeometry (6, 11, 29, 12);
  transpwin->show();

  Transparent* shadowwin = new Transparent(this, true);
  shadowwin->setText("shadow");
  shadowwin->setGeometry (47, 11, 29, 12);
  shadowwin->show();

  // Statusbar at the bottom
  FStatusBar* statusbar = new FStatusBar (this);
  statusbar->setMessage("Press Q to quit");

  addAccelerator('q');
}

//----------------------------------------------------------------------
MainWindow::~MainWindow()
{ }

//----------------------------------------------------------------------
void MainWindow::draw()
{
  FDialog::draw();
  updateVTerm(false);
  if ( isMonochron() )
    setReverse(true);

  setColor(foregroundColor, backgroundColor);
  gotoxy(xpos+xmin, ypos+ymin+2);
  print(line1);
  gotoxy(xpos+xmin, ypos+ymin+3);
  print(line2);

  if ( isMonochron() )
    setReverse(false);

  updateVTerm(true);
}

//----------------------------------------------------------------------
void MainWindow::onClose (FCloseEvent* ev)
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
void MainWindow::onShow (FShowEvent*)
{
  addTimer(100);
}

//----------------------------------------------------------------------
void MainWindow::onTimer (FTimerEvent*)
{
  wchar_t first_Char[2];
  uInt length = line1.getLength();
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
  if ( argv[1] && (  strcmp(argv[1], "--help") == 0
                  || strcmp(argv[1], "-h") == 0 ) )
  {
    std::cout << "Generic options:" << std::endl
              << "  -h, --help                  "
              << "Display this help and exit" << std::endl;
    FApplication::print_cmd_Options();
    exit(EXIT_SUCCESS);
  }

  FApplication app (argc, argv);

  MainWindow main_dlg (&app);
  main_dlg.setText ("non-transparent");
  main_dlg.setGeometry (27, 3, 26, 7);

  app.setMainWidget (&main_dlg);
  main_dlg.show();

  return app.exec();
}
