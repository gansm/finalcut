/***********************************************************************
* input-dialog.cpp - An input field example                            *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2020 Markus Gans                                      *
*                                                                      *
* FINAL CUT is free software; you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as       *
* published by the Free Software Foundation; either version 3 of       *
* the License, or (at your option) any later version.                  *
*                                                                      *
* FINAL CUT is distributed in the hope that it will be useful, but     *
* WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU Lesser General Public License for more details.                  *
*                                                                      *
* You should have received a copy of the GNU Lesser General Public     *
* License along with this program.  If not, see                        *
* <http://www.gnu.org/licenses/>.                                      *
***********************************************************************/

#include <final/final.h>

using finalcut::FPoint;
using finalcut::FSize;

// function prototypes
void cb_quit (finalcut::FApplication&);
void cb_publish (finalcut::FCheckBox&, finalcut::FCheckBox&);


//----------------------------------------------------------------------
// callback functions
//----------------------------------------------------------------------
void cb_quit (finalcut::FApplication& app)
{
  app.quit();
}

//----------------------------------------------------------------------
void cb_publish (finalcut::FCheckBox& cbox1, finalcut::FCheckBox& cbox2)
{
  if ( cbox1.isChecked() )
    cbox2.setEnable();
  else
  {
    cbox2.unsetChecked();
    cbox2.setDisable();
  }

  cbox2.redraw();
}

//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
{
  // Create the application object
  finalcut::FApplication app{argc, argv};

  // Create a simple dialog box
  finalcut::FDialog dgl{&app};
  dgl.setText ("Data input");
  dgl.setGeometry (FPoint{4, 2}, FSize{37, 22});
  dgl.setShadow();

  // Create input fields
  finalcut::FLineEdit name_field {&dgl};
  finalcut::FLineEdit pw_field {&dgl};
  finalcut::FLineEdit email_field {&dgl};
  finalcut::FLineEdit city_field {&dgl};
  finalcut::FLineEdit st_field {&dgl};
  finalcut::FLineEdit c_field {&dgl};

  // Set input type to password
  pw_field.setInputType (finalcut::FLineEdit::password);

  name_field.setLabelText (L"&Name");
  pw_field.setLabelText (L"&Password");
  email_field.setLabelText (L"&Email");
  city_field.setLabelText (L"&City");
  st_field.setLabelText (L"&State");
  c_field.setLabelText (L"&Country");

  name_field.setGeometry (FPoint{11, 1}, FSize{23, 1});
  pw_field.setGeometry (FPoint{11, 3}, FSize{23, 1});
  email_field.setGeometry (FPoint{11, 5}, FSize{23, 1});
  city_field.setGeometry (FPoint{11, 7}, FSize{23, 1});
  st_field.setGeometry (FPoint{11, 9}, FSize{23, 1});
  c_field.setGeometry (FPoint{11, 11}, FSize{4, 1});

  // Create the button group
  finalcut::FButtonGroup radioButtonGroup {"Sex", &dgl};
  radioButtonGroup.setGeometry(FPoint{2, 13}, FSize{13, 4});

  // Create radio buttons
  finalcut::FRadioButton male {"&Male", &radioButtonGroup};
  finalcut::FRadioButton female {"&Female", &radioButtonGroup};
  male.setGeometry (FPoint{1, 1}, FSize{8, 1});
  female.setGeometry (FPoint{1, 2}, FSize{10, 1});

  // Create another button group
  finalcut::FButtonGroup checkButtonGroup {"&Data options", &dgl};
  checkButtonGroup.setGeometry(FPoint{16, 13}, FSize{19, 4});

  // Create checkbox buttons
  finalcut::FCheckBox check1 {"Save data", &checkButtonGroup};
  finalcut::FCheckBox check2 {"Encrypt data", &checkButtonGroup};
  check1.setGeometry (FPoint{1, 1}, FSize{13, 1});
  check2.setGeometry (FPoint{1, 2}, FSize{16, 1});
  check2.setDisable();

  // Create a OK button
  finalcut::FButton btn {"&OK", &dgl};
  btn.setGeometry (FPoint{24, 18}, FSize{10, 1});

  // Connect checkbox signal "clicked" with a callback function
  check1.addCallback
  (
    "clicked",
    &cb_publish,
    std::ref(check1),
    std::ref(check2)
  );

  // Connect the button signal "clicked" with the callback function
  btn.addCallback
  (
    "clicked",
    &cb_quit,
    std::ref(app)
  );

  // Set dialog object as main widget
  finalcut::FWidget::setMainWidget(&dgl);

  // Show and start the application
  dgl.show();
  return app.exec();
}
