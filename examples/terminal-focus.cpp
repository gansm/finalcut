/***********************************************************************
* terminal-focus.cpp - Example for terminal focus-in and focus-out     *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2023 Markus Gans                                           *
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


//----------------------------------------------------------------------
// class MyApplication
//----------------------------------------------------------------------

class MyApplication : public finalcut::FApplication
{
  public:
    // Using-declaration
    using finalcut::FApplication::FApplication;

  private:
    // Event handler
    void onTermFocusIn (finalcut::FFocusEvent*) override;
    void onTermFocusOut (finalcut::FFocusEvent*) override;
};

//----------------------------------------------------------------------
void MyApplication::onTermFocusIn (finalcut::FFocusEvent*)
{
  auto color_theme = getColorTheme();
  color_theme->term_bg = finalcut::FColor::LightBlue;
  redraw();
}

//----------------------------------------------------------------------
void MyApplication::onTermFocusOut (finalcut::FFocusEvent*)
{
  auto color_theme = getColorTheme();
  color_theme->term_bg = finalcut::FColor::LightGray;
  redraw();
}


//----------------------------------------------------------------------
// class Dialog
//----------------------------------------------------------------------

class Dialog final : public finalcut::FDialog
{
  public:
    // Constructor
    explicit Dialog (FWidget* = nullptr);

  private:
    void initLayout() override;

    // Data members
    finalcut::FButton quit_button{"&Quit", this};
    finalcut::FLabel label{"Change terminal focus...", this};
};

//----------------------------------------------------------------------
Dialog::Dialog (FWidget* parent)
  : FDialog{"Terminal focus", parent}
{
  quit_button.addCallback
  (
    "clicked",
    finalcut::getFApplication(),
    &finalcut::FApplication::cb_exitApp,
    this
  );
}

//----------------------------------------------------------------------
void Dialog::initLayout()
{
  finalcut::FDialog::setGeometry (FPoint{21, 8}, FSize{40, 6});
  quit_button.setGeometry (FPoint{27, 2}, FSize{10, 1});
  label.setGeometry (FPoint{2, 2}, FSize{24, 1});
  finalcut::FDialog::initLayout();
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

int main (int argc, char* argv[])
{
  MyApplication app(argc, argv);
  Dialog dialog(&app);
  finalcut::FWidget::setMainWidget(&dialog);
  dialog.show();
  return app.exec();
}

