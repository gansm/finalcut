/***********************************************************************
* mouse.cpp - A small mouse-controlled drawing program                 *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2017-2018 Markus Gans                                      *
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
// class ColorChooser
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class ColorChooser : public finalcut::FWidget
{
  public:
    // Constructor
    explicit ColorChooser (finalcut::FWidget* = 0);

    // Destructor
    ~ColorChooser();

    // Accessors
    FColor getForeground();
    FColor getBackground();

  private:
    // Disable copy constructor
    ColorChooser (const ColorChooser&);
    // Disable assignment operator (=)
    ColorChooser& operator = (const ColorChooser&);

    // Method
    virtual void draw();

    // Event handler
    virtual void onMouseDown (finalcut::FMouseEvent*);

    // Data Members
    FColor fg_color;
    FColor bg_color;
    finalcut::FLabel headline;
};
#pragma pack(pop)

//----------------------------------------------------------------------
ColorChooser::ColorChooser (finalcut::FWidget* parent)
  : FWidget(parent)
  , fg_color(finalcut::fc::White)
  , bg_color(finalcut::fc::Black)
  , headline(this)
{
  setSize (8, 12);
  setFixedSize (8, 12);
  unsetFocusable();

  if ( parent )
  {
    FColor fg = parent->getForegroundColor();
    FColor bg = parent->getBackgroundColor();
    setForegroundColor(fg);
    setBackgroundColor(bg);
    headline.setForegroundColor(fg);
    headline.setBackgroundColor(bg);
  }

  // Text label
  headline.setGeometry (1, 1, 8, 1);
  headline.setEmphasis();
  headline.setAlignment (finalcut::fc::alignCenter);
  headline << "Color";
}

//----------------------------------------------------------------------
ColorChooser::~ColorChooser()
{ }

//----------------------------------------------------------------------
void ColorChooser::onMouseDown (finalcut::FMouseEvent* ev)
{
  int mouse_x = ev->getX();
  int mouse_y = ev->getY();

  if ( ev->getButton() == finalcut::fc::MiddleButton )
    return;

  for (int c = 0; c < 16; c++)
  {
    int xmin = 2 + (c / 8) * 3;
    int xmax = 4 + (c / 8) * 3;
    int y = 3 + c % 8;

    if ( mouse_x >= xmin && mouse_x <= xmax && mouse_y == y )
    {
      if ( ev->getButton() == finalcut::fc::LeftButton )
        bg_color = FColor(c);
      else if ( ev->getButton() == finalcut::fc::RightButton )
        fg_color = FColor(c);

      redraw();
      emitCallback("clicked");
    }
  }
}

//----------------------------------------------------------------------
void ColorChooser::draw()
{
  setColor();
  finalcut::FWidget::drawBorder (1, 2, 8, 11);

  for (FColor c = 0; c < 16; c++)
  {
    setPrintPos (2 + (c / 8) * 3, 3 + c % 8);

    if ( c < 6 )
      setColor (finalcut::fc::LightGray, c);
    else if ( c > 8 )
      setColor (finalcut::fc::DarkGray, c);
    else
      setColor (finalcut::fc::White, c);

    if ( c == bg_color )
    {
      print (' ');
      print (finalcut::fc::Times);
      print (' ');
    }
    else
      print ("   ");
  }
}

//----------------------------------------------------------------------
inline FColor ColorChooser::getForeground()
{
  return fg_color;
}

//----------------------------------------------------------------------
inline FColor ColorChooser::getBackground()
{
  return bg_color;
}


//----------------------------------------------------------------------
// class Brushes
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class Brushes : public finalcut::FWidget
{
  public:
    // Constructor
    explicit Brushes (finalcut::FWidget* = 0);

    // Destructor
    ~Brushes();

    // Accessor
    wchar_t getBrush();

    // Mutators
    void setForeground (FColor);
    void setBackground (FColor);

  private:
    // Disable copy constructor
    Brushes (const Brushes&);
    // Disable assignment operator (=)
    Brushes& operator = (const Brushes&);

    // Method
    virtual void draw();

    // Event handler
    virtual void onMouseDown (finalcut::FMouseEvent*);

    // Data Members
    wchar_t brush;
    FColor  fg_color;
    FColor  bg_color;
    finalcut::FLabel headline;
};
#pragma pack(pop)

//----------------------------------------------------------------------
Brushes::Brushes (finalcut::FWidget* parent)
  : FWidget(parent)
  , brush(L' ')
  , fg_color(finalcut::fc::White)
  , bg_color(finalcut::fc::Black)
  , headline(this)
{
  setSize (8, 4);
  setFixedSize (8, 4);
  unsetFocusable();

  if ( parent )
  {
    FColor fg = parent->getForegroundColor();
    FColor bg = parent->getBackgroundColor();
    setForegroundColor(fg);
    setBackgroundColor(bg);
    headline.setForegroundColor(fg);
    headline.setBackgroundColor(bg);
  }

  // Text label
  headline.setGeometry(1, 1, 8, 1);
  headline.setEmphasis();
  headline.setAlignment (finalcut::fc::alignCenter);
  headline << "Brush";
}

//----------------------------------------------------------------------
Brushes::~Brushes()
{ }


//----------------------------------------------------------------------
void Brushes::draw()
{
  int pos;
  setColor();
  finalcut::FWidget::drawBorder (1, 2, 8, 4);
  setColor (fg_color, bg_color);
  setPrintPos (2, 3);
  print("   ");
  print(finalcut::FString(3, wchar_t(finalcut::fc::MediumShade)));

  if ( brush == L' ' )
    pos = 0;
  else
    pos = 3;

  setColor();
  setPrintPos (3 + pos, 2);
  print(wchar_t(finalcut::fc::BlackDownPointingTriangle));
  setPrintPos (3 + pos, 4);
  print(wchar_t(finalcut::fc::BlackUpPointingTriangle));
}

//----------------------------------------------------------------------
void Brushes::onMouseDown (finalcut::FMouseEvent* ev)
{
  int mouse_x = ev->getX();
  int mouse_y = ev->getY();

  if ( ev->getButton() != finalcut::fc::LeftButton )
    return;

  if ( mouse_x >= 2 && mouse_x <= 4 && mouse_y == 3 )
  {
    brush = L' ';
    redraw();
  }
  else if ( mouse_x >= 5 && mouse_x <= 7 && mouse_y == 3 )
  {
    brush = wchar_t(finalcut::fc::MediumShade);
    redraw();
  }
}

//----------------------------------------------------------------------
inline wchar_t Brushes::getBrush()
{
  return brush;
}

//----------------------------------------------------------------------
inline void Brushes::setForeground (FColor color)
{
  fg_color = color;
}

//----------------------------------------------------------------------
inline void Brushes::setBackground (FColor color)
{
  bg_color = color;
}


//----------------------------------------------------------------------
// class MouseDraw
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class MouseDraw : public finalcut::FDialog
{
  public:
    // Using-declaration
    using FWidget::setGeometry;

    // Constructor
    explicit MouseDraw (finalcut::FWidget* = 0);

    // Destructor
    ~MouseDraw();

    // Methods
    void setGeometry (int, int, std::size_t, std::size_t, bool = true);

    // Event handlers
    virtual void onAccel (finalcut::FAccelEvent*);
    virtual void onClose (finalcut::FCloseEvent*);

  private:
    // Disable copy constructor
    MouseDraw (const MouseDraw&);
    // Disable assignment operator (=)
    MouseDraw& operator = (const MouseDraw&);

    // Methods
    virtual void draw();
    void drawBrush (int, int, bool = false);
    void drawCanvas();
    virtual void adjustSize();

    // Event handler
    virtual void onMouseDown (finalcut::FMouseEvent*);
    virtual void onMouseMove (finalcut::FMouseEvent*);

    // Callback methods
    void cb_colorChanged (finalcut::FWidget*, data_ptr);

    // Data Members
    term_area*   canvas;
    ColorChooser c_chooser;
    Brushes      brush;
};
#pragma pack(pop)

//----------------------------------------------------------------------
MouseDraw::MouseDraw (finalcut::FWidget* parent)
  : finalcut::FDialog(parent)
  , canvas(0)
  , c_chooser(this)
  , brush(this)
{
  setText ("Drawing with the mouse");
  c_chooser.setPos (1, 1);
  c_chooser.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MouseDraw::cb_colorChanged)
  );

  brush.setPos (1, 12);

  finalcut::FPoint no_shadow(0, 0);
  finalcut::FRect scroll_geometry(0, 0, 1, 1);
  createArea (scroll_geometry, no_shadow, canvas);
}

//----------------------------------------------------------------------
MouseDraw::~MouseDraw()
{ }

//----------------------------------------------------------------------
void MouseDraw::setGeometry ( int x, int y
                            , std::size_t w, std::size_t h
                            , bool adjust )
{
  int old_w, old_h;
  finalcut::FDialog::setGeometry (x, y, w, h, adjust);
  finalcut::FPoint no_shadow(0, 0);
  finalcut::FRect scroll_geometry (0, 0, w - 11, h - 3);
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
void MouseDraw::onAccel (finalcut::FAccelEvent* ev)
{
  close();
  ev->accept();
}

//----------------------------------------------------------------------
void MouseDraw::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void MouseDraw::draw()
{
  int y_max = int(getHeight());
  finalcut::FDialog::draw();
  setColor();

  if ( isNewFont() )
  {
    for (int y = 2; y < y_max; y++)
    {
      setPrintPos (10, y);
      print (wchar_t(finalcut::fc::NF_rev_border_line_right));
    }

    setPrintPos (10, y_max);
    print (wchar_t(finalcut::fc::NF_rev_border_corner_lower_right));
  }
  else
  {
    setPrintPos (10, 2);
    print (wchar_t(finalcut::fc::BoxDrawingsDownAndHorizontal));

    for (int y = 3; y < y_max; y++)
    {
      setPrintPos (10, y);
      print (wchar_t(finalcut::fc::BoxDrawingsVertical));
    }

    setPrintPos (10, y_max);
    print (wchar_t(finalcut::fc::BoxDrawingsUpAndHorizontal));
  }

  drawCanvas();
}

//----------------------------------------------------------------------
void MouseDraw::drawBrush (int x, int y, bool swap_color)
{
  int Cols  = int(getWidth());
  int Lines = int(getHeight());

  if ( x > 10 && x < Cols && y > 2 && y < Lines )
  {
    if ( swap_color )
      setColor (c_chooser.getBackground(), c_chooser.getForeground());
    else
      setColor (c_chooser.getForeground(), c_chooser.getBackground());

    // set canvas print cursor position
    canvas->cursor_x = x - canvas->offset_left - 10;
    canvas->cursor_y = y - canvas->offset_top - 2;
    // print on canvas
    print (canvas, brush.getBrush());
    // copy canvas to the dialog
    drawCanvas();
  }
}

//----------------------------------------------------------------------
void MouseDraw::drawCanvas()
{
  if ( ! hasPrintArea() )
    finalcut::FVTerm::getPrintArea();

  if ( ! (hasPrintArea() && canvas) )
    return;

  int ax = 9 + getTermX() - print_area->offset_left
    , ay = 1 + getTermY() - print_area->offset_top
    , y_end = canvas->height
    , x_end = canvas->width
    , w_line_len = print_area->width + print_area->right_shadow;

  for (int y = 0; y < y_end; y++)  // line loop
  {
    charData* canvaschar;  // canvas character
    charData* winchar;     // window character
    canvaschar = &canvas->text[y * x_end];
    winchar = &print_area->text[(ay + y) * w_line_len + ax];
    std::memcpy (winchar, canvaschar, sizeof(charData) * unsigned(x_end));

    if ( short(print_area->changes[ay + y].xmin) > ax )
      print_area->changes[ay + y].xmin = uInt(ax);

    if ( short(print_area->changes[ay + y].xmax) < ax + x_end - 1 )
      print_area->changes[ay + y].xmax = uInt(ax + x_end - 1);
  }

  print_area->has_changes = true;
}

//----------------------------------------------------------------------
void MouseDraw::adjustSize()
{
  std::size_t w = 60, h = 18;
  int x = 1 + int((getParentWidget()->getWidth() - w) / 2);
  int y = 1 + int((getParentWidget()->getHeight() - h) / 2);
  setGeometry (x, y, w, h, false);
  finalcut::FDialog::adjustSize();
}

//----------------------------------------------------------------------
void MouseDraw::onMouseDown (finalcut::FMouseEvent* ev)
{
  finalcut::FDialog::onMouseDown(ev);

  if ( ev->getButton() != finalcut::fc::LeftButton
    && ev->getButton() != finalcut::fc::RightButton )
    return;

  drawBrush ( ev->getX()
            , ev->getY()
            , ev->getButton() == finalcut::fc::RightButton );
}

//----------------------------------------------------------------------
void MouseDraw::onMouseMove (finalcut::FMouseEvent* ev)
{
  FDialog::onMouseMove(ev);

  if ( ev->getButton() != finalcut::fc::LeftButton
    && ev->getButton() != finalcut::fc::RightButton )
    return;

  drawBrush ( ev->getX()
            , ev->getY()
            , ev->getButton() == finalcut::fc::RightButton);
}

//----------------------------------------------------------------------
void MouseDraw::cb_colorChanged (finalcut::FWidget*, data_ptr)
{
  brush.setForeground (c_chooser.getForeground());
  brush.setBackground (c_chooser.getBackground());
  brush.redraw();
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
{
  // Create the application object
  finalcut::FApplication app(argc, argv);

  // Create a simple dialog box
  MouseDraw mouse_draw(&app);
  mouse_draw.setGeometry (12, 4, 60, 18);
  mouse_draw.addAccelerator('q');  // press 'q' to quit

  // Set dialog object mouse_draw as main widget
  app.setMainWidget(&mouse_draw);

  // Show and start the application
  mouse_draw.show();
  return app.exec();
}
