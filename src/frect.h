// File: frect.h
// Provides: class FRect
//
//  Standalone class
//  ════════════════
//
//     ▕▔▔▔▔▔▔▔▏1     *▕▔▔▔▔▔▔▔▔▏
//     ▕ FRect ▏- - - -▕ FPoint ▏
//     ▕▁▁▁▁▁▁▁▏       ▕▁▁▁▁▁▁▁▁▏

#ifndef _FRECT_H
#define _FRECT_H

#include <algorithm>
#include "fpoint.h"


//----------------------------------------------------------------------
// class FRect
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FRect
{
 private:
   short X1;
   short Y1;
   short X2;
   short Y2;

 public:
   FRect ();  // constructor
   FRect (int, int, int, int);  // constructor
   FRect (const FPoint&, const FPoint&);  // constructor
   virtual ~FRect();  // destructor
   virtual const char* getClassName();

   bool  isNull() const;
   int   getX1() const;
   int   getY1() const;
   int   getX2() const;
   int   getY2() const;
   int   getX() const;
   int   getY() const;
   int   getWidth() const;
   int   getHeight() const;

   void  setX1 (int);
   void  setY1 (int);
   void  setX2 (int);
   void  setY2 (int);
   void  setX (int);
   void  setY (int);
   void  setWidth (int);
   void  setHeight (int);
   void  setRect (const FRect&);
   void  setRect (int, int, int, int);

   void  move (int, int);
   void  move (const FPoint&);
   bool  contains (int, int) const;
   bool  contains (const FPoint&) const;
   bool  contains (const FRect&) const;
   bool  overlap  (const FRect&) const;
   FRect intersect (const FRect&) const;

   friend FRect operator + (const FRect&, const FPoint&);
   friend FRect operator - (const FRect&, const FPoint&);
   friend bool operator == (const FRect&, const FRect&);
   friend bool operator != (const FRect&, const FRect&);
};
#pragma pack(pop)


// FRect inline functions
//----------------------------------------------------------------------
inline FRect::FRect()
  : X1(0)
  , Y1(0)
  , X2(-1)
  , Y2(-1)
{ }

//----------------------------------------------------------------------
inline FRect::FRect (int x, int y, int width, int height)
  : X1(short(x))
  , Y1(short(y))
  , X2(short(x+width-1))
  , Y2(short(y+height-1))
{ }

//----------------------------------------------------------------------
inline const char* FRect::getClassName()
{ return "FRect"; }

//----------------------------------------------------------------------
inline int FRect::getX1() const
{ return X1; }

//----------------------------------------------------------------------
inline int FRect::getY1() const
{ return Y1; }

//----------------------------------------------------------------------
inline int FRect::getX2() const
{ return X2; }

//----------------------------------------------------------------------
inline int FRect::getY2() const
{ return Y2; }

//----------------------------------------------------------------------
inline int FRect::getX() const
{ return X1; }

//----------------------------------------------------------------------
inline int FRect::getY() const
{ return Y1; }

//----------------------------------------------------------------------
inline int FRect::getWidth() const
{ return X2 - X1 + 1; }

//----------------------------------------------------------------------
inline int FRect::getHeight() const
{ return Y2 - Y1 + 1; }

#endif  // _FRECT_H
