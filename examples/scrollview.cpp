/***********************************************************************
* scrollview.cpp - Shows client widgets in a scroll area               *
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
// class Scrollview
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class Scrollview : public finalcut::FScrollView
{
  public:
    // Constructor
    explicit Scrollview (finalcut::FWidget* = 0);

    // Destructor
    ~Scrollview  ();

    // Mutator
    void setScrollSize (std::size_t, std::size_t);

  private:
    // Disable copy constructor
    Scrollview (const Scrollview&);
    // Disable assignment operator (=)
    Scrollview& operator = (const Scrollview&);

    // Method
    virtual void draw();

    // Callback methods
    void cb_go_east (finalcut::FWidget*, data_ptr);
    void cb_go_south (finalcut::FWidget*, data_ptr);
    void cb_go_west (finalcut::FWidget*, data_ptr);
    void cb_go_north (finalcut::FWidget*, data_ptr);

    // Data Members
    wchar_t pointer_right;
    wchar_t pointer_down;
    wchar_t pointer_left;
    wchar_t pointer_up;
    finalcut::FButton go_east;
    finalcut::FButton go_south;
    finalcut::FButton go_west;
    finalcut::FButton go_north;
};
#pragma pack(pop)

//----------------------------------------------------------------------
Scrollview::Scrollview (finalcut::FWidget* parent)
  : finalcut::FScrollView(parent)
  , pointer_right(wchar_t(finalcut::fc::BlackRightPointingPointer))
  , pointer_down(wchar_t(finalcut::fc::BlackDownPointingTriangle))
  , pointer_left(wchar_t(finalcut::fc::BlackLeftPointingPointer))
  , pointer_up(wchar_t(finalcut::fc::BlackUpPointingTriangle))
  , go_east(pointer_right, this)
  , go_south(pointer_down, this)
  , go_west(pointer_left, this)
  , go_north(pointer_up, this)
{
  // Sets the navigation button geometry
  go_east.setGeometry (1, 1, 5, 1);
  go_south.setGeometry (int(getScrollWidth()) - 5, 1, 5, 1);
  go_west.setGeometry ( int(getScrollWidth()) - 5
                      , int(getScrollHeight()) - 2, 5, 1);
  go_north.setGeometry (1, int(getScrollHeight()) - 2, 5, 1);

  // Add scroll function callbacks to the buttons
  go_east.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &Scrollview::cb_go_east)
  );

  go_south.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &Scrollview::cb_go_south)
  );

  go_west.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &Scrollview::cb_go_west)
  );

  go_north.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &Scrollview::cb_go_north)
  );
}

//----------------------------------------------------------------------
Scrollview::~Scrollview()
{ }

//----------------------------------------------------------------------
void Scrollview::setScrollSize (std::size_t width, std::size_t height)
{
  FScrollView::setScrollSize (width, height);
  go_south.setPos (int(width) - 5, 1);
  go_west.setPos (int(width) - 5, int(height) - 1);
  go_north.setPos (1, int(height) - 1);
}

//----------------------------------------------------------------------
void Scrollview::draw()
{
  if ( isMonochron() )
    setReverse(true);

  setColor (wc.label_inactive_fg, wc.dialog_bg);
  clearArea();

  for (int y = 0; y < int(getScrollHeight()); y++)
  {
    setPrintPos (1, 1 + y);

    for (int x = 0; x < int(getScrollWidth()); x++)
      print (32 + ((x + y) % 0x5f));
  }

  if ( isMonochron() )
    setReverse(false);

  FScrollView::draw();
}

//----------------------------------------------------------------------
void Scrollview::cb_go_east (finalcut::FWidget*, data_ptr)
{
  scrollToX (int(getScrollWidth() - getViewportWidth()) + 1);
  go_south.setFocus();
  go_east.redraw();
  go_south.redraw();
}

//----------------------------------------------------------------------
void Scrollview::cb_go_south (finalcut::FWidget*, data_ptr)
{
  scrollToY (int(getScrollHeight() - getViewportHeight()) + 1);
  go_west.setFocus();
  go_south.redraw();
  go_west.redraw();
}

//----------------------------------------------------------------------
void Scrollview::cb_go_west (finalcut::FWidget*, data_ptr)
{
  scrollToX (1);
  go_north.setFocus();
  go_west.redraw();
  go_north.redraw();
}

//----------------------------------------------------------------------
void Scrollview::cb_go_north (finalcut::FWidget*, data_ptr)
{
  scrollToY (1);
  go_east.setFocus();
  go_north.redraw();
  go_east.redraw();
}


//----------------------------------------------------------------------
// class Scrollviewdemo
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class Scrollviewdemo : public finalcut::FDialog
{
  public:
    // Constructor
    explicit Scrollviewdemo (finalcut::FWidget* = 0);

    // Destructor
    ~Scrollviewdemo();

    // Event handler
    virtual void onClose (finalcut::FCloseEvent*);

    // Callback method
    void cb_quit (finalcut::FWidget* = 0, data_ptr = 0);

    // Data Members
    Scrollview sview;
    finalcut::FButton quit_btn;
    finalcut::FLabel label;
};
#pragma pack(pop)


//----------------------------------------------------------------------
Scrollviewdemo::Scrollviewdemo (finalcut::FWidget* parent)
  : finalcut::FDialog(parent)
  , sview(this)
  , quit_btn("&Quit", this)
  , label(this)
{
  setGeometry (16, 3, 50, 19);
  setText ("Scrolling viewport example");

  // The scrolling viewport widget
  sview.setGeometry(3, 2, 44, 12);
  sview.setScrollSize(188, 124);

  // Quit button
  quit_btn.setGeometry(37, 15, 10, 1);

  // Add function callback
  quit_btn.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &Scrollviewdemo::cb_quit)
  );

  // Text label
  label.setGeometry(2, 1, 46, 1);
  label.setEmphasis();
  label << L"Use scrollbars to change the viewport position";
}

//----------------------------------------------------------------------
Scrollviewdemo::~Scrollviewdemo()
{ }

//----------------------------------------------------------------------
void Scrollviewdemo::cb_quit (finalcut::FWidget*, data_ptr)
{
  close();
}

//----------------------------------------------------------------------
void Scrollviewdemo::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
{
  // Create the application object
  finalcut::FApplication app(argc, argv);

  // Create a simple dialog box
  Scrollviewdemo svdemo(&app);

  // Set dialog main_dlg as main widget
  app.setMainWidget(&svdemo);

  // Show and start the application
  svdemo.show();
  return app.exec();
}
