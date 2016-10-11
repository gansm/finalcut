// File: keyboard.cpp

#include "final.h"

//----------------------------------------------------------------------
// class keyboard
//----------------------------------------------------------------------

class keyboard : public FWidget
{
 public:
   // Constructor
   explicit keyboard (FWidget* = 0);

 protected:
   // Event handlers
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
  bool is_last_line = false;

  if ( getPrintPos().getY() == getLineNumber() )
    is_last_line = true;

  printf ("Key %s (id %d)\n", getKeyName(key_id).c_str(), key_id);


  if ( is_last_line )
  {
    scrollAreaForward (vdesktop);

    if ( ! scrollTermForward() )
      putArea (getTermPos(), vdesktop);
  }

  setAreaCursor (1, getPrintPos().getY(), true, vdesktop);
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
  clearArea (vdesktop);
  setPrintPos (1,1);
  print ("---------------\n");
  print ("Press Q to quit\n");
  print ("---------------\n");
  setAreaCursor (1, 4, true, vdesktop);
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
