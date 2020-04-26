/***********************************************************************
* rotozoomer.cpp - Rotozoomer effect demo                              *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2020 Markus Gans                                           *
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

#include <chrono>
#include <iomanip>

#include <cmath>

#include <final/final.h>

namespace fc = finalcut::fc;
using namespace std::chrono;
using finalcut::FPoint;
using finalcut::FSize;

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
    ~RotoZoomer() override;

    // Accessors
    finalcut::FString getReport();

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
    wchar_t                  data[256]{};
    finalcut::FString        report{};
    time_point<system_clock> start{};
    time_point<system_clock> end{};
};


//----------------------------------------------------------------------
RotoZoomer::RotoZoomer (finalcut::FWidget* parent, bool b, int l)
  : finalcut::FDialog(parent)
  , benchmark(b)
  , loops(l)
{
  FDialog::setText ("Rotozoomer effect");

  int h{0};

  for (int j{0}; j < 8; j++)
  {
    for (int i{0}; i < 8; i++)
    {
      data[h++] = L' ';
    }

    for (int i{0}; i < 8; i++)
    {
      data[h++] = L'+';
    }
  }

  for (int j{0}; j < 8; j++)
  {
    for (int i{0}; i < 8; i++)
    {
      data[h++] = L'x';
    }

    for (int i{0}; i < 8; i++)
    {
      data[h++] = L' ';
    }
  }
}

//----------------------------------------------------------------------
RotoZoomer::~RotoZoomer()
{ }

//----------------------------------------------------------------------
void RotoZoomer::draw()
{
  if ( benchmark && start == time_point<system_clock>() )
    start = system_clock::now();

  finalcut::FDialog::draw();
  double cx = double(80.0 / 2.0 + (80.0 / 2.0 * std::sin(double(path) / 50.0)));
  double cy = double(23.0 + (23.0 * std::cos(double(path) / 50.0)));
  double r  = double(128.0 + 96.0 * std::cos(double(path) / 10.0));
  double a  = double(path) / 50.0;
  rotozoomer (cx, cy, r, a);
}

//----------------------------------------------------------------------
void RotoZoomer::rotozoomer (double cx, double cy, double r, double a)
{
  const int Cols = int(getClientWidth());
  const int Lines = int(getClientHeight());
  int  Ax   = int(4096.0 * (cx + r * std::cos(a)));
  int  Ay   = int(4096.0 * (cy + r * std::sin(a)));
  int  Bx   = int(4096.0 * (cx + r * std::cos(a + 2.02358)));
  int  By   = int(4096.0 * (cy + r * std::sin(a + 2.02358)));
  int  Cx   = int(4096.0 * (cx + r * std::cos(a - 1.11701)));
  int  Cy   = int(4096.0 * (cy + r * std::sin(a - 1.11701)));
  int  dxdx = (Bx - Ax) / 80;
  int  dydx = (By - Ay) / 80;
  int  dxdy = (Cx - Ax) / 23;
  int  dydy = (Cy - Ay) / 23;

  for (int y = 0; y < Lines; y++)
  {
    Cx = Ax;
    Cy = Ay;
    print() << FPoint(2, 3 + y);

    for (int x = 0; x < Cols; x++)
    {
      wchar_t ch = data[((Cy >> 14) & 0xf) + ((Cx >> 10) & 0xf0)];

      if ( ch == '+' )
        print() << finalcut::FColorPair(fc::Black, fc::Red);
      else if ( ch == 'x' )
        print() << finalcut::FColorPair(fc::Black, fc::Cyan);
      else
        print() << finalcut::FColorPair(fc::Black, fc::White);

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
  finalcut::FString term_type = getTermType();
  finalcut::FString dimension_str{};
  finalcut::FString time_str{};
  finalcut::FString fps_str{};
  std::wostringstream rep;
  dimension_str << getDesktopWidth()
                << "x" << getDesktopHeight();
  int elapsed_ms = int(duration_cast<milliseconds>(end - start).count());
  time_str << double(elapsed_ms) / 1000 << "ms";
  fps_str << double(loops) * 1000.0 / double(elapsed_ms);

  rep << finalcut::FString(55, '-') << "\n"
      << "Terminal            Size    Time      Loops  Frame rate\n"
      << finalcut::FString(55, '-') << "\n"
      << std::left << std::setw(20) << term_type
      << std::setw(8) << dimension_str
      << std::setw(10) << time_str
      << std::setw(7) << loops
      << std::setw(7) << fps_str.left(7) << "fps\n";
  report << rep.str();
}

//----------------------------------------------------------------------
inline finalcut::FString RotoZoomer::getReport()
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
      updateTerminal();
    }

    end = system_clock::now();
    generateReport();
    flush();
    quit();
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
  updateTerminal();
  flush();
}

//----------------------------------------------------------------------
void RotoZoomer::onKeyPress (finalcut::FKeyEvent* ev)
{
  if ( ! ev )
    return;

  if ( ev->key() == 'q' )
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
  if ( ! benchmark )
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

    setGeometry(FPoint(5, 1), FSize(w, h), false);
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
  }

  {  // Create the application object in this scope
    finalcut::FApplication app(argc, argv);
    app.setNonBlockingRead();

    // Create a simple dialog box
    constexpr int iterations = 314;
    RotoZoomer roto(&app, benchmark, iterations);

    if ( benchmark )
      roto.setGeometry (FPoint(1, 1), FSize(80, 24));
    else
      roto.setGeometry (FPoint(5, 1), FSize(72, 23));

    roto.setShadow();

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
