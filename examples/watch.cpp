/***********************************************************************
* watch.cpp - A watch with FSwitch widgets                             *
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

#include <ctime>
#include <final/final.h>


//----------------------------------------------------------------------
// class Watch
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class Watch : public FDialog
{
  public:
    // Constructor
    explicit Watch (FWidget* = 0);

    // Destructor
    ~Watch();

    // Method
    void printTime();

    // Event handlers
    void onTimer (FTimerEvent*);
    void onClose (FCloseEvent*);

    // Callback methods
    void cb_clock (FWidget*, data_ptr);
    void cb_seconds (FWidget*, data_ptr);

  protected:
    // Method
    void adjustSize();

  private:
    // Disable copy constructor
    Watch (const Watch&);

    // Disable assignment operator (=)
    Watch& operator = (const Watch&);

    // Data Members
    bool     sec;
    FLabel*  time_label;
    FLabel*  time_str;
    FSwitch* clock_sw;
    FSwitch* seconds_sw;
};
#pragma pack(pop)

//----------------------------------------------------------------------
Watch::Watch (FWidget* parent)
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
    F_METHOD_CALLBACK (this, &Watch::cb_clock)
  );

  // Connect switch signal "toggled" with a callback member function
  seconds_sw->addCallback
  (
    "toggled",
    F_METHOD_CALLBACK (this, &Watch::cb_seconds)
  );

  // Connect button signal "clicked" with a callback member function
  quit_btn->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &FApplication::cb_exitApp)
  );
}

//----------------------------------------------------------------------
Watch::~Watch()
{
  delOwnTimer();
}

//----------------------------------------------------------------------
void Watch::printTime()
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
void Watch::onTimer (FTimerEvent*)
{
  printTime();
}

//----------------------------------------------------------------------
void Watch::onClose (FCloseEvent* ev)
{
  FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void Watch::cb_clock (FWidget*, data_ptr)
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
void Watch::cb_seconds (FWidget*, data_ptr)
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
void Watch::adjustSize()
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
  Watch w(&app);

  // Set dialog w as main widget
  app.setMainWidget(&w);

  // Show and start the application
  w.show();
  return app.exec();
}
