// File: mouse.cpp

#include "fapp.h"
#include "fdialog.h"
#include "flabel.h"
#include "fmessagebox.h"


//----------------------------------------------------------------------
// class ColorChooser
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class ColorChooser : public FWidget
{
 public:
   // Constructor
   explicit ColorChooser (FWidget* = 0);

   // Destructor
  ~ColorChooser();

   // Accessors
   short getForeground();
   short getBackground();

 private:
   // Disable copy constructor
   ColorChooser (const ColorChooser&);
   // Disable assignment operator (=)
   ColorChooser& operator = (const ColorChooser&);

   // Method
   void draw();

   // Event handler
   void onMouseDown (FMouseEvent*);

   // Data Members
   short fg_color;
   short bg_color;
};
#pragma pack(pop)

//----------------------------------------------------------------------
ColorChooser::ColorChooser (FWidget* parent)
  : FWidget(parent)
  , fg_color(fc::White)
  , bg_color(fc::Black)
{
  setSize (8, 12);
  setFixedSize (8, 12);
  unsetFocusable();

  if ( parent )
  {
    setForegroundColor(parent->getForegroundColor());
    setBackgroundColor(parent->getBackgroundColor());
  }

  // Text label
  FLabel* headline = new FLabel (this);
  headline->setGeometry(1, 1, 8, 1);
  headline->setText ("Color");
  headline->setEmphasis();
  headline->setAlignment (fc::alignCenter);
}

//----------------------------------------------------------------------
ColorChooser::~ColorChooser()
{ }

//----------------------------------------------------------------------
void ColorChooser::onMouseDown (FMouseEvent* ev)
{
  int mouse_x = ev->getX();
  int mouse_y = ev->getY();

  if ( ev->getButton() == fc::MiddleButton )
    return;

  for (int c=0; c < 16; c++)
  {
    int xmin = 2 + (c / 8) * 3;
    int xmax = 4 + (c / 8) * 3;
    int y = 3 + c % 8;

    if ( mouse_x >= xmin && mouse_x <= xmax && mouse_y == y )
    {
      if ( ev->getButton() == fc::LeftButton )
        bg_color = short(c);
      else if ( ev->getButton() == fc::RightButton )
        fg_color = short(c);

      redraw();
      emitCallback("clicked");
    }
  }
}

//----------------------------------------------------------------------
void ColorChooser::draw()
{
  setColor();
  FWidget::drawBorder (1, 2, 8, 11);

  for (short c=0; c < 16; c++)
  {
    setPrintPos (2 + (c/8)*3, 3 + c%8);

    if ( c < 6 )
      setColor (fc::LightGray, c);
    else if ( c > 8 )
      setColor (fc::DarkGray, c);
    else
      setColor (fc::White, c);

    if ( c == bg_color )
    {
      print (' ');
      print (fc::Times);
      print (' ');
    }
    else
      print ("   ");
  }
}

//----------------------------------------------------------------------
inline short ColorChooser::getForeground()
{
  return fg_color;
}

//----------------------------------------------------------------------
inline short ColorChooser::getBackground()
{
  return bg_color;
}


//----------------------------------------------------------------------
// class Brushes
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class Brushes : public FWidget
{
 public:
   // Constructor
   explicit Brushes (FWidget* = 0);

   // Destructor
  ~Brushes();

   // Accessor
   wchar_t getBrush();

   // Mutators
   void setForeground (short);
   void setBackground (short);

 private:
   // Disable copy constructor
   Brushes (const Brushes&);
   // Disable assignment operator (=)
   Brushes& operator = (const Brushes&);

   // Method
   void draw();

   // Event handler
   void onMouseDown (FMouseEvent*);

   // Data Members
   wchar_t brush;
   short   fg_color;
   short   bg_color;
};
#pragma pack(pop)

//----------------------------------------------------------------------
Brushes::Brushes (FWidget* parent)
  : FWidget(parent)
  , brush(L' ')
  , fg_color(fc::White)
  , bg_color(fc::Black)
{
  setSize (8, 4);
  setFixedSize (8, 4);
  unsetFocusable();

  if ( parent )
  {
    setForegroundColor(parent->getForegroundColor());
    setBackgroundColor(parent->getBackgroundColor());
  }

  // Text label
  FLabel* headline = new FLabel (this);
  headline->setGeometry(1, 1, 8, 1);
  headline->setText ("Brush");
  headline->setEmphasis();
  headline->setAlignment (fc::alignCenter);
}

