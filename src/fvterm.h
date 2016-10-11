// File: fvterm.h
// Provides: class FVTerm
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
//        ▕▔▔▔▔▔▔▔▔▏
//        ▕ FVTerm ▏
//        ▕▁▁▁▁▁▁▁▁▏


#ifndef _FVTERM_H
#define _FVTERM_H

#include "fterm.h"


// Buffer size for character output on the terminal
#define TERMINAL_OUTPUT_BUFFER_SIZE  32768

// class forward declaration
class FWidget;


//----------------------------------------------------------------------
// class FVTerm
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FVTerm : public FObject, public FTerm
{
 private:
   static std::queue<int>*     output_buffer;
   static FOptiAttr::char_data term_attribute;
   static FOptiAttr::char_data next_attribute;
   static FPoint*              term_pos;  // terminal cursor position
   static FPoint*              cursor;    // virtual print cursor
   static FTermcap::tcap_map*  tcap;

   static bool  hidden_cursor;
   static bool  terminal_update_pending;
   static bool  force_terminal_update;
   static bool  terminal_updates;
   static bool  stop_terminal_updates;
   static bool  vterm_updates;
   static int   skipped_terminal_update;

   enum covered_state
   {
     non_covered,
     half_covered,
     fully_covered
   };

   enum character_type
   {
     overlapped_character,
     covered_character
   };

 protected:
   typedef struct
   {
     uInt xmin;
     uInt xmax;
     uInt trans_count;
   } line_changes;

   typedef struct
   {
     int width;
     int height;
     int right_shadow;
     int bottom_shadow;
     int input_cursor_x;
     int input_cursor_y;
     int input_cursor_visible;
     FWidget* widget;
     line_changes* changes;
     FOptiAttr::char_data* text;
     bool visible;
   } term_area;

   static term_area* vterm;        // virtual terminal
   static term_area* vdesktop;     // virtual desktop
   static term_area* last_area;    // last used area
   static term_area* active_area;  // active area
   term_area* print_area;          // print area for this object
   term_area* vwin;                // virtual window

 protected:
   void         createArea (const FRect&, const FPoint&, FVTerm::term_area*&);
   void         createArea (int, int, int, int, FVTerm::term_area*&);
   static void  resizeArea (const FRect&, const FPoint&, FVTerm::term_area*);
   static void  resizeArea (int, int, int, int, FVTerm::term_area*);
   static void  removeArea (FVTerm::term_area*&);
   static void  restoreVTerm (const FRect&);
   static void  restoreVTerm (int, int, int, int);
   static FVTerm::covered_state isCovered (const FPoint&, FVTerm::term_area*);
   static FVTerm::covered_state isCovered (int, int, FVTerm::term_area*);
   static void  updateVTerm (bool);
   static void  updateVTerm (FVTerm::term_area*);
   static bool  updateVTermCursor (FVTerm::term_area*);
   static bool  isInsideArea (int, int, FVTerm::term_area*);
   static void  setAreaCursor (const FPoint&, bool, FVTerm::term_area*);
   static void  setAreaCursor (int, int, bool, FVTerm::term_area*);
   static void  getArea (const FPoint&, FVTerm::term_area*);
   static void  getArea (int, int, FVTerm::term_area*);
   static void  getArea (const FRect&, FVTerm::term_area*);
   static void  getArea (int, int, int, int, FVTerm::term_area*);
   static void  putArea (const FPoint&, FVTerm::term_area*);
   static void  putArea (int, int, FVTerm::term_area*);
   static void  scrollAreaForward (FVTerm::term_area*);
   static void  scrollAreaReverse (FVTerm::term_area*);
   static void  clearArea (FVTerm::term_area*);
   static FOptiAttr::char_data getCharacter (int, const FPoint&, FVTerm*);
   static FOptiAttr::char_data getCharacter (int, int, int, FVTerm*);
   static FOptiAttr::char_data getCoveredCharacter (const FPoint&, FVTerm*);
   static FOptiAttr::char_data getCoveredCharacter (int, int, FVTerm*);
   static FOptiAttr::char_data getOverlappedCharacter (const FPoint&, FVTerm*);
   static FOptiAttr::char_data getOverlappedCharacter (int, int, FVTerm*);

 private:
   // Disable copy constructor
   FVTerm (const FVTerm&);
   // Disable assignment operator (=)
   FVTerm& operator = (const FVTerm&);
   
   void init();
   void finish();
   term_area*     getPrintArea();
   void           setPrintArea (term_area*);
   
   // Friend class
   friend class FWidget;

 public:
   // Constructor
   explicit FVTerm (FVTerm* = 0);
   // Destructor
  ~FVTerm();

   virtual const char* getClassName() const;

   static void    setTermXY (register int, register int);

   static bool    hideCursor (bool);
   static bool    hideCursor();
   static bool    showCursor();
   static bool    isCursorHidden();

   static short   getTermForegroundColor();
   static short   getTermBackgroundColor();
   FVTerm::term_area* getVWin() const;
   void           createVTerm (const FRect&);
   void           createVTerm (int, int);
   static void    resizeVTerm (const FRect&);
   static void    resizeVTerm (int, int);
   static void    putVTerm();
   static void    updateTerminal (bool);
   static void    updateTerminal();
   static bool    updateTerminalCursor();
   static void    processTerminalUpdate();
   static bool    isInsideTerminal (int, int);
   
   void           setPrintCursor (const FPoint&);
   void           setPrintCursor (register int, register int);
   FPoint*        getPrintPos() const;
   int            printf (const wchar_t*, ...);
   int            printf (const char*, ...)
   #if defined(__clang__)
     __attribute__((__format__ (__printf__, 2, 3)))
   #elif defined(__GNUC__)
     __attribute__ ((format (printf, 2, 3)))
   #endif
                  ;
   int            print (const std::wstring&);
   int            print (FVTerm::term_area*, const std::wstring&);
   int            print (const wchar_t*);
   int            print (FVTerm::term_area*, const wchar_t*);
   int            print (const char*);
   int            print (FVTerm::term_area*, const char*);
   int            print (const std::string&);
   int            print (FVTerm::term_area*, const std::string&);
   int            print (FString&);
   int            print (FVTerm::term_area*, FString&);
   int            print (int);
   int            print (FVTerm::term_area*, int);
   static void    newFontChanges (FOptiAttr::char_data*&);
   static void    charsetChanges (FOptiAttr::char_data*&);
   static void    appendCharacter (FOptiAttr::char_data*&);
   static void    appendAttributes (FOptiAttr::char_data*&);
   static int     appendLowerRight (FOptiAttr::char_data*&);
   static void    appendOutputBuffer (std::string&);
   static void    appendOutputBuffer (const char*);
   static int     appendOutputBuffer (int);
   static void    flush_out();
};

