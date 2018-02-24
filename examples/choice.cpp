/***********************************************************************
* choice.cpp - FButtonGroup with scroll view                           *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2017 Markus Gans                                           *
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
void populateChoice (std::vector<FRadioButton*>&, FButtonGroup*);
void preset (std::vector<FRadioButton*>&);

//----------------------------------------------------------------------
// callback functions
//----------------------------------------------------------------------
void cb_quit (FWidget*, FWidget::data_ptr data)
{
  FDialog* dlg = static_cast<FDialog*>(data);
  dlg->close();
}

//----------------------------------------------------------------------
void populateChoice (std::vector<FRadioButton*>& os, FButtonGroup* group)
{
  os[0] = new FRadioButton("AIX", group);
  os[1] = new FRadioButton("Cygwin", group);
  os[2] = new FRadioButton("FreeBSD", group);
  os[3] = new FRadioButton("HP-UX", group);
  os[4] = new FRadioButton("Linux", group);
  os[5] = new FRadioButton("Mac OS X", group);
  os[6] = new FRadioButton("NetBSD", group);
  os[7] = new FRadioButton("OpenBSD", group);
  os[8] = new FRadioButton("Solaris", group);
}

//----------------------------------------------------------------------
void preset (std::vector<FRadioButton*>& os)
{
#if defined(_AIX)
  os[0]->setChecked();
  os[0]->setFocus();
#elif defined(__CYGWIN__)
  os[1]->setChecked();
  os[1]->setFocus();
#elif defined(__FreeBSD__)
  os[2]->setChecked();
  os[2]->setFocus();
#elif defined(__hpux)
  os[3]->setChecked();
  os[3]->setFocus();
#elif defined(__linux__)
  os[4]->setChecked();
  os[4]->setFocus();
#elif defined(__APPLE__) && defined(__MACH__)
  os[5]->setChecked();
  os[5]->setFocus();
#elif defined(__NetBSD__)
  os[6]->setChecked();
  os[6]->setFocus();
#elif defined(__OpenBSD__)
  os[7]->setChecked();
  os[7]->setFocus();
#elif defined(__sun) && defined(__SVR4)
  os[8]->setChecked();
  os[8]->setFocus();
#endif
}

//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
{
  int x, y, w, h;
  FString label_text = "no OS";

  // Create the application object
  FApplication app(argc, argv);

  // Create a simple modal dialog box
  FDialog* dgl = new FDialog(&app);
  dgl->setModal();
  dgl->setText ("UNIX select");
  w = 20;
  h = 13;
  x = (app.getDesktopWidth() - w) / 2;
  y = (app.getDesktopHeight() - h) / 2;
  dgl->setGeometry (x, y, w, h);

  // Create a button group
  FButtonGroup* checkButtonGroup = new FButtonGroup("choice", dgl);
  checkButtonGroup->setGeometry (2, 1, 16, 7);

  // Create radio buttons
  std::vector<FRadioButton*> os (9);
  populateChoice (os, checkButtonGroup);

  // Set the radio button geometry
  // => checkButtonGroup->setScrollSize(...) is not required
  //    because a FButtonGroup is self-adjusting
  for (uInt i = 0; i < os.size(); i++)
    os[i]->setGeometry(1, int(1 + i), 12, 1);

  preset(os);

  // Scroll to the focused child element
  FFocusEvent cfi (fc::ChildFocusIn_Event);
  FApplication::sendEvent(checkButtonGroup, &cfi);

  // Create a OK button
  FButton* ok = new FButton("&OK", dgl);
  ok->setGeometry (10, 9, 8, 1);

  // Connect the button signal "clicked" with the callback function
  ok->addCallback
  (
    "clicked",
    F_FUNCTION_CALLBACK (&cb_quit),
    dgl
  );

  // Show the dialog
  dgl->show();

  // Get the checked radio button text
  for (int n = 1; n <= int(checkButtonGroup->getCount()); n++)
  {
    if ( checkButtonGroup->isChecked(n) )
    {
      label_text = checkButtonGroup->getButton(n)->getText();
      break;
    }
  }

  // Hide and destroy the dialog object
  delete dgl;

  // Create and show tooltip for two seconds
  FToolTip* tooltip = new FToolTip(&app);
  tooltip->setText ("You have chosen " + label_text);
  tooltip->show();
  sleep(2);
  delete tooltip;
}
