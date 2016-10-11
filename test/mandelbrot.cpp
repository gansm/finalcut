// File: mandelbrot.cpp

#include "fapp.h"
#include "fdialog.h"
#include "fmessagebox.h"

//----------------------------------------------------------------------
// class Mandelbrot
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class Mandelbrot : public FDialog
{
 private:
   virtual void draw();
   void adjustSize();

 public:
   // Constructor
   explicit Mandelbrot (FWidget* = 0);
   // Destructor
  ~Mandelbrot();

   // Callback methods
   void onAccel (FAccelEvent*);
   void onClose (FCloseEvent*);
};
#pragma pack(pop)

//----------------------------------------------------------------------
Mandelbrot::Mandelbrot (FWidget* parent)
  : FDialog(parent)
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

  FDialog::draw();

  x_min = -2.20;
  x_max =  1.00;
  y_min = -1.05;
  y_max =  1.05;
  max_iter = 99;

  xoffset = 2;
  yoffset = 2;
  current_line = 0;
  Cols  = getClientWidth();
  Lines = getClientHeight();

  dX = (x_max - x_min) / (Cols - 1);
  dY = (y_max - y_min) / Lines;

  for (y0=y_min; y0 < y_max && current_line < Lines; y0+=dY)
  {
    current_line++;
    setPrintPos (xoffset, yoffset + current_line);

    for (x0=x_min; x0 < x_max; x0+=dX)
    {
      x = 0.0;
      y = 0.0;
      iter = 0;

      while ( x*x + y*y < 4 && iter < max_iter )
      {
        xtemp = x*x - y*y + x0;
        y = 2*x*y + y0;
        x = xtemp;
        iter++;
      }

      if ( iter < max_iter )
        setColor(fc::Black, iter%16);
      else
        setColor(fc::Black, 0);

      print(' ');
    }
  }

  updateVTerm(true);
}

//----------------------------------------------------------------------
void Mandelbrot::onAccel (FAccelEvent* ev)
{
  close();
  ev->accept();
}

//----------------------------------------------------------------------
void Mandelbrot::onClose (FCloseEvent* ev)
{
  int ret = FMessageBox::info ( this, "Quit"
                              , "Do you really want\n"
                                "to quit the program ?"
                              , FMessageBox::Yes
                              , FMessageBox::No );

  if ( ret == FMessageBox::Yes )
    ev->accept();
  else
    ev->ignore();
}

//----------------------------------------------------------------------
void Mandelbrot::adjustSize()
{
  int h = getParentWidget()->getHeight() - 1;
  int w = getParentWidget()->getWidth() - 10;
  setGeometry(6, 1, w, h, false);
  FDialog::adjustSize();
}

//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
{
  // Create the application object
  FApplication app(argc, argv);

  // Create a simple dialog box
  Mandelbrot mb(&app);
  mb.setGeometry (6, 1, 70, 23);
  mb.addAccelerator('q');  // press 'q' to quit
  mb.setShadow();

  app.setMainWidget(&mb);
  mb.show();
  return app.exec();
}
