/***********************************************************************
* input-dialog.cpp - an input field example                            *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2015-2018 Markus Gans                                      *
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
void cb_quit (finalcut::FWidget*, finalcut::FWidget::data_ptr);
void cb_publish (finalcut::FWidget*, finalcut::FWidget::data_ptr);

//----------------------------------------------------------------------
// callback functions
//----------------------------------------------------------------------
void cb_quit (finalcut::FWidget*, finalcut::FWidget::data_ptr data)
{
  finalcut::FApplication* app = static_cast<finalcut::FApplication*>(data);
  app->quit();
}

void cb_publish (finalcut::FWidget* widget, finalcut::FWidget::data_ptr data)
{
  finalcut::FCheckBox* cbox1 = static_cast<finalcut::FCheckBox*>(widget);
  finalcut::FCheckBox* cbox2 = static_cast<finalcut::FCheckBox*>(data);

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
  finalcut::FApplication app(argc, argv);

  // Create a simple dialog box
  finalcut::FDialog dgl(&app);
  dgl.setText ("Data input");
  dgl.setGeometry (4, 2, 37, 22);
  dgl.setShadow();

  // Create input fields
  finalcut::FLineEdit name_field (&dgl);
  finalcut::FLineEdit email_field (&dgl);
  finalcut::FLineEdit org_field (&dgl);
  finalcut::FLineEdit city_field (&dgl);
  finalcut::FLineEdit st_field (&dgl);
  finalcut::FLineEdit c_field (&dgl);

  name_field.setLabelText (L"&Name");
  email_field.setLabelText (L"&Email");
  org_field.setLabelText (L"Or&ganization");
  city_field.setLabelText (L"&City");
  st_field.setLabelText (L"&State");
  c_field.setLabelText (L"&Country");

  name_field.setGeometry (15, 1, 19, 1);
  email_field.setGeometry (15, 3, 19, 1);
  org_field.setGeometry (15, 5, 19, 1);
  city_field.setGeometry (15, 7, 19, 1);
  st_field.setGeometry (15, 9, 19, 1);
  c_field.setGeometry (15, 11, 4, 1);

  // Create the button group
  finalcut::FButtonGroup radioButtonGroup ("Sex", &dgl);
  radioButtonGroup.setGeometry(2, 13, 13, 4);

  // Create radio buttons
  finalcut::FRadioButton male ("&Male", &radioButtonGroup);
  finalcut::FRadioButton female ("&Female", &radioButtonGroup);
  male.setGeometry (1, 1, 8, 1);
  female.setGeometry (1, 2, 10, 1);

  // Create another button group
  finalcut::FButtonGroup checkButtonGroup ("&Data options", &dgl);
  checkButtonGroup.setGeometry(16, 13, 19, 4);

  // Create checkbox buttons
  finalcut::FCheckBox check1 ("Save data", &checkButtonGroup);
  finalcut::FCheckBox check2 ("Encrypt data", &checkButtonGroup);
  check1.setGeometry (1, 1, 13, 1);
  check2.setGeometry (1, 2, 16, 1);
  check2.setDisable();

  // Create a OK button
  finalcut::FButton btn("&OK", &dgl);
  btn.setGeometry (24, 18, 10, 1);

  // Connect checkbox signal "clicked" with a callback function
  check1.addCallback
  (
    "clicked",
    F_FUNCTION_CALLBACK (&cb_publish),
    &check2
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
