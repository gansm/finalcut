/***********************************************************************
* scrollview.cpp - Shows client widgets in a scroll area               *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2017-2020 Markus Gans                                      *
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
// class Scrollview
//----------------------------------------------------------------------

class Scrollview : public finalcut::FScrollView
{
  public:
    // Constructor
    explicit Scrollview (finalcut::FWidget* = nullptr);

    // Disable copy constructor
    Scrollview (const Scrollview&) = delete;

    // Destructor
    ~Scrollview() override;

    // Disable assignment operator (=)
    Scrollview& operator = (const Scrollview&) = delete;

    // Mutator
    void setScrollSize (const FSize&) override;

  private:
    // Method
    void draw() override;

    // Callback methods
    void cb_goEast (const finalcut::FWidget*, const FDataPtr);
    void cb_goSouth (const finalcut::FWidget*, const FDataPtr);
    void cb_goWest (const finalcut::FWidget*, const FDataPtr);
    void cb_goNorth (const finalcut::FWidget*, const FDataPtr);

    // Data members
    wchar_t pointer_right{fc::BlackRightPointingPointer};
    wchar_t pointer_down{fc::BlackDownPointingTriangle};
    wchar_t pointer_left{fc::BlackLeftPointingPointer};
    wchar_t pointer_up{fc::BlackUpPointingTriangle};
    finalcut::FButton go_east{pointer_right, this};
    finalcut::FButton go_south{pointer_down, this};
    finalcut::FButton go_west{pointer_left, this};
    finalcut::FButton go_north{pointer_up, this};
};

//----------------------------------------------------------------------
Scrollview::Scrollview (finalcut::FWidget* parent)
  : finalcut::FScrollView(parent)
{
  // Sets the navigation button geometry
  go_east.setGeometry (FPoint(1, 1), FSize(5, 1));
  go_south.setGeometry ( FPoint(int(getScrollWidth()) - 5, 1)
                       , FSize(5, 1) );
  go_west.setGeometry ( FPoint( int(getScrollWidth()) - 5
                              , int(getScrollHeight()) - 2 )
                      , FSize(5, 1) );
  go_north.setGeometry ( FPoint(1, int(getScrollHeight()) - 2)
                       , FSize(5, 1) );

  // Add scroll function callbacks to the buttons
  go_east.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &Scrollview::cb_goEast)
  );

  go_south.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &Scrollview::cb_goSouth)
  );

  go_west.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &Scrollview::cb_goWest)
  );

  go_north.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &Scrollview::cb_goNorth)
  );
}

//----------------------------------------------------------------------
Scrollview::~Scrollview()
{ }

//----------------------------------------------------------------------
void Scrollview::setScrollSize (const FSize& size)
{
  FScrollView::setScrollSize (size);
  const int width = int(size.getWidth());
  const int height = int(size.getHeight());
  go_south.setPos (FPoint(width - 5, 1));
  go_west.setPos (FPoint(width - 5, height - 1));
  go_north.setPos (FPoint(1, height - 1));
}

//----------------------------------------------------------------------
void Scrollview::draw()
{
  if ( isMonochron() )
    setReverse(true);

  const auto& wc = getFWidgetColors();
  setColor (wc.label_inactive_fg, wc.dialog_bg);
  clearArea();

  for (int y{0}; y < int(getScrollHeight()); y++)
  {
    print() << FPoint(1, 1 + y);

    for (int x{0}; x < int(getScrollWidth()); x++)
      print (32 + ((x + y) % 0x5f));
  }

  if ( isMonochron() )
    setReverse(false);

  FScrollView::draw();
}

//----------------------------------------------------------------------
void Scrollview::cb_goEast (const finalcut::FWidget*, const FDataPtr)
{
  scrollToX (int(getScrollWidth() - getViewportWidth()) + 1);
  go_south.setFocus();
  go_east.redraw();
  go_south.redraw();
}

//----------------------------------------------------------------------
void Scrollview::cb_goSouth (const finalcut::FWidget*, const FDataPtr)
{
  scrollToY (int(getScrollHeight() - getViewportHeight()) + 1);
  go_west.setFocus();
  go_south.redraw();
  go_west.redraw();
}

//----------------------------------------------------------------------
void Scrollview::cb_goWest (const finalcut::FWidget*, const FDataPtr)
{
  scrollToX (1);
  go_north.setFocus();
  go_west.redraw();
  go_north.redraw();
}

//----------------------------------------------------------------------
void Scrollview::cb_goNorth (const finalcut::FWidget*, const FDataPtr)
{
  scrollToY (1);
  go_east.setFocus();
  go_north.redraw();
  go_east.redraw();
}


//----------------------------------------------------------------------
// class Scrollviewdemo
//----------------------------------------------------------------------

class Scrollviewdemo : public finalcut::FDialog
{
  public:
    // Constructor
    explicit Scrollviewdemo (finalcut::FWidget* = nullptr);

    // Destructor
    ~Scrollviewdemo() override;

    // Event handler
    void onClose (finalcut::FCloseEvent*) override;

    // Callback method
    void cb_quit (const finalcut::FWidget* = nullptr, const FDataPtr = nullptr);

    // Data members
    Scrollview sview{this};
    finalcut::FButton quit_btn{"&Quit", this};
    finalcut::FLabel label{this};
};


//----------------------------------------------------------------------
Scrollviewdemo::Scrollviewdemo (finalcut::FWidget* parent)
  : finalcut::FDialog(parent)
{
  setGeometry (FPoint(16, 3), FSize(50, 19));
  setText ("Scrolling viewport example");

  // The scrolling viewport widget
  sview.setGeometry(FPoint(3, 2), FSize(44, 12));
  sview.setScrollSize(FSize(188, 124));

  // Quit button
  quit_btn.setGeometry(FPoint(37, 15), FSize(10, 1));

  // Add function callback
  quit_btn.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &Scrollviewdemo::cb_quit)
  );

  // Text label
  label.setGeometry(FPoint(2, 1), FSize(46, 1));
  label.setEmphasis();
  label << L"Use scrollbars to change the viewport position";
}

//----------------------------------------------------------------------
Scrollviewdemo::~Scrollviewdemo()
{ }

//----------------------------------------------------------------------
void Scrollviewdemo::cb_quit (const finalcut::FWidget*, const FDataPtr)
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
  finalcut::FWidget::setMainWidget(&svdemo);

  // Show and start the application
  svdemo.show();
  return app.exec();
}
