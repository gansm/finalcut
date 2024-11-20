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
  #define _USE_MATH_DEFINES
#endif

#include <cmath>

#include <final/final.h>

using finalcut::FPoint;
using finalcut::FSize;
using finalcut::FColor;
using finalcut::UniChar;
using finalcut::FVTerm;


//----------------------------------------------------------------------
// class GraphWindow
//----------------------------------------------------------------------

class GraphWindow final : public finalcut::FDialog
{
  public:
    // Constructor
    explicit GraphWindow (finalcut::FWidget* = nullptr, double = -10, double = 10, double = -10, double = 10);
    GraphWindow (const GraphWindow&) = delete;
    GraphWindow (GraphWindow&&) noexcept = delete;
    ~GraphWindow() noexcept override = default;
    auto operator = (const GraphWindow&) -> GraphWindow& = delete;
    auto operator = (GraphWindow&&) noexcept -> GraphWindow& = delete;
    void add_point(double, double);  // Add a new point to the graph
    void add_realtime_point (double, double = 1);
    void draw_line (double, double, double, double, FColor);
    void draw_line (double, double magnitude, FColor);

  private:
    void draw() override;
    FPoint transform_to_screen (double, double) const;

    // Event handlers
    void onTimer (finalcut::FTimerEvent*) override;
    void onClose (finalcut::FCloseEvent*) override;

    // Data members
    std::deque<std::pair<double, double>> _points{};
    std::size_t _maxPoints{40};
    double _xMin{};
    double _xMax{};
    double _yMin{};
    double _yMax{};
};

//----------------------------------------------------------------------
GraphWindow::GraphWindow(finalcut::FWidget* parent, double xMin, double xMax, double yMin, double yMax)
  : FDialog(parent)
  , _xMin(xMin)
  , _xMax(xMax)
  , _yMin(yMin)
  , _yMax(yMax)
{
  addTimer(200);
}

//----------------------------------------------------------------------
void GraphWindow::add_point (double x, double y)
{
  if ( _points.size() >= _maxPoints )
    _points.pop_front();

  _points.emplace_back(x, y);
  redraw();
}

//----------------------------------------------------------------------
void GraphWindow::add_realtime_point (double val, double interval)
{
  static double x = _xMin;
  add_point(x, val);
  x += interval;

  if ( x > _xMax )
    x = _xMin;

  redraw();
  flush();
}

//----------------------------------------------------------------------
FPoint GraphWindow::transform_to_screen (double x, double y) const
{
  auto client_width = int(getClientWidth());
  auto client_height = int(getClientHeight());
  auto screenX = static_cast<int>(((x - _xMin) / (_xMax - _xMin)) * (client_width - 1));
  auto screenY = static_cast<int>(client_height - 1 - ((y - _yMin) / (_yMax - _yMin)) * (client_height - 1));
  return { 1 + getLeftPadding() + screenX
         , 1 + getTopPadding() + screenY };
}

//----------------------------------------------------------------------
void GraphWindow::onTimer (finalcut::FTimerEvent*)
{
  static double x = _xMin;
  static const double interval = 10;  // Adjust this for the smoothness of the sine wave
  static const double amplitude = (_yMax - _yMin) / 4.0;  // Adjust amplitude (height of the wave)
  static const double frequency = 1.0;  // Frequency of the sine wave (number of cycles)
  double y = amplitude * std::sin(2 * M_PI * frequency * (x - _xMin) / (_xMax - _xMin));
  add_realtime_point(y, interval);

  // Increment x for the next point
  x += interval;

  if ( x > _xMax )
    x = _xMin;
}

