/***********************************************************************
* keyboard.cpp - Shows typed-in key name                               *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2022 Markus Gans                                      *
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
#include <sstream>
#include <final/final.h>

//----------------------------------------------------------------------
// class Keyboard
//----------------------------------------------------------------------

class Keyboard final : public finalcut::FWidget
{
  public:
    // Constructor
    explicit Keyboard (finalcut::FWidget* = nullptr);

  protected:
    // Event handlers
    void onKeyPress (finalcut::FKeyEvent*) override;
    void onAccel (finalcut::FAccelEvent*) override;

  private:
    // Methods
    void draw() override;
};

//----------------------------------------------------------------------
Keyboard::Keyboard (finalcut::FWidget* parent)
  : finalcut::FWidget{parent}
{
  getColorTheme()->term_fg = finalcut::FColor::Default;
  getColorTheme()->term_bg = finalcut::FColor::Default;
}

//----------------------------------------------------------------------
void Keyboard::onKeyPress (finalcut::FKeyEvent* ev)
{
  const auto key_id = ev->key();
  auto key_name = finalcut::FVTerm::getFOutput()->getKeyName(key_id);
  bool is_last_line{false};

  if ( key_name.isEmpty() )
    key_name = wchar_t(key_id);

  if ( getPrintPos().getY() == int(getDesktopHeight()) )
    is_last_line = true;

  if ( is_last_line )
  {
    scrollAreaForward (getVirtualDesktop());
    print() << '\r';
  }
  else
    print() << '\n';
  std::ostringstream ss;
  ss << std::hex << uInt32(key_id);
  finalcut::FString hexkey = ss.str();
  print() << "Key " << key_name << " (id " << hexkey << ")" ;
  setAreaCursor (getPrintPos(), true, getVirtualDesktop());
  forceTerminalUpdate();
}

//----------------------------------------------------------------------
void Keyboard::onAccel (finalcut::FAccelEvent* ev)
{
  quit();
  ev->accept();
}

//----------------------------------------------------------------------
void Keyboard::draw()
{
  print() << finalcut::FPoint{1, 1}
          << "---------------\n"
          << "Press Q to quit\n"
          << "---------------";
  setAreaCursor ({1, 4}, true, getVirtualDesktop());
}

//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
auto main (int argc, char* argv[]) -> int
{
  // Create the application object
  finalcut::FApplication app{argc, argv};

  // Force terminal initialization without calling show()
  app.initTerminal();
  app.setForegroundColor(finalcut::FColor::Default);
  app.setBackgroundColor(finalcut::FColor::Default);

  // Create a keyboard object
  Keyboard key{&app};
  key.addAccelerator(finalcut::FKey('q'));

  // Set the keyboard object as main widget
  finalcut::FWidget::setMainWidget(&key);

  // Show and start the application
  key.show();
  return app.exec();
}
