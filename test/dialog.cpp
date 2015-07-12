// dialog.cpp

#include "fapp.h"
#include "fbutton.h"
#include "fdialog.h"
#include "flabel.h"

// function prototype
void cb_quit (FWidget*, void* data_ptr);

//----------------------------------------------------------------------
// callback function
//----------------------------------------------------------------------
void cb_quit (FWidget*, void* data_ptr)
{
  FApplication* app = static_cast<FApplication*>(data_ptr);
  app->quit();
}

//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
{
  // Create the application object
  FApplication app(argc, argv);

  // Create a simple dialog box
  FDialog dgl(&app);
  dgl.setText ("FDialog");
  dgl.setGeometry (4, 3, 41, 11);
  dgl.setShadow();

  // Create text labels
  FLabel label_1(&dgl),
         label_2(&dgl),
         label_3(&dgl),
         label_4(&dgl),
         label_5(&dgl);

  label_1.setText (wchar_t(fc::BlackUpPointingTriangle));
  label_2.setText (wchar_t(fc::BoxDrawingsUpAndRight)
                  + FString(2, wchar_t(fc::BoxDrawingsHorizontal))
                  + FString(" Double click the title bar button,") );
  label_3.setText ("press Q on the keyboard,");
  label_4.setText ("or push the button below to exit");
  label_5.setText ("the program.");

  label_1.setGeometry (1, 1, 1, 1);
  label_2.setGeometry (1, 2, 38, 1);
  label_3.setGeometry (5, 3, 24, 1);
  label_4.setGeometry (5, 4, 32, 1);
  label_5.setGeometry (5, 5, 12, 1);

  // Create the quit button
  FButton btn("&Quit", &dgl);
  btn.setGeometry (16, 7, 9, 1);
  btn.setShadow();

  // Connect the button signal "clicked" with the callback function
  btn.addCallback
  (
    "clicked",
    reinterpret_cast<FWidget::FCallback>(&cb_quit),
    &app
  );

  app.setMainWidget(&dgl);
  dgl.show();
  return app.exec();
}
