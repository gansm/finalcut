// File: dialog.cpp

#include <final/fapplication.h>
#include <final/fbutton.h>
#include <final/fdialog.h>
#include <final/flabel.h>

// function prototype
void cb_quit (FWidget*, FWidget::data_ptr);

//----------------------------------------------------------------------
// callback function
//----------------------------------------------------------------------
void cb_quit (FWidget*, FWidget::data_ptr data)
{
  FApplication* app = static_cast<FApplication*>(data);
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

  // Create text labels
  FLabel label_1(&dgl),
         label_2(&dgl);

  label_1.setText ( wchar_t(fc::BlackUpPointingTriangle)
                  + std::wstring(L"\n")
                  + wchar_t(fc::BoxDrawingsUpAndRight)
                  + FString(2, wchar_t(fc::BoxDrawingsHorizontal))
                  + " Double click the title bar button," );
  label_2.setText ( "press Q on the keyboard,\n"
                    "or push the button below to exit\n"
                    "the program." );

  label_1.setGeometry (1, 1, 38, 2);
  label_2.setGeometry (5, 3, 34, 3);

  // Create the quit button
  FButton btn("&Quit", &dgl);
  btn.setGeometry (16, 7, 9, 1);

  // Connect the button signal "clicked" with the callback function
  btn.addCallback
  (
    "clicked",
    F_FUNCTION_CALLBACK (&cb_quit),
    &app
  );

  app.setMainWidget(&dgl);
  dgl.show();
  return app.exec();
}
