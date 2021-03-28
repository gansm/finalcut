/***********************************************************************
* mandelbrot.cpp - Shows a ASCII based Mandelbrot set                  *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2021 Markus Gans                                      *
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

using finalcut::FColor;
using finalcut::FPoint;
using finalcut::FSize;


//----------------------------------------------------------------------
// class Mandelbrot
//----------------------------------------------------------------------

class Mandelbrot final : public finalcut::FDialog
{
  public:
    // Constructor
    explicit Mandelbrot (finalcut::FWidget* = nullptr);

    // Destructor
    ~Mandelbrot() override = default;

    // Event handlers
    void onKeyPress (finalcut::FKeyEvent*) override;
    void onClose (finalcut::FCloseEvent*) override;

  private:
    // Methods
    void initLayout() override;
    void draw() override;
    void adjustSize() override;
};


//----------------------------------------------------------------------
Mandelbrot::Mandelbrot (finalcut::FWidget* parent)
  : finalcut::FDialog{parent}
{ }

//----------------------------------------------------------------------
void Mandelbrot::initLayout()
{
  FDialog::setText ("Mandelbrot set");
  FDialog::setGeometry (FPoint{6, 1}, FSize{70, 23});
  FDialog::initLayout();
}

//----------------------------------------------------------------------
void Mandelbrot::draw()
{
  finalcut::FDialog::draw();

  const double x_min{-2.20};
  const double x_max{+1.00};
  const double y_min{-1.05};
  const double y_max{+1.05};
  const int max_iter{99};

  const int xoffset{2};
  const int yoffset{2};
  const auto Cols = int(getClientWidth());
  const auto Lines = int(getClientHeight());
  int current_line{0};

  if ( Cols < 2 || Lines < 2 )
    return;

  const double dX = (x_max - x_min) / (Cols - 1);
  const double dY = (y_max - y_min) / Lines;
  double y0 = y_min;

  while ( y0 < y_max && current_line < Lines )
  {
    current_line++;
    print() << FPoint{xoffset, yoffset + current_line};
    double x0 = x_min;

    while ( x0 < x_max )
    {
      double x{0.0};
      double y{0.0};
      int iter{0};

      while ( x * x + y * y < 4 && iter < max_iter )
      {
        const double xtemp = x * x - y * y + x0;
        y = 2 * x * y + y0;
        x = xtemp;
        iter++;
      }

      if ( iter < max_iter )
        setColor(FColor::Black, FColor(iter % 16));
      else
        setColor(FColor::Black, FColor::Black);

      print(' ');
      x0 += dX;
    }

    y0 += dY;
  }
}

//----------------------------------------------------------------------
void Mandelbrot::onKeyPress (finalcut::FKeyEvent* ev)
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
void Mandelbrot::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void Mandelbrot::adjustSize()
{
  std::size_t h = getDesktopHeight();
  std::size_t w = getDesktopWidth();

  if ( h > 1 )
    h--;

  if ( w > 10 )
    w -= 10;

  setGeometry(FPoint{6, 1}, FSize{w, h}, false);
  finalcut::FDialog::adjustSize();
}

//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
{
  // Create the application object
  finalcut::FApplication app{argc, argv};

  // Create a simple dialog box
  Mandelbrot mb{&app};
  mb.setShadow();  // Instead of the transparent window shadow

  // Set the mandelbrot object as main widget
  finalcut::FWidget::setMainWidget(&mb);

  // Show and start the application
  mb.show();
  return app.exec();
}
