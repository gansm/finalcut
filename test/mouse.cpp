// File: mouse.cpp

#include "fapp.h"
#include "fdialog.h"

//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
{
  // Create the application object
  FApplication app(argc, argv);

  // Create a simple dialog box
  FDialog mouse_draw(&app);
  mouse_draw.setGeometry (12, 4, 60, 18);

  app.setMainWidget(&mouse_draw);
  mouse_draw.show();
  return app.exec();
}
