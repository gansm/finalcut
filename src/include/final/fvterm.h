/***********************************************************************
* fvterm.h - Virtual terminal implementation                           *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2016-2018 Markus Gans                                      *
*                                                                      *
* The Final Cut is free software; you can redistribute it and/or       *
* modify it under the terms of the GNU Lesser General Public License   *
* as published by the Free Software Foundation; either version 3 of    *
* the License, or (at your option) any later version.                  *
*                                                                      *
* The Final Cut is distributed in the hope that it will be useful,     *
* but WITHOUT ANY WARRANTY; without even the implied warranty of       *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU Lesser General Public License for more details.                  *
*                                                                      *
* You should have received a copy of the GNU Lesser General Public     *
* License along with this program.  If not, see                        *
* <http://www.gnu.org/licenses/>.                                      *
***********************************************************************/

/*  Inheritance diagram
 *  ═══════════════════
 *
 *  ▕▔▔▔▔▔▔▔▔▔▏
 *  ▕  FTerm  ▏
 *  ▕▁▁▁▁▁▁▁▁▁▏
 *       ▲
 *       │
 *  ▕▔▔▔▔▔▔▔▔▔▏1         *▕▔▔▔▔▔▔▔▔▔▏
 *  ▕ FVTerm  ▏- - -┬- - -▕ FString ▏
 *  ▕▁▁▁▁▁▁▁▁▁▏     :     ▕▁▁▁▁▁▁▁▁▁▏
 *                  :
 *                  :    *▕▔▔▔▔▔▔▔▔▏
 *                  :- - -▕ FPoint ▏
 *                  :     ▕▁▁▁▁▁▁▁▁▏
 *                  :
 *                  :    *▕▔▔▔▔▔▔▔▏
 *                  └- - -▕ FRect ▏
 *                        ▕▁▁▁▁▁▁▁▏
 */

#ifndef FVTERM_H
#define FVTERM_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#if defined(__CYGWIN__)
  #include "final/fconfig.h"  // includes _GNU_SOURCE for wcwidth()
#endif

#include <queue>
#include <sstream>  // std::stringstream
#include <string>
#include <vector>

#include "final/fterm.h"


// Preprocessing handler macro
#define F_PREPROC_HANDLER(i,h) \
           static_cast<FVTerm*>((i)) \
         , reinterpret_cast<FVTerm::FPreprocessingHandler>((h))

