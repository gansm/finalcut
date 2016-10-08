// File: ftooltip.cpp
// Provides: class FToolTip

#include "fapp.h"
#include "ftooltip.h"


//----------------------------------------------------------------------
// class FToolTip
//----------------------------------------------------------------------

// constructor and destructor
//----------------------------------------------------------------------
FToolTip::FToolTip (FWidget* parent)
  : FWindow(parent)
  , text()
  , text_components(0)
  , text_split()
  , max_line_width(0)
  , text_num_lines(0)
{
  init();
}

//----------------------------------------------------------------------
FToolTip::FToolTip (const FString& txt, FWidget* parent)
  : FWindow(parent)
  , text(txt)
  , text_components(0)
  , text_split()
  , max_line_width(0)
  , text_num_lines(0)
{
  init();
}

//----------------------------------------------------------------------
FToolTip::~FToolTip()  // destructor
{
  FApplication* fapp = static_cast<FApplication*>(getRootWidget());

  if ( ! fapp->isQuit() )
  {
    FWindow* parent_win = 0;

    if ( FWidget* parent = getParentWidget() )
      parent_win = getWindowWidget(parent);

    if ( parent_win )
      setActiveWindow (parent_win);
    else
      switchToPrevWindow();
  }

  delWindow(this);

  if ( ! fapp->isQuit() )
  {
    const FRect& t_geometry = getTermGeometryWithShadow();
    restoreVTerm (t_geometry);
  }

  removeArea (vwin);
}


// private methods of FToolTip
//----------------------------------------------------------------------
void FToolTip::init()
{
  setAlwaysOnTop();
  ignorePadding();
  // initialize geometry values
  setGeometry (1, 1, 3, 3, false);
  setMinimumSize (3, 3);
  createArea (vwin);
  addWindow(this);
  setForegroundColor (wc.tooltip_fg);
  setBackgroundColor (wc.tooltip_bg);
  calculateDimensions();
}

//----------------------------------------------------------------------
void FToolTip::calculateDimensions()
{
  int x, y, w, h;
  FWidget* r = getRootWidget();
  text_split = text.split("\n");
  text_num_lines = uInt(text_split.size());
  text_components = &text_split[0];
  max_line_width = 0;

  for (uInt i=0; i < text_num_lines; i++)
  {
    uInt len = text_components[i].getLength();

    if ( len > max_line_width )
      max_line_width = len;
  }

  h = int(text_num_lines) + 2 ;
  w = int(max_line_width + 4);

  if ( r )
  {
    x = 1 + int((r->getWidth()-w)/2);
    y = 1 + int((r->getHeight()-h)/2);
  }
  else
    x = y = 1;

  setGeometry (x, y, w, h);
}

//----------------------------------------------------------------------
void FToolTip::adjustSize()
{
  calculateDimensions();
  FWindow::adjustSize();
}


// public methods of FToolTip
//----------------------------------------------------------------------
void FToolTip::draw()
{
  updateVTerm(false);
  setColor();

  if ( getMaxColor() < 16 )
    setBold();

  clearArea (vwin);
  drawBorder();

  for (int i=0; i < int(text_num_lines); i++)
  {
    printPos (3, 2 + i);
    print(text_components[i]);
  }

  unsetBold();
  updateVTerm(true);
}

//----------------------------------------------------------------------
void FToolTip::show()
{
  if ( ! isVisible() )
    return;

  FWindow::show();
}

//----------------------------------------------------------------------
void FToolTip::hide()
{
  FWindow::hide();
}

//----------------------------------------------------------------------
void FToolTip::onMouseDown (FMouseEvent*)
{
  setClickedWidget(0);
  close();
}

//----------------------------------------------------------------------
void FToolTip::setText (const FString& txt)
{
  text = txt;
  calculateDimensions();
}

//----------------------------------------------------------------------
void FToolTip::setText (const std::string& txt)
{
  FString message_text(txt);
  setText( message_text );
}

//----------------------------------------------------------------------
void FToolTip::setText (const char* txt)
{
  FString message_text(txt);
  setText( message_text );
}
