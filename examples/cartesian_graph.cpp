/***********************************************************************
* cartesian_graph.cpp - A cartesian coordinate system example          *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2024 Yusuf Bülbül                                          *
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

#include <final/final.h>

using finalcut::FColor;
using finalcut::FColorPair;
using finalcut::FPoint;
using finalcut::FSize;
using finalcut::FVTerm;
using finalcut::UniChar;

template <typename T>
struct point
{
  T x{};
  T y{};
};

//----------------------------------------------------------------------
// class GraphWindow
//----------------------------------------------------------------------

class GraphWindow final : public finalcut::FDialog
{
  public:
    // Constructor
    explicit GraphWindow ( finalcut::FWidget* = nullptr
                         , point<double> = {-10.0, -10.0}
                         , point<double> = { 10.0,  10.0} );

    // Disable copy constructor
    GraphWindow (const GraphWindow&) = delete;

    // Disable move constructor
    GraphWindow (GraphWindow&&) noexcept = delete;

    // Destructor
    ~GraphWindow() noexcept override = default;

    // Disable copy assignment operator (=)
    auto operator = (const GraphWindow&) -> GraphWindow& = delete;

    // Disable move assignment operator (=)
    auto operator = (GraphWindow&&) noexcept -> GraphWindow& = delete;

    // Methods
    void addPoint (point<double>);
    void addRealtimePoint (double, double = 1);
    void drawLine (point<double>, point<double>, FColor);
    void drawLine (double, double, FColor);

  private:
    // Methods
    void draw() override;
    void drawLineImpl (const FPoint&, const FPoint&, FColor);
    FPoint transformToScreen (point<double>) const;

    // Event handlers
    void onTimer (finalcut::FTimerEvent*) override;
    void onClose (finalcut::FCloseEvent*) override;

    // Data members
    std::deque<point<double>> points{};
    std::size_t max_points{40};
    point<double> min{};
    point<double> max{};
};

//----------------------------------------------------------------------
GraphWindow::GraphWindow ( finalcut::FWidget* parent
                         , point<double> min_value
                         , point<double> max_value )
  : FDialog(parent)
  , min(min_value)
  , max(max_value)
{
  addTimer(200);
}

//----------------------------------------------------------------------
void GraphWindow::addPoint (point<double> pos)
{
  // Add a new point to the graph

  if ( points.size() >= max_points )
    points.pop_front();

  points.emplace_back(pos);
  redraw();
}

//----------------------------------------------------------------------
void GraphWindow::addRealtimePoint (double val, double interval)
{
  static double x = min.x;
  addPoint({x, val});
  x += interval;

  if ( x > max.x )
    x = min.x;

  redraw();
  flush();
}

//----------------------------------------------------------------------
FPoint GraphWindow::transformToScreen (point<double> p) const
{
  const auto client_width = int(getClientWidth());
  const auto client_height = int(getClientHeight());
  const auto screen_x = static_cast<int>( (p.x - min.x) /
      (max.x - min.x) * (client_width - 1) );
  const auto screen_y = static_cast<int>( client_height - 1 -
      ((p.y - min.y) / (max.y - min.y)) * (client_height - 1) );
  return { 1 + getLeftPadding() + screen_x
         , 1 + getTopPadding() + screen_y };
}

//----------------------------------------------------------------------
void GraphWindow::onTimer (finalcut::FTimerEvent*)
{
  static double x = min.x;
  // Adjust this for the smoothness of the sine wave
  constexpr double interval = 10;
  // Frequency of the sine wave (number of cycles)
  constexpr double frequency = 1.0;
  // Adjust amplitude (height of the wave)
  const double amplitude = (max.y - min.y) / 4.0;
  const double y = amplitude *
      std::sin(2 * M_PI * frequency * (x - min.x) / (max.x - min.x));
  addRealtimePoint(y, interval);

  // Increment x for the next point
  x += interval;

  if ( x > max.x )
    x = min.x;
}

//----------------------------------------------------------------------
void GraphWindow::draw()
{
  finalcut::FDialog::draw();
  setColor(FColor::Black, FColor::White);
  const auto client_width = int(getClientWidth());
  const auto client_height = int(getClientHeight());
  const auto zero = transformToScreen({0.0, 0.0});
  const auto x_step = static_cast<int>((max.x - min.x) / 10);
  const auto y_step = static_cast<int>((max.y - min.y) / 10);
  const auto border_top = getTopPadding();
  const auto border_left = getLeftPadding();
  const auto border_right = 1 + border_left + client_width;
  const auto border_bottom = 1 + getTopPadding() + client_height;

  for (int x{0}; x <= client_width; ++x)
    print() << FPoint{1 + border_left + x, zero.getY()}
            << UniChar::BoxDrawingsHorizontal;  // ─

  for (int y{0}; y < client_height; ++y)
    print() << FPoint{zero.getX(), 1 + border_top + y}
            << UniChar::BoxDrawingsVertical;  // │

  for (auto x{int(min.x)}; x <= int(max.x); x += x_step)
  {
    const auto screen_point = transformToScreen({double(x), 0.0});
    const auto x_as_string = std::to_string(x);
    const FPoint number_pos
    (
      std::min(screen_point.getX(), border_right - int(x_as_string.length())),
      screen_point.getY() + 1
    );
    print() << screen_point
            << UniChar::BoxDrawingsCross  // ┼
            << number_pos
            << x_as_string;
  }

  for (auto y{int(min.y)}; y <= int(max.y); y += y_step)
  {
    const auto screen_point = transformToScreen({0.0, double(y)});
    const auto y_as_string = std::to_string(y);
    const FPoint number_pos
    (
      zero.getX() - 1 - int(y_as_string.length()),
      screen_point.getY()
    );
    print() << screen_point
            << UniChar::BoxDrawingsCross  // ┼
            << number_pos
            << y_as_string;
  }

  for (const auto& p : points)
  {
    print() << transformToScreen(p)
            << FColorPair(FColor::Red, FColor::White)
            << UniChar::BlackCircle
            << FColorPair(FColor::Black, FColor::White);
  }

  print() << FPoint(zero.getX(), border_top)
          << UniChar::BoxDrawingsDownAndHorizontal  // ┬
          << FPoint(border_right, zero.getY())
          << UniChar::BoxDrawingsVerticalAndLeft    // ┤
          << FPoint(zero.getX(), border_bottom)
          << UniChar::BoxDrawingsUpAndHorizontal    // ┴
          << FPoint(getLeftPadding(), zero.getY())
          << UniChar::BoxDrawingsVerticalAndRight;  // ├
}

//----------------------------------------------------------------------
void GraphWindow::drawLine (point<double> p1, point<double> p2, FColor color)
{
  auto start = transformToScreen(p1);
  auto end = transformToScreen(p2);
  drawLineImpl (start, end, color);
}

//----------------------------------------------------------------------
void GraphWindow::drawLine (double angle, double magnitude, FColor color)
{
  const double radians = angle * (M_PI / 180.0);
  const point<double> end_pos = { magnitude * std::cos(radians)
                                , magnitude * std::sin(radians) };
  const auto start = transformToScreen({0.0, 0.0});
  const auto end = transformToScreen(end_pos);
  drawLineImpl (start, end, color);
}

//----------------------------------------------------------------------
inline void GraphWindow::drawLineImpl ( const FPoint& start
                                      , const FPoint& end
                                      , FColor color )
{
  const int dx = std::abs(end.getX() - start.getX());
  const int dy = std::abs(end.getY() - start.getY());
  const int sx = start.getX() < end.getX() ? 1 : -1;
  const int sy = start.getY() < end.getY() ? 1 : -1;
  int err = dx - dy;
  int x = start.getX();
  int y = start.getY();
  setForegroundColor(color);

  while ( true )
  {
    print() << FPoint{x, y} << UniChar::BlackCircle;

    if ( x == end.getX() && y == end.getY() )
      break;

    const int e2 = 2 * err;

    if ( e2 > -dy )
    {
      err -= dy;
      x += sx;
    }

    if ( e2 < dx )
    {
      err += dx;
      y += sy;
    }
  }
}

//----------------------------------------------------------------------
void GraphWindow::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
auto main (int argc, char* argv[]) -> int
{
  finalcut::FApplication app{argc, argv};
  GraphWindow graph{&app, {-200, -200}, {200, 200}};
  graph.setText ("Sinusoidal wave");
  const auto graph_size = FSize { FVTerm::getFOutput()->getColumnNumber() - 1
                                , FVTerm::getFOutput()->getLineNumber() - 1 };
  graph.setGeometry (FPoint{1, 1}, graph_size);
  graph.setMinimumSize (FSize{30, 15});
  graph.setResizeable();
  graph.setMinimizable();
  finalcut::FWidget::setMainWidget (&graph);
  graph.show();
  return app.exec();
}
