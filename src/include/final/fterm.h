/***********************************************************************
* fterm.h - Base class for terminal control                            *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2012-2020 Markus Gans                                      *
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

/*  Base class
 *  ══════════
 *
 * ▕▔▔▔▔▔▔▔▏1       1▕▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FTerm ▏-┬- - - -▕ FTermios ▏
 * ▕▁▁▁▁▁▁▁▏ :       ▕▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FTermcap ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FTermcapQuirks ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FTermDetection ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FTermXTerminal ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕   FTermLinux   ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕  FTermFreeBSD  ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕  FTermOpenBSD  ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FColorPalette ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FOptiAttr ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FOptiMove ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FKeyboard ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FMouseControl ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      *▕▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FString ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      *▕▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FPoint ▏
 *           :       ▕▁▁▁▁▁▁▁▁▏
 *           :
 *           :      *▕▔▔▔▔▔▔▔▏
 *           └- - - -▕ FRect ▏
 *                   ▕▁▁▁▁▁▁▁▏
 */

#ifndef FTERM_H
#define FTERM_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fconfig.h"

#include <sys/ioctl.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <langinfo.h>

#if F_HAVE_GETTTYNAM && F_HAVE_TTYENT_H
  #include <ttyent.h>
#endif

#include <clocale>
#include <cmath>
#include <csignal>
#include <functional>
#include <map>
#include <queue>
#include <utility>
#include <string>
#include <vector>

#include "final/fc.h"
#include "final/fstring.h"
#include "final/fsystem.h"

namespace finalcut
{

// class forward declaration
class FKeyboard;
class FMouseControl;
class FOptiAttr;
class FOptiMove;
class FStartOptions;
class FSize;
class FString;
class FTermBuffer;
class FTermData;
class FTermDebugData;
class FTermDetection;
class FTermXTerminal;

#if defined(UNIT_TEST)
  class FTermLinux;
  class FTermFreeBSD;
  class FTermOpenBSD;
#elif defined(__linux__)
  class FTermLinux;
#elif defined(__FreeBSD__) || defined(__DragonFly__)
  class FTermFreeBSD;
#elif defined(__NetBSD__) || defined(__OpenBSD__)
  class FTermOpenBSD;
#endif

//----------------------------------------------------------------------
// class FTerm
//----------------------------------------------------------------------

class FTerm final
{
  public:
    // Typedef
    typedef std::function<int(int)> defaultPutChar;

    // Constructor
    explicit FTerm (bool = false);

    // Disable copy constructor
    FTerm (const FTerm&) = delete;

    // Destructor
    virtual ~FTerm();

    // Disable assignment operator (=)
    FTerm& operator = (const FTerm&) = delete;

    // Accessors
    virtual const FString  getClassName() const;
    static std::size_t     getLineNumber();
    static std::size_t     getColumnNumber();
    static const FString   getKeyName (FKey);
    static int             getTTYFileDescriptor();
    static char*           getTermType();
    static char*           getTermFileName();
    static int             getTabstop();
    static int             getMaxColor();
    charSubstitution&      getCharSubstitutionMap();

    static FTermData*      getFTermData();
    static FSystem*        getFSystem();
    static FOptiMove*      getFOptiMove();
    static FOptiAttr*      getFOptiAttr();
    static FTermDetection* getFTermDetection();
    static FTermXTerminal* getFTermXTerminal();
    static FKeyboard*      getFKeyboard();
    static FMouseControl*  getFMouseControl();

#if defined(UNIT_TEST)
    static FTermLinux*     getFTermLinux();
    static FTermFreeBSD*   getFTermFreeBSD();
    static FTermOpenBSD*   getFTermOpenBSD();
#elif defined(__linux__)
    static FTermLinux*     getFTermLinux();
#elif defined(__FreeBSD__) || defined(__DragonFly__)
    static FTermFreeBSD*   getFTermFreeBSD();
#elif defined(__NetBSD__) || defined(__OpenBSD__)
    static FTermOpenBSD*   getFTermOpenBSD();
#endif

#if DEBUG
    static FTermDebugData& getFTermDebugData();
#endif

