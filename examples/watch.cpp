/***********************************************************************
* watch.cpp - A watch with FSwitch widgets                             *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2023 Markus Gans                                      *
*                                                                      *
* FINAL CUT is free software; you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as       *
* published by the Free Software Foundation; either version 3 of       *
* the License, or (at your option) any later version.                  *
*                                                                      *
* FINAL CUT is distributed in the hope that it will be useful, but     *
* WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU Lesser General Public License for more details.                  *
*                                                                      *
* You should have received a copy of the GNU Lesser General Public     *
* License along with this program.  If not, see                        *
* <http://www.gnu.org/licenses/>.                                      *
***********************************************************************/

#include <ctime>
#include <final/final.h>

using finalcut::FPoint;
using finalcut::FSize;


//----------------------------------------------------------------------
// class Watch
//----------------------------------------------------------------------

class Watch final : public finalcut::FDialog
{
  public:
    // Constructor
    explicit Watch (finalcut::FWidget* = nullptr);

    // Method
    void printTime();

    // Event handlers
    void onTimer (finalcut::FTimerEvent*) override;
    void onClose (finalcut::FCloseEvent*) override;

    // Callback methods
    void cb_clock();
    void cb_seconds();

  protected:
    // Method
    void initLayout() override;
    void adjustSize() override;

  private:
    // Data members
    bool              sec{true};
    finalcut::FLabel  time_label{L"Time", this};
    finalcut::FLabel  time_str{L"--:--:--", this};
    finalcut::FSwitch clock_sw{L"Clock", this};
    finalcut::FSwitch seconds_sw{L"Seconds", this};
    finalcut::FButton quit_btn{L"&Quit", this};
};

//----------------------------------------------------------------------
Watch::Watch (FWidget* parent)
  : finalcut::FDialog{parent}
{
  // Labels
  time_label.setEmphasis();

  // Switch
  seconds_sw.setChecked();
  sec = seconds_sw.isChecked();

  // Connect switch signal "toggled" with a callback member function
  clock_sw.addCallback
  (
    "toggled",
    this, &Watch::cb_clock
  );

  // Connect switch signal "toggled" with a callback member function
  seconds_sw.addCallback
  (
    "toggled",
    this, &Watch::cb_seconds
  );

  // Connect button signal "clicked" with a callback member function
  quit_btn.addCallback
  (
    "clicked",
    finalcut::getFApplication(),
    &finalcut::FApplication::cb_exitApp,
    this
  );
}

//----------------------------------------------------------------------
void Watch::printTime()
{
  finalcut::FString str{};
  struct tm local_tm{};
  using namespace std::chrono;
  auto now = system_clock::to_time_t(system_clock::now());
  auto lt = *localtime_r(&now, &local_tm);

  if ( sec )
    str.sprintf("%02d:%02d:%02d", lt.tm_hour, lt.tm_min, lt.tm_sec);
  else
    str.sprintf("%02d:%02d   ", lt.tm_hour, lt.tm_min);

  time_str = str;
  time_str.redraw();
}

//----------------------------------------------------------------------
void Watch::onTimer (finalcut::FTimerEvent*)
{
  printTime();
}

//----------------------------------------------------------------------
void Watch::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void Watch::cb_clock()
{
  if ( clock_sw.isChecked() )
  {
    printTime();
    addTimer(1000);  // Call onTimer() every second (1000 ms)
  }
  else
  {
    delAllTimers();  // Delete all timers and stop updating the time
    time_str = "--:--:--";
    time_str.redraw();
  }
}

//----------------------------------------------------------------------
void Watch::cb_seconds()
{
  sec = seconds_sw.isChecked();

  if ( clock_sw.isChecked() )
    printTime();
  else
  {
    if ( sec )
      time_str = "--:--:--";
    else
      time_str = "--:--   ";

    time_str.redraw();
  }
}

//----------------------------------------------------------------------
void Watch::initLayout()
{
  // Dialog settings
  FDialog::setText ("Watch");
  FDialog::setSize ({22, 13}, false);

  // Labels
  time_label.setGeometry(FPoint{5, 2}, FSize{5, 1});
  time_str.setGeometry(FPoint{10, 2}, FSize{8, 1});

  // Switches
  clock_sw.setGeometry(FPoint{4, 4}, FSize{9, 1});
  seconds_sw.setGeometry(FPoint{2, 6}, FSize{11, 1});

  // Quit button
  quit_btn.setGeometry(FPoint{6, 9}, FSize{9, 1});

  FDialog::initLayout();
}

//----------------------------------------------------------------------
void Watch::adjustSize()
{
  const auto pw = int(getDesktopWidth());
  setX (1 + (pw - 22) / 2, false);
  setY (3, false);
  finalcut::FDialog::adjustSize();
}

//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
auto main (int argc, char* argv[]) -> int
{
  // Create the application object
  finalcut::FApplication app{argc, argv};

  // Create a simple dialog box
  Watch w{&app};

  // Set dialog w as main widget
  finalcut::FWidget::setMainWidget(&w);

  // Show and start the application
  w.show();
  return app.exec();
}
