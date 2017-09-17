// File: transparent.cpp

#include <final/fapplication.h>
#include <final/fdialog.h>
#include <final/flabel.h>
#include <final/fmessagebox.h>
#include <final/fstatusbar.h>
#include <final/fstring.h>


//----------------------------------------------------------------------
// class Transparent
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class Transparent : public FDialog
{
  public:
    // Typedef and Enumeration
    typedef enum ttype
    {
      transparent        = 0,
      shadow             = 1,
      inherit_background = 2
    } trans_type;

  public:
    // Constructor
    explicit Transparent (FWidget* = 0, trans_type = transparent);

    // Destructor
    ~Transparent();

  private:
    // Disable copy constructor
    Transparent (const Transparent&);

    // Disable assignment operator (=)
    Transparent& operator = (const Transparent&);

    // Method
    void draw();

    // Event handlers
    void onKeyPress (FKeyEvent* ev);

    // Data Members
    trans_type type;
};
#pragma pack(pop)

//----------------------------------------------------------------------
Transparent::Transparent (FWidget* parent, Transparent::trans_type tt)
  : FDialog(parent)
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
  FDialog::draw();

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
      setColor(fc::Blue, fc::Black);
    else
      setColor(fc::Green, fc::Black);

    setInheritBackground();
  }
  else
    setTransparent();

  FString line(getClientWidth(), wchar_t('.'));

  for (int n = 1; n <= getClientHeight(); n++)
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
void Transparent::onKeyPress (FKeyEvent* ev)
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
    FDialog::onKeyPress(ev);
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
    // Disable copy constructor
    MainWindow (const MainWindow&);
    // Disable assignment operator (=)
    MainWindow& operator = (const MainWindow&);

    void draw();

    // Event handlers
    void onClose (FCloseEvent*);
    void onShow  (FShowEvent*);
    void onTimer (FTimerEvent*);
    void onKeyPress (FKeyEvent* ev)
    {
      if ( ! ev )
        return;

      if ( ev->key() == 'q' )
      {
        close();
        ev->accept();
      }
      else
        FDialog::onKeyPress(ev);
    }

  public:
    // Constructor
    explicit MainWindow (FWidget* = 0);
    // Destructor
    ~MainWindow();
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
  transpwin->setGeometry (6, 3, 29, 12);
  transpwin->unsetTransparentShadow();

  Transparent* shadowwin = new Transparent(this, Transparent::shadow);
  shadowwin->setText("shadow");
  shadowwin->setGeometry (46, 11, 29, 12);
  shadowwin->unsetTransparentShadow();

  Transparent* ibg = new Transparent(this, Transparent::inherit_background);
  ibg->setText("inherit background");
  ibg->setGeometry (42, 3, 29, 7);
  ibg->unsetTransparentShadow();

  // Statusbar at the bottom
  FStatusBar* statusbar = new FStatusBar (this);
  statusbar->setMessage("Press Q to quit");

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
  FDialog::draw();

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
  if ( argv[1] && ( std::strcmp(argv[1], "--help") == 0
                   || std::strcmp(argv[1], "-h") == 0 ) )
  {
    std::cout << "Generic options:" << std::endl
              << "  -h, --help                  "
              << "Display this help and exit" << std::endl;
    FApplication::print_cmd_Options();
    std::exit(EXIT_SUCCESS);
  }

  FApplication app (argc, argv);

  MainWindow main_dlg (&app);
  main_dlg.setText ("non-transparent");
  main_dlg.setGeometry (8, 16, 26, 7);

  app.setMainWidget (&main_dlg);
  main_dlg.show();

  return app.exec();
}