#pragma pack(pop)

// FVTerm inline functions
//----------------------------------------------------------------------
inline void FVTerm::setPrintArea (term_area* area)
{ print_area = area; }

//----------------------------------------------------------------------
inline const char* FVTerm::getClassName() const
{ return "FVTerm"; }

//----------------------------------------------------------------------
inline bool FVTerm::hideCursor()
{ return hideCursor(true); }

//----------------------------------------------------------------------
inline bool FVTerm::showCursor()
{ return hideCursor(false); }

//----------------------------------------------------------------------
inline bool FVTerm::isCursorHidden()
{ return hidden_cursor; }

//----------------------------------------------------------------------
inline short FVTerm::getTermForegroundColor()
{ return next_attribute.fg_color; }

//----------------------------------------------------------------------
inline short FVTerm::getTermBackgroundColor()
{ return next_attribute.bg_color; }

//----------------------------------------------------------------------
inline FVTerm::term_area* FVTerm::getVWin() const
{ return vwin; }

//----------------------------------------------------------------------
inline void FVTerm::setPrintCursor (const FPoint& pos)
{ setPrintCursor (pos.getX(), pos.getY()); }

//----------------------------------------------------------------------
inline void FVTerm::setPrintCursor (register int x, register int y)
{ cursor->setPoint(x,y); }

#endif  // _FVTERM_H
