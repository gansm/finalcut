/***********************************************************************
* background-color.cpp - Sets the background color palette             *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2019-2020 Markus Gans                                      *
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

#include <tuple>
#include <utility>
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
    // Typedef
    typedef std::tuple<uChar,uChar,uChar>  RGB;

    // Constructors
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
    void cb_choice (finalcut::FWidget*, FDataPtr);

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
  : FDialog(parent)
{
  // Dialog settings
  setText ("Background color palette");
  setGeometry (FPoint(25, 5), FSize(32, 12));

  // Combobox
  color_choice.setGeometry (FPoint(2, 2), FSize(18, 1));
  color_choice.setLabelOrientation (finalcut::FLineEdit::label_above);
  color_choice.setLabelText ("Color choice");
  color_choice.unsetEditable();

  for (auto& c : color_list)
  {
    FDataPtr data_ptr = reinterpret_cast<FDataPtr>(&c.second);
    finalcut::FListBoxItem item (c.first, data_ptr);
    color_choice.insert(item);
  }

  // Spin boxes
  red.setGeometry (FPoint(2, 5), FSize(7, 1));
  red.setLabelOrientation (finalcut::FLineEdit::label_above);
  red.setLabelText ("Red");
  red.setRange (0, 255);
  red.setValue (0x80);

  green.setGeometry (FPoint(12, 5), FSize(7, 1));
  green.setLabelOrientation (finalcut::FLineEdit::label_above);
  green.setLabelText ("Green");
  green.setRange (0, 255);
  green.setValue (0xa4);

  blue.setGeometry (FPoint(22, 5), FSize(7, 1));
  blue.setLabelOrientation (finalcut::FLineEdit::label_above);
  blue.setLabelText ("Blue");
  blue.setRange (0, 255);
  blue.setValue (0xec);

  // Set the initial palette values
  if ( finalcut::FTerm::canChangeColorPalette() )
  {
    finalcut::FTerm::setPalette ( finalcut::fc::LightMagenta
                                , int(red.getValue())
                                , int(green.getValue())
                                , int(blue.getValue()) );
  }

  // Quit button
  quit.setGeometry(FPoint(19, 8), FSize(10, 1));

  // Add some function callbacks
  quit.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &finalcut::FApplication::cb_exitApp)
  );

  for (const auto spinbox : {&red, &green, &blue})
  {
    spinbox->addCallback
    (
      "changed",
      F_METHOD_CALLBACK (this, &Background::cb_changed)
    );
  }

  for (const auto& signal : {"row-changed", "clicked"})
  {
    color_choice.addCallback
    (
      signal,
      F_METHOD_CALLBACK (this, &Background::cb_choice)
    );
  }
}

//----------------------------------------------------------------------
Background::~Background()  // destructor
{ }

//----------------------------------------------------------------------
void Background::cb_changed (finalcut::FWidget*, FDataPtr)
{
  if ( ! finalcut::FTerm::canChangeColorPalette() )
    return;

  finalcut::FTerm::setPalette ( finalcut::fc::LightMagenta
                              , int(red.getValue())
                              , int(green.getValue())
                              , int(blue.getValue()) );
  redraw();
  updateTerminal();
}

//----------------------------------------------------------------------
void Background::cb_choice (finalcut::FWidget*, FDataPtr)
{
  if ( ! finalcut::FTerm::canChangeColorPalette() )
    return;

  uChar r{}, g{}, b{};
  const FDataPtr data_ptr = color_choice.getItemData();
  const RGB* rgb = reinterpret_cast<RGB*>(data_ptr);
  std::tie(r, g, b) = *rgb;
  red.setValue(r);
  green.setValue(g);
  blue.setValue(b);
  finalcut::FTerm::setPalette ( finalcut::fc::LightMagenta
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

  if ( finalcut::FTerm::canChangeColorPalette() )
    app.setBackgroundColor(finalcut::fc::LightMagenta);

  Background dialog(&app);
  app.setMainWidget(&dialog);
  dialog.show();
  return app.exec();
}

