// File: timer.cpp

#include "final.h"

//----------------------------------------------------------------------
// class timer
//----------------------------------------------------------------------

class timer : public FWidget
{
 public:
   // Constructor
   explicit timer (FWidget* = 0);

 protected:
   // Method
   void draw();

   // Event handlers
   void onTimer (FTimerEvent*);
   void onAccel (FAccelEvent*);
};

//----------------------------------------------------------------------
timer::timer (FWidget* parent)
  : FWidget(parent)
{
  addTimer (60000);        // 1-minute timer
  int id = addTimer (50);  // 50-millisecond timer
  addTimer (1000);         // 1-second timer
  delTimer (id);
  addTimer (250);          // 250-millisecond timer

  resetXTermForeground();
  resetXTermBackground();
  wc.term_fg = fc::Default;
  wc.term_bg = fc::Default;
}

//----------------------------------------------------------------------
void timer::draw()
{
  setNormal();
  clearArea (vdesktop);
  setPrintPos (1,1);
  print ("---------------\n");
  print ("Press Q to quit\n");
  print ("---------------\n");
  setAreaCursor (1, 4, true, vdesktop);
}

//----------------------------------------------------------------------
void timer::onTimer (FTimerEvent* ev)
{
  bool is_last_line = false;
  int timer_id = ev->timerId();

  if ( getPrintPos().getY() == getLineNumber() )
    is_last_line = true;

  setColor (short(1 + timer_id), fc::Default);
  printf ("timer event, id %d\n", timer_id );

  if ( is_last_line )
  {
    scrollAreaForward (vdesktop);

    if ( ! scrollTermForward() )
      putArea (getTermPos(), vdesktop);
  }

  setAreaCursor (1, getPrintPos().getY(), true, vdesktop);
}

//----------------------------------------------------------------------
void timer::onAccel (FAccelEvent* ev)
{
  quit();
  ev->accept();
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
{
  FApplication app(argc, argv);
  timer t(&app);
  t.addAccelerator('q');
  app.setMainWidget(&t);
  t.show();
  return app.exec();
}
