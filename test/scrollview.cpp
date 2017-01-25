// File: scrollview.cpp

#include "fapp.h"
#include "fbutton.h"
#include "fdialog.h"
#include "flabel.h"
#include "fmessagebox.h"
#include "fscrollview.h"

//----------------------------------------------------------------------
// class scrollview
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class scrollview : public FScrollView
{
 public:
   // Constructor
   explicit scrollview (FWidget* = 0);

   // Destructor
  ~scrollview  ();

   // Mutator
   void setScrollSize (int, int);

 private:
   // Method
   void draw();

   // Callback methods
   void cb_go_east (FWidget*, void*);
   void cb_go_south (FWidget*, void*);
   void cb_go_west (FWidget*, void*);
   void cb_go_north (FWidget*, void*);

   // Data Members
   FButton* go_east;
   FButton* go_south;
   FButton* go_west;
   FButton* go_north;
};
#pragma pack(pop)

//----------------------------------------------------------------------
scrollview::scrollview (FWidget* parent)
  : FScrollView(parent)
{
  go_east = new FButton(wchar_t(fc::BlackRightPointingPointer) , this);
  go_east->setGeometry (1, 1, 5, 1);

  go_south = new FButton(wchar_t(fc::BlackDownPointingTriangle) , this);
  go_south->setGeometry (getScrollWidth() - 5, 1, 5, 1);

  go_west = new FButton(wchar_t(fc::BlackLeftPointingPointer) , this);
  go_west->setGeometry (getScrollWidth() - 5, getScrollHeight() - 2, 5, 1);

  go_north = new FButton(wchar_t(fc::BlackUpPointingTriangle) , this);
  go_north->setGeometry (1, getScrollHeight() - 2, 5, 1);

  if ( isCygwinTerminal() )
  {
    go_south->setText ('v');
    go_north->setText ('^');
  }

  go_east->addCallback
  (
    "clicked",
    _METHOD_CALLBACK (this, &scrollview::cb_go_east)
  );

  go_south->addCallback
  (
    "clicked",
    _METHOD_CALLBACK (this, &scrollview::cb_go_south)
  );

  go_west->addCallback
  (
    "clicked",
    _METHOD_CALLBACK (this, &scrollview::cb_go_west)
  );

  go_north->addCallback
  (
    "clicked",
    _METHOD_CALLBACK (this, &scrollview::cb_go_north)
  );
}

//----------------------------------------------------------------------
scrollview::~scrollview()
{ }

//----------------------------------------------------------------------
void scrollview::setScrollSize (int width, int height)
{
  FScrollView::setScrollSize (width, height);
  go_south->setPos (width - 5, 1);
  go_west->setPos (width - 5, height - 1);
  go_north->setPos (1, height - 1);
}

//----------------------------------------------------------------------
void scrollview::draw()
{
  setColor (wc.dialog_fg, wc.dialog_bg);
  clearArea();

  for (int y=0; y < getScrollHeight(); y++)
  {
    setPrintPos (1, 1 + y);

    for (int x=0; x < getScrollWidth(); x++)
      print (32 + ((x + y) % 0x5f));
  }

  FScrollView::draw();
}

//----------------------------------------------------------------------
void scrollview::cb_go_east (FWidget*, void*)
{
  scrollToX (getScrollWidth() - getViewportWidth() + 1);
  go_south->setFocus();
  go_east->redraw();
  go_south->redraw();
}

//----------------------------------------------------------------------
void scrollview::cb_go_south (FWidget*, void*)
{
  scrollToY (getScrollHeight() - getViewportHeight() + 1);
  go_west->setFocus();
  go_south->redraw();
  go_west->redraw();
}

//----------------------------------------------------------------------
void scrollview::cb_go_west (FWidget*, void*)
{
  scrollToX (1);
  go_north->setFocus();
  go_west->redraw();
  go_north->redraw();}

//----------------------------------------------------------------------
void scrollview::cb_go_north (FWidget*, void*)
{
  scrollToY (1);
  go_east->setFocus();
  go_north->redraw();
  go_east->redraw();
}


//----------------------------------------------------------------------
// class scrollviewdemo
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class scrollviewdemo : public FDialog
{
 public:
   // Constructor
   explicit scrollviewdemo (FWidget* = 0);

   // Destructor
  ~scrollviewdemo  ();

   // Event handler
   void onClose (FCloseEvent*);

   // Callback method
   void cb_quit (FWidget* = 0, void* = 0);
};
#pragma pack(pop)


//----------------------------------------------------------------------
scrollviewdemo::scrollviewdemo (FWidget* parent)
  : FDialog(parent)
{
  setGeometry (16, 3, 50, 19);
  setText ("Scrolling viewport example");

  // The scrolling viewport widget
  scrollview* sview = new scrollview (this);
  sview->setGeometry(3, 2, 44, 12);
  sview->setScrollSize(188, 124);

  // Quit button
  FButton* button = new FButton("&Quit", this);
  button->setGeometry(37, 15, 10, 1);

  // Add function callback
  button->addCallback
  (
    "clicked",
    _METHOD_CALLBACK (this, &scrollviewdemo::cb_quit)
  );

  // Text label
  FLabel* label = new FLabel (this);
  label->setGeometry(2, 1, 46, 1);
  label->setText (L"Use scrollbars to change the viewport position");
  label->setEmphasis();
}

//----------------------------------------------------------------------
scrollviewdemo::~scrollviewdemo()
{ }

//----------------------------------------------------------------------
void scrollviewdemo::cb_quit (FWidget*, void*)
{
  close();
}

//----------------------------------------------------------------------
void scrollviewdemo::onClose (FCloseEvent* ev)
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
//                               main part
//----------------------------------------------------------------------
int main (int argc, char* argv[])
{
  // Create the application object
  FApplication app(argc, argv);

  // Create a simple dialog box
  scrollviewdemo svdemo(&app);

  app.setMainWidget(&svdemo);
  svdemo.show();
  return app.exec();
}
