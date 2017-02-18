// File: input-dialog.cpp

#include "fapp.h"
#include "fbutton.h"
#include "fbuttongroup.h"
#include "fcheckbox.h"
#include "fdialog.h"
#include "flineedit.h"
#include "fradiobutton.h"

// function prototypes
void cb_quit (FWidget*, void*);
void cb_publish (FWidget*, void*);

//----------------------------------------------------------------------
// callback functions
//----------------------------------------------------------------------
void cb_quit (FWidget*, void* data_ptr)
{
  FApplication* app = static_cast<FApplication*>(data_ptr);
  app->quit();
}

void cb_publish (FWidget* widget, void* data_ptr)
{
  FCheckBox* cbox1 = static_cast<FCheckBox*>(widget);
  FCheckBox* cbox2 = static_cast<FCheckBox*>(data_ptr);

  if ( cbox1->isChecked() )
    cbox2->setEnable();
  else
  {
    cbox2->unsetChecked();
    cbox2->setDisable();
  }
  cbox2->redraw();
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
  dgl.setText ("Data input");
  dgl.setGeometry (4, 2, 37, 22);
  dgl.setShadow();

  // Create input fields
  FLineEdit* name_field  = new FLineEdit(&dgl);
  FLineEdit* email_field = new FLineEdit(&dgl);
  FLineEdit* org_field   = new FLineEdit(&dgl);
  FLineEdit* city_field  = new FLineEdit(&dgl);
  FLineEdit* st_field    = new FLineEdit(&dgl);
  FLineEdit* c_field     = new FLineEdit(&dgl);

  name_field->setLabelText(L"&Name");
  email_field->setLabelText(L"&Email");
  org_field->setLabelText(L"Or&ganization");
  city_field->setLabelText(L"&City");
  st_field->setLabelText(L"&State");
  c_field->setLabelText(L"&Country");

  name_field->setGeometry(15, 1, 19, 1);
  email_field->setGeometry(15, 3, 19, 1);
  org_field->setGeometry(15, 5, 19, 1);
  city_field->setGeometry(15, 7, 19, 1);
  st_field->setGeometry(15, 9, 19, 1);
  c_field->setGeometry(15, 11, 4, 1);

  // Create the button group
  FButtonGroup* radioButtonGroup = new FButtonGroup("Sex", &dgl);
  radioButtonGroup->setGeometry(2, 13, 13, 4);

  // Create radio buttons
  FRadioButton* male = new FRadioButton("&Male", radioButtonGroup);
  FRadioButton* female = new FRadioButton("&Female", radioButtonGroup);
  male->setGeometry(1, 1, 7, 1);
  female->setGeometry(1, 2, 7, 1);

  // Create another button group
  FButtonGroup* checkButtonGroup = new FButtonGroup("&Data options", &dgl);
  checkButtonGroup->setGeometry(16, 13, 19, 4);

  // Create checkbox buttons
  FCheckBox* check1 = new FCheckBox("Save data", checkButtonGroup);
  FCheckBox* check2 = new FCheckBox("Encrypt data", checkButtonGroup);
  check1->setGeometry(1, 1, 7, 1);
  check2->setGeometry(1, 2, 7, 1);
  check2->setDisable();

  // Create a OK button
  FButton btn("&OK", &dgl);
  btn.setGeometry (24, 18, 10, 1);

  // Connect checkbox signal "clicked" with a callback function
  check1->addCallback
  (
    "clicked",
    _FUNCTION_CALLBACK (&cb_publish),
    check2
  );

  // Connect the button signal "clicked" with the callback function
  btn.addCallback
  (
    "clicked",
    _FUNCTION_CALLBACK (&cb_quit),
    &app
  );

  app.setMainWidget(&dgl);
  dgl.show();
  return app.exec();
}
