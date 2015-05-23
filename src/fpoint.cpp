// fpoint.cpp
// class FPoint

#include "fpoint.h"


//----------------------------------------------------------------------
// class FPoint
//----------------------------------------------------------------------

FPoint::~FPoint()  // destructor
{
}

// public methods of FPoint
//----------------------------------------------------------------------
bool FPoint::isNull() const
{
  return xpos == 0 && ypos == 0;
}

//----------------------------------------------------------------------
void FPoint::setX (int x)
{
  xpos = short(x);
}

//----------------------------------------------------------------------
void FPoint::setY (int y)
{
  ypos = short(y);
}

//----------------------------------------------------------------------
void FPoint::setPoint (int x, int y)
{
  xpos = short(x);
  ypos = short(y);
}

//----------------------------------------------------------------------
FPoint& FPoint::operator = (const FPoint& p)
{
  xpos = p.xpos;
  ypos = p.ypos;
  return *this;
}

//----------------------------------------------------------------------
FPoint& FPoint::operator += (const FPoint& p)
{
  xpos += p.xpos;
  ypos += p.ypos;
  return *this;
}

//----------------------------------------------------------------------
FPoint& FPoint::operator -= (const FPoint& p)
{
  xpos -= p.xpos;
  ypos -= p.ypos;
  return *this;
}
