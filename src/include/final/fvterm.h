/***********************************************************************
* fvterm.h - Virtual terminal implementation                           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2016-2020 Markus Gans                                      *
*                                                                      *
* FINAL CUT is free software; you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as       *
* published by the Free Software Foundation; either version 3 of       *
* the License, or (at your option) any later version.                  *
*                                                                      *
* FINAL CUT is distributed in the hope that it will be useful, but     *
* WITHOUT ANY WARRANTY; without even the implied warranty of           *
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
 *  ▕▔▔▔▔▔▔▔▔▔▏1         *▕▔▔▔▔▔▔▔▏
 *  ▕ FVTerm  ▏- - -┬- - -▕ FTerm ▏
 *  ▕▁▁▁▁▁▁▁▁▁▏     :     ▕▁▁▁▁▁▁▁▏
 *                  :
 *                  :    *▕▔▔▔▔▔▔▔▔▔▏
 *                  :- - -▕ FString ▏
 *                  :     ▕▁▁▁▁▁▁▁▁▁▏
 *                  :
 *                  :    *▕▔▔▔▔▔▔▔▔▏
 *                  └- - -▕ FPoint ▏
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

#include <sys/time.h>  // need for timeval (cygwin)

#include <queue>
#include <string>
#include <utility>
#include <vector>

#include "final/fc.h"
#include "final/fstringstream.h"
#include "final/fterm.h"

#define F_PREPROC_HANDLER(i,h) \
    reinterpret_cast<FVTerm*>((i)), \
    std::bind ( reinterpret_cast<FVTerm::FPreprocessingHandler>((h)) \
              , reinterpret_cast<FVTerm*>((i)) )

namespace finalcut
{

// class forward declaration
class FColorPair;
class FKeyboard;
class FMouseControl;
class FPoint;
class FRect;
class FSize;
class FString;
class FSystem;
class FTerm;
class FTermBuffer;
class FTermDebugData;
class FStyle;
class FWidget;

//----------------------------------------------------------------------
// class FVTerm
//----------------------------------------------------------------------

class FVTerm
{
  public:
    // Typedefs and Enumeration
    typedef struct
    {
      uInt xmin;           // X-position with the first change
      uInt xmax;           // X-position with the last change
      uInt trans_count;    // Number of transparent characters
    } FLineChanges;

    typedef void (FVTerm::*FPreprocessingHandler)();
    typedef std::function<void()> FPreprocessingFunction;

    struct FTermArea;             // forward declaration
    struct FVTermPreprocessing;   // forward declaration

    typedef std::vector<FVTermPreprocessing> FPreprocessing;

    enum covered_state
    {
      non_covered,
      half_covered,
      fully_covered
    };

    enum terminal_update
    {
      stop_terminal_updates,      // No terminal refresh
      continue_terminal_updates,  // Resuming terminal refresh
      start_terminal_updates      // Allowing terminal refresh
    };

    // Constructor
    FVTerm();

    // Disable copy constructor
    FVTerm (const FVTerm&) = delete;

    // Destructor
    virtual ~FVTerm();

    // Disable copy assignment operator (=)
    FVTerm& operator = (const FVTerm&) = delete;

    // Overloaded operators
    template <typename typeT>
    FVTerm& operator << (const typeT&);
    FVTerm& operator << (fc::SpecialCharacter);
    FVTerm& operator << (const std::string&);
    FVTerm& operator << (const FTermBuffer&);
    FVTerm& operator << (const std::vector<FChar>&);
    FVTerm& operator << (const FPoint&);
    FVTerm& operator << (const FStyle&);
    FVTerm& operator << (const FColorPair&);

    // Accessors
    virtual FString       getClassName() const;
    static FColor         getTermForegroundColor();
    static FColor         getTermBackgroundColor();
    FTermArea*&           getVWin();
    const FTermArea*      getVWin() const;
    FPoint                getPrintCursor();
    static FChar          getAttribute();
    FTerm&                getFTerm() const;

    // Mutators
    void                  setTermXY (int, int) const;
    void                  setTerminalUpdates (terminal_update) const;
    void                  hideCursor (bool) const;
    void                  hideCursor() const;
    void                  showCursor() const;
    void                  setPrintCursor (const FPoint&);

    FColor                rgb2ColorIndex (uInt8, uInt8, uInt8) const;
    static void           setColor (FColor, FColor);
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

    static bool           setColorOverlay (bool);
    static bool           setColorOverlay();
    static bool           unsetColorOverlay();

    static bool           setInheritBackground (bool);
    static bool           setInheritBackground();
    static bool           unsetInheritBackground();

    static void           setNonBlockingRead (bool);
    static void           setNonBlockingRead();
    static void           unsetNonBlockingRead();

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

    // Methods
    virtual void          clearArea (int = ' ');
    void                  createVTerm (const FSize&);
    void                  resizeVTerm (const FSize&) const;
    void                  putVTerm() const;
    void                  updateTerminal() const;
    virtual void          addPreprocessingHandler ( const FVTerm*
                                                  , const FPreprocessingFunction& );
    virtual void          delPreprocessingHandler (const FVTerm*);
    template <typename... Args>
    int                   printf (const FString&, Args&&...);
    int                   print (const FString&);
    int                   print (FTermArea*, const FString&);
    int                   print (const FTermBuffer&);
    int                   print (FTermArea*, const FTermBuffer&);
    int                   print (const std::vector<FChar>&);
    int                   print (FTermArea*, const std::vector<FChar>&);
    int                   print (wchar_t);
    int                   print (FTermArea*, wchar_t);
    int                   print (FChar&);
    int                   print (FTermArea*, const FChar&);
    int                   print (FTermArea*, FChar&);
    virtual void          print (const FPoint&);
    virtual void          print (const FStyle&);
    virtual void          print (const FColorPair&);
    virtual FVTerm&       print();
    static void           flush();

  protected:
    // Accessor
    virtual FTermArea*    getPrintArea();
    FTermArea*            getChildPrintArea() const;
    FTermArea*            getCurrentPrintArea() const;
    FTermArea*            getVirtualDesktop() const;
    FTermArea*            getVirtualTerminal() const;

    // Mutators
    void                  setPrintArea (FTermArea*);
    void                  setChildPrintArea (FTermArea*);
    void                  setActiveArea (FTermArea*) const;

    // Inquiries
    bool                  hasPrintArea() const;
    bool                  hasChildPrintArea() const;
    bool                  isVirtualWindow() const;
    bool                  isCursorHideable() const;

    // Methods
    void                  createArea ( const FRect&
                                     , const FSize&
                                     , FTermArea*& );
    void                  resizeArea ( const FRect&
                                     , const FSize&
                                     , FTermArea* ) const;
    static void           removeArea (FTermArea*&);
    static void           restoreVTerm (const FRect&);
    bool                  updateVTermCursor (const FTermArea*) const;
    static void           setAreaCursor ( const FPoint&
                                        , bool, FTermArea* );
    static void           getArea (const FPoint&, const FTermArea*);
    static void           getArea (const FRect&, const FTermArea*);
    void                  putArea (const FTermArea*) const;
    static void           putArea (const FPoint&, const FTermArea*);
    void                  scrollAreaForward (FTermArea*) const;
    void                  scrollAreaReverse (FTermArea*) const;
    void                  clearArea (FTermArea*, int = ' ') const;
    void                  processTerminalUpdate() const;
    static void           startTerminalUpdate();
    static void           finishTerminalUpdate();
    virtual void          initTerminal();

  private:
    // Enumerations
    enum character_type
    {
      overlapped_character,
      covered_character
    };

    enum exit_state
    {
      not_used,
      used,
      line_completely_printed
    };

    // Constants
    //   Buffer size for character output on the terminal
    static constexpr uInt TERMINAL_OUTPUT_BUFFER_SIZE = 32768;

    // Methods
    void                  resetTextAreaToDefault ( const FTermArea*
                                                 , const FSize&) const;
    static bool           reallocateTextArea ( FTermArea*
                                             , std::size_t
                                             , std::size_t );
    static bool           reallocateTextArea ( FTermArea*
                                             , std::size_t );
    static covered_state  isCovered (const FPoint&, const FTermArea*);
    static void           updateOverlappedColor ( const FTermArea*
                                                , const FPoint&
                                                , const FPoint& );
    static void           updateOverlappedCharacter ( const FTermArea*
                                                    , const FPoint& );
    static void           updateShadedCharacter ( const FTermArea*
                                                , const FPoint&
                                                , const FPoint& );
    static void           updateInheritBackground ( const FTermArea*
                                                  , const FPoint&
                                                  , const FPoint& );
    static void           updateCharacter ( const FTermArea*
                                          , const FPoint&
                                          , const FPoint& );
    static bool           updateVTermCharacter ( const FTermArea*
                                               , const FPoint&
                                               , const FPoint& );
    void                  updateVTerm() const;
    static void           callPreprocessingHandler (const FTermArea*);
    bool                  hasChildAreaChanges (FTermArea*) const;
    void                  clearChildAreaChanges (const FTermArea*) const;
    static bool           isInsideArea (const FPoint&, const FTermArea*);
    static FChar          generateCharacter (const FPoint&);
    static FChar          getCharacter ( character_type
                                       , const FPoint&
                                       , FVTerm* );
    static FChar          getCoveredCharacter (const FPoint&, FVTerm*);
    static FChar          getOverlappedCharacter (const FPoint&, FVTerm*);
    void                  init();
    static void           init_characterLengths (const FOptiMove*);
    void                  finish();
    static void           putAreaLine (const FChar*, FChar*, int);
    static void           putAreaCharacter ( const FPoint&, FVTerm*
                                           , const FChar*, FChar* );
    static void           getAreaCharacter ( const FPoint&, const FTermArea*
                                           , FChar*& );
    bool                  clearTerm (int = ' ') const;
    bool                  clearFullArea (const FTermArea*, FChar&) const;
    static void           clearAreaWithShadow (const FTermArea*, const FChar&);
    static bool           canClearToEOL (uInt, uInt);
    static bool           canClearLeadingWS (uInt&, uInt);
    static bool           canClearTrailingWS (uInt&, uInt);
    bool                  skipUnchangedCharacters (uInt&, uInt, uInt) const;
    void                  printRange (uInt, uInt, uInt, bool) const;
    void                  replaceNonPrintableFullwidth (uInt, FChar*&) const;
    void                  printCharacter (uInt&, uInt, bool, FChar*&) const;
    void                  printFullWidthCharacter (uInt&, uInt, FChar*&) const;
    void                  printFullWidthPaddingCharacter (uInt&, uInt, FChar*&) const;
    void                  printHalfCovertFullWidthCharacter (uInt&, uInt, FChar*&) const;
    void                  skipPaddingCharacter (uInt&, uInt, const FChar* const&) const;
    exit_state            eraseCharacters (uInt&, uInt, uInt, bool) const;
    exit_state            repeatCharacter (uInt&, uInt, uInt) const;
    bool                  isFullWidthChar (const FChar* const&) const;
    bool                  isFullWidthPaddingChar (const FChar* const&) const;
    static void           cursorWrap();
    bool                  printWrap (FTermArea*) const;
    void                  printPaddingCharacter (FTermArea*, const FChar&);
    void                  updateTerminalLine (uInt) const;
    bool                  updateTerminalCursor() const;
    bool                  isInsideTerminal (const FPoint&) const;
    bool                  isTermSizeChanged() const;
    static bool           isTermSizeCheckTimeout();
    static void           markAsPrinted (uInt, uInt);
    static void           markAsPrinted (uInt, uInt, uInt);
    static void           newFontChanges (FChar*&);
    static void           charsetChanges (FChar*&);
    void                  appendCharacter (FChar*&) const;
    void                  appendChar (FChar*&) const;
    void                  appendAttributes (FChar*&) const;
    int                   appendLowerRight (FChar*&) const;
    static void           characterFilter (FChar*&);
    static void           appendOutputBuffer (const std::string&);
    static void           appendOutputBuffer (const char[]);
    static int            appendOutputBuffer (int);

    // Data members
    FTermArea*              print_area{nullptr};        // print area for this object
    FTermArea*              child_print_area{nullptr};  // print area for children
    FTermArea*              vwin{nullptr};              // virtual window
    static FSystem*         fsystem;
    static FTerm*           fterm;
    static FTermArea*       vterm;        // virtual terminal
    static FTermArea*       vdesktop;     // virtual desktop
    static FTermArea*       active_area;  // active area
    static std::queue<int>* output_buffer;
    static FChar            term_attribute;
    static FChar            next_attribute;
    static FChar            s_ch;      // shadow character
    static FChar            i_ch;      // inherit background character
    static FPoint*          term_pos;  // terminal cursor position
    static FKeyboard*       keyboard;
    static timeval          last_term_size_check;
    static bool             terminal_update_complete;
    static bool             terminal_update_pending;
    static bool             force_terminal_update;
    static bool             no_terminal_updates;
    static uInt64           term_size_check_timeout;
    static int              skipped_terminal_update;
    static uInt             erase_char_length;
    static uInt             repeat_char_length;
    static uInt             clr_bol_length;
    static uInt             clr_eol_length;
    static uInt             cursor_address_length;
    static bool             cursor_hideable;
};


//----------------------------------------------------------------------
// struct FVTerm::FTermArea
//----------------------------------------------------------------------

struct FVTerm::FTermArea  // define virtual terminal character properties
{
  // Constructor
  FTermArea() = default;

  // Disable copy constructor
  FTermArea (const FTermArea&) = delete;

  // Destructor
  ~FTermArea() = default;

  // Disable copy assignment operator (=)
  FTermArea& operator = (const FTermArea&) = delete;

  // Data members
  int offset_left{0};        // Distance from left terminal side
  int offset_top{0};         // Distance from top of the terminal
  int width{-1};             // Window width
  int height{-1};            // Window height
  int right_shadow{0};       // Right window shadow
  int bottom_shadow{0};      // Bottom window shadow
  int cursor_x{0};           // X-position for the next write operation
  int cursor_y{0};           // Y-position for the next write operation
  int input_cursor_x{-1};    // X-position input cursor
  int input_cursor_y{-1};    // Y-position input cursor
  FWidget* widget{nullptr};  // Widget that owns this FTermArea
  FPreprocessing preproc_list{};
  FLineChanges* changes{nullptr};
  FChar* data{nullptr};      // FChar data of the drawing area
  bool input_cursor_visible{false};
  bool has_changes{false};
  bool visible{false};
};


//----------------------------------------------------------------------
// struct FVTerm::FVTermPreprocessing
//----------------------------------------------------------------------

struct FVTerm::FVTermPreprocessing
{
  // Constructor
  FVTermPreprocessing()
    : instance(nullptr)
    , function(nullptr)
  { }

  FVTermPreprocessing (const FVTerm* i, const FPreprocessingFunction& f)
    : instance(i)
    , function(f)
  { }

  FVTermPreprocessing (const FVTermPreprocessing& p)  // copy constructor
    : instance(p.instance)
    , function(p.function)
  { }

  FVTermPreprocessing (FVTermPreprocessing&& p) noexcept  // move constructor
    : instance(std::move(p.instance))
    , function(std::move(p.function))
  { }

  // Overloaded operators
  FVTermPreprocessing& operator = (const FVTermPreprocessing& p)
  {
    instance = p.instance;
    function = p.function;
    return *this;
  }

  FVTermPreprocessing& operator = (FVTermPreprocessing&& p) noexcept
  {
    instance = p.instance;
    function = p.function;
    p.instance = nullptr;
    p.function = nullptr;
    return *this;
  }

  // Destructor
  ~FVTermPreprocessing()
  { }

  // Data members
  const FVTerm* instance{};
  FPreprocessingFunction function{};
};


// FVTerm inline functions
//----------------------------------------------------------------------
template <typename typeT>
inline FVTerm& FVTerm::operator << (const typeT& s)
{
  FStringStream outstream{std::ios_base::out};
  outstream << s;

  if ( ! outstream.str().isEmpty() )
    print (outstream.str());

  return *this;
}

//----------------------------------------------------------------------
inline FVTerm& FVTerm::operator << (fc::SpecialCharacter c)
{
  print (static_cast<wchar_t>(c));  // Required under Solaris
  return *this;
}

//----------------------------------------------------------------------
inline FVTerm& FVTerm::operator << (const std::string& string)
{
  print (string);
  return *this;
}

//----------------------------------------------------------------------
inline FVTerm& FVTerm::operator << \
    (const std::vector<FChar>& term_string)
{
  print (term_string);
  return *this;
}

//----------------------------------------------------------------------
inline FVTerm& FVTerm::operator << (const FPoint& pos)
{
  print (pos);
  return *this;
}

//----------------------------------------------------------------------
inline FVTerm& FVTerm::operator << (const FStyle& t_style)
{
  print (t_style);
  return *this;
}

//----------------------------------------------------------------------
inline FVTerm& FVTerm::operator << (const FColorPair& pair)
{
  print (pair);
  return *this;
}

//----------------------------------------------------------------------
inline FString FVTerm::getClassName() const
{ return "FVTerm"; }

//----------------------------------------------------------------------
inline FColor FVTerm::getTermForegroundColor()
{ return next_attribute.fg_color; }

//----------------------------------------------------------------------
inline FColor FVTerm::getTermBackgroundColor()
{ return next_attribute.bg_color; }

//----------------------------------------------------------------------
inline FVTerm::FTermArea*& FVTerm::getVWin()
{ return vwin; }

//----------------------------------------------------------------------
inline const FVTerm::FTermArea* FVTerm::getVWin() const
{ return vwin; }

//----------------------------------------------------------------------
inline FChar FVTerm::getAttribute()
{ return next_attribute; }

//----------------------------------------------------------------------
inline FTerm& FVTerm::getFTerm() const
{ return *fterm; }

//----------------------------------------------------------------------
inline void FVTerm::hideCursor() const
{ return hideCursor(true); }

//----------------------------------------------------------------------
inline void FVTerm::showCursor() const
{ return hideCursor(false); }

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
inline bool FVTerm::setBold (bool enable)
{ return (next_attribute.attr.bit.bold = enable); }

//----------------------------------------------------------------------
inline bool FVTerm::setBold()
{ return setBold(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetBold()
{ return setBold(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setDim (bool enable)
{ return (next_attribute.attr.bit.dim = enable); }

//----------------------------------------------------------------------
inline bool FVTerm::setDim()
{ return setDim(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetDim()
{ return setDim(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setItalic (bool enable)
{ return (next_attribute.attr.bit.italic = enable); }

//----------------------------------------------------------------------
inline bool FVTerm::setItalic()
{ return setItalic(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetItalic()
{ return setItalic(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setUnderline (bool enable)
{ return (next_attribute.attr.bit.underline = enable); }

//----------------------------------------------------------------------
inline bool FVTerm::setUnderline()
{ return setUnderline(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetUnderline()
{ return setUnderline(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setBlink (bool enable)
{ return (next_attribute.attr.bit.blink = enable); }

//----------------------------------------------------------------------
inline bool FVTerm::setBlink()
{ return setBlink(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetBlink()
{ return setBlink(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setReverse (bool enable)
{ return (next_attribute.attr.bit.reverse = enable); }

//----------------------------------------------------------------------
inline bool FVTerm::setReverse()
{ return setReverse(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetReverse()
{ return setReverse(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setStandout (bool enable)
{ return (next_attribute.attr.bit.standout = enable); }

//----------------------------------------------------------------------
inline bool FVTerm::setStandout()
{ return setStandout(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetStandout()
{ return setStandout(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setInvisible (bool enable)
{ return (next_attribute.attr.bit.invisible = enable); }

//----------------------------------------------------------------------
inline bool FVTerm::setInvisible()
{ return setInvisible(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetInvisible()
{ return setInvisible(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setProtected (bool enable)
{ return (next_attribute.attr.bit.protect = enable); }

//----------------------------------------------------------------------
inline bool FVTerm::setProtected()
{ return setProtected(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetProtected()
{ return setProtected(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setCrossedOut (bool enable)
{ return (next_attribute.attr.bit.crossed_out = enable); }

//----------------------------------------------------------------------
inline bool FVTerm::setCrossedOut()
{ return setCrossedOut(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetCrossedOut()
{ return setCrossedOut(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setDoubleUnderline (bool enable)
{ return (next_attribute.attr.bit.dbl_underline = enable); }

//----------------------------------------------------------------------
inline bool FVTerm::setDoubleUnderline()
{ return setDoubleUnderline(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetDoubleUnderline()
{ return setDoubleUnderline(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setAltCharset (bool enable)
{ return (next_attribute.attr.bit.alt_charset = enable); }

//----------------------------------------------------------------------
inline bool FVTerm::setAltCharset()
{ return setAltCharset(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetAltCharset()
{ return setAltCharset(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setPCcharset (bool enable)
{ return (next_attribute.attr.bit.pc_charset = enable); }

//----------------------------------------------------------------------
inline bool FVTerm::setPCcharset()
{ return setPCcharset(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetPCcharset()
{ return setPCcharset(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setTransparent (bool enable)
{ return (next_attribute.attr.bit.transparent = enable); }

//----------------------------------------------------------------------
inline bool FVTerm::setTransparent()
{ return setTransparent(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetTransparent()
{ return setTransparent(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setColorOverlay (bool enable)
{ return (next_attribute.attr.bit.color_overlay = enable); }

//----------------------------------------------------------------------
inline bool FVTerm::setColorOverlay()
{ return setColorOverlay(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetColorOverlay()
{ return setColorOverlay(false); }

//----------------------------------------------------------------------
inline bool FVTerm::setInheritBackground (bool enable)
{ return (next_attribute.attr.bit.inherit_background = enable); }

//----------------------------------------------------------------------
inline bool FVTerm::setInheritBackground()
{ return setInheritBackground(true); }

//----------------------------------------------------------------------
inline bool FVTerm::unsetInheritBackground()
{ return setInheritBackground(false); }

//----------------------------------------------------------------------
inline void FVTerm::setNonBlockingRead()
{ setNonBlockingRead(true); }

//----------------------------------------------------------------------
inline void FVTerm::unsetNonBlockingRead()
{ setNonBlockingRead(false); }

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
{ return next_attribute.attr.bit.color_overlay; }

//----------------------------------------------------------------------
inline bool FVTerm::isInheritBackground()
{ return next_attribute.attr.bit.inherit_background; }

//----------------------------------------------------------------------
template <typename... Args>
inline int FVTerm::printf (const FString& format, Args&&... args)
{
  FString str{};
  str.sprintf (format, std::forward<Args>(args)...);
  return print(str);
}

//----------------------------------------------------------------------
inline FVTerm& FVTerm::print()
{ return *this; }

//----------------------------------------------------------------------
inline FVTerm::FTermArea* FVTerm::getChildPrintArea() const
{ return child_print_area; }

//----------------------------------------------------------------------
inline FVTerm::FTermArea* FVTerm::getCurrentPrintArea() const
{ return print_area; }

//----------------------------------------------------------------------
inline FVTerm::FTermArea* FVTerm::getVirtualDesktop() const
{ return vdesktop; }

//----------------------------------------------------------------------
inline FVTerm::FTermArea* FVTerm::getVirtualTerminal() const
{ return vterm; }

//----------------------------------------------------------------------
inline void FVTerm::setPrintArea (FTermArea* area)
{ print_area = area; }

//----------------------------------------------------------------------
inline void FVTerm::setChildPrintArea (FTermArea* area)
{ child_print_area = area; }

//----------------------------------------------------------------------
inline void FVTerm::setActiveArea (FTermArea* area) const
{ active_area = area; }

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
inline bool FVTerm::isCursorHideable() const
{ return cursor_hideable; }


}  // namespace finalcut

#endif  // FVTERM_H
