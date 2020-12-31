/***********************************************************************
* 7segment.cpp - Seven-segment display                                 *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2012-2020 Markus Gans                                      *
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

#include <array>
#include <map>
#include <vector>

#include <final/final.h>

namespace fc = finalcut::fc;
using finalcut::FColorPair;
using finalcut::FColor;
using finalcut::FRect;
using finalcut::FPoint;
using finalcut::FSize;

//----------------------------------------------------------------------
// class SegmentView
//----------------------------------------------------------------------

class SegmentView final : public finalcut::FDialog
{
  public:
    // Constructor
    explicit SegmentView (finalcut::FWidget* = nullptr);

  private:
    struct sevenSegment
    {
      unsigned char a : 1;
      unsigned char b : 1;
      unsigned char c : 1;
      unsigned char d : 1;
      unsigned char e : 1;
      unsigned char f : 1;
      unsigned char g : 1;
      unsigned char   : 1;  // padding bit
    };

    // Methods
    void hexEncoding();
    void get7Segment (const wchar_t);
    void draw() override;

    // Data members
    std::map<wchar_t, sevenSegment> code{};
    std::array<finalcut::FString, 3> line{};
    finalcut::FLineEdit input{"0123", this};
    finalcut::FButton exit{"E&xit", this};
};

//----------------------------------------------------------------------
SegmentView::SegmentView (finalcut::FWidget* parent)
  : FDialog{parent}
{
  // Dialog settings
  //   Avoids calling a virtual function from the constructor
  //   (CERT, OOP50-CPP)
  FDialog::setText ("Seven-segment display");
  FDialog::setGeometry (FPoint{25, 5}, FSize{42, 15});

  // Set encoding
  hexEncoding();

  // Input field
  input.setGeometry (FPoint(2, 2), FSize{12, 1});
  input.setLabelText (L"&Hex value");
  input.setLabelText (L"&Hex-digits or (.) (:) (H) (L) (P) (U)");
  input.setLabelOrientation(finalcut::FLineEdit::LabelOrientation::Above);
  input.setMaxLength(9);
  input.setInputFilter("[:.hHlLpPuU[:xdigit:]]");

  // Exit button
  exit.setGeometry(FPoint{28, 11}, FSize{10, 1});

  // Add some function callbacks
  input.addCallback
  (
    "changed",
    [] (SegmentView& dialog)
    {
      dialog.redraw();
    },
    std::ref(*this)
  );

  exit.addCallback
  (
    "clicked",
    finalcut::getFApplication(),
    &finalcut::FApplication::cb_exitApp,
    this
  );
}

//----------------------------------------------------------------------
void SegmentView::hexEncoding()
{
  code['0'] = sevenSegment{1, 1, 1, 1, 1, 1, 0};
  code['1'] = sevenSegment{0, 1, 1, 0, 0, 0, 0};
  code['2'] = sevenSegment{1, 1, 0, 1, 1, 0, 1};
  code['3'] = sevenSegment{1, 1, 1, 1, 0, 0, 1};
  code['4'] = sevenSegment{0, 1, 1, 0, 0, 1, 1};
  code['5'] = sevenSegment{1, 0, 1, 1, 0, 1, 1};
  code['6'] = sevenSegment{1, 0, 1, 1, 1, 1, 1};
  code['7'] = sevenSegment{1, 1, 1, 0, 0, 0, 0};
  code['8'] = sevenSegment{1, 1, 1, 1, 1, 1, 1};
  code['9'] = sevenSegment{1, 1, 1, 1, 0, 1, 1};
  code['A'] = sevenSegment{1, 1, 1, 0, 1, 1, 1};
  code['B'] = sevenSegment{0, 0, 1, 1, 1, 1, 1};
  code['C'] = sevenSegment{1, 0, 0, 1, 1, 1, 0};
  code['D'] = sevenSegment{0, 1, 1, 1, 1, 0, 1};
  code['E'] = sevenSegment{1, 0, 0, 1, 1, 1, 1};
  code['F'] = sevenSegment{1, 0, 0, 0, 1, 1, 1};
}

//----------------------------------------------------------------------
void SegmentView::get7Segment (const wchar_t c)
{
  for (std::size_t i{0}; i < 3; i++)
    line[i].clear();

  switch ( c )
  {
    case ':':
      line[0] = ' ';
      line[1] = '.';
      line[2] = '.';
      break;

    case '.':
      line[0] = ' ';
      line[1] = ' ';
      line[2] = '.';
      break;

    case 'H':
      line[0] = "   ";
      line[1] = "|_|";
      line[2] = "| |";
      break;

    case 'L':
      line[0] = "   ";
      line[1] = "|  ";
      line[2] = "|_ ";
      break;

    case 'P':
      line[0] = " _ ";
      line[1] = "|_|";
      line[2] = "|  ";
      break;

    case 'U':
      line[0] = "   ";
      line[1] = "| |";
      line[2] = "|_|";
      break;

    default:
      // Hexadecimal digit from 0 up to f
      if ( code.find(c) != code.end() )
      {
        const sevenSegment& s = code[c];
        constexpr std::array<char, 2> h{{' ', '_'}};
        constexpr std::array<char, 2> v{{' ', '|'}};

        line[0] <<   ' '  << h[s.a] <<   ' ';
        line[1] << v[s.f] << h[s.g] << v[s.b];
        line[2] << v[s.e] << h[s.d] << v[s.c];
      }
  }
}

//----------------------------------------------------------------------
void SegmentView::draw()
{
  std::vector<finalcut::FTermBuffer> tbuffer{3};
  finalcut::FTermBuffer left_space{};

  FDialog::draw();
  setColor(FColor::LightGray, FColor::Black);
  finalcut::drawBorder(this, FRect(FPoint{3, 6}, FPoint{40, 11}));

  for (auto&& ch : input.getText().toUpper())
  {
    const FColorPair color{FColor::LightRed, FColor::Black};
    get7Segment(ch);

    for (std::size_t i{0}; i < 3; i++)
      tbuffer[i] << color << line[i] << " ";
  }

  const std::size_t length = tbuffer[0].getLength();

  if ( length < 36 )
    left_space << finalcut::FString(36 - length, ' ');

  print() << FPoint {4, 7} << left_space << tbuffer[0]
          << FPoint {4, 8} << left_space << tbuffer[1]
          << FPoint {4, 9} << left_space << tbuffer[2]
          << FPoint {4, 10} << finalcut::FString{36, ' '};
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

int main (int argc, char* argv[])
{
  finalcut::FApplication app(argc, argv);
  SegmentView dialog(&app);
  finalcut::FWidget::setMainWidget(&dialog);
  dialog.show();
  return app.exec();
}
