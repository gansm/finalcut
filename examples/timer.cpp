/***********************************************************************
* timer.cpp - Using timer events                                       *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2014-2018 Markus Gans                                      *
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

#include <final/final.h>


//----------------------------------------------------------------------
// class Timer
//----------------------------------------------------------------------

class Timer : public finalcut::FWidget
{
  public:
    // Constructor
    explicit Timer (finalcut::FWidget* = 0);

  protected:
    // Method
    virtual void draw();

    // Event handlers
    virtual void onTimer (finalcut::FTimerEvent*);
    virtual void onAccel (finalcut::FAccelEvent*);
};

//----------------------------------------------------------------------
Timer::Timer (finalcut::FWidget* parent)
  : finalcut::FWidget(parent)
{
  addTimer (60000);        // 1-minute timer
  int id = addTimer (50);  // 50-millisecond timer
  addTimer (1000);         // 1-second timer
  delTimer (id);
  addTimer (250);          // 250-millisecond timer

  wc.term_fg = finalcut::fc::Default;
  wc.term_bg = finalcut::fc::Default;
}

//----------------------------------------------------------------------
void Timer::draw()
{
  setPrintPos (1, 1);
  print() << "---------------\n"
          << "Press Q to quit\n"
          << "---------------\n";
  setAreaCursor (1, 4, true, vdesktop);
}

//----------------------------------------------------------------------
void Timer::onTimer (finalcut::FTimerEvent* ev)
{
  bool is_last_line = false;
  int timer_id = ev->timerId();

  if ( getPrintPos().getY() == int(getDesktopHeight()) )
    is_last_line = true;

  setColor (FColor(1 + timer_id), finalcut::fc::Default);
  print() << "Timer event, id " << timer_id << '\n';

  if ( is_last_line )
    scrollAreaForward (vdesktop);

  setAreaCursor (1, getPrintPos().getY(), true, vdesktop);
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
int main (int argc, char* argv[])
{
  // Create the application object
  finalcut::FApplication app(argc, argv);
  app.setForegroundColor(finalcut::fc::Default);
  app.setBackgroundColor(finalcut::fc::Default);

  // Create a timer object t
  Timer t(&app);
  t.addAccelerator('q');

  // Set the timer object t as main widget
  app.setMainWidget(&t);

  // Show and start the application
  t.show();
  return app.exec();
}