//----------------------------------------------------------------------
Brushes::~Brushes()
{ }


//----------------------------------------------------------------------
void Brushes::draw()
{
  int pos;
  setColor();
  FWidget::drawBorder (1, 2, 8, 4);
  setColor (fg_color, bg_color);
  setPrintPos (2, 3);
  print("   ");
  print(FString(3, wchar_t(fc::MediumShade)));

  if ( brush == L' ' )
    pos = 0;
  else
    pos = 3;

  setColor();
  setPrintPos (3 + pos, 2);
  print(wchar_t(fc::BlackDownPointingTriangle));
  setPrintPos (3 + pos, 4);
  print(wchar_t(fc::BlackUpPointingTriangle));
}

//----------------------------------------------------------------------
void Brushes::onMouseDown (FMouseEvent* ev)
{
  int mouse_x = ev->getX();
  int mouse_y = ev->getY();

  if ( ev->getButton() != fc::LeftButton )
    return;

  if ( mouse_x >= 2 && mouse_x <= 4 && mouse_y == 3 )
  {
    brush = L' ';
    redraw();
  }
  else if ( mouse_x >= 5 && mouse_x <= 7 && mouse_y == 3 )
  {
    brush = wchar_t(fc::MediumShade);
    redraw();
  }
}

//----------------------------------------------------------------------
inline wchar_t Brushes::getBrush()
{
  return brush;
}

//----------------------------------------------------------------------
inline void Brushes::setForeground (short color)
{
  fg_color = color;
}

//----------------------------------------------------------------------
inline void Brushes::setBackground (short color)
{
  bg_color = color;
}


//----------------------------------------------------------------------
// class MouseDraw
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class MouseDraw : public FDialog
{
 public:
   // Using-declaration
   using FWidget::setGeometry;

   // Constructor
   explicit MouseDraw (FWidget* = 0);

   // Destructor
  ~MouseDraw();

   // Methods
   void setGeometry (int, int, int, int, bool = true);

   // Event handlers
   void onAccel (FAccelEvent*);
   void onClose (FCloseEvent*);

 private:
   // Disable copy constructor
   MouseDraw (const MouseDraw&);
   // Disable assignment operator (=)
   MouseDraw& operator = (const MouseDraw&);

   // Methods
   virtual void draw();
   void drawBrush (int, int, bool = false);
   void drawCanvas();
   void adjustSize();

   // Event handler
   void onMouseDown (FMouseEvent*);
   void onMouseMove (FMouseEvent*);

   // Callback methods
   void cb_colorChanged (FWidget*, data_ptr);

   // Data Members
   term_area*    canvas;
   ColorChooser* c_chooser;
   Brushes*      brush;
};
#pragma pack(pop)

//----------------------------------------------------------------------
MouseDraw::MouseDraw (FWidget* parent)
  : FDialog(parent)
  , canvas(0)
  , c_chooser()
  , brush()
{
  setText ("Drawing with the mouse");
  c_chooser = new ColorChooser(this);
  c_chooser->setPos (1,1);
  c_chooser->addCallback
  (
    "clicked",
    _METHOD_CALLBACK (this, &MouseDraw::cb_colorChanged)
  );

  brush = new Brushes(this);
  brush->setPos (1,12);

  FPoint no_shadow(0,0);
  FRect scroll_geometry(0, 0, 1, 1);
  createArea (scroll_geometry, no_shadow, canvas);
}

//----------------------------------------------------------------------
MouseDraw::~MouseDraw()
{ }

//----------------------------------------------------------------------
void MouseDraw::setGeometry (int x, int y, int w, int h, bool adjust)
{
  int old_w, old_h;
  FDialog::setGeometry (x, y, w, h, adjust);

  FPoint no_shadow(0,0);
  FRect scroll_geometry (0, 0, w-11, h-3);
  old_w = canvas->width;
  old_h = canvas->height;
  resizeArea (scroll_geometry, no_shadow, canvas);

  if ( old_w != canvas->width || old_h != canvas->height )
  {
    setColor(getForegroundColor(), getBackgroundColor());
    clearArea (canvas, ' ');
  }
}

