/***********************************************************************
* choice.cpp - FButtonGroup with scroll view                           *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2017-2018 Markus Gans                                      *
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

#include <vector>
#include <final/final.h>

// Typedef
typedef std::shared_ptr<finalcut::FRadioButton> FRadioButtonPtr;

// Function prototypes
void cb_quit (finalcut::FWidget*, finalcut::FWidget::data_ptr);
void populateChoice (std::vector<FRadioButtonPtr>&, finalcut::FButtonGroup&);
void preset (std::vector<FRadioButtonPtr>&);

//----------------------------------------------------------------------
// Callback functions
//----------------------------------------------------------------------
void cb_quit (finalcut::FWidget*, finalcut::FWidget::data_ptr data)
{
  auto dlg = static_cast<finalcut::FDialog*>(data);
  dlg->close();
}

//----------------------------------------------------------------------
void populateChoice ( std::vector<FRadioButtonPtr>& os
                    , finalcut::FButtonGroup& group )
{
  os[0] = std::make_shared<finalcut::FRadioButton>("AIX", &group);
  os[1] = std::make_shared<finalcut::FRadioButton>("Cygwin", &group);
  os[2] = std::make_shared<finalcut::FRadioButton>("FreeBSD", &group);
  os[3] = std::make_shared<finalcut::FRadioButton>("HP-UX", &group);
  os[4] = std::make_shared<finalcut::FRadioButton>("Linux", &group);
  os[5] = std::make_shared<finalcut::FRadioButton>("Mac OS X", &group);
  os[6] = std::make_shared<finalcut::FRadioButton>("NetBSD", &group);
  os[7] = std::make_shared<finalcut::FRadioButton>("OpenBSD", &group);
  os[8] = std::make_shared<finalcut::FRadioButton>("Solaris", &group);
}

//----------------------------------------------------------------------
void preset (std::vector<FRadioButtonPtr>& os)
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
  finalcut::FString label_text = "no OS";

  // Create the application object
  finalcut::FApplication app(argc, argv);

  {  // Create a simple modal dialog box in this scope
    finalcut::FDialog dgl(&app);
    dgl.setModal();
    dgl.setText ("UNIX select");
    std::size_t w = 20;
    std::size_t h = 13;
    int x = int(app.getDesktopWidth() - w) / 2;
    int y = int(app.getDesktopHeight() - h) / 2;
    dgl.setGeometry (x, y, w, h);

    // Create a button group
    finalcut::FButtonGroup checkButtonGroup("choice", &dgl);
    checkButtonGroup.setGeometry (2, 1, 16, 7);

    // Create radio buttons
    std::vector<FRadioButtonPtr> os(9);
    populateChoice (os, checkButtonGroup);

    // Set the radio button geometry
    // => checkButtonGroup.setScrollSize(...) is not required
    //    because a FButtonGroup is self-adjusting
    for (uInt i = 0; i < os.size(); i++)
      os[i]->setGeometry(1, int(1 + i), 12, 1);

    preset(os);

    // Scroll to the focused child element
    finalcut::FFocusEvent cfi (finalcut::fc::ChildFocusIn_Event);
    finalcut::FApplication::queueEvent(&checkButtonGroup, &cfi);

    // Create a OK button
    finalcut::FButton ok("&OK", &dgl);
    ok.setGeometry (10, 9, 8, 1);

    // Connect the button signal "clicked" with the callback function
    ok.addCallback
    (
      "clicked",
      F_FUNCTION_CALLBACK (&cb_quit),
      &dgl
    );

    // Show the dialog
    dgl.show();

    // Get the checked radio button text
    for (int n = 1; n <= int(checkButtonGroup.getCount()); n++)
    {
      if ( checkButtonGroup.isChecked(n) )
      {
        label_text = checkButtonGroup.getButton(n)->getText();
        break;
      }
    }
  }  // Hide and destroy the dialog object


  // Create and show tooltip for two seconds
  finalcut::FToolTip tooltip(&app);
  tooltip.setText ("You have chosen " + label_text);
  tooltip.show();
  sleep(2);
}