namespace finalcut
{

// class forward declaration
class FWidget;

//----------------------------------------------------------------------
// class FVTerm
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FVTerm
{
  public:
    // Typedefs and Enumeration
    typedef struct
    {
      uInt xmin;           // X-position with the first change
      uInt xmax;           // X-position with the last change
      uInt trans_count;    // Number of transparent characters
    } line_changes;

    typedef FOptiAttr::charData  charData;
    typedef void (FVTerm::*FPreprocessingHandler)();

    struct term_area;  // forward declaration

    struct vterm_preprocessing
    {
      FVTerm*               instance;
      FPreprocessingHandler handler;
    };

    typedef std::vector<vterm_preprocessing> FPreprocessing;

    enum covered_state
    {
      non_covered,
      half_covered,
      fully_covered
    };

    enum terminal_update
    {
      stop_refresh,
      continue_refresh,
      start_refresh
    };

    // Constructor
    explicit FVTerm (bool, bool = false);

    // Destructor
    virtual ~FVTerm();

    // Overloaded operators
    template <typename type>
    FVTerm& operator << (const type&);
    FVTerm& operator << (const std::vector<charData>&);

    // Accessors
    virtual const char*   getClassName() const;
    static FColor         getTermForegroundColor();
    static FColor         getTermBackgroundColor();
    term_area*            getVWin() const;
    FPoint                getPrintCursor();
    static charData       getAttribute();
    static int            getMaxColor();
    static int            getTabstop();
    static fc::encoding   getEncoding();
    static std::string    getEncodingString();
    static const FString  getKeyName (FKey);
    static char*          getTermType();
    static char*          getTermFileName();
    FTerm&                getFTerm();
#if DEBUG
    FTermDebugData&       getFTermDebugData();
#endif

    // Mutators
    void                  setTermXY (int, int);
    void                  hideCursor (bool);
    void                  hideCursor();
    void                  showCursor();
    void                  setPrintCursor (const FPoint&);
    void                  setPrintCursor (int, int);
    FColor                rgb2ColorIndex (uInt8, uInt8, uInt8);
    void                  setColor (FColor, FColor);
    static void           setNormal();

    static bool           setBold (bool);
    static bool           setBold();
    static bool           unsetBold();

    static bool           setDim (bool);
    static bool           setDim();
    static bool           unsetDim();

    static bool           setItalic (bool);
    static bool           setItalic();
    static bool           unsetItalic();

    static bool           setUnderline (bool);
    static bool           setUnderline();
    static bool           unsetUnderline();

    static bool           setBlink (bool);
    static bool           setBlink();
    static bool           unsetBlink();

    static bool           setReverse (bool);
    static bool           setReverse();
    static bool           unsetReverse();

    static bool           setStandout (bool);
    static bool           setStandout();
    static bool           unsetStandout();

    static bool           setInvisible (bool);
    static bool           setInvisible();
    static bool           unsetInvisible();

    static bool           setProtected (bool);
    static bool           setProtected();
    static bool           unsetProtected();

    static bool           setCrossedOut (bool);
    static bool           setCrossedOut();
    static bool           unsetCrossedOut();

    static bool           setDoubleUnderline (bool);
    static bool           setDoubleUnderline();
    static bool           unsetDoubleUnderline();

    static bool           setAltCharset (bool);
    static bool           setAltCharset();
    static bool           unsetAltCharset();

    static bool           setPCcharset (bool);
    static bool           setPCcharset();
    static bool           unsetPCcharset();

    static bool           setTransparent (bool);
    static bool           setTransparent();
    static bool           unsetTransparent();

    static bool           setTransShadow (bool);
    static bool           setTransShadow();
    static bool           unsetTransShadow();

    static bool           setInheritBackground (bool);
    static bool           setInheritBackground();
    static bool           unsetInheritBackground();

    static void           setTermTitle (const FString&);
    static void           setEncoding (fc::encoding);
    static bool           setVGAFont();
    static bool           setNewFont();
    static bool           setOldFont();

    // Inquiries
    static bool           isBold();
    static bool           isDim();
    static bool           isItalic();
    static bool           isUnderline();
    static bool           isBlink();
    static bool           isReverse();
    static bool           isStandout();
    static bool           isInvisible();
    static bool           isProtected();
    static bool           isCrossedOut();
    static bool           isDoubleUnderline();
    static bool           isAltCharset();
    static bool           isPCcharset();
    static bool           isTransparent();
    static bool           isTransShadow();
    static bool           isInheritBackground();
    static bool           isMonochron();
    static bool           isXTerminal();
    static bool           isAnsiTerminal();
    static bool           isRxvtTerminal();
    static bool           isUrxvtTerminal();
    static bool           isMltermTerminal();
    static bool           isPuttyTerminal();
    static bool           isKdeTerminal();
    static bool           isGnomeTerminal();
    static bool           isKtermTerminal();
    static bool           isTeraTerm();
    static bool           isSunTerminal();
    static bool           isCygwinTerminal();
    static bool           isMinttyTerm();
    static bool           isLinuxTerm();
    static bool           isFreeBSDTerm();
    static bool           isNetBSDTerm();
    static bool           isOpenBSDTerm();
    static bool           isScreenTerm();
    static bool           isTmuxTerm();
    static bool           isNewFont();
    static bool           isCursorHideable();
    static bool           hasChangedTermSize();
    static bool           hasUTF8();

    // Methods
    virtual void          clearArea (int = ' ');
    void                  createVTerm (const FRect&);
    void                  createVTerm (int, int);
    void                  resizeVTerm (const FRect&);
    void                  resizeVTerm (int, int);
    void                  putVTerm();
    void                  updateTerminal (terminal_update);
    void                  updateTerminal();
    virtual void          addPreprocessingHandler ( FVTerm*
                                                  , FPreprocessingHandler );
    virtual void          delPreprocessingHandler (FVTerm*);

    int                   printf (const FString, ...);
    int                   print (const FString&);
    int                   print (term_area*, const FString&);
    int                   print (const std::vector<charData>&);
    int                   print (term_area*, const std::vector<charData>&);
    int                   print (int);
    int                   print (term_area*, int);
    int                   print (charData&);
    int                   print (term_area*, charData&);
    FVTerm&               print();
    static void           beep();
    static void           redefineDefaultColors (bool);
    static char*          moveCursor (int, int, int, int);
    static void           printMoveDurations();

  protected:
    // Enumeration
    enum character_type
    {
      overlapped_character,
      covered_character
    };

    // Accessor
    virtual term_area*    getPrintArea();
    std::size_t           getLineNumber();
    std::size_t           getColumnNumber();
    static bool           charEncodable (uInt);
    static FKeyboard*     getKeyboard();
    static FMouseControl* getMouseControl();
    FTerm::initializationValues& getInitValues();

    // Mutators
    static void           setInsertCursor (bool on);
    static void           setInsertCursor();
    static void           unsetInsertCursor();
    static bool           setUTF8 (bool);
    static bool           setUTF8();
    static bool           unsetUTF8();

    // Inquiries
    bool                  hasPrintArea() const;
    bool                  hasChildPrintArea() const;
    bool                  isVirtualWindow() const;
    static bool           hasHalfBlockCharacter();
    static bool           hasShadowCharacter();

    // Methods
    void                  createArea ( const FRect&
                                     , const FPoint&
                                     , term_area*& );

    void                  createArea ( int, int, int, int
                                     , int, int
                                     , term_area*& );

    void                  resizeArea ( const FRect&
                                     , const FPoint&
                                     , term_area* );

    void                  resizeArea ( int, int, int, int
                                     , int, int
                                     , term_area* );

    static void           removeArea (term_area*&);
    static void           restoreVTerm (const FRect&);
    static void           restoreVTerm (int, int, int, int);
    void                  setTextToDefault (term_area*, int, int);
    static bool           reallocateTextArea ( term_area*
                                             , std::size_t
                                             , std::size_t );
    static bool           reallocateTextArea ( term_area*
                                             , std::size_t );

    static covered_state  isCovered ( const FPoint&
                                    , term_area* );

    static covered_state  isCovered ( int, int
                                    , term_area* );

    static void           updateOverlappedColor ( term_area*
                                                , int, int, int, int );
    static void           updateOverlappedCharacter (term_area*, int, int);
    static void           updateShadedCharacter ( term_area*
                                                , int, int, int, int );
    static void           updateInheritBackground ( term_area*
                                                  , int, int, int, int );
    static void           updateCharacter ( term_area*
                                          , int, int, int, int );
    static bool           updateVTermCharacter ( term_area*
                                               , int, int, int, int );
    static void           callPreprocessingHandler (term_area*);
    void                  updateVTerm();
    void                  updateVTerm (term_area*);
    bool                  updateVTermCursor (term_area*);
    static bool           isInsideArea (int, int, term_area*);

    static void           setAreaCursor ( const FPoint&
                                        , bool, term_area* );

    static void           setAreaCursor ( int, int
                                        , bool, term_area*);

    static void           getArea (const FPoint&, term_area*);
    static void           getArea (int, int, term_area*);
    static void           getArea (const FRect&, term_area*);
    static void           getArea (int, int, int, int, term_area*);
    static void           putArea (const FPoint&, term_area*);
    static void           putArea (int, int, term_area*);
    void                  scrollAreaForward (term_area*);
    void                  scrollAreaReverse (term_area*);
    void                  clearArea (term_area*, int = ' ');

    static charData       generateCharacter (const FPoint&);
    static charData       generateCharacter (int, int);

    static charData       getCharacter ( character_type
                                       , const FPoint&
                                       , FVTerm* );

    static charData       getCharacter ( character_type
                                       , int
                                       , int, FVTerm* );

    static charData       getCoveredCharacter (const FPoint&, FVTerm*);
    static charData       getCoveredCharacter (int, int, FVTerm*);
    static charData       getOverlappedCharacter (const FPoint&, FVTerm*);
    static charData       getOverlappedCharacter (int, int, FVTerm*);
    void                  processTerminalUpdate();
    static void           startTerminalUpdate();
    static void           finishTerminalUpdate();
    static void           flush_out();
    static void           initScreenSettings();
    static void           changeTermSizeFinished();
    static void           exitWithMessage (const FString&)
    #if defined(__clang__) || defined(__GNUC__)
      __attribute__((noreturn))
    #endif
                           ;

    // Data Members
    static        term_area* vterm;        // virtual terminal
    static        term_area* vdesktop;     // virtual desktop
    static        term_area* active_area;  // active area
    term_area*    print_area;              // print area for this object
    term_area*    child_print_area;        // print area for children
    term_area*    vwin;                    // virtual window

  private:
    // Typedef and Enumeration
    typedef FTermcap::tcap_map termcap_map;

    enum exit_state
    {
      not_used,
      used,
      line_completely_printed
    };

    // Constants
    static const uInt TERMINAL_OUTPUT_BUFFER_SIZE = 32768;
    // Buffer size for character output on the terminal

    // Disable copy constructor
    FVTerm (const FVTerm&);

    // Disable assignment operator (=)
    FVTerm& operator = (const FVTerm&);

    // Mutators
    void                  setPrintArea (term_area*);

    // Methods
    void                  init (bool);
    static void           init_characterLengths (FOptiMove*);
    void                  finish();
    static void           putAreaLine (charData*, charData*, int);
    static void           putAreaCharacter ( int, int, FVTerm*
                                           , charData*, charData* );
    static void           getAreaCharacter ( int, int, term_area*
                                           , charData*& );
    bool                  clearTerm (int = ' ');
    bool                  clearFullArea (term_area*, charData&);
    static void           clearAreaWithShadow (term_area*, charData&);
    static bool           canClearToEOL (uInt, uInt);
    static bool           canClearLeadingWS (uInt&, uInt);
    static bool           canClearTrailingWS (uInt&, uInt);
    bool                  skipUnchangedCharacters (uInt&, uInt, uInt);
    void                  printRange (uInt, uInt, uInt, bool);
    exit_state            eraseCharacters (uInt&, uInt, uInt, bool);
    exit_state            repeatCharacter (uInt&, uInt, uInt);
    static void           cursorWrap();
    bool                  printWrap (term_area*);
    void                  updateTerminalLine (uInt);
    bool                  updateTerminalCursor();
    bool                  isInsideTerminal (int, int);
    static void           markAsPrinted (uInt, uInt);
    static void           markAsPrinted (uInt, uInt, uInt);
    static void           newFontChanges (charData*&);
    static void           charsetChanges (charData*&);
    void                  appendCharacter (charData*&);
    void                  appendChar (charData*&);
    void                  appendAttributes (charData*&);
    int                   appendLowerRight (charData*&);
    static void           appendOutputBuffer (const std::string&);
    static void           appendOutputBuffer (const char[]);

#if defined(__sun) && defined(__SVR4)
    static int            appendOutputBuffer (char);
#endif
    static int            appendOutputBuffer (int);

    // Data Members
    static FTerm*           fterm;
    static std::queue<int>* output_buffer;
    static charData         term_attribute;
    static charData         next_attribute;
    static charData         s_ch;      // shadow character
    static charData         i_ch;      // inherit background character
    static FPoint*          term_pos;  // terminal cursor position
    static FTermcap::tcap_map*     tcap;
    static FKeyboard*       keyboard;
    static bool             terminal_update_complete;
    static bool             terminal_update_pending;
    static bool             force_terminal_update;
    static bool             stop_terminal_updates;
    static int              skipped_terminal_update;
    static uInt             erase_char_length;
    static uInt             repeat_char_length;
    static uInt             clr_bol_length;
    static uInt             clr_eol_length;
    static uInt             cursor_address_length;
};
#pragma pack(pop)


//----------------------------------------------------------------------
// struct FVTerm::term_area
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

struct FVTerm::term_area  // define virtual terminal character properties
{
  public:
    term_area()
      : offset_left (0)
      , offset_top (0)
      , width (-1)
      , height (-1)
      , right_shadow (0)
      , bottom_shadow (0)
      , cursor_x (0)
      , cursor_y (0)
      , input_cursor_x (-1)
      , input_cursor_y (-1)
      , widget()
      , preprocessing_call()
      , changes (0)
      , text (0)
      , input_cursor_visible (false)
      , has_changes (false)
      , visible (false)
    { }

