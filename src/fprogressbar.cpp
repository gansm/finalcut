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
  , BarLength(width)
{
  unsetFocusable();
}

//----------------------------------------------------------------------
FProgressbar::~FProgressbar()
{ }

// private methods of FProgressbar
//----------------------------------------------------------------------
void FProgressbar::drawPercentage()
{
  setColor ( parentWidget()->getForegroundColor()
           , parentWidget()->getBackgroundColor() );
  if ( isMonochron() )
    setReverse(true);
  gotoxy (xpos+xmin+width-5, ypos+ymin-2);
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
  int i = 1;
  float length = float(BarLength*percentage)/100;
  
  gotoxy (xpos+xmin-1, ypos+ymin-1);
  if ( isMonochron() )
  {
    if ( round(length) >= 1)
    {
      setReverse(false);
      print (' ');
      setReverse(true);
    }
    else
      print (fc::MediumShade);  // ▒
  }
  else if ( getMaxColor() < 16 )
  {
    setColor ( wc.progressbar_bg
             , wc.progressbar_fg );
    if ( round(length) >= 1)
      print (' ');
    else
      print (fc::MediumShade);
  }

  if ( round(length) >= 1)
    setColor ( wc.progressbar_fg
             , parentWidget()->getBackgroundColor() );
  else
    setColor ( wc.progressbar_bg
             , parentWidget()->getBackgroundColor() );

  if ( ! isMonochron() &&  getMaxColor() >= 16 )
  {
    // Cygwin terminal use IBM Codepage 850
    if ( isCygwinTerminal() )
      print (fc::FullBlock); // █
    else if ( isTeraTerm() )
        print (0xdb);
    else
      print (fc::RightHalfBlock); // ▐
  }

  setColor ( wc.progressbar_bg
           , wc.progressbar_fg );
  if ( isMonochron() )
    setReverse(false);
  for (; i < trunc(length); i++)
    print (' ');
  if ( isMonochron() )
    setReverse(true);

  if ( trunc(length) >= 1 && trunc(length) < BarLength )
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
      setColor(wc.progressbar_fg, wc.progressbar_bg);
      print (fc::LeftHalfBlock); // ▌
    }
    i++;
  }

  setColor(wc.progressbar_fg, wc.progressbar_bg);
  if ( getMaxColor() < 16 )
  {
    for (; i < BarLength; i++)
      print (fc::MediumShade);  // ▒
  }
  else
  {
    for (; i < BarLength; i++)
      print (' ');
  }
  if ( isMonochron() )
    setReverse(false);

  updateTerminal();
  flush_out();
}


// protected methods of FProgressbar
//----------------------------------------------------------------------
void FProgressbar::draw()
{
  setUpdateVTerm(false);
  drawPercentage();
  drawBar();

  if ( (flags & SHADOW) != 0 )
    drawShadow ();
  setUpdateVTerm(true);
  flush_out();
}


// public methods of FProgressbar
//----------------------------------------------------------------------
void FProgressbar::hide()
{
  int fg, bg, s, size;
  char* blank;

  FWidget::hide();

  fg = parentWidget()->getForegroundColor();
  bg = parentWidget()->getBackgroundColor();
  setColor (fg, bg);

  s = hasShadow() ? 1 : 0;
  size = width + s;
  blank = new char[size+1];
  memset(blank, ' ', uLong(size));
  blank[size] = '\0';

  for (int y=0; y < height+s; y++)
  {
    gotoxy (xpos+xmin-1, ypos+ymin-1+y);
    print (blank);
  }
  delete[] blank;

  gotoxy (xpos+xmin+width-5, ypos+ymin-2);
  print ("     ");  // hide percentage
}

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

  setUpdateVTerm(false);
  if ( isVisible() )
  {
    drawPercentage();
    drawBar();
  }
  setUpdateVTerm(true);
  updateTerminal();
}

//----------------------------------------------------------------------
void FProgressbar::reset()
{
  setUpdateVTerm(false);
  percentage = -1;
  if ( isVisible() )
  {
    drawPercentage();
    drawBar();
  }
  setUpdateVTerm(true);
  updateTerminal();
}

//----------------------------------------------------------------------
void FProgressbar::setGeometry (int x, int y, int w, int h, bool adjust)
{
  FWidget::setGeometry (x, y, w, h, adjust);
  BarLength = w;
}

//----------------------------------------------------------------------
bool FProgressbar::setEnable (bool on)
{
  FWidget::setEnable(on);

  if ( on )
    flags |= ACTIVE;
  else
    flags &= ~ACTIVE;
  return on;
}

//----------------------------------------------------------------------
bool FProgressbar::setShadow (bool on)
{
  if (  on
     && (Encoding != fc::VT100 || isTeraTerm() )
     && Encoding != fc::ASCII )
    flags |= SHADOW;
  else
    flags &= ~SHADOW;
  return on;
}