    // Inquiries
    static bool            isNormal (FChar*&);
    static bool            isRaw();
    static bool            hasUTF8();
    static bool            hasVT100();
    static bool            hasASCII();
    static bool            isMonochron();
    static bool            isXTerminal();
    static bool            isAnsiTerminal();
    static bool            isRxvtTerminal();
    static bool            isUrxvtTerminal();
    static bool            isMltermTerminal();
    static bool            isPuttyTerminal();
    static bool            isKdeTerminal();
    static bool            isGnomeTerminal();
    static bool            isKtermTerminal();
    static bool            isTeraTerm();
    static bool            isSunTerminal();
    static bool            isCygwinTerminal();
    static bool            isMinttyTerm();
    static bool            isLinuxTerm();
    static bool            isFreeBSDTerm();
    static bool            isNetBSDTerm();
    static bool            isOpenBSDTerm();
    static bool            isScreenTerm();
    static bool            isTmuxTerm();
    static bool            isNewFont();
    static bool            isCursorHideable();
    static bool            hasChangedTermSize();
    static bool            hasShadowCharacter();
    static bool            hasHalfBlockCharacter();
    static bool            hasAlternateScreen();
    static bool            canChangeColorPalette();

    // Mutators
    static void            setFSystem (FSystem*);
    static void            setTermType (const char[]);
    static void            setInsertCursor (bool);
    static void            redefineDefaultColors (bool);
    static void            setDblclickInterval (const uInt64);
    static bool            setUTF8 (bool);
    static bool            setUTF8();
    static bool            unsetUTF8();

    // Methods
    static bool            setVGAFont();
    static bool            setNewFont();
    static bool            setOldFont();
    static int             openConsole();
    static int             closeConsole();
    static char*           moveCursorString (int, int, int, int);
    static char*           cursorsVisibilityString (bool);
    static void            detectTermSize();
    static void            setTermSize (const FSize&);
    static void            setTermTitle (const FString&);
    static void            setKDECursor (fc::kdeKonsoleCursorShape);
    static void            saveColorMap();
    static void            resetColorMap();
    static void            setPalette (FColor, int, int, int);
    static void            setBeep (int, int);
    static void            resetBeep();
    static void            beep();

    static void            setEncoding (fc::encoding);
    static fc::encoding    getEncoding();
    static std::string     getEncodingString();
    static bool            charEncodable (wchar_t);
    static wchar_t         charEncode (wchar_t);
    static wchar_t         charEncode (wchar_t, fc::encoding);

    static bool            scrollTermForward();
    static bool            scrollTermReverse();

    static defaultPutChar& putchar();  // function pointer
    template<typename... Args>
    static void            putstringf (const char[], Args&&...);
    static void            putstring (const char[], int = 1);
    static int             putchar_ASCII (int);
    static int             putchar_UTF8  (int);

    static void            initScreenSettings();
    static char*           changeAttribute (FChar*&, FChar*&);
    static void            changeTermSizeFinished();
    static void            exitWithMessage (const FString&)
    #if defined(__clang__) || defined(__GNUC__)
      __attribute__((noreturn))
    #endif
                           ;
  private:
    // Methods
    static FStartOptions&  getStartOptions();
    static void            init_global_values (bool);
    static void            init_terminal_device_path();
    static void            oscPrefix();
    static void            oscPostfix();
    static void            init_alt_charset();
    static void            init_pc_charset();
    static void            init_cygwin_charmap();
    static void            init_teraterm_charmap();
    static void            init_fixed_max_color();
    static void            init_keyboard();
    static void            init_termcap();
    static void            init_quirks();
    static void            init_optiMove();
    static void            init_optiAttr();
    static void            init_font();
    static void            init_locale();
    static void            init_encoding();
    static void            init_encoding_set();
    static void            init_term_encoding();
    static void            init_individual_term_encoding();
    static void            init_force_vt100_encoding();
    static void            init_utf8_without_alt_charset();
    static void            init_tab_quirks();
    static void            init_captureFontAndTitle();
    static bool            hasNoFontSettingOption();
    static void            redefineColorPalette();
    static void            restoreColorPalette();
    static void            setInsertCursorStyle();
    static void            setOverwriteCursorStyle();
    static char*           enableCursorString();
    static char*           disableCursorString();
    static void            enableMouse();
    static void            disableMouse();
    static void            enableApplicationEscKey();
    static void            disableApplicationEscKey();
    static void            enableKeypad();
    static void            disableKeypad();
    static void            enableAlternateCharset();
    static void            useAlternateScreenBuffer();
    static void            useNormalScreenBuffer();
    void                   allocationValues();
    void                   deallocationValues();
    void                   init (bool);
    void                   initOSspecifics();
    void                   initTermspecifics();
    void                   initBaudRate();
    void                   finish();
    void                   finishOSspecifics1();
    void                   finish_encoding();
    static void            setSignalHandler();
    static void            resetSignalHandler();
    static void            signal_handler (int);