//----------------------------------------------------------------------
void GraphWindow::draw()
{
  finalcut::FDialog::draw();
  setColor(FColor::Black, FColor::White);
  auto client_width = int(getClientWidth());
  auto client_height = int(getClientHeight());
  auto zeroY = transform_to_screen(0.0, 0.0).getY();
  auto zeroX = transform_to_screen(0.0, 0.0).getX();
  auto xStep = static_cast<int>((_xMax - _xMin) / 10);
  auto yStep = static_cast<int>((_yMax - _yMin) / 10);
  auto border_top = getTopPadding();
  auto border_left = getLeftPadding();
  auto border_right = 1 + border_left + client_width;
  auto border_bottom = 1 + getTopPadding() + client_height;

  for (int x{0}; x <= client_width; ++x)
    print() << FPoint{1 + border_left + x, zeroY}
            << UniChar::BoxDrawingsHorizontal;  // ─

  for (int y{0}; y < client_height; ++y)
    print() << FPoint{zeroX, 1 + border_top + y}
            << UniChar::BoxDrawingsVertical;  // │

  for (auto x{int(_xMin)}; x <= int(_xMax); x += xStep)
  {
    auto screenPoint = transform_to_screen(double(x), 0.0);
    auto x_as_string = std::to_string(x);
    FPoint number_pos( std::min(screenPoint.getX(), border_right - int(x_as_string.length()))
                     , screenPoint.getY() + 1 );
    print() << screenPoint
            << UniChar::BoxDrawingsCross  // ┼
            << number_pos
            << x_as_string;
  }

  for (auto y{int(_yMin)}; y <= int(_yMax); y += yStep)
  {
    auto screenPoint = transform_to_screen(0.0, double(y));
    auto y_as_string = std::to_string(y);
    FPoint number_pos(zeroX - 1 - int(y_as_string.length()), screenPoint.getY());
    print() << screenPoint
            << UniChar::BoxDrawingsCross  // ┼
            << number_pos
            << y_as_string;
  }

  for (const auto& point : _points)
  {
    auto x = point.first;
    auto y = point.second;
    print() << transform_to_screen(x, y)
            << UniChar::BlackCircle;
  }

  print() << FPoint(zeroX, border_top)
          << UniChar::BoxDrawingsDownAndHorizontal  // ┬
          << FPoint(border_right, zeroY)
          << UniChar::BoxDrawingsVerticalAndLeft    // ┤
          << FPoint(zeroX, border_bottom)
          << UniChar::BoxDrawingsUpAndHorizontal    // ┴
          << FPoint(getLeftPadding(), zeroY)
          << UniChar::BoxDrawingsVerticalAndRight;  // ├
}

//----------------------------------------------------------------------
void GraphWindow::draw_line (double x1, double y1, double x2, double y2, FColor color)
{
  auto start = transform_to_screen(x1, y1);
  auto end = transform_to_screen(x2, y2);
  int dx = std::abs(end.getX() - start.getX());
  int dy = std::abs(end.getY() - start.getY());
  int sx = (start.getX() < end.getX()) ? 1 : -1;
  int sy = (start.getY() < end.getY()) ? 1 : -1;
  int err = dx - dy;
  int x = start.getX();
  int y = start.getY();
  setForegroundColor(color);

  while ( true )
  {
    print() << FPoint{x, y} << UniChar::BlackCircle;

    if ( x == end.getX() && y == end.getY() )
      break;

    int e2 = 2 * err;

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
void GraphWindow::draw_line (double angle, double magnitude, FColor color)
{
  const double radians = angle * (M_PI / 180.0);
  double xEnd = magnitude * std::cos(radians);
  double yEnd = magnitude * std::sin(radians);
  auto start = transform_to_screen(0.0, 0.0);
  auto end = transform_to_screen(xEnd, yEnd);
  int dx = std::abs(end.getX() - start.getX());
  int dy = std::abs(end.getY() - start.getY());
  int sx = (start.getX() < end.getX()) ? 1 : -1;
  int sy = (start.getY() < end.getY()) ? 1 : -1;
  int err = dx - dy;
  int x = start.getX();
  int y = start.getY();
  setForegroundColor(color);

  while ( true )
  {
    print() << FPoint{x, y} << UniChar::BlackCircle;

    if (x == end.getX() && y == end.getY())
      break;

    int e2 = 2 * err;

    if (e2 > -dy)
    {
      err -= dy;
      x += sx;
    }

    if (e2 < dx)
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
  GraphWindow graph{&app, -200, 200, -200, 200};
  graph.setText ("Sinusoidal wave");
  auto graph_size = FSize { FVTerm::getFOutput()->getColumnNumber() - 1
                          , FVTerm::getFOutput()->getLineNumber() - 1 };
  graph.setGeometry (FPoint{1, 1}, graph_size);
  graph.setMinimumSize (FSize{30, 15});
  graph.setResizeable();
  graph.setMinimizable();
  finalcut::FWidget::setMainWidget (&graph);
  graph.show();
  return app.exec();
}