//----------------------------------------------------------------------
void MouseDraw::onAccel (FAccelEvent* ev)
{
  close();
  ev->accept();
}

//----------------------------------------------------------------------
void MouseDraw::onClose (FCloseEvent* ev)
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
void MouseDraw::draw()
{
  int y_max = getHeight();
  FDialog::draw();
  setColor();
  setPrintPos (10, 2);
  print (wchar_t(fc::BoxDrawingsDownAndHorizontal));

  for (int y=3; y < y_max; y++)
  {
    setPrintPos (10, y);
    print (wchar_t(fc::BoxDrawingsVertical));
  }

  setPrintPos (10, y_max);
  print (wchar_t(fc::BoxDrawingsUpAndHorizontal));
  drawCanvas();
}

//----------------------------------------------------------------------
void MouseDraw::drawBrush (int x, int y, bool swap_color)
{
  int Cols  = getWidth();
  int Lines = getHeight();

  if ( x > 10 && x < Cols && y > 2 && y < Lines )
  {
    if ( swap_color )
      setColor (c_chooser->getBackground(), c_chooser->getForeground());
    else
      setColor (c_chooser->getForeground(), c_chooser->getBackground());

    // set canvas print cursor position
    canvas->cursor_x = x - canvas->offset_left - 10;
    canvas->cursor_y = y - canvas->offset_top - 2;
    // print on canvas
    print (canvas, brush->getBrush());
    // copy canvas to the dialog
    drawCanvas();
  }
}

//----------------------------------------------------------------------
void MouseDraw::drawCanvas()
{
  int ax, ay, y_end, x_end;

  if ( ! hasPrintArea() )
    FVTerm::getPrintArea();

  if ( ! (hasPrintArea() && canvas) )
    return;

  ax = 9 + getTermX() - print_area->offset_left;
  ay = 1 + getTermY() - print_area->offset_top;
  y_end = canvas->height;
  x_end = canvas->width;
  int w_line_len = print_area->width + print_area->right_shadow;

  for (int y=0; y < y_end; y++)  // line loop
  {
    char_data* cc; // canvas character
    char_data* wc; // window character
    cc = &canvas->text[y * x_end];
    wc = &print_area->text[(ay+y) * w_line_len + ax];
    std::memcpy (wc, cc, sizeof(char_data) * unsigned(x_end));

    if ( short(print_area->changes[ay+y].xmin) > ax )
      print_area->changes[ay+y].xmin = uInt(ax);

    if ( short(print_area->changes[ay+y].xmax) < ax+x_end-1 )
      print_area->changes[ay+y].xmax = uInt(ax+x_end-1);
  }

  print_area->has_changes = true;
}

//----------------------------------------------------------------------
void MouseDraw::adjustSize()
{
  int w = 60;
  int h = 18;
  int x = 1 + (getParentWidget()->getWidth() - w) / 2;
  int y = 1 + (getParentWidget()->getHeight() - h) / 2;
  setGeometry (x, y, w, h, false);
  FDialog::adjustSize();
}

//----------------------------------------------------------------------
void MouseDraw::onMouseDown (FMouseEvent* ev)
{
  FDialog::onMouseDown(ev);

  if ( ev->getButton() != fc::LeftButton
      && ev->getButton() != fc::RightButton )
    return;

  drawBrush (ev->getX(), ev->getY(), ev->getButton() == fc::RightButton);
}

//----------------------------------------------------------------------
void MouseDraw::onMouseMove (FMouseEvent* ev)
{
  FDialog::onMouseMove(ev);

  if ( ev->getButton() != fc::LeftButton
      && ev->getButton() != fc::RightButton )
    return;

  drawBrush (ev->getX(), ev->getY(), ev->getButton() == fc::RightButton);
}

//----------------------------------------------------------------------
void MouseDraw::cb_colorChanged (FWidget*, data_ptr)
{
  brush->setForeground (c_chooser->getForeground());
  brush->setBackground (c_chooser->getBackground());
  brush->redraw();
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
{
  // Create the application object
  FApplication app(argc, argv);

  // Create a simple dialog box
  MouseDraw mouse_draw(&app);
  mouse_draw.setGeometry (12, 4, 60, 18);
  mouse_draw.addAccelerator('q');  // press 'q' to quit

  app.setMainWidget(&mouse_draw);
  mouse_draw.show();
  return app.exec();
}
