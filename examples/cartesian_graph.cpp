/***********************************************************************
* cartesian_graph.cpp - A Cartesian Coord. Sys. example                *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2022 Markus Gans                                      *
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

#include <final/final.h>

using finalcut::FPoint;
using finalcut::FSize;
using finalcut::FColor;
using finalcut::UniChar;
using finalcut::FVTerm;

class GraphWindow final : public finalcut::FDialog
{
  public:
    explicit GraphWindow (finalcut::FWidget* = nullptr, double xMin = -10, double xMax = 10, double yMin = -10, double yMax = 10);

    GraphWindow (const GraphWindow&) = delete;

    GraphWindow (GraphWindow&&) noexcept = delete;

    ~GraphWindow() noexcept override = default;

    auto operator = (const GraphWindow&) -> GraphWindow& = delete;

    auto operator = (GraphWindow&&) noexcept -> GraphWindow& = delete;

    void add_point(double x, double y); // Add a new point to the graph
    void add_realtime_point(double val, double interval = 1);
    void draw_line(double x1, double y1, double x2, double y2, FColor color);
    void draw_line(double angle, double magnitude, FColor color);

  private:
    void draw() override;

    std::deque<std::pair<double, double>> _points; 
    std::size_t _maxPoints{40};                  
    double _xMin, _xMax, _yMin, _yMax; 

    FPoint transform_to_screen(double x, double y) const; 
    void onTimer (finalcut::FTimerEvent* ev);
    void onClose (finalcut::FCloseEvent* ev) override;


};


GraphWindow::GraphWindow(finalcut::FWidget* parent, double xMin, double xMax, double yMin, double yMax)
  : FDialog(parent), _xMin(xMin), _xMax(xMax), _yMin(yMin), _yMax(yMax)
{
  addTimer(200);
}

void GraphWindow::add_point(double x, double y)
{
  if (_points.size() >= _maxPoints)
    _points.pop_front(); 

  _points.emplace_back(x, y);
  redraw(); 
}

void GraphWindow::add_realtime_point(double val, double interval)
{
    static double x = _xMin;
    add_point(x, val);
    x += interval;

    if (x > _xMax)
      x = _xMin;

    redraw();
    flush();
}

FPoint GraphWindow::transform_to_screen(double x, double y) const
{
  int width = getWidth();
  int height = getHeight();
  
  int screenX = static_cast<int>(((x - _xMin) / (_xMax - _xMin)) * (width - 1));
  int screenY = static_cast<int>(height - 1 - ((y - _yMin) / (_yMax - _yMin)) * (height - 1));
  
  return FPoint{screenX, screenY};
}

void GraphWindow::onTimer(finalcut::FTimerEvent* ev)
{
    static double x = _xMin; 
    static const double interval = 10; // Adjust this for the smoothness of the sine wave
    static const double amplitude = (_yMax - _yMin) / 4.0; // Adjust amplitude (height of the wave)
    static const double frequency = 1.0; // Frequency of the sine wave (number of cycles)

    double y = amplitude * std::sin(2 * M_PI * frequency * (x - _xMin) / (_xMax - _xMin));

    add_realtime_point(y, interval);

    // Increment x for the next point
    x += interval;

    if (x > _xMax)
        x = _xMin;
}

void GraphWindow::draw()
{
    finalcut::FDialog::draw();

    setColor(FColor::Black, FColor::LightGray);

    int width = getWidth();
    int height = getHeight();

    int zeroY = transform_to_screen(0, 0).getY();
    for (int x = 0; x < width; ++x)
        print() << FPoint{x, zeroY} << '-';

    int zeroX = transform_to_screen(0, 0).getX();
    for (int y = 0; y < height; ++y)
        print() << FPoint{zeroX, y} << '|';

    int xStep = static_cast<int>((_xMax - _xMin) / 10); 
    for (double x = _xMin; x <= _xMax; x += xStep)
    {
        FPoint screenPoint = transform_to_screen(x, 0);
        print() << screenPoint << ' ' << static_cast<int>(x); 
    }

    int yStep = static_cast<int>((_yMax - _yMin) / 10); 
    for (double y = _yMin; y <= _yMax; y += yStep)
    {
        FPoint screenPoint = transform_to_screen(0, y);
        print() << screenPoint << ' ' << static_cast<int>(y);
    }

    for (const auto& point : _points)
    {
        double x = point.first;
        double y = point.second;
        FPoint screenPoint = transform_to_screen(x, y);
        print() << screenPoint << UniChar::BlackCircle;
    }

}

void GraphWindow::draw_line(double x1, double y1, double x2, double y2, FColor color)
{
  FPoint start = transform_to_screen(x1, y1);
  FPoint end = transform_to_screen(x2, y2);

  int dx = std::abs(end.getX() - start.getX());
  int dy = std::abs(end.getY() - start.getY());
  int sx = (start.getX() < end.getX()) ? 1 : -1;
  int sy = (start.getY() < end.getY()) ? 1 : -1;
  int err = dx - dy;

  int x = start.getX();
  int y = start.getY();

  while (true)
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

void GraphWindow::draw_line(double angle, double magnitude, FColor color)
{

  const double radians = angle * (M_PI / 180.0);

  double xEnd = magnitude * std::cos(radians);
  double yEnd = magnitude * std::sin(radians);

  FPoint start = transform_to_screen(0, 0);
  FPoint end = transform_to_screen(xEnd, yEnd);

  int dx = std::abs(end.getX() - start.getX());
  int dy = std::abs(end.getY() - start.getY());
  int sx = (start.getX() < end.getX()) ? 1 : -1;
  int sy = (start.getY() < end.getY()) ? 1 : -1;
  int err = dx - dy;

  int x = start.getX();
  int y = start.getY();

  while (true)
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

void GraphWindow::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}



auto main (int argc, char* argv[]) -> int
{

  finalcut::FApplication app{argc, argv};
  GraphWindow graph{&app, -200, 200, -200, 200};
  graph.setText ("non-transparent");
  graph.setGeometry (FPoint{1, 1}, FSize{FVTerm::getFOutput()->getColumnNumber() - 1, FVTerm::getFOutput()->getLineNumber() - 1});
  graph.setMinimumSize (FSize{30, 15});

  graph.setResizeable();
  graph.setMinimizable();
  finalcut::FWidget::setMainWidget (&graph);

  graph.show();


  return app.exec();

}
