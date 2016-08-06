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
   void draw();
};

//----------------------------------------------------------------------
keyboard::keyboard (FWidget* parent)
  : FWidget(parent)
{
  resetXTermForeground();
  resetXTermBackground();
  wc.term_fg = fc::Default;
  wc.term_bg = fc::Default;
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
void keyboard::draw()
{
  setNormal();
  setColor(fc::Default, fc::Default);
  clearArea();
  gotoxy (1,1);
  print ("---------------\n");
  print ("Press Q to quit\n");
  print ("---------------\n");
  setCursorPos (1,5);
}

//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
{
  FApplication app(argc, argv);
  keyboard key(&app);
  key.addAccelerator('q');
  app.setMainWidget(&key);
  key.show();
  return app.exec();
}
