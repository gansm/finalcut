/***********************************************************************
* mandelbrot.cpp - Shows a ASCII based Mandelbrot set                  *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2015-2018 Markus Gans                                      *
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


//----------------------------------------------------------------------
// class Mandelbrot
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class Mandelbrot : public finalcut::FDialog
{
  public:
    // Constructor
    explicit Mandelbrot (finalcut::FWidget* = 0);

    // Destructor
    ~Mandelbrot();

    // Event handlers
    virtual void onAccel (finalcut::FAccelEvent*);
    virtual void onClose (finalcut::FCloseEvent*);

  private:
    // Methods
    virtual void draw();
    virtual void adjustSize();
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
  int iter, max_iter;
  int Cols, Lines, xoffset, yoffset, current_line;
  double x, y, xtemp, x0, y0, dX, dY;
  double x_min, x_max, y_min, y_max;

  finalcut::FDialog::draw();

  x_min = -2.20;
  x_max =  1.00;
  y_min = -1.05;
  y_max =  1.05;
  max_iter = 99;

  xoffset = 2;
  yoffset = 2;
  current_line = 0;
  Cols  = int(getClientWidth());
  Lines = int(getClientHeight());

  dX = (x_max - x_min) / (Cols - 1);
  dY = (y_max - y_min) / Lines;

  for (y0 = y_min; y0 < y_max && current_line < Lines; y0 += dY)
  {
    current_line++;
    setPrintPos (xoffset, yoffset + current_line);

    for (x0 = x_min; x0 < x_max; x0 += dX)
    {
      x = 0.0;
      y = 0.0;
      iter = 0;

      while ( x * x + y * y < 4 && iter < max_iter )
      {
        xtemp = x * x - y * y + x0;
        y = 2 * x * y + y0;
        x = xtemp;
        iter++;
      }

      if ( iter < max_iter )
        setColor(finalcut::fc::Black, iter % 16);
      else
        setColor(finalcut::fc::Black, 0);

      print(' ');
    }
  }
}

//----------------------------------------------------------------------
void Mandelbrot::onAccel (finalcut::FAccelEvent* ev)
{
  close();
  ev->accept();
}

//----------------------------------------------------------------------
void Mandelbrot::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void Mandelbrot::adjustSize()
{
  std::size_t h = getParentWidget()->getHeight() - 1;
  std::size_t w = getParentWidget()->getWidth() - 10;
  setGeometry(6, 1, w, h, false);
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
  mb.setGeometry (6, 1, 70, 23);
  mb.addAccelerator('q');  // press 'q' to quit
  mb.setShadow();

  // Set the mandelbrot object as main widget
  app.setMainWidget(&mb);

  // Show and start the application
  mb.show();
  return app.exec();
}
