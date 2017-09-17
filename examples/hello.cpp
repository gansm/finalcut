// File: hello.cpp

#include "fapplication.h"
#include "fmessagebox.h"


int main (int argc, char* argv[])
{
  // Create the application object
  FApplication app(argc, argv);

  // Create a simple dialog box
  FMessageBox mbox(&app);
  mbox.setText("Hello world");
  mbox.exec();
}
