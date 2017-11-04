/***********************************************************************
* input-dialog.cpp - an input field example                            *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2015-2017 Markus Gans                                      *
*                                                                      *
* The Final Cut is free software; you can redistribute it and/or       *
* modify it under the terms of the GNU Lesser General Public License   *
* as published by the Free Software Foundation; either version 3 of    *
* the License, or (at your option) any later version.                  *
*                                                                      *
* The Final Cut is distributed in the hope that it will be useful,     *
* but WITHOUT ANY WARRANTY; without even the implied warranty of       *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU Lesser General Public License for more details.                  *
*                                                                      *
* You should have received a copy of the GNU Lesser General Public     *
* License along with this program.  If not, see                        *
* <http://www.gnu.org/licenses/>.                                      *
***********************************************************************/

#include <final/final.h>


// function prototypes
void cb_quit (FWidget*, FWidget::data_ptr);
void cb_publish (FWidget*, FWidget::data_ptr);

//----------------------------------------------------------------------
// callback functions
//----------------------------------------------------------------------
void cb_quit (FWidget*, FWidget::data_ptr data)
{
  FApplication* app = static_cast<FApplication*>(data);
  app->quit();
}

void cb_publish (FWidget* widget, FWidget::data_ptr data)
{
  FCheckBox* cbox1 = static_cast<FCheckBox*>(widget);
  FCheckBox* cbox2 = static_cast<FCheckBox*>(data);

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
  male->setGeometry(1, 1, 8, 1);
  female->setGeometry(1, 2, 10, 1);

  // Create another button group
  FButtonGroup* checkButtonGroup = new FButtonGroup("&Data options", &dgl);
  checkButtonGroup->setGeometry(16, 13, 19, 4);

  // Create checkbox buttons
  FCheckBox* check1 = new FCheckBox("Save data", checkButtonGroup);
  FCheckBox* check2 = new FCheckBox("Encrypt data", checkButtonGroup);
  check1->setGeometry(1, 1, 13, 1);
  check2->setGeometry(1, 2, 16, 1);
  check2->setDisable();

  // Create a OK button
  FButton btn("&OK", &dgl);
  btn.setGeometry (24, 18, 10, 1);

  // Connect checkbox signal "clicked" with a callback function
  check1->addCallback
  (
    "clicked",
    F_FUNCTION_CALLBACK (&cb_publish),
    check2
  );

  // Connect the button signal "clicked" with the callback function
  btn.addCallback
  (
    "clicked",
    F_FUNCTION_CALLBACK (&cb_quit),
    &app
  );

  // Set dialog object as main widget
  app.setMainWidget(&dgl);

  // Show and start the application
  dgl.show();
  return app.exec();
}
