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
 public:
   // Constructors
   FRect ();
   FRect (int, int, int, int);
   FRect (const FPoint&, const FPoint&);

   // Destructor
   virtual ~FRect();

   // Overloaded operators
   FRect& operator = (const FRect&);

   friend FRect operator +  (const FRect&, const FPoint&);
   friend FRect operator -  (const FRect&, const FPoint&);
   friend bool  operator == (const FRect&, const FRect&);
   friend bool  operator != (const FRect&, const FRect&);

   // Accessors
   virtual const char* getClassName();
   int     getX1() const;
   int     getY1() const;
   int     getX2() const;
   int     getY2() const;
   int     getX() const;
   int     getY() const;
   FPoint  getPos() const;
   FPoint  getUpperLeftPos() const;
   FPoint  getUpperRightPos() const;
   FPoint  getLowerLeftPos() const;
   FPoint  getLowerRightPos() const;
   int     getWidth() const;
   int     getHeight() const;

   // Mutators
   void    setX1 (int);
   void    setY1 (int);
   void    setX2 (int);
   void    setY2 (int);
   void    setX (int);
   void    setY (int);
   void    setPos (int, int);
   void    setPos (const FPoint&);
   void    setWidth (int);
   void    setHeight (int);
   void    setSize (int, int);
   void    setRect (const FRect&);
   void    setRect (int, int, int, int);
   void    setCoordinates (const FPoint&, const FPoint&);
   void    setCoordinates (int, int, int, int);

   // Inquiry
   bool    isNull() const;

   // Coordinate references
   short&  x1_ref();
   short&  y1_ref();
   short&  x2_ref();
   short&  y2_ref();

   // Methods
   void    move (int, int);
   void    move (const FPoint&);
   bool    contains (int, int) const;
   bool    contains (const FPoint&) const;
   bool    contains (const FRect&) const;
   bool    overlap  (const FRect&) const;
   FRect   intersect (const FRect&) const;
   FRect   combined (const FRect&) const;

 private:
   // Data Members
   short X1;
   short Y1;
   short X2;
   short Y2;
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
  , X2(short(x + width - 1))
  , Y2(short(y + height - 1))
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
inline FPoint FRect::getPos() const
{ return FPoint(X1,Y1); }

//----------------------------------------------------------------------
inline FPoint FRect::getUpperLeftPos() const
{ return FPoint(X1,Y1); }

//----------------------------------------------------------------------
inline FPoint FRect::getUpperRightPos() const
{ return FPoint(X2,Y1); }

//----------------------------------------------------------------------
inline FPoint FRect::getLowerLeftPos() const
{ return FPoint(X1,Y2); }

//----------------------------------------------------------------------
inline FPoint FRect::getLowerRightPos() const
{ return FPoint(X2,Y2); }

//----------------------------------------------------------------------
inline int FRect::getWidth() const
{ return short(X2 - X1 + 1); }

//----------------------------------------------------------------------
inline int FRect::getHeight() const
{ return short(Y2 - Y1 + 1); }

//----------------------------------------------------------------------
inline short& FRect::x1_ref()
{ return X1; }

//----------------------------------------------------------------------
inline short& FRect::y1_ref()
{ return Y1; }

//----------------------------------------------------------------------
inline short& FRect::x2_ref()
{ return X2; }

//----------------------------------------------------------------------
inline short& FRect::y2_ref()
{ return Y2; }

#endif  // _FRECT_H
