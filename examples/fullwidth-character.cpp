/***********************************************************************
* fullwidth-letter.cpp - Demonstrates use of full-width characters     *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2019 Markus Gans                                           *
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

#define full(...)  finalcut::getFullWidth(__VA_ARGS__)

using finalcut::FPoint;
using finalcut::FSize;


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
{
  // Create the application object
  finalcut::FApplication app(argc, argv);

  // Create a simple dialog box
  finalcut::FDialog dgl(&app);
  dgl.setText (full("Dialog"));
  dgl.setSize (FSize(37, 16));
  dgl.setPos (FPoint( int(app.getDesktopWidth() - dgl.getWidth()) / 2
                    , int(app.getDesktopHeight() - dgl.getHeight()) / 2));
  dgl.setShadow();

  // Create input fields
  finalcut::FLineEdit field1 (&dgl);
  field1.setLabelText (full("Input"));
  field1.setText (L"你好");  // Nǐ hǎo (chinese)
  field1.setStatusbarMessage (full("Type your text here"));
  field1.setGeometry (FPoint(15, 1), FSize(19, 1));

  finalcut::FLineEdit field2 (&dgl);
  field2.setLabelText (L"Comment");
  field2.setText (full(L"Hello"));
  field2.setStatusbarMessage (full("Post a comment"));
  field2.setGeometry (FPoint(15, 3), FSize(19, 1));

  // Create the button group
  finalcut::FButtonGroup group (full("Side"), &dgl);
  group.setGeometry(FPoint(2, 5), FSize(32, 3));

  // Create radio buttons
  finalcut::FRadioButton left ("&" + full("Left"), &group);
  finalcut::FRadioButton right ("&" + full("Right"), &group);
  left.setStatusbarMessage (full("Prefer the left side"));
  right.setStatusbarMessage (full("Prefer the right side"));
  left.setGeometry (FPoint(1, 1), FSize(8, 1));
  right.setGeometry (FPoint(15, 1), FSize(10, 1));

  // Create a scrollable text field
  finalcut::FTextView scroll_text (&dgl);
  scroll_text.setGeometry (FPoint(2, 8), FSize(32, 3));
  finalcut::FString text_line{"FINAL CUT supports "
                              "full-width characters."};
  scroll_text.setStatusbarMessage ("You can scroll right and "
                                   "left with the arrow keys");
  scroll_text.append(full(text_line));

  // Create a OK button
  finalcut::FButton btn("&ＯＫ", &dgl);
  btn.setStatusbarMessage (full("Press Enter to exit the dialog"));
  btn.setGeometry (FPoint(24, 12), FSize(10, 1));

  // Create the status bar
  finalcut::FStatusBar sbar(&dgl);
  finalcut::FStatusKey key_F1 (finalcut::fc::Fkey_f1, "Info", &sbar);

  // Create the menu bar
  finalcut::FMenuBar Menubar(&dgl);

  // Create menu bar items
  finalcut::FMenu     File{L"&Ｆｉｌｅ", &Menubar};
  finalcut::FMenuItem Edit{L"&Ｅｄｉｔ", &Menubar};
  finalcut::FMenuItem Exit{L"Ｅ&ｘｉｔ", &Menubar};

  // Create file menu items
  finalcut::FMenuItem Open{"&Ｏｐｅｎ", &File};
  finalcut::FMenuItem Print{"&Ｐｒｉｎｔ", &File};
  finalcut::FMenuItem Line{&File};
  Line.setSeparator();
  finalcut::FMenuItem Quit{"&Ｑｕｉｔ", &File};
  Quit.addAccelerator (finalcut::fc::Fckey_q);  // Ctrl + Q

  // Callback lambda expressions
  auto cb_exit = \
      [] (finalcut::FWidget*, FDataPtr data)
      {
        auto a = static_cast<finalcut::FApplication*>(data);
        a->quit();
      };

  auto cb_tooltip = \
      [] (finalcut::FWidget*, FDataPtr data)
      {
        auto a = static_cast<finalcut::FApplication*>(data);
        finalcut::FToolTip tooltip(a);
        tooltip.setText (full("A tooltip with\ncharacters\n"
                              "in full-width\nfor 3 seconds"));
        tooltip.show();
        sleep(3);
      };

  // Connect the signals with the callback lambda expressions
  btn.addCallback ("clicked", cb_exit, &app);
  Exit.addCallback ("clicked", cb_exit, &app);
  Quit.addCallback ("clicked", cb_exit, &app);
  key_F1.addCallback ("activate",cb_tooltip, &app);

  // Set dialog object as main widget
  app.setMainWidget(&dgl);

  // Show and start the application
  dgl.show();
  return app.exec();
}
