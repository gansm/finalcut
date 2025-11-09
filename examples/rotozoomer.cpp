/***********************************************************************
* rotozoomer.cpp - Rotozoomer effect demo                              *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2020-2025 Markus Gans                                      *
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

#if defined(__CYGWIN__)
  #define _XOPEN_SOURCE 700
#endif

#include <cmath>

#include <array>
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

namespace {

// Constants
static constexpr int MAX_LOOPS{314};

// Non-member functions
constexpr auto generateSinTable() noexcept -> std::array<float, MAX_LOOPS>
{
  std::array<float, MAX_LOOPS> table{};

  for (int i{0}; i < MAX_LOOPS; ++i)
    table[i] = std::sin(float(i) / 50.0f);

  return table;
}

//----------------------------------------------------------------------
constexpr auto generateDataPattern() noexcept -> std::array<wchar_t, 256>
{
  std::array<wchar_t, 256> arr{};
  const std::array<wchar_t, 4> init_val{{L' ', L'+', L'x', L' '}};
  std::size_t h{0};

  for (int i{0}; i < 2; ++i)
  {
    for (int j{0}; j < 8; ++j)
    {
      for (int k{0}; k < 8; ++k)
      {
        arr[h] = init_val[unsigned(2 * i)];
        ++h;
      }

      for (int k{0}; k < 8; ++k)
      {
        arr[h] = init_val[unsigned((2 * i) + 1)];
        ++h;
      }
    }
  }

  return arr;
}

} // namespace


//----------------------------------------------------------------------
// class RotoZoomer
//----------------------------------------------------------------------

class RotoZoomer final : public finalcut::FDialog
{
  public:
    // Constructor
    explicit RotoZoomer (finalcut::FWidget* = nullptr, bool = false, int = MAX_LOOPS);

    // Accessors
    auto getReport() const -> finalcut::FString;

    // Event handlers
    void onShow (finalcut::FShowEvent*) override;
    void onTimer (finalcut::FTimerEvent*) override;
    void onKeyPress (finalcut::FKeyEvent*) override;
    void onClose (finalcut::FCloseEvent*) override;

  private:
    static constexpr auto color_space = finalcut::FColorPair{ FColor::Black, FColor::White };
    static constexpr auto color_plus  = finalcut::FColorPair{ FColor::Black, FColor::Red   };
    static constexpr auto color_x     = finalcut::FColorPair{ FColor::Black, FColor::Cyan  };
    static constexpr auto PHI_2       = 2.02358f;
    static constexpr auto PHI_3       = -1.11701f;
    static constexpr auto PHI_PI2     = float(M_PI_2);  // π/2
    static constexpr auto idx_2       = int(PHI_2 * 50.0f) % MAX_LOOPS;
    static constexpr auto idx_3       = int(PHI_3 * 50.0f) % MAX_LOOPS;
    static constexpr auto idx_cos     = int(PHI_PI2 * 50.0f) % MAX_LOOPS;
    static constexpr auto idx_2_cos   = int((PHI_2 + PHI_PI2) * 50.0f) % MAX_LOOPS;
    static constexpr auto idx_3_cos   = int((PHI_3 + PHI_PI2) * 50.0f) % MAX_LOOPS;

    // Methods
    auto getSine (int, int) const noexcept -> float;
    void draw() override;
    void rotozoomer (float, float, float);
    void generateReport();
    void adjustSize() override;

    // Data member
    bool  benchmark{false};
    int  loops{0};
    int  path{0};
    finalcut::FString  report{};
    time_point<system_clock>  start{};
    time_point<system_clock>  end{};
    static const std::array<float, MAX_LOOPS>  sin_table;
    static const std::array<wchar_t, 256>  data;
};

// static class attributes
constexpr finalcut::FColorPair RotoZoomer::color_space;
constexpr finalcut::FColorPair RotoZoomer::color_plus;
constexpr finalcut::FColorPair RotoZoomer::color_x;
const std::array<float, MAX_LOOPS> RotoZoomer::sin_table = generateSinTable();
const std::array<wchar_t, 256> RotoZoomer::data = generateDataPattern();

//----------------------------------------------------------------------
RotoZoomer::RotoZoomer (finalcut::FWidget* parent, bool is_benchmark, int num_loops)
  : finalcut::FDialog{parent}
  , benchmark{is_benchmark}
  , loops{num_loops}
{
  FDialog::setText ("Rotozoomer effect");
}

//----------------------------------------------------------------------
inline auto RotoZoomer::getSine (int i, int shift) const noexcept -> float
{
  // Sine lookup
  return sin_table[(i + shift + MAX_LOOPS) % MAX_LOOPS];
}

//----------------------------------------------------------------------
void RotoZoomer::draw()
{
  if ( benchmark && start == time_point<system_clock>() )
    start = system_clock::now();

  finalcut::FDialog::draw();
  const auto i1 = (path * 5) % MAX_LOOPS;
  const auto r  = 128.0f + (96.0f * getSine(i1, idx_cos));
  const auto i2 = path % MAX_LOOPS;
  const auto cx = 40.0f + (40.0f * getSine(i2, 0));
  const auto cy = 23.0f + (23.0f * getSine(i2, idx_cos));
  rotozoomer (cx, cy, r);
}

//----------------------------------------------------------------------
inline void RotoZoomer::rotozoomer (float cx, float cy, float r)
{
  const auto& cols = int(getClientWidth());
  const auto& lines = int(getClientHeight());

  if ( cols <= 0 || lines <= 0 )
    return;

  constexpr auto SCALE = int{4096};
  const auto i   = int(path % MAX_LOOPS);
  const auto Ax0 = int(SCALE * (cx + r * getSine(i, idx_cos)));
  const auto Ay0 = int(SCALE * (cy + r * getSine(i, 0)));
  const auto Bx0 = int(SCALE * (cx + r * getSine(i, idx_2_cos)));
  const auto By0 = int(SCALE * (cy + r * getSine(i, idx_2)));
  const auto Cx0 = int(SCALE * (cx + r * getSine(i, idx_3_cos)));
  const auto Cy0 = int(SCALE * (cy + r * getSine(i, idx_3)));
  int dxdx = (Bx0 - Ax0) / cols;
  int dydx = (By0 - Ay0) / cols;
  int dxdy = (Cx0 - Ax0) / lines;
  int dydy = (Cy0 - Ay0) / lines;

  int Ax = Ax0;
  int Ay = Ay0;
  auto& out = print();

  for (auto y{0}; y < lines; ++y)
  {
    int Cx = Ax;
    int Cy = Ay;
    out << FPoint{2, 3 + y};

    for (auto x{0}; x < cols; ++x)
    {
      const unsigned index = unsigned((Cy >> 14) & 0xF)
                           + unsigned((Cx >> 10) & 0xF0);
      const auto ch = data[index];

      if ( ch == L'+' )
        out << color_plus;
      else if ( ch == L'x' )
        out << color_x;
      else
        out << color_space;

      out << ch;
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
  const auto elapsed_ms = int(duration_cast<milliseconds>(end - start).count());
  time_str << float(elapsed_ms) / 1000 << "s";
  fps_str << float(loops) * 1000.0 / float(elapsed_ms);

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
inline auto RotoZoomer::getReport() const -> finalcut::FString
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
  if ( path >= MAX_LOOPS )  // More than 360 degrees
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
auto main (int argc, char* argv[]) -> int
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
    RotoZoomer roto{&app, benchmark, MAX_LOOPS};

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
