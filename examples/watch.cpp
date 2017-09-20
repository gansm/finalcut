// File: watch.cpp

#include <ctime>

#include <final/fapplication.h>
#include <final/flabel.h>
#include <final/fdialog.h>
#include <final/fmessagebox.h>
#include <final/fswitch.h>

//----------------------------------------------------------------------
// class watch
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class watch : public FDialog
{
  public:
    // Constructor
    explicit watch (FWidget* = 0);

    // Destructor
    ~watch();

    // Method
    void printTime();

    // Event handlers
    void onTimer (FTimerEvent*);
    void onClose (FCloseEvent*);

    // Callback methods
    void cb_clock (FWidget*, data_ptr);
    void cb_seconds (FWidget*, data_ptr);
    void cb_exitApp (FWidget*, data_ptr);

  protected:
    // Method
    void adjustSize();

  private:
    // Disable copy constructor
    watch (const watch&);

    // Disable assignment operator (=)
    watch& operator = (const watch&);

    // Data Members
    bool     sec;
    FLabel*  time_label;
    FLabel*  time_str;
    FSwitch* clock_sw;
    FSwitch* seconds_sw;
};
#pragma pack(pop)

//----------------------------------------------------------------------
watch::watch (FWidget* parent)
  : FDialog(parent)
  , sec(true)
  , time_label(0)
  , time_str(0)
  , clock_sw(0)
  , seconds_sw(0)
{
  setText ("Watch");
  int pw = getParentWidget()->getWidth();
  setGeometry (1 + (pw - 22) / 2, 3, 22, 13);

  // Create labels
  time_label = new FLabel(L"Time", this);
  time_label->setGeometry(5, 2, 5, 1);
  time_label->setEmphasis();
  time_str = new FLabel(L"--:--:--", this);
  time_str->setGeometry(10, 2, 8, 1);

  // Create checkbox buttons
  clock_sw = new FSwitch(L"Clock", this);
  seconds_sw = new FSwitch(L"Seconds", this);
  clock_sw->setGeometry(4, 4, 9, 1);
  seconds_sw->setGeometry(2, 6, 11, 1);
  sec = seconds_sw->setChecked();

  // Create button
  FButton* quit_btn = new FButton(L"&Quit", this);
  quit_btn->setGeometry(6, 9, 9, 1);

  // Connect switch signal "toggled" with a callback member function
  clock_sw->addCallback
  (
    "toggled",
    F_METHOD_CALLBACK (this, &watch::cb_clock)
  );

  // Connect switch signal "toggled" with a callback member function
  seconds_sw->addCallback
  (
    "toggled",
    F_METHOD_CALLBACK (this, &watch::cb_seconds)
  );

  // Connect button signal "clicked" with a callback member function
  quit_btn->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &watch::cb_exitApp)
  );
}

//----------------------------------------------------------------------
watch::~watch()
{
  delOwnTimer();
}

//----------------------------------------------------------------------
void watch::printTime()
{
  FString str;
  std::tm now;
  std::time_t t;

  t = std::time(0);  // get current time
  localtime_r(&t, &now);

  if ( sec )
    str.sprintf("%02d:%02d:%02d", now.tm_hour, now.tm_min, now.tm_sec);
  else
    str.sprintf("%02d:%02d   ", now.tm_hour, now.tm_min);

  *time_str = str;
  time_str->redraw();
}

//----------------------------------------------------------------------
void watch::onTimer (FTimerEvent*)
{
  printTime();
}

//----------------------------------------------------------------------
void watch::onClose (FCloseEvent* ev)
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
void watch::cb_clock (FWidget*, data_ptr)
{
  if ( clock_sw->isChecked() )
  {
    printTime();
    addTimer(1000);
  }
  else
  {
    delAllTimer();
    *time_str = "--:--:--";
    time_str->redraw();
  }
}

//----------------------------------------------------------------------
void watch::cb_seconds (FWidget*, data_ptr)
{
  if ( seconds_sw->isChecked() )
    sec = true;
  else
    sec = false;

  if ( clock_sw->isChecked() )
    printTime();
  else
  {
    if ( sec )
      *time_str = "--:--:--";
    else
      *time_str = "--:--   ";

    time_str->redraw();
  }
}

//----------------------------------------------------------------------
void watch::cb_exitApp (FWidget*, data_ptr)
{
  close();
}

//----------------------------------------------------------------------
void watch::adjustSize()
{
  int pw = getParentWidget()->getWidth();
  setX (1 + (pw - 22) / 2, false);
  FDialog::adjustSize();
}

//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
{
  // Create the application object
  FApplication app(argc, argv);

  // Create a simple dialog box
  watch w(&app);

  // Set dialog w as main widget
  app.setMainWidget(&w);

  // Show and start the application
  w.show();
  return app.exec();
}
