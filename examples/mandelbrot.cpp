/***********************************************************************
* mandelbrot.cpp - Shows a ASCII based Mandelbrot set                  *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2015-2019 Markus Gans                                      *
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

#include <final/final.h>

namespace fc = finalcut::fc;
using finalcut::FPoint;
using finalcut::FSize;


//----------------------------------------------------------------------
// class Mandelbrot
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class Mandelbrot : public finalcut::FDialog
{
  public:
    // Constructor
    explicit Mandelbrot (finalcut::FWidget* = nullptr);

    // Destructor
    ~Mandelbrot();

    // Event handlers
    void onKeyPress (finalcut::FKeyEvent*) override;
    void onClose (finalcut::FCloseEvent*) override;

  private:
    // Methods
    void draw() override;
    void adjustSize() override;
};
#pragma pack(pop)

//----------------------------------------------------------------------
Mandelbrot::Mandelbrot (finalcut::FWidget* parent)
  : finalcut::FDialog(parent)
{
  setText ("Mandelbrot set");
}

//----------------------------------------------------------------------
Mandelbrot::~Mandelbrot()
{ }

//----------------------------------------------------------------------
void Mandelbrot::draw()
{
  finalcut::FDialog::draw();

  double x_min{-2.20};
  double x_max{+1.00};
  double y_min{-1.05};
  double y_max{+1.05};
  int max_iter{99};

  int xoffset{2};
  int yoffset{2};
  int current_line{0};
  int Cols  = int(getClientWidth());
  int Lines = int(getClientHeight());

  double dX = (x_max - x_min) / (Cols - 1);
  double dY = (y_max - y_min) / Lines;

  for (double y0 = y_min; y0 < y_max && current_line < Lines; y0 += dY)
  {
    current_line++;
    print() << FPoint(xoffset, yoffset + current_line);

    for (double x0 = x_min; x0 < x_max; x0 += dX)
    {
      double x{0.0};
      double y{0.0};
      int iter{0};

      while ( x * x + y * y < 4 && iter < max_iter )
      {
        double xtemp = x * x - y * y + x0;
        y = 2 * x * y + y0;
        x = xtemp;
        iter++;
      }

      if ( iter < max_iter )
        setColor(fc::Black, iter % 16);
      else
        setColor(fc::Black, 0);

      print(' ');
    }
  }
}

//----------------------------------------------------------------------
void Mandelbrot::onKeyPress (finalcut::FKeyEvent* ev)
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
void Mandelbrot::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void Mandelbrot::adjustSize()
{
  std::size_t h = getDesktopHeight() - 1;
  std::size_t w = getDesktopWidth() - 10;
  setGeometry(FPoint(6, 1), FSize(w, h), false);
  finalcut::FDialog::adjustSize();
}

//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
{
  // Create the application object
  finalcut::FApplication app(argc, argv);

  // Create a simple dialog box
  Mandelbrot mb(&app);
  mb.setGeometry (FPoint(6, 1), FSize(70, 23));
  mb.setShadow();

  // Set the mandelbrot object as main widget
  app.setMainWidget(&mb);

  // Show and start the application
  mb.show();
  return app.exec();
}
