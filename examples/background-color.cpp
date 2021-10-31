/***********************************************************************
* background-color.cpp - Sets the background color palette             *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2019-2021 Markus Gans                                      *
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

#include <tuple>
#include <utility>
#include <vector>

#include <final/final.h>

using finalcut::FPoint;
using finalcut::FSize;
using finalcut::FColor;


//----------------------------------------------------------------------
// class Background
//----------------------------------------------------------------------

class Background final : public finalcut::FDialog
{
  public:
    // Using-declaration
    using RGB = std::tuple<uChar, uChar, uChar>;

    // Constructor
    explicit Background (finalcut::FWidget* = nullptr);

  private:
    // Methods
    void initLayout() override;

    // Callback method
    void cb_changed();
    void cb_choice();

    // Data members
    finalcut::FComboBox color_choice{this};
    finalcut::FSpinBox red{this};
    finalcut::FSpinBox green{this};
    finalcut::FSpinBox blue{this};
    finalcut::FButton quit{"&Quit", this};
    std::vector<std::pair<finalcut::FString, RGB>> color_list
    {
      { "Light blue"    , std::make_tuple(0x80, 0xa4, 0xec) },
      { "Vivid blue"    , std::make_tuple(0x37, 0x97, 0xfd) },
      { "Bright blue"   , std::make_tuple(0x3c, 0x85, 0xd2) },
      { "Strong blue"   , std::make_tuple(0x32, 0x64, 0x9f) },
      { "Light cyan"    , std::make_tuple(0x6c, 0xfe, 0xfe) },
      { "Vivid cyan"    , std::make_tuple(0x0b, 0xdd, 0xd4) },
      { "Soft cyan"     , std::make_tuple(0x49, 0xa8, 0xac) },
      { "Light green"   , std::make_tuple(0x81, 0xdf, 0xbb) },
      { "Vivid green"   , std::make_tuple(0x5c, 0x9e, 0x4a) },
      { "Bright green"  , std::make_tuple(0x0f, 0xba, 0x78) },
      { "Strong green"  , std::make_tuple(0x03, 0x8f, 0x68) },
      { "Mint green"    , std::make_tuple(0x4a, 0xfd, 0x91) },
      { "Green"         , std::make_tuple(0x6b, 0xe8, 0x1b) },
      { "Dark green"    , std::make_tuple(0x01, 0x65, 0x05) },
      { "Dark sea green", std::make_tuple(0x7d, 0xb6, 0x96) },
      { "Bright purple" , std::make_tuple(0x83, 0x76, 0xa2) },
      { "Taupe"         , std::make_tuple(0xa6, 0x8c, 0x99) },
      { "Silver"        , std::make_tuple(0xc1, 0xc1, 0xcb) }
    };
};

//----------------------------------------------------------------------
Background::Background (finalcut::FWidget* parent)
  : FDialog{parent}
{
  // Combobox
  color_choice.setLabelOrientation (finalcut::FLineEdit::LabelOrientation::Above);
  color_choice.setLabelText ("Color choice");
  color_choice.unsetEditable();

  for (auto& c : color_list)
  {
    finalcut::FListBoxItem item (c.first, c.second);
    color_choice.insert(item);
  }

  // Spin boxes
  red.setLabelOrientation (finalcut::FLineEdit::LabelOrientation::Above);
  red.setLabelText ("Red");
  red.setRange (0, 255);
  red.setValue (0x80);

  green.setLabelOrientation (finalcut::FLineEdit::LabelOrientation::Above);
  green.setLabelText ("Green");
  green.setRange (0, 255);
  green.setValue (0xa4);

  blue.setLabelOrientation (finalcut::FLineEdit::LabelOrientation::Above);
  blue.setLabelText ("Blue");
  blue.setRange (0, 255);
  blue.setValue (0xec);

  // Set the initial palette values
  if ( finalcut::FTerm::canChangeColorPalette() )
  {
    finalcut::FTerm::setPalette ( FColor::LightMagenta
                                , int(red.getValue())
                                , int(green.getValue())
                                , int(blue.getValue()) );
  }

  // Add some function callbacks
  quit.addCallback
  (
    "clicked",
    finalcut::getFApplication(),
    &finalcut::FApplication::cb_exitApp,
    this
  );

  for (const auto spinbox : {&red, &green, &blue})
  {
    spinbox->addCallback
    (
      "changed",
      this, &Background::cb_changed
    );
  }

  for (const auto& signal : {"row-changed", "clicked"})
  {
    color_choice.addCallback
    (
      signal,
      this, &Background::cb_choice
    );
  }
}

//----------------------------------------------------------------------
void Background::initLayout()
{
  FDialog::setText ("Background color palette");
  FDialog::setGeometry (FPoint{25, 5}, FSize{32, 12});
  color_choice.setGeometry (FPoint{2, 2}, FSize{18, 1});
  red.setGeometry (FPoint{2, 5}, FSize{7, 1});
  green.setGeometry (FPoint{12, 5}, FSize{7, 1});
  blue.setGeometry (FPoint{22, 5}, FSize{7, 1});
  quit.setGeometry(FPoint{19, 8}, FSize{10, 1});  // Quit button
  FDialog::initLayout();
}

//----------------------------------------------------------------------
void Background::cb_changed()
{
  if ( ! finalcut::FTerm::canChangeColorPalette() )
    return;

  finalcut::FTerm::setPalette ( FColor::LightMagenta
                              , int(red.getValue())
                              , int(green.getValue())
                              , int(blue.getValue()) );
  redraw();
}

//----------------------------------------------------------------------
void Background::cb_choice()
{
  if ( ! finalcut::FTerm::canChangeColorPalette() )
    return;

  uChar r{};
  uChar g{};
  uChar b{};
  const RGB rgb = color_choice.getItemData<RGB>();
  std::tie(r, g, b) = rgb;
  red.setValue(r);
  green.setValue(g);
  blue.setValue(b);
  finalcut::FTerm::setPalette ( FColor::LightMagenta
                              , int(red.getValue())
                              , int(green.getValue())
                              , int(blue.getValue()) );
  redraw();
}

//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

int main (int argc, char* argv[])
{
  // Create the application object
  finalcut::FApplication app(argc, argv);

  // Force terminal initialization without calling show()
  app.initTerminal();

  // The following lines require an initialized terminal
  if ( finalcut::FTerm::canChangeColorPalette() )
    app.setBackgroundColor(FColor::LightMagenta);

  Background dialog(&app);
  finalcut::FWidget::setMainWidget(&dialog);
  dialog.show();
  return app.exec();
}

