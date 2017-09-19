// File: keyboard.cpp

#include <final/final.h>

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

  print() << "Key " << getKeyName(key_id).c_str()
          << " (id " << key_id << ")\n";

  if ( is_last_line )
    scrollAreaForward (vdesktop);

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
  setPrintPos (1, 1);
  print() << "---------------\n"
          << "Press Q to quit\n"
          << "---------------\n";
  setAreaCursor (1, 4, true, vdesktop);
}

//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
{
  // Create the application object
  FApplication app(argc, argv);
  app.setForegroundColor(fc::Default);
  app.setBackgroundColor(fc::Default);

  // Create a keyboard object
  keyboard key(&app);
  key.addAccelerator('q');

  // Set the keyboard object as main widget
  app.setMainWidget(&key);

  // Show and start the application
  key.show();
  return app.exec();
}
