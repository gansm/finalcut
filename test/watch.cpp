// watch.cpp

#include <ctime>

#include "fapp.h"
#include "flabel.h"
#include "fdialog.h"
#include "fmessagebox.h"
#include "fswitch.h"

//----------------------------------------------------------------------
// class watch
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class watch : public FDialog
{
 private:
   bool sec;

 private:
   FLabel* time_label;
   FLabel* time_str;
   FSwitch* clock_sw;
   FSwitch* seconds_sw;

 private:
   watch (const watch&);             // Disabled copy constructor
   watch& operator = (const watch&); // and operator '='

 public:
   explicit watch (FWidget* parent=0);  // constructor
  ~watch();  // destructor
   void printTime();
   void onTimer (FTimerEvent*);
   void onClose (FCloseEvent*);
   void cb_clock (FWidget*, void*);
   void cb_seconds (FWidget*, void*);
   void cb_exitApp (FWidget*, void*);

 protected:
   void adjustSize();
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
  setShadow();
  int pw = parentWidget()->getWidth();
  setGeometry (1+(pw-22)/2, 3, 22, 13);

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
  quit_btn->setShadow();


  // Connect switch signal "toggled" with a callback member function
  clock_sw->addCallback
  (
    "toggled",
    _METHOD_CALLBACK (this, &watch::cb_clock)
  );

  // Connect switch signal "toggled" with a callback member function
  seconds_sw->addCallback
  (
    "toggled",
    _METHOD_CALLBACK (this, &watch::cb_seconds)
  );

  // Connect button signal "clicked" with a callback member function
  quit_btn->addCallback
  (
    "clicked",
    _METHOD_CALLBACK (this, &watch::cb_exitApp)
  );
}

//----------------------------------------------------------------------
watch::~watch()
{
  delAllTimer();
}

//----------------------------------------------------------------------
void watch::printTime()
{
  FString str;
  struct tm* now;
  time_t t;

  t = time(0);  // get current time
  now = localtime(&t);

  if ( sec )
    str.sprintf("%02d:%02d:%02d", now->tm_hour, now->tm_min, now->tm_sec);
  else
    str.sprintf("%02d:%02d   ", now->tm_hour, now->tm_min);

  time_str->setText(str);
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
void watch::cb_clock (FWidget*, void*)
{
  if ( clock_sw->isChecked() )
  {
    printTime();
    addTimer(1000);
  }
  else
  {
    delAllTimer();
    time_str->setText("--:--:--");
    time_str->redraw();
  }
}

//----------------------------------------------------------------------
void watch::cb_seconds (FWidget*, void*)
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
      time_str->setText("--:--:--");
    else
      time_str->setText("--:--   ");
    time_str->redraw();
  }
}

//----------------------------------------------------------------------
void watch::cb_exitApp (FWidget*, void*)
{
  close();
}

//----------------------------------------------------------------------
void watch::adjustSize()
{
  int pw = parentWidget()->getWidth();
  setX (1+(pw-22)/2, false);
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

  app.setMainWidget(&w);
  w.show();
  return app.exec();
}
