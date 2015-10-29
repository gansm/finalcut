// File: keyboard.cpp

#include "final.h"

//----------------------------------------------------------------------
// class keyboard
//----------------------------------------------------------------------

class keyboard : public FWidget
{
 public:
   explicit keyboard (FWidget* = 0);
 protected:
   void onKeyPress (FKeyEvent*);
   void onAccel (FAccelEvent*);
};

//----------------------------------------------------------------------
keyboard::keyboard (FWidget* parent)
  : FWidget(parent)
{
  setColor(fc::LightGray, fc::Black);
  clrscr();
  updateTerminal();
}

//----------------------------------------------------------------------
void keyboard::onKeyPress (FKeyEvent* ev)
{
  int key_id = ev->key();
  ::printf("Key %s (id %d)\n\r", getKeyName(key_id).c_str(), key_id);
}

//----------------------------------------------------------------------
void keyboard::onAccel (FAccelEvent* ev)
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
  keyboard t(&app);
  t.addAccelerator('q');
  app.setMainWidget(&t);
  t.show();
  t.setTermXY(0,0);
  t.flush_out();
  ::printf("---------------\n\r");
  ::printf("Press Q to quit\n\r");
  ::printf("---------------\n\r\n");
  return app.exec();
}
