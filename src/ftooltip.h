// File: ftooltip.h
// Provides: class FTooltip
//
//  Inheritance diagram
//  ═══════════════════
//
// ▕▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▏
// ▕ FObject ▏ ▕  FTerm  ▏
// ▕▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▏
//      ▲           ▲
//      │           │
//      └─────┬─────┘
//            │
//       ▕▔▔▔▔▔▔▔▔▔▏
//       ▕ FWidget ▏
//       ▕▁▁▁▁▁▁▁▁▁▏
//            ▲
//            │
//       ▕▔▔▔▔▔▔▔▔▔▏
//       ▕ FWindow ▏
//       ▕▁▁▁▁▁▁▁▁▁▏
//            ▲
//            │
//       ▕▔▔▔▔▔▔▔▔▔▔▏
//       ▕ FToolTip ▏
//       ▕▁▁▁▁▁▁▁▁▁▁▏

#ifndef _FTOOLTIP_H
#define _FTOOLTIP_H

#include "fwindow.h"


//----------------------------------------------------------------------
// class FToolTip
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FToolTip : public FWindow
{
 private:
   FString  text;
   FString* text_components;
   std::vector<FString> text_split;
   uInt     max_line_width;
   uInt     text_num_lines;

 private:
   // Disable copy constructor
   FToolTip (const FToolTip&);
   // Disable assignment operator (=)
   FToolTip& operator = (const FToolTip&);

   void            init();
   void            calculateDimensions();
   virtual void    adjustSize();

 public:
   // Constructor
   explicit FToolTip (FWidget* = 0);
   FToolTip (const FString&, FWidget* = 0);
   // Destructor
   virtual ~FToolTip ();

   const char*   getClassName() const;
   virtual void  draw();
   void          show();
   void          hide();
   // Event handler
   void          onMouseDown (FMouseEvent*);
   const FString getText() const;
   void          setText (const FString&);
   void          setText (const std::string&);
   void          setText (const char*);
};
#pragma pack(pop)


// FToolTip inline functions
//----------------------------------------------------------------------
inline const char* FToolTip::getClassName() const
{ return "FToolTip"; }


#endif  // _FTOOLTIP_H
