/***********************************************************************
* parallax-scrolling.cpp - Parallax scrolling in four layers           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2021-2023 Markus Gans                                      *
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

#include <cmath>
#include <ctime>
#include <array>
#include <random>
#include <unordered_set>

#include <final/final.h>
#include "xpmimage.h"

namespace fc = finalcut;

// XPM data
constexpr std::array<const char*, 97> saturn_xpm = \
{{
  "20 20 76 1",
  " \tc None",
  ".\tc #4F4F4F",
  "+\tc #767676",
  "@\tc #363636",
  "#\tc #3E3E3E",
  "$\tc #727272",
  "%\tc #909090",
  "&\tc #878787",
  "*\tc #717171",
  "=\tc #656565",
  "-\tc #858585",
  ";\tc #8E8E8E",
  ">\tc #898989",
  ",\tc #606060",
  "'\tc #838383",
  ")\tc #7A7A7A",
  "!\tc #3A3A3A",
  "~\tc #575757",
  "{\tc #464646",
  "]\tc #6A6A6A",
  "^\tc #797979",
  "/\tc #9D9D9D",
  "(\tc #A9A9A9",
  "_\tc #AEAEAE",
  ":\tc #A2A2A2",
  "<\tc #535353",
  "[\tc #5D5D5D",
  "}\tc #A3A3A3",
  "|\tc #B5B5B5",
  "1\tc #BEBEBE",
  "2\tc #BBBBBB",
  "3\tc #B2B2B2",
  "4\tc #4A4A4A",
  "5\tc #434343",
  "6\tc #A1A1A1",
  "7\tc #B9B9B9",
  "8\tc #C9C9C9",
  "9\tc #C4C4C4",
  "0\tc #C1C1C1",
  "a\tc #929292",
  "b\tc #7F7F7F",
  "c\tc #B4B4B4",
  "d\tc #CBCBCB",
  "e\tc #CFCFCF",
  "f\tc #CCCCCC",
  "g\tc #C5C5C5",
  "h\tc #D1D1D1",
  "i\tc #B6B6B6",
  "j\tc #999999",
  "k\tc #C0C0C0",
  "l\tc #CDCDCD",
  "m\tc #DDDDDD",
  "n\tc #8F8F8F",
  "o\tc #6D6D6D",
  "p\tc #C8C8C8",
  "q\tc #E4E4E4",
  "r\tc #626262",
  "s\tc #E2E2E2",
  "t\tc #B3B3B3",
  "u\tc #AAAAAA",
  "v\tc #7E7E7E",
  "w\tc #D5D5D5",
  "x\tc #C7C7C7",
  "y\tc #919191",
  "z\tc #4D4D4D",
  "A\tc #A8A8A8",
  "B\tc #2B2B2B",
  "C\tc #959595",
  "D\tc #A5A5A5",
  "E\tc #888888",
  "F\tc #777777",
  "G\tc #828282",
  "H\tc #949494",
  "I\tc #7D7D7D",
  "J\tc #2E2E2E",
  "K\tc #494949",
  "                    ",
  "                .+@ ",
  "              #$%&* ",
  "             =&-;>. ",
  "        ,'>).!~{>]  ",
  "      =^/(_:>< ['@  ",
  "      +}|123(;4-5   ",
  "     <6788920ab.    ",
  "     ^cdefghi>$     ",
  "     jk8l8m8n}o     ",
  "     /kphqh%__r     ",
  "    =j|fs8%t1u.     ",
  "   =v;kwc%|x7y      ",
  "  =*=z3%y|xkA.      ",
  " B[-# ]Ctk7D[       ",
  " 5EFrG+~EHI         ",
  "J]Cy>[              ",
  "5IE*5               ",
  "##KJ                ",
  "                    "
}};

struct restoreOverlaid : public fc::FVTerm
{
  ~restoreOverlaid() override;

  void operator () (fc::FVTerm& obj) const
  {
    if ( ! getWindowList() || getWindowList()->empty() )
      return;

    bool overlaid{false};

    for (auto&& window : *getWindowList())
    {
      const auto win = static_cast<fc::FWidget*>(window);

      if ( overlaid && win->getVWin()->visible )
        putArea (win->getTermPos(), win->getVWin());

      if ( obj.getVWin() == win->getVWin() )
        overlaid = true;
    }
  }
};

restoreOverlaid::~restoreOverlaid() = default;

template <>
struct std::hash<fc::FPoint>
{
  auto operator () (const fc::FPoint& p) const noexcept -> std::size_t
  {
    size_t seed = 0;
    const auto hash1 = std::hash<int>{}(p.getY());
    const auto hash2 = std::hash<int>{}(p.getX());
    seed ^= hash1 + 0x9e3779b9 + (seed << 6u) + (seed >> 2u);
    seed ^= hash2 + 0x9e3779b9 + (seed << 6u) + (seed >> 2u);
    return seed;
  }
};

//----------------------------------------------------------------------
// class TextWindow
//----------------------------------------------------------------------
class TextWindow final : public fc::FWindow
{
  public:
    // Constructor
    explicit TextWindow (fc::FColor, fc::FColor, fc::FWidget* = nullptr);
    void setPos (const fc::FPoint&, bool = true) override;

  private:
    // Method
    void draw() override;
    void initLayout() override;
    void adjustSize() override;
};

//----------------------------------------------------------------------
TextWindow::TextWindow (fc::FColor fg, fc::FColor bg, fc::FWidget* parent)
  : fc::FWindow{parent}
{
  fc::FWidget::setForegroundColor (fg);
  fc::FWidget::setBackgroundColor (bg);
}

//----------------------------------------------------------------------
void TextWindow::setPos (const fc::FPoint& pos, bool)
{
  fc::FWindow::setPos (pos, false);
  putArea (getTermPos(), getVWin());
  restoreOverlaid{}(*this);
}

//----------------------------------------------------------------------
void TextWindow::draw()
{
  setColor();
  setTransparent();
  clearArea();
  unsetTransparent();
  print() << fc::FPoint(1, 1) << "┌┬┐┬ ┬┌─┐  ┌─┐┌─┐┌─┐┌─┐┌─┐"
          << fc::FPoint(1, 2) << " │ ├─┤├─   └─┐├─┘├─┤│  ├─ "
          << fc::FPoint(1, 3) << " ┴ ┴ ┴└─┘  └─┘┴  ┴ ┴└─┘└─┘"
          << fc::FPoint(6, 5) << "Press 'q' to quit";
}

//----------------------------------------------------------------------
void TextWindow::initLayout()
{
  auto size = fc::FSize{26, 5};
  auto x = int(getDesktopWidth() - size.getWidth()) / 2;
  auto y = int(getDesktopHeight() - size.getHeight()) / 2;
  auto position = fc::FPoint{x, y};
  setGeometry(position, size, false);
  fc::FWidget::initLayout();
}

//----------------------------------------------------------------------
void TextWindow::adjustSize()
{
  fc::FWindow::adjustSize();
  initLayout();
}


//----------------------------------------------------------------------
// class SpaceWindow
//----------------------------------------------------------------------
class SpaceWindow : public fc::FWindow
{
  public:
    // Constructor
    explicit SpaceWindow (fc::FColor, fc::FColor, fc::FWidget* = nullptr);
    void setPos (const fc::FPoint&, bool = true) override;

  protected:
    // Method
    void draw() override;
    void drawNightSky();
    void initLayout() override;
    void adjustSize() override;
};

//----------------------------------------------------------------------
SpaceWindow::SpaceWindow (fc::FColor fg, fc::FColor bg, fc::FWidget* parent)
  : fc::FWindow{parent}
{
  fc::FWidget::setForegroundColor (fg);
  fc::FWidget::setBackgroundColor (bg);
}

//----------------------------------------------------------------------
void SpaceWindow::setPos (const fc::FPoint& pos, bool)
{
  fc::FWindow::setPos (pos, false);
  putArea (getTermPos(), getVWin());
  restoreOverlaid{}(*this);
}

//----------------------------------------------------------------------
void SpaceWindow::draw()
{
  setColor();
  setTransparent();
  clearArea();
  unsetTransparent();
  drawNightSky();
}

//----------------------------------------------------------------------
void SpaceWindow::drawNightSky()
{
  auto width = int(getDesktopWidth());
  auto height = int(getDesktopHeight());
  auto loops = std::size_t(width * height / 50);

  auto gen = std::default_random_engine{std::random_device{}()};
  std::uniform_int_distribution<> distrib_width(1, width);
  std::uniform_int_distribution<> distrib_height(1, height);
  std::unordered_set<fc::FPoint, std::hash<fc::FPoint>> generated{};

  while ( generated.size() < loops )
    generated.emplace(distrib_width(gen), distrib_height(gen));

  for (const auto& point : generated)
    print() << point << '.';
}

//----------------------------------------------------------------------
void SpaceWindow::initLayout()
{
  auto term_size = fc::FSize({getDesktopWidth(), getDesktopHeight()});
  setSize(term_size, false);
  fc::FWidget::initLayout();
}

//----------------------------------------------------------------------
void SpaceWindow::adjustSize()
{
  fc::FWindow::adjustSize();
  initLayout();
}


//----------------------------------------------------------------------
// class PictureSpaceWindow
//----------------------------------------------------------------------
class PictureSpaceWindow final : public SpaceWindow
{
  public:
    // Constructor
    explicit PictureSpaceWindow (fc::FColor, fc::FColor, fc::FWidget* = nullptr);

  private:
    // Method
    void draw() override;
};

//----------------------------------------------------------------------
PictureSpaceWindow::PictureSpaceWindow (fc::FColor fg, fc::FColor bg, fc::FWidget* parent)
  : SpaceWindow{fg, bg, parent}
{ }

//----------------------------------------------------------------------
void PictureSpaceWindow::draw()
{
  SpaceWindow::draw();

  if ( fc::FVTerm::getFOutput()->getMaxColor() < 16 )
    return;

  XpmImage xmp_image{};
  xmp_image.setBackgroundTransparency();
  xmp_image.setBackgroundColor(fc::FColor::Black);
  xmp_image.parseXPM3(saturn_xpm);
  auto pict_size = xmp_image.getSize();
  pict_size.setHeight(pict_size.getHeight() / 2);
  finalcut::FRect planet_geometry(fc::FPoint(0, 0), pict_size);
  std::shared_ptr<FTermArea> planet(createArea (planet_geometry));
  setColor(fc::FColor::Black, fc::FColor::Black);
  clearArea(planet.get());
  planet->setCursorPos (1, 1);
  planet->print(xmp_image.getTermBuffer());
  copyArea (getVWin(), fc::FPoint(10, 10), planet.get());
}


//----------------------------------------------------------------------
// class ParallaxScrolling
//----------------------------------------------------------------------
class ParallaxScrolling final : public fc::FWidget
{
  public:
    // Constructor
    explicit ParallaxScrolling (fc::FWidget* = nullptr);

  private:
    // Method
    void draw() override;
    void initLayout() override;
    void adjustSize() override;
    void scrollLeft (SpaceWindow&, SpaceWindow&) const;

    // Event handlers
    void onTimer (fc::FTimerEvent*) override;
    void onAccel (fc::FAccelEvent*) override;
    void onClose (fc::FCloseEvent*) override;

    // Data members
    TextWindow          text_layer{fc::FColor::Yellow, fc::FColor::Black, this};
    SpaceWindow         layer1_lhs{fc::FColor::DarkGray, fc::FColor::Black, this};
    SpaceWindow         layer1_rhs{fc::FColor::DarkGray, fc::FColor::Black, this};
    SpaceWindow         layer2_lhs{fc::FColor::LightGray, fc::FColor::Black, this};
    SpaceWindow         layer2_rhs{fc::FColor::LightGray, fc::FColor::Black, this};
    SpaceWindow         layer3_lhs{fc::FColor::White, fc::FColor::Black, this};
    PictureSpaceWindow  layer3_rhs{fc::FColor::White, fc::FColor::Black, this};
    int                 timer1{-1};
    int                 timer2{-1};
    int                 timer3{-1};
    bool                quit_app{false};
};

//----------------------------------------------------------------------
ParallaxScrolling::ParallaxScrolling (fc::FWidget* parent)
  : fc::FWidget{parent}
{
  // Scroll speed in characters per second (cps) respectively hertz (Hz)

  timer1 = addTimer (300);  // 300 ms (3.3 cps)
  timer2 = addTimer (150);  // 150 ms (6.6 cps)
  timer3 = addTimer (100);  // 100 ms (10 cps)
  getColorTheme()->term.fg = fc::FColor::LightGray;
  getColorTheme()->term.bg = fc::FColor::Black;
  setForegroundColor(fc::FColor::LightGray);
  setBackgroundColor(fc::FColor::Black);
}

//----------------------------------------------------------------------
void ParallaxScrolling::draw()
{
  setColor();
  clearArea();
}

//----------------------------------------------------------------------
void ParallaxScrolling::initLayout()
{
  auto term_size = fc::FSize({getDesktopWidth(), getDesktopHeight()});
  setGeometry({1, 1}, term_size, false);
  auto lhs_pos = fc::FPoint{1, 1};
  auto rhs_pos = fc::FPoint{int(getDesktopWidth() + 1), 1};
  layer1_lhs.setPos(lhs_pos, false);
  layer2_lhs.setPos(lhs_pos, false);
  layer3_lhs.setPos(lhs_pos, false);
  layer1_rhs.setPos(rhs_pos, false);
  layer2_rhs.setPos(rhs_pos, false);
  layer3_rhs.setPos(rhs_pos, false);
  fc::FWidget::initLayout();
}

//----------------------------------------------------------------------
void ParallaxScrolling::adjustSize()
{
  fc::FWidget::adjustSize();
  initLayout();
}

//----------------------------------------------------------------------
void ParallaxScrolling::scrollLeft (SpaceWindow& lhs, SpaceWindow& rhs) const
{
  auto new_lhs_pos = ( rhs.getPos().getX() == 1 )
                   ? fc::FPoint{int(getDesktopWidth() + 1), 1}
                   : lhs.getPos() - fc::FPoint{1, 0};

  auto new_rhs_pos = ( new_lhs_pos.getX() == 1
                    && new_lhs_pos.getX() > rhs.getPos().getX() )
                   ? fc::FPoint{int(getDesktopWidth() + 1), 1}
                   : rhs.getPos() - fc::FPoint{1, 0};

  lhs.setPos (new_lhs_pos);
  rhs.setPos (new_rhs_pos);
}

//----------------------------------------------------------------------
void ParallaxScrolling::onTimer (fc::FTimerEvent* ev)
{
  if ( ! isShown() )
    return;

  const int timer_id = ev->getTimerId();

  if ( timer1 == timer_id )
  {
    scrollLeft (layer1_lhs, layer1_rhs);
  }
  else if ( timer2 == timer_id )
  {
    scrollLeft (layer2_lhs, layer2_rhs);
  }
  else if ( timer3 == timer_id )
  {
    scrollLeft (layer3_lhs, layer3_rhs);
  }

  forceTerminalUpdate();
}

//----------------------------------------------------------------------
void ParallaxScrolling::onAccel (fc::FAccelEvent* ev)
{
  if ( quit_app )
    return;

  quit_app = true;
  close();
  ev->accept();
  quit_app = false;
}

//----------------------------------------------------------------------
void ParallaxScrolling::onClose (fc::FCloseEvent* ev)
{
  fc::FApplication::closeConfirmationDialog (this, ev);
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
auto main (int argc, char* argv[]) -> int
{
  // Create the application object
  fc::FApplication app{argc, argv};
  fc::FVTerm::setNonBlockingRead();

  // Force terminal initialization without calling show()
  app.initTerminal();
  app.setForegroundColor(fc::FColor::LightGray);
  app.setBackgroundColor(fc::FColor::Black);

  // Create a parallax scrolling object
  ParallaxScrolling parallax{&app};
  parallax.addAccelerator(fc::FKey('q'));

  // Set the parallax object as main widget
  fc::FWidget::setMainWidget(&parallax);

  // Show and start the application
  parallax.show();
  return app.exec();
}

