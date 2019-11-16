/***********************************************************************
* background-color.cpp - Sets the background color palette             *
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

#include <map>
#include <vector>

#include <final/final.h>

using finalcut::FPoint;
using finalcut::FSize;


//----------------------------------------------------------------------
// class Background
//----------------------------------------------------------------------

class Background : public finalcut::FDialog
{
  public:
    explicit Background (finalcut::FWidget* = nullptr);

    // Disable copy constructor
    Background (const Background&) = delete;

    // Destructor
    ~Background();

    // Disable assignment operator (=)
    Background& operator = (const Background&) = delete;

  private:
    // Callback method
    void cb_changed (finalcut::FWidget*, FDataPtr);

    // Data members
    finalcut::FSpinBox red{this};
    finalcut::FSpinBox green{this};
    finalcut::FSpinBox blue{this};
    finalcut::FButton quit{"&Quit", this};
};

//----------------------------------------------------------------------
Background::Background (finalcut::FWidget* parent)
  : FDialog(parent)
{
  // Dialog settings
  setText ("Background color palette");
  setGeometry (FPoint(25, 5), FSize(32, 9));

  // Spin boxes
  red.setGeometry (FPoint(2, 2), FSize(7, 1));
  red.setLabelOrientation (finalcut::FLineEdit::label_above);
  red.setLabelText ("Red");
  red.setRange (0, 255);
  red.setValue (0x80);

  green.setGeometry (FPoint(12, 2), FSize(7, 1));
  green.setLabelOrientation (finalcut::FLineEdit::label_above);
  green.setLabelText ("Green");
  green.setRange (0, 255);
  green.setValue (0xa4);

  blue.setGeometry (FPoint(22, 2), FSize(7, 1));
  blue.setLabelOrientation (finalcut::FLineEdit::label_above);
  blue.setLabelText ("Blue");
  blue.setRange (0, 255);
  blue.setValue (0xec);

  // Set the initial palette values
  const auto& wc = getFWidgetColors();
  finalcut::FTerm::setPalette ( wc.term_bg
                              , int(red.getValue())
                              , int(green.getValue())
                              , int(blue.getValue()) );

  // Quit button
  quit.setGeometry(FPoint(19, 5), FSize(10, 1));

  // Add some function callbacks
  quit.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &finalcut::FApplication::cb_exitApp)
  );

  red.addCallback
  (
    "changed",
    F_METHOD_CALLBACK (this, &Background::cb_changed)
  );

  green.addCallback
  (
    "changed",
    F_METHOD_CALLBACK (this, &Background::cb_changed)
  );

  blue.addCallback
  (
    "changed",
    F_METHOD_CALLBACK (this, &Background::cb_changed)
  );
}

//----------------------------------------------------------------------
Background::~Background()  // destructor
{ }

//----------------------------------------------------------------------
void Background::cb_changed (finalcut::FWidget*, FDataPtr)
{
  const auto& wc = getFWidgetColors();
  finalcut::FTerm::setPalette ( wc.term_bg
                              , int(red.getValue())
                              , int(green.getValue())
                              , int(blue.getValue()) );
  redraw();
  updateTerminal();
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

int main (int argc, char* argv[])
{
  finalcut::FApplication app(argc, argv);
  Background dialog(&app);
  app.setMainWidget(&dialog);
  dialog.show();
  return app.exec();
}

