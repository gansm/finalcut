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

 public:
   explicit watch (FWidget* parent=0);  // constructor
  ~watch();  // destructor
   void printTime();
   void onTimer (FTimerEvent*);
   void onAccel (FAccelEvent*);
   void onClose (FCloseEvent*);
   void cb_clock (FWidget*, void*);
   void cb_seconds (FWidget*, void*);
   void cb_exitApp (FWidget*, void*);

 protected:
   void adjustSize();
};
#pragma pack(pop)

//----------------------------------------------------------------------
watch::watch (FWidget* parent) : FDialog(parent)
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
  seconds_sw->setChecked();
  sec = true;

  // Create button
  FButton* quit = new FButton(L"&Quit", this);
  quit->setGeometry(6, 9, 9, 1);
  quit->setShadow();
  quit->setFocus();

  // Connect switch signal "toggled" with a callback member function
  clock_sw->addCallback
  (
    "toggled",
    this,
    reinterpret_cast<FWidget::FMemberCallback>(&watch::cb_clock),
    null
  );

  // Connect switch signal "toggled" with a callback member function
  seconds_sw->addCallback
  (
    "toggled",
    this,
    reinterpret_cast<FWidget::FMemberCallback>(&watch::cb_seconds),
    null
  );

  // Connect button signal "clicked" with a callback member function
  quit->addCallback
  (
    "clicked",
    this,
    reinterpret_cast<FWidget::FMemberCallback>(&watch::cb_exitApp),
    null
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
void watch::onAccel (FAccelEvent* ev)
{
  close();
  ev->accept();
}

//----------------------------------------------------------------------
void watch::onClose (FCloseEvent* event)
{
  int ret = FMessageBox::info ( this, "Quit",
                                "Do you really want\n"
                                "to quit the program ?",
                                FMessageBox::Yes,
                                FMessageBox::No );
  if ( ret == FMessageBox::Yes )
    event->accept();
  else
    event->ignore();
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