    // Data members
    static FTermData*      data;
    static FSystem*        fsys;
    static FOptiMove*      opti_move;
    static FOptiAttr*      opti_attr;
    static FTermDetection* term_detection;
    static FTermXTerminal* xterm;
    static FKeyboard*      keyboard;
    static FMouseControl*  mouse;

#if defined(UNIT_TEST)
    #undef linux
    static FTermLinux*     linux;
    static FTermFreeBSD*   freebsd;
    static FTermOpenBSD*   openbsd;
#elif defined(__linux__)
    #undef linux
    static FTermLinux*     linux;
#elif defined(__FreeBSD__) || defined(__DragonFly__)
    static FTermFreeBSD*   freebsd;
#elif defined(__NetBSD__) || defined(__OpenBSD__)
    static FTermOpenBSD*   openbsd;
#endif

#if DEBUG
    static FTermDebugData* debug_data;
#endif
};


// non-member function forward declarations
// implemented in fterm_functions.cpp
//----------------------------------------------------------------------
uInt env2uint (const char*);
bool isReverseNewFontchar (wchar_t);
bool hasFullWidthSupports();
wchar_t cp437_to_unicode (uChar);
uChar unicode_to_cp437 (wchar_t);
FString getFullWidth (const FString&);
FString getHalfWidth (const FString&);
FString getColumnSubString (const FString&, std::size_t, std::size_t);
std::size_t getLengthFromColumnWidth (const FString&, std::size_t);
std::size_t getColumnWidth (const FString&, std::size_t);
std::size_t getColumnWidth (const FString&);
std::size_t getColumnWidth (const wchar_t);
std::size_t getColumnWidth (FChar&);
std::size_t getColumnWidth (const FTermBuffer&);


// FTerm inline functions
//----------------------------------------------------------------------
inline const FString FTerm::getClassName() const
{ return "FTerm"; }

//----------------------------------------------------------------------
inline void FTerm::setFSystem (FSystem* fsystem)
{ fsys = fsystem; }

//----------------------------------------------------------------------
inline bool FTerm::setUTF8()
{ return setUTF8(true); }

//----------------------------------------------------------------------
inline bool FTerm::unsetUTF8()
{ return setUTF8(false); }

//----------------------------------------------------------------------
template<typename... Args>
inline void FTerm::putstringf (const char format[], Args&&... args)
{
  const int size = std::snprintf ( nullptr, 0, format
                                 , std::forward<Args>(args)... ) + 1;

  if ( size == -1 )
    return;

  if ( ! fsys )
    getFSystem();

  const std::size_t count = std::size_t(size);
  std::vector<char> buf(count);
  std::snprintf (&buf[0], count, format, std::forward<Args>(args)...);
  fsys->tputs (&buf[0], 1, FTerm::putchar_ASCII);
}

}  // namespace finalcut


namespace
{

//----------------------------------------------------------------------
inline std::ostream& operator << ( std::ostream& os
                                 , finalcut::fc::SpecialCharacter c )
{
  return os << static_cast<char>(c);
}

//----------------------------------------------------------------------
inline std::wostream& operator << ( std::wostream& os
                                  , finalcut::fc::SpecialCharacter c )
{
  return os << static_cast<wchar_t>(c);
}

}  // namespace

#endif  // FTERM_H
