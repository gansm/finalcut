// fprogressbar.h
// class FProgressbar

#ifndef _FPROGRESSBAR_H
#define _FPROGRESSBAR_H

#include "fwidget.h"


//----------------------------------------------------------------------
// class FProgressbar
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FProgressbar : public FWidget
{
 private:
   int  percentage;
   int  BarLength;

 private:
   void init();
   void drawPercentage();
   void drawBar();

 protected:
   virtual void draw();

 public:
   explicit FProgressbar(FWidget* parent=0);  // constructor
   virtual ~FProgressbar();
   const char* getClassName() const;

   void hide();

   int  getPercentage();
   void setPercentage (int);
   void reset();
   void setGeometry (int, int, int, int, bool adjust=true);
   bool setEnable (bool);
   bool setEnable();
   bool unsetEnable();
   bool setDisable();
   bool setShadow (bool);
   bool setShadow();
   bool unsetShadow();
   bool hasShadow();
};
#pragma pack(pop)


// FProgressbar inline functions
//----------------------------------------------------------------------
inline const char* FProgressbar::getClassName() const
{ return "FProgressbar"; }

//----------------------------------------------------------------------
inline int FProgressbar::getPercentage()
{ return percentage; }

//----------------------------------------------------------------------
inline bool FProgressbar::setEnable()
{ return setEnable(true); }

//----------------------------------------------------------------------
inline bool FProgressbar::unsetEnable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FProgressbar::setDisable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FProgressbar::setShadow()
{ return setShadow(true); }

//----------------------------------------------------------------------
inline bool FProgressbar::unsetShadow()
{ return setShadow(false); }

//----------------------------------------------------------------------
inline bool FProgressbar::hasShadow()
{ return ((flags & SHADOW) != 0); }


#endif  // _FPROGRESSBAR_H
