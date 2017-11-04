/***********************************************************************
* keyboard.cpp - Shows typed-in key name                               *
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

//----------------------------------------------------------------------
// class Keyboard
//----------------------------------------------------------------------

class Keyboard : public FWidget
{
  public:
    // Constructor
    explicit Keyboard (FWidget* = 0);

  protected:
    // Event handlers
    void onKeyPress (FKeyEvent*);
    void onAccel (FAccelEvent*);

    void draw();
};

//----------------------------------------------------------------------
Keyboard::Keyboard (FWidget* parent)
  : FWidget(parent)
{
  resetXTermForeground();
  resetXTermBackground();
  wc.term_fg = fc::Default;
  wc.term_bg = fc::Default;
}

//----------------------------------------------------------------------
void Keyboard::onKeyPress (FKeyEvent* ev)
{
  int key_id = ev->key();
  bool is_last_line = false;

  if ( getPrintPos().getY() == getLineNumber() )
    is_last_line = true;

  print() << "Key " << getKeyName(key_id).c_str()
          << " (id " << key_id << ")\n";

  if ( is_last_line )
    scrollAreaForward (vdesktop);

  setAreaCursor (1, getPrintPos().getY(), true, vdesktop);
}

//----------------------------------------------------------------------
void Keyboard::onAccel (FAccelEvent* ev)
{
  quit();
  ev->accept();
}

//----------------------------------------------------------------------
void Keyboard::draw()
{
  setPrintPos (1, 1);
  print() << "---------------\n"
          << "Press Q to quit\n"
          << "---------------\n";
  setAreaCursor (1, 4, true, vdesktop);
}

//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
{
  // Create the application object
  FApplication app(argc, argv);
  app.setForegroundColor(fc::Default);
  app.setBackgroundColor(fc::Default);

  // Create a keyboard object
  Keyboard key(&app);
  key.addAccelerator('q');

  // Set the keyboard object as main widget
  app.setMainWidget(&key);

  // Show and start the application
  key.show();
  return app.exec();
}