    ~term_area()
    { }

    int offset_left;     // Distance from left terminal side
    int offset_top;      // Distance from top of the terminal
    int width;           // Window width
    int height;          // Window height
    int right_shadow;    // Right window shadow
    int bottom_shadow;   // Bottom window shadow
    int cursor_x;        // X-position for the next write operation
    int cursor_y;        // Y-position for the next write operation
    int input_cursor_x;  // X-position input cursor
    int input_cursor_y;  // Y-position input cursor
    FWidget* widget;     // Widget that owns this term_area
    FPreprocessing preprocessing_call;
    line_changes* changes;
    charData* text;     // Text data for the output
    bool input_cursor_visible;
    bool has_changes;
    bool visible;

  private:
    // Disable copy constructor
    term_area (const term_area&);
    // Disable assignment operator (=)
    term_area& operator = (const term_area&);
};
#pragma pack(pop)


// FVTerm inline functions
//----------------------------------------------------------------------
template <typename type>
inline FVTerm& FVTerm::operator << (const type& s)
{
  std::wostringstream outstream;
  outstream << s;

  if ( ! outstream.str().empty() )
    print (outstream.str());

  return *this;
}

//----------------------------------------------------------------------
inline FVTerm& FVTerm::operator << \
    (const std::vector<FVTerm::charData>& termString)
{
  print (termString);
  return *this;
}

//----------------------------------------------------------------------
inline const char* FVTerm::getClassName() const
{ return "FVTerm"; }

//----------------------------------------------------------------------
inline FColor FVTerm::getTermForegroundColor()
{ return next_attribute.fg_color; }

//----------------------------------------------------------------------
inline FColor FVTerm::getTermBackgroundColor()
{ return next_attribute.bg_color; }

//----------------------------------------------------------------------
inline FVTerm::term_area* FVTerm::getVWin() const
{ return vwin; }

//----------------------------------------------------------------------
inline FVTerm::charData FVTerm::getAttribute()
{ return next_attribute; }

//----------------------------------------------------------------------
inline int FVTerm::getMaxColor()
{ return FTerm::getMaxColor(); }

//----------------------------------------------------------------------
inline int FVTerm::getTabstop()
{ return FTerm::getTabstop(); }

//----------------------------------------------------------------------
inline fc::encoding FVTerm::getEncoding()
{ return FTerm::getEncoding(); }

//----------------------------------------------------------------------
inline std::string FVTerm::getEncodingString()
{ return FTerm::getEncodingString(); }

//----------------------------------------------------------------------
inline const FString FVTerm::getKeyName (FKey keynum)
{ return FTerm::getKeyName(keynum); }

//----------------------------------------------------------------------
inline char* FVTerm::getTermType()
{ return FTerm::getTermType(); }

//----------------------------------------------------------------------
inline char* FVTerm::getTermFileName()
{ return FTerm::getTermFileName(); }

//----------------------------------------------------------------------
inline FTerm& FVTerm::getFTerm()
{ return *fterm; }

//----------------------------------------------------------------------
#if DEBUG
inline FTermDebugData& FVTerm::getFTermDebugData()
{ return getFTerm().getFTermDebugData(); }
#endif

//----------------------------------------------------------------------
inline void FVTerm::hideCursor()
{ return hideCursor(true); }

//----------------------------------------------------------------------
inline void FVTerm::showCursor()
{ return hideCursor(false); }

//----------------------------------------------------------------------
inline void FVTerm::setPrintCursor (const FPoint& pos)
{ setPrintCursor (pos.getX(), pos.getY()); }

//----------------------------------------------------------------------
inline void FVTerm::setColor (FColor fg, FColor bg)
{
  // Changes colors
  next_attribute.fg_color = fg;
  next_attribute.bg_color = bg;
}

//----------------------------------------------------------------------
inline void FVTerm::setNormal()
{
  // reset all character attributes
  next_attribute.attr.byte[0] = 0;
  next_attribute.attr.byte[1] = 0;
  next_attribute.attr.bit.no_changes = false;
  next_attribute.fg_color = fc::Default;
  next_attribute.bg_color = fc::Default;
}

//----------------------------------------------------------------------
inline bool FVTerm::setBold (bool on)
{ return (next_attribute.attr.bit.bold = on); }

//----------------------------------------------------------------------
inline bool FVTerm::setBold()
{ return setBold(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetBold()
{ return setBold(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setDim (bool on)
{ return (next_attribute.attr.bit.dim = on); }

//----------------------------------------------------------------------
inline bool FVTerm::setDim()
{ return setDim(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetDim()
{ return setDim(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setItalic (bool on)
{ return (next_attribute.attr.bit.italic = on); }

//----------------------------------------------------------------------
inline bool FVTerm::setItalic()
{ return setItalic(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetItalic()
{ return setItalic(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setUnderline (bool on)
{ return (next_attribute.attr.bit.underline = on); }

//----------------------------------------------------------------------
inline bool FVTerm::setUnderline()
{ return setUnderline(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetUnderline()
{ return setUnderline(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setBlink (bool on)
{ return (next_attribute.attr.bit.blink = on); }

//----------------------------------------------------------------------
inline bool FVTerm::setBlink()
{ return setBlink(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetBlink()
{ return setBlink(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setReverse (bool on)
{ return (next_attribute.attr.bit.reverse = on); }

//----------------------------------------------------------------------
inline bool FVTerm::setReverse()
{ return setReverse(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetReverse()
{ return setReverse(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setStandout (bool on)
{ return (next_attribute.attr.bit.standout = on); }

//----------------------------------------------------------------------
inline bool FVTerm::setStandout()
{ return setStandout(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetStandout()
{ return setStandout(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setInvisible (bool on)
{ return (next_attribute.attr.bit.invisible = on); }

//----------------------------------------------------------------------
inline bool FVTerm::setInvisible()
{ return setInvisible(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetInvisible()
{ return setInvisible(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setProtected (bool on)
{ return (next_attribute.attr.bit.protect = on); }

//----------------------------------------------------------------------
inline bool FVTerm::setProtected()
{ return setProtected(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetProtected()
{ return setProtected(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setCrossedOut (bool on)
{ return (next_attribute.attr.bit.crossed_out = on); }

//----------------------------------------------------------------------
inline bool FVTerm::setCrossedOut()
{ return setCrossedOut(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetCrossedOut()
{ return setCrossedOut(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setDoubleUnderline (bool on)
{ return (next_attribute.attr.bit.dbl_underline = on); }

//----------------------------------------------------------------------
inline bool FVTerm::setDoubleUnderline()
{ return setDoubleUnderline(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetDoubleUnderline()
{ return setDoubleUnderline(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setAltCharset (bool on)
{ return (next_attribute.attr.bit.alt_charset = on); }

//----------------------------------------------------------------------
inline bool FVTerm::setAltCharset()
{ return setAltCharset(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetAltCharset()
{ return setAltCharset(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setPCcharset (bool on)
{ return (next_attribute.attr.bit.pc_charset = on); }

//----------------------------------------------------------------------
inline bool FVTerm::setPCcharset()
{ return setPCcharset(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetPCcharset()
{ return setPCcharset(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setTransparent (bool on)
{ return (next_attribute.attr.bit.transparent = on); }

//----------------------------------------------------------------------
inline bool FVTerm::setTransparent()
{ return setTransparent(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetTransparent()
{ return setTransparent(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setTransShadow (bool on)
{ return (next_attribute.attr.bit.trans_shadow = on); }

//----------------------------------------------------------------------
inline bool FVTerm::setTransShadow()
{ return setTransShadow(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetTransShadow()
{ return setTransShadow(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setInheritBackground (bool on)
{ return (next_attribute.attr.bit.inherit_bg = on); }

//----------------------------------------------------------------------
inline bool FVTerm::setInheritBackground()
{ return setInheritBackground(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetInheritBackground()
{ return setInheritBackground(false); }

//----------------------------------------------------------------------
inline void FVTerm::setTermTitle (const FString& title)
{ FTerm::setTermTitle(title); }

//----------------------------------------------------------------------
inline void FVTerm::setEncoding (fc::encoding enc)
{ FTerm::setEncoding(enc); }

//----------------------------------------------------------------------
inline bool FVTerm::setVGAFont()
{ return FTerm::setVGAFont(); }

//----------------------------------------------------------------------
inline bool FVTerm::setNewFont()
{ return FTerm::setNewFont(); }

//----------------------------------------------------------------------
inline bool FVTerm::setOldFont()
{ return FTerm::setOldFont(); }

//----------------------------------------------------------------------
inline bool FVTerm::isBold()
{ return next_attribute.attr.bit.bold; }

//----------------------------------------------------------------------
inline bool FVTerm::isDim()
{ return next_attribute.attr.bit.dim; }

//----------------------------------------------------------------------
inline bool FVTerm::isItalic()
{ return next_attribute.attr.bit.italic; }

//----------------------------------------------------------------------
inline bool FVTerm::isUnderline()
{ return next_attribute.attr.bit.underline; }

//----------------------------------------------------------------------
inline bool FVTerm::isBlink()
{ return next_attribute.attr.bit.blink; }

//----------------------------------------------------------------------
inline bool FVTerm::isReverse()
{ return next_attribute.attr.bit.reverse; }

//----------------------------------------------------------------------
inline bool FVTerm::isStandout()
{ return next_attribute.attr.bit.standout; }

//----------------------------------------------------------------------
inline bool FVTerm::isInvisible()
{ return next_attribute.attr.bit.invisible; }

//----------------------------------------------------------------------
inline bool FVTerm::isProtected()
{ return next_attribute.attr.bit.protect; }

//----------------------------------------------------------------------
inline bool FVTerm::isCrossedOut()
{ return next_attribute.attr.bit.crossed_out; }

//----------------------------------------------------------------------
inline bool FVTerm::isDoubleUnderline()
{ return next_attribute.attr.bit.dbl_underline; }

//----------------------------------------------------------------------
inline bool FVTerm::isAltCharset()
{ return next_attribute.attr.bit.alt_charset; }

//----------------------------------------------------------------------
inline bool FVTerm::isPCcharset()
{ return next_attribute.attr.bit.pc_charset; }

//----------------------------------------------------------------------
inline bool FVTerm::isTransparent()
{ return next_attribute.attr.bit.transparent; }

//----------------------------------------------------------------------
inline bool FVTerm::isTransShadow()
{ return next_attribute.attr.bit.trans_shadow; }

//----------------------------------------------------------------------
inline bool FVTerm::isInheritBackground()
{ return next_attribute.attr.bit.inherit_bg; }

//----------------------------------------------------------------------
inline bool FVTerm::isMonochron()
{ return FTerm::isMonochron(); }

//----------------------------------------------------------------------
inline bool FVTerm::isXTerminal()
{ return FTerm::isXTerminal(); }

//----------------------------------------------------------------------
inline bool FVTerm::isAnsiTerminal()
{ return FTerm::isAnsiTerminal(); }

//----------------------------------------------------------------------
inline bool FVTerm::isRxvtTerminal()
{ return FTerm::isRxvtTerminal(); }

//----------------------------------------------------------------------
inline bool FVTerm::isUrxvtTerminal()
{ return FTerm::isUrxvtTerminal(); }

//----------------------------------------------------------------------
inline bool FVTerm::isMltermTerminal()
{ return FTerm::isMltermTerminal(); }

//----------------------------------------------------------------------
inline bool FVTerm::isPuttyTerminal()
{ return FTerm::isPuttyTerminal(); }

//----------------------------------------------------------------------
inline bool FVTerm::isKdeTerminal()
{ return FTerm::isKdeTerminal(); }

//----------------------------------------------------------------------
inline bool FVTerm::isGnomeTerminal()
{ return FTerm::isGnomeTerminal(); }

//----------------------------------------------------------------------
inline bool FVTerm::isKtermTerminal()
{ return FTerm::isKtermTerminal(); }

//----------------------------------------------------------------------
inline bool FVTerm::isTeraTerm()
{ return FTerm::isTeraTerm(); }

//----------------------------------------------------------------------
inline bool FVTerm::isSunTerminal()
{ return FTerm::isSunTerminal(); }

//----------------------------------------------------------------------
inline bool FVTerm::isCygwinTerminal()
{ return FTerm::isCygwinTerminal(); }

//----------------------------------------------------------------------
inline bool FVTerm::isMinttyTerm()
{ return FTerm::isMinttyTerm(); }

//----------------------------------------------------------------------
inline bool FVTerm::isLinuxTerm()
{ return FTerm::isLinuxTerm(); }

//----------------------------------------------------------------------
inline bool FVTerm::isFreeBSDTerm()
{ return FTerm::isFreeBSDTerm(); }

//----------------------------------------------------------------------
inline bool FVTerm::isNetBSDTerm()
{ return FTerm::isNetBSDTerm(); }

//----------------------------------------------------------------------
inline bool FVTerm::isOpenBSDTerm()
{ return FTerm::isOpenBSDTerm(); }

//----------------------------------------------------------------------
inline bool FVTerm::isScreenTerm()
{ return FTerm::isScreenTerm(); }

//----------------------------------------------------------------------
inline bool FVTerm::isTmuxTerm()
{ return FTerm::isTmuxTerm(); }

//----------------------------------------------------------------------
inline bool FVTerm::isNewFont()
{ return FTerm::isNewFont(); }

//----------------------------------------------------------------------
inline bool FVTerm::isCursorHideable()
{ return FTerm::isCursorHideable(); }

//----------------------------------------------------------------------
inline bool FVTerm::hasChangedTermSize()
{ return FTerm::hasChangedTermSize(); }

//----------------------------------------------------------------------
inline bool FVTerm::hasUTF8()
{ return FTerm::hasUTF8(); }

//----------------------------------------------------------------------
inline FVTerm& FVTerm::print()
{ return *this; }

//----------------------------------------------------------------------
inline void FVTerm::beep()
{ FTerm::beep(); }

//----------------------------------------------------------------------
inline void FVTerm::redefineDefaultColors (bool on)
{ FTerm::redefineDefaultColors(on); }

//----------------------------------------------------------------------
inline char* FVTerm::moveCursor (int xold, int yold, int xnew, int ynew)
{ return FTerm::moveCursor (xold, yold, xnew, ynew); }

//----------------------------------------------------------------------
inline void FVTerm::printMoveDurations()
{ return FTerm::printMoveDurations(); }

//----------------------------------------------------------------------
inline std::size_t FVTerm::getLineNumber()
{ return FTerm::getLineNumber(); }

//----------------------------------------------------------------------
inline std::size_t FVTerm::getColumnNumber()
{ return FTerm::getColumnNumber(); }

//----------------------------------------------------------------------
inline bool FVTerm::charEncodable (uInt c)
{ return FTerm::charEncodable(c); }

//----------------------------------------------------------------------
inline FKeyboard* FVTerm::getKeyboard()
{ return FTerm::getKeyboard(); }

//----------------------------------------------------------------------
inline FMouseControl* FVTerm::getMouseControl()
{ return FTerm::getMouseControl(); }

//----------------------------------------------------------------------
inline FTerm::initializationValues& FVTerm::getInitValues()
{ return getFTerm().getInitValues(); }

//----------------------------------------------------------------------
inline void FVTerm::setInsertCursor (bool on)
{ return FTerm::setInsertCursor(on); }

//----------------------------------------------------------------------
inline void FVTerm::setInsertCursor()
{ return FTerm::setInsertCursor(true); }

//----------------------------------------------------------------------
inline void FVTerm::unsetInsertCursor()
{ return FTerm::setInsertCursor(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setUTF8 (bool on)
{ return FTerm::setUTF8(on); }

//----------------------------------------------------------------------
inline bool FVTerm::setUTF8()
{ return FTerm::setUTF8(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetUTF8()
{ return FTerm::setUTF8(false); }

//----------------------------------------------------------------------
inline bool FVTerm::hasPrintArea() const
{ return print_area; }

//----------------------------------------------------------------------
inline bool FVTerm::hasChildPrintArea() const
{ return child_print_area; }

//----------------------------------------------------------------------
inline bool FVTerm::isVirtualWindow() const
{ return vwin; }

//----------------------------------------------------------------------
inline bool FVTerm::hasHalfBlockCharacter()
{ return FTerm::hasHalfBlockCharacter(); }

//----------------------------------------------------------------------
inline bool FVTerm::hasShadowCharacter()
{ return FTerm::hasShadowCharacter(); }

//----------------------------------------------------------------------
inline void FVTerm::initScreenSettings()
{ FTerm::initScreenSettings(); }

//----------------------------------------------------------------------
inline void FVTerm::changeTermSizeFinished()
{ FTerm::changeTermSizeFinished(); }

//----------------------------------------------------------------------
inline void FVTerm::exitWithMessage (const FString& message)
{ FTerm::exitWithMessage(message); }

//----------------------------------------------------------------------
inline void FVTerm::setPrintArea (term_area* area)
{ print_area = area; }

}  // namespace finalcut

#endif  // FVTERM_H
