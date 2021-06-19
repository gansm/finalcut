/***********************************************************************
* rotozoomer.cpp - Rotozoomer effect demo                              *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2020-2021 Markus Gans                                      *
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

#include <chrono>
#include <iomanip>
#include <string>

#include <final/final.h>

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::system_clock;
using std::chrono::time_point;
using finalcut::FPoint;
using finalcut::FSize;
using finalcut::FColor;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This rotozoomer demo is based on the code of Robert Dörfler
// http://bloerp.de/code/tempel/roto.c
// http://bloerp.de/code/tempel/tempel-des-codes.html
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//----------------------------------------------------------------------
// class RotoZoomer
//----------------------------------------------------------------------

class RotoZoomer final : public finalcut::FDialog
{
  public:
    // Constructor
    explicit RotoZoomer (finalcut::FWidget* = nullptr, bool = false, int = 314);

    // Destructor
    ~RotoZoomer() override = default;

    // Accessors
    finalcut::FString getReport() const;

    // Event handlers
    void onShow (finalcut::FShowEvent*) override;
    void onTimer (finalcut::FTimerEvent*) override;
    void onKeyPress (finalcut::FKeyEvent*) override;
    void onClose (finalcut::FCloseEvent*) override;

  private:
    // Methods
    void draw() override;
    void rotozoomer (double, double, double, double);
    void generateReport();
    void adjustSize() override;

    // Data member
    bool                     benchmark{false};
    int                      loops{0};
    int                      path{0};
    std::wstring             data{std::wstring(256, L'\0')};
    finalcut::FString        report{};
    time_point<system_clock> start{};
    time_point<system_clock> end{};
};


//----------------------------------------------------------------------
RotoZoomer::RotoZoomer (finalcut::FWidget* parent, bool b, int l)
  : finalcut::FDialog{parent}
  , benchmark{b}
  , loops{l}
{
  FDialog::setText ("Rotozoomer effect");

  std::size_t h{0};

  for (std::size_t j{0}; j < 8; j++)
  {
    for (std::size_t i{0}; i < 8; i++)
    {
      data[h] = L' ';
      h++;
    }

    for (std::size_t i{0}; i < 8; i++)
    {
      data[h] = L'+';
      h++;
    }
  }

  for (std::size_t j{0}; j < 8; j++)
  {
    for (std::size_t i{0}; i < 8; i++)
    {
      data[h] = L'x';
      h++;
    }

    for (std::size_t i{0}; i < 8; i++)
    {
      data[h] = L' ';
      h++;
    }
  }
}

//----------------------------------------------------------------------
void RotoZoomer::draw()
{
  if ( benchmark && start == time_point<system_clock>() )
    start = system_clock::now();

  finalcut::FDialog::draw();
  auto cx = double(80.0 / 2.0 + (80.0 / 2.0 * std::sin(double(path) / 50.0)));
  auto cy = double(23.0 + (23.0 * std::cos(double(path) / 50.0)));
  auto r  = double(128.0 + 96.0 * std::cos(double(path) / 10.0));
  auto a  = double(path) / 50.0;
  rotozoomer (cx, cy, r, a);
}

//----------------------------------------------------------------------
void RotoZoomer::rotozoomer (double cx, double cy, double r, double a)
{
  const auto Cols = int(getClientWidth());
  const auto Lines = int(getClientHeight());
  auto Ax   = int(4096.0 * (cx + r * std::cos(a)));
  auto Ay   = int(4096.0 * (cy + r * std::sin(a)));
  auto Bx   = int(4096.0 * (cx + r * std::cos(a + 2.02358)));
  auto By   = int(4096.0 * (cy + r * std::sin(a + 2.02358)));
  auto Cx   = int(4096.0 * (cx + r * std::cos(a - 1.11701)));
  auto Cy   = int(4096.0 * (cy + r * std::sin(a - 1.11701)));
  int  dxdx = (Bx - Ax) / 80;
  int  dydx = (By - Ay) / 80;
  int  dxdy = (Cx - Ax) / 23;
  int  dydy = (Cy - Ay) / 23;

  for (auto y = 0; y < Lines; y++)
  {
    Cx = Ax;
    Cy = Ay;
    print() << FPoint{2, 3 + y};

    for (auto x = 0; x < Cols; x++)
    {
      auto ch = data[((Cy >> 14) & 0xf) + ((Cx >> 10) & 0xf0)];

      if ( ch == '+' )
        print() << finalcut::FColorPair{FColor::Black, FColor::Red};
      else if ( ch == 'x' )
        print() << finalcut::FColorPair{FColor::Black, FColor::Cyan};
      else
        print() << finalcut::FColorPair{FColor::Black, FColor::White};

      print() << ch;
      Cx += dxdx;
      Cy += dydx;
    }

    Ax += dxdy;
    Ay += dydy;
  }
}

//----------------------------------------------------------------------
void RotoZoomer::generateReport()
{
  finalcut::FString term_type = finalcut::FTerm::getTermType();
  finalcut::FString dimension_str{};
  finalcut::FString time_str{};
  finalcut::FString fps_str{};
  finalcut::FStringStream rep;
  dimension_str << getDesktopWidth()
                << "x" << getDesktopHeight();
  auto elapsed_ms = int(duration_cast<milliseconds>(end - start).count());
  time_str << double(elapsed_ms) / 1000 << "s";
  fps_str << double(loops) * 1000.0 / double(elapsed_ms);

  rep << finalcut::FString{55, '-'} << "\n"
      << "Terminal            Size    Time      Loops  Frame rate\n"
      << finalcut::FString{55, '-'} << "\n"
      << std::left << std::setw(20) << term_type
      << std::setw(8) << dimension_str
      << std::setw(10) << time_str
      << std::setw(7) << loops
      << std::setw(7) << fps_str.left(7) << "fps\n";
  report << rep.str();
}

//----------------------------------------------------------------------
inline finalcut::FString RotoZoomer::getReport() const
{
  return report;
}

//----------------------------------------------------------------------
void RotoZoomer::onShow (finalcut::FShowEvent*)
{
  if ( ! benchmark )
    addTimer(33);  // Starts the timer every 33 milliseconds
  else
  {
    for (path = 1; path < loops; path++)
    {
      redraw();
      forceTerminalUpdate();
    }

    end = system_clock::now();
    generateReport();
    flush();
    close();
  }
}

//----------------------------------------------------------------------
void RotoZoomer::onTimer (finalcut::FTimerEvent*)
{
  if ( path >= 314 )  // More than 360 degrees
    path = 0;
  else
    path++;

  redraw();
  forceTerminalUpdate();
}

//----------------------------------------------------------------------
void RotoZoomer::onKeyPress (finalcut::FKeyEvent* ev)
{
  if ( ! ev )
    return;

  if ( ev->key() == finalcut::FKey('q') )
  {
    close();
    ev->accept();
  }
  else
    finalcut::FDialog::onKeyPress(ev);
}

//----------------------------------------------------------------------
void RotoZoomer::onClose (finalcut::FCloseEvent* ev)
{
  if ( benchmark )
    ev->accept();
  else
    finalcut::FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void RotoZoomer::adjustSize()
{
  if ( ! benchmark )
  {
    std::size_t h = getDesktopHeight();
    std::size_t w = getDesktopWidth();

    if ( h > 1 )
      h--;

    if ( w > 8 )
      w -= 8;

    setGeometry(FPoint{5, 1}, FSize{w, h}, false);
  }

  finalcut::FDialog::adjustSize();
}

//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
{
  bool benchmark{false};
  finalcut::FString report{};
  int quit_code{0};

  if ( argv[1] && ( strcmp(argv[1], "--help") == 0
                 || strcmp(argv[1], "-h") == 0 ) )
  {
    std::cout << "RotoZoomer options:\n"
              << "  -b, --benchmark               "
              << "Starting a benchmark run\n\n";
  }
  else if ( argv[1] && ( strcmp(argv[1], "--benchmark") == 0
                      || strcmp(argv[1], "-b") == 0 ) )
  {
    benchmark = true;
    // Disable terminal data requests
    auto& start_options = finalcut::FStartOptions::getInstance();
    start_options.terminal_data_request = false;
  }

  {  // Create the application object in this scope
    finalcut::FApplication app{argc, argv};
    finalcut::FVTerm::setNonBlockingRead();

    // Create a simple dialog box
    constexpr int iterations = 314;
    RotoZoomer roto{&app, benchmark, iterations};

    if ( benchmark )
      roto.setGeometry (FPoint{1, 1}, FSize{80, 24});

    roto.setShadow();  // Instead of the transparent window shadow

    // Set the RotoZoomer object as main widget
    finalcut::FWidget::setMainWidget(&roto);

    // Show and start the application
    roto.show();
    quit_code = app.exec();

    if ( benchmark )
      report = roto.getReport();
  }  // Hide and destroy the application object

  if ( benchmark )
  {
    std::cout << "Benchmark:\n" << report;
  }

  return quit_code;
}
