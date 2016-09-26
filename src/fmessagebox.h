// File: fmessagebox.h
// Provides: class FMessageBox
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
//       ▕▔▔▔▔▔▔▔▔▔▏
//       ▕ FDialog ▏
//       ▕▁▁▁▁▁▁▁▁▁▏
//            ▲
//            │
//     ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
//     ▕ FMessageBox ▏
//     ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏

#ifndef _FMESSAGEBOX_H
#define _FMESSAGEBOX_H

#include <cstring>

#include "fbutton.h"
#include "fdialog.h"
#include "fterm.h"


//----------------------------------------------------------------------
// class FMessageBox
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FMessageBox : public FDialog
{
 public:
   enum
   {
     Reject = 0,
     Ok     = 1,
     Cancel = 2,
     Yes    = 3,
     No     = 4,
     Abort  = 5,
     Retry  = 6,
     Ignore = 7
   };

 private:
   FString  headline_text;
   FString  text;
   FString* text_components;
   std::vector<FString> text_split;
   uInt     max_line_width;
   bool     center_text;
   short    emphasis_color;
   uInt     num_buttons;
   uInt     text_num_lines;
   int*     button_digit[3];
   FButton* button[3];

 protected:
   void adjustSize();

 private:
   void init(int, int, int);
   void calculateDimensions();
   virtual void draw();
   void resizeButtons();
   void adjustButtons();
   void cb_processClick (FWidget*, void*);

 public:
   explicit FMessageBox (FWidget* = 0);
   FMessageBox (const FMessageBox&);       // copy constructor
   FMessageBox ( const FString&, const FString&
               , int, int, int
               , FWidget* = 0 );
  ~FMessageBox();
   FMessageBox& operator = (const FMessageBox&); // assignment
   const char* getClassName() const;

   const FString getTitlebarText() const;
   void  setTitlebarText (const FString&);

   const FString getHeadline() const;
   void  setHeadline (const FString&);
   void  setHeadline (const std::string&);
   void  setHeadline (const char*);

   const FString getText() const;
   void  setText (const FString&);
   void  setText (const std::string&);
   void  setText (const char*);

   bool setCenterText(bool);
   bool setCenterText();
   bool unsetCenterText();

   static int info ( FWidget*
                   , const FString&
                   , const FString&
                   , int = FMessageBox::Ok
                   , int = 0
                   , int = 0 );

   static int info ( FWidget*
                   , const FString&
                   , int
                   , int = FMessageBox::Ok
                   , int = 0
                   , int = 0 );

   static int error ( FWidget*
                    , const FString&
                    , int = FMessageBox::Ok
                    , int = 0
                    , int = 0 );
};
#pragma pack(pop)


// FMessageBox inline functions
//----------------------------------------------------------------------
inline const char* FMessageBox::getClassName() const
{ return "FMessageBox"; }

//----------------------------------------------------------------------
inline const FString FMessageBox::getTitlebarText() const
{ return FDialog::getText(); }

//----------------------------------------------------------------------
inline void FMessageBox::setTitlebarText (const FString& txt)
{ return FDialog::setText(txt); }

//----------------------------------------------------------------------
inline const FString FMessageBox::getHeadline() const
{ return headline_text; }

//----------------------------------------------------------------------
inline const FString FMessageBox::getText() const
{ return text; }

//----------------------------------------------------------------------
inline bool FMessageBox::setCenterText(bool on)
{ return center_text = on; }

//----------------------------------------------------------------------
inline bool FMessageBox::setCenterText()
{ return setCenterText(true); }

//----------------------------------------------------------------------
inline bool FMessageBox::unsetCenterText()
{ return setCenterText(false); }

#endif  // _FMESSAGEBOX_H
