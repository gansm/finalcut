/***********************************************************************
* timer.cpp - Using timer events                                       *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2023 Markus Gans                                      *
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

#include <final/final.h>


//----------------------------------------------------------------------
// class Timer
//----------------------------------------------------------------------

class Timer final : public finalcut::FWidget
{
  public:
    // Constructor
    explicit Timer (finalcut::FWidget* = nullptr);

  private:
    // Method
    void draw() override;

    // Event handlers
    void onTimer (finalcut::FTimerEvent*) override;
    void onAccel (finalcut::FAccelEvent*) override;
};

//----------------------------------------------------------------------
Timer::Timer (finalcut::FWidget* parent)
  : finalcut::FWidget{parent}
{
  addTimer (60000);              // 1-minute timer
  addTimer (250);                // 250-millisecond timer
  addTimer (1000);               // 1-second timer
  const int id = addTimer (50);  // 50-millisecond timer
  delTimer (id);

  getColorTheme()->term.fg = finalcut::FColor::Default;
  getColorTheme()->term.bg = finalcut::FColor::Default;
}

//----------------------------------------------------------------------
void Timer::draw()
{
  print() << finalcut::FPoint{1, 1}
          << "---------------\n"
          << "Press Q to quit\n"
          << "---------------";
  setAreaCursor ({1, 4}, true, getVirtualDesktop());
}

//----------------------------------------------------------------------
void Timer::onTimer (finalcut::FTimerEvent* ev)
{
  bool is_last_line{false};
  const int timer_id = ev->getTimerId();

  if ( getPrintPos().getY() == int(getDesktopHeight()) )
    is_last_line = true;

  if ( is_last_line )
  {
    scrollAreaForward (getVirtualDesktop());
    print() << '\r';
  }
  else
    print() << '\n';

  print() << finalcut::FColorPair {finalcut::FColor(1 + timer_id)}
          << "Timer event, id " << timer_id;
  setAreaCursor (getPrintPos(), true, getVirtualDesktop());
  forceTerminalUpdate();
}

//----------------------------------------------------------------------
void Timer::onAccel (finalcut::FAccelEvent* ev)
{
  quit();
  ev->accept();
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
auto main (int argc, char* argv[]) -> int
{
  // Create the application object
  finalcut::FApplication app{argc, argv};
  finalcut::FVTerm::setNonBlockingRead();

  // Force terminal initialization without calling show()
  app.initTerminal();
  app.setForegroundColor(finalcut::FColor::Default);
  app.setBackgroundColor(finalcut::FColor::Default);

  // Create a timer object t
  Timer t{&app};
  t.addAccelerator(finalcut::FKey('q'));

  // Set the timer object t as main widget
  finalcut::FWidget::setMainWidget(&t);

  // Show and start the application
  t.show();
  return app.exec();
}
