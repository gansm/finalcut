// File: fprogressbar.cpp
// Provides: class FProgressbar

#include "fprogressbar.h"

//----------------------------------------------------------------------
// class FProgressbar
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FProgressbar::FProgressbar(FWidget* parent)
  : FWidget(parent)
  , percentage(-1)
  , bar_length(getWidth())
{
  unsetFocusable();
  setShadow();
}

//----------------------------------------------------------------------
FProgressbar::~FProgressbar()
{ }


// public methods of FProgressbar
//----------------------------------------------------------------------
void FProgressbar::setPercentage (int percentage_value)
{
  if ( percentage_value <= percentage )
    return;

  if ( percentage_value > 100 )
    percentage = 100;
  else if ( percentage_value < 0 )
    percentage = 0;
  else
    percentage = percentage_value;

  updateVTerm(false);

  if ( isVisible() )
  {
    drawPercentage();
    drawBar();
  }

  updateVTerm(true);
  updateTerminal();
}

//----------------------------------------------------------------------
void FProgressbar::setGeometry (int x, int y, int w, int h, bool adjust)
{
  FWidget::setGeometry (x, y, w, h, adjust);
  bar_length = w;
}

//----------------------------------------------------------------------
bool FProgressbar::setEnable (bool on)
{
  FWidget::setEnable(on);

  if ( on )
    flags |= fc::active;
  else
    flags &= ~fc::active;

  return on;
}

//----------------------------------------------------------------------
bool FProgressbar::setShadow (bool on)
{
  if (  on
     && (Encoding != fc::VT100 || isTeraTerm() )
     && Encoding != fc::ASCII )
    flags |= fc::shadow;
  else
    flags &= ~fc::shadow;

  return on;
}

//----------------------------------------------------------------------
void FProgressbar::hide()
{
  int s, size;
  short fg, bg;
  char* blank;
  FWidget* parent_widget = getParentWidget();

  FWidget::hide();

  if ( parent_widget )
  {
    fg = parent_widget->getForegroundColor();
    bg = parent_widget->getBackgroundColor();
  }
  else
  {
    fg = wc.dialog_fg;
    bg = wc.dialog_bg;
  }

  setColor (fg, bg);
  s = hasShadow() ? 1 : 0;
  size = getWidth() + s;

  if ( size < 0 )
    return;

  blank = new char[size+1];
  std::memset(blank, ' ', uLong(size));
  blank[size] = '\0';

  for (int y=0; y < getHeight()+s; y++)
  {
    setPrintPos (1, 1 + y);
    print (blank);
  }

  delete[] blank;
  setPrintPos (getWidth() - 4, 0);
  print ("     ");  // hide percentage
}

//----------------------------------------------------------------------
void FProgressbar::reset()
{
  updateVTerm(false);
  percentage = -1;

  if ( isVisible() )
  {
    drawPercentage();
    drawBar();
  }

  updateVTerm(true);
  updateTerminal();
}


// private methods of FProgressbar
//----------------------------------------------------------------------
void FProgressbar::draw()
{
  updateVTerm(false);
  drawPercentage();
  drawBar();

  if ( (flags & fc::shadow) != 0 )
    drawShadow ();

  updateVTerm(true);
  flush_out();
}

//----------------------------------------------------------------------
void FProgressbar::drawPercentage()
{
  FWidget* parent_widget = getParentWidget();

  if ( parent_widget )
    setColor ( parent_widget->getForegroundColor()
             , parent_widget->getBackgroundColor() );
  else
    setColor ( wc.dialog_fg, wc.dialog_bg );

  if ( isMonochron() )
    setReverse(true);

  setPrintPos (getWidth() - 3, 0);

  if ( percentage < 0 || percentage > 100 )
    print ("--- %");
  else
    printf ("%3d %%", percentage);

  if ( isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
void FProgressbar::drawBar()
{
  int i = 0;
  float length = float(bar_length * percentage) / 100;
  setPrintPos (1,1);
  setColor ( wc.progressbar_bg
           , wc.progressbar_fg );

  if ( isMonochron() )
    setReverse(false);

  for (; i < trunc(length); i++)
    print (' ');

  if ( isMonochron() )
    setReverse(true);

  if ( percentage > 0.0f && trunc(length) < bar_length )
  {
    if (  round(length) > trunc(length)
       || isCygwinTerminal()
       || getMaxColor() < 16 )
    {
      if ( isMonochron() )
      {
        setReverse(false);
        print (' ');
        setReverse(true);
      }
      else
        print (' ');
    }
    else
    {
      setColor (wc.progressbar_fg, wc.progressbar_bg);
      print (fc::LeftHalfBlock); // ▌
    }

    i++;
  }

  setColor (wc.progressbar_fg, wc.progressbar_bg);

  if ( getMaxColor() < 16 )
  {
    for (; i < bar_length; i++)
      print (fc::MediumShade);  // ▒
  }
  else
  {
    for (; i < bar_length; i++)
      print (' ');
  }

  if ( isMonochron() )
    setReverse(false);

  updateTerminal();
  flush_out();
}
