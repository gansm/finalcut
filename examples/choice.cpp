/************************************************************************
* os-choice.cpp - FButtonGroup with scroll view                         *
*                                                                       *
* This file is part of the Final Cut widget toolkit                     *
*                                                                       *
* Copyright 2017 Markus Gans                                            *
*                                                                       *
* The Final Cut is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by  *
* the Free Software Foundation; either version 3 of the License, or     *
* (at your option) any later version.                                   *
*                                                                       *
* The Final Cut is distributed in the hope that it will be useful,      *
* but WITHOUT ANY WARRANTY; without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
* GNU General Public License for more details.                          *
*                                                                       *
* You should have received a copy of the GNU General Public License     *
* along with this program.  If not, see <http://www.gnu.org/licenses/>. *
************************************************************************/

#include <final/fapplication.h>
#include <final/fbutton.h>
#include <final/fbuttongroup.h>
#include <final/fdialog.h>
#include <final/fradiobutton.h>
#include <final/ftooltip.h>

// function prototypes
void cb_quit (FWidget*, FWidget::data_ptr);


//----------------------------------------------------------------------
// callback functions
//----------------------------------------------------------------------
void cb_quit (FWidget*, FWidget::data_ptr data)
{
  FDialog* dlg = static_cast<FDialog*>(data);
  dlg->close();
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
  x = (app.getColumnNumber() - w) / 2;
  y = (app.getLineNumber() - h) / 2;
  dgl->setGeometry (x, y, w, h);

  // Create another button group
  FButtonGroup* checkButtonGroup = new FButtonGroup("choice", dgl);
  checkButtonGroup->setGeometry (2, 1, 16, 7);

  // Create radio buttons
  FRadioButton* os1 = new FRadioButton("AIX", checkButtonGroup);
  FRadioButton* os2 = new FRadioButton("Cygwin", checkButtonGroup);
  FRadioButton* os3 = new FRadioButton("FreeBSD", checkButtonGroup);
  FRadioButton* os4 = new FRadioButton("HP-UX", checkButtonGroup);
  FRadioButton* os5 = new FRadioButton("Linux", checkButtonGroup);
  FRadioButton* os6 = new FRadioButton("Mac OS X", checkButtonGroup);
  FRadioButton* os7 = new FRadioButton("NetBSD", checkButtonGroup);
  FRadioButton* os8 = new FRadioButton("OpenBSD", checkButtonGroup);
  FRadioButton* os9 = new FRadioButton("Solaris", checkButtonGroup);

  // Set the radio button geometry
  // => checkButtonGroup->setScrollSize(...) is not required
  //    because a FButtonGroup is self-adjusting
  os1->setGeometry(1, 1, 12, 1);
  os2->setGeometry(1, 2, 12, 1);
  os3->setGeometry(1, 3, 12, 1);
  os4->setGeometry(1, 4, 12, 1);
  os5->setGeometry(1, 5, 12, 1);
  os6->setGeometry(1, 6, 12, 1);
  os7->setGeometry(1, 7, 12, 1);
  os8->setGeometry(1, 8, 12, 1);
  os9->setGeometry(1, 9, 12, 1);

#if defined(_AIX)
  os1->setChecked();
  os1->setFocus();
#elif defined(__CYGWIN__)
  os2->setChecked();
  os2->setFocus();
#elif defined(__FreeBSD__)
  os3->setChecked();
  os3->setFocus();
#elif defined(__hpux)
  os4->setChecked();
  os4->setFocus();
#elif defined(__linux__)
  os5->setChecked();
  os5->setFocus();
#elif defined(__APPLE__) && defined(__MACH__)
  os6->setChecked();
  os6->setFocus();
#elif defined(__NetBSD__)
  os7->setChecked();
  os7->setFocus();
#elif defined(__OpenBSD__)
  os8->setChecked();
  os8->setFocus();
#elif defined(__sun) && defined(__SVR4)
  os9->setChecked();
  os9->setFocus();
#endif

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
