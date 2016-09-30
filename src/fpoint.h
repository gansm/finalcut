// File: fpoint.h
// Provides: class FPoint
//
//  Standalone class
//  ════════════════
//
// ▕▔▔▔▔▔▔▔▔▏
// ▕ FPoint ▏
// ▕▁▁▁▁▁▁▁▁▏

#ifndef _FPOINT_H
#define _FPOINT_H

//----------------------------------------------------------------------
// class FPoint
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FPoint
{
 private:
   short xpos;
   short ypos;

 public:
   // Constructors
   FPoint ();
   FPoint (int, int);
   // Destructor
   virtual ~FPoint();

   virtual const char* getClassName();

   bool    isNull() const;
   int     getX() const;
   int     getY() const;
   short&  x_ref();
   short&  y_ref();
   void    setX (int);
   void    setY (int);
   void    setPoint (int, int);

   FPoint& operator = (const FPoint&);
   FPoint& operator += (const FPoint&);
   FPoint& operator -= (const FPoint&);

   friend inline bool   operator == (const FPoint&, const FPoint&);
   friend inline bool   operator != (const FPoint&, const FPoint&);
   friend inline FPoint operator +  (const FPoint&, const FPoint&);
   friend inline FPoint operator -  (const FPoint&, const FPoint&);
   friend inline FPoint operator -  (const FPoint&);
};
#pragma pack(pop)


// FPoint inline functions
//----------------------------------------------------------------------
inline FPoint::FPoint()
  : xpos(0)
  , ypos(0)
{ }

//----------------------------------------------------------------------
inline FPoint::FPoint (int x, int y)
  : xpos(short(x))
  , ypos(short(y))
{ }

//----------------------------------------------------------------------
inline const char* FPoint::getClassName()
{ return "FPoint"; }

//----------------------------------------------------------------------
inline int FPoint::getX() const
{ return xpos; }

//----------------------------------------------------------------------
inline int FPoint::getY() const
{ return ypos; }

//----------------------------------------------------------------------
inline short& FPoint::x_ref()
{ return xpos; }

//----------------------------------------------------------------------
inline short& FPoint::y_ref()
{ return ypos; }

//----------------------------------------------------------------------
inline bool operator == (const FPoint& p1, const FPoint& p2)
{ return p1.xpos == p2.xpos && p1.ypos == p2.ypos; }

//----------------------------------------------------------------------
inline bool operator != (const FPoint& p1, const FPoint& p2)
{ return p1.xpos != p2.xpos || p1.ypos != p2.ypos; }

//----------------------------------------------------------------------
inline FPoint operator + (const FPoint& p1, const FPoint& p2)
{ return FPoint(p1.xpos+p2.xpos, p1.ypos+p2.ypos); }

//----------------------------------------------------------------------
inline FPoint operator - (const FPoint& p1, const FPoint& p2)
{ return FPoint(p1.xpos-p2.xpos, p1.ypos-p2.ypos); }

//----------------------------------------------------------------------
inline FPoint operator - (const FPoint& p)
{ return FPoint(-p.xpos, -p.ypos); }

#endif  // _FPOINT_H
