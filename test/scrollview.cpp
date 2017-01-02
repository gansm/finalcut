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

 private:
   // Methods
   void draw();
};
#pragma pack(pop)

//----------------------------------------------------------------------
scrollview::scrollview (FWidget* parent)
  : FScrollView(parent)
{ }

//----------------------------------------------------------------------
scrollview::~scrollview()
{ }

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
  sview->setScrollSize(88, 24);

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
