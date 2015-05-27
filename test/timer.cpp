// timer.cpp

#include "final.h"

//----------------------------------------------------------------------
// class timer
//----------------------------------------------------------------------

class timer : public FWidget
{
 public:
   explicit timer (FWidget* parent = 0);
 protected:
   virtual void draw();
   void onTimer (FTimerEvent*);
   void onAccel (FAccelEvent*);
};

//----------------------------------------------------------------------
timer::timer (FWidget* parent) : FWidget(parent)
{
  addTimer (60000);        // 1-minute timer
  int id = addTimer (50);  // 50-millisecond timer
  addTimer (1000);         // 1-second timer
  delTimer (id);
  addTimer (250);          // 250-millisecond timer

  setColor(fc::LightGray, fc::Black);
  clrscr();
  updateTerminal();
}

//----------------------------------------------------------------------
void timer::draw()
{
  setTermXY(1,2);
  ::printf("---------------\n\r");
  ::printf("Press Q to quit\n\r");
  ::printf("---------------\n\r\n");
}

//----------------------------------------------------------------------
void timer::onTimer (FTimerEvent* ev)
{
  ::printf("timer event, id %d\n\r", ev->timerId() );
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
