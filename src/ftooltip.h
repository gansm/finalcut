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
//       ▕▔▔▔▔▔▔▔▔▏
//       ▕ FVTerm ▏
//       ▕▁▁▁▁▁▁▁▁▏
//            ▲
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
 public:
   // Constructor
   explicit FToolTip (FWidget* = 0);
   FToolTip (const FString&, FWidget* = 0);

   // Destructor
   virtual ~FToolTip ();

   // Accessors
   const char*   getClassName() const;
   const FString getText() const;

   // Mutators
   void          setText (const FString&);
   void          setText (const std::string&);
   void          setText (const char*);

   // Methods
   virtual void  draw();
   void          show();
   void          hide();

   // Event handler
   void          onMouseDown (FMouseEvent*);

 private:
   // Typedef
   typedef std::vector<FString> textLines;

   // Disable copy constructor
   FToolTip (const FToolTip&);

   // Disable assignment operator (=)
   FToolTip& operator = (const FToolTip&);

   // Methods
   void          init();
   void          calculateDimensions();
   virtual void  adjustSize();

   // Data Members
   FString       text;
   FString*      text_components;
   textLines     text_split;
   uInt          max_line_width;
   uInt          text_num_lines;
};
#pragma pack(pop)


// FToolTip inline functions
//----------------------------------------------------------------------
inline const char* FToolTip::getClassName() const
{ return "FToolTip"; }


#endif  // _FTOOLTIP_H
