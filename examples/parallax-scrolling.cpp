/***********************************************************************
* parallax-scrolling.cpp - Parallax scrolling in four layers           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2021 Markus Gans                                           *
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
#include <random>
#include <unordered_set>

#include <final/final.h>

namespace fc = finalcut;


struct restoreOverlaidWindows : public fc::FVTerm
{
  void operator () (fc::FVTerm& obj) const
  {
    if ( ! getWindowList() || getWindowList()->empty() )
      return;
  
    bool overlaid{false};
  
    for (auto&& window : *getWindowList())
    {
      const auto win = static_cast<fc::FWidget*>(window);
  
      if ( overlaid )
        putArea (win->getTermPos(), win->getVWin());
  
      if ( obj.getVWin() == win->getVWin() )
        overlaid = true;
    }
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
  restoreOverlaidWindows()(*this);
}

//----------------------------------------------------------------------
void TextWindow::draw()
{
  setTransparent();
  clearArea();
  unsetTransparent();
  setColor();
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
  auto position = fc::FPoint{x,  y};
  setGeometry(position, size, false);
  fc::FWidget::initLayout();
}

//----------------------------------------------------------------------
void TextWindow::adjustSize()
{
  fc::FWidget::adjustSize();
  initLayout();
}


//----------------------------------------------------------------------
// class SpaceWindow
//----------------------------------------------------------------------
class SpaceWindow final : public fc::FWindow
{
  public:
    // Constructor
    explicit SpaceWindow (fc::FColor, fc::FColor, fc::FWidget* = nullptr);
    void setPos (const fc::FPoint&, bool = true) override;

  private:
    // Method
    void draw() override;
    void drawNightSky();
    void initLayout() override;
    void adjustSize() override;

    struct FPointHash
    {
      std::size_t operator () (const fc::FPoint& p) const noexcept;
    };

    // Data members
    std::random_device rd{"/dev/random"};
    std::mt19937       gen{rd()};
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
  restoreOverlaidWindows()(*this);
}

//----------------------------------------------------------------------
void SpaceWindow::draw()
{
  setTransparent();
  clearArea();
  unsetTransparent();
  setColor();
  drawNightSky();
}

//----------------------------------------------------------------------
void SpaceWindow::drawNightSky()
{
  auto width = int(getDesktopWidth());
  auto height = int(getDesktopHeight());
  auto loops = std::size_t(width * height / 50);

  std::uniform_int_distribution<> distrib_width(1, width);
  std::uniform_int_distribution<> distrib_height(1, height);
  std::unordered_set<fc::FPoint, FPointHash> generated{};

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
  fc::FWidget::adjustSize();
  initLayout();
}

//----------------------------------------------------------------------
std::size_t SpaceWindow::FPointHash::operator () (const fc::FPoint& p) const noexcept
{
  size_t seed = 0;
  const auto hash1 = std::hash<int>()(p.getY());
  const auto hash2 = std::hash<int>()(p.getX());
  seed ^= hash1 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  seed ^= hash2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  return seed;
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
    void scrollLeft (SpaceWindow&, SpaceWindow&);

    // Event handlers
    void onTimer (fc::FTimerEvent*) override;
    void onAccel (fc::FAccelEvent*) override;
    void onClose (fc::FCloseEvent*) override;

    // Data members
    TextWindow   text_layer{fc::FColor::Yellow, fc::FColor::Black, this};
    SpaceWindow  layer3_lhs{fc::FColor::White, fc::FColor::Black, this};
    SpaceWindow  layer3_rhs{fc::FColor::White, fc::FColor::Black, this};
    SpaceWindow  layer2_lhs{fc::FColor::LightGray, fc::FColor::Black, this};
    SpaceWindow  layer2_rhs{fc::FColor::LightGray, fc::FColor::Black, this};
    SpaceWindow  layer1_lhs{fc::FColor::DarkGray, fc::FColor::Black, this};
    SpaceWindow  layer1_rhs{fc::FColor::DarkGray, fc::FColor::Black, this};
    int          timer1{-1};
    int          timer2{-1};
    int          timer3{-1};
};

//----------------------------------------------------------------------
ParallaxScrolling::ParallaxScrolling (fc::FWidget* parent)
  : fc::FWidget{parent}
{
  // Scroll speed in characters per second (cps) respectively hertz (Hz)
  timer1 = addTimer (300);  // 300 ms (3.3 cps)
  timer2 = addTimer (150);  // 150 ms (6.6 cps)
  timer3 = addTimer (100);  // 100 ms (10 cps)
  getColorTheme()->term_fg = fc::FColor::LightGray;
  getColorTheme()->term_bg = fc::FColor::Black;
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
void ParallaxScrolling::scrollLeft (SpaceWindow& lhs, SpaceWindow& rhs)
{
  if ( rhs.getPos().getX() == 1 )
    lhs.setPos ({int(getDesktopWidth() + 1), 1});

  if ( lhs.getPos().getX() == 1
    && lhs.getPos().getX() > rhs.getPos().getX() )
    rhs.setPos ({int(getDesktopWidth() + 1), 1});

  lhs.setPos (lhs.getPos() - fc::FPoint(1,0));
  rhs.setPos (rhs.getPos() - fc::FPoint(1,0));
}

//----------------------------------------------------------------------
void ParallaxScrolling::onTimer (fc::FTimerEvent* ev)
{
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
}

//----------------------------------------------------------------------
void ParallaxScrolling::onAccel (fc::FAccelEvent* ev)
{
  close();
  ev->accept();
}

//----------------------------------------------------------------------
void ParallaxScrolling::onClose (fc::FCloseEvent* ev)
{
  fc::FApplication::closeConfirmationDialog (this, ev);
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
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

